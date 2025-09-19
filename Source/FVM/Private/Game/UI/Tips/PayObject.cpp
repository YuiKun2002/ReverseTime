// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/UI/Tips/PayObject.h"
#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/Tips/UI_ShopPayTip.h"

bool UPayObject::Pay_Implementation() {

	if (this->GetPayUI()->IsInViewport())
	{
		//检测可购买的剩余容量
		this->GetPayUI()->CheckPay();

		FItemPrice Data;
		this->GetPayUI()->GetItemPrice(Data);

		//判断货币资源是否足够
		if (GET_PLAYERDATAINS()->CheckCoin(
			this->GetPayUI()->GetPayItemCount() * Data.M_ItemPrice,
			Data.M_ItemPriceType
		))
		{
			if (GET_PLAYERDATAINS()->ReduceCoin(
				this->GetPayUI()->GetPayItemCount() * Data.M_ItemPrice,
				Data.M_ItemPriceType
			))
			{
#if WITH_EDITOR
				UE_LOG(LogTemp, Warning,
					TEXT("Ptr：%p，Name：%s，支付成功！【ID：%d，Level：%d，Count：%d，Price：%d，Type：%d】"),
					this, *FString(__FUNCTION__), Data.ItemID, Data.ItemLevel,
					this->GetPayUI()->GetPayItemCount() * Data.ItemCount,
					this->GetPayUI()->GetPayItemCount() * Data.M_ItemPrice, Data.M_ItemPriceType
				);
#endif

				//发送道具
				if (UGameSystemFunction::SendItemToBag(
					Data.ItemID,
					Data.ItemLevel,
					this->GetPayUI()->GetPayItemCount() * Data.ItemCount
				)) {

					//支付成功
					this->PayResultDelegate.Broadcast(true);

					//购买成功
					FString Tip = TEXT("获得道具:") +
						FString::FromInt(Data.ItemID) +
						TEXT(" x") +
						FString::FromInt(this->GetPayUI()->GetPayItemCount() * Data.ItemCount);
					UWidgetBase::CreateTipWidget(Tip, FVector(1.f, 0.5f, 0.f));

					return true;
				}
				else {
#if WITH_EDITOR
					UE_LOG(LogTemp, Error,
						TEXT("Ptr：%p，Name：%s，道具发送失败，出现意外情况！【ID：%d，Level：%d，Count：%d，Price：%d，Type：%d】"),
						this, *FString(__FUNCTION__), Data.ItemID, Data.ItemLevel, this->GetPayUI()->GetPayItemCount(),
						this->GetPayUI()->GetPayItemCount() * Data.M_ItemPrice, Data.M_ItemPriceType
					);
#endif
					//返还支付的货币数量
					GET_PLAYERDATAINS()->AddCoin(
						this->GetPayUI()->GetPayItemCount() * Data.M_ItemPrice,
						Data.M_ItemPriceType
					);

					this->PayResultDelegate.Broadcast(false);
					return false;
				}
			}
		}
		else {
#if WITH_EDITOR
			UE_LOG(LogTemp, Error,
				TEXT("Ptr：%p，Name：%s，支付失败！货币数量不足！【ID：%d，Level：%d，Count：%d，Price：%d，Type：%d】"),
				this, *FString(__FUNCTION__), Data.ItemID, Data.ItemLevel, this->GetPayUI()->GetPayItemCount(),
				this->GetPayUI()->GetPayItemCount() * Data.M_ItemPrice, Data.M_ItemPriceType
			);
#endif
			this->PayResultDelegate.Broadcast(false);
			return false;
		}
	}

	this->PayResultDelegate.Broadcast(false);
	return false;
}

void UPayObject::CancelPay_Implementation() {

	if (this->GetPayUI()->IsInViewport())
	{
		FItemPrice Data;
		this->GetPayUI()->GetItemPrice(Data);
#if WITH_EDITOR
		UE_LOG(LogTemp, Warning,
			TEXT("Ptr：%p，Name：%s，支付取消！【ID：%d，Level：%d，Count：%d，Price：%d，Type：%d】"),
			this, *FString(__FUNCTION__), Data.ItemID, Data.ItemLevel, this->GetPayUI()->GetPayItemCount(),
			this->GetPayUI()->GetPayItemCount() * Data.M_ItemPrice, Data.M_ItemPriceType
		);
#endif
	}

	this->PayCancelDelegate.Broadcast();
}

UUI_ShopPayTip* UPayObject::GetPayUI() {
	if (IsValid(this->PayUI))
	{
		return this->PayUI;
	}

	this->PayUI = GET_UI_CAST(UUI_ShopPayTip, Global, Pay);

	return this->PayUI;
}

