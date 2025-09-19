// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameStart/VS/MapBaseType.h"
#include "MapBase.generated.h"

class APaperSpriteActor;

UCLASS()
class FVM_API AMapBase : public AActor
{
	GENERATED_BODY()
protected:
	//地图位置
	UPROPERTY(EditAnywhere, Category = "MapBaseStructData")
	FMapTranform M_MapTranform;

	//地图需要的精灵资源
	UPROPERTY(EditAnywhere, Category = "MapBaseStructData")
	FMapSprite M_MapSprite;

private:
	//初始化精灵资源
	void InitMapSprite();

public:
	// Sets default values for this actor's properties
	AMapBase();

	//创建一个2D的Actor到世界
	UFUNCTION(BlueprintCallable)
	void CreatePaperSpriteActorToWorld(
		APaperSpriteActor* _Actor,
		UPaperSprite* _PSprite,
		const FTransform& _ActorTranform
	);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//初始化地图的背景
	void InitGameMapBackground();
private:
	//地图背景
	UPROPERTY()
	APaperSpriteActor* M_AMapBackground = nullptr;
};
