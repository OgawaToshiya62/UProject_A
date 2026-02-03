#pragma once

// タグ確認用の列挙型（Yes/No）
UENUM()
enum class EWarriorConfirmType : uint8
{
	Yes,
	No
};

// 戦闘コンポーネントが有効かどうかを表す列挙型
UENUM()
enum class EWarriorValidType : uint8
{
	Valid,
	Invalid
};

// GameplayEffect の適用処理などで成功／失敗を表す列挙型
UENUM()
enum class EWarriorSuccessType : uint8
{
	Successful,
	Failed
};

// カウントダウン処理に対する入力アクション
UENUM()
enum class EWarriorCountDownActionInput : uint8
{
	Start,
	Cancel
};

// カウントダウン処理の結果を表す出力アクション
UENUM()
enum class EWarriorCountDownActionOutput : uint8
{
	Updated,
	Completed,
	Cancelled
};

// ゲームの難易度を表す列挙体
UENUM(BlueprintType)
enum class EWarriorGameDifficulty : uint8
{
	Easy,
	Normal,
	Hard,
	VeryHard
};

// ゲームの入力モードを表す列挙体
UENUM(BlueprintType)
enum class EWarriorInputMode : uint8
{
	GameOnly,
	UIOnly
};