// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"

// 入力タグに対応するアビリティを検索して発動する
void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	// 無効な入力タグなら処理しない
	if (!InInputTag.IsValid())
	{
		return;
	}

	// アクティブ化可能なアビリティをスキャン
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// このアビリティが入力タグを持っていなければスキップ
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;

		TryActivateAbility(AbilitySpec.Handle);
	}
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
}

// 指定された武器アビリティセットをヒーローに付与する処理
void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	// アビリティセットが空なら何もせず終了
	if (InDefaultWeaponAbilities.IsEmpty())
	{
		return;
	}

	// 各アビリティセットを走査
	for (const FWarriorHeroAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		// 無効なアビリティセットはスキップ
		if (!AbilitySet.IsValid()) continue;

		// アビリティの仕様を作成
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();                    // 所有者（キャラクター）を設定
		AbilitySpec.Level = ApplyLevel;                                 // 指定されたレベルで付与
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);     // 指定されたレベルで付与

		// AbilitySystem にアビリティを付与し、そのハンドルを結果配列に追加
		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}
