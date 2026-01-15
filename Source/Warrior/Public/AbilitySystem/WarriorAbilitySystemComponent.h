// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "WarriorAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	// 指定された入力タグに対応するアビリティの押下イベントを処理する
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	// 指定された入力タグに対応するアビリティの解放イベントを処理する
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);

	// 指定された武器アビリティセットをヒーローに付与する関数
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability", meta = (ApplyLevel = "1"))
	void GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, const TArray<FWarriorHeroSpecialAbilitySet>& InSpecialWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);

	// 指定されたアビリティハンドルに対応する武器アビリティを削除する関数
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	void RemovedGrantHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);

	// 指定したGameplayTagを持つアビリティを探し、起動できるなら起動する
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
};
