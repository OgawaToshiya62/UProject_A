// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "WarriorHeroCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UDataAsset_InputConfig;
struct FInputActionValue;
class UHeroCombatComponent;
class UHeroUIComponent;

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorHeroCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()
	
public:
	AWarriorHeroCharacter();

	//~ Begin IPawnCombatInterface Interface.
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;     // この関数をオーバーライドすることで、Pawn が持つ戦闘コンポーネントを返せるようになる
	//~ End IPawnCombatInterface Interface

	//~ Begin IPawnUIInterface Interface.
	virtual UPawnUIComponent* GetPawnUIComponent() const override;     // IPawnUIInterface の実装: この Pawn が持つ UI コンポーネントを返す
	virtual UHeroUIComponent* GetHeroUIComponent() const override;     // IPawnUIInterface の実装: プレイヤー専用のUIコンポーネント(HeroUIComponent)を返す
	//~ End IPawnUIInterface Interface

protected:     // 親クラスと子クラスからアクセス可能にする
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;     // キャラクターが Controller に所有されたときに呼ばれる処理（能力システムの初期化などに使用）
	//~ End APawn Interface

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

	// キャラクターの戦闘処理をするコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UHeroCombatComponent* HeroCombatComponent;

	// キャラクターのUI処理を担当するコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UHeroUIComponent* HeroUIComponent;

#pragma endregion

#pragma region Inputs

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))     // EditDefaultsOnly BPでクラスのデフォルト値のみ編集可能 BlueprintReadOnly BPから読み取りができるが書き換え不可 AllowPrivateAccess BPからpribateでもみえるようにする補足設定
	UDataAsset_InputConfig* InputConfigUDataAsset;                                                                       // UDataAsset_InputConfig UDataAssetを継承したクラス InputConfigUDataAssetは変数名

	// ターゲット切り替え入力の方向（左右）を保持するための変数
	UPROPERTY()
	FVector2D SwitchDirection = FVector2D::ZeroVector;

	void Input_Move(const FInputActionValue& InputActionValue);                                                          // プレイヤーの移動(Move)入力を処理する関数の宣言
	void Input_Look(const FInputActionValue& InputActionValue);                                                          // プレイヤーの視点(Look)入力を処理する関数の宣言

	void Input_SwitchTargetTriggered(const FInputActionValue& InputActionValue);     // ターゲット切り替えボタンが押されたときに呼ばれる処理
	void Input_SwitchTargetCompleted(const FInputActionValue& InputActionValue);     // ターゲット切り替えボタンが離されたときに呼ばれる処理

	void Input_PickUpStonesStarted(const FInputActionValue& InputActionValue);     // 「拾う」入力が押されたときに呼ばれる関数

	// プレイヤーがキーを押したときに呼ばれる関数の宣言 入力タグに対応するアビリティの開始を通知する
	void Input_AbilityInputPressed(FGameplayTag InInputTag);
	// プレイヤーがキーを離したときに呼ばれる関数の宣言 入力タグに対応するアビリティの終了を通知する
	void Input_AbilityInputReleased(FGameplayTag InInputTag);

#pragma endregion

public:
	// 戦闘コンポーネントを取得するためのゲッター関数
	FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const { return HeroCombatComponent; }
};
