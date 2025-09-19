// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "ItemBaseGridFunc.generated.h"

/**
 * 功能接口
 */
UINTERFACE(MinimalAPI)
class UItemBaseGridFuncInterface : public UInterface
{
	GENERATED_BODY()
};

class FVM_API IItemBaseGridFuncInterface
{
	GENERATED_BODY()

public:
	// 纯虚函数，必须在实现类中实现
	virtual void Execute(
		class UItemGridBase* GridPtr,
		struct FItemBaseStructData* ItemBaseDataPtr
	) = 0;
};


/**
 * 物品基础格子函数
 */
UCLASS()
class FVM_API UItemBaseGridFunc : public UObject, public IItemBaseGridFuncInterface
{
	GENERATED_BODY()

	friend class UItemCardBaseGridFunc;

public:

	//获取提示UI
	UFUNCTION(BlueprintPure)
	class UUI_ItemDesTip* GetTipWidget();

protected:

	virtual void Execute(
		class UItemGridBase* GridPtr,
		struct FItemBaseStructData* ItemBaseDataPtr
	) override;

	//设置提示UI
	UFUNCTION(BlueprintCallable)
	void SetTipWidget(class UUI_ItemDesTip* TipWidgetInstance);

private:

	UPROPERTY()
	class UUI_ItemDesTip* TipComp = nullptr;
};

/**
 * 物品卡片基础格子函数
 */
UCLASS()
class FVM_API UItemCardBaseGridFunc : public UItemBaseGridFunc {

	GENERATED_BODY()

	friend class UItemBaseGridFunc;

public:

	//获取卡片格子
	UFUNCTION(BlueprintPure)
	FORCEINLINE	class UItemCountGridBase* GetCardGrid() { return this->CardGrid; }

	//获取卡片提示UI
	UFUNCTION(BlueprintPure)
	class UUI_CardItemTip* GetCardTipWidget();


protected:

	virtual void Execute(
		class UItemGridBase* GridPtr,
		struct FItemBaseStructData* ItemBaseDataPtr
	) override;

protected:

	//卡片格子
	UPROPERTY()
	class UItemCountGridBase* CardGrid = nullptr;

};
