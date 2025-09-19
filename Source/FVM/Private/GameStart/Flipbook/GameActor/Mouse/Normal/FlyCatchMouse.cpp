// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/Flipbook/GameActor/Mouse/Normal/FlyCatchMouse.h"
#include "GameStart/VS/Components/ResourceManagerComponent.h"
#include "GameStart/VS/Components/MesheControllComponent.h"
#include "GameStart/Flipbook/GameActor/FlyItemActor.h"
#include "GameStart/Flipbook/GameActor/CardActor.h"
#include "GameSystem/GameMapStructManager.h"
#include "GameStart/VS/GameMapInstance.h"
#include <Components/SphereComponent.h>
#include <Components/BoxComponent.h>
#include "Data/GameLogSubsystem.h"
#include "GameStart/VS/MapMeshe.h"

AFlyCatchMouse::AFlyCatchMouse()
{
	this->MMesheComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("FCMouseMeshComp"));
	this->MBodyComponent = CreateDefaultSubobject<USphereComponent>(TEXT("FCMouseBodyComp"));

	this->CurCardAnim = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FCCardAnimComp"));

	//设置依附
	this->MMesheComponent->SetupAttachment(this->GetRootComponent());
	this->MBodyComponent->SetupAttachment(this->MMesheComponent);

	//卡片动画
	this->CurCardAnim->SetupAttachment(this->GetRenderComponent());
}

void AFlyCatchMouse::MouseTick(const float& DeltaTime)
{
	Super::MouseTick(DeltaTime);

	if (this->bEnter)
	{
		if (this->DealyTime > 0.f)
		{
			this->DealyTime -= DeltaTime;
			return;
		}
		else {
			if (this->fToGroundTime > 0.f)
			{
				this->fToGroundTime -= DeltaTime;

				//设置位置
				this->SetActorLocation(
					UKismetMathLibrary::VLerp(this->CurLocation,
						CurUI->GetMapMeshe()->GetActorLocation(),
						(1.f - this->fToGroundTime))
				);
			}
			else {
				this->bEnter = false;

				if (IsValid(this->CurFlag))
				{
					this->CurFlag->Destroy();
				}

				//落地
				this->SetActorLocation(
					CurUI->GetMapMeshe()->GetActorLocation()
				);

				//播放抓取动画
				this->SetPlayAnimation(UGameSystemFunction::LoadRes(this->AnimRes.Catch), true);

				//切换陆地形态
				this->GetMouseManager()->ChangeMouseLineType(this,
					this->GetMouseLine().Row, ELineType::OnGround,
					this->MMesheComponent,
					this->MBodyComponent
				);

				//切换目标线路
				this->GetMouseManager()->ForceChangeLine(
					UGameSystemFunction::GetObjectName(this),
					this->GetMouseLine().Row, this->CurUI->GetMapMeshe()->GetLine().Row, -1
				);

				//设置渲染层
				this->SetRenderLayer(
					this->CurUI->GetMapMeshe()->GetRenderLayer()
				);

				//绑定动画结束
				this->GetRenderComponent()->OnAnimationPlayEnd.BindUObject(this, &AFlyCatchMouse::OnAnimationPlayEnd);
			}
		}
	}

	if (this->bExit)
	{
		if (this->fToGroundTime > 0.f)
		{
			this->fToGroundTime -= DeltaTime;
			//设置位置
			this->SetActorLocation(
				UKismetMathLibrary::VLerp(this->CurExitLocation,
					this->CurLocation,
					1.f - this->fToGroundTime)
			);
		}
		else {
			this->bExit = false;

			this->SetMouseDeath(true);
		}
	}
}

void AFlyCatchMouse::BeginPlay()
{
	Super::BeginPlay();

	this->CurCardAnim->SetFlipbook(nullptr);
	UGameSystemFunction::InitMouseMeshe(this->MMesheComponent, this->MBodyComponent);
	this->GetRenderComponent()->OnAnimationPlayEnd.Unbind();
	//this->GetRenderComponent()->OnAnimationPlayEnd.BindUObject(this, &AFlyCatchMouse::OnAnimationPlayEnd);
}

