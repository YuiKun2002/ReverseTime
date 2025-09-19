// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/VS/UI/Meshe/UIMesheWidthMagma.h"
#include "GameStart/VS/UI/Meshe/UIMesheWidthAll.h"

#include "GameStart/Flipbook/GameActor/CardActor.h"
#include "GameStart/Flipbook/GameActor/GamePlayer.h"
#include "GameStart/Flipbook/GameActor/FlyItemActor.h"

#include "GameStart/VS/Components/CardManagerComponent.h"
#include "GameStart/VS/Components/VSManagerComponent.h"
#include "GameStart/VS/CardCoolDownManager.h"
#include "GameStart/VS/GameMapInstance.h"
#include "GameStart/VS/MapMeshe.h"

#include "GameStart/VS/Components/ResourceManagerComponent.h"
#include "GameStart/VS/Components/MesheControllComponent.h"
#include "GameStart/VS/Components/Item\ShootLineComponent.h"

#include "GameStart/VS/CreateCardBar.h"
#include "GameStart/VS/UI/UI_Card.h"

#include "GameSystem/FVMGameInstance.h"

#include <Kismet/GameplayStatics.h>

#include <Components/Image.h>

bool UUIMesheWidthMagma::Initialize()
{
	return Super::Initialize();
}

bool UUIMesheWidthMagma::CreateCard(
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

				//如果卡片是岩浆类型
				if (CardData.M_ELineType == ELineType::Magma)
				{
					//放置卡片
					return this->PlayCard(CardManagerComponent, CardRes, CardData, ResetCoolDown);
				}
				else {
					//如果是其他类型
					if (
						CardData.M_ELineType == ELineType::OnGround
						|| CardData.M_ELineType == ELineType::Sky
						|| CardData.M_ELineType == ELineType::UnderwaterAndGround
						)
					{
						return this->PlayCard(CardManagerComponent, CardRes, CardData, ResetCoolDown);
					}
				}

				return false;
			});
	}

	return false;
}

void UUIMesheWidthMagma::BoxOverlapBeginByAMapMeshe(AActor* _Actor)
{
	Super::BoxOverlapBeginByAMapMeshe(_Actor);

	//查询卡片 ->层级为-1 （查询承载类型卡片）
	ACardActor* _Reult = this->GetCardLayers(-1);
	if (IsValid(_Reult))
	{
		if ((_Reult)->GetCardSourceData().ItemID == 102)
		{
			return;
		}
	}

	//获取直线类型子弹对象
	AFlyItemActor* FlyActor = Cast<AFlyItemActor>(_Actor);

	if (IsValid(FlyActor))
	{
		if (IsValid(FlyActor->GetComponentByClass(UShootLineComponent::StaticClass())))
		{
			FlyActor->ReturnPool();
		}
	}
}

void UUIMesheWidthMagma::BoxOverlapEndByAMapMeshe(AActor* _Actor)
{
	Super::BoxOverlapEndByAMapMeshe(_Actor);
}

void UUIMesheWidthMagma::TickMapMeshe(float DeltaTime)
{
	Super::TickMapMeshe(DeltaTime);

	if (!this->M_MapMeshUpdate.M_IsbUpdate)
	{
		return;
	}

	this->M_MapMeshUpdate.M_Time += DeltaTime;

	if (this->M_MapMeshUpdate.M_Time > 1.f)
	{
		this->M_MapMeshUpdate.M_Time = 0.f;

		bool LResult = false;

		for (auto& Card : this->M_MapMeshUpdate.GetUpdateCards())
		{
			if (IsValid(Card))
			{
				//更新卡片血量
				if (Card->AttackCard(1, this))
				{
					if (Card->GetbCardDeaded())
					{
						Card->KillCard();
					}
				}

				LResult = true;

				break;
			}
		}

		//如果列表为空
		if (!LResult)
		{
			this->M_MapMeshUpdate.GetUpdateCards().Empty();
			this->M_MapMeshUpdate.M_Time = 0.f;
			this->M_MapMeshUpdate.M_IsbUpdate = false;
		}
	}
}

bool UUIMesheWidthMagma::OnCardLayersUpdate(
	ELineType CardLineType,
	int32 CardLayer,
	ACardActor* CardActor
)
{
	if (Super::OnCardLayersUpdate(CardLineType, CardLayer, CardActor))
	{
		switch (CardActor->GetLineType())
		{
		case ELineType::OnGround:return false;
		case ELineType::SkyAndGround:return false;
		case ELineType::UnderwaterAndGround:return false;
		}
	}

	CardActor->KillCard(this);

	return true;
}

void UUIMesheWidthMagma::PlayFinish(ACardActor* NewCard, bool bResetPlay)
{
	Super::PlayFinish(NewCard, bResetPlay);

	FItemCard CardData = NewCard->GetCardSourceData();

	//如果卡片是岩浆类型
	if (CardData.M_ELineType == ELineType::Magma)
	{
		//查询卡片 ->层级为-1 （查询承载类型卡片）
		ACardActor* _Reult = this->GetCardLayers(-1);

		//如果查询成功并且这个卡片类型和当前网格类型匹配
		if (IsValid(_Reult))
		{
			//设置MapMeshe网格更新
			this->M_MapMeshUpdate.M_IsbUpdate = true;

			if ((_Reult)->GetCardSourceData().ItemID == 102)
			{
				//优先消耗棉花糖的血量
				this->M_MapMeshUpdate.AddCardUpdateAt((_Reult), 0);
			}
		}
		else {
			uint8 CardID[] = { 1,0 };
			for (auto& ID : CardID)
			{
				//查询卡片 ->层级为-1 （查询水上承载类型卡片）
				ACardActor* LReult = this->GetCardLayers(ID);
				//如果查询成功并且这个卡片类型和当前网格类型匹配
				if (IsValid(LReult))
				{
					this->M_MapMeshUpdate.AddCardUpdate((LReult));
					//设置MapMeshe网格更新
					this->M_MapMeshUpdate.M_IsbUpdate = true;
				}
			}
		}
	}
	else {
		//如果是其他类型
		if (
			CardData.M_ELineType == ELineType::OnGround
			||
			CardData.M_ELineType == ELineType::UnderwaterAndGround
			)
		{
			uint8 CardID[] = { 1,0 };
			for (auto& ID : CardID)
			{
				//查询卡片 ->层级为-1 （查询水上承载类型卡片）
				ACardActor* LReult = this->GetCardLayers(ID);
				//如果查询成功并且这个卡片类型和当前网格类型匹配
				if (IsValid(LReult))
				{
					this->M_MapMeshUpdate.AddCardUpdate((LReult));
					//设置MapMeshe网格更新
					this->M_MapMeshUpdate.M_IsbUpdate = true;
				}
			}
		}
	}

}
