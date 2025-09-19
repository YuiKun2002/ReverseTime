// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/UI/UI_PlayerBag.h"
#include <Components/Button.h>
#include "Game/UI/UI_BagArrangement.h"
#include "Game/UI/UI_CardSkillTable.h"
#include "Game/UI/Tips/UI_ShopPayTip.h"
#include "Game/UI/UI_PlayerBagRemove.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "Game/UI/UI_PlayerInformationShow.h"
#include "Game/UI/UI_PlayerBagMaterialGrid.h"
#include "Game/UI/UI_PlayerBagEquipmentGrid.h"
#include "Game/UI/UI_PlayerBagEquipment_Bag_Grid.h"

#include "Data/EquipmentDataSturct.h"

#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerDataHandle.h"
#include "GameSystem/GameConfigSubsystem.h"
#include "GameSystem/PlayerData/LocalSkillBookDataHandle.h"

#include <Components/UniformGridPanel.h>
#include <Components/CanvasPanel.h>
#include <Components/TextBlock.h>



bool UPlayerBagPayObject::Pay_Implementation() {

	if (this->GetPayUI()->IsInViewport())
	{
		FItemPrice Data;
		this->GetPayUI()->GetItemPrice(Data);

		UPlayerStructManager* Player = GET_PLAYERDATAINS();

		//检测货币是否充足
		if (Player->CheckCoin(Data.M_ItemPrice, Data.M_ItemPriceType))
		{
			if (Player->ReduceCoin(Data.M_ItemPrice, Data.M_ItemPriceType))
			{
				FPlayerBagGird Grid;
				Grid.BlockIndex.BlockIndex = -1;

				switch (Data.ItemID)
				{
				case 0: { Player->M_PlayerBagGirdGroup.M_EquipmentGrid.Emplace(Grid); } break;
				case 1: { Player->M_PlayerBagGirdGroup.M_CardGrid.Emplace(Grid); }break;
				case 2: { Player->M_PlayerBagGirdGroup.M_MaterialGrid.Emplace(Grid); }break;
				}

				this->PayResultDelegate.Broadcast(true);

				return true;
			}
		}
	}

	this->PayResultDelegate.Broadcast(false);
	return false;
}

