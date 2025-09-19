// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStart/VS/MapBaseType.h"
#include "Components/ActorComponent.h"
#include "GameSystem/GameStartSubsystem.h"
#include "GameSystem/GameMapStructManager.h"
#include "MesheControllComponent.generated.h"

class UGameMapStructManager;
class AMapLauncherBase;
class AMapMouseMesheManager;
class AMapMeshe;

//网格控制管理器
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FVM_API UMesheControllComponent :public UActorComponent, public IGameStartInterface
{
	GENERATED_BODY()

public:

	UMesheControllComponent();

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
	//获取游戏地图配置
	UFUNCTION(BlueprintCallable)
	UGameMapStructManager* const GetGameMapStructManager();
	//获取网格
	UFUNCTION(BlueprintCallable, Category = "AMapMesh")
	AMapMeshe* GetMapMesh(int32 _Row, int32 _Col);
	//通过线路获取网格忽略被禁用的网格
	UFUNCTION(BlueprintCallable, Category = "AMapMesh")
	AMapMeshe* GetMapMeshByLine(int32 _Row, int32 _Col);
	//获取当前网格的最顶部网格
	UFUNCTION(BlueprintCallable, Category = "AMapMesh")
	AMapMeshe* GetMapMeshCurrentTop(int32 _Col);
	//获取当前网格的最底部网格
	UFUNCTION(BlueprintCallable, Category = "AMapMesh")
	AMapMeshe* GetMapMeshCurrentBottom(int32 _Col);
	//获取当前网格的最左边的网格
	UFUNCTION(BlueprintCallable, Category = "AMapMesh")
	AMapMeshe* GetMapMeshCurrentLeft(int32 Row);
	//获取当前网格的最右边的网格
	UFUNCTION(BlueprintCallable, Category = "AMapMesh")
	AMapMeshe* GetMapMeshCurrentRight(int32 Row);
	//获取当前行，含有卡片的网格
	UFUNCTION(BlueprintCallable, Category = "AMapMesh")
	AMapMeshe* GetHasCardMeshe(int32 Row);
	//获取老鼠网格管理器
	UFUNCTION(BlueprintCallable, Category = "MouseMeshManager")
	AMapMouseMesheManager* GetMapMouseMesh(int32 _Row, int32 _Col);
	//获取当前网格的最顶部网格
	UFUNCTION(BlueprintCallable, Category = "MouseMeshManager")
	AMapMouseMesheManager* GetMapMouseMeshCurrentTop(int32 _Col);
	//获取当前网格的最底部网格
	UFUNCTION(BlueprintCallable, Category = "MouseMeshManager")
	AMapMouseMesheManager* GetMapMouseMeshCurrentBottom(int32 _Col);
	//获取当前网格的最左边的网格
	UFUNCTION(BlueprintCallable, Category = "MouseMeshManager")
	AMapMouseMesheManager* GetMapMouseMeshCurrentLeft(int32 Row);
	//获取当前网格的最右边的网格
	UFUNCTION(BlueprintCallable, Category = "MouseMeshManager")
	AMapMouseMesheManager* GetMapMouseMeshCurrentRight(int32 Row);
	//获取网格
	UFUNCTION(BlueprintCallable)
	AMapMeshe* GetMesh(const int32& _Row, const int32& _Col);
	//获取网格的坐标
	UFUNCTION(BlueprintCallable)
	FVector GetMapMeshLocation(const int32& _Row, const int32& _Col);
	//通过索引获取网格最后一格的坐标
	UFUNCTION(BlueprintPure)
	FVector GetMapMesheLastLocationByIndex(int32 Row);
	//获取地图网格的总行和总列
	UFUNCTION(BlueprintCallable)
	FLine GetMapMeshRowAndCol();
	//获取网格数量
	UFUNCTION(BlueprintCallable)
	int32 GetMesheCount() const;
	//获取所有的网格
	UFUNCTION()
	TArray<AActor*> GetAllMapMeshes();
	//获取地图发射器
	UFUNCTION(BlueprintPure)
	class AMapLauncherBase* GetMapLauncherByIndex(int32 Index = 0);

public:
	//检测网格
	UFUNCTION(BlueprintCallable)
	bool CheckMesheIsValid(const int32& _Row, const int32& _Col);
	//删除所有的网格
	UFUNCTION(BlueprintCallable)
	void ClearAllMeshes();
	//生成网格
	UFUNCTION(BlueprintCallable)
	void SpawnMeshes();
	//创建的网格
	UFUNCTION(BlueprintCallable, Category = "MouseMeshManager")
	static AMapMeshe* CreateNewMapMeshe(
		UMesheControllComponent* Comp, //网格控制组件
		FTransform Trans,//网格位置
		ELineType LineType,//网格线路类型
		int32 Translucy,//网格渲染层
		FLine Line,//线路
		const TArray<FMesheColPrePlayCard>& PlayCard,//预放置卡片
		bool bCreateBG,//是否创建背景
		FSoftObjectPath BgPath,//背景路径
		FVector BgOffset,//背景偏移
		bool bInitMeshe = true//是否初始化到网格控制器组件中
	);
	//替换网格类型
	UFUNCTION(BlueprintCallable, Category = "MouseMeshManager")
	static AMapMeshe* ReplaceMapMeshe(FLine Line, ELineType LineType);
	//替换所有网格
	UFUNCTION(BlueprintCallable, Category = "MouseMeshManager")
	static void ReplaceAllMapMeshe(ELineType CurLineType, ELineType TargetLineType);
protected:
	//初始化地图
	UFUNCTION()
	void InitGameMapMeshe();
	//初始化网格地图
	UFUNCTION()
	void InitMeshesMap(const int32& _Row, const int32& _Col);
	//初始化网格
	UFUNCTION()
	void InitMesh(AActor* _Mesh, const FVector& _MeshLocation);
	//设置网格
	UFUNCTION()
	void SetMapMesh(AMapMeshe* NewMapMeshe, int32 _Row, int32 _Col);
private:
	//每一行的最后一个的位置
	UPROPERTY()
	TArray<FVector> M_LineLastLocation;
	//当前地图的总行和总列
	UPROPERTY()
	FLine M_CurMapLine;
	//当前所有网格合集
	UPROPERTY()
	TArray<AActor*> M_CurMeshes;
	//当前所有网格的位置
	UPROPERTY()
	TArray<FVector> M_CurMeshesLocation;
	//地图配置管理器
	UPROPERTY()
	UGameMapStructManager* M_GameMapStructManager = nullptr;
	//当前所有的老鼠网格管理器
	UPROPERTY()
	TArray<AMapMouseMesheManager*> CurMapMouseMesheManager;
	//当前动物管理器
	UPROPERTY()
	TArray<class AVSAnimalBase*> Animals;
	//地图的发射器
	UPROPERTY()
	TArray<class AMapLauncherBase*> MapLaunchers;
private:
	//地图的网格
	UPROPERTY()
	UClass* MapMesheClass = nullptr;
	//地图网格老鼠管理器
	UPROPERTY()
	UClass* MapMouseMesheManagerClass = nullptr;
	//动物类
	UPROPERTY()
	TArray<UClass*> AnimalsClass;
	//地图发射器类
	UPROPERTY()
	TArray<UClass*> MapLauncherClass;
	//精灵Actor类
	UPROPERTY()
	UClass* SpriteActorClass = nullptr;
};
