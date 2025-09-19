// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Item/ItemBaseGridFunc.h"
#include "GameSystem/Item/Equipment/PlayerEquipmentStruct.h"
#include "EquipmentFuncEquipment.generated.h"

/**
 * 装备功能，服装
 */
UCLASS()
class FVM_API UEquipmentFuncEquipment : public UItemBaseGridFunc
{
	GENERATED_BODY()

public:

	virtual void Execute(
		class UItemGridBase* EquipGridPtr,
		struct FItemBaseStructData* ItemBasePtr
	) override;

protected:

	//装备
	UFUNCTION()
	void Equip();

private:
	//服装数据
	UPROPERTY()
	FEquipment_PlayerEquipment_Data EquipData;
	//装备网格
	UPROPERTY()
	class UUI_EquipmentGrid* EquipGrid = nullptr;
};
