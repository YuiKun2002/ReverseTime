// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/GeneralWidget/LoaderRangeWidget.h"
#include <Components/ScrollBox.h>
#include <Components/CanvasPanelSlot.h>
#include <Components/UniformGridPanel.h>
#include <Blueprint/WidgetLayoutLibrary.h>

bool ULoaderRangeWidget::Initialize() {

	if (Super::Initialize())
	{

#if WITH_EDITOR
		if (!IsValid(this->LoaderObject))
		{
			this->LoaderObject = NewObject<UItemLoaderObject>();
		}

		//不能出现对应模式只有一行的情况发生
		//否则会出现数据滑动时，不符合预期的预览方式
		if (this->ItemLoaderData.bLoaderModeHorizontal)
		{
			if (this->ItemLoaderData.LoaderCol <= 1)
			{
				this->ItemLoaderData.LoaderCol = 2;
			}
		}
		else {
			if (this->ItemLoaderData.LoaderRow <= 1)
			{
				this->ItemLoaderData.LoaderRow = 2;
			}
		}

		if (this->ItemLoaderData.LoadItemDelayTime < 0.01f)
		{
			this->ItemLoaderData.LoadItemDelayTime = 0.01f;
		}

		if (this->ItemLoaderData.LoadItemMaxCount < 0)
		{
			this->ItemLoaderData.LoadItemMaxCount = 0;
		}

		if (this->SetSize())
		{
			this->LoaderObject->SetLoaderRangeWidget(this);
			this->LoaderObject->SetLoaderWidgetMargin(this->ViewItemMargin);
			return true;
		}
		return false;
#endif

		//初始化加载器,非编辑器模式，则初始化一次后，则不会进行二次初始化
		if (!IsValid(this->LoaderObject))
		{
			this->LoaderObject = NewObject<UItemLoaderObject>();
			if (this->SetSize())
			{
				this->LoaderObject->SetLoaderRangeWidget(this);
				this->LoaderObject->SetLoaderWidgetMargin(this->ViewItemMargin);
				return true;
			}
		}
	}
	return false;
}

void ULoaderRangeWidget::BeginDestroy() {

	Super::BeginDestroy();

	if (IsValid(this->ExternalArea))
		this->ExternalArea->OnUserScrolled.Clear();
	if (IsValid(this->InternalArea))
		this->InternalArea->ClearChildren();
	this->LoaderObject = nullptr;
}

bool ULoaderRangeWidget::SetSize() {

	UCanvasPanelSlot* ThisSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(this);

	if (IsValid(ThisSlot))
	{
		FVector2D NewSize
		(this->ItemLoaderData.LoaderCol * this->ViewItemSize.X,
			this->ItemLoaderData.LoaderRow * this->ViewItemSize.Y);

		if (this->ItemLoaderData.bLoaderModeHorizontal)
		{
			/*水平模式*/
			if (ThisSlot->GetSize().Y >= NewSize.Y)
			{
				NewSize.Y = ThisSlot->GetSize().Y;
			}
		}
		else {
			/*垂直模式*/
			if (ThisSlot->GetSize().X >= NewSize.X)
			{
				NewSize.X = ThisSlot->GetSize().X;
			}
		}

		ThisSlot->SetSize(
			NewSize
		);

		return true;
	}
	return false;
}

