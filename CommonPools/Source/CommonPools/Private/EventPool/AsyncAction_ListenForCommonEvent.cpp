// Fill out your copyright notice in the Description page of Project Settings.

#include "EventPool/AsyncAction_ListenForCommonEvent.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EventPool/CommonEventPoolSubsystem.h"
#include "UObject/ScriptMacros.h"
#include "UObject/Stack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ListenForCommonEvent)

UAsyncAction_ListenForCommonEvent* UAsyncAction_ListenForCommonEvent::ListenForCommonEvent(UObject* WorldContextObject, TSubclassOf<UCommonEventArgsBase> EventClass, const bool bInMatchChild)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	UAsyncAction_ListenForCommonEvent* Action = NewObject<UAsyncAction_ListenForCommonEvent>();
	Action->WorldPtr = World;
	Action->bMatchChild = bInMatchChild;
	Action->EventClass = EventClass;
	Action->RegisterWithGameInstance(World);

	return Action;
}

void UAsyncAction_ListenForCommonEvent::Activate()
{
	if (const UWorld* World = WorldPtr.Get())
	{
		if (UCommonEventPoolSubsystem* Subsystem = UCommonEventPoolSubsystem::Get(World))
		{
			Subsystem->Subscribe(EventHandle, EventClass, this, &ThisClass::HandleEvent, bMatchChild);
			return;
		}
	}

	SetReadyToDestroy();
}

void UAsyncAction_ListenForCommonEvent::SetReadyToDestroy()
{
	if (const UWorld* World = WorldPtr.Get())
	{
		if (UCommonEventPoolSubsystem* Subsystem = UCommonEventPoolSubsystem::Get(World))
		{
			Subsystem->Unsubscribe(EventHandle);
		}
	}

	Super::SetReadyToDestroy();
}

bool UAsyncAction_ListenForCommonEvent::GetEventArgs(UCommonEventArgsBase*& OutArgs)
{
	OutArgs = ReceivedEventArgs.Get();
	return OutArgs != nullptr;
}

void UAsyncAction_ListenForCommonEvent::HandleEvent(UObject* Sender, UCommonEventArgsBase* EventArgs)
{
	ReceivedEventArgs = EventArgs;
	OnEventReceived.Broadcast(Sender, EventArgs);
	ReceivedEventArgs = nullptr;

	if (!OnEventReceived.IsBound())
	{
		SetReadyToDestroy();
	}
}
