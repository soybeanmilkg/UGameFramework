// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/CommonObjectBase.h"

void UCommonObjectBase::Initialize(UObject* Object, const bool InIsLocked, const int InPriority)
{
	Target = Object;
	bIsLocked = InIsLocked;
	Priority = InPriority;
	LastUseTime = FDateTime::UtcNow();
}

void UCommonObjectBase::Clear()
{
	Target = nullptr;
	bIsLocked = false;
	Priority = 0;
}

void UCommonObjectBase::SetLocked(const bool Locked)
{
	bIsLocked = Locked;
}

void UCommonObjectBase::SetPriority(const int32 InPriority)
{
	Priority = InPriority;
}

void UCommonObjectBase::RefreshLastUseTime()
{
	LastUseTime = FDateTime::UtcNow();
}
