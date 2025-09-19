// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStart/VS/Components/GameStartActorComponent.h"
#include "ShootCruveComponent.generated.h"

class AFlyItemActor;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FVM_API UShootCruveComponent : public UGameStartActorComponent
{
	GENERATED_BODY()
private:
	//当前坐标
	FVector M_CurrentLocation = FVector(0.f);
	//目前坐标
	FVector Target = FVector(0.f);
public:
	//当前Actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AFlyItemActor* M_Owner = nullptr;
	//曲线最高高度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHight = 5.f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	//初始化目标
	UFUNCTION(BlueprintCallable)
	void InitTarget();

	//获取曲线路线
	UFUNCTION(BlueprintCallable)
	FVector GetCurveLine(float FlyTime, float FlyTime_MaxHight);
};
