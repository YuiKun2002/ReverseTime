// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/UI_EquipmentGrid.h"
#include <Components/Image.h>
#include <Components/Button.h>
#include <Components/TextBlock.h>
#include "Game/UI/Tips/UI_ItemDesTip.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"
#include "Data/Texture/TextureDataAssetCache.h"
#include "GameSystem/Item/Equipment/Func/EquipmentFuncGem.h"
#include "GameSystem/Item/Equipment/Func/EquipmentFuncBag.h"
#include "GameSystem/Item/Equipment/Func/EquipmentFuncGift.h"
#include "GameSystem/Item/Equipment/Func/EquipmentFuncWeapon.h"
#include "GameSystem/Item/Equipment/Func/EquipmentFuncEquipment.h"

FString UUI_EquipmentGrid::ToString_Implementation() {

#if WITH_EDITOR

	return this->CurItemContViewBlock.Name +
		FString(TEXT("\nEquip ID ：")) +
		FString::FromInt(this->GetItemViewBlock().ItemID) +
		FString(TEXT("\nUI Index ：")) +
		FString::FromInt(this->GetUiIndex()) +
		FString(TEXT("\nBlock Index ：")) +
		FString::FromInt(this->GetItemViewBlock().BlockIndex) +
		FString(TEXT("\nItem Count ：")) +
		FString::FromInt(this->CurItemContViewBlock.Count);
#endif

	return this->CurItemContViewBlock.Name;
}

void UUI_EquipmentGrid::ShowItemTip_Implementation() {

	UUI_EquipmentGrid::ShowEquipmentGrid(this, true);

}

void UUI_EquipmentGrid::ShowEquipmentGrid(class UPlayerBagItemGrid* ItemGridIns, bool bPreview) {

	uint8 Type = 0u;
	FItemBaseStructData* DataStruct;
	if (GET_ITEM(ItemGridIns->GetItemViewBlock().ItemID, Type, DataStruct, GET_DEF_CATEGORYNAME(Equip)))
	{
		if (ItemGridIns->GetButtonColorIsDarkColor())
		{
			ItemGridIns->SetFuncObject(NewObject<UItemBaseGridFunc>(), DataStruct);
		}
		else {
			switch ((EEquipment)(Type))
			{
			case EEquipment::E_Bag:
			{
				//有专属的UI特化背包的使用功能
				//此处仅展示道具的信息
				UEquipmentFuncBag* Bag = NewObject<UEquipmentFuncBag>();
				Bag->SetPreview();
				ItemGridIns->SetFuncObject(Bag, DataStruct);
			}
			break;
			case EEquipment::E_Gift:
			{
				UEquipmentFuncGift* Gift = NewObject<UEquipmentFuncGift>();
				ItemGridIns->SetFuncObject(Gift, DataStruct);
				if (bPreview)
				{
					Gift->GetTipWidget()->SetHidden();
				}

			}
			break;
			case EEquipment::E_PlayerEquipment:
			{
				UEquipmentFuncEquipment* Equip = NewObject<UEquipmentFuncEquipment>();
				ItemGridIns->SetFuncObject(Equip, DataStruct);
				if (bPreview)
				{
					Equip->GetTipWidget()->SetHidden();
				}

			}break;
			case EEquipment::E_PlayerWeaponFirst:
			case EEquipment::E_PlayerWeaponSecond:
			case EEquipment::E_PlayerWeaponSuper:
			{
				UEquipmentFuncWeapon* Weapon = NewObject<UEquipmentFuncWeapon>();
				Weapon->SetType(Type);
				ItemGridIns->SetFuncObject(Weapon, DataStruct);
				if (bPreview)
				{
					Weapon->GetTipWidget()->SetHidden();
				}
			}
			break;
			case EEquipment::E_WeaponGem:
			{
				UEquipmentFuncGem* Gem = NewObject<UEquipmentFuncGem>();
				Gem->SetOperateType(0);
				ItemGridIns->SetFuncObject(Gem, DataStruct);
				if (bPreview)
				{
					Gem->GetTipWidget()->SetHidden();
				}

			}
			break;
			default: {
				ItemGridIns->SetFuncObject(NewObject<UItemBaseGridFunc>(), DataStruct);
			}break;
			}
		}
	}
}

void UUI_EquipmentGrid::SetItemCount(int32 NewCount) {

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