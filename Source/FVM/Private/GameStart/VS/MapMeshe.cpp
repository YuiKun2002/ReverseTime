// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStart/VS/MapMeshe.h"
#include "GameStart/VS/UI/UI_MapMeshe.h"
#include "GameStart/VS/GameMapInstance.h"
#include "GameStart/Flipbook/SpriteActor.h"
#include "GameStart/Flipbook/GameActor/CardActor.h"
#include "GameStart/Flipbook/GameActor/GamePlayer.h"
#include "GameStart/Flipbook/GameActor/MouseActor.h"
#include "GameStart/Flipbook/GameActor/FlyItemActor.h"
#include "GameStart/VS/Components/MapMesheWidgetComponent.h"
#include "GameStart/VS/Components/Item/ShootLineComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include <Components/WidgetComponent.h>
#include <Components/SphereComponent.h>

void AMapMouseMesheManager::BoxOverlapBegin(AActor* OtherActor)
{
	//老鼠处理
	if (Cast<AMouseActor>(OtherActor))
	{
		//如果当前老鼠是这一行的，即可进入
		if (Cast<AMouseActor>(OtherActor)->GetMouseLine().Row == this->M_Line.Row)
		{
			this->M_CurMouses.Emplace(
				UKismetSystemLibrary::GetObjectName(Cast<AMouseActor>(OtherActor)),
				Cast<AMouseActor>(OtherActor)
			);

			Cast<AMouseActor>(OtherActor)->SetMouseLine(this->M_Line);
		}
	}
}

void AMapMouseMesheManager::BoxOverlapEnd(AActor* OtherActor)
{
	//老鼠处理
	if (Cast<AMouseActor>(OtherActor))
	{
		this->M_CurMouses.Remove(UKismetSystemLibrary::GetObjectName(Cast<AMouseActor>(OtherActor)));
	}
}

const TMap<FString, AMouseActor*>& AMapMouseMesheManager::GetCurMouse() const
{
	return this->M_CurMouses;
}

void AMapMouseMesheManager::SetLine(FLine Line)
{
	this->M_Line = Line;
}

// Sets default values
AMapMeshe::AMapMeshe()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//创建格子组件
	this->M_MapMesheWidgetComponent = this->CreateDefaultSubobject<UMapMesheWidgetComponent>("M_UMapMesheWidgetComponent");
	//创建坐标点
	this->M_Point = this->CreateDefaultSubobject<USceneComponent>("M_Point");
	//盒体碰撞
	this->M_MesheBoxComp = this->CreateDefaultSubobject<USphereComponent>("M_SphereComponent");
	//设置根组件
	this->SetRootComponent(this->M_Point);

	this->M_MapMesheWidgetComponent->SetupAttachment(this->M_Point);
	this->M_MesheBoxComp->SetupAttachment(this->M_MapMesheWidgetComponent);

	this->M_MapMesheWidgetComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->M_MapMesheWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->M_MapMesheWidgetComponent->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_GameTraceChannel13,
		ECollisionResponse::ECR_Block
	);
	this->M_MapMesheWidgetComponent->SetBackgroundColor(FLinearColor(FVector(1.f)));
	this->M_MapMesheWidgetComponent->SetTintColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.4f));
	this->M_MapMesheWidgetComponent->SetBlendMode(EWidgetBlendMode::Transparent);
}

