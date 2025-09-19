// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/Task/TaskFunc/GemTask.h"
#include "GameSystem/Item/PlayerRecord.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/Tools/GameSystemFunction.h"

//武器开槽镶嵌
#include "Game/UI/Synthesis/SynModel_WeaponGems.h"
//宝石强化
#include "Game/UI/Synthesis/SynModel_GemsUpGrade.h"
//宝石分解
#include "Game/UI/Synthesis/SynModel_GemSplit.h"

#include "GameSystem/TaskSubsystem.h"


bool UGemTask::ExeTask(const FString& TaskName, const FTaskIndex& TaskIndex, UObject* TargetObject)
{

	//武器开凿镶嵌
	UUI_SynModel_WeaponGem* CurWeaponGems = Cast<UUI_SynModel_WeaponGem>(TargetObject);
	if (IsValid(CurWeaponGems))
	{
		FGemTaskDataTable CurData;
		if (GET_CACHE(Task)->GetImp_Gem(TaskIndex, CurData))
		{
			if (CurData.GemTaskType == EGemTaskType::GemAttach)
			{
				//任意宝石
				if (CurData.GemName.Equals(TEXT("")))
				{
					return true;
				}
				else {
					if (CurData.GemName.Equals(UPlayerRecord::GetDayDayNewGemAttachName()))
					{
						return true;
					}
					else {
						return false;
					}
				}
			}
		}
		else {
			return false;
		}
	}

	//宝石强化
	USynModel_GemsUpGrade* GemsUpGrade = Cast<USynModel_GemsUpGrade>(TargetObject);
	if (IsValid(GemsUpGrade))
	{
		FGemTaskDataTable CurData;
		if (GET_CACHE(Task)->GetImp_Gem(TaskIndex, CurData))
		{
			if (CurData.GemTaskType == EGemTaskType::GemUpGrade)
			{
				//任意宝石
				if (CurData.GemName.Equals(TEXT("")))
				{
					if (CurData.GemUpGrade == 0)
					{
						return true;
					}
					else {
						//return ((GemsUpGrade->GetCurrentGemGrade() >= CurData.GemUpGrade) ? true : false);

						return false;
					}
				}
				else {
					if (CurData.GemName.Equals(UPlayerRecord::GetDayDayNewGemAttachName()))
					{
						//return ((GemsUpGrade->GetCurrentGemGrade() >= CurData.GemUpGrade) ? true : false);
						return false;
					}
					else {
						return false;
					}
				}
			}
		}
	}

	//宝石分解
	USynModel_GemSplit* GemSplit = Cast<USynModel_GemSplit>(TargetObject);
	if (IsValid(GemSplit))
	{
		FGemTaskDataTable CurData;
		if (GET_CACHE(Task)->GetImp_Gem(TaskIndex, CurData))
		{
			if (CurData.GemTaskType == EGemTaskType::GemSplit)
			{
				if (CurData.GemName.Equals(TEXT("")))
				{
					return true;
				}
				else {
					//return CurData.GemName.Equals(UPlayerRecord::GetDayDayNewGemAttachName());
					return true;
				}
			}
		}
	}

	return false;
}

UTaskFunction* UGemTask::MakeNewClass()
{
	return NewObject<UGemTask>();
}
