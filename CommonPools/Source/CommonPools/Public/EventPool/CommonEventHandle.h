// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CommonEventHandle.generated.h"

class UCommonEventArgsBase;

/**
 * 事件数据
 */
USTRUCT()
struct FCommonEventData
{
	GENERATED_BODY()

	// 发送者
	UPROPERTY(Transient)
	TObjectPtr<UObject> Sender { nullptr };

	// 事件参数
	UPROPERTY(Transient)
	TObjectPtr<UCommonEventArgsBase> EventArgs { nullptr };
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FCommonEventDynamicDelegate, UObject*, Sender, UCommonEventArgsBase*, EventArgs);

using FCommonEventFunction = TFunction<void(UObject*, UCommonEventArgsBase*)>;
using FCommonEventDelegateVariant = TVariant<TYPE_OF_NULLPTR, FCommonEventDynamicDelegate, FCommonEventFunction>;

/**
 * 事件触发委托
 */
struct FCommonEventUnifiedDelegate
{
	FCommonEventDelegateVariant VariantDelegate {};

	//@formatter:off
	FCommonEventUnifiedDelegate() = default;
	explicit FCommonEventUnifiedDelegate(FCommonEventDynamicDelegate const& Delegate) : VariantDelegate(TInPlaceType<FCommonEventDynamicDelegate>(), Delegate) { }
	explicit FCommonEventUnifiedDelegate(FCommonEventFunction&& Callback) : VariantDelegate(TInPlaceType<FCommonEventFunction>(), MoveTemp(Callback)) { }
	//@formatter:on

	void Execute(UObject* Sender, UCommonEventArgsBase* EventArgs);
	bool IsBound() const;
	const void* GetBoundObject() const;
	void Unbind() { VariantDelegate.Set<TYPE_OF_NULLPTR>(nullptr); }

	FCommonEventUnifiedDelegate(FCommonEventUnifiedDelegate&&) = default;
	FCommonEventUnifiedDelegate(const FCommonEventUnifiedDelegate&) = delete;
	FCommonEventUnifiedDelegate& operator=(FCommonEventUnifiedDelegate&&) = default;
	FCommonEventUnifiedDelegate& operator=(const FCommonEventUnifiedDelegate&) = delete;
};

/**
 * 事件句柄
 */
USTRUCT(BlueprintType)
struct FCommonEventHandle
{
	GENERATED_BODY()

public:
	FCommonEventHandle() = default;

	explicit FCommonEventHandle(const int32 InIndex) : Index(InIndex) {};

	int32 GetIndex() const { return Index; }

	bool IsValid() const { return Index >= 0; }

	void Invalidate() { Index = -1; }

	auto operator<=>(const FCommonEventHandle&) const = default;

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	int32 Index { -1 };
};

uint32 GetTypeHash(const FCommonEventHandle& Handle);
