// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Tips/UI_ItemDesTip.h"
#include <Components/VerticalBox.h>
#include <Components/TextBlock.h>
#include <Components/Button.h>
#include <Components/Image.h>

// WidgetBlueprint'/Game/Resource/BP/Game/UI/UI_Tip/BPUI_ItemDesTip.BPUI_ItemDesTip_C'

void UUI_ItemDesTip::SetHidden(bool bOkButt, bool bCancelButt) {
	if (bOkButt)
	{
		this->Ok->GetParent()->SetVisibility(ESlateVisibility::Collapsed);

	}
	else {
		this->Ok->GetParent()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	}
	if (bCancelButt)
	{
		this->Cancel->GetParent()->SetVisibility(ESlateVisibility::Collapsed);

	}
	else {
		this->Cancel->GetParent()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

FOnButtonClickedEvent& UUI_ItemDesTip::GetOkButtonDelegate()
{
	return this->Ok->OnClicked;
}

FOnButtonClickedEvent& UUI_ItemDesTip::GetCancelButtonDelegate()
{
	return this->Cancel->OnClicked;
}

UButton* UUI_ItemDesTip::GetOkButtonComponent()
{
	return this->Ok;
}

UButton* UUI_ItemDesTip::GetCancelButtonComponent()
{
	return this->Cancel;
}

void UUI_ItemDesTip::SetOkButtonTitleText(FString _Content)
{
	this->OkTitle->SetText(FText::FromString(_Content));
}

void UUI_ItemDesTip::SetItemCount(int32 Count) {
	this->SetCountText(Count);
}

void UUI_ItemDesTip::SetCountText(int32 Count) {

	if (Count == 0)
	{
		this->ItemCount_Text->SetVisibility(ESlateVisibility::Collapsed);
	}
	else {
		this->ItemCount_Text->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (Count > 9999)
	{
		this->ItemCount_Text->SetText(
			FText::FromString(TEXT("x9999+"))
		);
	}
	else {
		this->ItemCount_Text->SetText(FText::FromString(TEXT("x") + FString::FromInt(Count)));
	}
}

void UUI_ItemDesTip::SetContentTitleText(FString _Content)
{
	this->Title->SetText(FText::FromString(_Content));
}

void UUI_ItemDesTip::SetContentText(FString _Content)
{
	this->Content->SetText(FText::FromString(_Content));
}

void UUI_ItemDesTip::AddContentText(FString _Content) {
	this->Content->SetText(
		FText::FromString(this->Content->GetText().ToString() + _Content)
	);
}

UVerticalBox* UUI_ItemDesTip::GetShowListBox() {
	return this->ListBox;
}

void UUI_ItemDesTip::SetItemHeadPath(FString _2DTexture_Path)
{
	UWidgetBase::SetImageBrush(
		this->ItemHead,
		_2DTexture_Path,
		FVector(1.f), 1.f
		, false, FVector(1.f, 1.f, 0.f));
}

void UUI_ItemDesTip::SetItemHead(class UTexture2D* Texture) {
	if (IsValid(Texture))
	{
		if (this->ItemHead->GetBrush().GetResourceObject() != Cast<UObject>(Texture))
		{

			//this->ItemHead->Brush.DrawAs = ESlateBrushDrawType::Image;
			//FVector2D Size(Texture->GetSizeX(), Texture->GetSizeY());
			//this->ItemHead->Brush.ImageSize = Size;
			this->ItemHead->SetBrushResourceObject(Texture);

			//UWidgetBase::WidgetToCanvasSlot(this->ItemHead)->SetSize(Size);
		}

		this->ItemHead->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		this->ItemHead->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUI_ItemDesTip::BindClickFunc(UObject* Object, const FName& ClickFunc) {
	//绑定函数 点击之后显示细节面板
	this->GetOkButtonComponent()->OnClicked.Clear();
	FScriptDelegate  CallFunc, ClickAudio;
	CallFunc.BindUFunction(Object, ClickFunc);
	ClickAudio.BindUFunction(this, TEXT("PlayOperateAudioDef"));
	this->GetOkButtonComponent()->OnClicked.Add(MoveTemp(CallFunc));

	//界面移除
	if (!this->GetOkButtonComponent()->OnClicked.Contains(this, TEXT("RemoveFromParent")))
	{
		FScriptDelegate CallFuncRemoveFromParent;
		CallFuncRemoveFromParent.BindUFunction(this, TEXT("RemoveFromParent"));
		this->GetOkButtonComponent()->OnClicked.Add(MoveTemp(CallFuncRemoveFromParent));
	}

	this->GetOkButtonComponent()->OnClicked.Add(MoveTemp(ClickAudio));

	if (!this->GetCancelButtonComponent()->OnClicked.IsBound())
	{
		this->GetCancelButtonComponent()->OnClicked.Clear();
		FScriptDelegate  CallFunc2, ClickAudio2;
		CallFunc2.BindUFunction(this, TEXT("RemoveFromParent"));
		ClickAudio2.BindUFunction(this, TEXT("PlayOperateAudioDef"));
		this->GetOkButtonComponent()->OnClicked.Add(MoveTemp(CallFunc2));
		this->GetOkButtonComponent()->OnClicked.Add(MoveTemp(ClickAudio2));
	}
}

