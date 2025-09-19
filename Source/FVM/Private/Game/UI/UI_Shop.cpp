// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/UI/UI_Shop.h"

#include "Game/UI/UI_PlayerInformationShow.h"

#include <Components/CanvasPanel.h>
#include <Components/UniformGridPanel.h>
#include <Components/ScrollBox.h>
#include <Components/Button.h>

#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"

#include "Data/CardData/CardDataStruct.h"

#include "GameSystem/PlayerStructManager.h"


UWidget* UUI_Shop::WidgetCreate_Init_0(const UItemLoadDataBlock* const _Data, int32 _Index)
{
	UUI_ShopGrid* _Grid = CreateWidget<UUI_ShopGrid>(this,
		LoadClass<UUI_ShopGrid>(0, TEXT("WidgetBlueprint'/Game/Resource/BP/Game/UI/MainFrame/BPUI_Shop_Grid_Larg.BPUI_Shop_Grid_Larg_C'")));

	this->InitShopGridData(_Data, _Grid);

	return _Grid;
}

UWidget* UUI_Shop::WidgetCreate_Init_1(const UItemLoadDataBlock* const _Data, int32 _Index)
{
	UUI_ShopGrid* _Grid = CreateWidget<UUI_ShopGrid>(this,
		LoadClass<UUI_ShopGrid>(0, TEXT("WidgetBlueprint'/Game/Resource/BP/Game/UI/MainFrame/BPUI_Shop_Grid_Min.BPUI_Shop_Grid_Min_C'")));

	this->InitShopGridData(_Data, _Grid);

	return _Grid;
}

void UUI_Shop::WidgetRefresh_Update_0(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget)
{
	this->InitShopGridData(_Data, Cast<UUI_ShopGrid>(_UWidget));
}

void UUI_Shop::InitShopGridData(const UItemLoadDataBlock* const _Data, UUI_ShopGrid* _UWidget)
{

	GAME_LOG(__FUNCTION__, TEXT("初始化商城格子数据"), {
	GAME_LOG_LINE(TEXT("需要重做"));
		});

	/*
	_UWidget->M_UUI_Shop = this;
	_UWidget->SetItemTipsType(((FItem_Price_Data*)(_Data->GetValue()))->M_FItemPrice.M_ItemTipsImageType);

	int32 LItemPrice = ((FItem_Price_Data*)(_Data->GetValue()))->M_FItemPrice.M_ItemPrice;
	FItemPrice LFItemPrice = ((FItem_Price_Data*)(_Data->GetValue()))->M_FItemPrice;

	//更具商品的价格类型进行设置价格
	switch (_UWidget->GetEItemTipsImageType())
	{
	case EItemTipsImageType::E_HalfPirce:LItemPrice = LItemPrice / 2; break;
	case EItemTipsImageType::E_New:LItemPrice = LItemPrice * 0.85f; break;
	}

	LFItemPrice.M_ItemPrice = LItemPrice;

	//设置格子的数据
	UWidgetBase::SetImageBrush(_UWidget->GetItemHeadImage(),
		((FItem_Price_Data*)(_Data->GetValue()))->M_FItemPrice.M_ItemHeadTexture2DPath.ToString(),
		FVector(1.f), 1.f, true, FVector(2.f));

	_UWidget->M_ItemName = ((FItem_Price_Data*)(_Data->GetValue()))->M_FItemPrice.M_ItemName;
	_UWidget->M_ItemTips = ((FItem_Price_Data*)(_Data->GetValue()))->M_FItemPrice.M_ItemTips;
	_UWidget->M_ItemPrice = FString(FString::FromInt(LItemPrice) + ((FItem_Price_Data*)(_Data->GetValue()))->M_FItemPrice.M_ItemMoneyTypeName);
	_UWidget->M_FItemPrice = LFItemPrice;

	//_UWidget->PlayAnimation_1();

	*/
}

