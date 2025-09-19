// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "UI_PlayerGetItemBox.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerGetItemBoxRemoveDelegate);

/**
 * 玩家获取物品时的物品提示盒-列表项目
 */
UCLASS()
class FVM_API UUI_PlayerGetItemBox_Item : public UWidgetBase
{
	GENERATED_BODY()

public:

	//当UI项目移除时触发
	UPROPERTY(BlueprintAssignable)
	FPlayerGetItemBoxRemoveDelegate OnRemoveDelegate;

	//移除当前项目
	UFUNCTION(BlueprintCallable)
	void RemoveItem();

	void SetPlayerGetItemBox_Item(FText ItemName, int32 ItemID, int32 ItemLevel, int32 ItemCount);

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

};


/**
 * 描述提示界面
 */
UCLASS()
class FVM_API UUI_DesTip : public UWidgetBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SetContent(FString Cont);

protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class URichTextBlock* Content = nullptr;
};

/**
 * 玩家获取物品时的物品提示盒
 */
UCLASS()
class FVM_API UUI_PlayerGetItemBox : public UWidgetBase
{
	GENERATED_BODY()

public:

	//获取物品获取提示
	UFUNCTION(BlueprintPure)
	static UUI_PlayerGetItemBox* GetPlayerGetItemBoxTipWidget();
	
	//新增物品
	UFUNCTION(BlueprintCallable)
	void AddItemName(FText fItemName, int32 iItemID, int32 iItemLevel, int32 iItemCount = 1);

	//新增物品
	UFUNCTION(BlueprintCallable)
	void AddItem(int32 iItemID, int32 iItemLevel, int32 iItemCount = 1);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UFUNCTION()
	void OnRemoved();


	UFUNCTION()
	void Show();

protected:

	//展示区域
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UVerticalBox* ListBox = nullptr;


private:
	//缓存
	UPROPERTY()
	TArray<class UUI_PlayerGetItemBox_Item*> Items_Cache;
	//队列
	UPROPERTY()
	TArray<class UUI_PlayerGetItemBox_Item*> Items_Queue;
	//延迟动画
	UPROPERTY()
	bool bDelayAnim = false;
	//固定时间
	UPROPERTY()
	float fConstTime = 0.2f;
	//当前累计时间
	UPROPERTY()
	float fTime = 0.f;
};

UCLASS()
class FVM_API UPlayerGetItemBox : public UGameDataStaticObject {

	GENERATED_BODY()

public:
	class UUI_PlayerGetItemBox* GetBox();
private:
	UPROPERTY()
	class UUI_PlayerGetItemBox* Box = nullptr;
};