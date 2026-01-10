// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/EnemyCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorGameplayTags.h"
#include "WarriorFunctionLibrary.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Components/BoxComponent.h"

#include "WarriorDebugHelper.h"

// 武器がターゲットにヒットしたときに呼ばれる処理
void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}

	OverlappedActors.AddUnique(HitActor);

	// TODO::Implement block check
	bool bIsValidBlock = false;

	const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitActor, WarriorGameplayTags::Player_Status_Blocking);
	const bool bIsMyAttackUnblockable = UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetOwningPawn(), WarriorGameplayTags::Enemy_Status_Unbloackable);

	if (bIsPlayerBlocking && !bIsMyAttackUnblockable)
	{
		bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}
	
	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;

	if (bIsValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			WarriorGameplayTags::Player_Event_SuccessfulBlock,
			EventData
		);
	}
	else
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetOwningPawn(),
			WarriorGameplayTags::Shared_Event_MeleeHit,
			EventData
		);
	}
}

// 左手・右手のボディコリジョン（素手攻撃用）を有効化／無効化するための関数
void UEnemyCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamegeType ToggleDamageType)
{
	// このコンポーネントを持っている敵キャラクターを取得
	AWarriorEnemyCharacter* OwningEnemyCharacter = GetOwningPawn<AWarriorEnemyCharacter>();

	check(OwningEnemyCharacter);

	// 敵キャラが持つ左手・右手のコリジョンボックスを取得
	UBoxComponent* LeftHandCollisionBox = OwningEnemyCharacter -> GetLeftHandCollisionBox();
	UBoxComponent* RightHandCollisionBox = OwningEnemyCharacter -> GetRightHandCollisionBox();

	// どちらも必ず存在する前提なので check で確認
	check(LeftHandCollisionBox && RightHandCollisionBox);

	// どちらの手の攻撃判定を切り替えるかを ToggleDamageType で判定
	switch (ToggleDamageType)
	{
	case EToggleDamegeType::LeftHand:
		// 左手攻撃：コリジョンを有効化（QueryOnly）または無効化
		LeftHandCollisionBox -> SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		break;

	case EToggleDamegeType::RightHand:
		// 右手攻撃：コリジョンを有効化（QueryOnly）または無効化
		RightHandCollisionBox -> SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		break;

	default:
		break;
	}

	// 攻撃終了時（無効化時）は、多段ヒット防止のために
	// すでに当たったアクターの記録をリセットする
	if (!bShouldEnable)
	{
		OverlappedActors.Empty();
	}
}
