// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotateToFaceTarget.generated.h"

// このタスクが Tick 中に使用する一時データを保持する構造体
struct FRotateToFaceTargetTaskMemory
{
	TWeakObjectPtr<APawn> OwningPawn;
	TWeakObjectPtr<AActor> TargetActor;

	// Pawn と Target がどちらも有効かどうかを確認する
	bool IsValid() const
	{
		return OwningPawn.IsValid() && TargetActor.IsValid();
	}

	// タスク終了時にメモリをリセットする
	void Reset()
	{
		OwningPawn.Reset();
		TargetActor.Reset();
	}
};

/**
 * 
 */
UCLASS()
class WARRIOR_API UBTTask_RotateToFaceTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
	// タスクの初期設定を行うコンストラクタ
	UBTTask_RotateToFaceTarget();

	//~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;     // Behavior Tree アセットから Blackboard のキー情報を解決する
	virtual uint16 GetInstanceMemorySize() const override;               // このタスクが必要とするメモリ領域のサイズを返す
	virtual FString GetStaticDescription() const override;               // Behavior Tree エディタでノードを選択したときに表示される説明文
	//~ End UBTNode Interface

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;       // タスク開始時に1度だけ呼ばれる処理
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;     // タスクが InProgress の間、毎フレーム呼ばれる処理

	bool HasReachedAnglePercision(APawn* QueryPawn, AActor* TargetActor) const;     // Pawn がターゲットの方向を向いているかどうかを角度で判定する

	UPROPERTY(EditAnywhere, Category = "Face Target")
	float AnglePrecision;

	UPROPERTY(EditAnywhere, Category = "Face Target")
	float RotationInterpSpeed;

	UPROPERTY(EditAnywhere, Category = "Face Target")
	FBlackboardKeySelector InTargetToFaceKey;
};
