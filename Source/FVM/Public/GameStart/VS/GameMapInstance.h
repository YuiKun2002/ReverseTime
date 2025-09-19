// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameStart/VS/MapBase.h"
#include "GameSystem/Item/Card/CardBaseStruct.h"
#include "GameSystem/GameDataSubsystem.h"
#include "GameSystem/GameStartSubsystem.h"
#include "GameMapInstance.generated.h"

class UGameMapFogManager;
class APaperSpriteActor;
class UPaperSpriteComponent;
class UGameMapStructManager;
class UMesheControllComponent;
class UVSManagerComponent;
class UCardManagerComponent;
class UMouseManagerComponent;
class UResourceManagerComponent;
class UAudioComponent;
class UWidgetBase;
class AGamePlayer;
class AMapMeshe;
class UUI_MapMeshe;

//关卡跟随角色预放置卡片
USTRUCT(BlueprintType)
struct FVM_API FGameMapLevelPlayPreCardStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	//卡片放置层级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CardLayer = 0;

	//卡片基础数据
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	struct FItemCard CardBaseData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<class ACardActor> CardResource;
};

//关卡数据缓存
UCLASS(BlueprintType, Blueprintable)
class FVM_API UGameMapDataAssetCache : public UGameDataAssetCache
{
	GENERATED_BODY()
public:
	
private:
	DataTableAssetData<FGameMapLevelPlayPreCardStruct> LevelPlayPreCardStructs;
};

//当游戏结束时触发的委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameOverDelegate, bool, bGameWin);
//当放置角色的时候委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayPlayerDelegate);
//生成老鼠时触发点委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpawnMouseDelegate, FString, MouseName, bool, bBoss);
//生成Boss时
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpawnMouseBossDelegate);
//当更新回合时
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateRoundDelegate, int32, Round);
//当最后一波时
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateRoundLastDelegate);


//游戏地图实例
UCLASS()
class FVM_API AGameMapInstance : public AMapBase
{
	GENERATED_BODY()

	friend class UGameStartSubsystem;
public:
	//当游戏结束时触发
	UPROPERTY(BlueprintAssignable)
	FOnGameOverDelegate OnGameOverDelegate;
	//当放置角色时触发
	UPROPERTY(BlueprintAssignable)
	FOnPlayPlayerDelegate OnPlayPlayerDelegate;
	//当生成老鼠时
	UPROPERTY(BlueprintAssignable)
	FOnSpawnMouseDelegate OnSpawnMouseDelegate;
	//生成Boss时
	UPROPERTY(BlueprintAssignable)
	FOnSpawnMouseBossDelegate OnSpawnMouseBossDelegate;
	//当更新回合时
	UPROPERTY(BlueprintAssignable)
	FOnUpdateRoundDelegate OnUpdateRoundDelegate;
	//当最后一波时
	UPROPERTY(BlueprintAssignable)
	FOnUpdateRoundLastDelegate OnUpdateRoundLastDelegate;
public:
	//地图的配置结构
	UFUNCTION(BlueprintPure)
	static UGameMapStructManager* GetGameMapStructManagerStatic();
	//获取卡片管理组件
	UFUNCTION(BlueprintPure)
	static UCardManagerComponent* GetGameMapCardManagerStatic();
	//获取VS管理组件
	UFUNCTION(BlueprintPure)
	static UVSManagerComponent* GetGameMapVsManagerStatic();
	//获取格子控制管理组件
	UFUNCTION(BlueprintPure)
	static UMesheControllComponent* GetGameMapMesheControllManagerStatic();
	//获取老鼠管理组件
	UFUNCTION(BlueprintPure)
	static UMouseManagerComponent* GetGameMapMouseManagerStatic();
	//获取资源管理组件
	UFUNCTION(BlueprintPure)
	static UResourceManagerComponent* GetGameMapResourceManagerStatic();
	//获取迷雾管理器
	UFUNCTION(BlueprintPure)
	static UGameMapFogManager* GetGameMapFogManagerStatic();
	//获取游戏地图实例
	UFUNCTION(BlueprintCallable)
	static AGameMapInstance* GetGameMapInstance();
	//获取网格控制组件
	UFUNCTION(BlueprintCallable)
	UMesheControllComponent* GetMesheControllComponent();
	//获取当前地图所使用的地图配置管理器
	UFUNCTION(BlueprintCallable)
	UGameMapStructManager* GetGameMapStructManager();
public:
	//播放背景音乐
	UFUNCTION(BlueprintCallable)
	void PlaySound(const FSoftObjectPath& _MusicName);
private:
	//游戏开始
	virtual	void GameStart();
	//游戏结束
	virtual void GameOver(bool bGameWin);
	//游戏初始化
	void GameMapInstanceInit();

private:
	//地图的配置结构
	UPROPERTY()
	UGameMapStructManager* M_UGameMapStructManager = nullptr;
	//卡片管理
	UPROPERTY()
	UCardManagerComponent* M_CardManagerComponent = nullptr;
	//格子控制组件
	UPROPERTY()
	UMesheControllComponent* M_MesheControllComponent = nullptr;
	//老鼠管理
	UPROPERTY()
	UMouseManagerComponent* M_MouseManagerComponent = nullptr;
	//游戏开始VS管理器
	UPROPERTY()
	UVSManagerComponent* M_VSManagerComponent = nullptr;
	//资源管理
	UPROPERTY()
	UResourceManagerComponent* M_ResourceManagerComponent = nullptr;
	//迷雾管理器
	UPROPERTY()
	class UGameMapFogManager* FogManager = nullptr;
private:
	//当前正在播放的音频名称
	UPROPERTY()
	FString M_CurrentPlaySoundName = FString(TEXT(""));
};
