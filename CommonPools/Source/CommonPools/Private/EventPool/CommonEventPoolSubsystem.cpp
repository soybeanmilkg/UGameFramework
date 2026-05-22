// Fill out your copyright notice in the Description page of Project Settings.


#include "EventPool/CommonEventPoolSubsystem.h"

#include "Kismet/GameplayStatics.h"

void UCommonEventPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	EventPool = NewObject<UCommonEventPool>(this);
}

void UCommonEventPoolSubsystem::Deinitialize()
{
	Super::Deinitialize();

	EventPool->Shutdown();
	EventPool = nullptr;
}

UCommonEventPoolSubsystem* UCommonEventPoolSubsystem::Get(const UObject* WorldContextObject)
{
	if (const auto GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		return GameInstance->GetSubsystem<UCommonEventPoolSubsystem>();
	}

	return nullptr;
}

void UCommonEventPoolSubsystem::Subscribe(FCommonEventHandle& OutHandle, const TSubclassOf<UCommonEventArgsBase> InEventClass, const FCommonEventDynamicDelegate& InEventCallback, const bool bMatchChild)
{
	EventPool->Subscribe(OutHandle, InEventClass, InEventCallback, bMatchChild);
}

void UCommonEventPoolSubsystem::Unsubscribe(FCommonEventHandle& InHandle)
{
	EventPool->Unsubscribe(InHandle);
}

void UCommonEventPoolSubsystem::Fire(UObject* Sender, UCommonEventArgsBase* EventArgs)
{
	EventPool->Fire(Sender, EventArgs);
}

void UCommonEventPoolSubsystem::FireNow(UObject* Sender, UCommonEventArgsBase* EventArgs)
{
	EventPool->FireNow(Sender, EventArgs);
}

bool UCommonEventPoolSubsystem::Check(const FCommonEventHandle& InHandle) const
{
	return EventPool->Check(InHandle);
}

int32 UCommonEventPoolSubsystem::GetEventSubscribeCount() const
{
	return EventPool->GetEventSubscribeCount();
}

int32 UCommonEventPoolSubsystem::GetEventSubscribeCountByClass(const TSubclassOf<UCommonEventArgsBase> EventClass) const
{
	return EventPool->GetEventSubscribeCountByClass(EventClass);
}

int32 UCommonEventPoolSubsystem::GetEventCount() const
{
	return EventPool->GetEventCount();
}

void UCommonEventPoolSubsystem::Empty()
{
	EventPool->Empty();
}
