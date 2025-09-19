// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "UI_MainFramework.generated.h"

/**
 *主框架UI
 */
UCLASS()
class FVM_API UUI_MainFramework : public UWidgetBase
{
	GENERATED_BODY()
public:
	//资源初始化
	UFUNCTION(BlueprintCallable)
	void Init();
	//显示邮件
	UFUNCTION(BlueprintCallable)
	void ShowMailTip();
protected:
	//邮件提示
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* Mail_tip = nullptr;
};
