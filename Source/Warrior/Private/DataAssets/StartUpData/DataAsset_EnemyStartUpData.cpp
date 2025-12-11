// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartUpData/DataAsset_EnemyStartUpData.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"

// 敵の初期データアセットに登録されている戦闘アビリティを指定された AbilitySystemComponent に付与する処理
void UDataAsset_EnemyStartUpData::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	// 親クラスの初期化処理を呼び出す
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

	// 登録済みの戦闘アビリティがあれば付与する
	if (!EnemyCombatAbilities.IsEmpty())
	{
		for (const TSubclassOf < UWarriorEnemyGameplayAbility >& AbilityClass : EnemyCombatAbilities)
		{
			if (!AbilityClass) continue;

			// アビリティ仕様を作成し、AbilitySystem に登録
			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;

			InASCToGive->GiveAbility(AbilitySpec);
		}
	}
}
