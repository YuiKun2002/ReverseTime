﻿// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "DataTableAssetData.generated.h"

class UDataTable;

//初始化数据表
UDataTable* InitDataTable(FName&& Name1, FName&& Name2);

//纹理数据资产
USTRUCT(BlueprintType)
struct FVM_API FDataTableManager
{
	GENERATED_USTRUCT_BODY()

public:
	FDataTableManager() {}
	FDataTableManager(FName&& Name1, FName&& Name2) {
		this->DataTableInstance = InitDataTable(MoveTemp(Name1), MoveTemp(Name2));
	}

	//初始化
	void InitDataTableManager(FName&& Name1, FName&& Name2) {
		if (this->DataTableInstance)
		{
			return;
		}
		else {
			this->DataTableInstance = InitDataTable(MoveTemp(Name1), MoveTemp(Name2));
		}
	}

	//卸载
	void Unload() {
		this->DataTableInstance = nullptr;
	}

	//是否有效
	bool IsValid() {
		return this->DataTableInstance == nullptr ? false : true;
	}

	//是否存在
	bool Find(const FName& Name) {
		return this->DataTableInstance->GetRowMap().Contains(Name);
	}

	//获取名称
	void GetNames(TArray<FName>& Names) {
		this->DataTableInstance->GetRowMap().GetKeys(Names);
	}

	//获取数量
	int32 GetNum() {
		return this->DataTableInstance->GetRowMap().Num();
	}

	//获取数据
	template<typename ValueType>
	void GetValues(TArray<ValueType>& OutValues) {
		OutValues.Reserve(OutValues.Num() + this->DataTableInstance->GetRowMap().Num());
		for (
			TMap<FName, uint8*>::TConstIterator RowMapIter(
				this->DataTableInstance->GetRowMap().CreateConstIterator());
			RowMapIter;
			++RowMapIter
			)
		{
			OutValues.Add(
				*(reinterpret_cast<ValueType*>(
					RowMapIter.Value()
					)));
		}
	}

	template<typename ValueType>
	ValueType GetDataByName(const FName& Name)
	{
		uint8* const* TempData = this->DataTableInstance->GetRowMap().Find(Name);
		if (TempData)
		{
			return *(reinterpret_cast<ValueType*>(
				*TempData
				));
		}

		ValueType NullValue;
		return NullValue;
	}

	template<typename ValueType>
	void GetDataByName(const FName& Name, ValueType& OutData) {
		OutData = this->GetDataByName<ValueType>(Name);
	}

	template<typename ValueType>
	FORCEINLINE ValueType* Find(const FName& Name) {
		return reinterpret_cast<ValueType*>(*DataTableInstance->GetRowMap().Find(Name));
	}

	template<typename ValueType>
	FORCEINLINE const ValueType* Find(const FName& Name) const {
		return (reinterpret_cast<const ValueType*>(*DataTableInstance->GetRowMap().Find(Name)));
	}

private:
	UPROPERTY()
	UDataTable* DataTableInstance = nullptr;
};

//数据表获取
template<class DataTableType>
class DataTableAssetData
{
public:
	DataTableAssetData(UDataTable* Data);
	DataTableAssetData() {};
public:
	//初始化数据表
	void InitDataTable(UDataTable* Data);
	//获取数据表
	const TMap<FName, DataTableType>& GetDatas();
	//获取值
	DataTableType GetDataByName(FName Name);
	void GetDataByName(FName Name, DataTableType& OutData);
	//获取Name数组
	void GetNames(TArray<FName>& Names);
	//获取值
	void GetValues(TArray<DataTableType>& Values);
public:
	//新增
	bool AddTemp(FName Name, const DataTableType& Data);
	//查询
	bool Find(FName Name);
public:
	//获取数量
	int32 Num() const;
private:
	void InitData(UDataTable* DataTable, TMap<FName, DataTableType>& OutDatas);
private:
	UPROPERTY()
	TMap<FName, DataTableType> SourceDatas;
};

template<class DataTableType>
bool DataTableAssetData<DataTableType>::Find(FName Name)
{
	return (SourceDatas.Find(Name) != nullptr);
}

template<class DataTableType>
bool DataTableAssetData<DataTableType>::AddTemp(FName Name, const DataTableType& Data)
{
	if (this->Find(Name))
	{
		return false;
	}

	this->SourceDatas.Emplace(Name, Data);
}


