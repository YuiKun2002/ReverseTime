// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/GameLogSubsystem.h"
#include "GiftInstructions.generated.h"

/**
 * 礼盒模式指令，当礼包界面被打开时
 */
UCLASS()
class FVM_API UGiftInstructions : public ULogInstructionSystemBase
{
	GENERATED_BODY()
public:
	virtual bool ReadInstruction(const TArray<FString>& Instruction) override;
private:
	//UI指令
	UFUNCTION()
	bool UI(const TArray<FString>& Instruction);

	//set 指令
	bool UI_Set(const TArray<FString>& Instruction);
	
	//edit 指令
	bool UI_Edit(const TArray<FString>& Instruction);

	//编辑cmd
	bool UI_Edit_cmd(class UEquipmentFuncGift* GiftObj,const TArray<FString>& Instruction);
	//编辑列表数据
	bool UI_Edit_ListData(class UEquipmentFuncGift* GiftObj,const TArray<FString>& Instruction);
	//编辑随机列表数据
	bool UI_Edit_RandomListData(class UEquipmentFuncGift* GiftObj,const TArray<FString>& Instruction);
	bool UI_Edit_RandomListData_setrandomcount(class UEquipmentFuncGift* GiftObj,const TArray<FString>& Instruction);

	//移除数据
	bool UI_Rem(const TArray<FString>& Instruction);
	//移除编辑列表
	bool UI_Rem_ListData(class UEquipmentFuncGift* GiftObj,const TArray<FString>& Instruction);
	//移除随机列表
	bool UI_Rem_RandomListData(class UEquipmentFuncGift* GiftObj,const TArray<FString>& Instruction);
};
