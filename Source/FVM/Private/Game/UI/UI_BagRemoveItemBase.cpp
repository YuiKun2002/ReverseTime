// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/UI/UI_BagRemoveItemBase.h"

#include "Game/UI/UI_PlayerBagRemove.h"

#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/GameConfigManager.h"

#include <Components/Image.h>
#include <Components/Button.h>

void UUI_BagRemoveItemBase::SelectItems(bool _Tag)
{

}


void UUI_BagRemoveItemBase::SetUIPlayerBagRemove(UUI_PlayerBagRemove* _Class)
{
	this->M_ParentUI_Remove = _Class;
}


void UUI_BagRemoveItemBase::SetItemName(const FString& _ItemName)
{
	this->M_ItemName = _ItemName;
}

void UUI_BagRemoveItemBase::Select()
{
	if (this->M_bIsSelectItem)
	{
		//取消选择
		this->SetCancel();

		this->SelectItems(false);

		//向删除UI移出位置
		this->M_ParentUI_Remove->RemoveCurrentSelect(this->M_ItemIndex);
	}
	else {
		//选择
		this->SetSelect();

		this->SelectItems(true);

		//向删除UI申请位置
		this->M_ParentUI_Remove->AddCurrentSelect(this->M_ItemIndex, this->M_ItemName);
	}
}

void UUI_BagRemoveItemBase::Update()
{
	if (this->M_ParentUI_Remove->FindCurrentSelect(this->M_ItemIndex))
	{
		//选择
		this->SetSelect();

		this->SelectItems(true);
	}
	else {
		//取消选择
		this->SetCancel();

		this->SelectItems(false);
	}
}

bool UUI_BagRemoveItemBase::Initialize()
{
	if (!Super::Initialize())
		return false;

	this->M_SelectButton = this->GetWidgetComponent<UButton>(this, "Select");
	this->M_ShowSelectTag = this->GetWidgetComponent<UImage>(this, "SelecState_Image");

	M_SelectButtonText = FString(TEXT("取消选择"));
	return true;
}

void UUI_BagRemoveItemBase::SetTagItems(int32 _ItemIndex, uint8 _PanelNumber)
{
	this->M_ItemIndex = _ItemIndex;
	this->M_PanelNumber = _PanelNumber;
}

void UUI_BagRemoveItemBase::SetSelect()
{
	//选择
	this->M_bIsSelectItem = true;
	UWidgetBase::SetImageBrush(this->M_ShowSelectTag, "Texture2D'/Game/Resource/Texture/UI/Game/PlayerBag/T_PB_13.T_PB_13'");
	M_SelectButtonText = FString(TEXT("取消选择"));
}

void UUI_BagRemoveItemBase::SetCancel()
{
	this->M_bIsSelectItem = false;
	this->M_ShowSelectTag->SetVisibility(ESlateVisibility::Hidden);
	M_SelectButtonText = FString(TEXT("选择"));
}

UButton* UUI_BagRemoveItemBase::GetSelectButton()
{
	return this->M_SelectButton;
}

bool UUI_BagRemoveItemBase::GetCurrentSelect()
{
	return this->M_bIsSelectItem;
}

void UUI_BagRemoveItemBase::BindSelectFunc()
{
	FScriptDelegate Func;
	Func.BindUFunction(this, "Select");
	this->M_SelectButton->OnClicked.Add(Func);
}
