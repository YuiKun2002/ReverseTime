// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/UI/UI_PlayerBagEquipmentGrid.h"

void UUI_PlayerBagEquipmentGrid::ShowItemTip_Implementation() {
	UUI_EquipmentGrid::ShowEquipmentGrid(this, false);
}

void UUI_PlayerBagEquipmentGrid::SetItemAttribute(UItemBaseView* View, const FItemViewBlock& BlockData, int32 Index) {

	//设置按钮禁用
	if (Index < Cast<UPlayerBagEquipView>(View)->GetbEquipItemCount()) {

		this->SetButtonColorIsDark();
		this->OnLockItem();
	}
	else {
		this->SetButtonColorIsDark(false);
		this->OnUnLockItem();
	}

	//得到当前需要显示的数据
	FItemCountViewBlock* Block = CAST_TYPE_PTR(
		FItemCountViewBlock,
		View->GetItem(BlockData.ItemID, BlockData.ItemLevel)
	);

	//设置数据视图块
	this->SetItemCountViewBlock(&BlockData, Block, Index);

	this->OnViewUpdate();
}