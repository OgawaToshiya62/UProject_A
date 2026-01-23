// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/WarriorBaseGameMode.h"

AWarriorBaseGameMode::AWarriorBaseGameMode()
{
	// このアクターが毎フレーム更新処理を実行できるようにする
	PrimaryActorTick.bCanEverTick = true;
	// ゲーム開始時点から Tick を有効にしておく
	PrimaryActorTick.bStartWithTickEnabled = true;
}
