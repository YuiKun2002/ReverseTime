// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Game/UI/UI_EquipmentGrid.h"
#include "UI_PlayerBagEquipment_Bag_Grid.generated.h"

class UUI_PlayerBag;
class IEquipmentFuncInterface;

/**
 * 专属于：角色背包格子->道具->背包道具，定制格子
 */
UCLASS()
class FVM_API UUI_PlayerBagEquipment_Bag_Grid : public UUI_EquipmentGrid
{
	GENERATED_BODY()

public:

	virtual	void OnClicked_Implementation() override;
};
