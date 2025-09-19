// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Synthesis/UI_SynItemGrid.h"
#include <Components/TextBlock.h>
#include "Data/MaterialDataStruct.h"
#include "Game/UI/UI_PlayerSynthesis.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/Synthesis/SynModel_MakeCard.h"
#include "Game/UI/Synthesis/SynModel_CardUpgrade.h"
#include "Game/UI/Synthesis/SynModel_GemsUpGrade.h"
#include "GameSystem/Item/Equipment/Func/EquipmentFuncGem.h"
#include "GameSystem/Item/Equipment/Func/EquipmentFuncWeapon.h"

FString UUI_SynItemGrid::ToString_Implementation() {
	if (this->GetItemViewBlock().IsValid())
	{
#if WITH_EDITOR
		switch (this->InitType)
		{
		case 1: {
			return GameDataStaticObject<
				UPlayerBagMaterialView
			>()->GetItemName(this->GetItemViewBlock().ItemID) +
				FString(TEXT("\nItem ID ：")) +
				FString::FromInt(this->GetItemViewBlock().ItemID) +
				FString(TEXT("\nUI Index ：")) +
				FString::FromInt(this->GetUiIndex()) +
				FString(TEXT("\nBlock Index ：")) +
				FString::FromInt(this->GetItemViewBlock().BlockIndex);
		}break;

		default: {
			return GameDataStaticObject<
				UPlayerBagEquipView
			>()->GetItemName(this->GetItemViewBlock().ItemID) +
				FString(TEXT("\nItem ID ：")) +
				FString::FromInt(this->GetItemViewBlock().ItemID) +
				FString(TEXT("\nUI Index ：")) +
				FString::FromInt(this->GetUiIndex()) +
				FString(TEXT("\nBlock Index ：")) +
				FString::FromInt(this->GetItemViewBlock().BlockIndex);
		}break;
		}

#endif
		switch (this->InitType)
		{
		case 1: {
			return GameDataStaticObject<
				UPlayerBagMaterialView
			>()->GetItemName(this->GetItemViewBlock().ItemID);
		}break;

		default: {
			return GameDataStaticObject<
				UPlayerBagEquipView
			>()->GetItemName(this->GetItemViewBlock().ItemID);
		}break;
		}
	}

	return FString("");
}

void UUI_SynItemGrid::OnClicked_Implementation() {
	switch (this->InitType)
	{
	case 1: {
		this->OperMater();
	}break;

	default: {
		this->OperEquip();
	}break;
	}
}

bool UUI_SynItemGrid::SetItemCountViewBlock(
	const FItemViewBlock* ItemViewBlock,
	const FItemCountViewBlock* Data,
	int32 Index
) {

	if (!Super::SetItemCountViewBlock(ItemViewBlock, Data, Index))
	{
		return false;
	}

	if (!this->GetButtonIsEnabled())
	{
		this->SetButtonIsEnabled(true);
	}

	return true;
}

void UUI_SynItemGrid::SetItemCount(int32 NewCount) {

	if (this->InitType == 0)
	{
		UPlayerStructManager* Player = GET_PLAYERDATAINS();

		const FItemViewBlock& Blcok = this->GetItemViewBlock();

		//获取当前装备的数据存储数量-装备数量-初始化数据的数量
		bool bEquipCount = false;
		bool NotbEquipCount = false;

		Player->FindbEquipItem(Blcok.ItemID,
			Blcok.ItemLevel, Blcok.BlockIndex, bEquipCount, NotbEquipCount);

		if (bEquipCount || NotbEquipCount)
		{
			//如果当前装备物品，已经初始化数据了，默认数量为1
			this->CurItemContViewBlock.Count = 1;

			this->SetItemCountTextShow(false);

		}
		else {

			//新的物品数量
			int32 NewItemCount = NewCount;

			FEquipItemSave* bEquipSave = Player->GetEquipItem(Blcok.ItemID);
			if (bEquipSave)
			{
				NewItemCount -= bEquipSave->GetItemCount(Blcok.ItemLevel);
			}

			FEquipItemSave* NotbEquipSave = Player->GetEquipItem(Blcok.ItemID, false);
			if (NotbEquipSave)
			{
				NewItemCount -= NotbEquipSave->GetItemCount(Blcok.ItemLevel);
			}

			//修改当前格子显示数量
			if (this->CurItemContViewBlock.Count != NewItemCount)
			{
				this->CurItemContViewBlock.Count = NewItemCount;

				if (NewItemCount < 2)
				{
					this->SetItemCountTextShow(false);
				}
				else {
					//设置数量显示
					if (NewItemCount > 9999)
					{
						//设置数量
						this->Count_Text->SetText(
							FText::FromString(TEXT("x9999+"))
						);
					}
					else {
						//设置数量
						this->Count_Text->SetText(
							FText::FromString(TEXT("x") +
								FString::FromInt(NewItemCount)
							)
						);
					}

					this->SetItemCountTextShow(true);
				}
			}
		}
	}
	else {
		Super::SetItemCount(NewCount);
	}
}


