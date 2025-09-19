// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStart/VS/GameMapInstance.h"
#include "GameSystem/GameStartSubsystem.h"
#include "GameSystem/GameMapStructManager.h"
#include "GameStart/VS/GameMapFogManager.h"
#include "Data/GameLogSubsystem.h"
#include "GameStart/VS/Components/MesheControllComponent.h"
#include "GameStart/VS/Components/VSManagerComponent.h"
#include "GameStart/VS/Components/CardManagerComponent.h"
#include "GameStart/VS/Components/MouseManagerComponent.h"
#include "GameStart/VS/Components/ResourceManagerComponent.h"
#include "GameStart/VS/PlayerCameraPosition.h"
#include "GameStart/VS/MapMeshe.h"
#include "GameStart/VS/PlayerView.h"
#include "GameStart/Flipbook/GameActor/GamePlayer.h"
#include <Paper2D/Classes/PaperSprite.h>
#include <Paper2D/Classes/PaperSpriteActor.h>
#include <Paper2D/Classes/PaperSpriteComponent.h>
#include "GameSystem/FVMGameInstance.h"
#include "UI/WidgetBase.h"
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundBase.h>

UGameMapStructManager* AGameMapInstance::GetGameMapStructManagerStatic()
{
	if (AGameMapInstance::GetGameMapInstance())
	{
		return AGameMapInstance::GetGameMapInstance()->M_UGameMapStructManager;
	}

	return nullptr;
}

UCardManagerComponent* AGameMapInstance::GetGameMapCardManagerStatic()
{
	if (AGameMapInstance::GetGameMapInstance())
	{
		return AGameMapInstance::GetGameMapInstance()->M_CardManagerComponent;
	}
	return nullptr;
}

UVSManagerComponent* AGameMapInstance::GetGameMapVsManagerStatic()
{
	if (AGameMapInstance::GetGameMapInstance())
	{
		return AGameMapInstance::GetGameMapInstance()->M_VSManagerComponent;
	}
	return nullptr;
}

UMesheControllComponent* AGameMapInstance::GetGameMapMesheControllManagerStatic()
{
	if (AGameMapInstance::GetGameMapInstance())
	{
		return AGameMapInstance::GetGameMapInstance()->M_MesheControllComponent;
	}
	return nullptr;
}

UMouseManagerComponent* AGameMapInstance::GetGameMapMouseManagerStatic()
{
	if (AGameMapInstance::GetGameMapInstance())
	{
		return AGameMapInstance::GetGameMapInstance()->M_MouseManagerComponent;
	}
	return nullptr;
}

UResourceManagerComponent* AGameMapInstance::GetGameMapResourceManagerStatic()
{
	if (AGameMapInstance::GetGameMapInstance())
	{
		return AGameMapInstance::GetGameMapInstance()->M_ResourceManagerComponent;
	}
	return nullptr;
}

UGameMapFogManager* AGameMapInstance::GetGameMapFogManagerStatic()
{
	if (AGameMapInstance::GetGameMapInstance())
	{
		return AGameMapInstance::GetGameMapInstance()->FogManager;
	}
	return nullptr;
}

AGameMapInstance* AGameMapInstance::GetGameMapInstance()
{
	if (IsValid(UGameStartSubsystem::GetGameStartSubsystemStatic()))
	{
		return UGameStartSubsystem::GetGameStartSubsystemStatic()->GetGameMapInstance();
	}
	return nullptr;
}

UMesheControllComponent* AGameMapInstance::GetMesheControllComponent()
{
	return this->M_MesheControllComponent;
}

UGameMapStructManager* AGameMapInstance::GetGameMapStructManager()
{
	return this->M_UGameMapStructManager;
}



void AGameMapInstance::PlaySound(const FSoftObjectPath& _MusicName)
{
	//如果正在播放，那么判断正在播放的音频是否一样如果一样则不改变
	if (_MusicName.ToString().Equals(this->M_CurrentPlaySoundName))
	{
		return;
	}

	UFVMGameInstance::PlayBGM_Static(_MusicName);

	this->M_CurrentPlaySoundName = _MusicName.ToString();
}

void AGameMapInstance::GameMapInstanceInit()
{
	//初始化 地图的配置结构 管理器
	this->M_UGameMapStructManager = UFVMGameInstance::GetFVMGameInstance()->GetGameMapStructManager();

	//初始化格子管理器
	this->M_MesheControllComponent = NewObject<UMesheControllComponent>(this);
	this->M_MesheControllComponent->RegisterComponent();
	UGameStartSubsystem::AddGameStartObject(this->M_MesheControllComponent);

	//初始化迷雾管理器
	this->FogManager = NewObject<UGameMapFogManager>();
	UGameStartSubsystem::AddGameStartObject(this->FogManager);

	//初始化资源管理器
	this->M_ResourceManagerComponent = NewObject<UResourceManagerComponent>(this);
	this->M_ResourceManagerComponent->RegisterComponent();
	UGameStartSubsystem::AddGameStartObject(this->M_ResourceManagerComponent);

	//初始化卡片管理器
	this->M_CardManagerComponent = NewObject<UCardManagerComponent>(this);
	this->M_CardManagerComponent->RegisterComponent();
	UGameStartSubsystem::AddGameStartObject(this->M_CardManagerComponent);

	//初始化老鼠管理器
	this->M_MouseManagerComponent = NewObject<UMouseManagerComponent>(this);
	this->M_MouseManagerComponent->RegisterComponent();
	UGameStartSubsystem::AddGameStartObject(this->M_MouseManagerComponent);

	//初始化游戏开始管理器
	this->M_VSManagerComponent = NewObject<UVSManagerComponent>(this);
	this->M_VSManagerComponent->RegisterComponent();
	UGameStartSubsystem::AddGameStartObject(this->M_VSManagerComponent);

	//音乐初始化
	this->PlaySound(this->M_UGameMapStructManager->LevelConfig.LevelDefMusic);

	//初始化背景
	this->InitGameMapBackground();
}

void AGameMapInstance::GameStart()
{
#if PLATFORM_WINDOWS
	UGameLogSubsystem::GetGameLogSubsystemStatic()->ExeInstructions(TEXT("/clear"));
	UGameLogSubsystem::GetGameLogSubsystemStatic()->AddCurrent(RICHTEXT(TEXT("【1键】可以连续铲除卡片")));
	UGameLogSubsystem::GetGameLogSubsystemStatic()->AddCurrent(RICHTEXT(TEXT("【空格键】可以暂停游戏，暂停的同时可以使用指令！")));
	UGameLogSubsystem::GetGameLogSubsystemStatic()->AddCurrent(RICHTEXT(TEXT("【F11键】可以退出或者进入全屏模式")));
#endif
}

void AGameMapInstance::GameOver(bool bGameWin)
{
	this->OnGameOverDelegate.Broadcast(bGameWin);

	this->OnGameOverDelegate.Clear();
	this->OnPlayPlayerDelegate.Clear();
	this->OnSpawnMouseDelegate.Clear();
	this->OnSpawnMouseBossDelegate.Clear();
	this->OnUpdateRoundDelegate.Clear();
	this->OnUpdateRoundLastDelegate.Clear();

	FInputModeGameAndUI GameUI;
	GameUI.SetHideCursorDuringCapture(false);
	GameUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	UGameplayStatics::GetPlayerController(this->GetWorld(), 0)->SetInputMode(GameUI);
}