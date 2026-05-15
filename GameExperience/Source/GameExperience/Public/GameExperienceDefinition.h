// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameExperienceDefinition.generated.h"

class UGameExperienceActionSet;
class UGameFeatureAction;

/**
 * Definition of an experience
 */
UCLASS(BlueprintType, Const)
class GAMEEXPERIENCE_API UGameExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UGameExperienceDefinition();

	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	//~UPrimaryDataAsset interface
#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	//~End of UPrimaryDataAsset interface

public:
	// List of Game Feature Plugins this experience wants to have active
	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	TArray<FString> GameFeaturesToEnable;

	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	// List of additional action sets to compose into this experience
	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	TArray<TObjectPtr<UGameExperienceActionSet>> ActionSets;
};
