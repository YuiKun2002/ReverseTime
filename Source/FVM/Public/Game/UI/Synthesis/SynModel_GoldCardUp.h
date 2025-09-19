// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "Game/UI/Synthesis/SynModel_Structures.h"
#include "Game/UI/Synthesis/SynthesisGeneralGrid.h"
#include "SynModel_GoldCardUp.generated.h"



/**
 *金卡进化-卡片材料
 */
UCLASS()
class FVM_API UUI_SynModel_GoldCardUp_CardMaterialBox : public UUI_SynModel_GeneralRectangleBox
{
	GENERATED_BODY()
};

/**
 *金卡进化-进化卡片
 */
UCLASS()
class FVM_API UUI_SynModel_GoldCardUp_CardBox : public UUI_SynModel_GeneralRectangleBox
{
	GENERATED_BODY()
};

/**
 *金卡进化-进化材料
 */
UCLASS()
class FVM_API UUI_SynModel_GoldCardUp_EvolveMaterial : public UUI_SynModel_GeneralBox
{
	GENERATED_BODY()
};


/**
 *金卡进化
 */
UCLASS()
class FVM_API USynModel_GoldCardUp : public UWidgetBase, public ISynthesisWidgetInterface
{
	GENERATED_BODY()

public:

	//初始化
	virtual void InitWidget_Implementation() override;

	//初始化
	void Init();

	//卡片选择
	void SelectCard(class UUI_PlayerBagCardGrid* CardIns);

	//加载防御卡
	void LoadCard();

	//加载材料
	void LoadMaterial();

	//取消选择
	UFUNCTION(BlueprintCallable)
	void Cancel();

	//金卡进化
	UFUNCTION(BlueprintCallable)
	void GoldCardEvolveFunc();

protected:

	//卡片材料
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GoldCardUp_CardMaterialBox* CardMaterialBox1 = nullptr;

	//卡片材料
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GoldCardUp_CardMaterialBox* CardMaterialBox2 = nullptr;

	//进化凭证材料
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GoldCardUp_EvolveMaterial* EvolveMaterialBox = nullptr;

	//进化卡片盒子
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GoldCardUp_CardBox* CardBox = nullptr;

	//进化按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* EvolveCard_Butt = nullptr;

private:

	//材料消耗的ID
	UPROPERTY()
	int32 MaterialID = -1;

	//目标卡片
	UPROPERTY()
	int32 TargetCardID = 0;

	//卡片等级
	UPROPERTY()
	int32 TargetLevel = 0;

	UPROPERTY()
	class UUI_PlayerSynthesis* Syn = nullptr;
};