bool UUI_Shop::Initialize()
{
	if (!Super::Initialize())
		return false;

	this->M_SnapUp_0 = this->GetWidgetComponent<UUniformGridPanel>(this, "Left_Items_Content");
	this->M_SnapUp_1 = this->GetWidgetComponent<UUniformGridPanel>(this, "Right_Items_Content");

	this->M_SnapUp_ScrollBox_0 = this->GetWidgetComponent<UScrollBox>(this, "Left_Items_Scroll");
	this->M_SnapUp_ScrollBox_1 = this->GetWidgetComponent<UScrollBox>(this, "Right_Items_Scroll");


	//-------------------------------------------------------界面容器
	this->M_ContentGrid_2 = this->GetWidgetComponent<UUniformGridPanel>(this, "Items_Content");
	this->M_Content_ScrollBox_2 = this->GetWidgetComponent<UScrollBox>(this, "ScrollBox_70");
	//-------------------------------------------------------


	//------------------------------初始化--角色形象界面-------------------------
	this->M_PlayerSuit_BoxPanel = this->GetWidgetComponent<UCanvasPanel>(this, "PlayerSuitPanel");
	//------------------------------初始化--角色形象界面-------------------------


	return true;
}

void UUI_Shop::InitDataTable()
{
	if (this->M_UUI_PlayerInformationShow)
		return;

	//初始化界面选项卡----------------------------------------------------------------
	for (int32 i = 0; i < 8; i++)
		this->M_ShopTabs.Emplace(this->GetWidgetComponent<UButton>(this, FString(TEXT("_Func_")) + FString::FromInt(i) + FString(TEXT("_"))));

	//选项卡路径
	this->M_ShopTabsDef.Append({ FString(TEXT("T_Shop_3")),FString(TEXT("T_Shop_1")) ,FString(TEXT("T_Shop_2")) ,FString(TEXT("T_Shop_6")) ,FString(TEXT("T_Shop_5")) ,FString(TEXT("T_Shop_4")) ,FString(TEXT("T_Shop_7")),FString(TEXT("T_Shop_8")) });

	//初始化界面选项卡----------------------------------------------------------------



	//初始化角色
	this->M_UUI_PlayerInformationShow = CreateWidget<UUI_PlayerInformationShow>(this, LoadClass<UUI_PlayerInformationShow>(0, TEXT("WidgetBlueprint'/Game/Resource/BP/Game/UI/BPUI_PlayerInforPanel.BPUI_PlayerInforPanel_C'")));
	//this->M_UUI_PlayerInformationShow->SetPlayerSuitConst();
	this->M_PlayerSuit_BoxPanel->AddChildToCanvas(this->M_UUI_PlayerInformationShow);

	//--------------------------------------------------------------左侧数据  右侧数据
	//先存储临时数据(把需要的放在加载列表)
	TArray<FItem_Price_Data> TempItem;
	UGameSystemFunction::GetDataTableRows<FItem_Price_Data>((0, TEXT("DataTable'/Game/Resource/BP/Data/ShopData/ShopItem_SnapUpLeft.ShopItem_SnapUpLeft'")), TempItem);
	this->TransformData(TempItem, this->M_ShopItemDatas_Left);
	TempItem.Empty();
	//--------------------------------------------------------------
	UGameSystemFunction::GetDataTableRows<FItem_Price_Data>((0, TEXT("DataTable'/Game/Resource/BP/Data/ShopData/ShopItem_SnapUpRight.ShopItem_SnapUpRight'")), TempItem);
	this->TransformData(TempItem, this->M_ShopItemDatas_Right);
	TempItem.Empty();

	//热卖
	//--------------------------------------------------------------
	UGameSystemFunction::GetDataTableRows<FItem_Price_Data>((0, TEXT("DataTable'/Game/Resource/BP/Data/ShopData/ShopItem_SellingItem.ShopItem_SellingItem'")), TempItem);
	this->TransformData(TempItem, this->M_ShopItemDatas_SellingRange);
	TempItem.Empty();

	//折扣
	//--------------------------------------------------------------
	UGameSystemFunction::GetDataTableRows<FItem_Price_Data>((0, TEXT("DataTable'/Game/Resource/BP/Data/ShopData/ShopItem_DiscountItem.ShopItem_DiscountItem'")), TempItem);
	this->TransformData(TempItem, this->M_ShopItemDatas_DiscountRange);
	TempItem.Empty();

	//武器
	//--------------------------------------------------------------
	UGameSystemFunction::GetDataTableRows<FItem_Price_Data>("DataTable'/Game/Resource/BP/Data/ShopData/ShopItem_Weapons.ShopItem_Weapons'", TempItem);
	this->TransformData(TempItem, this->M_ShopItemDatas_WeaponRange);
	TempItem.Empty();

	//防御卡
	//--------------------------------------------------------------
	UGameSystemFunction::GetDataTableRows<FItem_Price_Data>("DataTable'/Game/Resource/BP/Data/ShopData/ShopItemData_0.ShopItemData_0'", TempItem);
	this->TransformData(TempItem, this->M_ShopItemDatas_Cards);
	TempItem.Empty();

	//道具
	//--------------------------------------------------------------
	UGameSystemFunction::GetDataTableRows<FItem_Price_Data>("DataTable'/Game/Resource/BP/Data/ShopData/ShopItemData_1.ShopItemData_1'", TempItem);
	UGameSystemFunction::GetDataTableRows<FItem_Price_Data>("DataTable'/Game/Resource/BP/Data/ShopData/Item_CardSkillBooks.Item_CardSkillBooks'", TempItem);
	UGameSystemFunction::GetDataTableRows<FItem_Price_Data>("DataTable'/Game/Resource/BP/Data/ShopData/Item_CardChangeJob.Item_CardChangeJob'", TempItem);
	this->TransformData(TempItem, this->M_ShopItemDatas_ItemRange);
	TempItem.Empty();

	//时装
	//--------------------------------------------------------------
	UGameSystemFunction::GetDataTableRows<FItem_Price_Data>("DataTable'/Game/Resource/BP/Data/ShopData/ShopItemData_2.ShopItemData_2'", TempItem);
	this->TransformData(TempItem, this->M_ShopItemDatas_FashionRange);
	TempItem.Empty();

	/*
	//初始化加载器
	this->M_UItemLoaderObject_0 = NewObject<UItemLoaderObject>(this, TEXT("Shop_UItemLoaderObject_0"));
	this->M_UItemLoaderObject_0->InitWidget_First(this->M_SnapUp_0, this->M_SnapUp_ScrollBox_0, 0.01f);
	this->M_UItemLoaderObject_0->InitRange_Second(1, 4, 4, 210.f, 290.f);
	this->M_UItemLoaderObject_0->SetGridWidgetMargin(FVector2D(0.f, 0.f));
	this->M_UItemLoaderObject_0->OnNew.BindUObject(this, &UUI_Shop::WidgetCreate_Init_0);
	this->M_UItemLoaderObject_0->OnUpdate.BindUObject(this, &UUI_Shop::WidgetRefresh_Update_0);

	//初始化加载器
	this->M_UItemLoaderObject_1 = NewObject<UItemLoaderObject>(this, TEXT("Shop_UItemLoaderObject_1"));
	this->M_UItemLoaderObject_1->InitWidget_First(this->M_SnapUp_1, this->M_SnapUp_ScrollBox_1, 0.01f);
	this->M_UItemLoaderObject_1->InitRange_Second(3, 2, 4, 400.f, 160.f);
	this->M_UItemLoaderObject_1->SetGridWidgetMargin(FVector2D(20.f, 0.f));
	this->M_UItemLoaderObject_1->SetScrollTransfrom(FVector2D(50.f, 0.f));
	this->M_UItemLoaderObject_1->OnNew.BindUObject(this, &UUI_Shop::WidgetCreate_Init_1);
	this->M_UItemLoaderObject_1->OnUpdate.BindUObject(this, &UUI_Shop::WidgetRefresh_Update_0);

	//初始化加载器
	this->M_UItemLoaderObject_2 = NewObject<UItemLoaderObject>(this, TEXT("Shop_UItemLoaderObject_2"));
	this->M_UItemLoaderObject_2->InitWidget_First(this->M_ContentGrid_2, this->M_Content_ScrollBox_2, 0.01f);
	this->M_UItemLoaderObject_2->InitRange_Second(4, 4, 4, 400.f, 160.f);
	this->M_UItemLoaderObject_2->SetGridWidgetMargin(FVector2D(50.f, 0.f));
	this->M_UItemLoaderObject_2->OnNew.BindUObject(this, &UUI_Shop::WidgetCreate_Init_1);
	this->M_UItemLoaderObject_2->OnUpdate.BindUObject(this, &UUI_Shop::WidgetRefresh_Update_0);
	*/
}

