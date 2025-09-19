// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameStartActorBase.generated.h"

//游戏结束委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameStartActorGameOverDelegate);

/**
 * 游戏开始基础Actor【自动管理Tick】
 */
UCLASS()
class FVM_API AGameStartActorBase : public AActor
{
	GENERATED_BODY()

	friend class AGameStartActor;

public:
	//当游戏结束时
	UPROPERTY(EditAnywhere, BlueprintAssignable)
	FGameStartActorGameOverDelegate OnGameOverDelegate;

public:
	// Sets default values for this actor's properties
	AGameStartActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//游戏更新
	virtual void GameTick(float DeltaTime);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "GameTick"))
	void BlueprintTick(float DeltaSeconds);

public:

	//设置排序优先级
	UFUNCTION(BlueprintCallable)
	virtual void SetRenderLayer(int32 Layer);

public:
	//获取排序优先级
	UFUNCTION(BlueprintPure)
	virtual int32 GetRenderLayer() const;

	//获取游戏是否结束
	UFUNCTION(BlueprintPure)
	bool GetbGameOver();

private:
	//游戏状态-游戏开始
	bool* bSafePtr_bGameStartState = nullptr;
	//游戏状态-游戏结束
	bool* bSafePtr_bGameOverState = nullptr;
private:
	//对象的渲染顺序
	int32 GameActorLayer = 0;
};
