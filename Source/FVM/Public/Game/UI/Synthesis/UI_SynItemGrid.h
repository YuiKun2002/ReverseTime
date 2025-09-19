// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Game/UI/ItemGridBase.h"
#include "UI_SynItemGrid.generated.h"

/**
 * 合成屋-物品格子-材料格子用于选择材料或者查看材料
 */
UCLASS()
class FVM_API UUI_SynItemGrid : public UItemCountGridBase
{
	GENERATED_BODY()

public:

	virtual FString ToString_Implementation() override;

	//点击事件
	virtual	void OnClicked_Implementation() override;

	//设置初始化类型
	FORCEINLINE void SetInitType(int32 NewType) { this->InitType = NewType; }
	//设置操作界面类型
	FORCEINLINE void SetOperateType(FName OperType) { this->InitOperType = OperType; }

protected:
	//设置物品视图数据
	virtual	bool SetItemCountViewBlock(
		const FItemViewBlock* ItemViewBlock,
		const FItemCountViewBlock* Data,
		int32 Index
	) override;

	virtual	void SetItemCount(int32 NewCount) override;

private:
	void OperEquip();
	//武器宝石镶嵌
	bool OpenEquip_WG_WeaponAndGem();
	//宝石强化
	bool OpenEquip_WG_UpgradeGem();
	//宝石分解
	bool OpenEquip_SG_SplitGem();



	void OperMater();
	//卡片制作[合成配方]
	bool OperMater_BM_MakeCard();
	//卡片制作[合成配方]香料
	bool OperMater_SP_MakeCard();
	//卡片强化[强化四叶草]
	bool OperMater_Clover_UpgradeCard();
	//宝石强化[强化四叶草]
	bool OperMater_Clover_UpgradeGem();

protected:
	//初始化类型[0装备][2材料]
	UPROPERTY()
	int32 InitType = 0;
	//操作类型[MakeCard][CardPpgrade][........]
	UPROPERTY()
	FName InitOperType = FName();
};
