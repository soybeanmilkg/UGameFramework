// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoadingProcessInterface.h"
#include "Components/GameStateComponent.h"
#include "GameExperienceManagerComponent.generated.h"

namespace UE::GameFeatures
{
	struct FResult;
}

class UGameExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameExperienceLoaded, const UGameExperienceDefinition* /*Experience*/);

enum class EGameExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	LoadingChaosTestingDelay,
	ExecutingActions,
	Loaded,
	Deactivating
};

UCLASS()
class GAMEEXPERIENCE_API UGameExperienceManagerComponent : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:
	explicit UGameExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//~ Begin UObject
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject

	//~ Begin AActor
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor

public:
	//~ Begin ILoadingProcessInterface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~ End ILoadingProcessInterface

	// Tries to set the current experience, either a UI or gameplay one
	void SetCurrentExperience(const FPrimaryAssetId& ExperienceId);

	// Ensures the delegate is called once the experience has been loaded,
	// before others are called.
	// However, if the experience has already loaded, calls the delegate immediately.
	void CallOrRegister_OnExperienceLoaded_HighPriority(FOnGameExperienceLoaded::FDelegate&& Delegate);

	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	void CallOrRegister_OnExperienceLoaded(FOnGameExperienceLoaded::FDelegate&& Delegate);

	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	void CallOrRegister_OnExperienceLoaded_LowPriority(FOnGameExperienceLoaded::FDelegate&& Delegate);

	// This returns the current experience if it is fully loaded, asserting otherwise
	// (i.e., if you called it too soon)
	const UGameExperienceDefinition* GetCurrentExperienceChecked() const;

	// Returns true if the experience is fully loaded
	bool IsExperienceLoaded() const;

private:
	UPROPERTY(Transient, ReplicatedUsing=OnRep_CurrentExperience)
	TObjectPtr<const UGameExperienceDefinition> CurrentExperience { nullptr };

	UFUNCTION()
	void OnRep_CurrentExperience();

	EGameExperienceLoadState LoadState { EGameExperienceLoadState::Unloaded };

	int32 NumGameFeaturePluginsLoading { 0 };
	TArray<FString> GameFeaturePluginURLs {};

	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;

	/**
	 * Delegate called when the experience has finished loading just before others
	 * (e.g., subsystems that set up for regular gameplay)
	 */
	FOnGameExperienceLoaded OnExperienceLoaded_HighPriority;

	/** Delegate called when the experience has finished loading */
	FOnGameExperienceLoaded OnExperienceLoaded;

	/** Delegate called when the experience has finished loading */
	FOnGameExperienceLoaded OnExperienceLoaded_LowPriority;

	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();

	void OnActionDeactivationCompleted();
	void OnAllActionsDeactivated();
};
