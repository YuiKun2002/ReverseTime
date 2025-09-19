// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/PlayerUpdate/PlayerMaterialUpdate.h"

void UPlayerMaterialUpdate::Update_Implementation(UPlayerStructManager* PlayerDataIns)
{
	GAME_LOG_LINE(TEXT("UPlayerMaterialUpdate::Update：初始化材料视图"));

	GameDataStaticObject<UPlayerBagMaterialView>()->GenerateItemView(0);
}