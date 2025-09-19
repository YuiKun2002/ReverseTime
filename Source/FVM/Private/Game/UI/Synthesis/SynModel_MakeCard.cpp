// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Synthesis/SynModel_MakeCard.h"
#include <Components/Image.h>
#include <Components/Button.h>
#include "Data/MaterialDataStruct.h"
#include "GameSystem/TaskSubsystem.h"
#include "GameSystem/Item/ItemStruct.h"
#include "Game/UI/UI_PlayerSynthesis.h"
#include <Components/UniformGridPanel.h>
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/Synthesis/UI_SynItemGrid.h"
#include "Game/UI/Synthesis/UI_SynCardGrid.h"
#include "Game/UI/GeneralWidget/LoaderRangeWidget.h"

//取消材料的选择
void UUI_SynModel_MakeCardBPMaterialBox::OnClicked_Implementation() {
	if (this->GetItemViewBlock().IsValid())
	{

		Cast<UUI_SynModel_MakeCard>(this->GetParentClass())->SetBlueprintMaterialID(nullptr);
		Cast<UUI_SynModel_MakeCard>(this->GetParentClass())->LoadBpMater();
	}
}

void UUI_SynModel_MakeCard::InitWidget_Implementation() {

	this->Syn = Cast<UUI_PlayerSynthesis>(this->GetParentClass());
	this->BP_Syn_MakeCardSpiceBox->SetParentClass(this);
	this->BP_Syn_MakeBPMater->SetParentClass(this);
	this->BP_Syn_MakeMater0->SetParentClass(this);
	this->BP_Syn_MakeMater1->SetParentClass(this);
	this->BP_Syn_MakeMater2->SetParentClass(this);

	if (!IsValid(this->MakeCard))
	{
		this->MakeCard = NewObject<USynModel_MakeCard>(this, TEXT("Synthesis_USynModel_MakeCard"));
	}
	this->SetBlueprintMaterialID(nullptr);
	this->MakeCard->InitializeBySynthesis(this->Syn);

	//显示界面
	this->Syn->SetCardAndGemsPanelShow(this);
}

void UUI_SynModel_MakeCard::LoadBpMater() {
	this->MakeCard->LoadMaterialsToMakeCard();
}

void UUI_SynModel_MakeCard::LoadMaterialSpice() {
	this->MakeCard->LoadSpicesToMakeCard();
}

