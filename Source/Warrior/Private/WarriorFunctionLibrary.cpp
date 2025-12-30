// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Interface/PawnCombatInterface.h"
#include "GenericTeamAgentInterface.h"

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
void UWarriorFunctionLibrary::RemoveGameplayFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
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
