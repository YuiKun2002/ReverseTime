// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/UI/UI_Mail_MessageBox.h"
#include "Game/UI/UI_Mail_GiftBox.h"
#include <Components/TextBlock.h>
#include "Game/UI/UI_Mail.h"
#include <Components/Image.h>
#include <Components/Button.h>

void UUI_Mail_MessageBox::SetMailMessageBox(int32 ID, const FMailItemSave& Data) {
	this->MailID = ID;
	this->MailData = Data;
}

void UUI_Mail_MessageBox::InitData()
{
	this->MailStateUpdate(this->MailData.bRead);

	FMail Data;
	bool bResult;
	//查询邮件
	GET_CACHE(Mail)->GetMailData(this->MailID, Data, bResult);
	if (bResult)
	{
		this->Mail = MoveTempIfPossible(Data);
		this->MailTitle->SetText(FText::FromString(this->Mail.MailTheme));
		this->SendMailPlayerName->SetText(FText::FromString(this->Mail.MailSendPlayer));
	}
}

void UUI_Mail_MessageBox::SelectMail() {
	UUI_Mail* MailUI = GET_UI_CAST(UUI_Mail, Global, Mail);
	if (IsValid(MailUI) && MailUI->IsInViewport())
	{
		this->MailStateUpdate(true);
		MailUI->SelectMail(this->MailID, this->MailData.Count, this->Mail);
	}
}