void AMapMeshe::InitUUI_MapMesh(const ELineType& _LineType, const int32& _Translucency)
{
	this->SetRenderLayer(_Translucency);

	//网格有效
	if (IsValid(this->M_MapMesheWidgetComponent))
	{
		//初始化网格类型，并且生成对应的网格地形
		this->M_MapMesheWidgetComponent->InitByMesheControllComp(_LineType, _Translucency);
#if WITH_EDITOR
		//做颜色区分
		if (UFVMGameInstance::GetDebug())
		{
			switch (_LineType)
			{
			case ELineType::OnGround:
				this->M_MapMesheWidgetComponent->SetTintColorAndOpacity(FLinearColor(0.f, 1.f, 0.f, 0.4f));
				break;
			case ELineType::OnWater:
				this->M_MapMesheWidgetComponent->SetTintColorAndOpacity(FLinearColor(0.f, 0.f, 0.5f, 0.4f));
				break;
			case ELineType::Underwater:
				this->M_MapMesheWidgetComponent->SetTintColorAndOpacity(FLinearColor(0.f, 0.f, 1.f, 0.4f));
				break;
			case ELineType::Magma:
				this->M_MapMesheWidgetComponent->SetTintColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 0.4f));
				break;
			case ELineType::All:
				this->M_MapMesheWidgetComponent->SetTintColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.4f));
				break;
			}
		}
		else {
			this->M_MapMesheWidgetComponent->SetTintColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.0f));
		}
#endif
		//得到网格，并且设置属性
		UUI_MapMeshe* L_UUI_MapMeshe = Cast<UUI_MapMeshe>(this->M_MapMesheWidgetComponent->GetWidget());

		if (L_UUI_MapMeshe)
		{
			L_UUI_MapMeshe->SetMapMeshe(this);
			L_UUI_MapMeshe->SetLine(this->GetLine());
			L_UUI_MapMeshe->M_ELineType = this->GetLineType();

			this->M_MapMesheWidgetComponent->SetDrawSize(this->M_MeshSize);
		}
		else {
			this->M_MapMesheWidgetComponent->DestroyComponent(true);
		}

		switch (this->GetLineType())
		{
		case ELineType::DisEnable:this->M_MapMesheWidgetComponent->DestroyComponent(true); break;
		case ELineType::OnWater:this->M_PlayerMesheOffset = FVector(0.f, 0.f, 15.f); break;
		default:
			this->M_PlayerMesheOffset = FVector(0.f, 0.f, -5.f);
			break;
		}

		if (this->GetLineType() == ELineType::DisEnable)
		{

		}

		this->UIMapMeshe = this->GetUIMapMeshe();
	}
}

int32 AMapMeshe::GetRenderLayer() const
{
	if (IsValid(this->M_MapMesheWidgetComponent))
	{
		return this->M_MapMesheWidgetComponent->GetTranslucency();
	}

	return Super::GetRenderLayer();
}

void AMapMeshe::SetRenderLayer(int32 Layer)
{
	Super::SetRenderLayer(Layer);

	if (IsValid(this->M_MapMesheWidgetComponent))
	{
		this->M_MapMesheWidgetComponent->SetTranslucency(Layer);
	}
}

void AMapMeshe::SetMesheBg(ASpriteActor* _Bg)
{
	this->M_MesheBG = _Bg;
}

void AMapMeshe::SetMesheMove(float _CloudDownTime, float _Time, FLine _Offset, FVector _TargetLocation)
{
	this->M_bMove = true;
	this->M_BeginMoveingCloudDownTime = _CloudDownTime;
	this->M_MoveingTime = _Time;
	this->M_MoveingOffSet = _Offset;
	this->M_TargetLocation = _TargetLocation;
}

void AMapMeshe::SetLine(const FLine& _Line)
{
	this->M_Type.M_Line = _Line;
}

void AMapMeshe::SetLineType(const ELineType _Type)
{
	this->M_Type.M_ELineType = _Type;

	if (IsValid(this->M_MapMesheWidgetComponent))
	{
		UUI_MapMeshe* L_UUI_MapMeshe = Cast<UUI_MapMeshe>(this->M_MapMesheWidgetComponent->GetWidget());
		if (IsValid(L_UUI_MapMeshe))
		{
			L_UUI_MapMeshe->M_ELineType = _Type;
		}
	}
}

void AMapMeshe::SetWidgetComponentTransform(const FTransform& _ActorTransform)
{
	if (this->M_MapMesheWidgetComponent && this->GetLineType() != ELineType::DisEnable)
	{
		UUI_MapMeshe* L_UUI_MapMeshe = Cast<UUI_MapMeshe>(this->M_MapMesheWidgetComponent->GetWidget());
		if (L_UUI_MapMeshe)
		{
			L_UUI_MapMeshe->M_MapMesheTransform = _ActorTransform;
		}
	}
}

