// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Game/UI/Tips/UI_SelectTip.h"
#include "GameSystem/Item/ItemBaseStruct.h"
#include "UI_NotUnlockedItemTip.generated.h"

/**
 * 未解锁的物品提示框 
 * 1.绑定Ok键位可以生效
 * 2.绑定Cancel键位无效！
 */
UCLASS()
class FVM_API UUI_NotUnlockedItemTip : public UUI_SelectTip
{
	GENERATED_BODY()

public:

	//创建未解锁的物品提示框UI
	UFUNCTION(BlueprintCallable)
	static UUI_NotUnlockedItemTip* CreateNotUnlockedItemTip(
		int32 CreateType, //创建的类型
		FItemViewBlock Block,//类型ID数据
		FText Text//提示文本
	);

protected:
	//创建一个装备提示
	UFUNCTION(BlueprintCallable)
	void CreateEquip(const FItemViewBlock& Block);

	//创建一个卡片提示
	UFUNCTION(BlueprintCallable)
	void CreateCard(const FItemViewBlock& Block);

	//创建一个材料提示
	UFUNCTION(BlueprintCallable)
	void CreateMater(const FItemViewBlock& Block);

protected:

	//显示区域
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UHorizontalBox* ShowArea = nullptr;

private:
	UPROPERTY()
	class UItemCountGridBase* EquipGrid = nullptr;
	UPROPERTY()
	class UItemCountGridBase* CardGrid = nullptr;
	UPROPERTY()
	class UItemCountGridBase* MaterGrid = nullptr;
};
