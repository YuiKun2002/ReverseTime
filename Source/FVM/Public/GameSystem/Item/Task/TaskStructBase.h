// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/GameDataSubsystem.h"
#include "TaskStructBase.generated.h"

/**
 * 任务结构
 */

class UTaskFunction;

//任务类型
UENUM(BlueprintType)
enum class EGameTaskType : uint8 {
	//主线任务
	MainTask UMETA(DisplayName = "MainTask"),
	//每日任务
	DayTask UMETA(DisplayName = "DayTask"),
	//升级任务
	PlayerUp UMETA(DisplayName = "PlayerUp"),
	//挑战任务
	Challenge UMETA(DisplayName = "Challenge"),

	//注意，每一次新加类型后，需要去任务子系统更新任务的加载方式
	MaxEnd,
};

//任务奖励结构
USTRUCT(BlueprintType)
struct FTaskIndex {
	GENERATED_USTRUCT_BODY()
public:
	//任务ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ID = 0;
	//任务类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameTaskType TaskType = EGameTaskType::MainTask;
};

//关卡任务表结构
USTRUCT(BlueprintType)
struct FTaskDataTable : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	//对应的任务名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TaskName;
	//任务索引
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTaskIndex TaskIndex;
};

//初始化一个实现任务类型
#define INITTASKDATATABLEMANAGERNAME(Name)\
GET_DEF_NAME(Task, Name)\

//关卡任务表结构初始化管理器
template<typename ImpTaskDataType, FName& CategoryName>
class TaskDataTableInitManager {
public:
	static TSharedPtr<TaskDataTableInitManager<ImpTaskDataType, CategoryName>> MakeTaskDataTableManager()
	{
		TSharedPtr<TaskDataTableInitManager<ImpTaskDataType, CategoryName>> NewPtr =
			MakeShareable(
				new TaskDataTableInitManager<ImpTaskDataType, CategoryName>()
			);
		NewPtr->InitArray();
		NewPtr->InitTaskDataTable();
		return NewPtr;
	};

	struct TaskDataTableDataStruct
	{
		TMap<int32, ImpTaskDataType> Data;
	};

	//查询
	bool FindImpTask(const FTaskIndex& TaskIndex, ImpTaskDataType& OutTaskImpData)
	{
		TaskDataTableDataStruct* DataStructPtr = this->CurTasks.Find(TaskIndex.TaskType);
		if (DataStructPtr)
		{
			ImpTaskDataType* FindData = DataStructPtr->Data.Find(TaskIndex.ID);
			if (FindData)
			{
				OutTaskImpData = *FindData;

				return true;
			}
		}

		return false;
	}
protected:
	//初始化数组
	void InitArray() {
		GET_SOURCE_DATA_NO_MOVE(ImpTaskDataType,
			this->TaskArray,
			this->TaskSourceDataTable,
			Task,
			MoveTemp(CategoryName)
		);
	};
	//初始化任务数据
	void InitTaskDataTable() {
		//添加新数据
		for (const ImpTaskDataType& TaskData : this->TaskArray)
		{
			this->AddNewData(TaskData.TaskIndex.TaskType, TaskData.TaskIndex.ID, TaskData);
		}
	}
	void AddNewData(const EGameTaskType& TaskType, const int32& TaskID, const ImpTaskDataType& InputData)
	{
		TaskDataTableDataStruct* DataStructPtr = this->CurTasks.Find(TaskType);
		if (DataStructPtr)
		{
			DataStructPtr->Data.Emplace(TaskID, InputData);
		}
		else {
			TaskDataTableDataStruct NewDataStruct;
			NewDataStruct.Data.Emplace(TaskID, InputData);
			this->CurTasks.Emplace(TaskType, NewDataStruct);
		}
	}


	//全部可执行任务
	TMap<EGameTaskType, TaskDataTableDataStruct> CurTasks;

	//数据表数据
	DataTableAssetData<ImpTaskDataType> TaskSourceDataTable;
	TArray<ImpTaskDataType> TaskArray;
private:
	bool bInit = false;
};

//任务实现数据表获取
template<class Type, FName& Name>
bool __Task_Imp_Get__(
	TSharedPtr<TaskDataTableInitManager<Type, Name>>& RefClass,
	const FTaskIndex& TaskIndex,
	Type& OutType
)
{
	if (RefClass.IsValid())
	{
		return RefClass.Get()->FindImpTask(TaskIndex, OutType);
	}

	RefClass = TaskDataTableInitManager<Type, Name>::MakeTaskDataTableManager();

	return RefClass.Get()->FindImpTask(TaskIndex, OutType);
};

