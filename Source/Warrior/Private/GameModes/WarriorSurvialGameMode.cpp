// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/WarriorSurvialGameMode.h"
#include "Engine/AssetManager.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "NavigationSystem.h"
#include "WarriorFunctionLibrary.h"

#include "WarriorDebugHelper.h"

// ゲーム開始時に呼ばれる初期化処理
void AWarriorSurvialGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	// 親クラスの初期化処理を必ず呼ぶ
	Super::InitGame(MapName, Options, ErrorMessage);

	// セーブデータから難易度を読み込むための変数
	EWarriorGameDifficulty SavedGameDifficulty;

	// セーブデータが存在し、ロードに成功した場合のみ難易度を反映
	if (UWarriorFunctionLibrary::TryLoadSavedGameDifficulty(SavedGameDifficulty))
	{
		// GameMode の現在の難易度をセーブされた値に更新
		CurrentGameDifficulty = SavedGameDifficulty;
	}
}

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

	PreLoadNextWaveEnemies();
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
			CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();

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
				PreLoadNextWaveEnemies();
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

// 次のウェーブで必要な敵クラスを非同期ロードしてキャッシュする
void AWarriorSurvialGameMode::PreLoadNextWaveEnemies()
{
	// 全ウェーブ終了なら何もしない
	if (HasFinishedAllWaves())
	{
		return;
	}

	// 前ウェーブのキャッシュをクリア
	PreLoadedEnemyClassMap.Empty();

	// 現在のウェーブで必要な敵クラスをすべて非同期ロード
	for (const FWarriorEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		// 非同期ロード要求
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SpawnerInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda(
				[SpawnerInfo, this]()
				{
					// ロード完了後に実クラスを取得してキャッシュ
					if (UClass* LoadedEnemyClass = SpawnerInfo.SoftEnemyClassToSpawn.Get())
					{
						PreLoadedEnemyClassMap.Emplace(SpawnerInfo.SoftEnemyClassToSpawn, LoadedEnemyClass);
					}
				}
			)
		);
	}
}

// 現在のウェーブに対応する DataTable の行を取得
FWarriorEnemyWaveSpawnerTableRow* AWarriorSurvialGameMode::GetCurrentWaveSpawnerTableRow() const
{
	// DataTable の行名は "Wave1" の形式
	const FName RowName = FName(TEXT("Wave") + FString::FromInt(CurrentWaveCount));

	// 行を取得
	FWarriorEnemyWaveSpawnerTableRow* FoundRow = EnemyWaveSpawnerDataTable->FindRow<FWarriorEnemyWaveSpawnerTableRow>(RowName, FString());

	// 行が見つからない場合はクラッシュ（設定ミス防止）
	checkf(FoundRow, TEXT("Could not find a valid row under the name %s in the data table"), *RowName.ToString());

	return FoundRow;
}

// 敵をスポーンし、今回スポーンした数を返す
int32 AWarriorSurvialGameMode::TrySpawnWaveEnemies()
{
	// TargetPoint が未取得ならレベルから取得
	if (TargetPointsArray.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPointsArray);
	}

	// スポーン位置が無い場合はエラー
	checkf(!TargetPointsArray.IsEmpty(), TEXT("No valid target point found in level: %s for spawning enemies"), *GetWorld()->GetName());

	uint32 EnemiesSpawnedThisTime = 0;

	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 各スポーン定義に対してスポーン処理
	for (const FWarriorEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		// この定義でスポーンする数をランダム決定
		const int32 NumToSpawn = FMath::RandRange(SpawnerInfo.MinPerSpawnCount, SpawnerInfo.MaxPerSpawnCount);

		// プリロード済みクラスを取得
		UClass* LoadedEnemyClass = PreLoadedEnemyClassMap.FindChecked(SpawnerInfo.SoftEnemyClassToSpawn);

		for (int32 i = 0; i < NumToSpawn; i++)
		{
			// ランダムな TargetPoint を選択
			const int32 RandomTargetPointIndex = FMath::RandRange(0, TargetPointsArray.Num() - 1 );
			const FVector SpawnOrigin = TargetPointsArray[RandomTargetPointIndex] -> GetActorLocation();
			const FRotator SpawnRotation = TargetPointsArray[RandomTargetPointIndex] -> GetActorForwardVector().ToOrientationRotator();

			// NavMesh 上のランダム位置を取得
			FVector RandomLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, SpawnOrigin, RandomLocation, 400.f);

			// 少し浮かせてスポーン
			RandomLocation += FVector(0.f, 0.f, 150.f);

			// 敵をスポーン
			AWarriorEnemyCharacter* SpawnedEnemy = GetWorld() -> SpawnActor<AWarriorEnemyCharacter>(LoadedEnemyClass, RandomLocation, SpawnRotation, SpawnParam);

			if (SpawnedEnemy)
			{
				// Destroy 時のコールバック登録
				SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);

				EnemiesSpawnedThisTime++;
				TotalSpawnedEnemiesThisWaveCounter++;
			}

			// ウェーブのスポーン上限に達したら終了
			if (!ShouldKeepSpawnEnemies())
			{
				return EnemiesSpawnedThisTime;
			}
		}
	}
	
	return EnemiesSpawnedThisTime;

}

// まだ敵をスポーンすべきかどうか（総数に達していないか）を判定
bool AWarriorSurvialGameMode::ShouldKeepSpawnEnemies() const
{
	// スポーンした数がウェーブの上限に達したら false
	return TotalSpawnedEnemiesThisWaveCounter < GetCurrentWaveSpawnerTableRow()->TotalEnemyToSpawnThisWave;
}

// 敵が Destroy された時に呼ばれるコールバック
void AWarriorSurvialGameMode::OnEnemyDestroyed(AActor* DestroyedActor)
{
	// 生存中の敵数を減らす
	CurrentSpawnedEnemiesCounter--;

	// まだスポーンすべき敵が残っているなら追加スポーン
	if (ShouldKeepSpawnEnemies())
	{
		CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
	}
	// 全て倒され、かつスポーン上限に達しているならウェーブ完了
	else if (CurrentSpawnedEnemiesCounter == 0)
	{
		TotalSpawnedEnemiesThisWaveCounter = 0;
		CurrentSpawnedEnemiesCounter = 0;

		SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::WaveCompleted);
	}
}

void AWarriorSurvialGameMode::RegisterSpawnedEnemies(const TArray<AWarriorEnemyCharacter*>& InEnemiesToRegister)
{
	for (AWarriorEnemyCharacter* SpawnedEnemy : InEnemiesToRegister)
	{
		if (SpawnedEnemy)
		{
			CurrentSpawnedEnemiesCounter++;

			SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);
		}
	}
}
