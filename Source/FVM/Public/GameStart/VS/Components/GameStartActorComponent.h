// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameStartActorComponent.generated.h"


//游戏开始组件【自动管理Tick】
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FVM_API UGameStartActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGameStartActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GameTick(float DeltaTime);

	//游戏开始


	//游戏结束
	UFUNCTION(BlueprintPure)
	bool GetbGameOver();

private:
	//游戏结束
	UFUNCTION()
	void GameOver(bool bGameWin);

	//游戏状态-游戏开始
	bool* bSafePtr_bGameStartState = nullptr;
	//游戏状态-游戏结束
	bool* bSafePtr_bGameOverState = nullptr;
};
