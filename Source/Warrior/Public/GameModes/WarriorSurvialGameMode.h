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

// 敵のスポーン情報を定義する構造体
USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerInfo
{
	GENERATED_BODY()

	// スポーンする敵のクラス
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn;

	// 1回のスポーンで出現する最小の敵数
	UPROPERTY(EditAnywhere)
	int32 MinPerSpawnCount = 1;

	// 1回のスポーンで出現する最大の敵数
	UPROPERTY(EditAnywhere)
	int32 MaxPerSpawnCount = 3;
};

// 1ウェーブ分のスポーン設定をまとめた構造体
USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerTableRow : public FTableRowBase
{
	GENERATED_BODY()

	// このウェーブで使用するスポーン定義の配列
	UPROPERTY(EditAnywhere)
	TArray<FWarriorEnemyWaveSpawnerInfo> EnemyWaveSpawnerDefinitions;

	// このウェーブでスポーンする敵の総数
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
	// 全ウェーブが完了したかどうかを判定する関数
	bool HasFinishedAllWaves() const;

	// 現在のサバイバルゲームモードの状態
	UPROPERTY()
	EWarriorSurvialGameModeState CurrentSurvialGameModeState;

	// 状態変更を Blueprint に通知するデリゲート
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnSurvialGameModeStateChangedDelegate OnSurvialGameModeStateChanged;

	// 敵ウェーブの定義を格納したDataTable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	UDataTable* EnemyWaveSpawnerDataTable;

	// スポーンすべきウェーブの総数
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	int32 TotalWavesToSpawn;

	// 現在進行中のウェーブ番号
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	int32 CurrentWaveCount = 1;

	// ゲーム開始からの経過時間
	UPROPERTY()
	float TimePassedSinceStart = 0.f;

	// 新しいウェーブを開始するまでの待機時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	float SpawnNewWaveWaitTime = 5.f;

	// 敵をスポーンする際の遅延時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	float SpawnEnemiesDelayTime = 2.f;

	// ウェーブ完了後の待機時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	float WaveCompletedWaitTime = 5.f;
};
