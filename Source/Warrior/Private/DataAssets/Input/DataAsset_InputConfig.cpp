// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/Input/DataAsset_InputConfig.h"

UInputAction* UDataAsset_InputConfig::FindNativeInputActionByTag(const FGameplayTag& InInputTag) const
{
	for (const FWarriorInputActionConfig& InputActionConfig : NataiveInputActions)         // NataiveInputActions タグとアクションのペアを格納した配列 この中を1つずつ調べる
	{
		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)     // タグが一致していてInputActionがnullptrでないなら
		{
			return InputActionConfig.InputAction;                                          // 一致したアクションを見つけたら、それを返して終了
		}
	}

	return nullptr;                                                                        // 一致するタグが無ければnullptrを返す
}
