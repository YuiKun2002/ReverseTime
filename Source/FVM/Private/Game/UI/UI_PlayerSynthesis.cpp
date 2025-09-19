// Fill out your copyright notice in the Description page of Project Settings.
#include "Game/UI/UI_PlayerSynthesis.h"

#include <Components/Image.h>
#include <Components/Button.h>
#include <Components/TextBlock.h>
#include <Components/ScrollBox.h>
#include <Components/VerticalBox.h>
#include <Components/CanvasPanel.h>

#include <Components/UniformGridPanel.h>//cpp
#include "GameSystem/PlayerStructManager.h"//cpp
#include "GameSystem/Tools/ItemLoadManager.h"//cpp
#include "Game/UI/Synthesis/UI_SynCardGrid.h"
#include "Game/UI/Synthesis/UI_SynItemGrid.h"
#include "GameSystem/Tools/GameSystemFunction.h"//cpp

#include "Game/UI/GeneralWidget/LoaderRangeWidget.h"

//金卡进化
#include "Game/UI/Synthesis/SynModel_GoldCardUp.h"

//材料基础结构
#include "GameSystem/Item/Material/MaterialBaseStruct.h"
#include "Game/UI/UI_PlayerBagMaterialGrid.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "GameSystem/FVMGameInstance.h"

bool UUI_SynthesisSynInsurance::PaySynInsurance() {

	if (this->bSynInsuranceSelect)
	{
		if (GET_PLAYERDATAINS()->CheckCoin(this->PayPrice, 2U))
		{
			return GET_PLAYERDATAINS()->ReduceCoin(this->PayPrice, 2U);
		}
	}

	return false;
}

void UUI_SynthesisSynInsurance::SetSynInsuranceSelect(bool bSelect) {
	this->bSynInsuranceSelect = bSelect;
	this->SetSynInsuranceSelectEvent(bSelect);
}

void UUI_SynthesisSynInsurance::SetSynInsurancePrice(int32 Price) {
	this->PayPrice = Price;
	this->SetSynInsurancePriceEvent(Price);
}

//初始化所有的加载器
void UUI_PlayerSynthesis::Loader_Init()
{
	if (this->M_TabWidgets_Panels.Num())
	{
		return;
	}
	/*----------------------------------------[(右边大界面)卡片界面主容器的子界面【卡片界面，装备界面】]----------------------------------------*/
	this->M_TabWidgets_Panels.Emplace(this->RightAll_1_Loader);
	this->M_TabWidgets_Panels.Emplace(this->RightAll_2_Loader);
	/*----------------------------------------[(右边大界面)卡片界面主容器的子界面【卡片界面，装备界面】]----------------------------------------*/

	//绑定角色货币更新
	GET_PLAYERDATAINS()->OnCoinUpdateDelegate.AddDynamic(this, &UUI_PlayerSynthesis::Update_CoinText);

	//材料加载器
	this->MaterialLoader.LoaderObject = this->LeftBottom_Loader->GetLoaderObject();
	this->MaterialLoader.LoaderObject->OnNew.BindUObject(this,
		&UUI_PlayerSynthesis::WidgetCreate_InitMaterial);
	this->MaterialLoader.LoaderObject->OnUpdate.BindUObject(this,
		&UUI_PlayerSynthesis::WidgetRefresh_UpdateMaterial);
	this->MaterialLoader.LoaderObject->OnLoadEnd.BindUObject(
		this, &UUI_PlayerSynthesis::WidgetRefresh_MaterialsLoadEnd
	);

	//卡片加载器
	this->CardLoader.LoaderObject = this->RightAll_1_Loader->GetLoaderObject();
	this->CardLoader.LoaderObject->OnNew.BindUObject(this,
		&UUI_PlayerSynthesis::WidgetCreate_InitCards);
	this->CardLoader.LoaderObject->OnUpdate.BindUObject(this,
		&UUI_PlayerSynthesis::WidgetRefresh_UpdateCards);
	this->CardLoader.LoaderObject->OnLoadEnd.BindUObject(
		this, &UUI_PlayerSynthesis::WidgetRefresh_CardsLoadEnd
	);

	//装备加载器
	this->EquipsLoader.LoaderObject = this->RightAll_2_Loader->GetLoaderObject();
	this->EquipsLoader.LoaderObject->OnNew.BindUObject(this,
		&UUI_PlayerSynthesis::WidgetCreate_InitEquips);
	this->EquipsLoader.LoaderObject->OnUpdate.BindUObject(this,
		&UUI_PlayerSynthesis::WidgetRefresh_UpdateEquips);
	this->EquipsLoader.LoaderObject->OnLoadEnd.BindUObject(
		this, &UUI_PlayerSynthesis::WidgetRefresh_EquipsLoadEnd
	);
}

