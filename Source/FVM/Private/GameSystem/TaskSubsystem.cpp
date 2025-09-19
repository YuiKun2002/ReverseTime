// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/TaskSubsystem.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/Item/PlayerRecord.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/GameUserInterfaceSubsystem.h"
#include "GameSystem/Item/Task/Inter/TaskInterface.h"

UTaskSubsystem* UTaskSubsystem::GetTaskSubsystemStatic()
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return UFVMGameInstance::GetFVMGameInstance()->GetSubsystem<UTaskSubsystem>();
	}

	return nullptr;
}

void UTaskSubsystem::MoveTaskIndexSingle(
	const FTaskIndex& TaskIndex,
	FLineTaskProgress& TaskProgress
)
{
	TaskProgress.MoveFinishTaskNotClaimed(TaskIndex);
}

void UTaskSubsystem::LoadTaskSystem()
{
	//加载角色数据
	this->Player = UFVMGameInstance::GetPlayerStructManager_Static();

	//加载系统数据
	this->PlayerName = this->Player->M_PlayerName;
	this->bUpdateUI = true;

	//重新更新任务列表
	this->UpdateTask(EGameTaskType::MainTask);
	this->UpdateTask(EGameTaskType::DayTask);
	this->UpdateTask(EGameTaskType::PlayerUp);
	this->UpdateTask(EGameTaskType::Challenge);
}

void UTaskSubsystem::Deinitialize()
{
	//卸载
	this->LevelTaskDataTable.Empty();
	this->CurExecTasksHandle.Empty();
	this->SynMakeCardTaskDataTable.Empty();
	this->SynCardUpGradeDataTable.Empty();
	this->LookInforTaskDataTable.Empty();
	this->CardSkillTaskDataTable.Empty();
	this->GemTaskDataTable.Empty();
}

void UTaskSubsystem::ExecuteTasks(UObject* ObjectClass)
{
	//需要移除的任务
	TMap<int32, UTaskStructBase*> CurFinishIns;
	TMap<class UTaskFunction*, class UTaskStructBase*> CurFinishHandle;

	//当前已经完成的任务类型
	bool bFinish = false;

	//执行所有任务
	FTaskIndex CurExecTaskIndex;
	for (const auto& Task : this->CurExecTasksHandle)
	{
		if (IsValid(Task.Key) && IsValid(Task.Value))
		{
			//初始化ID和类型
			CurExecTaskIndex.ID = Task.Value->GetTaskID();
			CurExecTaskIndex.TaskType = Task.Value->GetTaskType();
			//执行任务
			if (Task.Key->ExeTask(Task.Value->GetTaskName(), CurExecTaskIndex, ObjectClass))
			{
				//添加已经完成的任务类型
				bFinish = true;

				//更新显示UI
				this->bUpdateUI = true;

				//将完成的任务移动
				CurFinishIns.Emplace(CurExecTaskIndex.ID, Task.Value);
				CurFinishHandle.Emplace(Task.Key, Task.Value);

				UGameUserInterfaceSubsystem::ShowTaskFinishTipUISub(
					nullptr,
					UFVMGameInstance::GetFVMGameInstance()
				);
			}
		}
	}

	if (bFinish)
	{
		//移除当前正在执行的列表任务
		this->RemoveTask(CurFinishHandle, this->CurExecTasksHandle);

		//重新赋值角色
		this->Player = UFVMGameInstance::GetFVMGameInstance()->GetPlayerStructManager();
		//添加完成的任务
		for (auto PP = CurFinishIns.CreateConstIterator(); PP; ++PP)
		{
			FTaskIndex Index;
			Index.ID = PP->Key;
			Index.TaskType = PP->Value->GetTaskType();

			//任务结束
			FTaskBase NewFinishTaskData = PP->Value->GetData();
			NewFinishTaskData.TaskState = 2;
			//添加完成的任务
			Player->LineTaskProgress.AddFinishTaskNotClaimed(Index, NewFinishTaskData);
			//完成当前执行的任务
			Player->AllTasks.FinishTask(Index);
		}

		UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("保存完成的{任务}")));
	}
}

