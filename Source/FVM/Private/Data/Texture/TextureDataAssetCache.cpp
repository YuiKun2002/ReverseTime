// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/Texture/TextureDataAssetCache.h"
#include "GameSystem/Item/ItemBaseStruct.h"
#include "Engine/AssetManager.h"
#include "Engine/Texture2D.h"

UTextureDataAssetCache* UTextureDataAssetCache::GetTextureDataAssetCache() {
	return GET_CACHE(Texture);
}

UTexture2D* UTextureDataAssetCache::GetItemForLevel(
	FDataTableManager& Manager, int32 Level, int32 MinLevel, int32 MaxLevel) {

	if (Level < MinLevel)
	{
		Level = MinLevel;
	}
	else if (Level > MaxLevel)
	{
		Level = MaxLevel;
	}

	FTextureDataAsset* TempData = Manager.Find<FTextureDataAsset>(FName(FString::FromInt(Level)));
	if (TempData)
	{
		if (TempData->TextureInstance == nullptr)
		{
			TempData->TextureInstance = Cast<UTexture2D>(
				UAssetManager::GetStreamableManager().LoadSynchronous(
					TempData->Texture
				));
		}

		return TempData->TextureInstance;
	}

	return nullptr;
}

UTexture2D* UTextureDataAssetCache::GetPlayerLevelTexture(int32 Level) {
	this->PlayerLevelTextureSource.InitDataTableManager(
		GET_MOVE_CATEGORYNAME(Texture),
		GET_MOVE_NAME(Texture, PlayerLevelTexture)
	);

	return this->GetItemForLevel(this->PlayerLevelTextureSource, Level, 1, 60);
}

UTexture2D* UTextureDataAssetCache::GetItemLevelTexture(int32 Level) {

	this->ItemLevelSmallLogSource.InitDataTableManager(
		GET_MOVE_CATEGORYNAME(Texture),
		GET_MOVE_NAME(Texture, ItemLevelLog)
	);

	return this->GetItemForLevel(this->ItemLevelSmallLogSource, Level, 1, 16);
}

UTexture2D* UTextureDataAssetCache::GetCoinTexture(int32 Level) {

	this->CoinLevelLogSource.InitDataTableManager(
		GET_MOVE_CATEGORYNAME(Texture),
		GET_MOVE_NAME(Texture, Coin)
	);

	return this->GetItemForLevel(this->CoinLevelLogSource, Level, 0, this->CoinLevelLogSource.GetNum() - 1);
}

UTexture2D* UTextureDataAssetCache::GetItemTexture(int32 ItemID) {
	UTexture2D** Texture = this->ItemsTexture2D.Find(ItemID);
	if (Texture)
	{
		return *Texture;
	}

	uint8 Type;
	FName TypeName = FName();
	FItemBaseStructData* DataStruct;
	if (GET_ITEM(ItemID, Type, DataStruct, TypeName))
	{
		FItemBase Base;
		if (DataStruct->GetSourceData(Base))
		{

			this->AddItemTextureCache(ItemID, Base.ItemTexturePath);

			return this->GetItemTexture(ItemID);
		}
	}

	return nullptr;
}

void UTextureDataAssetCache::AddItemTextureCache(int32 ItemID, const FSoftObjectPath& Texture2DPath) {

	if (!(this->ItemsTexture2D.Find(ItemID)))
	{
		//新增到  纹理
		UTexture2D* NewTextureIns = Cast<UTexture2D>(
			UAssetManager::GetStreamableManager().LoadSynchronous(Texture2DPath)
		);

		this->ItemsTexture2D.Emplace(ItemID, NewTextureIns);
	}
}