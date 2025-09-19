// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStart/Flipbook/GameActor/FlyItemActor.h"
#include "GameStart/Flipbook/GameActor/MouseActor.h"
#include "GameStart/VS/Components/ResourceManagerComponent.h"
#include "GameStart/VS/Components/Item/ShootLineComponent.h"
#include "GameStart/VS/Components/MesheControllComponent.h"
#include "GameStart/VS/MapMeshe.h"
#include "GameStart/VS/GameMapInstance.h"
#include "GameSystem/GameMapStructManager.h"
#include "GameSystem/FVMGameInstance.h"
#include <Components/SphereComponent.h>
#include <Kismet/GameplayStatics.h>


UActorComponent* const AFlyItemActor::GetActor_CardFunction_Component()
{
	return this->M_AActorComponent_CardFunction;
}

void AFlyItemActor::SetActor_CardFunction_Component(UActorComponent* _Value)
{
	this->M_AActorComponent_CardFunction = _Value;
}

AFlyItemActor::AFlyItemActor()
{
	this->M_SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	this->M_SphereCollision->SetupAttachment(this->GetRootComponent());

	//绑定事件
	this->M_SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AFlyItemActor::OnBoxOverlapBegin);
	this->M_SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AFlyItemActor::OnBoxOverlapEnd);
	//绑定播放完成事件
	this->GetRenderComponent()->OnAnimationComplete.Unbind();
	this->GetRenderComponent()->OnAnimationComplete.BindUObject(this, &AFlyItemActor::AnimComplete);
}