void UTaskSubsystem::GetTasksList(TArray<FTaskBase>& Out)
{
	//添加当前正在执行的任务
	for (const auto& Task : this->CurExecTasksHandle)
	{
		FTaskBase Cur = Task.Value->GetData();
		Cur.TaskType = Task.Value->GetTaskType();
		Out.Emplace(Cur);
	}

	//添加已经完成但是没有领取奖励的任务
	this->Player->LineTaskProgress.GetFinishTaskNotClaimed(Out);
}

FString UTaskSubsystem::GetPlayerName()
{
	return this->PlayerName;
}

bool UTaskSubsystem::FindTask(int32 TaskID, EGameTaskType TaskType, FTaskBase& Out)
{
	FAllTaskDataBlock CurBlock = GET_CACHE(Task)->GetAllTaskData().GetTaskBlock(TaskType);
	FTaskBase* CurTaskBase = CurBlock.AllTaskData.Find(TaskID);
	if (CurTaskBase)
	{
		Out = *CurTaskBase;
		return true;
	}
	return false;
}

bool UTaskSubsystem::FindFinishedTask(int32 TaskID, EGameTaskType TaskType)
{
	//重新赋值角色
	this->Player = UFVMGameInstance::GetFVMGameInstance()->GetPlayerStructManager();

	if (IsValid(this->Player))
	{
		FTaskIndex NewIndex;
		NewIndex.ID = TaskID;
		NewIndex.TaskType = TaskType;

		//如果是非线序任务，【可以同时开启多个同类型的任务】，则只需要通过查询来判断是否完成
		if (TaskType == EGameTaskType::DayTask)
		{
			//谁的数据小，查谁
		//未完成的任务数量+未领取的任务数量 大于 已经完成的任务数量。
			if (
				this->Player->AllTasks.GetTaskCount() + this->Player->LineTaskProgress.GetFinishTaskNotClaimedCount()
		>
				this->Player->LineTaskProgress.GetFinishTaskCount())
			{
				return this->Player->LineTaskProgress.FindFinishTask(NewIndex);
			}
			else {

				//表示任务未领取，也算作未完成
				if (this->Player->LineTaskProgress.FindFinishTaskNotClaimed(NewIndex))
				{
					return false;
				}

				//未完成的任务
				if (this->Player->AllTasks.FindTask(NewIndex))
				{
					return false;
				}
			}
		}
		else {

			//查询未领取的任务
			if (this->Player->LineTaskProgress.FindFinishTaskNotClaimed(NewIndex))
			{
				return false;
			}

			//如果当前ID有效，表示任务未完成
			return !this->Player->AllTasks.CheckValidTaskID(NewIndex);
		}
	}

	return false;
}

bool UTaskSubsystem::GetUpdate()
{
	bool bTemp = this->bUpdateUI;

	this->bUpdateUI = false;

	return bTemp;
}

bool UTaskSubsystem::GetHasFinishTask()
{
	if (this->Player)
	{
		if (this->Player->LineTaskProgress.GetFinishTaskCount() > 0)
		{
			return true;
		}
	}

	return false;
}

void UTaskSubsystem::GetAllTaskData(FAllTaskData& AllTaskData)
{
	AllTaskData = GET_CACHE(Task)->GetAllTaskData();
}

void UTaskSubsystem::UpdateTask(const EGameTaskType& NewTaskType)
{
	switch (NewTaskType)
	{
	case EGameTaskType::DayTask: { this->InitDayTask(); }; break;
	default: {
		if (NewTaskType != EGameTaskType::MaxEnd)
		{
			this->AddLineTask(NewTaskType);
		}
	}break;
	}
}

