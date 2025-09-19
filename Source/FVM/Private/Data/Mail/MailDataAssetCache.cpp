// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/Mail/MailDataAssetCache.h"
#include "GameSystem/Tools/GameSystemFunction.h"

void UMailDataAssetCache::GetMailData(int32 ID, FMail& OutMailData, bool& bResult) {
	if (this->MailInit())
	{
		FMail* mail = this->MailDataTable.Find(ID);
		if (mail)
		{
			OutMailData = *mail;
			bResult = true;
			return;
		}
	}

	bResult = false;
}

void UMailDataAssetCache::GetAllMailData(TMap<int32, FName>& OutData) {
	if (this->MailInit())
	{
		OutData = this->MailName;
	}
}

bool UMailDataAssetCache::MailInit() {

	if (this->MailDataTable.Num())
	{
		return true;
	}

	UDataTable* MailTable = this->GetMailDataTable();
	if (IsValid(MailTable))
	{
		this->Maila.InitDataTable(MailTable);

		for (const auto& Mail : this->Maila.GetDatas())
		{
			this->MailDataTable.Emplace(Mail.Value.ItemID, Mail.Value.M_FEquipment);
			this->MailName.Emplace(Mail.Value.ItemID, Mail.Key);
		}
	}
	else {
		return false;
	}

	this->Maila.InitDataTable(nullptr);

	return true;
}

UDataTable* UMailDataAssetCache::GetMailDataTable() {

	UGameDataSubsystem* Data = UGameDataSubsystem::GetGameDataSubsystemStatic();

	if (!IsValid(Data))
	{
		return nullptr;
	}

	//获取装备都数据对象【邮件数据存放在装备列表中】
	UGameDataAssetObject* Object = Data->GetAsset()->GetDataByName_Imp(GET_MOVE_CATEGORYNAME(Mail));
	if (IsValid(Object))
	{
		return Object->GetDataByName_Imp(GET_MOVE_NAME(Mail, Mail));
	}

	return nullptr;
}