// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Tips/UI_PlayerGetItemBox.h"
#include "GameSystem/GameUserInterfaceSubsystem.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "Data/Texture/TextureDataAssetCache.h"
#include "GameSystem/Item/ItemBaseStruct.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include <Components/VerticalBox.h>
#include <Components/TextBlock.h>
#include <Components/RichTextBlock.h>
#include <Components/Image.h>

void UUI_PlayerGetItemBox_Item::RemoveItem() {

	if (this->OnRemoveDelegate.IsBound())
	{
		this->OnRemoveDelegate.Broadcast();
	}
}


void UUI_PlayerGetItemBox_Item::SetPlayerGetItemBox_Item(FText fItemName, int32 iItemID, int32 iItemLevel, int32 iItemCount)
{
	this->ItemName->SetText(fItemName);

	this->ItemCount->SetText(FText::FromString(FString::FromInt(iItemCount)));



	UTextureDataAssetCache* Cache = GET_CACHE(Texture);

	if (iItemID != -1 && iItemLevel > 0)
	{
		this->ItemLevel->SetRenderOpacity(1.f);
		this->ItemLevel->SetBrushFromTexture(Cache->GetItemLevelTexture(iItemLevel), true);
		this->ItemHead->SetBrushFromTexture(Cache->GetItemTexture(iItemID), true);
	}
	else if (iItemID == -1 && iItemLevel >= 0)
	{
		this->ItemLevel->SetRenderOpacity(0.f);
		this->ItemHead->SetBrushFromTexture(Cache->GetCoinTexture(iItemLevel), true);
	}
	else {
		this->ItemLevel->SetRenderOpacity(0.f);
		this->ItemHead->SetBrushFromTexture(Cache->GetItemTexture(iItemID), true);
	}

	if (this->ItemHead->GetBrush().GetImageSize().Y > 60)
	{
		this->ItemHead->SetDesiredSizeOverride(
			FVector2D(
				this->ItemHead->GetBrush().GetImageSize().X * 0.7f,
				this->ItemHead->GetBrush().GetImageSize().Y * 0.7f
			)
		);
	}
}

void UUI_DesTip::SetContent(FString Cont) {
	this->Content->SetText(FText::FromString(Cont));
}

UUI_PlayerGetItemBox* UUI_PlayerGetItemBox::GetPlayerGetItemBoxTipWidget() {
	return GameDataStaticObject<UPlayerGetItemBox>()->GetBox();
}

void UUI_PlayerGetItemBox::AddItemName(FText fItemName, int32 iItemID, int32 iItemLevel, int32 iItemCount) {

	UUI_PlayerGetItemBox_Item* Item = nullptr;

	if (this->Items_Cache.Num())
	{
		int32 Index = this->Items_Cache.Num() - 1;
		Item = this->Items_Cache[Index];
		this->Items_Cache.RemoveAt(Index);
	}
	else {
		Item = CREATE_UI(this, UUI_PlayerGetItemBox_Item, Tip, PlayerItemTipBoxItem);
	}

	//设置属性
	Item->SetPlayerGetItemBox_Item(fItemName, iItemID, iItemLevel, iItemCount);

	if (!Item->OnRemoveDelegate.IsBound())
	{
		Item->OnRemoveDelegate.AddDynamic(this, &UUI_PlayerGetItemBox::OnRemoved);
	}

	//新增
	this->Items_Queue.Emplace(Item);

	//显示
	this->Show();
}

void UUI_PlayerGetItemBox::AddItem(int32 iItemID, int32 iItemLevel, int32 iItemCount) {

	uint8 type = 0U;
	FName Name = FName();
	FItemBaseStructData* ItemPtr = nullptr;
	if (GET_ITEM(iItemID, type, ItemPtr, Name))
	{
		FItemBase Base;
		if (ItemPtr->GetSourceData(Base))
		{
			this->AddItemName(FText::FromString(Base.ItemName), iItemID, iItemLevel, iItemCount);
		}
	}
}

void UUI_PlayerGetItemBox::OnRemoved() {

	if (this->Items_Queue.Num() == 0 && this->ListBox->GetChildrenCount() == 0)
	{
		this->RemoveFromParent();
		this->bDelayAnim = false;
		return;
	}

	//移除第一位
	if (this->ListBox->GetChildrenCount())
	{
		//放入缓存
		this->Items_Cache.Emplace(Cast<UUI_PlayerGetItemBox_Item>(this->ListBox->GetChildAt(0)));
		this->ListBox->RemoveChildAt(0);

		//如果列表中还有项目
		if (this->Items_Queue.Num())
		{
			//移除过后，如果列表小于5个，则使用延迟动画
			if (this->ListBox->GetChildrenCount() < 5)
			{
				this->bDelayAnim = true;
			}
			else {
				//移除过后，如果列表等于5个，则使用延迟动画
				this->bDelayAnim = false;
				this->fTime = 0.f;
				this->ListBox->AddChildToVerticalBox(this->Items_Queue[0]);
				//从队列移除
				this->Items_Queue.RemoveAt(0);
			}
		}
	}

	if (this->Items_Queue.Num() == 0 && this->ListBox->GetChildrenCount() == 0)
	{
		this->RemoveFromParent();
		this->bDelayAnim = false;
		return;
	}
}

void UUI_PlayerGetItemBox::Show() {

	//如果列表数量小于7，并且显示列表也小于6
	if (this->ListBox->GetChildrenCount() < 6)
	{
		//使用延迟播放
		this->bDelayAnim = true;
	}

	if (!this->IsInViewport())
	{
		//获取视口缩放
		FVector2D Vc2 = GetWorld()->GetGameViewport()->Viewport->GetSizeXY();

		//设置界面大小位置
		this->SetPositionInViewport(FVector2D(
			0.f, (Vc2.Y * 0.6) * GetWorld()->GetGameViewport()->GetDPIScale()
		));

		//显示
		this->AddToViewport(9999999);
	}
}

void UUI_PlayerGetItemBox::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {

	Super::NativeTick(MyGeometry, InDeltaTime);

	if (this->bDelayAnim)
	{
		if (this->fTime < this->fConstTime) {
			this->fTime += InDeltaTime;
		}
		else {
			if (this->Items_Queue.Num())
			{
				this->fTime = 0.f;

				this->ListBox->AddChildToVerticalBox(this->Items_Queue[0]);

				this->Items_Queue.RemoveAt(0);
			}
			else {
				this->bDelayAnim = false;
			}
		}
	}
}

UUI_PlayerGetItemBox* UPlayerGetItemBox::GetBox() {
	if (IsValid(this->Box))
	{
		return this->Box;
	}

	this->Box = GET_UI_CAST(UUI_PlayerGetItemBox, Tip, PlayerItemTipBox);

	return this->Box;
}