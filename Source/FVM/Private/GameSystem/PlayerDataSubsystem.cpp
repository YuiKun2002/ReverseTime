// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/PlayerDataSubsystem.h"
#include <Kismet/GameplayStatics.h>
#include "GameSystem/PlayerDataHandle.h"
#include "GameSystem/PlayerStructManager.h"


bool IDataInterface::SaveData_Implementation(const FString& SaveLogMsg) { return false; }

bool IDataInterface::LoadData_Implementation(const FString& LoadLogMsg) { return false; }

bool IDataInterface::CreateData_Implementation() { return false; }

bool IDataInterface::RemoveData_Implementation() { return false; }

USaveGame* IDataInterface::GetData_Implementation() { return nullptr; }

const FString IDataInterface::GetTag_Implementation() const { return TEXT("PlayerDataInterface"); };


void ULocalDataHandle::CreateLocalName(const FString& Name) {
	this->LocalName = Name;
}

const FString ULocalDataHandle::GetLocalName() const {
	return FString(this->LocalName + TEXT("_") + this->Execute_GetTag(this));
}

const FString ULocalDataHandle::GetLocalNameBase() const {
	return this->LocalName;
}

bool ULocalDataHandle::LocalFileIsExist()
{
	return UGameplayStatics::DoesSaveGameExist(this->GetLocalName(), 0);
}

USaveGame* ULocalDataHandle::LoadLocalSaveGame(const FString& LogMsg)
{
	USaveGame* SaveIns = UGameplayStatics::LoadGameFromSlot(this->GetLocalName(), 0);
#if WITH_EDITOR
	if (IsValid(SaveIns))
	{
		if (UFVMGameInstance::GetDebug())
		{
			UGameSystemFunction::FVMLog(__FUNCTION__,
				FString(this->GetLocalName() +
					TEXT("[加载]{")) +
				LogMsg + TEXT("}"));
		}
	}
#endif
	return SaveIns;
}

void ULocalDataHandle::SaveLocalSaveGame(USaveGame* SaveGameObjectIns, const FString& LogMsg)
{
	if (IsValid(SaveGameObjectIns))
	{
		if (UGameplayStatics::SaveGameToSlot(SaveGameObjectIns, this->GetLocalName(), 0))
		{
#if WITH_EDITOR
			if (UFVMGameInstance::GetDebug())
			{
				UGameSystemFunction::FVMLog(__FUNCTION__,
					FString(this->GetLocalName() +
						TEXT("[存储]{")) +
					LogMsg + TEXT("}"));
			}
#endif
		}
	}
	else {
#if WITH_EDITOR
		if (UFVMGameInstance::GetDebug())
		{
			UGameSystemFunction::FVMLog(__FUNCTION__,
				FString(this->GetLocalName() +
					TEXT("[存储]{")) +
				LogMsg + TEXT("}，失败！，句柄无效！NULL"));
		}
#endif
	}

}




UPlayerDataSubsystem* UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return UFVMGameInstance::GetFVMGameInstance()->GetSubsystem<UPlayerDataSubsystem>();
	}

	return nullptr;
}

UPlayerStructManager* UPlayerDataSubsystem::CreateEmptyPlayerInstance()
{
	return Cast<UPlayerStructManager>(
		UGameplayStatics::CreateSaveGameObject(
			LoadClass<UPlayerStructManager>(0, TEXT("Class'/Script/FVM.PlayerStructManager'"))));
}

UPlayerStructManager* UPlayerDataSubsystem::GetPlayerDataInstance()
{
	return this->CurrentPlayerDataInstance;
}

void UPlayerDataSubsystem::SetPlayerDataInstance(UPlayerStructManager* NewPlayerIns)
{
	this->CurrentPlayerDataInstance = NewPlayerIns;
}

void UPlayerDataSubsystem::SetLocalPlayerDataHandle(ULocalPlayerDataHandle* NewPlayerHandle)
{
	this->CurrentLocalPlayerDataHandle = NewPlayerHandle;
}

ULocalPlayerDataHandle* UPlayerDataSubsystem::GetLocalPlayerDataHandle()
{
	return this->CurrentLocalPlayerDataHandle;
}

bool UPlayerDataSubsystem::SavePlayerData(UObject* Inter, const FString& SaveLogMsg)
{
	if (Cast<IDataInterface>(Inter))
	{
		return Cast<IDataInterface>(Inter)->Execute_SaveData(Inter, SaveLogMsg);
	}

	return false;
}

bool UPlayerDataSubsystem::LoadPlayerData(UObject* Inter, const FString& LoadLogMsg)
{
	if (Cast<IDataInterface>(Inter))
	{
		return Cast<IDataInterface>(Inter)->Execute_LoadData(Inter, LoadLogMsg);
	}

	return false;
}

bool UPlayerDataSubsystem::CreateNewLocalPlayerData(ULocalPlayerDataHandle* LocalPlayerHandle)
{
	if (Cast<IDataInterface>(LocalPlayerHandle))
	{
		if (Cast<IDataInterface>(LocalPlayerHandle)->Execute_CreateData(LocalPlayerHandle))
		{
			//构造本地角色存档
			this->CurrentLocalPlayerDataHandle = LocalPlayerHandle;

			return true;
		}
	}
	return false;
}

ULocalPlayerDataHandle* UPlayerDataSubsystem::LoadLocalPlayerData(FString LocalPlayerName, const FString& LoadLogMsg)
{
	ULocalPlayerDataHandle* NewHandle = ULocalPlayerDataHandle::MakeLocalPlayerDataHandle(LocalPlayerName, 0);

	if (UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->LoadPlayerData(NewHandle,
		TEXT("本地加载UPlayerDataSubsystem::LoadLocalPlayerData：") + LoadLogMsg))
	{
		return NewHandle;
	}

	return nullptr;
}

bool UPlayerDataSubsystem::SaveLocalPlayerData(ULocalPlayerDataHandle* LocalPlayerHandle, const FString& SaveLogMsg)
{
	if (IsValid(LocalPlayerHandle))
	{
		return UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->SavePlayerData(LocalPlayerHandle,
			TEXT("本地保存UPlayerDataSubsystem::SaveLocalPlayerData：") + SaveLogMsg);
	}

	return false;
}

bool UPlayerDataSubsystem::RemoveLocalPlayerData(ULocalPlayerDataHandle* LocalPlayerHandle)
{
	if (Cast<IDataInterface>(LocalPlayerHandle))
	{
		return Cast<IDataInterface>(LocalPlayerHandle)->Execute_RemoveData(LocalPlayerHandle);
	}

	return false;
}