// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameStart/VS/UI/UI_MapMeshe.h"
#include "GameStart/VS/UI/Meshe/UIMesheWidthMagma.h"
#include "UIMesheWidthUnderWater.generated.h"

/**
 *
 */

class ACardActor;
class AActor;

UCLASS()
class FVM_API UUIMesheWidthUnderWater : public UUI_MapMeshe
{
	GENERATED_BODY()
public:

	virtual bool CreateCard(
		UUI_MapMeshe* UI_MapMeshe,
		UCardManagerComponent* CardManagerComponent,
		int32 CardID,
		bool ResetCoolDown,
		bool bCheck
	) override;

	virtual void TickMapMeshe(float DeltaTime) override;

	virtual bool OnCardLayersUpdate(ELineType CardLineType, int32 CardLayer, ACardActor* CardActor) override;

	virtual void PlayFinish(ACardActor* NewCard, bool bResetPlay = false) override;

private:
	//网格更新
	UPROPERTY()
	FMapMeshUpdate M_MapMeshUpdate;
	//气泡
	UPROPERTY()
	AActor* WaterBubbles = nullptr;
};
