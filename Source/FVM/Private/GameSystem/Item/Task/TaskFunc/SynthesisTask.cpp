#include "GameSystem/Item/Task/TaskFunc/SynthesisTask.h"
#include "GameSystem/TaskSubsystem.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/Synthesis/SynModel_MakeCard.h"
#include "Game/UI/Synthesis/SynModel_CardUpgrade.h"
#include "GameSystem/Tools/GameSystemFunction.h"

bool USynthesisTask::ExeTask(const FString& TaskName, const FTaskIndex& TaskIndex, UObject* TargetObject)
{
	//卡片制作
	if (USynModel_MakeCard* MakeCard = Cast<USynModel_MakeCard>(TargetObject))
	{
		FSynMakeCardTaskDataTable Cur;
		if (GET_CACHE(Task)->GetImp_SynMakeCard(TaskIndex, Cur))
		{
			//查询历史记录[卡片制作次数]
			int32 MakeCardCount = UPlayerRecord::GetDayCardMakeCount(Cur.MakeCardName);
			if (MakeCardCount < Cur.MakeCount)
			{
				return false;
			}

			//如果是任意卡片
			if (Cur.MakeCardName.Equals(TEXT("")))
			{
				return true;
			}
			else {
				/*if (Cur.MakeCardName.Equals(MakeCard->GetCurrentCardName()))
				{
					return true;
				}*/
				return false;
			}
		}
	}

	if (UUI_SynModel_UpgradeCard* CardUp = Cast<UUI_SynModel_UpgradeCard>(TargetObject))
	{
		FSynCardUpGradeDataTable Cur;
		if (GET_CACHE(Task)->GetImp_SynCardUp(TaskIndex, Cur))
		{
			//任意卡片等级强化
			if (Cur.UpGradeCardName.Equals(TEXT("")))
			{
				if (Cur.CardUpGrade == 0)
				{
					return true;
				}
				else {
					//获取当前对应强化等级的卡片
					const FString& CurCardName = UPlayerRecord::GetDayCardUpCount(Cur.CardUpGrade);
					//如果匹配强化卡片名称则完成任务
					//if (CurCardName.Equals(CardUp->GetCurrentCardName()))
					{
						return true;
					}
				}
				return false;
			}
			else {
				//获取当前对应强化等级的卡片
				const FString& CurCardName = UPlayerRecord::GetDayCardUpCount(Cur.CardUpGrade);
				if (CurCardName.Equals(Cur.UpGradeCardName))
				{
					return true;
				}
				return false;
			}
		}
	}

	return false;
}

UTaskFunction* USynthesisTask::MakeNewClass()
{
	return NewObject<USynthesisTask>();
}