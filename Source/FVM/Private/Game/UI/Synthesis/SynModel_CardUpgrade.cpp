// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Synthesis/SynModel_CardUpgrade.h"
#include <Components/TextBlock.h>
#include <Components/Button.h>
#include "GameSystem/TaskSubsystem.h"
#include "Game/UI/UI_PlayerSynthesis.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "GameSystem/PlayerStructManager.h"

void UUI_SynModel_UpgradeCard_Box::OnClicked_Implementation() {
	if (this->GetItemViewBlock().IsValid())
	{
		this->Cancel();

		Cast<UUI_SynModel_UpgradeCard>(Cast<UUI_PlayerSynthesis>(
			this->GetParentClass()
		)->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, UpgradeCard)))->Reload();
		Cast<UUI_SynModel_UpgradeCard>(Cast<UUI_PlayerSynthesis>(
			this->GetParentClass()
		)->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, UpgradeCard)))->Select();

	}
}

FString UUI_SynModel_UpgradeCard_Box::ToString_Implementation() {
	if (this->GetItemViewBlock().IsValid())
	{
		return this->GetItemCountViewBlock().Name;
	}
	return FString();
}

void UUI_SynModel_UpgradeCard_Box::SetItemAttribute(
	UItemBaseView* View,
	const FItemViewBlock& BlockData,
	int32 Index
) {
	Super::SetItemAttribute(View, BlockData, Index);
	Cast<UUI_SynModel_UpgradeCard>(Cast<UUI_PlayerSynthesis>(
		this->GetParentClass()
	)->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, UpgradeCard)))->Select();
}

void UUI_SynModel_UpgradeCard::InitWidget_Implementation() {

	this->Syn = Cast<UUI_PlayerSynthesis>(this->GetParentClass());

	this->BP_Syn_UpCardCloverBox->SetParentClass(this);
	this->CardBox_Center->SetParentClass(this->Syn);
	this->CardBox_CenterTop->SetParentClass(this->Syn);
	this->CardBox_Left->SetParentClass(this->Syn);
	this->CardBox_Right->SetParentClass(this->Syn);

	this->CardBox_Center->OnClicked();
	this->CardBox_CenterTop->OnClicked();
	this->CardBox_Left->OnClicked();
	this->CardBox_Right->OnClicked();

	this->Select();

	this->Reload();

	//显示界面
	this->Syn->SetCardAndGemsPanelShow(this,
		ESlateVisibility::SelfHitTestInvisible, 0,
		ESlateVisibility::SelfHitTestInvisible);
}

void UUI_SynModel_UpgradeCard::Select() {

	if (!this->CardBox_Center->GetItemViewBlock().IsValid())
	{
		this->CardBox_Center->Cancel();
	}
	if (!this->CardBox_CenterTop->GetItemViewBlock().IsValid())
	{
		this->CardBox_CenterTop->Cancel();
	}
	if (!this->CardBox_Left->GetItemViewBlock().IsValid())
	{
		this->CardBox_Left->Cancel();
	}
	if (!this->CardBox_Right->GetItemViewBlock().IsValid())
	{
		this->CardBox_Right->Cancel();
	}

	if (this->CardBox_Center->GetItemViewBlock().IsValid())
	{
		if (this->CardBox_CenterTop->GetItemViewBlock().IsValid() ||
			this->CardBox_Left->GetItemViewBlock().IsValid() ||
			this->CardBox_Right->GetItemViewBlock().IsValid()
			)
		{
			this->UpdateRate();
			this->Upgrade_Butt->SetIsEnabled(true);
			this->RateText->SetText(FText::FromString(
				FString::Printf(TEXT("%.2f"), this->UpRate) + TEXT("%")
			));
			this->CoinText->SetText(FText::FromString(FString::FromInt(this->UpCoin)));
			this->LoadCard(true);
			return;
		}
	}


	this->RateText->SetText(FText::FromString(TEXT("0.00%")));
	this->CoinText->SetText(FText::FromString(TEXT("0")));
	this->Upgrade_Butt->SetIsEnabled(false);
}

void UUI_SynModel_UpgradeCard::SetClover(UItemGridBase* CopyCloverGrid, float Rate) {

	//设置按钮
	this->BP_Syn_UpCardCloverBox->SetItemAttributeCopy(CopyCloverGrid);

	this->CloverID = this->BP_Syn_UpCardCloverBox->GetItemID();
	this->CloverRate = Rate;
	this->Select();
	this->Reload();
}

