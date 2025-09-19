// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "Game/UI/ItemGridBase.h"
#include "UI_Mail_GiftBox.generated.h"

/**
 *	邮件展示
 */
UCLASS()
class FVM_API UUI_Mail_GiftBox : public UItemGridBase
{
	GENERATED_BODY()

public:

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override {
		return FReply::Unhandled();
	};
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override {};

	virtual	void OnClicked_Implementation() override;

	void SetPlayerGetItemBox_Item(FText ItemName, int32 ItemID, int32 ItemLevel, int32 ItemCount);

protected:

	//物品头像
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* ItemHead = nullptr;

	//物品名称
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ItemName = nullptr;

	//物品数量
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ItemCount = nullptr;

private:
	UPROPERTY()
	int32 ID = -1;
};
