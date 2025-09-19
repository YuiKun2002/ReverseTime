// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/PlayerUpdate/PlayerTaskUpdate.h"
#include "GameSystem/TaskSubsystem.h"
#include "GameSystem/FVMGameInstance.h"

void UPlayerTaskUpdate::Update_Implementation(class UPlayerStructManager* PlayerDataIns)
{
	//初始化每日任务
	PlayerDataIns->AllTasks.InitDayTask();
	//计数任务数量[正在执行的任务数量]
	int32 CurExecCount = PlayerDataIns->AllTasks.GetTaskCount();
	//已经完成的任务数量
	int32 CurFinishCount =
		PlayerDataIns->LineTaskProgress.GetFinishTaskCount() +
		PlayerDataIns->LineTaskProgress.GetFinishTaskNotClaimedCount();

	//加载总任务
	//初始化任务索引
	const FAllTaskData& AllTask = GET_CACHE(Task)->GetAllTaskData();
	//当前总任务数量
	int32 CurAllTaskCount = AllTask.GetAllTaskCount();

	//如果游戏中任务数量出现新的变化，则需要出现更新最新任务进度【收缩任务】
	if (CurAllTaskCount != CurExecCount + CurFinishCount)
	{
		if (CurAllTaskCount < CurExecCount + CurFinishCount)
		{
			if (UFVMGameInstance::GetDebug())
			{
				FVMLOG(__FUNCTION__,
					FString(
						FString(TEXT("总任务数量小于当前角色存档的总任务，正在移除还为完成的任务，请检查数据表：")) +
						FString::FromInt(CurExecCount + CurFinishCount) +
						FString(TEXT(" / ")) +
						FString::FromInt(CurAllTaskCount)
					)
				);
			}

			//全部移除当前还未完成的任务，重新匹配
			PlayerDataIns->AllTasks.Clear();
			//初始化每日任务
			PlayerDataIns->AllTasks.InitDayTask();
		}

		if (UFVMGameInstance::GetDebug())
		{
			FVMLOG(__FUNCTION__,
				FString(
					FString(TEXT("出现新任务，重新计算任务进度：")) +
					FString::FromInt(CurExecCount + CurFinishCount) +
					FString(TEXT(" / ")) +
					FString::FromInt(CurAllTaskCount)
				)
			);
		}

		//更新线性任务索引
		uint8 MaxEnd = (uint8)EGameTaskType::MaxEnd;
		for (uint8 TEnum = 0; TEnum < MaxEnd; TEnum++)
		{
			//当前类型
			EGameTaskType CurType = (EGameTaskType)TEnum;

			//忽略每日任务
			if (CurType == EGameTaskType::DayTask)
			{
				continue;
			}

			//查询任务数据块
			FAllExecTaskDataBlock* ExecTaskBlock = PlayerDataIns->AllTasks.CurTasks.Find(CurType);
			if (ExecTaskBlock)
			{
				//生成【当前玩家】还需要执行的任务
				TArray<int32> Keys;
				ExecTaskBlock->AllTaskData.GetKeys(Keys);

				//生成最新的任务数据
				TArray<FTaskBase> TaskBase;
				AllTask.GetTaskBlock(CurType).AllTaskData.GenerateValueArray(TaskBase);

				//如果还有没有完成的任务
				if (Keys.Num() > 0)
				{
					//当前新任务的最后一个任务ID
					int32 NewTaskCount = TaskBase.Num() - 1;
					//新任务ID
					int32 NewID = Keys[Keys.Num() - 1] + 1;
					//计算新增ID数量
					int32 NewAddedCount = TaskBase.Num() - NewID;

					if (NewAddedCount < 0)
					{
						NewAddedCount = 0;
					}

					//如果新任务数量大于0，表示有新的任务
					if (NewAddedCount > 0)
					{
						for (; NewAddedCount > 0;)
						{
							FTaskBase& TaskData = TaskBase[NewID];

							//新增任务
							ExecTaskBlock->AllTaskData.Emplace(NewID, TaskData);
							PlayerDataIns->AllTasks.TaskCount++;

							if (UFVMGameInstance::GetDebug())
							{
								FVMLOG(__FUNCTION__, FString(
									FString(TEXT("新增任务：")) +
									TaskData.GetTaskName() +
									FString(TEXT("任务类型：")) +
									FString::FromInt((int32)TaskData.GetTaskType())
								));
							}

							NewID++;
							NewAddedCount--;
						}
					}
				}
				else {
					//当前新任务的最后一个任务ID
					int32 NewTaskCount = TaskBase.Num();
					//如果任务全部完成了,直接获取新任务ID
					int32 CurrentTaskCount = PlayerDataIns->LineTaskProgress.GetFinishTaskCountByTaskType(CurType);
					//计算新增任务数量
					int32 NewAddedCount = NewTaskCount - CurrentTaskCount;
					for (; NewAddedCount > 0;)
					{
						//直接新增任务
						FTaskBase& TaskData = TaskBase[CurrentTaskCount];
						ExecTaskBlock->AllTaskData.Emplace(CurrentTaskCount, TaskBase[CurrentTaskCount]);
						PlayerDataIns->AllTasks.TaskCount++;
						CurrentTaskCount++;
						NewAddedCount--;

						if (UFVMGameInstance::GetDebug())
						{
							FVMLOG(__FUNCTION__, FString(
								FString(TEXT("新增任务：")) +
								TaskData.GetTaskName() +
								FString(TEXT("任务类型：")) +
								FString::FromInt((int32)TaskData.GetTaskType())
							));
						}
					}
				}
			}
			else {

				//搜集完成的任务
				FAllTaskDataBlock CurrentFinishTask;
				PlayerDataIns->LineTaskProgress.GetAllFinishTask(CurType, CurrentFinishTask);

				if (UFVMGameInstance::GetDebug())
				{
					FVMLOG(__FUNCTION__, FString(
						FString(TEXT("当前完成任务总数量：")) +
						FString::FromInt(CurrentFinishTask.AllTaskData.Num())
					));
				}

				//获取新增的任务
				FAllTaskDataBlock NewTask;
				AllTask.GetTaskMatchReverse(CurType, CurrentFinishTask, NewTask);

				if (UFVMGameInstance::GetDebug())
				{
					FVMLOG(__FUNCTION__, FString(
						FString(TEXT("当前新增的任务总数量：")) +
						FString::FromInt(NewTask.AllTaskData.Num()) +
						FString(TEXT("新增的任务类型：")) +
						FString::FromInt(TEnum)
					));
				}

				//重新添加数据块
				PlayerDataIns->AllTasks.ReAddedTaskBlock(CurType, NewTask);
			}
		}

		if (UFVMGameInstance::GetDebug())
		{
			//已经完成的任务数量
			int32 TempCurFinishCount =
				PlayerDataIns->LineTaskProgress.GetFinishTaskCount() +
				PlayerDataIns->LineTaskProgress.GetFinishTaskNotClaimedCount();

			FVMLOG(__FUNCTION__,
				FString(
					FString(TEXT("[当前待执行任务数量]：")) +
					FString::FromInt(PlayerDataIns->AllTasks.GetTaskCount()) +
					FString(TEXT(" [当前已经完成的任务数量]：")) +
					FString::FromInt(TempCurFinishCount) +
					FString(TEXT(" / ")) +
					FString(TEXT("[当前游戏总任务数量]：")) +
					FString::FromInt(CurAllTaskCount)
				)
			);
		}
	}
	else {
		if (UFVMGameInstance::GetDebug())
		{
			FVMLOG(__FUNCTION__,
				FString(
					FString(TEXT("无新任务，目前任务数量：")) +
					FString::FromInt(CurExecCount + CurFinishCount) +
					FString(TEXT(" / ")) +
					FString::FromInt(CurAllTaskCount)
				)
			);
		}
	}
}
