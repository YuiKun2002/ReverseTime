// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/ItemGridBase.h"
#include <Components/Image.h>
#include <Components/Button.h>
#include <Components/TextBlock.h>
#include "GameSystem/FVMGameInstance.h"
#include "Game/UI/Tips/UI_ItemTitleTip.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/Item/ItemBaseGridFunc.h"
#include "Data/Texture/TextureDataAssetCache.h"


FReply UItemGridBase::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

#if PLATFORM_WINDOWS

	UUI_ItemTitleTip::ShowWidgetTip(this, true);

#endif

	return SuperReply;
}

void UItemGridBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) {

	Super::NativeOnMouseLeave(InMouseEvent);

#if PLATFORM_WINDOWS

	UUI_ItemTitleTip::ShowWidgetTip(this, false);

#endif
}

bool UItemGridBase::Initialize() {
	if (Super::Initialize())
	{
		this->SetBind();

		if (IsValid(this->ItemLevel_Img))
		{
			this->ItemLevel_Img->SetVisibility(ESlateVisibility::Hidden);
		}

		return true;
	}
	return false;
}

FString UItemGridBase::ToString_Implementation() {
	return FString();
}

void UItemGridBase::OnClicked_Implementation() {}

void UItemGridBase::SetBind() {

	if (!IsValid(this->Item_Butt))
	{
		return;
	}

	//绑定按钮点击
	if (!this->Item_Butt->OnClicked.IsBound())
	{
		//绑定函数 点击之后显示细节面板
		FScriptDelegate CallFunc, ClickAudio;
		CallFunc.BindUFunction(this, TEXT("OnClicked"));
		ClickAudio.BindUFunction(this, TEXT("PlayOperateAudioDef"));
		this->Item_Butt->OnClicked.Add(MoveTemp(CallFunc));
		this->Item_Butt->OnClicked.Add(MoveTemp(ClickAudio));
	}

#if PLATFORM_WINDOWS

	//绑定鼠标从当前按钮移入时，触发音效
	if (!this->Item_Butt->OnHovered.IsBound())
	{
		FScriptDelegate Hover;
		Hover.BindUFunction(this, TEXT("MouseHoveredAudio"));
		this->Item_Butt->OnHovered.Add(MoveTemp(Hover));
	}
#endif

}

bool UItemGridBase::SetItemViewBlock(
	const FItemViewBlock* ItemViewBlock,
	class UTexture2D* ItemHeadTexture,
	int32 Index
) {
	if (ItemViewBlock == nullptr)
	{
		return false;
	}
	this->SetLevelAndButtonImage(ItemViewBlock->ItemLevel, ItemHeadTexture);
	this->ItemViewBlockBaseData = *ItemViewBlock;
	this->UiIndex = Index;
	return true;
}

void UItemGridBase::SetItemViewBlockCopy(UItemGridBase* CopyGrid) {
	this->SetItemViewBlock(
		&CopyGrid->GetItemViewBlock(), Cast<UTexture2D>(CopyGrid->ItemHeadTextureIns), this->GetUiIndex()
	);
}

void UItemGridBase::SetItemAttribute(UItemBaseView* View, const FItemViewBlock& BlockData, int32 Index) {
	FItemBaseViewBlock* ViewBlockData = CAST_TYPE_PTR(FItemBaseViewBlock,
		View->GetItem(BlockData.ItemID, BlockData.ItemLevel));
	this->SetItemViewBlock(&BlockData, ViewBlockData->Texture, Index);
	this->OnViewUpdate();
}

void UItemGridBase::SetItemAttributeCopy(UItemGridBase* ItemGrid) {
	this->SetItemViewBlockCopy(ItemGrid);
	this->OnViewUpdate();
}

