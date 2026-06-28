// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReferencePool/CommonReferenceObjectInterface.h"
#include "UObject/Object.h"
#include "CommonObjectBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class COMMONPOOLS_API UCommonObjectBase : public UObject, public ICommonReferenceObjectInterface
{
	GENERATED_BODY()

public:
	virtual void Initialize(UObject* Object, const bool InIsLocked = false, const int32 InPriority = 0);

	virtual void OnSpawn() {}
	virtual void OnDespawn() {}

	virtual void Release(const bool IsShutdown) {}

	//~ Begin ICommonReferenceObjectInterface
	virtual void Clear() override;
	//~ End ICommonReferenceObjectInterface

	UFUNCTION(BlueprintPure, Category="CommonObjectPool")
	UObject* GetTarget() const { return Target.Get(); }

	UFUNCTION(BlueprintPure, Category="CommonObjectPool")
	bool IsLocked() const { return bIsLocked; }

	UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
	void SetLocked(const bool Locked);

	UFUNCTION(BlueprintPure, Category="CommonObjectPool")
	int32 GetPriority() const { return Priority; }

	UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
	void SetPriority(const int32 InPriority);

	UFUNCTION(BlueprintPure, Category="CommonObjectPool")
	FDateTime GetLastUseTime() const { return LastUseTime; }

	UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
	void RefreshLastUseTime();

private:
	bool bIsLocked { false };

	UPROPERTY(Transient)
	TObjectPtr<UObject> Target { nullptr };

	int32 Priority { 0 };

	FDateTime LastUseTime { FDateTime::UtcNow() };
};
