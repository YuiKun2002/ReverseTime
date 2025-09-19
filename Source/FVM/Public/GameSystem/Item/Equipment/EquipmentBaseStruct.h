// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

// #include"GameSystem/Item/Equipment/EquipmentBaseStruct.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Item/ItemBaseStruct.h"
#include "EquipmentBaseStruct.generated.h"

UENUM(BlueprintType)
enum class EEquipment : uint8
{
	//背包
	E_Bag UMETA(DisplayName = "Bag"),
	//礼盒
	E_Gift UMETA(DisplayName = "Gift"),
	//角色服装
	E_PlayerEquipment UMETA(DisplayName = "PlayerEquipment"),
	//角色主武器
	E_PlayerWeaponFirst UMETA(DisplayName = "WeaponFirst"),
	//角色副武器
	E_PlayerWeaponSecond UMETA(DisplayName = "WeaponSecond"),
	//角色超级武器
	E_PlayerWeaponSuper UMETA(DisplayName = "WeaponSuper"),
	//武器宝石
	E_WeaponGem UMETA(DisplayName = "WeaponGem"),


	//最大值->(武器宝石)
	E_Max = E_WeaponGem + 1u
};

//道具->底层
USTRUCT(BlueprintType)
struct FEquipmentBase : public FItemBase {

	GENERATED_USTRUCT_BODY()

	//道具类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipment M_EquipmentType = EEquipment::E_Bag;

};