void AMapMeshe::SetUUI_MapMeshRotatonPitchY(const float& _Angle)
{
	if (this->M_MapMesheWidgetComponent)
		this->M_MapMesheWidgetComponent->SetRelativeRotation(FRotator(0.f, _Angle, 0.f));
}

void AMapMeshe::SetPrePlayCard(const TArray<FMesheColPrePlayCard>& Cards)
{
	this->MesheColPrePlayCard = Cards;
}

bool AMapMeshe::GetbUse() const
{
	return this->bUse;
}

bool AMapMeshe::GetbMovementEnabled() const {
	return this->M_bMove;
}

bool AMapMeshe::GetbMovement() const
{
	return this->bMovement;
}

FLine AMapMeshe::GetLine() const
{
	return this->M_Type.M_Line;
}

ELineType AMapMeshe::GetLineType() const
{
	return this->M_Type.M_ELineType;
}

FTransform AMapMeshe::GetWidgetComponentTransform()
{
	if (this->M_MapMesheWidgetComponent)
	{
		UUI_MapMeshe* L_UUI_MapMeshe = Cast<UUI_MapMeshe>(this->M_MapMesheWidgetComponent->GetWidget());
		if (L_UUI_MapMeshe)
		{
			return  L_UUI_MapMeshe->M_MapMesheTransform;
		}
	}

	return FTransform();
}

UUI_MapMeshe* AMapMeshe::GetUIMapMeshe()
{
	if (IsValid(this->UIMapMeshe))
	{
		return this->UIMapMeshe;
	}

	if (IsValid(this->M_MapMesheWidgetComponent))
	{
		this->UIMapMeshe = Cast<UUI_MapMeshe>(this->M_MapMesheWidgetComponent->GetWidget());
		return  this->UIMapMeshe;
	}
	return nullptr;
}

bool AMapMeshe::GetbHasPlayer()
{
	if (IsValid(this->GetUIMapMeshe()))
	{
		return this->GetUIMapMeshe()->GetbHasPlayer();
	}

	return false;
}

bool AMapMeshe::GetMapMesheOccupancy()
{
	if (IsValid(this->GetUIMapMeshe()))
	{
		return this->GetUIMapMeshe()->GetbUIMesheOccupancy();
	}

	if (this->M_MesheDis.Num() != 0)
	{
		return false;
	}

	return true;
}

