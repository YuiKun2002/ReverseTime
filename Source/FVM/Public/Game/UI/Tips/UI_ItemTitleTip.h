// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "GameSystem/GameDataSubsystem.h"
#include "UI_ItemTitleTip.generated.h"

/**
 * 争对Windows平台   当鼠标移入某个地方，显示标题
 */

UCLASS()
class FVM_API UUI_ItemTitleTip : public UWidgetBase
{
	GENERATED_BODY()
public:
	//--------------------------------------------------[UI界面]-------------------------------------------------------
	//创建一个基础标题提示(显示名称，是否显示)
	UFUNCTION(BlueprintCallable)
	void CreateBaseTipWidget(const FString& _ShowTitle, bool _bShow);
	//--------------------------------------------------[UI界面]-------------------------------------------------------

	UFUNCTION(BlueprintImplementableEvent)
	void SetText(const FString& Title);

	//显示界面提示
	UFUNCTION(BlueprintCallable)
	static void ShowWidgetTip(UWidgetBase* WidgetInstance, bool bShowState);

private:
	//当前显示状态
	UPROPERTY()
	bool bShowState = false;


};


//UI提示
UCLASS()
class FVM_API UItemTitleTip : public UGameDataStaticObject {

	GENERATED_BODY()
public:
	class UUI_ItemTitleTip* Get();
private:
	UPROPERTY()
	class UUI_ItemTitleTip* Tip = nullptr;
};