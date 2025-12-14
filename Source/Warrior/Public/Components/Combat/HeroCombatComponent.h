// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "HeroCombatComponent.generated.h"

class AWarriorHeroWeapon;

/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()
	
public:
	// 指定された武器タグに対応するヒーローの所持武器を取得する関数
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorHeroWeapon* GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const;

	// 現在装備している武器を返す関数
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorHeroWeapon* GetHeroCurrentEquippedWeapon() const;

	// 現在装備している武器の指定レベルでのダメージ値を返す関数
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	float GetHeroCurrentEquippWeaponDamageAtLevel(float InLevel) const;

	// 武器がアクターにヒットしたときの処理を定義する仮想関数
	virtual void OnHitTargetActor(AActor* HitActor) override;
	// 武器がアクターから離れたときの処理を定義する仮想関数
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor) override;
};
