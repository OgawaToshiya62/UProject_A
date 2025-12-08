// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WarriorStructTypes.generated.h"

class UWarriorHeroLinkedAnimLayer;

// ヒーローの武器に関連するデータを保持する構造体
USTRUCT(BlueprintType)
struct FWarriorHeroWeaponData
{
	GENERATED_BODY()

	// 武器に応じてリンクするアニメーションレイヤーのクラス
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UWarriorHeroLinkedAnimLayer> WeaponAnimLayerToLink;
};
