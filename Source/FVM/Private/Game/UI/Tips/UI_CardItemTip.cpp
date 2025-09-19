// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Tips/UI_CardItemTip.h"
#include <Components/Image.h>
#include <Components/Button.h>
#include <Components/TextBlock.h>
#include <Components/VerticalBox.h>
#include "GameSystem/Item/ItemBaseStruct.h"
#include "GameSystem/Item/ItemBaseGridFunc.h"

void UUI_CardItemTip::SetCountText(int32 Count) {
	this->ItemCount_Text->SetText(FText::FromString(FString::FromInt(Count)));
}

