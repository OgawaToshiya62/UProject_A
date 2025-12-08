// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/Hero/WarriorHeroLinkedAnimLayer.h"
#include "AnimInstances/Hero/WarriorHeroAnimInstance.h"

// このアニメーションレイヤーが所属するスケルタルメッシュコンポーネントからアニメーションインスタンスを取得し、UWarriorHeroAnimInstance型にキャストして返す
UWarriorHeroAnimInstance* UWarriorHeroLinkedAnimLayer::GetHeroAnimInstance() const
{
    return Cast<UWarriorHeroAnimInstance>(GetOwningComponent() -> GetAnimInstance());
}
