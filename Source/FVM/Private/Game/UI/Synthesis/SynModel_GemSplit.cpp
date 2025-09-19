// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Synthesis/SynModel_GemSplit.h"
#include <Components/TextBlock.h>
#include "Data/MaterialDataStruct.h"
#include "GameSystem/TaskSubsystem.h"
#include "Game/UI/UI_PlayerSynthesis.h"
#include "GameSystem/PlayerStructManager.h"

void USynModel_GemSplit::InitWidget_Implementation()
{
	this->Syn = Cast<UUI_PlayerSynthesis>(this->GetParentClass());

	this->BP_Syn_GemBox->SetParentClass(this);
	this->BP_Syn_CrystalBox->SetParentClass(this);

	this->Cancel();

	//显示界面
	this->Syn->SetCardAndGemsPanelShow(this, ESlateVisibility::Visible, 1);
}

void USynModel_GemSplit::Cancel() {
	this->BP_Syn_CrystalBox->SetVisibility(ESlateVisibility::Collapsed);
	this->SplitGemCoinText->SetText(FText::FromString("0"));
	this->SetIsEnabled(false);
	this->LoadGem();
	this->LoadCrystal();
}

void USynModel_GemSplit::SelectGem(UItemGridBase* Gem) {

	this->BP_Syn_GemBox->SetItemAttributeCopy(Gem);

	if (this->BP_Syn_GemBox->GetItemViewBlock().IsValid())
	{
		this->SetIsEnabled(true);
		this->BP_Syn_CrystalBox->SetVisibility(ESlateVisibility::Visible);

		if (this->BP_Syn_GemBox->GetItemViewBlock().ItemLevel > 10)
		{
			FItemViewBlock Block{ 870,0,0 };
			this->BP_Syn_CrystalBox->SetItemAttribute(
				GameDataStaticObject<UPlayerBagMaterialView>(), Block, 0
			);
		}
		else {
			FItemViewBlock Block{ 869,0,0 };
			this->BP_Syn_CrystalBox->SetItemAttribute(
				GameDataStaticObject<UPlayerBagMaterialView>(), Block, 0
			);
		}

		this->BP_Syn_CrystalBox->SetItemAttribute_ItemCount(
			this->SplitCrystalCount[this->BP_Syn_GemBox->GetItemViewBlock().ItemLevel]
		);

		this->SplitGemCoinText->SetText(FText::FromString(
			FString::FromInt(
				this->NeedCoin[this->BP_Syn_GemBox->GetItemViewBlock().ItemLevel]
			)
		));
	}
}

void USynModel_GemSplit::SplitGem() {


	UPlayerStructManager* Player = GET_PLAYERDATAINS();


	if (!Player->CheckCoin(this->NeedCoin[this->BP_Syn_GemBox->GetItemViewBlock().ItemLevel], 0U))
	{
		UWidgetBase::CreateTipWidget(TEXT("金币不足，无法分解"));
		return;
	}

	//删除宝石
	Player->RemoveEquipItem(this->BP_Syn_GemBox->GetItemViewBlock().ItemID,
		this->BP_Syn_GemBox->GetItemViewBlock().ItemLevel, -1, 1, 6);
	//发送水晶
	if (this->BP_Syn_GemBox->GetItemViewBlock().ItemLevel > 10)
	{
		UGameSystemFunction::SendItemToBag(
			870, 0
			, this->SplitCrystalCount[this->BP_Syn_GemBox->GetItemViewBlock().ItemLevel], 3, false, true
		);
	}
	else {
		UGameSystemFunction::SendItemToBag(
			869, 0
			, this->SplitCrystalCount[this->BP_Syn_GemBox->GetItemViewBlock().ItemLevel], 3, false, true
		);
	}

	/*
			添加历史记录
	*/
	UPlayerRecord::AddGemSplitCount();
	/*
		调用任务系统-完成宝石强化任务
	*/
	UTaskSubsystem::GetTaskSubsystemStatic()->ExecuteTasks(this);

	UGameSystemFunction::SaveCurrentPlayerData(TEXT("宝石强化"));

	this->BP_Syn_GemBox->Cancel();
	this->Cancel();

}

void USynModel_GemSplit::LoadGem() {
	this->Syn->LoadPublicEquips(
		{ 6U }, {}, GET_UI_NAME(Synthesis, SplitGem)
	);
}

void USynModel_GemSplit::LoadCrystal() {
	this->Syn->LoadPublicMaterials(
		{ 7U }, {}, GET_UI_NAME(Synthesis, SplitGem)
	);
}