// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/DataDataAssetCache.h"

void UDataDataAssetCache::InitCache_Implementation() {

}

void UDataDataAssetCache::InitGameID() {

	if (!this->ItemIds.Num())
	{
		UDataTable* Tab = UGameDataSubsystem::GetGameDataSubsystemStatic()->GetAsset()->
			GetDataByName(GET_MOVE_CATEGORYNAME(Data))->GetDataByName(GET_MOVE_NAME(Data, GameID));
		if (IsValid(Tab))
		{
			//初始化数据表 行(Ids)对应的数据
			this->ItemIds.Emplace(FName("Ids"), *Tab->FindRow<FItemIdDataTable>(FName("Ids"), TEXT("IdsCont")));
		}
	}

}

void UDataDataAssetCache::InitGameItemID() {
	if (!this->GameItemIds.Num())
	{
		UDataTable* Tab = UGameDataSubsystem::GetGameDataSubsystemStatic()->GetAsset()->
			GetDataByName(GET_MOVE_CATEGORYNAME(Data))->GetDataByName(GET_MOVE_NAME(Data, ItemID));
		if (IsValid(Tab))
		{
			//初始化数据表 行(Ids)对应的数据
			this->GameItemIds.Emplace(FName("Ids"), *Tab->FindRow<FItemId>(FName("Ids"), TEXT("IdsCont")));
		}
	}
}

void UDataDataAssetCache::GetItemIDAndName(TMap<FString, int32>& OutIds) {

	this->InitGameID();

	FItemIdDataTable* Data = this->ItemIds.Find(FName("Ids"));
	if (Data) {
		OutIds = Data->Item;
	}

}

void UDataDataAssetCache::GetItemMaxID(int32& MaxID) {

	this->InitGameItemID();

	FItemId* Data = this->GameItemIds.Find(FName("Ids"));
	if (Data) {
		MaxID = Data->MaxID;
	}
}

void UDataDataAssetCache::GetItemDeleteID(TArray<int32>& OutDeleteIds) {
	this->InitGameItemID();

	FItemId* Data = this->GameItemIds.Find(FName("Ids"));
	if (Data) {
		OutDeleteIds = Data->DeleteID;
	}
}

