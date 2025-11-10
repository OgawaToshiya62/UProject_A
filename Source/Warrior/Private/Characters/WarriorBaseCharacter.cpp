// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorBaseCharacter.h"

// Sets default values
AWarriorBaseCharacter::AWarriorBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	PrimaryActorTick.bCanEverTick = false;              // Tickを一切使わない ・CPUの負担を減らす ・VFXの時間依存処理がキャラクターに関与しない
	PrimaryActorTick.bStartWithTickEnabled = false;     // 起動時Tickを無効化した状態で開始 bCanEverTickがfalseなら意味がないが保険的に併用

	GetMesh() -> bReceivesDecals = false;               // キャラクターのメッシュがデカール(地面に投影されるVFX)を受け取らないようにする
}