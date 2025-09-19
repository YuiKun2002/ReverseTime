// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/GameDataSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameSystem/Item/Task/TaskStructBase.h"
#include "GameSystem/Tools/GameSystemFunction.h"

//关卡任务
#include "GameSystem/Item/Task/TaskFunc/LevelTask.h"
//合成屋-卡片制作
#include "GameSystem/Item/Task/TaskFunc/SynthesisTask.h"
//情报岛
#include "GameSystem/Item/Task/TaskFunc/LookInforTask.h"
//角色升级
#include "GameSystem/Item/Task/TaskFunc/PlayerUpTask.h"
//卡片技能书
#include "GameSystem/Item/Task/TaskFunc/CardSkillTask.h"
//宝石
#include "GameSystem/Item/Task/TaskFunc/GemTask.h"

#include "TaskSubsystem.generated.h"

class UPlayerStructManager;

/**
 * 任务子系统
 */
UCLASS()
class FVM_API UTaskSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	//获取子系统
	UFUNCTION(BlueprintCallable, Category = "任务子系统")
	static UTaskSubsystem* GetTaskSubsystemStatic();

	/*
	* MoveTaskIndexSingle
	@param1 当前需要移动的任务索引
	@param2 当前移动的索引存放在那个位置
	@param3 需要将当前索引移动到目标位置
	*/
	UFUNCTION(BlueprintCallable, Category = "任务子系统 | 移动任务索引")
	static void MoveTaskIndexSingle(
		const FTaskIndex& TaskIndex,
		FLineTaskProgress& TaskProgress
	);
public:
	//加载任务系统
	UFUNCTION(BlueprintCallable)
	void LoadTaskSystem();
	//执行系统任务->执行任务
	UFUNCTION(BlueprintCallable)
	void ExecuteTasks(UObject* ObjectClass);
	//获取任务条目->需要显示到UI上的任务
	UFUNCTION(BlueprintCallable)
	void GetTasksList(TArray<FTaskBase>& Out);
	//获取玩家名称
	UFUNCTION(BlueprintCallable)
	FString GetPlayerName();
	//查询任务数据
	UFUNCTION(BlueprintCallable)
	bool FindTask(int32 TaskID, EGameTaskType TaskType, FTaskBase& Out);
	//查询已经完成的任务[注意如果任务未领取也算任务未完成]
	UFUNCTION(BlueprintCallable)
	bool FindFinishedTask(int32 TaskID, EGameTaskType TaskType);
	//是否更新UI刷新任务条目
	UFUNCTION(BlueprintCallable)
	bool GetUpdate();
	//是否有完成的任务
	UFUNCTION(BlueprintCallable)
	bool GetHasFinishTask();
	//获取任务数据块
	UFUNCTION(BlueprintPure)
	void GetAllTaskData(FAllTaskData& AllTaskData);
public:
	//初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//取消初始化
	virtual void Deinitialize() override;
public:
	//关卡任务数据表
	UPROPERTY()
	TArray<FLevelTaskDataTable> LevelTaskDataTable;
	//角色升级
	UPROPERTY()
	TArray<FPlayerUpTaskDataTable> PlayerUpTaskDataTable;
	//情报岛
	UPROPERTY()
	TArray<FLookInforTaskDataTable> LookInforTaskDataTable;
	//合成屋，卡片制作任务数据表
	UPROPERTY()
	TArray<FSynMakeCardTaskDataTable> SynMakeCardTaskDataTable;
	//合成屋，卡片强化
	UPROPERTY()
	TArray<FSynCardUpGradeDataTable> SynCardUpGradeDataTable;
	//卡片技能书
	UPROPERTY()
	TArray<FCardSkillTaskDataTable> CardSkillTaskDataTable;
	//宝石
	UPROPERTY()
	TArray<FGemTaskDataTable> GemTaskDataTable;

protected:
	//角色管理器
	UPROPERTY()
	UPlayerStructManager* Player = nullptr;
	//角色名称
	UPROPERTY()
	FString PlayerName;
	//UI更新
	UPROPERTY()
	bool bUpdateUI = false;

	//任务句柄
	UPROPERTY()
	TMap<class UTaskFunction*, class UTaskStructBase*> CurExecTasksHandle;
	//任务索引
	UPROPERTY()
	TMap<class UTaskFunction*, FTaskIndex> CurExecTasksIndex;
	//任务占位
	UPROPERTY()
	TMap<EGameTaskType, FAllTaskDataBlock> CurExecTasksTag;
private:
	//更新任务
	void UpdateTask(const EGameTaskType& NewTaskType);
	//添加任务
	void AddNewTask(const FTaskBase& Task);
	//添加线性任务功能
	void AddLineTask(const EGameTaskType& Tasks);
	//批量移除任务
	void RemoveTask(
		const TMap<class UTaskFunction*, class UTaskStructBase*>& Tasks,
		TMap<class UTaskFunction*, class UTaskStructBase*>& OutTasks
	);
	//初始化每日任务[日，周，月，需要刷新的任务]
	void InitDayTask();