void AMapMeshe::CopyUIMapMesheData(AMapMeshe* NewMapMeshe, UUI_MapMeshe* NewUIMapMeshe)
{
	//表示当前网格无效，禁用网格【不能复制】
	if (!IsValid(this->GetUIMapMeshe()))
	{
		return;
	}

	//复制网格数据
	//网格禁用
	this->M_MesheDis = NewUIMapMeshe->M_MesheDis;
	//是否可以使用
	this->bUse = NewMapMeshe->bUse;
	//是否可以移动
	this->M_bMove = NewMapMeshe->M_bMove;
	//优先级
	this->SetRenderLayer(NewMapMeshe->GetRenderLayer());

	if (NewMapMeshe->M_bMove)
	{
		//设置移动
		this->SetMesheMove(
			NewMapMeshe->M_BeginMoveingCloudDownTime,
			NewMapMeshe->M_MoveingTime,
			NewMapMeshe->M_MoveingOffSet,
			NewMapMeshe->M_TargetLocation
		);
	}

	//移动时间
	this->M_MoveingTime = NewMapMeshe->M_MoveingTime;
	//移动总时间
	this->M_CurrentMoveingTime = NewMapMeshe->M_CurrentMoveingTime;
	//当前位置
	this->M_Type.M_Line = NewMapMeshe->M_Type.M_Line;
	this->M_CurrentLine = NewMapMeshe->M_CurrentLine;
	//移动偏移
	this->M_MoveingOffSet = NewMapMeshe->M_MoveingOffSet;
	//移动冷却总时间
	this->M_BeginMoveingCloudDownTime = NewMapMeshe->M_BeginMoveingCloudDownTime;
	//当前移动冷却时间
	this->M_CurrentMoveingCloudDownTime = NewMapMeshe->M_CurrentMoveingCloudDownTime;
	//是否已经开始
	this->M_bBegin = NewMapMeshe->M_bBegin;
	//当前坐标
	this->M_CurrentLocation = NewMapMeshe->M_CurrentLocation;
	//目标坐标
	this->M_TargetLocation = NewMapMeshe->M_TargetLocation;

	//复制UI网格
	UUI_MapMeshe* CurUIMapMeshe = this->GetUIMapMeshe();
	CurUIMapMeshe->M_MesheDis.Append(UIMapMeshe->M_MesheDis);
	CurUIMapMeshe->M_UpdateTime = UIMapMeshe->M_UpdateTime;
	CurUIMapMeshe->M_Translucency = UIMapMeshe->M_Translucency;
	CurUIMapMeshe->M_Line = UIMapMeshe->M_Line;
	CurUIMapMeshe->bHasChar = UIMapMeshe->bHasChar;
	CurUIMapMeshe->bMesheEnabled = UIMapMeshe->bMesheEnabled;
	CurUIMapMeshe->bPlayer = UIMapMeshe->bPlayer;

	//判断如果当前网格为、水，岩浆，水中
	//根据类型，排除不符合网格的卡片，如果存在符合网格的底卡则忽略，否则删除
	switch (this->GetLineType())
	{
	case ELineType::OnWater:
	{
		if (UIMapMeshe->GetbHasPlayer())
		{
			//木盘子
			CurUIMapMeshe->CreateFreeCard(96);
		}
	}break;
	case ELineType::Underwater:
	{
		if (UIMapMeshe->GetbHasPlayer())
		{
			//苏打气泡
			CurUIMapMeshe->CreateFreeCard(107);
		}
	}break;
	case ELineType::Magma:
	{
		if (UIMapMeshe->GetbHasPlayer())
		{
			//棉花糖
			CurUIMapMeshe->CreateFreeCard(102);
		}
	}break;
	default: {
	}break;
	}

	//更新卡片的类型
	CurUIMapMeshe->SetCardLayers(UIMapMeshe->M_CardLayers);
	//更新卡片的碰撞
	CurUIMapMeshe->UpdateAllCardsCollision();
	//更新卡片
	this->UpdateCardsLocation(nullptr);
	//更新角色位置
	this->UpdatePlayerLocation();

	//当前卡片拥有集合[绑定移动模式]
	for (auto& CurCard : CurUIMapMeshe->GetCardLayers())
	{
		this->AddGameStartActorMovement(CurCard.Value);
	}
}

void AMapMeshe::UpdatePlayerLocation()
{
	if (IsValid(this->GetUIMapMeshe()) && this->GetUIMapMeshe()->GetbHasPlayer())
	{
		//更新网格
		this->GetUIMapMeshe()->M_PlayerIns->InitMeshe(this->GetUIMapMeshe(), this);

		this->GetUIMapMeshe()->M_PlayerIns->PlayerLocationUpdate(0.f);

		this->GetUIMapMeshe()->M_PlayerIns->UpdateWeaponLocation(0.f);
	}
}

void AMapMeshe::UpdateCardsLocation(ACardActor* SingleCard)
{
	if (!IsValid(this->GetUIMapMeshe()))
	{
		return;
	}
	this->GetUIMapMeshe()->UpdateCardLocation(SingleCard);
}

