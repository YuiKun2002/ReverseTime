// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/GameLogSubsystem.h"
#include "UI/WidgetBase.h"
#include "Data/Instructions/PlayerInstructions.h"
#include "Data/Instructions/GameInstructions.h"
#include "Data/Instructions/GiftInstructions.h"
#include "Data/Instructions/SymbolInstructions.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/Tools/GameSystemFunction.h"

/*
//聊天窗口线程-锁
static FCriticalSection GameLogSubsystemRunnableLock;

bool FGameLogSubsystemRunnable::InitRunnable(UObject* Instructions, TArray<FString>&& Ins, bool bTip) {
	//判断对象是否有效
	if (IsValid(Instructions))
	{
		this->bStop = false;

		this->bTipState = bTip;
		this->CurIns = MoveTempIfPossible(Ins);
		this->CurInstructions = Instructions;;
		this->CurInstructions->AddToRoot();

		UE_LOG(LogTemp, Error, TEXT("线程[%p]初始化成功"), this);

		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("线程[%p]初始化失败"), this);
	return false;
}

FGameLogSubsystemRunnable::~FGameLogSubsystemRunnable() {
	//销毁线程对象
	UE_LOG(LogTemp, Error, TEXT("线程[%p]被释放"), this);
}

void FGameLogSubsystemRunnable::Create() {

	//创建新的线程
	FString ThreadName = UGameSystemFunction::GetObjectName(CurInstructions) + TEXT("GameLogSubsystemRunnable");
	this->NewThread = MakeShareable(FRunnableThread::Create(this,
		*ThreadName, TPri_Normal));

	UE_LOG(LogTemp, Error, TEXT("线程[%p]，名称：%s，开启"), this, *ThreadName);

}

bool FGameLogSubsystemRunnable::GetbState() {
	return this->bStop;
}

uint32 FGameLogSubsystemRunnable::Run() {

	//执行指令
	FScopeLock Lock(&GameLogSubsystemRunnableLock);

	if (Cast<ILogInstructionsInterface>(this->CurInstructions)->ReadInstruction(MoveTempIfPossible(this->CurIns)))
	{
	}

	this->CurInstructions->RemoveFromRoot();
	this->CurInstructions = nullptr;

	UE_LOG(LogTemp, Error, TEXT("线程[%p]，Run"), this);

	return 0U;
}

void FGameLogSubsystemRunnable::Exit() {
	UE_LOG(LogTemp, Error, TEXT("线程[%p]，Exit"), this);
	this->NewThread->Kill(true);
}

void FGameLogSubsystemRunnable::Stop() {
	this->bStop = true;
	UE_LOG(LogTemp, Error, TEXT("线程[%p]，Stop"), this);
	this->NewThread.Reset();
	this->NewThread = nullptr;
}

bool GameLogSubsysRunnableManager::Create(UObject* Instructions, TArray<FString>&& Ins, bool bTip) {
	for (auto& Run : this->Runnables)
	{
		//当前线程为空的状态
		if (Run->GetbState() == true)
		{
			if (Run->InitRunnable(Instructions, MoveTempIfPossible(Ins), bTip))
			{
				Run->Create();
				return true;
			}
			else {
				return false;
			}
		}
	}

	//创建新的线程
	TSharedPtr<FGameLogSubsystemRunnable> RunObj = MakeShareable(new FGameLogSubsystemRunnable());
	if (RunObj->InitRunnable(Instructions, MoveTempIfPossible(Ins), bTip))
	{
		RunObj->Create();
		this->Runnables.Emplace(MoveTempIfPossible(RunObj));
		return true;
	}
	this->Runnables.Emplace(MoveTempIfPossible(RunObj));
	return false;
}

void GameLogSubsysRunnableManager::Unload() {
	for (auto& Run : this->Runnables)
	{
		Run.Reset();
		Run = nullptr;
	}

	this->Runnables.Empty();
}
*/

void UGameLogSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

	//this->RunnableManager = MakeShareable(new GameLogSubsysRunnableManager());

	this->AddPrivate(RICHTEXT(TEXT("游戏测试版本信息面板：")));
}

void UGameLogSubsystem::Deinitialize()
{
	/*if (this->RunnableManager.IsValid())
	{
		this->RunnableManager->Unload();
	}

	this->RunnableManager.Reset();*/
}

