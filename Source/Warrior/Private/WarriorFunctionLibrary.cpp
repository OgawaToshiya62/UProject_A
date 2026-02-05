// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Interface/PawnCombatInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "WarriorGameplayTags.h"
#include "WarriorTypes/WarriorCountDownAction.h"
#include "WarriorGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/WarriorSaveGame.h"

#include "WarriorDebugHelper.h"

// 指定アクターから WarriorAbilitySystemComponent を取得する（必ず存在する前提）
UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	check(InActor);

	return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

// アクターに指定タグがなければ追加する
void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	if (!ASC -> HasMatchingGameplayTag(TagToAdd))
	{
		ASC -> AddLooseGameplayTag(TagToAdd);
	}
}

// アクターに指定タグがあれば削除する
void UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	if (ASC -> HasMatchingGameplayTag(TagToRemove))
	{
		ASC -> RemoveLooseGameplayTag(TagToRemove);
	}
}

// アクターが指定タグを持っているかを bool で返す
bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	return ASC->HasMatchingGameplayTag(TagToCheck);
}

// アクターが指定タグを持っているかを Yes/No で返す（Blueprint用）
void UWarriorFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EWarriorConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? EWarriorConfirmType::Yes : EWarriorConfirmType::No;
}

// Actorから戦闘コンポーネントを取得する
UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);

	if (IPawnCombatInterface* PawnCombatInterface = Cast< IPawnCombatInterface >(InActor))
	{
		return PawnCombatInterface -> GetPawnCombatComponent();
	}

	return nullptr;
}

// Actorから戦闘コンポーネントを取得し結果をEnumで返す
UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor, EWarriorValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

	OutValidType = CombatComponent? EWarriorValidType::Valid : EWarriorValidType::Invalid;

	return CombatComponent;
}

// 敵対関係にあるかどうかを判定する
bool UWarriorFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	// どちらかが null ならゲームとして成立しないためクラッシュさせて気づけるようにする
	check(QueryPawn && TargetPawn);

	// Pawn の Controller が IGenericTeamAgentInterface を実装しているかチェック
	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn -> GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn -> GetController());

	// 両方がチーム情報を持っている場合のみ比較する
	if (QueryTeamAgent && TargetTeamAgent)
	{
		// チームIDが異なれば敵対関係とみなす
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent -> GetGenericTeamId();
	}

	// どちらかがチーム情報を持っていない場合は敵対とみなさない
	return false;
}

// 指定したレベルでの最終的な値を返す
float UWarriorFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}

// 攻撃者と被弾者の位置関係から、被弾方向の角度を計算する
FGameplayTag UWarriorFunctionLibrary::ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference)
{
	check(InAttacker && InVictim);

	// 被弾者の前方向ベクトル
	const FVector VictimForward = InVictim->GetActorForwardVector();
	// 被弾者から攻撃者 の方向ベクトル
	const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

	// 内積から角度差（0〜180°）を求める
	const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
	OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

	// 外積で左右どちら側から攻撃されたかを判定
	const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);

	// Z が負なら左側から角度を負にする
	if (CrossResult.Z < 0.f)
	{
		OutAngleDifference *= -1.f;
	}

	if (OutAngleDifference >= -45.f && OutAngleDifference <= 45.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Front;
	}
	else if (OutAngleDifference < -45.f && OutAngleDifference >= -135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Left;
	}
	else if (OutAngleDifference < -135.f || OutAngleDifference > 135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Back;
	}
	else if (OutAngleDifference > 45.f && OutAngleDifference <= 135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Right;
	}

	return WarriorGameplayTags::Shared_Status_HitReact_Front;
}

// 防御者が攻撃方向に対して正しく向いている場合のみブロック成功とする
bool UWarriorFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker && InDefender);

	const float DotResult = FVector::DotProduct(InAttacker->GetActorForwardVector(), InDefender->GetActorForwardVector());

	/*const FString DebugString = FString::Printf(TEXT("Dot Result: %f %s"), DotResult, DotResult < -0.1f ? TEXT("Valid Block") : TEXT("InvalidBlock"));

	Debug::Print(DebugString, DotResult < -0.1f ? FColor::Green : FColor::Red);*/

	return DotResult < -0.1f;
}

// GameplayEffectSpecHandle をターゲットに適用するためのユーティリティ関数
bool UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
	// 発射した側（Instigator）の AbilitySystemComponent を取得
	UWarriorAbilitySystemComponent* SourceASC = NativeGetWarriorASCFromActor(InInstigator);
	// ダメージを受ける側（Target）の AbilitySystemComponent を取得
	UWarriorAbilitySystemComponent* TargetASC = NativeGetWarriorASCFromActor(InTargetActor);

	// GameplayEffectSpec をターゲットに適用
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);

	// 成功したかどうかを返す
	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

