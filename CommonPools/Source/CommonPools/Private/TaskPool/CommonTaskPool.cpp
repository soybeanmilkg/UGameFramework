// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskPool/CommonTaskPool.h"

#include "LogCommonPool.h"
#include "ReferencePool/CommonReferencePoolSubsystem.h"
#include "TaskPool/CommonTaskAgent.h"

struct FCommonTaskComparer
{
	bool operator()(const UCommonTaskBase& TaskA, const UCommonTaskBase& TaskB) const
	{
		if (TaskA.GetPriority() == TaskB.GetPriority())
		{
			return TaskA.GetSerialId() < TaskB.GetSerialId();
		}

		return TaskA.GetPriority() > TaskB.GetPriority();
	}
};

bool UCommonTaskPool::IsTickable() const
{
	return !IsTemplate() && !bPaused && (!WorkingAgents.IsEmpty() || !WaitingTasks.IsEmpty());
}

void UCommonTaskPool::Tick(const float DeltaTime)
{
	if (!bPaused)
	{
		HandleWorkingAgents(DeltaTime);
		HandleWaitingTasks(DeltaTime);
	}
}

void UCommonTaskPool::Shutdown()
{
	ClearTasks();

	for (const TObjectPtr<UCommonTaskAgent>& Agent : FreeAgents)
	{
		Agent->Shutdown();
	}

	FreeAgents.Empty();
}

void UCommonTaskPool::AddAgent(UCommonTaskAgent* Agent)
{
	if (Agent)
	{
		Agent->Initialize();
		FreeAgents.Emplace(Agent);
	}
}

void UCommonTaskPool::AddTask(UCommonTaskBase* Task)
{
	if (Task == nullptr)
	{
		return;
	}

	if (Task->GetStatus() == ECommonTaskStatus::Todo)
	{
		constexpr FCommonTaskComparer Comparer {};
		WaitingTasks.HeapPush(Task, Comparer);
	}
	else
	{
		UE_LOG(LogCommonPool, Warning, TEXT("[UCommonTaskPool] AddTask: Task status %s is not Todo"),
		       *StaticEnum<ECommonTaskStatus>()->GetDisplayNameTextByValue(static_cast<int64>(Task->GetStatus())).ToString());
	}
}

bool UCommonTaskPool::RemoveTask(const int32 SerialId)
{
	for (int i = 0; i < WaitingTasks.Num(); ++i)
	{
		UCommonTaskBase* Task = WaitingTasks[i].Get();
		if (Task->GetSerialId() == SerialId)
		{
			WaitingTasks.RemoveAtSwap(i);
			WaitingTasks.Heapify(FCommonTaskComparer {});
			FCommonReferencePoolStatic::Release(this, Task);
			return true;
		}
	}

	for (int i = 0; i < WorkingAgents.Num(); ++i)
	{
		const TObjectPtr<UCommonTaskAgent>& WorkingAgent = WorkingAgents[i];
		UCommonTaskBase* Task = WorkingAgent->GetTask();

		if (Task->GetSerialId() == SerialId)
		{
			WorkingAgent->Reset();
			FreeAgents.Emplace(WorkingAgent);
			WorkingAgents.RemoveAtSwap(i);
			FCommonReferencePoolStatic::Release(this, Task);
			return true;
		}
	}

	return false;
}

int32 UCommonTaskPool::RemoveTasks(const FGameplayTag& Tag)
{
	int32 Count = WaitingTasks.RemoveAll([this, &Tag](const TObjectPtr<UCommonTaskBase>& Task)
	{
		if (ensureAlways(Task) && Task->HasMatchingGameplayTag(Tag))
		{
			FCommonReferencePoolStatic::Release(this, Task);
			return true;
		}

		return false;
	});

	if (Count > 0)
	{
		WaitingTasks.Heapify(FCommonTaskComparer {});
	}

	Count += WorkingAgents.RemoveAll([this, &Tag](const TObjectPtr<UCommonTaskAgent>& Agent)
	{
		if (ensureAlways(Agent))
		{
			UCommonTaskBase* Task = Agent->GetTask();
			if (ensureAlways(Task) && Task->HasMatchingGameplayTag(Tag))
			{
				Agent->Reset();
				FreeAgents.Emplace(Agent);
				FCommonReferencePoolStatic::Release(this, Task);
				return true;
			}
		}

		return false;
	});

	return Count;
}

