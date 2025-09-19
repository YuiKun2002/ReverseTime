// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Item/Equipment/EquipmentBaseStruct.h"
#include "GameSystem/Item/Equipment/GemsStruct.h"
#include "PlayerWeaponStruct.generated.h"


//---------------------------------------------------------------------------------------------------------
// 
//    武器结构开始
//---------------------------------------------------------------------------------------------------------

//道具->武器宝石->槽
USTRUCT(BlueprintType)
struct FPlayerWeaponGemSlot {
	GENERATED_USTRUCT_BODY()
public:

	//宝石不需要ID存储到武器->使用了宝石，宝石道具会消失->如果卸载宝石，宝石将返回背包

	//当前槽位是否解锁
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_CurrentWeaponGemEnable = false;
	//宝石的名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString M_WeaponGemName = TEXT("");
	//宝石的等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_WeaponGemGrade = 0;
	//宝石纹理[不可手动修改，从数据表修改]
	UPROPERTY()
	FString M_WeaponGemHeadPath = TEXT("");
};


//玩家武器-装备数据-宝石数据
USTRUCT(BlueprintType)
struct FPlayerWeaponEquipGemDataBlock {
	GENERATED_USTRUCT_BODY()

	//宝石ID号
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GemID = -1;

	//宝石等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GemLevel = -1;
};

//玩家武器-装备数据宝石槽位
USTRUCT(BlueprintType)
struct FPlayerWeaponEquipGemSlotDataBlock {
	GENERATED_USTRUCT_BODY()

	/*
		镶嵌的宝石数据槽位
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPlayerWeaponEquipGemDataBlock> Slot;

};
USTRUCT(BlueprintType)
struct FPlayerWeaponEquipData {

	GENERATED_USTRUCT_BODY()

	//数据存储，int32 BlockIndex
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FPlayerWeaponEquipGemSlotDataBlock> Datas;
};

//武器分类
UENUM(BlueprintType)
enum class EPlayerWeaponType : uint8
{
	//主武器
	E_First UMETA(DisplayName = "First"),
	//副武器
	E_Second UMETA(DisplayName = "Second"),
	//超级武器
	E_Super UMETA(DisplayName = "Super"),
};

//道具->玩家武器
USTRUCT(BlueprintType)
struct FPlayerWeaponBase : public FEquipmentBase {

	GENERATED_USTRUCT_BODY()

	//主武器
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayerWeaponType M_EPlayerWeaponType = EPlayerWeaponType::E_First;
	//武器Def动画路径
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath M_WeaponDef_Path;
	//武器Attack动画路径
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath M_WeaponAttack_Path;
	//对象路径
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftClassPath M_WeaponClassPath;
	//宝石槽位限制(最多能开启4个槽位)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_WeaponGemCountMax = 4;
};

//道具->玩家武器->主武器
USTRUCT(BlueprintType)
struct FPlayerWeaponFirst : public FPlayerWeaponBase
{
	GENERATED_USTRUCT_BODY()

	//卡片基础攻击力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_ATKBase = 0.f;
	//卡片攻击力加成比率
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_ATKRate = 0.f;
	//是否有溅射伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_SputteringHurt = false;
	//溅射伤害比例
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_SputteringATKRate = 0.f;
	//攻击速度(攻击延迟)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_ShootSpeed = 2.f;
	//首次投射时间 {延迟}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_FirstShootSpeed = 0.5f;
	//投射延迟
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_ShootDelay = 0.1f;
	//投射多少发
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_ProjectionItemCount = 1;
	//射线设置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLineTraceSetting> M_LineTraceSettings;
};

//道具->玩家武器->副武器
USTRUCT(BlueprintType)
struct FPlayerWeaponSecond : public FPlayerWeaponBase
{
	GENERATED_USTRUCT_BODY()

	FPlayerWeaponSecond() {
		this->M_EPlayerWeaponType = EPlayerWeaponType::E_Second;
	}

	//直接给玩家进行的体力加成
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_HP_Value = 10.f;
};

//道具->玩家武器->超级武器
USTRUCT(BlueprintType)
struct FPlayerWeaponSuper : public FPlayerWeaponBase
{
	GENERATED_USTRUCT_BODY()

	FPlayerWeaponSuper() {
		this->M_EPlayerWeaponType = EPlayerWeaponType::E_Super;
	}

	//当前武器的基础伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_ATK_Value = 30.f;
};

//---------------------------------------------------------------------------------------------------------
// 
//    武器结构开结束
//---------------------------------------------------------------------------------------------------------

//装备-玩家主武器数据
USTRUCT(BlueprintType)
struct FEquipment_WeaponFirst_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//物品结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerWeaponFirst M_FEquipment;

	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FEquipment;
		return true;
	}
};

//装备-玩家副武器数据
USTRUCT(BlueprintType)
struct FEquipment_WeaponSecond_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//物品结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerWeaponSecond M_FEquipment;

	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FEquipment;
		return true;
	}
};

//装备-玩家超级武器数据
USTRUCT(BlueprintType)
struct FEquipment_WeaponSuper_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//物品结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerWeaponSuper M_FEquipment;

	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FEquipment;
		return true;
	}
};
