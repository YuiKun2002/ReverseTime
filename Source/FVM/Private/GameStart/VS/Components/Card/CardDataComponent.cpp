// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStart/VS/Components/Card/CardDataComponent.h"

FItemCardATK UCardDataObject::GetCardATKData(int32 ID)
{ 
	return this->GetCardDataByID<FItemCardATK>(ID);
}

FItemCardSpawn UCardDataObject::GetCardSpawnData(int32 ID)
{
	return this->GetCardDataByID<FItemCardSpawn>(ID);
}

FItemCardDefence UCardDataObject::GetCardDefenceData(int32 ID)
{
	return  this->GetCardDataByID<FItemCardDefence>(ID);
}

FItemCardFunction UCardDataObject::GetCardFunction(int32 ID)
{
	return  this->GetCardDataByID<FItemCardFunction>(ID);
}

FItemCardRangeATK UCardDataObject::GetCardRangeATKData(int32 ID)
{
	return this->GetCardDataByID<FItemCardRangeATK>(ID);
}
