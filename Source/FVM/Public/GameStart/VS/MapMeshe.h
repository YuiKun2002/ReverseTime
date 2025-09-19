// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStart/VS/MapMesheInterface.h"
#include "GameSystem/GameMapStructManager.h"
#include "GameStart/Flipbook/GameStartActorBase.h"
#include "MapMeshe.generated.h"

class AMapMeshe;
class ACardActor;
class AMouseActor;
class UUI_MapMeshe;
class ASpriteActor;
class UWidgetComponent;
class USphereComponent;
class UMapMesheWidgetComponent;

USTRUCT(BlueprintType)
struct FAMapMeshType {
	GENERATED_USTRUCT_BODY()
public:
	//当前线路
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLine M_Line;
	//格子类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELineType M_ELineType;
};


//老鼠网格管理器【只会管理老鼠】
UCLASS()
class FVM_API AMapMouseMesheManager : public AActor
{
	GENERATED_BODY()
public:
	//盒体重叠开始
	UFUNCTION(BlueprintCallable)
	void BoxOverlapBegin(AActor* OtherActor);
	//盒体重叠结束
	UFUNCTION(BlueprintCallable)
	void BoxOverlapEnd(AActor* OtherActor);
public:
	const TMap<FString, AMouseActor*>& GetCurMouse() const;
public:
	//设置线路
	void SetLine(FLine Line);
private:
	//当前的线路
	UPROPERTY()
	FLine M_Line;
	//当前网格拥有的老鼠集合
	UPROPERTY()
	TMap<FString, AMouseActor*> M_CurMouses;
};

//地图网格
UCLASS()
class FVM_API AMapMeshe : public AGameStartActorBase
{
	GENERATED_BODY()

	friend class UUI_MapMeshe;

public:
	//界面
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UMapMesheWidgetComponent* M_MapMesheWidgetComponent = nullptr;
	//坐标（点）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	USceneComponent* M_Point = nullptr;
	//一些属性类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	FAMapMeshType M_Type;
	//板块偏移量【水中偏移，陆地偏移，水上偏移等】
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	FVector M_PlayerMesheOffset;
	//网格的背景
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	ASpriteActor* M_MesheBG = nullptr;
	//盒体碰撞
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SphereComp")
	USphereComponent* M_MesheBoxComp = nullptr;
	//格子的大小
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIMeshSize")
	FVector2D M_MeshSize = FVector2D::ZeroVector;
public:

	AMapMeshe();
	// Called every frame
	virtual void GameTick(float DeltaTime) override;
	//获取排序优先级
	virtual int32 GetRenderLayer() const override;
	//设置排序优先级
	virtual void SetRenderLayer(int32 Layer) override;
	//初始化网格
	void InitUUI_MapMesh(const ELineType& _LineType, const int32& _Translucency);
	//更新卡片位置
	UFUNCTION(BlueprintCallable)
	virtual	void UpdateCardsLocation(ACardActor* SingleCard = nullptr);
	//复制数据
	UFUNCTION(BlueprintCallable)
	void CopyUIMapMesheData(AMapMeshe* NewMapMeshe, UUI_MapMeshe* NewUIMapMeshe);
	//更新角色位置
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerLocation();
	//盒体重叠开始
	UFUNCTION()
	void BoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//盒体重叠结束
	UFUNCTION()
	void BoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	//添加游戏对象移动
	UFUNCTION()
	void AddGameStartActorMovement(TScriptInterface<IMapMesheMovementInterface> InterfaceIns);
	//移除游戏对象移动
	UFUNCTION()
	void RemoveGameStartActorMovement(TScriptInterface<IMapMesheMovementInterface> InterfaceIns);
public:
	//设置网格背景
	UFUNCTION(BlueprintCallable)
	void SetMesheBg(ASpriteActor* _Bg);
	//设置网格移动(每次移动的冷却时间，每次移动到目标的总时间，格子偏移，坐标)
	UFUNCTION(BlueprintCallable)
	void SetMesheMove(float _CloudDownTime, float _Time, FLine _Offset, FVector _TargetLocation);
	//设置路线
	UFUNCTION(BlueprintCallable)
	void SetLine(const FLine& _Line);
	//设置当前格子类型
	UFUNCTION(BlueprintCallable)
	void SetLineType(const ELineType _Type);
	//设置UI组件的位置
	UFUNCTION(BlueprintCallable)
	void SetWidgetComponentTransform(const FTransform& _ActorTransform);
	//设置UI组件旋转
	UFUNCTION(BlueprintCallable)
	void SetUUI_MapMeshRotatonPitchY(const float& _Angle);
	//设置预备放置卡片
	UFUNCTION(BlueprintCallable)
	void SetPrePlayCard(const TArray<FMesheColPrePlayCard>& Cards);
	//设置网格的使用
	UFUNCTION(BlueprintCallable)
	void SetMapMesheOccupancy(bool bEnable, UObject* CurObj);
public:
	//获取当前板块是否可以使用
	UFUNCTION(BlueprintPure)
	bool GetbUse() const;
	//获取地图网格是否开启移动模式
	UFUNCTION(BlueprintPure)
	bool GetbMovementEnabled() const;
	//获取板块是否正在移动
	UFUNCTION(BlueprintPure)
	bool GetbMovement() const;
	//获取角色
	UFUNCTION(BlueprintPure)
	bool GetbHasPlayer();
	//获取当前网格是否启用
	UFUNCTION(BlueprintPure)
	bool GetMapMesheOccupancy();

