// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/Task/TaskFunc/LookInforTask.h"
#include "GameSystem/TaskSubsystem.h"
#include "GameSystem/FVMGameInstance.h"
#include "Game/UI/UI_GameInformation.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/Tools/GameSystemFunction.h"

bool ULookInforTask::ExeTask(const FString& TaskName, const FTaskIndex& TaskIndex, UObject* TargetObject)
{
	if (UUI_GameInformationBox* CurUI = Cast<UUI_GameInformationBox>(TargetObject))
	{
		//拿到任务数据
		FLookInforTaskDataTable Cur;
		if (GET_CACHE(Task)->GetImp_LookInfor(TaskIndex, Cur))
		{
			for (const auto& CurName : Cur.CurLook)
			{
				//查询历史记录
				if (!UPlayerRecord::GetDaySeeInfor(CurName))
				{
					return false;
				}
			}

			return true;
		}
	}

	return false;
}

UTaskFunction* ULookInforTask::MakeNewClass()
{
	return NewObject<ULookInforTask>();
}
