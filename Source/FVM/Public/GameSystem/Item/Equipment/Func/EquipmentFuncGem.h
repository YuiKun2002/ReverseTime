// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Item/ItemBaseGridFunc.h"
#include "GameSystem/Item/Equipment/GemsStruct.h"
#include "EquipmentFuncGem.generated.h"

/**
 * 装备-宝石功能
 */
UCLASS()
class FVM_API UEquipmentFuncGem : public UItemBaseGridFunc
{
	GENERATED_BODY()

public:

	virtual void Execute(
		class UItemGridBase* GridPtr,
		struct FItemBaseStructData* ItemBaseDataPtr
	) override;

	//设置操作类型
	FORCEINLINE	void SetOperateType(int32 OperType) { this->OperateType = OperType; }

protected:

	//合成屋-武器宝石镶嵌
	UFUNCTION()
	void SynthesisWeaponGemSelect();

	//合成屋-宝石强化
	UFUNCTION()
	void SynthesisUpgradeGemSelect();

	//合成屋-宝石分解
	UFUNCTION()
	void SynthesisSplitGemSelect();

private:

	/*
		道具点击的操作类型
		0 显示宝石数据
		1 合成屋-武器和宝石镶嵌-宝石的选择
	*/
	UPROPERTY()
	int32 OperateType = 0;

	UPROPERTY()
	UItemGridBase* Grid = nullptr;

	UPROPERTY()
	FEquipment_WeaponGem_Data GemData;
};
