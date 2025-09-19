// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Paper2D/Classes/PaperFlipbookComponent.h>
#include "GameStart/Flipbook/Components/FlipbookBaseComponent.h"
#include "GameStart/Flipbook/GameStartActorBase.h"
#include "GameActorFlipbookBase.generated.h"


class UPaperSprite;
class UFlipbookBaseComponent;
class UPaperFlipbookComponent;
class USceneComponent;

UCLASS()
class FVM_API AGameActorFlipbookBase : public AGameStartActorBase
{
	GENERATED_BODY()

	friend class UFlipbookBaseComponent;
public:
	//自定义Actor的拥有者
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AFlipbookCustomActorOwner")
	AActor* M_CustomActorOwner = nullptr;
	//渲染的翻书对象
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UFlipbookBaseComponent* M_Actor = nullptr;
	//位置[root]
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* M_Position = nullptr;
	//所有子对象的锚点
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* M_Location = nullptr;
public:
	// Sets default values for this actor's properties
	AGameActorFlipbookBase();
	//获取翻书对象MyActor
	UFUNCTION(BlueprintPure)
	UFlipbookBaseComponent* GetMyActor();
	//获取当前的Scene组件
	UFUNCTION(BlueprintPure)
	USceneComponent* GetMyPosition();
	//获取当前的统一坐标
	UFUNCTION(BlueprintPure)
	USceneComponent* GetGameLocation();
	//获取渲染组件
	UFUNCTION(BlueprintPure)
	UFlipbookBaseComponent* GetRenderComponent() const;

	//获取翻书的颜色
	UFUNCTION(BlueprintPure)
	FLinearColor GetFlipbookColor();
	
	//
	UFUNCTION(BlueprintPure)
	float GetFlipbookPitchRotation();

	virtual int32 GetRenderLayer() const override;

public:
	
	virtual void SetRenderLayer(int32 Layer) override;

	UFUNCTION(BlueprintCallable)
	void SetAnimationPlayEndDestroy();
	//设置单次动画播放切换(注意直接使用SetFlipBook会受到单次动画切换的影响)
	UFUNCTION(BlueprintCallable)
	void SetPlayAnimationOnce(UPaperFlipbook* _PlayAnim, UPaperFlipbook* _PlayEndAnim);
	//设置播放的动画(设置之后会影响(单次动画播放切换->失效))
	UFUNCTION(BlueprintCallable)
	void SetPlayAnimation(UPaperFlipbook* _PlayAnim, bool _bInfluence_AnimOnce = false);
	//设置播放速度
	UFUNCTION(BlueprintCallable)
	void SetAnimationPlayRate(float _Value);
	//设置翻书的颜色
	UFUNCTION(BlueprintCallable)
	void SetFlipbookColor(FVector _Color = FVector(1.f), float _Alpha = 1.f);

	//设置翻书的旋转(会调用Location相关的设置修正)
	UFUNCTION(BlueprintCallable)
	void SetFlipbookRotation(FRotator _rotation);
	//设置翻书旋转(Y-Pitch) 平面旋转
	UFUNCTION(BlueprintCallable)
	void SetFlipbookPitchRotation(float _angle);
	//设置游戏Actor翻书对象旋转
	UFUNCTION(BlueprintCallable)
	void SetGameActorFlipbookRotation(FRotator _rotation);
public:
	//判断该Actor是否等待被销毁
	UFUNCTION(BlueprintCallable)
	bool IsPendingKill(AActor* _Actor);

	//初始化旋转(直接校准)注意：初始化后X与Y的坐标会进行交换
	UFUNCTION(BlueprintCallable)
	void InitRotation();

	//播放完成
	UFUNCTION()
	void FuncDelegatePlayComplete();

	//播放结束
	UFUNCTION()
	void FuncDelegatePlayEnd();

	//动画播放完成
	UFUNCTION(BlueprintImplementableEvent)
	void OnAnimationPlayEndBlueprint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