void UUI_Shop::SelectShopTab(EShopTab _Tab)
{
	for (uint8 i = 0; i < M_ShopTabs.Num(); i++)
		if (i != (uint8)(_Tab))
			UWidgetBase::SetButtonStyle(this->M_ShopTabs[i], FString(TEXT("Texture2D'/Game/Resource/Texture/UI/Game/Shop/Tab/")) + this->M_ShopTabsDef[i] + FString(TEXT("_a.")) + FString(TEXT(".")) + this->M_ShopTabsDef[i] + FString(TEXT("_a'")));

	UWidgetBase::SetButtonStyle(this->M_ShopTabs[(uint8)(_Tab)], FString(TEXT("Texture2D'/Game/Resource/Texture/UI/Game/Shop/Tab/")) + this->M_ShopTabsDef[(uint8)(_Tab)] + FString(TEXT(".")) + this->M_ShopTabsDef[(uint8)(_Tab)] + FString(TEXT("'")));

	switch (_Tab)
	{
	case EShopTab::ESnapUp: {
		this->LoadSnapUpLeft();
		this->LoadSnapUpRight();
	}break;
	case EShopTab::ESelling:this->LoadSellingItemRange(); break;
	case EShopTab::EDiscount: this->LoadDiscountItemRange(); break;
	case EShopTab::EWeapon: this->LoadWeaponRange();  break;
	case EShopTab::ECards: this->LoadCardRange(); break;
	case EShopTab::EItem: this->LoadItemRange(); break;
	case EShopTab::EMakeup: this->LoadMakeupRange(); break;
	case EShopTab::EFashion: this->LoadFashionRange(); break;
	default:break;
	}
}

