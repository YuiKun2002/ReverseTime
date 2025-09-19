// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "EditorAssetBPLibrary.generated.h"

//编辑器资产库
UCLASS()
class UEditorAssetBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	//仅在编辑器模式下使用-打包后无效
	UFUNCTION(BlueprintCallable, Category = "打开资产")
	static void OpenAsset(UObject* Asset);
};
