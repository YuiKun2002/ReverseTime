// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "Game/UI/Synthesis/SynModel_Structures.h"
#include "Game/UI/Synthesis/SynthesisGeneralGrid.h"
#include "SynModel_GemSplit.generated.h"

/**
 *合成屋-宝石分解主界面
 */
UCLASS()
class FVM_API USynModel_GemSplit : public UWidgetBase, public ISynthesisWidgetInterface
{
	GENERATED_BODY()

public:

	//初始化
	virtual void InitWidget_Implementation() override;

	//需要宝石的选择
	UFUNCTION(BlueprintCallable)
	void Cancel();

	//选择宝石
	void SelectGem(class UItemGridBase* Gem);

	//加载宝石
	void LoadGem();

	//加载水晶
	void LoadCrystal();

	//分解
	UFUNCTION(BlueprintCallable)
	void SplitGem();

protected:

	//分解宝石的金币文本
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* SplitGemCoinText = nullptr;

	//当前需要被分解的宝石
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GeneralBox* BP_Syn_GemBox = nullptr;

	//分解的水晶数量显示
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UItemCountGridBase* BP_Syn_CrystalBox = nullptr;

	//分解宝石按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* SplitGem_Butt = nullptr;

private:
	UPROPERTY()
	class UUI_PlayerSynthesis* Syn = nullptr;

	//水晶分解数量
	const int32 SplitCrystalCount[17] = { 1,2,3,5,7,11,15,20,25,32,100,120,150,175,210,333,0 };
	//强化金额
	const int32 NeedCoin[17] = { 100,200,400,1000,2000,2500,3000,3500,4000,4500,5000,6000,7000,8000,9000,10000 ,0 };
};
