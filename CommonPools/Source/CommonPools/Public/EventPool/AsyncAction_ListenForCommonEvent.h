// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "CommonEventArgsBase.h"
#include "CommonEventHandle.h"
#include "AsyncAction_ListenForCommonEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsyncCommonEventDelegate, UObject*, Sender, UCommonEventArgsBase*, EventArgs);

/**
 * 事件监听异步Action
 * 用于 Blueprint 异步监听事件
 */
UCLASS(MinimalAPI, BlueprintType)
class UAsyncAction_ListenForCommonEvent : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	/**
	 * 异步监听事件
	 * @param WorldContextObject World上下文
	 * @param EventClass 事件类
	 * @param bMatchChild 是否匹配子类事件
	 */
	UFUNCTION(BlueprintCallable, Category="CommonEventPool", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
	static UAsyncAction_ListenForCommonEvent* ListenForCommonEvent(UObject* WorldContextObject, TSubclassOf<UCommonEventArgsBase> EventClass, bool bMatchChild = false);

	/**
	 * 收到事件时的回调
	 */
	UPROPERTY(BlueprintAssignable)
	FAsyncCommonEventDelegate OnEventReceived;

	/**
	 * 获取事件参数
	 * @param OutArgs 事件参数输出
	 * @return 是否成功
	 */
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	bool GetEventArgs(UCommonEventArgsBase*& OutArgs);

	//~ Begin UBlueprintAsyncActionBase
	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;
	//~ End UBlueprintAsyncActionBase

private:
	void HandleEvent(UObject* Sender, UCommonEventArgsBase* EventArgs);

	TWeakObjectPtr<UWorld> WorldPtr { nullptr };
	TSubclassOf<UCommonEventArgsBase> EventClass { nullptr };
	FCommonEventHandle EventHandle {};
	TWeakObjectPtr<UCommonEventArgsBase> ReceivedEventArgs { nullptr };
	bool bMatchChild { false };
};
