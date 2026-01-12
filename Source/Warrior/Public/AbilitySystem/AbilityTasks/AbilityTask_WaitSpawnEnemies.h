// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitSpawnEnemies.generated.h"

class AWarriorEnemyCharacter;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitSpawnEnemiesDelegate, const TArray<AWarriorEnemyCharacter*>&, SpawnedEnemies);     // 敵スポーン完了時に、スポーンした敵の配列を通知するデリゲート

/**
 * 
 */
UCLASS()
class WARRIOR_API UAbilityTask_WaitSpawnEnemies : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	// GameplayEvent を待ち、イベントが発生したら敵をスポーンする AbilityTask を生成する
	UFUNCTION(BlueprintCallable, Category = "Warrior|AbilityTasks", meta = (DisplayName = "Wait Gameplay Event And Spawn Enemies", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", NumToSpawn = "1", RandomSpawnRadius = "200"))
	static UAbilityTask_WaitSpawnEnemies* WaitSpawnEnemies(
		UGameplayAbility* OwningAbility,
		FGameplayTag EventTag,
		TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn,
		int32 NumToSpawn,
		const FVector& SpawnOrigin,
		float RandomSpawnRadius
	);

	// 敵のスポーンが成功したときに呼ばれる
	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate OnSpawnFinished;

	// 敵のスポーンが失敗したときに呼ばれる
	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate DidNotSpawn;

	//~ Begin UGameplayTask Interface
	virtual void Activate() override;                           // AbilityTask が開始されたときに呼ばれる
	virtual void OnDestroy(bool bInOwnerFinished) override;     // 登録したデリゲートを解除して後処理を行う
	//~ End UGameplayTask Interface

private:
	// 後でスポーン処理を行うために必要な情報をキャッシュしておく
	FGameplayTag CachedEventTag;
	TSoftClassPtr<AWarriorEnemyCharacter> CachedSoftEnemyClassToSpawn;
	int32 CachedNumToSpawn;
	FVector CachedSpawnOrigin;
	float CachedRandomSpawnRadius;
	// GameplayEvent のデリゲート登録を解除するためのハンドル
	FDelegateHandle DelegateHandle;

	// イベントを受け取ったら敵スポーン処理を行い、完了後にタスクを終了する
	void OnGameplayEventReceived(const FGameplayEventData* InPayLoad);
	// ロード完了後に実際の敵スポーン処理を行う
	void OnEnemyClassLoaded();
};