bool UUI_PlayerBag::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	//获取角色形象界面
	if (!this->M_PlayerInformationShow)
	{
		this->M_PlayerInformationShow = CREATE_UI(this, UUI_PlayerInformationShow, PlayerBag, PlayerInforPanel);

		if (IsValid(this->M_PlayerInformationShow))
		{
			this->PlayerPoint->AddChildToCanvas(this->M_PlayerInformationShow);

			if (!this->M_PlayerInformationShow->OnDataChangedDelegate.IsBound())
			{
				this->M_PlayerInformationShow->OnDataChangedDelegate.BindUFunction(this,
					TEXT("RefreshPlayerSuit"));
			}
		}
	}

	if (this->M_PlayerItemPanels.Num() == 0)
	{
		this->M_PlayerItemPanels.Emplace(this->GetWidgetComponent<UUniformGridPanel>(this, "PlayerItemList"));
		this->M_PlayerItemPanels.Emplace(this->GetWidgetComponent<UUniformGridPanel>(this, "PlayerItemList_1"));
		this->M_PlayerItemPanels.Emplace(this->GetWidgetComponent<UUniformGridPanel>(this, "PlayerItemList_2"));

		//背包物品删除按钮绑定
		this->RemoveItems_Butt->OnClicked.AddDynamic(this, &UUI_PlayerBag::OpenRemoveItemPanel);

		//背包格子的按钮绑定
		this->PlayerBag_0_CountButt->OnClicked.AddDynamic(this, &UUI_PlayerBag::BagGridButton_1_Func);
		this->PlayerBag_1_CountButt->OnClicked.AddDynamic(this, &UUI_PlayerBag::BagGridButton_2_Func);
		this->PlayerBag_2_CountButt->OnClicked.AddDynamic(this, &UUI_PlayerBag::BagGridButton_3_Func);
		this->PlayerBag_3_CountButt->OnClicked.AddDynamic(this, &UUI_PlayerBag::BagGridButton_4_Func);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//    技能书区域
	//---------------------------------------------------------------------------------------------------------------------
	if (this->M_SkillBookButtons.Num() == 0)
	{
		//绑定(选项卡)按钮组件
		this->M_SkillBookButtons.Emplace(this->GetWidgetComponent<UButton>(this, TEXT("Button_1")));
		this->M_SkillBookButtons.Emplace(this->GetWidgetComponent<UButton>(this, TEXT("Button_2")));
		this->M_SkillBookButtons.Emplace(this->GetWidgetComponent<UButton>(this, TEXT("Button_3")));
		this->M_SkillBookButtons.Emplace(this->GetWidgetComponent<UButton>(this, TEXT("Button_4")));
		//绑定对应功能
		this->M_SkillBookButtons[0]->OnClicked.AddDynamic(this, &UUI_PlayerBag::Open_SkillBooks_Normal);
		this->M_SkillBookButtons[1]->OnClicked.AddDynamic(this, &UUI_PlayerBag::Open_SkillBooks_Style0);
		this->M_SkillBookButtons[2]->OnClicked.AddDynamic(this, &UUI_PlayerBag::Open_SkillBooks_Style1);
		this->M_SkillBookButtons[3]->OnClicked.AddDynamic(this, &UUI_PlayerBag::Open_SkillBooks_Style2);
		//添加对应样式
		this->M_SkillBookDefStyle = {
		FString(TEXT("T_PB_22")),
		FString(TEXT("T_PB_27")) ,
		FString(TEXT("T_PB_29")) ,
		FString(TEXT("T_PB_31")) };
		this->M_SkillBookClickStyle = {
		FString(TEXT("T_PB_21")),
		FString(TEXT("T_PB_26")) ,
		FString(TEXT("T_PB_28")) ,
		FString(TEXT("T_PB_30")) };
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------------------

//选项卡按钮功能
//UUI_PlayerBag::LoadCardSkillBookTabs_DefCategory()   关联函数

void UUI_PlayerBag::Open_SkillBooks_Normal()
{
	this->PlayOperateAudio();
	UGameSystemFunction::TabSelect(this->M_SkillBookButtons,
		FString(TEXT("Texture2D'/Game/Resource/Texture/UI/Game/PlayerBag/")),
		this->M_SkillBookDefStyle, this->M_SkillBookClickStyle, 0);
	this->LoadCardSkillBookTabs(ESkillBookTipCategory::S_Normal);
}

void UUI_PlayerBag::Open_SkillBooks_Style0()
{
	this->PlayOperateAudio();
	UGameSystemFunction::TabSelect(this->M_SkillBookButtons,
		FString(TEXT("Texture2D'/Game/Resource/Texture/UI/Game/PlayerBag/")),
		this->M_SkillBookDefStyle, this->M_SkillBookClickStyle, 1);
	this->LoadCardSkillBookTabs(ESkillBookTipCategory::S_Style_1);
}

void UUI_PlayerBag::Open_SkillBooks_Style1()
{
	this->PlayOperateAudio();
	UGameSystemFunction::TabSelect(this->M_SkillBookButtons,
		FString(TEXT("Texture2D'/Game/Resource/Texture/UI/Game/PlayerBag/")),
		this->M_SkillBookDefStyle, this->M_SkillBookClickStyle, 2);
	this->LoadCardSkillBookTabs(ESkillBookTipCategory::S_Style_2);
}

void UUI_PlayerBag::Open_SkillBooks_Style2()
{
	this->PlayOperateAudio();
	UGameSystemFunction::TabSelect(this->M_SkillBookButtons,
		FString(TEXT("Texture2D'/Game/Resource/Texture/UI/Game/PlayerBag/")),
		this->M_SkillBookDefStyle, this->M_SkillBookClickStyle, 3);
	this->LoadCardSkillBookTabs(ESkillBookTipCategory::S_Style_3);
}
//------------------------------------------------------------------------------------------------------------------------


void UUI_PlayerBag::RefreshPlayerSuit()
{
	this->M_PlayerInformationShow->SetCurrentPlayer(UFVMGameInstance::GetPlayerStructManager_Static());
	this->M_PlayerInformationShow->RefreshPlayerSuit(false);
	this->PlayerBagEquipView->ClearCache();
}

int32 UUI_PlayerBag::SetBagGridStyle(int32 M_Count, const TArray<FPlayerBagGird>& _BagGrid)
{

	this->SetImageBrush(this->PlayerBag_0_CountImg,
		"Texture2D'/Game/Resource/Texture/UI/Game/PlayerBag/T_PB_10.T_PB_10'");
	this->SetImageBrush(this->PlayerBag_1_CountImg,
		"Texture2D'/Game/Resource/Texture/UI/Game/PlayerBag/T_PB_10.T_PB_10'");
	this->SetImageBrush(this->PlayerBag_2_CountImg,
		"Texture2D'/Game/Resource/Texture/UI/Game/PlayerBag/T_PB_10.T_PB_10'");
	this->SetImageBrush(this->PlayerBag_3_CountImg,
		"Texture2D'/Game/Resource/Texture/UI/Game/PlayerBag/T_PB_10.T_PB_10'");


	this->PlayerBag_0_CountShowText->SetText(FText());
	this->PlayerBag_1_CountShowText->SetText(FText());
	this->PlayerBag_2_CountShowText->SetText(FText());
	this->PlayerBag_3_CountShowText->SetText(FText());

	int32 GridCount = 150;
	if (M_Count > 0)
	{
		this->PlayerBag_0_CountShowText->SetText(FText::FromString(FString::FromInt(_BagGrid[0].M_GridCount)));
		this->SetImageBrush(this->PlayerBag_0_CountImg, _BagGrid[0].M_GridImagePath);
		GridCount += _BagGrid[0].M_GridCount;
	}
	if (M_Count > 1)
	{
		this->PlayerBag_1_CountShowText->SetText(FText::FromString(FString::FromInt(_BagGrid[1].M_GridCount)));
		this->SetImageBrush(this->PlayerBag_1_CountImg, _BagGrid[1].M_GridImagePath);
		GridCount += _BagGrid[1].M_GridCount;
	}
	if (M_Count > 2)
	{
		this->PlayerBag_2_CountShowText->SetText(FText::FromString(FString::FromInt(_BagGrid[2].M_GridCount)));
		this->SetImageBrush(this->PlayerBag_2_CountImg, _BagGrid[2].M_GridImagePath);
		GridCount += _BagGrid[2].M_GridCount;
	}
	if (M_Count > 3)
	{
		this->PlayerBag_3_CountShowText->SetText(FText::FromString(FString::FromInt(_BagGrid[3].M_GridCount)));
		this->SetImageBrush(this->PlayerBag_3_CountImg, _BagGrid[3].M_GridImagePath);
		GridCount += _BagGrid[3].M_GridCount;
	}

	return GridCount;
}

void UUI_PlayerBag::LoaderInit()
{
	//初始化卡片视图
	this->PlayerBagCardView = GameDataStaticObject<UPlayerBagCardView>();
	//初始化装备视图
	this->PlayerBagEquipView = GameDataStaticObject<UPlayerBagEquipView>();
	//初始化材料视图
	this->PlayerBagMaterialView = GameDataStaticObject<UPlayerBagMaterialView>();

	if (!IsValid(this->TimeLoad_Equipment_Manager))
	{
		//装备加载器
		this->TimeLoad_Equipment_Manager = this->CreateItemLoaderObject(
			Cast<UScrollBox>(this->M_PlayerItemPanels[0]->GetParent()),
			this->M_PlayerItemPanels[0],
			{ 6,7, GET_PLAYERDATAINS()->GetBagGridCount(0),0.001f,false }
		);
		this->TimeLoad_Equipment_Manager->OnNew.BindUObject(this, &UUI_PlayerBag::WidgetCreate_Equipment);
		this->TimeLoad_Equipment_Manager->OnUpdate.BindUObject(this, &UUI_PlayerBag::WidgetRefresh_Equipment);

		//卡片加载器
		this->TimeLoad_Cards_Manager = this->CreateItemLoaderObject(
			Cast<UScrollBox>(this->M_PlayerItemPanels[1]->GetParent()),
			this->M_PlayerItemPanels[1],
			{ 4,6, GET_PLAYERDATAINS()->GetBagGridCount(1),0.001f,false }
		);
		this->TimeLoad_Cards_Manager->OnNew.BindUObject(this, &UUI_PlayerBag::WidgetCreate_Cards);
		this->TimeLoad_Cards_Manager->OnUpdate.BindUObject(this, &UUI_PlayerBag::WidgetRefresh_Cards);

		//材料加载器
		this->TimeLoad_Materials_Manager = this->CreateItemLoaderObject(
			Cast<UScrollBox>(this->M_PlayerItemPanels[2]->GetParent()),
			this->M_PlayerItemPanels[2],
			{ 6,7, GET_PLAYERDATAINS()->GetBagGridCount(2),0.001f,false }
		);
		this->TimeLoad_Materials_Manager->OnNew.BindUObject(this, &UUI_PlayerBag::WidgetCreate_Materials);
		this->TimeLoad_Materials_Manager->OnUpdate.BindUObject(this, &UUI_PlayerBag::WidgetRefresh_Materials);

		//技能书加载器
		this->TimeLoad_CardSkillBooks_Manager = this->CreateItemLoaderObject(
			this->PlayerBag_SkillBooks_Scroll,
			this->PlayerBag_SkillBooks_Uniform,
			{ 7,1, 999,0.001f,false }
		);
		this->TimeLoad_CardSkillBooks_Manager->OnNew.BindUObject(this, &UUI_PlayerBag::WidgetCreate_CardSkillBooks);
		this->TimeLoad_CardSkillBooks_Manager->OnUpdate.BindUObject(this, &UUI_PlayerBag::WidgetRefresh_CardSkillBooks);
	}
}

void UUI_PlayerBag::RelaodCurrentItemData() {
	switch (this->M_Panel_Index)
	{
	case 0U: {
		if (IsValid(this->PlayerBagEquipView))
		{
			this->PlayerBagEquipView->ClearCache();
			this->LoadItemsData();
		}
	}; break;
	case 1U: {
		if (IsValid(this->PlayerBagCardView))
		{
			this->PlayerBagCardView->ClearCache();
			this->LoadCardData();
		}
	}; break;
	case 2U: {
		if (IsValid(this->PlayerBagMaterialView))
		{
			this->PlayerBagMaterialView->ClearCache();
			this->LoadMaterialData();
		}
	}; break;
	}

	this->UpdateShowCoinText();
}

UWidget* UUI_PlayerBag::WidgetCreate_Equipment(const UItemLoadDataBlock* const _Data, int32 _Index)
{
	UUI_PlayerBagEquipmentGrid* Grid = CREATE_UI(this, UUI_PlayerBagEquipmentGrid, PlayerBag, EquipmentGrid);

	// 

	Grid->SetItemAttribute(this->PlayerBagEquipView, _Data->GetValue<FItemViewBlock>(), _Index);

	this->UpdateBagCount(0u, this->TimeLoad_Equipment_Manager->GetCount());

	return Grid;
}

void UUI_PlayerBag::WidgetRefresh_Equipment(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget)
{
	Cast<UUI_PlayerBagEquipmentGrid>(_UWidget)->SetItemAttribute(
		this->PlayerBagEquipView, _Data->GetValue<FItemViewBlock>(), _Index
	);

	this->UpdateBagCount(0u, this->TimeLoad_Equipment_Manager->GetCount());
}

UWidget* UUI_PlayerBag::WidgetCreate_Cards(const UItemLoadDataBlock* const _Data, int32 _Index)
{
	UUI_PlayerBagCardGrid* M_TempCardGrid = CREATE_UI(this, UUI_PlayerBagCardGrid, PlayerBag, CardGrid);

	M_TempCardGrid->SetItemAttribute(this->PlayerBagCardView, _Data->GetValue<FItemViewBlock>(), _Index);

	this->UpdateBagCount(1u, this->TimeLoad_Cards_Manager->GetCount());

	return M_TempCardGrid;
}

void UUI_PlayerBag::WidgetRefresh_Cards(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget)
{

	Cast<UUI_PlayerBagCardGrid>(_UWidget)->SetItemAttribute(
		this->PlayerBagCardView, _Data->GetValue<FItemViewBlock>(), _Index
	);

	this->UpdateBagCount(1u, this->TimeLoad_Cards_Manager->GetCount());
}

UWidget* UUI_PlayerBag::WidgetCreate_Materials(const UItemLoadDataBlock* const _Data, int32 _Index)
{
	UUI_PlayerBagMaterialGrid* Grid = CREATE_UI(this, UUI_PlayerBagMaterialGrid, PlayerBag, MaterialGrid);

	Grid->SetItemAttribute(
		this->PlayerBagMaterialView, _Data->GetValue<FItemViewBlock>(), _Index
	);

	this->UpdateBagCount(2u, this->TimeLoad_Materials_Manager->GetCount());

	return Grid;
}
void UUI_PlayerBag::WidgetRefresh_Materials(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget)
{

	Cast<UUI_PlayerBagMaterialGrid>(_UWidget)->SetItemAttribute(
		this->PlayerBagMaterialView, _Data->GetValue<FItemViewBlock>(), _Index
	);

	this->UpdateBagCount(2u, this->TimeLoad_Materials_Manager->GetCount());
}

//创建防御卡技能书道具界面
UWidget* UUI_PlayerBag::WidgetCreate_CardSkillBooks(const UItemLoadDataBlock* const _Data, int32 _Index)
{
	UUI_CardSkillTable* Skill_Table = CREATE_UI(this, UUI_CardSkillTable, PlayerBag, SkillBookTab);



	Skill_Table->InitData(_Data->GetValue<FSkillBookData>());

	return Skill_Table;
}
//刷新防御卡技能书道具界面
void UUI_PlayerBag::WidgetRefresh_CardSkillBooks(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget)
{
	Cast<UUI_CardSkillTable>(_UWidget)->InitData(
		_Data->GetValue<FSkillBookData>()
	);
}

void UUI_PlayerBag::LoadPlayerBags(UUI_PlayerBag* _Class, TArray<FItemViewBlock>& _Items_, int32 _Index)
{
	int32 Count = _Class->PlayerBagEquipPanel_Uniform->GetAllChildren().Num();
	int32 ItemCount = _Items_.Num();

	if (_Index < Count)
	{
		UUI_PlayerBagEquipment_Bag_Grid* Grid =
			Cast<UUI_PlayerBagEquipment_Bag_Grid>(_Class->PlayerBagEquipPanel_Uniform->GetAllChildren()[_Index]);

		//得到视图块
		FItemViewBlock& BlockData = _Items_[_Index];

		//设置数据视图块
		Grid->SetItemAttribute(this->PlayerBagEquipView, BlockData, _Index);

		Grid->SetVisibility(ESlateVisibility::Visible);
	}
	else {

		//创建背包格子
		UUI_PlayerBagEquipment_Bag_Grid* Grid = CREATE_UI(this,
			UUI_PlayerBagEquipment_Bag_Grid, PlayerBag, Equipment_Bag_Grid);
		//得到视图块
		FItemViewBlock& BlockData = _Items_[_Index];
		//设置数据视图块
		Grid->SetItemAttribute(this->PlayerBagEquipView, BlockData, _Index);

		//添加到格子列表
		_Class->PlayerBagEquipPanel_Uniform->AddChildToUniformGrid(Grid,
			(_Index / 5), _Index - 5 * (_Index / 5));
	}

	//更新背包格子数量显示  如果加载的个数到达最大值，剩余的则不会加载
	if (this->UpdateBagCount(this->M_Panel_Index, _Index + 1) == _Index + 1)
	{
		this->TimeLoad_PlayerBags.Stop();
	}

}


void UUI_PlayerBag::LoadCardSkillBookTabs(ESkillBookTipCategory M_ESkillBookTipCategory)
{
	//重置索引
	this->M_FItemData_Show_CardSkillBooks.Empty();
	this->M_CardSkillBooksCategory = M_ESkillBookTipCategory;

	ULocalPlayerDataHandle* Hd = UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->GetLocalPlayerDataHandle();
	ULocalSkillBookDataHandle* SkillHd = Cast<ULocalSkillBookDataHandle>(
		Hd->LoadPlayerDataPageHandle(EPlayerDataConfigPage::SkillBook)
	);
	if (IsValid(SkillHd))
	{
		ULocalSkillBookDataSaveGame* SkillSaveGame = Cast<ULocalSkillBookDataSaveGame>(
			SkillHd->Execute_GetData(SkillHd)
		);

		SkillSaveGame->GenSkillBookListForWidget(
			this->M_CardSkillBooksCategory,
			this->M_FItemData_Show_CardSkillBooks);

		this->TimeLoad_CardSkillBooks_Manager->UpdateDatatable(this->M_FItemData_Show_CardSkillBooks);
		this->TimeLoad_CardSkillBooks_Manager->SetLoaderItemMaxCount(this->M_FItemData_Show_CardSkillBooks.Num());
		this->TimeLoad_CardSkillBooks_Manager->SetLoaderPage(0);
		this->TimeLoad_CardSkillBooks_Manager->Execute();
	}
}


void UUI_PlayerBag::LoadCardSkillBookTabs_DefCategory()
{
	this->LoadCardSkillBookTabs(this->M_CardSkillBooksCategory);

	switch (this->M_CardSkillBooksCategory)
	{
	case ESkillBookTipCategory::S_Normal:; break;
	case ESkillBookTipCategory::S_Style_1:; break;
	case ESkillBookTipCategory::S_Style_2:; break;
	case ESkillBookTipCategory::S_Style_3:; break;
	}
}

void UUI_PlayerBag::BagGridButton_BuyGrid(uint8 _Panel_Index, uint8 _Gird_Index, int32 _Price)
{
	FItemPrice Price;
	Price.M_ItemPriceType = 1;//礼券
	Price.M_ItemPrice = _Price;//价格
	Price.ItemID = _Panel_Index;//购买类型
	Price.PayObject = NewObject<UPlayerBagPayObject>();//自定义支付规则

	//调出支付界面
	UUI_ShopPayTip* PayPanel = GET_UI_CAST(UUI_ShopPayTip, Global, Pay);

	PayPanel->SetItemPrice(Price);
	PayPanel->SetPayTitle(
		FText::FromString(FString(TEXT("开启背包格子第") + FString::FromInt(_Gird_Index) + TEXT("格")))
	);
	PayPanel->SetPayContent(
		FText::FromString(FString(TEXT("开启格子之后,你可以装备一个背包用于扩充你的存储格子")))
	);
	PayPanel->SetPayPrice(FText::FromString(FString::FromInt(_Price)));
	PayPanel->SetPayCoinName(FText::FromString(TEXT("礼券")));

	PayPanel->SetInputHidden();
	PayPanel->AddToViewport();

	//绑定支付回调
	if (IsValid(PayPanel->GetPayObject()))
	{
		Cast<UPlayerBagPayObject>(PayPanel->GetPayObject())
			->PayResultDelegate.AddDynamic(this,
				&UUI_PlayerBag::BagGridButton_BuyGridResult);
	}
}

void UUI_PlayerBag::BagGridButton_BuyGridResult(bool bResult) {
	if (bResult)
	{
		//重新加载当前界面
		this->SetBagGridStyle(
			this->GetBagGridData(this->M_Panel_Index).Num(),
			this->GetBagGridData(this->M_Panel_Index));

		//提示
		UWidgetBase::CreateTipWidget(TEXT("背包格子+1"));
	}

	this->UpdateShowCoinText();
}


void UUI_PlayerBag::BagGridButton_EquipAndUnEquip(uint8 _PanelIndex, uint8 _BagNumber, int32 _Price)
{
	//通过界面索引，获取对应界面的四个背包格子
	TArray<FPlayerBagGird>& Grid = this->GetBagGridData(_PanelIndex);

	UPlayerStructManager* Player = GET_PLAYERDATAINS();

	//如果当前背包格子数量大于当前选择的背包号码，表示当前背包格子处于解锁状态
	if (Grid.Num() > _BagNumber - 1)
	{
		//判断当前格子是否为空格子[打开背包装备界面]
		if (Grid[_BagNumber - 1].BlockIndex.BlockIndex == -1)
		{
			//设置选择的装备背包索引
			this->M_SelectEquipBag_Index = _BagNumber - 1;

			//重置背包数据列表
			this->PlayerBagListTableNormalArray.Empty();
			TMap<int32, FBaseItemSave> PlayerBagListTable;

			this->PlayerBagEquipView->GenerateItemViewSourceByType(EEquipment::E_Bag, PlayerBagListTable);
			TArray<FItemViewBlock> bEquipArray;
			for (auto& Data : PlayerBagListTable)
			{
				//初始化可装备的背包道具
				Data.Value.ToArrayZipCheckEquip(
					Player,
					Data.Key,
					this->PlayerBagEquipView->GenerateEuipItemListTable()
					, bEquipArray, this->PlayerBagListTableNormalArray);
			}

			//隐藏所有的格子
			for (auto UniformGrid : this->PlayerBagEquipPanel_Uniform->GetAllChildren())
			{
				UniformGrid->SetVisibility(ESlateVisibility::Collapsed);
			}

			//加载背包列表
			this->TimeLoad_PlayerBags.SetClass(this, &UUI_PlayerBag::LoadPlayerBags);
			this->TimeLoad_PlayerBags.SetDataList(this->PlayerBagListTableNormalArray);
			this->TimeLoad_PlayerBags.Run(0.001f);

			//打开背包额外界面界面
			this->PlayerBagEquipPanel->SetVisibility(ESlateVisibility::Visible);
		}
		else {
			//背包索引
			FPlayerBagGird& Block = Grid[_BagNumber - 1];
			//从装备栏卸载【卸载背包】
			Player->AddEquipItem(
				Block.BlockIndex.ItemID,
				Block.BlockIndex.ItemLevel,
				Block.BlockIndex.BlockIndex, false);
			//清理数据
			Block.BlockIndex.BlockIndex = -1;
			Block.BlockIndex.ItemID = -1;
			Block.BlockIndex.ItemLevel = -1;
			Block.M_GridCount = 0;
			Block.M_GridImagePath = FString();
			//设置背包风格
			this->SetBagGridStyle(
				this->GetBagGridData(this->M_Panel_Index).Num(),
				this->GetBagGridData(this->M_Panel_Index)
			);

			//在其他界面，卸载背包道具的时候，需要清理缓存，切换到背包栏时才会正常显示
			if (_PanelIndex != 0U)
			{
				GameDataStaticObject<UPlayerBagEquipView>()->ClearCache();
			}

			//重新加载数据
			this->RelaodCurrentItemData();
		}
	}
	else {
		//如果当前的拥有的背包格子号码 与 选择的背包号码相同，表示可以进行支付货币来解锁当前背包
		if (Grid.Num() == _BagNumber - 1)
			//调出支付界面
		{
			this->BagGridButton_BuyGrid(_PanelIndex, _BagNumber, _Price);
		}
	}
}

void UUI_PlayerBag::BagGridButton_1_Func()
{
	this->BagGridButton_EquipAndUnEquip(this->M_Panel_Index, 1, 500);
}

void UUI_PlayerBag::BagGridButton_2_Func()
{
	this->BagGridButton_EquipAndUnEquip(this->M_Panel_Index, 2, 1000);
}

void UUI_PlayerBag::BagGridButton_3_Func()
{
	this->BagGridButton_EquipAndUnEquip(this->M_Panel_Index, 3, 2000);
}

void UUI_PlayerBag::BagGridButton_4_Func()
{
	this->BagGridButton_EquipAndUnEquip(this->M_Panel_Index, 4, 5000);
}

void UUI_PlayerBag::ArrangementItem() {

	//获取玩家背包
	UPlayerStructManager* Player = GET_PLAYERDATAINS();

	//获取配置
	UGameConfigManager* Config = UGameConfigSubsystem::GetGameConfigSubsystemStatic()->GetGameConfigInstance();

	//初始化整理配置
	GET_UI_CAST(UUI_BagArrangement, PlayerBag, Arrangement)->InitBagArrangement(this->M_Panel_Index, false);

	FGamePlayerItemArrangeConfig* ArrConfig = nullptr;

	switch (this->M_Panel_Index)
	{
	case 1: {
		ArrConfig = Config->ArrangeConfig.Find(GET_DEF_NAME(Data, CardArrange));
		if (ArrConfig)
		{
			TMap<int32, FBaseItemSave> NewItems;
			this->PlayerBagCardView->Arrangement(
				ArrConfig->Types,
				NewItems,
				true
			);
			Player->GetCardList().Empty();
			Player->GetCardList().Append(NewItems);
		}
	}break;
	case 2: {
		ArrConfig = Config->ArrangeConfig.Find(GET_DEF_NAME(Data, MaterialArrange));
		if (ArrConfig)
		{
			TMap<int32, FBaseItemSave> NewItems;
			this->PlayerBagMaterialView->Arrangement(
				ArrConfig->Types,
				NewItems,
				true
			);
			Player->GetMaterList().Empty();
			Player->GetMaterList().Append(NewItems);
		}
	}break;
	default:
	{
		ArrConfig = Config->ArrangeConfig.Find(GET_DEF_NAME(Data, EquipArrange));
		if (ArrConfig)
		{
			TMap<int32, FBaseItemSave> NewItems;
			this->PlayerBagEquipView->Arrangement(
				ArrConfig->Types,
				NewItems,
				true
			);
			Player->GetSourceEquipList().Empty();
			Player->GetSourceEquipList().Append(NewItems);
		}
	}break;
	}

	UGameSystemFunction::SaveCurrentPlayerData(TEXT("物品整理"));

	this->RelaodCurrentItemData();
}

void UUI_PlayerBag::ArrangementItemSetting() {
	GET_UI_CAST(UUI_BagArrangement, PlayerBag, Arrangement)->InitBagArrangement(this->M_Panel_Index, true);
}

TArray<FPlayerBagGird>& UUI_PlayerBag::GetBagGridData(uint8 _PanelNumber)
{
	//判断当前格子是否拥有
	FPlayerBagGirdGroup& Grid = UFVMGameInstance::GetFVMGameInstance()->GetPlayerStructManager()->M_PlayerBagGirdGroup;

	switch (_PanelNumber)
	{
	case 0: return Grid.M_EquipmentGrid;
	case 2: return Grid.M_MaterialGrid;
	}
	return Grid.M_CardGrid;
}

void UUI_PlayerBag::LoadItemsData()
{
	this->UpdateShowCoinText();

	this->TimeLoad_PlayerBags.Stop();

	this->M_Panel_Index = 0;

	UGameSystemFunction::WidgetSelect(this->M_PlayerItemPanels, ESlateVisibility::Visible, 0);

	for (auto Panel : this->M_PlayerItemPanels[0]->GetAllChildren())
	{
		Panel->SetVisibility(ESlateVisibility::Collapsed);
	}

	//解析装备
	this->TimeLoad_Equipment_Manager->UpdateDatatable(
		this->PlayerBagEquipView->GenerateItemView(
			GET_PLAYERDATAINS()->M_PlayerBagGirdGroup.GetBagCount(0)
		)
	);

	this->TimeLoad_Equipment_Manager->Execute();
}

void UUI_PlayerBag::LoadCardData()
{
	this->M_Panel_Index = 1;

	UGameSystemFunction::WidgetSelect(this->M_PlayerItemPanels, ESlateVisibility::Visible, 1);

	for (auto Panel : this->M_PlayerItemPanels[1]->GetAllChildren())
	{
		Panel->SetVisibility(ESlateVisibility::Collapsed);
	}

	//解析卡片
	this->TimeLoad_Cards_Manager->UpdateDatatable(
		this->PlayerBagCardView->GenerateItemView(
			GET_PLAYERDATAINS()->M_PlayerBagGirdGroup.GetBagCount(1)
		)
	);

	this->TimeLoad_Cards_Manager->Execute();
}

void UUI_PlayerBag::LoadMaterialData()
{
	this->M_Panel_Index = 2;

	UGameSystemFunction::WidgetSelect(this->M_PlayerItemPanels, ESlateVisibility::Visible, 2);

	for (auto Panel : this->M_PlayerItemPanels[2]->GetAllChildren())
	{
		Panel->SetVisibility(ESlateVisibility::Collapsed);
	}

	this->TimeLoad_Materials_Manager->UpdateDatatable(
		this->PlayerBagMaterialView->GenerateItemView(
			GET_PLAYERDATAINS()->M_PlayerBagGirdGroup.GetBagCount(2)
		)
	);

	this->TimeLoad_Materials_Manager->Execute();
}

void UUI_PlayerBag::UpdateShowCoinText()
{
	this->Coin_0->SetText(FText::FromString(FString::FromInt(GET_PLAYERDATAINS()->GetCoin(0).CoinValue)));
	this->Coin_1->SetText(FText::FromString(FString::FromInt(GET_PLAYERDATAINS()->GetCoin(1).CoinValue)));
	this->Coin_2->SetText(FText::FromString(FString::FromInt(GET_PLAYERDATAINS()->GetCoin(2).CoinValue)));
}

uint8 UUI_PlayerBag::GetSelectPanelIndex()
{
	return this->M_Panel_Index;
}

int32 UUI_PlayerBag::GetSelectEquipBagIndex() {
	return this->M_SelectEquipBag_Index;
}

void UUI_PlayerBag::ClosePlayerBagEquipPanel()
{
	this->PlayerBagEquipPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UUI_PlayerBag::OpenRemoveItemPanel()
{
	//初始化界面UI
	if (this->M_UUI_PlayerBagRemove_Panels.Num() != 3)
	{
		//绑定的函数
		FString LBindFunc[3] = { TEXT("SearchBag_Equipment"),TEXT("SearchBag_Card"),TEXT("SearchBag_Materials") };

		//初始化界面
		for (int32 i = 0; i < 3; i++)
		{
			this->M_UUI_PlayerBagRemove_Panels.Emplace(CreateWidget<UUI_PlayerBagRemove>(this, UAssetManager::GetStreamableManager().LoadSynchronous<UClass>(FSoftClassPath(FString(TEXT("WidgetBlueprint'/Game/Resource/BP/Game/UI/MainFrame/BP_PlayerBagRemoveItems.BP_PlayerBagRemoveItems_C'"))))));
		}

		//初始化界面数据据
		for (int32 i = 0; i < 3; i++)
		{
			//初始化背包UI
			this->M_UUI_PlayerBagRemove_Panels[i]->M_UIBag = this;
			//添加代理函数
			FScriptDelegate CallFunc;
			CallFunc.BindUFunction(this->M_UUI_PlayerBagRemove_Panels[i], *LBindFunc[i]);
			this->M_UUI_PlayerBagRemove_Panels[i]->GetSearchButton()->OnClicked.Add(CallFunc);
			//初始化加载器
			this->M_UUI_PlayerBagRemove_Panels[i]->InitLoader(i);
		}
	}

	//加载显示界面
	auto LoadWidget = [&](TArray<UUI_PlayerBagRemove*>& _UUI_PlayerBagRemove_Panels, int32 _Index) {
		_UUI_PlayerBagRemove_Panels[_Index]->SearchBag(_Index);
		_UUI_PlayerBagRemove_Panels[_Index]->AddToViewport();
		};

	//根据当前界面索引显示对应界面
	switch (this->GetSelectPanelIndex())
	{
	case 0:LoadWidget(this->M_UUI_PlayerBagRemove_Panels, this->GetSelectPanelIndex()); break;
	case 1:LoadWidget(this->M_UUI_PlayerBagRemove_Panels, this->GetSelectPanelIndex()); break;
	case 2:LoadWidget(this->M_UUI_PlayerBagRemove_Panels, this->GetSelectPanelIndex()); break;
	}
}

int32 UUI_PlayerBag::UpdateBagCount(uint8 _UpdatePanel, int32 Count)
{
	//获取玩家购买的背包个数最多4个
	int32 _Count = this->GetBagGridData(_UpdatePanel).Num();

	//获取背包总容量
	int32 _GridCount = this->SetBagGridStyle(_Count, this->GetBagGridData(_UpdatePanel));

	//更新文字
	this->PlayerBagCountShowText->SetText(FText::FromString(FString::FromInt(Count) +
		" / " + FString::FromInt(_GridCount)));

	//返回总容量
	return _GridCount;
}

void UUI_PlayerBag::CloseBagPanel()
{
	//清理缓存
	this->TimeLoad_PlayerBags.Stop();
	this->M_Panel_Index = 0;
	this->PlayerBagEquipView->ClearCache();
	this->PlayerBagCardView->ClearCache();
	this->PlayerBagMaterialView->ClearCache();

	//保存游戏
	UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("关闭背包界面")));

	this->RemoveFromParent();
}