// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStart/VS/MapBaseType.h"
#include "GameSystem/Item/ItemStruct.h"
#include "GameStart/VS/MapMesheInterface.h"
#include "GameStart/Flipbook/GameStartActor.h"
#include "GameSystem/Item/Card/CardAtkStruct.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "GameStart/VS/Components/MouseManagerComponent.h"
#include "GameStart/VS/Components/Card/CardDataComponent.h"
#include "CardActor.generated.h"


class UStaticMeshComponent;
class UCardDataComponent;
class UMouseManagerComponent;
class UPaperFlipbook;
class AGameMapInstance;

class AMapMeshe;
class AMapMouseMesheManager;
class UUI_MapMeshe;

class AMouseActor;
class UUI_Card;

class UBoxComponent;

class UPaperFlipbookComponent;


//生命数据
USTRUCT(BlueprintType)
struct FCard_HP {
	GENERATED_USTRUCT_BODY()
public:
	//当前生命值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_Life = 10.f;
	//总生命值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_TotalLife = 10.f;
};

//卡片属性
USTRUCT(BlueprintType)
struct FCard_Property {
	GENERATED_USTRUCT_BODY()
public:
	//白天或者是夜晚(地图)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_Day = true;
	//白天或者是夜晚(卡片)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_CardDay = true;
	//卡片名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString M_CardName;
	//卡片优先级(一个格子只能存在不同优先级的卡片(只能存在一次))
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_CardLayer = 0;
	//当前卡片所在的路线
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLine M_Line;
	//适应类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELineType M_ELineType;
};

//当卡片被放置时
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCardPlayedDelegate, UObject*, TriggerObject);
//当卡片被激活时
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCardActivatedDelegate, bool&, bCardActiveState, UObject*, TriggerObject);
//当卡片被替换时
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCardReplacedDelegate, UClass*, NewCardResourceClass, const FItemCard&, NewCardData);
//当卡片受到攻击时
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCardAttackedDelegate, float&, AttackValue, UObject*, TriggerObject);
//当卡片回复时
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCardReplyedDelegate, float&, ReplyValue, UObject*, TriggerObject);
//当卡片死亡时
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCardDeathedDelegate, UObject*, TriggerObject);


//卡片对象，基础
UCLASS()
class FVM_API ACardActor : public AGameStartActor, public IMapMesheMovementInterface
{
	GENERATED_BODY()
public:
	//当卡片被放置时
	UPROPERTY(EditAnywhere, BlueprintAssignable)
	FCardPlayedDelegate OnCardPlayedDelegate;
	//当卡片被激活时
	UPROPERTY(EditAnywhere, BlueprintAssignable)
	FCardActivatedDelegate OnCardActivatedDelegate;
	//当卡片被替换时
	UPROPERTY(EditAnywhere, BlueprintAssignable)
	FCardReplacedDelegate OnCardReplacedDelegate;
	//当卡片受到攻击时
	UPROPERTY(EditAnywhere, BlueprintAssignable)
	FCardAttackedDelegate OnCardAttackedDelegate;
	//当卡片回复时
	UPROPERTY(EditAnywhere, BlueprintAssignable)
	FCardReplyedDelegate OnCardReplyedDelegate;
	//当卡片死亡时
	UPROPERTY(EditAnywhere, BlueprintAssignable)
	FCardDeathedDelegate OnCardDeathedDelegate;

public:
	/*
		卡片核心函数
		1.调用优先级，如下排列
		2.放置卡片->激活卡片->[回复,攻击,死亡 并列]
	*/
	//放置卡片
	UFUNCTION(BlueprintCallable)
	virtual	void PlayCard(class UObject* TriggerObject = nullptr);
	//激活卡片
	UFUNCTION(BlueprintCallable)
	virtual void ActiveCard(bool bCardActiveState, UObject* TriggerObject);
	//替换卡片
	UFUNCTION(BlueprintCallable)
	virtual void ReplaceCard(UClass* NewCardResourceClass, const FItemCard& NewCardData);
	//回复卡片
	UFUNCTION(BlueprintCallable)
	virtual bool ReplyCard(float ReplyValue, class UObject* TriggerObject = nullptr);
	//攻击卡片
	UFUNCTION(BlueprintCallable)
	virtual bool AttackCard(float AttackValue, class UObject* TriggerObject = nullptr);
	//销毁卡片
	UFUNCTION(BlueprintCallable)
	virtual void KillCard(class UObject* TriggerObject = nullptr);

public:
	//获取浮动模式
	UFUNCTION(BlueprintPure)
	bool GetbMovement() const;
	//获取卡片原数据，源数据不提供修改的入口，获取的是最基础的数据
	UFUNCTION(BlueprintPure)
	FItemCard GetCardSourceData() const;
	//获取卡片名称
	UFUNCTION(BlueprintPure)
	int32 GetCardID() const;
	//获取当前生命值
	UFUNCTION(BlueprintPure)
	float GetCurrentHP() const;
	//获取总生命值
	UFUNCTION(BlueprintPure)
	float GetTotalHP() const;
	//获取地图是白天还是夜晚
	UFUNCTION(BlueprintPure)
	bool GetMapDay() const;
	//获取卡片是白天还是夜晚,true 白天，false 夜晚
	UFUNCTION(BlueprintPure)
	bool GetCardDay() const;
	//获取卡片是满足死亡条件
	UFUNCTION(BlueprintPure)
	bool GetbCardDeaded() const;
	//获取卡片激活状态
	UFUNCTION(BlueprintPure)
	bool GetbCardActive() const;
	//获取卡片等级
	UFUNCTION(BlueprintPure)
	int32 GetCardGrade() const;
	//获取卡片层级
	UFUNCTION(BlueprintPure)
	int32 GetCardLayer() const;
	//获取线路
	UFUNCTION(BlueprintPure)
	FLine GetLine() const;
	//获取线路类型
	UFUNCTION(BlueprintPure)
	ELineType GetLineType() const;

