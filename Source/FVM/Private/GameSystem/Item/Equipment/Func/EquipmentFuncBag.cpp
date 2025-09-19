// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/Equipment/Func/EquipmentFuncBag.h"
#include "Game/UI/UI_PlayerBagEquipmentGrid.h"
#include "Game/UI/Tips/UI_ItemDesTip.h"
#include "Data/EquipmentDataSturct.h"
#include "Game/UI/UI_PlayerBag.h"

void UEquipmentFuncBag::Execute(
	class UItemGridBase* EquipGridPtr,
	struct FItemBaseStructData* ItemBasePtr
) {

	Super::Execute(EquipGridPtr, ItemBasePtr);

	this->EquipGrid = Cast<UUI_EquipmentGrid>(EquipGridPtr);

	FEquipment_Bag_Data TempData = CAST_TYPE(FEquipment_Bag_Data, ItemBasePtr);

	this->BagData = TempData.M_FEquipment;

	if (this->bPreview)
	{
		this->GetTipWidget()->SetHidden();
	}
	else {
		this->GetTipWidget()->SetHidden(false);
		this->GetTipWidget()->SetOkButtonTitleText(TEXT("装备"));
		this->GetTipWidget()->BindClickFunc(this, TEXT("Equip"));
	}

}

void UEquipmentFuncBag::SetPreview() {
	this->bPreview = true;
}

void UEquipmentFuncBag::Equip() {

	const FItemViewBlock& Blcok = this->EquipGrid->GetItemViewBlock();

	UPlayerStructManager* Player = GET_PLAYERDATAINS();

	UUI_PlayerBag* PlayerBagUI = GET_UI_CAST(UUI_PlayerBag, Global, PlayerBag);

	int32 Index = PlayerBagUI->GetSelectEquipBagIndex();

	Player->AddEquipItem(Blcok.ItemID, Blcok.ItemLevel, Blcok.BlockIndex);

	//关闭背包界面
	PlayerBagUI->ClosePlayerBagEquipPanel();

	//获取界面索引
	switch (PlayerBagUI->GetSelectPanelIndex())
	{
	case 0U: {
		FPlayerBagGird& Grid = Player->M_PlayerBagGirdGroup.M_EquipmentGrid[Index];
		Grid.BlockIndex = Blcok;
		Grid.M_GridCount = BagData.M_ContentCount;
		Grid.M_GridImagePath = BagData.ItemTexturePath.ToString();
	}; break;
	case 1U: {
		FPlayerBagGird& Grid = Player->M_PlayerBagGirdGroup.M_CardGrid[Index];
		Grid.BlockIndex = Blcok;
		Grid.M_GridCount = BagData.M_ContentCount;
		Grid.M_GridImagePath = BagData.ItemTexturePath.ToString();
	}; break;
	case 2U: {
		FPlayerBagGird& Grid = Player->M_PlayerBagGirdGroup.M_MaterialGrid[Index];
		Grid.BlockIndex = Blcok;
		Grid.M_GridCount = BagData.M_ContentCount;
		Grid.M_GridImagePath = BagData.ItemTexturePath.ToString();
	}; break;
	}

	//在其他界面时，装备道具，需要清理缓存，当切换到装备栏界面时，才会正常显示
	if (PlayerBagUI->GetSelectPanelIndex() != 0U)
	{
		GameDataStaticObject<UPlayerBagEquipView>()->ClearCache();
	}

	PlayerBagUI->RelaodCurrentItemData();

	//保存游戏
	UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("进行背包装备")));
}
