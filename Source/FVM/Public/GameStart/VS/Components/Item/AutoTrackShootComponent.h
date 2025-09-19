// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStart/VS/Components/GameStartActorComponent.h"
#include "AutoTrackShootComponent.generated.h"

class UMouseManagerComponent;
class AFlyItemActor;
class AMouseActor;

//飞行物自动追踪功能
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FVM_API UAutoTrackShootComponent : public UGameStartActorComponent
{
	GENERATED_BODY()


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	virtual void GameTick(float DeltaTime) override;

	//停止移动
	UFUNCTION(BlueprintCallable)
	void Stop();
	UFUNCTION(BlueprintCallable)
	void Start();
	//初始化
	UFUNCTION(BlueprintCallable)
	void InitLocation();
	UFUNCTION(BlueprintCallable)
	bool GetTarget();
private:
	UPROPERTY()
	UMouseManagerComponent* M_MouseManager = nullptr;
	UPROPERTY()
	AFlyItemActor* M_Owner = nullptr;
	//更新时间
	UPROPERTY()
	float _TickTime = 0.f;
	//追踪时间
	UPROPERTY()
	float _AutoTrackTickTime = 0.f;
	//当前锁定的老鼠
	UPROPERTY()
	AMouseActor* M_CurrentMouse = nullptr;
	//当前坐标
	UPROPERTY()
	FVector M_OwnerLocation = FVector::ZeroVector;
	//是否停止
	UPROPERTY()
	bool bStop = false;
	//是否有目标
	UPROPERTY()
	bool bTarget = false;
};
