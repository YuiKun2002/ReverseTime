// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除
// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Item/ItemBaseGridFunc.h"
#include "GameSystem/Item/Equipment/GiftBoxStruct.h"
#include "EquipmentFuncGift.generated.h"

class UUI_GiftItemTip_Item;

/**
 * 礼盒功能
 */
UCLASS()
class FVM_API UEquipmentFuncGift : public UItemBaseGridFunc
{
	GENERATED_BODY()

public:

	virtual void Execute(
		class UItemGridBase* EquipGridPtr,
		struct FItemBaseStructData* ItemBasePtr
	) override;

	//获取指令
	FORCEINLINE	FString& GetCmd() { return this->Cmd; }

	//获取列表数据
	FORCEINLINE TArray<FItemViewBlockGift>& GetListData() { return this->ListData; }

	//获取随机列表数据
	FORCEINLINE FGiftBox_RandomItemMode& GetRandomData() { return this->RandomData; }

protected:

	//使用
	UFUNCTION()
	void Use();

	//显示内容
	void ShowListDataContent();

	//显示随机抽奖内容
	void ShowRandomDataContent();

	//显示指令内容
	void ShowCmdContent();

	//使用指令模式
	void UseInstruction(FString CurCmd, int32 Count);

	//发送道具
	void SendItem(int32 UseCount);

	//生成概率
	void GenRandom(
		TArray<int32>& OutRandomValuesRange,
		TMap<int32, int32>& RandomValuesRangeToKey,
		TMap<int32, int32>& KeyToRandomValuesRange,
		int32& AddedValueSum
	);

private:

	//提示UI
	UPROPERTY()
	class UUI_GiftItemTip* TipUI = nullptr;

	//指令
	UPROPERTY()
	FString Cmd;

	//运行指令
	UPROPERTY()
	FString ExecCmd;

	//道具列表【可直接发送给玩家的数据】
	UPROPERTY()
	TArray<FItemViewBlockGift> ListData;

	//随机道具列表
	UPROPERTY()
	FGiftBox_RandomItemMode RandomData;

};
