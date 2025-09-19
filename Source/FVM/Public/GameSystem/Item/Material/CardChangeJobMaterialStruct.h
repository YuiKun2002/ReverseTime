// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Item/Material/MaterialBaseStruct.h"
#include "CardChangeJobMaterialStruct.generated.h"

//材料->卡片转职材料
USTRUCT(BlueprintType)
struct FCardChangeJobMaterial : public FMaterialBase {
	GENERATED_USTRUCT_BODY()
};

//金卡进化对应的材料
USTRUCT(BlueprintType)
struct FMaterial_GoldCardUpNeedMaterData
{

	GENERATED_USTRUCT_BODY();

	//消耗的进化材料
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NeedItemID = -1;

	//合成的物品ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TargetItemID = -1;
};

//材料-防御卡金卡进化数据片【用于材料逻辑匹配并非物品】
USTRUCT(BlueprintType)
struct FMaterial_GoldCardUp_Data : public FTableRowBase
{

	GENERATED_USTRUCT_BODY();

	// Row Name Key ID
	// 数据表行Name表示 主卡ID 
	
	//副卡ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FMaterial_GoldCardUpNeedMaterData> ItemID;
};