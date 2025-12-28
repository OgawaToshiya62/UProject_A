// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/WarriorBaseAnimInstance.h"
#include "WarriorFunctionLibrary.h"

// アニメーションBPからキャラクターのGameplayTagを安全にチェックするための関数
bool UWarriorBaseAnimInstance::DoseOwnerHaveTag(FGameplayTag TagToCheck) const
{
    // このアニメーションが適用されている Pawn を取得
    if (APawn* OwningPawn = TryGetPawnOwner())
    {
        // キャラクターが指定した GameplayTag を持っているかどうかを判定
        return UWarriorFunctionLibrary::NativeDoesActorHaveTag(OwningPawn, TagToCheck);
    }

    // Pawn が取得できなかった場合はタグを持っていないとみなす
    return false;
}
