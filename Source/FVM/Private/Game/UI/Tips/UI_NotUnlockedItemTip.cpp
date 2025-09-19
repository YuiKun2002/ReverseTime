// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Tips/UI_NotUnlockedItemTip.h"

#include <Components/Button.h>
#include <Components/HorizontalBox.h>

//合成屋的UI格子实现了信息显示功能
#include "Game/UI/Synthesis/UI_SynCardGrid.h"
#include "Game/UI/Synthesis/UI_SynItemGrid.h"

#include "Data/EquipmentDataSturct.h"
#include "Data/MaterialDataStruct.h"
#include "Data/CardData/CardDataStruct.h"


UUI_NotUnlockedItemTip* UUI_NotUnlockedItemTip::CreateNotUnlockedItemTip(
	int32 CreateType,
	FItemViewBlock Block,
	FText Text
) {

	UUI_NotUnlockedItemTip* Widget = GET_UI_CAST(UUI_NotUnlockedItemTip, Global, NotUnlockedItemTip);
	if (!Widget->IsInViewport())
	{
		Widget->ShowArea->ClearChildren();

		switch (CreateType)
		{
		case 1: {
			Widget->CreateCard(Block);
		}break;
		case 2: {
			Widget->CreateMater(Block);
		}break;
		default: {
			Widget->CreateEquip(Block);
		}break;
		}

		Widget->SetContentText(Text);

		Widget->Ok->OnClicked.Clear();
		Widget->BindDef(Widget->Ok);

		Widget->AddToViewport();
	}
	return Widget;
}

void UUI_NotUnlockedItemTip::CreateEquip(const FItemViewBlock& Block) {
	if (!IsValid(this->EquipGrid))
	{
		this->EquipGrid = CREATE_UI(this, UUI_SynItemGrid, Synthesis, SynItemGrid);
		Cast<UUI_SynItemGrid>(this->EquipGrid)->SetInitType(0);
	}
	this->EquipGrid->SetItemAttribute(GameDataStaticObject<UPlayerBagEquipView>(), Block, 0);
	if (this->EquipGrid->GetItemCountViewBlock().Count == 0)
	{
		this->EquipGrid->SetButtonColorIsDark();
	}
	else {
		this->EquipGrid->SetButtonColorIsDark(false);
	}
	this->ShowArea->AddChildToHorizontalBox(this->EquipGrid);
}

void UUI_NotUnlockedItemTip::CreateCard(const FItemViewBlock& Block) {

	if (!IsValid(EquipGrid))
	{
		this->CardGrid = CREATE_UI(this, UUI_SynCardGrid, Synthesis, SynItemGrid);
		Cast<UUI_SynCardGrid>(this->CardGrid)->SetCardOperateType(-1);
	}
	this->CardGrid->SetItemAttribute(GameDataStaticObject<UPlayerBagCardView>(), Block, 0);
	if (this->CardGrid->GetItemCountViewBlock().Count == 0)
	{
		this->CardGrid->SetButtonColorIsDark();
	}
	else {
		this->CardGrid->SetButtonColorIsDark(false);
	}
	this->ShowArea->AddChildToHorizontalBox(this->CardGrid);
}

void UUI_NotUnlockedItemTip::CreateMater(const FItemViewBlock& Block) {
	if (!IsValid(this->MaterGrid))
	{
		this->MaterGrid = CREATE_UI(this, UUI_SynItemGrid, Synthesis, SynItemGrid);
		Cast<UUI_SynItemGrid>(this->MaterGrid)->SetInitType(1);
	}

	this->MaterGrid->SetItemAttribute(GameDataStaticObject<UPlayerBagMaterialView>(), Block, 0);
	if (this->MaterGrid->GetItemCountViewBlock().Count == 0)
	{
		this->MaterGrid->SetButtonColorIsDark();
	}
	else {
		this->MaterGrid->SetButtonColorIsDark(false);
	}
	this->ShowArea->AddChildToHorizontalBox(this->MaterGrid);
}