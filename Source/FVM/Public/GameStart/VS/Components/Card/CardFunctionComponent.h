// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStart/VS/Components/GameStartActorComponent.h"
#include <Components/BoxComponent.h>
#include "GameStart/Flipbook/GameActor/FlyItemActor.h"
#include "CardFunctionComponent.generated.h"

class AFunctionCardActor;
class AFlyItemActor;
class UUI_MapMeshe;

//卡片功能
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FVM_API UCardFunctionComponent : public UGameStartActorComponent
{
	GENERATED_BODY()

public:
	virtual void GameTick(float DeltaTime) override;
public:
	//当动画播放完毕
	UFUNCTION()
	void OnAnimationPlayEnd();
	//获取上一次使用的卡片名称
	UFUNCTION()
	int32 GetLastCardID() const;
	//获取上一次使用的卡片的资产类
	UFUNCTION()
	UClass* GetLasrCardResourceClass() const;
	//获取对应的防御卡
	UFUNCTION()
	AFunctionCardActor* GetCardActor();
	//获取卡片对应的网格
	UFUNCTION()
	UUI_MapMeshe* GetCardMeshe();
	//事件触发器
	UFUNCTION()
	void EventTrigger(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
private:
	//卡片组件
	UPROPERTY()
	AFunctionCardActor* FunctionCardActor = nullptr;
	//格子组件(当卡片放下时->获取格子)
	UPROPERTY()
	UUI_MapMeshe* M_CardMapMeshe = nullptr;
private:
	//当前上一次选择的卡片每次
	UPROPERTY()
	int32 M_PlayerLastSelectCardActorID;
	UPROPERTY()
	UClass* M_PlayerLastSelectCardActorClass = nullptr;
};
