// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Tips/UI_GiftItemTip.h"
#include "GameSystem/GameUserInterfaceSubsystem.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "Data/Texture/TextureDataAssetCache.h"
#include <Components/CanvasPanelSlot.h>
#include "Kismet/KismetTextLibrary.h"
#include <Components/VerticalBox.h>
#include <Components/TextBlock.h>
#include <Components/Button.h>
#include <Components/Image.h>

void UUI_GiftItemTip_Item::SetGiftItemItemData(
	int32 iItemID,
	int32 iItemLevel,
	FText tItemName,
	int32 iItemCount,
	float fRandomValue
) {
	//货币类型
	if (iItemID == -1 && iItemLevel != -1)
	{
		//物品类型
		this->ItemHead->SetBrushFromSoftTexture(GET_CACHE(Texture)->GetCoinTexture(iItemLevel), true);
		//隐藏等级
		this->ItemLevel->SetRenderOpacity(0.f);
	}
	else {
		//物品类型
		this->ItemHead->SetBrushFromSoftTexture(GET_CACHE(Texture)->GetItemTexture(iItemID), true);
		if (this->ItemHead->GetBrush().GetImageSize().Y > 60)
		{
			//this->ItemHead->Brush.SetImageSize();

			this->ItemHead->SetDesiredSizeOverride(FVector2D(
				this->ItemHead->GetBrush().GetImageSize().X * 0.7f,
				this->ItemHead->GetBrush().GetImageSize().Y * 0.7f
			));
		}
		//等级
		if (iItemLevel == 0) {
			this->ItemLevel->SetRenderOpacity(0.f);
		}
		else {
			this->ItemLevel->SetRenderOpacity(1.f);
			this->ItemLevel->SetBrushFromSoftTexture(GET_CACHE(Texture)->GetItemLevelTexture(iItemLevel));
		}
	}

	//名称
	this->ItemName->SetText(tItemName);
	//数量
	this->ItemCount->SetText(FText::FromString(FString::FromInt(iItemCount)));

	//如果整数位为0，则表示概率为 0.xx %   ，小数位则往后
	if (((int32)(fRandomValue)) == 0)
	{
		//设置概率
		this->ItemValue->SetText(
			UKismetTextLibrary::Conv_DoubleToText(
				fRandomValue,
				ERoundingMode::HalfToEven,
				false, true, 1, 324, 1, 3
			)
		);
	}
	else {
		//设置概率
		this->ItemValue->SetText(
			UKismetTextLibrary::Conv_DoubleToText(
				fRandomValue,
				ERoundingMode::HalfToEven,
				false, true, 1, 324, 0, 1
			)
		);
	}


}

void UUI_GiftItemTip::SetItemCountMax(int32 MaxCount) {
	this->ItemCountMax = MaxCount;
}

void UUI_GiftItemTip::SetTipContent(FString RechTextContent) {
	this->TipContent = RechTextContent;
}

void UUI_GiftItemTip::AddTipContent(FString RechTextContent) {
	this->TipContent.Append(RechTextContent);
}

void UUI_GiftItemTip::AddGiftItemTipItem(UUI_GiftItemTip_Item* GiftItemTip_Item) {
	this->GetShowListBox()->AddChildToVerticalBox(GiftItemTip_Item);
}

void UUI_GiftItemTip::SetEquipmentFuncGiftObject(UObject* EquipmentFuncGiftIns) {
	this->FuncGiftIns = nullptr;
	this->FuncGiftIns = EquipmentFuncGiftIns;
	this->TipItems.Append(this->GetShowListBox()->GetAllChildren());
	this->GetShowListBox()->ClearChildren();
}

UObject* UUI_GiftItemTip::GetEquipmentFuncGiftObject() {
	return this->FuncGiftIns;
}

int32 UUI_GiftItemTip::GetUseCount() {
	return this->UseCount;
}

UUI_GiftItemTip_Item* UUI_GiftItemTip::GetGiftItemTipItem() {

	UUI_GiftItemTip_Item* Item = nullptr;
	if (this->TipItems.Num())
	{
		Item = Cast<UUI_GiftItemTip_Item>(this->TipItems[0]);
		this->TipItems.RemoveAtSwap(0);
		return Item;
	}

	Item = CREATE_UI(this, UUI_GiftItemTip_Item, PlayerBag, GiftItemTip_Item);
	return Item;
}

void UUI_GiftItemTip::GetTipContent(FString& OutStr) {
	OutStr = this->TipContent;
}

int32 UUI_GiftItemTip::GetItemCount() {
	return this->ItemCountMax;
}

void UUI_GiftItemTip::UpdateUseCount(const int32& Count) {
	if (Count == 0)
	{
		this->UseCount = 1;
	}
	else
	{
		this->UseCount = Count;
	}
}