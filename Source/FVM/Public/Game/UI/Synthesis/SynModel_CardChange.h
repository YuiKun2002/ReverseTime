// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/UI/Synthesis/SynthesisGeneralGrid.h"
#include "SynModel_CardChange.generated.h"

/**
 *卡片转职-材料盒体
 */
UCLASS()
class FVM_API UUI_SynModel_TransferMaterialBox : public UUI_SynModel_GeneralBox {
	GENERATED_BODY()
};
/**
 *卡片转职-卡片盒体
 */
UCLASS()
class FVM_API UUI_SynModel_TransferCardBox : public UUI_SynModel_GeneralRectangleBox {
	GENERATED_BODY()
public:
	virtual void OnClicked_Implementation() override;
};


/**
 *卡片转职主界面
 */
UCLASS()
class FVM_API UUI_SynModel_TransferCardMainPanel : public UWidgetBase, public ISynthesisWidgetInterface
{
	GENERATED_BODY()

public:

	//初始化
	virtual void InitWidget_Implementation() override;

	//选择道具
	void SelectItem(const FItemViewBlock& Block, const FItemCountViewBlock& Count, int32 Index);

	//取消道具
	void CancelItem();

	//加载道具
	void LoadItem();

	//转职
	UFUNCTION(BlueprintCallable)
	void Transfer();

protected:
	//主要转职卡
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_TransferCardBox* TransferMainCardBox = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_TransferMaterialBox* TransferMaterialBox_A = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_TransferMaterialBox* TransferMaterialBox_B = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_TransferMaterialBox* TransferMaterialBox_C = nullptr;

	//卡片转职按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* TransferCard_Butt = nullptr;
	//转职花费
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TransferCardCoinText = nullptr;
	//转职概率
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TransferCardRateText = nullptr;
private:
	UPROPERTY()
	int32 CurSelectID = -1;
	UPROPERTY()
	int32 CurSelectLevel = -1;
	UPROPERTY()
	int32 TargetCardID = -1;
	UPROPERTY()
	int32 Coin = 0;
	UPROPERTY()
	float Rate = 30.f;

	//需要选择的转职材料
	TArray<FItemViewBlock> CurSelectTransferMaters;

	//保险金金额
	UPROPERTY()
	TArray<int32> LInsuranceCoinNum =
	{ 0,20,40,80,100,200,500,500,600,1000,3000,10000,18000,32000,45000,50000 ,0 };

	//金额
	UPROPERTY()
	TArray<int32> LCoinNum =
	{ 100,200,400,1000,2000,3000,6000,8000,9000,10000,30000,40000,48000,62000,75000,90000 ,0 };
};