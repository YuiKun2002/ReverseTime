// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/Equipment/Func/EquipmentFuncGem.h"
#include "Game/UI/UI_PlayerSynthesis.h"
#include "Game/UI/Tips/UI_ItemDesTip.h"
#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/Synthesis/SynModel_GemSplit.h"
#include "Game/UI/Synthesis/SynModel_WeaponGems.h"
#include "Game/UI/Synthesis/SynModel_GemsUpGrade.h"

void UEquipmentFuncGem::Execute(UItemGridBase* GridPtr, FItemBaseStructData* ItemBaseDataPtr
) {
	Super::Execute(GridPtr, ItemBaseDataPtr);

	this->Grid = GridPtr;
	this->GemData = CAST_TYPE(FEquipment_WeaponGem_Data, ItemBaseDataPtr);


	this->GetTipWidget()->SetHidden(false);
	this->GetTipWidget()->SetOkButtonTitleText(TEXT("选择"));
	switch (this->OperateType)
	{
	case 1: {
		this->GetTipWidget()->BindClickFunc(this, TEXT("SynthesisWeaponGemSelect"));
	}; break;
	case 2: {
		this->GetTipWidget()->BindClickFunc(this, TEXT("SynthesisUpgradeGemSelect"));
	}; break;
	case 3: {
		this->GetTipWidget()->BindClickFunc(this, TEXT("SynthesisSplitGemSelect"));
	}; break;
	default: {
		this->GetTipWidget()->SetHidden(true);
	}break;
	}
}

void UEquipmentFuncGem::SynthesisWeaponGemSelect() {
	//获取合成屋界面
	UUI_PlayerSynthesis* Syn = GET_UI_CAST(UUI_PlayerSynthesis, Global, Synthesis);

	if (IsValid(Syn) && Syn->IsInViewport())
	{
		//获取角色
		UPlayerStructManager* const Player = GET_PLAYERDATAINS();

		//获取武器宝石界面
		UUI_SynModel_WeaponGem* WG = Cast<UUI_SynModel_WeaponGem>(
			Syn->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, WeaponAndGem))
		);

		if (IsValid(WG))
		{
			bool bCheck = WG->CheckTheGemMatchTheWeapon(
				GemData.M_FEquipment.WeaponItemConstantID,
				(uint8)GemData.M_FEquipment.GemType
			);

			if (bCheck)
			{
				WG->SelectGem(
					this->Grid->GetItemViewBlock().ItemID,
					this->Grid->GetItemViewBlock().ItemLevel
				);
			}
		}
	}
}

void UEquipmentFuncGem::SynthesisUpgradeGemSelect() {
	//获取合成屋界面
	UUI_PlayerSynthesis* Syn = GET_UI_CAST(UUI_PlayerSynthesis, Global, Synthesis);

	if (IsValid(Syn) && Syn->IsInViewport())
	{
		//获取角色
		UPlayerStructManager* const Player = GET_PLAYERDATAINS();

		//获取武器宝石界面
		USynModel_GemsUpGrade* UG = Cast<USynModel_GemsUpGrade>(
			Syn->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, UpgradeGem))
		);

		if (IsValid(UG))
		{
			UG->SelectGemSlot(this->GemData.M_FEquipment.M_GemLevelMax, Cast<UItemGridBase>(this->Grid));
		}
	}
}

void UEquipmentFuncGem::SynthesisSplitGemSelect() {

	//获取合成屋界面
	UUI_PlayerSynthesis* Syn = GET_UI_CAST(UUI_PlayerSynthesis, Global, Synthesis);

	if (IsValid(Syn) && Syn->IsInViewport())
	{
		//获取角色
		UPlayerStructManager* const Player = GET_PLAYERDATAINS();

		//获取武器宝石界面
		USynModel_GemSplit* SG = Cast<USynModel_GemSplit>(
			Syn->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, SplitGem))
		);

		if (IsValid(SG))
		{
			SG->SelectGem(Cast<UItemGridBase>(this->Grid));
		}
	}
}
