// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorBaseAnimInstance.h"
#include "WarriorHeroLinkedAnimLayer.generated.h"

class UWarriorHeroAnimInstance;

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorHeroLinkedAnimLayer : public UWarriorBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	// キャラクターに紐づいているアニメーションインスタンスを取得する関数
	UFUNCTION(BlueprintPure, meta = (NotBlueprintThreadSafe))
	UWarriorHeroAnimInstance* GetHeroAnimInstance() const;
};
