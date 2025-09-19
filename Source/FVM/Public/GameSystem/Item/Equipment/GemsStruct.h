// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Item/Equipment/EquipmentBaseStruct.h"
#include "GemsStruct.generated.h"

//---------------------------------------------------------------------------------------------------------
// 
//    宝石结构开始
//---------------------------------------------------------------------------------------------------------

//宝石镶嵌的类型
UENUM(BlueprintType)
enum class EGemAttachType : uint8
{
	//主武器
	First UMETA(DisplayName = "First"),
	//副武器
	Second UMETA(DisplayName = "Second"),
	//超级武器
	Super UMETA(DisplayName = "Super"),
	//通用类型
	Universal UMETA(DisplayName = "Universal")
};

//道具->宝石装备
USTRUCT(BlueprintType)
struct FWeaponGem : public FEquipmentBase {
	GENERATED_USTRUCT_BODY()
public:
	//当前宝石镶嵌的武器类型-默认可以镶嵌到主武器
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGemAttachType GemType = EGemAttachType::First;
	//武器镶嵌的专属宝石，-1表示通用
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponItemConstantID = -1;
	//宝石技能
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftClassPtr<UObject>> GemSkills;
	//宝石技能触发时间（冷却）[需要启用计时]
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_GemTriggerTime = 30.f;
	//宝石等级上限
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_GemLevelMax = 10;
};


//装备-玩家宝石数据
USTRUCT(BlueprintType)
struct FEquipment_WeaponGem_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//物品结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponGem M_FEquipment;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FEquipment;
		return true;
	}
	virtual void SetID(int32 ID) override{
		Super::SetID(ID);
		this->M_FEquipment.ItemID = ID;
	}
};