// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/AssetManager.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StreamableManager.h"
#include "Engine/Classes/Engine/DataTable.h"
#include "ItemIdDataTable.generated.h"


/*ID总表
1.用于编辑器，ID总表
2.描述所有物品的ID号
*/
USTRUCT(BlueprintType)
struct FItemIdDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();
public:
	//物品名称关联ID号
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, int32> Item;
};

/*ID号*/
USTRUCT(BlueprintType)
struct FItemId : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();
public:
	//物品名称关联ID号
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxID = -1;

	//已经被删除的ID号[重新测试，重新进入数据被恢复]
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> DeleteID;
};

//物品ID管理器
UCLASS()
class FVM_API UItemIdEditorManager : public UObject
{
	GENERATED_BODY()
public:
	//获取物品ID编辑管理器
	UFUNCTION(BlueprintPure)
	static UItemIdEditorManager* GetItemIdEditorManager();
public:

	//获取关联数据表
	UFUNCTION(BlueprintPure)
	class UDataTable* GetItemIdDataTable();

	//获取ID数据表
	UFUNCTION(BlueprintPure)
	class UDataTable* GetIdDataTable();

	//获取新的ID
	UFUNCTION(BlueprintPure)
	int32 GetNewID();

	//获取ID库名称
	UFUNCTION(BlueprintPure)
	void GetRowNames(TArray<FString>& OutNames);

	//移除一个ID
	UFUNCTION(BlueprintCallable)
	void RemoveID(FString ItemName);

	//添加一个ID
	UFUNCTION(BlueprintCallable)
	void AddID(FString ItemName);

protected:
	//新增移除ID
	UFUNCTION()
	void AddDeleteID(int32 ID);

	//获取一个移除ID
	UFUNCTION()
	int32 GetDeleteID();

	//移除一个删除ID
	UFUNCTION()
	void RemoveDeleteID(int32 ID);
private:
	void UpdateMaxID();

	/*
		UFUNCTION(BlueprintCallable, CustomThunk, meta = (CustomStructureParam = "RowData"))
		static bool AddDataTableRow(UDataTable* Table, FName RowName, const FTableRowBase& RowData);
		static bool Generic_AddDataTableRow(UDataTable* Table, FName RowName, const uint8* RowData);
		DECLARE_FUNCTION(execAddDataTableRow);
	private:
		static bool AddNewRow_Source(UDataTable* Table, FName RowName, const uint8* NewRowData);
		*/
private:
	//id编辑器管理器
	static UItemIdEditorManager* IdEditorOperate;

	//关联表格
	UPROPERTY()
	class UDataTable* ItemIdDataTablePtr = nullptr;
	//ID表格
	UPROPERTY()
	class UDataTable* ItemIdPtr = nullptr;
};
