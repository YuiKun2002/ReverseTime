// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/ItemBaseGridFunc.h"
#include "Game/UI/ItemGridBase.h"
#include "Game/UI/Tips/UI_ItemDesTip.h"
#include "Game/UI/Tips/UI_CardItemTip.h"
#include "GameSystem/Item/ItemBaseStruct.h"


void UItemBaseGridFunc::Execute(
	UItemGridBase* GridPtr,
	FItemBaseStructData* ItemBaseDataPtr
) {
	if (ItemBaseDataPtr)
	{
		FItemBase BaseData;
		if (ItemBaseDataPtr->GetSourceData(BaseData))
		{
			//创建物品提示按钮
			if (!IsValid(this->TipComp))
			{
				this->TipComp = GET_UI_CAST(UUI_ItemDesTip, PlayerBag, ItemTip);
			}

			this->TipComp->SetContentTitleText(BaseData.ItemName);
			this->TipComp->SetContentText(BaseData.ItemDescrible);

			//设置数量
			UItemCountGridBase* CountGrid = Cast<UItemCountGridBase>(GridPtr);
			if (IsValid(CountGrid))
			{
				if (CountGrid->GetItemCountViewBlock().Count == 0)
				{
					this->TipComp->SetItemCount(1);
				}
				else {
					this->TipComp->SetItemCount(CountGrid->GetItemCountViewBlock().Count);
				}
			}
			else {
				this->TipComp->SetItemCount(0);
			}

			//设置头像
			if (IsValid(GridPtr->GetItemHeadTextureIns()))
			{
				this->TipComp->SetItemHead(Cast<UTexture2D>(GridPtr->GetItemHeadTextureIns()));
			}

			//折叠取消一个按钮
			this->TipComp->SetHidden(true, false);

			this->TipComp->AddToViewport(1);
		}
	}
}

UUI_ItemDesTip* UItemBaseGridFunc::GetTipWidget() {
	return this->TipComp;
}

void UItemBaseGridFunc::SetTipWidget(UUI_ItemDesTip* TipWidgetInstance) {

	//释放之前的UI
	if (IsValid(this->TipComp))
	{
		this->TipComp->UnloadWidget(true);
		this->TipComp = nullptr;
	}

	this->TipComp = TipWidgetInstance;
}

void UItemCardBaseGridFunc::Execute(
	UItemGridBase* GridPtr,
	FItemBaseStructData* ItemBaseDataPtr
) {
	if (ItemBaseDataPtr)
	{
		FItemBase BaseData;
		if (ItemBaseDataPtr->GetSourceData(BaseData))
		{
			//创建物品提示按钮
			if (!IsValid(this->TipComp))
			{
				this->TipComp = GET_UI_CAST(UUI_ItemDesTip, PlayerBag, ItemCardTip);
			}

			this->TipComp->SetContentTitleText(BaseData.ItemName);
			this->TipComp->SetContentText(BaseData.ItemDescrible);

			if (!IsValid(this->CardGrid))
			{
				//设置数量
				this->CardGrid = Cast<UItemCountGridBase>(GridPtr);

				if (IsValid(this->CardGrid))
				{
					this->TipComp->SetItemCount(this->CardGrid->GetItemCountViewBlock().Count);
				}
				else {
#if WITH_EDITOR
					ensureMsgf(false, TEXT("This Ptr Param \"GridPtr\" Must be a derived class of UItemCountGridBase type"));
#endif
					return;
				}
			}
			else {
				this->TipComp->SetItemCount(this->CardGrid->GetItemCountViewBlock().Count);
			}

			//设置头像
			if (IsValid(GridPtr->GetItemHeadTextureIns()))
			{
				this->TipComp->SetItemHead(Cast<UTexture2D>(GridPtr->GetItemHeadTextureIns()));
			}

			//折叠取消一个按钮
			this->TipComp->SetHidden(true, false);

			this->TipComp->AddToViewport(1);
		}
	}
}

UUI_CardItemTip* UItemCardBaseGridFunc::GetCardTipWidget() {
	return Cast<UUI_CardItemTip>(this->GetTipWidget());
}
