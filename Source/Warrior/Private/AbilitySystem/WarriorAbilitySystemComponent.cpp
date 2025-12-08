// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAbilitySystemComponent.h"

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
