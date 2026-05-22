// Fill out your copyright notice in the Description page of Project Settings.


#include "ReferencePool/CommonReferencePoolSubsystem.h"

#include "LogCommonPool.h"
#include "Kismet/GameplayStatics.h"
#include "ReferencePool/CommonReferenceObjectInterface.h"

void UCommonReferencePoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCommonReferencePoolSubsystem::Deinitialize()
{
	Super::Deinitialize();

	RemoveAllPool();
}

UCommonReferencePoolSubsystem* UCommonReferencePoolSubsystem::Get(const UObject* WorldContextObject)
{
	if (const auto GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		return GameInstance->GetSubsystem<UCommonReferencePoolSubsystem>();
	}

	return nullptr;
}

UObject* UCommonReferencePoolSubsystem::Acquire(const UClass* ReferenceClass, UObject* Outer)
{
	if (const auto Pool = GetOrCreateReferencePool(ReferenceClass))
	{
		return Pool->Acquire(Outer);
	}

	return nullptr;
}

void UCommonReferencePoolSubsystem::Release(UObject* InObject)
{
	if (InObject == nullptr)
	{
		return;
	}

	if (const auto Pool = GetOrCreateReferencePool(InObject->GetClass()))
	{
		Pool->Release(InObject);
	}
}

void UCommonReferencePoolSubsystem::Add(const UClass* ReferenceClass, const int32 Count, UObject* Outer)
{
	if (const auto Pool = GetOrCreateReferencePool(ReferenceClass))
	{
		Pool->Add(Count, Outer);
	}
}

void UCommonReferencePoolSubsystem::Remove(const UClass* ReferenceClass, const int32 Count)
{
	if (const auto Pool = GetOrCreateReferencePool(ReferenceClass))
	{
		Pool->Remove(Count);
	}
}

void UCommonReferencePoolSubsystem::Clear(const UClass* ReferenceClass)
{
	if (ReferenceClass && ReferencePools.Contains(ReferenceClass))
	{
		ReferencePools[ReferenceClass].Clear();
	}
}

void UCommonReferencePoolSubsystem::ClearAll()
{
	for (auto& [_, Pool] : ReferencePools)
	{
		Pool.Clear();
	}
}

void UCommonReferencePoolSubsystem::RemovePool(const UClass* ReferenceClass)
{
	if (ReferenceClass && ReferencePools.Contains(ReferenceClass))
	{
		ReferencePools[ReferenceClass].Clear();
		ReferencePools.Remove(ReferenceClass);
	}
}

void UCommonReferencePoolSubsystem::RemoveAllPool()
{
	ClearAll();

	ReferencePools.Empty();
}

TArray<FCommonReferencePoolInfo> UCommonReferencePoolSubsystem::GetAllReferencePoolInfo() const
{
	TArray<FCommonReferencePoolInfo> PoolInfos {};
	PoolInfos.Reserve(ReferencePools.Num());
	for (auto& [_, Pool] : ReferencePools)
	{
		PoolInfos.Add(Pool.GetPoolInfo());
	}
	return PoolInfos;
}

FCommonReferencePoolInfo UCommonReferencePoolSubsystem::GetReferencePoolInfo(const UClass* ReferenceClass) const
{
	if (ReferenceClass && ReferencePools.Contains(ReferenceClass))
	{
		return ReferencePools[ReferenceClass].GetPoolInfo();
	}

	return {};
}

FCommonReferencePool* UCommonReferencePoolSubsystem::GetOrCreateReferencePool(const UClass* ReferenceClass)
{
	if (ReferenceClass == nullptr)
	{
		UE_LOG(LogCommonPool, Error, TEXT("[UCommonReferencePoolSubsystem] ReferenceClass is nullptr."))
		return nullptr;
	}

	if (!ReferenceClass->ImplementsInterface(UCommonReferenceObjectInterface::StaticClass()))
	{
		UE_LOG(LogCommonPool, Error, TEXT("[UCommonReferencePoolSubsystem] %s must implement UCommonReferenceObjectInterface."), *ReferenceClass->GetName())
		return nullptr;
	}

	if (!ReferencePools.Contains(ReferenceClass))
	{
		ReferencePools.Emplace(ReferenceClass, ReferenceClass);
	}

	return &ReferencePools[ReferenceClass];
}
