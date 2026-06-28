// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/CommonObjectWrapper.h"

#include "LogCommonPool.h"
#include "ObjectPool/CommonObjectBase.h"
#include "ReferencePool/CommonReferencePoolSubsystem.h"

FName UCommonObjectWrapper::GetObjectName() const
{
	return Object != nullptr ? Object->GetFName() : NAME_None;
}

bool UCommonObjectWrapper::IsLocked() const
{
	return Object != nullptr && Object->IsLocked();
}

void UCommonObjectWrapper::SetLocked(const bool Locked) const
{
	if (Object != nullptr)
	{
		Object->SetLocked(Locked);
	}
}

int32 UCommonObjectWrapper::GetPriority() const
{
	return Object != nullptr ? Object->GetPriority() : 0;
}

void UCommonObjectWrapper::SetPriority(const int32 Priority) const
{
	if (Object != nullptr)
	{
		Object->SetPriority(Priority);
	}
}

FDateTime UCommonObjectWrapper::GetLastUseTime() const
{
	return Object != nullptr ? Object->GetLastUseTime() : FDateTime::MinValue();
}

UCommonObjectBase* UCommonObjectWrapper::Spawn()
{
	if (Object == nullptr)
	{
		UE_LOG(LogCommonPool, Error, TEXT("[UCommonObjectWrapper] Object is invalid."))
		return nullptr;
	}

	SpawnCount++;
	Object->RefreshLastUseTime();
	Object->OnSpawn();

	return Object.Get();
}

void UCommonObjectWrapper::Despawn()
{
	if (Object == nullptr)
	{
		UE_LOG(LogCommonPool, Error, TEXT("[UCommonObjectWrapper] Object is invalid."))
		return;
	}

	Object->OnDespawn();
	Object->RefreshLastUseTime();
	SpawnCount--;
	if (SpawnCount < 0)
	{
		UE_LOG(LogCommonPool, Error, TEXT("[UCommonObjectWrapper] Object '{%s}' spawn count is less than 0."), *GetObjectName().ToString())
	}
}

void UCommonObjectWrapper::Release(const bool IsShutdown) const
{
	if (Object != nullptr)
	{
		Object->Release(IsShutdown);
		FCommonReferencePoolStatic::Release(Object);
	}
}

void UCommonObjectWrapper::Clear()
{
	Object = nullptr;
	SpawnCount = 0;
}

UCommonObjectWrapper* UCommonObjectWrapper::Create(UCommonObjectBase* Object, const bool bSpawned)
{
	if (Object == nullptr)
	{
		UE_LOG(LogCommonPool, Error, TEXT("[UCommonObjectWrapper] Object is invalid."))
		return nullptr;
	}

	const auto Wrapper = FCommonReferencePoolStatic::Acquire<UCommonObjectWrapper>();
	Wrapper->Object = Object;
	Wrapper->SpawnCount = bSpawned ? 1 : 0;
	if (bSpawned)
	{
		Object->OnSpawn();
	}
	return Wrapper;
}

FCommonObjectDebugInfo UCommonObjectWrapper::GetDebugInfo() const
{
	return {
		.ObjectName = GetObjectName(),
		.IsLocked = IsLocked(),
		.Priority = GetPriority(),
		.SpawnCount = GetSpawnCount(),
		.LastUseTime = GetLastUseTime().ToString(),
	};
}
