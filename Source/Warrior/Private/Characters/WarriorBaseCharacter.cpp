// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorBaseCharacter.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/WarriorAttributeSet.h"
#include "MotionWarpingComponent.h"

// Sets default values
AWarriorBaseCharacter::AWarriorBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	PrimaryActorTick.bCanEverTick = false;              // Tickを一切使わない ・CPUの負担を減らす ・VFXの時間依存処理がキャラクターに関与しない
	PrimaryActorTick.bStartWithTickEnabled = false;     // 起動時Tickを無効化した状態で開始 bCanEverTickがfalseなら意味がないが保険的に併用

	GetMesh() -> bReceivesDecals = false;               // キャラクターのメッシュがデカール(地面に投影されるVFX)を受け取らないようにする

	// キャラクターの能力管理用コンポーネントを生成（スキルやアビリティの制御に使用）
	WarriorAbilitySystemComponent = CreateDefaultSubobject<UWarriorAbilitySystemComponent>(TEXT("WarriorAbilitySystemComponent"));

	// キャラクターの属性セット（体力・攻撃力など）を生成し、能力システムと連携
	WarriorAttributeSet = CreateDefaultSubobject<UWarriorAttributeSet>(TEXT("WarriorAttributeSet"));

	// MotionWarpingComponent のインスタンスを生成し、キャラクターにアタッチする
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

// GameplayAbilitySystem に準拠するための AbilitySystemComponent 取得関数
UAbilitySystemComponent* AWarriorBaseCharacter::GetAbilitySystemComponent() const
{
	return GetWarriorAbilitySystemComponent();
}

// ベースキャラクターは戦闘コンポーネントを持たないため nullptr を返す
UPawnCombatComponent* AWarriorBaseCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}

// ベースキャラクターはUIコンポーネントを持たないため nullptr を返す
UPawnUIComponent* AWarriorBaseCharacter::GetPawnUIComponent() const
{
	return nullptr;
}

// キャラクターが Controller に所有されたとき、能力システムに自身の情報を初期化
void AWarriorBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 能力システムが存在する場合のみ初期化処理を行う
	if (WarriorAbilitySystemComponent)
	{
		WarriorAbilitySystemComponent->InitAbilityActorInfo(this, this);

		ensureMsgf(!CharacterStartUpData.IsNull(), TEXT("Forgot to assign start up data to %s"), *GetName());
	}
}
