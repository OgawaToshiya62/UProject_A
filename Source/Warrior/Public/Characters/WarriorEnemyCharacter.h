// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "WarriorEnemyCharacter.generated.h"

class UEnemyCombatComponent;
class UEnemyUIComponent;

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorEnemyCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()
	
public:
	// コンストラクタ
	AWarriorEnemyCharacter();

	//~ Begin IPawnCombatInterface Interface.
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;     // この関数をオーバーライドすることで、Pawn が持つ戦闘コンポーネントを返せるようになる
	//~ End IPawnCombatInterface Interface

	//~ Begin IPawnUIInterface Interface.
	virtual UPawnUIComponent* GetPawnUIComponent() const override;     // IPawnUIInterface の実装: この Pawn が持つ UI コンポーネントを返す
	virtual UEnemyUIComponent* GetEnemyUIComponent() const override;
	//~ End IPawnUIInterface Interface

protected:
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;     // キャラクターが Controller に所有されたときに呼ばれる処理（能力システムの初期化などに使用）
	//~ End APawn Interface
	
	// 敵キャラクターの戦闘処理をするコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UEnemyCombatComponent* EnemyCombatComponent;

	// 敵キャラクターのUI処理を担当するコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UEnemyUIComponent* EnemyUIComponent;

private:
	// 敵キャラクターの初期データをセットアップする内部関数
	void InitEnemyStartUpData();

public:
	// 敵キャラクターの戦闘コンポーネントを取得する関数
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const { return EnemyCombatComponent; }
};
