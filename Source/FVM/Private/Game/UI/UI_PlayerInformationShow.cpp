// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/UI_PlayerInformationShow.h"
//角色形象
#include "Game/UI/UI_PlayerShow.h"
#include "Game/UI/UI_PlayerBag.h"
//游戏实例
#include "GameSystem/FVMGameInstance.h"
#include "Data/EquipmentDataSturct.h"
#include <Components/Button.h>
#include <Components/Image.h>
#include <Components/CanvasPanel.h>

bool UUI_PlayerInformationShow::Initialize()
{
	if (!Super::Initialize())
		return false;

	//获取角色形象界面
	this->M_PlayerPoint = this->GetWidgetComponent<UCanvasPanel>(this, "PlayerPoint");

	this->M_PlayerSuitBG_Img = this->GetWidgetComponent<UImage>(this, "Player_Bg");
	this->M_PlayerSuitBG_Panel = this->GetWidgetComponent<UCanvasPanel>(this, "PlayerSuitBG");
	this->M_PlayerFristWeaponBG_Panel = this->GetWidgetComponent<UCanvasPanel>(this, "PlayerFristWeaponBG");
	this->M_PlayerSecondWeaponBG_Panel = this->GetWidgetComponent<UCanvasPanel>(this, "PlayerSecondWeaponBG");
	this->M_PlayerSuperWeaponBG_Panel = this->GetWidgetComponent<UCanvasPanel>(this, "PlayerSuperWeaponBG");

	return true;
}

void UUI_PlayerInformationShow::InitPlayerBaseData()
{
	if (this->M_UUI_PlayerShow)
		return;

	//角色形象----------------------------------------------------------------------------------------------------
	this->M_UUI_PlayerShow = CreateWidget<UUI_PlayerShow>(this, LoadClass<UUI_PlayerShow>(0,
	TEXT("WidgetBlueprint'/Game/Resource/BP/Game/UI/MainFrame/BP_PlayerShow.BP_PlayerShow_C'")));
	this->M_PlayerPoint->AddChildToCanvas(this->M_UUI_PlayerShow);

	//角色基础着装图片
	this->M_PlayerBaseSuitButtons.Emplace(this->GetWidgetComponent<UButton>(this, "But_FlyItem"));

	this->M_PlayerBaseSuitButtons.Emplace(this->GetWidgetComponent<UButton>(this, "But_Hair"));//头发

	this->M_PlayerBaseSuitButtons.Emplace(this->GetWidgetComponent<UButton>(this, "But_Body"));//着装

	this->M_PlayerBaseSuitButtons.Emplace(this->GetWidgetComponent<UButton>(this, "But_Eye"));//眼睛

	this->M_PlayerBaseSuitButtons.Emplace(this->GetWidgetComponent<UButton>(this, "But_Face"));//脸饰

	this->M_PlayerBaseSuitButtons.Emplace(this->GetWidgetComponent<UButton>(this, "But_Glasses"));//眼镜

	this->M_PlayerBaseSuitButtons.Emplace(this->GetWidgetComponent<UButton>(this, "But_Cap"));//帽子

	this->M_PlayerBaseSuitButtons.Emplace(this->GetWidgetComponent<UButton>(this, "But_WeaponFirst"));//主武器

	this->M_PlayerBaseSuitButtons.Emplace(this->GetWidgetComponent<UButton>(this, "But_WeaponSecond"));//副武器

	this->M_PlayerBaseSuitButtons.Emplace(this->GetWidgetComponent<UButton>(this, "But_WeaponSuper"));//超级武器

	this->M_PlayerBaseSuitButton = this->GetWidgetComponent<UButton>(this, "But_Suit");

	//角色形象----------------------------------------------------------------------------------------------------
}

void UUI_PlayerInformationShow::SetPlayerSuitConst()
{
	if (IsValid(this->M_UUI_PlayerShow))
	{
		this->M_UUI_PlayerShow->SetConstCurrentPlayer();
		this->RefreshPlayerSuit();
	}
}