//任务奖励结构
USTRUCT(BlueprintType)
struct FTaskItemStruct {
	GENERATED_USTRUCT_BODY()
public:
	//奖励名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;
	//奖励个数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemCount;
	//奖励指令
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemIns = TEXT("player send equip");
	//奖励图标
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath ImagePath;
};

//任务表结构
USTRUCT(BlueprintType)
struct FTaskTableRowBase : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	//获取任务名称
	virtual	FString GetTaskName() const { return FString(); };
	//获取任务经验值
	virtual int32 GetTaskEx()const { return 0; };
	//获取任务货币
	virtual int32 GetTaskCoin() const { return 0; };
	//获取任务奖励
	virtual TArray<FTaskItemStruct> GetTaskItems() const { return TArray<FTaskItemStruct>(); };
	//获取任务类型
	virtual EGameTaskType GetTaskType() const { return EGameTaskType::MainTask; };
};

//任务基本结构
USTRUCT(BlueprintType)
struct FTaskBase : public FTaskTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	//任务类型
	UPROPERTY()
	EGameTaskType TaskType = EGameTaskType::MainTask;
	//任务状态[新，进行中，未领取，完成]
	UPROPERTY()
	uint8 TaskState = 0;
	//任务ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TaskID = 0;
	//任务开启等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TaskLevel = 1;
	//任务类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UTaskFunction> TaskFunctionClass = TSoftClassPtr<UTaskFunction>(FSoftObjectPath(TEXT("/Script/FVM.TaskFunction")));
	//任务名称{ID}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TaskName;
	//任务副标题
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TaskOtherTitle;
	//任务描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TaskContent;
	//任务目标
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TaskTarget;
	//任务经验值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TaskEx = 10;
	//任务金币资源
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TaskCoin = 100;
	//任务奖励
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTaskItemStruct> TaskItems;
	//任务完成需要执行的对象
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UTaskFuncObjectExe> ExeObject;
private:

public:
	//获取任务名称
	virtual	FString GetTaskName() const override { return this->TaskName; };
	//获取任务经验值
	virtual int32 GetTaskEx()const override { return this->TaskEx; };
	//获取任务货币
	virtual int32 GetTaskCoin()const override { return this->TaskCoin; };
	//获取任务奖励
	virtual TArray<FTaskItemStruct> GetTaskItems()const override { return this->TaskItems; };
	//获取任务类型
	virtual EGameTaskType GetTaskType()const override { return this->TaskType; };
};

//主线任务结构【线性】
USTRUCT(BlueprintType)
struct FMainTask : public FTaskBase {
	GENERATED_USTRUCT_BODY()
public:
	FMainTask() {
		this->TaskType = EGameTaskType::MainTask;
	};
};

//角色升级任务结构【线性】
USTRUCT(BlueprintType)
struct FPlayerUpTask : public FTaskBase {
	GENERATED_USTRUCT_BODY()
public:
	FPlayerUpTask() {
		this->TaskType = EGameTaskType::PlayerUp;
	};
};

//挑战任务结构【线性】
USTRUCT(BlueprintType)
struct FChallengeTask : public FTaskBase {
	GENERATED_USTRUCT_BODY()
public:
	FChallengeTask() {
		this->TaskType = EGameTaskType::Challenge;
	};
};

//每日任务结构【非线性】
USTRUCT(BlueprintType)
struct FDayTask : public FTaskBase {
	GENERATED_USTRUCT_BODY()
public:
	FDayTask() {
		this->TaskType = EGameTaskType::DayTask;
	};
};

USTRUCT(BlueprintType)
struct FAllTaskDataBlock {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FTaskBase> AllTaskData;
public:
	template<class T>
	void GetAllTaskData(TArray<T>& OutAllTaskData)
	{
		TArray<FTaskBase> Temp;
		AllTaskData.GenerateValueArray(Temp);

		OutAllTaskData = (TArray<T>)Temp;
	}
};

//全任务数据[任务数据库]
USTRUCT(BlueprintType)
struct FAllTaskData {
	GENERATED_USTRUCT_BODY()
public:

	//获取当前总任务数量
	int32 GetAllTaskCount() const;

	//获取任务数据块
	FAllTaskDataBlock GetTaskBlock(const EGameTaskType& TaskType) const
	{
		const FAllTaskDataBlock* CurBlock = this->TaskDataBlock.Find(TaskType);
		if (CurBlock)
		{
			return	*CurBlock;
		}

		return FAllTaskDataBlock();
	}

