// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStart/VS/Components/Card/CardDefenceComponent.h"
#include "GameStart/Flipbook/GameActor/MouseActor.h"

#include <Paper2D/Classes/PaperFlipbook.h>
#include <Paper2D/Classes/PaperFlipbookComponent.h>

#include "GameStart/Flipbook/GameActor/Card/DefenceCardActor.h"
#include "GameStart/Flipbook/GameActor/MouseActor.h"

#include <Kismet/GameplayStatics.h>

#include "GameStart/VS/Components/MesheControllComponent.h"
#include "GameStart/VS/GameMapInstance.h"

// Sets default values for this component's properties
UCardDefenceComponent::UCardDefenceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCardDefenceComponent::UpdateState()
{
	if (IsValid(this->DefenceCardActor))
	{
		//获取到卡片生命值
		const float& HPTop = this->DefenceCardActor->GetTotalHP();
		float HP = this->DefenceCardActor->GetCurrentHP();

		if (HP >= HPTop * (0.6f))
		{
			this->PlayAnimation(this->State1, this->DefenceCardActor->CardActor_DefAnim1);
		}
		else if (HP >= HPTop * (0.3f) && HP < HPTop * (0.6f))
		{
			this->PlayAnimation(this->State2, this->DefenceCardActor->CardActor_DefAnim2);
		}
		else {
			//第三状态
			this->PlayAnimation(this->State3, this->DefenceCardActor->CardActor_DefAnim3);
		}
	}
}

void UCardDefenceComponent::LoadResource()
{

}

void UCardDefenceComponent::OnCardAttacked(float& AttackValue, class UObject* AttackObject)
{

#if WITH_EDITOR
	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__, TEXT("受到老鼠的攻击"));
	}
#endif

	//获取卡片数据
	const FItemCardDefence& CardData = this->DefenceCardActor->GetDefenceCardData();

	//反伤技能
	if (CardData.M_bEnableReturnATK)
	{
		this->bEnabledReturnAttackValue(AttackValue, AttackObject);
	}

	//更新状态
	this->UpdateState();
}

void UCardDefenceComponent::OnCardReplyed(float& ReplyValue, class UObject* ReplyObject)
{
	//更新状态
	this->UpdateState();
}

void UCardDefenceComponent::OnCardDeathed(class UObject* TriggerObject)
{

#if WITH_EDITOR
	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__, TEXT("防御类型的卡片死亡"));
	}
#endif

	//获取卡片数据
	const FItemCardDefence& CardData = this->DefenceCardActor->GetDefenceCardData();

	//死亡爆炸技能
	if (CardData.M_bEnableDeathBomb)
	{
		this->bEnabledDeadedBomb(TriggerObject);
	}
}


// Called when the game starts
void UCardDefenceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	this->DefenceCardActor = Cast<ADefenceCardActor>(this->GetOwner());

	if (!this->DefenceCardActor)
	{
		return;
	}

	//绑定函数代理

	//被攻击
	this->DefenceCardActor->OnCardAttackedDelegate.AddDynamic(this,
		&UCardDefenceComponent::OnCardAttacked);
	//回复血量
	this->DefenceCardActor->OnCardReplyedDelegate.AddDynamic(this,
		&UCardDefenceComponent::OnCardReplyed);
	//死亡
	this->DefenceCardActor->OnCardDeathedDelegate.AddDynamic(this,
		&UCardDefenceComponent::OnCardDeathed);
}

void UCardDefenceComponent::PlayAnimation(
	class UPaperFlipbook*& AnimPtr,
	const TSoftObjectPtr<UPaperFlipbook>& Res
) {
	if (IsValid(AnimPtr))
	{
		this->DefenceCardActor->SetPlayAnimation(AnimPtr);
	}
	else {
		AnimPtr = UGameSystemFunction::LoadRes(
			this->DefenceCardActor->CardActor_DefAnim1
		);
		this->DefenceCardActor->SetPlayAnimation(AnimPtr);
	}
}

