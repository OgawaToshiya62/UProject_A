// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/PawnUIInterface.h"

// Add default functionality here for any IPawnUIInterface functions that are not pure virtual.

// Hero専用UIコンポーネントを持たないためnullptrを返す
UHeroUIComponent* IPawnUIInterface::GetHeroUIComponent() const
{
	return nullptr;
}

// Enemy専用UIコンポーネントを持たないためnullptrを返す
UEnemyUIComponent* IPawnUIInterface::GetEnemyUIComponent() const
{
	return nullptr;
}
