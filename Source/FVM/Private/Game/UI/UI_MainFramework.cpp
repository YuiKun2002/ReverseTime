// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/UI/UI_MainFramework.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include <Components/Image.h>

//资源初始化
void UUI_MainFramework::Init()
{
	this->ShowMailTip();
}

void UUI_MainFramework::ShowMailTip() {
	if (IsValid(this->Mail_tip))
	{
		int32 Count = UFVMGameInstance::GetPlayerStructManager_Static()->GetMailList().Num();
		if (Count > 0)
		{
			this->Mail_tip->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else {
			this->Mail_tip->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
