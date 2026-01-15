// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "WarriorGameplayTags.h"

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

		if (InInputTag.MatchesTag(WarriorGameplayTags::InputTag_Toggleable) && (AbilitySpec.IsActive()))
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		else
		{
			TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

// 指定された入力タグに対応するアビリティの解放イベントを処理する
void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid() || !InInputTag.MatchesTag(WarriorGameplayTags::InputTag_MustBeHeld))
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

// 指定された武器アビリティセットをヒーローに付与する処理
void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, const TArray<FWarriorHeroSpecialAbilitySet>& InSpecialWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
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

	for (const FWarriorHeroSpecialAbilitySet& AbilitySet : InSpecialWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}

// 指定されたアビリティハンドルを AbilitySystem から削除する処理
void UWarriorAbilitySystemComponent::RemovedGrantHeroWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	// 削除対象が空なら何もせず終了
	if (InSpecHandlesToRemove.IsEmpty())
	{
		return;
	}

	// 各アビリティハンドルを走査
	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		// 有効なハンドルのみ削除
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);     // AbilitySystem からアビリティを削除
		}
	}

	// 削除対象リストを空にする（管理上の後処理）
	InSpecHandlesToRemove.Empty();
}

// 指定したGameplayTagを持つアビリティを探し、起動できるなら起動する
bool UWarriorAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	// 渡されたタグが有効かチェック
	check(AbilityTagToActivate.IsValid());

	// このタグを持つアビリティを格納する配列
	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundAbilitySpecs);

	// 一致するアビリティが見つかった場合
	if (!FoundAbilitySpecs.IsEmpty())
	{
		// 複数ある場合はランダムで1つ選ぶ
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);

		// まだアビリティが発動中でないなら起動する
		if (!SpecToActivate -> IsActive())
		{
			return TryActivateAbility(SpecToActivate -> Handle);
		}
	}

	// 見つからない or 起動できない場合
	return false;
}
