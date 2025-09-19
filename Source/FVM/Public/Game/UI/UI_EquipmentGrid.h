// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Game/UI/PlayerBagItemGrid.h"
#include "Data/EquipmentDataSturct.h"
#include "GameSystem/Item/ItemBaseStruct.h"
#include "UI_EquipmentGrid.generated.h"


/**
 * 装备格子
 */
UCLASS()
class FVM_API UUI_EquipmentGrid : public UPlayerBagItemGrid
{
	GENERATED_BODY()

public:

	//显示文字
	virtual FString ToString_Implementation() override;

	//显示物品提示
	virtual	void ShowItemTip_Implementation() override;

	//设置装备格子显示
	UFUNCTION(BlueprintCallable)
	static void ShowEquipmentGrid(class UPlayerBagItemGrid* ItemGridIns, bool bPreview = true);

protected:

	//设置物品数量
	virtual	void SetItemCount(int32 NewCount) override;
};
