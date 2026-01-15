// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "GameplayTagContainer.h"
#include "HeroUIComponent.generated.h"

// 装備武器が変更されたときに、武器アイコン（SoftObject）を通知するデリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate, TSoftObjectPtr<UTexture2D>, SoftWeaponIcon);

// アビリティアイコンのスロット更新を通知するデリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityIconSlotUpdateDelegate, FGameplayTag, AbilityInputTag, TSoftObjectPtr<UMaterialInterface>, SoftAbilityIconMaterial);

/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()
	
public:
	// 現在のRage割合が変化したときに通知するイベント
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnCurrentRageChanged;

	// Blueprint からバインド可能な武器変更イベント
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnEquippedWeaponChangedDelegate OnEquippedWeaponChanged;

	// Blueprint からバインド可能なアビリティアイコン更新イベント
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAbilityIconSlotUpdateDelegate OnAbilityIconSlotUpdate;
};
