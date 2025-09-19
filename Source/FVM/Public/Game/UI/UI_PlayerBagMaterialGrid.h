// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/UI/PlayerBagItemGrid.h"
#include "UI_PlayerBagMaterialGrid.generated.h"

/**
 *材料格子
 */
UCLASS()
class FVM_API UUI_PlayerBagMaterialGrid : public UPlayerBagItemGrid
{
	GENERATED_BODY()

public:

	//显示物品提示
	virtual	void ShowItemTip_Implementation() override;

	//设置装备格子显示
	UFUNCTION(BlueprintCallable)
	static void ShowMaterialGrid(class UPlayerBagItemGrid* ItemGridIns, bool bPreview = true);
};
