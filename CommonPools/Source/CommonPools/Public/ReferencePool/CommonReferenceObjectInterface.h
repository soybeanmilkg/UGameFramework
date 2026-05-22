// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CommonReferenceObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCommonReferenceObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COMMONPOOLS_API ICommonReferenceObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Clear() {}

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Clear"))
	void ReceiveClear();
};
