// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/Equipment/Func/EquipmentFuncWeapon.h"
#include "Game/UI/UI_PlayerBag.h"
#include "Game/UI/Tips/UI_ItemDesTip.h"
#include "Game/UI/UI_PlayerSynthesis.h"
#include "GameSystem/Item/ItemBaseStruct.h"
#include "Game/UI/UI_PlayerBagEquipmentGrid.h"
#include "GameSystem/GameUserInterfaceSubsystem.h"
#include "Game/UI/Synthesis/SynModel_WeaponGems.h"
#include "Game/UI/Synthesis/SynModel_GemsUpGrade.h"
#include "GameSystem/PlayerData/LocalEquipmentDataHandle.h"

void UEquipmentFuncWeapon::SetType(uint8 Type) {
	this->EuipType = Type;
}

void UEquipmentFuncWeapon::SetOperateType(int32 OperType) {
	this->OperateType = OperType;
}

void UEquipmentFuncWeapon::Execute(
	UItemGridBase* GridPtr,
	FItemBaseStructData* ItemBasePtr
) {

	Super::Execute(GridPtr, ItemBasePtr);

	this->ItemGrid = Cast<UItemCountGridBase>(GridPtr);

	//转换数据,并且缓存
	switch ((EEquipment)(this->EuipType))
	{
	case EEquipment::E_PlayerWeaponFirst:
	{
		this->FirstWeapon = CAST_TYPE(FEquipment_WeaponFirst_Data, ItemBasePtr);
	}break;
	case EEquipment::E_PlayerWeaponSecond:
	{
		this->SecondWeapon = CAST_TYPE(FEquipment_WeaponSecond_Data, ItemBasePtr);

	}break;
	case EEquipment::E_PlayerWeaponSuper:
	{
		this->SuperWeapon = CAST_TYPE(FEquipment_WeaponSuper_Data, ItemBasePtr);
	}break;
	}

	this->GetTipWidget()->SetHidden(false);

	switch (this->OperateType)
	{
	case 1: {
		this->GetTipWidget()->SetOkButtonTitleText(TEXT("选择"));
		this->GetTipWidget()->BindClickFunc(this, TEXT("SynthesisWeaponSelect"));
	}; break;
	case 2: {
		this->GetTipWidget()->SetOkButtonTitleText(TEXT("选择"));
		this->GetTipWidget()->BindClickFunc(this, TEXT("SynthesisUpgradeGemWeaponSelect"));
	}; break;
	default: {
		this->GetTipWidget()->SetOkButtonTitleText(TEXT("装备"));
		this->GetTipWidget()->BindClickFunc(this, TEXT("Equip"));
	}break;
	}


}

void UEquipmentFuncWeapon::AddDetailData(UPlayerStructManager* const Player, const FItemViewBlock& Block) {

	//----------------------------------------------
	//添加武器细节数据
	ULocalEquipmentDataSaveGame* SaveIns = Cast<ULocalEquipmentDataSaveGame>(
		Player->GetLocalEquipmentDataHandle()->Execute_GetData(
			Player->GetLocalEquipmentDataHandle()
		));

	//新增武器装备情况【如果之前不存在任何数据，则新增记录】
	FPlayerWeaponEquipData* EquipWeaponData = SaveIns->GetbEquipWeaponList().Find(Block.ItemID);
	if (!EquipWeaponData)
	{
		FPlayerWeaponEquipGemSlotDataBlock Slot;
		FPlayerWeaponEquipData NewEquipData;
		NewEquipData.Datas.Emplace(Block.BlockIndex, Slot);
		SaveIns->GetbEquipWeaponList().Emplace(Block.ItemID, NewEquipData);

		//保存存档
		Player->GetLocalEquipmentDataHandle()->Execute_SaveData(
			Player->GetLocalEquipmentDataHandle(), TEXT("新增武器细节数据")
		);
	}
	else {
		//如果此索引无法被找到，则添加此索引
		if (!EquipWeaponData->Datas.Contains(Block.BlockIndex))
		{
			FPlayerWeaponEquipGemSlotDataBlock Slot;
			EquipWeaponData->Datas.Emplace(
				Block.BlockIndex, Slot
			);

			//保存存档
			Player->GetLocalEquipmentDataHandle()->Execute_SaveData(
				Player->GetLocalEquipmentDataHandle(), TEXT("新增武器细节数据")
			);
		}
	}
	//----------------------------------------------
}

