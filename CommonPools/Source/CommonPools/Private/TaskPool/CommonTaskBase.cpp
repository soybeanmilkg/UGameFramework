// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskPool/CommonTaskBase.h"

#include "ReferencePool/CommonReferencePoolSubsystem.h"

UCommonTaskBase* UCommonTaskBase::CreateTask(
	UObject* WorldContextObject, const int32 InSerialId, const TSubclassOf<UCommonTaskBase> InTaskClass,
	UObject* InOuter, const FGameplayTagContainer& InTags, const int32 InPriority, UObject* InUserData)
{
	UCommonTaskBase* Task = Cast<UCommonTaskBase>(FCommonReferencePoolStatic::Acquire<UCommonTaskBase>(WorldContextObject, InTaskClass, InOuter));
	Task->Initialize(InSerialId, InTags, InPriority, InUserData);
	return Task;
}

void UCommonTaskBase::Clear()
{
	Status = ECommonTaskStatus::Todo;
	SerialId = 0;
	Tags.Reset();
	Priority = 0;
	UserData = nullptr;
}

void UCommonTaskBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.Reset();
	TagContainer.AppendTags(Tags);
}

void UCommonTaskBase::TaskEnd()
{
	Status = ECommonTaskStatus::Done;
}

void UCommonTaskBase::Initialize(const int32 InSerialId, const FGameplayTagContainer& InTags, const int32 InPriority, UObject* InUserData)
{
	Status = ECommonTaskStatus::Todo;
	SerialId = InSerialId;
	Tags = InTags;
	Priority = InPriority;
	UserData = InUserData;
}

ECommonStartTaskStatus UCommonTaskBase::Start()
{
	TaskEnd();
	return ECommonStartTaskStatus::Done;
}
