// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameStart/VS/UI/UI_MapMeshe.h"
#include "UIMesheWidthOnWater.generated.h"

/**
 *
 */
UCLASS()
class FVM_API UUIMesheWidthOnWater : public UUI_MapMeshe
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

	virtual bool OnCardLayersUpdate(ELineType CardLineType, int32 CardLayer, ACardActor* CardActor) override;

	virtual	void UpdateCardLocation(ACardActor* SingleCard = nullptr) override;

	virtual void PlayFinish(ACardActor* NewCard, bool bResetPlay = false) override;
private:
	//线路不匹配：
	// 1.陆地卡放水上
	bool GroundCardToOnWater(const FItemCard& _Card, UCardManagerComponent* _CardMComp);
};
