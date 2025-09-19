// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/GameDataSubsystem.h"
#include "TextureDataAssetCache.generated.h"

class UTexture2D;

//纹理数据资产
USTRUCT(BlueprintType)
struct FVM_API FTextureDataAsset : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	//纹理资源路径
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath Texture;

	//纹理对象
	UPROPERTY()
	class UTexture2D* TextureInstance = nullptr;
};

/**
 * 纹理数据资产缓存
 */
UCLASS()
class FVM_API UTextureDataAssetCache : public UGameDataAssetCache
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure)
	static UTextureDataAssetCache* GetTextureDataAssetCache();
	//获取物品等级纹理 1-16级
	UFUNCTION(BlueprintPure)
	UTexture2D* GetItemLevelTexture(int32 Level);
	//获取物品等级纹理 1-60级
	UFUNCTION(BlueprintPure)
	UTexture2D* GetPlayerLevelTexture(int32 Level);
	//获取货币纹理 0-xxx
	UFUNCTION(BlueprintPure)
	UTexture2D* GetCoinTexture(int32 Level);
	//获取物品头像
	UFUNCTION(BlueprintPure)
	UTexture2D* GetItemTexture(int32 ItemID);
public:
	//新增物品纹理缓存
	void AddItemTextureCache(int32 ItemID, const FSoftObjectPath& Texture2DPath);
private:
	//物品等级纹理资源   1-16级
	UPROPERTY()
	FDataTableManager ItemLevelSmallLogSource;
	//玩家等级纹理资源   1-60级
	UPROPERTY()
	FDataTableManager PlayerLevelTextureSource;
	//货币等级纹理资源   0-xxxx级
	UPROPERTY()
	FDataTableManager CoinLevelLogSource;
	//物品ID，图像管理
	UPROPERTY()
	TMap<int32, UTexture2D*> ItemsTexture2D;
private:
	//获取物品通过等级
	UTexture2D* GetItemForLevel(FDataTableManager& Manager, int32 Level, int32 MinLevel, int32 MaxLevel);
};
