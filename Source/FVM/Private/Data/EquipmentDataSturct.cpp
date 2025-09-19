// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/EquipmentDataSturct.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "GameSystem/PlayerStructManager.h"

const TMap<int32, FBaseItemSave>& UPlayerBagEquipView::GetItemTable() const {
	return GET_PLAYERDATAINS()->GetSourceEquipList();
}

FName UPlayerBagEquipView::GetTag() {
	return GET_DEF_CATEGORYNAME(Equip);
}

void UPlayerBagEquipView::Arrangement(
	const TArray<uint8>& Type,
	TMap<int32, FBaseItemSave>& NewItems
	, bool bAddOtherData
) {
	this->Arrangement_Temp(this->EquipBlock, Type, NewItems, bAddOtherData);
}

TArray<FItemViewBlock> UPlayerBagEquipView::GenerateItemView(int32 GenerateMax) {
	/*
		动态解析，具有较大的消耗
	*/
	//SCOPE_CYCLE_COUNTER(STAT_GenerateItemView);

	//获取角色存档
	UPlayerStructManager* Player = GET_PLAYERDATAINS();

	//装备的物品
	TArray<FItemViewBlock> bEuipArray;
	//未装备的物品
	TArray<FItemViewBlock> bNotEquipArray;

	//遍历  ID ,未装备的物品
	for (const auto& SourceItem : this->GetItemTable())
	{
		//压缩数据
		SourceItem.Value.ToArrayZipCheckEquip(
			Player, SourceItem.Key, this->GenerateEuipItemListTable(),
			bEuipArray, bNotEquipArray, GenerateMax
		);
	}

	this->EuipItemCount = bEuipArray.Num();
	bEuipArray.Append(MoveTempIfPossible(bNotEquipArray));
	return bEuipArray;
}

const TMap<int32, FEquipItemSave>& UPlayerBagEquipView::GenerateEuipItemListTable() const {
	return GET_PLAYERDATAINS()->GetbEquipList();
}

void UPlayerBagEquipView::GenerateItemViewSourceByType(
	const EEquipment& EquipType,
	TMap<int32, FBaseItemSave>& OutItemViewSource) {

	for (const auto& Data : this->GetItemTable())
	{
		for (const auto& EquipData : Data.Value.ItemCount)
		{
			//根据配置表，来决定是否使用压缩数据
			FItemCountViewBlock* TempBlock = CAST_TYPE_PTR(FItemCountViewBlock,
				this->GetItem(
					Data.Key,
					EquipData.Key));

			//获取背包类型
			if ((EEquipment)TempBlock->Type == EquipType)
			{
				OutItemViewSource.Emplace(Data.Key, Data.Value);
				break;
			}
		}
	}
}

FItemBaseViewBlock* UPlayerBagEquipView::GetItem(int32 ID, int32 Level) {

	FItemCountViewBlock* StrBlock = this->EquipBlock.Find(ID);
	if (StrBlock)
	{
		//重新更新数量
		const FBaseItemSave* Save = this->GetItemTable().Find(ID);
		if (Save)
		{
			const int32* Count = Save->ItemCount.Find(Level);
			if (Count)
			{
				StrBlock->Count = *Count;
			}
		}

		return StrBlock;
	}

	//查询
	FEquipmentBase Data;
	if (GET_CACHE(Equip)->GetEquipData(ID, Data))
	{
		FItemCountViewBlock NewData;
		NewData.Name = Data.ItemName;
		NewData.Texture = this->GetItemTexture(ID);
		NewData.Type = (uint8)Data.M_EquipmentType;
		//设置物品数量
		const FBaseItemSave* const EquipSave = this->GetItemTable().Find(ID);
		if (EquipSave)
		{
			const int32* const EquipCount = EquipSave->ItemCount.Find(Level);
			if (EquipCount)
			{
				NewData.Count = *EquipCount;
				return &this->EquipBlock.Emplace(ID, NewData);
			}
		}

		NewData.Count = 0;
		return &this->EquipBlock.Emplace(ID, NewData);
	}

	return StrBlock;
}

