// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/GameDataSubsystem.h"
#include "GameSystem/Item/ItemIdDataTable.h"
#include "DataDataAssetCache.generated.h"

/**
 * 一些特别的数据-数据缓存对象
 */
UCLASS()
class FVM_API UDataDataAssetCache : public UGameDataAssetCache
{
	GENERATED_BODY()

	virtual void InitCache_Implementation() override;


public:
	//获取物品的ID号和物品名称
	void GetItemIDAndName(TMap<FString, int32>& OutIds);
	//获取当前物品最大的ID号
	void GetItemMaxID(int32& MaxID);
	//获取当前已经被删除的ID号数组
	void GetItemDeleteID(TArray<int32>& OutDeleteIds);
protected:
	//初始化游戏ID
	void InitGameID();
	//初始化游戏ID[游戏ID最大ID以及删除的ID]
	void InitGameItemID();

private:
	//游戏ID集合
	UPROPERTY()
	TMap<FName, FItemIdDataTable> ItemIds;
	//游戏ID最大ID以及删除的ID
	UPROPERTY()
	TMap<FName, FItemId> GameItemIds;
};
