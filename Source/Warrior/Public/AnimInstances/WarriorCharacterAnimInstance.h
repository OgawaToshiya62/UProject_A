// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorBaseAnimInstance.h"
#include "WarriorCharacterAnimInstance.generated.h"

class AWarriorBaseCharacter;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorCharacterAnimInstance : public UWarriorBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;                    // アニメーションインスタンスが初めて使われるときに呼ばれる関数　キャラクター参照、取得をしたり初期状態に設定する
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;     // 毎フレーム呼ばれる更新関数　通常のNativeUpdateAnimationと違いスレッドセーフ(並列処理に安全)な更新処理を行うための関数　複雑なロジックや状態更新をここに書くことでアニメーションの滑らかさや安全性を保つことができる

protected:
	UPROPERTY()                                 // キャラクター参照（前方宣言＋空のUPROPERTYで依存を最小化）
	AWarriorBaseCharacter* OwningCharacter;     // TryGetPawnOwner() で取得したキャラクターを AWarriorBaseCharacter にキャストして保持する

	// キャラクターの移動状態（速度・加速度など）を取得するための MovementComponent の参照
	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;

	// 地面上の移動速度（XY平面の速さ）。アニメーション制御に使用
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float GroundSpeed;

	// キャラクターが加速中かどうか（移動入力があるか）を示すフラグ
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAcceleration;

	// キャラクターの移動方向（前後左右の角度）を保持する
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float LocomotionDirection;
};
