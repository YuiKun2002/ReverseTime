// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameStart/Flipbook/GameStartActorBase.h"

#include "GameSystem/GameMapStructManager.h"
#include "GameStart/VS/MapMeshe.h"

#include "MapLauncherBase.generated.h"

class UMesheControllComponent;

UCLASS()
class FVM_API AMapLauncherBase : public AGameStartActorBase
{
	GENERATED_BODY()
public:
	//设置关卡配置
	void SetLevelConfig(FLevelConfig _data);
	//设置地图配置
	void SetMapConfig(FGameMapStruct _data);
	//设置网格控制器
	void SetMapMesheControllComponent(UMesheControllComponent* _UMesheControllComponent);
public:
	//获取地图数据
	FLevelConfig& GetLevelConfig();
	FGameMapStruct& GetMapStructData();
	//获取地图网格
	AMapMeshe* GetMapMeshes(const FLine& _Line);
	//获取地图总行总列
	FLine GetMapMeshLine();
private:
	//地图数据                                
	UPROPERTY()
	FGameMapStruct GameMapStruct;
	//关卡数据                                
	UPROPERTY()
	FLevelConfig LevelConfig;
private:
	//网格控制器
	UPROPERTY()
	UMesheControllComponent* M_UMesheControllComponent = nullptr;
};