void UEquipmentFuncWeapon::Equip() {

	auto Equip_Weapon = [&](
		UPlayerStructManager* const Player,
		FPlayerWeapon& Weapon,
		FItemBase WeaponData,
		FItemViewBlock Block
		) {
			//卸载之前的武器
			if (Weapon.WeaponBlock.IsValid())
			{
				Player->AddEquipItem(
					Weapon.WeaponBlock.ItemID,
					Weapon.WeaponBlock.ItemLevel,
					Weapon.WeaponBlock.BlockIndex, false);
			}

			Weapon.WeaponHeadPath = WeaponData.ItemTexturePath.ToString();
			Weapon.WeaponBlock.ItemID = Block.ItemID;
			Weapon.WeaponBlock.ItemLevel = Block.ItemLevel;
			Weapon.WeaponBlock.BlockIndex = Block.BlockIndex;

			//添加物品索引
			Player->AddEquipItem(Block.ItemID, Block.ItemLevel, Block.BlockIndex);

			//添加武器细节
			this->AddDetailData(Player, Block);

			//刷新加载界面
			GET_UI_CAST(UUI_PlayerBag, Global, PlayerBag)->LoadItemsData();
			//重新服装界面
			GET_UI_CAST(UUI_PlayerBag, Global, PlayerBag)->RefreshPlayerSuit();

			//保存游戏
			UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("进行角色武器装备")));
		};


	//获取角色
	UPlayerStructManager* const Player = UFVMGameInstance::GetPlayerStructManager_Static();

	//转换数据,并且缓存
	switch ((EEquipment)(this->EuipType))
	{
	case EEquipment::E_PlayerWeaponFirst:
	{
		Equip_Weapon(
			Player,
			Player->M_FPlayerSuit.M_PlayerWeapons.FirstWeapon,
			this->FirstWeapon.M_FEquipment,
			this->ItemGrid->GetItemViewBlock()
		);

	}break;
	case EEquipment::E_PlayerWeaponSecond:
	{
		Equip_Weapon(
			Player,
			Player->M_FPlayerSuit.M_PlayerWeapons.SecondWeapon,
			this->SecondWeapon.M_FEquipment,
			this->ItemGrid->GetItemViewBlock()
		);

	}break;
	case EEquipment::E_PlayerWeaponSuper:
	{
		Equip_Weapon(
			Player,
			Player->M_FPlayerSuit.M_PlayerWeapons.SuperWeapon,
			this->SuperWeapon.M_FEquipment,
			this->ItemGrid->GetItemViewBlock()
		);
	}break;
	}
}

void UEquipmentFuncWeapon::SynthesisWeaponSelect() {

	//获取合成屋界面
	UUI_PlayerSynthesis* Syn = GET_UI_CAST(UUI_PlayerSynthesis, Global, Synthesis);

	if (IsValid(Syn) && Syn->IsInViewport())
	{
		//获取角色
		UPlayerStructManager* const Player = UFVMGameInstance::GetPlayerStructManager_Static();

		//添加物品索引,并且不会进行装备操作，仅添加索引数据
		Player->AddEquipItem(
			this->ItemGrid->GetItemViewBlock().ItemID,
			this->ItemGrid->GetItemViewBlock().ItemLevel,
			this->ItemGrid->GetItemViewBlock().BlockIndex,
			false
		);

		//添加武器细节
		this->AddDetailData(Player, this->ItemGrid->GetItemViewBlock());

		//获取武器宝石界面
		UUI_SynModel_WeaponGem* WG = Cast<UUI_SynModel_WeaponGem>(
			Syn->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, WeaponAndGem))
		);

		if (IsValid(WG))
		{
			switch ((EEquipment)(this->EuipType))
			{
			case EEquipment::E_PlayerWeaponFirst: {
				WG->SelectWeapon(this->EuipType, this->ItemGrid->GetItemViewBlock(),
					this->FirstWeapon.M_FEquipment);
			}break;
			case EEquipment::E_PlayerWeaponSecond: {
				WG->SelectWeapon(this->EuipType, this->ItemGrid->GetItemViewBlock(),
					this->SecondWeapon.M_FEquipment);
			}break;
			case EEquipment::E_PlayerWeaponSuper: {
				WG->SelectWeapon(this->EuipType, this->ItemGrid->GetItemViewBlock(),
					this->SuperWeapon.M_FEquipment);
			}break;
			}

		}
	}
}

void UEquipmentFuncWeapon::SynthesisUpgradeGemWeaponSelect() {

	//获取合成屋界面
	UUI_PlayerSynthesis* Syn = GET_UI_CAST(UUI_PlayerSynthesis, Global, Synthesis);

	if (IsValid(Syn) && Syn->IsInViewport())
	{
		//获取角色
		UPlayerStructManager* const Player = UFVMGameInstance::GetPlayerStructManager_Static();

		//获取武器宝石界面
		USynModel_GemsUpGrade* WG = Cast<USynModel_GemsUpGrade>(
			Syn->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, UpgradeGem))
		);

		if (IsValid(WG))
		{
			switch ((EEquipment)(this->EuipType))
			{
			case EEquipment::E_PlayerWeaponFirst: {
				WG->SelectWeapon(this->EuipType, this->ItemGrid->GetItemViewBlock(),
					this->FirstWeapon.M_FEquipment);
			}break;
			case EEquipment::E_PlayerWeaponSecond: {
				WG->SelectWeapon(this->EuipType, this->ItemGrid->GetItemViewBlock(),
					this->SecondWeapon.M_FEquipment);
			}break;
			case EEquipment::E_PlayerWeaponSuper: {
				WG->SelectWeapon(this->EuipType, this->ItemGrid->GetItemViewBlock(),
					this->SuperWeapon.M_FEquipment);
			}break;
			}

		}
	}
}