//创建-材料区域-材料UI界面
UWidget* UUI_PlayerSynthesis::WidgetCreate_InitMaterial(const UItemLoadDataBlock* const _Data, int32 _Index)
{
	UUI_SynItemGrid* Grid = CREATE_UI(this, UUI_SynItemGrid, Synthesis, SynItemGrid);

	this->SetMaterialsData(Grid, _Data, _Index);

	return Grid;
}
//刷新-材料区域-材料UI界面
void UUI_PlayerSynthesis::WidgetRefresh_UpdateMaterial(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget)
{
	this->SetMaterialsData(Cast<UUI_SynItemGrid>(_UWidget), _Data, _Index);
}
//（香料，四叶草，等等）UI界面
void UUI_PlayerSynthesis::SetMaterialsData(UUI_SynItemGrid* Grid, const UItemLoadDataBlock* const _CardData, int32 _Index)
{
	Grid->SetInitType(1);
	Grid->SetOperateType(this->MaterialLoader.OperateTypeName);
	FItemViewBlock  BlockData = _CardData->GetValue<FItemViewBlock>();
	Grid->SetItemAttribute(GameDataStaticObject<UPlayerBagMaterialView>(), BlockData, _Index);
}

void UUI_PlayerSynthesis::WidgetRefresh_MaterialsLoadEnd() {

	TSet<int32> Id = MoveTempIfPossible(
		GameDataStaticObject<UPlayerBagMaterialView>()->GetLoaderIndexFromItemViewBlock(
			this->MaterialLoader.LoaderObject,
			this->MaterialLoader.SelectBlockDatas,
			this->MaterialLoader.NewLoadItems
		));

	for (const auto& Index : Id)
	{
		Cast<UUI_SynItemGrid>(
			this->LeftBottom_Loader->GetInternalArea()->GetChildAt(Index)
		)->SetButtonIsEnabled(false);
	}
}


/*加载--卡片界面（卡片制作，金卡进化，卡片转职）显示区域--开始*/
//加载卡片UI界面
UWidget* UUI_PlayerSynthesis::WidgetCreate_InitCards(const UItemLoadDataBlock* const _Data, int32 _Index)
{

	UUI_SynCardGrid* Grid = CREATE_UI(this, UUI_SynCardGrid, Synthesis, SynCardGrid);

	this->SetCardsData(Grid, _Data, _Index);

	return Grid;
}
//更新卡片UI界面
void UUI_PlayerSynthesis::WidgetRefresh_UpdateCards(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget)
{
	this->SetCardsData(Cast<UUI_SynCardGrid>(_UWidget), _Data, _Index);
}

void UUI_PlayerSynthesis::WidgetRefresh_CardsLoadEnd() {

	TSet<int32> Id = MoveTempIfPossible(
		GameDataStaticObject<UPlayerBagCardView>()->GetLoaderIndexFromItemViewBlock(
			this->CardLoader.LoaderObject,
			this->CardLoader.SelectBlockDatas,
			{}
		));

	for (const auto& Index : Id)
	{
		Cast<UUI_SynCardGrid>(
			this->RightAll_1_Loader->GetInternalArea()->GetChildAt(Index)
		)->SetButtonIsEnabled(false);
	}

}

void UUI_PlayerSynthesis::SetCardsData(
	class UUI_SynCardGrid* Grid,
	const UItemLoadDataBlock* const _CardData,
	int32 _Index
) {
	Grid->SetItemAttribute(
		GameDataStaticObject<UPlayerBagCardView>(),
		_CardData->GetValue<FItemViewBlock>(),
		_Index
	);

	Grid->SetCardOperateType(this->CardLoader.GridType);

	Grid->SetButtonIsEnabled(true);
}

