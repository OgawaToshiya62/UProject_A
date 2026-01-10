// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/PawnCombatComponent.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "Components/BoxComponent.h"

#include "WarriorDebugHelper.h"

// 新しく生成された武器をタグと共に登録（同じタグは重複不可）
void UPawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bResgiterAsEquippedWeapon)
{
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister), TEXT("A named named %s has already been added as carried weapon"), *InWeaponTagToRegister.ToString());
	check(InWeaponToRegister);

	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);

	InWeaponToRegister -> OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
	InWeaponToRegister -> OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTargetActor);

	if (bResgiterAsEquippedWeapon)
	{
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
	}
}

// 指定したタグに対応する武器を取得（存在しなければ nullptr）
AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
	if (CharacterCarriedWeaponMap.Contains(InWeaponTagToGet))
	{
		if (AWarriorWeaponBase* const* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
		{
			return *FoundWeapon;
		}
	}

	return nullptr;
}

// 現在装備している武器を取得（タグが無効なら nullptr）
AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		return nullptr;
	}
	
	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

// 武器のコリジョン（当たり判定）をオン／オフする関数
void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, EToggleDamegeType ToggleDamageType)
{
	if (ToggleDamageType == EToggleDamegeType::CurrentEquippedWeapon)
	{
		ToggleCurrentEquippedWeaponCollision(bShouldEnable);
	}
	else
	{
		ToggleBodyCollisionBoxCollision(bShouldEnable, ToggleDamageType);
	}
}

// 武器がアクターにヒットしたときの処理を定義する仮想関数
void UPawnCombatComponent::OnHitTargetActor(AActor* HitActor)
{
}

// 武器がアクターから離れたときの処理を定義する仮想関数
void UPawnCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
}

// 現在装備している武器のコリジョン（当たり判定）を有効化／無効化するための関数
void UPawnCombatComponent::ToggleCurrentEquippedWeaponCollision(bool bShouldEnable)
{
	// 現在キャラクターが装備している武器を取得
	AWarriorWeaponBase* WeaponToToggle = GetCharacterCurrentEquippedWeapon();

	check(WeaponToToggle);

	if (bShouldEnable)
	{
		// 攻撃開始：武器のコリジョンを有効化
		WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		// 攻撃終了：武器のコリジョンを無効化
		WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 多段ヒットを防ぐため、攻撃中に当たったアクターの記録をリセット
		OverlappedActors.Empty();
	}
}

// 左手・右手のボディコリジョン（素手攻撃用）を有効化／無効化するための関数
void UPawnCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamegeType ToggleDamageType)
{
}
