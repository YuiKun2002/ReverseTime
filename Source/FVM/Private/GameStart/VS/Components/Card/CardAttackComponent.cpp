// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/VS/Components/Card/CardAttackComponent.h"
#include "GameStart/Flipbook/GameActor/CardActor/AttackCardActor.h"
#include "GameStart/Flipbook/GameActor/FlyItemActor.h"
#include "GameStart/VS/Components/CardManagerComponent.h"
#include "GameStart/VS/MapMeshe.h"
#include "GameStart/VS/GameMapInstance.h"
#include "GameStart/VS/Components/MesheControllComponent.h"
#include <Paper2D/Classes/PaperSprite.h>
#include <Paper2D/Classes/PaperFlipbook.h>
#include <Paper2D/Classes/PaperFlipbookComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include "GameSystem/GameDataSubsystem.h"

void UCardAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	//初始化攻击卡片
	this->AttackCardActor = Cast<AAttackCardActor>(this->GetOwner());

	if (!this->AttackCardActor)
	{
		if (UFVMGameInstance::GetDebug())
		{
			UGameSystemFunction::FVMLog(TEXT("UCardAttackComponent"), TEXT("攻击卡片初始化失败"));
			this->SetComponentTickEnabled(false);
		}
	}
}

void UCardAttackComponent::SpawnBullet(AFlyItemActor* NewBullet)
{
	//获取对象的变换位置
	FTransform NewTrans;
	NewTrans.SetLocation(this->AttackCardActor->GetBulletLauncherLocation());
	NewTrans.SetLocation(
		FVector(
			550.f,
			NewTrans.GetLocation().Y,
			NewTrans.GetLocation().Z
		)
	);

	//新生成的对象设置自定义拥有者(CardActor)
	NewBullet->SetFlyParentActor(this->AttackCardActor);
	NewBullet->SetLine(this->AttackCardActor->GetLine().Row);
	NewBullet->SetActorTransform(NewTrans);
	NewBullet->SetObjectActorLocation(this->AttackCardActor->GetCurrentMouse());
	NewBullet->SetATK(this->AttackCardActor->GetCurrentATK());
	NewBullet->SetSecondATK(
		this->AttackCardActor->GetCurrentSecondATK(
			this->AttackCardActor->GetATKCardData().M_SputteringATKRate)
	);
	NewBullet->SetFloatModeEnable(this->AttackCardActor->GetbMovement());
	NewBullet->Init();
}

void UCardAttackComponent::LoadResource()
{
	Super::LoadResource();

	//初始化条件
	this->InitLaunchProperty(
		this->AttackCardActor->GetCurrentAttackCount(),
		this->AttackCardActor->GetCurrentAttackSpeed(),
		this->AttackCardActor->GetCurrentFristAttackDelay(),
		this->AttackCardActor->GetCurrentSecondAttackDelay()
	);

	//初始化默认子弹
	this->InitLaunchBulletByDef(
		this->AttackCardActor->CardActor_BulletClassObj,
		this->AttackCardActor->CardActor_DefAnim,
		this->AttackCardActor->CardActor_AttackAnim
	);

	//播放发呆动画
	this->AttackCardActor->SetPlayAnimation(
		this->GetIdleAnimName()
	);

}

void UCardAttackComponent::PlayAttackAnimation()
{
	Super::PlayAttackAnimation();

	this->AttackCardActor->SetPlayAnimation(
		this->GetAttackAnimName()
	);
}

void UCardAttackComponent::PlayIdleAnimation()
{
	Super::PlayIdleAnimation();

	this->AttackCardActor->SetPlayAnimation(
		this->GetIdleAnimName()
	);
}

void UCardAttackComponent::GameTick(float DeltaTime)
{
	Super::GameTick(DeltaTime);

	if (time < 0.3f)
	{
		this->time += DeltaTime;
	}
	else {
		//卡片休息
		if (!this->AttackCardActor->GetCardDay() && this->AttackCardActor->GetMapDay())
		{
			return;
		}

		this->time = 0;

		//计算浮动地板的检测线条
		if (this->AttackCardActor->GetbMovement())
		{
			this->AttackCardActor->CalculateLineTracePosition(
				this->AttackCardActor->GetLineTracePosition()
			);
		}

		//当前检测到的老鼠线路
		ELineTraceType CurCheckLineType;
		//当前老鼠
		AMouseActor* CurMouse = Cast<AMouseActor>(UGameSystemFunction::CreateCheckMouseLineTrace(
			this->GetWorld(),
			this->AttackCardActor->GetLineTraceSetting(), CurCheckLineType
		));

		//老鼠有效则设置攻击模式
		if (IsValid(CurMouse))
		{
			this->AttackCardActor->SetCurrentMouse(CurMouse);
			this->SetAttackModEnabled(true);
		}
		else
		{
			this->AttackCardActor->SetCurrentMouse(nullptr);
			this->SetAttackModEnabled(false);
		}
	}
}