void UItemGridBase::SetLevelAndButtonImage(int32 Level, UTexture2D* Texture) {

	//等级更新
	if (this->ItemViewBlockBaseData.ItemLevel != Level)
	{
		//更新等级显示
		if (Level <= 0)
		{
			if (this->bLevelVisiable)
			{
				this->ItemLevel_Img->SetVisibility(ESlateVisibility::Hidden);
				this->bLevelVisiable = false;
			}
		}
		else {
			if (!this->bLevelVisiable)
			{
				this->ItemLevel_Img->SetVisibility(ESlateVisibility::HitTestInvisible);
				this->bLevelVisiable = true;
			}

			this->UpdateLevelTexture(
				GET_CACHE(Texture)->GetItemLevelTexture(
					Level
				)
			);
		}
	}

	//按钮头像更新
	if (this->ItemHeadTextureIns != Cast<UObject>(Texture))
	{
		this->ItemHeadTextureIns = Cast<UObject>(Texture);

		//资源
		FSlateBrush Brush;
		Brush.SetResourceObject(Texture);

		//设置按钮不同事件时图片颜色深浅
		FButtonStyle Style;

		//如果预设的是不做图形处理
		if (this->Item_Butt->GetStyle().Normal.GetDrawType() == ESlateBrushDrawType::Type::NoDrawType)
		{
			//则无需修改
			Style.SetNormal(this->Item_Butt->GetStyle().Normal);
		}
		else {
			if (this->bDarkColor)
			{
				Brush.TintColor = this->Item_Butt->GetStyle().Normal.TintColor;
				Style.SetNormal(Brush);
			}
			else {
				Brush.TintColor = FLinearColor(FVector4(1.f, 1.f, 1.f, 1.f));
				Style.SetNormal(Brush);
			}
		}

		if (this->Item_Butt->GetStyle().Hovered.GetDrawType() == ESlateBrushDrawType::Type::NoDrawType)
		{
			Style.SetHovered(this->Item_Butt->GetStyle().Hovered);
		}
		else {
			Brush.TintColor = FLinearColor(FVector4(.7f, .7f, .7f, 1.f));
			Style.SetHovered(Brush);
		}

		if (this->Item_Butt->GetStyle().Pressed.GetDrawType() == ESlateBrushDrawType::Type::NoDrawType)
		{
			Style.SetPressed(this->Item_Butt->GetStyle().Pressed);
		}
		else {
			Brush.TintColor = FLinearColor(FVector4(.5f, .5f, .5f, 1.f));
			Style.SetPressed(Brush);
		}

		this->Item_Butt->SetStyle(Style);
	}
}

void UItemGridBase::UpdateLevelTexture(UTexture2D* Texture) {
	this->ItemLevel_Img->SetBrushFromTexture(Texture);
}

void UItemGridBase::MouseHoveredAudio() {
	this->PlayOperateAudio(false, true, true);
}

void UItemGridBase::SetButtonIsEnabled(bool Enabled) {
	if (this->bButtonEnabled != Enabled)
	{
		this->bButtonEnabled = Enabled;
		this->GetButton()->SetIsEnabled(Enabled);
	}
}

void UItemGridBase::SetButtonColorIsDark(bool bValue) {

	if (this->bDarkColor != bValue)
	{
		this->bDarkColor = bValue;

		FButtonStyle ButtonStyle = this->Item_Butt->GetStyle();

		FSlateBrush Brush = ButtonStyle.Normal;

		if (bValue)
		{
			FSlateColor Color(FLinearColor(0.5f, 0.5f, 0.5f));
			Brush.TintColor = Color;
		}
		else {
			FSlateColor Color(FLinearColor(1.f, 1.f, 1.f));
			Brush.TintColor = Color;
		}

		ButtonStyle.SetNormal(Brush);

		Brush = ButtonStyle.Hovered;
		ButtonStyle.SetHovered(Brush);

		Brush = ButtonStyle.Pressed;
		ButtonStyle.SetPressed(Brush);

		this->Item_Butt->SetStyle(ButtonStyle);
	}
}

void UItemGridBase::SetButtonImageMatchSize() {
	if (IsValid(this->ItemHeadTextureIns))
	{
		FVector2D Size = FVector2D(
			Cast<UTexture2D>(this->ItemHeadTextureIns)->GetSizeX(),
			Cast<UTexture2D>(this->ItemHeadTextureIns)->GetSizeY()
		);

		FButtonStyle ButtonStyle = this->Item_Butt->GetStyle();

		FSlateBrush Brush = ButtonStyle.Normal;
		Brush.SetImageSize(Size);
		ButtonStyle.SetNormal(Brush);

		Brush = ButtonStyle.Hovered;
		Brush.SetImageSize(Size);
		ButtonStyle.SetHovered(Brush);

		Brush = ButtonStyle.Pressed;
		Brush.SetImageSize(Size);
		ButtonStyle.SetPressed(Brush);

		this->Item_Butt->SetStyle(ButtonStyle);
	}
}

