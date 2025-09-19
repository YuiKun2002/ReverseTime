// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PayObject.generated.h"

//支付结果
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPayResultDelegate, bool, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPayCancelDelegate);

/*
	支付接口
*/
UINTERFACE(MinimalAPI)
class  UPayInterface : public UInterface {
	GENERATED_BODY()
};

class FVM_API IPayInterface {

	GENERATED_BODY()

public:

	//支付
	UFUNCTION(BlueprintNativeEvent)
	bool Pay();
	virtual bool Pay_Implementation() { return false; };


	//取消支付
	UFUNCTION(BlueprintNativeEvent)
	void CancelPay();
	virtual void CancelPay_Implementation() {};
};

/*
	支付对象
*/
UCLASS()
class FVM_API UPayObject : public UObject, public IPayInterface
{
	GENERATED_BODY()

	friend class UUI_ShopPayTip;

public:
	//支付结果
	UPROPERTY(BlueprintAssignable)
	FPayResultDelegate PayResultDelegate;
	//支付取消
	UPROPERTY(BlueprintAssignable)
	FPayCancelDelegate PayCancelDelegate;

protected:
	//支付
	virtual	bool Pay_Implementation() override;
	//取消
	virtual void CancelPay_Implementation() override;
protected:
	//获取支付UI界面
	UFUNCTION(BlueprintPure)
	class UUI_ShopPayTip* GetPayUI();
private:
	//支付UI
	UPROPERTY()
	class UUI_ShopPayTip* PayUI = nullptr;
};