private:
	//添加任务
	template<class T>
	void AddNewTask(T* NewTask)
	{
		if (NewTask)
		{
			this->AddNewTask(*NewTask);
		}
	}
};

//继承缓存模板
UCLASS(BlueprintType, Blueprintable)
class FVM_API UTaskDataAssetCache : public UGameDataAssetCache
{
	GENERATED_BODY()
public:
	//获取 实现 关卡任务
	bool GetImp_Level(const FTaskIndex& TaskIndex, FLevelTaskDataTable& OutTaskImpData);
	//获取 实现 角色升级任务
	bool GetImp_PlayerUp(const FTaskIndex& TaskIndex, FPlayerUpTaskDataTable& OutTaskImpData);
	//获取 实现 情报岛任务
	bool GetImp_LookInfor(const FTaskIndex& TaskIndex, FLookInforTaskDataTable& OutTaskImpData);
	//获取 实现 卡片制作任务
	bool GetImp_SynMakeCard(const FTaskIndex& TaskIndex, FSynMakeCardTaskDataTable& OutTaskImpData);
	//获取 实现 卡片强化任务
	bool GetImp_SynCardUp(const FTaskIndex& TaskIndex, FSynCardUpGradeDataTable& OutTaskImpData);
	//获取 实现 卡片技能书任务
	bool GetImp_CardSkill(const FTaskIndex& TaskIndex, FCardSkillTaskDataTable& OutTaskImpData);
	//获取 实现 宝石任务
	bool GetImp_Gem(const FTaskIndex& TaskIndex, FGemTaskDataTable& OutTaskImpData);



	//获取全部任务数据
	const FAllTaskData& GetAllTaskData();
	//获取主线任务数据
	const TArray<FMainTask>& GetMainTask();
	//获取每日任务数据
	const TArray<FDayTask>& GetDayTask();
	//获取角色升级任务数据
	const TArray<FPlayerUpTask>& GetPlayerUpTask();
	//获取挑战任务数据
	const TArray<FChallengeTask>& GetChallengeTask();
private:
	//全部任务数据
	UPROPERTY()
	FAllTaskData AllData;
	//主线任务
	DataTableAssetData<FMainTask> MainTaskSourceDataTable;
	UPROPERTY()
	TArray<FMainTask> MainTask;
	//每日任务
	DataTableAssetData<FDayTask> DayTaskSourceDataTable;
	UPROPERTY()
	TArray<FDayTask> DayTask;
	//角色升级任务
	DataTableAssetData<FPlayerUpTask> PlayerUpTaskSourceDataTable;
	UPROPERTY()
	TArray<FPlayerUpTask> PlayerUpTask;
	//挑战任务
	DataTableAssetData<FChallengeTask> ChallengeTaskSourceDataTable;
	UPROPERTY()
	TArray<FChallengeTask> ChallengeTask;

	//关卡任务数据表【任务实现】
	TSharedPtr<TaskDataTableInitManager<
		FLevelTaskDataTable,
		INITTASKDATATABLEMANAGERNAME(Imp_Task_Level)
		>> Imp_Level;
	//角色升级任务数据表
	TSharedPtr<TaskDataTableInitManager<
		FPlayerUpTaskDataTable,
		INITTASKDATATABLEMANAGERNAME(Imp_Task_PlayerUpgrade)
		>> Imp_PlayerUp;
	//情报岛任务数据表
	TSharedPtr<TaskDataTableInitManager<
		FLookInforTaskDataTable,
		INITTASKDATATABLEMANAGERNAME(Imp_Task_Information)
		>> Imp_LookInfor;
	//卡片制作任务数据表
	TSharedPtr<TaskDataTableInitManager<
		FSynMakeCardTaskDataTable,
		INITTASKDATATABLEMANAGERNAME(Imp_Task_MakeCard)
		>> Imp_SynMakeCard;
	//卡片强化任务数据表
	TSharedPtr<TaskDataTableInitManager<
		FSynCardUpGradeDataTable,
		INITTASKDATATABLEMANAGERNAME(Imp_Task_CardUpgrade)
		>> Imp_SynCardUp;
	//卡片技能书任务数据表
	TSharedPtr<TaskDataTableInitManager<
		FCardSkillTaskDataTable,
		INITTASKDATATABLEMANAGERNAME(Imp_Task_CardSkillBooks)
		>> Imp_CardSkill;
	//宝石任务数据表
	TSharedPtr<TaskDataTableInitManager<
		FGemTaskDataTable,
		INITTASKDATATABLEMANAGERNAME(Imp_Task_Gem)
		>> Imp_Gem;
};