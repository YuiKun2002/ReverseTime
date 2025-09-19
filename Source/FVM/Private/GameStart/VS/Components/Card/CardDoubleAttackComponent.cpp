// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/VS/Components/Card/CardDoubleAttackComponent.h"
#include "GameStart/Flipbook/GameActor/Mouse/Normal/FlyStateMouse.h"
#include "GameStart/Flipbook/GameActor/CardActor/AttackCardActor.h"
#include "GameStart/Flipbook/GameActor/FlyItemActor.h"
#include "GameStart/VS/MapMeshe.h"
#include "GameStart/VS/GameMapInstance.h"
#include "GameStart/VS/Components/MesheControllComponent.h"
#include <Paper2D/Classes/PaperSprite.h>
#include <Paper2D/Classes/PaperFlipbook.h>
#include <Paper2D/Classes/PaperFlipbookComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include "GameSystem/Tools/GameSystemFunction.h"
#include "GameStart/VS/Components/MouseManagerComponent.h"

void UCardDoubleAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	this->MouseComp = AGameMapInstance::GetGameMapMouseManagerStatic();
}

void UCardDoubleAttackComponent::GameTick(float DeltaTime)
{
	if (time < 0.3f)
	{
		this->time += DeltaTime;
	}
	else {
		this->CurSky = this->LineComp->GetMouseTopBySky();
		this->time = 0.f;
		if (IsValid(this->CurSky))
		{
			this->bSky = true;
			this->AttackCardActor->SetCurrentMouse(this->CurSky);
			this->SetAttackModEnabled(true);
		}
		else {
			this->bSky = false;
		}
	}

	if (this->bSky)
	{
		this->TickCondition(DeltaTime);
	}
	else {
		//陆地攻击
		Super::GameTick(DeltaTime);
	}
}

void UCardDoubleAttackComponent::LoadResource()
{
	Super::LoadResource();

	this->LineComp = this->MouseComp->GetMouseLineManager(this->AttackCardActor->GetLine().Row);
}

void UCardDoubleAttackComponent::Spawn()
{
	if (IsValid(this->CurSky))
	{
		//当前老鼠无效
		if (!IsValid(this->AttackCardActor->GetCurrentMouse()))
		{
			//手动置空
			this->AttackCardActor->SetCurrentMouse(nullptr);
			this->SetAttackModEnabled(false);
			return;
		}

		if (this->bSFirst)
		{
			//设置随机数
			this->SetRandom();
			//随机子弹
			this->LauncherItem(this->SOtherItems, this->CurSFinishItems, this->STargetCardOtherItem);
		}
		else {
			//首次发射完毕，下一次发射将进行初始化随机数
			this->bSFirst = true;
		}

		if (IsValid(this->STargetCardOtherItem.GetRes()))
		{
			if (!IsValid(this->SPool[this->STargetCardOtherItem.GetIndex()]) || this->SPool.Num() < 1)
			{
				return;
			}

			//获取对象的变换位置
			const FTransform& _trans = this->AttackCardActor->GetActorTransform();
			FTransform NewTrans;
			NewTrans.SetLocation(_trans.GetLocation());

			float CurATK = this->AttackCardActor->GetCurrentATK();
			if (Cast<AFlyStateMouse>(this->CurSky))
			{
				CurATK = this->CurSky->GetTotalHP() * 0.21f;
			}

			//获取对象池的对象
			//生成子弹
			AFlyItemActor* _TargetActor = Cast<AFlyItemActor>(
				this->SPool[this->STargetCardOtherItem.GetIndex()]->GetObjectActor()
			);

			this->SpawnBullet(_TargetActor);
		}
	}
	else {
		Super::Spawn();
	}
}

