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