void AMapMeshe::SetMapMesheOccupancy(bool bEnable, UObject* CurObj)
{
	if (IsValid(this->GetUIMapMeshe()))
	{
		this->GetUIMapMeshe()->SetUIMesheOccupancy(bEnable, CurObj);
	}
	else {
		if (IsValid(CurObj))
		{
			if (bEnable)
			{
				this->M_MesheDis.Remove(UGameSystemFunction::GetObjectName(CurObj));
			}
			else
			{
				this->M_MesheDis.Emplace(UGameSystemFunction::GetObjectName(CurObj), CurObj);
			}
		}
		else {
			TMap<FString, UObject*> Temp;
			for (const auto& Cur : this->M_MesheDis)
			{
				if (IsValid(Cur.Value))
				{
					Temp.Emplace(Cur.Key, Cur.Value);
				}
			}
			this->M_MesheDis.Empty();
			this->M_MesheDis = Temp;
		}
	}
}

TArray<UObject*> AMapMeshe::GetMapMesheOccupancyObjects()
{
	if (IsValid(this->GetUIMapMeshe()))
	{
		return this->GetUIMapMeshe()->GetMapMesheOccupancyObjects();
	}
	else {
		TArray<UObject*> CurObjs;
		for (const auto& Cur : this->M_MesheDis)
		{
			CurObjs.Emplace(Cur.Value);
		}

		return CurObjs;
	}

	return TArray<UObject*>();
}

const TArray<FMesheColPrePlayCard>& AMapMeshe::GetPrePlayCard() const
{
	return this->MesheColPrePlayCard;
}

// Called when the game starts or when spawned
void AMapMeshe::BeginPlay()
{
	Super::BeginPlay();

	this->M_MesheBoxComp->OnComponentBeginOverlap.AddDynamic(this, &AMapMeshe::BoxOverlapBegin);
	this->M_MesheBoxComp->OnComponentEndOverlap.AddDynamic(this, &AMapMeshe::BoxOverlapEnd);
}

void AMapMeshe::MesheMoveBegin()
{
	this->bMovement = true;

	this->GetUIMapMeshe()->SetLine(this->M_Type.M_Line);

	//执行
	for (auto PP = this->MesheColPrePlayCard.CreateConstIterator(); PP; ++PP)
	{
		IMapMesheMovementInterface::Execute_OnMapMesheMoveBegin(
			this,
			this->GetUIMapMeshe()->GetPlayerRenderLayer(),
			this->M_CurrentLine);
	}

	this->M_CurrentLocation = this->GetActorLocation();
	this->M_CurrentLine = this->GetLine();
}

void AMapMeshe::MesheMoveEnd()
{
	this->bMovement = false;

	this->GetUIMapMeshe()->SetLine(this->M_Type.M_Line);

	//执行
	for (auto PP = this->MesheColPrePlayCard.CreateConstIterator(); PP; ++PP)
	{
		IMapMesheMovementInterface::Execute_OnMapMesheMoveEnd(
			this,
			this->GetUIMapMeshe()->GetPlayerRenderLayer(),
			this->M_CurrentLine);
	}


	FVector LocationTemp;
	FLine LineTemp;

	//位置交换
	LocationTemp = this->M_CurrentLocation;
	this->M_CurrentLocation = this->M_TargetLocation;
	this->M_TargetLocation = LocationTemp;

	//线路交换
	LineTemp = this->M_CurrentLine;
	this->M_CurrentLine = this->M_MoveingOffSet;
	this->M_MoveingOffSet = LineTemp;

}

void AMapMeshe::BoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//得到网格
	if (this->M_MapMesheWidgetComponent)
	{
		UUI_MapMeshe* L_UUI_MapMeshe = Cast<UUI_MapMeshe>(this->M_MapMesheWidgetComponent->GetWidget());
		if (L_UUI_MapMeshe)
		{
			L_UUI_MapMeshe->BoxOverlapBeginByAMapMeshe(OtherActor);
		}
	}
}

void AMapMeshe::BoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//得到网格
	if (this->M_MapMesheWidgetComponent)
	{
		UUI_MapMeshe* L_UUI_MapMeshe = Cast<UUI_MapMeshe>(this->M_MapMesheWidgetComponent->GetWidget());
		if (L_UUI_MapMeshe)
		{
			L_UUI_MapMeshe->BoxOverlapEndByAMapMeshe(OtherActor);
		}
	}
}

