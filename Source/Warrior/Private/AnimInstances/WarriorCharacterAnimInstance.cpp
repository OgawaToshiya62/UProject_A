// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/WarriorCharacterAnimInstance.h"
#include "Characters/WarriorBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

// アニメーションインスタンスの初期化処理
void UWarriorCharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<AWarriorBaseCharacter>(TryGetPawnOwner());     // TryGetPawnOwner アニメーションが属しているキャラクターを取得する関数 Cast<AWarriorBaseCharacter>(...) TryGetPawnOwnerはAPawnを返すがAWarriorBaseCharacter型として扱いたいため使用 Castで安全に型変換する(失敗するとnull)
	
	// キャラクターが存在する場合、その移動コンポーネントを取得して保存
	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter -> GetCharacterMovement();
	}
}

// 毎フレーム（またはスレッドセーフなタイミング）で呼ばれるアニメーション更新処理
void UWarriorCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	// キャラクターまたは移動コンポーネントが無効な場合は処理を中断
	if (!OwningCharacter || !OwningMovementComponent)
	{
		return;
	}

	//地面上の移動速度を取得（アニメーション用）
	GroundSpeed = OwningCharacter -> GetVelocity().Size2D();

	// 移動入力があるかどうかを判定（加速度があるか）
	bHasAcceleration = OwningMovementComponent -> GetCurrentAcceleration().SizeSquared2D() > 0.f;

	// キャラクターの移動方向を計算する
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
}