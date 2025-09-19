// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Tips/UI_ItemTitleTip.h"
#include <Components/SizeBox.h>
#include <Components/CanvasPanelSlot.h>
#include "GameSystem/FVMGameInstance.h" 
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameSystem/GameUserInterfaceSubsystem.h"

void UUI_ItemTitleTip::CreateBaseTipWidget(const FString& _ShowTitle, bool _bShow)
{
	if (_bShow && !_ShowTitle.IsEmpty())
	{
		//获取鼠标位置
		FVector2D LPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(
			UFVMGameInstance::GetFVMGameInstance()->GetWorld()
		);
		//获取视口缩放
		float CurScale = UWidgetLayoutLibrary::GetViewportScale(
			UFVMGameInstance::GetFVMGameInstance()->GetWorld());

		FVector2D LViewSize = UWidgetLayoutLibrary::GetViewportSize(
			UFVMGameInstance::GetFVMGameInstance()->GetWorld());

		//设置缩放
		LPosition.X = LPosition.X - this->GetDesiredSize().X;
		LPosition.X *= CurScale;

		if ((LPosition.Y + this->GetDesiredSize().Y) * CurScale > LViewSize.Y * 0.95f)
		{
			LPosition.Y *= CurScale;
			LPosition.Y = LPosition.Y - (this->GetDesiredSize().Y * CurScale);

			if (LPosition.X <= 0)
			{
				LPosition.X = 0.f;
			}
		}
		else {
			LPosition.Y *= CurScale;

			if (LPosition.X <= 0)
			{
				LPosition.X = 0.f;
				LPosition.Y = LPosition.Y - (this->GetDesiredSize().Y * CurScale);
			}
		}

		//设置最终位置
		this->SetPositionInViewport(LPosition, true);

		if (!this->IsInViewport())
		{
			//设置标题
			this->AddToViewport(9999);
		}

		if (!this->bShowState)
		{
			this->bShowState = true;
			this->SetRenderOpacity(1.f);
			this->SetText(_ShowTitle);
		}
	}
	else {
		if (this->bShowState)
		{
			this->SetRenderOpacity(0.f);
			this->bShowState = false;
		}
	}
}

void UUI_ItemTitleTip::ShowWidgetTip(UWidgetBase* WidgetInstance, bool bShowState)
{
#if PLATFORM_WINDOWS
	GameDataStaticObject<UItemTitleTip>()->Get()
		->CreateBaseTipWidget(WidgetInstance->ToString(), bShowState);
#endif
}

UUI_ItemTitleTip* UItemTitleTip::Get() {
	if (!IsValid(this->Tip))
	{
		this->Tip = GET_UI_CAST(UUI_ItemTitleTip, Global, ItemTitleTip);
	}
	return this->Tip;
}