void UUI_Shop::UpdateCoinText()
{
	this->M_Coin_Text =
		TEXT("金币[") +
		FString::FromInt(GET_PLAYERDATAINS()->GetCoin(0).CoinValue) +
		TEXT("]") +
		TEXT("礼券[") +
		FString::FromInt(GET_PLAYERDATAINS()->GetCoin(1).CoinValue) +
		TEXT("]") +
		TEXT("点券[") +
		FString::FromInt(GET_PLAYERDATAINS()->GetCoin(2).CoinValue) +
		TEXT("]") +
		TEXT("威望[") +
		FString::FromInt(GET_PLAYERDATAINS()->GetCoin(3).CoinValue) +
		TEXT("]");
}

void UUI_Shop::TransformData(const TArray<FItem_Price_Data>& _Datas, TArray<FItem_Price_Data>& _OutResult)
{
	for (auto& LData : _Datas)
		if (LData.M_FItemPrice.M_IsShow)
			_OutResult.Emplace(LData);
}

void UUI_Shop::LoadSnapUpLeft()
{
	this->ShowPlayerSuitPanel(true);
	this->M_UItemLoaderObject_0->UpdateDatatable(this->M_ShopItemDatas_Left);
	this->M_UItemLoaderObject_0->SetLoaderItemMaxCount(4);
	this->M_UItemLoaderObject_0->SetLoaderPage(0);
	this->M_UItemLoaderObject_0->Execute();


	this->UpdateCoinText();
}

void UUI_Shop::LoadSnapUpRight()
{
	this->ShowPlayerSuitPanel(true);
	this->M_UItemLoaderObject_1->UpdateDatatable(this->M_ShopItemDatas_Right);
	this->M_UItemLoaderObject_1->SetLoaderItemMaxCount(this->M_ShopItemDatas_Right.Num());
	this->M_UItemLoaderObject_1->SetLoaderPage(0);
	this->M_UItemLoaderObject_1->Execute();
}

void UUI_Shop::LoadSellingItemRange()
{
	//显示界面(热卖区)
	this->ShowPlayerSuitPanel(true);
	this->M_UItemLoaderObject_2->UpdateDatatable(this->M_ShopItemDatas_SellingRange);
	this->M_UItemLoaderObject_2->SetLoaderItemMaxCount(this->M_ShopItemDatas_SellingRange.Num());
	this->LoadList();
}

