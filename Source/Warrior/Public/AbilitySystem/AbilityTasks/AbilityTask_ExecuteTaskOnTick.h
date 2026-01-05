// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ExecuteTaskOnTick.generated.h"

// Ability 実行中に毎フレーム処理（Tick）を行うための AbilityTask
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityTaskTickDelegate, float, DeltaTime);

/**
 * 
 */
UCLASS()
class WARRIOR_API UAbilityTask_ExecuteTaskOnTick : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	// AbilityTask のコンストラクタ（Tick を有効化）
	UAbilityTask_ExecuteTaskOnTick();

	// Ability 内でこのタスクを開始するための静的関数
	UFUNCTION(BlueprintCallable, Category = "Warrior|AbilityTasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_ExecuteTaskOnTick* ExecuteTaskOnTick(UGameplayAbility* OwningAbility);

	//~ Begin UGameplayTask Interface
	// 毎フレーム呼ばれる Tick 処理
	virtual void TickTask(float DeltaTime) override;
	//~ End UGameplayTask Interface

	// Tick を Blueprint に通知するデリゲート
	UPROPERTY(BlueprintAssignable)
	FOnAbilityTaskTickDelegate OnAbilityTaskTick;
};
