// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/WarriorHeroWeapon.h"

// 外部から渡されたアビリティハンドルを武器に保存する
void AWarriorHeroWeapon::AssignGrantedAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles)
{
	GrantedAbilitySpecHandles = InSpecHandles;
}

// 武器に保存されているアビリティハンドル一覧を返す
TArray<FGameplayAbilitySpecHandle> AWarriorHeroWeapon::GetGrantedAbilitySpecHandles() const
{
	return GrantedAbilitySpecHandles;
}
