// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/PlayerData/LocalEquipmentDataHandle.h"

bool ULocalEquipmentDataSaveGame::RemoveEquipType(int32 BlockIndex, uint8 Type) {

	switch ((EEquipment)Type)
	{

	//删除武器细节数据
	case EEquipment::E_PlayerWeaponFirst:
	case EEquipment::E_PlayerWeaponSecond:
	case EEquipment::E_PlayerWeaponSuper:
	{
		if (BlockIndex != -1)
		{
			if (this->bEuipWeaponItems.Contains(BlockIndex))
			{
				this->bEuipWeaponItems.Remove(BlockIndex);
				return true;
			}
		}
	}
	break;

	//删除宝石细节数据
	case EEquipment::E_WeaponGem:
		break;
	}

	return false;
}

TMap<int32, FPlayerWeaponEquipData>& ULocalEquipmentDataSaveGame::GetbEquipWeaponList() {
	return this->bEuipWeaponItems;
}

bool ULocalEquipmentDataHandle::CreateData_Implementation() {

	this->SaveGameObject = NewObject<ULocalEquipmentDataSaveGame>();

	this->Execute_SaveData(this, TEXT("新建-武器数据存档"));

	return true;
}