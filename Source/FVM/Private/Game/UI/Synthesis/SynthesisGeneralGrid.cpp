// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Synthesis/SynthesisGeneralGrid.h"
#include <Components/Image.h>
#include <Components/Button.h> 
#include <Components/TextBlock.h>
#include "Game/UI/UI_PlayerSynthesis.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "Game/UI/Synthesis/SynModel_CardChange.h"

FString UUI_SynModel_GeneralBox::ToString_Implementation() {

	if (this->GetItemViewBlock().IsValid())
	{
		return GameDataStaticObject<
			UPlayerBagMaterialView
		>()->GetItemName(this->GetItemViewBlock().ItemID);
	}

	return FString("");
}

void UUI_SynModel_GeneralBox::OnClicked_Implementation() {

	if (this->GetItemViewBlock().IsValid())
	{
		FItemBaseStructData* BasePtr = nullptr;
		uint8 Type = 0U;
		FName Name = GET_DEF_CATEGORYNAME(Material);
		if (GET_ITEM(this->GetItemViewBlock().ItemID, Type, BasePtr, Name))
		{
			//弹出提示窗口
			this->SetFuncObject(NewObject<UItemBaseGridFunc>(), BasePtr);
		}
	}
}

void UUI_SynModel_GeneralBox::Cancel() {

	FItemViewBlock TempData;
	this->SetItemViewBlock(&TempData, nullptr, -1);
	this->SetButtonColorIsDark(false);
	this->CancelSelectEvent();
}


//------------------------------------------------------------------------------

FString UUI_SynModel_GeneralRectangleBox::ToString_Implementation() {

	if (this->GetItemViewBlock().IsValid())
	{
		return this->GetItemCountViewBlock().Name;
	}

	return FString("");
}

void UUI_SynModel_GeneralRectangleBox::OnClicked_Implementation() {}

void UUI_SynModel_GeneralRectangleBox::SetItemAttribute(
	class UItemBaseView* View,
	const FItemViewBlock& BlockData,
	int32 Index
) {
	Super::SetItemAttribute(View, BlockData, Index);
	this->SetButtonImageMatchSize();
}

void UUI_SynModel_GeneralRectangleBox::SetItemViewBlockCopy(UItemGridBase* CopyGrid) {
	Super::SetItemViewBlockCopy(CopyGrid);
	this->SetButtonImageMatchSize();
}

void UUI_SynModel_GeneralRectangleBox::Cancel() {
	FItemViewBlock TempData;
	this->SetItemViewBlock(&TempData, nullptr, -1);
	this->CurItemContViewBlock = FItemCountViewBlock();
	this->SetItemCountTextShow(false);
	this->CancelSelectEvent();
}