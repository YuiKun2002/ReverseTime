// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Item/ItemBaseGridFunc.h"
#include "GameSystem/Item/Equipment/BagStruct.h"
#include "EquipmentFuncBag.generated.h"

/**
 * 背包道具
 */
UCLASS()
class FVM_API UEquipmentFuncBag : public UItemBaseGridFunc
{
	GENERATED_BODY()

public:
	virtual void Execute(
		class UItemGridBase* EquipGridPtr,
		struct FItemBaseStructData* ItemBasePtr
	) override;


	void SetPreview();

protected:
	//装备
	UFUNCTION()
	void Equip();
private:
	UPROPERTY()
	FBag BagData;
	UPROPERTY()
	class UUI_EquipmentGrid* EquipGrid = nullptr;
	UPROPERTY()
	bool bPreview = false;
};
