// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/NoExportTypes.h"
#include "GameStart/VS/MapBaseType.h"
#include "GameSystem/GameStartSubsystem.h"
#include "GameMapFogManager.generated.h"

/**
 * 迷雾管理器
 */

class AGameMapInstance;

//迷雾块
UCLASS()
class FVM_API AGameMapFogBlock : public AActor
{
	GENERATED_BODY()
public:
	//显示事件
	UFUNCTION(BlueprintImplementableEvent)
	void Dispaly(bool Value);
	//时间迷雾消失[强行消失一段时间]
	UFUNCTION(BlueprintImplementableEvent)
	void TimeDispaly(float time);
	//设置渲染层
	UFUNCTION(BlueprintImplementableEvent)
	void SetTrans(int32 Trans);
	//获取显示结果
	UFUNCTION(BlueprintCallable)
	bool GetDisplayResult();
public:
	void SetObj(UObject* NewObj, bool bAdd);
	void TimeHidd(float time);
private:
	UPROPERTY()
	TArray<UObject*> Objs;
	UPROPERTY()
	bool bInit = false;
};

//迷雾管理器
UCLASS()
class FVM_API UGameMapFogManager : public UObject, public IGameStartInterface
{
	GENERATED_BODY()
public:
	UGameMapFogManager();

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
	//显示单个迷雾
	UFUNCTION(BlueprintCallable)
	void SingleBlock(UObject* NewObj, bool bShow, int32 RowIndex, int32 ColIndex);
	//范围显示[Row,Col]
	UFUNCTION(BlueprintCallable)
	void RangeBlock(UObject* NewObj, bool bShow, FLine RangeRow, FLine RangeCol);
	//全屏设置
	UFUNCTION(BlueprintCallable)
	void FullFogBlock(UObject* NewObj, bool bShow);
	//全屏时间消失
	UFUNCTION(BlueprintCallable)
	void FullTimeHiddBlock(float time);
private:
	//初始化全部迷雾块，
	UFUNCTION()
	bool InitAllBlock();
private:
	//所有存在的迷雾块
	UPROPERTY()
	TArray<AGameMapFogBlock*> AllFogBlocks;
	//当前行列
	UPROPERTY()
	FLine CurRowAndCol;
	//初始化
	UPROPERTY()
	bool bInit = false;
	//释放
	UPROPERTY()
	bool bUnload = false;
	//迷雾类
	UPROPERTY()
	UClass* FogBlockClass = nullptr;
};