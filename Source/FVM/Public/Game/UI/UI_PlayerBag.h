// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "Game/UI/Tips/PayObject.h"
#include "GameSystem/Item/ItemStruct.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "GameSystem/Tools/ItemLoadManager.h"
#include "Data/MaterialDataStruct.h"
#include "GameSystem/PlayerStructManager.h"
#include "UI_PlayerBag.generated.h"

class UImage;
class UWidget;
class UVerticalBox;
class UCanvasPanel;
class UUniformGridPanel;
class UUI_CardSkillTable;
class UUI_PlayerBagRemove;
class UUI_PlayerInformationShow;

/*
背包购买规则
*/
UCLASS()
class FVM_API UPlayerBagPayObject : public UPayObject
{
	GENERATED_BODY()

public:

	virtual	bool Pay_Implementation() override;

};

/**
 *玩家背包
 */
UCLASS()
class FVM_API UUI_PlayerBag : public UWidgetBase
{
	GENERATED_BODY()

public:

	virtual bool Initialize() override;

	//设置背包格子图片以及文字
	int32 SetBagGridStyle(int32 M_Count, const TArray<FPlayerBagGird>& _BagGrid);

public:

	//获取当前界面的索引
	UFUNCTION(BlueprintPure)
	uint8 GetSelectPanelIndex();
	//获取当前选择的装备背包索引
	UFUNCTION(BlueprintPure)
	int32 GetSelectEquipBagIndex();
	//加载背包格子的数据
	UFUNCTION()
	TArray<FPlayerBagGird>& GetBagGridData(uint8 _PanelNumber);
public:
	//加载器初始化
	UFUNCTION(BlueprintCallable)
	void LoaderInit();
	//重新加载当前物品数据
	UFUNCTION(BlueprintCallable)
	void RelaodCurrentItemData();

	//加载道具界面->道具
	UFUNCTION(BlueprintCallable)
	void LoadItemsData();
	//加载角色装备的背包【背包装备界面】
	UFUNCTION()
	void LoadPlayerBags(UUI_PlayerBag* _Class, TArray<FItemViewBlock>& _Items_, int32 _Index);

	//加载卡片数据->防御卡界面
	UFUNCTION(BlueprintCallable)
	void LoadCardData();
	//加载材料数据->材料界面 
	UFUNCTION(BlueprintCallable)
	void LoadMaterialData();

	//加载防御卡技能书选项卡(并且设置加载分类)
	UFUNCTION(BlueprintCallable)
	void LoadCardSkillBookTabs(ESkillBookTipCategory M_ESkillBookTipCategory);
	//加载防御卡技能书选项卡(使用默认的加载分类或者使用上一次的分类加载)
	UFUNCTION(BlueprintCallable)
	void LoadCardSkillBookTabs_DefCategory();

	//更新背包容量[返回总容量]
	UFUNCTION(BlueprintCallable)
	int32 UpdateBagCount(uint8 _UpdatePanel, int32 Count);
	//更新货币显示
	UFUNCTION(BlueprintCallable)
	void UpdateShowCoinText();
	//更新角色形象
	UFUNCTION()
	void RefreshPlayerSuit();

	//购买背包装备格子
	UFUNCTION()
	void BagGridButton_BuyGrid(uint8 _Panel_Index, uint8 _Gird_Index, int32 _Price);
	//购买背包装备格子-购买结果
	UFUNCTION()
	void BagGridButton_BuyGridResult(bool bResult);
	//使用背包装备格子
	UFUNCTION()
	void BagGridButton_EquipAndUnEquip(uint8 _PanelIndex, uint8 _BagNumber, int32 _Price);

	UFUNCTION()
	void BagGridButton_1_Func();
	UFUNCTION()
	void BagGridButton_2_Func();
	UFUNCTION()
	void BagGridButton_3_Func();
	UFUNCTION()
	void BagGridButton_4_Func();

	//整理物品
	UFUNCTION(BlueprintCallable)
	void ArrangementItem();

	//整理物品设置界面
	UFUNCTION(BlueprintCallable)
	void ArrangementItemSetting();

	//打开删除物品的界面
	UFUNCTION()
	void OpenRemoveItemPanel();

	//关闭选择背包道具的界面
	UFUNCTION()
	void ClosePlayerBagEquipPanel();
	//关闭BagUI界面
	UFUNCTION(BlueprintCallable)
	void CloseBagPanel();
protected:

	//背包金币文字显示
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Coin_0 = nullptr;
	//背包礼券文字显示
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Coin_1 = nullptr;
	//背包点券文字显示
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Coin_2 = nullptr;

	//---------------------------------------
	//背包格子文字总容量显示
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* PlayerBagCountShowText = nullptr;

	//背包格子装备栏 1
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* PlayerBag_0_CountButt = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* PlayerBag_0_CountImg = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* PlayerBag_0_CountShowText = nullptr;