void UGameLogSubsystem::AddCurrent(FString Content)
{
	this->AddCur(MoveTempIfPossible(Content));
}

void UGameLogSubsystem::AddCur(FString&& Con) {
	if (this->CurrentListConetnt.Num() > 100)
	{
		TArray<FString> Temp;
		for (int32 i = this->CurrentListConetnt.Num() - 6; i < this->CurrentListConetnt.Num(); i++)
		{
			Temp.Emplace(MoveTempIfPossible(this->CurrentListConetnt[i]));
		}
		this->CurrentListConetnt.Empty();
		this->CurrentListConetnt = MoveTempIfPossible(Temp);
	}

	this->CurrentListConetnt.Emplace(MoveTempIfPossible(Con));
}

void UGameLogSubsystem::GetCurrent(TArray<FString>& Contents)
{
	Contents = this->CurrentListConetnt;
}

int32 UGameLogSubsystem::GetCurrentCount()
{
	return this->CurrentListConetnt.Num();
}

void UGameLogSubsystem::AddPrivate(const FString& Content)
{
	if (this->PrivateListConetnt.Num() > 6000)
	{
		this->PrivateListConetnt.Empty();
	}

	this->PrivateListConetnt.Emplace(Content);
}

void UGameLogSubsystem::GetPrivate(TArray<FString>& Contents)
{
	Contents = this->PrivateListConetnt;
}

int32 UGameLogSubsystem::GetPrivateCount()
{
	return this->PrivateListConetnt.Num();
}

bool UGameLogSubsystem::ExeInstructions(const FString& Content)
{

	UInstructionDataAssetCache* Cache = GET_CACHE(Instruction);
	if (!IsValid(Cache))
	{
		return false;
	}

	//源指令串
	FString CurCo = Content.ToLower();

	//指令分割
	FString SpSym(TEXT("/"));

	bool bExeResult = false;

	//得到分割后的指令串
	TArray<FString> InsArray;
	CurCo.ParseIntoArray(InsArray, TEXT(","), false);
	for (FString& Str : InsArray)
	{
		//解析单个指令
		FString Le, Ri;
		if (Str.Split(SpSym, &Le, &Ri))
		{
			//拆分指令
			TArray<FString> Ins;
			Ri.ParseIntoArray(Ins, TEXT(" "), false);

			//如果没有信息则退出
			if (Ins.Num() == 0)
			{
				this->AddCurrent(
					RICHTEXT_LABLE(TEXT("请输入 -h 或者 -help 来查看所有指令"), TEXT("c"))
				);

				continue;
			}

			//执行帮助命令
			if (UGameLogSubsystem::AddHelpInstructionAndPrint(
				Ins[0],
				GET_DEF_NAME(Instruction, All)
			))
			{
				continue;
			}

			if (Ins[0].ToLower().Equals(TEXT("clear")))
			{
				this->CurrentListConetnt.Empty();
				this->PrivateListConetnt.Empty();
				continue;
			}

			//打印指令
			if (Ins[0].ToLower().Equals(TEXT("print")))
			{
				//指令符合引用替换
				for (FString& CurIn : Ins)
				{
					Cache->GetSymbolType(CurIn);
				}

				Ins.RemoveAt(0);
				FString LocalStr;
				for (FString& CurIn : Ins)
				{
					LocalStr += TEXT(" ");
					LocalStr += MoveTempIfPossible(CurIn);
				}

				this->AddCurrent(
					RICHTEXT_LABLE(LocalStr, TEXT("c"))
				);

				continue;
			}

			//指令对象
			UObject* CurInstructions = nullptr;

			//FString Head = 邮件
			//GenInstruction<UMailInstructions>(Ins[0], TEXT("mail"), CurInstructions);

			GenInstruction<USymbolInstructions>(Ins[0], TEXT("sym"), CurInstructions);			
			GenInstruction<UPlayerInstructions>(Ins[0], TEXT("player"), CurInstructions);
			GenInstruction<UGameInstructions>(Ins[0], TEXT("game"), CurInstructions);
			GenInstruction<UGiftInstructions>(Ins[0], TEXT("gift"), CurInstructions);

			//移除头部信息
			Ins.RemoveAt(0);

			//指令符合引用替换
			for (FString& CurIn : Ins)
			{
				Cache->GetSymbolType(CurIn);
			}

			if (
				CurInstructions &&
				Cast<ILogInstructionsInterface>(CurInstructions)->ReadInstruction(MoveTempIfPossible(Ins)
				))
			{
				bExeResult = true;
			}

		}
	}

	return bExeResult;
}

