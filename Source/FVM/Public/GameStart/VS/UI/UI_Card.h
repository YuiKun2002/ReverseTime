// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "UI_Card.generated.h"

class ACardCoolDownManager;
class UCardManagerComponent;
class UImage;
class UPaperFlipbook;


UCLASS()
class FVM_API UUI_Card : public UWidgetBase
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
public:
	//获取冷却时间
	UFUNCTION(BlueprintPure)
	bool GetCoolDownFinish();
	//获取完成比率
	UFUNCTION(BlueprintPure)
	float GetCoolDownFinishRate();
	//获取背景
	UFUNCTION(BlueprintPure)
	UButton* GetCardButtonA();
	//获取卡片头像
	UFUNCTION(BlueprintPure)
	UImage* GetCardHead();
	//获取卡片是否被选择
	UFUNCTION(BlueprintPure)
	bool GetCardSelectState();
	//获取鼠标移入移出状态
	UFUNCTION(BlueprintPure)
	bool GetMouseEnterState();
	//获取卡片ID
	UFUNCTION(BlueprintPure)
	int32 GetCardID() const;
	//设置卡片火苗
	UFUNCTION(BlueprintPure)
	int32 GetCardFlame() const;
	//设置卡片是否可以被复制
	UFUNCTION(BlueprintPure)
	bool GetbEnableCopy() const;
	//获取卡片实例Class
	UFUNCTION(BlueprintPure)
	UClass* const GetCardClassInstance();
	//获取卡片冷却管理器
	UFUNCTION(BlueprintPure)
	class ACardCoolDownManager* GetCardCoolDownManager();
public:
	//设置卡片ID
	UFUNCTION(BlueprintCallable)
	void SetCardID(int32 ID);
	//设置卡片火苗
	UFUNCTION(BlueprintCallable)
	void SetCardFlame(int32 FlameNum, bool bUp);
	//设置卡片是否可以被复制
	UFUNCTION(BlueprintCallable)
	void SetbEnableCopy(bool bCopy);
	//设置卡片等级
	UFUNCTION(BlueprintCallable)
	void SetCardLevelTexture(const int32& _Grade);
	//设置冷却完成
	UFUNCTION(BlueprintCallable)
	void SetCoolDownFinish();
	//设置选择的状态
	UFUNCTION(BlueprintCallable)
	void SetCardSelect(bool _bSelect);
	//设置鼠标移入移出的状态
	UFUNCTION(BlueprintCallable)
	void SetMouseEnter(bool _bEnter);
	//设置卡片实例类型路径
	UFUNCTION(BlueprintCallable)
	void SetCardSourcePath(const FSoftClassPath& Path);
	//设置卡片冷却
	UFUNCTION(BlueprintCallable)
	void SetCardCoolDownManager(ACardCoolDownManager* CoolDownManager);
	//设置卡片实例
	UFUNCTION(BlueprintCallable)
	void SetCardClassInstance(UClass* Instance);
public:
	//播放卡片选择动画-bForward正常播放  否则 逆播放
	UFUNCTION(BlueprintCallable)
	void PlaySelectCardAnimation(bool bForward = true);
public:
	//创建卡片动画(Flipbook 3D场景  用于被检测到拖拽时创建)
	UFUNCTION(BlueprintCallable)
	UPaperFlipbook* CreateCard3DSceneFlipbook();
public:
	//选中时的颜色
	UFUNCTION(BlueprintCallable)
	void SelectColor();
public:
	//取消时的颜色
	UFUNCTION(BlueprintCallable)
	void CancelColor();

protected:
	//卡片火苗数量文本
	UPROPERTY(meta = (BindWidegt))
	class UTextBlock* CardFlame;
	//卡片按钮
	UPROPERTY()
	UButton* M_Card_Butt = nullptr;
	//卡片选择头像
	UPROPERTY()
	UImage* M_Card_Select_Head = nullptr;
	//卡片等级图片
	UPROPERTY()
	UImage* M_UImage_Grade = nullptr;
private:
	//卡片索引
	UPROPERTY()
	int32 M_CardID = -1;
	//需要的火苗数量
	UPROPERTY()
	int32 M_NeedFlame = -1;

	//是否被选中
	UPROPERTY()
	bool bSelect = false;
	//鼠标是否移入
	UPROPERTY()
	bool bMouseEnter = false;
	//是否允许被复制
	UPROPERTY()
	bool bEnableCopy = true;
private:
	//卡片实例类
	UPROPERTY()
	UClass* CardSourceInstance = nullptr;
	//卡片冷却管理器
	UPROPERTY()
	class ACardCoolDownManager* CardCoolDownManager = nullptr;
	//卡片管理器
	UPROPERTY()
	class UCardManagerComponent* CardManager = nullptr;
};