void AFlyItemActor::InitCollision()
{
	this->M_SphereCollision->SetCollisionObjectType(ECC_Pawn);
	this->M_SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->M_SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	this->M_SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AFlyItemActor::BeginPlay()
{
	Super::BeginPlay();

	this->MesheControll = AGameMapInstance::GetGameMapMesheControllManagerStatic();

	//初始化碰撞方式
	this->InitCollision();
	this->InitRotation();
	this->AddActorLocalOffset(FVector(-4.f, 0.f, 0.f));
	//设置渲染层8000
	this->GetRenderComponent()->SetTranslucency(8000);
	this->CurReturnTime = this->ReturnTime;
}

void AFlyItemActor::GameTick(float DeltaTime)
{
	Super::GameTick(DeltaTime);

	if (this->bReturnPool)
	{
		return;
	}

	if (this->ReturnTime == 0.f)
	{
		return;
	}

	if (this->CurReturnTime > 0.f)
	{
		this->CurReturnTime -= DeltaTime;
	}
	else {
		this->CurReturnTime = this->ReturnTime;
		this->ReturnPool();
	}
}

void AFlyItemActor::PoolCreate(class UObjectPoolManager* PoolManager)
{
	this->CurPoolManager = PoolManager;
	this->SetActorLocation(FVector(-1000.f, 0.f, 0.f));
	this->SetActorHiddenInGame(true);
	this->SetActorTickEnabled(false);
	this->bReturnPool = true;
	this->OnReturn();
}

void AFlyItemActor::PoolInit(class UObjectPoolManager* PoolManager)
{
	this->GetRenderComponent()->CancelAnimationPlayEndDestroy();
	this->SetActorHiddenInGame(false);
	this->SetActorTickEnabled(true);
	this->bReturnPool = false;
	this->SetTargetHitState(false);
	this->M_CurrentHitObjectActor = nullptr;
	this->bFirstHitResult = false;
	this->OnPoolInit();
}

bool AFlyItemActor::ReturnPool()
{
	if (!this->bReturnPool)
	{
		this->bReturnPool = true;
	}
	else
	{
		return true;
	}

	this->M_ObjectActorLocation = nullptr;
	this->M_CurrentHitObjectActor = nullptr;
	this->M_LastHitObjectActor = nullptr;
	this->bFirstHitResult = false;
	this->M_AActorComponent_CardFunction = nullptr;
	this->CurReturnTime = this->ReturnTime;
	this->SetPlayAnimation(nullptr);
	this->SetActor_CardFunction_Component(nullptr);
	this->SetTargetHitState(false);
	this->SetActorLocation(FVector(-1000.f, 0.f, 0.f));
	this->SetActorHiddenInGame(true);
	this->SetActorTickEnabled(false);

	//关闭碰撞
	this->SetCollisionEnabled(false);

	if (IsValid(this->CurPoolManager))
	{
		this->OnReturn();

		return this->CurPoolManager->ReturnObjectActor(this);
	}
	else {
		this->Destroy();
	}

	return false;
}

void AFlyItemActor::AnimComplete()
{
	this->ReturnPool();
}

void AFlyItemActor::Init()
{
	this->SetFloatModeEnable(false);

	//初始化动画资源
	this->M_FlyItem_Property_AnimRes.M_Begin =
		UGameSystemFunction::LoadRes(this->M_FlyItem_Property_AnimRes.M_FlyItemAnim_Fly);

	this->M_FlyItem_Property_AnimRes.M_End =
		UGameSystemFunction::LoadRes(this->M_FlyItem_Property_AnimRes.M_FlyItemAnim_Split);

	//播放BGM
	UFVMGameInstance::PlayBGM_S_Static(
		this->M_FlyItem_Property_AudioBegin.M_AudioName,
		this->M_FlyItem_Property_AudioBegin.M_AudioRootPathName
	);

	//播放Split动画
	if (this->M_FlyItem_Property_AnimRes.M_Begin)
	{
		this->GetRenderComponent()->SetFlipbook(this->M_FlyItem_Property_AnimRes.M_Begin);
	}

	this->M_AActorComponent_CardFunction = nullptr;
	this->SetTargetHitState(false);

	//获取指定的组件
	for (UActorComponent* Comp : this->GetComponents())
	{
		if (Cast<UShootLineComponent>(Comp))
		{
			//如果是浮动模式则不受行限制
			if (this->M_bFloatMode)
			{
				this->SetFlyConstraintLine(false);
			}

			break;
		}
	}

	this->OnInit();
	this->SetCollisionEnabled(true);
}

void AFlyItemActor::SetFloatModeEnable(bool _Value)
{
	this->M_bFloatMode = _Value;
}

void AFlyItemActor::SetFlyConstraintLine(bool _value)
{
	this->M_FlyCondition.M_bConstraintLine = _value;
}

void AFlyItemActor::SetFlyInfluence(bool _value)
{
	this->M_FlyCondition.M_bInfluence = _value;
}

void AFlyItemActor::SetATK(float _value)
{
	this->M_FlyData.ATK = _value;
	this->M_FlyData.CurATK = _value;
}

void AFlyItemActor::SetCurATK(float _value)
{
	this->M_FlyData.CurATK = _value;
}

void AFlyItemActor::SetSecondATK(float _value)
{
	this->M_FlyData._SecondATK = _value;
}

void AFlyItemActor::SetLine(int32 _line)
{
	this->M_FlyData.M_Line = _line;
}

void AFlyItemActor::SetFlyItemLine(FLine NewLine)
{
	this->M_FlyData.M_LineT = NewLine;
	this->SetLine(NewLine.Row);
}

FVector AFlyItemActor::GetObjectActorLocation()
{
	if (!IsValid(this->M_ObjectActorLocation))
	{
		return AGameMapInstance::GetGameMapInstance()->
			GetMesheControllComponent()->GetMapMeshCurrentRight(
				this->GetLine()
			)->GetActorLocation();
	}

	//如果位置存在
	if (Cast<AMouseActor>(this->M_ObjectActorLocation)->M_MousePosition)
	{
		return  Cast<AMouseActor>(this->M_ObjectActorLocation)->M_MousePosition->GetComponentLocation();
	}
	else
	{
		return this->M_ObjectActorLocation->GetActorLocation();
	}
}

FGameBuffInfor& AFlyItemActor::GetBuff()
{
	return this->M_Buff;
}

AActor* const AFlyItemActor::GetObjectActor()
{
	return this->M_ObjectActorLocation;
}

AActor* const AFlyItemActor::GetLastObjectActor()
{
	return this->M_LastHitObjectActor;
}

TArray<ELineType>& AFlyItemActor::GetAttackType()
{
	return this->M_AttackType;
}

bool AFlyItemActor::GetDidTargetIsHit()
{
	return this->M_FlyCondition.M_bHitTarget;
}

void AFlyItemActor::SetCollisionEnabled(bool _Value)
{
	if (_Value)
		this->M_SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	else
		this->M_SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AFlyItemActor::SetReturnPoolTime(float Time /*= 0.f*/)
{
	this->ReturnTime = Time;
	this->CurReturnTime = Time;
}

void AFlyItemActor::SetFlyParentActor(AActor* NewParent)
{
	this->ParentActor = NewParent;
}

void AFlyItemActor::PlayAnim_Fly()
{
	//播放Fly动画
	if (this->M_FlyItem_Property_AnimRes.M_Begin)
	{
		this->GetRenderComponent()->SetPlayAnimation(this->M_FlyItem_Property_AnimRes.M_Begin);
	}
}

void AFlyItemActor::PlayAnim_Split()
{
	//关闭碰撞
	this->SetCollisionEnabled(false);

	if (!this->M_FlyItem_Property_AnimRes.M_End)
	{
		this->ReturnPool();
		return;
	}

	//播放Split动画
	this->GetRenderComponent()->SetPlayAnimation(this->M_FlyItem_Property_AnimRes.M_End);
}

void AFlyItemActor::HitEnd(UPrimitiveComponent* _UBoxComp)
{

}

void AFlyItemActor::Hit()
{
	//执行蓝图层的事件
	this->OnHit(this->M_FlyCondition.M_FlyItemAttackType);

	this->bFirstHitResult = true;

	//播放BGM
	UFVMGameInstance::PlayBGM_S_Static(
		this->M_FlyItem_Property_AudioEnd.M_AudioName,
		this->M_FlyItem_Property_AudioEnd.M_AudioRootPathName
	);

	//如果不是穿透类型
	if (this->M_FlyCondition.M_FlyItemAttackType != EFlyItemAttackType::Panetrate)
	{
		//创建静态飞行物对象
		for (auto ResPath_C : this->M_StaticFlyItemClass)
		{
			this->CreateStaticItem(ResPath_C);
		}
	}
	else {
		//如果是穿透类型
		if (!this->bCreateStaticItemOnceOverride)
		{
			//创建静态飞行物对象
			for (auto ResPath_C : this->M_StaticFlyItemClass)
			{
				this->CreateStaticItem(ResPath_C);
			}
		}
	}

	//如果子弹类型不是穿透类型
	if (this->M_FlyCondition.M_FlyItemAttackType != EFlyItemAttackType::Panetrate)
	{
		if (this->M_FlyCondition.M_FlyItemAttackType != EFlyItemAttackType::Sputtering)
		{
			//设置击中状态
			this->SetTargetHitState(true);
		}

		//播放Split动画
		this->PlayAnim_Split();

		//如果是静态对象则直接返回
		if (this->M_FlyCondition.M_bStaticFlyItem)
		{
			this->ReturnPool();
			return;
		}
		else {
			this->SetAnimationPlayEndDestroy();
		}
	}
}

void AFlyItemActor::CreateStaticItem(TSoftClassPtr<AFlyItemActor> CurFlyItemActor)
{
	//获取资源管理器
	UResourceManagerComponent* CurComp = AGameMapInstance::GetGameMapResourceManagerStatic();

	//获取对象池
	UObjectPoolManager* CurPool = nullptr;
	CurPool = CurComp->GetStaticPool(UGameSystemFunction::GetObjectName(this));

	//对象池无效，重新生成对象池
	if (!IsValid(CurPool))
	{
		//生成对象池
		CurPool = UObjectPoolManager::MakePoolManager(this->GetWorld(), CurFlyItemActor, 1);
		//添加对象池
		CurComp->AddNewStaticPool(UGameSystemFunction::GetObjectName(this), CurPool);
	}

	AFlyItemActor* _TargetActor = Cast<AFlyItemActor>(CurPool->GetObjectActor());

	FTransform Trans;
	Trans.SetLocation(
		FVector(
			this->GetActorLocation().X,
			this->GetActorLocation().Y,
			AGameMapInstance::GetGameMapInstance()->
			GetMesheControllComponent()->
			GetMapMeshCurrentRight(
				this->GetLine()
			)->GetActorLocation().Z
		)
	);

	_TargetActor->M_FlyCondition.M_bStaticFlyItem = true;
	_TargetActor->SetActorTransform(Trans);
	_TargetActor->SetATK(this->GetSecondATK());
	_TargetActor->SetSecondATK(this->GetSecondATK());
	_TargetActor->Init();
}

bool AFlyItemActor::GetFlyOutsideAutoDestroy()
{
	return this->M_OutsideAutoDestroy;
}

bool AFlyItemActor::GetFlyConstraintLine()
{
	return this->M_FlyCondition.M_bConstraintLine;
}

float AFlyItemActor::GetATK()
{
	return this->M_FlyData.ATK;
}

float AFlyItemActor::GetCurATK()
{
	return this->M_FlyData.CurATK;
}

float AFlyItemActor::GetSecondATK()
{
	return this->M_FlyData._SecondATK;
}

int32 AFlyItemActor::GetLine()
{
	return this->M_FlyData.M_Line;
}

bool AFlyItemActor::GetFlyInfluence()
{
	return this->M_FlyCondition.M_bInfluence;
}

void AFlyItemActor::CreateFlyActor_ShootLine(
	TSoftClassPtr<AFlyItemActor> _FlyActorPath_C,
	FTargetNode _Node,
	int32 _LineOffset,
	float _Time,
	FVector Offset,
	bool _IsbConstaintLine,
	FString ObjPoolID,
	bool bUseLineHight
)
{
	//线路判断 不能小于0 不能大于最大值
	if (this->M_FlyData.M_Line + _LineOffset < 0 ||
		this->M_FlyData.M_Line + _LineOffset >=
		this->MesheControll->
		GetGameMapStructManager()->
		GameMapStruct.M_Meshe.Num()
		)
	{
		return;
	}

	//获取资源管理器
	UResourceManagerComponent* CurComp = AGameMapInstance::GetGameMapResourceManagerStatic();

	//获取对象池
	UObjectPoolManager* CurPool = nullptr;
	CurPool = CurComp->GetStaticPool(ObjPoolID);

	//对象池无效，重新生成对象池
	if (!IsValid(CurPool))
	{
		//生成对象池
		CurPool = UObjectPoolManager::MakePoolManager(this->GetWorld(), _FlyActorPath_C, 1);
		//添加对象池
		CurComp->AddNewStaticPool(ObjPoolID, CurPool);
	}

	AFlyItemActor* _TargetActor = Cast<AFlyItemActor>(CurPool->GetObjectActor());


	//坐标
	FTransform Trans = this->GetTransform();
	Trans.SetLocation(Trans.GetLocation() + Offset);
	_TargetActor->SetActorTransform(Trans);
	_TargetActor->SetObjectActorLocation(this->M_ObjectActorLocation);
	_TargetActor->SetATK(this->M_FlyData.ATK);
	_TargetActor->SetSecondATK(this->M_FlyData._SecondATK);
	_TargetActor->SetLine(this->M_FlyData.M_Line + _LineOffset);
	_TargetActor->SetFlyConstraintLine(_IsbConstaintLine);
	_TargetActor->Init();

	//使用行高
	if (bUseLineHight && IsValid(AGameMapInstance::GetGameMapInstance()))
	{
		float CurZ = this->MesheControll->GetMapMeshLocation(
			this->M_FlyData.M_Line + _LineOffset,
			0
		).Z;
		float CurFlyZ = this->GetActorLocation().Z;

		// -  往上
		if (_LineOffset < 0)
		{
			if (CurZ > CurFlyZ)
			{
				_Node.M_Hight = (CurZ - CurFlyZ) * 1.f;
			}
			else {
				_Node.M_Hight = (CurFlyZ - CurZ) * -1.f;
			}
		}
		else if (_LineOffset > 0) {
			// + 往下

			if (CurFlyZ > CurZ)
			{
				_Node.M_Hight = (CurFlyZ - CurZ) * -1.f;
			}
			else {
				_Node.M_Hight = (CurZ - CurFlyZ) * 1.f;
			}
		}
		else {
			_Node.M_Hight = CurZ - CurFlyZ;
		}
	}

	//通过node节点更具Direction设置Pitch旋转
	switch (_Node.M_EShootDirection)
	{
	case EShootDirection::EUp: { _TargetActor->SetFlipbookPitchRotation(90.f); } break;
	case EShootDirection::EDown: { _TargetActor->SetFlipbookPitchRotation(-90.f); }break;
	case EShootDirection::ELeft: {
		_TargetActor->SetFlipbookPitchRotation(180.f);
		_TargetActor->GetMyActor()->SetRelativeLocation(_TargetActor->GetMyActor()->GetRelativeLocation() * -1.f);
	}break;
	default:; break;
	}

	UShootLineComponent* CurSComp = Cast<UShootLineComponent>(
		_TargetActor->GetComponentByClass(UShootLineComponent::StaticClass())
	);

	if (IsValid(CurSComp))
	{
		CurSComp->SetTargetNode(_Node);
		CurSComp->SetMoveDirection(_Node.M_EShootDirection);

		//如果是浮动模式则不受行限制
		if (this->M_bFloatMode)
		{
			_TargetActor->SetFlyConstraintLine(false);
		}
	}
}

void AFlyItemActor::CreateFlyActor_ShootLine_Slash(
	TSoftClassPtr<AFlyItemActor> _FlyActorPath_C,
	float _RotationAngle,
	float _Time,
	FVector Offset,
	FString ObjPoolID
)
{
	//获取资源管理器
	UResourceManagerComponent* CurComp = AGameMapInstance::GetGameMapResourceManagerStatic();

	//获取对象池
	UObjectPoolManager* CurPool = nullptr;
	CurPool = CurComp->GetStaticPool(ObjPoolID);

	//对象池无效，重新生成对象池
	if (!IsValid(CurPool))
	{
		//生成对象池
		CurPool = UObjectPoolManager::MakePoolManager(this->GetWorld(), _FlyActorPath_C, 1);
		//添加对象池
		CurComp->AddNewStaticPool(ObjPoolID, CurPool);
	}

	AFlyItemActor* _TargetActor = Cast<AFlyItemActor>(CurPool->GetObjectActor());

	//坐标
	FTransform Trans = this->GetTransform();
	Trans.SetLocation(Trans.GetLocation() + Offset);

	//新生成的对象设置自定义拥有者(CardActor)
	_TargetActor->SetActorTransform(Trans);
	_TargetActor->SetObjectActorLocation(this->M_ObjectActorLocation);
	_TargetActor->SetATK(this->M_FlyData.ATK);
	_TargetActor->SetSecondATK(this->M_FlyData._SecondATK);
	_TargetActor->SetFlyConstraintLine(false);
	_TargetActor->AddActorLocalOffset(Offset);
	_TargetActor->SetActorRelativeRotation(FRotator(0.f, 0.f, _RotationAngle));
	_TargetActor->Init();

	FTargetNode Node;
	Node.M_Speed = 4.f;
	Node.M_EShootDirection = EShootDirection::ERotation;
	//获取指定的组件
	for (UActorComponent* Comp : _TargetActor->GetComponents())
	{
		if (Cast<UShootLineComponent>(Comp))
		{
			Cast<UShootLineComponent>(Comp)->SetTargetNode(Node);
			break;
		}
	}
}

void AFlyItemActor::SetFlipbook(TSubclassOf<UPaperFlipbook>& _Res)
{
	this->GetRenderComponent()->SetFlipbook(_Res.GetDefaultObject());
}

AFlyItemActor* AFlyItemActor::FlyItemActorSwap(AFlyItemActor* _FlyActor)
{
	if (IsValid(_FlyActor))
	{
		//新生成的对象设置自定义拥有者(CardActor)
		_FlyActor->M_AttackType = this->M_AttackType;
		_FlyActor->M_ObjectActorLocation = this->M_ObjectActorLocation;
		_FlyActor->M_FlyData.ATK = this->M_FlyData.ATK;
		_FlyActor->M_FlyData.CurATK = this->M_FlyData.CurATK;
		_FlyActor->M_FlyData._SecondATK = this->M_FlyData._SecondATK;
		_FlyActor->M_FlyData.M_Line = this->M_FlyData.M_Line;
		_FlyActor->M_LastHitObjectActor = this->M_LastHitObjectActor;
		_FlyActor->SetFlyConstraintLine(this->M_FlyCondition.M_bConstraintLine);
		return  _FlyActor;
	}

	return nullptr;
}

void AFlyItemActor::UpdateRatation360(float _deltaTime, USceneComponent* _Comp)
{

	if (_Comp->GetComponentRotation().Pitch >= 360.f)
	{
		_Comp->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	}

	_Comp->AddLocalRotation(FRotator(1.f * _deltaTime, 90.f, 0.f));

}

void AFlyItemActor::HitMouse_OverlapBegin(AActor* _Mouse)
{
	//老鼠对象
	if (AMouseActor* Mouse = Cast<AMouseActor>(_Mouse))
	{
		if (this->M_LastHitObjectActor == Mouse)
		{
			return;
		}

		if (Mouse->GetCurrentHP() < 0.f)
		{
			return;
		}

		//判断是（约束本行）并且 当前行和老鼠行不匹配
		if (this->GetFlyConstraintLine() && Mouse->GetMouseLine().Row != this->GetLine())
		{
			return;
		}

		//非静态对象
		if (!this->M_FlyCondition.M_bStaticFlyItem)
		{
			//当前用于老鼠对象则跳出
			if (this->M_CurrentHitObjectActor)
			{
				return;
			}
		}


		bool _bHit = false;
		if (Mouse->GetMouseLineType() == ELineType::All)
		{
			_bHit = true;
		}
		else {
			//判断是否可以击中当前老鼠
			for (auto& Type : this->GetAttackType())
			{
				if (
					Type == ELineType::All
					||
					Mouse->GetMouseLineType() == Type)
				{
					_bHit = true;
					break;
				}
				else {
					switch (Type)
					{
					case ELineType::SkyAndGround:
					{
						if (
							Mouse->GetMouseLineType() == ELineType::OnGround
							||
							Mouse->GetMouseLineType() == ELineType::Sky
							)
						{
							_bHit = true;
							break;
						}
					}break;
					default:break;
					}

					//击中则跳出
					if (_bHit)
					{
						break;
					}
				}
			}
		}

		if (_bHit)
		{
			float bCurHP = Mouse->GetCurrentHP();
			//解析Buff信息
			Mouse->ParseBuff_Information(this->M_Buff, this);
			//被命中传入伤害数值
			if (Mouse->BeHit(this, this->GetCurATK(), this->M_FlyCondition.M_FlyItemAttackType))
			{
				if (this->M_FlyCondition.M_FlyItemAttackType != EFlyItemAttackType::Panetrate)
				{
					//设置当前击中的老鼠
					this->M_CurrentHitObjectActor = Mouse;
				}
				else {
					if (this->M_FlyCondition.PanetrateLayers > 0)
					{
						this->M_FlyCondition.PanetrateLayers -= 1;
						if (this->M_FlyCondition.PanetrateLayers == 0)
						{
							this->M_FlyCondition.M_FlyItemAttackType = EFlyItemAttackType::Def;
						}
					}
				}
				//上一次击中的老鼠
				this->M_LastHitObjectActor = Mouse;
				//飞行物击中
				this->Hit();
				//设置老鼠状态->被击中
				Mouse->SetbIsHurt(true);
			}
			else {
				//执行了BeHit修改了血量后，返回false，此时判断是否是最后一击
				if (Mouse->GetMouseIsDeath() && bCurHP > 0.f)
				{
					if (this->M_FlyCondition.M_FlyItemAttackType != EFlyItemAttackType::Panetrate)
					{
						//设置当前击中的老鼠
						this->M_CurrentHitObjectActor = Mouse;
					}
					else {
						if (this->M_FlyCondition.PanetrateLayers > 0)
						{
							this->M_FlyCondition.PanetrateLayers -= 1;
							if (this->M_FlyCondition.PanetrateLayers == 0)
							{
								this->M_FlyCondition.M_FlyItemAttackType = EFlyItemAttackType::Def;
							}
						}
					}
					//上一次击中的老鼠
					this->M_LastHitObjectActor = Mouse;
					//飞行物击中
					this->Hit();
					//设置老鼠状态->被击中
					Mouse->SetbIsHurt(true);

					//创建静态飞行物对象
					for (auto ResPath_C : this->M_TargetDeathStaticFlyItemClass)
					{
						this->CreateStaticItem(ResPath_C);
					}
				}
			}

			if (UFVMGameInstance::GetDebug())
			{
				UE_LOG(
					LogTemp,
					Error,
					TEXT("(%s)击中[%s]造成伤害 %.2f"),
					*UGameSystemFunction::GetObjectName(this),
					*UGameSystemFunction::GetObjectName(Mouse),
					this->GetCurATK()
				);
			}
		}
	}
}

void AFlyItemActor::OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//与老鼠重叠
	this->HitMouse_OverlapBegin(OtherActor);
}

void AFlyItemActor::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UGameSystemFunction::Log(FString(__FUNCTION__), );
		//开启范围伤害并且开启了静态对象
}

void AFlyItemActor::AddBuff(const FGameBuffInfor& _buff)
{
	this->M_Buff = _buff;
}

void AFlyItemActor::SetObjectActorLocation(AActor* ObjectActorLocatio)
{
	this->M_ObjectActorLocation = ObjectActorLocatio;
}

void AFlyItemActor::AddAttackType(ELineType _type)
{
	this->M_AttackType.Emplace(_type);
}

bool AFlyItemActor::GetFirstHitResult() const
{
	return this->bFirstHitResult;
}

void AFlyItemActor::SetTargetHitState(bool _Value)
{
	this->M_FlyCondition.M_bHitTarget = _Value;
}

void AFlyItemActor::AddAttackType(TArray<ELineType>& _types)
{
	this->M_AttackType.Append(_types);
}
