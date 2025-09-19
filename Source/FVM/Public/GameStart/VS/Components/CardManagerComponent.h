// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameSystem/Item/ItemStruct.h"
#include "GameSystem/GameStartSubsystem.h"
#include "CardManagerComponent.generated.h"

class UUI_CardBar;
class ACardActor;
class ACardCoolDownManager;
//卡片
class UUI_Card;
//铲子
class UUI_EradicateCard;
//网格
class UUI_MapMeshe;
//翻书动画
class UPaperFlipbook;

//卡片管理器组件
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FVM_API UCardManagerComponent : public UActorComponent, public IGameStartInterface
{
	GENERATED_BODY()
public:

	// Sets default values for this component's properties
	UCardManagerComponent();

	//游戏开始
	virtual void GameStartInit(class AGameMapInstance* GameMapInstance) override;
	virtual void GameStart(class AGameMapInstance* GameMapInstance) override;

	//更新
	virtual void GameTick(float DeltaTime) override;

	//游戏结束【可以关联其他管理器，或者访问其他对象，访问】
	virtual void GameOverInit(class AGameMapInstance* GameMapInstance, bool bGameWin) override;
	virtual void GameOver(class AGameMapInstance* GameMapInstance, bool bGameWin) override;

	//资源释放【不能访问其他对象，收尾】
	virtual void GameResourceUnload() override;

public:
	//设置当前卡片网格
	UFUNCTION(BlueprintCallable)
	void SetCurrentCardMapMeshe(UUI_MapMeshe* _MapMeshe);
	//设置一张卡片的背景
	UFUNCTION(BlueprintCallable)
	void SetCardUITexture(UUI_Card* _CardUI, const FString& _CardBg);
	//设置上一次选择的卡片ID和实例
	UFUNCTION(BlueprintCallable)
	void SetLastCardIDAndClass(int32 LastCardID, UClass* LastCardIns);
	//设置铲子UI实例
	UFUNCTION(BlueprintCallable)
	void SetEradicateCardInstance(class UUI_EradicateCard* EradicateCardIns);
public:

	//获取当前放置卡片所在的网格
	UFUNCTION(BlueprintPure)
	UUI_MapMeshe* GetCurrentCardMapMeshe();
	//获取当前选中的Actor对象
	UFUNCTION(BlueprintPure)
	UClass* GetCurrentSelectActor();
	//获取当前选择的卡片名称（通过UI选择卡片->选择卡片的名称）
	UFUNCTION(BlueprintPure)
	int32 GetCurrentSelectCardID() const;

	//获取上一次选择的卡片实例对象
	UFUNCTION(BlueprintPure)
	UClass* GetLastSelectCardClass();
	//获取上一次选择的卡片
	UFUNCTION(BlueprintPure)
	int32 GetLastSelectCardID();

	//获取卡片的火苗数量
	UFUNCTION(BlueprintPure)
	const int32 GetCardFlameByID(int32 ID);

	//获取指定卡片，是否冷却完成
	UFUNCTION(BlueprintPure)
	bool GetCardCoolDownFinish(int32 ID);
	//是否选择铲子
	UFUNCTION(BlueprintPure)
	bool GetbSelectEradicate() const;
	
	//获取卡片UI
	UFUNCTION(BlueprintPure)
	void GetCardsUI(TArray<UUI_Card*>& OutCardsUI);

	//获取卡片数据通过卡片名称获取
	UFUNCTION(BlueprintPure)
	bool GetCardDataByID(int32 ID, FItemCard& _CardFItemData) const;
	FItemCard* GetCardDataByID(int32 ID);

	//获取UICard
	UFUNCTION(BlueprintPure)
	UUI_Card* const GetUICard(int32 ID);

	//获取当前选中卡片的冷却
	UFUNCTION(BlueprintPure)
	ACardCoolDownManager* GetCurrentCoolDownManager();
	//获取卡片冷却管理器
	UFUNCTION(BlueprintPure)
	ACardCoolDownManager* GetCardCoolDownManagerByID(int32 ID);

	//获取卡片栏
	UFUNCTION(BlueprintPure)
	UUI_CardBar* GetUICardBar();
	//获取卡片数据对象
	UFUNCTION(BlueprintPure)
	class UCardDataObject* GetCardDataObject();

public:

	//添加卡片（到卡片列表存储起来）
	UFUNCTION(BlueprintCallable)
	void AddNewCardData(int32 ID, const FItemCard& _CardData);
	bool AddNewCardDataAndOut(int32 ID, FItemCard& OutData,int32 NewLevel = 0);

	//选择一张卡片（通过UI选择卡片->选择）
	UFUNCTION(BlueprintCallable)
	bool SelectCurrentActor(int32 ID, bool ForceSelect = false);
	//取消当前选择的卡片（通过UI选择卡片->取消）
	UFUNCTION(BlueprintCallable)
	void CancelSelect();

	//选择铲子功能
	UFUNCTION(BlueprintCallable)
	void SelectEradicate();
	//取消铲子功能
	UFUNCTION(BlueprintCallable)
	void CancelEradicate();

	//显示卡片栏
	UFUNCTION(BlueprintCallable)
	void ShowCardBar();
	//隐藏卡片栏
	UFUNCTION(BlueprintCallable)
	void HiddenCardBar();

private:
	//卡片数据
	UPROPERTY()
	TMap<int32, FItemCard> M_CardData;
	//UIcard数据
	UPROPERTY()
	TMap<int32, UUI_Card*> M_UICard;

	//当前选中的卡片实例
	UPROPERTY()
	UClass* M_CurrentSelectCardInstance;
	//当前选择的卡片
	UPROPERTY()
	int32 M_CurrentSelectCardID;

	//上一次选择的卡片
	UPROPERTY()
	UClass* M_LastSelectCardClass;
	//上一次选择的卡片
	UPROPERTY()
	int32 M_LastSelectCardID;

	//当前是否选择了铲子
	UPROPERTY()
	bool M_CurrentSelectEradicate = false;
	//卡片选择界面
	UPROPERTY()
	UUI_CardBar* M_UUI_CardBar = nullptr;
	//当前玩家放置卡片发位置
	UPROPERTY()
	UUI_MapMeshe* M_CurrentPlayCardMapMeshe = nullptr;
	//铲子UI
	UPROPERTY()
	class UUI_EradicateCard* M_UUI_EradicateCard = nullptr;
	//卡片数据组件
	UPROPERTY()
	class UCardDataObject* CardDataObject = nullptr;
};
