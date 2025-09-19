// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Classes/Engine/DataTable.h"
#include "GameSystem/Item/ItemStruct.h"
#include "CardDataStruct.generated.h"


//卡片关键字映射默认动画
USTRUCT(BlueprintType)
struct FCard_KeyIdleAnim_Data : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();
public:
	//卡片动画映射存储
	UPROPERTY(EditDefaultsOnly)
	FCardKeyIdleAnimation M_CardKeyIdleAnim;
};

//卡片关键字映射默认动画
USTRUCT(BlueprintType)
struct FCard_FCardpreViewHead_Data : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();
public:
	//卡片预览图存储
	UPROPERTY(EditDefaultsOnly)
	FCardpreViewHead M_CardpreViewHead;
};

//卡片等级动画
USTRUCT(BlueprintType)
struct FCard_GradeAnim_Data : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();
public:
	//卡片动画映射存储
	UPROPERTY(EditDefaultsOnly)
	FSoftObjectPath CardGradeResource;
	//基础偏移
	UPROPERTY(EditDefaultsOnly)
	FVector2D RelativeLocation;
	//缩放
	UPROPERTY(EditDefaultsOnly)
	FVector2D Scale;
};

//攻击类型卡片结构
USTRUCT(BlueprintType)
struct FCard_ATK_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//攻击类型的卡片
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemCardATK M_FItemCard;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FItemCard;
		return true;
	}
};

//范围攻击类型卡片结构
USTRUCT(BlueprintType)
struct FCard_RangeATK_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//范围攻击类型的卡片
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemCardRangeATK M_FItemCard;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FItemCard;
		return true;
	}
};

//生产类型卡片结构
USTRUCT(BlueprintType)
struct FCard_Spawn_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//生产类型的卡片
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemCardSpawn M_FItemCard;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FItemCard;
		return true;
	}
};

//防御类型卡片结构
USTRUCT(BlueprintType)
struct FCard_Defence_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//防御类型的卡片
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemCardDefence M_FItemCard;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FItemCard;
		return true;
	}
};

//辅助类型卡片结构
USTRUCT(BlueprintType)
struct FCard_Aid_Data : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();
public:
	//辅助类型的卡片
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemCardAid M_FItemCard;
};

//功能类型卡片结构
USTRUCT(BlueprintType)
struct FCard_Function_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//功能类型的卡片
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemCardFunction M_FItemCard;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FItemCard;
		return true;
	}
};

/*
	卡片选择列表配置页
1.当游戏加载时，会自动加载第一个配置页面
2.每个数据包含ID和等级
3.ID会查询背包中是否存在此卡片，如果不存在则不会被加载
4.不会被加载的数据并不会被移除，则是被覆盖
*/
USTRUCT(BlueprintType)
struct FPlayerSelectListConfig {

	GENERATED_USTRUCT_BODY();

	//卡片列表配置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemViewBlock> CardsConfig;
};

//角色背包卡片更新视图【适用于在UI列表中需要显示的数据】
UCLASS()
class FVM_API UPlayerBagCardView : public UItemBaseView
{
	GENERATED_BODY()
public:

	//获取源数据
	virtual const TMap<int32, FBaseItemSave>& GetItemTable() const override;
	//获取类型的Tag
	virtual FName GetTag() override;

	virtual void Arrangement(
		const TArray<uint8>& Type, //筛选类型
		TMap<int32, FBaseItemSave>& NewItems,//筛选结果
		bool bAddOtherData = true//添加不符合条件的数据到末尾
	) override;

	//获取卡片数据[会进行查询]
	virtual FItemBaseViewBlock* GetItem(int32 ID, int32 Level = -1) override;

	//检测当前卡片是否可以自动加价
	FORCEINLINE	bool CheckCardAutoPrice(int32 ID) { return this->AutoPrice.Contains(ID); }
	//检测当前卡片是否是高级卡片
	FORCEINLINE	bool CheckAdvancedCard(int32 ID) { return this->AdvancedCard.Contains(ID); }
	//获取卡片视图
	UFUNCTION(BlueprintPure)
	FORCEINLINE class UItemBaseView* GetPlayerBagCardView() { return GameDataStaticObject<UPlayerBagCardView>(); }

private:
	UPlayerBagCardView() {};
private:
	//卡片价格
	UPROPERTY()
	TMap<int32, FItemCountViewBlock> CardBlock;
	//自动加价
	UPROPERTY()
	TSet<int32> AutoPrice;
	//高级卡片
	UPROPERTY()
	TSet<int32> AdvancedCard;
};


//卡片数据资产缓存
UCLASS()
class FVM_API UCardDataAssetCache : public UGameDataAssetCache, public IFindItemInterface
{
	GENERATED_BODY()
public:
	//当前接口的名称
	virtual	const FName GetClassTag() override;
	//通过id获取数据
	virtual bool GetItemByID(int32 ItemID, uint8& OutType, void*& OutData) override;
	//获取所有的ID和名称
	virtual void GetAllItemID(TMap<int32, FString>& OutData) override;

	//获取卡片数据
	static bool GetCardData(int32 ID, ECardType& OutType, FItemCard*& OutDataStruct);
	//通过ID获取卡片数据
	UFUNCTION(BlueprintPure, Category = "CardDataAssetCache")
	static bool GetCardData(int32 ID, FItemCard& OutDataStruct);
	//获取全部的卡片名称和ID
	UFUNCTION(BlueprintPure, Category = "CardDataAssetCache")
	static void GetAllCardData(TMap<int32, FString>& OutData);
private:
	DataTableAssetData<FCard_ATK_Data> CardATK;
	//攻击卡片数据表
	UPROPERTY()
	TMap<int32, FCard_ATK_Data> CardATKDataTable;

	DataTableAssetData<FCard_Spawn_Data> CardSpawn;
	//生产卡片数据表
	UPROPERTY()
	TMap<int32, FCard_Spawn_Data> CardSpawnDataTable;

	DataTableAssetData<FCard_Defence_Data> CardDefence;
	//防御卡片数据表
	UPROPERTY()
	TMap<int32, FCard_Defence_Data> CardDefenceDataTable;

	DataTableAssetData<FCard_Function_Data> CardFunction;
	//功能卡片数据表
	UPROPERTY()
	TMap<int32, FCard_Function_Data>CardFunctionDataTable;

	DataTableAssetData<FCard_RangeATK_Data> CardRangeATK;
	//范围攻击卡片数据表
	UPROPERTY()
	TMap<int32, FCard_RangeATK_Data> CardRangeATKDataTable;
};
