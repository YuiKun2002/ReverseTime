// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/VS/Components/Card/AttackLauncherComponent.h"
#include "GameStart/Flipbook/GameActor/FlyItemActor.h"
#include "GameStart/VS/MapMeshe.h"
#include "GameStart/VS/GameMapInstance.h"
#include "GameStart/VS/Components/MesheControllComponent.h"
#include <Paper2D/Classes/PaperSprite.h>
#include <Paper2D/Classes/PaperFlipbook.h>
#include <Paper2D/Classes/PaperFlipbookComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include "GameSystem/GameDataSubsystem.h"

void UAttackLauncherComponent::BeginPlay()
{
	Super::BeginPlay();

	this->bFirst = false;
	this->bInitItemSort = false;
}

void UAttackLauncherComponent::LoadResource()
{
	Super::LoadResource();
}

void UAttackLauncherComponent::Spawn()
{
	if (this->bFirst)
	{
		//设置随机数
		this->SetRandom();
		//随机子弹
		this->LauncherItem(this->OtherItems, this->CurFinishItems, this->TargetCardOtherItem);
	}
	else {
		//首次发射完毕，下一次发射将进行初始化随机数
		this->bFirst = true;
	}

	if (IsValid(this->TargetCardOtherItem.GetRes()))
	{
		if (!IsValid(this->Pool[this->TargetCardOtherItem.GetIndex()]) || this->Pool.Num() < 1)
		{
			return;
		}

		//生成子弹
		this->SpawnBullet(Cast<AFlyItemActor>(
			this->Pool[this->TargetCardOtherItem.GetIndex()]->GetObjectActor()
		));
	}
}

void UAttackLauncherComponent::SpawnBullet(AFlyItemActor* NewBullet) {}

void UAttackLauncherComponent::InitLaunchBulletByDef(
	TSoftClassPtr<AFlyItemActor> BulletRes,
	TSoftObjectPtr<UPaperFlipbook> IdleName,
	TSoftObjectPtr<UPaperFlipbook> AttackName
)
{
	if (this->bInit)
	{
		return;
	}

	this->bInit = true;
	//初始化默认子弹的对象池
	this->Pool.Emplace(UObjectPoolManager::MakePoolManager(
		this->GetWorld(),
		BulletRes,
		1));


	//初始化默认子弹资源[默认子弹，默认动画] 添加默认攻击动作
	this->OtherItems.Emplace(FCardOtherItem(this->Pool.Num() - 1, 100,
		BulletRes,
		AttackName.LoadSynchronous()
	));

	//添加默认Idle动画
	this->TargetIdleAnimationNames.Emplace(
		IdleName.LoadSynchronous()
	);
}

void UAttackLauncherComponent::PlayAttackAnimation()
{
	Super::PlayAttackAnimation();

	//初始化首次发射
	this->bFirst = false;
	//初始化
	this->InitLaunch(this->bInitItemSort, this->OtherItems, this->CurFinishItems);
	//设置随机数
	this->SetRandom();
	//初始化数据
	this->LauncherItem(this->OtherItems, this->CurFinishItems, this->TargetCardOtherItem);
}

void UAttackLauncherComponent::PlayIdleAnimation()
{
	Super::PlayIdleAnimation();
}

void UAttackLauncherComponent::BeginDestroy()
{
	Super::BeginDestroy();

	for (auto& Cur : this->Pool)
	{
		if (IsValid(Cur))
		{
			Cur->ClearAllObjectActor();
			Cur = nullptr;
		}
	}

	this->Pool.Empty();
}

bool UAttackLauncherComponent::AddLaunchBulletByRandomValue(
	int32 RandomValue,
	TSoftClassPtr<AFlyItemActor> Res,
	TSoftObjectPtr<UPaperFlipbook> Anim
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

	UPaperFlipbook* AnimObj = Anim.LoadSynchronous();
	if (IsValid(AnimObj))
	{
		//添加新的攻击方式
		this->Pool.Emplace(UObjectPoolManager::MakePoolManager(this->GetWorld(), Res, 1));
		this->OtherItems.Emplace(FCardOtherItem(
			this->Pool.Num() - 1,
			RandomValue,
			Res,
			AnimObj
		));

		return true;
	}

	return false;
}

void UAttackLauncherComponent::AddIdleAnimName(
	const TArray<TSoftObjectPtr<UPaperFlipbook>>& IdleAnimNames
)
{
	for (const TSoftObjectPtr<UPaperFlipbook>& Name : IdleAnimNames)
	{
		UPaperFlipbook* NewAnim = Name.LoadSynchronous();
		if (IsValid(NewAnim))
		{
			this->TargetIdleAnimationNames.Emplace(NewAnim);
		}
	}
}

