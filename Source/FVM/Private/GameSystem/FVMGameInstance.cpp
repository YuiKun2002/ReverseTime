// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/FVMGameInstance.h" 
#include "GameSystem/MultiplayerOnlineObject.h"
#include "Data/GameLogSubsystem.h"
#include "Data/MapData/MapDataStruct.h"

#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>

#include "GameSystem/GameConfigSubsystem.h"
#include "GameSystem/PlayerDataSubsystem.h"

#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/GameMapStructManager.h"
#include "GameSystem/GameConfigManager.h"
#include "GameSystem/MouseStructManager.h"
#include "GameSystem/GameUserInterfaceSubsystem.h"
#include "GameSystem/PlayerDataHandle.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "GameFramework/GameUserSettings.h"

void UFVMGameInstance::Init() {

	Super::Init();

	this->bCreateSession = false;
	this->bMultiplayer = false;
	this->LoginPlayerName.Empty(0);
}

void UFVMGameInstance::Shutdown() {

	Super::Shutdown();

	if (IsValid(this->M_UGameMapStructManager))
	{
		this->M_UGameMapStructManager->MouseStructManagerObject = nullptr;
		this->M_UGameMapStructManager->RemoveFromRoot();
		this->M_UGameMapStructManager = nullptr;
	}

	this->bCreateSession = false;
	this->bMultiplayer = false;
	this->LoginPlayerName.Empty(0);
}

FString UFVMGameInstance::GetGameVersion()
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return UFVMGameInstance::GetFVMGameInstance()->GetGameConfigManager()->M_ConfigVersion;
	}

	return TEXT("");
}

void UFVMGameInstance::SetUpdateGame()
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		UFVMGameInstance::GetFVMGameInstance()->M_bGameUpdate = true;
	}
}

void UFVMGameInstance::SetGameDebug(bool _Value)
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		UFVMGameInstance::GetFVMGameInstance()->M_bShowDebugText = _Value;
	}
}

bool UFVMGameInstance::GetDebug()
{
#if WITH_EDITOR
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return UFVMGameInstance::GetFVMGameInstance()->M_bShowDebugText;
	}
#endif

#if UE_BUILD_DEVELOPMENT
	return true;
#endif

	return false;
}

void UFVMGameInstance::SetGameVersion(FString _Version)
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		UFVMGameInstance::GetFVMGameInstance()->GetGameConfigManager()->M_ConfigVersion = _Version;
	}
}

bool UFVMGameInstance::GetUpdateGame()
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return UFVMGameInstance::GetFVMGameInstance()->M_bGameUpdate;
	}

	return false;
}

UGameMapStructManager* UFVMGameInstance::LoadGameMapStructManager(const FString& _FileName)
{
	UDataTable* GameMapData = LoadObject<UDataTable>(0,
		TEXT("DataTable'/Game/Resource/BP/Data/MapData/DT_GameMapDataList.DT_GameMapDataList'"));

	if (IsValid(GameMapData))
	{
		FGameMapDataList* TempData = GameMapData->FindRow<FGameMapDataList>(
			FName(*_FileName), TEXT("TargetMapData")
		);
		if (TempData)
		{
			UMapDataStructAsset* Asset = TempData->MapDataTable.LoadSynchronous();
			if (!IsValid(Asset))
			{
				return nullptr;
			}

			if (IsValid(this->M_UGameMapStructManager))
			{
				this->M_UGameMapStructManager->MouseStructManagerObject = nullptr;
				this->M_UGameMapStructManager->RemoveFromRoot();
				this->M_UGameMapStructManager = nullptr;
			}

			//创建新的地图结构并且将数据表的数据赋予给新创建的地图结构
			this->M_UGameMapStructManager = NewObject<UGameMapStructManager>(this);
			this->M_UGameMapStructManager->AddToRoot();
			this->M_UGameMapStructManager->GameMapStruct = Asset->Data.M_FGameMapStruct;
			this->M_UGameMapStructManager->LevelConfig = Asset->Data.M_FLevelConfig;
			this->M_UGameMapStructManager->MouseStructManagerObject = NewObject<UMouseStructManager>(this);

			//设置老鼠配置
			this->M_UGameMapStructManager->MouseStructManagerObject->SetMouseConfig(
				Asset->Data.M_FMouseConfig
			);
			//设置行数量
			this->M_UGameMapStructManager->MouseStructManagerObject->SetRowCount(
				this->M_UGameMapStructManager->GameMapStruct.M_Meshe.Num()
			);

			//返回数据
			return this->M_UGameMapStructManager;
		}
	}
	return nullptr;
}

