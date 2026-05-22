// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonEventHandle.h"
#include "UObject/Object.h"
#include "CommonEventPool.generated.h"

// 订阅数据
struct FCommonSubscribeData
{
	// 订阅的事件类
	TSubclassOf<UCommonEventArgsBase> EventClass { nullptr };

	// 订阅的回调
	FCommonEventUnifiedDelegate Callback {};

	// 是否匹配子类事件
	bool bMatchChild { false };
};

/**
 * 
 */
UCLASS(BlueprintType)
class COMMONPOOLS_API UCommonEventPool : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//~ Begin FTickableObjectBase
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UCommonEventPool, STATGROUP_Tickables); }
	virtual bool IsTickable() const override;
	virtual void Tick(float DeltaTime) override;
	//~ End FTickableObjectBase

public:
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	void Shutdown();

	// 清空事件池
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	FORCEINLINE void Empty() { Events.Empty(); }

	// 获取事件数量
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	FORCEINLINE int32 GetEventCount() const { return Events.Num(); }

	// 获取事件订阅数量
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	FORCEINLINE int32 GetEventSubscribeCount() const { return SubscribeData.Num(); }

	// 获取指定事件类订阅数量
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	int32 GetEventSubscribeCountByClass(const TSubclassOf<UCommonEventArgsBase> EventClass);

	// 检查句柄是否有效
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	bool Check(const FCommonEventHandle& InHandle) const;

	// 订阅事件
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	void Subscribe(FCommonEventHandle& OutHandle, const TSubclassOf<UCommonEventArgsBase> InEventClass, const FCommonEventDynamicDelegate InEventCallback, const bool bMatchChild = false)
	{
		InternalSubscribe(OutHandle, InEventClass, FCommonEventUnifiedDelegate(InEventCallback), bMatchChild);
	}

	// 订阅事件
	template <typename EventClass>
	void Subscribe(FCommonEventHandle& OutHandle, const TSubclassOf<EventClass>& InEventClass, TFunction<void(UObject*, EventClass*)>&& InEventCallback, const bool bMatchChild = false)
	{
		auto ThunkCallback = [InnerCallback = MoveTemp(InEventCallback)](UObject* Sender, UCommonEventArgsBase* EventArgs)
		{
			InnerCallback(Sender, Cast<EventClass>(EventArgs));
		};
		return InternalSubscribe(OutHandle, InEventClass, FCommonEventUnifiedDelegate(Forward<FCommonEventFunction>(ThunkCallback)), bMatchChild);
	}

	// 订阅事件
	template <typename EventClass, typename TOwner = UObject>
	void Subscribe(FCommonEventHandle& OutHandle, const TSubclassOf<EventClass>& InEventClass, TOwner* Object, void (TOwner::*Function)(UObject*, EventClass*), const bool bMatchChild = false)
	{
		TWeakObjectPtr<TOwner> WeakObject(Object);
		return Subscribe<EventClass>(OutHandle, InEventClass, [WeakObject, Function](UObject* Sender, EventClass* EventArgs)
		{
			if (TOwner* StrongObject = WeakObject.Get())
			{
				(StrongObject->*Function)(Sender, EventArgs);
			}
		}, bMatchChild);
	}

	// 订阅事件
	template <typename EventClass>
	void Subscribe(FCommonEventHandle& OutHandle, TFunction<void(UObject*, EventClass*)>&& InEventCallback, const bool bMatchChild = false)
	{
		auto ThunkCallback = [InnerCallback = MoveTemp(InEventCallback)](UObject* Sender, UCommonEventArgsBase* EventArgs)
		{
			InnerCallback(Sender, Cast<EventClass>(EventArgs));
		};
		return InternalSubscribe(OutHandle, EventClass::StaticClass(), FCommonEventUnifiedDelegate(Forward<FCommonEventFunction>(ThunkCallback)), bMatchChild);
	}

	// 订阅事件
	template <typename EventClass, typename TOwner = UObject>
	void Subscribe(FCommonEventHandle& OutHandle, TOwner* Object, void (TOwner::*Function)(UObject*, EventClass*), const bool bMatchChild = false)
	{
		TWeakObjectPtr<TOwner> WeakObject(Object);
		return Subscribe<EventClass>(OutHandle, [WeakObject, Function](UObject* Sender, EventClass* EventArgs)
		{
			if (TOwner* StrongObject = WeakObject.Get())
			{
				(StrongObject->*Function)(Sender, EventArgs);
			}
		}, bMatchChild);
	}

	// 取消订阅事件
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	void Unsubscribe(FCommonEventHandle& InHandle);

	// 触发事件
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	void Fire(UObject* Sender, UCommonEventArgsBase* EventArgs);

	// 立即触发事件
	UFUNCTION(BlueprintCallable, Category="CommonEventPool")
	void FireNow(UObject* Sender, UCommonEventArgsBase* EventArgs);

private:
	TSparseArray<FCommonSubscribeData> SubscribeData {};
	TSet<FCommonEventHandle> PendingHandles {};
	TMap<TSubclassOf<UCommonEventArgsBase>, TArray<FCommonEventHandle>> EventHandles {};

	UPROPERTY(Transient)
	TArray<FCommonEventData> Events {};
	TArray<FCommonEventData> TempEvents {};

	bool bHandleEvent { false };
	void HandleEvent(const FCommonEventData& Event);

	void InternalSubscribe(FCommonEventHandle& OutHandle, const TSubclassOf<UCommonEventArgsBase>& InEventClass, FCommonEventUnifiedDelegate&& InEventCallback, const bool bMatchChild = false);
	void AddHandle(const FCommonEventHandle& InHandle);

	FCommonSubscribeData* GetSubscribeData(const FCommonEventHandle& InHandle);
};
