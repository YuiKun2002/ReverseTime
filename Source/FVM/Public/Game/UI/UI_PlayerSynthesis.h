#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "GameSystem/Item/ItemBaseStruct.h"
#include "Game/UI/Synthesis/SynModel_Structures.h"
#include "UI_PlayerSynthesis.generated.h"

//【卡片界面显示与隐藏，装备界面】
#define UPS_REF_CARDS_E_PANEL TEXT("Cards_E_Panel")

/*
 合成屋-保险金界面
*/
UCLASS()
class FVM_API UUI_SynthesisSynInsurance : public UWidgetBase
{
	GENERATED_BODY()

public:

	//支付保险金
	UFUNCTION(BlueprintCallable)
	bool PaySynInsurance();

	//设置保险金的价格
	UFUNCTION(BlueprintCallable)
	void SetSynInsurancePrice(int32 Price);
	//设置保险金的选择
	UFUNCTION(BlueprintCallable)
	void SetSynInsuranceSelect(bool bSelect);

	//获取保险金的选择
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetSynInsuranceSelect() { return this->bSynInsuranceSelect; }

protected:
	//设置保险金价格
	UFUNCTION(BlueprintImplementableEvent)
	void SetSynInsurancePriceEvent(int32 Price);
	//设置保险金选择
	UFUNCTION(BlueprintImplementableEvent)
	void SetSynInsuranceSelectEvent(bool bSelect);

	//选择
	UPROPERTY()
	bool bSynInsuranceSelect = false;
	//支付金额
	UPROPERTY()
	int32 PayPrice = 0;
};


/*
 合成屋-主界面
*/
UCLASS()
class FVM_API UUI_PlayerSynthesis : public UWidgetBase
{

	GENERATED_BODY()

public:
	//设置显示公共区域
	UFUNCTION(BlueprintCallable)
	class UVerticalBox* SetCardAndGemsPanelShow(
		UWidgetBase* NewPanel,
		ESlateVisibility RightBigPanelVisibility = ESlateVisibility::Collapsed,//右侧大界面
		int32 RightBigPanelIndex = 0,//右侧界面索引:0卡片，1装备武器
		ESlateVisibility InsurancePanelVisibility = ESlateVisibility::Collapsed//保险金界面
	);


	//公共加载

	//左下材料区域
	UFUNCTION(BlueprintCallable)
	void LoadPublicMaterials(
		TArray<uint8> Types,
		TArray<FItemViewBlock> SelectBlock,
		FName OperateTypeName,
		bool bNotLoad = false
	);

	//右边卡片大界面
	UFUNCTION(BlueprintCallable)
	void LoadPublicCards(
		TArray<FItemViewBlock> SelectBlock,
		int32 InitGridType,
		bool bNotLoad = false
	);

	//右边装备大界面
	UFUNCTION(BlueprintCallable)
	void LoadPublicEquips(
		TArray<uint8> Types,
		TArray<FItemViewBlock> SelectBlock,
		FName OperateTypeName,
		bool bNotLoad = false
	);

public:

	/*操作界面获取*/
	UFUNCTION(BlueprintPure)
	class UWidgetBase* GetSynthesisOperaterWidget(FName WidgetName);

	/*通用界面获取*/

	//获取 保险金界面
	UFUNCTION(BlueprintPure)
	FORCEINLINE class UUI_SynthesisSynInsurance* GetSynInsurancePanel() { return this->SynInsurancePanel; }

	//获取 右上滚动框[层级0]
	UFUNCTION(BlueprintPure)
	FORCEINLINE	class ULoaderRangeWidget* GetRightTop_Loader() { return this->RightTop_Loader; }

	//获取 右下滚动框[层级0]
	UFUNCTION(BlueprintPure)
	FORCEINLINE	class ULoaderRangeWidget* GetRightBottom_Loader() { return this->RightBottom_Loader; }

	//获取 左下滚动框[层级0]
	UFUNCTION(BlueprintPure)
	FORCEINLINE	class ULoaderRangeWidget* GetLeftBottom_Loader() { return this->LeftBottom_Loader; }

	//获取 右边大界面滚动框1[层级1]
	UFUNCTION(BlueprintPure)
	FORCEINLINE	class ULoaderRangeWidget* GetRightAll_1_Loader() { return this->RightAll_1_Loader; }

	//获取 右边大界面滚动框2[层级2]
	UFUNCTION(BlueprintPure)
	FORCEINLINE	class ULoaderRangeWidget* GetRightAll_2_Loader() { return this->RightAll_2_Loader; }

protected:
	
	//显示帮助界面
	UFUNCTION(BlueprintCallable)
	void ShowHelpPanel();