UWidgetBase* UUI_PlayerSynthesis::GetSynthesisOperaterWidget(FName WidgetName) {
	UWidgetBase** Wid = this->AllMainPanels.Find(WidgetName);

	if (Wid)
	{
		return *(Wid);
	}

	return nullptr;
}


//界面显示
#define SYN_SHOW(ParentClass,PanelName){\
UWidgetBase** Widget = this->AllMainPanels.Find(GET_UI_NAME(Synthesis, PanelName));\
if (Widget)\
{this->CurShowWidget = *Widget; Cast<ISynthesisWidgetInterface>((*Widget))->Execute_InitWidget((*Widget));}else{\
UWidgetBase* NewWidget = CREATE_UI(ParentClass,UWidgetBase,Synthesis, PanelName);\
this->CurShowWidget = NewWidget;\
this->AllMainPanels.Emplace(GET_UI_NAME(Synthesis, PanelName), NewWidget);\
Cast<ISynthesisWidgetInterface>(NewWidget)->Execute_InitWidget(NewWidget);}\
}\
\

void UUI_PlayerSynthesis::ShowHelpPanel() {
	if (IsValid(this->CurShowWidget))
	{
		Cast<ISynthesisWidgetInterface>(
			(this->CurShowWidget)
		)->Execute_ShowHelpWidget((this->CurShowWidget));
	}
}

void UUI_PlayerSynthesis::ShowMakeCardPanel() {
	SYN_SHOW(this, MakeCard);
}

void UUI_PlayerSynthesis::ShowUpgradeCardPanel() {
	SYN_SHOW(this, UpgradeCard);
}

void UUI_PlayerSynthesis::ShowTransferCardPanel() {
	SYN_SHOW(this, TransferCard);
}

void UUI_PlayerSynthesis::ShowWeaponAndGemPanel() {
	SYN_SHOW(this, WeaponAndGem);
}

void UUI_PlayerSynthesis::ShowUpgradeGemPanel() {
	SYN_SHOW(this, UpgradeGem);
}

void UUI_PlayerSynthesis::ShowSplitGemPanel() {
	SYN_SHOW(this, SplitGem);
}

void UUI_PlayerSynthesis::ShowGoldCardEvolvePanel() {
	SYN_SHOW(this, GoldCardEvolve);
}

UVerticalBox* UUI_PlayerSynthesis::SetCardAndGemsPanelShow(
	UWidgetBase* NewPanel,
	ESlateVisibility RightBigPanelVisibility,//右侧大界面
	int32 RightBigPanelIndex,//右侧界面索引:0卡片，1装备武器
	ESlateVisibility InsurancePanelVisibility//保险金界面
) {
	//右侧大界面（最大的界面）
	this->RightBigPanel->SetVisibility(RightBigPanelVisibility);

	//右侧大界面（最大的界面）-> 0 表示显示卡片，2表示显示装备武器宝石等
	UGameSystemFunction::WidgetSelect<UWidget>(
		this->M_TabWidgets_Panels,
		ESlateVisibility::Visible,
		RightBigPanelIndex
	);

	//设置保险金界面
	this->SynInsurancePanel->SetVisibility(InsurancePanelVisibility);

	this->MainContentPanel->ClearChildren();
	this->MainContentPanel->AddChild(NewPanel);

	return this->MainContentPanel;
}

void UUI_PlayerSynthesis::LoadPublicMaterials(TArray<uint8> Types, TArray<FItemViewBlock> SelectBlock, FName OperateTypeName, bool bNotLoad) {

	this->MaterialLoader.SelectBlockDatas.Empty();
	this->MaterialLoader.SelectBlockDatas = MoveTempIfPossible(SelectBlock);
	this->MaterialLoader.OperateTypeName = OperateTypeName;

	if (bNotLoad)
	{
		return;
	}

	//视图
	TArray<FItemViewBlock> Blocks;
	this->MaterialLoader.NewLoadItems.Empty();

	GameDataStaticObject<UPlayerBagMaterialView>()
		->Arrangement(Types, this->MaterialLoader.NewLoadItems, false);

	//遍历  ID
	for (const auto& Data : this->MaterialLoader.NewLoadItems)
	{
		Data.Value.ToArrayZip(Data.Key, Blocks);
	}

	this->MaterialLoader.LoaderObject->UpdateDatatable(Blocks);
	this->MaterialLoader.LoaderObject->SetLoaderItemMaxCount(Blocks.Num());
	this->MaterialLoader.LoaderObject->Execute();
}