	//查询任务
	bool FindTask(const FTaskIndex& Index, FTaskBase& TaskBaseData) const {
		FAllTaskDataBlock Block = this->GetTaskBlock(Index.TaskType);
		FTaskBase* NewData = Block.AllTaskData.Find(Index.ID);
		if (NewData)
		{
			TaskBaseData = *NewData;
			return true;
		}
		return false;
	}

	//通过一个类型的数据块返回另一边同类型的数据块
	void GetTaskMatchReverse(
		const EGameTaskType& TaskType,
		const FAllTaskDataBlock& InputTask,
		FAllTaskDataBlock& OutTask
	)const;

	//初始化任务数据
	void InitTaskBlock();

	//初始化任务数据块
	template<class T>
	void InitTaskDataBlock(const EGameTaskType& TaskType, TArray<T> TaskBlock)
	{
		FAllTaskDataBlock* CurBlock = this->TaskDataBlock.Find(TaskType);
		if (!CurBlock)
		{
			FAllTaskDataBlock NewBlock;
			int32 i = 0;
			for (T& TaskData : TaskBlock)
			{
				//重新赋予ID号
				TaskData.TaskID = i;
				NewBlock.AllTaskData.Emplace(i, TaskData);
				i += 1;
			}
			this->TaskCount += NewBlock.AllTaskData.Num();
			this->TaskDataBlock.Emplace(TaskType, NewBlock);
		}
	}
protected:
	//全部的任务数据
	UPROPERTY()
	TMap<EGameTaskType, FAllTaskDataBlock> TaskDataBlock;
	//是否初始化
	UPROPERTY()
	bool bInit = false;
	//任务总数量
	UPROPERTY()
	int32 TaskCount = 0;
};


//当前任务进度【玩家需要执行的任务，带有任务进度】
USTRUCT(BlueprintType)
struct FAllExecTaskDataBlock : public FAllTaskDataBlock {
	GENERATED_USTRUCT_BODY()
public:
	//当前需要执行的任务ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TaskID = 0;

public:
	//获取有效的任务ID
	int32 GetTaskID() {
		return this->TaskID;
	}
};


//全部可执行的任务【当前玩家账号的全部待执行任务】
USTRUCT(BlueprintType)
struct FAllTask {
	GENERATED_USTRUCT_BODY()
public:
	//全部可执行任务
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EGameTaskType, FAllExecTaskDataBlock> CurTasks;
	//数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TaskCount = 0;
public:
	//初始化可执行任务【角色创建账号时调用一次】
	void InitTaskToNewPlayer(const FAllTaskData& TaskBlock) {
		//初始化主线任务
		this->InitTaskBlock(EGameTaskType::MainTask, TaskBlock);
		//初始化角色升级任务
		this->InitTaskBlock(EGameTaskType::PlayerUp, TaskBlock);
		//初始化挑战任务
		this->InitTaskBlock(EGameTaskType::Challenge, TaskBlock);


		//初始化每日任务,由任务更新系统执行，任务系统更新，角色创建
	}

	//初始化每日任务，周任务，月任务
	void InitDayTask(class UPlayerStructManager* PlayerIns = nullptr);

	//直接就可以完成的任务
	void DirectFinishDayTask(class UPlayerStructManager* Player, const FAllTaskData& AllTask);

	void InitTaskBlock(const EGameTaskType& TaskType, const FAllTaskData& TaskBlock)
	{
		FAllTaskDataBlock NewTaskBlock = TaskBlock.GetTaskBlock(TaskType);
		//初始化可执行的任务数据块
		FAllExecTaskDataBlock NewExecTaskBlock;
		NewExecTaskBlock.AllTaskData = NewTaskBlock.AllTaskData;
		NewExecTaskBlock.TaskID = 0;
		this->CurTasks.Emplace(TaskType, NewExecTaskBlock);
		//新增任务数量
		this->TaskCount += NewExecTaskBlock.AllTaskData.Num();
	}

	//重新添加任务块
	void ReAddedTaskBlock(const EGameTaskType& TaskType, const FAllTaskDataBlock& NewTaskBlock) {

		FAllExecTaskDataBlock* TaskBlock = this->CurTasks.Find(TaskType);

		//填充数据
		FAllExecTaskDataBlock NewExecTaskBlock;
		NewExecTaskBlock.AllTaskData = NewTaskBlock.AllTaskData;

		//重置索引
		for (auto PP = NewTaskBlock.AllTaskData.CreateConstIterator(); PP; ++PP)
		{
			NewExecTaskBlock.TaskID = PP->Key;
			break;
		}

		//如果之前存在任务，则移除重新添加
		if (TaskBlock)
		{
			this->TaskCount -= TaskBlock->AllTaskData.Num();
			this->CurTasks.Remove(TaskType);

		}

		this->CurTasks.Emplace(TaskType, NewExecTaskBlock);
		this->TaskCount += NewExecTaskBlock.AllTaskData.Num();
	}

