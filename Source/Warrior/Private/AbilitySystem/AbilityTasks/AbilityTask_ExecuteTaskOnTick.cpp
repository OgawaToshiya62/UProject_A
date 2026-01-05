// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_ExecuteTaskOnTick.h"

// コンストラクタ：この AbilityTask を Tick 可能にする
UAbilityTask_ExecuteTaskOnTick::UAbilityTask_ExecuteTaskOnTick()
{
    bTickingTask = true;
}

// Ability 内でこのタスクを生成して開始する
UAbilityTask_ExecuteTaskOnTick* UAbilityTask_ExecuteTaskOnTick::ExecuteTaskOnTick(UGameplayAbility* OwningAbility)
{
    UAbilityTask_ExecuteTaskOnTick* Node = NewAbilityTask<UAbilityTask_ExecuteTaskOnTick>(OwningAbility);

    return Node;
}

// 毎フレーム呼ばれる処理 ロックオンUIの追従など、毎フレーム更新が必要な処理を Blueprint に通知する
void UAbilityTask_ExecuteTaskOnTick::TickTask(float DeltaTime)
{
    Super::TickTask(DeltaTime);

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        OnAbilityTaskTick.Broadcast(DeltaTime);
    }
    else
    {
        EndTask();
    }
}