void UUI_PlayerInformationShow::InitPlayerSuit()
{
	//套装与服装界面显示与关闭
	if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_bPlayerSuit)
	{
		//显示套装
		if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_Suit.Block.IsValid())
		{
			UWidgetBase::SetButtonStyle(this->M_PlayerBaseSuitButton,
				this->M_UUI_PlayerShow->GetPlayerSuit()->M_Suit.HeadPath
			);

			this->M_PlayerBaseSuitButton->SetVisibility(ESlateVisibility::Visible);
			this->M_PlayerSuitBG_Img->SetVisibility(ESlateVisibility::HitTestInvisible);
			this->M_PlayerSuitBG_Panel->SetVisibility(ESlateVisibility::HitTestInvisible);

		}
	}
	else {
		this->M_PlayerBaseSuitButton->SetVisibility(ESlateVisibility::Collapsed);
		this->M_PlayerSuitBG_Panel->SetVisibility(ESlateVisibility::Collapsed);
		this->M_PlayerSuitBG_Img->SetVisibility(ESlateVisibility::Collapsed);
	}

	//显示->翅膀
	if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_FlyItem.Block.IsValid())
	{
		UWidgetBase::SetButtonStyle(this->M_PlayerBaseSuitButtons[0],
			this->M_UUI_PlayerShow->GetPlayerSuit()->M_FlyItem.HeadPath);

		this->M_PlayerBaseSuitButtons[0]->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		this->M_PlayerBaseSuitButtons[0]->SetVisibility(ESlateVisibility::Collapsed);
	}

	//显示->头发
	if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_Hair.Block.IsValid())
	{
		//显示头发前
		UWidgetBase::SetButtonStyle(this->M_PlayerBaseSuitButtons[1],
			this->M_UUI_PlayerShow->GetPlayerSuit()->M_Hair.HeadPath);

		this->M_PlayerBaseSuitButtons[1]->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		this->M_PlayerBaseSuitButtons[1]->SetVisibility(ESlateVisibility::Collapsed);
	}

	//显示->着装
	if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_Body.Block.IsValid())
	{
		UWidgetBase::SetButtonStyle(this->M_PlayerBaseSuitButtons[2],
			this->M_UUI_PlayerShow->GetPlayerSuit()->M_Body.HeadPath);

		this->M_PlayerBaseSuitButtons[2]->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		this->M_PlayerBaseSuitButtons[2]->SetVisibility(ESlateVisibility::Collapsed);
	}


	//显示->眼睛
	if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_Eye.Block.IsValid())
	{
		UWidgetBase::SetButtonStyle(this->M_PlayerBaseSuitButtons[3],
			this->M_UUI_PlayerShow->GetPlayerSuit()->M_Eye.HeadPath);

		this->M_PlayerBaseSuitButtons[3]->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		this->M_PlayerBaseSuitButtons[3]->SetVisibility(ESlateVisibility::Collapsed);
	}


	//显示->脸饰
	if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_Face.Block.IsValid())
	{
		UWidgetBase::SetButtonStyle(this->M_PlayerBaseSuitButtons[4],
			this->M_UUI_PlayerShow->GetPlayerSuit()->M_Face.HeadPath);

		this->M_PlayerBaseSuitButtons[4]->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		this->M_PlayerBaseSuitButtons[4]->SetVisibility(ESlateVisibility::Collapsed);
	}

	//显示->眼镜
	if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_Glasses.Block.IsValid())
	{
		UWidgetBase::SetButtonStyle(this->M_PlayerBaseSuitButtons[5],
			this->M_UUI_PlayerShow->GetPlayerSuit()->M_Glasses.HeadPath);

		this->M_PlayerBaseSuitButtons[5]->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		this->M_PlayerBaseSuitButtons[5]->SetVisibility(ESlateVisibility::Collapsed);
	}

	//显示->帽子
	if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_Cap.Block.IsValid())
	{
		UWidgetBase::SetButtonStyle(this->M_PlayerBaseSuitButtons[6],
			this->M_UUI_PlayerShow->GetPlayerSuit()->M_Cap.HeadPath);

		this->M_PlayerBaseSuitButtons[6]->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		this->M_PlayerBaseSuitButtons[6]->SetVisibility(ESlateVisibility::Collapsed);
	}

	//显示主武器
	if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_PlayerWeapons.FirstWeapon.WeaponBlock.IsValid())
	{
		UWidgetBase::SetButtonStyle(
			this->M_PlayerBaseSuitButtons[7],
			this->M_UUI_PlayerShow->GetPlayerSuit()->M_PlayerWeapons.FirstWeapon.WeaponHeadPath);
		this->M_PlayerBaseSuitButtons[7]->SetVisibility(ESlateVisibility::Visible);
		this->M_PlayerFristWeaponBG_Panel->SetVisibility(ESlateVisibility::HitTestInvisible);


	}
	else {
		this->M_PlayerBaseSuitButtons[7]->SetVisibility(ESlateVisibility::Collapsed);
		this->M_PlayerFristWeaponBG_Panel->SetVisibility(ESlateVisibility::Collapsed);
	}

	//显示副武器
	if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_PlayerWeapons.SecondWeapon.WeaponBlock.IsValid())
	{
		UWidgetBase::SetButtonStyle(this->M_PlayerBaseSuitButtons[8],
			this->M_UUI_PlayerShow->GetPlayerSuit()->M_PlayerWeapons.SecondWeapon.WeaponHeadPath);
		this->M_PlayerBaseSuitButtons[8]->SetVisibility(ESlateVisibility::Visible);
		this->M_PlayerSecondWeaponBG_Panel->SetVisibility(ESlateVisibility::HitTestInvisible);

	}
	else {
		this->M_PlayerBaseSuitButtons[8]->SetVisibility(ESlateVisibility::Collapsed);
		this->M_PlayerSecondWeaponBG_Panel->SetVisibility(ESlateVisibility::Collapsed);

	}

	//显示超级武器
	if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_PlayerWeapons.SuperWeapon.WeaponBlock.IsValid())
	{
		UWidgetBase::SetButtonStyle(this->M_PlayerBaseSuitButtons[9],
			this->M_UUI_PlayerShow->GetPlayerSuit()->M_PlayerWeapons.SuperWeapon.WeaponHeadPath);
		this->M_PlayerBaseSuitButtons[9]->SetVisibility(ESlateVisibility::Visible);
		this->M_PlayerSuperWeaponBG_Panel->SetVisibility(ESlateVisibility::HitTestInvisible);

	}
	else {
		this->M_PlayerBaseSuitButtons[9]->SetVisibility(ESlateVisibility::Collapsed);
		this->M_PlayerSuperWeaponBG_Panel->SetVisibility(ESlateVisibility::Collapsed);

	}
}

