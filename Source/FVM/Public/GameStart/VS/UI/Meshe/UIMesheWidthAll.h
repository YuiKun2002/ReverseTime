// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameStart/VS/UI/UI_MapMeshe.h"
#include "UIMesheWidthAll.generated.h"

/**
 *	允许任何卡牌放置
 */
UCLASS()
class FVM_API UUIMesheWidthAll : public UUI_MapMeshe
{
	GENERATED_BODY()

public:

	virtual bool CreateCard(UUI_MapMeshe* UI_MapMeshe,
		UCardManagerComponent* CardManagerComponent,
		int32 CardID,
		bool ResetCoolDown,
		bool bCheck) override;
public:

	//获取卡片实例对象，返回卡片的资源类型和基础数据
	//1.如果卡片的放置类型为All，则会直接被放置，并且返回false中断其他检查
	static bool GetCardInstance(
		UUI_MapMeshe* UI_MapMeshe,
		UCardManagerComponent* CardManagerComponent,
		int32 CardID,
		bool bResetCoolDown,
		TFunctionRef<
		//卡片管理器，卡片资源实例，卡片基础数据，卡片价格，放置成功后是否重置冷却？
		bool(UClass*, const FItemCard&)
		> _Func);

private:
	//检查卡片的类实例[如果卡片的Class类无效，则无法放置卡片]
	//1.检查并返回对应的资源数据
	//2.如果返回失败表示，当前卡片始终无法创建!!!!
	static bool CheckCardClassInstance(
		UUI_MapMeshe*& UI_MapMeshe,
		UCardManagerComponent*& CardManagerComponent,
		UClass*& CardResourceClass,
		FItemCard& CardData);

};
