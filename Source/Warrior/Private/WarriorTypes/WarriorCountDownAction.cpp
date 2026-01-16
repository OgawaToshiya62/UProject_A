// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorTypes/WarriorCountDownAction.h"

// カウントダウンの進行を毎フレーム更新する
void FWarriorCountDownAction::UpdateOperation(FLatentResponse& Response)
{
	// --- キャンセル処理 ---
	if (bNeedToCancel)
	{
		CountDownOutput = EWarriorCountDownActionOutput::Cancelled;

		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);

		return;
	}

	// --- 完了処理 ---
	if (ElapsedTimeSinceStart >= TotalCountDownTime)
	{
		CountDownOutput = EWarriorCountDownActionOutput::Completed;

		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);

		return;
	}

	// --- 更新間隔の経過チェック ---
	if (ElapsedInterval < UpdateInterval)
	{
		// UpdateInterval に達するまで経過時間を積算
		ElapsedInterval += Response.ElapsedTime();
	}
	else
	{
		// UpdateInterval 経過後：残り時間を更新
		ElapsedTimeSinceStart += UpdateInterval > 0.f ? UpdateInterval : Response.ElapsedTime();

		OutRemainingTime = TotalCountDownTime - ElapsedTimeSinceStart;

		// Blueprint に「更新された」ことを通知
		CountDownOutput = EWarriorCountDownActionOutput::Updated;

		Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);

		// 次の更新間隔に向けてリセット
		ElapsedInterval = 0.f;
	}
}

// カウントダウンをキャンセルするための関数
void FWarriorCountDownAction::CancelAction()
{
	bNeedToCancel = true;
}
