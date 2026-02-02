// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "Engine/AssetManager.h"
#include "DataAssets/StartUpData/DataAsset_EnemyStartUpData.h"
#include "Components/UI/EnemyUIComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/WarriorWidgetBase.h"
#include "Components/BoxComponent.h"
#include "WarriorFunctionLibrary.h"
#include "GameModes/WarriorBaseGameMode.h"

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

	// 左手の攻撃判定用コリジョン
	LeftHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("LeftHandCollisionBox");
	LeftHandCollisionBox -> SetupAttachment(GetMesh());
	LeftHandCollisionBox -> SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollisionBox -> OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);

	// 右手の攻撃判定用コリジョン
	RightHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("RightHandCollisionBox");
	RightHandCollisionBox->SetupAttachment(GetMesh());
	RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);
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

// エディタ上でプロパティが変更されたときに呼ばれるコールバック
#if WITH_EDITOR
void AWarriorEnemyCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// 親クラスの処理を必ず実行
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 左手のボーン名が変更された場合、コリジョンを新しいボーンへ再アタッチ
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, LeftHandCollisionBoxAttachBoneName))
	{
		LeftHandCollisionBox -> AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandCollisionBoxAttachBoneName);
	}

	// 右手のボーン名が変更された場合も同様に再アタッチ
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, RightHandCollisionBoxAttachBoneName))
	{
		RightHandCollisionBox -> AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandCollisionBoxAttachBoneName);
	}
}
#endif

// 手のコリジョンが他のアクターと重なった瞬間に呼ばれるコールバック
void AWarriorEnemyCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(this, HitPawn))
		{
			EnemyCombatComponent -> OnHitTargetActor(HitPawn);
		}
	}
}

// 敵キャラクターの初期データを非同期ロードして AbilitySystem に付与する
void AWarriorEnemyCharacter::InitEnemyStartUpData()
{
	if (CharacterStartUpData.IsNull())
	{
		return;
	}

	// デフォルトの適用レベル
	int32 AbilityApplyLevel = 1;

	// 現在の GameMode を取得
	if (AWarriorBaseGameMode* BaseGameMode = GetWorld()->GetAuthGameMode<AWarriorBaseGameMode>())
	{
		// ゲーム難易度に応じて Ability の適用レベルを変更
		switch (BaseGameMode->GetCurrentGameDifficulty())
		{
		case EWarriorGameDifficulty::Easy:
			AbilityApplyLevel = 1;
			break;

		case EWarriorGameDifficulty::Normal:
			AbilityApplyLevel = 2;
			break;

		case EWarriorGameDifficulty::Hard:
			AbilityApplyLevel = 3;
			break;

		case EWarriorGameDifficulty::VeryHard:
			AbilityApplyLevel = 4;
			break;

		default:
			break;
		}
	}

	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[this, AbilityApplyLevel]()
			{
				if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get())
				{
					LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent, AbilityApplyLevel);
				}
			}
		)
	);
}
