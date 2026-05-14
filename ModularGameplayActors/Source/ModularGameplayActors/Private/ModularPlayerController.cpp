// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularPlayerController.h"

#include "Components/ControllerComponent.h"
#include "Components/GameFrameworkComponent.h"
#include "Components/GameFrameworkComponentManager.h"

void AModularPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AModularPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void AModularPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	// 玩家控制器被分配给一个玩家之后才算准备好
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	for (TComponentIterator<UControllerComponent> It(this); It; ++It)
	{
		It->ReceivedPlayer();
	}
}

void AModularPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	for (TComponentIterator<UControllerComponent> It(this); It; ++It)
	{
		It->PlayerTick(DeltaTime);
	}
}
