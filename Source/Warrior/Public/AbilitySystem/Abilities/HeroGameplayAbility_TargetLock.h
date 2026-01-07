// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_TargetLock.generated.h"

class UWarriorWidgetBase;
class UInputMappingContext;

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

	// 入力方向（左 or 右）に応じてロックオン対象を切り替える
	UFUNCTION(BlueprintCallable)
	void SwitchTarget(const FGameplayTag& InSwitchDirectionTag);

private:
	// ロックオン開始処理
	void TryLockOnTarget();
	// プレイヤー前方に BoxTrace を飛ばし、ロックオン候補のアクターを収集
	void GetAvailableActorsToLock();
	// ロックオン候補の中から、プレイヤーに最も近いアクターを返す
	AActor* GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors);
	// 現在ロック中のターゲットを基準に、周囲の敵を左右に分類する
	void GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft, TArray<AActor*>& OutActorsOnRight);
	// ターゲットロック用のウィジェットを画面に表示する
	void DrawTargetLockWidget();
	// ロックオン中のターゲットのワールド位置をスクリーン座標に変換し,ターゲットロック用ウィジェットをその位置に配置する
	void SetTargetLockWidgetPosition();
	// ターゲットロック開始時に移動速度を制限するための初期化処理
	void InitTargetLockMovement();
	// ターゲットロック開始時に、ロックオン専用の入力マッピングを追加する
	void InitTargetLockMappingContext();

	// ロックオンアビリティを強制終了する
	void CancelTargetLockAbility();
	// ロックオン解除時に内部状態をリセットする
	void CleanUp();
	// ターゲットロック終了時に移動速度を元に戻す処理
	void ResetTargetLockMovement();
	// ターゲットロック終了時に、追加した入力マッピングを削除して元の操作に戻す
	void ResetTargetLockMappingContext();

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

	// ターゲットロック中にキャラクターの向きを補正する際の補間速度
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float TargetLockRotationInterpSpeed = 5.f;

	// ターゲットロック中の最大移動速度
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float TargetLockMaxWalkSpeed = 150.f;

	// ロックオン中に使用する入力マッピング
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	UInputMappingContext* TargetLockMappingContext;

	// ロックオン時にカメラの視点を少し上げる
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float TargetLockCameraOffsetDistance = 20.f;

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

	// ロックオン前の移動速度を保存しておくキャッシュ
	UPROPERTY()
	float CachedDefaultMaxWalkSpeed = 0.f;
};
