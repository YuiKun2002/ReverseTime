// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Item/ItemStruct.h"
#include "EquipmentDataSturct.generated.h"


//装备-玩家补偿数据
USTRUCT(BlueprintType)
struct FPlayerVersionCompensate_Data : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();
public:
	//物品结构
	UPROPERTY(EditDefaultsOnly)
	FPlayerVersionCompensate M_FPlayerVersionCompensate;
};

//调试视图->UE 调试，显示代码运行的时间
//DECLARE_STATS_GROUP(TEXT("MyStat"), STATGROUP_MyStat, STATCAT_Test);
//DECLARE_CYCLE_STAT(TEXT("PlayerBagEquipView"), STAT_GenerateItemView, STATGROUP_MyStat);
//DECLARE_CYCLE_STAT(TEXT(&#34;MyActor Tick&#34;), STAT_Tick, STATGROUP_MyStat);

//角色背包装备更新视图【适用于在UI列表中需要显示的数据】
UCLASS()
class FVM_API UPlayerBagEquipView : public UItemBaseView
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

	//生成一组可以用于加载列表的数据
	virtual	TArray<FItemViewBlock> GenerateItemView(int32 GenerateMax) override;
	//生成装备过后的列表
	const TMap<int32, FEquipItemSave>& GenerateEuipItemListTable() const;
	//通过类型生成的数据视图
	void GenerateItemViewSourceByType(
		const EEquipment& EquipType,
		TMap<int32, FBaseItemSave>& OutItemViewSource
	);
	//获取物品装备数据[会进行查询]
	virtual FItemBaseViewBlock* GetItem(int32 ID, int32 Level) override;

	//获取已经装备的物品个数
	FORCEINLINE	int32 GetbEquipItemCount() const { return this->EuipItemCount; }

	//获取装备视图
	UFUNCTION(BlueprintPure)
	FORCEINLINE class UItemBaseView* GetPlayerBagEquipView() { return GameDataStaticObject<UPlayerBagEquipView>(); }


private:
	UPlayerBagEquipView() {};
private:
	//装备数据库
	UPROPERTY()
	TMap<int32, FItemCountViewBlock> EquipBlock;
	//当前装备物品的个数
	UPROPERTY()
	int32 EuipItemCount = 0;
};

//装备数据资产缓存
UCLASS()
class FVM_API UEquipDataAssetCache : public UGameDataAssetCache, public IFindItemInterface
{
	GENERATED_BODY()
public:
	//当前接口的名称
	virtual	const FName GetClassTag() override;
	//通过id获取数据
	virtual bool GetItemByID(int32 ItemID, uint8& OutType, void*& OutData) override;
	//获取全部的装备名称和ID
	virtual void GetAllItemID(TMap<int32, FString>& OutData) override;
	//获取装备数据
	static bool GetEquipData(int32 ID, EEquipment& OutType, FEquipmentBase*& OutDataStruct);
	//通过ID获取装备数据
	UFUNCTION(BlueprintPure, Category = "EquipmentDataAssetCache")
	static bool GetEquipData(int32 ID, FEquipmentBase& OutDataStruct);
	//获取全部的装备名称和ID
	UFUNCTION(BlueprintPure, Category = "EquipmentDataAssetCache")
	static void GetAllEquipData(TMap<int32, FString>& OutData);

private:
	//初始化角色服装
	void InitPlayerEquipment();

public:
	//背包
	DataTableAssetData<FEquipment_Bag_Data> BagA;
	TMap<int32, FEquipment_Bag_Data> BagDataTable;

	//礼包
	DataTableAssetData<FEquipment_GiftBox_Data> GiftBoxA;
	TMap<int32, FEquipment_GiftBox_Data> GiftBoxDataTable;

	//主武器
	DataTableAssetData<FEquipment_WeaponFirst_Data> WeaponFirstA;
	TMap<int32, FEquipment_WeaponFirst_Data> WeaponFirstDataTable;

	//副武器
	DataTableAssetData<FEquipment_WeaponSecond_Data> WeaponSecondA;
	TMap<int32, FEquipment_WeaponSecond_Data> WeaponSecondDataTable;

	//超级武器
	DataTableAssetData<FEquipment_WeaponSuper_Data> WeaponSuperA;
	TMap<int32, FEquipment_WeaponSuper_Data> WeaponSuperDataTable;

	//武器宝石
	DataTableAssetData<FEquipment_WeaponGem_Data> WeaponGemA;
	TMap<int32, FEquipment_WeaponGem_Data>WeaponGemDataTable;

	//服装
	DataTableAssetData<FEquipment_PlayerEquipment_Data> PlayerEquipmentA;
	TMap<int32, FEquipment_PlayerEquipment_Data>PlayerEquipmentDataTable;
};