void UUI_SynModel_UpgradeCard::CancelSelectClover() {
	this->CloverID = -1;
	this->CloverRate = 1.f;
	this->Select();
	this->Reload();
}

void UUI_SynModel_UpgradeCard::Reload() {
	this->LoadCard();
	this->LoadMaterial();
}

void UUI_SynModel_UpgradeCard::LoadCard(bool bNotLoad) {
	//选择防御卡
	TArray<FItemViewBlock> Selects;
	//主卡
	if (this->CardBox_Center->GetItemViewBlock().IsValid())
	{
		Selects.Emplace(this->CardBox_Center->GetItemViewBlock());
	}
	if (this->CardBox_CenterTop->GetItemViewBlock().IsValid())
	{
		Selects.Emplace(this->CardBox_CenterTop->GetItemViewBlock());
	}
	if (this->CardBox_Left->GetItemViewBlock().IsValid())
	{
		Selects.Emplace(this->CardBox_Left->GetItemViewBlock());
	}
	if (this->CardBox_Right->GetItemViewBlock().IsValid())
	{
		Selects.Emplace(this->CardBox_Right->GetItemViewBlock());
	}
	this->Syn->LoadPublicCards(Selects, 0, bNotLoad);
}

void UUI_SynModel_UpgradeCard::LoadMaterial(bool bNotLoad) {
	this->Syn->LoadPublicMaterials(
		{ (uint8)EMaterialType::E_Clover },
		{ {this->GetCloverID(),0,0} },
		GET_UI_NAME(Synthesis, UpgradeCard), bNotLoad
	);
}

void UUI_SynModel_UpgradeCard::UpdateRate() {
	//初始化卡槽
	UUI_SynModel_UpgradeCard_Box* Items[4] = {
	this->CardBox_Center,this->CardBox_CenterTop,this->CardBox_Left,this->CardBox_Right
	};

	if (!Items[0]->GetItemViewBlock().IsValid())
	{
		return;
	}

	//主卡等级
	int32 MainCardLevel = Items[0]->GetItemViewBlock().ItemLevel;
	//当前卡片概率（概率）集合
	TArray<float> CardRateArray;
	//初始化强化概率
	this->UpRate = 0.f;
	//初始化强化金额
	this->UpCoin = this->LUpCoinNum[MainCardLevel];
	//设置保险金金额
	this->Syn->GetSynInsurancePanel()->SetSynInsurancePrice(this->LInsuranceCoinNum[MainCardLevel]);
	//如果当前卡片等级为最大等级，则无法强化
	if (MainCardLevel >= 16)
	{
		this->Upgrade_Butt->SetIsEnabled(false);
		return;
	}

	//循环概率【从第一张副卡开始】
	for (uint8 Index = 1U; Index < 4U; Index += 1U)
	{
		const FItemViewBlock& CurData = (*(Items + Index))->GetItemViewBlock();
		const FItemCountViewBlock& CountData = (*(Items + Index))->GetItemCountViewBlock();

		if (CurData.IsValid())
		{
			CardRateArray.Emplace(
				this->GetUpgradeRate(
					GameDataStaticObject<UPlayerBagCardView>()->CheckCardAutoPrice(CurData.ItemID),
					MainCardLevel,
					CountData.Count,
					(MainCardLevel - CurData.ItemLevel)
				));
		}
		else {
			CardRateArray.Emplace(0.f);
		}
	}

	//排序大小(概率)
	CardRateArray.Sort([](const float& _A, const float& _B) {return _A > _B; });

	//设置概率
	this->UpRate = CardRateArray[0];
	CardRateArray.RemoveAt(0);
	for (auto Item : CardRateArray) {
		//剩下的概率衰减原来的 0.3
		this->UpRate = this->UpRate + Item * 0.3f;
	}

	//是否有四叶草
	this->UpRate = this->UpRate * this->CloverRate;

	//限制概率100 显示100
	if (this->UpRate > 100.f)
	{
		this->UpRate = 100.f;
	}
}

