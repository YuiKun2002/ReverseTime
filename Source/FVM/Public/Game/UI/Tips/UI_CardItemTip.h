// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Game/UI/Tips/UI_ItemDesTip.h"
#include "UI_CardItemTip.generated.h"

/**
 * 卡片物品提示
 */
UCLASS()
class FVM_API UUI_CardItemTip : public UUI_ItemDesTip
{
	GENERATED_BODY()

protected:

	virtual void SetCountText(int32 Count) override;
};
