// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_TargetLock.generated.h"

class UWarriorWidgetBase;

/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroGameplayAbility_TargetLock : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()
	
protected:

	// ~ Begin UGameplayAbility Interface
	// ターゲットロック能力の開始・終了処理をフックするためのオーバーライド
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// ~ End UGameplayAbility Interface

	// ターゲットロック中に毎フレーム呼ばれる処理
	UFUNCTION(BlueprintCallable)
	void OnTargetLockTick(float DeltaTime);

private:
	// ロックオン開始処理
	void TryLockOnTarget();
	// プレイヤー前方に BoxTrace を飛ばし、ロックオン候補のアクターを収集
	void GetAvailableActorsToLock();
	// ロックオン候補の中から、プレイヤーに最も近いアクターを返す
	AActor* GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors);
	// ターゲットロック用のウィジェットを画面に表示する
	void DrawTargetLockWidget();
	// ロックオン中のターゲットのワールド位置をスクリーン座標に変換し,ターゲットロック用ウィジェットをその位置に配置する
	void SetTargetLockWidgetPosition();

	// ロックオンアビリティを強制終了する
	void CancelTargetLockAbility();
	// ロックオン解除時に内部状態をリセットする
	void CleanUp();

	// BoxTrace を飛ばす距離（プレイヤー前方方向）
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float BoxTraceDistance = 5000.f;

	// BoxTrace のサイズ（横・奥行き・高さ）
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	FVector TraceBoxSize = FVector(5000.f, 5000.f, 300.f);

	// BoxTrace がヒット対象とするオブジェクトタイプ
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	TArray< TEnumAsByte < EObjectTypeQuery > > BoxTraceChannel;

	// デバッグ用に BoxTrace を表示するかどうか
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	bool bShowPersistentDebugShape = false;

	// ロックオン時に表示するウィジェットのクラス
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	TSubclassOf<UWarriorWidgetBase> TargetLockWidgetClass;

	// ロックオン候補として検出されたアクター一覧
	UPROPERTY()
	TArray<AActor*> AvailableActorsToLock;

	// 現在ロックオン中のアクター
	UPROPERTY()
	AActor* CurrentLockedActor;

	// 実際に生成されたターゲットロックウィジェットのインスタンス
	UPROPERTY()
	UWarriorWidgetBase* DrawnTargetLockWidget;

	// ターゲットロックウィジェットのサイズ（初回に自動取得）
	UPROPERTY()
	FVector2D TargetLockWidgetSize = FVector2D::ZeroVector;
};
