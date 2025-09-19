// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "UObject/NoExportTypes.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "GameSystem/Item/Card/CardBaseStruct.h"
#include "Game/UI/Synthesis/SynModel_Structures.h"
#include "Game/UI/Synthesis/SynthesisGeneralGrid.h"
#include "SynModel_CardUpgrade.generated.h"

/**
 *卡片制作-卡片盒体
 */
UCLASS()
class FVM_API UUI_SynModel_UpgradeCard_Box : public UUI_SynModel_GeneralRectangleBox
{
	GENERATED_BODY()
public:
	virtual FString ToString_Implementation() override;
	virtual void OnClicked_Implementation() override;
	virtual	void SetItemAttribute(class UItemBaseView* View,const FItemViewBlock& BlockData,int32 Index) override;
};

/**
 *卡片强化主界面
 */
UCLASS()
class FVM_API UUI_SynModel_UpgradeCard : public UWidgetBase, public ISynthesisWidgetInterface
{
	GENERATED_BODY()

public:

	//初始化
	virtual void InitWidget_Implementation() override;

	//选择卡片
	void Select();

	//选择四叶草
	void SetClover(class UItemGridBase* CopyCloverGrid, float Rate);

	//取消四叶草的选择
	UFUNCTION(BlueprintCallable)
	void CancelSelectClover();

	//获取四叶草ID
	FORCEINLINE int32 GetCloverID() { return this->CloverID; }

	//重新加载卡片
	void Reload();

	//加载卡片
	void LoadCard(bool bNotLoad = false);

	//加载材料
	void LoadMaterial(bool bNotLoad = false);

	//更新概率
	void UpdateRate();

protected:

	//卡片强化
	UFUNCTION(BlueprintCallable)
	void UpgradeCard();

	//获取强化概率
	float GetUpgradeRate(bool bAdvancedCard,
		const int32& CardLevel,
		const int32& CardPrice,
		const int32& D_Value);

public:
	//卡片强化卡槽
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_UpgradeCard_Box* CardBox_CenterTop = nullptr;

	//卡片强化卡槽
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_UpgradeCard_Box* CardBox_Center = nullptr;

	//卡片强化卡槽
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_UpgradeCard_Box* CardBox_Left = nullptr;

	//卡片强化卡槽
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_UpgradeCard_Box* CardBox_Right = nullptr;

	//四叶草
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GeneralBox* BP_Syn_UpCardCloverBox = nullptr;

protected:

	//卡片强化按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Upgrade_Butt = nullptr;

	//货币文本
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* CoinText = nullptr;

	//强化概率文本
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* RateText = nullptr;

private:
	UPROPERTY()
	class UUI_PlayerSynthesis* Syn = nullptr;
private:
	//当前四叶草的ID
	UPROPERTY()
	int32 CloverID = -1;
	//当前四叶草的强化概率
	UPROPERTY()
	float CloverRate = 1.f;

	//当前强化概率
	UPROPERTY()
	float UpRate = 0.f;
	//当前强化金额
	UPROPERTY()
	int32 UpCoin = 0;


	//强化所用概率
	UPROPERTY()
	TArray<float> M_UpGradeRate_0_Arrays_2_Low =
	{ 0.f,0.f, 48.0, 32.f,   19.f,    16.f,    10.f,    8.f,    4.f,  1.0f,   1.0f,  1.0f,  1.0f ,  1.0f, 1.0f, 1.0f };
	UPROPERTY()
	TArray<float> M_UpGradeRate_0_Arrays_2_Mid =
	{ 0.f,0.f, 51.0, 36.f,   22.f,    18.f,    12.f,    9.f,    5.f,  2.f,    1.0f,  1.0f,  1.0f ,  1.0f, 1.0f, 1.0f };
	UPROPERTY()
	TArray<float> M_UpGradeRate_0_Arrays_2_Hei =
	{ 0.f,0.f, 60.0, 42.90f, 24.20f,  20.10f,  13.20f,  10.6f,  6.0f, 2.20f,  1.80f, 1.70f, 1.60f , 1.40f,1.3f, 1.0f };

