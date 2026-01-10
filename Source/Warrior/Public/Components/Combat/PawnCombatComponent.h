// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "GameplayTagContainer.h"
#include "PawnCombatComponent.generated.h"

class AWarriorWeaponBase;

// 武器の当たり判定を切り替える対象を指定する列挙体
UENUM(BlueprintType)
enum class EToggleDamegeType : uint8
{
	CurrentEquippedWeapon,     // 現在装備している武器
	LeftHand,                  // 左手の武器
	RightHand                  // 右手の武器
};

/**
 * 
 */
UCLASS()
class WARRIOR_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	// 新しく生成された武器をタグと共に登録する（必要なら装備中として設定）
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bResgiterAsEquippedWeapon = false);

	// 指定したタグに対応する武器を取得
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	// 現在装備している武器のタグ
	UPROPERTY(BlueprintReadWrite, Category = "Warrior|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	// 現在装備している武器を取得
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;
	
	// 武器の当たり判定をオン／オフする関数
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	void ToggleWeaponCollision(bool bShouldEnable, EToggleDamegeType ToggleDamageType = EToggleDamegeType::CurrentEquippedWeapon);

	// 武器がアクターにヒットしたときの処理を定義する仮想関数
	virtual void OnHitTargetActor(AActor* HitActor);
	// 武器がアクターから離れたときの処理を定義する仮想関数
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);

protected:
	// 現在装備している武器のコリジョン（当たり判定）を有効化／無効化するための関数
	virtual void ToggleCurrentEquippedWeaponCollision(bool bShouldEnable);
	// 左手・右手のボディコリジョン（素手攻撃用）を有効化／無効化するための関数
	virtual void ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamegeType ToggleDamageType);

	// 現在コリジョンで重なっているアクターを保持する配列
	TArray<AActor*> OverlappedActors;

private:
	// キャラクターが所持している武器をタグで管理するマップ
	TMap< FGameplayTag, AWarriorWeaponBase* > CharacterCarriedWeaponMap;
};
