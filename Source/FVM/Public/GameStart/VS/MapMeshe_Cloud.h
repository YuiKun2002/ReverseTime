// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameStart/VS/CustomMapMeshe.h"
#include "MapMeshe_Cloud.generated.h"

class ASpriteActor;

UCLASS()
class FVM_API AMapMeshe_Cloud : public ACustomMapMeshe
{
	GENERATED_BODY()

public:
	//设置云朵标记
	void SetCloudTag(ASpriteActor* _Cloud);
	//移除标记
	void RemoveCloudTag();

public:
	// Called every frame
	virtual void GameTick(float DeltaTime) override;
private:
	//被标记的云朵
	UPROPERTY()
	ASpriteActor* M_CloudActor = nullptr;

};
