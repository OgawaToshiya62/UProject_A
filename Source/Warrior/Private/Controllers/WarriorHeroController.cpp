// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/WarriorHeroController.h"

// プレイヤー側のコントローラーのコンストラクタ このコントローラーが属するチームIDを設定する
AWarriorHeroController::AWarriorHeroController()
{
	HeroTeamID = FGenericTeamId(0);
}

// このコントローラーのチームIDを返す 敵AIの敵味方判定
FGenericTeamId AWarriorHeroController::GetGenericTeamId() const
{
	return HeroTeamID;
}