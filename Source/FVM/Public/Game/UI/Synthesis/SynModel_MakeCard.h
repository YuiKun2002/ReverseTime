// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "UObject/NoExportTypes.h"
#include "Game/UI/Synthesis/SynModel_Structures.h"
#include "Game/UI/Synthesis/SynthesisGeneralGrid.h"
#include "SynModel_MakeCard.generated.h"


/**
 *卡片制作-材料格子
 */
UCLASS()
class FVM_API UUI_SynModel_MakeCardMaterialBox : public UUI_SynModel_GeneralBox {

	GENERATED_BODY()
};

/**
 *卡片制作-材料配方格子
 */
UCLASS()
class FVM_API UUI_SynModel_MakeCardBPMaterialBox : public UUI_SynModel_GeneralBox {

	GENERATED_BODY()
public:
	virtual void OnClicked_Implementation() override;
};

/**
 *卡片制作主界面，制作
 */
UCLASS()
class FVM_API UUI_SynModel_MakeCard : public UWidgetBase, public ISynthesisWidgetInterface
{
	GENERATED_BODY()

public:

	//初始化
	virtual void InitWidget_Implementation() override;

	//加载配方
	void LoadBpMater();

	//加载香料
	void LoadMaterialSpice();

	//卡片制作
	UFUNCTION(BlueprintCallable)
	void MakeCardBegin();

public:

	//设置配方的选择
	UFUNCTION(BlueprintCallable)
	void SetBlueprintMaterialID(class UItemCountGridBase* CopyGrid);

	//设置香料的选择
	UFUNCTION(BlueprintCallable)
	void SetSpiceMaterialID(class UItemCountGridBase* CopyGrid);

	void SetGridStyle(int32 Index, int32 ID);

protected:
	//制作卡片显示的货币文本
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* MakeCardCoinText = nullptr;
	//制作成功的概率文本
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* MakeCardRateText = nullptr;

	//卡片制作按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* MakeCard_Butt = nullptr;
	//香料按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GeneralBox* BP_Syn_MakeCardSpiceBox = nullptr;

	//材料 1 - 3
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_MakeCardMaterialBox* BP_Syn_MakeMater0 = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_MakeCardMaterialBox* BP_Syn_MakeMater1 = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_MakeCardMaterialBox* BP_Syn_MakeMater2 = nullptr;

	//主材料配方
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_MakeCardBPMaterialBox* BP_Syn_MakeBPMater = nullptr;

protected:
	//合成配方ID
	UPROPERTY(BlueprintReadOnly)
	int32 BlueprintMaterialID = -1;
	//材料的ID
	UPROPERTY(BlueprintReadOnly)
	TSet<int32> MaterialsID;
	//目标卡片
	UPROPERTY(BlueprintReadOnly)
	int32 TargetID = -1;
	//目标等级
	UPROPERTY(BlueprintReadOnly)
	int32 TargetLevel = 0;
	//香料ID
	UPROPERTY(BlueprintReadOnly)
	int32 SpiceMaterialID = -1;
private:

	UPROPERTY()
	class UUI_PlayerSynthesis* Syn = nullptr;
	UPROPERTY()
	class USynModel_MakeCard* MakeCard = nullptr;
};

/**
 *卡片制作功能-格子显示
 */
UCLASS()
class FVM_API USynModel_MakeCard : public UObject
{
	GENERATED_BODY()

public:

	void InitializeBySynthesis(class UUI_PlayerSynthesis* Class);

	void WidgetResetLoadData();

	//加载材料-卡片制作界面
	UFUNCTION()
	void LoadMaterialsToMakeCard();
	//加载卡片-卡片制作界面
	UFUNCTION()
	void LoadCardsToMakeCard();
	//加载香料-卡片制作界面
	UFUNCTION()
	void LoadSpicesToMakeCard();

public:
	//合成配方ID
	UPROPERTY()
	int32 BlueprintMaterialID = -1;
	//香料ID
	UPROPERTY()
	int32 SpiceMaterialID = -1;

protected:

	//合成屋
	UPROPERTY()
	class UUI_PlayerSynthesis* PlayerSynthesis = nullptr;

	//加载器
	UPROPERTY()
	class UItemLoaderObject* M_UItemLoaderObject_MakeCard = nullptr;
	//创建材界面
	class UWidget* WidgetCreate_InitMakeCard(const  class  UItemLoadDataBlock* const _Data, int32 _Index);
	//刷新界面
	void WidgetRefresh_UpdateMakeCard(const  class  UItemLoadDataBlock* const   _Data, int32 _Index, class UWidget* _UWidget);

	//加载器
	UPROPERTY()
	class UItemLoaderObject* M_UItemLoaderObject_MakeCard_Materials = nullptr;
	//创建材界面
	class UWidget* WidgetCreate_InitMakeCard_Material(const class  UItemLoadDataBlock* const _Data, int32 _Index);
	//刷新界面
	void WidgetRefresh_UpdateMakeCard_Material(const class  UItemLoadDataBlock* const _Data, int32 _Index, class UWidget* _UWidget);
	//设置制作卡片界面-配方材料UI的数据
	void SetMakeCard_Material_PanelData(class UUI_SynItemGrid* _Grid, const class  UItemLoadDataBlock* const _CardData, int32 _Index);
};
