// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/CardData/CardDataStruct.h"
#include "CardDataComponent.generated.h"


//卡片数据对象
UCLASS()
class FVM_API UCardDataObject : public UObject
{
	GENERATED_BODY()

public:

	//获取攻击类型的卡片数据
	UFUNCTION(BlueprintPure)
	FItemCardATK GetCardATKData(int32 ID);
	//获取生产类型的卡片数据
	UFUNCTION(BlueprintPure)
	FItemCardSpawn GetCardSpawnData(int32 ID);
	//获取防御类型的卡片数据
	UFUNCTION(BlueprintPure)
	FItemCardDefence GetCardDefenceData(int32 ID);
	//获取功能类型的卡片数据
	UFUNCTION(BlueprintPure)
	FItemCardFunction GetCardFunction(int32 ID);
	//获取范围类型的卡片数据
	UFUNCTION(BlueprintPure)
	FItemCardRangeATK GetCardRangeATKData(int32 ID);

	//获取卡片数据
	template<typename Type>
	Type GetCardDataByID(int32 ID)
	{
		if (!this->DataCachePtr.IsValid())
		{
			this->DataCachePtr = MakeShareable(new TMap<int32, void*>());
		}
		else {
			void** Data = this->DataCachePtr->Find(ID);
			if (Data)
			{
				return CAST_TYPE(Type,*Data);
			}
		}

		if (!IsValid(this->CardCache))
		{
			this->CardCache = GET_CACHE(Card);
		}

		ECardType CardType;
		FItemCard* Data = nullptr;
		if (this->CardCache->GetCardData(ID, CardType, Data))
		{
			DataCachePtr->Emplace(ID, CAST_TYPE_PTR(void, Data));
			return CAST_TYPE(Type, Data);
		}

		return Type();
	}
private:
	UPROPERTY()
	class UCardDataAssetCache* CardCache = nullptr;

	//数据缓存，所有查询过的数据都将被缓存
	TSharedPtr<TMap<int32, void*>> DataCachePtr;
};