FString UGameLogSubsystem::GetInstructions(FName InstructionName)
{
	TArray<FInstructionsDataTable> AllList;

	GET_CACHE(Instruction)->GetIns(InstructionName, AllList);

	FString CurText = TEXT("<t>指令列表：</>\r\n");
	for (const FInstructionsDataTable& CurRow : AllList)
	{
		FString CurDes = CurRow.Des.Replace(TEXT(";"), TEXT("</>\r\n<t>"));

		FString A = CurDes.Replace(TEXT("【"), TEXT("</><g>【</><t>"));
		FString B = A.Replace(TEXT("】"), TEXT("</><g>】</><t>"));
		FString C = B.Replace(TEXT("param"), TEXT("</><c>param</><t>"));

		CurText += RICHTEXT_LABLE(TEXT("---【"), TEXT("g")) +
			RICHTEXT_LABLE(CurRow.InstructionName, TEXT("g")) +
			RICHTEXT_LABLE(TEXT("】---"),
				TEXT("g")) + FString(TEXT("\r\n")) +
			RICHTEXT(C) + FString(TEXT("\r\n"));
	}

	return CurText;
}

bool UGameLogSubsystem::AddHelpInstruction(const FString& Instruction)
{
	if (Instruction.ToLower().Equals(TEXT("-help")) || Instruction.ToLower().Equals(TEXT("-h")))
	{
		return true;
	}

	return false;
}

UGameLogSubsystem* UGameLogSubsystem::GetGameLogSubsystemStatic()
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return UFVMGameInstance::GetFVMGameInstance()->GetSubsystem<UGameLogSubsystem>();
	}

	return nullptr;
}

bool UGameLogSubsystem::AddHelpInstructionAndPrint(const FString& Instruction, FName InstructionName)
{
	if (UGameLogSubsystem::GetGameLogSubsystemStatic()->AddHelpInstruction(Instruction))
	{
		UGameLogSubsystem::GetGameLogSubsystemStatic()->AddCurrent(
			UGameLogSubsystem::GetGameLogSubsystemStatic()->GetInstructions(InstructionName)
		);

		return true;
	}

	return false;
}

bool UGameLogSubsystem::InstructionTest(const FString& Instruction, const FString& TestInstruction)
{
	return Instruction.ToLower().Equals(TestInstruction);
}

void UGameLogSubsystem::AddPlayerGetEquipmentLog(const FString& ItemName, int32 Count, int32 Grade)
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		if (Grade == 0)
		{
			UGameLogSubsystem::GetGameLogSubsystemStatic()->AddCurrent(
				RICHTEXT(TEXT("【公告】说：玩家【")) + RICHTEXT_LABLE(UFVMGameInstance::GetFVMGameInstance()->GetPlayerStructManager()->M_PlayerName,
					TEXT("player")) + RICHTEXT(TEXT("】获得道具【")) + RICHTEXT_LABLE(ItemName, TEXT("c")) + RICHTEXT(TEXT("】")) +
				RICHTEXT_LABLE(FString(TEXT("*") + FString::FromInt(Count)), TEXT("g"))
			);
		}
		else {
			UGameLogSubsystem::GetGameLogSubsystemStatic()->AddCurrent(
				RICHTEXT(TEXT("【公告】说：玩家【")) + RICHTEXT_LABLE(UFVMGameInstance::GetFVMGameInstance()->GetPlayerStructManager()->M_PlayerName,
					TEXT("player")) + RICHTEXT(TEXT("】获得")) +
				RICHTEXT_LABLE(FString(FString::FromInt(Grade) + TEXT("级")), TEXT("g")) +
				RICHTEXT(TEXT("道具【")) + RICHTEXT_LABLE(ItemName, TEXT("c")) + RICHTEXT(TEXT("】")) +
				RICHTEXT_LABLE(FString(TEXT("*") + FString::FromInt(Count)), TEXT("g"))
			);
		}
	}
}

