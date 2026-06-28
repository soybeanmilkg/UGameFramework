// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonObjectWrapper.h"
#include "Templates/SubclassOf.h"
#include "UObject/Object.h"
#include "CommonObjectPool.generated.h"

class UCommonObjectBase;

/**
 * 对象池创建参数
 */
USTRUCT(BlueprintType)
struct FCommonObjectPoolCreateParams
{
    GENERATED_BODY()

    // 单个对象是否允许多次Spawn
    // 这对于资源对象这类可以复用的对象很有用
    bool bAllowMultiSpawn { false };

    // 自动释放间隔
    float AutoReleaseInterval { TNumericLimits<float>::Max() };

    // 对象池容量
    int32 Capacity { TNumericLimits<int>::Max() };

    // 对象过期时间
    float ExpireTime { TNumericLimits<float>::Max() };

    // 对象池优先级，影响在子系统中 Tick 的顺序
    int32 Priority { 0 };
};

/**
 * 
 */
UCLASS(BlueprintType)
class COMMONPOOLS_API UCommonObjectPool : public UObject
{
    GENERATED_BODY()

public:
    void Init(const TSubclassOf<UCommonObjectBase>& InObjectClass, const FCommonObjectPoolCreateParams& Params);
    void Shutdown();
    void Tick(const float DeltaTime);

    // 获取对象类
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    TSubclassOf<UCommonObjectBase> GetObjectClass() const { return ObjectClass; }

    // 获取对象池名称
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    FName GetObjectPoolName() const { return Name; }

    // 获取对象池总对象数量
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    int32 GetCount() const { return ObjectMap.Num(); }

    // 获取对象池中Spawn出来的对象数量
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    int32 GetSpawnCount() const;

    // 获取对象池中可释放的对象数量
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    int32 GetCanReleaseCount();

    // 获取对象池是否允许一个对象多次Spawn
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    bool IsAllowMultiSpawn() const { return bAllowMultiSpawn; }

    // 获取对象池自动释放间隔
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    float GetAutoReleaseInterval() const { return AutoReleaseInterval; }

    // 设置对象池自动释放间隔
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    void SetAutoReleaseInterval(const float InAutoReleaseInterval);

    // 获取对象池容量
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    int32 GetCapacity() const { return Capacity; }

    // 设置对象池容量
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    void SetCapacity(const int InCapacity);

    // 获取对象池对象过期时间
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    float GetExpireTime() const { return ExpireTime; }

    // 设置对象池对象过期时间
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    void SetExpireTime(const float InExpireTime);

    // 获取对象池优先级
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    float GetPriority() const { return Priority; }

    // 设置对象池优先级
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    void SetPriority(const float InPriority);

    // 注册对象到对象池中
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    void Register(UCommonObjectBase* Object, const bool bSpawned = false);

    // 判断对象池中是否有空余对象可Spawn
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    bool CanSpawn(const FName& InName = NAME_None);

    // Spawn对象
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    UCommonObjectBase* Spawn(const FName& InName);

    // Despawn对象
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    void Despawn(const UCommonObjectBase* Object);

    // Despawn对象
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    void DespawnByUObject(const UObject* Object);

    // 设置对象锁定状态
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    void SetLocked(const UCommonObjectBase* Object, const bool IsLocked);

    // 设置对象锁定状态
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    void SetLockedByUObject(UObject* Object, const bool IsLocked);

    // 设置对象优先级
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    void SetObjectPriority(const UCommonObjectBase* Object, const int InPriority);

    // 设置对象优先级
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    void SetObjectPriorityByUObject(UObject* Object, const int InPriority);

    // 释放对象
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    bool ReleaseObject(const UCommonObjectBase* Object);

    // 释放对象
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    bool ReleaseObjectByUObject(UObject* Object);

    // 释放冗余对象
    void Release() { Release(ObjectMap.Num() - Capacity, DefaultReleaseObjectFilter); }

    // 释放冗余对象
    void Release(const int32 ToReleaseCount) { Release(ToReleaseCount, DefaultReleaseObjectFilter); }

    DECLARE_DELEGATE_FourParams(FReleaseObjectFilterDelegate, TArray<TObjectPtr<UCommonObjectBase>>&, int32, const FDateTime&, TArray<TObjectPtr<UCommonObjectBase>>&);

    // 释放冗余对象
    void Release(const FReleaseObjectFilterDelegate& Filter) { Release(ObjectMap.Num() - Capacity, Filter); }

    // 释放冗余对象
    void Release(int32 ToReleaseCount, const FReleaseObjectFilterDelegate& Filter);

    // 释放所有未使用的对象
    UFUNCTION(BlueprintCallable, Category="CommonObjectPool")
    void ReleaseAllUnused();

    // 获取正在使用的对象
    void GetUsingObject(const FName& ObjectName, TArray<UCommonObjectBase*>& Result);

    // 获取所有正在使用的对象
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    void GetAllUsingObjects(TArray<UCommonObjectBase*>& Result) const;

    // 获取已注册的对象
    void GetRegisteredObjects(const FName& ObjectName, TArray<UCommonObjectBase*>& Result);

    // 获取所有已注册的对象
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    void GetAllRegisteredObjects(TArray<UCommonObjectBase*>& Result) const;

    // 获取对象池对象调试信息
    UFUNCTION(BlueprintPure, Category="CommonObjectPool")
    TArray<FCommonObjectDebugInfo> GetObjectDebugInfos() const;

private:
    // 基本属性
    UPROPERTY(Transient)
    TSubclassOf<UCommonObjectBase> ObjectClass { nullptr };
    FName Name { NAME_None };

    bool bAllowMultiSpawn { false };
    float AutoReleaseInterval { 0.f };
    int32 Capacity { 0 };
    float ExpireTime { 0.f };
    int32 Priority { 0 };

    // 对象池对象
    TMap<FName, TArray<TObjectPtr<UCommonObjectWrapper>>> Objects {};
    UPROPERTY(Transient)
    TMap<TObjectPtr<UObject>, TObjectPtr<UCommonObjectWrapper>> ObjectMap {};

    // 自动释放时间
    float AutoReleaseTime { 0.f };
    // 缓存可释放对象
    UPROPERTY(Transient)
    TArray<TObjectPtr<UCommonObjectBase>> CachedCanReleaseObjects {};
    // 缓存可释放对象数量
    void GetCanReleaseObjects(TArray<TObjectPtr<UCommonObjectBase>>& Result);
    // 清除无效对象
    void ClearInvalidWrappers();

    // 默认释放对象过滤器
    FReleaseObjectFilterDelegate DefaultReleaseObjectFilter;
    static void OnDefaultReleaseObjectFilter(
        TArray<TObjectPtr<UCommonObjectBase>>& CandidateObjects,
        int ToReleaseCount,
        const FDateTime& ExpireTime,
        TArray<TObjectPtr<UCommonObjectBase>>& Result
    );

    UCommonObjectWrapper* GetCommonObjectWrapper(const UObject* Object);
};
