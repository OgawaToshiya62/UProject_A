// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameplayTagContainer.h"
#include "WarriorGameInstance.generated.h"

// レベル情報をまとめた構造体
USTRUCT(BlueprintType)
struct FWarriorGameLevelSet
{
	GENERATED_BODY()

	// レベルを識別する GameplayTag
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "GameData.Level"))
	FGameplayTag LevelTag;

	// 対応するレベルアセット
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> Level;

	// タグとレベルの両方が有効かどうかを判定
	bool IsValid() const
	{
		return LevelTag.IsValid() && !Level.IsNull();
	}
};

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
protected:
	// ゲーム内で使用可能なレベルセット一覧
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FWarriorGameLevelSet> GameLevelSets;

public:
	// タグを指定して対応するレベルを取得する
	UFUNCTION(BlueprintPure, meta = (GameplayTagFilter = "GameData.Level"))
	TSoftObjectPtr<UWorld> GetGameLevelByTag(FGameplayTag InTag) const;
};