template<class DataTableType>
DataTableAssetData<DataTableType>::DataTableAssetData(
	UDataTable* Data
)
{
	if (IsValid(Data))
	{
		this->InitDataTable(Data);
	}
}

template<class DataTableType>
void DataTableAssetData<DataTableType>::InitDataTable(UDataTable* Data)
{
	if (IsValid(Data))
	{
		if (!this->SourceDatas.Num())
		{
			this->InitData(Data, this->SourceDatas);
		}
	}
	else {
		SourceDatas.Empty();
	}
}

template<class DataTableType>
void DataTableAssetData<DataTableType>::InitData(UDataTable* DataTable, TMap<FName, DataTableType>& OutDatas)
{
	OutDatas.Reserve(OutDatas.Num() + DataTable->GetRowMap().Num());
	for (
		TMap<FName, uint8*>::TConstIterator RowMapIter(DataTable->GetRowMap().CreateConstIterator());
		RowMapIter;
		++RowMapIter
		)
	{
		OutDatas.Add(
			RowMapIter.Key(),
			*(reinterpret_cast<DataTableType*>(
				RowMapIter.Value()
				)));
	}
}

template<class DataTableType>
const TMap<FName, DataTableType>& DataTableAssetData<DataTableType>::GetDatas()
{
	return this->SourceDatas;
}

template<class DataTableType>
void DataTableAssetData<DataTableType>::GetDataByName(FName Name, DataTableType& OutData)
{
	OutData = this->GetDataByName(Name);
}

template<class DataTableType>
DataTableType DataTableAssetData<DataTableType>::GetDataByName(FName Name)
{
	DataTableType* Target = this->SourceDatas.Find(Name);
	if (Target)
	{
		return *Target;
	}

	UE_LOG(LogTemp, Error, TEXT("[%s]%s：未找到Row？"), __FUNCTION__, *Name.ToString());

	return DataTableType();
}

template<class DataTableType>
void DataTableAssetData<DataTableType>::GetValues(TArray<DataTableType>& Values)
{
	this->SourceDatas.GenerateValueArray(Values);
}

template<class DataTableType>
void DataTableAssetData<DataTableType>::GetNames(TArray<FName>& Names)
{
	this->SourceDatas.GenerateKeyArray(Names);
}

template<class DataTableType>
int32 DataTableAssetData<DataTableType>::Num() const
{
	return this->SourceDatas.Num();
}

//填充数据,用于填充物品类型的数据表，带有ID结构的结构体
template<typename ValueType>
void FillDataTable(
	TMap<int32, ValueType>& InData,
	DataTableAssetData<ValueType>& InTable,
	FName&& DataName,
	FName&& Name
) {
	if (InData.Num() == 0)
	{
		InTable.InitDataTable(InitDataTable(MoveTemp(DataName), MoveTemp(Name)));

		TArray<ValueType> Values;
		InTable.GetValues(Values);

		for (const ValueType& Data : Values)
		{
			InData.Emplace(Data.ItemID, Data);
		}

		InTable.InitDataTable(nullptr);
	}
}

//初始化数据表
template<typename ValueType>
void FillDataTable(
	TMap<FName, ValueType>& InData,
	DataTableAssetData<ValueType>& InTable,
	FName&& DataName,
	FName&& Name
) {
	if (InData.Num() == 0)
	{
		InTable.InitDataTable(InitDataTable(MoveTemp(DataName), MoveTemp(Name)));

		TArray<ValueType> Values;
		InData.Append(InTable.GetDatas());

		InTable.InitDataTable(nullptr);
	}
}

////初始化数据表
//template<typename ValueType>
//void FillDataTable(
//	DataTableAssetData<ValueType>& InTable,
//	FName&& DataName,
//	FName&& Name
//) {
//	if (InData.Num() == 0)
//	{
//		InTable.InitDataTable(InitDataTable(MoveTemp(DataName), MoveTemp(Name)));
//
//		TArray<ValueType> Values;
//		InData.Append(InTable.GetDatas());
//
//		InTable.InitDataTable(nullptr);
//	}
//}

//获取数据表原生数据
template<typename Type>
TArray<Type>& GetDataTableSourceData(
	TArray<Type>& InData,
	DataTableAssetData<Type>& InTable,
	FName&& DataName,
	FName&& Name
) {
	if (InData.Num())
	{
		return InData;
	}
	else {

		InTable.InitDataTable(InitDataTable(MoveTemp(DataName), MoveTemp(Name)));

		InTable.GetValues(InData);

		InTable.InitDataTable(nullptr);
	}

	return InData;
}