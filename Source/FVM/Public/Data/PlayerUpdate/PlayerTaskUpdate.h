// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/PlayerStructManager.h"
#include "PlayerTaskUpdate.generated.h"

/**
 * 任务更新
 */
UCLASS()
class FVM_API UPlayerTaskUpdate : public UPlayerUpdateClass
{
	GENERATED_BODY()
public:
	virtual void Update_Implementation(class UPlayerStructManager* PlayerDataIns) override;
};
