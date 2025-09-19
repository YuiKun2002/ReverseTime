// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/UI/UI_Mail.h"
#include "Data/GameLogSubsystem.h"
#include "Game/UI/UI_Mail_GiftBox.h"
#include "Game/UI/UI_Mail_MessageBox.h"
#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/Tips/UI_PlayerGetItemBox.h"

#include <Components/Image.h>
#include <Components/Button.h>
#include <Components/TextBlock.h>
#include <Components/VerticalBox.h>
#include <Components/HorizontalBox.h>
#include <Components/MultiLineEditableText.h>

void UUI_Mail::GetMail() {

	if (this->CurMailNum <= 0)
	{
		return;
	}

	FString NewCmd = Cmd.ToLower();
	UGameLogSubsystem* Cur = UGameplayStatics::GetGameInstance(
		GAMEINS()->GetWorld()
	)->GetSubsystem<UGameLogSubsystem>();
	if (IsValid(Cur))
	{
		Cur->ExeInstructions(NewCmd);
	}

	//所有获得的物品
	TMap<int32, TMap<int32, int32>> Items;
	//所有获得的货币
	TMap<int32, FItemViewBlock> Coins;


	UPlayerStructManager* Player = GET_PLAYERDATAINS();

	//发送列表道具
	for (const auto& ListItem : this->ListData)
	{
		if (ListItem.ItemID == -1 && ListItem.ItemLevel != -1)
		{
			FItemViewBlock* Block = Coins.Find(ListItem.ItemLevel);
			if (Block)
			{
				Block->BlockIndex += ListItem.BlockIndex * this->CurMailNum;
			}
			else {
				Coins.Emplace(ListItem.ItemLevel,
					FItemViewBlock{ ListItem.ItemID,
					ListItem.ItemLevel,
					ListItem.BlockIndex * this->CurMailNum });
			}

		}
		else {
			TMap<int32, int32>* TBlock = Items.Find(ListItem.ItemID);
			if (TBlock)
			{
				int32* Count = TBlock->Find(ListItem.ItemLevel);
				if (Count)
				{
					*Count += ListItem.BlockIndex * this->CurMailNum;
				}
				else {
					TBlock->Emplace(ListItem.ItemLevel, ListItem.BlockIndex * this->CurMailNum);
				}
			}
			else {
				TMap<int32, int32> TpBlock;
				TpBlock.Emplace(ListItem.ItemLevel, ListItem.BlockIndex * this->CurMailNum);
				Items.Emplace(ListItem.ItemID, TpBlock);
			}
		}
	}


	//发送道具
	for (const auto& Item : Items)
	{
		for (const auto& ItemLevel : Item.Value)
		{
			UGameSystemFunction::SendItemToBag(
				Item.Key,
				ItemLevel.Key,
				ItemLevel.Value,
				0U, false);

			UUI_PlayerGetItemBox::GetPlayerGetItemBoxTipWidget()->AddItem(Item.Key, ItemLevel.Key, ItemLevel.Value);
		}
	}


	//发送货币
	for (const auto& Item : Coins)
	{
		Player->AddCoin(Item.Value.BlockIndex, Item.Value.ItemLevel);

		UUI_PlayerGetItemBox::GetPlayerGetItemBoxTipWidget()->AddItemName(
			Player->GetCoin(Item.Value.ItemLevel).CoinName,
			-1, Item.Value.ItemLevel, Item.Value.BlockIndex);

	}

	Player->GetMailList().Remove(this->MailID);

	UGameSystemFunction::SaveCurrentPlayerData(TEXT("邮件领取"));

	this->LoadList();
}

