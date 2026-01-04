// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Interface/PawnCombatInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "WarriorGameplayTags.h"

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
