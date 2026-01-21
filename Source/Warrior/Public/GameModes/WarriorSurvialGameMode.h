// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModes/WarriorBaseGameMode.h"
#include "WarriorSurvialGameMode.generated.h"

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

	// 現在のサバイバルゲームモードの状態
	UPROPERTY()
	EWarriorSurvialGameModeState CurrentSurvialGameModeState;

	// 状態変更を Blueprint に通知するデリゲート
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnSurvialGameModeStateChangedDelegate OnSurvialGameModeStateChanged;
};
