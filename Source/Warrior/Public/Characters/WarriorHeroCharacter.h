// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "WarriorHeroCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorHeroCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()
	
public:
	AWarriorHeroCharacter();

protected:
	virtual void BeginPlay() override;

private:
#pragma region Components

	// スプリングアーム (VisibleAnywhere エディタで見えるが変更不可、BlueprintReadOnly Blueprintから読み取り可能　AllowPrivateAccess="true" Blueprintでアクセス出来るようにするためのメタ設定)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// 追従カメラ 実際にプレイヤーが見るカメラ CameraBoomにアタッチされてキャラクターの後ろから追従し視点を提供
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
#pragma endregion

};
