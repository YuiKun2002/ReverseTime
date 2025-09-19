// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/UI/Tips/UI_ShopPayTip.h"
#include "Game/UI/Tips/PayObject.h"
#include "Game/UI/UI_Shop.h"

#include <Components/Button.h>
#include <Components/EditableTextBox.h>
#include <Components/TextBlock.h>
#include <Components/HorizontalBox.h>

#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/GameConfigManager.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/PlayerDataSubsystem.h"

void UUI_ShopPayTip::SetItemPrice(const FItemPrice& ItemPrice) {

	this->PlayerPayButton->SetIsEnabled(true);

	this->PayObject = nullptr;

	this->ItemPriceData = ItemPrice;

	//设置默认的购买个数
	this->PlayerPayCountTextBox->SetText(FText::FromString(TEXT("1")));

	//如果没有设置支付类型，则使用默认支付
	if (IsValid(ItemPrice.PayType.Get()))
	{
		this->PayObject = ItemPrice.PayType.GetDefaultObject();
	}
	else {
		if (IsValid(ItemPrice.PayObject))
		{
			this->PayObject = ItemPrice.PayObject;
		}
		else {

			this->PayObject = NewObject<UPayObject>();

			//设置货币名称
			FCoin* Obj = GET_PLAYERDATAINS()->M_FPlayerCoin.GetCoinRef(ItemPrice.M_ItemPriceType);
			if (Obj)
			{
				this->SetPayCoinName(Obj->CoinName);
			}

			//设置价格
			this->SetPayPrice(FText::FromString(FString::FromInt(ItemPrice.M_ItemPrice * 1.f)));

			//获取物品，并且设置基本属性
			uint8 Type;
			FItemBaseStructData* Data = nullptr;
			FName Tag;
			if (GET_ITEM(ItemPrice.ItemID, Type, Data, Tag))
			{
				FItemBase ItemData;
				Data->GetSourceData(ItemData);
				this->SetPayTitle(FText::FromString(ItemData.ItemName));
				this->SetPayContent(FText::FromString(ItemData.ItemDescrible));
			}

			//检测支付(默认)
			this->CheckPay();
		}
	}
}

void UUI_ShopPayTip::SetInputHidden(bool bHidd) {
	if (bHidd)
	{
		this->InputBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	else {
		this->InputBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UUI_ShopPayTip::SetPayTitle(FText Title) {
	this->PayTitle->SetText(Title);
}

void UUI_ShopPayTip::SetPayContent(FText Content) {
	this->PayContent->SetText(Content);
}

void UUI_ShopPayTip::SetPayPrice(FText PriceText) {
	this->Price->SetText(PriceText);
}

void UUI_ShopPayTip::SetPayCoinName(FText Name) {
	this->CoinName->SetText(Name);
}

UPayObject* UUI_ShopPayTip::GetPayObject() {
	return this->PayObject;
}

void UUI_ShopPayTip::PayButton()
{

	//读取存档
	if (UGameSystemFunction::LoadCurrentPlayerData(__FUNCTION__ + FString(TEXT("准备进行支付操作"))))
	{
		if (IsValid(this->PayObject))
		{
			IPayInterface* Iterface = Cast<IPayInterface>(this->PayObject);
			if (Iterface)
			{
				if (Iterface->Execute_Pay(this->PayObject))
				{
					//保存数据
					UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("支付完成")));
				}
				else {
					UWidgetBase::CreateTipWidget(TEXT("支付失败"), FVector(1.f, 0.5f, 0.f));
				}
			}
		}
	}

	//移除界面
	this->RemoveFromParent();
}

UEditableTextBox* UUI_ShopPayTip::GetEditableTextBox()
{
	return this->PlayerPayCountTextBox;
}

UButton* UUI_ShopPayTip::GetInputTextButton()
{
	return this->PlayerInputButton;
}

void UUI_ShopPayTip::GetItemPrice(FItemPrice& OutItemPrice)const {
	OutItemPrice = this->ItemPriceData;
}

int32 UUI_ShopPayTip::GetPayItemCount() {
	return FCString::Atoi(*this->PlayerPayCountTextBox->GetText().ToString());
}

void UUI_ShopPayTip::InputNum(FString _value)
{
	if (this->_Arrays_CoinNums.Num() >= 4)
		return;

	this->_Arrays_CoinNums.Emplace(_value);

	//更新文字
	FString LText;
	for (auto Text : this->_Arrays_CoinNums)
	{
		LText.Append(Text);
	}

	this->PlayerPayCountTextBox->SetText(FText::FromString(LText));
}

void UUI_ShopPayTip::PopText()
{
	if (this->_Arrays_CoinNums.Num() == 0)
		return;


	this->_Arrays_CoinNums.Pop();

	//更新文字
	FString LText;
	for (auto Text : this->_Arrays_CoinNums)
	{
		LText.Append(Text);
	}

	this->PlayerPayCountTextBox->SetText(FText::FromString(LText));
}

void UUI_ShopPayTip::CheckPay()
{
	//获取购买个数或者背包索引
	int32 ItemCount = this->GetPayItemCount();

	//检测可购买的剩余容量
	int32 CheckCount = UGameSystemFunction::GetItemAddedCount(
		this->ItemPriceData.ItemID,
		this->ItemPriceData.ItemLevel,
		ItemCount * this->ItemPriceData.ItemCount
	);

	//如果超出范围，则自动调整购买次数
	if (CheckCount == 0)
	{
		this->PlayerPayButton->SetIsEnabled(false);
		ItemCount = 0;
		this->PlayerPayCountTextBox->SetText(FText::FromString(FString::FromInt(0)));
	}
	else if (CheckCount < 0)
	{
		//表示溢出,重新设置合适的购买数量
		ItemCount -= (CheckCount * -1.f) / this->ItemPriceData.ItemCount;
		this->PlayerPayCountTextBox->SetText(FText::FromString(FString::FromInt(ItemCount)));
	}

	//更新价格
	this->SetPayPrice(FText::FromString(FString::FromInt(ItemCount * this->ItemPriceData.M_ItemPrice)));
}

bool UUI_ShopPayTip::Initialize()
{
	if (Super::Initialize())
	{
		if (IsValid(this->PlayerPayButton))
		{
			FScriptDelegate CallFunc;
			CallFunc.BindUFunction(this, TEXT("PayButton"));
			this->PlayerPayButton->OnClicked.Add(MoveTempIfPossible(CallFunc));
		}

		return true;
	}

	return false;
}


