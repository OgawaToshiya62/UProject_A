// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarriorWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// ウィジェット初期化時に一度だけ呼ばれる処理をオーバーライド
	virtual void NativeOnInitialized() override;

	// Hero の UI コンポーネントが初期化されたときに Blueprint 側で処理を行うイベント
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Owning Hero UI Component Initialized"))
	void BP_OnOwningHeroUIComponentInitialized(UHeroUIComponent* OwningHeroUIComponent);

	// Enemy の UI コンポーネントが初期化されたときに Blueprint 側で処理を行うイベント
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Owning Enemy UI Component Initialized"))
	void BP_OnOwningEnemyUIComponentInitialized(UEnemyUIComponent* OwningEnemyUIComponent);

public:
	// 敵キャラクター用ウィジェットを初期化し、対象の敵アクターを紐づける
	UFUNCTION(BlueprintCallable)
	void InitEnemyCreatedWidget(AActor* OwningEnemyActor);
};
