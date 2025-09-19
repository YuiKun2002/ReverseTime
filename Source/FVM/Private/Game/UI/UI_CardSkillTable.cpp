// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/UI_CardSkillTable.h"
#include <Components/Image.h>
#include <Components/TextBlock.h>

void UUI_CardSkillTable::InitData(FSkillBookData _Data)
{
	int32 LIndex = 0;

	if (_Data.Level.LV == 0 || _Data.Level.LV == 1)
	{
		LIndex = 0;
	}
	else if (_Data.Level.LV > 1 && _Data.Level.LV < 5)
	{
		LIndex = _Data.Level.LV - 1;
	}
	else
	{
		LIndex = 3;
	}

	//如果等级为0设置头像为灰色
	if (_Data.Level.LV == 0)
	{
		//设置头像
		UWidgetBase::SetImageBrush(this->ItemHead, _Data.HeadPath[LIndex].ToString(), FVector(0.4f, 0.4f, 0.4f));
	}
	else {
		//设置头像
		UWidgetBase::SetImageBrush(this->ItemHead, _Data.HeadPath[LIndex].ToString());
	}

	//设置名称
	this->ItemName->SetText(_Data.Name);
	//设置等级
	this->ItemLV->SetText(FText::FromString(FString(TEXT("Lv:") + FString::FromInt(_Data.Level.LV))));
	//设置提示
	this->ItemTip->SetText(_Data.Tip);
	//经验
	this->ItemEx->SetText(FText::FromString(
		FString(FString::FromInt(_Data.Level.CurrentEx) + TEXT("/") + FString::FromInt(_Data.Level.CurrentTotalEx))));

	//更新进度条
	UWidgetBase::SetWidgetTransformProgress(this->ItemPro_Img, 3.f, 219.f,
		((_Data.Level.CurrentEx + 0.f) /
			_Data.Level.CurrentTotalEx));
}

void UUI_CardSkillTable::LoadData()
{

}