//当前接口的名称
const FName UEquipDataAssetCache::GetClassTag() {
	return GET_DEF_CATEGORYNAME(Equip);
}
//通过id获取数据
bool UEquipDataAssetCache::GetItemByID(int32 ItemID, uint8& OutType, void*& OutData) {

	uint8 Max = ((uint8)EEquipment::E_Max);
	for (uint8 LocalType = 0u; LocalType < Max; LocalType++)
	{
		OutType = LocalType;
		switch ((EEquipment)(LocalType))
		{
		case EEquipment::E_Bag:GETITEMBYID_IMP(this->BagDataTable, this->BagA, Equip, Bag);
		case EEquipment::E_Gift:GETITEMBYID_IMP(this->GiftBoxDataTable, this->GiftBoxA, Equip, GiftBox);
		case EEquipment::E_PlayerEquipment: {
			//初始化服装数据
			this->InitPlayerEquipment();
			//查询服装数据
			FIND_MAP_BODY_FINDINTERFACE(this->PlayerEquipmentDataTable);
		}break;
		case EEquipment::E_PlayerWeaponFirst:GETITEMBYID_IMP(this->WeaponFirstDataTable, this->WeaponFirstA, Equip, WeaponFirst);
		case EEquipment::E_PlayerWeaponSecond:GETITEMBYID_IMP(this->WeaponSecondDataTable, this->WeaponSecondA, Equip, WeaponSecond);
		case EEquipment::E_PlayerWeaponSuper:GETITEMBYID_IMP(this->WeaponSuperDataTable, this->WeaponSuperA, Equip, WeaponSuper);
		case EEquipment::E_WeaponGem:GETITEMBYID_IMP(this->WeaponGemDataTable, this->WeaponGemA, Equip, WeaponGem);

		}
	}

	return false;
}

void UEquipDataAssetCache::GetAllItemID(TMap<int32, FString>& OutData) {
	UEquipDataAssetCache::GetAllEquipData(OutData);
}

bool UEquipDataAssetCache::GetEquipData(int32 ID, EEquipment& OutType, FEquipmentBase*& OutDataStruct) {

	//通过ID拿到Type，进行数据类型转换
	uint8 Type;
	FItemBaseStructData* DataStruct;
	if (GET_ITEM(ID, Type, DataStruct, GET_DEF_CATEGORYNAME(Equip)))
	{
		OutType = (EEquipment)(Type);
		switch (OutType)
		{
		case EEquipment::E_Bag:
		{ OutDataStruct = &CAST_TYPE_PTR(FEquipment_Bag_Data, DataStruct)->M_FEquipment; }break;
		case EEquipment::E_Gift:
		{ OutDataStruct = &CAST_TYPE_PTR(FEquipment_GiftBox_Data, DataStruct)->M_FEquipment; }break;
		case EEquipment::E_PlayerEquipment:
		{ OutDataStruct = &CAST_TYPE_PTR(FEquipment_PlayerEquipment_Data, DataStruct)->M_FEquipment; }break;
		case EEquipment::E_PlayerWeaponFirst:
		{ OutDataStruct = &CAST_TYPE_PTR(FEquipment_WeaponFirst_Data, DataStruct)->M_FEquipment; }break;
		case EEquipment::E_PlayerWeaponSecond:
		{ OutDataStruct = &CAST_TYPE_PTR(FEquipment_WeaponSecond_Data, DataStruct)->M_FEquipment; }break;
		case EEquipment::E_PlayerWeaponSuper:
		{ OutDataStruct = &CAST_TYPE_PTR(FEquipment_WeaponSuper_Data, DataStruct)->M_FEquipment; }break;
		case EEquipment::E_WeaponGem:
		{ OutDataStruct = &CAST_TYPE_PTR(FEquipment_WeaponGem_Data, DataStruct)->M_FEquipment; }break;
		}

		if (OutDataStruct)
		{
			OutDataStruct->ItemID = ID;
			return true;
		}

	}

	return false;
}

