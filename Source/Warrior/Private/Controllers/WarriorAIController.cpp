// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/WarriorAIController.h"
#include "Navigation/CrowdFollowingComponent.h"

#include "WarriorDebugHelper.h"

// コンストラクタ：AI の PathFollowingComponent を Crowd 用に差し替える
AWarriorAIController::AWarriorAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		Debug::Print(TEXT("CrowdFollowingComponent valid"), FColor::Green);
	}
}
