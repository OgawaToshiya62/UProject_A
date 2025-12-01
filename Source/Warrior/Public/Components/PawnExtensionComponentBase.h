// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WARRIOR_API UPawnExtensionComponentBase : public UActorComponent
{
	GENERATED_BODY()

protected:
	// このオブジェクトを所有する Pawn を指定型 T として取得（T は APawn 派生型である必要がある）
	template <class T>
	T* GetOwningPawn() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "'T' Template Parameter to GetPawn must be derived from APawn");
		return CastChecked<T>(GetOwner());
	}

	// このオブジェクトを所有する Pawn を取得
	APawn* GetOwningPawn() const
	{
		return GetOwningPawn<APawn>();
	}

	// このオブジェクトを所有する Pawn の Controller を指定型 T として取得（T は AController 派生型である必要がある）
	template <class T>
	T* GetOwningController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AController>::Value, "'T' Template Parameter to GetController must be derived from AController");
		return GetOwningPawn<APawn>()->GetController<T>();
	}
};
