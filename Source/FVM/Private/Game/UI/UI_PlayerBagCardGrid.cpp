// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/UI/UI_PlayerBagCardGrid.h"
#include <Components/TextBlock.h>
#include "Game/UI/UI_GamePrepare.h"
#include "Data/CardData/CardDataStruct.h"

FString UUI_PlayerBagCardGrid::ToString_Implementation() {

#if WITH_EDITOR

	return this->CurItemContViewBlock.Name +
		FString(TEXT("\nCard ID ：")) +
		FString::FromInt(this->GetItemViewBlock().ItemID) +
		FString(TEXT("\nUI Index ：")) +
		FString::FromInt(this->GetUiIndex()) +
		FString(TEXT("\nBlock Index ：")) +
		FString::FromInt(this->GetItemViewBlock().BlockIndex) +
		FString(TEXT("\nCard Price ：")) +
		FString::FromInt(this->CurItemContViewBlock.Count);
#endif
	return this->CurItemContViewBlock.Name;
}

void UUI_PlayerBagCardGrid::OnClicked_Implementation() {
	switch (this->OperType)
	{
	case 1: { this->SelectCurrentCard(); }break;
	case 2: { this->RemoveCurrentSelectCard(); }break;
	default:
	{
		this->ShowCardTip();
	}break;
	}
}

void UUI_PlayerBagCardGrid::ShowCardTip() {

	uint8 Type;
	FItemBaseStructData* BasePtr = nullptr;
	if (GET_ITEM(this->GetItemID(), Type, BasePtr, GET_DEF_CATEGORYNAME(Card)))
	{
		this->SetFuncObject(NewObject<UItemCardBaseGridFunc>(), BasePtr);
	}
}

