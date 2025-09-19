// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Item/Material/MaterialBaseStruct.h"
#include "CardSkillBookMaterialStruct.generated.h"


//材料-功能道具-技能书
USTRUCT(BlueprintType)
struct FCardSkillBookMaterial : public FMaterialBase {
	GENERATED_USTRUCT_BODY()
public:
	//ID对应卡片的技能书
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_CardSkillBookMaterial_ID = 0;
	//经验-增加值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_CardSkillBookMaterial_ExAdd = 10;
	//经验书等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_CardSkillBookMaterial_ExLevel = 1;
};

UENUM(BlueprintType)
enum class ESkillBookTipCategory : uint8 {
	//普通卡
	S_Normal UMETA(DisplayName = "Normal"),
	//纪念卡
	S_Style_1 UMETA(DisplayName = "Style_1"),
	//星座卡
	S_Style_2 UMETA(DisplayName = "Style_2"),
	//金卡
	S_Style_3 UMETA(DisplayName = "Style_3"),
	//最大值
	Max = S_Style_3 + 1U
};

//技能书数据结构(用于加载展示的结构)
USTRUCT(BlueprintType)
struct FSkillBookData {

	GENERATED_USTRUCT_BODY()

	//ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ID = 0;
	//技能书名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;
	//提示信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Tip;
	//头像
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSoftObjectPath> HeadPath;
	//技能书等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemLevel Level = FItemLevel(0, 8, 200, 1.5f);
	//技能书分类
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESkillBookTipCategory TabCategory = ESkillBookTipCategory::S_Normal;
};

//存储结构-技能书
USTRUCT(BlueprintType)
struct FSkillBookData_Data : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();
public:
	//物品结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSkillBookData M_FSkillBookData;
};