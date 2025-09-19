// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStart/VS/Components/Item/ShootCruveComponent.h"
#include "GameStart/Flipbook/GameActor/FlyItemActor.h"
#include <Kismet/KismetMathLibrary.h>

// Called when the game starts
void UShootCruveComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	this->M_Owner = Cast<AFlyItemActor>(this->GetOwner());
	if (!Cast<AFlyItemActor>(this->M_Owner))
	{
		UE_LOG(LogTemp, Error, TEXT("[ShootCurveComponent.cpp UShootCruveComponent::BeginPlay] 当前FlyActor无效"));
		this->SetComponentTickEnabled(false);
	}

}

void UShootCruveComponent::InitTarget()
{
	//因为Actor组件BeginPlay() 先于 Actor 本身
	this->M_CurrentLocation = this->M_Owner->GetActorLocation();
	this->Target = Cast<AFlyItemActor>(this->M_Owner)->GetObjectActorLocation();
}

FVector UShootCruveComponent::GetCurveLine(float FlyTime, float FlyTime_MaxHight)
{
	FVector A = this->M_CurrentLocation;
	A.Z = FlyTime_MaxHight * this->MaxHight + this->M_CurrentLocation.Z;

	FVector B = this->Target;
	B.Z = FlyTime_MaxHight * this->MaxHight + this->Target.Z;

	return UKismetMathLibrary::VLerp(A, B, FlyTime);
}