void UUI_PlayerBagCardGrid::SetItemCount(int32 NewCount) {


	//防御卡默认显示
	if (!this->bCountVisiable)
	{
		this->bCountVisiable = true;
		this->Count_Text->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	//设置价格
	if (this->CurItemContViewBlock.Count != NewCount)
	{
		this->CurItemContViewBlock.Count = NewCount;

		if (GameDataStaticObject<UPlayerBagCardView>()->CheckCardAutoPrice(this->GetItemViewBlock().ItemID))
		{
			this->Count_Text->SetText(FText::FromString(FString::FromInt(NewCount) + TEXT("+")));
		}
		else {
			this->Count_Text->SetText(FText::FromString(FString::FromInt(NewCount)));
		}
	}
}

void UUI_PlayerBagCardGrid::SelectCurrentCard()
{
	//获取准备UI
	UUI_GamePrepare* Game = UUI_GamePrepare::GetGamePrepareUiStatic();
	if (IsValid(Game))
	{
		Game->OnSelectCard(this);
	}
}

void UUI_PlayerBagCardGrid::RemoveCurrentSelectCard()
{
	//获取准备UI
	UUI_GamePrepare* Game = UUI_GamePrepare::GetGamePrepareUiStatic();
	if (IsValid(Game))
	{
		Game->OnRemoveCard(this);
	}
}
/*
void UUI_PlayerBagCardGrid::AddUpGradeFunc()
{
	GAME_LOG(__FUNCTION__, TEXT("背包卡片UI-将次卡片数据添加到合成屋-卡片强化"), {
		GAME_LOG_LINE(TEXT("需要重写"));
		});
	if (IsValid(this->M_UI_PlayerSynthesis->GetCardUpgradeFunction()))
	{
		FSynModelCardUpgradeData Data;
		Data.PlayerBagIndex = M_CardIndex;
		Data.CardGrid = this;
		Data.CardData = this->CopyData;
		this->M_UI_PlayerSynthesis->GetCardUpgradeFunction()->AddCardDataMain(Data);
	}
}

void UUI_PlayerBagCardGrid::AddSynthesisChangeJobs()
{
	GAME_LOG(__FUNCTION__, TEXT("背包卡片UI-将次卡片数据添加到合成屋-转职"), {
		GAME_LOG_LINE(TEXT("需要重写"));
		});

	if (!this->M_UI_PlayerSynthesis)
	{
		return;
	}

	if (!IsValid(this->M_UI_PlayerSynthesis->GetCardChangeFunction()))
	{
		return;
	}

	//数据
	FMainCardChangeType& CardData = this->M_UI_PlayerSynthesis->GetCardChangeFunction()->GetFMainCardChangeData();

	//设置UI
	if (IsValid(CardData.Grid))
	{
		CardData.Grid->GetButton()->SetIsEnabled(true);
	}
	CardData.Grid = this;
	CardData.UIGrideIndex = this->GetUIIndex();
	this->GetButton()->SetIsEnabled(false);

	CardData.ChangeCardName = this->CopyData.ItemName;
	CardData.ChangeCardHeadPath = this->CopyData.ItemTexturePath.ToString();
	CardData.TargetChangeCardName = this->CopyData.M_FCardTransfer.M_ChangeMaterialsName;
	CardData.TargetType = this->CopyData.M_FCardTransfer.M_ChangeType;
	CardData.CardGrade = this->CopyData.M_FCardTransfer.M_ChangeGrade;
	//赋值索引
	const TMap<int32, FItemCard>& Cards = UFVMGameInstance::GetPlayerStructManager_Static()->FindCardByName(this->CopyData.ItemName);
	for (const auto& CurData : Cards)
	{
		if (CurData.Value.M_CardGrade == this->CopyData.M_CardGrade)
		{
			CardData.PlayerBagIndex = CurData.Key;
			break;
		}
	}

	//拿到转职关键词
	FString ChangeKeyName = this->CopyData.M_FCardTransfer.M_ChangeKeyName;
	//拿到转职等级
	FString ChangeGradeKeyName = this->CopyData.M_FCardTransfer.M_ChangeGradeKeyName;
	//转职顺序字母
	TArray<FString> GradeNum = { TEXT("A"),TEXT("B"),TEXT("C") };

	//设置材料
	auto AddMaterials = [&](const int32& i) {
		FCardChangeMaterialType& CurData = CardData.FCardChangeMaterials[i];
		FCardChangeJobMaterial _Material;
		if (!UMaterialBaseStruct::GetMaterialSourceData<FCardChangeJobMaterial>(
			FString(ChangeKeyName + TEXT("[") +
				ChangeGradeKeyName + PBG_TEXT_CHANGE + GradeNum[i] +
				TEXT("]")),
			_Material,
			EMaterialType::E_CardChangeJobMaterial
		))
		{
			CurData.Visible = ESlateVisibility::Collapsed;
		}
		else {
			//查询成功
			CurData.Visible = ESlateVisibility::SelfHitTestInvisible;
			CurData.ChangeMaterialHeadPath = _Material.ItemTexturePath.ToString();
			CurData.ChangeMaterialName = FString(
				ChangeKeyName + TEXT("[") +
				ChangeGradeKeyName + PBG_TEXT_CHANGE +
				GradeNum[i] + TEXT("]")
			);
			CurData.PlayerBagIndex = UFVMGameInstance::GetPlayerStructManager_Static()->FindMaterialByName(
				CurData.ChangeMaterialName
			);
		}
		};

	//更具等级决定材料数量
	if (this->CopyData.M_FCardTransfer.M_ChangeGrade > 9)
	{
		//三种
		for (int32 i = 0; i < 3; i++)
		{
			AddMaterials(i);
		}
	}
	else {
		//两种
		CardData.FCardChangeMaterials[2].Visible = ESlateVisibility::Collapsed;
		for (int32 i = 0; i < 2; i++)
		{
			AddMaterials(i);
		}
	}

	//更新显示
	CardData.ShowStyle(this->M_UI_PlayerSynthesis->GetCardChangeFunction());


}

void UUI_PlayerBagCardGrid::AddSynthesisGoldCardUp()
{
	GAME_LOG(__FUNCTION__, TEXT("背包卡片UI-将次卡片数据添加到合成屋-金卡进化"), {
		GAME_LOG_LINE(TEXT("需要重写"));
		});

	if (!IsValid(this->M_UI_PlayerSynthesis))
	{
		return;
	}

	//获取金卡进化界面
	USynModel_GoldCardUp* Class = this->M_UI_PlayerSynthesis->GetGoldCardUpFunction();
	if (!IsValid(Class))
	{
		return;
	}

	FGoldCardUpCardType Data;
	Data.Grid = this;
	Data.UIGridIndex = this->GetUIIndex();
	Data.CardName = this->CopyData.ItemName;
	Data.CardHeadPath = this->CopyData.ItemTexturePath.ToString();
	Data.CardGrade = this->CopyData.M_CardGrade;

	const TMap<int32, FItemCard>& Cards = UFVMGameInstance::GetPlayerStructManager_Static()->FindCardByName(this->CopyData.ItemName);
	for (const auto& LCard : Cards)
	{
		if (LCard.Value.M_CardGrade == this->CopyData.M_CardGrade)
		{
			Data.PlayerBagIndex = LCard.Key;
			break;
		}
	}

	if (!Class->AddCardData(Data))
	{
		return;
	}
	this->GetButton()->SetIsEnabled(false);
	Class->UpdateListData();


		查询卡槽是否有空位

		int32 LResult = 0;
	//判断卡片名字是否已经填写
	for (auto& State : this->M_UI_PlayerSynthesis->M_GoldCard_Name_State)
	{
		if (!State.Equals(""))
		{
			LResult++;
		}
	}

	if (LResult >= 2)
	{
		UWidgetBase::CreateTipWidget(TEXT("无法选择,卡槽已满"));
		return;
	}

	//隐藏显示所有的空卡槽位置
	for (auto State = this->M_UI_PlayerSynthesis->M_GoldCard_Name_State.CreateConstIterator(); State; ++State)
	{
		if ((*State).Equals(""))
		{
			this->M_UI_PlayerSynthesis->M_GoldCard_Select_Button[State.GetIndex()]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	//将当前卡片设置为被选中状态
	this->GetButton()->SetIsEnabled(false);
	//隐藏其他的道具(因为材料不齐全所以需要隐藏)
	this->M_UI_PlayerSynthesis->M_GoldCardUpHead->SetVisibility(ESlateVisibility::Collapsed);
	this->M_UI_PlayerSynthesis->M_GoldCardUpResultHead->SetVisibility(ESlateVisibility::Collapsed);
	//关闭进化按钮(因为材料不齐全所以需要关闭进化按钮)
	this->M_UI_PlayerSynthesis->M_GoldCardUp_Butt->SetIsEnabled(false);


	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__, TEXT("当前金卡进化防御卡卡槽名称："));
	}

	//设置卡片卡槽名字
	for (auto State = this->M_UI_PlayerSynthesis->M_GoldCard_Name_State.CreateConstIterator(); State; ++State)
	{
		//判断当前卡槽是否是空的
		if (this->M_UI_PlayerSynthesis->M_GoldCard_Name_State[State.GetIndex()].Equals(""))
		{
			//添加名字
			this->M_UI_PlayerSynthesis->M_GoldCard_Name_State[State.GetIndex()] = this->GetFItemCardData()->ItemName;
			//设置卡槽
			UWidgetBase::SetButtonStyle(this->M_UI_PlayerSynthesis->M_GoldCard_Select_Button[State.GetIndex()], this->GetFItemCardData()->ItemTexturePath.ToString());
			//开启卡槽
			this->M_UI_PlayerSynthesis->M_GoldCard_Select_Button[State.GetIndex()]->SetVisibility(ESlateVisibility::Visible);
			//添加被消耗的卡片
			this->M_UI_PlayerSynthesis->M_NeedCards[State.GetIndex()] = this->GetFItemCardData()->ItemName;
			//添加卡片等级
			this->M_UI_PlayerSynthesis->M_NeedGradeCards[State.GetIndex()] = this->GetFItemCardData()->M_CardGrade;
			//添加卡片对应的UI索引
			this->M_UI_PlayerSynthesis->M_NeedCard_UI_Index[State.GetIndex()] = this->GetUIIndex();

			if (UFVMGameInstance::GetDebug())
			{
				UGameSystemFunction::FVMLog(__FUNCTION__, this->M_UI_PlayerSynthesis->M_GoldCard_Name_State[State.GetIndex()]);
			}

			break;
		}
	}

	//卡片名称合并查询->进化凭证
	TArray<FMaterial_GoldCardUp_Data> Datas;
	UGameSystemFunction::GetDataTableRows(
		TEXT("DataTable'/Game/Resource/BP/Data/EquipmentData/GoldCardUpData.GoldCardUpData'"),
		Datas
	);

	//最后从背包搜寻的金卡转职凭证
	FString LResultItemName = TEXT("");
	//对应的卡片名称
	FString LResultCardName = TEXT("");

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__, TEXT("当前金卡进化查询进化凭证："));
	}

	for (auto Data = Datas.CreateConstIterator(); Data; ++Data)
	{
		bool TLResult = true;

		FString StateResult = TEXT("");

		//搜寻凭证
		for (auto& State : this->M_UI_PlayerSynthesis->M_GoldCard_Name_State)
		{
			StateResult += State;
		}

		if (UFVMGameInstance::GetDebug())
		{
			FString Content;
			Content += TEXT("当前凭证名称:");
			Content += (*Data).M_GoldCardItemName;
			Content += TEXT(" 当前合成名字结果:");
			Content += StateResult;
			Content += TEXT(" 目标合成名称结果:");
			Content += (*Data).M_AddCardName;
			UGameSystemFunction::FVMLog(__FUNCTION__, Content);
		}

		//查询匹配最终结果
		if ((*Data).M_AddCardName.Equals(StateResult))
		{
			LResultItemName = (*Data).M_GoldCardItemName;
			LResultCardName = (*Data).M_GoldCardName;

			break;
		}
	}

	//没有任何结果，返回
	if (LResultItemName.Equals(""))
	{
		return;
	}

	//从背包搜索->玩家是否含有进化凭证
	if (FMaterialBase* LMaterialData = UFVMGameInstance::GetPlayerStructManager_Static()->GetMaterialByName(LResultItemName))
	{
		//如果从背包种查询失败，则直接返回
		if (!LMaterialData)
		{
			return;
		}

		//查询卡片【对应进化凭证的卡片】
		FItemCard CardData;
		//如果查询成功开启进化按钮
		if (UCardBaseStruct::SearchCardFromDataTable(LResultCardName, CardData))
		{
			UWidgetBase::SetImageBrush(this->M_UI_PlayerSynthesis->M_GoldCardUpResultHead, CardData.ItemTexturePath.ToString());
			this->M_UI_PlayerSynthesis->M_GoldCardUpResultHead->SetVisibility(ESlateVisibility::Visible);

			UWidgetBase::SetImageBrush(this->M_UI_PlayerSynthesis->M_GoldCardUpHead, LMaterialData->ItemTexturePath.ToString());

			this->M_UI_PlayerSynthesis->M_GoldCardUpHead->SetVisibility(ESlateVisibility::Visible);

			//开启进化按钮
			this->M_UI_PlayerSynthesis->M_GoldCardUp_Butt->SetIsEnabled(true);

			//赋予对应的数值
			this->M_UI_PlayerSynthesis->M_GoldCardUp_ItemName = LResultItemName;
			this->M_UI_PlayerSynthesis->M_GoldCardUp_CardName = LResultCardName;
		}
	}


}

void UUI_PlayerBagCardGrid::AddExchangeSplit_SplitSlot()
{
	GAME_LOG(__FUNCTION__, TEXT("背包卡片UI-将次卡片数据添加到兑换与分解-将卡片进行分解为兑换券"), {
		GAME_LOG_LINE(TEXT("需要重写"));
		});

	if (IsValid(this->M_UI_Other))
	{
		if (UUI_ExchangeAndSplit* const LExUI = Cast<UUI_ExchangeAndSplit>(this->M_UI_Other))
		{
			if (IsValid(LExUI->GetCurrentSelectButt()))
			{
				LExUI->GetCurrentSelectButt()->SetIsEnabled(true);
			}

			UItemDataTable* Data = NewObject<UItemDataTable>(this);
			Data->SetValue((FTableRowBase*)(&this->CopyData));
			LExUI->SetCurrentSlipItem(EItemType::E_Card, this->GetUIIndex(), Data, this->GetButton());
			this->GetButton()->SetIsEnabled(false);
		}
	}

}

*/