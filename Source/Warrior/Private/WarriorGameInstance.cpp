// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorGameInstance.h"

// タグを指定して対応するレベルを取得する
TSoftObjectPtr<UWorld> UWarriorGameInstance::GetGameLevelByTag(FGameplayTag InTag) const
{
	// 登録されているレベルセットを走査
	for (const FWarriorGameLevelSet& GameLevelSet : GameLevelSets)
	{
		// タグとレベルが有効でない場合はスキップ
		if (!GameLevelSet.IsValid()) continue;

		// タグが一致したらそのレベルを返す
		if (GameLevelSet.LevelTag == InTag)
		{
			return GameLevelSet.Level;
		}
	}

	// 見つからなかった場合は空の SoftObjectPtr を返す
	return TSoftObjectPtr<UWorld>();
}