	//查询任务
	bool FindTask(const FTaskIndex& Index)
	{
		FAllExecTaskDataBlock* CurTask = this->GetTaskBlock(Index.TaskType);
		if (CurTask)
		{
			return CurTask->AllTaskData.Contains(Index.ID);
		}

		return false;
	}

	//检查有效ID，如果传入的ID大于当前任务执行ID，表示当前任务未执行
	bool CheckValidTaskID(const FTaskIndex& Index)
	{
		FAllExecTaskDataBlock* CurTask = this->GetTaskBlock(Index.TaskType);
		if (CurTask)
		{
			return (Index.ID > CurTask->TaskID);
		}

		return false;
	}
	//获取任务
	FAllExecTaskDataBlock* GetTaskBlock(const EGameTaskType& TaskType) {
		return this->CurTasks.Find(TaskType);
	}

	//完成任务
	void FinishTask(const FTaskIndex& Index);


	//移除任务
	void RemoveTaskBlock(const EGameTaskType& TaskType)
	{
		FAllExecTaskDataBlock* RemoveExecTaskBlock = this->CurTasks.Find(TaskType);
		if (RemoveExecTaskBlock)
		{
			//减少任务数量
			this->TaskCount -= RemoveExecTaskBlock->AllTaskData.Num();
			this->CurTasks.Remove(TaskType);
		}
	}

	//获取任务数量
	int32 GetTaskCount()
	{
		return this->TaskCount;
	}

	//清理任务
	void Clear()
	{
		this->CurTasks.Empty();
		this->TaskCount = 0;
	}
};

//任务完成奖励结构
USTRUCT(BlueprintType)
struct FFinishTask {
	GENERATED_USTRUCT_BODY()
public:
	//任务ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FTaskBase> TaskID;
};

//任务完成次数【当前玩家账号已经完成的并且记录的任务】
USTRUCT(BlueprintType)
struct FLineTaskProgress {
	GENERATED_USTRUCT_BODY()
public:
	//全部已经完成的任务
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EGameTaskType, FFinishTask> AllFinishTask;
	//全部已经完成但是未领取奖励的任务
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EGameTaskType, FFinishTask> AllFinishTaskNotClaimed;
	//当前完成任务的数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FinishTaskCount = 0;
	//当前完成任务未领取的数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FinishTaskNotClaimedCount = 0;
public:
	//将未领取的任务，移动到任务完成的位置
	void MoveFinishTaskNotClaimed(FTaskIndex Index)
	{
		//将未领取的任务，移动到任务完成的集合中
		FFinishTask* NotClaimedFinishTask = AllFinishTaskNotClaimed.Find(Index.TaskType);
		if (NotClaimedFinishTask)
		{
			//查询对应的任务
			FTaskBase* TaskBase = NotClaimedFinishTask->TaskID.Find(Index.ID);
			if (TaskBase)
			{
				//添加到完成任务的集合中
				this->AddFinishTask(Index, *TaskBase);
				//移除当前集合
				NotClaimedFinishTask->TaskID.Remove(Index.ID);
				this->FinishTaskNotClaimedCount--;
			}
		}
	}
	//新增完成的任务
	void AddFinishTaskNotClaimed(FTaskIndex Index, const FTaskBase& BaseData)
	{
		FTaskBase CopyTaskBase = BaseData;
		CopyTaskBase.TaskState = 2u;
		this->AddFinishTask(this->AllFinishTaskNotClaimed, Index, CopyTaskBase);
		this->FinishTaskNotClaimedCount++;
	}
	void AddFinishTask(FTaskIndex Index, const FTaskBase& BaseData)
	{
		FTaskBase CopyTaskBase = BaseData;
		CopyTaskBase.TaskState = 3u;
		this->AddFinishTask(this->AllFinishTask, Index, CopyTaskBase);
		this->FinishTaskCount++;
	}
	//获取完成的任务
	void GetFinishTask(TArray<FTaskBase>& OutFinishTask) {
		this->GetFinishTask(this->AllFinishTask, OutFinishTask);
	}
	void GetFinishTaskNotClaimed(TArray<FTaskBase>& OutFinishTask) {
		this->GetFinishTask(this->AllFinishTaskNotClaimed, OutFinishTask);
	}
	//获取当前类型全部完成的任务
	void GetAllFinishTask(const EGameTaskType& TaskType, FAllTaskDataBlock& OutFinishTask);

