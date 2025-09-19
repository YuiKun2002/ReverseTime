// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除
#include "GameSystem/DataTableAssetData.h"
#include "GameSystem/GameDataSubsystem.h"

UDataTable* InitDataTable(FName&& Name1, FName&& Name2)
{
	return UGameDataSubsystem::GetGameDataSubsystemStatic()->GetAsset()->
		GetDataByName_Imp(MoveTemp(Name1))->GetDataByName_Imp(MoveTemp(Name2));
}