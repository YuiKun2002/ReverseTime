// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Components/ActorComponent.h"
#include "GameSystem/GameStartSubsystem.h"
#include "GameSystem/Item/PlayerStructBase.h"
#include "VSManagerComponent.generated.h"

class UUI_GameOver;
class UUI_VSCountDown;
class UUI_BottomTip;

//关卡VS管理器
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FVM_API UVSManagerComponent : public UActorComponent, public IGameStartInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UVSManagerComponent();

	//游戏开始
	virtual void GameStartInit(class AGameMapInstance* GameMapInstance) override;
	virtual void GameStart(class AGameMapInstance* GameMapInstance) override;

	//更新
	virtual void GameTick(float DeltaTime) override;

	//游戏结束【可以关联其他管理器，或者访问其他对象，访问】
	virtual void GameOverInit(class AGameMapInstance* GameMapInstance, bool bGameWin) override;
	virtual void GameOver(class AGameMapInstance* GameMapInstance, bool bGameWin) override;

	//资源释放【不能访问其他对象，收尾】
	virtual void GameResourceUnload() override;
public:

	//创建一个角色
	UFUNCTION(BlueprintCallable)
	static bool CreatePlayer(
		bool bMainPlayer,
		FString PlayerName,
		int32 Row,
		int32 Col,
		int32 PlayerGrade,
		FSoftObjectPath PlayerHead,
		FPlayerSuitItem NewSuit,
		FString FirstWeaponName
	);

	//通过数据创建一个角色
	UFUNCTION(BlueprintCallable)
	static bool CreatePlayerByData(
		FString PlayerName,
		int32 Row,
		int32 Col,
		int32 PlayerGrade,
		TSoftObjectPtr<UDataTable> PlayerDataPtr
	);

	//生成角色[并且游戏开始]
	UFUNCTION(BlueprintCallable)
	bool SpawnMainPlayer(
		FLine PlayerLine
	);

public:
	//获取角色对象
	UFUNCTION(BlueprintPure)
	AGamePlayer* GetPlayer(FString PlayerName);
	//获取主角
	UFUNCTION(BlueprintPure)
	AGamePlayer* GetMainPlayer();
	//获取主角名称
	UFUNCTION(BlueprintPure)
	FString GetMainPlayerName();
	//获取游戏结束状态
	UFUNCTION(BlueprintPure)
	bool GetbGameWin() const;
public:
	//注册角色
	UFUNCTION(BlueprintCallable)
	void RegistPlayer(FString PlayerName, AGamePlayer* NewPlayerIns);
	//显示文字到底部
	UFUNCTION(BlueprintCallable)
	void ShowText(const FString& _Title);
	//使用钥匙
	UFUNCTION(BlueprintCallable)
	void UseLevelKey();
public:
	//直接游戏胜利，结算
	UFUNCTION(BlueprintCallable)
	static void GameWin();
	//直接游戏失败，结算
	UFUNCTION(BlueprintCallable)
	static void GameFailed();
	//游戏暂停
	UFUNCTION(BlueprintCallable)
	static void GamePause();
	//游戏继续
	UFUNCTION(BlueprintCallable)
	static void GameContinue();
private:
	//游戏结束UI
	UPROPERTY()
	UUI_GameOver* M_UI_GameOver = nullptr;
	//游戏底部提示
	UPROPERTY()
	UUI_BottomTip* M_UIBottomTip = nullptr;
	//游戏倒计时
	UPROPERTY()
	UUI_VSCountDown* UI_VSCountDown = nullptr;
	//玩家实体对象
	UPROPERTY()
	TMap<FString, AGamePlayer*> M_PlayerInstance;
	//提示玩家放置角色的提示界面界面
	UPROPERTY()
	class UWidgetBase* M_UI_PlayPlayerTipsWidget = nullptr;
private:
	//是否是胜利
	UPROPERTY()
	bool M_Win = true;
	//是否使用过钥匙或者徽章
	UPROPERTY()
	bool M_bUseKey = false;
};
