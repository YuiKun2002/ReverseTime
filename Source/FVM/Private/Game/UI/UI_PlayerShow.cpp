// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/UI_PlayerShow.h"
#include <Components/Image.h>
#include <Components/CanvasPanel.h>
#include "GameSystem/Tools/GameSystemFunction.h"


bool UUI_PlayerShow::GetPlayerSuitConst()
{
	return this->M_bConst;
}

FPlayerSuit* UUI_PlayerShow::GetPlayerSuit()
{

	static FPlayerSuit LSuit;

	if (this->M_bConst)
	{
		return &this->M_FPlayerSuitTemp;
	}
	else {
		return &this->M_CurrentPlayer->M_FPlayerSuit;
	}

	return &LSuit;
}

bool UUI_PlayerShow::GetbConst()
{
	return this->M_bConst;
}

void UUI_PlayerShow::SetCurrentPlayer(UPlayerStructManager* _CurrentPlayer)
{
	this->M_CurrentPlayer = _CurrentPlayer;
}

void UUI_PlayerShow::SetConstCurrentPlayer()
{
	if (IsValid(this->M_CurrentPlayer))
	{
		this->M_FPlayerSuitTemp = this->M_CurrentPlayer->M_FPlayerSuit;
	}

	this->M_bConst = true;
}

