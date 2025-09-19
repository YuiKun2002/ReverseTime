// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/PlayerDataSubsystem.h"
#include "PlayerDataHandle.generated.h"

/*	
	角色数据配置页面
	1.与 ULocalPlayerDataHandle::CreatePlayerDataPageHandle 密切相关
	2.修改此枚举希望将 ULocalPlayerDataHandle::CreatePlayerDataPageHandle 一并修改！
*/
UENUM(BlueprintType)
enum class EPlayerDataConfigPage : uint8 {
	//技能书存储
	SkillBook UMETA(DisplayName = "SkillBook"),
	//装备存储
	Equip UMETA(DisplayName = "Equip"),
	//最大值
	Max = Equip + 1u
};

/*
*角色数据句柄
本地角色数据对象句柄
*/
UCLASS(BlueprintType)
class FVM_API ULocalPlayerDataHandle : public ULocalDataHandle
{
	GENERATED_BODY()
public:
	//构造本地角色数据对象
	static ULocalPlayerDataHandle* MakeLocalPlayerDataHandle(
		FString PlayerName,
		int32 PlayerGender
	);
	UFUNCTION(BlueprintCallable)
	ULocalDataHandle* LoadPlayerDataPageHandle(EPlayerDataConfigPage Config);
	//删除
	UFUNCTION(BlueprintCallable)
	bool RemovePlayerDataPageHandle(EPlayerDataConfigPage Config);
public:
	//加载本地存档
	virtual bool LoadData_Implementation(const FString& LoadLogMsg) override;
	//创建新的角色数据
	virtual bool CreateData_Implementation() override;
	//移除数据
	virtual bool RemoveData_Implementation() override;
	//获取存储Tag
	virtual const FString GetTag_Implementation() const override { return TEXT("Player"); };
private:
	/*
		加载 角色数据配置页面 数据句柄
		1.如果修改了 EPlayerDataConfigPage ，那么需要继续修改 LoadPlayerDataPageHandle RemovePlayerDataPageHandle函数
	*/
	ULocalDataHandle* CreatePlayerDataPageHandle(EPlayerDataConfigPage Config);
private:
	UPROPERTY()
	int32 LocalPlayerGender = 0;
};