// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/MaterialDataStruct.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"

const TMap<int32, FBaseItemSave>& UPlayerBagMaterialView::GetItemTable() const {
	return GET_PLAYERDATAINS()->GetMaterList();
}

FName UPlayerBagMaterialView::GetTag() {
	return GET_DEF_CATEGORYNAME(Material);
}

FItemViewIndexRange UPlayerBagMaterialView::GetItemIndexRange(int32 ID, int32 Level, const TMap<int32, FBaseItemSave>& NewLoadItems, int32 JumpPosition)
{

	/*
		 折叠数据实现
		 1.将对应ID位置查询
		 2.累计等级作为索引的偏移
		 3.和数量无关

		例如：  itemID：20，itemLevelCount：6，itemCount：40，最终范围：[20,26)

	*/

	int32 IDBegin = JumpPosition;
	int32 BeginIndex = 0;
	const TMap<int32, FBaseItemSave>& Temp = NewLoadItems.Num() ? NewLoadItems : this->GetItemTable();
	for (const auto& Data : Temp)
	{
		//当ID不匹配时，直接将个数增加作为下一个物品的起始索引
		if (Data.Key != ID)
		{
			if (IDBegin != -1)
			{
				continue;
			}

			//每一个等级对应一个索引
			BeginIndex += Data.Value.ItemCount.Num();
		}
		else
		{

			//只有匹配到ID之后，就一定返回数据

			if (IDBegin != -1)
			{
				BeginIndex = IDBegin;
			}
			else {
				IDBegin = BeginIndex;
			}

			for (const auto& LocalLevel : Data.Value.ItemCount)
			{
				if (LocalLevel.Key == Level)
				{
					if (LocalLevel.Value > 0)
					{
						return FItemViewIndexRange(IDBegin, IDBegin, BeginIndex);
					}
				}
				else {
					//累计等级次数
					BeginIndex += 1;
				}
			}

			return FItemViewIndexRange(-1);
		}
	}

	return FItemViewIndexRange(-1);
}

FItemBaseViewBlock* UPlayerBagMaterialView::GetItem(int32 ID, int32 Level) {

	FItemCountViewBlock* Item = this->MaterialBlock.Find(ID);
	if (Item)
	{
		//更新数量
		const FBaseItemSave* NewItem = this->GetItemTable().Find(ID);
		if (NewItem)
		{
			const int32* Count = NewItem->ItemCount.Find(Level);
			if (Count)
			{
				Item->Count = *Count;
				return Item;
			}
		}

		//如果查询失败，数量则为0
		Item->Count = 0;
		return Item;
	}

	//查询
	FMaterialBase Data;
	if (GET_CACHE(Material)->GetMaterData(ID, Data))
	{
		FItemCountViewBlock NewData;
		NewData.Name = Data.ItemName;
		NewData.Texture = this->GetItemTexture(ID);
		NewData.Type = (uint8)Data.M_MaterialType;
		//设置物品数量
		const FBaseItemSave* const SaveBlock = this->GetItemTable().Find(ID);
		if (SaveBlock)
		{
			const int32* const ItemCount = SaveBlock->ItemCount.Find(Level);
			if (ItemCount)
			{
				NewData.Count = *ItemCount;
				return &this->MaterialBlock.Emplace(ID, NewData);
			}
		}
		NewData.Count = 0;
		return &this->MaterialBlock.Emplace(ID, NewData);
	}


	return nullptr;
}

TArray<FItemViewBlock> UPlayerBagMaterialView::GenerateItemView(int32 GenerateMax) {

	TArray<FItemViewBlock> GenList;

	//遍历  ID
	for (const auto& Data : this->GetItemTable())
	{
		Data.Value.ToArrayZip(Data.Key, GenList, GenerateMax);
	}

	return GenList;
}

void UPlayerBagMaterialView::ClearCache() {
	Super::ClearCache();
}

