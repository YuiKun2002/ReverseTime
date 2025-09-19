// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/PlayerRecord.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/Tools/GameSystemFunction.h"

#define MAKEADDFUNC(ValueType,Code)\
TFunctionRef<ValueType(const ValueType&)>([&](const ValueType& InputValue)->ValueType Code )\
\

//map数组添加元素
template<typename KeyType = int32, typename ValueType = int32, typename MapType = TMap<KeyType, ValueType>>
void TMapAdd(const KeyType& Key, const ValueType& Value, TFunctionRef<ValueType(const ValueType&)> AddFunc, MapType& Data)
{
	ValueType* FindValue = Data.Find(Key);
	if (FindValue)
	{
		*FindValue = AddFunc(*FindValue);
	}
	else {
		Data.Emplace(Key, Value);
	}
}

#define MAKEGETFUNC(KeyType,ValueType,Code)\
TFunctionRef<ValueType(TMap<KeyType,ValueType>&,const KeyType&)>([&](TMap<KeyType,ValueType>& InputMap,const KeyType& InputKey)->ValueType Code )\
\

//map数组获取元素
template<typename KeyType = int32, typename ValueType = int32, typename MapType = TMap<KeyType, ValueType>>
ValueType TMapGet(
	const KeyType& Key,
	TFunctionRef<ValueType(MapType&, const KeyType&)> GetFunc,
	MapType& Data
)
{
	ValueType* FindValue = Data.Find(Key);
	if (FindValue)
	{
		return *FindValue;
	}

	return GetFunc(Data, Key);
}



UPlayerStructManager* UPlayerRecord::GetPlayer()
{
	return UFVMGameInstance::GetPlayerStructManager_Static();
}

void UPlayerRecord::InitDayRecord()
{
	UPlayerStructManager* Player = GET_PLAYERDATAINS();
	if (IsValid(Player))
	{
		Player->PlayerRecord.DayLevelWinCount = 0;
		Player->PlayerRecord.DayCardUpCount.Empty();
		Player->PlayerRecord.DayCardMakeCount.Empty();
		Player->PlayerRecord.DaySeeInforCount.Empty();
		Player->PlayerRecord.DayUseCardSkillBooks.Empty();
		Player->PlayerRecord.DayCardCount.Empty();
		Player->PlayerRecord.DayKillMouseCount.Empty();
	}
}

int32 UPlayerRecord::AddDayLevelWinCount()
{
	UPlayerStructManager* Player = GET_PLAYERDATAINS();
	if (IsValid(Player))
	{
		Player->PlayerRecord.DayLevelWinCount = Player->PlayerRecord.DayLevelWinCount + 1;
		return Player->PlayerRecord.DayLevelWinCount;
	}
	return 0;
}

void UPlayerRecord::AddDayCardCount(const TMap<FString, int32>& InputCards)
{
	for (const auto& Cur : InputCards)
	{
		TMapAdd(Cur.Key, Cur.Value, MAKEADDFUNC(int32, {
		return InputValue + Cur.Value;
			})
			, GET_PLAYERDATAINS()->PlayerRecord.DayCardCount);
	}
}

void UPlayerRecord::GetDayCardCount(TMap<FString, int32>& OutCards)
{
	OutCards.Append(GET_PLAYERDATAINS()->PlayerRecord.DayCardCount);
}

void UPlayerRecord::AddDayKillMouseCount(const TMap<FString, int32>& InputMouses)
{
	for (const auto& Cur : InputMouses)
	{
		TMapAdd(Cur.Key, Cur.Value, MAKEADDFUNC(int32, {
			return InputValue + Cur.Value;
			})
			, GET_PLAYERDATAINS()->PlayerRecord.DayKillMouseCount);
	}
}

void UPlayerRecord::GetDayKillMouseCount(TMap<FString, int32>& OutMouses)
{
	OutMouses.Append(GET_PLAYERDATAINS()->PlayerRecord.DayKillMouseCount);
}

