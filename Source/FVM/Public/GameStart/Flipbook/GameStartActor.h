// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameStart/Flipbook/GameActorFlipbookBase.h"
#include "GameStartActor.generated.h"

/**
 * 游戏开始Actor【自动管理Tick】
 */
UCLASS()
class FVM_API AGameStartActor : public AGameActorFlipbookBase
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void GameStartActorGameOver();
};
