// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "EnemyUIComponent.generated.h"

class UWarriorWidgetBase;

/**
 * 
 */
UCLASS()
class WARRIOR_API UEnemyUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()
	
public:
	// 敵に関連するウィジェット（ヘルスバーなど）を登録する
	UFUNCTION(BlueprintCallable)
	void RegisterEnemyDrawnWidget(UWarriorWidgetBase* InWidgetToRegister);

	// 登録されているウィジェットがあれば、すべて画面から取り除く (敵が死亡したときなどに呼ばれる想定)
	UFUNCTION(BlueprintCallable)
	void RemoveEnemyDrawnWidgetsIfAny();

private:
	// 敵に紐づいて現在表示されているウィジェットのリスト
	TArray< UWarriorWidgetBase* > EnemyDrawnWidgets;
};
