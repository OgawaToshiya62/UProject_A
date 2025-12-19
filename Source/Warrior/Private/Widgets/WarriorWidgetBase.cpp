// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/WarriorWidgetBase.h"
#include "Interface/PawnUIInterface.h"

// ウィジェット初期化時に、所有プレイヤーの HeroUIComponent を取得して Blueprint 側へ通知する
void UWarriorWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
	{
		if (UHeroUIComponent* HeroUIComponent = PawnUIInterface->GetHeroUIComponent())
		{
			BP_OnOwningHeroUIComponentInitialized(HeroUIComponent);
		}
	}

}

// 敵アクターをUIインターフェースとして扱い、EnemyUIComponentを取得して Blueprint 側へ初期化通知を送る
void UWarriorWidgetBase::InitEnemyCreatedWidget(AActor* OwningEnemyActor)
{
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(OwningEnemyActor))
	{
		UEnemyUIComponent* EnemyUIComponent = PawnUIInterface -> GetEnemyUIComponent();

		checkf(EnemyUIComponent, TEXT("Failed to extrac an EnemyUIComponent from %s"), *OwningEnemyActor -> GetActorNameOrLabel());

		BP_OnOwningEnemyUIComponentInitialized(EnemyUIComponent);
	}
}
