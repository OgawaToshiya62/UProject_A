// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "Engine/AssetManager.h"
#include "DataAssets/StartUpData/DataAsset_EnemyStartUpData.h"
#include "Components/UI/EnemyUIComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/WarriorWidgetBase.h"

#include "WarriorDebugHelper.h"

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

	// 敵キャラクターのUI処理を担当するコンポーネントを生成・初期化
	EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>("EnemyUIComponent");

	// 敵の頭上に表示するHPバー用のWidgetComponentを生成し、メッシュにアタッチ
	EnemyHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("EnemyHealthWidgetComponent");
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());
}

// 敵キャラクターの戦闘コンポーネントを返す
UPawnCombatComponent* AWarriorEnemyCharacter::GetPawnCombatComponent() const
{
	return EnemyCombatComponent;
}

// 敵キャラクターのUIコンポーネントを返す
UPawnUIComponent* AWarriorEnemyCharacter::GetPawnUIComponent() const
{
	return EnemyUIComponent;
}

UEnemyUIComponent* AWarriorEnemyCharacter::GetEnemyUIComponent() const
{
	return EnemyUIComponent;
}

// ゲーム開始時に、WidgetComponent 内のウィジェットを取得しこの敵キャラを対象として初期化する
void AWarriorEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UWarriorWidgetBase* HealthWidget = Cast<UWarriorWidgetBase>(EnemyHealthWidgetComponent -> GetUserWidgetObject()))
	{
		HealthWidget -> InitEnemyCreatedWidget(this);
	}
}

// 敵キャラクターがコントローラに操作され始めたときに呼ばれる
void AWarriorEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 初期データをセットアップ
	InitEnemyStartUpData();
}

// 敵キャラクターの初期データを非同期ロードして AbilitySystem に付与する
void AWarriorEnemyCharacter::InitEnemyStartUpData()
{
	if (CharacterStartUpData.IsNull())
	{
		return;
	}

	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[this]()
			{
				if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get())
				{
					LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
				}
			}
		)
	);
}
