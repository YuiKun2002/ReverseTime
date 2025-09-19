// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/VS/UI/Meshe/UIMesheWidthAll.h"
#include "GameStart/Flipbook/GameActor/CardActor.h"
#include "GameStart/Flipbook/GameActor/GamePlayer.h"

#include "GameStart/VS/Components/CardManagerComponent.h"
#include "GameStart/VS/Components/VSManagerComponent.h"
#include "GameStart/VS/Components/Card/CardDataComponent.h"
#include "GameStart/VS/CardCoolDownManager.h"
#include "GameStart/VS/GameMapInstance.h"
#include "GameStart/VS/MapMeshe.h"


#include "GameStart/VS/Components/ResourceManagerComponent.h"
#include "GameStart/VS/Components/MesheControllComponent.h"

#include "GameStart/VS/CreateCardBar.h"
#include "GameStart/VS/UI/UI_Card.h"

#include "GameSystem/FVMGameInstance.h"

#include <Kismet/GameplayStatics.h>

#include <Components/Image.h>

bool UUIMesheWidthAll::CreateCard(
	UUI_MapMeshe* UI_MapMeshe,
	UCardManagerComponent* CardManagerComponent,
	int32 CardID,
	bool ResetCoolDown,
	bool bCheck)
{
	if (Super::CreateCard(UI_MapMeshe, CardManagerComponent, CardID, ResetCoolDown, bCheck))
	{
		//当前网格类型为All，只要卡片实例是有效的，可以直接进行创建
		if (
			UUIMesheWidthAll::GetCardInstance(
				UI_MapMeshe,
				CardManagerComponent,
				CardID,
				ResetCoolDown,
				[&](UClass* CardRes, const FItemCard& CardData)->bool {
					return true;
				})
			)
		{
			return true;
		}
	}

	return false;
}

bool UUIMesheWidthAll::GetCardInstance(
	UUI_MapMeshe* UI_MapMeshe,
	UCardManagerComponent* CardManagerComponent,
	int32 CardID,
	bool bResetCoolDown,
	TFunctionRef<bool(UClass*, const FItemCard&)> _Func
)
{
	FItemCard CardBaseData;
	CardBaseData.ItemID = CardID;

	//卡片资源
	UClass* CardInstance = nullptr;

	//检查卡片资源和获取卡片数据
	if (
		UUIMesheWidthAll::CheckCardClassInstance(
			UI_MapMeshe,
			CardManagerComponent,
			CardInstance,
			CardBaseData)
		)
	{
		//如果当前卡片类型为All或者网格类型为All，可以直接进行卡片放置操作
		if (CardBaseData.M_ELineType == ELineType::All || UI_MapMeshe->GetLineType() == ELineType::All)
		{
			//测试卡片层级
			if (UI_MapMeshe->TestCardLayer(CardBaseData.M_CardLayer))
			{
				UI_MapMeshe->PlayCard(CardManagerComponent, CardInstance, CardBaseData, bResetCoolDown);
			}

#if WITH_EDITOR
			if (UFVMGameInstance::GetDebug())
			{
				UGameSystemFunction::FVMLog(__FUNCTION__,
					FString(TEXT("【卡片检查截断】当前卡片类型为All，可以直接放置：") +
						CardBaseData.ItemName));
			}
#endif
			return false;
		}


		//返回数据
		return _Func(CardInstance, CardBaseData);
	}

	return false;
}

bool UUIMesheWidthAll::CheckCardClassInstance(
	UUI_MapMeshe*& UI_MapMeshe,
	UCardManagerComponent*& CardManagerComponent,
	UClass*& CardResourceClass,
	FItemCard& CardData
)
{
	//获取ID
	int32 CardID = CardData.ItemID;

	//获取卡片数据
	FItemCard* TempCardData = CardManagerComponent->GetCardDataByID(CardID);
	//如果当前卡片无效,重新获取数据，如果数据查询失败，则返回false，无法放置
	if (!TempCardData)
	{
		if (CardManagerComponent->AddNewCardDataAndOut(CardID, *TempCardData))
		{
#if WITH_EDITOR
			GAME_LOG(__FUNCTION__, TEXT("{不存在列表中}，重新通过卡片ID号，获取基础数据"), {
						GAME_LOG_LINE(TEXT("数据获取成功，新增到列表：[%s]"),*(TempCardData->ItemName));
				});
#endif
		}
		else {
#if WITH_EDITOR
			GAME_LOG(__FUNCTION__, TEXT("{不存在列表中}，重新通过卡片ID号，获取基础数据"), {
						GAME_LOG_LINE(TEXT("数据获取失败，结束创建卡片实例"));
				});
#endif
			return false;
		}
	}


	//赋值卡片数据
	CardData = *TempCardData;

	//获取卡片UI的数据
	UUI_Card* UICardIns = CardManagerComponent->GetUICard(CardID);
	if (IsValid(UICardIns))
	{
		CardData.M_CardPrice = UICardIns->GetCardFlame();
		CardResourceClass = UICardIns->GetCardClassInstance();
	}

#if WITH_EDITOR
	GAME_LOG(__FUNCTION__, TEXT("通过卡片ID号，获取基础数据"), {
						GAME_LOG_LINE(TEXT("数据获取成功：[%s]"),*(CardData.ItemName));
		});
#endif


	//判断当前格子是否有角色
	if (CardData.M_CardLayer == 0 && UI_MapMeshe->GetbHasPlayer())
	{

#if WITH_EDITOR
		//查询到角色
		if (UFVMGameInstance::GetDebug())
		{
			UGameSystemFunction::FVMLog(__FUNCTION__,
				FString(TEXT("卡片放置失败，当前ID已经被角色占用")));
		}
#endif
		AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(
			TEXT("当前位置被主角占用，你将无法放置"));

		return false;
	}

	//初始化卡片资源
	if (!IsValid(CardResourceClass))
	{
		CardResourceClass = CardData.CardActorResource.TryLoadClass<ACardActor>();
		if (!IsValid(CardResourceClass))
		{
#if WITH_EDITOR
			//查询到角色
			UGameSystemFunction::FVMLog(__FUNCTION__,
				FString(TEXT("卡片资源类型加载失败，请检查数据表FSoftClassPath的引用，是否有效！：")) +
				CardData.ItemName
			);
#endif
			return false;
		}
	}

	return true;
}
