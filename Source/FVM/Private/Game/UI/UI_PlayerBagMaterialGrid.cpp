// Fill out your copyright notice in the Description page of Prthis->MaterialBaseCopyData.oject Settings.


#include "Game/UI/UI_PlayerBagMaterialGrid.h"
#include <Components/TextBlock.h>
#include "GameSystem/Item/Material/Func/MaterialFuncSkillBook.h"

void UUI_PlayerBagMaterialGrid::ShowItemTip_Implementation() {
	UUI_PlayerBagMaterialGrid::ShowMaterialGrid(this, false);
}

void UUI_PlayerBagMaterialGrid::ShowMaterialGrid(class UPlayerBagItemGrid* ItemGridIns, bool bPreview) {
	uint8 Type = 0u;
	FItemBaseStructData* DataStruct;
	if (GET_ITEM(ItemGridIns->GetItemViewBlock().ItemID, Type, DataStruct, GET_DEF_CATEGORYNAME(Material)))
	{
		switch ((EMaterialType)ItemGridIns->GetItemCountViewBlock().Type)
		{
		case EMaterialType::E_CardSkillBook: {
			if (bPreview)
			{
				UMaterialFuncSkillBook* Obj = NewObject<UMaterialFuncSkillBook>();
				ItemGridIns->SetFuncObject(Obj, DataStruct);
				Obj->Preview();
			}
			else {
				ItemGridIns->SetFuncObject(NewObject<UMaterialFuncSkillBook>(), DataStruct);
			}
		}break;
		default: {
			ItemGridIns->SetFuncObject(NewObject<UItemBaseGridFunc>(), DataStruct);
		}break;
		}
	}
}