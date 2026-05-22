// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonEventPool.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CommonEventPoolSubsystem.generated.h"

/**
 * 事件池子系统
 * 提供全局事件池访问和管理接口
 */
UCLASS()
class COMMONPOOLS_API UCommonEventPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	//~ Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem

public:
	static UCommonEventPoolSubsystem* Get(const UObject* WorldContextObject);

	/**
	 * 获取事件池
	 * @return 事件池实例
	 */
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	UCommonEventPool* GetEventPool() const { return EventPool; }

	/**
	 * 订阅事件
	 * @param OutHandle 事件句柄
	 * @param InEventClass 事件类
	 * @param InEventCallback 事件回调
	 * @param bMatchChild 是否匹配子类事件
	 */
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	void Subscribe(FCommonEventHandle& OutHandle, const TSubclassOf<UCommonEventArgsBase> InEventClass, const FCommonEventDynamicDelegate& InEventCallback, const bool bMatchChild = false);

	/**
	 * 订阅事件
	 * @param OutHandle 事件句柄
	 * @param InEventClass 事件类
	 * @param InEventCallback 事件回调
	 * @param bMatchChild 是否匹配子类事件
	 */
	template <typename EventClass>
	void Subscribe(FCommonEventHandle& OutHandle, const TSubclassOf<EventClass>& InEventClass, TFunction<void(UObject*, EventClass*)>&& InEventCallback, const bool bMatchChild = false)
	{
		EventPool->Subscribe<EventClass>(OutHandle, InEventClass, MoveTemp(InEventCallback), bMatchChild);
	}

	/**
	 * 订阅事件
	 * @param OutHandle 事件句柄
	 * @param InEventClass 事件类
	 * @param Object 回调对象
	 * @param Function 成员函数指针
	 * @param bMatchChild 是否匹配子类事件
	 */
	template <typename EventClass, typename TOwner = UObject>
	void Subscribe(FCommonEventHandle& OutHandle, const TSubclassOf<EventClass>& InEventClass, TOwner* Object, void (TOwner::*Function)(UObject*, EventClass*), const bool bMatchChild = false)
	{
		EventPool->Subscribe<EventClass>(OutHandle, InEventClass, Object, Function, bMatchChild);
	}

	/**
	 * 订阅事件
	 * @param OutHandle 事件句柄
	 * @param InEventCallback 事件回调
	 * @param bMatchChild 是否匹配子类事件
	 */
	template <typename EventClass>
	void Subscribe(FCommonEventHandle& OutHandle, TFunction<void(UObject*, EventClass*)>&& InEventCallback, const bool bMatchChild = false)
	{
		EventPool->Subscribe<EventClass>(OutHandle, MoveTemp(InEventCallback), bMatchChild);
	}

	/**
	 * 订阅事件
	 * @param OutHandle 事件句柄
	 * @param Object 回调对象
	 * @param Function 成员函数指针
	 * @param bMatchChild 是否匹配子类事件
	 */
	template <typename EventClass, typename TOwner = UObject>
	void Subscribe(FCommonEventHandle& OutHandle, TOwner* Object, void (TOwner::*Function)(UObject*, EventClass*), const bool bMatchChild = false)
	{
		EventPool->Subscribe<EventClass>(OutHandle, Object, Function, bMatchChild);
	}

	/**
	 * 取消订阅事件
	 * @param InHandle 事件句柄
	 */
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	void Unsubscribe(FCommonEventHandle& InHandle);

	/**
	 * 触发事件
	 * @param Sender 发送者
	 * @param EventArgs 事件参数
	 */
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	void Fire(UObject* Sender, UCommonEventArgsBase* EventArgs);

	/**
	 * 立即触发事件
	 * @param Sender 发送者
	 * @param EventArgs 事件参数
	 */
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	void FireNow(UObject* Sender, UCommonEventArgsBase* EventArgs);

	/**
	 * 检查句柄是否有效
	 * @param InHandle 事件句柄
	 * @return 是否有效
	 */
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	bool Check(const FCommonEventHandle& InHandle) const;

	/**
	 * 获取事件订阅数量
	 * @return 订阅数量
	 */
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	int32 GetEventSubscribeCount() const;

	/**
	 * 获取指定事件类订阅数量
	 * @param EventClass 事件类
	 * @return 订阅数量
	 */
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	int32 GetEventSubscribeCountByClass(const TSubclassOf<UCommonEventArgsBase> EventClass) const;

	/**
	 * 获取待处理事件数量
	 * @return 事件数量
	 */
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	int32 GetEventCount() const;

	/**
	 * 清空事件池
	 */
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	void Empty();

private:
	UPROPERTY(Transient)
	TObjectPtr<UCommonEventPool> EventPool { nullptr };
};
