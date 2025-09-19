// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameStart/VS/MapBaseType.h"
#include "MapMesheInterface.generated.h"

//地图网格移动接口
UINTERFACE(MinimalAPI)
class UMapMesheMovementInterface : public UInterface
{
	GENERATED_BODY()
};

class FVM_API IMapMesheMovementInterface
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MapMesheMovement")
	void OnMapMesheMoveBegin(
		const int32& ActorLayer,
		const FLine& ActorLine
	);

	// 蓝图可调用的接口函数
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MapMesheMovement")
	void OnMapMesheMovedUpdate(
		const float& DeltaTime,
		const int32& ActorLayer,
		const FVector& ActorLocation,
		const FLine& ActorLine);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MapMesheMovement")
	void OnMapMesheMoveEnd(
		const int32& CardLayer,
		const FLine& CardLine
	);
};