void UPlayerBagMaterialView::Arrangement(
	const TArray<uint8>& Type,
	TMap<int32, FBaseItemSave>& NewItems
	, bool bAddOtherData
) {
	this->Arrangement_Temp(this->MaterialBlock, Type, NewItems, bAddOtherData);
}

const FName UMaterialDataAssetCache::GetClassTag() {
	return GET_DEF_CATEGORYNAME(Material);
}

bool UMaterialDataAssetCache::GetItemByID(int32 ItemID, uint8& OutType, void*& OutData) {
	uint8 Max = ((uint8)EMaterialType::E_Max);
	//如果outType是被设置过的类型，则直接跳转对应类型
	for (uint8 LocalType = 0U; LocalType < Max; LocalType++)
	{
		OutType = LocalType;
		switch ((EMaterialType)(LocalType))
		{
		case EMaterialType::E_Blueprint:GETITEMBYID_IMP(this->BlueprintDataTable, this->Blueprint, Material, Blueprint);
		case EMaterialType::E_CardSynthesisMaterial:GETITEMBYID_IMP(this->BlueprintMaterDataTable, this->BlueprintMater, Material, BlueprintMater);
		case EMaterialType::E_CardChangeJobMaterial:GETITEMBYID_IMP(this->ChangeDataTable, this->Change, Material, Change);
		case EMaterialType::E_CardSkillBook:GETITEMBYID_IMP(this->SkillBookDataTable, this->SkillBook, Material, SkillBook);
		case EMaterialType::E_Clover:GETITEMBYID_IMP(this->CloverDataTable, this->Clover, Material, Clover);
		case EMaterialType::E_Crystal:GETITEMBYID_IMP(this->CrystalDataTable, this->Crystal, Material, Crystal);
		case EMaterialType::E_LevelKey:GETITEMBYID_IMP(this->LevelKeyDataTable, this->LevelKey, Material, LevelKey);
		case EMaterialType::E_Bit:GETITEMBYID_IMP(this->BitDataTable, this->Bit, Material, Bit);
		case EMaterialType::E_Spices:GETITEMBYID_IMP(this->SpicesDataTable, this->Spices, Material, Spices);
		case EMaterialType::E_Ticket:GETITEMBYID_IMP(this->TicketDataTable, this->Ticket, Material, Ticket);
		}
	}
	return false;
}

void UMaterialDataAssetCache::GetAllItemID(TMap<int32, FString>& OutData) {
	UMaterialDataAssetCache::GetAllMaterData(OutData);
}

bool UMaterialDataAssetCache::GetMaterData(int32 ID, EMaterialType& OutType, FMaterialBase*& OutDataStruct) {
	//通过ID拿到Type，进行数据类型转换
	uint8 Type = 0U;
	FItemBaseStructData* DataStruct = nullptr;
	if (GET_ITEM(ID, Type, DataStruct, GET_DEF_CATEGORYNAME(Material)))
	{
		OutType = (EMaterialType)(Type);
		switch (OutType)
		{
		case EMaterialType::E_Blueprint:
		{ OutDataStruct = &CAST_TYPE_PTR(FMaterial_CardSynthesisBlueprint_Data, DataStruct)->M_FMaterial; }break;
		case EMaterialType::E_CardSynthesisMaterial:
		{ OutDataStruct = &CAST_TYPE_PTR(FMaterial_CardSynthesisMaterial_Data, DataStruct)->M_FMaterial; }break;
		case EMaterialType::E_CardChangeJobMaterial:
		{ OutDataStruct = &CAST_TYPE_PTR(FMaterial_CardChangeJobMaterial_Data, DataStruct)->M_FMaterial; }break;
		case EMaterialType::E_Spices:
		{ OutDataStruct = &CAST_TYPE_PTR(FMaterial_SpicesMaterial_Data, DataStruct)->M_FMaterial; }break;
		case EMaterialType::E_Clover:
		{ OutDataStruct = &CAST_TYPE_PTR(FMaterial_CloverMaterial_Data, DataStruct)->M_FMaterial; }break;
		case EMaterialType::E_CardSkillBook:
		{ OutDataStruct = &CAST_TYPE_PTR(FCardSkillBooks_Data, DataStruct)->M_FMaterial; }break;
		case EMaterialType::E_Ticket:
		{ OutDataStruct = &CAST_TYPE_PTR(FMaterial_TicketMaterial_Data, DataStruct)->M_FMaterial; }break;
		case EMaterialType::E_Crystal:
		{ OutDataStruct = &CAST_TYPE_PTR(FMaterial_Crystal_Data, DataStruct)->M_FMaterial; }break;
		case EMaterialType::E_Bit:
		{ OutDataStruct = &CAST_TYPE_PTR(FMaterial_Bit_Data, DataStruct)->M_FMaterial; }break;
		case EMaterialType::E_LevelKey:
		{ OutDataStruct = &CAST_TYPE_PTR(FMaterial_LevelKey_Data, DataStruct)->M_FMaterial; }break;

		}

		if (OutDataStruct)
		{
			OutDataStruct->ItemID = ID;
			return true;
		}
	}
	return false;
}

