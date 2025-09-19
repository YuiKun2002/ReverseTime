// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CardCoolDownManager.generated.h"

//卡片冷却管理器
UCLASS()
class FVM_API ACardCoolDownManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACardCoolDownManager();

	//获取完成的比率
	UFUNCTION(BlueprintCallable)
	float GetFinishRate();

	//直接完成冷却
	UFUNCTION(BlueprintCallable)
	void SetCoolDownFinish(bool _finish = true);

	//判断是否冷却完成
	UFUNCTION(BlueprintCallable)
	bool GetIsCoolDownFinish();

	//启动冷却
	UFUNCTION(BlueprintCallable)
	void SetCoolDownEnable(bool _enable = true);

	//设置冷却时间
	UFUNCTION(BlueprintCallable)
	void SetCoolDownTime(const float& _cooldown_time);

	//设置永久冷却
	UFUNCTION(BlueprintCallable)
	void SetNeverCoolDown();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	UPROPERTY()
	bool bNeverCoolDown = false;
private:
	//冷却时间
	UPROPERTY()
	float CoolDownTime = 4.f;
	//当前冷却时间
	UPROPERTY()
	float CurrentCoolDownTime = 0.f;
	//是否冷却完成
	UPROPERTY()
	bool IsCoolDownFinish = true;
};