void UCardDefenceComponent::bEnabledReturnAttackValue(float& AttackValue, class UObject* AttackObject)
{
	//获取当前攻击该卡片的老鼠
	if (AMouseActor* MouseActor = Cast<AMouseActor>(AttackObject))
	{
		const FItemCardDefence& CardData = this->DefenceCardActor->GetDefenceCardData();

		FGameBuffInfor buff;
		if (UGameSystemFunction::HitMouse(this->DefenceCardActor,
			CardData.M_ATK * CardData.M_ATK_UpRate *
			this->DefenceCardActor->GetTotalHP() * 0.1f / 2.f,
			MouseActor, buff, EFlyItemAttackType::Def)
			)
		{
#if WITH_EDITOR
			if (UFVMGameInstance::GetDebug())
			{
				UGameSystemFunction::FVMLog(__FUNCTION__, FString(TEXT("给攻击") +
					CardData.ItemName + TEXT("的老鼠造成反伤：")) +
					FString::SanitizeFloat(CardData.M_ATK *
						CardData.M_ATK_UpRate * this->DefenceCardActor->GetTotalHP() * 0.1f / 2.f)
				);
			}
#endif
		}
	}
}

void UCardDefenceComponent::bEnabledDeadedBomb(class UObject* TriggerObject) {

	const FItemCardDefence& CardData = this->DefenceCardActor->GetDefenceCardData();

	//是否开启播放音效
	if (CardData.M_FCardRangeATK_Audio.M_bEnable)
	{
		UFVMGameInstance::PlayBGM_S_Static(
			CardData.M_FCardRangeATK_Audio.M_AudioName,
			TEXT("ItemAudio"));
	}

	//是否开启动画播放
	if (CardData.M_bEnableDeathBombAnim)
	{
		FTransform Trans = this->DefenceCardActor->GetTransform();
		Trans.SetLocation(Trans.GetLocation() + CardData.M_AnimOffset);

		AGameActorFlipbookBase* Flip = this->DefenceCardActor->GetWorld()->SpawnActor<AGameActorFlipbookBase>(
			LoadClass<AGameActorFlipbookBase>(0, TEXT("Class'/Script/FVM.GameActorFlipbookBase'"))
		);
		Flip->SetRenderLayer(8000);
		Flip->InitRotation();
		Flip->SetActorLocation(Trans.GetLocation());
		Flip->GetMyActor()->SetFlipbook(Cast<UPaperFlipbook>(
			CardData.M_DeathBombAnimFlipbookPath.TryLoad()));
		Flip->SetAnimationPlayEndDestroy();
	}


	TArray<FHitResult> Hits;
	if (UGameSystemFunction::AddSphereTraceMulti(
		this->DefenceCardActor,
		Hits,
		FVector::ZeroVector,
		FVector::ZeroVector,
		CardData.M_ATKRadius,
		UGameSystemFunction::GetMouseCollisionTraceType(CardData.M_ELineTraceType)
	))
	{
		float HurtValue = this->DefenceCardActor->GetCurrentHP() > 0.f ?
			this->DefenceCardActor->GetCurrentHP() * CardData.M_LifeToATK_Rate :
			this->DefenceCardActor->GetTotalHP() * 0.1f;

		FGameBuffInfor buff = CardData.M_Buffs;

		for (auto& Mouse : Hits) {
			if (
				UGameSystemFunction::HitMouse(this->DefenceCardActor,
					HurtValue,
					Cast<AMouseActor>(Mouse.GetActor()), buff, EFlyItemAttackType::Bomb)
				)
			{
#if WITH_EDITOR
				if (UFVMGameInstance::GetDebug())
				{
					UGameSystemFunction::FVMLog(__FUNCTION__,
						FString(TEXT("给攻击") +
							CardData.ItemName +
							TEXT("的老鼠造成反伤：")) +
						FString::SanitizeFloat(HurtValue)
					);
				}
#endif
			}
		}
	}
}