void UUI_SynItemGrid::OperEquip() {

	if (this->OpenEquip_WG_WeaponAndGem()) {}
	else if (this->OpenEquip_WG_UpgradeGem()) {}
	else if (this->OpenEquip_SG_SplitGem()) {}


	else {
		uint8 Type;
		FName Tag = GET_DEF_CATEGORYNAME(Equip);
		FItemBaseStructData* BasePtr = nullptr;
		if (GET_ITEM(this->GetItemViewBlock().ItemID, Type, BasePtr, Tag))
		{
			this->SetFuncObject(NewObject<UItemBaseGridFunc>(), BasePtr);
		}
	}
}

bool UUI_SynItemGrid::OpenEquip_WG_WeaponAndGem() {

	bool bRe = this->InitOperType.IsEqual(GET_UI_NAME(Synthesis, WeaponAndGem));
	uint8 Ty = this->GetItemCountViewBlock().Type;
	//主武器、副武器、超级武器
	if ((Ty == 3U || Ty == 4U || Ty == 5U) && bRe)
	{
		FItemBaseStructData* DataStruct = nullptr;
		if (GET_ITEM(this->GetItemViewBlock().ItemID, Ty, DataStruct, GET_DEF_CATEGORYNAME(Equip)))
		{
			UEquipmentFuncWeapon* Weapon = NewObject<UEquipmentFuncWeapon>();
			Weapon->SetType(Ty);
			Weapon->SetOperateType(1);
			this->SetFuncObject(Weapon, DataStruct);
		}
		else {
			//查询失败
			return false;
		}
	}
	else if (Ty == 6U && bRe)//宝石
	{
		FItemBaseStructData* DataStruct = nullptr;
		if (GET_ITEM(this->GetItemViewBlock().ItemID, Ty, DataStruct, GET_DEF_CATEGORYNAME(Equip)))
		{
			UEquipmentFuncGem* Gem = NewObject<UEquipmentFuncGem>();
			Gem->SetOperateType(1);
			this->SetFuncObject(Gem, DataStruct);
		}
		else {
			//查询失败
			return false;
		}
	}
	else {
		return false;
	}

	return true;
}

bool UUI_SynItemGrid::OpenEquip_WG_UpgradeGem() {
	bool bRe = this->InitOperType.IsEqual(GET_UI_NAME(Synthesis, UpgradeGem));
	uint8 Ty = this->GetItemCountViewBlock().Type;
	//主武器、副武器、超级武器
	if ((Ty == 3U || Ty == 4U || Ty == 5U) && bRe)
	{
		FItemBaseStructData* DataStruct = nullptr;
		if (GET_ITEM(this->GetItemViewBlock().ItemID, Ty, DataStruct, GET_DEF_CATEGORYNAME(Equip)))
		{
			UEquipmentFuncWeapon* Weapon = NewObject<UEquipmentFuncWeapon>();
			Weapon->SetType(Ty);
			Weapon->SetOperateType(2);
			this->SetFuncObject(Weapon, DataStruct);
		}
		else {
			//查询失败
			return false;
		}
	}
	else if (Ty == 6U && bRe)//宝石
	{
		FItemBaseStructData* DataStruct = nullptr;
		if (GET_ITEM(this->GetItemViewBlock().ItemID, Ty, DataStruct, GET_DEF_CATEGORYNAME(Equip)))
		{
			UEquipmentFuncGem* Gem = NewObject<UEquipmentFuncGem>();
			Gem->SetOperateType(2);
			this->SetFuncObject(Gem, DataStruct);
		}
		else {
			//查询失败
			return false;
		}
	}
	else {
		return false;
	}

	return true;
}

bool UUI_SynItemGrid::OpenEquip_SG_SplitGem() {
	bool bRe = this->InitOperType.IsEqual(GET_UI_NAME(Synthesis, SplitGem));
	uint8 Ty = this->GetItemCountViewBlock().Type;
	//宝石分解
	if (Ty == 6U && bRe)
	{
		FItemBaseStructData* DataStruct = nullptr;
		if (GET_ITEM(this->GetItemViewBlock().ItemID, Ty, DataStruct, GET_DEF_CATEGORYNAME(Equip)))
		{
			UEquipmentFuncGem* Gem = NewObject<UEquipmentFuncGem>();
			Gem->SetOperateType(3);
			this->SetFuncObject(Gem, DataStruct);

			return true;
		}
	}

	return false;
}

