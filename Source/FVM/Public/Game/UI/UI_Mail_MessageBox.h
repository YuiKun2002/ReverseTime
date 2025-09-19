// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "Data/Mail/MailDataAssetCache.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "UI_Mail_MessageBox.generated.h"

/**
 *邮件项目
 */
UCLASS()
class FVM_API UUI_Mail_MessageBox : public UWidgetBase
{
	GENERATED_BODY()

public:
	//设置邮件信息
	void SetMailMessageBox(int32 ID, const FMailItemSave& Data);
	//初始化数据
	void InitData();
	//选择
	UFUNCTION(BlueprintCallable)
	void SelectMail();
	//邮件状态更新
	UFUNCTION(BlueprintImplementableEvent)
	void MailStateUpdate(bool bRead);
protected:
	//当前这封邮件的主题显示名称
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* MailTitle = nullptr;
	//当前这封邮件的主题显示名称
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* SendMailPlayerName = nullptr;
private:
	UPROPERTY()
	int32 MailID = -1;
	UPROPERTY()
	FMailItemSave MailData;
	UPROPERTY()
	FMail Mail;
};
