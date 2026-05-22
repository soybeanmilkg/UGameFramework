// Fill out your copyright notice in the Description page of Project Settings.


#include "EventPool/CommonEventPool.h"

#include "LogCommonPool.h"
#include "EventPool/CommonEventArgsBase.h"
#include "ReferencePool/CommonReferencePoolSubsystem.h"

bool UCommonEventPool::IsTickable() const
{
	return !IsTemplate() && !Events.IsEmpty();
}

void UCommonEventPool::Tick(float DeltaTime)
{
	if (Events.Num() > 0)
	{
		TempEvents = MoveTemp(Events);
		for (const auto& Event : TempEvents)
		{
			HandleEvent(Event);
		}
		TempEvents.Empty();
	}
}

void UCommonEventPool::Shutdown()
{
	SubscribeData.Empty();
	PendingHandles.Empty();
	EventHandles.Empty();
	Events.Empty();
	TempEvents.Empty();
}

int32 UCommonEventPool::GetEventSubscribeCountByClass(const TSubclassOf<UCommonEventArgsBase> EventClass)
{
	if (EventClass && EventHandles.Contains(EventClass))
	{
		return EventHandles[EventClass].Num();
	}

	return 0;
}

bool UCommonEventPool::Check(const FCommonEventHandle& InHandle) const
{
	return InHandle.IsValid() && SubscribeData.IsValidIndex(InHandle.GetIndex());
}

void UCommonEventPool::Unsubscribe(FCommonEventHandle& InHandle)
{
	const auto Data = GetSubscribeData(InHandle);
	if (Data == nullptr) return;

	for (FCommonEventHandle& Handle : EventHandles[Data->EventClass])
	{
		if (Handle == InHandle)
		{
			// 如果正在处理事件，不能修改 EventHandles
			if (!bHandleEvent)
			{
				EventHandles[Data->EventClass].Remove(Handle);
			}

			Handle.Invalidate();
			break;
		}
	}

	SubscribeData.RemoveAt(InHandle.GetIndex());
	InHandle.Invalidate();
}

void UCommonEventPool::Fire(UObject* Sender, UCommonEventArgsBase* EventArgs)
{
	if (EventArgs != nullptr)
	{
		FCommonEventData Event
		{
			.Sender = Sender,
			.EventArgs = EventArgs,
		};
		Events.Emplace(Event);
	}
}

void UCommonEventPool::FireNow(UObject* Sender, UCommonEventArgsBase* EventArgs)
{
	if (EventArgs != nullptr)
	{
		const FCommonEventData Event
		{
			.Sender = Sender,
			.EventArgs = EventArgs,
		};
		HandleEvent(Event);
	}
}

void UCommonEventPool::HandleEvent(const FCommonEventData& Event)
{
	if (PendingHandles.Num() > 0)
	{
		for (FCommonEventHandle& Handle : PendingHandles)
		{
			AddHandle(Handle);
		}
		PendingHandles.Empty();
	}

	UClass* CurrentEventClass = Event.EventArgs->GetClass();
	bool bIsExactMatch { true };
	TGuardValue Guard(bHandleEvent, true);

	while (CurrentEventClass != nullptr && CurrentEventClass != UCommonEventArgsBase::StaticClass())
	{
		if (EventHandles.Contains(CurrentEventClass))
		{
			TArray<FCommonEventHandle>& Handles = EventHandles[CurrentEventClass];
			for (const FCommonEventHandle& Handle : Handles)
			{
				const auto Data = GetSubscribeData(Handle);
				if (Data && (bIsExactMatch || Data->bMatchChild))
				{
					Data->Callback.Execute(Event.Sender, Event.EventArgs);
				}
			}
		}

		CurrentEventClass = CurrentEventClass->GetSuperClass();
		bIsExactMatch = false;
	}

	for (auto It = EventHandles.CreateIterator(); It; ++It)
	{
		TArray<FCommonEventHandle>& Handles = It.Value();

		Handles.RemoveAll([this](const FCommonEventHandle& Handle)
		{
			return GetSubscribeData(Handle) == nullptr;
		});

		if (Handles.IsEmpty())
		{
			It.RemoveCurrent();
		}
	}

	FCommonReferencePoolStatic::Release(this, Event.EventArgs);
}

void UCommonEventPool::InternalSubscribe(FCommonEventHandle& OutHandle, const TSubclassOf<UCommonEventArgsBase>& InEventClass, FCommonEventUnifiedDelegate&& InEventCallback, const bool bMatchChild)
{
	if (InEventClass == nullptr)
	{
		UE_LOG(LogCommonPool, Warning, TEXT("[UCommonEventPool] EventClass is nullptr"))
		return;
	}

	if (Check(OutHandle))
	{
		Unsubscribe(OutHandle);
	}

	FCommonSubscribeData Data
	{
		.EventClass = InEventClass,
		.Callback = MoveTemp(InEventCallback),
		.bMatchChild = bMatchChild,
	};

	OutHandle = FCommonEventHandle(SubscribeData.Emplace(MoveTemp(Data)));

	if (bHandleEvent)
	{
		PendingHandles.Add(OutHandle);
	}
	else
	{
		AddHandle(OutHandle);
	}
}

void UCommonEventPool::AddHandle(const FCommonEventHandle& InHandle)
{
	if (const auto Data = GetSubscribeData(InHandle))
	{
		if (!EventHandles.Contains(Data->EventClass))
		{
			EventHandles.Add(Data->EventClass, TArray<FCommonEventHandle>());
		}

		EventHandles[Data->EventClass].Emplace(InHandle);
	}
}

FCommonSubscribeData* UCommonEventPool::GetSubscribeData(const FCommonEventHandle& InHandle)
{
	const int32 Index = InHandle.GetIndex();
	return SubscribeData.IsValidIndex(Index) ? &SubscribeData[Index] : nullptr;
}