void UUI_SynItemGrid::OperMater() {

	if (this->OperMater_BM_MakeCard()) { this->SetButtonIsEnabled(false); }
	else if (this->OperMater_SP_MakeCard()) { this->SetButtonIsEnabled(false); }
	else if (this->OperMater_Clover_UpgradeCard()) { this->SetButtonIsEnabled(false); }
	else if (this->OperMater_Clover_UpgradeGem()) { this->SetButtonIsEnabled(false); }

	else {
		uint8 Type;
		FName Tag = GET_DEF_CATEGORYNAME(Material);
		FItemBaseStructData* BasePtr = nullptr;
		if (GET_ITEM(this->GetItemViewBlock().ItemID, Type, BasePtr, Tag))
		{
			this->SetFuncObject(NewObject<UItemBaseGridFunc>(), BasePtr);
		}
	}
}

bool UUI_SynItemGrid::OperMater_BM_MakeCard() {
	//卡片制作-合成配方
	if (
		this->GetItemCountViewBlock().Type == 0U
		&&
		this->InitOperType.IsEqual(GET_UI_NAME(Synthesis, MakeCard)))
	{
		//获取道具 【获取合成屋的UI】
		//为什么要这样获取操作界面，而不是直接GET_UI?操作界面被合成界面管理，直接获取和合成管理获取的界面是不同对象
		UUI_SynModel_MakeCard* Grid = Cast<UUI_SynModel_MakeCard>(Cast<UUI_PlayerSynthesis>(
			this->GetParentClass()
		)->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, MakeCard)));
		Grid->SetBlueprintMaterialID(this);
		return true;
	}
	return false;
}

bool UUI_SynItemGrid::OperMater_SP_MakeCard() {
	//卡片制作-香料
	if (
		this->GetItemCountViewBlock().Type == 3U
		&&
		this->InitOperType.IsEqual(GET_UI_NAME(Synthesis, MakeCard)))
	{

		//获取数量
		if (this->GetItemCountViewBlock().Count < 5)
		{

			UWidgetBase::CreateNotUnlockedItemTipWidget(
				2, this->GetItemID(), 0,
				FText::FromString(TEXT("香料不足，香料的数量至少为5个"))
			);

			return true;
		}

		//获取道具
		UUI_SynModel_MakeCard* Grid = Cast<UUI_SynModel_MakeCard>(Cast<UUI_PlayerSynthesis>(
			this->GetParentClass()
		)->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, MakeCard)));
		Grid->SetSpiceMaterialID(this);

		return true;
	}
	return false;
}

bool UUI_SynItemGrid::OperMater_Clover_UpgradeCard() {

	if (
		this->GetItemCountViewBlock().Type == 4U
		&&
		this->InitOperType.IsEqual(GET_UI_NAME(Synthesis, UpgradeCard)))
	{
		EMaterialType Type;
		FMaterialBase* BasePtr = nullptr;
		if (GET_CACHE(Material)->GetMaterData(this->GetItemViewBlock().ItemID, Type, BasePtr))
		{
			//获取道具
			UUI_SynModel_UpgradeCard* Grid = Cast<UUI_SynModel_UpgradeCard>(Cast<UUI_PlayerSynthesis>(
				this->GetParentClass()
			)->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, UpgradeCard)));
			Grid->SetClover(this, CAST_TYPE(FCloverMaterial, BasePtr).M_UpGrateRate);

			return true;
		}
	}

	return false;
}

bool UUI_SynItemGrid::OperMater_Clover_UpgradeGem() {
	if (
		this->GetItemCountViewBlock().Type == 4U
		&&
		this->InitOperType.IsEqual(GET_UI_NAME(Synthesis, UpgradeGem)))
	{
		EMaterialType Type;
		FMaterialBase* BasePtr = nullptr;
		if (GET_CACHE(Material)->GetMaterData(this->GetItemViewBlock().ItemID, Type, BasePtr))
		{
			//获取道具
			USynModel_GemsUpGrade* Grid = Cast<USynModel_GemsUpGrade>(Cast<UUI_PlayerSynthesis>(
				this->GetParentClass()
			)->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, UpgradeGem)));
			Grid->SetRate(this, CAST_TYPE(FCloverMaterial, BasePtr).M_UpGrateRate);

			return true;
		}
	}
	return false;
}