	//获取当前卡片对应的UI
	UFUNCTION(BlueprintPure)
	UUI_Card* GetCardUI();
	//获取地图网格UI
	UFUNCTION(BlueprintPure)
	UUI_MapMeshe* GetUIMapMesh();
	//获取卡片数据对象
	UFUNCTION(BlueprintPure)
	UCardDataObject* GetCardDataObject();
	//获取盒体碰撞组件
	UFUNCTION(BlueprintPure)
	UBoxComponent* GetBoxComponent();
	//获取当前触发对象
	UFUNCTION(BlueprintPure)
	UObject* GetTriggerObject();
public:
	//设置层级
	virtual	void SetRenderLayer(int32 _Layer) override;

	//设置卡片数据对象
	UFUNCTION(BlueprintCallable)
	void SetCardDataObject(UCardDataObject* CardDataObject);
	//设置当前卡片对应的UI
	UFUNCTION(BlueprintCallable)
	void SetCardUI(UUI_Card* _card_ui);
	//设置线路
	UFUNCTION(BlueprintCallable)
	void SetLine(const FLine& _Line);
	//设置类型和层
	UFUNCTION(BlueprintCallable)
	void SetCardType(const ELineType& _Type, const int32& _CardID);
	//设置卡片的生命值
	UFUNCTION(BlueprintCallable)
	void SetCardHP(const float& ATK_Value, float UpHP_Value);
	//设置卡片当前生命值
	UFUNCTION(BlueprintCallable)
	void SetCardCurrentHP(const float& _HP);
	//设置地图网格UI
	UFUNCTION(BlueprintCallable)
	void SetUIMapMesh(UUI_MapMeshe* _MapMesh);
	//开关碰撞体
	UFUNCTION(BlueprintCallable)
	void SetCollisionEnable(bool bValue);
public:

	ACardActor();

	//更新
	virtual void GameTick(float DeltaSeconds) override;

	/*
			卡片数据初始化时
		1.默认会进行一次最基础的数据初始化
		2.建议在此处只做一些数据上的初始化
	*/
	virtual void OnCardInitData(const FItemCard& NewBaseData);

protected:

	//类开始
	virtual void BeginPlay() override;

	/*
			卡片被放置时
		1.会调用一次放置委托
		2.逻辑执行
	*/
	virtual void OnCardPlayed(class UObject* TriggerObject = nullptr);

	/*
			卡片被激活时
		1.会调用一次激活委托
		PS：本函数不会执行卡片激活的状态设置，只会进行卡片激活数值的修改，
		@Param1 bCardActiveState，可以修改
	*/
	virtual void OnCardActivated(bool& bCardActiveState, class UObject* TriggerObject = nullptr);