void UCommonTaskPool::ClearTasks()
{
	for (TObjectPtr<UCommonTaskBase>& Task : WaitingTasks)
	{
		FCommonReferencePoolStatic::Release(this, Task);
	}
	WaitingTasks.Empty();

	for (TObjectPtr<UCommonTaskAgent>& Agent : WorkingAgents)
	{
		UCommonTaskBase* Task = Agent->GetTask();
		Agent->Reset();
		FreeAgents.Emplace(Agent);
		FCommonReferencePoolStatic::Release(this, Task);
	}
	WorkingAgents.Empty();
}

bool UCommonTaskPool::HasTask(const int32 SerialId)
{
	for (const auto& WaitingTask : WaitingTasks)
	{
		if (WaitingTask->GetSerialId() == SerialId)
		{
			return true;
		}
	}

	for (const auto& WorkingAgent : WorkingAgents)
	{
		const UCommonTaskBase* Task = WorkingAgent->GetTask();
		if (Task->GetSerialId() == SerialId)
		{
			return true;
		}
	}

	return false;
}

void UCommonTaskPool::HandleWorkingAgents(const float DeltaTime)
{
	TArray<TObjectPtr<UCommonTaskAgent>> PendingReleaseAgents {};
	PendingReleaseAgents.Reserve(WorkingAgents.Num());

	for (const TObjectPtr<UCommonTaskAgent>& WorkingAgent : WorkingAgents)
	{
		UCommonTaskBase* Task = WorkingAgent->GetTask();
		if (Task->IsDone())
		{
			WorkingAgent->Reset();
			FCommonReferencePoolStatic::Release(this, Task);
			PendingReleaseAgents.Add(WorkingAgent);
		}
		else
		{
			WorkingAgent->Tick(DeltaTime);
		}
	}

	for (TObjectPtr<UCommonTaskAgent>& ReleaseAgent : PendingReleaseAgents)
	{
		FreeAgents.Add(ReleaseAgent);
		WorkingAgents.Remove(ReleaseAgent);
	}
}

void UCommonTaskPool::HandleWaitingTasks(const float DeltaTime)
{
	constexpr FCommonTaskComparer Comparer {};
	while (WaitingTasks.Num() > 0 && FreeAgents.Num() > 0)
	{
		TObjectPtr<UCommonTaskAgent>& Agent = FreeAgents.Last();

		TObjectPtr<UCommonTaskBase> Task { nullptr };
		WaitingTasks.HeapPop(Task, Comparer);

		if (ensure(Agent && Task))
		{
			// 如果因为某种原因，导致任务提前结束，跳过该任务
			if (Task->IsDone())
			{
				continue;
			}

			FreeAgents.Remove(Agent);
			WorkingAgents.Add(Agent);

			const ECommonStartTaskStatus Status = Agent->Start(Task);

			// 代理开始任务结束，释放代理
			if (Status != ECommonStartTaskStatus::CanResume)
			{
				Agent->Reset();
				FreeAgents.Add(Agent);
				WorkingAgents.Remove(Agent);
			}

			// 任务还不能执行，重回任务池
			if (Status == ECommonStartTaskStatus::HasToWait)
			{
				WaitingTasks.HeapPush(Task, Comparer);
			}
			// 任务执行结束或执行存在错误，释放任务对象
			else if (Status == ECommonStartTaskStatus::Done || Status == ECommonStartTaskStatus::UnknownError)
			{
				FCommonReferencePoolStatic::Release(this, Task);
			}
		}
	}
}
