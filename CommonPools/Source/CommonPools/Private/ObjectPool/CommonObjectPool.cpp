// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/CommonObjectPool.h"

#include "LogCommonPool.h"
#include "ObjectPool/CommonObjectBase.h"
#include "ReferencePool/CommonReferencePoolSubsystem.h"

struct FCommonObjectComparer
{
	bool operator()(const UCommonObjectBase& A, const UCommonObjectBase& B) const
	{
		if (A.GetPriority() != B.GetPriority())
		{
			return A.GetPriority() < B.GetPriority();
		}
		return A.GetLastUseTime() < B.GetLastUseTime();
	}
};

void UCommonObjectPool::Init(const TSubclassOf<UCommonObjectBase>& InObjectClass, const FCommonObjectPoolCreateParams& Params)
{
	ObjectClass = InObjectClass;

	bAllowMultiSpawn = Params.bAllowMultiSpawn;
	AutoReleaseInterval = Params.AutoReleaseInterval;
	Capacity = Params.Capacity;
	ExpireTime = Params.ExpireTime;
	Priority = Params.Priority;

	Objects.Empty();
	ObjectMap.Empty();
	CachedCanReleaseObjects.Empty();
	AutoReleaseTime = 0.f;

	DefaultReleaseObjectFilter = FReleaseObjectFilterDelegate::CreateStatic(OnDefaultReleaseObjectFilter);
}

void UCommonObjectPool::Shutdown()
{
	for (auto& [_, Wrapper] : ObjectMap)
	{
		Wrapper->Release(true);
		FCommonReferencePoolStatic::Release(Wrapper);
	}

	Objects.Empty();
	ObjectMap.Empty();
	CachedCanReleaseObjects.Empty();
}

void UCommonObjectPool::Tick(const float DeltaTime)
{
	AutoReleaseTime += DeltaTime;
	if (AutoReleaseTime >= AutoReleaseInterval)
	{
		Release();
	}
}

int32 UCommonObjectPool::GetSpawnCount() const
{
	int32 SpawnCount = 0;
	for (auto& [_, Wrapper] : ObjectMap)
	{
		SpawnCount += Wrapper->GetSpawnCount();
	}
	return SpawnCount;
}

int32 UCommonObjectPool::GetCanReleaseCount()
{
	GetCanReleaseObjects(CachedCanReleaseObjects);
	return CachedCanReleaseObjects.Num();
}

void UCommonObjectPool::SetAutoReleaseInterval(const float InAutoReleaseInterval)
{
	AutoReleaseInterval = InAutoReleaseInterval;
}

void UCommonObjectPool::SetCapacity(const int InCapacity)
{
	if (Capacity != InCapacity)
	{
		Capacity = FMath::Max(InCapacity, 0);
		Release();
	}
}

void UCommonObjectPool::SetExpireTime(const float InExpireTime)
{
	if (!FMath::IsNearlyEqual(ExpireTime, InExpireTime))
	{
		ExpireTime = InExpireTime;
		Release();
	}
}

void UCommonObjectPool::SetPriority(const float InPriority)
{
	Priority = InPriority;
}

void UCommonObjectPool::Register(UCommonObjectBase* Object, const bool bSpawned)
{
	if (Object == nullptr)
	{
		UE_LOG(LogCommonPool, Error, TEXT("[UCommonObjectPool] Object is invalid."))
		return;
	}

	ensure(ObjectClass);
	if (Object->GetClass() != ObjectClass)
	{
		UE_LOG(LogCommonPool, Error, TEXT("[UCommonObjectPool] Object is not a {'%s'}."), *GetNameSafe(ObjectClass))
		return;
	}

	UCommonObjectWrapper* Wrapper = UCommonObjectWrapper::Create(Object, bSpawned);
	const auto ObjectName = Wrapper->GetObjectName();
	TArray<TObjectPtr<UCommonObjectWrapper>>& WrapperList = Objects.FindOrAdd(ObjectName);

	WrapperList.Emplace(Wrapper);
	ObjectMap.Emplace(Object->GetTarget(), Wrapper);

	if (ObjectMap.Num() > Capacity)
	{
		Release();
	}
}

bool UCommonObjectPool::CanSpawn(const FName& InName)
{
	if (Objects.Contains(InName))
	{
		for (const auto& Wrapper : Objects[InName])
		{
			if (bAllowMultiSpawn || !Wrapper->IsInUse())
			{
				return true;
			}
		}
	}

	return false;
}