void UUI_PlayerSynthesis::LoadPublicCards(TArray<FItemViewBlock> SelectBlock, int32 InitGridType, bool bNotLoad) {
	this->CardLoader.SelectBlockDatas.Empty();
	this->CardLoader.SelectBlockDatas = MoveTempIfPossible(SelectBlock);

	this->CardLoader.GridType = InitGridType;

	if (bNotLoad)
	{
		return;
	}

	//获取卡片视图
	TArray<FItemViewBlock> CardViewBlock = MoveTempIfPossible(
		GameDataStaticObject<UPlayerBagCardView>()->GenerateItemView(
			GET_PLAYERDATAINS()->GetBagGridCount(1)
		));

	this->CardLoader.LoaderObject->UpdateDatatable(CardViewBlock);
	this->CardLoader.LoaderObject->SetLoaderItemMaxCount(CardViewBlock.Num());
	this->CardLoader.LoaderObject->Execute();
}

void UUI_PlayerSynthesis::LoadPublicEquips(
	TArray<uint8> Types,
	TArray<FItemViewBlock> SelectBlock,
	FName OperateTypeName,
	bool bNotLoad
) {
	this->EquipsLoader.SelectBlockDatas.Empty();
	this->EquipsLoader.SelectBlockDatas = MoveTempIfPossible(SelectBlock);
	this->EquipsLoader.OperateTypeName = OperateTypeName;

	if (bNotLoad)
	{
		return;
	}

	//获取角色存档
	UPlayerStructManager* Player = GET_PLAYERDATAINS();

	//视图
	TArray<FItemViewBlock> Equip;
	TArray<FItemViewBlock> Blocks;

	this->EquipsLoader.NewLoadItems.Empty();
	UPlayerBagEquipView* View = GameDataStaticObject<UPlayerBagEquipView>();
	View->Arrangement(Types, this->EquipsLoader.NewLoadItems, false);

	//遍历  ID
	for (const auto& Data : this->EquipsLoader.NewLoadItems)
	{
		//压缩数据
		Data.Value.ToArrayZipCheckEquip(
			Player, Data.Key, GET_PLAYERDATAINS()->GetbEquipList(),
			Equip, Blocks,
			GET_PLAYERDATAINS()->GetBagGridCount(0)
		);
	}

	this->EquipsLoader.LoaderObject->UpdateDatatable(Blocks);
	this->EquipsLoader.LoaderObject->SetLoaderItemMaxCount(Blocks.Num());
	this->EquipsLoader.LoaderObject->Execute();
}

//创建材界面
UWidget* UUI_PlayerSynthesis::WidgetCreate_InitEquips(const UItemLoadDataBlock* const Data, int32 Index) {
	UUI_SynItemGrid* Grid = CREATE_UI(this, UUI_SynItemGrid, Synthesis, SynItemGrid);
	this->SetEquipsData(Grid, Data, Index);
	return Grid;
}
//刷新界面
void UUI_PlayerSynthesis::WidgetRefresh_UpdateEquips(const UItemLoadDataBlock* const Data, int32 Index, UWidget* Widget) {
	this->SetEquipsData(Cast<UUI_SynItemGrid>(Widget), Data, Index);
}
//加载结束
void UUI_PlayerSynthesis::WidgetRefresh_EquipsLoadEnd() {

}
//设置数据
void UUI_PlayerSynthesis::SetEquipsData(
	class UUI_SynItemGrid* Grid,
	const UItemLoadDataBlock* const Data,
	int32 Index
) {
	Grid->SetInitType(0);
	Grid->SetOperateType(this->EquipsLoader.OperateTypeName);
	FItemViewBlock BlockData = Data->GetValue<FItemViewBlock>();
	Grid->SetItemAttribute(GameDataStaticObject<UPlayerBagEquipView>(), BlockData, Index);
}

//【更新显示】更新金币的文本
void UUI_PlayerSynthesis::Update_CoinText()
{
	this->Coin0Text->SetText(
		FText::FromString(FString::FromInt(GET_PLAYERDATAINS()->GetCoin(0).CoinValue))
	);
	this->Coin1Text->SetText(
		FText::FromString(FString::FromInt(GET_PLAYERDATAINS()->GetCoin(2).CoinValue))
	);
}