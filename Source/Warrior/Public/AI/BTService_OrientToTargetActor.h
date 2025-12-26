// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_OrientToTargetActor.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UBTService_OrientToTargetActor : public UBTService
{
	GENERATED_BODY()
	
	// ターゲットの方向へ向き直るための BTService
	UBTService_OrientToTargetActor();

	//~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;     // Behavior Tree アセットから設定を読み込む
	virtual FString GetStaticDescription() const override;               // Behavior Tree エディタ上で、このノードの説明文として表示されるテキストを返す
	//~ End UBTNode Interface

	// Service が一定間隔で呼ばれるたびに実行される処理
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// Blackboard から取得するターゲット Actor のキー
	UPROPERTY(EditAnywhere, Category = "Target")
	FBlackboardKeySelector InTargetActorKey;

	// ターゲット方向へ回転するときの補間速度
	UPROPERTY(EditAnywhere, Category = "Target")
	float RotationInterpSpeed;
};
