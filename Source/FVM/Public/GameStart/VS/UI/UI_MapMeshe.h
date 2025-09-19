// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "GameStart/VS/MapBaseType.h"
#include "GameSystem/Item/ItemStruct.h"
#include "UI_MapMeshe.generated.h"

class UImage;
class ACardActor;
class AMapMeshe;
class UCardManagerComponent;
class UUI_Card;
class AMouseActor;
class AGamePlayer;

UCLASS()
class FVM_API UUI_MapMeshe : public UWidgetBase
{
	GENERATED_BODY()

	friend class AMapMeshe;
public:
	//设置UI网格颜色【Debug】
	UFUNCTION(BlueprintCallable)
	void SetColor(FVector _Vec, float _Alpha);
	//设置网格地图
	UFUNCTION(BlueprintCallable)
	void SetMapMeshe(AMapMeshe* _Meshe);
	//设置角色实例【放置角色后会自动调用】
	UFUNCTION(BlueprintCallable)
	void SetPlayerInstance(class AGamePlayer* NewPlayerIns);
	//设置线路
	UFUNCTION(BlueprintCallable)
	void SetLine(FLine CurLine);
	//设置卡片碰撞
	UFUNCTION(BlueprintCallable)
	void SetCardCollisionByID(int32 ID);
	//设置网格禁用
	UFUNCTION(BlueprintCallable)
	void SetUIMesheOccupancy(bool value, UObject* CurObj);
	//设置新的卡片层，传入的卡片层会被修改！！！
	UFUNCTION(BlueprintCallable)
	void SetCardLayers(TMap<int32, ACardActor*>& LastCards);
public:
	//检查网格是否被其他独立对象占用
	UFUNCTION(BlueprintPure)
	bool CheckMapMesheOccupancy();
	//当前网格是否开启
	UFUNCTION(BlueprintPure)
	bool GetbUIMesheOccupancy();
	//获取是否存在角色
	UFUNCTION(BlueprintPure)
	bool GetbHasPlayer() const;
	//获取当前Actor网格对象是否被使用，如果为false网格类型则为Disable,无法放置卡片
	UFUNCTION(BlueprintPure)
	bool GetbMapMesheUsed() const;

	//获取当前网格的最顶部网格位置
	UFUNCTION(BlueprintPure)
	FVector GetMapMeshCurrentTop(int32 _Col) const;
	//获取当前网格的最底部网格位置
	UFUNCTION(BlueprintPure)
	FVector GetMapMeshCurrentBottom(int32 _Col) const;
	//获取当前网格的最左边的网格位置
	UFUNCTION(BlueprintPure)
	FVector GetMapMeshCurrentLeft(int32 Row) const;
	//获取当前网格的最右边的网格位置
	UFUNCTION(BlueprintPure)
	FVector GetMapMeshCurrentRight(int32 Row) const;
	//获取线路
	UFUNCTION(BlueprintPure)
	FLine GetLine() const;
	//获取线路类型
	UFUNCTION(BlueprintPure)
	ELineType GetLineType() const;
	//获取UI在世界中的变换
	UFUNCTION(BlueprintPure)
	FTransform GetUIMapMesheTransform() const;

	//获取网格禁用的对象
	UFUNCTION(BlueprintPure)
	TArray<UObject*> GetMapMesheOccupancyObjects();
	//获取当前网格
	UFUNCTION(BlueprintPure)
	AMapMeshe* GetMapMeshe();
	//获取所有的卡片对象
	UFUNCTION(BlueprintPure)
	void GetAllCardsInstance(TArray<ACardActor*>& OutAllCards);
	//获取当前网格所有的卡片数据
	UFUNCTION(BlueprintPure)
	ACardActor* GetCardLayers(int32 CardLayer);
	const TMap<int32, ACardActor*>& GetCardLayers();
public:
	//创建卡片实例
	UFUNCTION(BlueprintCallable)
	bool CreateNewCard(UCardManagerComponent* CardManagerComponent);

	//创建一张免费的卡片[等级为-1，则根据默认等级进行创建]
	UFUNCTION(BlueprintCallable)
	bool CreateFreeCard(int32 CardID, int32 CardLevel = -1);

