// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/Task/TaskStructBase.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/TaskSubsystem.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "GameSystem/Item/Task/Inter/TaskInterface.h"

void FAllTask::InitDayTask(UPlayerStructManager* PlayerIns)
{
	UPlayerStructManager* Player = PlayerIns;
	if (!IsValid(PlayerIns))
	{
		Player = UFVMGameInstance::GetFVMGameInstance()->GetPlayerStructManager();
		if (!IsValid(Player))
		{
#if WITH_EDITOR
			GAME_LOG_LINE(TEXT("初始化每日任务失败，角色存档无效"));
#endif
			return;
		}
	}

	//当前任务类型
	EGameTaskType CurTaskType = EGameTaskType::DayTask;
	if (Player->M_FPlayerEx.M_PlayerGrade >= 3)
	{
		FDateTime GameSystem_Time_Now = FDateTime::Now();
		FString Year = FString::FromInt(GameSystem_Time_Now.GetYear());
		FString Month = FString::FromInt(GameSystem_Time_Now.GetMonth());
		FString Day = FString::FromInt(GameSystem_Time_Now.GetDay());
		FString Target = Year + TEXT("-") + Month + TEXT("-") + Day;

		if (!Player->DayTaskTime.Equals(Target))
		{
			//设置时间
			Player->DayTaskTime = Target;

			//清理每日累积
			UPlayerRecord::InitDayRecord();

			//初始化任务索引
			const FAllTaskData& AllTask = GET_CACHE(Task)->GetAllTaskData();

			//任务刷新[移除任务]
			Player->LineTaskProgress.RemoveFinishTask(CurTaskType);
			Player->AllTasks.RemoveTaskBlock(CurTaskType);
			//新增每日任务数据库
			Player->AllTasks.InitTaskBlock(CurTaskType, AllTask);

			//直接新增登陆完成任务
			this->DirectFinishDayTask(Player, AllTask);

			//保存
			UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("初始化每日任务")));
		}
		else {

			//检索每日任务
			const FAllTaskData& AllTask = GET_CACHE(Task)->GetAllTaskData();
			//当前需要执行的每日任务
			FAllTaskDataBlock* DayTaskBlock = Player->AllTasks.CurTasks.Find(CurTaskType);
			if (DayTaskBlock)
			{

				this->DirectFinishDayTask(Player, AllTask);

				//获取每日任务的完成情况[完成数量]
				int32 DayTaskFinishCount = Player->LineTaskProgress.GetFinishTaskCountByTaskType(
					CurTaskType
				);

				//获取最新的每日任务数据块[最新数量]
				FAllTaskDataBlock TempBlock = AllTask.GetTaskBlock(CurTaskType);

				//完成数量+未完成数量 匹配 最新数量
				if (DayTaskFinishCount + DayTaskBlock->AllTaskData.Num() == TempBlock.AllTaskData.Num())
				{
					return;
				}

				//搜集完成的任务
				FAllTaskDataBlock CurrentFinishTask;
				Player->LineTaskProgress.GetAllFinishTask(CurTaskType, CurrentFinishTask);

				//获取新增的任务
				FAllTaskDataBlock NewTask;
				AllTask.GetTaskMatchReverse(CurTaskType, CurrentFinishTask, NewTask);

				//重新添加数据块
				Player->AllTasks.ReAddedTaskBlock(CurTaskType, NewTask);
			}
		}
	}
	else {
		Player->DayTaskTime = TEXT("");

		//初始化任务索引
		const FAllTaskData& AllTask = GET_CACHE(Task)->GetAllTaskData();
		//任务刷新[移除任务]
		Player->LineTaskProgress.RemoveFinishTask(EGameTaskType::DayTask);
		Player->AllTasks.RemoveTaskBlock(EGameTaskType::DayTask);
		//新增每日任务数据库
		Player->AllTasks.InitTaskBlock(EGameTaskType::DayTask, AllTask);
	}
}

