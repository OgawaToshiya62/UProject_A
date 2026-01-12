// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_WaitSpawnEnemies.h"
#include "AbilitySystemComponent.h"
#include "Engine/AssetManager.h"
#include "NavigationSystem.h"
#include "Characters/WarriorEnemyCharacter.h"

#include "WarriorDebugHelper.h"

// GameplayEvent を待ち、イベントが発生したら敵をスポーンする AbilityTask を生成する
UAbilityTask_WaitSpawnEnemies* UAbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(UGameplayAbility* OwningAbility, FGameplayTag EventTag, TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn, int32 NumToSpawn, const FVector& SpawnOrigin, float RandomSpawnRadius)
{
    // AbilityTask のインスタンスを生成
    UAbilityTask_WaitSpawnEnemies* Node = NewAbilityTask<UAbilityTask_WaitSpawnEnemies>(OwningAbility);

    // 後でスポーン処理を行うために必要な情報をキャッシュ
    Node -> CachedEventTag = EventTag;
    Node -> CachedSoftEnemyClassToSpawn = SoftEnemyClassToSpawn;
    Node -> CachedNumToSpawn = NumToSpawn;
    Node -> CachedSpawnOrigin = SpawnOrigin;
    Node -> CachedRandomSpawnRadius = RandomSpawnRadius;

    return Node;
}

// AbilityTask が開始されたときに呼ばれる
void UAbilityTask_WaitSpawnEnemies::Activate()
{
    // このタスクが待つイベントタグに対応するデリゲートを取得（なければ作成）
    FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);

    // GameplayEvent を受信したときに OnGameplayEventReceived が呼ばれるよう登録
    DelegateHandle = Delegate.AddUObject(this, &ThisClass::OnGameplayEventReceived);
}

// 登録したデリゲートを解除して後処理を行う
void UAbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
    // Activate で登録したデリゲートを解除してクリーンアップ
    FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);

    Delegate.Remove(DelegateHandle);

    // 親クラスの後処理も実行
    Super::OnDestroy(bInOwnerFinished);
}

// イベントを受け取ったら敵スポーン処理を行い、完了後にタスクを終了する
void UAbilityTask_WaitSpawnEnemies::OnGameplayEventReceived(const FGameplayEventData* InPayLoad)
{
    // 敵クラス（SoftClass）が設定されているか確認
    if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
    {
        // SoftClass を非同期ロードする
        UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
            CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),
            FStreamableDelegate::CreateUObject(this, &ThisClass::OnEnemyClassLoaded)
        );
    }
    else
    {
        // 敵クラスが設定されていない場合はスポーンできないので DidNotSpawn を通知
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
        }

        // タスクを終了
        EndTask();
    }
    
}

// ロード完了後に実際の敵スポーン処理を行う
void UAbilityTask_WaitSpawnEnemies::OnEnemyClassLoaded()
{
    // ロードされたクラスを取得
    UClass* LoadedClass = CachedSoftEnemyClassToSpawn.Get();
    UWorld* World = GetWorld();

    // クラスまたは World が無効ならスポーンできない
    if (!LoadedClass || !World)
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
        }
        
        EndTask();

        return;
    }

    // スポーンした敵を格納する配列
    TArray<AWarriorEnemyCharacter*> SpawnedEnemies;

    // スポーンパラメータ設定（衝突があっても必ずスポーンする）
    FActorSpawnParameters SpawnParam;
    SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // 指定された数だけ敵をスポーンする
    for (int32 i = 0; i < CachedNumToSpawn; i++)
    {
        FVector RandomLocation;
        // ナビメッシュ上のランダムな地点を取得
        UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, CachedSpawnOrigin, RandomLocation, CachedRandomSpawnRadius);

        // 少し浮かせてスポーン（地面に埋まらないように）
        RandomLocation += FVector(0.f, 0.f, 150.f);

        // キャラクターが現在向いている方向を回転値に変換し敵をスポーンさせる際の向きとして使用する
        const FRotator SpawnFacingRotation = AbilitySystemComponent->GetAvatarActor()->GetActorForwardVector().ToOrientationRotator();

        // 敵をスポーン
        AWarriorEnemyCharacter* SpawnedEnemy = World->SpawnActor<AWarriorEnemyCharacter>(LoadedClass, RandomLocation, SpawnFacingRotation, SpawnParam);

        if (SpawnedEnemy)
        {
            SpawnedEnemies.Add(SpawnedEnemy);
        }
    }

    // スポーン結果を Blueprint に通知
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        if (!SpawnedEnemies.IsEmpty())
        {
            // 成功：スポーンした敵の配列を渡す
            OnSpawnFinished.Broadcast(SpawnedEnemies);
        }
        else
        {
            // 失敗：空の配列を渡す
            DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
        }
    }

    // タスク終了
    EndTask();
}