// Blueprint から呼び出せるカウントダウン処理
void UWarriorFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval, float& OutRemainingTime, EWarriorCountDownActionInput CountDownInput, UPARAM(DisplayName = "Output") EWarriorCountDownActionOutput& CountDownOutput, FLatentActionInfo LatentInfo)
{
	UWorld* World = nullptr;

	// WorldContextObject から UWorld を取得
	if (GEngine)
	{
		World = GEngine -> GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}

	if (!World)
	{
		return;     // ワールドが無ければ何もできない
	}

	// ワールドに紐づく Latent Action Manager を取得
	FLatentActionManager& LatentActionManager = World -> GetLatentActionManager();

	// すでに同じ UUID のカウントダウンアクションが存在するか確認
	FWarriorCountDownAction* FoundAction = LatentActionManager.FindExistingAction<FWarriorCountDownAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);

	// --- カウントダウン開始 ---
	if (CountDownInput == EWarriorCountDownActionInput::Start)
	{
		// 既に同じアクションが存在しない場合のみ新規作成
		if (!FoundAction)
		{
			LatentActionManager.AddNewAction(
				LatentInfo.CallbackTarget,
				LatentInfo.UUID,
				new FWarriorCountDownAction(TotalTime, UpdateInterval, OutRemainingTime, CountDownOutput, LatentInfo)
			);
		}
	}

	// --- カウントダウンキャンセル ---
	if (CountDownInput == EWarriorCountDownActionInput::Cancel)
	{
		if (FoundAction)
		{
			// 次の Tick でキャンセルされるようフラグを立てる
			FoundAction -> CancelAction();
		}
	}
}

// Blueprint から WarriorGameInstance を取得するための純粋関数
UWarriorGameInstance* UWarriorFunctionLibrary::GetWarriorGameInstance(const UObject* WorldContextObject)
{
	// GEngine が有効か確認
	if (GEngine)
	{
		// WorldContextObject から UWorld を取得
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			// World から GameInstance を取得し、指定した型にキャストして返す
			return World->GetGameInstance<UWarriorGameInstance>();
		}
	}

	// 取得できなかった場合は nullptr
	return nullptr;
}

// Blueprint から入力モード（GameOnly / UIOnly）を切り替えるための関数
void UWarriorFunctionLibrary::ToggleInputMode(const UObject* WorldContextObject, EWarriorInputMode InInputMode)
{
	APlayerController* PlayerController = nullptr;

	// WorldContextObject から World を取得し、そこから PlayerController を取得
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			PlayerController = World->GetFirstPlayerController();
		}
	}

	// PlayerController が取得できなければ何もせず終了
	if (!PlayerController)
	{
		return;
	}

	// UE 標準の入力モード構造体
	FInputModeGameOnly GameOnlyMode;
	FInputModeUIOnly UIOnlyMode;

	// 指定された入力モードに応じて切り替え
	switch (InInputMode)
	{
	case EWarriorInputMode::GameOnly:

		// ゲーム操作のみ受け付ける
		PlayerController->SetInputMode(GameOnlyMode);
		PlayerController->bShowMouseCursor = false;

		break;

	case EWarriorInputMode::UIOnly:

		// UI 操作のみ受け付ける（マウスカーソル表示）
		PlayerController->SetInputMode(UIOnlyMode);
		PlayerController->bShowMouseCursor = true;

		break;

	default:
		break;
	}
}

void UWarriorFunctionLibrary::SaveCurrentGameDifficulty(EWarriorGameDifficulty InDifficultyToSave)
{
	// SaveGame オブジェクトを作成
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(UWarriorSaveGame::StaticClass());

	// 正しくキャストできた場合のみ処理
	if (UWarriorSaveGame* WarriorSaveGameObject = Cast<UWarriorSaveGame>(SaveGameObject))
	{
		// 難易度をセーブデータに書き込む
		WarriorSaveGameObject->SavedCurrentGameDifficulty = InDifficultyToSave;

		// 指定スロットに保存
		const bool bWasSaved = UGameplayStatics::SaveGameToSlot(WarriorSaveGameObject, WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);

		//Debug::Print(bWasSaved ? TEXT("Difficulty Saved") : TEXT("Difficulty NOT Saved"));
	}
}

bool UWarriorFunctionLibrary::TryLoadSavedGameDifficulty(EWarriorGameDifficulty& OutSavedDifficulty)
{
	// セーブデータが存在するか確認
	if (UGameplayStatics::DoesSaveGameExist(WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0))
	{
		// セーブデータをロード
		USaveGame* SaveGameObject = UGameplayStatics::LoadGameFromSlot(WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);

		// 正しくキャストできた場合のみ処理
		if (UWarriorSaveGame* WarriorSaveGameObject = Cast<UWarriorSaveGame>(SaveGameObject))
		{
			// 保存されていた難易度を出力引数にセット
			OutSavedDifficulty = WarriorSaveGameObject->SavedCurrentGameDifficulty;

			//Debug::Print(TEXT("Loading Successful"), FColor::Green);

			return true;
		}
	}

	return false;
}

