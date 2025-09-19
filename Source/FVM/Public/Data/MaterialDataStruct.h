// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Classes/Engine/DataTable.h"
#include "GameSystem/Item/ItemStruct.h"
#include "MaterialDataStruct.generated.h"

 //材料-防御卡合成配方数据
USTRUCT(BlueprintType)
struct FMaterial_CardSynthesisBlueprint_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//配方结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCardBlueprint M_FMaterial;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FMaterial;
		return true;
	}
};

//材料-防御卡合成配方材料数据
USTRUCT(BlueprintType)
struct FMaterial_CardSynthesisMaterial_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//配方材料结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCardBlueprintMaterial M_FMaterial;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FMaterial;
		return true;
	}
};

//材料-兑换券材料数据
USTRUCT(BlueprintType)
struct FMaterial_TicketMaterial_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//兑换券结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTicket M_FMaterial;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FMaterial;
		return true;
	}
};

//强化水晶
USTRUCT(BlueprintType)
struct FMaterial_Crystal_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//物品结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCrystalMaterial M_FMaterial;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FMaterial;
		return true;
	}
};

//武器开槽-钻头
USTRUCT(BlueprintType)
struct FMaterial_Bit_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//物品结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBitMaterial M_FMaterial;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FMaterial;
		return true;
	}
};

//关卡钥匙
USTRUCT(BlueprintType)
struct FMaterial_LevelKey_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//物品结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLevelKey M_FMaterial;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FMaterial;
		return true;
	}
};



//---------------------------------------------------------------------------------
//材料-防御卡转职材料数据
USTRUCT(BlueprintType)
struct FMaterial_CardChangeJobMaterial_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//转职材料结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCardChangeJobMaterial M_FMaterial;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FMaterial;
		return true;
	}

	virtual void SetID(int32 ID) override {
		Super::SetID(ID);
		this->M_FMaterial.ItemID = ID;
	}
};
//---------------------------------------------------------------------------------

//材料-香料数据
USTRUCT(BlueprintType)
struct FMaterial_SpicesMaterial_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//转职材料结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSpicesMaterial M_FMaterial;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FMaterial;
		return true;
	}
};

//材料-四叶草数据
USTRUCT(BlueprintType)
struct FMaterial_CloverMaterial_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//四叶草结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCloverMaterial M_FMaterial;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FMaterial;
		return true;
	}
};

//---------------------------------------------------------------------------------
//角色-技能书
USTRUCT(BlueprintType)
struct FCardSkillBooks_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//物品结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCardSkillBookMaterial M_FMaterial;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FMaterial;
		return true;
	}

	virtual void SetID(int32 ID) override {
		Super::SetID(ID);
		this->M_FMaterial.ItemID = ID;
	}
};

//---------------------------------------------------------------------------------

//角色背包材料更新视图【适用于在UI列表中需要显示的数据】
UCLASS()
class FVM_API UPlayerBagMaterialView : public UItemBaseView
{
	GENERATED_BODY()

public:

	//获取源数据
	virtual const TMap<int32, FBaseItemSave>& GetItemTable() const override;
	//获取类型的Tag
	virtual FName GetTag() override;

	virtual FItemViewIndexRange GetItemIndexRange(int32 ID, int32 Level,
		const TMap<int32, FBaseItemSave>& NewLoadItems, int32 JumpPosition = -1);


	/*
 整理物品
 1.类型的顺序
 2.返回的存储结果
 3.是否返回除筛选类型以外的数据，例如：筛选 1 3 4 以内。但是出现2，末尾也包含2的数据。
*/
	virtual void Arrangement(
		const TArray<uint8>& Type, //筛选类型
		TMap<int32, FBaseItemSave>& NewItems,//筛选结果
		bool bAddOtherData = true//添加不符合条件的数据到末尾
	) override;

	virtual	TArray<FItemViewBlock> GenerateItemView(int32 GenerateMax) override;

