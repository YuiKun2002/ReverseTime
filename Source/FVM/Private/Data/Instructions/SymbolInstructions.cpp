// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/Instructions/SymbolInstructions.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"

bool USymbolInstructions::ReadInstruction(const TArray<FString>& Instruction) {

	if (Instruction.Num() == 0)
	{
		return false;
	}

	//设置指令集合描述
	this->SetInstructions(GET_DEF_NAME(Instruction, Symbol));

	//帮助指令
	if (this->GetSystem()->AddHelpInstructionAndPrint(Instruction[0], GET_DEF_NAME(Instruction, Symbol)))
	{
		return false;
	}

	//发送帮助指令
	if (this->PrintHelpInstruction(TEXT("sym"), Instruction))
	{
		return false;
	}


	// /sym if ( (playerlevel)  == 1 ) {  /print 123 , /print 321 }
	//条件语句指令
	if (this->ExecuteInsHead(Instruction, TEXT("if"),
		std::bind(&USymbolInstructions::IfIns, this, std::placeholders::_1)))
	{
		return true;
	}

	//bool
	if (this->ExecuteInsHead(Instruction, TEXT("bool"),
		std::bind(&USymbolInstructions::BoolIns, this, std::placeholders::_1)))
	{
		return true;
	}

	//type
	if (this->ExecuteInsHead(Instruction, TEXT("type"),
		std::bind(&USymbolInstructions::TypeIns, this, std::placeholders::_1)))
	{
		return true;
	}

	//type
	if (this->ExecuteInsHead(Instruction, TEXT("symbol"),
		std::bind(&USymbolInstructions::SymbolIns, this, std::placeholders::_1)))
	{
		return true;
	}

	this->PrintErrorIns(TEXT("sym"), Instruction[0]);
	return false;
}

bool USymbolInstructions::SymbolIns(const TArray<FString>& Instruction) {

	TArray<FName> Names;
	TArray<FString> Des;
	GET_CACHE(Instruction)->GetSymbolNameAndDes(Names, Des);

	int32 Count = Names.Num();
	FString TargetStr;
	for (int32 i = 0; i < Count; i++)
	{
		TargetStr += MoveTempIfPossible(
			RICHTEXT_LABLE(Names[i].ToString(), RichText_MsgLog::C)
		);

		TargetStr += MoveTempIfPossible(
			RICHTEXT_LABLE(FString(TEXT("->") + Des[i]), RichText_MsgLog::INS)
		);

		TargetStr += RICHLINE;
	}

	UGameSystemFunction::AddGameLog(GAMEINS()->GetWorld(), TargetStr);

	return true;
}

bool USymbolInstructions::TypeIns(const TArray<FString>& Instruction) {

	UGameSystemFunction::AddGameLog(GAMEINS()->GetWorld(),
		RICHTEXT_LABLE(TEXT("int类型：表示整数。例子：123"), RichText_MsgLog::INS) + RICHLINE +
		RICHTEXT_LABLE(TEXT("str类型：表示字符串。例子：#123#"), RichText_MsgLog::INS) + RICHLINE
	);

	return true;
}

bool USymbolInstructions::BoolIns(const TArray<FString>& Instruction) {

	UGameSystemFunction::AddGameLog(GAMEINS()->GetWorld(),
		RICHTEXT_LABLE(TEXT("==：表示左右是否相等;例子: if ( 1 == 1 )"), RichText_MsgLog::INS) + RICHLINE +
		RICHTEXT_LABLE(TEXT(">：表示左边大于右边;例子: if ( 1 > 1 )"), RichText_MsgLog::INS) + RICHLINE +
		RICHTEXT_LABLE(TEXT("<：表示左边小于右边;例子: if ( 1 < 1 )"), RichText_MsgLog::INS) + RICHLINE +
		RICHTEXT_LABLE(TEXT(">=：表示左边大于等于右边;例子: if ( 1 >= 1 )"), RichText_MsgLog::INS) + RICHLINE +
		RICHTEXT_LABLE(TEXT("<=：表示左边小于等于右边;例子: if ( 1 <= 1 )"), RichText_MsgLog::INS) + RICHLINE +
		RICHTEXT_LABLE(TEXT("!=：表示左边不等于右边;例子: if ( 1 != 1 )"), RichText_MsgLog::INS) + RICHLINE +
		RICHTEXT_LABLE(TEXT("==：表示左右是否相等;例子: if ( 1 == 1 )"), RichText_MsgLog::INS) + RICHLINE + RICHLINE +

		RICHTEXT_LABLE(TEXT("+：两个数相加;例子: if ( 1 + 1 )"), RichText_MsgLog::INS) + RICHLINE +
		RICHTEXT_LABLE(TEXT("-：两个数相减;例子: if ( 1 - 1 )"), RichText_MsgLog::INS) + RICHLINE +
		RICHTEXT_LABLE(TEXT("*：两个数相乘;例子: if ( 1 * 1 )"), RichText_MsgLog::INS) + RICHLINE +
		RICHTEXT_LABLE(TEXT("/：两个数相除数;例子: if ( 1 / 1 )"), RichText_MsgLog::INS) + RICHLINE + RICHLINE +
		RICHTEXT_LABLE(TEXT("&&：如果两个操作数都 true，则条件为 true;例子: if ( 1 && 1 )"), RichText_MsgLog::INS) +
		RICHLINE + RICHLINE +
		RICHTEXT_LABLE(TEXT("||：如果两个操作数中有任意一个 true，则条件为 true;例子: if ( 0 || 1 )"),
			RichText_MsgLog::INS) + RICHLINE
	);

	return true;
}

