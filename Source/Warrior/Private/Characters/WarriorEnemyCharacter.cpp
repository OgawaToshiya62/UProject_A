// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Combat/EnemyCombatComponent.h"

// 敵キャラクターのコンストラクタ
AWarriorEnemyCharacter::AWarriorEnemyCharacter()
{
	// ワールドに配置またはスポーンされたときに AI に自動操作させる
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// コントローラの回転を直接使わず、移動方向に応じて回転させる
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// キャラクタームーブメントの設定
	GetCharacterMovement() -> bUseControllerDesiredRotation = false;
	GetCharacterMovement() -> bOrientRotationToMovement = true;
	GetCharacterMovement() -> RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement() -> MaxWalkSpeed = 300.f;
	GetCharacterMovement() -> BrakingDecelerationWalking = 1000.f;

	// 敵キャラクター専用の戦闘コンポーネントを生成
	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");
}
