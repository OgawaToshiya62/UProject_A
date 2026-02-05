// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "Components/Input/WarriorInputComponent.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Components/UI/HeroUIComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameModes/WarriorBaseGameMode.h"

#include "WarriorDebugHelper.h"     // DebugHelperをインクルード

AWarriorHeroCharacter::AWarriorHeroCharacter()
{
	// カプセルコンポーネントのサイズ(当たり判定 半径42 高さ96)
	GetCapsuleComponent() -> InitCapsuleSize(42.f, 96.f);

	// コントローラーの回転 キャラクターの移動方向に合わせて自動で向きを変えたいため false に設定
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));     // キャラクターにUSpringArmComponent(スプリングアーム)を追加
	CameraBoom -> SetupAttachment(GetRootComponent());                                // CameraBoomをキャラクターのルートコンポーネントにアタッチ
	CameraBoom -> TargetArmLength = 200.f;                                            // カメラとキャラクターの距離を設定(200cm = 2m)
	CameraBoom -> SocketOffset = FVector(0.f, 55.f, 65.f);                            // カメラの位置を調整
	CameraBoom -> bUsePawnControlRotation = true;                                     // プレイヤーのコントローラーの回転に合わせてカメラの向きが変わるようにする

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));    // キャラクターにUCameraComponent(カメラ)を追加 プレイヤーの視点
	FollowCamera -> SetupAttachment(CameraBoom, USpringArmComponent::SocketName);     // カメラをCameraBoom(スプリングアーム)にアタッチ SocketNameはスプリングアームの先端にあるsocketの名前
	FollowCamera -> bUsePawnControlRotation = false;                                  // カメラがプレイヤーのコントローラーの回転に従うかどうかの設定 falseにしているためカメラの回転はスプリングアームに追従する

	GetCharacterMovement() -> bOrientRotationToMovement = true;                       // キャラクターが移動方向に自動で向きを変える
	GetCharacterMovement() -> RotationRate = FRotator(0.f, 500.f, 0.f);               // キャラクターが方向転換する速度
	GetCharacterMovement() -> MaxWalkSpeed = 400.f;                                   // キャラクターの最大歩行速度
	GetCharacterMovement() -> BrakingDecelerationWalking = 2000.f;                    // キャラクターが止まるときの減速力

	// キャラクターに戦闘コンポーネントを生成し、"HeroCombatComponent" という名前で登録
	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
	
	// プレイヤーキャラクターのUI処理を担当するコンポーネントを生成・初期化
	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));
}

// ヒーローキャラクターの戦闘コンポーネントを返す
UPawnCombatComponent* AWarriorHeroCharacter::GetPawnCombatComponent() const
{
	return HeroCombatComponent;
}

// ヒーローキャラクターのUIコンポーネントを返す
UPawnUIComponent* AWarriorHeroCharacter::GetPawnUIComponent() const
{
	return HeroUIComponent;
}

// ヒーローキャラクターのUIコンポーネントを返す
UHeroUIComponent* AWarriorHeroCharacter::GetHeroUIComponent() const
{
	return HeroUIComponent;
}

// キャラクターが Controller に所有されたときに呼ばれる処理（能力システムの初期化や確認に使用）
void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// スタートアップデータが設定されている場合のみ処理を実行
	if (!CharacterStartUpData.IsNull())
	{
		// スタートアップデータを同期的にロード
		if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			// デフォルトの適用レベル
			int32 AbilityApplyLevel = 1;

			// 現在の GameMode を取得
			if (AWarriorBaseGameMode* BaseGameMode = GetWorld()->GetAuthGameMode<AWarriorBaseGameMode>())
			{
				// ゲーム難易度に応じて Ability の適用レベルを変更
				switch (BaseGameMode->GetCurrentGameDifficulty())
				{
				case EWarriorGameDifficulty::Easy:
					AbilityApplyLevel = 4;
					//Debug::Print(TEXT("Current Difficulty: Easy"));
					break;

				case EWarriorGameDifficulty::Normal:
					AbilityApplyLevel = 3;
					//Debug::Print(TEXT("Current Difficulty: Normal"));
					break;

				case EWarriorGameDifficulty::Hard:
					AbilityApplyLevel = 2;
					//Debug::Print(TEXT("Current Difficulty: Hard"));
					break;

				case EWarriorGameDifficulty::VeryHard:
					AbilityApplyLevel = 1;
					//Debug::Print(TEXT("Current Difficulty: Very Hard"));
					break;

				default:
					break;
				}
			}

			// ロードしたデータからアビリティを能力システムに付与
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent, AbilityApplyLevel);
		}
	}
}

void AWarriorHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)        // AWarriorHeroCharacter クラスのメンバー関数 SetupPlayerInputComponent プレイヤーの入力をキャラクターにバインドする為 UInputComponent*PlayerInputCompoent 入力をバインドする対象のコンポーネント
{
	checkf(InputConfigUDataAsset, TEXT("Forgot to assign a valid data asset as input config"));     // InputConfigUDataAsset nullだった場合この行でクラッシュしてTEXTのエラーメッセージが表示

	ULocalPlayer* LocalPlayer = GetController<APlayerController>() -> GetLocalPlayer();             // GetController<APlayerController> このキャラクターのコントローラー(プレイヤー操作の元)を取得

	UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);     // UEnhancedInputLocalPlayerSubsystem EnhancedInputをお使うためのサブシステム GetSubsystem<>() 指定した型のサブシステムを取得する関数

	check(subsystem);                                                                               // 上に合ったcheckfと同じ感じ subsystemがnullだった場合クラッシュを知らせる

	subsystem -> AddMappingContext(InputConfigUDataAsset -> DefaultMappingContext, 0);              // 入力マッピングコンテキストを登録する処理 InputConfigUDataAsset 入力設定を格納したデータアセット DefaultMappingContext その中で定義された基本入力マッピング

	UWarriorInputComponent* WarriorInputComponent = CastChecked<UWarriorInputComponent>(PlayerInputComponent);     // プレイヤーの入力コンポーネントをWarrior用の拡張型に変換 CastChecked 型変換を行い失敗したらクラッシュを知らせる 

	WarriorInputComponent->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);     // InputConfigUDataAsset 入力設定アセット InputTag 意味を持った入力識別子 ETriggerEvent 入力が発生した瞬間に反応する this このキャラクター自身 &ThisClass:: 呼び出す関数ポインタ
	WarriorInputComponent->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);     // 上と同じ感じ

	WarriorInputComponent->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Triggered, this, &ThisClass::Input_SwitchTargetTriggered);
	WarriorInputComponent->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Completed, this, &ThisClass::Input_SwitchTargetCompleted);

	WarriorInputComponent->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::InputTag_PickUp_Stones, ETriggerEvent::Started, this, &ThisClass::Input_PickUpStonesStarted);

	//入力設定データアセットに基づいてアビリティ入力をバインドする
	WarriorInputComponent->BindAbilityInputAction(InputConfigUDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);

}

void AWarriorHeroCharacter::BeginPlay()
{
	Super::BeginPlay();                // 親クラスの BeginPlay を呼び出す (必須)
}

void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();                             // プレイヤーがどの方向に動こうとしているかを取得

	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);                // プレイヤーが向いて言うる方向に合わせて移動ベクトルを回転させる準備

	if (MovementVector.Y != 0.f)                                                                    // 前後移動の入力値　0.fで無い場合
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);     // MovementRotation.RotateVector(...) プレイヤーの視点(Yaw)に合わせて前方向ベクトルを計算 FVector::ForwardVector ワールドの前方向

		AddMovementInput(ForwardDirection, MovementVector.Y);                                       // 前後方向の入力に応じてキャラクターを動かす ForwardDirection 動くべき方向 MovementVector.Y 動く強さ
	}

	if (MovementVector.X != 0.f)                                                                    // 左右移動の入力値　0.fで無い場合
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);         // FVector::RightVector ワールドの右方向

		AddMovementInput(RightDirection, MovementVector.X);                                         // 左右方向の入力に応じてキャラクターを動かす
	}
}

void AWarriorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();                             // プレイヤーがどの方向に視点を動かそうとしているかを取得 InputActionValueからFVector2D型の値を取得

	if (LookAxisVector.X != 0.f)                                                                    // 左右方向(Yaw方向)の入力があるかの確認 0.fで無い場合処理を実行
	{
		AddControllerYawInput(LookAxisVector.X);                                                    // 左右方向(Yaw方向)の入力に回転させる処理
	}

	if (LookAxisVector.Y != 0.f)                                                                    // 上下方向(Pitch方向)の入力があるかの確認 0.fで無い場合処理を実行
	{
		AddControllerPitchInput(LookAxisVector.Y);                                                  // 上下方向(Pitch方向)の入力に回転させる処理
	}
}

// ターゲット切り替えボタンが押されたときに呼ばれる処理
void AWarriorHeroCharacter::Input_SwitchTargetTriggered(const FInputActionValue& InputActionValue)
{
	SwitchDirection = InputActionValue.Get<FVector2D>();
}

// ターゲット切り替えボタンが離されたときに呼ばれる処理
void AWarriorHeroCharacter::Input_SwitchTargetCompleted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		SwitchDirection.X > 0.f ? WarriorGameplayTags::Player_Event_SwitchTarget_Right : WarriorGameplayTags::Player_Event_SwitchTarget_Left,
		Data
	);
}

// 「拾う」入力が押された瞬間に呼ばれる
void AWarriorHeroCharacter::Input_PickUpStonesStarted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;

	// このキャラクターに対して GameplayEvent を送信
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		WarriorGameplayTags::Player_Event_ConsumeStones,
		Data
	);
}

// プレイヤーがキーを押したことを AbilitySystemComponent に通知する
void AWarriorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

// プレイヤーがキーを離したことを AbilitySystemComponent に通知する
void AWarriorHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}