void UUI_SynModel_MakeCard::SetBlueprintMaterialID(UItemCountGridBase* CopyGrid) {

	int32 NewID = IsValid(CopyGrid) ? CopyGrid->GetItemID() : -1;

	this->MakeCard->BlueprintMaterialID = NewID;

	//如果ID不一致，需要重新加载对应的格子
	if (NewID == -1)
	{
		this->MaterialsID.Empty();
		this->MakeCard_Butt->SetIsEnabled(false);

		//取消选择
		this->BP_Syn_MakeBPMater->Cancel();
		this->BP_Syn_MakeMater0->Cancel();
		this->BP_Syn_MakeMater1->Cancel();
		this->BP_Syn_MakeMater2->Cancel();
		this->BP_Syn_MakeMater2->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (this->BlueprintMaterialID != NewID)
	{

		//设置制作按钮的开启和禁用
		this->MakeCard_Butt->SetIsEnabled(true);

		//设置第三个材料的格子为隐藏
		this->BP_Syn_MakeMater2->SetVisibility(ESlateVisibility::Collapsed);

		//查询数据
		EMaterialType Type;
		FMaterialBase* BasePtr = nullptr;
		if (GET_CACHE(Material)->GetMaterData(NewID, Type, BasePtr))
		{
			this->MaterialsID = CAST_TYPE(FCardBlueprint, BasePtr).ItemsID;
			this->TargetID = CAST_TYPE(FCardBlueprint, BasePtr).TargetCardID;

			//查询并设置图标
			int32 Index = 0;
			for (const auto& ID : this->MaterialsID)
			{
				//设置图像
				this->SetGridStyle(Index, ID);
				Index++;
			}

			//设置合成配方的图像
			this->BP_Syn_MakeBPMater->SetItemAttributeCopy(CopyGrid);

		}
	}
	this->BlueprintMaterialID = NewID;
}

void UUI_SynModel_MakeCard::SetSpiceMaterialID(UItemCountGridBase* CopyGrid) {

	int32 NewID = IsValid(CopyGrid) ? CopyGrid->GetItemID() : -1;

	this->MakeCard->SpiceMaterialID = NewID;

	if (NewID == -1)
	{
		this->TargetLevel = 0;
	}
	else if (this->SpiceMaterialID != NewID) {
		//查询数据
		EMaterialType Type;
		FMaterialBase* BasePtr = nullptr;
		if (GET_CACHE(Material)->GetMaterData(NewID, Type, BasePtr))
		{
			this->TargetLevel = CAST_TYPE(FSpicesMaterial, BasePtr).M_MakeCardGrade;
			this->BP_Syn_MakeCardSpiceBox->SetItemAttributeCopy(CopyGrid);
		}
	}

	this->SpiceMaterialID = NewID;

	this->LoadMaterialSpice();
}

void UUI_SynModel_MakeCard::SetGridStyle(int32 Index, int32 ID) {

	auto SetGrid = [this](UUI_SynModel_MakeCardMaterialBox* Ins, int32 ID, int32 Index, bool bFind) {
		FItemViewBlock Block = { ID,0,Index };
		Ins->SetItemAttribute(GameDataStaticObject<UPlayerBagMaterialView>()
			, Block, Index);
		if (bFind)
		{
			Ins->SetButtonColorIsDark(false);
		}
		else {
			Ins->SetButtonColorIsDark();
		}
		};

	bool bFind = GameDataStaticObject<UPlayerBagMaterialView>()->GetItemTable().Contains(ID);

	//如果出现材料不齐全的情况，则无法合成
	if (!bFind)
	{
		this->MakeCard_Butt->SetIsEnabled(false);
	}

	//第一个图标
	if (Index == 0)
	{
		SetGrid(this->BP_Syn_MakeMater0, ID, Index, bFind);
	}

	//第二个图标
	if (Index == 1)
	{
		SetGrid(this->BP_Syn_MakeMater1, ID, Index, bFind);
	}

	//第三个图标
	if (Index == 2)
	{
		SetGrid(this->BP_Syn_MakeMater2, ID, Index, bFind);
		this->BP_Syn_MakeMater2->SetVisibility(ESlateVisibility::Visible);
	}
}

void UUI_SynModel_MakeCard::MakeCardBegin() {

	UPlayerStructManager* Player = GET_PLAYERDATAINS();

	if (!Player->CheckCoin(200))
	{
		UWidgetBase::CreateTipWidget(TEXT("金币不足，无法制作"));
		return;
	}

	//香料的判断
	if (this->SpiceMaterialID != -1)
	{
		if (Player->FindMaterialCount(this->SpiceMaterialID, 0, 5))
		{
			//消耗道具
			Player->ConsumeMaterial(this->SpiceMaterialID, 0, 5);
		}
	}
	else {
		this->TargetLevel = 0;
	}

	//消耗材料
	//查询配方材料
	int32 TempIndex = 0;
	for (const auto& ID : this->MaterialsID)
	{
		Player->ConsumeMaterial(ID, 0, 1);

		if (!Player->FindMaterialCount(ID, 0, 1))
		{
			this->SetGridStyle(TempIndex, ID);
		}

		TempIndex++;
	}

	//消耗配方
	Player->ConsumeMaterial(this->BlueprintMaterialID, 0, 1);

	//将道具发送到背包
	if (UGameSystemFunction::SendItemToBag(this->TargetID, this->TargetLevel, 1, 2U, false, true))
	{
		//执行任务
		UTaskSubsystem::GetTaskSubsystemStatic()->ExecuteTasks(this);
		//记录合成的卡片名称
		UPlayerRecord::AddCardMakeCount(1);
		UPlayerRecord::AddDayCardMakeCount(
			GameDataStaticObject<UPlayerBagCardView>()->GetItemName(this->TargetID)
		);
	}

	//香料的查询
	if (!Player->FindMaterialCount(this->SpiceMaterialID, 0, 5))
	{
		//取消香料
		this->BP_Syn_MakeCardSpiceBox->Cancel();
		this->SetSpiceMaterialID(nullptr);
	}

	//配方查询
	if (!Player->FindMaterialCount(this->BlueprintMaterialID, 0, 1))
	{
		this->BP_Syn_MakeBPMater->Cancel();
		this->SetBlueprintMaterialID(nullptr);
	}

	//减少货币
	Player->ReduceCoin(200);

	UGameSystemFunction::SaveCurrentPlayerData(TEXT("卡片合成"));

	//重新加载列表
	this->MakeCard->InitializeBySynthesis(this->Syn);
}



void USynModel_MakeCard::InitializeBySynthesis(UUI_PlayerSynthesis* Class)
{
	if (IsValid(this->PlayerSynthesis))
	{
		//加载UI
		this->WidgetResetLoadData();

		return;
	}

	this->PlayerSynthesis = Class;

	//初始化卡片制作加载器
	this->M_UItemLoaderObject_MakeCard = Class->GetRightBottom_Loader()->GetLoaderObject();
	this->M_UItemLoaderObject_MakeCard->OnNew.BindUObject(this,
		&USynModel_MakeCard::WidgetCreate_InitMakeCard);
	this->M_UItemLoaderObject_MakeCard->OnUpdate.BindUObject(this,
		&USynModel_MakeCard::WidgetRefresh_UpdateMakeCard);


	//初始化卡片制作-材料区域-加载器
	this->M_UItemLoaderObject_MakeCard_Materials = Class->GetRightTop_Loader()->GetLoaderObject();
	this->M_UItemLoaderObject_MakeCard_Materials->OnNew.BindUObject(this,
		&USynModel_MakeCard::WidgetCreate_InitMakeCard_Material);
	this->M_UItemLoaderObject_MakeCard_Materials->OnUpdate.BindUObject(this,
		&USynModel_MakeCard::WidgetRefresh_UpdateMakeCard_Material);


	//加载
	this->WidgetResetLoadData();
}


void USynModel_MakeCard::WidgetResetLoadData()
{
	this->LoadMaterialsToMakeCard();
	this->LoadCardsToMakeCard();
	this->LoadSpicesToMakeCard();
}

//创建界面-卡片制作-卡片显示区域
UWidget* USynModel_MakeCard::WidgetCreate_InitMakeCard(const  UItemLoadDataBlock* const _Data, int32 _Index)
{
	UUI_SynCardGrid* Grid = CREATE_UI(this->PlayerSynthesis, UUI_SynCardGrid, Synthesis, SynCardGrid);
	Grid->SetCardOperateType(-1);
	Grid->SetItemAttribute(
		GameDataStaticObject<UPlayerBagCardView>(),
		_Data->GetValue<FItemViewBlock>(),
		_Index
	);

	return Grid;
}
//更新界面-卡片制作-卡片显示区域
void USynModel_MakeCard::WidgetRefresh_UpdateMakeCard(const  UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget)
{
	Cast<UUI_SynCardGrid>(_UWidget)->SetItemAttribute(
		GameDataStaticObject<UPlayerBagCardView>(),
		_Data->GetValue<FItemViewBlock>(),
		_Index
	);
}

//更新界面-卡片制作-防御卡合成配方材料显示区域【显示，一般不需要】
void USynModel_MakeCard::WidgetRefresh_UpdateMakeCard_Material(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget)
{
	this->SetMakeCard_Material_PanelData(Cast<UUI_SynItemGrid>(_UWidget), _Data, _Index);
}

//创建界面-卡片制作-防御卡合成配方材料显示区域
UWidget* USynModel_MakeCard::WidgetCreate_InitMakeCard_Material(const UItemLoadDataBlock* const _Data, int32 _Index)
{
	UUI_SynItemGrid* Grid = CREATE_UI(this->PlayerSynthesis, UUI_SynItemGrid, Synthesis, SynItemGrid);
	this->SetMakeCard_Material_PanelData(Grid, _Data, _Index);
	return Grid;
}
//卡片制作界面-设置材料配方，配方材料UI界面
void USynModel_MakeCard::SetMakeCard_Material_PanelData(UUI_SynItemGrid* _Grid, const UItemLoadDataBlock* const _CardData, int32 _Index)
{
	//材料
	_Grid->SetInitType(1);

	_Grid->SetOperateType(GET_UI_NAME(Synthesis, MakeCard));

	const FItemViewBlock& BlockData = _CardData->GetValue<FItemViewBlock>();

	_Grid->SetItemAttribute(GameDataStaticObject<UPlayerBagMaterialView>(), BlockData, _Index);

	//设置按钮状态
	if (this->BlueprintMaterialID != -1 && this->BlueprintMaterialID == BlockData.ItemID)
	{
		_Grid->SetButtonIsEnabled(false);
	}
}

//加载材料-卡片制作界面（防御卡配方-配方材料-卡片制作界面）
void USynModel_MakeCard::LoadMaterialsToMakeCard()
{
	//视图
	TArray<FItemViewBlock> Blocks;
	TMap<int32, FBaseItemSave> NewItems;
	UPlayerBagMaterialView* View = GameDataStaticObject<UPlayerBagMaterialView>();
	View->Arrangement(
		TArray<uint8>{(uint8)EMaterialType::E_Blueprint, (uint8)EMaterialType::E_CardSynthesisMaterial},
		NewItems, false
	);

	//遍历  ID
	for (const auto& Data : NewItems)
	{
		Data.Value.ToArrayZip(Data.Key, Blocks);
	}

	this->M_UItemLoaderObject_MakeCard_Materials->UpdateDatatable(Blocks);
	this->M_UItemLoaderObject_MakeCard_Materials->SetLoaderItemMaxCount(Blocks.Num());
	this->M_UItemLoaderObject_MakeCard_Materials->Execute();
}
//加载卡片-卡片制作界面（防御卡-卡片制作界面）
void USynModel_MakeCard::LoadCardsToMakeCard()
{
	TArray<FItemViewBlock> Blocks =
		GameDataStaticObject<UPlayerBagCardView>()->GenerateItemView(
			GET_PLAYERDATAINS()->GetBagGridCount(1)
		);

	this->M_UItemLoaderObject_MakeCard->UpdateDatatable(Blocks);
	this->M_UItemLoaderObject_MakeCard->SetLoaderItemMaxCount(Blocks.Num());
	this->M_UItemLoaderObject_MakeCard->Execute();
}
//加载香料-卡片制作界面（材料区域-香料加载-卡片制作界面）
void USynModel_MakeCard::LoadSpicesToMakeCard()
{
	this->PlayerSynthesis->LoadPublicMaterials(
		{ (uint8)EMaterialType::E_Spices },
		{ {this->SpiceMaterialID,0,0} },
		GET_UI_NAME(Synthesis, MakeCard)
	);
}
