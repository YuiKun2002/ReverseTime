// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Data/GameLogSubsystem.h"
#include "SymbolInstructions.generated.h"

//符号引用列表
USTRUCT(BlueprintType)
struct FInstructionSymbolRefDataTable : public FInstructionsDataTable
{
	GENERATED_USTRUCT_BODY();

public:
	//符号类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class USymbolRefReturnValueBase> SymbolRefReturnClass;
};

//符号引用返回值
UCLASS(Blueprintable)
class FVM_API USymbolRefReturnValueBase : public UObject
{
	GENERATED_BODY()
};
//符号引用返回值int
UCLASS(Blueprintable)
class FVM_API USymbolRefReturnValueInt : public USymbolRefReturnValueBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	int Get();
	virtual int Get_Implementation() { return 0; };
};

//符号引用返回值str
UCLASS(Blueprintable)
class FVM_API USymbolRefReturnValueStr : public USymbolRefReturnValueBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	FString Get();
	virtual FString Get_Implementation() { return FString(); };
};

//类型检测接口
class ICheckType {

public:
	virtual FString Get() = 0;
	virtual bool Return() = 0;
	virtual ~ICheckType() {}
public:
	void SetLastCheckType(TSharedPtr<ICheckType> Type, FString Sym) {
		this->LastType = Type;
		this->CurSym = Sym;
	}
	TSharedPtr<ICheckType> GetLastCheckType() { return LastType; }
	FString GetSym() { return this->CurSym; }
private:
	TSharedPtr<ICheckType> LastType;
	FString CurSym;
};

//字符串逻辑判断
class CheckStr : public ICheckType {

public:
	//添加左边的内容
	bool AddBegin(FString Str);
	bool AddCheck(FString Sym, FString Str);
	bool SymCheck(FString Sym, FString Str);
	virtual FString Get() override {
		return FString(TEXT("str"));
	}
	virtual bool Return() override {
		if (CheckCount > 0)
		{
			return bResult;
		}
		return !BeginStr.IsEmpty();
	}
private:
	FString BeginStr;
	int32 CheckCount = 0;
	bool bResult = false;
};

//整数逻辑判断
class CheckInt : public ICheckType {

public:
	bool AddBegin(int32 Value);
	bool AddCheck(FString Sym, int32 Value);
	bool SymCheck(FString Sym, int32 Value);
	virtual FString Get() override {
		return FString(TEXT("int"));
	}
	virtual bool Return() override {
		if (CheckCount > 0)
		{
			return bResult;
		}
		if (IValue == 0)
			return false;

		return true;
	}
private:
	int32 IValue = 0;
	int32 CheckCount = 0;
	bool bResult = false;
};

/**
 * 符号指令
 */
UCLASS()
class FVM_API USymbolInstructions : public ULogInstructionSystemBase
{
	GENERATED_BODY()
public:
	virtual bool ReadInstruction(const TArray<FString>& Instruction) override;

private:
	// if 指令
	UFUNCTION()
	bool IfIns(const TArray<FString>& Instruction);
	// 读取某个范围的内容
	bool ReadIns(
		int32& Depth,
		const FString& CurIns,
		FString MatchStr,
		FString MatchStr2,
		FString& OutContent,
		bool bUsePlusSplit = true
	);



	// if 条件执行判断
	bool IfExec(const TArray<FString>& Instruction);
	// 检查表达式
	bool CheckIfSym(FString Str);
	// 添加字符串逻辑判断
	bool AddStr(TArray<TSharedPtr<ICheckType>>& Types, FString Sym, FString Str, bool bAddType);
	// 添加整数逻辑判断
	bool AddInt(TArray<TSharedPtr<ICheckType>>& Types, FString Sym, int32 Value, bool bAddType);
private:
	//显示所有的引用符号
	UFUNCTION()
	bool SymbolIns(const TArray<FString>& Instruction);
	//显示所有的基础类型
	UFUNCTION()
	bool TypeIns(const TArray<FString>& Instruction);
	//显示所有的运算符
	UFUNCTION()
	bool BoolIns(const TArray<FString>& Instruction);
};
