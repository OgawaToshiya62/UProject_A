// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HeroGameplayAbility_PickUpStones.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Items/PickUps/WarriorStoneBase.h"
#include "Components/UI/HeroUIComponent.h"

// アビリティ発動時の処理
void UHeroGameplayAbility_PickUpStones::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	GetHeroUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(true);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

// アビリティ終了時の処理
void UHeroGameplayAbility_PickUpStones::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetHeroUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(false);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// 足元方向に BoxTrace を行い、拾える石を収集する
void UHeroGameplayAbility_PickUpStones::CollectStones()
{
	// 前回の結果をクリア
	CollectedStones.Empty();

	TArray<FHitResult> TraceHits;

	// 足元方向へ BoxTrace を実行
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		GetHeroCharacterFromActorInfo()->GetActorLocation() + -GetHeroCharacterFromActorInfo()->GetActorUpVector() * BoxTraceDistance,
		TraceBoxSize / 2.f,
		(-GetHeroCharacterFromActorInfo()->GetActorUpVector()).ToOrientationRotator(),
		StoneTraceChannel,
		false,
		TArray<AActor*>(),
		bDrawDebugShape ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		TraceHits,
		true
	);

	// ヒットしたアクターの中から石だけを抽出
	for (const FHitResult& TraceHit : TraceHits)
	{
		if (AWarriorStoneBase* FoundStone = Cast<AWarriorStoneBase>(TraceHit.GetActor()))
		{
			CollectedStones.AddUnique(FoundStone);
		}
	}

	// 一つも見つからなければアビリティをキャンセル
	if (CollectedStones.IsEmpty())
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	}
}

// 収集済みの石を順番に消費し、対応する GameplayEffect を適用する
void UHeroGameplayAbility_PickUpStones::ConsumeStones()
{
	// 石が一つも無ければアビリティを終了
	if (CollectedStones.IsEmpty())
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}

	// 収集した石を順番に処理
	for (AWarriorStoneBase* CollectedStone : CollectedStones)
	{
		if (CollectedStone)
		{
			// 石が持つ GameplayEffect をプレイヤーに適用
			CollectedStone->Consume(GetWarriorAbilitySystemComponentFromActorInfo(), GetAbilityLevel());
		}
	}
}
