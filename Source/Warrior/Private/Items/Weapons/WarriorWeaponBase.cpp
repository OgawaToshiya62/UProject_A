// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/WarriorWeaponBase.h"
#include "Components/BoxComponent.h"

AWarriorWeaponBase::AWarriorWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 武器のメッシュコンポーネントを生成
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	// 武器の当たり判定用ボックスコンポーネントを生成
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox -> SetupAttachment(GetRootComponent());                     // 武器メッシュに当たり判定ボックスをアタッチ
	WeaponCollisionBox -> SetBoxExtent(FVector(20.f));                             // 当たり判定ボックスのサイズを設定（20x20x20）
	WeaponCollisionBox -> SetCollisionEnabled(ECollisionEnabled::NoCollision);     // 初期状態では衝突判定を無効化（必要時に有効化）
}

