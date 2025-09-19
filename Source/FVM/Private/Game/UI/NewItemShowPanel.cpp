// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/NewItemShowPanel.h"
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Components/UniformGridPanel.h>
#include <Components/ScrollBox.h>
#include <Kismet/GameplayStatics.h>
#include "Game/UI/GeneralWidget/LoaderRangeWidget.h"

void UNewItemShowPanel::Load(const TArray<FTaskItemStruct>& Data)
{
	this->Datas.Empty();
	this->Datas.Append(Data);

	//初始化任务显示
	if (!this->BP_LoaderRangeWidget->GetLoaderObject()->OnNew.IsBound())
	{
		this->BP_LoaderRangeWidget->GetLoaderObject()->OnNew.BindUObject(this, &UNewItemShowPanel::WidgetCreateList);
		this->BP_LoaderRangeWidget->GetLoaderObject()->OnUpdate.BindUObject(this, &UNewItemShowPanel::WidgetRefreshList);
	}

	this->BP_LoaderRangeWidget->GetLoaderObject()->UpdateDatatable(this->Datas);
	this->BP_LoaderRangeWidget->GetLoaderObject()->SetLoaderItemMaxCount(this->Datas.Num());
	this->BP_LoaderRangeWidget->GetLoaderObject()->Execute();
}

UWidget* UNewItemShowPanel::WidgetCreateList(const UItemLoadDataBlock* const _Data, int32 _Index)
{
	UNewItemShow* NewUI = CreateWidget<UNewItemShow>(
		this, LoadClass<UNewItemShow>(nullptr,
			TEXT("WidgetBlueprint'/Game/Resource/BP/Game/UI/UI_Tip/BP_NewItemTip.BP_NewItemTip_C'"))
	);

	FTaskItemStruct CurData = _Data->GetValue<FTaskItemStruct>();
	NewUI->Set(CurData.ItemName, CurData.ImagePath);

	return NewUI;
}

void UNewItemShowPanel::WidgetRefreshList(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget)
{
	UNewItemShow* NewUI = Cast<UNewItemShow>(_UWidget);
	FTaskItemStruct CurData = _Data->GetValue<FTaskItemStruct>();
	NewUI->Set(CurData.ItemName, CurData.ImagePath);
}

bool UNewItemShow::Initialize()
{
	Super::Initialize();

	this->Name = this->GetWidgetComponent<UTextBlock>(this, TEXT("TextBlock_42"));
	this->Bg = this->GetWidgetComponent<UImage>(this, TEXT("Image_71"));

	return true;
}

void UNewItemShow::Set(const FString& ItemName, const FSoftObjectPath& ImgPath)
{
	this->Name->SetText(FText::FromString(ItemName));
	UWidgetBase::SetImageBrush(this->Bg,
		ImgPath.ToString(),
		FVector(1.f), 1.f, true);
}
