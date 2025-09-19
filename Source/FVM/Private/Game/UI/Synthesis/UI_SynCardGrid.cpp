// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Synthesis/UI_SynCardGrid.h"
#include "Data/MaterialDataStruct.h"
#include "Game/UI/UI_PlayerSynthesis.h"
#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/Synthesis/SynModel_CardChange.h"
#include "Game/UI/Synthesis/SynModel_GoldCardUp.h"
#include "Game/UI/Synthesis/SynModel_CardUpgrade.h"


void UUI_SynCardGrid::OnClicked_Implementation() {
	switch (this->OperType)
	{
		//卡片强化
	case 0:this->CardUp(); break;
		//卡片转职
	case 1:this->CardTransfer(); break;
		//金卡进化
	case 2:this->CardGoldEvolve(); break;


	default:this->ShowCardTip(); break;
	}
}

void UUI_SynCardGrid::CardUp() {
	UUI_SynModel_UpgradeCard* UpCardUI = Cast<UUI_SynModel_UpgradeCard>(Cast<UUI_PlayerSynthesis>(
		this->GetParentClass()
	)->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, UpgradeCard)));

	auto SetIA = [this](UUI_PlayerBagCardGrid* Grid) {
		Grid->SetItemAttribute(
			GameDataStaticObject<UPlayerBagCardView>(),
			this->GetItemViewBlock(),
			this->GetUiIndex()
		);
		this->SetButtonIsEnabled(false);
		};

	//主卡
	if (!UpCardUI->CardBox_Center->GetItemViewBlock().IsValid())
	{
		SetIA(UpCardUI->CardBox_Center);
		return;
	}
	if (!UpCardUI->CardBox_CenterTop->GetItemViewBlock().IsValid())
	{
		SetIA(UpCardUI->CardBox_CenterTop);
		return;
	}
	if (!UpCardUI->CardBox_Left->GetItemViewBlock().IsValid())
	{
		SetIA(UpCardUI->CardBox_Left);
		return;
	}
	if (!UpCardUI->CardBox_Right->GetItemViewBlock().IsValid())
	{
		SetIA(UpCardUI->CardBox_Right);
		return;
	}

	this->ShowCardTip();
}

void UUI_SynCardGrid::CardTransfer() {
	UUI_SynModel_TransferCardMainPanel* TransferCardUI =
		Cast<UUI_SynModel_TransferCardMainPanel>(Cast<UUI_PlayerSynthesis>(
			this->GetParentClass()
		)->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, TransferCard)));

	TransferCardUI->SelectItem(
		this->GetItemViewBlock(),
		this->GetItemCountViewBlock(),
		this->GetUiIndex()
	);
}

void UUI_SynCardGrid::CardGoldEvolve() {
	USynModel_GoldCardUp* CardUI =
		Cast<USynModel_GoldCardUp>(Cast<UUI_PlayerSynthesis>(
			this->GetParentClass()
		)->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, GoldCardEvolve)));

	CardUI->SelectCard(this);
}