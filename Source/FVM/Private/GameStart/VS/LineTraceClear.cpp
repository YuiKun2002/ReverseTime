// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStart/VS/LineTraceClear.h"

// Called when the game starts or when spawned
void ALineTraceClear::BeginPlay()
{
	Super::BeginPlay();

	switch (this->M_EDirection)
	{
	case EDirection::_EUp:
		this->_M_Direction = FVector(this->GetActorLocation().X, this->GetActorLocation().Y, this->GetActorLocation().Z + this->M_End);
		break;
	case EDirection::_EDown:
		this->_M_Direction = FVector(this->GetActorLocation().X, this->GetActorLocation().Y, this->GetActorLocation().Z - this->M_End);
		break;
	case EDirection::_ELeft:
		this->_M_Direction = FVector(this->GetActorLocation().X, this->GetActorLocation().Y - this->M_End, this->GetActorLocation().Z);
		break;
	case EDirection::_ERight:
		this->_M_Direction = FVector(this->GetActorLocation().X, this->GetActorLocation().Y + this->M_End, this->GetActorLocation().Z);
		break;
	}
}

