// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModes/WarriorBaseGameMode.h"
#include "WarriorSurvialGameMode.generated.h"

class AWarriorEnemyCharacter;

// サバイバルゲームモードの進行状態を表す列挙体
UENUM(BlueprintType)
enum class EWarriorSurvialGameModeState : uint8
{
	WaitSpawnNewWave,
	SpawningNewWave,
	InProgress,
	WaveCompleted,
	AllWavesDone,
	PlayerDied
};

// 1つのスポーン定義（どの敵を何体スポーンさせるか）を表す構造体
USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerInfo
{
	GENERATED_BODY()

	// スポーンする敵キャラクターのクラス
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn;

	// 1回のスポーンで最低何体出すか
	UPROPERTY(EditAnywhere)
	int32 MinPerSpawnCount = 1;

	// 1回のスポーンで最大何体出すか
	UPROPERTY(EditAnywhere)
	int32 MaxPerSpawnCount = 3;
};

// データテーブルの1行を表す構造体
USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerTableRow : public FTableRowBase
{
	GENERATED_BODY()

	// この Wave で使用するスポーン定義のリスト
	UPROPERTY(EditAnywhere)
	TArray<FWarriorEnemyWaveSpawnerInfo> EnemyWaveSpawnerDefinitions;

	// この Wave 全体でスポーンする敵の総数
	UPROPERTY(EditAnywhere)
	int32 TotalEnemyToSpawnThisWave = 1;
};

// ゲームモードの状態が変化したときに通知するデリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurvialGameModeStateChangedDelegate, EWarriorSurvialGameModeState, CurrentState);

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorSurvialGameMode : public AWarriorBaseGameMode
{
	GENERATED_BODY()
	
protected:
	// ゲーム開始時に呼ばれる。初期状態の設定などを行う
	virtual void BeginPlay() override;
	// 毎フレーム呼ばれる。Wave の進行管理などを行う予定
	virtual void Tick(float DeltaTime) override;

private:
	// 現在のゲームモード状態を変更し、Blueprint に通知する
	void SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState InState);
	// 全ての Wave を終えたかどうかを判定する
	bool HasFinishedAllWaves() const;
	// 次の Wave で使用する敵クラスを非同期ロードしてキャッシュしておく
	void PreLoadNextWaveEnemies();
	// 現在の Wave に対応するデータテーブル行を取得する
	FWarriorEnemyWaveSpawnerTableRow* GetCurrentWaveSpawnerTableRow() const;

	int32 TrySpawnWaveEnemies();
	bool ShouldKeepSpawnEnemies() const;

	// 現在のサバイバルゲームモードの状態
	UPROPERTY()
	EWarriorSurvialGameModeState CurrentSurvialGameModeState;

	// 状態変更を Blueprint に通知するデリゲート
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnSurvialGameModeStateChangedDelegate OnSurvialGameModeStateChanged;

	// Wave の定義を格納したデータテーブル
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	UDataTable* EnemyWaveSpawnerDataTable;

	// データテーブルに定義された Wave の総数
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	int32 TotalWavesToSpawn;

	// 現在の Wave数
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	int32 CurrentWaveCount = 1;

	UPROPERTY()
	int32 CurrentSpawnedEnemiesCounter = 0;

	UPROPERTY()
	int32 TotalSpawnedEnemiesThisWaveCounter = 0;

	UPROPERTY()
	TArray<AActor*> TargetPointsArray;

	// 状態遷移のために経過時間を蓄積するタイマー
	UPROPERTY()
	float TimePassedSinceStart = 0.f;

	// 次の Wave をスポーンするまでの待機時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	float SpawnNewWaveWaitTime = 5.f;

	// Wave スポーン開始後、敵を出現させるまでの遅延時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	float SpawnEnemiesDelayTime = 2.f;

	// Wave 完了後、次の Wave に移行するまでの待機時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	float WaveCompletedWaitTime = 5.f;

	// SoftClass（遅延ロード用）→ 実際の UClass（ロード済み）のマッピングを保持するキャッシュ
	UPROPERTY()
	TMap< TSoftClassPtr < AWarriorEnemyCharacter >, UClass* > PreLoadedEnemyClassMap;
};
