// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "GameSystem/Item/ItemBaseStruct.h"
#include "GameSystem/Item/ItemBaseGridFunc.h"
#include "ItemGridBase.generated.h"

class UImage;
class UButton;
class UCanvasPanel;

/**
 * 所有网格的基本类型
 * 1.可用于 背包的道具展示
 * 2.或者抽奖的道具，等等
 * 3.包含基本的UI组件：物品等级 物品头像 可以点击的按钮
 */
UCLASS()
class FVM_API UItemGridBase : public UWidgetBase
{
	GENERATED_BODY()

public:
	//点击事件
	UFUNCTION(BlueprintNativeEvent)
	void OnClicked();
	//更新事件-当视图数据出现变化时触发
	UFUNCTION(BlueprintNativeEvent)
	void OnViewUpdate();

	//实现win平台文字移动
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual bool Initialize() override;
public:
	//设置网格加载属性
	virtual	void SetItemAttribute(
		class UItemBaseView* View,
		const FItemViewBlock& BlockData,
		int32 Index);
	//设置网格加载属性-复制到当前实例
	UFUNCTION(BlueprintCallable)
	void SetItemAttributeCopy(UItemGridBase* ItemGrid);
	//设置按钮头像
	UFUNCTION(BlueprintCallable)
	void SetLevelAndButtonImage(int32 Level, class UTexture2D* Texture);
	//设置按钮状态
	UFUNCTION(BlueprintCallable)
	void SetButtonIsEnabled(bool Enabled);
	//设置按钮颜色为偏黑暗色 
	UFUNCTION(BlueprintCallable)
	void SetButtonColorIsDark(bool bValue = true);
	//设置按钮图形匹配大小
	UFUNCTION(BlueprintCallable)
	void SetButtonImageMatchSize();

public:

	//获取按钮
	UFUNCTION(BlueprintPure)
	FORCEINLINE	UButton* GetButton() { return this->Item_Butt; }

	//获取头像图形实例
	UFUNCTION(BlueprintPure)
	FORCEINLINE UObject* GetItemHeadTextureIns() { return this->ItemHeadTextureIns; }

	//获取物品数据视图
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Item View Block"))
	FORCEINLINE void BPGetFItemViewBlock(FItemViewBlock& OutData) { OutData = this->GetItemViewBlock(); }

	//获取物品数据视图
	FORCEINLINE const FItemViewBlock& GetItemViewBlock() const { return this->ItemViewBlockBaseData; };

	//获取物品ID->获取当前UI的位置ID
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetItemID() const { return this->ItemViewBlockBaseData.ItemID; };

	//获取按钮状态
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetButtonIsEnabled() const { return this->bButtonEnabled; }

	//获取颜色状态
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetButtonColorIsDarkColor() const { return this->bDarkColor; }

	FORCEINLINE const int32& GetUiIndex() const { return this->UiIndex; };

protected:

	virtual FString ToString_Implementation() override;
	virtual void OnViewUpdate_Implementation() {};
	virtual void OnClicked_Implementation();

	//设置基础视图数据
	virtual bool SetItemViewBlock(
		const FItemViewBlock* ItemViewBlock,
		class UTexture2D* ItemHeadTexture,
		int32 Index);

	//设置功能对象
	virtual void SetFuncObject(
		UItemBaseGridFunc* Object,
		FItemBaseStructData* DataPtr
	);

	//设置基础视图复制数据
	virtual void SetItemViewBlockCopy(UItemGridBase* CopyGrid);

	//获取功能对象
	UFUNCTION(BlueprintPure)
	UItemBaseGridFunc* GetFuncObject();

	//获取功能接口
	UFUNCTION(BlueprintPure)
	TScriptInterface<class IItemBaseGridFuncInterface> GetFuncInterface();

protected:
	//按钮->调出界面
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Item_Butt = nullptr;
	//物品等级->等级图片显示
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* ItemLevel_Img = nullptr;
	//头像实例
	UPROPERTY()
	class UObject* ItemHeadTextureIns = nullptr;
private:
	//更新等级
	UFUNCTION()
	void UpdateLevelTexture(class UTexture2D* Texture);
	//音效
	UFUNCTION()
	void MouseHoveredAudio();
	//设置按钮绑定
	UFUNCTION()
	void SetBind();
private:
	//UI索引
	UPROPERTY()
	int32 UiIndex = -1;
	//等级状态显示
	UPROPERTY()
	bool bLevelVisiable = false;
	//按钮的状态
	UPROPERTY()
	bool bButtonEnabled = true;
	//颜色状态
	UPROPERTY()
	bool bDarkColor = false;
	//基础数据视图
	UPROPERTY()
	FItemViewBlock ItemViewBlockBaseData;
	//装备功能接口
	UPROPERTY()
	TScriptInterface<class IItemBaseGridFuncInterface> FuncInter;
	//装备功能对象
	UPROPERTY()
	class UItemBaseGridFunc* FuncBaseObject = nullptr;
};


/**
 * 带有数量的格子基础类型
 * 1.可用于背包的道具展示
 * 2.或者抽奖的道具，等等
 * 3.包含基本的UI组件：物品等级 物品头像 物品的显示数量 可以点击的按钮
 */
UCLASS()
class FVM_API UItemCountGridBase : public UItemGridBase
{
	GENERATED_BODY()


public:
	
	//返回字符串
	virtual FString ToString_Implementation() override;

	//设置网格加载属性
	UFUNCTION(BlueprintCallable)
	virtual	void SetItemAttribute(
		class UItemBaseView* View,
		const FItemViewBlock& BlockData,
		int32 Index) override;

	//设置数量文本显示状态
	UFUNCTION(BlueprintCallable)
	void SetItemCountTextShow(bool bShow);

	//设置物品数量
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Item Count"))
	void SetItemAttribute_ItemCount(int32 NewCount) { this->SetItemCount(NewCount); }

	//获取材料的视图数据
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Item Count View Block"))
	FORCEINLINE void BPGetItemCountViewBlock(FItemCountViewBlock& OutData) { OutData = this->GetItemCountViewBlock(); }

	//获取材料的视图数据
	FORCEINLINE const FItemCountViewBlock& GetItemCountViewBlock()const { return this->CurItemContViewBlock; };

protected:
	//设置物品视图数据
	virtual	bool SetItemCountViewBlock(
		const FItemViewBlock* ItemViewBlock,
		const FItemCountViewBlock* Data,
		int32 Index
	);
	//设置基础视图复制数据
	virtual void SetItemViewBlockCopy(UItemGridBase* CopyGrid) override;
	//设置物品数量
	virtual	void SetItemCount(int32 NewCount);
protected:
	//数量文本
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Count_Text = nullptr;
	//数量是否显示
	UPROPERTY()
	bool bCountVisiable = false;
	//数据
	UPROPERTY()
	FItemCountViewBlock CurItemContViewBlock;
};