// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"
#include "Characters/WarriorEnemyCharacter.h"

// アクター情報から敵キャラクターを取得する（キャッシュを利用して効率化）
AWarriorEnemyCharacter* UWarriorEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
    if (!CachedWarriorEnemyCharacter.IsValid())
    {
        CachedWarriorEnemyCharacter = Cast<AWarriorEnemyCharacter>(CurrentActorInfo -> AvatarActor);
    }

    return CachedWarriorEnemyCharacter.IsValid()? CachedWarriorEnemyCharacter.Get() : nullptr;
}

// アクター情報から敵キャラクターの戦闘コンポーネントを取得する
UEnemyCombatComponent* UWarriorEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
    return GetEnemyCharacterFromActorInfo() -> GetEnemyCombatComponent();
}
