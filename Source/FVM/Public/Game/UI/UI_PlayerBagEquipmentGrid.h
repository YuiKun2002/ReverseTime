// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/UI/UI_EquipmentGrid.h"
#include "GameSystem/PlayerStructManager.h"
#include "UI_PlayerBagEquipmentGrid.generated.h"

/**
 *玩家装备物品格子
 */
UCLASS()
class FVM_API UUI_PlayerBagEquipmentGrid : public UUI_EquipmentGrid
{
	GENERATED_BODY()

public:

	virtual	void ShowItemTip_Implementation() override;

	virtual void SetItemAttribute(
		class UItemBaseView* View,
		const FItemViewBlock& BlockData,
		int32 Index
	) override;


	//锁定物品
	UFUNCTION(BlueprintImplementableEvent)
	void OnLockItem();

	//解锁物品
	UFUNCTION(BlueprintImplementableEvent)
	void OnUnLockItem();
};
