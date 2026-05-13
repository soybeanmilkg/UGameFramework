// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularPlayerState.h"

#include "Components/GameFrameworkComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Components/PlayerStateComponent.h"

void AModularPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AModularPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
}

void AModularPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void AModularPlayerState::Reset()
{
	Super::Reset();

	for (TComponentIterator<UPlayerStateComponent> It(this); It; ++It)
	{
		It->Reset();
	}
}

void AModularPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	for (TComponentIterator<UPlayerStateComponent> It(this); It; ++It)
	{
		if (UPlayerStateComponent* TargetComp = Cast<UPlayerStateComponent>(FindObjectWithOuter(PlayerState, It->GetClass(), It->GetFName())))
		{
			It->CopyProperties(TargetComp);
		}
	}
}
