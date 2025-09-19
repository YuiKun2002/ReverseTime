// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/VS/UI/Meshe/UIMesheWidthGround.h"
#include "GameStart/VS/UI/Meshe/UIMesheWidthAll.h"
#include "GameStart/Flipbook/GameActor/CardActor.h"
#include "GameStart/VS/Components/CardManagerComponent.h"



bool UUIMesheWidthGround::CreateCard(
	UUI_MapMeshe* UI_MapMeshe,
	UCardManagerComponent* CardManagerComponent,
	int32 CardID,
	bool ResetCoolDown,
	bool bCheck)
{
	if (Super::CreateCard(UI_MapMeshe, CardManagerComponent, CardID, ResetCoolDown, bCheck))
	{
		return UUIMesheWidthAll::GetCardInstance(
			UI_MapMeshe,
			CardManagerComponent,
			CardID,
			ResetCoolDown,
			[&](UClass* CardRes, const FItemCard& CardData)->bool {

				//卡片地形判断
				if (
					CardData.M_ELineType == ELineType::OnGround ||
					CardData.M_ELineType == ELineType::UnderwaterAndGround ||
					CardData.M_ELineType == ELineType::SkyAndGround
					)
				{
					return this->PlayCard(CardManagerComponent, CardRes, CardData, ResetCoolDown);
				}

				return false;
			});
	}

	return false;
}

bool UUIMesheWidthGround::OnCardLayersUpdate(
	ELineType CardLineType,
	int32 CardLayer,
	ACardActor* CardActor)
{
	if (Super::OnCardLayersUpdate(CardLineType, CardLayer, CardActor))
	{
		switch (CardActor->GetLineType())
		{
		case ELineType::UnderwaterAndGround:return false;
		case ELineType::SkyAndGround:return false;
		}

		CardActor->KillCard(this);

		return true;
	}

	return false;
}
