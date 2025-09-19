// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStart/VS/UI/UI_Card.h"
#include "GameStart/VS/CardCoolDownManager.h"
#include "GameStart/VS/Components/CardManagerComponent.h"
#include "GameStart/VS/Components/ResourceManagerComponent.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "GameStart/Flipbook/GameActor/CardActor.h"
#include "Data/Texture/TextureDataAssetCache.h"
//地图实例
#include "GameStart/VS/GameMapInstance.h"
//卡片管理器
#include "GameStart/VS/Components/CardManagerComponent.h"

#include <Components/Image.h>
#include <Components/Button.h>
#include <Components/TextBlock.h>
#include <Components/CanvasPanelSlot.h>

//用于拖拽时，创建对应卡片的动画
#include "GameStart/Flipbook/GameActorFlipbookBase.h"
//卡片冷却管理器【一张卡对应一个卡片冷却管理器】
#include "GameStart/VS/CardCoolDownManager.h"

#include <kismet/GameplayStatics.h>


bool UUI_Card::Initialize()
{
	if (!Super::Initialize())
		return false;

	//this->M_Card_Butt = this->GetWidgetComponent<UButton>(this, TEXT("Bg"));
	this->M_Card_Select_Head = this->GetWidgetComponent<UImage>(this, TEXT("Card_Head"));
	this->M_UImage_Grade = this->GetWidgetComponent<UImage>(this, TEXT("Grade"));

	if (IsValid(AGameMapInstance::GetGameMapInstance()))
	{
		this->CardManager = AGameMapInstance::GetGameMapCardManagerStatic();
	}
	//this->M_Card_Select_Head->OnMouseButtonDownEvent.BindUFunction(this, TEXT(""));

	return true;
}

bool UUI_Card::GetCoolDownFinish()
{
	if (this->M_CardID == -1)
		return false;

	if (this->CardManager->GetCardCoolDownFinish(this->M_CardID))
	{
		if (AGameMapInstance::GetGameMapResourceManagerStatic()->M_FlameNum >= this->M_NeedFlame)
		{
			return true;
		}
	}

	return false;

}

float UUI_Card::GetCoolDownFinishRate()
{
	if (this->M_CardID == -1)
	{
		return false;
	}

	return this->CardManager->GetCardCoolDownManagerByID(this->M_CardID)->GetFinishRate();
}

int32 UUI_Card::GetCardID() const
{
	return this->M_CardID;
}

int32 UUI_Card::GetCardFlame() const {
	return this->M_NeedFlame;
}

bool UUI_Card::GetbEnableCopy() const
{
	return this->bEnableCopy;
}

UClass* const UUI_Card::GetCardClassInstance()
{
	return this->CardSourceInstance;
}

ACardCoolDownManager* UUI_Card::GetCardCoolDownManager()
{
	return this->CardCoolDownManager;
}

void UUI_Card::SetCardID(int32 ID) {
	this->M_CardID = ID;
}

void UUI_Card::SetCardFlame(int32 FlameNum, bool bUp) {

	if (this->M_NeedFlame != FlameNum)
	{
		this->M_NeedFlame = FlameNum;

		if (bUp)
		{
			this->CardFlame->SetText(FText::FromString(FString::FromInt(FlameNum) + TEXT("+")));
		}
		else {
			this->CardFlame->SetText(FText::FromString(FString::FromInt(FlameNum)));
		}
	}
}

void UUI_Card::SetbEnableCopy(bool bCopy)
{
	this->bEnableCopy = bCopy;
}

void UUI_Card::SetCardLevelTexture(const int32& _Grade)
{
	if (_Grade > 0 && _Grade < 17)
	{
		FSlateBrush Image;
		Image.SetResourceObject(GET_CACHE(Texture)->GetItemLevelTexture(_Grade));
		Image.DrawAs = ESlateBrushDrawType::Image;
		this->M_UImage_Grade->SetBrush(Image);
		this->M_UImage_Grade->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else {
		this->M_UImage_Grade->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUI_Card::SetCoolDownFinish()
{
	this->CardManager->GetCardCoolDownManagerByID(this->M_CardID)->SetCoolDownFinish();
}

void UUI_Card::SetCardSelect(bool _bSelect)
{
	this->bSelect = _bSelect;
}

void UUI_Card::SetMouseEnter(bool _bEnter)
{
	this->bMouseEnter = _bEnter;
}

void UUI_Card::SetCardSourcePath(const FSoftClassPath& Path)
{
	TSoftClassPtr<ACardActor> CardResourceClassPtr(Path);
	this->CardSourceInstance = UGameSystemFunction::LoadRes(CardResourceClassPtr);
}

void UUI_Card::SetCardCoolDownManager(ACardCoolDownManager* CoolDownManager) {
	this->CardCoolDownManager = CoolDownManager;
}

void UUI_Card::SetCardClassInstance(UClass* Instance) {
	this->CardSourceInstance = Instance;
}

void UUI_Card::PlaySelectCardAnimation(bool bForward)
{
	////Windows
	//if (UGameplayStatics::GetPlatformName().Equals(TEXT("Windows")))
	//{
	//	//播放选择动画
	//	if (bForward)
	//		this->PlayAnimation(this->GetWidgetTreeOwningClass()->Animations[0], 0.f, 1, EUMGSequencePlayMode::Forward);
	//	else
	//		//鼠标移出 才可以播放逆序
	//		if (!this->bMouseEnter)
	//			this->PlayAnimation(this->GetWidgetTreeOwningClass()->Animations[0], 0.f, 1, EUMGSequencePlayMode::Reverse);
	//}

}

UPaperFlipbook* UUI_Card::CreateCard3DSceneFlipbook()
{
	//return AGameMapInstance::GetGameMapCardManagerStatic()->GetCardIdleAnimByName(this->M_Card_ActorName);
	return nullptr;
}

UButton* UUI_Card::GetCardButtonA()
{
	return this->M_Card_Butt;
}

UImage* UUI_Card::GetCardHead()
{
	return this->M_Card_Select_Head;
}

bool UUI_Card::GetCardSelectState()
{
	return this->bSelect;
}

bool UUI_Card::GetMouseEnterState()
{
	return this->bMouseEnter;
}

void UUI_Card::SelectColor()
{
	//UWidgetBase::SetImageColor(this->GetCardButton()->WidgetStyle.Normal., FVector(0.2f, 0.2f, 0.2f), 1.f);

//	this->GetCardButton()->SetBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.f));

	this->M_Card_Select_Head->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.f));
}

void UUI_Card::CancelColor()
{
	//this->GetCardButton()->SetBackgroundColor(FLinearColor(1.f, 1.f, 1.f, 1.f));

	//鼠标移出 才可以取消选择的颜色
	if (!this->bMouseEnter)
	{
		this->M_Card_Select_Head->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	}
}



