// Fill out your copyright notice in the Description page of Project Settings.


#include "ReferencePool/CommonReferencePool.h"

#include "LogCommonPool.h"
#include "ReferencePool/CommonReferenceObjectInterface.h"

FCommonReferencePoolInfo FCommonReferencePool::GetPoolInfo() const
{
	const FCommonReferencePoolInfo Info
	{
		.ClassName = GetFNameSafe(ReferenceClass),
		.UnusedReferenceCount = ReferenceObjects.Num(),
		.UsingReferenceCount = UsingReferenceCount,
		.AcquireReferenceCount = AcquireReferenceCount,
		.ReleaseReferenceCount = ReleaseReferenceCount,
		.AddReferenceCount = AddReferenceCount,
		.RemoveReferenceCount = RemoveReferenceCount,
	};
	return Info;
}

UObject* FCommonReferencePool::Acquire(UObject* Outer)
{
	check(ReferenceClass != nullptr);

	UsingReferenceCount++;
	AcquireReferenceCount++;

	while (ReferenceObjects.Num() > 0)
	{
		const auto ReferenceObj = ReferenceObjects.Last().Get();
		ReferenceObjects.RemoveAt(ReferenceObjects.Num() - 1);

		// References 内可能会有一些失效的 Object
		if (ReferenceObj == nullptr) continue;

		if (Outer != ReferenceObj->GetOuter() && Outer != nullptr)
		{
			ReferenceObj->Rename(*MakeUniqueObjectName(Outer, ReferenceClass).ToString(), Outer);
		}

		return ReferenceObj;
	}

	return NewObject<UObject>(Outer != nullptr ? Outer : GetTransientPackage(), ReferenceClass);
}

void FCommonReferencePool::Release(UObject* InObject)
{
	if (InObject == nullptr) return;

	check(ReferenceClass != nullptr);

	if (InObject->GetClass() != ReferenceClass)
	{
		UE_LOG(LogCommonPool, Error, TEXT("[FCommonReferencePool] Release Object must be a %s."), *ReferenceClass->GetName())
		return;
	}

	// 如果 ReferenceObjects 数量太多，导致这里出现性能瓶颈，或许可以考虑使用一个 TSet 容器存储 FObjectKey 来判断去重？
	if (ReferenceObjects.Contains(InObject))
	{
		UE_LOG(LogCommonPool, Warning, TEXT("[FCommonReferencePool] '%s' is already release."), *GetNameSafe(InObject))
		return;
	}

	if (const auto Interface = Cast<ICommonReferenceObjectInterface>(InObject))
	{
		Interface->Clear();
	}
	if (InObject->Implements<UCommonReferenceObjectInterface>())
	{
		ICommonReferenceObjectInterface::Execute_ReceiveClear(InObject);
	}

	ReferenceObjects.Emplace(InObject);

	ReleaseReferenceCount++;
	UsingReferenceCount--;
}

void FCommonReferencePool::Add(int32 Count, UObject* Outer)
{
	AddReferenceCount += Count;

	ReferenceObjects.Reserve(ReferenceObjects.Num() + Count);

	while (Count-- > 0)
	{
		ReferenceObjects.Emplace(NewObject<UObject>(Outer != nullptr ? Outer : GetTransientPackage(), ReferenceClass));
	}
}

void FCommonReferencePool::Remove(int32 Count)
{
	const int32 Remain = ReferenceObjects.Num();
	Count = Count > Remain ? Remain : Count;
	RemoveReferenceCount += Count;

	ReferenceObjects.RemoveAt(Remain - Count, Count);
}

void FCommonReferencePool::Clear()
{
	RemoveReferenceCount += ReferenceObjects.Num();
	ReferenceObjects.Empty();
}
