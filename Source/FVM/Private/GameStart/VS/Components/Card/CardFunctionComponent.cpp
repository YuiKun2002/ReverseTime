// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStart/VS/Components/Card/CardFunctionComponent.h"
#include "GameStart/Flipbook/GameActor/Card/FunctionCardActor.h"
#include "GameStart/VS/Components/Card/CardFuncCompImplement.h"
#include "GameStart/VS/Components/CardManagerComponent.h"
#include "GameSystem/GameStartSubsystem.h"
#include "GameStart/VS/MapBaseType.h"
#include "GameStart/VS/GameMapInstance.h"

#include "GameSystem/Tools/GameSystemFunction.h"

#include  <Kismet/GameplayStatics.h>

// Called when the game starts
void UCardFunctionComponent::BeginPlay()
{
	Super::BeginPlay();

	//获取卡片拥有者
	this->FunctionCardActor = Cast<AFunctionCardActor>(this->GetOwner());

	if (!this->FunctionCardActor)
	{
		this->SetComponentTickEnabled(false);
		return;
	}

	//设置功能组件
	this->FunctionCardActor->SetFunctionComponent(this);

	//获取上一次使用的卡片
	this->M_PlayerLastSelectCardActorClass = AGameMapInstance::GetGameMapCardManagerStatic()
		->GetLastSelectCardClass();

	this->M_PlayerLastSelectCardActorID = AGameMapInstance::GetGameMapCardManagerStatic()
		->GetLastSelectCardID();

	//获取卡片格子
	this->M_CardMapMeshe = this->FunctionCardActor->GetUIMapMesh();

	//播放默认动画
	if (UGameSystemFunction::LoadRes(this->FunctionCardActor->CardActor_DefAniml))
	{
		this->FunctionCardActor->SetPlayAnimation(UGameSystemFunction::LoadRes(this->FunctionCardActor->CardActor_DefAniml));
	}

	//绑定动画播放完毕函数
	this->FunctionCardActor->GetRenderComponent()->OnAnimationPlayEnd.BindUObject(
		this, &UCardFunctionComponent::OnAnimationPlayEnd
	);

	//运行功能函数
	this->FunctionCardActor->ExecuteCardFuncClassByCardFunction(this);
}


void UCardFunctionComponent::OnAnimationPlayEnd()
{
	//卡片休息
	if (!this->FunctionCardActor->GetCardDay() && this->FunctionCardActor->GetMapDay())
	{
		return;
	}

	//运行功能函数
	this->FunctionCardActor->ExecuteCardFuncClassByCardFunctionOnAnimPlayEnd(this);
}

int32 UCardFunctionComponent::GetLastCardID() const
{
	return this->M_PlayerLastSelectCardActorID;
}

UClass* UCardFunctionComponent::GetLasrCardResourceClass() const
{
	return this->M_PlayerLastSelectCardActorClass;
}

AFunctionCardActor* UCardFunctionComponent::GetCardActor()
{
	return this->FunctionCardActor;
}

UUI_MapMeshe* UCardFunctionComponent::GetCardMeshe()
{
	return this->M_CardMapMeshe;
}

void UCardFunctionComponent::EventTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//卡片休息
	if (!this->FunctionCardActor->GetCardDay() && this->FunctionCardActor->GetMapDay())
	{
		return;
	}

	if (IsValid(OtherActor))
	{

		//运行功能函数
		this->FunctionCardActor->ExecuteCardFuncClassByCardFunctionOnOverlapBegin(this, OtherActor);
	}
}

void UCardFunctionComponent::GameTick(float DeltaTime)
{
	//卡片休息
	if (!this->FunctionCardActor->GetCardDay() && this->FunctionCardActor->GetMapDay())
	{
		return;
	}

	//运行功能函数
	this->FunctionCardActor->ExecuteCardFuncClassByCardFunctionUpdate(this, DeltaTime);
}
