// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/WarriorSurvialGameMode.h"

// ゲーム開始時に呼ばれる
void AWarriorSurvialGameMode::BeginPlay()
{
	Super::BeginPlay();

	// DataTable が設定されているかチェック
	checkf(EnemyWaveSpawnerDataTable, TEXT("Forgot to assign a valid datat table in survial game mode blueprint"));

	// 最初の状態を「新しいウェーブを待機中」に設定
	SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::WaitSpawnNewWave);

	// DataTable の行数から、スポーンすべきウェーブ総数を取得
	TotalWavesToSpawn = EnemyWaveSpawnerDataTable -> GetRowNames().Num();
}

// 毎フレーム呼ばれる
void AWarriorSurvialGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// --- 新しいウェーブを待機中 ---
	if (CurrentSurvialGameModeState == EWarriorSurvialGameModeState::WaitSpawnNewWave)
	{
		// 経過時間を加算
		TimePassedSinceStart += DeltaTime;

		// 待機時間を超えたらウェーブ開始へ
		if (TimePassedSinceStart >= SpawnNewWaveWaitTime)
		{
			TimePassedSinceStart = 0.f;

			SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::SpawningNewWave);
		}
	}

	// --- 新しいウェーブの敵をスポーン中 ---
	if (CurrentSurvialGameModeState == EWarriorSurvialGameModeState::SpawningNewWave)
	{
		TimePassedSinceStart += DeltaTime;

		// 敵スポーンの遅延時間を超えたらスポーン処理を実行
		if (TimePassedSinceStart >= SpawnEnemiesDelayTime)
		{
			// TODO : Handle spawn new enemies

			TimePassedSinceStart = 0.f;

			// スポーン完了後、ウェーブ進行中へ
			SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::InProgress);
		}
	}

	// --- ウェーブ完了後の待機 ---
	if (CurrentSurvialGameModeState == EWarriorSurvialGameModeState::WaveCompleted)
	{
		TimePassedSinceStart += DeltaTime;

		// 次のウェーブへ進むタイミング
		if (TimePassedSinceStart >= WaveCompletedWaitTime)
		{
			TimePassedSinceStart = 0.f;

			// 現在のウェーブ番号を進める
			CurrentWaveCount++;

			// 全ウェーブ終了かどうか判定
			if (HasFinishedAllWaves())
			{
				SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::AllWavesDone);
			}
			else
			{
				// 次のウェーブ待機へ戻る
				SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::WaitSpawnNewWave);
			}
		}
	}
}

// ゲームモードの状態を変更し、Blueprint に通知する
void AWarriorSurvialGameMode::SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState InState)
{
	CurrentSurvialGameModeState = InState;

	// 状態変更を Blueprint に通知
	OnSurvialGameModeStateChanged.Broadcast(CurrentSurvialGameModeState);
}

// 全ウェーブが終了したかどうかを判定
bool AWarriorSurvialGameMode::HasFinishedAllWaves() const
{
	// 現在のウェーブ番号が総ウェーブ数を超えたら終了
	return CurrentWaveCount > TotalWavesToSpawn;
}