float UUI_SynModel_UpgradeCard::GetUpgradeRate(bool bAdvancedCard,
	const int32& CardLevel,
	const int32& CardPrice,
	const int32& D_Value) {
	//同星
	if (D_Value == 0)
	{
		if (bAdvancedCard)
		{
			return M_UpGradeRate_0_Arrays_0_Hei[CardLevel];
		}

		if (CardPrice < 100)
		{
			return M_UpGradeRate_0_Arrays_0_Low[CardLevel];
		}
		else if (CardPrice >= 100 && CardPrice < 200)
		{
			return M_UpGradeRate_0_Arrays_0_Mid[CardLevel];
		}
		else
		{
			return M_UpGradeRate_0_Arrays_0_Hei[CardLevel];
		}
	}//差1星
	else if (D_Value == 1)
	{
		if (bAdvancedCard)
		{
			return M_UpGradeRate_0_Arrays_1_Hei[CardLevel];
		}

		if (CardPrice < 100)
		{
			return M_UpGradeRate_0_Arrays_1_Low[CardLevel];
		}
		else if (CardPrice >= 100 && CardPrice < 200)
		{
			return M_UpGradeRate_0_Arrays_1_Mid[CardLevel];
		}
		else
		{
			return M_UpGradeRate_0_Arrays_1_Hei[CardLevel];
		}
	}//差2星
	else if (D_Value == 2)
	{
		if (bAdvancedCard)
		{
			return M_UpGradeRate_0_Arrays_2_Hei[CardLevel];
		}

		if (CardPrice < 100)
		{
			return M_UpGradeRate_0_Arrays_2_Low[CardLevel];
		}
		else if (CardPrice >= 100 && CardPrice < 200)
		{
			return M_UpGradeRate_0_Arrays_2_Mid[CardLevel];
		}
		else
		{
			return M_UpGradeRate_0_Arrays_2_Hei[CardLevel];
		}
	}
	else if (D_Value > 3) // 差3直接0
	{
		return  0.f;
	}//比主高1
	else if (D_Value == -1)
	{
		if (bAdvancedCard)
		{
			return M_UpGradeRate_0_Arrays_P1_Hei[CardLevel];
		}

		if (CardPrice < 100)
		{
			return M_UpGradeRate_0_Arrays_P1_Low[CardLevel];
		}
		else if (CardPrice >= 100 && CardPrice < 200)
		{
			return M_UpGradeRate_0_Arrays_P1_Mid[CardLevel];
		}
		else
		{
			return M_UpGradeRate_0_Arrays_P1_Hei[CardLevel];
		}

	}//比主高2
	else if (D_Value <= -2) {
		return 100.f;
	}

	return 0.f;
}