void UUI_Mail::LoadList()
{

	this->UpdateMailGetState(false);

	if (!IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return;
	}

	UPlayerStructManager* Player = UFVMGameInstance::GetFVMGameInstance()->GetPlayerStructManager();
	if (!IsValid(Player))
	{
		return;
	}

	TMap<int32, FMailItemSave>& Mails = Player->GetMailList();


	int32 Index = 0;
	int32 Max = 30;//最大显示30封邮件
	int32 ChildCount = this->MailVerticalList->GetChildrenCount();
	for (const auto& Data : Mails)
	{
		//跳出
		if (Index >= Max)
		{
			break;
		}

		if (Index < ChildCount)
		{
			//使用列表中的对象
			UUI_Mail_MessageBox* Box = Cast<UUI_Mail_MessageBox>(this->MailVerticalList->GetChildAt(Index));
			Box->SetMailMessageBox(Data.Key, Data.Value);
		}
		else {
			//新增对象
			UUI_Mail_MessageBox* Box = CREATE_UI(this, UUI_Mail_MessageBox, Mail, MessageBox);
			Box->SetMailMessageBox(Data.Key, Data.Value);
			this->MailVerticalList->AddChildToVerticalBox(Box);
		}

		Index++;
	}
	//索引结束表示当前邮件数量
	this->CurMailCount = Index;
	this->CurBoxCount = this->MailVerticalList->GetChildrenCount();

	UGameSystemFunction::ClearTimeClip(this->GetWorld()->GetTimerManager(), this->M_FTimeClip_0);

	this->GetWorld()->GetTimerManager().SetTimer(this->M_FTimeClip_0.M_FTimerHandle, [&]() {

		//判断索引是否有效
		if (this->M_FTimeClip_0.M_Count < this->CurBoxCount)
		{

			if (this->M_FTimeClip_0.M_Count < this->CurMailCount)
			{
				//显示
				UUI_Mail_MessageBox* Box = Cast<UUI_Mail_MessageBox>(
					this->MailVerticalList->GetChildAt(this->M_FTimeClip_0.M_Count)
				);
				Box->InitData();
				Box->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else {
				//影藏
				this->MailVerticalList->GetChildAt(this->M_FTimeClip_0.M_Count)->
					SetVisibility(ESlateVisibility::Collapsed);
			}


			this->M_FTimeClip_0.M_Count++;
		}
		else {

			//影藏所有的奖励
			for (auto& Widget : this->MailGiftContentVerticalList->GetAllChildren())
			{
				Widget->SetVisibility(ESlateVisibility::Collapsed);
			}

			//选择第一个邮件
			if (this->CurMailCount > 0 && this->CurBoxCount > 0)
			{
				UUI_Mail_MessageBox* Box = Cast<UUI_Mail_MessageBox>(
					this->MailVerticalList->GetChildAt(0)
				);
				Box->SelectMail();
				this->UpdateMailGetState(true);
			}

			UGameSystemFunction::ClearTimeClip(this->GetWorld()->GetTimerManager(), this->M_FTimeClip_0);
		}
		}, 0.02f, true);

	if (this->CurMailCount == 0)
	{
		this->MailTheme_Text->SetText(FText::FromString(TEXT("美食大战老鼠-逆转时光")));

		this->MailContent_Text->SetText(FText::FromString(TEXT("")));

		this->UpdateMailGiftContentState(false);
	}
}

void UUI_Mail::SelectMail(int32 ID, int32 MailCount, const FMail& Data) {

	this->CurMailNum = MailCount;
	this->MailID = ID;

	UPlayerStructManager* Player = UFVMGameInstance::GetFVMGameInstance()->GetPlayerStructManager();

	FMailItemSave* Save = Player->GetMailList().Find(ID);
	if (Save)
	{
		Save->bRead = true;
	}

	//名称
	this->RecvPlayerName_Text->SetText(FText::FromString(Player->M_PlayerName));
	//主题
	this->MailTheme_Text->SetText(FText::FromString(Data.MailTheme));
	//内容
	this->MailContent_Text->SetText(FText::FromString(Data.MailContent));
	//奖励显示
	if (Data.ShowList.Items.Num())
	{
		this->ListData.Empty();
		Data.ShowList.Items.GenerateValueArray(this->ListData);
		this->ShowGiftContent();
		this->UpdateMailGiftContentState(true);
	}
	else {
		this->UpdateMailGiftContentState(false);
	}

	this->UpdateMailGetState(true);
}

void UUI_Mail::ShowGiftContent() {

	int32 i = 0;
	int32 Count = this->MailGiftContentVerticalList->GetChildrenCount();
	for (const auto& ListItem : this->ListData)
	{
		if (i < Count)
		{
			UUI_Mail_GiftBox* Item = Cast<UUI_Mail_GiftBox>(this->MailGiftContentVerticalList->GetChildAt(i));
			Item->SetPlayerGetItemBox_Item(
				ListItem.ItemName,
				ListItem.ItemID,
				ListItem.ItemLevel,
				ListItem.BlockIndex * this->CurMailNum);
			Item->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else {
			//加载礼盒项目UI UUI_GiftItemTip_Item
			UUI_Mail_GiftBox* Item = CREATE_UI(this, UUI_Mail_GiftBox, Mail, GiftBox);
			Item->SetPlayerGetItemBox_Item(
				ListItem.ItemName,
				ListItem.ItemID,
				ListItem.ItemLevel,
				ListItem.BlockIndex * this->CurMailNum);
			this->MailGiftContentVerticalList->AddChildToVerticalBox(Item);
		}
		i++;
	}
}

void UUI_Mail::CloseMailPanel()
{
	//更新邮件提示
	UUI_MainFramework* Frame = GET_UI_CAST(UUI_MainFramework, Global, MainFrame);
	if (IsValid(Frame) && Frame->IsInViewport())
	{
		Frame->ShowMailTip();
	}
	UGameSystemFunction::ClearTimeClip(this->GetWorld()->GetTimerManager(), this->M_FTimeClip_0);
	this->RemoveFromParent();
}