	bool FindFinishTask(FTaskIndex Index)
	{
		return this->FindTask(this->AllFinishTask, Index);
	}
	bool FindFinishTaskNotClaimed(FTaskIndex Index)
	{
		return this->FindTask(this->AllFinishTaskNotClaimed, Index);
	}
	//移除任务块
	void RemoveFinishTask(EGameTaskType GameTaskType)
	{
		FFinishTask* CurTask = this->AllFinishTask.Find(GameTaskType);
		if (CurTask)
		{
			this->FinishTaskCount -= CurTask->TaskID.Num();
		}
		FFinishTask* CurTaskNotClaimed = this->AllFinishTaskNotClaimed.Find(GameTaskType);
		if (CurTaskNotClaimed)
		{
			this->FinishTaskNotClaimedCount -= CurTaskNotClaimed->TaskID.Num();
		}

		this->AllFinishTaskNotClaimed.Remove(GameTaskType);
		this->AllFinishTask.Remove(GameTaskType);
	}
	//获取完成的任务数量
	const int32& GetFinishTaskCount() const {
		return this->FinishTaskCount;
	}
	const int32& GetFinishTaskNotClaimedCount() const {
		return this->FinishTaskNotClaimedCount;
	}
	//获取当前分类的任务数量【完成】【未领取】
	int32 GetFinishTaskCountByTaskType(const EGameTaskType& Type)
	{
		int32 DyanmicCount = 0;

		FFinishTask* FinishTask = this->AllFinishTask.Find(Type);
		if (FinishTask)
		{
			DyanmicCount += FinishTask->TaskID.Num();
		}

		FFinishTask* FinishTaskNotClaimed = this->AllFinishTaskNotClaimed.Find(Type);
		if (FinishTaskNotClaimed)
		{
			DyanmicCount += FinishTaskNotClaimed->TaskID.Num();
		}

		return DyanmicCount;
	}
private:
	bool FindTask(TMap<EGameTaskType, FFinishTask>& Tasks, FTaskIndex Index) {
		FFinishTask* CurTask = Tasks.Find(Index.TaskType);
		if (CurTask)
		{
			return CurTask->TaskID.Contains(Index.ID);
		}
		return false;
	}

	void GetFinishTask(
		const TMap<EGameTaskType, FFinishTask>& Tasks,
		TArray<FTaskBase>& OutFinishTask
	)
	{
		for (auto PP = Tasks.CreateConstIterator(); PP; ++PP)
		{
			for (auto AA = PP.Value().TaskID.CreateConstIterator(); AA; ++AA)
			{
				OutFinishTask.Emplace(AA.Value());
			}
		}
	}

	void AddFinishTask(TMap<EGameTaskType, FFinishTask>& CurFinishTask, FTaskIndex Index, const FTaskBase& BaseData)
	{
		FFinishTask* CurTask = CurFinishTask.Find(Index.TaskType);
		if (CurTask)
		{
			CurTask->TaskID.Emplace(Index.ID, BaseData);
		}
		else {
			FFinishTask NewFinishTask;
			NewFinishTask.TaskID.Emplace(Index.ID, BaseData);
			CurFinishTask.Emplace(Index.TaskType, NewFinishTask);
		}
	}
};

//任务完成后执行的对象
UCLASS(Blueprintable, BlueprintType)
class FVM_API UTaskFuncObjectExe : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnExe();
public:
	virtual	void OnInit();
};

//任务结构基础
UCLASS()
class FVM_API UTaskStructBase : public UObject
{
	GENERATED_BODY()
public:
	template<class T = FTaskBase>
	static UTaskStructBase* MakeTaskObject(const T& Data)
	{
		UTaskStructBase* Cur = NewObject<UTaskStructBase>();
		Cur->Data = Data;
		Cur->TaskType = Data.GetTaskType();
		return Cur;
	}
public:
	//获取任务名称
	const FString& GetTaskName() const;
	//获取任务ID
	const int32& GetTaskID() const;
	//获取任务类型
	const EGameTaskType& GetTaskType() const;
	//获取数据
	const FTaskBase& GetData() const;
private:
	UPROPERTY()
	FTaskBase Data;
	UPROPERTY()
	EGameTaskType TaskType = EGameTaskType::MainTask;
};
