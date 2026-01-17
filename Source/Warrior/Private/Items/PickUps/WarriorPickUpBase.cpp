// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickUps/WarriorPickUpBase.h"
#include "Components/SphereComponent.h"

// ピックアップアクターの初期設定
AWarriorPickUpBase::AWarriorPickUpBase()
{
	// 毎フレーム更新は不要
	PrimaryActorTick.bCanEverTick = false;

	// コリジョン球の作成と設定
	PickUpCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpCollisionSphere"));
	SetRootComponent(PickUpCollisionSphere);
	PickUpCollisionSphere->InitSphereRadius(50.f);
	// オーバーラップ開始イベントをバインド
	PickUpCollisionSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnPickUpCollisionSphereBeginOverlap);
}

// コリジョン球に他のアクターが入ったときに呼ばれる関数
void AWarriorPickUpBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}


