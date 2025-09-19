// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorAssetBPLibrary.h"
#include "EditorAsset.h"
#include "Subsystems/AssetEditorSubsystem.h"

UEditorAssetBPLibrary::UEditorAssetBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UEditorAssetBPLibrary::OpenAsset(UObject* Asset) {
#if WITH_EDITOR
	if (GEditor)
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Asset);
	}
#endif
}