bool UEquipDataAssetCache::GetEquipData(int32 ID, FEquipmentBase& OutDataStruct) {
	EEquipment Type;
	FEquipmentBase* CurData = nullptr;
	if (UEquipDataAssetCache::GetEquipData(ID, Type, CurData))
	{
		OutDataStruct = *CurData;
		return true;
	}
	return false;
}

DECLAREGETALLITEMDATAFUNC(GetAllEquipData, Equip)
void UEquipDataAssetCache::GetAllEquipData(TMap<int32, FString>& OutData) {
	UEquipDataAssetCache* Cache = GET_CACHE(Equip);
	if (IsValid(Cache))
	{
		//通过当前缓存，返回所有的数据名称
		GetAllEquipDataFunc(Cache->BagDataTable, Cache->BagA, GET_MOVE_NAME(Equip, Bag), OutData);
		GetAllEquipDataFunc(Cache->GiftBoxDataTable, Cache->GiftBoxA, GET_MOVE_NAME(Equip, GiftBox), OutData);
		GetAllEquipDataFunc(Cache->WeaponFirstDataTable, Cache->WeaponFirstA, GET_MOVE_NAME(Equip, WeaponFirst), OutData);
		GetAllEquipDataFunc(Cache->WeaponSecondDataTable, Cache->WeaponSecondA, GET_MOVE_NAME(Equip, WeaponSecond), OutData);
		GetAllEquipDataFunc(Cache->WeaponSuperDataTable, Cache->WeaponSuperA, GET_MOVE_NAME(Equip, WeaponSuper), OutData);
		GetAllEquipDataFunc(Cache->WeaponGemDataTable, Cache->WeaponGemA, GET_MOVE_NAME(Equip, WeaponGem), OutData);

		//初始化服装数据
		Cache->InitPlayerEquipment();
		GetAllEquipDataFunc(Cache->PlayerEquipmentDataTable, Cache->PlayerEquipmentA, GET_MOVE_NAME(Equip, BodyMan), OutData);

	}

#if WITH_EDITOR
	GAME_LOG(__FUNCTION__, TEXT("装备名称返回"), {
		for (auto TempData : OutData)
		{
			GAME_LOG_LINE(TEXT("Name:[%s]，ID:[%d]"), *TempData.Value, TempData.Key);
		}
		});
#endif
}

void UEquipDataAssetCache::InitPlayerEquipment() {

	//如果已经初始化了数据，则退出
	if (this->PlayerEquipmentDataTable.Num())
	{
		return;
	}

	TArray<FEquipment_PlayerEquipment_Data> LoadData;
	auto TempInitData = [&](GameDataName&& Name) {
		UGameSystemFunction::GetDataTableRows(
			UGameDataSubsystem::GetGameDataSubsystemStatic()->GetAsset()->
			GetDataByName(GET_MOVE_CATEGORYNAME(Equip))->GetDataByName(Name), LoadData);
		};

	TempInitData(GET_MOVE_NAME(Equip, BodyMan));
	TempInitData(GET_MOVE_NAME(Equip, BodyMen));
	TempInitData(GET_MOVE_NAME(Equip, HairMen));
	TempInitData(GET_MOVE_NAME(Equip, GlassesMen));
	TempInitData(GET_MOVE_NAME(Equip, CapMen));
	TempInitData(GET_MOVE_NAME(Equip, CapBase));
	TempInitData(GET_MOVE_NAME(Equip, FaceMen));
	TempInitData(GET_MOVE_NAME(Equip, EyeMen));
	TempInitData(GET_MOVE_NAME(Equip, EyeBase));
	TempInitData(GET_MOVE_NAME(Equip, FlyItemMan));
	TempInitData(GET_MOVE_NAME(Equip, FlyItemMen));
	TempInitData(GET_MOVE_NAME(Equip, FlyItemBase));
	TempInitData(GET_MOVE_NAME(Equip, SuitMan));
	TempInitData(GET_MOVE_NAME(Equip, SuitMen));
	TempInitData(GET_MOVE_NAME(Equip, SuitBase));

	for (const auto& CurData : LoadData)
	{
		this->PlayerEquipmentDataTable.Emplace(CurData.ItemID, CurData);
	}
}