void UGameLogSubsystem::AddPlayerGetCardLog(const FString& ItemName, int32 Grade /*= 0*/)
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		UGameLogSubsystem::GetGameLogSubsystemStatic()->AddCurrent(
			RICHTEXT(TEXT("【公告】说：玩家【")) + RICHTEXT_LABLE(UFVMGameInstance::GetFVMGameInstance()->GetPlayerStructManager()->M_PlayerName,
				TEXT("player")) + RICHTEXT(TEXT("】获得一张")) +
			RICHTEXT_LABLE(FString(FString::FromInt(Grade) + TEXT("星")), TEXT("g")) +
			RICHTEXT(TEXT("[")) + RICHTEXT_LABLE(ItemName, TEXT("c")) + RICHTEXT(TEXT("]卡片"))
		);
	}
}

void UGameLogSubsystem::AddPlayerGetMaterialLog(const FString& ItemName, int32 Count /*= 0*/)
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		UGameLogSubsystem::GetGameLogSubsystemStatic()->AddCurrent(
			RICHTEXT(TEXT("【公告】说：玩家【")) + RICHTEXT_LABLE(UFVMGameInstance::GetFVMGameInstance()->GetPlayerStructManager()->M_PlayerName,
				TEXT("player")) + RICHTEXT(TEXT("】获得材料【")) + RICHTEXT_LABLE(ItemName, TEXT("c")) + RICHTEXT(TEXT("】")) +
			RICHTEXT_LABLE(FString(TEXT("*") + FString::FromInt(Count)), TEXT("g"))
		);
	}
}

bool UMailInstructions::ReadInstruction(const TArray<FString>& Instruction)
{
	return false;
}

void UInstructionDataAssetCache::InitCache_Implementation() {

	//初始化所有的指令数据表格
	UGameDataAssetObject* InstructionObject = UGameDataSubsystem::GetGameDataSubsystemStatic()
		->GetAsset()->GetDataByName_Imp(GET_MOVE_CATEGORYNAME(Instruction));
	if (IsValid(InstructionObject))
	{
		for (auto& Data : InstructionObject->Datas)
		{
			InstructionObject->GetDataByName(Data.Key);
		}
	}
}

void UInstructionDataAssetCache::GetIns(FName InstructionName, TArray<FInstructionsDataTable>& OutIns) {
	UGameDataAssetObject* InstructionObject = UGameDataSubsystem::GetGameDataSubsystemStatic()
		->GetAsset()->GetDataByName_Imp(GET_MOVE_CATEGORYNAME(Instruction));
	if (IsValid(InstructionObject))
	{
		UDataTable* Tab = InstructionObject->GetDataByName(InstructionName);
		if (IsValid(Tab)) {
			TArray<FInstructionsDataTable*> L;
			Tab->GetAllRows(TEXT("List"), L);
			for (auto Content : L)
			{
				OutIns.Emplace(*Content);
			}
		}
	}
}

void UInstructionDataAssetCache::InitSymData() {
	if (!IsValid(this->SymbolDataTable))
	{
		UGameDataAssetObject* InstructionObject = UGameDataSubsystem::GetGameDataSubsystemStatic()
			->GetAsset()->GetDataByName_Imp(GET_MOVE_CATEGORYNAME(Instruction));
		if (IsValid(InstructionObject))
		{
			this->SymbolDataTable = InstructionObject->GetDataByName(GET_MOVE_NAME(Instruction, SymbolRef));
			if (!IsValid(this->SymbolDataTable))
			{
				return;
			}
		}
	}
}