void UUI_PlayerInformationShow::RefreshPlayerSuit(bool bReloadBag)
{
	//初始化显示
	this->M_UUI_PlayerShow->InitPlayerSuit();

	this->InitPlayerSuit();

	if (bReloadBag)
	{
		//刷新背包
		UUI_PlayerBag* Bag = GET_UI_CAST(UUI_PlayerBag, Global, PlayerBag);
		if (Bag->IsInViewport())
		{
			Bag->RelaodCurrentItemData();
		}
	}
}

void UUI_PlayerInformationShow::UnInstallPlayerSuit(EPlayerEquipmentSlotPosition _LEPlayerEquipmentSlotPosition)
{
	//卸载
	this->M_UUI_PlayerShow->UnInstallPlayerSuit(_LEPlayerEquipmentSlotPosition);

	this->RefreshPlayerSuit();

	if (this->OnDataChangedDelegate.IsBound())
		this->OnDataChangedDelegate.Execute();
}

void UUI_PlayerInformationShow::PlayerSuitHidden(UButton* _Button_)
{
	if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_ShowPlayerSuit)
	{
		this->M_UUI_PlayerShow->PlayerSuitHidden(false);

		//修改按钮样式
		if (_Button_)
			UWidgetBase::SetButtonStyle(_Button_, TEXT("Texture2D'/Game/Resource/Texture/UI/Game/PlayerBag/569.569'"));
	}
	else {
		this->M_UUI_PlayerShow->PlayerSuitHidden(true);

		//修改按钮样式
		if (_Button_)
			UWidgetBase::SetButtonStyle(_Button_, TEXT("Texture2D'/Game/Resource/Texture/UI/Game/PlayerBag/568.568'"));
	}

	this->RefreshPlayerSuit();
}

