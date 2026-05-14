// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularGameMode.h"

#include "ModularGameState.h"
#include "ModularHUD.h"
#include "ModularPawn.h"
#include "ModularPlayerController.h"
#include "ModularPlayerState.h"
#include "Components/GameFrameworkComponentManager.h"

AModularGameMode::AModularGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AModularGameState::StaticClass();
	PlayerControllerClass = AModularPlayerController::StaticClass();
	PlayerStateClass = AModularPlayerState::StaticClass();
	DefaultPawnClass = AModularPawn::StaticClass();
	HUDClass = AModularHUD::StaticClass();
}

void AModularGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AModularGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
}

void AModularGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}
