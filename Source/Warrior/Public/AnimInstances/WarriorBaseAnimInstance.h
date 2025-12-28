// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "WarriorBaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	// アニメーションBPからキャラクターのGameplayTagを安全にチェックするための関数
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool DoseOwnerHaveTag(FGameplayTag TagToCheck) const;
};