void UUI_PlayerShow::InitPlayerSuit(uint8 PlayerGender, FPlayerSuit LSuit)
{
	//初始化服装数据
	if (this->M_bConst)
	{
		LSuit = this->M_FPlayerSuitTemp;
	}
	else {
		if (IsValid(this->M_CurrentPlayer))
		{
			LSuit = this->M_CurrentPlayer->M_FPlayerSuit;
		}
	}

	//套装与服装界面显示与关闭
	if (LSuit.M_bPlayerSuit && LSuit.M_ShowPlayerSuit)
	{
		this->Player_Suit1_Panel->SetVisibility(ESlateVisibility::Hidden);
		this->Player_Suit2_Panel->SetVisibility(ESlateVisibility::HitTestInvisible);

		//显示套装
		if (LSuit.M_Suit.Block.IsValid())
		{
			UWidgetBase::SetImageBrush(this->Player_Uni, LSuit.M_Suit.SuitFrontTextureShowPath);
		}
	}
	else {
		this->Player_Suit2_Panel->SetVisibility(ESlateVisibility::Hidden);
		this->Player_Suit1_Panel->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	//显示->翅膀
	if (LSuit.M_FlyItem.Block.IsValid())
	{
		UWidgetBase::SetImageBrush(this->Player_FlyItem, LSuit.M_FlyItem.SuitFrontTextureShowPath);
	}
	else
	{
		this->Player_FlyItem->SetVisibility(ESlateVisibility::Collapsed);
	}

	//显示->头发
	if (LSuit.M_Hair.Block.IsValid())
	{
		//显示头发前
		UWidgetBase::SetImageBrush(this->Player_Head_Front, LSuit.M_Hair.SuitFrontTextureShowPath);

		//显示头发后
		if (LSuit.M_Hair.SuitBackTextureShowPath.Equals(""))
		{
			this->Player_Head_Back->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			UWidgetBase::SetImageBrush(this->Player_Head_Back, LSuit.M_Hair.SuitBackTextureShowPath);
			this->Player_Head_Back->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else {

		bool bMale = false;

		if (IsValid(this->M_CurrentPlayer))
		{
			if (this->M_CurrentPlayer->M_PlayerGender != 0)
			{
				bMale = true;
			}
		}
		else {
			if (PlayerGender != 0)
			{
				bMale = true;
			}
		}

		//根据性别显示基础着装
		if (bMale)
		{
			//男
			UWidgetBase::SetImageBrush(this->Player_Head_Front,
				TEXT("Texture2D'/Game/Resource/Texture/PlayerItems/Player_Mod/Base/角色基础头发-男_前.角色基础头发-男_前'"));
			this->Player_Head_Back->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			//女
			UWidgetBase::SetImageBrush(this->Player_Head_Back,
				TEXT("Texture2D'/Game/Resource/Texture/PlayerItems/Player_Mod/Base/角色基础头发_女_后.角色基础头发_女_后'"));
			UWidgetBase::SetImageBrush(this->Player_Head_Front,
				TEXT("Texture2D'/Game/Resource/Texture/PlayerItems/Player_Mod/Base/角色基础头发_女_前.角色基础头发_女_前'"));
			this->Player_Head_Back->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}

	//显示->着装
	if (LSuit.M_Body.Block.IsValid())
	{
		UWidgetBase::SetImageBrush(this->Player_Body, LSuit.M_Body.SuitFrontTextureShowPath);
	}
	else
	{
		UWidgetBase::SetImageBrush(this->Player_Body,
			TEXT("Texture2D'/Game/Resource/Texture/PlayerItems/Player_Mod/Base/角色基础服装.角色基础服装'"));
	}


	//显示->头
	if (LSuit.M_Head.Block.IsValid())
	{
		UWidgetBase::SetImageBrush(this->Player_Head, LSuit.M_Head.SuitFrontTextureShowPath);
	}
	else
	{
		UWidgetBase::SetImageBrush(this->Player_Head,
			TEXT("Texture2D'/Game/Resource/Texture/PlayerItems/Player_Mod/Base/角色基础脸.角色基础脸'"));
	}

	//显示->眼睛
	if (LSuit.M_Eye.Block.IsValid())
	{
		UWidgetBase::SetImageBrush(this->Player_eye, LSuit.M_Eye.SuitFrontTextureShowPath);
	}
	else
	{

		bool bMale = false;

		if (IsValid(this->M_CurrentPlayer))
		{
			if (this->M_CurrentPlayer->M_PlayerGender != 0)
			{
				bMale = true;
			}
		}
		else {
			if (PlayerGender != 0)
			{
				bMale = true;
			}
		}

		//根据性别显示基础着装
		if (bMale)
		{
			//男
			UWidgetBase::SetImageBrush(this->Player_eye,
				TEXT("Texture2D'/Game/Resource/Texture/PlayerItems/Player_Mod/Base/角色基础眼睛_男.角色基础眼睛_男'"));
		}
		else
		{
			//女
			UWidgetBase::SetImageBrush(this->Player_eye,
				TEXT("Texture2D'/Game/Resource/Texture/PlayerItems/Player_Mod/Base/角色基础眼睛_女.角色基础眼睛_女'"));
		}
	}


	//显示->脸饰
	if (LSuit.M_Face.Block.IsValid())
	{
		UWidgetBase::SetImageBrush(this->Player_Face, LSuit.M_Face.SuitFrontTextureShowPath);
	}
	else
	{
		this->Player_Face->SetVisibility(ESlateVisibility::Collapsed);
	}

	//显示->眼镜
	if (LSuit.M_Glasses.Block.IsValid())
	{
		UWidgetBase::SetImageBrush(this->Player_eye_2, LSuit.M_Glasses.SuitFrontTextureShowPath);
	}
	else
	{
		this->Player_eye_2->SetVisibility(ESlateVisibility::Collapsed);
	}

	//显示->帽子
	if (LSuit.M_Cap.Block.IsValid())
	{
		UWidgetBase::SetImageBrush(this->Player_Head_Top, LSuit.M_Cap.SuitFrontTextureShowPath);
	}
	else
	{
		this->Player_Head_Top->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUI_PlayerShow::UnInstallPlayerSuit(EPlayerEquipmentSlotPosition _LEPlayerEquipmentSlotPosition)
{
	FPlayerSuit* LSuit = nullptr;

	if (this->M_bConst)
	{
		LSuit = &this->M_FPlayerSuitTemp;
	}
	else {
		if (this->M_CurrentPlayer)
			LSuit = &this->M_CurrentPlayer->M_FPlayerSuit;
	}

	if (!LSuit)
	{
		return;
	}

	//使用音效
	this->PlayOperateAudio();

	//卸载
	auto UnIns = [&](FPlayerSuitItem& LData) {
		if (LData.Block.IsValid()) {

			if (!this->M_bConst)
			{
				GET_PLAYERDATAINS()->AddEquipItem(LData.Block.ItemID, LData.Block.ItemLevel, LData.Block.BlockIndex, false);
			}

			FPlayerEquipment Data;
			FItemViewBlock Block1, Block2;
			LData.CopyValue(Data, Block1, Block2);
		}
		};

	switch (_LEPlayerEquipmentSlotPosition)
	{
	case EPlayerEquipmentSlotPosition::E_Player_FlyItem:UnIns(LSuit->M_FlyItem); break;
	case EPlayerEquipmentSlotPosition::E_Player_Hair:UnIns(LSuit->M_Hair); break;
	case EPlayerEquipmentSlotPosition::E_Player_Body:UnIns(LSuit->M_Body); break;
	case EPlayerEquipmentSlotPosition::E_Player_Head:UnIns(LSuit->M_Head); break;
	case EPlayerEquipmentSlotPosition::E_Player_Eye:UnIns(LSuit->M_Eye); break;
	case EPlayerEquipmentSlotPosition::E_Player_Face:UnIns(LSuit->M_Face); break;
	case EPlayerEquipmentSlotPosition::E_Player_Glasses:UnIns(LSuit->M_Glasses); break;
	case EPlayerEquipmentSlotPosition::E_Player_Cap:UnIns(LSuit->M_Cap); break;
	case EPlayerEquipmentSlotPosition::E_Player_Suit: {
		UnIns(LSuit->M_Suit);
		LSuit->M_bPlayerSuit = false;
		LSuit->M_HiddenFlyItem = false;
	}break;
	}

	if (!this->M_bConst)
	{
		UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("角色服装卸载操作")));
	}
}

void UUI_PlayerShow::PlayerSuitHidden(bool _bShowSuit)
{
	FPlayerSuit* LSuit = nullptr;
	if (this->M_bConst)
	{
		LSuit = &this->M_FPlayerSuitTemp;
		LSuit->M_ShowPlayerSuit = _bShowSuit;
	}
	else {
		LSuit = &this->M_CurrentPlayer->M_FPlayerSuit;
		this->M_CurrentPlayer->M_FPlayerSuit.M_ShowPlayerSuit = _bShowSuit;
	}
}