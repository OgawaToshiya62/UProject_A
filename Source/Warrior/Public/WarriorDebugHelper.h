#pragma once

namespace Debug     // 画面上とログに同時にメッセージを出力する関数
{
	static void Print(const FString& Msg, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
	{
		if (GEngine)
		{
			GEngine -> AddOnScreenDebugMessage(InKey, 7.f, Color, Msg);     // 画面上にデバッグメッセージを表示(7秒間、指定の色で)

			UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);                     // コンソールログにメッセージを出力(カテゴリ:LogTemp, レベル:Warning)
		}
	}

	// 数値を画面とログに出力する関数
	static void Print(const FString& FloatTitle, float FloatValueToPrint, int32 InKey = -1, const FColor& Color = FColor::MakeRandomColor())
	{
		if (GEngine)
		{
			// ラベルと数値を組み合わせた文字列を作成
			const FString FinalMsg = FloatTitle + TEXT(": ") + FString::SanitizeFloat(FloatValueToPrint);

			// 画面上に表示（7秒間）
			GEngine->AddOnScreenDebugMessage(InKey, 7.f, Color, FinalMsg);

			// コンソールログにも出力
			UE_LOG(LogTemp, Warning, TEXT("%s"), *FinalMsg);
		}
	}
}