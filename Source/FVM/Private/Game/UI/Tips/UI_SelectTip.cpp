// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/UI/Tips/UI_SelectTip.h"
#include <Components/Button.h>
#include <Components/TextBlock.h>

void UUI_SelectTip::SetContentText(FText Text) {
	this->Content->SetText(Text);
}

void UUI_SelectTip::BindOkDelegate(FSelectClickedDelegate ClickEvent) {
	FScriptDelegate Bind;
	Bind.BindUFunction(ClickEvent.GetUObject(), ClickEvent.GetFunctionName());
	this->BindOk(MoveTempIfPossible(Bind));
}

void UUI_SelectTip::BindCancelDelegate(FSelectClickedDelegate ClickEvent) {
	FScriptDelegate Bind;
	Bind.BindUFunction(ClickEvent.GetUObject(), ClickEvent.GetFunctionName());
	this->BindCancel(MoveTempIfPossible(Bind));
}

void UUI_SelectTip::BindOk(FScriptDelegate Callback)
{
	this->Ok->OnClicked.Clear();
	this->Ok->OnClicked.Add(MoveTempIfPossible(Callback));
	this->BindDef(this->Ok);
}

void UUI_SelectTip::BindCancel(FScriptDelegate Callback) {
	this->Cancel->OnClicked.Clear();
	this->Cancel->OnClicked.Add(MoveTempIfPossible(Callback));
	this->BindDef(this->Cancel);
}

void UUI_SelectTip::BindDef(UButton* But) {
	FScriptDelegate Click, Close;
	Click.BindUFunction(this, TEXT("PlayOperateAudioDef"));
	Close.BindUFunction(this, TEXT("RemoveCur"));
	But->OnClicked.Add(MoveTempIfPossible(Click));
	But->OnClicked.Add(MoveTempIfPossible(Close));
}

void UUI_SelectTip::RemoveCur() {
	this->RemoveFromParent();
}

UUI_SelectTip* UUI_SelectTip::CreateSelectTip()
{
	UUI_SelectTip* Widget = GET_UI_CAST(UUI_SelectTip, Global, SelectTip);
	if (!Widget->IsInViewport())
	{
		if (!Widget->Ok->OnClicked.IsBound())
		{
			Widget->BindDef(Widget->Ok);
		}
		if (!Widget->Cancel->OnClicked.IsBound())
		{
			Widget->BindDef(Widget->Cancel);
		}
		Widget->AddToViewport();
	}
	else {
		if (!IsValid(Widget->SecondSelectTipWidgetBuffer))
		{
			Widget->SecondSelectTipWidgetBuffer = CREATE_UI(Widget, UUI_SelectTip, Global, SelectTip);
			Widget->SecondSelectTipWidgetBuffer->BindDef(
				Widget->SecondSelectTipWidgetBuffer->Ok
			);
			Widget->SecondSelectTipWidgetBuffer->BindDef(
				Widget->SecondSelectTipWidgetBuffer->Cancel
			);
			Widget->SecondSelectTipWidgetBuffer->AddToViewport();
			return Widget->SecondSelectTipWidgetBuffer;
		}
		else {
			if (!Widget->SecondSelectTipWidgetBuffer->IsInViewport())
			{
				Widget->SecondSelectTipWidgetBuffer->AddToViewport();
			}
		}
		//返回第二界面
		return Widget->SecondSelectTipWidgetBuffer;
	}

	//返回第一界面
	return Widget;
}

