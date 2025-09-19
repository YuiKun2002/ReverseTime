// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/PlayerUpdate/PlayerCardUpdate.h"

void UPlayerCardUpdate::Update_Implementation(UPlayerStructManager* PlayerDataIns)
{

	GAME_LOG_LINE(TEXT("UPlayerCardUpdate::Update：初始化卡片视图"));

	GameDataStaticObject<UPlayerBagCardView>()->GenerateItemView(0);
}