int32 AMapMeshe::ValueRound(double n)
{
	// 若为负数，则先化为正数再进行四舍五入
	if (n > 0.f)
	{
		return n - int32(n) >= 0.5f ? int32(n) + 1.f : int32(n);
	}
	else
	{
		return -n - int32(-n) >= 0.5f ? -(int32(-n) + 1.f) : -int32(-n);
	}
}

void AMapMeshe::AddGameStartActorMovement(TScriptInterface<IMapMesheMovementInterface> InterfaceIns)
{
	if (this->M_bMove)
	{
		if (!InterfaceIns.GetInterface())
		{
			return;
		}

		this->M_CurMeshCards.Emplace(InterfaceIns);
	}
}

void AMapMeshe::RemoveGameStartActorMovement(TScriptInterface<IMapMesheMovementInterface> InterfaceIns)
{
	if (this->M_bMove)
	{
		this->M_CurMeshCards.Remove(InterfaceIns);
	}
}

// Called every frame
void AMapMeshe::GameTick(float DeltaTime)
{
	Super::GameTick(DeltaTime);

	if (this->M_bMove)
	{
		//开始移动的冷却
		if (!this->M_bBegin)
		{
			this->M_CurrentMoveingCloudDownTime += DeltaTime;
			if (this->M_CurrentMoveingCloudDownTime >= this->M_BeginMoveingCloudDownTime)
			{
				//开始移动
				this->MesheMoveBegin();
				this->M_bBegin = true;
			}
		}

		//开始移动更新
		if (this->M_bBegin)
		{
			//移动时间长度
			this->M_CurrentMoveingTime += DeltaTime;
			if (this->M_CurrentMoveingTime >= this->M_MoveingTime)
			{
				//移动结束
				this->MesheMoveEnd();

				//UKismetMathLibrary::VLerp(A, B, FlyTime);

				this->M_bBegin = false;
				this->M_CurrentMoveingTime = 0.f;
				this->M_CurrentMoveingCloudDownTime = 0.f;
			}


			float Value = this->M_CurrentMoveingTime / this->M_MoveingTime;
			if (Value >= 1.f)
			{
				Value = 1.f;
			}

			//插值法->时间网格移动
			int32 Row = this->ValueRound(UKismetMathLibrary::Lerp(this->M_CurrentLine.Row, this->M_MoveingOffSet.Row, Value));
			int32 Col = this->ValueRound(UKismetMathLibrary::Lerp(this->M_CurrentLine.Col, this->M_MoveingOffSet.Col, Value));
			this->M_Type.M_Line.Row = Row;
			this->M_Type.M_Line.Col = Col;

			this->GetUIMapMeshe()->SetLine(this->M_Type.M_Line);

			this->SetActorLocation(
				UKismetMathLibrary::VLerp(this->M_CurrentLocation, this->M_TargetLocation, Value)
			);

			//设置背景跟随移动
			if (IsValid(this->M_MesheBG))
			{
				this->M_MesheBG->SetSpriteLocation(this->GetActorLocation(), true);
			}

			//卡片移动更新
			for (auto PP = this->MesheColPrePlayCard.CreateConstIterator(); PP; ++PP)
			{
				IMapMesheMovementInterface::Execute_OnMapMesheMovedUpdate(
					this,
					DeltaTime,
					this->GetUIMapMeshe()->GetPlayerRenderLayer(),
					this->GetActorLocation(),
					this->GetLine());
			}

		}

	}

	//更新网格
	if (this->M_MapMesheWidgetComponent)
	{
		if (Cast<UUI_MapMeshe>(this->M_MapMesheWidgetComponent->GetWidget()))
		{
			Cast<UUI_MapMeshe>(this->M_MapMesheWidgetComponent->GetWidget())->TickMapMeshe(DeltaTime);
		}
	}

}