void UInstructionDataAssetCache::GetSymbolType(FString& Ins) {
	this->InitSymData();
	if (this->Syms.Num() == 0)
	{
		this->Syms = MoveTempIfPossible(this->SymbolDataTable->GetRowNames());
	}

	FString CurIns = Ins;


	for (FName& CurSym : this->Syms)
	{
		//如果不满足
		if (CurIns.Contains(CurSym.ToString()))
		{
			FInstructionSymbolRefDataTable* SymType = this->SymbolDataTable->
				FindRow<FInstructionSymbolRefDataTable>(CurSym, TEXT("SymbolRef_T"));
			if (SymType)
			{
				if (SymType->InstructionName.Equals(TEXT("int")))
				{
					USymbolRefReturnValueInt* IntObj = Cast<USymbolRefReturnValueInt>(
						SymType->SymbolRefReturnClass.GetDefaultObject());
					if (IsValid(IntObj))
					{
						int32 Value = IntObj->Get();
						CurIns = MoveTempIfPossible(CurIns.Replace(*CurSym.ToString(),
							*FString::FromInt(Value)));
					}
				}
				else if (SymType->InstructionName.Equals(TEXT("str")))
				{
					USymbolRefReturnValueStr* StrObj = Cast<USymbolRefReturnValueStr>(
						SymType->SymbolRefReturnClass.GetDefaultObject());
					if (IsValid(StrObj))
					{
						FString Target = TEXT("#") + StrObj->Get() + TEXT("#");
						CurIns = MoveTempIfPossible(CurIns.Replace(*CurSym.ToString(),
							*Target));
					}
				}
				else {
					continue;
				}
			}
		}
	}

	Ins = MoveTempIfPossible(CurIns);
}

void UInstructionDataAssetCache::GetSymbolNameAndDes(TArray<FName>& OutName, TArray<FString>& OutDes) {
	this->InitSymData();
	if (this->Syms.Num())
	{
		TArray<FInstructionSymbolRefDataTable*> Lists;
		this->SymbolDataTable->GetAllRows<FInstructionSymbolRefDataTable>(TEXT("SymList"), Lists);
		OutDes.Empty(Lists.Num());
		for (auto& Li : Lists)
		{
			OutDes.Add(FString(TEXT("(") + Li->InstructionName + TEXT("类型)，") + Li->Des));
		}

		OutName = this->Syms;
	}
}

bool ULogInstructionSystemBase::ReadInstruction(const TArray<FString>& Instruction)
{
	return false;
}

UGameLogSubsystem* ULogInstructionSystemBase::GetSystem()
{
	return UGameLogSubsystem::GetGameLogSubsystemStatic();
}

void ULogInstructionSystemBase::SetInstructions(FName InstructionName)
{
	this->CurData.Empty();

	GET_CACHE(Instruction)->GetIns(InstructionName, this->CurData);
}

FString ULogInstructionSystemBase::GetInstructionDes(const FString& Instruction)
{
	for (const FInstructionsDataTable& Cur : this->CurData)
	{
		if (Cur.InstructionName.Equals(Instruction))
		{
			return Cur.Des;
		}
	}

	return TEXT("");
}

bool ULogInstructionSystemBase::PrintHelpInstruction(const FString& HeadIns, const TArray<FString>& Instruction)
{
	int32 CurIndex = Instruction.Find(TEXT("-h"));
	if (CurIndex >= 0)
	{
		FString CurIns = HeadIns + TEXT(" ");
		for (int32 i = 0; i <= CurIndex; i++)
		{
			if (i == CurIndex)
			{
				CurIns += Instruction[i];
			}
			else {
				CurIns += Instruction[i] + TEXT(" ");
			}
		}

		FString TargetIns = this->GetInstructionDes(CurIns);
		if (TargetIns.Equals(""))
		{
			return false;
		}
		else {

			FString Cur = TargetIns.Replace(TEXT(";"), TEXT("</>\r\n<t>"));
			FString A = Cur.Replace(TEXT("【"), TEXT("</><g>【</><t>"));
			FString B = A.Replace(TEXT("】"), TEXT("</><g>】</><t>"));
			FString C = B.Replace(TEXT("param"), TEXT("</><c>param</><t>"));
			this->GetSystem()->AddCurrent(RICHTEXT(C));

			return true;
		}
	}

	return false;
}

void ULogInstructionSystemBase::PrintErrorIns(const FString& InsHead, const FString& ErrorIns) {
	UGameSystemFunction::AddGameLog(GAMEINS()->GetWorld(),
		RICHTEXT_LABLE(TEXT("/") + InsHead + TEXT(" "), RichText_MsgLog::G) +
		RICHTEXT_LABLE(ErrorIns + TEXT(" 指令出现错误"), RichText_MsgLog::G) + RICHLINE +
		RICHTEXT_LABLE(TEXT("/") + InsHead + TEXT(" -h 查看指令"), RichText_MsgLog::C)
	);
}

