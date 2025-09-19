// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/VS/UI/Meshe/UIMesheWidthOnWater.h"
#include "GameStart/VS/UI/Meshe/UIMesheWidthAll.h"
#include "GameStart/Flipbook/GameActor/CardActor.h"

#include "GameStart/VS/Components/CardManagerComponent.h"
#include "GameStart/VS/Components/VSManagerComponent.h"
#include "GameStart/VS/GameMapInstance.h"
#include "GameStart/VS/MapMeshe.h"


bool UUIMesheWidthOnWater::CreateCard(
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

				//卡片属于水上类型
				if (CardData.M_ELineType == ELineType::OnWater)
				{
					return this->PlayCard(CardManagerComponent, CardRes, CardData, ResetCoolDown);
				}
				else {
					//卡片属于非水上类型
					if (this->GroundCardToOnWater(CardData, CardManagerComponent))
					{
						return this->PlayCard(CardManagerComponent, CardRes, CardData, ResetCoolDown);
					}
				}

				return false;
			}
		);
	}

	return false;
}

bool UUIMesheWidthOnWater::OnCardLayersUpdate(ELineType CardLineType, int32 CardLayer, ACardActor* CardActor)
{
	if (Super::OnCardLayersUpdate(CardLineType, CardLayer, CardActor))
	{
		switch (CardActor->GetLineType())
		{
		case ELineType::OnGround:
		{
			//创建落水动画
			AActor* CurAnim = this->GetMapMeshe()->GetWorld()->SpawnActor(
				LoadClass<AActor>(
					0,
					TEXT("Blueprint'/Game/Resource/Texture/Sprite/VS/Map/0/WaterAnim/BP_落水动画.BP_落水动画_C'")
				)
			);

			CurAnim->SetActorLocation(CardActor->GetUIMapMesh()->GetMapMeshe()->GetActorLocation());
		}
		break;
		case ELineType::SkyAndGround:return false;
		case ELineType::Sky:return false;

		}
	}

	CardActor->KillCard(this);

	return true;
}

void UUIMesheWidthOnWater::UpdateCardLocation(ACardActor* SingleCard /*= nullptr*/)
{
	if (IsValid(SingleCard))
	{
		int32 CurLayer = SingleCard->GetCardSourceData().M_CardLayer;
		SingleCard->SetActorLocation(this->GetMapMeshe()->GetActorLocation());
		if (CurLayer == -1)
		{
			SingleCard->AddActorLocalOffset(FVector(0.f, 0.f, 5.f));
		}
		else {
			SingleCard->AddActorLocalOffset(FVector(0.f, 0.f, 12.f));
		}
	}
	else {
		for (const auto& Cur : this->GetCardLayers())
		{
			if (IsValid(Cur.Value))
			{
				int32 CurLayer = Cur.Value->GetCardSourceData().M_CardLayer;
				Cur.Value->SetActorLocation(this->GetMapMeshe()->GetActorLocation());
				if (CurLayer == -1)
				{
					Cur.Value->AddActorLocalOffset(FVector(0.f, 0.f, 5.f));
				}
				else {
					Cur.Value->AddActorLocalOffset(FVector(0.f, 0.f, 12.f));
				}
			}
		}
	}
}

void UUIMesheWidthOnWater::PlayFinish(ACardActor* NewCard, bool bResetPlay)
{
	Super::PlayFinish(NewCard, bResetPlay);

	if (this->GetMapMeshe())
	{
		this->GetMapMeshe()->UpdateCardsLocation(NewCard);
	}
}

bool UUIMesheWidthOnWater::GroundCardToOnWater(const FItemCard& _Card, UCardManagerComponent* _CardMComp)
{
	//如果卡片是陆地类型
	if (
		_Card.M_ELineType == ELineType::OnGround
		||
		_Card.M_ELineType == ELineType::UnderwaterAndGround
		)
	{
		//查询卡片 ->层级为-1 （查询承载类型卡片）
		ACardActor* _Reult = this->GetCardLayers(-1);

		//如果查询成功并且这个卡片类型和当前网格类型匹配
		if (IsValid(_Reult) && (_Reult)->GetLineType() == this->GetLineType())
		{
			return true;
		}
	}
	else {
		if (
			_Card.M_ELineType == ELineType::SkyAndGround
			||
			_Card.M_ELineType == ELineType::Sky
			)
		{
			return true;
		}
	}

	return false;
}
