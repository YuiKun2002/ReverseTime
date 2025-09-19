#include "GameSystem/Item/Equipment/Func/EquipmentFuncEquipment.h"
#include "GameSystem/GameUserInterfaceSubsystem.h"
#include "Game/UI/UI_PlayerBagEquipmentGrid.h"
#include "GameSystem/Item/ItemBaseStruct.h"
#include "Game/UI/Tips/UI_ItemDesTip.h"
#include "Game/UI/UI_PlayerBag.h"

void UEquipmentFuncEquipment::Execute(
	UItemGridBase* EquipGridPtr,
	FItemBaseStructData* ItemBasePtr
) {

	Super::Execute(EquipGridPtr, ItemBasePtr);
	
	this->EquipGrid = Cast<UUI_EquipmentGrid>(EquipGridPtr);

	//转换成服装数据
	this->EquipData = CAST_TYPE(FEquipment_PlayerEquipment_Data, ItemBasePtr);

	this->GetTipWidget()->SetHidden(false);

	this->GetTipWidget()->SetOkButtonTitleText(TEXT("装备"));

	this->GetTipWidget()->BindClickFunc(this, TEXT("Equip"));
};

void UEquipmentFuncEquipment::Equip() {

	UPlayerStructManager* Player = GET_PLAYERDATAINS();

	//是否可以使用
	bool bUse = true;

	if (this->EquipData.M_FEquipment.M_PlayerEquipmentType != EPlayerEquipmentType::E_Base)
	{
		//服装性别区分
		if (
			(Player->M_PlayerGender == 0U &&
				this->EquipData.M_FEquipment.M_PlayerEquipmentType != EPlayerEquipmentType::E_Men) ||
			(Player->M_PlayerGender == 1U &&
				this->EquipData.M_FEquipment.M_PlayerEquipmentType != EPlayerEquipmentType::E_Man)
			)
		{
			bUse = false;
			UWidgetBase::CreateTipWidget(TEXT("这个服装不适合你穿"));
		}
	}

	//可以进行穿戴
	if (bUse)
	{
		const FItemViewBlock& Block = this->EquipGrid->GetItemViewBlock();
		FItemViewBlock LastBlockData;
		switch (this->EquipData.M_FEquipment.M_EPlayerEquipmentSlotPosition)
		{
		case EPlayerEquipmentSlotPosition::E_Player_FlyItem: {
			Player->M_FPlayerSuit.M_FlyItem.CopyValue(this->EquipData.M_FEquipment, Block, LastBlockData);
		} break;
		case EPlayerEquipmentSlotPosition::E_Player_Hair: {
			Player->M_FPlayerSuit.M_Hair.CopyValue(this->EquipData.M_FEquipment, Block, LastBlockData);
		} break;
		case EPlayerEquipmentSlotPosition::E_Player_Body: {
			Player->M_FPlayerSuit.M_Body.CopyValue(this->EquipData.M_FEquipment, Block, LastBlockData);
		} break;
		case EPlayerEquipmentSlotPosition::E_Player_Head: {
			Player->M_FPlayerSuit.M_Head.CopyValue(this->EquipData.M_FEquipment, Block, LastBlockData);
		}  break;
		case EPlayerEquipmentSlotPosition::E_Player_Eye: {
			Player->M_FPlayerSuit.M_Eye.CopyValue(this->EquipData.M_FEquipment, Block, LastBlockData);
		}break;
		case EPlayerEquipmentSlotPosition::E_Player_Face: {
			Player->M_FPlayerSuit.M_Face.CopyValue(this->EquipData.M_FEquipment, Block, LastBlockData);
		} break;
		case EPlayerEquipmentSlotPosition::E_Player_Glasses: {
			Player->M_FPlayerSuit.M_Glasses.CopyValue(this->EquipData.M_FEquipment, Block, LastBlockData);
		}break;
		case EPlayerEquipmentSlotPosition::E_Player_Cap: {
			Player->M_FPlayerSuit.M_Cap.CopyValue(this->EquipData.M_FEquipment, Block, LastBlockData);
		}break;
		case EPlayerEquipmentSlotPosition::E_Player_Suit: {
			Player->M_FPlayerSuit.M_Suit.CopyValue(this->EquipData.M_FEquipment, Block, LastBlockData);
			Player->M_FPlayerSuit.M_bPlayerSuit = true;
			Player->M_FPlayerSuit.M_HiddenFlyItem = this->EquipData.M_FEquipment.M_bHiddenFlyItemByPlayer_Suit;
		}break;
		}

		//取消之前的装备
		if (LastBlockData.IsValid())
		{
			Player->AddEquipItem(LastBlockData.ItemID, LastBlockData.ItemLevel, LastBlockData.BlockIndex, false);
		}

		//装备当前服装
		Player->AddEquipItem(Block.ItemID, Block.ItemLevel, Block.BlockIndex);

		//清理缓存
		GameDataStaticObject<UPlayerBagEquipView>()->ClearCache();
		//获取背包UI
		GET_UI_CAST(UUI_PlayerBag, Global, PlayerBag)->RefreshPlayerSuit();
		GET_UI_CAST(UUI_PlayerBag, Global, PlayerBag)->LoadItemsData();
	}
}