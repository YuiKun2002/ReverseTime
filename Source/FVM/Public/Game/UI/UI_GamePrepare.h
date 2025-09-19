// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "GameSystem/Item/ItemStruct.h"
#include "GameSystem/Tools/ItemLoadManager.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "UI_GamePrepare.generated.h"



class UUniformGridPanel;
class UUI_PlayerBagCardGrid;
class UHorizontalBox;
class UVerticalBox;
class UUI_TableTextBlock;
class UUI_PlayerShow;
class UImage;


//委托绑定
#define GP_DELEGATE_FUNC_SELECTCURRENTCARD TEXT("SelectCurrentCard")
#define GP_DELEGATE_FUNC_REMOVECURRENTSELECTCARD TEXT("RemoveCurrentSelectCard")

//卡片显示的列数【加载卡片时，加载多少列】
#define GP_LOADCARDUICOLSNUMS 9
//卡片最大的等级
#define GP_CARDMAXLEVEL 16

/**
 *   游戏准备界面
 */
UCLASS()
class FVM_API UUI_GamePrepare : public UWidgetBase
{
	GENERATED_BODY()
public:
	//获取准备UI界面
	UFUNCTION(BlueprintPure)
	static UUI_GamePrepare* GetGamePrepareUiStatic();
	//获取当前选择的卡片数量
	UFUNCTION(BlueprintPure)
	int32 GetSelectCardCount() const { return this->CurrentSelectCardNum; }

public:
	//从角色背包中加载拥有的所有卡片
	UFUNCTION(BlueprintCallable)
	void LoadCardList(class UFVMGameInstance* GameIns);
	//加载选择的卡片配置
	UFUNCTION(BlueprintCallable)
	void LoadCardSelectConfigList(class UFVMGameInstance* GameIns);
	//保存配置
	UFUNCTION(BlueprintCallable)
	void SaveLocalPlayerData();

	//加载老鼠列表
	UFUNCTION(BlueprintCallable)
	void LoadMouseList();
public:
	//当选择卡片时
	void OnSelectCard(UUI_PlayerBagCardGrid* CurSelectBagCardGrid);
	//当移除卡片时
	void OnRemoveCard(UUI_PlayerBagCardGrid* CurSelectBagCardGrid);
protected:
	UUI_PlayerBagCardGrid** GetValidGrid(uint8 TotalSize);
protected:
	//老鼠的名称列表
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* MouseList = nullptr;
	//卡片列表
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UUniformGridPanel* CardGrid = nullptr;
	//选择的卡片列表
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* SelectCardList = nullptr;
protected:
	//当前卡片待选择列表
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, UUI_PlayerBagCardGrid*> CardUiPool;
	//当前卡片已选择列表
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, UUI_PlayerBagCardGrid*> CardSelectUiPool;
	//选择卡片的数量->Max 18
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentSelectCardNum = 0;
	//玩家存档
	UPROPERTY()
	class UPlayerStructManager* LocalPlayerData = nullptr;
};
