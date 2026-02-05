// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorGameInstance.h"
#include "MoviePlayer.h"

// GameInstance 初期化時に呼ばれる。ローディング画面用のデリゲート登録などを行う
void UWarriorGameInstance::Init()
{
	Super::Init();

	// マップ読み込み前に呼ばれるデリゲートへ登録
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::OnPreLoadMap);
	// マップ読み込み後に呼ばれるデリゲートへ登録
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnDestinationWorldLoaded);
}

// マップ読み込み開始前に呼ばれる（ローディング画面を表示する）
void UWarriorGameInstance::OnPreLoadMap(const FString& MapName)
{
	// ローディング画面の設定を作成
	FLoadingScreenAttributes LoadingScreenAttributes;
	// 読み込み完了で自動終了
	LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = true;
	// 最低2秒は表示
	LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 2.f;
	// 表示するWidget
	LoadingScreenAttributes.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

	// ローディング画面をセットアップ
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}

// マップ読み込み完了後に呼ばれる（ローディング画面を閉じる）
void UWarriorGameInstance::OnDestinationWorldLoaded(UWorld* LoadedWorld)
{
	// マップ読み込み完了後にローディング画面を閉じる
	GetMoviePlayer()->StopMovie();
}

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