void UUI_SynModel_UpgradeCard::UpgradeCard() {

	if (!GET_PLAYERDATAINS()->CheckCoin(this->UpCoin, 0U))
	{
		UWidgetBase::CreateTipWidget(TEXT("金币不足，无法强化"));
		return;
	}

	bool bSelect = this->Syn->GetSynInsurancePanel()->GetSynInsuranceSelect();
	if (bSelect)
	{
		//支付保险金
		if (!this->Syn->GetSynInsurancePanel()->PaySynInsurance())
		{
			UWidgetBase::CreateTipWidget(TEXT("点券不足，无法强化"));
			return;
		}
	}

	//检测是否有四叶草
	if (this->BP_Syn_UpCardCloverBox->GetItemViewBlock().IsValid())
	{
		//消耗四叶草
		UPlayerStructManager* Player = GET_PLAYERDATAINS();

		//消耗结果
		bool bRe = Player->ConsumeMaterial(
			this->BP_Syn_UpCardCloverBox->GetItemViewBlock().ItemID, 0, 1
		);

		if (bRe)
		{
			//如果四叶草消耗完毕了，则取消对四叶草的选择
			if (!Player->FindMaterialCount(
				this->BP_Syn_UpCardCloverBox->GetItemViewBlock().ItemID, 0, 1
			))
			{
				this->BP_Syn_UpCardCloverBox->OnClicked();
			}
		}
	}

	float CurRate = UGameSystemFunction::GetRandomRangeFloat(0.001f, 100.f);
	bool bReSelect = true;
	bool bUpResult = false;
	if (this->UpRate > 0.f && this->UpRate >= CurRate)
	{
		bUpResult = true;

		UWidgetBase::CreateTipWidget(TEXT("强化成功"));

		//消耗卡片
		UPlayerStructManager* Player = GET_PLAYERDATAINS();
		if (this->CardBox_Center->GetItemViewBlock().IsValid())
		{
			Player->ConsumeCard(
				this->CardBox_Center->GetItemViewBlock().ItemID,
				this->CardBox_Center->GetItemViewBlock().ItemLevel,
				1
			);
		}

		if (this->CardBox_CenterTop->GetItemViewBlock().IsValid())
		{
			Player->ConsumeCard(
				this->CardBox_CenterTop->GetItemViewBlock().ItemID,
				this->CardBox_CenterTop->GetItemViewBlock().ItemLevel,
				1
			);
		}

		if (this->CardBox_Left->GetItemViewBlock().IsValid())
		{
			Player->ConsumeCard(
				this->CardBox_Left->GetItemViewBlock().ItemID,
				this->CardBox_Left->GetItemViewBlock().ItemLevel,
				1
			);
		}

		if (this->CardBox_Right->GetItemViewBlock().IsValid())
		{
			Player->ConsumeCard(
				this->CardBox_Right->GetItemViewBlock().ItemID,
				this->CardBox_Right->GetItemViewBlock().ItemLevel,
				1
			);
		}

		//强化成功，发送卡片
		UGameSystemFunction::SendItemToBag(
			this->CardBox_Center->GetItemViewBlock().ItemID,
			this->CardBox_Center->GetItemViewBlock().ItemLevel + 1,
			1, 2U, false, true
		);

		/*
			添加历史记录
		*/
		UPlayerRecord::AddDayCardUpCount(
			this->CardBox_Center->GetItemViewBlock().ItemLevel + 1,
			this->CardBox_Center->GetItemCountViewBlock().Name
		);
		UPlayerRecord::AddCardUpGradeCount(1);
		/*
			调用任务系统-完成卡片强化任务
		*/
		UTaskSubsystem::GetTaskSubsystemStatic()->ExecuteTasks(this);
	}
	else {
		UWidgetBase::CreateTipWidget(TEXT("强化失败，不够好运"));
		//强化失败
		if (!bSelect)
		{

			UPlayerStructManager* Player = GET_PLAYERDATAINS();

			if (this->CardBox_Center->GetItemViewBlock().ItemLevel >= 6)
			{
				//强化失败，发送卡片
				UGameSystemFunction::SendItemToBag(
					this->CardBox_Center->GetItemViewBlock().ItemID,
					this->CardBox_Center->GetItemViewBlock().ItemLevel - 1,
					1, 2U, false, true
				);

				//消耗卡片
				if (this->CardBox_Center->GetItemViewBlock().IsValid())
				{
					Player->ConsumeCard(
						this->CardBox_Center->GetItemViewBlock().ItemID,
						this->CardBox_Center->GetItemViewBlock().ItemLevel,
						1
					);
				}
			}

			if (this->CardBox_CenterTop->GetItemViewBlock().IsValid())
			{
				Player->ConsumeCard(
					this->CardBox_CenterTop->GetItemViewBlock().ItemID,
					this->CardBox_CenterTop->GetItemViewBlock().ItemLevel,
					1
				);
			}
			if (this->CardBox_Left->GetItemViewBlock().IsValid())
			{
				Player->ConsumeCard(
					this->CardBox_Left->GetItemViewBlock().ItemID,
					this->CardBox_Left->GetItemViewBlock().ItemLevel,
					1
				);
			}
			if (this->CardBox_Right->GetItemViewBlock().IsValid())
			{
				Player->ConsumeCard(
					this->CardBox_Right->GetItemViewBlock().ItemID,
					this->CardBox_Right->GetItemViewBlock().ItemLevel,
					1
				);
			}
		}
		else {
			bReSelect = false;
		}
	}

	//支付货币
	GET_PLAYERDATAINS()->ReduceCoin(this->UpCoin, 0U);

	//保存存档
	UGameSystemFunction::SaveCurrentPlayerData(TEXT("卡片强化"));

	if (bReSelect)
	{
		//取消其他三项卡片的选择
		this->CardBox_CenterTop->Cancel();
		this->CardBox_Left->Cancel();
		this->CardBox_Right->Cancel();
		//调整结构
		if (bUpResult)
		{
			FItemViewBlock Data = this->CardBox_Center->GetItemViewBlock();
			Data.ItemLevel += 1;
			//调整主卡的等级
			this->CardBox_Center->SetItemAttribute(
				GameDataStaticObject<UPlayerBagCardView>(),
				Data,
				this->CardBox_Center->GetUiIndex()
			);
		}
		else {
			FItemViewBlock Data = this->CardBox_Center->GetItemViewBlock();
			if (Data.ItemLevel > 6)
			{
				Data.ItemLevel -= 1;

				//调整主卡的等级
				this->CardBox_Center->SetItemAttribute(
					GameDataStaticObject<UPlayerBagCardView>(),
					Data,
					this->CardBox_Center->GetUiIndex()
				);
			}
		}
	}
	this->Select();
	this->Reload();
}

