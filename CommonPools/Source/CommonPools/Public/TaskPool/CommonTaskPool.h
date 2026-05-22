// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTaskAgent.h"
#include "CommonTaskBase.h"
#include "UObject/Object.h"
#include "CommonTaskPool.generated.h"

/**
 * 
 */
UCLASS()
class COMMONPOOLS_API UCommonTaskPool : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//~ Begin FTickableObjectBase
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UCommonTaskPool, STATGROUP_Tickables); }
	virtual bool IsTickable() const override;
	virtual void Tick(float DeltaTime) override;
	//~ End FTickableObjectBase

public:
	// 关闭任务池
	UFUNCTION(BlueprintCallable, Category="CommonTaskPool")
	void Shutdown();

	// 获取任务池代理数量
	UFUNCTION(BlueprintPure, Category="CommonTaskPool")
	int32 GetTotalAgentCount() const { return FreeAgents.Num() + WorkingAgents.Num(); }

	// 获取空闲代理数量
	UFUNCTION(BlueprintPure, Category="CommonTaskPool")
	int32 GetFreeAgentCount() const { return FreeAgents.Num(); }

	// 获取工作中的代理数量
	UFUNCTION(BlueprintPure, Category="CommonTaskPool")
	int32 GetWorkingAgentCount() const { return WorkingAgents.Num(); }

	// 获取等待中的任务数量
	UFUNCTION(BlueprintPure, Category="CommonTaskPool")
	int32 GetWaitingTaskCount() const { return WaitingTasks.Num(); }

	// 添加代理
	UFUNCTION(BlueprintCallable, Category="CommonTaskPool")
	void AddAgent(UCommonTaskAgent* Agent);

	// 添加任务
	UFUNCTION(BlueprintCallable, Category="CommonTaskPool")
	void AddTask(UCommonTaskBase* Task);

	// 通过序列号移除任务
	UFUNCTION(BlueprintCallable, Category="CommonTaskPool")
	bool RemoveTask(const int32 SerialId);

	// 通过Tag移除任务
	UFUNCTION(BlueprintCallable, Category="CommonTaskPool")
	int32 RemoveTasks(const FGameplayTag& Tag);

	// 移除所有任务
	UFUNCTION(BlueprintCallable, Category="CommonTaskPool")
	void ClearTasks();

	// 通过序列号移除任务
	UFUNCTION(BlueprintCallable, Category="CommonTaskPool")
	bool HasTask(const int32 SerialId);

private:
	bool bPaused { false };

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCommonTaskBase>> WaitingTasks { nullptr };

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCommonTaskAgent>> FreeAgents { nullptr };

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCommonTaskAgent>> WorkingAgents { nullptr };

	void HandleWorkingAgents(const float DeltaTime);
	void HandleWaitingTasks(const float DeltaTime);
};
