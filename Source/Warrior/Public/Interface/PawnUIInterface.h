// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PawnUIInterface.generated.h"

class UPawnUIComponent;
class UHeroUIComponent;
class UEnemyUIComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPawnUIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WARRIOR_API IPawnUIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Pawn が持つ基本的な UI コンポーネントを返す
	virtual UPawnUIComponent* GetPawnUIComponent() const = 0;

	// Hero専用の UI コンポーネントを返す
	virtual UHeroUIComponent* GetHeroUIComponent() const;

	// 敵専用の UI コンポーネントを返す
	virtual UEnemyUIComponent* GetEnemyUIComponent() const;
};
