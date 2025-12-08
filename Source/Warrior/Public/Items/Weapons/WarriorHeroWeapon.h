// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "WarriorHeroWeapon.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorHeroWeapon : public AWarriorWeaponBase
{
	GENERATED_BODY()
	
public:
	// ヒーローが持つ武器に関連するデータを保持する変数武器に応じてリンクするアニメーションレイヤーなどを管理する
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	FWarriorHeroWeaponData HeroWeaponData;
};
