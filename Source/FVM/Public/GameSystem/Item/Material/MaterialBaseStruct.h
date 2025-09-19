// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Item/ItemBaseStruct.h"
#include "MaterialBaseStruct.generated.h"

UENUM(BlueprintType)
enum class EMaterialType : uint8//[当你添加新材料时，记得将Max改为最新的材料，最后去填写数据表路径]
{
	//卡片合成配方
	E_Blueprint UMETA(DisplayName = "Blueprint"),
	//卡片配方材料
	E_CardSynthesisMaterial UMETA(DisplayName = "CardSynthesisMaterial"),
	//卡片转职材料
	E_CardChangeJobMaterial UMETA(DisplayName = "CardChangeJobMaterial"),
	//香料
	E_Spices UMETA(DisplayName = "Spices"),
	//四叶草
	E_Clover UMETA(DisplayName = "Clover"),
	//防御卡技能书
	E_CardSkillBook UMETA(DisplayName = "CardSkillBook"),
	//兑换券
	E_Ticket UMETA(DisplayName = "Ticket"),
	//强化水晶
	E_Crystal UMETA(DisplayName = "Crystal"),
	//武器开槽钻头
	E_Bit UMETA(DisplayName = "Bit"),
	//关卡钥匙
	E_LevelKey UMETA(DisplayName = "LevelKey"),
	//最大值
	E_Max = E_LevelKey + 1u
};

//道具->材料->底层
USTRUCT(BlueprintType)
struct FMaterialBase : public FItemBase {
	GENERATED_USTRUCT_BODY()
public:
	//数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_Count = 0;
	//材料类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMaterialType M_MaterialType = EMaterialType::E_Blueprint;
};
