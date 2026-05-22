// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CommonReferencePool.generated.h"

/**
 * 引用池信息
 */
USTRUCT(BlueprintType)
struct FCommonReferencePoolInfo
{
	GENERATED_BODY()

	// 类型名称
	UPROPERTY(BlueprintReadOnly)
	FName ClassName { NAME_None };

	// 未使用引用对象数量
	UPROPERTY(BlueprintReadOnly)
	int32 UnusedReferenceCount { 0 };

	// 使用中引用对象数量
	UPROPERTY(BlueprintReadOnly)
	int32 UsingReferenceCount { 0 };

	// 总获取引用对象数量
	UPROPERTY(BlueprintReadOnly)
	int32 AcquireReferenceCount { 0 };

	// 总释放引用对象数量
	UPROPERTY(BlueprintReadOnly)
	int32 ReleaseReferenceCount { 0 };

	// 添加引用对象数量
	UPROPERTY(BlueprintReadOnly)
	int32 AddReferenceCount { 0 };

	// 移除引用对象数量
	UPROPERTY(BlueprintReadOnly)
	int32 RemoveReferenceCount { 0 };
};

/**
 * 引用池
 */
USTRUCT()
struct COMMONPOOLS_API FCommonReferencePool
{
	GENERATED_BODY()

public:
	FCommonReferencePool() = default;

	explicit FCommonReferencePool(const UClass* InReferenceClass)
		: ReferenceClass(InReferenceClass)
	{
	}

	// 获取引用池信息
	FCommonReferencePoolInfo GetPoolInfo() const;

	// 获取未使用引用对象数量
	FORCEINLINE int32 GetUnusedReferenceCount() const { return ReferenceObjects.Num(); }

	// 获取使用中引用对象数量
	FORCEINLINE int32 GetUsingReferenceCount() const { return UsingReferenceCount; }

	// 获取总获取引用对象数量
	FORCEINLINE int32 GetAcquireReferenceCount() const { return AcquireReferenceCount; }

	// 获取总释放引用对象数量
	FORCEINLINE int32 GetReleaseReferenceCount() const { return ReleaseReferenceCount; }

	// 获取总添加引用对象数量
	FORCEINLINE int32 GetAddReferenceCount() const { return AddReferenceCount; }

	// 获取总移除引用对象数量
	FORCEINLINE int32 GetRemoveReferenceCount() const { return RemoveReferenceCount; }

	// 获取引用对象
	template <class T>
	T* Acquire(UObject* Outer = nullptr)
	{
		checkf(T::StaticClass() == ReferenceClass, TEXT("Invalid type! Expected %s, but got %s."), *GetNameSafe(ReferenceClass), *T::StaticClass()->GetName());
		return Cast<T>(Acquire(Outer));
	}

	// 获取引用对象
	UObject* Acquire(UObject* Outer = nullptr);

	// 释放引用对象
	void Release(UObject* InObject);

	// 添加引用对象
	void Add(int32 Count = 1, UObject* Outer = nullptr);

	// 移除引用对象
	void Remove(int32 Count = 1);

	// 移除所有引用对象，清空集合
	void Clear();

private:
	int32 UsingReferenceCount { 0 };
	int32 AcquireReferenceCount { 0 };
	int32 ReleaseReferenceCount { 0 };
	int32 AddReferenceCount { 0 };
	int32 RemoveReferenceCount { 0 };

	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> ReferenceObjects {};

	UPROPERTY(Transient)
	const UClass* ReferenceClass { nullptr };
};
