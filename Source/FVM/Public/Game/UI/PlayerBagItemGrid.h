// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Game/UI/ItemGridBase.h"
#include "PlayerBagItemGrid.generated.h"

/**
 * 角色背包物品格子
 * 1.装备格子，材料格子都可以继承此类
 * 2.获得共同属性
 */
UCLASS()
class FVM_API UPlayerBagItemGrid : public UItemCountGridBase
{
	GENERATED_BODY()

public:

	//设置功能对象, UPlayerBagItemGrid类型 公开此函数
	virtual void SetFuncObject(
		UItemBaseGridFunc* Object,
		FItemBaseStructData* DataPtr
	) override;

protected:
	
	/*
		显示物品提示
		1.在蓝图请勿随意重写 OnClicked 事件
		2.如果重写 OnClicked 事件后，请主动调用 ShowItemTip 事件，否则 ShowItemTip 不会被触发！
		3.点击此界面，会自动调用 ShowItemTip 事件
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ShowItemTip();
	virtual void ShowItemTip_Implementation() {}

private:

	//点击事件
	virtual	void OnClicked_Implementation() override;
};
