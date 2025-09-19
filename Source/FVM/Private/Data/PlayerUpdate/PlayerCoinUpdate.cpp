// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/PlayerUpdate/PlayerCoinUpdate.h"

void UPlayerCoinUpdate::Update_Implementation(class UPlayerStructManager* PlayerDataIns) {

	PlayerDataIns->M_FPlayerCoin.InitCoin();

}