bool USymbolInstructions::IfIns(const TArray<FString>& Instruction) {

	//指令索引和指令数量
	int32 InsPtrIndex = 0;
	int32 InsCount = Instruction.Num();

	//读取 (   ) 里面的内容
	int32 BeginDp = 0;
	bool bIfEnd = false;
	bool bIfFinish = false;
	FString BeginContent;
	TArray<FString> IfContent;

	//读取true内容部分
	bool bTrueFinish = false;
	int32 TrueDp = 0;
	FString TrueContent;

	//读取false内容部分
	int32 FalseDp = 0;
	FString FalseContent;

	//执行true部分
	bool bExeTrue = false;
	//执行false部分
	bool bExeFalse = false;

	//解析指令
	while (InsPtrIndex < InsCount)
	{
		if (!bIfEnd)
		{
			if (
				this->ReadIns(BeginDp,
					Instruction[InsPtrIndex],
					MoveTempIfPossible(TEXT("(")),
					MoveTempIfPossible(TEXT(")")),
					BeginContent, false))
			{
				bIfEnd = true;
				bIfFinish = true;
				BeginContent.ParseIntoArray(IfContent, TEXT(" "), true);
			}

			InsPtrIndex++;
			continue;
		}

		//执行条件内容
		if (bIfFinish)
		{

			if (bExeTrue)
			{
				//读取true部分
				if (
					this->ReadIns(TrueDp,
						Instruction[InsPtrIndex],
						MoveTempIfPossible(TEXT("{")),
						MoveTempIfPossible(TEXT("}")),
						TrueContent))
				{
					//执行命令
					UGameSystemFunction::AddGameinstructions(
						GAMEINS()->GetWorld(), TrueContent,false
					);
					return true;
				}

				InsPtrIndex++;
				continue;
			}

			//执行false部分
			if (bExeFalse)
			{
				if (bTrueFinish)
				{
					//读取false部分
					if (this->ReadIns(FalseDp, Instruction[InsPtrIndex], MoveTempIfPossible(TEXT("{")),
						MoveTempIfPossible(TEXT("}")), FalseContent))
					{
						//执行命令
						UGameSystemFunction::AddGameinstructions(
							GAMEINS()->GetWorld(), FalseContent,false
						);
						return true;
					}
				}
				else {
					if (!bTrueFinish)
					{
						//读取true部分
						if (this->ReadIns(TrueDp, Instruction[InsPtrIndex], MoveTempIfPossible(TEXT("{")),
							MoveTempIfPossible(TEXT("}")), TrueContent))
						{
							bTrueFinish = true;
						}
					}
				}

				InsPtrIndex++;
				continue;
			}

			// /gift if ( 1 == 1 ) { 123 } { 321 }
			// /gift if ( #1# == #1# ) { 123 } { 321 }
			// /gift if ( #1# == #2# ) { 123 } { 321 }
			if ((!bExeTrue && !bExeFalse))
			{

				if (this->IfExec(IfContent))
				{
					bExeTrue = true;
					continue;
				}
				else {
					bExeFalse = true;
					continue;
				}

			}
		}

		InsPtrIndex++;
	}

	UGameSystemFunction::AddGameLog(GAMEINS()->GetWorld(),
		RICHTEXT_LABLE(TEXT("if 语法错误"), RichText_MsgLog::G) + RICHLINE +
		RICHTEXT_LABLE(TEXT("if语法：/sym if ( 布尔表达式 ) { 指令+指令+... } { 指令+指令+... }"), RichText_MsgLog::C)
	);
	return false;
}

