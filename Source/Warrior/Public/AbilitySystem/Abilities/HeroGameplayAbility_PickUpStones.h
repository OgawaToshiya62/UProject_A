// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_PickUpStones.generated.h"

class AWarriorStoneBase;

/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroGameplayAbility_PickUpStones : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()
	
protected:
	// ~ Begin UGameplayAbility Interface
	// アビリティが発動した瞬間に呼ばれる
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	// アビリティが終了したときに呼ばれる
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// ~ End UGameplayAbility Interface

	// プレイヤーの足元にある石を BoxTrace で検知し、CollectedStones に追加
	UFUNCTION(BlueprintCallable)
	void CollectStones();

	// CollectStones() で収集した石を実際に消費し、各石が持つ GameplayEffect をプレイヤーに適用
	UFUNCTION(BlueprintCallable)
	void ConsumeStones();

private:
	// トレースを行う距離（キャラの足元方向へ）
	UPROPERTY(EditDefaultsOnly)
	float BoxTraceDistance = 50.f;

	// BoxTrace のサイズ（半径ではなくフルサイズ）
	UPROPERTY(EditDefaultsOnly)
	FVector TraceBoxSize = FVector(100.f);

	// 石を検知するための ObjectType チャンネル
	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<	EObjectTypeQuery> > StoneTraceChannel;

	// デバッグ用にトレース形状を描画するかどうか
	UPROPERTY(EditDefaultsOnly)
	bool bDrawDebugShape = false;

	// 検知された石のリスト（重複なし）
	UPROPERTY()
	TArray<AWarriorStoneBase*> CollectedStones;
};
