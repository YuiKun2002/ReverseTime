// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/UI_PlayerBagEquipment_Bag_Grid.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/Item/Equipment/Func/EquipmentFuncBag.h"

void UUI_PlayerBagEquipment_Bag_Grid::OnClicked_Implementation() {
	uint8 Type = 0u;
	FItemBaseStructData* DataStruct;
	if (GET_ITEM(this->GetItemViewBlock().ItemID, Type, DataStruct, GET_DEF_CATEGORYNAME(Equip)))
	{
		switch ((EEquipment)(Type))
		{
		case EEquipment::E_Bag:
		{
			UEquipmentFuncBag* Bag = NewObject<UEquipmentFuncBag>();
			this->SetFuncObject(Bag, DataStruct);
		}
		}
	}
}