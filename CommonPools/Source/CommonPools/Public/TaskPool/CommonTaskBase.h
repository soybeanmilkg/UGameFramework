// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "ReferencePool/CommonReferenceObjectInterface.h"
#include "UObject/Object.h"
#include "CommonTaskBase.generated.h"

/**
 * 任务状态
 */
UENUM(BlueprintType)
enum class ECommonTaskStatus : uint8
{
	// 未开始
	Todo = 0,
	// 执行中
	Doing,
	// 完成
	Done,
};

/**
 * 开始处理任务的状态
 */
UENUM(BlueprintType)
enum class ECommonStartTaskStatus : uint8
{
	// 可以立刻处理完成此任务
	Done = 0,
	// 可以继续处理此任务
	CanResume,
	// 不能继续处理此任务，需等待其它任务执行完成
	HasToWait,
	// 不能继续处理此任务，出现未知错误
	UnknownError,
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class COMMONPOOLS_API UCommonTaskBase : public UObject
	, public ICommonReferenceObjectInterface
	, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	/**
	 * 创建任务
	 * 
	 * @tparam T 任务类型 
	 * @param WorldContextObject World上下文对象
	 * @param InSerialId 任务序列号 
	 * @param InOuter 外部对象
	 * @param InTags 任务标签
	 * @param InPriority 任务优先级
	 * @param InUserData 任务用户数据
	 * @return 
	 */
	template <typename T>
	static T* Create(UObject* WorldContextObject, const int32 InSerialId, UObject* InOuter = nullptr, const FGameplayTagContainer& InTags = FGameplayTagContainer(), const int32 InPriority = 0, UObject* InUserData = nullptr)
	{
		return CreateTask(WorldContextObject, InSerialId, T::StaticClass(), InOuter, InTags, InPriority, InUserData);
	}

	// 创建任务
	UFUNCTION(BlueprintCallable, Category="CommonTaskPool", meta=(AdvancedDisplay="InTag,InPriority,InUserData"))
	static UCommonTaskBase* CreateTask(
		const int32 InSerialId, const TSubclassOf<UCommonTaskBase> InTaskClass, UObject* InOuter = nullptr,
		const FGameplayTagContainer& InTags = FGameplayTagContainer(), const int32 InPriority = 0, UObject* InUserData = nullptr);

public:
	// 是否完成
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsDone() const { return Status == ECommonTaskStatus::Done; }

	// 获取任务状态
	UFUNCTION(BlueprintPure)
	FORCEINLINE ECommonTaskStatus GetStatus() const { return Status; }

	// 获取序列号
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetSerialId() const { return SerialId; }

	// 获取任务优先级
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetPriority() const { return Priority; }

	// 获取用户数据
	UFUNCTION(BlueprintPure)
	FORCEINLINE UObject* GetObject() const { return UserData.Get(); }

	//~ Begin IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	//~ End IGameplayTagAssetInterface

	// 设置任务状态
	UFUNCTION(BlueprintCallable)
	void SetTaskStatus(const ECommonTaskStatus InTaskStatus) { Status = InTaskStatus; }

	// 任务结束
	UFUNCTION(BlueprintCallable)
	void TaskEnd();

public:
	virtual void Initialize(const int32 InSerialId, const FGameplayTagContainer& InTags, const int32 InPriority, UObject* InUserData);

	virtual ECommonStartTaskStatus Start();

	//~ Begin ICommonReferenceObjectInterface
	virtual void Clear() override;
	//~ End ICommonReferenceObjectInterface

private:
	ECommonTaskStatus Status { ECommonTaskStatus::Todo };
	int32 SerialId { 0 };
	int32 Priority { 0 };
	FGameplayTagContainer Tags {};
	UPROPERTY(Transient)
	TObjectPtr<UObject> UserData { nullptr };
};
