// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStart/Flipbook/GameStartActorBase.h"
#include "GameStart/VS/MapBaseType.h"
#include "LineTraceClear.generated.h"

UCLASS()
class FVM_API ALineTraceClear : public AGameStartActorBase
{
	GENERATED_BODY()

public:
	//射线结束位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float M_End;

	//射线最后的位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector _M_Direction;

	//方向
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EDirection M_EDirection;

	//清除飞行物
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool ClearItem = false;

	//清除老鼠
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool ClearMouse = false;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
