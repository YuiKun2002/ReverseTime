// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/GameDataSubsystem.h"
#include "Engine/Classes/Engine/DataTable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameLogSubsystem.generated.h"


//指令显示列表，提示玩家
USTRUCT(BlueprintType)
struct FInstructionsDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString InstructionName = TEXT("");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Des = TEXT("");
};

/*
//聊天窗口线程
class FGameLogSubsystemRunnable : public FRunnable {

	friend class UGameLogSubsystem;
	friend class GameLogSubsysRunnableManager;

public:

	~FGameLogSubsystemRunnable();

	//初始化
	bool InitRunnable(UObject* Instructions, TArray<FString>&& Ins, bool bTip);

	//创建
	void Create();

	//获取线程状态
	bool GetbState();

public:

	virtual uint32 Run() override;

	virtual void Exit() override;

	virtual void Stop() override;

private:

	FGameLogSubsystemRunnable() {};

	UObject* CurInstructions = nullptr;

	bool bTipState = false;

	bool bStop = false;

	TArray<FString> CurIns;

	TSharedPtr<FRunnableThread> NewThread;
};
//线程管理
class GameLogSubsysRunnableManager {

	friend class FGameLogSubsystemRunnable;
public:

	bool Create(UObject* Instructions, TArray<FString>&& Ins, bool bTip);

	void Unload();
private:
	TArray<TSharedPtr<FGameLogSubsystemRunnable>> Runnables;
};
*/

//指令数据资产缓存
UCLASS()
class FVM_API UInstructionDataAssetCache : public UGameDataAssetCache
{
	GENERATED_BODY()

public:

	//初始化缓存
	virtual void InitCache_Implementation() override;

	//获取指令
	UFUNCTION(BlueprintPure)
	void GetIns(FName InstructionName, TArray<FInstructionsDataTable>& OutIns);
	//获取符号类型
	UFUNCTION(BlueprintPure)
	void GetSymbolType(FString& Ins);
	//获取符号的名称和描述
	UFUNCTION(BlueprintPure)
	void GetSymbolNameAndDes(TArray<FName>& OutName, TArray<FString>& OutDes);
private:
	//初始化符号数据表
	void InitSymData();
private:
	UPROPERTY()
	class UDataTable* SymbolDataTable = nullptr;
	UPROPERTY()
	TArray<FName> Syms;
};

/**
 * 聊天窗口子系统
 */
UCLASS()
class FVM_API UGameLogSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class FGameLogSubsystemRunnable;

public:
	//初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//取消初始化
	virtual void Deinitialize() override;
public:
	UFUNCTION(BlueprintCallable)
	void AddCurrent(FString Content);
	UFUNCTION(BlueprintCallable)
	void GetCurrent(TArray<FString>& Contents);
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentCount();

	UFUNCTION(BlueprintCallable)
	void AddPrivate(const FString& Content);
	UFUNCTION(BlueprintCallable)
	void GetPrivate(TArray<FString>& Contents);
	UFUNCTION(BlueprintCallable)
	int32 GetPrivateCount();

	//执行指令
	UFUNCTION(BlueprintCallable)
	bool ExeInstructions(const FString& Content);
	//打印指令列表
	UFUNCTION(BlueprintCallable)
	FString GetInstructions(FName InstructionName);
	//添加帮助指令
	UFUNCTION(BlueprintCallable)
	bool AddHelpInstruction(const FString& Instruction);

	//获取子系统
	UFUNCTION(BlueprintCallable)
	static UGameLogSubsystem* GetGameLogSubsystemStatic();
	//添加并且打印指令列表
	UFUNCTION(BlueprintCallable)
	static bool AddHelpInstructionAndPrint(const FString& Instruction, FName InstructionName);
	//指令判断
	UFUNCTION(BlueprintCallable)
	static bool InstructionTest(const FString& Instruction, const FString& TestInstruction);
	//添加一个玩家获得装备的信息
	UFUNCTION(BlueprintCallable)
	static void AddPlayerGetEquipmentLog(const FString& ItemName, int32 Count = 1, int32 Grade = 0);
	//添加一个玩家获得卡片的信息
	UFUNCTION(BlueprintCallable)
	static void AddPlayerGetCardLog(const FString& ItemName, int32 Grade = 0);
	//添加一个玩家获得材料的信息
	UFUNCTION(BlueprintCallable)
	static void AddPlayerGetMaterialLog(const FString& ItemName, int32 Count = 0);
