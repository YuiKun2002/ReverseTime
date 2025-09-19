// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/GameStartSubsystem.h"
#include <Kismet/GameplayStatics.h>
#include "GameSystem/FVMGameInstance.h"
#include "GameStart/VS/GameMapInstance.h"

TStatId UGameStartSubsystem::GetStatId() const
{
	return GetStatID();
}

void UGameStartSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	this->bGameStart = false;
	this->bGameOver = false;
}

void UGameStartSubsystem::Deinitialize() {
	Super::Deinitialize();

	//移除静态实例
	if (IsValid(UGameStartSubsystem::GameStartSubsystemStatic))
	{
		UGameStartSubsystem::GameStartSubsystemStatic->RemoveFromRoot();
		UGameStartSubsystem::GameStartSubsystemStatic = nullptr;
	}
}

void UGameStartSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->bGameStart && !this->bGameOver)
	{
		for (TScriptInterface<IGameStartInterface>& GameItem : this->Games)
		{
			GameItem->GameTick(DeltaTime);
		}
	}
}

void UGameStartSubsystem::InitGameStartSubsystem(TSoftClassPtr<AGameMapInstance> GameMapRes)
{
	if (IsValid(this->GameMapIns))
	{
		return;
	}

	// Blueprint'/Game/Resource/BP/GameStart/VS/A_VSMap.A_VSMap'

	//初始化
	this->GameMapIns = this->GetWorld()->SpawnActor<AGameMapInstance>(GameMapRes.LoadSynchronous());
	//初始化地图管理对象
	this->GameMapIns->GameMapInstanceInit();
	//初始化资源
	for (TScriptInterface<IGameStartInterface>& GameItem : this->Games)
	{
		GameItem->GameStartInit(this->GameMapIns);
	}
}

UGameStartSubsystem* UGameStartSubsystem::GameStartSubsystemStatic = nullptr;

UGameStartSubsystem* UGameStartSubsystem::GetGameStartSubsystemStatic()
{
	if (UGameStartSubsystem::GameStartSubsystemStatic)
	{
		return UGameStartSubsystem::GameStartSubsystemStatic;
	}

	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		if (IsValid(UFVMGameInstance::GetFVMGameInstance()->GetWorld()))
		{
			UGameStartSubsystem::GameStartSubsystemStatic =
				UFVMGameInstance::GetFVMGameInstance()->
				GetWorld()->GetSubsystem<UGameStartSubsystem>();

			//防止意外发生
			UGameStartSubsystem::GameStartSubsystemStatic->AddToRoot();

			return UGameStartSubsystem::GameStartSubsystemStatic;
		}
	}

	return nullptr;
}

void UGameStartSubsystem::AddGameStartObject(TScriptInterface<IGameStartInterface> Interface)
{
	UGameStartSubsystem* Sub = UGameStartSubsystem::GetGameStartSubsystemStatic();
	if (IsValid(Sub))
	{
		Sub->Games.Emplace(Interface);
	}
}

void UGameStartSubsystem::GameStart()
{
	this->bGameStart = true;
	this->bGameOver = false;
	for (TScriptInterface<IGameStartInterface>& GameItem : this->Games)
	{
		GameItem->GameStart(this->GameMapIns);
	}

	this->GameMapIns->GameStart();
	this->OnGameStartSubsystemGameStartDelegate.Broadcast();
}

void UGameStartSubsystem::GameOver(bool bGameWin)
{
	this->bGameStart = true;
	this->bGameOver = true;

	for (TScriptInterface<IGameStartInterface>& GameItem : this->Games)
	{
		GameItem->GameOverInit(this->GameMapIns, bGameWin);
	}

	for (TScriptInterface<IGameStartInterface>& GameItem : this->Games)
	{
		GameItem->GameOver(this->GameMapIns, bGameWin);
	}

	for (TScriptInterface<IGameStartInterface>& GameItem : this->Games)
	{
		GameItem->GameResourceUnload();
	}

	this->GameMapIns->GameOver(bGameWin);
	this->OnGameStartSubsystemGameOverDelegate.Broadcast(bGameWin);

	//移出本子系统的静态保护
	if (IsValid(UGameStartSubsystem::GameStartSubsystemStatic))
	{
		UGameStartSubsystem::GameStartSubsystemStatic->RemoveFromRoot();
	}
}

void UGameStartSubsystem::QuitGame()
{

	FInputModeGameAndUI GameUI;
	GameUI.SetHideCursorDuringCapture(false);
	GameUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	UGameplayStatics::GetPlayerController(this->GetWorld(), 0)->SetInputMode(GameUI);

	this->bGameStart = true;
	this->bGameOver = true;
	this->OnGameStartSubsystemGameOverDelegate.Broadcast(false);

	//移出本子系统的静态保护
	if (IsValid(UGameStartSubsystem::GameStartSubsystemStatic))
	{
		UGameStartSubsystem::GameStartSubsystemStatic->RemoveFromRoot();
	}
}

bool UGameStartSubsystem::GetbGameStart()
{
	UGameStartSubsystem* Sub = UGameStartSubsystem::GetGameStartSubsystemStatic();
	if (IsValid(Sub))
	{
		if (Sub->bGameStart && !Sub->bGameOver)
		{
			return true;
		}
	}

	return false;
}

bool UGameStartSubsystem::GetbGameOver()
{
	UGameStartSubsystem* Sub = UGameStartSubsystem::GetGameStartSubsystemStatic();
	if (IsValid(Sub))
	{
		if (Sub->bGameStart && Sub->bGameOver)
		{
			return true;
		}
	}

	return false;
}

bool& UGameStartSubsystem::GetbGameStartState()
{
	return this->bGameStart;
}

bool& UGameStartSubsystem::GetbGameOverState()
{
	return this->bGameOver;
}

AGameMapInstance* UGameStartSubsystem::GetGameMapInstance()
{
	return this->GameMapIns;
}