UCommonObjectBase* UCommonObjectPool::Spawn(const FName& InName)
{
	if (Objects.Contains(InName))
	{
		for (const auto& Wrapper : Objects[InName])
		{
			if (bAllowMultiSpawn || !Wrapper->IsInUse())
			{
				Wrapper->Spawn();
				UE_LOG(LogCommonPool, Verbose, TEXT("[UCommonObjectPool] '{%s}' Spawn, Spawn Count: %d."), *Name.ToString(), Wrapper->GetSpawnCount())
				return Wrapper->Peek();
			}
		}
	}

	return nullptr;
}

void UCommonObjectPool::Despawn(const UCommonObjectBase* Object)
{
	DespawnByUObject(Object->GetTarget());
}

void UCommonObjectPool::DespawnByUObject(const UObject* Object)
{
	UCommonObjectWrapper* Wrapper = GetCommonObjectWrapper(Object);
	if (Wrapper == nullptr) return;
	if (!Wrapper->IsInUse())
	{
		UE_LOG(LogCommonPool, Warning, TEXT("[UCommonObjectPool] Try despawn an unused object {'%s'}."), *Wrapper->GetObjectName().ToString())
		return;
	}

	Wrapper->Despawn();
	UE_LOG(LogCommonPool, Verbose, TEXT("[UCommonObjectPool] Despawn {'%s'}."), *Name.ToString())
	if (ObjectMap.Num() > Capacity && Wrapper->GetSpawnCount() <= 0)
	{
		Release();
	}
}

void UCommonObjectPool::SetLocked(const UCommonObjectBase* Object, const bool IsLocked)
{
	if (Object != nullptr)
	{
		SetLockedByUObject(Object->GetTarget(), IsLocked);
	}
}

void UCommonObjectPool::SetLockedByUObject(UObject* Object, const bool IsLocked)
{
	const UCommonObjectWrapper* Wrapper = GetCommonObjectWrapper(Object);
	if (Wrapper == nullptr)
	{
		UE_LOG(
			LogCommonPool, Error, TEXT("[UCommonObjectPool] Can not find target in object pool '{%s%s}', target class is '{%s}', target value is '{%s}'."),
			*ObjectClass->GetName(), *Name.ToString(), Object != nullptr ? *Object->StaticClass()->GetName() : TEXT("(None)"), *GetNameSafe(Object)
		)
		return;
	}

	Wrapper->SetLocked(IsLocked);
}

void UCommonObjectPool::SetObjectPriority(const UCommonObjectBase* Object, const int InPriority)
{
	SetObjectPriorityByUObject(Object->GetTarget(), InPriority);
}

void UCommonObjectPool::SetObjectPriorityByUObject(UObject* Object, const int InPriority)
{
	const UCommonObjectWrapper* Wrapper = GetCommonObjectWrapper(Object);
	if (Wrapper == nullptr)
	{
		UE_LOG(
			LogCommonPool, Error, TEXT("[UCommonObjectPool] Can not find target in object pool '{%s%s}', target class is '{%s}', target value is '{%s}'."),
			*ObjectClass->GetName(), *Name.ToString(), Object != nullptr ? *Object->StaticClass()->GetName() : TEXT("(None)"), *GetNameSafe(Object)
		)
		return;
	}

	Wrapper->SetPriority(InPriority);
}

bool UCommonObjectPool::ReleaseObject(const UCommonObjectBase* Object)
{
	return ReleaseObjectByUObject(Object->GetTarget());
}

bool UCommonObjectPool::ReleaseObjectByUObject(UObject* Object)
{
	UCommonObjectWrapper* Wrapper = GetCommonObjectWrapper(Object);
	if (Wrapper == nullptr || Wrapper->IsInUse() || Wrapper->IsLocked())
	{
		return false;
	}

	Objects[Wrapper->GetObjectName()].Remove(Wrapper);
	ObjectMap.Remove(Object);
	Wrapper->Release(false);
	FCommonReferencePoolStatic::Release(Wrapper);
	return true;
}

void UCommonObjectPool::Release(int32 ToReleaseCount, const FReleaseObjectFilterDelegate& Filter)
{
	if (ToReleaseCount < 0)
	{
		ToReleaseCount = 0;
	}

	auto Time = FDateTime::MinValue();
	if (ExpireTime < TNumericLimits<float>::Max())
	{
		Time = FDateTime::UtcNow() - FTimespan(0, 0, ExpireTime);
	}

	AutoReleaseTime = 0.f;
	ClearInvalidWrappers();
	GetCanReleaseObjects(CachedCanReleaseObjects);
	TArray<TObjectPtr<UCommonObjectBase>> ReleaseObjects {};
	Filter.ExecuteIfBound(CachedCanReleaseObjects, ToReleaseCount, Time, ReleaseObjects);
	for (auto& Object : ReleaseObjects)
	{
		ReleaseObject(Object.Get());
	}
}