void ULogInstructionSystemBase::DepthReadStrReplace(
	const FString& SourceIns,
	TArray<FString>& OutFinishIns,
	FString MatchStr,
	FString ReplacePtr,
	int32 MinDepth,
	int32 MaxDepth) {


	// 匹配模式
	// 1.       忽略()内的内容：例如：  ( 1 + 1 )  匹配之后  ( 1 + 1 )
	// 2.       根据深度(1)替换 { { }+{ { } }+ } 内的内容
	//			例如：{ { 321+321 }+{ { 123+123 } }+123+123 } 匹配之后  { { 321+321 },{ { 123+123 } },123,123 }

	//深度判断
	if (MinDepth > MaxDepth)
	{
		MinDepth = MaxDepth;
	}

	//转换字符串
	SourceIns.ParseIntoArray(OutFinishIns, *MatchStr, false);

	//当前位置索引
	int32 Index = 0;
	//当前数量
	int32 Count = OutFinishIns.Num() - 1;
	//当前深度
	int32 Depth = 0;
	//不能被替换的深度
	int32 ConstDepth = 0;

	while (Index < Count)
	{

		FString& Str = OutFinishIns[Index];

		if (Str.Contains(TEXT("{")))
		{
			//增加深度
			Depth++;

			//如果不匹配则不修改
			if (Depth > MaxDepth)
			{
				Str += MatchStr;
			}

			if (Str.Contains(TEXT("(")))
			{
				ConstDepth++;

				Str.Append(MatchStr);
			}
			else if (Str.Contains(TEXT(")")))
			{
				ConstDepth--;

				if (Index - 1 > 0)
				{
					(OutFinishIns[Index - 1]) += Str;
					OutFinishIns.RemoveAt(Index);
					Count--;
					Index--;
				}
			}

			Index++;
			continue;
		}
		else if (Str.Contains(TEXT("}")))
		{
			//如果不匹配则不修改
			if (Depth > MaxDepth)
			{
				if (Index - 1 > 0)
				{
					(OutFinishIns[Index - 1]) += Str;
					OutFinishIns.RemoveAt(Index);
					Count--;
					Index--;
				}
			}

			//深度减少
			Depth--;

			//如果当前深度大于等于最小深度并且当前深度小于等于最大深度，则可以替换 [0 min,1 max]
			if (Depth >= MinDepth && Depth <= MaxDepth)
			{
				OutFinishIns.EmplaceAt(Index + 1, ReplacePtr);
				Count++;
				Index++;
			}

			Index++;
			continue;
		}
		else {

			if (ConstDepth > 0)
			{
				if (Str.Contains(TEXT("(")))
				{
					ConstDepth++;

					Str.Append(MatchStr);
				}
				else if (Str.Contains(TEXT(")")))
				{
					ConstDepth--;

					if (Index - 1 > 0)
					{
						(OutFinishIns[Index - 1]) += Str;
						OutFinishIns.RemoveAt(Index);
						Count--;
						Index--;
					}
				}
				else {
					Str.Append(MatchStr);
				}
			}
			else {
				//如果当前深度大于等于最小深度并且当前深度小于等于最大深度，则可以替换 [0 min,1 max]
				if (Depth >= MinDepth && Depth <= MaxDepth)
				{
					OutFinishIns.EmplaceAt(Index + 1, ReplacePtr);
					Count++;
					Index++;
				}
				else {
					if (Index - 1 > 0)
					{
						(OutFinishIns[Index - 1]) += Str + MatchStr;
						OutFinishIns.RemoveAt(Index);
						Count--;
						Index--;
					}
					else {
						Str += MatchStr;
					}
				}
			}
		}
		Index++;
	}
}

bool ULogInstructionSystemBase::ExecuteInsHead(const TArray<FString>& Instruction, const FString& TargetIns, TFunction<bool(const TArray<FString>&)> ExeFunc)
{
	if (Instruction.Num() > 0)
	{
		//发送指令
		if (this->GetSystem()->InstructionTest(Instruction[0], TargetIns))
		{
			TArray<FString> CurIns = Instruction;
			CurIns.RemoveAt(0);
			return ExeFunc(CurIns);
		}
	}

	return false;
}
