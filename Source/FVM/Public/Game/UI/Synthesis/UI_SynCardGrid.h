// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "UI_SynCardGrid.generated.h"

/**
 * 合成屋卡片格子-卡片区域用于选择卡片
 */
UCLASS()
class FVM_API UUI_SynCardGrid : public UUI_PlayerBagCardGrid
{
	GENERATED_BODY()

public:

	//点击事件
	virtual	void OnClicked_Implementation() override;

private:
	//卡片强化
	void CardUp();
	//卡片转职
	void CardTransfer();
	//卡片进化
	void CardGoldEvolve();
};
