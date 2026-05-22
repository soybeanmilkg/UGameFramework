// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonReferencePool.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CommonReferencePoolSubsystem.generated.h"

/**
 * 引用池子系统
 * 提供全局引用池访问和管理接口
 */
UCLASS()
class COMMONPOOLS_API UCommonReferencePoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	//~ Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem

public:
	static UCommonReferencePoolSubsystem* Get(const UObject* WorldContextObject);

	/**
	 * 从引用池中获取一个引用对象
	 * @param ReferenceClass 对象类型
	 * @param Outer 创建新对象的外部对象
	 * @return Object
	 */
	UFUNCTION(BlueprintCallable, Category="CommonReferencePool")
	UObject* Acquire(const UClass* ReferenceClass, UObject* Outer = nullptr);

	/**
	 * 从引用池中获取一个引用对象
	 * @tparam T 实现 IReference 的对象
	 * @param Class 对象类型
	 * @param Outer 创建新对象的外部对象
	 * @return Object
	 */
	template <class T>
	T* Acquire(const UClass* Class = nullptr, UObject* Outer = nullptr)
	{
		return Cast<T>(Acquire(Class != nullptr ? Class : T::StaticClass(), Outer));
	}

	/**
	 * 释放一个引用对象
	 * @param InObject 引用对象
	 */
	UFUNCTION(BlueprintCallable, Category="CommonReferencePool")
	void Release(UObject* InObject);

	/**
	 * 新增指定数量的引用对象
	 * @param ReferenceClass 
	 * @param Count 
	 * @param Outer 
	 */
	UFUNCTION(BlueprintCallable, Category="CommonReferencePool")
	void Add(const UClass* ReferenceClass, const int32 Count = 1, UObject* Outer = nullptr);

	/**
	 * 新增指定数量的引用对象
	 * @tparam T 
	 * @param Count 
	 * @param Outer 
	 */
	template <class T>
	void Add(const int Count = 1, UObject* Outer = nullptr)
	{
		Add(T::StaticClass(), Count, Outer);
	}

	/**
	 * 移除指定数量的引用对象
	 * @param ReferenceClass 
	 * @param Count 
	 */
	UFUNCTION(BlueprintCallable, Category="CommonReferencePool")
	void Remove(const UClass* ReferenceClass, const int32 Count = 1);

	/**
	 * 移除指定数量的引用对象
	 * @tparam T 
	 * @param Count 
	 */
	template <class T>
	void Remove(const int32 Count)
	{
		Remove(T::StaticClass(), Count);
	}

	/**
	 * 移除指定类型的所有引用对象
	 * @param ReferenceClass 
	 */
	UFUNCTION(BlueprintCallable, Category="CommonReferencePool")
	void Clear(const UClass* ReferenceClass);

	/**
	 * 移除指定类型的所有引用对象
	 * @tparam T 
	 */
	template <class T>
	void Clear()
	{
		Clear(T::StaticClass());
	}

	/**
	 * 清理所有引用对象
	 */
	UFUNCTION(BlueprintCallable, Category="CommonReferencePool")
	void ClearAll();

	/**
	 * 移除指定类型的引用池
	 * @param ReferenceClass 
	 */
	UFUNCTION(BlueprintCallable, Category="CommonReferencePool")
	void RemovePool(const UClass* ReferenceClass);

	/**
	 * 移除指定类型的引用池
	 * @tparam T 
	 */
	template <class T>
	void RemovePool()
	{
		RemovePool(T::StaticClass());
	}

	/**
	 * 移除所有的引用池
	 */
	UFUNCTION(BlueprintCallable, Category="CommonReferencePool")
	void RemoveAllPool();

	/**
	 * 获取所有引用池信息
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category="CommonReferencePool")
	TArray<FCommonReferencePoolInfo> GetAllReferencePoolInfo() const;

	/**
	 * 获取引用池信息
	 * @param ReferenceClass 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category="CommonReferencePool")
	FCommonReferencePoolInfo GetReferencePoolInfo(const UClass* ReferenceClass) const;

private:
	// 引用池
	UPROPERTY(Transient)
	TMap<const UClass*, FCommonReferencePool> ReferencePools {};

	/**
	 * 获取某个类型的引用池
	 * @param ReferenceClass 
	 * @return 
	 */
	FCommonReferencePool* GetOrCreateReferencePool(const UClass* ReferenceClass);
};

struct FCommonReferencePoolStatic
{
	template <class T>
	static T* Acquire(UObject* WorldContextObject, const UClass* Class = nullptr, UObject* Outer = nullptr)
	{
		if (const auto Subsystem = UCommonReferencePoolSubsystem::Get(WorldContextObject))
		{
			return Subsystem->Acquire<T>(Class, Outer);
		}

		return nullptr;
	}

	static void Release(const UObject* WorldContextObject, UObject* InObject)
	{
		if (const auto Subsystem = UCommonReferencePoolSubsystem::Get(WorldContextObject))
		{
			Subsystem->Release(InObject);
		}
	}
};
