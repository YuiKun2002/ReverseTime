// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/PlayerBagItemGrid.h"

void UPlayerBagItemGrid::OnClicked_Implementation() {
	this->ShowItemTip();
}

void UPlayerBagItemGrid::SetFuncObject(
	UItemBaseGridFunc* Object,
	FItemBaseStructData* DataPtr
) {
	Super::SetFuncObject(Object, DataPtr);
}
