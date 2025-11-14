// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "WarriorHeroCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UDataAsset_InputConfig;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorHeroCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()
	
public:
	AWarriorHeroCharacter();

protected:     // 親クラスと子クラスからアクセス可能にする
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;     // プレイヤーの入力(キーバードなど)をキャラクターにバインドする関数 Overrideは親クラスで定義された関数を上書きして使うと言った意味
	virtual void BeginPlay() override;                                                                // ゲーム開始時(レベルにスポーンした瞬間)に呼ばれる初期化関数

private:
	// #pragma region はVisualStudio上でコードを折りたたみ見やすくするため
#pragma region Components

	// スプリングアーム (VisibleAnywhere エディタで見えるが変更不可、BlueprintReadOnly Blueprintから読み取り可能　AllowPrivateAccess="true" Blueprintでアクセス出来るようにするためのメタ設定)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// 追従カメラ 実際にプレイヤーが見るカメラ CameraBoomにアタッチされてキャラクターの後ろから追従し視点を提供
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

#pragma endregion

#pragma region Inputs

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))     // EditDefaultsOnly BPでクラスのデフォルト値のみ編集可能 BlueprintReadOnly BPから読み取りができるが書き換え不可 AllowPrivateAccess BPからpribateでもみえるようにする補足設定
	UDataAsset_InputConfig* InputConfigUDataAsset;                                                                       // UDataAsset_InputConfig UDataAssetを継承したクラス InputConfigUDataAssetは変数名

	void Input_Move(const FInputActionValue& InputActionValue);                                                          // プレイヤーの移動(Move)入力を処理する関数の宣言
	void Input_Look(const FInputActionValue& InputActionValue);                                                          // プレイヤーの視点(Look)入力を処理する関数の宣言

#pragma endregion


};