void UTaskSubsystem::AddNewTask(const FTaskBase& Task)
{

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__,
			FString(TEXT("添加新任务：")) + Task.GetTaskName() + TEXT("  Type:") +
			FString::FromInt(((int32)Task.GetTaskType()))
		);
	}

	//如果等级不满足，则无法添加任务
	if (this->Player->M_FPlayerEx.M_PlayerGrade < Task.TaskLevel)
	{
		if (UFVMGameInstance::GetDebug())
		{
			UGameSystemFunction::FVMLog(__FUNCTION__,
				FString(TEXT("新任务：")) + Task.GetTaskName() + TEXT("等级不足，无法触发  Type:") +
				FString::FromInt(((int32)Task.GetTaskType()))
			);
		}
		return;
	}

	//查询当前任务是否存在
	FAllTaskDataBlock* TaskBlock = this->CurExecTasksTag.Find(Task.GetTaskType());
	if (TaskBlock)
	{
		if (TaskBlock->AllTaskData.Contains(Task.TaskID))
		{
			return;
		}
	}

	//添加任务句柄
	FTaskIndex TaskIndex;
	TaskIndex.ID = Task.TaskID;
	TaskIndex.TaskType = Task.GetTaskType();

	UTaskFunction* CurNewTaskHandle = nullptr;
	UClass* CurClass = Task.TaskFunctionClass.LoadSynchronous();
	if (IsValid(CurClass))
	{
		TSubclassOf<UTaskFunction> CurFunClass(CurClass);
		if (CurFunClass.GetDefaultObject())
		{
			CurNewTaskHandle = CurFunClass.GetDefaultObject()->MakeNewClass();
		}
	}

	this->CurExecTasksHandle.Emplace(CurNewTaskHandle, UTaskStructBase::MakeTaskObject(Task));
	this->CurExecTasksIndex.Emplace(CurNewTaskHandle, TaskIndex);

	//添加任务占位
	FAllTaskDataBlock* TempTaskBlock = this->CurExecTasksTag.Find(Task.GetTaskType());
	if (TempTaskBlock)
	{
		TempTaskBlock->AllTaskData.Emplace(Task.TaskID, Task);
	}
	else {
		FAllTaskDataBlock NewTaskBlock;
		NewTaskBlock.AllTaskData.Emplace(Task.TaskID, Task);
		this->CurExecTasksTag.Emplace(Task.GetTaskType(), NewTaskBlock);
	}
}

void UTaskSubsystem::AddLineTask(const EGameTaskType& Tasks)
{
	if (Tasks == EGameTaskType::DayTask)
	{
		return;
	}

	FAllExecTaskDataBlock* TaskBlock = this->Player->AllTasks.GetTaskBlock(Tasks);
	if (TaskBlock)
	{
		//查询当前任务不存在
		if (!this->CurExecTasksTag.Contains(Tasks))
		{
			//添加一个新的任务
			this->AddNewTask(TaskBlock->AllTaskData.Find(TaskBlock->GetTaskID()));
		}
	}
}

void UTaskSubsystem::RemoveTask(
	const TMap<class UTaskFunction*, class UTaskStructBase*>& Tasks,
	TMap<class UTaskFunction*, class UTaskStructBase*>& OutTasks
)
{
	//存储当前正在执行的全部任务
	TMap<class UTaskFunction*, class UTaskStructBase*> Temp = OutTasks;
	//清理之前正在执行的全部任务
	OutTasks.Empty();

	//遍历当前已经完成的任务[需要index与对象保持一致]
	for (auto PP = Tasks.CreateConstIterator(); PP; ++PP)
	{
		//移除占位符
		FAllTaskDataBlock* TaskBlock = this->CurExecTasksTag.Find(PP->Value->GetTaskType());
		if (TaskBlock)
		{
			TaskBlock->AllTaskData.Remove(PP->Value->GetTaskID());
		}
		//移除任务执行对象
		Temp.Remove(PP->Key);
		//移除任务索引
		this->CurExecTasksIndex.Remove(PP->Key);
	}

	OutTasks.Append(Temp);
}

void UTaskSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

}

