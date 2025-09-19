// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "Data/ShopItemPriceStruct.h"
#include "UI_ShopPayTip.generated.h"

class UButton;
class UTextBlock;
class UEditableTextBox;

/*
	支付UI
*/
UCLASS()
class FVM_API UUI_ShopPayTip : public UWidgetBase
{
	GENERATED_BODY()

	friend class UPayObject;

public:

	//设置物品价格
	UFUNCTION(BlueprintCallable)
	void SetItemPrice(const FItemPrice& ItemPrice);

	//设置标题
	UFUNCTION(BlueprintCallable)
	void SetPayTitle(FText Title);

	//设置内容
	UFUNCTION(BlueprintCallable)
	void SetPayContent(FText Content);

	//设置支付价格
	UFUNCTION(BlueprintCallable)
	void SetPayPrice(FText PriceText);

	//设置支付货币的名称
	UFUNCTION(BlueprintCallable)
	void SetPayCoinName(FText Name);

	//设置输入隐藏【静止玩家输入】
	UFUNCTION(BlueprintCallable)
	void SetInputHidden(bool bHidd = true);

public:
	//获取支付对象
	UFUNCTION(BlueprintPure)
	class UPayObject* GetPayObject();
	//获取物品价格
	UFUNCTION(BlueprintPure)
	void GetItemPrice(FItemPrice& OutItemPrice) const;
	//获取购买数量
	UFUNCTION(BlueprintPure)
	int32 GetPayItemCount();
private:
	//支付
	UFUNCTION()
	void PayButton();
	//获取输入框
	UFUNCTION()
	UEditableTextBox* GetEditableTextBox();
	//获取输入数字的按钮
	UFUNCTION()
	UButton* GetInputTextButton();
public:
	//输入数字【文字输入】
	UFUNCTION(BlueprintCallable)
	void InputNum(FString _value);
	//删除一个【文字输入】
	UFUNCTION(BlueprintCallable)
	void PopText();
public:
	virtual bool Initialize() override;

protected:
	//支付标题
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* PayTitle = nullptr;
	//支付内容
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* PayContent = nullptr;
	//价格
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Price = nullptr;
	//支付的货币名称
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* CoinName = nullptr;
	//输入盒
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UHorizontalBox* InputBox = nullptr;
	//输入的需要支付的物品个数
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* PlayerPayCountTextBox = nullptr;
	//支付按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* PlayerPayButton = nullptr;
	//输入个数按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* PlayerInputButton = nullptr;
private:
	//检查支付情况
	UFUNCTION(BlueprintCallable)
	void CheckPay();
private:
	//通过键盘输入的物品数量
	UPROPERTY()
	TArray<FString> _Arrays_CoinNums;
	//价格
	UPROPERTY()
	FItemPrice ItemPriceData;
private:
	UPROPERTY()
	class UPayObject* PayObject = nullptr;
};