	//获取路线
	UFUNCTION(BlueprintPure)
	FLine GetLine() const;
	UFUNCTION(BlueprintPure)
	ELineType GetLineType() const;
	//'获取UI组件的位置
	UFUNCTION(BlueprintPure)
	FTransform GetWidgetComponentTransform();
	//获取UImapmeshe
	UFUNCTION(BlueprintPure)
	UUI_MapMeshe* GetUIMapMeshe();

	//获取禁用的网格
	UFUNCTION(BlueprintPure)
	TArray<UObject*> GetMapMesheOccupancyObjects();
	//获取预备放置卡片
	UFUNCTION(BlueprintPure)
	const TArray<FMesheColPrePlayCard>& GetPrePlayCard() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//开始网格移动
	void MesheMoveBegin();
	//开始网格移动
	void MesheMoveEnd();
private:
	//四舍五入
	int32 ValueRound(double n);
private:
	//是否可以使用
	UPROPERTY()
	bool bUse = true;
	//是否可以移动
	UPROPERTY()
	bool M_bMove = false;
	//是否在移动
	UPROPERTY()
	bool bMovement = false;
	//移动时间
	UPROPERTY()
	float M_MoveingTime = 0.f;
	//移动总时间
	UPROPERTY()
	float M_CurrentMoveingTime = 0.f;
	//移动冷却总时间
	UPROPERTY()
	float M_BeginMoveingCloudDownTime = 0.f;
	//当前移动冷却时间
	UPROPERTY()
	float M_CurrentMoveingCloudDownTime = 0.f;
	//是否已经开始
	UPROPERTY()
	bool M_bBegin = false;
	//当前位置
	UPROPERTY()
	FLine M_CurrentLine;
	//移动偏移
	UPROPERTY()
	FLine M_MoveingOffSet;
	//当前坐标
	UPROPERTY()
	FVector M_CurrentLocation;
	//目标坐标
	UPROPERTY()
	FVector M_TargetLocation;

	//当前网格拥有的可移动的对象集合
	UPROPERTY()
	TArray<TScriptInterface<IMapMesheMovementInterface>> M_CurMeshCards;
	//网格列预放置卡片
	UPROPERTY()
	TArray<FMesheColPrePlayCard> MesheColPrePlayCard;
	//占用道具，禁用当前网格
	UPROPERTY()
	TMap<FString, UObject*> M_MesheDis;

	//UI网格
	UPROPERTY()
	UUI_MapMeshe* UIMapMeshe = nullptr;
};