	UPROPERTY()
	TArray<float> M_UpGradeRate_0_Arrays_1_Low =
	{ 0.f,72.0f, 56.0f, 48.0f,  32.0f,  27.0f, 21.0f,  17.0f, 10.0f,   3.0f,  3.0f,   3.0f,   3.0f ,  2.0f ,  2.0f,  2.0f,  1.0f ,0.0f };
	UPROPERTY()
	TArray<float> M_UpGradeRate_0_Arrays_1_Mid =
	{ 0.f,80.0f, 72.0f, 50.0f,  36.0f,  30.0f, 24.0f,  19.0f, 5.f,     12.0f, 4.0f,   3.0f,   3.0f,   3.0f ,  2.0f,  2.0f,  2.0f ,0.0f };
	UPROPERTY()
	TArray<float> M_UpGradeRate_0_Arrays_1_Hei =
	{ 0.f,88.0f, 79.2f, 55.0f,  40.3f,  33.0f, 26.40f, 21.2f, 13.2f,   4.50f, 4.60f,  4.3f,   4.0f ,  4.0f,   3.40f, 3.0f , 0.0f ,0.0f };

	UPROPERTY()
	TArray<float> M_UpGradeRate_0_Arrays_0_Low =
	{ 100.0f,  100.0f,  80.0f,   56.0f,    40.0f,    32.0f,   26.0f,    21.0f,     16.0f,    11.0f,     10.0f,     9.0f,     8.0f ,    8.0f ,     7.0f,  6.0f ,  0.0f };
	UPROPERTY()
	TArray<float> M_UpGradeRate_0_Arrays_0_Mid =
	{ 100.0f,  100.0f,  88.0f,   62.0f,    45.0f,    36.0f,   29.0f,    24.0f,     20.0f,    12.0f,     11.0f,     10.0f,    9.0f,     9.0f ,     8.0f,  7.0f ,  0.0f };
	UPROPERTY()
	TArray<float> M_UpGradeRate_0_Arrays_0_Hei =
	{ 100.0f,   100.0f, 96.80f,  68.60f,   49.50f,   39.60f,  31.90f,   26.40f,    22.0f,    13.50f,    12.50f,    11.60f,   10.7f,    10.1f ,    9.5f,  8.80f , 0.0f };

	UPROPERTY()
	TArray<float> M_UpGradeRate_0_Arrays_P1_Low =
	{ 100.0f,  100.0f, 100.0f,   72.0f,    72.0f,    72.0f,   72.0f,   72.0f,    72.0f,    72.0f,    72.0f,    72.0f,   72.0f ,   72.0f ,    72.0f,  72.0f ,72.0f };
	UPROPERTY()
	TArray<float> M_UpGradeRate_0_Arrays_P1_Mid =
	{ 100.0f,  100.0f, 100.0f,   80.0f,    80.0f,    80.0f,   80.0f,   80.0f,    80.0f,    80.0f,    80.0f,    80.0f,   80.0f,    80.0f,     80.0f,  80.0f ,80.0f };
	UPROPERTY()
	TArray<float> M_UpGradeRate_0_Arrays_P1_Hei =
	{ 100.0f,   100.0f, 100.0f,   88.0f,   88.0f,     88.0f,   88.0f,   88.0f,    88.0f,    88.0f,    88.0f,    88.0f,   88.0f,    88.0f,    88.0f,  88.0f, 88.0f };

	//保险金金额
	UPROPERTY()
	TArray<int32> LInsuranceCoinNum =
	{ 0,20,40,80,100,200,500,500,600,1000,3000,10000,18000,32000,45000,50000 ,0 };

	//强化金额
	UPROPERTY()
	TArray<int32> LUpCoinNum =
	{ 100,200,400,1000,2000,3000,6000,8000,9000,10000,30000,40000,48000,62000,75000,90000 ,0 };
};