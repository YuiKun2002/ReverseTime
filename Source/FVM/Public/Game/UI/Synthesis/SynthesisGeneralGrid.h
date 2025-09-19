// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/UI/UI_PlayerSynthesis.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "SynthesisGeneralGrid.generated.h"

/**
 *通用方形盒体UI类型
 */
UCLASS()
class FVM_API UUI_SynModel_GeneralBox : public UItemGridBase {

	GENERATED_BODY()
public:
	virtual FString ToString_Implementation() override;
	virtual void OnClicked_Implementation() override;

	//取消操作
	UFUNCTION(BlueprintCallable)
	void Cancel();
protected:

	//取消选择
	UFUNCTION(BlueprintImplementableEvent)
	void CancelSelectEvent();
};

/**
 *通用长方形盒体UI类型
 */
UCLASS()
class FVM_API UUI_SynModel_GeneralRectangleBox : public UUI_PlayerBagCardGrid
{
	GENERATED_BODY()
public:
	virtual FString ToString_Implementation() override;
	virtual void OnClicked_Implementation() override;
	virtual	void SetItemAttribute(class UItemBaseView* View, const FItemViewBlock& BlockData, int32 Index) override;

	//取消操作
	UFUNCTION(BlueprintCallable)
	void Cancel();
protected:
	
	virtual void SetItemViewBlockCopy(UItemGridBase* CopyGrid) override;

	//取消选择
	UFUNCTION(BlueprintImplementableEvent)
	void CancelSelectEvent();
};