bool USymbolInstructions::ReadIns(
	int32& Depth,
	const FString& CurIns,
	FString MatchStr,
	FString MatchStr2,
	FString& OutContent,
	bool bUsePlusSplit
) {

	//识别开启位置
	if (CurIns.Equals(MatchStr))
	{
		//bTrueBegin = true;

		// { 123 {  }  {  }  }   {  }

		//如果有深度，则添加
		if (Depth > 0)
		{
			OutContent += CurIns + TEXT(" ");
		}

		//增加深度
		Depth++;
	}
	else if (CurIns.Equals(MatchStr2))
	{
		//减少深度
		Depth--;
		//如果有深度，则添加
		if (Depth > 0)
		{
			OutContent += CurIns + TEXT(" ");
		}
		else {
			//如果内容有+，则进行分割处理
			if (bUsePlusSplit)
			{
				//得到分割后的指令串
				TArray<FString> InsArray;
				//深度替换
				this->DepthReadStrReplace(OutContent, InsArray, TEXT("+"), TEXT(","), 0, 0);

				FString MewIns;
				for (auto& T : InsArray)
				{
					MewIns += MoveTempIfPossible(T);
					MewIns += TEXT(" ");
				}

				OutContent = MoveTempIfPossible(MewIns);
				OutContent.RemoveFromEnd(TEXT(" "));
			}

			return true;
		}
	}
	else {
		if (Depth > 0)
		{
			OutContent += CurIns + TEXT(" ");
		}
	}

	return false;
}

bool USymbolInstructions::IfExec(const TArray<FString>& Instruction) {

	if (Instruction.Num() == 0)
	{
		return false;
	}

	UInstructionDataAssetCache* Cache = GET_CACHE(Instruction);
	if (!IsValid(Cache))
	{
		return false;
	}

	TArray<FString> CurIns = Instruction;
	for (FString& In : CurIns)
	{
		Cache->GetSymbolType(In);
	}

	//指令位置
	int32 Index = 1;
	//最后一个内容是否是符号
	bool bSym = false;
	//当前类型
	TArray<TSharedPtr<ICheckType>> TempClassLists;
	//当前符号
	FString CurSym;

	bool bAdd = false;

	for (auto& In : CurIns)
	{
		//符号
		if (Index % 2 == 0)
		{
			bSym = true;
			if (this->CheckIfSym(In))
			{
				CurSym = In;
				if (CurSym.Equals(TEXT("&&")) || CurSym.Equals(TEXT("||")))
				{
					bAdd = true;
				}
				else {
					bAdd = false;
				}
			}
			else {
				UGameSystemFunction::AddGameLog(GAMEINS()->GetWorld(),
					RICHTEXT_LABLE(TEXT("语法错误！没有当前符号："), RichText_MsgLog::INS)
				);
				UGameSystemFunction::AddGameLog(GAMEINS()->GetWorld(),
					RICHTEXT_LABLE(In, RichText_MsgLog::G)
				);
				//符号错误
				return false;
			}
		}
		else {

			//条件
			bSym = false;

			//执行判断
			if (In.Contains(TEXT("#"))) {
				if (this->AddStr(TempClassLists, CurSym, In, bAdd) == false)
				{
					return false;
				}
			}
			else {
				//数字类型
				int32 Value = FCString::Atoi(*In);
				if (this->AddInt(TempClassLists, CurSym, Value, bAdd) == false)
				{
					return false;
				}
			}
		}

		Index++;
	}

	//如果以符合结尾，返回false,语法错误
	if (bSym)
	{
		//返回结果
		return false;
	}


	//   /gift if ( 1 == 1 && 0 || 1 < 2 && 1 == 2 || 1  ) { 123 } { 321 }
	//   1 == 1 && 0 || 1 < 2 && 1 == 2 || 1 
	//   true && false || false && false || true
	//   false || false && false || true
	//   false && false || true
	//   fasel || true
	//   true



	FString Log;
	if (TempClassLists.Num())
	{
		TSharedPtr<ICheckType> Check;
		//当前循环的结果
		bool bCurResult = false;
		//循环类型
		for (auto& Ptr : TempClassLists)
		{
			if (Ptr.IsValid())
			{
				//链接类型是否有效
				if (Ptr->GetLastCheckType().IsValid())
				{
					Check = Ptr->GetLastCheckType();

					//符号匹配
					FString CurSymTemp = Ptr->GetSym();
					if (CurSymTemp.Equals(TEXT("&&")))
					{
						if (Check->Return() && Ptr->Return())
						{
							bCurResult = true;
							continue;
						}
						else {
							bCurResult = false;
							continue;
						}
					}
					else {
						if (Check->Return() || Ptr->Return())
						{
							bCurResult = true;
							continue;
						}
						else {
							bCurResult = false;
							continue;
						}
					}
				}
				else {
					if (Ptr->Return())
					{
						bCurResult = true;
						continue;
					}
					else {
						bCurResult = false;
						continue;
					}
				}
			}
		}
		return bCurResult;
	}

	return false;
}