	//卡片制作界面显示
	UFUNCTION(BlueprintCallable)
	void ShowMakeCardPanel();

	//卡片强化界面显示
	UFUNCTION(BlueprintCallable)
	void ShowUpgradeCardPanel();

	//卡片转职界面显示
	UFUNCTION(BlueprintCallable)
	void ShowTransferCardPanel();

	//武器宝石界面显示
	UFUNCTION(BlueprintCallable)
	void ShowWeaponAndGemPanel();

	//宝石强化界面显示
	UFUNCTION(BlueprintCallable)
	void ShowUpgradeGemPanel();

	//宝石强分解界面显示
	UFUNCTION(BlueprintCallable)
	void ShowSplitGemPanel();

	//金卡进化界面显示
	UFUNCTION(BlueprintCallable)
	void ShowGoldCardEvolvePanel();
protected:

	//----------------------------------四叶草+香料+转职材料【材料区域】--------------------------------------

	//材料加载器 
	UPROPERTY()
	FSynthesisItemLoader MaterialLoader;
	//创建材界面
	UWidget* WidgetCreate_InitMaterial(const class UItemLoadDataBlock* const _Data, int32 _Index);
	//刷新界面
	void WidgetRefresh_UpdateMaterial(const class UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget);
	//设置->材料数据->四叶草-转职道具-等【材料区域的统一材料设置】
	void SetMaterialsData(
		class UUI_SynItemGrid* Grid,
		const class UItemLoadDataBlock* const _CardData,
		int32 _Index
	);
	void WidgetRefresh_MaterialsLoadEnd();


	//----------------------------------右边大界面【卡片区域】--------------------------------------
	//卡片界面加载器(共用于 卡片强化，1卡片转职，2金卡进化等等)

	//卡片加载器 
	UPROPERTY()
	FSynthesisItemLoader CardLoader;
	//创建材界面
	UWidget* WidgetCreate_InitCards(const class UItemLoadDataBlock* const _Data, int32 _Index);
	//刷新界面
	void WidgetRefresh_UpdateCards(const class UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget);
	//加载结束
	void WidgetRefresh_CardsLoadEnd();
	//设置卡片数据
	void SetCardsData(
		class UUI_SynCardGrid* Grid,
		const UItemLoadDataBlock* const _CardData,
		int32 _Index
	);

	//----------------------------------右边大界面【装备区域】--------------------------------------
	//装备加载器 
	UPROPERTY()
	FSynthesisItemLoader EquipsLoader;

	//创建材界面
	UWidget* WidgetCreate_InitEquips(const class UItemLoadDataBlock* const Data, int32 Index);
	//刷新界面
	void WidgetRefresh_UpdateEquips(const class UItemLoadDataBlock* const Data, int32 Index, UWidget* Widget);
	//加载结束
	void WidgetRefresh_EquipsLoadEnd();
	//设置数据
	void SetEquipsData(
		class UUI_SynItemGrid* Grid,
		const UItemLoadDataBlock* const Data,
		int32 Index
	);

protected:

	//主要显示界面
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UVerticalBox* MainContentPanel = nullptr;

	//数据界面[右侧大界面]
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidget* RightBigPanel = nullptr;

	/*通用界面*/

	//保险金界面
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynthesisSynInsurance* SynInsurancePanel = nullptr;

	//右上滚动框[层级0]
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class ULoaderRangeWidget* RightTop_Loader = nullptr;

	//右下滚动框[层级0]
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class ULoaderRangeWidget* RightBottom_Loader = nullptr;

	//左下加载器
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class ULoaderRangeWidget* LeftBottom_Loader = nullptr;

	//右边半屏幕加载器1
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class ULoaderRangeWidget* RightAll_1_Loader = nullptr;

	//右边半屏幕加载器2
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class ULoaderRangeWidget* RightAll_2_Loader = nullptr;
private:
	//所有的主界面【操作界面核心】
	UPROPERTY()
	TMap<FName, UWidgetBase*> AllMainPanels;
	//当前显示的操作界面
	UPROPERTY()
	UWidgetBase* CurShowWidget = nullptr;


protected:

	//部分旧代码

	//金币文本
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Coin0Text = nullptr;
	//点券文本
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Coin1Text = nullptr;

	//[卡片界面主容器的子界面【卡片界面，装备界面】]
	UPROPERTY()
	TArray<UWidget*> M_TabWidgets_Panels;

	//加载器初始化
	UFUNCTION(BlueprintCallable)
	void Loader_Init();

	//更新金币文字
	UFUNCTION(BlueprintCallable)
	void Update_CoinText();
};
