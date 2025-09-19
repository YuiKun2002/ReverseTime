// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Game/UI/Tips/UI_ItemDesTip.h"
#include "UI_GiftItemTip.generated.h"


/**
 * 礼盒物品列表项目
 */
UCLASS()
class FVM_API UUI_GiftItemTip_Item : public UWidgetBase
{
	GENERATED_BODY()

public:

	//设置项目数据
	void SetGiftItemItemData(
		int32 iItemID,
		int32 iItemLevel,
		FText tItemName,
		int32 iItemCount,
		float fRandomValue
	);

protected:
	//物品头像
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* ItemHead = nullptr;
	//物品等级
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* ItemLevel = nullptr;
	//物品名称
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ItemName = nullptr;
	//物品数量
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ItemCount = nullptr;
	//物品概率
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ItemValue = nullptr;
};


/**
 * 礼盒提示窗口
 */
UCLASS()
class FVM_API UUI_GiftItemTip : public UUI_ItemDesTip
{
	GENERATED_BODY()

public:

	//设置物品的最大数量
	UFUNCTION(BlueprintCallable)
	void SetItemCountMax(int32 MaxCount);

	//设置界面提示
	UFUNCTION(BlueprintCallable)
	void SetTipContent(FString RechTextContent);

	//追加界面提示
	UFUNCTION(BlueprintCallable)
	void AddTipContent(FString RechTextContent);

	//添加展示物品
	UFUNCTION(BlueprintCallable)
	void AddGiftItemTipItem(class UUI_GiftItemTip_Item* GiftItemTip_Item);

	//设置礼盒数据对象
	UFUNCTION(BlueprintCallable)
	void SetEquipmentFuncGiftObject(UObject* EquipmentFuncGiftIns);


	//获取物品数量
	UFUNCTION(BlueprintPure)
	int32 GetUseCount();

	//获取礼盒数据对象
	UFUNCTION(BlueprintPure)
	UObject* GetEquipmentFuncGiftObject();

	//获取礼盒物品展示UI
	UFUNCTION(BlueprintPure)
	class UUI_GiftItemTip_Item* GetGiftItemTipItem();

protected:

	//获取物品数量
	UFUNCTION(BlueprintPure)
	int32 GetItemCount();

	//获取提示文本
	UFUNCTION(BlueprintPure)
	void GetTipContent(FString& OutStr);

	//更新使用数量
	UFUNCTION(BlueprintCallable)
	void UpdateUseCount(const int32& Count);

private:

	//提示内容
	UPROPERTY()
	FString TipContent;

	//使用数量
	UPROPERTY()
	int32 UseCount = 1;

	//物品最大数量
	UPROPERTY()
	int32 ItemCountMax = 1;

	//礼盒数据对象
	UPROPERTY()
	UObject* FuncGiftIns = nullptr;

	//展示项UI数组
	UPROPERTY()
	TArray<class UWidget* > TipItems;

};
