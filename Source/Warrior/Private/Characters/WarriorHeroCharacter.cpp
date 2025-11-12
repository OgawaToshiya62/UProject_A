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
}

void AWarriorHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)     //
{
	checkf(InputConfigUDataAsset, TEXT("Forgot to assign a valid data asset as input config"));

	ULocalPlayer* LocalPlayer = GetController<APlayerController>() -> GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(subsystem);

	subsystem -> AddMappingContext(InputConfigUDataAsset -> DefaultMappingContext, 0);

	UWarriorInputComponent* WarriorInputComponent = CastChecked<UWarriorInputComponent>(PlayerInputComponent);

	WarriorInputComponent->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
}

void AWarriorHeroCharacter::BeginPlay()
{
	Super::BeginPlay();                // 親クラスの BeginPlay を呼び出す (必須)

	Debug::Print(TEXT("Working"));     // デバッグメッセージを表示
}

void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)     //
{
}