void UUI_Shop::LoadDiscountItemRange()
{
	//显示界面(折扣区)
	this->ShowPlayerSuitPanel(true);
	this->M_UItemLoaderObject_2->UpdateDatatable(this->M_ShopItemDatas_DiscountRange);
	this->M_UItemLoaderObject_2->SetLoaderItemMaxCount(this->M_ShopItemDatas_DiscountRange.Num());
	this->LoadList();
}

void UUI_Shop::LoadWeaponRange()
{
	//显示界面(武器区)
	this->ShowPlayerSuitPanel(true);
	this->M_UItemLoaderObject_2->UpdateDatatable(this->M_ShopItemDatas_WeaponRange);
	this->M_UItemLoaderObject_2->SetLoaderItemMaxCount(this->M_ShopItemDatas_WeaponRange.Num());
	this->LoadList();
}

void UUI_Shop::LoadCardRange()
{
	//显示界面(防御卡区)
	this->ShowPlayerSuitPanel(true);
	this->M_UItemLoaderObject_2->UpdateDatatable(this->M_ShopItemDatas_Cards);
	this->M_UItemLoaderObject_2->SetLoaderItemMaxCount(this->M_ShopItemDatas_Cards.Num());
	this->LoadList();
}

void UUI_Shop::LoadItemRange()
{
	//显示界面(道具区)
	this->ShowPlayerSuitPanel(true);

	this->M_UItemLoaderObject_2->UpdateDatatable(this->M_ShopItemDatas_ItemRange);
	this->M_UItemLoaderObject_2->SetLoaderItemMaxCount(this->M_ShopItemDatas_ItemRange.Num());
	this->LoadList();
}

void UUI_Shop::LoadMakeupRange()
{
	//显示界面(妆容区)
	this->ShowPlayerSuitPanel(false);

	this->M_UItemLoaderObject_2->SetLoaderRowCol(4, 2);
	this->M_UItemLoaderObject_2->SetLoaderTransfrom(FVector2D(400.f, 0.f));

	this->M_UItemLoaderObject_2->UpdateDatatable(this->M_ShopItemDatas_FashionRange);
	this->M_UItemLoaderObject_2->SetLoaderItemMaxCount(this->M_ShopItemDatas_FashionRange.Num());

	this->M_UItemLoaderObject_2->SetLoaderPage(0);
	this->M_UItemLoaderObject_2->Execute();
}

void UUI_Shop::LoadFashionRange()
{
	//显示界面(时装区)
	this->ShowPlayerSuitPanel(false);

	this->M_UItemLoaderObject_2->SetLoaderRowCol(4, 2);
	this->M_UItemLoaderObject_2->SetLoaderTransfrom(FVector2D(400.f, 0.f));

	this->M_UItemLoaderObject_2->UpdateDatatable(this->M_ShopItemDatas_FashionRange);
	this->M_UItemLoaderObject_2->SetLoaderItemMaxCount(this->M_ShopItemDatas_FashionRange.Num());

	this->M_UItemLoaderObject_2->SetLoaderPage(0);
	this->M_UItemLoaderObject_2->Execute();
}

void UUI_Shop::ShowPlayerSuitPanel(bool _bHidden)
{
	if (_bHidden)
	{
		this->M_PlayerSuit_BoxPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	else {
		this->M_PlayerSuit_BoxPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (!this->M_UUI_PlayerInformationShow->GetPlayerSuitConst())
			this->M_UUI_PlayerInformationShow->SetPlayerSuitConst();
	}
}

void UUI_Shop::LoadList()
{
	this->M_UItemLoaderObject_2->SetLoaderRowCol(4, 4);
	this->M_UItemLoaderObject_2->SetLoaderTransfrom(FVector2D(0.f, 0.f));
	this->M_UItemLoaderObject_2->SetLoaderPage(0);
	this->M_UItemLoaderObject_2->Execute();
}

void UUI_Shop::ClosePanel()
{
	this->M_UUI_PlayerInformationShow->SetPlayerSuitConst();

	this->RemoveFromParent();
}
