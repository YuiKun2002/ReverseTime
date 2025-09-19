// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStart/VS/MapBase.h"
#include <Paper2D/Classes/PaperSpriteActor.h>
#include "PaperSpriteComponent.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/GameMapStructManager.h"
#include <Paper2D/Classes/PaperSprite.h>

// Sets default values
AMapBase::AMapBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMapBase::BeginPlay()
{
	Super::BeginPlay();

	//UE_LOG(LogTemp, Warning, TEXT("AMapBase"));

	//设置当前地图Actor的位置
	this->SetActorLocation(this->M_MapTranform.M_Location);
	this->SetActorRotation(this->M_MapTranform.M_Rotation);
	this->SetActorScale3D(this->M_MapTranform.M_Scale);

	this->InitMapSprite();
}

void AMapBase::CreatePaperSpriteActorToWorld(APaperSpriteActor* _Actor, UPaperSprite* _PSprite, const FTransform& _ActorTranform)
{
	if (!_Actor)
		return;

	APaperSpriteActor* LAMapBackground = this->GetWorld()->SpawnActor<APaperSpriteActor>(_Actor->GetClass(), _ActorTranform);
	LAMapBackground->GetRootComponent()->SetMobility(EComponentMobility::Movable);
	LAMapBackground->GetRenderComponent()->SetSprite(_PSprite);
}

void AMapBase::InitMapSprite()
{
	if (!IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return;
	}

	//通过存档获取地图的背景
	this->M_MapSprite.M_MapBackgroundSprite = LoadObject<UPaperSprite>(0,
		*UFVMGameInstance::GetFVMGameInstance()->GetGameMapStructManager()->LevelConfig.LevelBGName.ToString()
	);
}

void AMapBase::InitGameMapBackground()
{
	//创建背景
	this->M_AMapBackground = NewObject<APaperSpriteActor>(this->GetWorld(), "M_AMapBackground");

	this->CreatePaperSpriteActorToWorld(this->M_AMapBackground, this->M_MapSprite.M_MapBackgroundSprite,
		this->GetActorTransform());
}

