// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Engine/GameInstance.h"
#include "FVMGameInstance.generated.h"

// #include "GameSystem/FVMGameInstance.h"

//获取游戏实例
#define GAMEINS() UFVMGameInstance::GetFVMGameInstance()



//DECLARE_DYNAMIC_DELEGATE(FTimerDynamicDelegate);

//FVM游戏实例
UCLASS()
class FVM_API UFVMGameInstance : public UGameInstance
{
	GENERATED_BODY()

	friend class UMultiplayerOnlineObject;

public:

	// base
	virtual void Init() override;
	virtual void Shutdown() override;

public:


	//获取当前角色的登陆名称
	UFUNCTION(BlueprintPure)
	FORCEINLINE FString GetPlayerLoginName() const { return this->LoginPlayerName; };

	//是否为服务器
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsServer() const { return this->bCreateSession; };

	//是否为多人游戏
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsMultiplayer() const { return this->bMultiplayer; };

	//获取FVM2游戏实例
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static class UFVMGameInstance* GetFVMGameInstance(const UObject* WorldContextObject = nullptr);

	//获取游戏配置
	UFUNCTION(BlueprintPure)
	class UGameConfigManager* GetGameConfigManager();
	//获取游戏地图配置管理
	UFUNCTION(BlueprintPure)
	class UGameMapStructManager* GetGameMapStructManager();
	//获取角色结构管理
	UFUNCTION(BlueprintPure)
	class UPlayerStructManager* GetPlayerStructManager();
	//获取老鼠结构管理
	UFUNCTION(BlueprintPure)
	class UMouseStructManager* GetMouseStructManager();

	//获取音效音量
	UFUNCTION(BlueprintPure)
	static float GetBGMS_Volume();
	//获取游戏debug测试
	UFUNCTION(BlueprintPure)
	static bool GetDebug();
	//获取角色存档
	UFUNCTION(BlueprintPure)
	static class UPlayerStructManager* GetPlayerStructManager_Static();
	//获取配置文件
	UFUNCTION(BlueprintPure)
	static class UGameConfigManager* GetGameConfigManager_Static();
	//获取游戏版本
	UFUNCTION(BlueprintPure)
	static FString GetGameVersion();
	//获取更新
	UFUNCTION(BlueprintPure)
	static bool GetUpdateGame();

	//--------------------------------------------------[角色配置]-------------------------------------------------------
	//获取游戏特效是否开启
	UFUNCTION(BlueprintPure)
	static bool GetGameConfig_GameEFEnabled();
	//--------------------------------------------------[角色配置]-------------------------------------------------------

	//获取当前播放的背景音乐
	UFUNCTION(BlueprintPure)
	static FSoftObjectPath GetPlayBGMName();

public:

	//设置更新
	UFUNCTION()
	static void SetUpdateGame();
	//设置游戏debug测试
	UFUNCTION(BlueprintCallable)
	static void SetGameDebug(bool _Value);
	//设置游戏版本
	UFUNCTION(BlueprintCallable)
	static void SetGameVersion(FString _Version);
	//设置背景音乐缩放
	UFUNCTION(BlueprintCallable)
	static void SetBGMScale(float Value);

public:

	//加载一个游戏地图管理器（可以选择是否从数据表中加载数据,会先判断本地是否存在,如果选择true,会从数据表检索，如果成功则会自动创建）
	UFUNCTION(BlueprintCallable)
	class UGameMapStructManager* LoadGameMapStructManager(const FString& _FileName);
	//加载角色存档
	UFUNCTION(BlueprintCallable)
	class UPlayerStructManager* LoadPlayerStructManager(const FString& PlayerName);

	//更新背景音乐的音量
	UFUNCTION(BlueprintCallable)
	void UpdateBGMVolume();
	//播放背景音乐
	UFUNCTION(BlueprintCallable)
	static void PlayBGM_Static(const FSoftObjectPath& _MusicName, bool _Loop = true);
	//播放音效
	UFUNCTION(BlueprintCallable)
	static void PlayBGM_S_Static(
		const FString& _MusicName,
		const FString& _MusicPath = FString(TEXT("GameBG"))
	);

private:

	//是否为服务器
	UPROPERTY()
	bool bCreateSession = false;
	//是否是多人游戏
	UPROPERTY()
	bool bMultiplayer = false;
	//角色名称
	UPROPERTY()
	FString LoginPlayerName = FString();

	//设置Debug(true则显示测试文字)
	UPROPERTY()
	bool M_bShowDebugText = false;
	//游戏是否更新
	UPROPERTY(EditAnywhere)
	bool M_bGameUpdate = false;

	//游戏地图数据
	UPROPERTY()
	class UGameMapStructManager* M_UGameMapStructManager = nullptr;
};
