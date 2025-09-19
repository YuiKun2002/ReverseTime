// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/PlayerUpdate/PlayerEquipmentUpdate.h"

void UPlayerEquipmentUpdate::Update_Implementation(UPlayerStructManager* PlayerDataIns)
{

	GAME_LOG_LINE(TEXT("UPlayerEquipmentUpdate::Update：初始化装备道具视图"));

	GameDataStaticObject<UPlayerBagEquipView>()->GenerateItemView(0);

}
