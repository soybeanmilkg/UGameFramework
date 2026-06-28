// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReferencePool/CommonReferenceObjectInterface.h"
#include "UObject/Object.h"
#include "CommonObjectWrapper.generated.h"

class UCommonObjectBase;

USTRUCT(BlueprintType)
struct FCommonObjectDebugInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName ObjectName { NAME_None };

	UPROPERTY(BlueprintReadOnly)
	bool IsLocked { false };

	UPROPERTY(BlueprintReadOnly)
	int32 Priority { 0 };

	UPROPERTY(BlueprintReadOnly)
	int32 SpawnCount { 0 };

	UPROPERTY(BlueprintReadOnly)
	FString LastUseTime { TEXT("") };
};

/**
 * 
 */
UCLASS()
class COMMONPOOLS_API UCommonObjectWrapper : public UObject, public ICommonReferenceObjectInterface
{
	GENERATED_BODY()

public:
	FName GetObjectName() const;
	bool IsLocked() const;
	void SetLocked(const bool Locked) const;
	int32 GetPriority() const;
	void SetPriority(const int32 Priority) const;
	bool IsInUse() const { return SpawnCount > 0; }
	FDateTime GetLastUseTime() const;
	int32 GetSpawnCount() const { return SpawnCount; }

	UCommonObjectBase* Peek() const { return Object.Get(); }
	UCommonObjectBase* Spawn();
	void Despawn();

	void Release(const bool IsShutdown) const;

	//~ Begin ICommonReferenceObjectInterface
	virtual void Clear() override;
	//~ End ICommonReferenceObjectInterface

	static UCommonObjectWrapper* Create(UCommonObjectBase* Object, const bool bSpawned);

	FCommonObjectDebugInfo GetDebugInfo() const;

private:
	UPROPERTY(Transient)
	TObjectPtr<UCommonObjectBase> Object { nullptr };

	int32 SpawnCount { 0 };
};