bool USymbolInstructions::AddStr(TArray<TSharedPtr<ICheckType>>& Types, FString Sym, FString Str, bool bAddType) {
	if (bAddType)
	{
		//设置连接符号 && ||
		TSharedPtr<ICheckType> CurPtr = MakeShareable(new CheckStr());
		CurPtr->SetLastCheckType(Types[Types.Num() - 1], Sym);

		Types.Emplace(CurPtr);
		if (!((CheckStr*)(Types[Types.Num() - 1].Get()))->AddBegin(Str)) {
			//添加失败
			return false;
		}

		return true;
	}

	if (Types.Num())
	{
		TSharedPtr<ICheckType>& ptr = Types[Types.Num() - 1];
		if (ptr->Get().Equals(TEXT("str")))
		{
			//类型正确，继续,返回匹配结果
			bool re = ((CheckStr*)(ptr.Get()))->AddCheck(Sym, Str);
			if (!re)
			{
				//匹配结果false，直接退出
				return false;
			}
		}
		else {
			//类型错误
			return false;
		}
	}
	else {
		Types.Emplace(MakeShareable(new CheckStr()));
		if (!((CheckStr*)(Types[Types.Num() - 1].Get()))->AddBegin(Str)) {
			//添加失败
			return false;
		}
	}

	return true;
}

bool USymbolInstructions::AddInt(TArray<TSharedPtr<ICheckType>>& Types, FString Sym, int32 Value, bool bAddType) {
	if (bAddType)
	{
		//设置连接符号 && ||
		TSharedPtr<ICheckType> CurPtr = MakeShareable(new CheckInt());
		CurPtr->SetLastCheckType(Types[Types.Num() - 1], Sym);

		Types.Emplace(CurPtr);
		if (!((CheckInt*)(Types[Types.Num() - 1].Get()))->AddBegin(Value)) {
			//添加失败
			return false;
		}

		return true;
	}

	if (Types.Num())
	{
		TSharedPtr<ICheckType>& ptr = Types[Types.Num() - 1];
		if (ptr->Get().Equals(TEXT("int")))
		{
			//类型正确，继续,返回匹配结果
			((CheckInt*)(ptr.Get()))->AddCheck(Sym, Value);
		}
		else {
			//类型错误
			return false;
		}
	}
	else {
		Types.Emplace(MakeShareable(new CheckInt()));
		if (!((CheckInt*)(Types[Types.Num() - 1].Get()))->AddBegin(Value)) {
			//添加失败
			return false;
		}
	}

	return true;
}

bool USymbolInstructions::CheckIfSym(FString Str) {
	if (Str.Equals(TEXT("==")))
	{
		return true;
	}

	if (Str.Equals(TEXT("!=")))
	{
		return true;
	}

	if (Str.Equals(TEXT(">"))) {
		return true;
	}

	if (Str.Equals(TEXT(">="))) {
		return true;
	}

	if (Str.Equals(TEXT("<"))) {
		return true;
	}

	if (Str.Equals(TEXT("<="))) {
		return true;
	}

	if (Str.Equals(TEXT("+"))) {
		return true;
	}

	if (Str.Equals(TEXT("-"))) {
		return true;
	}

	if (Str.Equals(TEXT("*"))) {
		return true;
	}

	if (Str.Equals(TEXT("/"))) {
		return true;
	}

	if (Str.Equals(TEXT("&&"))) {
		return true;
	}

	if (Str.Equals(TEXT("||"))) {
		return true;
	}

	return false;
}

//添加左边的内容
bool CheckStr::AddBegin(FString Str) {
	//如果不是起始点，则开始条件匹配
	FString CurText = Str;
	if (CurText.RemoveFromStart(TEXT("#")))
	{
		if (CurText.RemoveFromEnd(TEXT("#")))
		{
			this->BeginStr = CurText;
			return true;
		}
	}

	return false;
}

