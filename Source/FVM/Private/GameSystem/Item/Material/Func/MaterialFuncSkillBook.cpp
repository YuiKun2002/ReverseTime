// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/Material/Func/MaterialFuncSkillBook.h"
#include "Game/UI/UI_PlayerBag.h"
#include "Data/MaterialDataStruct.h"
#include "Game/UI/Tips/UI_ItemDesTip.h"
#include "GameSystem/PlayerDataHandle.h"
#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/UI_PlayerBagMaterialGrid.h"
#include "GameSystem/PlayerData/LocalSkillBookDataHandle.h"

void UMaterialFuncSkillBook::Execute(
	UItemGridBase* GridPtr,
	FItemBaseStructData* ItemBaseDataPtr
) {
	Super::Execute(GridPtr, ItemBaseDataPtr);

	this->Data = CAST_TYPE(FCardSkillBooks_Data, ItemBaseDataPtr);

	this->Grid = Cast<UUI_PlayerBagMaterialGrid>(GridPtr);

	ULocalPlayerDataHandle* Hd = UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->
		GetLocalPlayerDataHandle();
	if (IsValid(Hd))
	{
		ULocalSkillBookDataHandle* SHD = Cast<ULocalSkillBookDataHandle>(
			Hd->LoadPlayerDataPageHandle(EPlayerDataConfigPage::SkillBook)
		);
		if (IsValid(SHD))
		{
			ULocalSkillBookDataSaveGame* Save = Cast<ULocalSkillBookDataSaveGame>(SHD->Execute_GetData(SHD));
			if (IsValid(Save))
			{
				this->GetTipWidget()->SetHidden(false);
				this->GetTipWidget()->SetOkButtonTitleText(TEXT("使用"));
				this->GetTipWidget()->BindClickFunc(this, TEXT("Use"));

				if (Save->CheckSkillBook(this->Data.M_FMaterial))
				{
					this->GetTipWidget()->GetOkButtonComponent()->SetIsEnabled(true);
				}
				else {
					this->GetTipWidget()->GetOkButtonComponent()->SetIsEnabled(false);
				}
			}
		}
	}
}

void UMaterialFuncSkillBook::Preview() {
	this->GetTipWidget()->SetHidden();
}

void UMaterialFuncSkillBook::Use() {

	//获取数量
	FItemViewBlock Block = this->Grid->GetItemViewBlock();

	int32 Count = this->Grid->GetItemCountViewBlock().Count;

	//使用技能书
	this->UseSkillBookData(this->Data.M_FMaterial, Count);

	//获取数据
	if (GET_PLAYERDATAINS()->ConsumeMaterial(Block.ItemID, Block.ItemLevel, Count))
	{
		//保存数据
		ULocalPlayerDataHandle* Hd = UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->
			GetLocalPlayerDataHandle();
		if (IsValid(Hd))
		{
			Hd->Execute_SaveData(Hd, TEXT("技能书使用，道具消耗"));
			//重新加载列表
			GET_UI_CAST(UUI_PlayerBag, Global, PlayerBag)->RelaodCurrentItemData();
			GET_UI_CAST(UUI_PlayerBag, Global, PlayerBag)->LoadCardSkillBookTabs_DefCategory();
		}
	}
}

bool UMaterialFuncSkillBook::UseSkillBookData(FCardSkillBookMaterial _Data, int32& _BooksCount)
{
	//获取数据
	ULocalPlayerDataHandle* Hd = UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->
		GetLocalPlayerDataHandle();

	if (!IsValid(Hd))
	{
		_BooksCount = 0;
		return false;
	}

	ULocalSkillBookDataHandle* SHD = Cast<ULocalSkillBookDataHandle>(
		Hd->LoadPlayerDataPageHandle(EPlayerDataConfigPage::SkillBook)
	);
	if (IsValid(SHD))
	{
		ULocalSkillBookDataSaveGame* Save = Cast<ULocalSkillBookDataSaveGame>(SHD->Execute_GetData(SHD));
		if (IsValid(Save))
		{
			if (Save->AddSkillBookList(_Data, _BooksCount))
			{
				//保存技能书数据
				SHD->Execute_SaveData(SHD, TEXT("技能书存档保存"));

				//创建提升
				UWidgetBase::CreateTipWidget(TEXT("使用成功"));
			}

			if (_BooksCount > 0)
			{
				//添加历史记录
				UPlayerRecord::AddDayCardSkillBook(_Data.ItemName);
				//UTaskSubsystem::GetTaskSubsystemStatic()->ExecuteTasks(this);
			}

			return true;
		}
	}

	_BooksCount = 0;
	return false;
}