	//背包格子装备栏 2
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* PlayerBag_1_CountButt = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* PlayerBag_1_CountImg = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* PlayerBag_1_CountShowText = nullptr;

	//背包格子装备栏 3
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* PlayerBag_2_CountButt = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* PlayerBag_2_CountImg = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* PlayerBag_2_CountShowText = nullptr;

	//背包格子装备栏 4
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* PlayerBag_3_CountButt = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* PlayerBag_3_CountImg = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* PlayerBag_3_CountShowText = nullptr;

	//背包装备栏选择界面
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* PlayerBagEquipPanel = nullptr;
	//背包装备栏列表内容
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUniformGridPanel* PlayerBagEquipPanel_Uniform = nullptr;

	//背包物品删除按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* RemoveItems_Butt = nullptr;

	//-------------------------------------------------------------------------角色基础着装
	//角色创建点位
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* PlayerPoint = nullptr;
	UPROPERTY()
	UUI_PlayerInformationShow* M_PlayerInformationShow = nullptr;

	//-------------------------------------------------------------------------技能书界面

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UScrollBox* PlayerBag_SkillBooks_Scroll = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUniformGridPanel* PlayerBag_SkillBooks_Uniform = nullptr;

	//技能书分类
	UPROPERTY()
	ESkillBookTipCategory M_CardSkillBooksCategory = ESkillBookTipCategory::S_Normal;
	//按钮组
	UPROPERTY()
	TArray<UButton*> M_SkillBookButtons;
	//默认图片
	UPROPERTY()
	TArray<FString> M_SkillBookDefStyle;
	//点击图片
	UPROPERTY()
	TArray<FString> M_SkillBookClickStyle;
	//技能书索引存储器
	UPROPERTY()
	TArray<FSkillBookData> M_FItemData_Show_CardSkillBooks;

	//功能
	UFUNCTION(BlueprintCallable)
	void Open_SkillBooks_Normal();
	UFUNCTION()
	void Open_SkillBooks_Style0();
	UFUNCTION()
	void Open_SkillBooks_Style1();
	UFUNCTION()
	void Open_SkillBooks_Style2();

private:

	//卡片视图
	UPROPERTY()
	class UPlayerBagCardView* PlayerBagCardView = nullptr;
	//装备视图
	UPROPERTY()
	class UPlayerBagEquipView* PlayerBagEquipView = nullptr;
	//材料视图
	UPROPERTY()
	class UPlayerBagMaterialView* PlayerBagMaterialView = nullptr;

	//加载结构(角色背包加载器)
	FTimerLoad<UUI_PlayerBag, FItemViewBlock> TimeLoad_PlayerBags;

	//装备加载器
	UPROPERTY()
	UItemLoaderObject* TimeLoad_Equipment_Manager = nullptr;
	//创建装备道具界面
	UWidget* WidgetCreate_Equipment(const UItemLoadDataBlock* const _Data, int32 _Index);
	//刷新装备道具界面
	void WidgetRefresh_Equipment(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget);

	//防御卡加载器
	UPROPERTY()
	UItemLoaderObject* TimeLoad_Cards_Manager = nullptr;
	//创建卡片道具界面
	UWidget* WidgetCreate_Cards(const UItemLoadDataBlock* const _Data, int32 _Index);
	//刷新卡片道具界面
	void WidgetRefresh_Cards(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget);

	//材料加载器
	UPROPERTY()
	UItemLoaderObject* TimeLoad_Materials_Manager = nullptr;
	//创建材料道具界面
	UWidget* WidgetCreate_Materials(const UItemLoadDataBlock* const _Data, int32 _Index);
	//刷新材料道具界面
	void WidgetRefresh_Materials(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget);

	//技能书加载器
	UPROPERTY()
	UItemLoaderObject* TimeLoad_CardSkillBooks_Manager = nullptr;
	//创建防御卡技能书道具界面
	UWidget* WidgetCreate_CardSkillBooks(const UItemLoadDataBlock* const _Data, int32 _Index);
	//刷新防御卡技能书道具界面
	void WidgetRefresh_CardSkillBooks(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget);

private:
	//当前选择的界面->索引值 ->装备界面 0  卡片界面1  材料界面2
	UPROPERTY()
	uint8 M_Panel_Index = 0U;
	//当前选择装备背包的索引
	UPROPERTY()
	int32 M_SelectEquipBag_Index = 0;
	//背包道具数据组
	UPROPERTY()
	TArray<FItemViewBlock> PlayerBagListTableNormalArray;
	//道具删除界面
	UPROPERTY()
	TArray<UUI_PlayerBagRemove*> M_UUI_PlayerBagRemove_Panels;
	//加载道具的格子
	UPROPERTY()
	TArray<UUniformGridPanel*> M_PlayerItemPanels;
};
