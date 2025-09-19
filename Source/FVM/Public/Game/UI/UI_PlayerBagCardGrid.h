// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/UI/ItemGridBase.h"
#include "UI_PlayerBagCardGrid.generated.h"


/**
 * 背包卡片格子
 */
UCLASS()
class FVM_API UUI_PlayerBagCardGrid : public UItemCountGridBase
{
	GENERATED_BODY()

public:

	virtual FString ToString_Implementation() override;

	virtual void OnClicked_Implementation() override;

	//显示卡片提示窗口
	UFUNCTION(BlueprintCallable)
	void ShowCardTip();

	//设置卡片操作
	FORCEINLINE	void SetCardOperateType(int32 NewType) { this->OperType = NewType; }

protected:

	//设置价格
	virtual	void SetItemCount(int32 NewCount) override;

private:
	/*----------------------------------------卡片选择【准备界面】----------------------------------------*/

	//选择当前卡片(游戏准备-选卡)
	UFUNCTION()
	void SelectCurrentCard();
	//移除当前选择的卡片
	UFUNCTION()
	void RemoveCurrentSelectCard();
	/*----------------------------------------卡片选择【准备界面】----------------------------------------*/

protected:
	/*设置操作类型[基类表示]
	0，显示卡片详细数据
	1，关卡卡片选择阶段-卡片选择
	2，关卡卡片选择阶段-卡片移除

	*/

	UPROPERTY()
	int32 OperType = 0;
};