bool CheckStr::AddCheck(FString Sym, FString Str) {

	// /gift if ( #a# + #b# == #ab# ) { 123 } { 321 }
	FString CurText = Str;
	if (CurText.RemoveFromStart(TEXT("#")))
	{
		if (CurText.RemoveFromEnd(TEXT("#")))
		{
			if (this->SymCheck(Sym, CurText))
			{
				if (Sym.Equals(TEXT("+")))
				{
					this->BeginStr += CurText;
					return true;
				}
				else {

					//替换内容
					this->BeginStr = CurText;

					return true;
				}
			}
		}
	}

	UGameSystemFunction::AddGameLog(GAMEINS()->GetWorld(),
		RICHTEXT_LABLE(TEXT("语法错误！没有当前符号："), RichText_MsgLog::INS)
	);
	UGameSystemFunction::AddGameLog(GAMEINS()->GetWorld(),
		RICHTEXT_LABLE(Sym, RichText_MsgLog::INS)
	);
	CheckCount = 0;
	bResult = false;
	this->BeginStr.Empty();
	return false;
}

bool CheckStr::SymCheck(FString Sym, FString Str)
{
	if (Sym.Equals(TEXT("==")))
	{
		CheckCount++;
		bResult = BeginStr.Equals(Str);
		return true;
	}

	if (Sym.Equals(TEXT("!=")))
	{
		CheckCount++;
		bResult = (!BeginStr.Equals(Str));
		return true;
	}

	if (Sym.Equals(TEXT(">"))) {
		CheckCount++;
		bResult = (BeginStr > Str);
		return true;
	}

	if (Sym.Equals(TEXT(">="))) {
		CheckCount++;
		bResult = (BeginStr >= Str);
		return true;
	}

	if (Sym.Equals(TEXT("<"))) {
		CheckCount++;
		bResult = (BeginStr < Str);
		return true;
	}

	if (Sym.Equals(TEXT("<="))) {
		CheckCount++;
		bResult = (BeginStr <= Str);
		return true;
	}

	if (Sym.Equals(TEXT("+"))) {
		return true;
	}

	return false;
}

bool CheckInt::AddBegin(int32 Value) {

	this->IValue = Value;

	return true;
}

bool CheckInt::AddCheck(FString Sym, int32 Value) {

	if (this->SymCheck(Sym, Value))
	{
		if (Sym.Equals(TEXT("+")))
		{
			IValue += Value;
		}
		else if (Sym.Equals(TEXT("-")))
		{
			IValue -= Value;
		}
		else if (Sym.Equals(TEXT("*")))
		{
			IValue *= Value;
		}
		else if (Sym.Equals(TEXT("/")))
		{
			if (IValue == 0)
			{
				return false;
			}

			IValue /= Value;
		}
		else {
			this->IValue = Value;
		}

		if (CheckCount < 1 && this->IValue == 0)
		{
			this->bResult = false;
		}

		return true;
	}

	UGameSystemFunction::AddGameLog(GAMEINS()->GetWorld(),
		RICHTEXT_LABLE(TEXT("语法错误！没有当前符号："), RichText_MsgLog::INS)
	);
	UGameSystemFunction::AddGameLog(GAMEINS()->GetWorld(),
		RICHTEXT_LABLE(Sym, RichText_MsgLog::INS)
	);
	this->bResult = false;
	this->IValue = 0;
	return false;
}

// /gift if ( 1 == 1 ) { 123 } { 321 }
bool CheckInt::SymCheck(FString Sym, int32 Value) {
	if (Sym.Equals(TEXT("==")))
	{
		CheckCount++;
		bResult = (IValue == Value);
		return true;
	}

	if (Sym.Equals(TEXT("!=")))
	{
		CheckCount++;
		bResult = (IValue != Value);
		return true;
	}

	if (Sym.Equals(TEXT(">"))) {
		CheckCount++;
		bResult = (IValue > Value);
		return true;
	}

	if (Sym.Equals(TEXT(">="))) {
		CheckCount++;
		bResult = (IValue >= Value);
		return true;
	}

	if (Sym.Equals(TEXT("<"))) {
		CheckCount++;
		bResult = (IValue < Value);
		return true;
	}

	if (Sym.Equals(TEXT("<="))) {
		CheckCount++;
		bResult = (IValue <= Value);
		return true;
	}

	if (
		Sym.Equals(TEXT("+")) ||
		Sym.Equals(TEXT("-")) ||
		Sym.Equals(TEXT("/")) ||
		Sym.Equals(TEXT("*"))
		) {
		return true;
	}

	return false;
}
