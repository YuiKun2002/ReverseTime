// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStart/VS/Components/GameStartActorComponent.h"
#include "CardDefenceComponent.generated.h"

class ACardActor;
class ADefenceCardActor;
class UPaperFlipbook;

//防御卡-防御类型组件
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FVM_API UCardDefenceComponent : public UGameStartActorComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UCardDefenceComponent();

	//更新状态
	UFUNCTION(BlueprintCallable)
	void UpdateState();

	//加载资源
	UFUNCTION(BlueprintCallable)
	void LoadResource();
public:
	//受到伤害时
	UFUNCTION()
	void OnCardAttacked(float& AttackValue, class UObject* AttackObject);
	//受到伤害时
	UFUNCTION()
	void OnCardReplyed(float& ReplyValue, class UObject* ReplyObject);
	//死亡时
	UFUNCTION()
	void OnCardDeathed(class UObject* TriggerObject);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//播放动画
	void PlayAnimation(
		class UPaperFlipbook*& AnimPtr,
		const TSoftObjectPtr<UPaperFlipbook>& Res
	);
	
private:
	//开启反伤效果
	void bEnabledReturnAttackValue(float& AttackValue, class UObject* AttackObject);
	//开启死亡爆炸
	void bEnabledDeadedBomb(class UObject* TriggerObject);
private:
	//防御卡
	UPROPERTY()
	ADefenceCardActor* DefenceCardActor = nullptr;
	//防御卡动画资源[默认]
	UPROPERTY()
	class UPaperFlipbook* State1 = nullptr;
	//【受伤】
	UPROPERTY()
	class UPaperFlipbook* State2 = nullptr;
	//【残血】
	UPROPERTY()
	class UPaperFlipbook* State3 = nullptr;
};