void UCardDoubleAttackComponent::SpawnBullet(AFlyItemActor* NewBullet)
{
	if (IsValid(this->CurSky))
	{
		FTransform NewTrans;
		//如果第二个发射坐标有效
		if (IsValid(this->SecondBulletLauncherPointComp))
		{
			FVector NewLocaltion =
				this->AttackCardActor->GetActorLocation() +
				this->AttackCardActor->GetGameLocation()->GetRelativeLocation() +
				this->SecondBulletLauncherPointComp->GetRelativeLocation();

			//设置未旋转的位置
			NewTrans.SetLocation(NewLocaltion);
			//设置旋转后的位置
			NewTrans.SetLocation(
				FVector(
					550.f,
					NewTrans.GetLocation().Y,
					NewTrans.GetLocation().Z
				)
			);
		}
		else {
			NewTrans.SetLocation(
				this->AttackCardActor->GetBulletLauncherLocation()
			);
			NewTrans.SetLocation(
				FVector(
					550.f,
					NewTrans.GetLocation().Y,
					NewTrans.GetLocation().Z
				)
			);
		}

		float CurATK = this->AttackCardActor->GetCurrentATK();
		if (Cast<AFlyStateMouse>(this->CurSky))
		{
			CurATK = this->CurSky->GetTotalHP() * 0.21f;
		}

		//新生成的对象设置自定义拥有者(CardActor)
		NewBullet->SetFlyParentActor(this->AttackCardActor);
		NewBullet->SetLine(this->AttackCardActor->GetLine().Row);
		NewBullet->SetActorTransform(NewTrans);
		NewBullet->SetObjectActorLocation(this->AttackCardActor->GetCurrentMouse());
		NewBullet->SetATK(CurATK);
		NewBullet->SetSecondATK(
			this->AttackCardActor->GetCurrentSecondATK(
				this->AttackCardActor->GetATKCardData().M_SputteringATKRate)
		);
		NewBullet->SetFloatModeEnable(this->AttackCardActor->GetbMovement());
		NewBullet->Init();

	}
	else {
		Super::SpawnBullet(NewBullet);
	}
}

void UCardDoubleAttackComponent::PlayAttackAnimation()
{
	if (IsValid(this->CurSky))
	{
		//初始化首次发射
		this->bSFirst = false;
		//初始化
		this->InitLaunch(this->bSInitItemSort, this->SOtherItems, this->CurSFinishItems);
		//设置随机数
		this->SetRandom();
		//初始化数据
		this->LauncherItem(this->SOtherItems, this->CurSFinishItems, this->STargetCardOtherItem);

		//播放动画
		this->AttackCardActor->SetPlayAnimation(
			this->STargetCardOtherItem.GetAnim()
		);
	}
	else {
		Super::PlayAttackAnimation();
	}
}

void UCardDoubleAttackComponent::BeginDestroy()
{
	Super::BeginDestroy();

	for (auto& Cur : this->SPool)
	{
		if (IsValid(Cur))
		{
			Cur->ClearAllObjectActor();
			Cur = nullptr;
		}
	}

	this->SPool.Empty();
}

void UCardDoubleAttackComponent::InitSecondLaunchBullet(
	TSoftClassPtr<AFlyItemActor> Res,
	TSoftObjectPtr<UPaperFlipbook> AnimName
)
{
	if (this->bSInit)
	{
		return;
	}

	this->bSInit = true;

	this->AddSecondLaunchBulletByRandomValue(100, Res, AnimName);
}

void UCardDoubleAttackComponent::AddSecondLaunchBulletByRandomValue(
	int32 RandomValue,
	TSoftClassPtr<AFlyItemActor> Res,
	TSoftObjectPtr<UPaperFlipbook> AnimName
)
{
	if (RandomValue <= 0)
	{
		RandomValue = 1;
	}
	else if (RandomValue > 100)
	{
		RandomValue = 0;
	}


	//如果没有初始化，则初始化
	if (!this->bSInit)
	{
		this->InitSecondLaunchBullet(Res, AnimName);
		return;
	}

	UPaperFlipbook* NameClass = AnimName.LoadSynchronous();
	if (IsValid(NameClass))
	{
		//添加新的攻击方式
		this->SPool.Emplace(UObjectPoolManager::MakePoolManager(this->GetWorld(), Res, 1));
		this->SOtherItems.Emplace(FCardOtherItem(
			this->SPool.Num() - 1,
			RandomValue,
			Res,
			NameClass
		));
	}
}