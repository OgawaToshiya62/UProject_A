// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorCharacterAnimInstance.h"
#include "WarriorHeroAnimInstance.generated.h"

class AWarriorHeroCharacter;

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorHeroAnimInstance : public UWarriorCharacterAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;                    // アニメーション初期化時に呼ばれる関数（キャラクター参照などを取得）
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;     // 毎フレーム呼ばれるスレッドセーフなアニメーション更新関数

protected:
	// アニメーションの持ち主キャラクター（Blueprintから読み取り可能）
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Refrences")
	AWarriorHeroCharacter* OwningHeroCharacter;

	// 一定時間動きがなければリラックス状態に入るかどうかのフラグ
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bShouldEnterRelaxState;

	// リラックス状態に入るまでの待機時間（秒）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float EnterRelaxtStateThreshold = 5.f;

	// キャラクターが動いていない時間の累積（リラックス判定用）
	float IdleElpasedTime;
};
