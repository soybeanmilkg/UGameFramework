// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularGameState.h"

#include "Components/GameFrameworkComponentManager.h"
#include "Components/GameStateComponent.h"

void AModularGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AModularGameState::BeginPlay()
{
	Super::BeginPlay();

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
}

void AModularGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void AModularGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	for (TComponentIterator<UGameStateComponent> It(this); It; ++It)
	{
		It->HandleMatchHasStarted();
	}
}

void AModularGameState::HandleMatchHasEnded()
{
	for (TComponentIterator<UGameStateComponent> It(this); It; ++It)
	{
		It->HandleMatchHasEnded();
	}

	Super::HandleMatchHasEnded();
}
