// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#include "Game/UI/UI_Notice.h"
#include "Game/UI/UI_ButtonTemp.h"
#include <Components/Button.h>
#include <Components/RichTextBlock.h>
#include <Kismet/KismetStringLibrary.h>
#include <Components/VerticalBox.h>
#include <Components/CanvasPanelSlot.h>
#include "GameSystem/Item/Card/CardBaseStruct.h"
#include "GameSystem/Tools/ItemLoadManager.h"

bool UUI_Notice::Initialize()
{
	if (!Super::Initialize())
		return false;

	this->M_URichTextBlock = this->GetWidgetComponent<URichTextBlock>(this, TEXT("LineText"));
	this->M_Load_Uniform_Grid = this->GetWidgetComponent<UUniformGridPanel>(this, TEXT("UniformGridPanel_28"));
	this->M_TableListGridList = this->GetWidgetComponent<UScrollBox>(this, TEXT("ScrollBox_111"));

	return true;
}

UWidget* UUI_Notice::BeginLoadList(const UItemLoadDataBlock* const _item, int32 _index)
{
	//加载按钮
	UUI_ButtonTemp* TabButt = CreateWidget<UUI_ButtonTemp>(this, LoadClass<UUI_ButtonTemp>(0,
		TEXT("WidgetBlueprint'/Game/Resource/BP/Game/UI/MainFrame/BPUI_ButtonTemp.BPUI_ButtonTemp_C'"))
	);
	//设置UI实例
	TabButt->SetUINotice(this);
	//设置标题
	TabButt->SetTitle(_item->GetValue<FGameNoticeDataTable>().M_FGameNotice.M_ButtonTitle);
	//播放动画
	//TabButt->PlayAnimation(TabButt->GetWidgetTreeOwningClass()->Animations[0]);
	//添加到组
	this->AddToGroup(_item->GetValue<FGameNoticeDataTable>(), TabButt);

	if (_index == 0)
	{
		this->ClickTab(0);
	}

	return TabButt;
}

void UUI_Notice::UpdateList(const UItemLoadDataBlock* const _item, int32 _Data_Index, UWidget* _Widget)
{
	if (Cast<UUI_ButtonTemp>(_Widget))
	{

		Cast<UUI_ButtonTemp>(_Widget)->PlayAnimation(Cast<UUI_ButtonTemp>(_Widget)->GetWidgetTreeOwningClass()->Animations[0]);

		//设置标题
		Cast<UUI_ButtonTemp>(_Widget)->SetTitle(
			_item->GetValue<FGameNoticeDataTable>().M_FGameNotice.M_ButtonTitle
		);

		//添加到组
		this->AddToGroup(_item->GetValue<FGameNoticeDataTable>(), Cast<UUI_ButtonTemp>(_Widget));

	}
}

void UUI_Notice::BeginUpdate()
{
	this->M_ButtonTempGroup.Empty();
	this->M_ButtonTempGroupDatas.Empty();
}

void UUI_Notice::AddToGroup(FGameNoticeDataTable _Data, UUI_ButtonTemp* _table_button)
{
	if (!_table_button)
		return;

	//设置索引
	_table_button->SetIndex(this->M_ButtonTempGroup.Num());
	//添加元素
	this->M_ButtonTempGroup.Emplace(_table_button);
	this->M_ButtonTempGroupDatas.Emplace(_Data);
}

void UUI_Notice::ClickTab(int32 _index)
{
	for (auto E : this->M_ButtonTempGroup)
	{
		UWidgetBase::SetButtonStyle(E->GetButton(),
			TEXT("Texture2D'/Game/Resource/Texture/UI/InGame/T_NoticeTab_DisEnable.T_NoticeTab_DisEnable'"));
	}

	if (_index > -1 && _index < this->M_ButtonTempGroup.Num())
	{
		UWidgetBase::SetButtonStyle(this->M_ButtonTempGroup[_index]->GetButton(),
			TEXT("Texture2D'/Game/Resource/Texture/UI/InGame/T_NoticeTab_Enable.T_NoticeTab_Enable'"));
		//显示内容
		this->M_Title = this->M_ButtonTempGroupDatas[_index].M_FGameNotice.M_ContentTitle;

		FString Content_0 = UKismetStringLibrary::Replace(this->M_ButtonTempGroupDatas[_index].M_FGameNotice.M_Content,
			TEXT("<br>"), TEXT("\r\n"));
		FString Content = UKismetStringLibrary::Replace(Content_0, TEXT("<nbsp>"), TEXT("   "));
		this->M_URichTextBlock->SetText(FText::FromString(Content));
	}

}

void UUI_Notice::Begin()
{
	if (this->bInit)
	{
		this->M_UItemLoaderObject->SetLoaderPage(0);
		this->M_UItemLoaderObject->UpdateDatatable(this->LData);
		this->M_UItemLoaderObject->Execute();
	}
	else {
		this->bInit = true;

		TArray<FTableRowBase*> LocalData;
		UGameSystemFunction::GetDataTableRows(
			TEXT("DataTable'/Game/Resource/BP/Data/GameNotice/GameNotice.GameNotice'"),
			LocalData
		);

		for (auto Item : LocalData)
		{
			this->LData.Emplace(*((FGameNoticeDataTable*)(Item)));
		}


		this->M_UItemLoaderObject = this->CreateItemLoaderObject(
			this->M_TableListGridList, this->M_Load_Uniform_Grid, {
			5,1,this->LData.Num(),0.01f,false
			}
		);

		this->M_UItemLoaderObject->UpdateDatatable(this->LData);

		this->M_UItemLoaderObject->OnNew.BindUObject(this, &UUI_Notice::BeginLoadList);
		this->M_UItemLoaderObject->OnUpdate.BindUObject(this, &UUI_Notice::UpdateList);
		this->M_UItemLoaderObject->OnUpdateBefore.BindUObject(this, &UUI_Notice::BeginUpdate);

		this->M_UItemLoaderObject->Execute();
	}
}

void UUI_Notice::bClosePanel()
{
	this->RemoveFromParent();
}
