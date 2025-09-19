// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/GameConfigManager.h"

#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/Tools/GameSystemFunction.h"

void UGameConfigManager::SetCurrentLoginPlayerName(FString CurPlayerName)
{
	this->CurrentSelectPlayerDataName = CurPlayerName;
}

void UGameConfigManager::SetPlayerItemArrangeConfig(const TMap<FName, FGamePlayerItemArrangeConfig>& Maps) {
	this->ArrangeConfig = Maps;
}

FString UGameConfigManager::GetCurrentLoginPlayerName()
{
	return this->CurrentSelectPlayerDataName;
}

void UGameConfigManager::GetPlayerItemArrangeConfig(TMap<FName, FGamePlayerItemArrangeConfig>& OutMaps) {
	OutMaps = this->ArrangeConfig;
}

void UGameConfigManager::GetLoginPlayerNameList(TMap<FString, FPlayerLoginBaseData>& OutData)
{
	OutData = this->M_LoginPlayerNames;
}

void UGameConfigManager::AddPlayerLoginName(FString NewPlayerName, FPlayerLoginBaseData PlayerLoginData)
{
	this->M_LoginPlayerNames.Emplace(NewPlayerName, PlayerLoginData);
}

bool UGameConfigManager::RemovePlayerLoginName(FString NewPlayerName)
{
	if (this->M_LoginPlayerNames.Remove(NewPlayerName) != -1)
	{
		return true;
	}

	return false;
}

bool UGameConfigManager::CheckPlayerLoginNameIsValid(FString NewPlayerName)
{
	FPlayerLoginBaseData* LocalFindData = this->M_LoginPlayerNames.Find(NewPlayerName);
	if (LocalFindData)
	{
		return true;
	}

	return false;
}
