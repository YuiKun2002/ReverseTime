// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/UI/UI_Mail_GiftBox.h"
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include "Data/Texture/TextureDataAssetCache.h"

void UUI_Mail_GiftBox::OnClicked_Implementation() {

	FName OutName;
	uint8 OutType = 0U;
	FItemBaseStructData* BasePtr = nullptr;
	if (GET_ITEM(this->ID, OutType, BasePtr, OutName))
	{
		this->SetFuncObject(NewObject<UItemBaseGridFunc>(), BasePtr);
	}
}

void UUI_Mail_GiftBox::SetPlayerGetItemBox_Item(FText fItemName, int32 iItemID, int32 iItemLevel, int32 iItemCount)
{

	this->ID = iItemID;

	this->ItemName->SetText(fItemName);

	this->ItemCount->SetText(FText::FromString(FString::FromInt(iItemCount)));

	UTextureDataAssetCache* Cache = GET_CACHE(Texture);
	if (iItemID != -1 && iItemLevel > 0)
	{
		this->ItemLevel_Img->SetRenderOpacity(1.f);
		this->ItemLevel_Img->SetBrushFromTexture(Cache->GetItemLevelTexture(iItemLevel), true);
		this->ItemHead->SetBrushFromTexture(Cache->GetItemTexture(iItemID), true);
	}
	else if (iItemID == -1 && iItemLevel >= 0)
	{
		this->ItemLevel_Img->SetRenderOpacity(0.f);
		this->ItemHead->SetBrushFromTexture(Cache->GetCoinTexture(iItemLevel), true);
	}
	else {
		this->ItemLevel_Img->SetRenderOpacity(0.f);
		this->ItemHead->SetBrushFromTexture(Cache->GetItemTexture(iItemID), true);
	}

	if (this->ItemHead->GetBrush().GetImageSize().Y > 60)
	{
		//this->ItemHead->Brush.SetImageSize(this->ItemHead->Brush.GetImageSize() * 0.7f);

		this->ItemHead->SetDesiredSizeOverride(
			FVector2D(
				this->ItemHead->GetBrush().GetImageSize().X * 0.7f,
				this->ItemHead->GetBrush().GetImageSize().Y * 0.7f
			)
		);

	}

	FItemViewBlock Block;
	this->SetItemViewBlock(&Block, Cast<UTexture2D>(this->ItemHead->GetBrush().GetResourceObject()), 0);
}