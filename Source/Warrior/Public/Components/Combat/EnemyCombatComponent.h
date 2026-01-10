// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "EnemyCombatComponent.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UEnemyCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnHitTargetActor(AActor* HitActor) override;

protected:
	// 左手・右手のボディコリジョン（素手攻撃用）を有効化／無効化するための関数
	virtual void ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamegeType ToggleDamageType) override;
};