bool UMaterialDataAssetCache::GetMaterData(int32 ID, FMaterialBase& OutDataStruct) {
	EMaterialType Type = EMaterialType::E_Blueprint;
	FMaterialBase* CurData = nullptr;
	if (UMaterialDataAssetCache::GetMaterData(ID, Type, CurData))
	{
		OutDataStruct = *CurData;
		return true;
	}
	return false;
}

DECLAREGETALLITEMDATAFUNC(GetAllMaterData, Material)
void UMaterialDataAssetCache::GetAllMaterData(TMap<int32, FString>& OutData) {
	UMaterialDataAssetCache* Cache = GET_CACHE(Material);
	if (IsValid(Cache))
	{
		//通过当前缓存，返回所有的数据名称
		GetAllMaterDataFunc(Cache->BlueprintDataTable, Cache->Blueprint, GET_MOVE_NAME(Material, Blueprint), OutData);
		GetAllMaterDataFunc(Cache->BlueprintMaterDataTable, Cache->BlueprintMater, GET_MOVE_NAME(Material, BlueprintMater), OutData);
		GetAllMaterDataFunc(Cache->ChangeDataTable, Cache->Change, GET_MOVE_NAME(Material, Change), OutData);
		GetAllMaterDataFunc(Cache->SpicesDataTable, Cache->Spices, GET_MOVE_NAME(Material, Spices), OutData);
		GetAllMaterDataFunc(Cache->CloverDataTable, Cache->Clover, GET_MOVE_NAME(Material, Clover), OutData);
		GetAllMaterDataFunc(Cache->SkillBookDataTable, Cache->SkillBook, GET_MOVE_NAME(Material, SkillBook), OutData);
		GetAllMaterDataFunc(Cache->TicketDataTable, Cache->Ticket, GET_MOVE_NAME(Material, Ticket), OutData);
		GetAllMaterDataFunc(Cache->CrystalDataTable, Cache->Crystal, GET_MOVE_NAME(Material, Crystal), OutData);
		GetAllMaterDataFunc(Cache->BitDataTable, Cache->Bit, GET_MOVE_NAME(Material, Bit), OutData);
		GetAllMaterDataFunc(Cache->LevelKeyDataTable, Cache->LevelKey, GET_MOVE_NAME(Material, LevelKey), OutData);
	}

#if WITH_EDITOR
	GAME_LOG(__FUNCTION__, TEXT("材料名称返回"), {
		for (auto TempData : OutData)
		{
			GAME_LOG_LINE(TEXT("Name:[%s]，ID:[%d]"), *TempData.Value, TempData.Key);
		}
		});
#endif
}