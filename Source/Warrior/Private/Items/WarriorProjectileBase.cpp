// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/WarriorProjectileBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "WarriorDebugHelper.h"

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
	// コリジョンイベントをバインドする
	ProjectileCollisionBox -> OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox -> OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);

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
	
	// ダメージポリシーが「オーバーラップ時」の場合、Pawnに対して Overlap 反応に切り替える
	if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}

// 投射物が何かに衝突したときに呼ばれる
void AWarriorProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);

	APawn* HitPawn = Cast<APawn>(OtherActor);

	if (!HitPawn || !UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
	{
		Destroy();
		return;
	}

	bool bIsValidBlock = false;

	const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitPawn, WarriorGameplayTags::Player_Status_Blocking);

	if (bIsPlayerBlocking)
	{
		bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(this, HitPawn);
	}

	FGameplayEventData Data;
	Data.Instigator = this;
	Data.Target = HitPawn;

	if (bIsValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitPawn,
			WarriorGameplayTags::Player_Event_SuccessfulBlock,
			Data
		);
	}
	else
	{
		HandleApplyProjectileDamage(HitPawn, Data);
	}

	Destroy();
}

// 投射物が何かと重なったときに呼ばれる
void AWarriorProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedActors.Contains(OtherActor))
	{
		return;
	}

	OverlappedActors.AddUnique(OtherActor);

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		FGameplayEventData Data;
		Data.Instigator = GetInstigator();
		Data.Target = HitPawn;

		if (UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{
			HandleApplyProjectileDamage(HitPawn, Data);
		}
	}
}

// 投射物が命中した相手にダメージを適用する処理
void AWarriorProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	// ProjectileDamageEffectSpecHandle が有効かチェック
	checkf(ProjectileDamageEffectSpecHandle.IsValid(), TEXT("Forgot to assign a valid spec handle to the projectile: %s"), *GetActorNameOrLabel());

	// ダメージを適用する
	const bool bWasApplied = UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, ProjectileDamageEffectSpecHandle);

	// ダメージが適用された場合、ヒットリアクションのイベントを送る
	if (bWasApplied)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			WarriorGameplayTags::Shared_Event_HitReact,
			InPayload
		);
	}
}
