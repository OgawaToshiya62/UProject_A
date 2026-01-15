// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "WarriorStructTypes.generated.h"

class UWarriorHeroLinkedAnimLayer;
class UWarriorHeroGameplayAbility;
class UInputMappingContext;

USTRUCT(BlueprintType)
// ヒーローが持つアビリティセット（入力タグと付与するアビリティの組み合わせ）
struct FWarriorHeroAbilitySet
{
	GENERATED_BODY()

	// このアビリティを呼び出すための入力タグ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	// 付与するアビリティのクラス
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UWarriorHeroGameplayAbility> AbilityToGrant;

	// 入力タグとアビリティが有効かどうかを判定する
	bool IsValid() const;
};

// ヒーローのスペシャルアビリティ用のデータセット
USTRUCT(BlueprintType)
struct FWarriorHeroSpecialAbilitySet : public FWarriorHeroAbilitySet
{
	GENERATED_BODY()

	// UI に表示するアビリティアイコンのマテリアル
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UMaterialInterface> SoftAbilityIconMaterial;

	// このスペシャルアビリティに対応するクールダウンタグ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Player.Cooldown"))
	FGameplayTag AbilityCooldownTag;
};

// ヒーローの武器に関連するデータを保持する構造体
USTRUCT(BlueprintType)
struct FWarriorHeroWeaponData
{
	GENERATED_BODY()

	// 武器に応じてリンクするアニメーションレイヤーのクラス
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UWarriorHeroLinkedAnimLayer> WeaponAnimLayerToLink;

	// 武器用の入力マッピングコンテキスト
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputMappingContext* WeaponInputMappingContext;

	// 武器に紐づくデフォルトアビリティの一覧
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FWarriorHeroAbilitySet> DefaultWeaponAbilities;

	// ヒーローが持つスペシャルアビリティの一覧
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FWarriorHeroSpecialAbilitySet> SpecialWeaponAbilities;

	// 武器の基本ダメージを表すプロパティ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat WeaponBaseDamage;

	// 武器アイコン用のテクスチャ（SoftObject参照 必要時にロードされる）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> SoftWeaponIconTexture;
};
