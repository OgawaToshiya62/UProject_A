// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarriorTypes/WarriorEnumTypes.h"

class FWarriorCountDownAction : public FPendingLatentAction
{
public:
	// カウントダウンアクションの初期化
	FWarriorCountDownAction(float InTotalCountDownTime, float InUpdateInterval, float& InOutRemainingTime, EWarriorCountDownActionOutput& InCountDownOutput, const FLatentActionInfo& LatentInfo)
		: bNeedToCancel(false)
		, TotalCountDownTime(InTotalCountDownTime)
		, UpdateInterval(InUpdateInterval)
		, OutRemainingTime(InOutRemainingTime)
		, CountDownOutput(InCountDownOutput)
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, ElapsedInterval(0.f)
		, ElapsedTimeSinceStart(0.f)
	{
	}

	// Latent Action の毎フレーム更新処理
	virtual void UpdateOperation(FLatentResponse& Response) override;

	// カウントダウンをキャンセルするための関数
	void CancelAction();

private:
	// カウントダウンがキャンセルされたか
	bool bNeedToCancel;
	// カウントダウンの総時間
	float TotalCountDownTime;
	// 残り時間を更新する間隔
	float UpdateInterval;
	// Blueprint に返す残り時間
	float& OutRemainingTime;
	// Blueprint に返す出力状態
	EWarriorCountDownActionOutput& CountDownOutput;
	// Latent アクション完了時に呼び戻す関数名
	FName ExecutionFunction;
	// Latent アクション完了時に実行する
	int32 OutputLink;
	// Latent アクション完了時に呼び戻す対象オブジェクト
	FWeakObjectPtr CallbackTarget;
	// UpdateInterval の経過時間
	float ElapsedInterval;
	// カウントダウン開始からの経過時間
	float ElapsedTimeSinceStart;
};