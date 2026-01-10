// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UI/EnemyUIComponent.h"
#include "Widgets/WarriorWidgetBase.h"

// 敵に関連するウィジェット（ヘルスバーなど）を登録する
void UEnemyUIComponent::RegisterEnemyDrawnWidget(UWarriorWidgetBase* InWidgetToRegister)
{
	// 渡されたウィジェットを配列に追加して記録する
	EnemyDrawnWidgets.Add(InWidgetToRegister);
}

// 登録されているウィジェットがあれば、すべて画面から取り除く
void UEnemyUIComponent::RemoveEnemyDrawnWidgetsIfAny()
{
	// 配列が空なら、削除すべきウィジェットが無いので何もしない
	if (EnemyDrawnWidgets.IsEmpty())
	{
		return;
	}

	// 配列に登録されているすべてのウィジェットを処理する
	for (UWarriorWidgetBase* DrawnWidget : EnemyDrawnWidgets)
	{
		// ウィジェットが有効なら、画面から取り除く
		if (DrawnWidget)
		{
			DrawnWidget -> RemoveFromParent();
		}
	}

	EnemyDrawnWidgets.Empty();
}