void AFlyCatchMouse::MouseInit()
{
	Super::MouseInit();

	this->bEnter = false;
	this->bExit = false;

	//切换空中形态
	this->GetMouseManager()->ChangeMouseLineType(this,
		this->GetMouseLine().Row,
		ELineType::Sky,
		this->MMesheComponent,
		this->MBodyComponent
	);

	//初始化位置
	this->SetRenderLayer(99999);
	this->CurLocation = this->GetActorLocation();
	this->bEnter = false;

	//查询地图实例是否存在
	if (IsValid(AGameMapInstance::GetGameMapInstance()))
	{
		UMesheControllComponent* CurComp = AGameMapInstance::GetGameMapMesheControllManagerStatic();
		if (IsValid(CurComp))
		{

			//存储可以放置卡片的UI
			TArray<UUI_MapMeshe*> TempMeshe;
			//带有目标卡片的网格
			TArray<UUI_MapMeshe*> HasCardMeshe;

			for (int32 R = 0; R < CurComp->GetMapMeshRowAndCol().Row; R++)
			{
				for (int32 C = 0; C < CurComp->GetMapMeshRowAndCol().Col; C++)
				{
					if (AMapMeshe* CurMesh = CurComp->GetMapMesh(R, C))
					{
						if (IsValid(CurMesh->GetUIMapMeshe()))
						{
							UUI_MapMeshe* LUI = CurMesh->GetUIMapMeshe();
							TempMeshe.Emplace(LUI);
							//1号卡片
							ACardActor* Cur0 = LUI->GetCardLayers(0);
							if (IsValid(Cur0) && Cur0->GetCurrentHP() > 0.f)
							{
								//this->CurCatchCard = Cur0;

								HasCardMeshe.Emplace(LUI);
								continue;
							}

							//2号卡片
							ACardActor* Cur1 = LUI->GetCardLayers(1);
							if (IsValid(Cur1) && Cur1->GetCurrentHP() > 0.f)
							{
								//this->CurCatchCard = Cur0;

								HasCardMeshe.Emplace(LUI);

								continue;
							}
						}
					}
				}
			}


			//优先抓去带有卡片的网格
			if (HasCardMeshe.Num() > 0)
			{
				//随机选择网格进行抓取
				int32 Index = UGameSystemFunction::GetRandomRange(0, HasCardMeshe.Num() - 1);
				this->CurUI = HasCardMeshe[Index];
			}
			else {
				if (TempMeshe.Num() > 0)
				{
					int32 Index = UGameSystemFunction::GetRandomRange(0, TempMeshe.Num() - 1);
					this->CurUI = TempMeshe[Index];
				}
			}


			if (IsValid(this->CurUI))
			{
				//创建标记
				UClass* CurFlagClass = UGameSystemFunction::LoadRes(this->AnimRes.FlagClass);
				if (IsValid(CurFlagClass))
				{
					//设置老鼠位置
					this->SetActorLocation(this->CurUI->GetMapMeshe()->GetActorLocation() + FVector(0.f, 0.f, 1000.f));
					this->CurLocation = this->GetActorLocation();
					this->CurFlag = this->GetWorld()->SpawnActor<AFlyCatchMouseFlag>(CurFlagClass);
					this->CurFlag->SetActorLocation(this->CurUI->GetMapMeshe()->GetActorLocation());
					this->CurFlag->SetRenderLayer(9999);
					this->DealyTime = 3.f;
					this->fToGroundTime = 1.f;
					this->bEnter = true;
					return;
				}
				else {
					this->SetMouseDeath(true);
					return;
				}
				return;
			}
		}
	}

	this->SetMouseDeath(true);
}

void AFlyCatchMouse::MoveingUpdate(float DeltaTime)
{
	Super::MoveingUpdate(DeltaTime);
}

bool AFlyCatchMouse::BeHit(UObject* CurHitMouseObj, float _HurtValue, EFlyItemAttackType AttackType)
{
	return Super::BeHit(CurHitMouseObj, _HurtValue, AttackType);
}

void AFlyCatchMouse::MouseDeathed()
{
	Super::MouseDeathed();

	//关闭碰撞
	this->ClosedBoxComponent(this->MMesheComponent);
	this->ClosedBoxComponent(this->MBodyComponent);

	this->bEnter = false;
	this->bExit = false;

	if (!this->GetPlayPlayBombEffAnim())
	{
		this->SetPlayAnimation(nullptr);
		this->CurCardAnim->SetFlipbook(nullptr);
	}

	if (IsValid(this->CurFlag))
	{
		this->CurFlag->Destroy();
	}
}

void AFlyCatchMouse::OnAnimationPlayEnd()
{
	if (this->GetCurrentHP() > 0.f)
	{
		//关闭碰撞
		this->ClosedBoxComponent(this->MMesheComponent);
		this->ClosedBoxComponent(this->MBodyComponent);

		//获取防御卡动画
		if (IsValid(CurUI))
		{
			//1号卡片
			ACardActor* Cur0 = this->CurUI->GetCardLayers(0);
			//2号卡片
			ACardActor* Cur1 = this->CurUI->GetCardLayers(1);

			if (IsValid(Cur0) && Cur0->GetCurrentHP() > 0.f)
			{
				this->CurCatchCard = Cur0;
			}
			else if (IsValid(Cur1) && Cur1->GetCurrentHP() > 0.f)
			{
				this->CurCatchCard = Cur1;
			}
			else {
				this->CurCatchCard = nullptr;
			}
		}

		//设置防御卡动画
		if (IsValid(this->CurCatchCard))
		{
			this->CurCardAnim->SetFlipbook(
				this->CurCatchCard->GetRenderComponent()->GetFlipbook()
			);
			this->CurCardAnim->SetTranslucentSortPriority(this->GetRenderLayer() - 1);
			this->CurCardAnim->Stop();
		}
		else {
			this->CurCardAnim->SetFlipbook(nullptr);
		}

		//消灭防御卡	
		if (IsValid(this->CurCatchCard))
		{
			//直接消灭卡片
			if (this->CurCatchCard->AttackCard(this->CurCatchCard->GetTotalHP(), this))
			{
				this->CurCatchCard->KillCard();
			}
			
			UResourceManagerComponent::ResourceAddBadCard();
		}

		//播放退场
		this->GetRenderComponent()->OnAnimationPlayEnd.Unbind();

		this->SetPlayAnimation(UGameSystemFunction::LoadRes(this->AnimRes.Exit), true);

		this->fToGroundTime = 1.f;
		this->bExit = true;
		//对齐坐标
		this->CurLocation.Z = 1000.f;
		this->CurExitLocation = this->GetActorLocation();
	}
}
