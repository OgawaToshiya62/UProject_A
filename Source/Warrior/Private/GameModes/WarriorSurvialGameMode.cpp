// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/WarriorSurvialGameMode.h"

// ゲーム開始時に呼ばれる
void AWarriorSurvialGameMode::BeginPlay()
{
	Super::BeginPlay();
}

// 毎フレーム呼ばれる
void AWarriorSurvialGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ゲームモードの状態を変更し、Blueprint に通知する
void AWarriorSurvialGameMode::SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState InState)
{
	CurrentSurvialGameModeState = InState;

	// 状態変更を Blueprint に通知
	OnSurvialGameModeStateChanged.Broadcast(CurrentSurvialGameModeState);
}
