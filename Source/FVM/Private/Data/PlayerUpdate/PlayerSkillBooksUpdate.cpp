// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/PlayerUpdate/PlayerSkillBooksUpdate.h"
#include "GameSystem/PlayerDataHandle.h"
#include "GameSystem/PlayerData/LocalSkillBookDataHandle.h"

void UPlayerSkillBooksUpdate::Update_Implementation(class UPlayerStructManager* PlayerDataIns)
{
	ULocalPlayerDataHandle* Hd = UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->
		GetLocalPlayerDataHandle();
	if (IsValid(Hd))
	{
		ULocalSkillBookDataHandle* SHD = Cast<ULocalSkillBookDataHandle>(
			Hd->LoadPlayerDataPageHandle(EPlayerDataConfigPage::SkillBook)
		);
		if (IsValid(SHD))
		{
			ULocalSkillBookDataSaveGame* Save = Cast<ULocalSkillBookDataSaveGame>(SHD->Execute_GetData(SHD));
			if (IsValid(Save))
			{
				Save->Update();

				SHD->Execute_SaveData(SHD,TEXT("技能书存档保存"));
			}
		}
	}
}