UPaperFlipbook* UAttackLauncherComponent::GetIdleAnimName()
{
	return this->TargetIdleAnimationNames[
		UGameSystemFunction::GetRandomRange(0, this->TargetIdleAnimationNames.Num() - 1)
	];
}

UPaperFlipbook* UAttackLauncherComponent::GetAttackAnimName()
{
	return this->TargetCardOtherItem.GetAnim();
}

void UAttackLauncherComponent::InitLaunch(bool& InitItemSort, const TArray<FCardOtherItem>& Items, TMap<int32, FCardOtherItemInitFinish>& Out)
{
	if (!InitItemSort)
	{
		InitItemSort = true;

		/*
		将所有的攻击动作按照概率分类
		【45%】多少个攻击动作
		【100%】多少个攻击动作
		*/
		//初始化数据
		TMap<int32, TArray<const FCardOtherItem*>> CurMap;
		for (const FCardOtherItem& Cur : Items)
		{
			//集合存在元素
			if (CurMap.Num())
			{
				//遍历集合，添加相同概率的飞行物
				bool bAdd = false;
				for (auto& LMap : CurMap)
				{
					//判断有没有相同概率的对象
					if (LMap.Key == Cur.GetValue())
					{
						LMap.Value.Emplace(&Cur);
						bAdd = true;
					}
				}

				//不相同的概率，单独添加集合
				if (!bAdd)
				{
					TArray<const FCardOtherItem*> CurU;
					CurU.Emplace(&Cur);
					CurMap.Emplace(Cur.GetValue(), CurU);
				}
			}
			else {
				//添加集合
				TArray<const FCardOtherItem*> CurU;
				CurU.Emplace(&Cur);
				CurMap.Emplace(Cur.GetValue(), CurU);
			}
		}

		//排序
		CurMap.KeySort([](const int32& A, const int32& B) {
			return (A < B);
			});

		//初始化元素【完成分类】
		for (const auto& LMap : CurMap)
		{
			TArray<FCardOtherItem> Temp;
			for (const auto& LItem : LMap.Value)
			{
				Temp.Emplace(*LItem);
			}
			Out.Emplace(LMap.Key, FCardOtherItemInitFinish(Temp));
		}
	}
}

void UAttackLauncherComponent::LauncherItem(const TArray<FCardOtherItem>& Items, const TMap<int32, FCardOtherItemInitFinish>& OtherItemInitFinish, FCardOtherItem& Out)
{
	if (!Items.Num())
	{
		return;
	}

	//获取随机数
	int32 CurRandom = this->GetRandom();


	/*
		根据数值概率随机选择一种攻击动作
	*/
	int32 CurSelectIndexKey = 100;
	//选择资源
	for (const auto& LMap : OtherItemInitFinish)
	{
		if (LMap.Key >= CurRandom)
		{
			CurSelectIndexKey = LMap.Key;
			break;
		}
	}

	//选择当前段
	int32 CurSplitIndex = UGameSystemFunction::GetRandomRange(0,
		(*(OtherItemInitFinish.Find(CurSelectIndexKey))).GetCount() - 1);

	//获取当前需要发射的物品【初始化当前需要使用的攻击动作】
	Out = (*(OtherItemInitFinish.Find(CurSelectIndexKey))).GetItems()[CurSplitIndex];
}

void UAttackLauncherComponent::SetRandom()
{
	this->RandomNumber = UGameSystemFunction::GetRandomRange(1, 100);
}

int32 UAttackLauncherComponent::GetRandom()
{
	return this->RandomNumber;
}

FCardOtherItem::FCardOtherItem(
	int32 PoolIndex,
	int32 RValue,
	TSoftClassPtr<AFlyItemActor> Res,
	UPaperFlipbook* Anim) : CurValue(RValue), CurPoolIndex(PoolIndex)
{
	this->CurRes = UGameSystemFunction::LoadRes(Res);
	this->ActionAnim = Anim;
}

int32 FCardOtherItem::GetValue() const
{
	return this->CurValue;
}

int32 FCardOtherItem::GetIndex() const
{
	return this->CurPoolIndex;
}

UClass* FCardOtherItem::GetRes() const
{
	return this->CurRes;
}

UPaperFlipbook* FCardOtherItem::GetAnim() const
{
	return this->ActionAnim;
}
