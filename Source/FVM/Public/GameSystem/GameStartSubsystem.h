// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameStartSubsystem.generated.h"

class AGameMapInstance;

//游戏接口
UINTERFACE(MinimalAPI)
class UGameStartInterface : public UInterface
{
	GENERATED_BODY()
};
//游戏接口
class FVM_API IGameStartInterface
{
	GENERATED_BODY()

public:

	/*
		以下表示执行顺序
	*/

	//游戏开始
	virtual void GameStartInit(class AGameMapInstance* GameMapInstance) = 0;
	virtual void GameStart(class AGameMapInstance* GameMapInstance) = 0;

	//更新
	virtual void GameTick(float DeltaTime) = 0;

	//游戏结束【可以关联其他管理器，或者访问其他对象，访问】
	virtual void GameOverInit(class AGameMapInstance* GameMapInstance, bool bGameWin) = 0;
	virtual void GameOver(class AGameMapInstance* GameMapInstance, bool bGameWin) = 0;

	//资源释放【不能访问其他对象，收尾】
	virtual void GameResourceUnload() = 0;
};

//子系统游戏开始
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStartSubsystemGameStartDelegate);
//子系统游戏结束
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStartSubsystemGameOverDelegate, bool, bGameWin);

/**
 * 游戏开始子系统
 */
UCLASS()
class FVM_API UGameStartSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	friend class AGameMapInstance;

public:
	//子系统游戏开始
	FOnGameStartSubsystemGameStartDelegate OnGameStartSubsystemGameStartDelegate;
	//子系统游戏结束
	FOnGameStartSubsystemGameOverDelegate OnGameStartSubsystemGameOverDelegate;

public:

	virtual TStatId GetStatId() const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;

	//初始化游戏子系统
	UFUNCTION(BlueprintCallable)
	void InitGameStartSubsystem(TSoftClassPtr<AGameMapInstance> GameMapRes);

public:

	//获取游戏开始子系统
	UFUNCTION(BlueprintPure, Category = "游戏开始子系统")
	static UGameStartSubsystem* GetGameStartSubsystemStatic();
	//添加游戏开始接口对象
	UFUNCTION(BlueprintCallable, Category = "游戏开始子系统")
	static void AddGameStartObject(TScriptInterface<IGameStartInterface> Interface);
	//---------------------------------------------------------------------游戏
	//游戏开始
	UFUNCTION(BlueprintCallable, Category = "游戏开始子系统 | 游戏")
	void GameStart();
	//游戏结束
	UFUNCTION(BlueprintCallable, Category = "游戏开始子系统 | 游戏")
	void GameOver(bool bGameWin);
	//退出游戏
	UFUNCTION(BlueprintCallable, Category = "游戏开始子系统 | 游戏")
	void QuitGame();
	//获取游戏开始状态
	UFUNCTION(BlueprintPure, Category = "游戏开始子系统 | 游戏")
	static bool GetbGameStart();
	//获取游戏结束状态
	UFUNCTION(BlueprintPure, Category = "游戏开始子系统 | 游戏")
	static bool GetbGameOver();
	//获取游戏开始状态
	bool& GetbGameStartState();
	//获取游戏结束状态
	bool& GetbGameOverState();
	//---------------------------------------------------------------------地图
		//获取游戏地图实例对象
	UFUNCTION(BlueprintPure, Category = "游戏开始子系统 | 游戏地图")
	AGameMapInstance* GetGameMapInstance();

	//---------------------------------------------------------------------角色

private:
	//---------------------------------------------------------------------游戏
	//游戏状态
	UPROPERTY()
	bool bGameStart = false;
	UPROPERTY()
	bool bGameOver = false;
	//所有继承与游戏开始接口的对象
	UPROPERTY()
	TArray<TScriptInterface<IGameStartInterface>> Games;

	//---------------------------------------------------------------------地图
	//地图实例
	UPROPERTY()
	class AGameMapInstance* GameMapIns = nullptr;
private:
	static UGameStartSubsystem* GameStartSubsystemStatic;
};