private:
	//添加当前窗口
	void AddCur(FString&& Con);

	template<class T>
	void GenInstruction(const FString& Head, const FString& TargetHead, UObject*& OutInstruction)
	{
		if (IsValid(OutInstruction))
		{
			return;
		}

		OutInstruction = nullptr;

		if (Head.ToLower().Equals(TargetHead))
		{
			OutInstruction = NewObject<T>();
		}
	}
private:
	//当前窗口的信息内容
	UPROPERTY()
	TArray<FString> CurrentListConetnt;
	//私聊窗口的信息内容
	UPROPERTY()
	TArray<FString> PrivateListConetnt;
	//当前的所有线程
	//TSharedPtr<GameLogSubsysRunnableManager> RunnableManager;
};


UINTERFACE(MinimalAPI)
class ULogInstructionsInterface : public UInterface
{
	GENERATED_BODY()
};

class FVM_API ILogInstructionsInterface
{
	GENERATED_BODY()
public:
	virtual	bool ReadInstruction(const TArray<FString>& Instruction) = 0;
};

//日志指令系统基础类
UCLASS()
class FVM_API ULogInstructionSystemBase : public UObject, public ILogInstructionsInterface {

	GENERATED_BODY()
public:
	virtual bool ReadInstruction(const TArray<FString>& Instruction) override;
public:
	//获取日志系统
	UFUNCTION(BlueprintCallable)
	UGameLogSubsystem* GetSystem();
	//设置指令集
	UFUNCTION(BlueprintCallable)
	void SetInstructions(FName InstructionName);
	//获取指令描述
	UFUNCTION(BlueprintCallable)
	FString GetInstructionDes(const FString& Instruction);
	//打印帮助指令
	UFUNCTION(BlueprintCallable)
	bool PrintHelpInstruction(const FString& HeadIns, const TArray<FString>& Instruction);
	//打印错误的指令提示
	UFUNCTION(BlueprintCallable)
	void PrintErrorIns(const FString& InsHead, const FString& ErrorIns);
	/*	
		字符串深度替换
		1.源指令串: /gift if ( 1 ) {  /gift if ( 1 ) { 123  }+/gift if ( 1 ) { 321 } + 1234567  }
		2.返回替换完成的切割指令
		3.需要被匹配的字符串 +
		4.需要替换目标字符串 ,
		5.替换的最小深度 0
		6.替换的最大深度 1
		范围[min,max]
		深度:当指令中存在 { } 时，深度会+或者-。如果当前字符串在深度范围内则触发替换操作
		0表示在最外层{}外面替换
		1表示在第一层{}内替换，2，3，4以此类推
	*/
	void DepthReadStrReplace(
		const FString& SourceIns,
		TArray<FString>& OutFinishIns,
		FString MatchStr,
		FString ReplacePtr,
		int32 MinDepth = 0,
		int32 MaxDepth = 1);
	//执行命令头部
	bool ExecuteInsHead(
		const TArray<FString>& Instruction,
		const FString& TargetIns,
		TFunction<bool(const TArray<FString>&)> ExeFunc
	);
private:
	//当前数据表数据
	UPROPERTY()
	TArray<FInstructionsDataTable> CurData;
};

//邮件指令【测试类型的指令】
UCLASS()
class FVM_API UMailInstructions : public ULogInstructionSystemBase {

	GENERATED_BODY()
public:
	virtual bool ReadInstruction(const TArray<FString>& Instruction) override;
};
