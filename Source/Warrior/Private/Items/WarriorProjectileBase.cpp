// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/WarriorProjectileBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AWarriorProjectileBase::AWarriorProjectileBase()
{
	// プロジェクタイルは Tick を使わない
	PrimaryActorTick.bCanEverTick = false;

	// 投射物の当たり判定となる BoxComponent を作成し、Root に設定
	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	// 衝突判定のみ行う（物理シミュレーションはしない）
	ProjectileCollisionBox -> SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// プレイヤー・敵（Pawn）や壁（WorldStatic/WorldDynamic）に当たるように設定
	ProjectileCollisionBox -> SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	ProjectileCollisionBox -> SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ProjectileCollisionBox -> SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	// Niagara
	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent -> SetupAttachment(GetRootComponent());

	// 移動設定
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp -> InitialSpeed = 700.f;
	ProjectileMovementComp -> MaxSpeed = 900.f;
	ProjectileMovementComp -> Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp -> ProjectileGravityScale = 0.f;

	// 4秒後に自動で消滅
	InitialLifeSpan = 4.f;
}

void AWarriorProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}