	/*
			卡片被替换时
		1.会调用一次替换委托
		2.逻辑执行
	*/
	virtual void OnCardReplaced(UClass* NewCardResourceClass, const FItemCard& NewCardData);

	/*
			卡片受到伤害时
		1.当卡片被攻击时，调用一次被攻击的委托，@Param1 AttackValue 表示此次攻击的伤害，可以修改
		2.逻辑执行
		PS：本函数不会进行伤害结算，只会进行伤害数值修改
	*/
	virtual void OnCardAttacked(float& AttackValue, class UObject* AttackObject = nullptr);

	/*
			卡片受到治愈时
		1.当卡片被治愈时，调用一次被治愈的委托，@Param1 ReplyValue 表示此次治愈的数值，可以修改
		2.逻辑执行
		PS：本函数不会进行治愈结算，只会进行治愈数值修改
	*/
	virtual void OnCardReplyed(float& ReplyValue, class UObject* ReplyObject = nullptr);

	/*
			卡片死亡时
		1.会调用一次卡片死亡委托
		2.逻辑执行
	*/
	virtual void OnCardDeathed(class UObject* TriggerObject = nullptr);


	//初始化卡片等级到地图上
	void InitCardGradeToMap();
	//设置卡片数据
	void SetCardData(const FItemCard& _CardData);
	//设置卡片碰撞的状态
	void SetCardCollisonState(bool bActive);

protected:
	/*
		网格移动-开始【网格移动组件接口C++实现】
		1.当网格移动时，卡片跟随移动
	*/
	//当卡片移动时【网格调用】
	virtual void OnMapMesheMovedUpdate_Implementation(
		const float& DeltaTime,
		const int32& _Layer,
		const FVector& _Location,
		const FLine& _Line
	)override;

	//当卡片开始移动【网格调用】
	virtual void OnMapMesheMoveBegin_Implementation(
		const int32& _Layer,
		const FLine& _Line
	)override;

	//当卡片移动结束时【网格调用】
	virtual void OnMapMesheMoveEnd_Implementation(
		const int32& _Layer,
		const FLine& _Line
	) override;
	/*
		网格移动-结束
	*/

private:
	//编辑卡片价格
	void EditCardPrice(int32 EditValue);

protected:
	//卡片名称
	UPROPERTY()
	int32 CardID = -1;
	//卡片数据
	UPROPERTY()
	FItemCard M_SourceCardDataBase;
	//当前线路
	UPROPERTY()
	FLine CurCheckMesheLine;
	//生命类
	UPROPERTY()
	FCard_HP M_FCardActor_HP;
	//卡片属性
	UPROPERTY()
	FCard_Property M_CardProperty;
	//颜色设置控制变量
	UPROPERTY()
	bool M_bColorSet = false;
	//碰撞是否激活
	UPROPERTY(EditAnywhere)
	bool bCollisionActive = false;
	//当前碰撞状态
	UPROPERTY(EditAnywhere)
	bool bCollisionState = false;
	//更新延迟
	UPROPERTY(EditAnywhere)
	float DelayTime = 0.2f;
	//卡片活动状态
	UPROPERTY(EditAnywhere)
	bool bCardActive = false;
protected:
	//触发对象实例【对当前卡片有一定影响的对象】
	UPROPERTY()
	UObject* TriggerObjectInstance = nullptr;
	//当前卡片所对应的UI
	UPROPERTY()
	UUI_Card* M_UUI_Card = nullptr;
	//当前卡片所在的UI网格
	UPROPERTY()
	UUI_MapMeshe* M_UUI_MapMeshe = nullptr;
	//卡片等级动画
	UPROPERTY()
	UStaticMeshComponent* M_CardGradeStaticMesh = nullptr;
	//游戏地图实例
	UPROPERTY()
	AGameMapInstance* M_AGameMapInstance = nullptr;
	//卡片碰撞
	UPROPERTY()
	UBoxComponent* M_CardTypeBoxCollision = nullptr;
	//卡片数据组件
	UPROPERTY()
	UCardDataObject* CurCardDataComponent = nullptr;
	//卡片管理器
	UPROPERTY()
	class UCardManagerComponent* CardManagerComponent = nullptr;
};