// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ModularGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class MODULARGAMEPLAYACTORS_API AModularGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

protected:
	//~ Begin AActor
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor
};