	//放置卡片（卡片组件，卡片数据，卡片价格，是否重置卡片冷却时间）核心
	UFUNCTION(BlueprintCallable)
	bool PlayCard(
		UCardManagerComponent* CardManagerComponent,
		UClass* CardResourceInstance,
		const FItemCard& CardData,
		bool bResetCoolDown
	);
	////放置卡片
	//bool BeginPlayCard(
	//	FItemCard& _Card,
	//	UCardManagerComponent* _CardMComp,
	//	UClass* _CardRes,
	//	int32 _Price,
	//	bool ResetCoolDown
	//);
	//销毁卡片
	UFUNCTION(BlueprintCallable)
	bool EradicateCard();
	//通过ID销毁卡片
	UFUNCTION(BlueprintCallable)
	bool EradicateCardByLayer(int32 CardLayer);
	//销毁所有卡片
	UFUNCTION(BlueprintCallable)
	void KillAllCards();
	//根据卡片的类型，打印提示文字
	void CardTypeDebug(const FString& _CardName, ELineType _LineType);
	//放置角色
	UFUNCTION(BlueprintCallable)
	bool PlayPlayer();
	//关闭所有卡片碰撞
	UFUNCTION(BlueprintCallable)
	void CloseAllCardsCollision();
	//更新所有卡片碰撞
	UFUNCTION(BlueprintCallable)
	void UpdateAllCardsCollision();
	//添加ID
	UFUNCTION(BlueprintCallable)
	bool AddCardLayer(const int32 CardLayer, ACardActor* _Card);
	//卡片ID测试
	UFUNCTION(BlueprintCallable)
	bool TestCardLayer(const int32 CardLayer);

public:

	//渲染层级出现更新修改时
	UFUNCTION(BlueprintImplementableEvent)
	void OnRenderLayerChanged(int32 _CurTrans);

	//初始化
	virtual bool Initialize() override;
	//当一个Actor开始与网格发生重叠
	virtual void BoxOverlapBeginByAMapMeshe(AActor* _Actor);
	//当一个Actor与网格发生重叠结束
	virtual void BoxOverlapEndByAMapMeshe(AActor* _Actor);
	//每一帧都会更新网格，你可以设置是否更新
	virtual void TickMapMeshe(float DeltaTime);
	/*
			当卡片层级出现更新了
		1.例如 当前网格类型为陆地，将以前水路的卡片层级传入陆地的网格类型中，则只能放在水路的卡片需要被销毁
	*/
	virtual bool OnCardLayersUpdate(ELineType CardLineType, int32 CardLayer, ACardActor* CardActor);
	//更新卡片位置，如果当前Actor网格可移动，才会更新卡片位置
	virtual	void UpdateCardLocation(ACardActor* SingleCard = nullptr);
	/*
	  创建一张防御卡
	*/
	virtual bool CreateCard(
		UUI_MapMeshe* UI_MapMeshe,
		UCardManagerComponent* CardManagerComponent,
		int32 CardID,
		bool ResetCoolDown,
		bool bCheck
	);
	//卡片放置完成
	virtual void PlayFinish(ACardActor* NewCard, bool bResetPlay = false);
public:
	//获取排序优先级
	UFUNCTION(BlueprintPure)
	int32 GetRenderLayer() const;

	//获取角色的渲染层
	UFUNCTION(BlueprintPure)
	int32 GetPlayerRenderLayer() const;

	//设置排序优先级
	UFUNCTION(BlueprintCallable)
	void SetRenderLayer(int32 _Translucency);
private:
	//当老鼠进入时
	void OnMouseEnter(AMouseActor* Mouse);
	//当老鼠退出时
	void OnMouseEnd(AMouseActor* Mouse);
	//播放音效
	void PlayCard_PlayBGM();
private:
	//格子图片
	UPROPERTY()
	UImage* M_Gird = nullptr;
	//地图Actor网格
	UPROPERTY()
	AMapMeshe* M_MapMeshe = nullptr;
	//玩家角色或者【电脑】
	UPROPERTY()
	AGamePlayer* M_PlayerIns = nullptr;
	//网格控制组件
	UPROPERTY()
	class UMesheControllComponent* MesheCtr = nullptr;
	//卡片放置动画
	UPROPERTY()
	UClass* CardPlayAnimClass = nullptr;
private:
	//Tick更新时间
	UPROPERTY()
	float M_UpdateTime = 0.f;
	//渲染优先级
	UPROPERTY()
	int32 M_Translucency = 0;
	//是否有角色
	UPROPERTY()
	bool bHasChar = false;
	//是否开启网格
	UPROPERTY()
	bool bMesheEnabled = true;
	//是否有角色
	UPROPERTY()
	bool bPlayer = false;

	//UIMapMeshe线路
	UPROPERTY()
	FLine M_Line;
	//地图网格的位置
	UPROPERTY()
	FTransform M_MapMesheTransform;
	//格子类型
	UPROPERTY()
	ELineType M_ELineType;
	//卡片信息集合,Layer【卡片层】
	UPROPERTY()
	TMap<int32, ACardActor*> M_CardLayers;
	//占用道具，禁用当前网格
	UPROPERTY()
	TMap<FString, UObject*> M_MesheDis;
};