void UPlayerRecord::AddDayCardSkillBook(FString CardSkillName)
{
	GET_PLAYERDATAINS()->PlayerRecord.DayUseCardSkillBooks.Emplace(CardSkillName);
}

bool UPlayerRecord::GetDayCardSkillBook(FString CardSkillName)
{
	return  GET_PLAYERDATAINS()->PlayerRecord.DayUseCardSkillBooks.Contains(CardSkillName);
}

void UPlayerRecord::AddDayCardUpCount(int32 CurUpGrade, FString UpCardName)
{
	TMapAdd(CurUpGrade, UpCardName, MAKEADDFUNC(FString, {
			return UpCardName;
		})
		, GET_PLAYERDATAINS()->PlayerRecord.DayCardUpCount);
}

FString UPlayerRecord::GetDayCardUpCount(int32 CurUpGrade)
{
	return TMapGet(CurUpGrade,
		MAKEGETFUNC(int32, FString, {
		return FString();
			}),
		GET_PLAYERDATAINS()->PlayerRecord.DayCardUpCount);
}

void UPlayerRecord::AddDayCardMakeCount(FString CardName)
{
	TMapAdd(CardName, 1, MAKEADDFUNC(int32, {
			return InputValue + 1;
		})
		, GET_PLAYERDATAINS()->PlayerRecord.DayCardMakeCount);
}

int32 UPlayerRecord::GetDayCardMakeCount(FString CardName)
{
	return TMapGet(CardName, MAKEGETFUNC(FString, int32, {
		return 0;
		})
		, GET_PLAYERDATAINS()->PlayerRecord.DayCardMakeCount
	);
}

void UPlayerRecord::AddDaySeeInforCount(FString InforName)
{
	GET_PLAYERDATAINS()->PlayerRecord.DaySeeInforCount.Emplace(InforName);
}

bool UPlayerRecord::GetDaySeeInfor(FString InforName)
{
	return GET_PLAYERDATAINS()->PlayerRecord.DaySeeInforCount.Contains(InforName);
}

void UPlayerRecord::AddDayDayNewGemAttachName(FString Name)
{
	GET_PLAYERDATAINS()->PlayerRecord.DayNewGemAttachName = Name;
}

FString UPlayerRecord::GetDayDayNewGemAttachName()
{
	return GET_PLAYERDATAINS()->PlayerRecord.DayNewGemAttachName;
}

int32 UPlayerRecord::AddLevelWinCount()
{
	UPlayerStructManager* Player = GET_PLAYERDATAINS();
	if (IsValid(Player))
	{
		Player->PlayerRecord.LevelWinCount = Player->PlayerRecord.LevelWinCount + 1;
		return Player->PlayerRecord.LevelWinCount;
	}
	return 0;
}






void UPlayerRecord::AddCardMakeCount(int32 Level)
{
	TMapAdd(Level, 1, MAKEADDFUNC(int32, {
			return InputValue + 1;
		})
		, GET_PLAYERDATAINS()->PlayerRecord.CardMakeCount);
}

void UPlayerRecord::AddCardUpGradeCount(int32 Grade)
{
	TMapAdd(Grade, 1, MAKEADDFUNC(int32, {
			return InputValue + 1;
		})
		, GET_PLAYERDATAINS()->PlayerRecord.CardUpCount);
}

void UPlayerRecord::AddCardTransferCount(int32 Num) {
	GET_PLAYERDATAINS()->PlayerRecord.CardChangeFinishCount += Num;
}

void UPlayerRecord::AddGemAttachCount()
{
	GET_PLAYERDATAINS()->PlayerRecord.GemSetWeaponCount += 1;
}

void UPlayerRecord::AddGemUpGrade(FString GemName, int32 GemGrade)
{
	TMapAdd(GemName, GemGrade, MAKEADDFUNC(int32, {
			return(InputValue <= GemGrade ? GemGrade : InputValue);
		})
		, GET_PLAYERDATAINS()->PlayerRecord.GemsGradeCount);
}

void UPlayerRecord::AddGemSplitCount()
{
	GET_PLAYERDATAINS()->PlayerRecord.GemsSplitCount += 1;
}
