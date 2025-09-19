// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/CardData/CardDataStruct.h"
#include "GameSystem/PlayerStructManager.h"

const TMap<int32, FBaseItemSave>& UPlayerBagCardView::GetItemTable() const {
	return GET_PLAYERDATAINS()->GetCardList();
}

FName UPlayerBagCardView::GetTag() {
	return GET_DEF_CATEGORYNAME(Card);
}

void UPlayerBagCardView::Arrangement(
	const TArray<uint8>& Type,
	TMap<int32, FBaseItemSave>& NewItems,
	bool bAddOtherData
) {
	this->Arrangement_Temp(this->CardBlock, Type, NewItems, bAddOtherData);
}

FItemBaseViewBlock* UPlayerBagCardView::GetItem(int32 ID, int32 Level)
{
	FItemCountViewBlock* StrCardBlock = this->CardBlock.Find(ID);
	if (StrCardBlock)
	{
		return StrCardBlock;
	}

	//查询卡片
	FItemCard Data;
	if (GET_CACHE(Card)->GetCardData(ID, Data))
	{

		//自动提升价格
		if (Data.M_CardPriceAutoUp)
		{
			this->AutoPrice.Emplace(ID);
		}

		//高级卡片
		if (Data.M_SpecialCard_SynthesisUpGrade)
		{
			this->AdvancedCard.Emplace(ID);
		}

		FItemCountViewBlock NewData;
		NewData.Count = Data.M_CardPrice;
		NewData.Name = Data.ItemName;
		NewData.Type = (uint8)Data.M_ECardType;
		NewData.Texture = this->GetItemTexture(ID);

		return &this->CardBlock.Emplace(ID, NewData);
	}

	return StrCardBlock;
}

const FName UCardDataAssetCache::GetClassTag() {
	return GET_DEF_CATEGORYNAME(Card);
}

bool UCardDataAssetCache::GetItemByID(int32 ItemID, uint8& OutType, void*& OutData) {

	//循环Type 查询ID，一并返回Type
	uint8 Max = ((uint8)ECardType::E_Max);
	for (uint8 LocalType = 0u; LocalType < Max; LocalType++)
	{
		OutType = LocalType;
		switch ((ECardType)(LocalType))
		{
		case ECardType::E_ATK:GETITEMBYID_IMP(this->CardATKDataTable, this->CardATK, Card, ATK);
		case ECardType::E_SPAWN:GETITEMBYID_IMP(this->CardSpawnDataTable, this->CardSpawn, Card, Spawn);
		case ECardType::E_DEFENCE:GETITEMBYID_IMP(this->CardDefenceDataTable, this->CardDefence, Card, Defence);
		case ECardType::E_Function:GETITEMBYID_IMP(this->CardFunctionDataTable, this->CardFunction, Card, Function);
		case ECardType::E_RangeATK:GETITEMBYID_IMP(this->CardRangeATKDataTable, this->CardRangeATK, Card, RangeATK);
		}
	}
	return false;
}

void UCardDataAssetCache::GetAllItemID(TMap<int32, FString>& OutData) {

	UCardDataAssetCache::GetAllCardData(OutData);

}

bool UCardDataAssetCache::GetCardData(int32 ID, ECardType& OutType, FItemCard*& OutDataStruct) {

	//通过ID拿到Type，进行数据类型转换
	uint8 Type;
	FItemBaseStructData* DataStruct;
	if (GET_ITEM(ID, Type, DataStruct, GET_DEF_CATEGORYNAME(Card)))
	{
		OutType = (ECardType)(Type);
		switch (OutType)
		{
		case ECardType::E_ATK:
		{ OutDataStruct = &CAST_TYPE_PTR(FCard_ATK_Data, DataStruct)->M_FItemCard; }break;
		case ECardType::E_SPAWN:
		{ OutDataStruct = &CAST_TYPE_PTR(FCard_Spawn_Data, DataStruct)->M_FItemCard; }break;
		case ECardType::E_DEFENCE:
		{ OutDataStruct = &CAST_TYPE_PTR(FCard_Defence_Data, DataStruct)->M_FItemCard; }break;
		case ECardType::E_Function:
		{ OutDataStruct = &CAST_TYPE_PTR(FCard_Function_Data, DataStruct)->M_FItemCard; }break;
		case ECardType::E_RangeATK:
		{ OutDataStruct = &CAST_TYPE_PTR(FCard_RangeATK_Data, DataStruct)->M_FItemCard; }break;

		}

		if (OutDataStruct)
		{
			OutDataStruct->ItemID = ID;
			return true;
		}
	}
	return false;
}

bool UCardDataAssetCache::GetCardData(int32 ID, FItemCard& OutDataStruct)
{
	ECardType Type;
	FItemCard* CurData = nullptr;
	if (UCardDataAssetCache::GetCardData(ID, Type, CurData))
	{
		OutDataStruct = *CurData;
		return true;
	}
	return false;
}

DECLAREGETALLITEMDATAFUNC(GetAllCardData, Card)
void UCardDataAssetCache::GetAllCardData(TMap<int32, FString>& OutData) {
	UCardDataAssetCache* Cache = GET_CACHE(Card);
	if (IsValid(Cache))
	{
		//通过当前缓存，返回所有的数据名称
		GetAllCardDataFunc(Cache->CardATKDataTable, Cache->CardATK, GET_MOVE_NAME(Card, ATK), OutData);
		GetAllCardDataFunc(Cache->CardSpawnDataTable, Cache->CardSpawn, GET_MOVE_NAME(Card, Spawn), OutData);
		GetAllCardDataFunc(Cache->CardDefenceDataTable, Cache->CardDefence, GET_MOVE_NAME(Card, Defence), OutData);
		GetAllCardDataFunc(Cache->CardFunctionDataTable, Cache->CardFunction, GET_MOVE_NAME(Card, Function), OutData);
		GetAllCardDataFunc(Cache->CardRangeATKDataTable, Cache->CardRangeATK, GET_MOVE_NAME(Card, RangeATK), OutData);
	}
#if WITH_EDITOR
	GAME_LOG(__FUNCTION__, TEXT("卡片名称返回"), {
		for (auto TempData : OutData)
		{
			GAME_LOG_LINE(TEXT("Name:[%s]，ID:[%d]"), *TempData.Value, TempData.Key);
		}
		});
#endif
}