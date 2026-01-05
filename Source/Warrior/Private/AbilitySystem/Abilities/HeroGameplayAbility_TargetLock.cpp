// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/WarriorWidgetBase.h"
#include "Controllers/WarriorHeroController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"

#include "WarriorDebugHelper.h"

// アビリティ発動時に呼ばれる
void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	TryLockOnTarget();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

// アビリティ終了時に呼ばれる
void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CleanUp();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ターゲットロック中に毎フレーム呼ばれる処理
void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	if (!CurrentLockedActor ||
		UWarriorFunctionLibrary::NativeDoesActorHaveTag(CurrentLockedActor, WarriorGameplayTags::Shared_Status_Dead) ||
		UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Shared_Status_Dead)
		)
	{
		CancelTargetLockAbility();
		return;
	}

	SetTargetLockWidgetPosition();
}

// ロックオンを試みる際に呼ばれる
void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableActorsToLock();

	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	CurrentLockedActor = GetNearestTargetFromAvailableActors(AvailableActorsToLock);

	if (CurrentLockedActor)
	{
		DrawTargetLockWidget();

		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelTargetLockAbility();
	}
}

// プレイヤーの前方に BoxTrace を飛ばし、ロックオン可能なアクターを収集する
void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{
	TArray<FHitResult> BoxTraceHits;

	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo() -> GetActorLocation(),
		GetHeroCharacterFromActorInfo() -> GetActorLocation() + GetHeroCharacterFromActorInfo() -> GetActorForwardVector() * BoxTraceDistance,
		TraceBoxSize / 2.f,
		GetHeroCharacterFromActorInfo() -> GetActorForwardVector().ToOrientationRotator(),
		BoxTraceChannel,
		false,
		TArray< AActor* >(),
		bShowPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		BoxTraceHits,
		true
	);

	for (const FHitResult& TraceHit : BoxTraceHits)
	{
		if (AActor* HitActor = TraceHit.GetActor())
		{
			if (HitActor != GetHeroCharacterFromActorInfo())
			{
				AvailableActorsToLock.AddUnique(HitActor);
			}
		}
	}
}

// ロックオン候補の配列から、プレイヤーに最も近いアクターを返す
AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	float ClosestDistance = 0.f;
	return UGameplayStatics::FindNearestActor(GetHeroCharacterFromActorInfo()->GetActorLocation(), InAvailableActors, ClosestDistance);
}

// ターゲットロック用ウィジェットを画面に描画する
void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if (!DrawnTargetLockWidget)
	{
		checkf(TargetLockWidgetClass, TEXT("Forgot to assign a valid widget class in Blueprint"));

		DrawnTargetLockWidget = CreateWidget<UWarriorWidgetBase>(GetHeroControllerFromActorInfo(), TargetLockWidgetClass);

		check(DrawnTargetLockWidget);

		DrawnTargetLockWidget -> AddToViewport();
	}
}

// ターゲットロックウィジェットを、ロック中の敵の位置に追従させる
void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	// ウィジェットまたはターゲットが無効ならロックオンを終了
	if (!DrawnTargetLockWidget || !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}

	// 敵のワールド座標 → スクリーン座標へ変換
	FVector2D ScreenPosition;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetHeroControllerFromActorInfo(),
		CurrentLockedActor -> GetActorLocation(),
		ScreenPosition,
		true
	);

	// ウィジェットのサイズを初回のみ取得
	if (TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
		[this](UWidget* FoundWidget)
		{
			if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
			{
				TargetLockWidgetSize.X = FoundSizeBox -> GetWidthOverride();
				TargetLockWidgetSize.Y = FoundSizeBox -> GetHeightOverride();
			}
		}
	);
	}

	// ウィジェットの中心がターゲットに重なるように補正
	ScreenPosition -= (TargetLockWidgetSize / 2.f);

	// ウィジェットを画面上に配置
	DrawnTargetLockWidget -> SetPositionInViewport(ScreenPosition, false);
}

// ロックオンアビリティを強制終了する
void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

// ロックオン解除時に内部状態をリセットする
void UHeroGameplayAbility_TargetLock::CleanUp()
{
	AvailableActorsToLock.Empty();

	CurrentLockedActor = nullptr;

	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}

	DrawnTargetLockWidget = nullptr;

	TargetLockWidgetSize = FVector2D::ZeroVector;
}