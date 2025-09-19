// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "UI_Mail.generated.h"



class UImage;
class UButton;
class UTextBlock;
class UMultiLineEditableText;
class UHorizontalBox;
class UVerticalBox;

// Mail
// MessageBox
// GiftBox

/**
 *邮件UI
 */
UCLASS()
class FVM_API UUI_Mail : public UWidgetBase
{
	GENERATED_BODY()

public:

	//提取邮件奖励
	UFUNCTION(BlueprintCallable)
	void GetMail();

	//加载邮件列表
	UFUNCTION(BlueprintCallable)
	void LoadList();

	//更新邮件领取按钮
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMailGetState(bool bMailGet);

	//更新邮件奖励内容
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMailGiftContentState(bool bShow);

	//选择邮件
	UFUNCTION()
	void SelectMail(int32 ID, int32 MailCount, const FMail& Data);
	void ShowGiftContent();

	//关闭邮件
	UFUNCTION(BlueprintCallable)
	void CloseMailPanel();

protected:

	//邮件接收者的名称【玩家名称】
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* RecvPlayerName_Text = nullptr;

	//当前这封邮件的主题显示名称
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* MailTheme_Text = nullptr;

	//当前这封邮件的文字内容
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* MailContent_Text = nullptr;

	//邮件项目的展示容器
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UVerticalBox* MailVerticalList = nullptr;

	//邮件的奖励内容展示容器
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UVerticalBox* MailGiftContentVerticalList = nullptr;

private:
	//时间
	FTimeClip M_FTimeClip_0;
	//当前邮件数量
	int32 CurMailCount = 0;
	//当前容器数量
	int32 CurBoxCount = 0;
	//当前邮件数量
	int32 CurMailNum = 0;
	//邮件ID
	int32 MailID = -1;
	//当前指令
	FString Cmd;
	//礼盒内容
	TArray<FItemViewBlockGift> ListData;
};
