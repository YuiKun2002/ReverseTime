// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "UI_SelectTip.generated.h"

DECLARE_DYNAMIC_DELEGATE(FSelectClickedDelegate);

/**
 *选择提示
 */
UCLASS()
class FVM_API UUI_SelectTip : public UWidgetBase
{
	GENERATED_BODY()
public:

	//绑定Ok按钮
	void BindOk(FScriptDelegate Callback);
	UFUNCTION(BlueprintCallable)
	void BindOkDelegate(FSelectClickedDelegate ClickEvent);

	//绑定Cancel按钮
	void BindCancel(FScriptDelegate Callback);
	UFUNCTION(BlueprintCallable)
	void BindCancelDelegate(FSelectClickedDelegate ClickEvent);

	//设置内容文本
	UFUNCTION(BlueprintCallable)
	void SetContentText(FText Text);

	//创建选择提示UI
	UFUNCTION(BlueprintCallable)
	static UUI_SelectTip* CreateSelectTip();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Ok = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Cancel = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Content = nullptr;

	//实现两个界面的交替
	UPROPERTY()
	UUI_SelectTip* SecondSelectTipWidgetBuffer = nullptr;
protected:
	UFUNCTION()
	void BindDef(class UButton* But);
	UFUNCTION()
	void RemoveCur();
};