void UTaskSubsystem::InitDayTask()
{
	if (this->Player->M_FPlayerEx.M_PlayerGrade >= 3)
	{
		//初始化每日任务
		this->Player->AllTasks.InitDayTask();

		//将任务添加到执行列表
		FAllExecTaskDataBlock* TaskBlock = this->Player->AllTasks.GetTaskBlock(EGameTaskType::DayTask);
		if (TaskBlock)
		{
			for (const auto& CurDayTask : TaskBlock->AllTaskData)
			{
				this->AddNewTask(CurDayTask.Value);
			}
		}
	}
	else {
		this->Player->DayTaskTime = TEXT("");
	}
}

const FAllTaskData& UTaskDataAssetCache::GetAllTaskData()
{
	this->AllData.InitTaskBlock();
	return this->AllData;
}

//获取主线任务数据
const TArray<FMainTask>& UTaskDataAssetCache::GetMainTask()
{
	return GET_SOURCE_DATA(FMainTask,
		this->MainTask,
		this->MainTaskSourceDataTable,
		Task,
		MainTask
	);
}

const TArray<FDayTask>& UTaskDataAssetCache::GetDayTask()
{
	return GET_SOURCE_DATA(FDayTask,
		this->DayTask,
		this->DayTaskSourceDataTable,
		Task,
		DayTask
	);
}

const TArray<FPlayerUpTask>& UTaskDataAssetCache::GetPlayerUpTask()
{
	return GET_SOURCE_DATA(FPlayerUpTask,
		this->PlayerUpTask,
		this->PlayerUpTaskSourceDataTable,
		Task,
		PlayerUpTask
	);
}

const TArray<FChallengeTask>& UTaskDataAssetCache::GetChallengeTask()
{
	return GET_SOURCE_DATA(FChallengeTask,
		this->ChallengeTask,
		this->ChallengeTaskSourceDataTable,
		Task,
		ChallengeTask
	);
}

bool UTaskDataAssetCache::GetImp_Level(
	const FTaskIndex& TaskIndex,
	FLevelTaskDataTable& OutTaskImpData
)
{
	return __Task_Imp_Get__(
		this->Imp_Level,
		TaskIndex,
		OutTaskImpData
	);
}

bool UTaskDataAssetCache::GetImp_PlayerUp(
	const FTaskIndex& TaskIndex, FPlayerUpTaskDataTable& OutTaskImpData
)
{
	return __Task_Imp_Get__(
		this->Imp_PlayerUp,
		TaskIndex,
		OutTaskImpData
	);
}

bool UTaskDataAssetCache::GetImp_LookInfor(
	const FTaskIndex& TaskIndex,
	FLookInforTaskDataTable& OutTaskImpData
)
{
	return __Task_Imp_Get__(
		this->Imp_LookInfor,
		TaskIndex,
		OutTaskImpData
	);
}

bool UTaskDataAssetCache::GetImp_SynMakeCard(
	const FTaskIndex& TaskIndex, FSynMakeCardTaskDataTable& OutTaskImpData
)
{
	return __Task_Imp_Get__(
		this->Imp_SynMakeCard,
		TaskIndex,
		OutTaskImpData
	);
}

bool UTaskDataAssetCache::GetImp_SynCardUp(
	const FTaskIndex& TaskIndex, FSynCardUpGradeDataTable& OutTaskImpData
)
{
	return __Task_Imp_Get__(
		this->Imp_SynCardUp,
		TaskIndex,
		OutTaskImpData
	);
}

bool UTaskDataAssetCache::GetImp_CardSkill(
	const FTaskIndex& TaskIndex,
	FCardSkillTaskDataTable& OutTaskImpData
)
{
	return __Task_Imp_Get__(
		this->Imp_CardSkill,
		TaskIndex,
		OutTaskImpData
	);
}

bool UTaskDataAssetCache::GetImp_Gem(
	const FTaskIndex& TaskIndex, FGemTaskDataTable& OutTaskImpData
)
{
	return __Task_Imp_Get__(
		this->Imp_Gem,
		TaskIndex,
		OutTaskImpData
	);
}