void UItemGridBase::SetFuncObject(UItemBaseGridFunc* Object, FItemBaseStructData* DataPtr) {
	if (IsValid(Object))
	{
		//释放之前的对象
		this->FuncBaseObject = nullptr;
		this->FuncInter.GetObjectRef() = nullptr;
		this->FuncInter = nullptr;

		//指向对象
		this->FuncBaseObject = Object;
		this->FuncInter = Object;
		//执行对象
		this->FuncInter->Execute(this, DataPtr);
	}
}

UItemBaseGridFunc* UItemGridBase::GetFuncObject() {
	return this->FuncBaseObject;
}

TScriptInterface<class IItemBaseGridFuncInterface> UItemGridBase::GetFuncInterface() {
	return this->FuncInter;
}

FString UItemCountGridBase::ToString_Implementation()
{
#if WITH_EDITOR
	return this->CurItemContViewBlock.Name +
		FString(TEXT("\nItem ID ：")) +
		FString::FromInt(this->GetItemViewBlock().ItemID) +
		FString(TEXT("\nUI Index ：")) +
		FString::FromInt(this->GetUiIndex()) +
		FString(TEXT("\nBlock Index ：")) +
		FString::FromInt(this->GetItemViewBlock().BlockIndex) +
		FString(TEXT("\nItem Count ：")) +
		FString::FromInt(this->GetItemCountViewBlock().Count);
#endif
	return this->CurItemContViewBlock.Name;
}

bool UItemCountGridBase::SetItemCountViewBlock(
	const FItemViewBlock* ItemViewBlock,
	const FItemCountViewBlock* Data,
	int32 Index
) {

	if (!ItemViewBlock || !Data)
	{
		return false;
	}

	//初始化物品数量视图数据
	this->CurItemContViewBlock.Name = Data->Name;
	this->CurItemContViewBlock.Texture = Data->Texture;
	this->CurItemContViewBlock.Type = Data->Type;
	
	//设置物品视图数据
	this->SetItemViewBlock(ItemViewBlock, Data->Texture, Index);

	//设置物品的数量
	this->SetItemCount(Data->Count);

	return true;
}

void UItemCountGridBase::SetItemAttribute(UItemBaseView* View, const FItemViewBlock& BlockData, int32 Index) {
	FItemCountViewBlock* ViewBlockData = CAST_TYPE_PTR(FItemCountViewBlock,
		View->GetItem(BlockData.ItemID, BlockData.ItemLevel));
	this->SetItemCountViewBlock(&BlockData, ViewBlockData, Index);
	this->OnViewUpdate();
}

void UItemCountGridBase::SetItemCountTextShow(bool bShow) {
	if (this->bCountVisiable == bShow)
	{
		return;
	}
	else {
		this->bCountVisiable = bShow;
		if (this->bCountVisiable)
		{
			if (this->Count_Text)
				this->Count_Text->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else {
			if (this->Count_Text)
				this->Count_Text->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UItemCountGridBase::SetItemViewBlockCopy(UItemGridBase* CopyGrid) {
	this->CurItemContViewBlock = FItemCountViewBlock();
	this->SetItemCountViewBlock(
		&CopyGrid->GetItemViewBlock(),
		&(Cast<UItemCountGridBase>(CopyGrid)->GetItemCountViewBlock()),
		CopyGrid->GetUiIndex()
	);
}

void UItemCountGridBase::SetItemCount(int32 NewCount) {
	if (this->CurItemContViewBlock.Count != NewCount)
	{
		this->CurItemContViewBlock.Count = NewCount;

		if (this->bCountVisiable)
		{
			if (NewCount > 1)
			{
				if (NewCount > 9999)
				{
					this->Count_Text->SetText(FText::FromString(TEXT("x9999+")));
				}
				else {
					this->Count_Text->SetText(
						FText::FromString(TEXT("x") + FString::FromInt(NewCount))
					);
				}
			}
			else {
				this->bCountVisiable = false;
				this->Count_Text->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		else {
			if (NewCount > 1)
			{
				this->bCountVisiable = true;
				this->Count_Text->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

				if (NewCount > 9999)
				{
					this->Count_Text->SetText(FText::FromString(TEXT("x9999+")));
				}
				else {
					this->Count_Text->SetText(
						FText::FromString(TEXT("x") + FString::FromInt(NewCount))
					);
				}
			}
		}
	}
}