void UUI_PlayerInformationShow::InitPlayerSuitButtonStyle(UButton* _Button_)
{
	if (this->M_UUI_PlayerShow->GetPlayerSuit()->M_ShowPlayerSuit)
	{
		//修改按钮样式
		if (_Button_)
			UWidgetBase::SetButtonStyle(_Button_, TEXT("Texture2D'/Game/Resource/Texture/UI/Game/PlayerBag/568.568'"));
	}
	else {
		//修改按钮样式
		if (_Button_)
			UWidgetBase::SetButtonStyle(_Button_, TEXT("Texture2D'/Game/Resource/Texture/UI/Game/PlayerBag/569.569'"));
	}
}

void UUI_PlayerInformationShow::UnEquip_Weapon(EEquipment _LEEquipment)
{
	//卸载武器
	auto UnEquip = [&](FPlayerWeapon& _LWeapon) {

		if (_LWeapon.WeaponBlock.IsValid())
		{
			if (this->M_UUI_PlayerShow->GetbConst())
			{
				_LWeapon.WeaponBlock.SetNull();
			}
			else {
				//获取角色
				UPlayerStructManager* const Player = UFVMGameInstance::GetPlayerStructManager_Static();

				//卸载物品
				Player->AddEquipItem(_LWeapon.WeaponBlock.ItemID, _LWeapon.WeaponBlock.ItemLevel, _LWeapon.WeaponBlock.BlockIndex, false);

				_LWeapon.WeaponBlock.SetNull();
				_LWeapon.WeaponResource_C_Path = FString();
			}
		}
		};

	//武器卸载
	switch (_LEEquipment)
	{
	case EEquipment::E_PlayerWeaponFirst:
		UnEquip(this->M_UUI_PlayerShow->GetPlayerSuit()->M_PlayerWeapons.FirstWeapon); break;
	case EEquipment::E_PlayerWeaponSecond:
		UnEquip(this->M_UUI_PlayerShow->GetPlayerSuit()->M_PlayerWeapons.SecondWeapon); break;
	case EEquipment::E_PlayerWeaponSuper:
		UnEquip(this->M_UUI_PlayerShow->GetPlayerSuit()->M_PlayerWeapons.SuperWeapon); break;
	}

	//如果是非固定的修改
	if (!this->M_UUI_PlayerShow->GetbConst())
	{
		//清理装备视图的缓存
		GameDataStaticObject<UPlayerBagEquipView>()->ClearCache();
	}

	//刷新UI
	//刷新服装显示
	this->RefreshPlayerSuit();

	//代理函数
	if (this->OnDataChangedDelegate.IsBound())
	{
		this->OnDataChangedDelegate.Execute();
	}
}

void UUI_PlayerInformationShow::SetCurrentPlayer(UPlayerStructManager* _CurrentPlayer)
{
	if (this->M_UUI_PlayerShow)
		this->M_UUI_PlayerShow->SetCurrentPlayer(_CurrentPlayer);
}

bool UUI_PlayerInformationShow::GetPlayerSuitConst()
{
	return this->M_UUI_PlayerShow->GetPlayerSuitConst();
}
