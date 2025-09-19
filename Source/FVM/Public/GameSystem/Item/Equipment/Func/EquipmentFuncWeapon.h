// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Item/ItemBaseGridFunc.h"
#include "GameSystem/Item/Equipment/PlayerWeaponStruct.h"
#include "EquipmentFuncWeapon.generated.h"

/**
 * 装备功能-武器
 */
UCLASS()
class FVM_API UEquipmentFuncWeapon : public UItemBaseGridFunc
{
	GENERATED_BODY()

public:
	virtual void Execute(
		class UItemGridBase* GridPtr,
		struct FItemBaseStructData* ItemBasePtr
	) override;

	//设置类型
	void SetType(uint8 Type);
	//设置操作类型
	void SetOperateType(int32 OperType);
protected:

	//添加细节数据
	void AddDetailData(
		class UPlayerStructManager* const Player,
		const FItemViewBlock& Block
	);

	//装备
	UFUNCTION()
	void Equip();

	//合成屋-武器和宝石镶嵌-武器的选择
	UFUNCTION()
	void SynthesisWeaponSelect();
	//合成屋-宝石强化-武器的选择
	UFUNCTION()
	void SynthesisUpgradeGemWeaponSelect();
private:

	//物品类型
	UPROPERTY()
	uint8 EuipType = 0u;

	/*
		道具点击的操作类型
		0 背包装备
		1 合成屋-武器和宝石镶嵌-武器的选择
	*/
	UPROPERTY()
	int32 OperateType = 0;

	//物品网格
	UPROPERTY()
	class UItemCountGridBase* ItemGrid = nullptr;

	//主武器数据
	UPROPERTY()
	FEquipment_WeaponFirst_Data FirstWeapon;
	//副武器数据
	UPROPERTY()
	FEquipment_WeaponSecond_Data SecondWeapon;
	//超级武器数据
	UPROPERTY()
	FEquipment_WeaponSuper_Data SuperWeapon;
};
