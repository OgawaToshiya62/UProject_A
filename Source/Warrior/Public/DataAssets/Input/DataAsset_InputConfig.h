// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DataAsset_InputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FWarriorInputActionConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))     // EditDefaultsOnly:Blueprintで編集可能だがデフォルト値のみ BlueprintReadOnly:Blueprintから読み取りはできるが書き換えは出来ない meta=(Categories="InputTag"):エディタ上でタグの候補を絞るための分類
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAction;     // UInputAction EnhancedInputSystemの入力アクション 実際の入力イベント(キーやスティック操作など)を定義したアセットが入る

	// 入力タグと入力アクションが両方有効なら true を返す（設定が正しいかの判定）
	bool IsValid() const
	{
		return InputTag.IsValid() && InputAction;
	}
};
/**
 * 
 */
UCLASS()
class WARRIOR_API UDataAsset_InputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputMappingContext* DefaultMappingContext;                                            // UInputMappingContext どのキーやボタンがどのアクションに対応するかを定義するアセット ここではデフォルト入力マッピングを指定しているためゲーム開始時に使われる入力設定

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))     // meta=(TitleProperty="InputTag") エディタ上で一覧表示されたときに InputTag がタイトルとして表示
	TArray<FWarriorInputActionConfig> NataiveInputActions;                                  // TArray　で複数の入力アクションを一覧で管理可能 FWarriorInputActionConfig 入力アクションとタグを紐づけた構造体(前に定義した InputTag + InputAction)

	UInputAction* FindNativeInputActionByTag(const FGameplayTag& InInputTag) const;         // 指定されたタグに対応するUInputActionを検索して返す

	// アビリティ用の入力アクション一覧（タグとアクションの組み合わせを管理）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FWarriorInputActionConfig> AbilityInputActions;
};
