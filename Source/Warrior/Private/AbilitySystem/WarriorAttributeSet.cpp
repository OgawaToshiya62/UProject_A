// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAttributeSet.h"

// コンストラクタ：キャラクターの属性を初期化する
UWarriorAttributeSet::UWarriorAttributeSet()
{
	// 体力
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);

	// 怒りゲージ
	InitCurrentRage(1.f);
	InitMaxRage(1.f);

	// 攻撃力・防御力
	InitAttackPower(1.f);
	InitDefensePower(1.f);
}