	//获取材料数据[会进行查询]
	virtual FItemBaseViewBlock* GetItem(int32 ID, int32 Level) override;

	//获取材料视图
	UFUNCTION(BlueprintPure)
	FORCEINLINE class UItemBaseView* GetPlayerBagMaterialView() { return GameDataStaticObject<UPlayerBagMaterialView>(); }

public:
	virtual	void ClearCache() override;
private:
	UPlayerBagMaterialView() {};
private:
	//装备数据库
	UPROPERTY()
	TMap<int32, FItemCountViewBlock> MaterialBlock;
};


//材料数据资产缓存
UCLASS()
class FVM_API UMaterialDataAssetCache : public UGameDataAssetCache, public IFindItemInterface
{
	GENERATED_BODY()
public:
	//当前接口的名称
	virtual	const FName GetClassTag() override;
	//通过id获取数据
	virtual bool GetItemByID(int32 ItemID, uint8& OutType, void*& OutData) override;
	//获取全部的材料名称和ID
	virtual void GetAllItemID(TMap<int32, FString>& OutData) override;
	//获取材料数据
	static bool GetMaterData(int32 ID, EMaterialType& OutType, FMaterialBase*& OutDataStruct);
	//通过ID获取材料数据
	UFUNCTION(BlueprintPure, Category = "MaterialDataAssetCache")
	static bool GetMaterData(int32 ID, FMaterialBase& OutDataStruct);
	//获取全部的材料名称和ID
	UFUNCTION(BlueprintPure, Category = "MaterialDataAssetCache")
	static void GetAllMaterData(TMap<int32, FString>& OutData);

private:
	//合成配方数据表
	DataTableAssetData<FMaterial_CardSynthesisBlueprint_Data> Blueprint;
	UPROPERTY()
	TMap<int32, FMaterial_CardSynthesisBlueprint_Data> BlueprintDataTable;

	//合成材料配方数据表
	DataTableAssetData<FMaterial_CardSynthesisMaterial_Data> BlueprintMater;
	UPROPERTY()
	TMap<int32, FMaterial_CardSynthesisMaterial_Data> BlueprintMaterDataTable;

	//转职数据表
	DataTableAssetData<FMaterial_CardChangeJobMaterial_Data> Change;
	UPROPERTY()
	TMap<int32, FMaterial_CardChangeJobMaterial_Data> ChangeDataTable;

	//香料数据表
	DataTableAssetData<FMaterial_SpicesMaterial_Data> Spices;
	UPROPERTY()
	TMap<int32, FMaterial_SpicesMaterial_Data> SpicesDataTable;

	//四叶草数据表
	DataTableAssetData<FMaterial_CloverMaterial_Data> Clover;
	UPROPERTY()
	TMap<int32, FMaterial_CloverMaterial_Data> CloverDataTable;

	//技能书数据表
	DataTableAssetData<FCardSkillBooks_Data> SkillBook;
	UPROPERTY()
	TMap<int32, FCardSkillBooks_Data> SkillBookDataTable;

	//兑换券数据表
	DataTableAssetData<FMaterial_TicketMaterial_Data> Ticket;
	UPROPERTY()
	TMap<int32, FMaterial_TicketMaterial_Data> TicketDataTable;

	//强化水晶数据表
	DataTableAssetData<FMaterial_Crystal_Data> Crystal;
	UPROPERTY()
	TMap<int32, FMaterial_Crystal_Data> CrystalDataTable;

	//钻头数据表
	DataTableAssetData<FMaterial_Bit_Data> Bit;
	UPROPERTY()
	TMap<int32, FMaterial_Bit_Data> BitDataTable;

	//关卡钥匙数据表
	DataTableAssetData<FMaterial_LevelKey_Data> LevelKey;
	UPROPERTY()
	TMap<int32, FMaterial_LevelKey_Data> LevelKeyDataTable;
};