UPlayerStructManager* UFVMGameInstance::LoadPlayerStructManager(const FString& PlayerName) {

	if (IsValid(UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()))
	{
		ULocalPlayerDataHandle* LocalPlayerHD =
			UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->
			LoadLocalPlayerData(PlayerName, TEXT("UFVMGameInstance：加载角色存档！"));

		if (IsValid(LocalPlayerHD))
		{
			return Cast<UPlayerStructManager>(LocalPlayerHD->Execute_GetData(LocalPlayerHD));
		}
	}

	return nullptr;
}

UGameConfigManager* UFVMGameInstance::GetGameConfigManager()
{
	if (UGameConfigSubsystem::GetGameConfigSubsystemStatic())
	{
		return UGameConfigSubsystem::GetGameConfigSubsystemStatic()->GetGameConfigInstance();
	}

	return nullptr;
}

UGameMapStructManager* UFVMGameInstance::GetGameMapStructManager()
{
	return this->M_UGameMapStructManager;
}

UPlayerStructManager* UFVMGameInstance::GetPlayerStructManager()
{
	if (IsValid(UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()))
	{
		return UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->GetPlayerDataInstance();
	}

	return nullptr;
}

bool UFVMGameInstance::GetGameConfig_GameEFEnabled()
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return UFVMGameInstance::GetFVMGameInstance()->GetGameConfigManager()->M_GameEF.M_bEnable;
	}

	return false;
}

void UFVMGameInstance::UpdateBGMVolume()
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		UGameConfigSubsystem::GetGameConfigSubsystemStatic()->UpdateGameBGMVolume();
	}
}

void UFVMGameInstance::SetBGMScale(float Value)
{
	if (UFVMGameInstance::GetFVMGameInstance())
	{
		UGameConfigSubsystem::GetGameConfigSubsystemStatic()->SetGameBGMVolume(Value);
	}
}

UMouseStructManager* UFVMGameInstance::GetMouseStructManager()
{
	if (
		IsValid(this->M_UGameMapStructManager) &&
		IsValid(this->M_UGameMapStructManager->MouseStructManagerObject))
	{
		return this->M_UGameMapStructManager->MouseStructManagerObject;
	}
	return NewObject<UMouseStructManager>(this);
}

UPlayerStructManager* UFVMGameInstance::GetPlayerStructManager_Static()
{
	if (IsValid(UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()))
	{
		return UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->GetPlayerDataInstance();
	}
	return nullptr;
}

UGameConfigManager* UFVMGameInstance::GetGameConfigManager_Static()
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return UGameConfigSubsystem::GetGameConfigSubsystemStatic()->GetGameConfigInstance();
	}

	return nullptr;
}

UFVMGameInstance* UFVMGameInstance::GetFVMGameInstance(const UObject* WorldContextObject)
{
#if WITH_EDITOR
	if (IsValid(GEngine) && IsValid(GWorld))
	{
		if (IsValid(WorldContextObject))
		{
			return Cast<UFVMGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
		}
		else
		{
			FWorldContext* context = GEngine->GetWorldContextFromGameViewport(GWorld->GetGameViewport());
			if (context)
			{
				return Cast<UFVMGameInstance>(context->OwningGameInstance);
			}
		}
	}
#else 
	if (IsValid(GEngine))
	{
		if (IsValid(WorldContextObject))
		{
			return Cast<UFVMGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
		}
		else {
			FWorldContext* context = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
			if (context)
			{
				return Cast<UFVMGameInstance>(context->OwningGameInstance);
			}
		}
	}
#endif
	return nullptr;
}

void UFVMGameInstance::PlayBGM_Static(const FSoftObjectPath& _MusicName, bool _Loop)
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		UGameConfigSubsystem::GetGameConfigSubsystemStatic()->PlayGameBGM(_MusicName, _Loop);
	}
}

FSoftObjectPath UFVMGameInstance::GetPlayBGMName()
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return UGameConfigSubsystem::GetGameConfigSubsystemStatic()->GetCurrentPlayGameBgmPath();
	}

	return FSoftObjectPath();
}

void UFVMGameInstance::PlayBGM_S_Static(const FString& _MusicName, const FString& _MusicPath)
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		UGameConfigSubsystem::GetGameConfigSubsystemStatic()->PlayGameSoundEffects(_MusicName, _MusicPath);
	}
}

float UFVMGameInstance::GetBGMS_Volume()
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return UGameConfigSubsystem::GetGameConfigSubsystemStatic()->GetGameSoundEffectsVolume();
	}

	return 0.f;
}