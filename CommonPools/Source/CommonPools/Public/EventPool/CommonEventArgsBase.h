// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReferencePool/CommonReferenceObjectInterface.h"
#include "UObject/Object.h"
#include "CommonEventArgsBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class COMMONPOOLS_API UCommonEventArgsBase : public UObject, public ICommonReferenceObjectInterface
{
	GENERATED_BODY()

public:
	//~ Begin ICommonReferenceObjectInterface
	virtual void Clear() override;
	//~ End ICommonReferenceObjectInterface
};
