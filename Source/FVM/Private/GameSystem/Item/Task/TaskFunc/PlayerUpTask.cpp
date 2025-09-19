// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#include "GameSystem/Item/Task/TaskFunc/PlayerUpTask.h"
#include "GameSystem/TaskSubsystem.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/Tools/GameSystemFunction.h"

bool UPlayerUpTaskFunc::ExeTask(const FString& TaskName, const FTaskIndex& TaskIndex, UObject* TargetObject)
{
	if (UPlayerStructManager* CurPlayer = Cast<UPlayerStructManager>(TargetObject))
	{
		//拿到任务数据
		FPlayerUpTaskDataTable Cur;
		if (GET_CACHE(Task)->GetImp_PlayerUp(TaskIndex, Cur))
		{
			if (CurPlayer->M_FPlayerEx.M_PlayerGrade >= Cur.Level)
			{
				return true;
			}
		}
	}

	return false;
}

UTaskFunction* UPlayerUpTaskFunc::MakeNewClass()
{
	return NewObject<UPlayerUpTaskFunc>();
}
