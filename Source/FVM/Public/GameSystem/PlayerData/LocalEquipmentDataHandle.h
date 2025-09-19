// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/PlayerDataSubsystem.h"
#include "GameSystem/Item/Equipment/PlayerWeaponStruct.h"
#include "LocalEquipmentDataHandle.generated.h"


/**
 * 本地角色装备数据
 */
UCLASS()
class FVM_API ULocalEquipmentDataSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	/*删除存储的装备数据*/
	bool RemoveEquipType(int32 BlockIndex, uint8 Type);

	//获取玩家武器装备数据【武器装备情况】
	UFUNCTION()
	TMap<int32, FPlayerWeaponEquipData>& GetbEquipWeaponList();
public:
	/*-----------------------装备-详细数据存储------------------------*/
	//玩家武器装备数据【武器装备情况】int32 物品的ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FPlayerWeaponEquipData> bEuipWeaponItems;
	/*-----------------------装备-详细数据存储------------------------*/
};


/**
 * 本地装备数据句柄
 */
UCLASS()
class FVM_API ULocalEquipmentDataHandle : public ULocalDataHandle
{
	GENERATED_BODY()

public:
	//创建新的数据
	virtual bool CreateData_Implementation() override;
	//获取存储Tag
	virtual const FString GetTag_Implementation() const override { return TEXT("Equip"); };
};
