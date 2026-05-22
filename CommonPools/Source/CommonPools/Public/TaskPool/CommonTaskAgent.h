// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTaskBase.h"
#include "UObject/Object.h"
#include "CommonTaskAgent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class COMMONPOOLS_API UCommonTaskAgent : public UObject
{
	GENERATED_BODY()

public:
	// 获取当前执行的任务
	UCommonTaskBase* GetTask() { return Task; }

	virtual ECommonStartTaskStatus Start(UCommonTaskBase* InTask);
	virtual void Reset();
	virtual void Initialize() {}
	virtual void Tick(const float DeltaTime) {}
	virtual void Shutdown() {}

private:
	UPROPERTY(Transient)
	TObjectPtr<UCommonTaskBase> Task { nullptr };
};