void UCommonObjectPool::ReleaseAllUnused()
{
	AutoReleaseTime = 0.f;
	ClearInvalidWrappers();
	GetCanReleaseObjects(CachedCanReleaseObjects);
	for (auto& Object : CachedCanReleaseObjects)
	{
		ReleaseObject(Object.Get());
	}
}

void UCommonObjectPool::GetUsingObject(const FName& ObjectName, TArray<UCommonObjectBase*>& Result)
{
	Result.Empty();
	if (Objects.Contains(ObjectName))
	{
		for (const TObjectPtr<UCommonObjectWrapper>& Wrapper : Objects[ObjectName])
		{
			if (Wrapper->IsInUse())
			{
				Result.Emplace(Wrapper->Peek());
			}
		}
	}
}

void UCommonObjectPool::GetAllUsingObjects(TArray<UCommonObjectBase*>& Result) const
{
	Result.Empty();
	for (auto& [_, Wrapper] : ObjectMap)
	{
		if (Wrapper->IsInUse())
		{
			Result.Emplace(Wrapper->Peek());
		}
	}
}

void UCommonObjectPool::GetRegisteredObjects(const FName& ObjectName, TArray<UCommonObjectBase*>& Result)
{
	Result.Empty();
	if (Objects.Contains(ObjectName))
	{
		for (const TObjectPtr<UCommonObjectWrapper>& Wrapper : Objects[ObjectName])
		{
			Result.Emplace(Wrapper->Peek());
		}
	}
}

void UCommonObjectPool::GetAllRegisteredObjects(TArray<UCommonObjectBase*>& Result) const
{
	Result.Empty();
	for (auto& [_, Wrapper] : ObjectMap)
	{
		Result.Emplace(Wrapper->Peek());
	}
}

TArray<FCommonObjectDebugInfo> UCommonObjectPool::GetObjectDebugInfos() const
{
	TArray<FCommonObjectDebugInfo> Result;
	for (auto& [_, Wrapper] : ObjectMap)
	{
		Result.Emplace(Wrapper->GetDebugInfo());
	}
	return Result;
}

void UCommonObjectPool::GetCanReleaseObjects(TArray<TObjectPtr<UCommonObjectBase>>& Result)
{
	Result.Empty();
	for (auto& [Obj, Wrapper] : ObjectMap)
	{
		if (!Wrapper->IsInUse() && !Wrapper->IsLocked())
		{
			Result.Emplace(Wrapper->Peek());
		}
	}
}

void UCommonObjectPool::ClearInvalidWrappers()
{
	TMap<TObjectPtr<UObject>, TObjectPtr<UCommonObjectWrapper>> NewObjectMap;
	for (auto& [Obj, Wrapper] : ObjectMap)
	{
		if (Obj == nullptr)
		{
			if (Wrapper->IsLocked()) Wrapper->SetLocked(false);
			if (Wrapper->IsInUse())
			{
				for (int i = 0; i < Wrapper->GetSpawnCount(); ++i)
				{
					Wrapper->Despawn();
				}
			}

			Objects[Wrapper->GetObjectName()].Remove(Wrapper);
			Wrapper->Release(false);
			FCommonReferencePoolStatic::Release(Wrapper);
		}
		else
		{
			NewObjectMap.Emplace(Obj, Wrapper);
		}
	}
	ObjectMap = MoveTemp(NewObjectMap);
}

void UCommonObjectPool::OnDefaultReleaseObjectFilter(TArray<TObjectPtr<UCommonObjectBase>>& CandidateObjects, int ToReleaseCount, const FDateTime& ExpireTime, TArray<TObjectPtr<UCommonObjectBase>>& Result)
{
	Result.Empty();

	// 将超过过期时间的对象加入结果中
	if (ExpireTime > FDateTime::MinValue())
	{
		for (int i = CandidateObjects.Num() - 1; i >= 0; --i)
		{
			if (CandidateObjects[i]->GetLastUseTime() > ExpireTime)
			{
				continue;
			}
			Result.Emplace(CandidateObjects[i]);
			CandidateObjects.RemoveAt(i);
		}

		ToReleaseCount -= Result.Num();
	}

	// 如果还有需要释放对象，则将带评估对象列表排序，依顺序添加到结果中
	if (ToReleaseCount > 0 && CandidateObjects.Num() > 0)
	{
		CandidateObjects.Sort(FCommonObjectComparer());
		for (int i = 0; i < ToReleaseCount && i < CandidateObjects.Num(); ++i)
		{
			Result.Emplace(CandidateObjects[i]);
		}
	}
}

UCommonObjectWrapper* UCommonObjectPool::GetCommonObjectWrapper(const UObject* Object)
{
	if (Object != nullptr && ObjectMap.Contains(Object))
	{
		return ObjectMap[Object].Get();
	}

	return nullptr;
}
