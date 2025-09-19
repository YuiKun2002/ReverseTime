// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/Task/TaskFunc/CardSkillTask.h"
#include "GameSystem/TaskSubsystem.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/UI_PlayerBagMaterialGrid.h"
#include "GameSystem/Tools/GameSystemFunction.h"

bool UCardSkillTask::ExeTask(const FString& TaskName, const FTaskIndex& TaskIndex, UObject* TargetObject)
{
	if (UUI_PlayerBagMaterialGrid* Grid = Cast<UUI_PlayerBagMaterialGrid>(TargetObject))
	{
		//查询数据表
		FCardSkillTaskDataTable Temp;
		if (GET_CACHE(Task)->GetImp_CardSkill(TaskIndex, Temp))
		{
			if (Temp.CardSkillName.Equals(TEXT("")))
			{
				return true;
			}
			else {
				return UPlayerRecord::GetDayCardSkillBook(Temp.CardSkillName);
			}
		}
		return false;
	}
	return false;
}

UTaskFunction* UCardSkillTask::MakeNewClass()
{
	return NewObject<UCardSkillTask>();
}