void FAllTask::DirectFinishDayTask(UPlayerStructManager* Player, const FAllTaskData& AllTask)
{
	//检测常回家看看任务的完成情况
	FTaskIndex LoginGameDayTask;
	LoginGameDayTask.ID = 1;
	LoginGameDayTask.TaskType = EGameTaskType::DayTask;

	//查询任务完成情况
	if (
		!Player->LineTaskProgress.FindFinishTask(LoginGameDayTask)
		&&
		!Player->LineTaskProgress.FindFinishTaskNotClaimed(LoginGameDayTask)
		)
	{
		//表示未完成

		//查询任务数据，将任务添加到执行列表
		FTaskBase NewBase;
		if (AllTask.FindTask(LoginGameDayTask, NewBase))
		{
			Player->LineTaskProgress.AddFinishTaskNotClaimed(LoginGameDayTask, NewBase);
			Player->AllTasks.FinishTask(LoginGameDayTask);
		}
	}
}

void FAllTaskData::GetTaskMatchReverse(
	const EGameTaskType& TaskType,
	const FAllTaskDataBlock& InputTask,
	FAllTaskDataBlock& OutTask
)const
{
	FAllTaskDataBlock TempBlock = this->GetTaskBlock(TaskType);

	for (const auto& Task : InputTask.AllTaskData)
	{
		TempBlock.AllTaskData.Remove(Task.Key);
	}

	OutTask = TempBlock;
}

void FLineTaskProgress::GetAllFinishTask(const EGameTaskType& TaskType, FAllTaskDataBlock& OutFinishTask) {
	//新增完成的任务
	FFinishTask* FTask = this->AllFinishTask.Find(TaskType);
	if (FTask)
	{
		OutFinishTask.AllTaskData.Append(FTask->TaskID);
	}
	//新增未领取的任务
	FFinishTask* FTaskNotClaimed = this->AllFinishTaskNotClaimed.Find(TaskType);
	if (FTaskNotClaimed)
	{
		OutFinishTask.AllTaskData.Append(FTaskNotClaimed->TaskID);
	}
}

void FAllTaskData::InitTaskBlock()
{
	if (this->bInit)
	{

	}
	else {

		this->bInit = true;

		//初始化主线任务数据
		this->InitTaskDataBlock(EGameTaskType::MainTask,
			GET_CACHE(Task)->GetMainTask()
		);

		//初始化每日任务数据
		this->InitTaskDataBlock(EGameTaskType::DayTask,
			GET_CACHE(Task)->GetDayTask()
		);

		//初始化角色升级任务数据
		this->InitTaskDataBlock(EGameTaskType::PlayerUp,
			GET_CACHE(Task)->GetPlayerUpTask()
		);

		//初始化挑战任务数据
		this->InitTaskDataBlock(EGameTaskType::Challenge,
			GET_CACHE(Task)->GetChallengeTask()
		);
	}
}

int32 FAllTaskData::GetAllTaskCount() const
{
	return this->TaskCount;
}

void FAllTask::FinishTask(const FTaskIndex& Index)
{
	FAllExecTaskDataBlock* TaskBlock = this->CurTasks.Find(Index.TaskType);
	if (TaskBlock)
	{
		//移除当前任务ID
		TaskBlock->AllTaskData.Remove(Index.ID);
		//减少任务数量
		this->TaskCount--;
		//更新下一个任务
		for (auto PP = TaskBlock->AllTaskData.CreateConstIterator(); PP; ++PP)
		{
			TaskBlock->TaskID = PP->Key;
			break;
		}
	}
	else {
		if (UFVMGameInstance::GetDebug())
		{
			FVMLOG(__FUNCTION__, FString(
				FString("任务完成，但是未查询到该任务！") +
				FString::FromInt(Index.ID) +
				FString(" : ") +
				FString::FromInt((int32)Index.TaskType)
			));
		}
	}
}

const FString& UTaskStructBase::GetTaskName() const
{
	return this->Data.TaskName;
}

const int32& UTaskStructBase::GetTaskID() const
{
	return this->Data.TaskID;
}

const EGameTaskType& UTaskStructBase::GetTaskType() const
{
	return this->TaskType;
}

const FTaskBase& UTaskStructBase::GetData() const
{
	return this->Data;
}

void UTaskFuncObjectExe::OnInit()
{
	this->OnExe();
}
