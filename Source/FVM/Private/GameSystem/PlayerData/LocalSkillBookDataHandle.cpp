// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/PlayerData/LocalSkillBookDataHandle.h"

void ULocalSkillBookDataSaveGame::GenSkillBookListForWidget(
	ESkillBookTipCategory Type,
	TArray<FSkillBookData>& OutData) {

	OutData.Empty();
	TArray<FSkillBookData> Data;
	this->SkillBookBaseDatas.GenerateValueArray(Data);

	for (auto& Cur : Data)
	{
		if (Cur.TabCategory == Type)
		{
			OutData.Emplace(Cur);
		}
	}
}

bool ULocalSkillBookDataSaveGame::AddSkillBookList(const FCardSkillBookMaterial& NewData, int32& OutCount) {

	FSkillBookData* BookData = this->SkillBookBaseDatas.Find(NewData.M_CardSkillBookMaterial_ID);
	if (BookData)
	{
		//满级
		if (BookData->Level.IsMax())
		{
			OutCount = 0;
			return true;
		}

		//大于4级不再限制
		if (BookData->Level.LV >= 4)
		{
			for (int32 i = 1; i <= OutCount; i++)
			{
				if (BookData->Level.AddEx(NewData.M_CardSkillBookMaterial_ExAdd))
				{
					//设置使用的次数
					OutCount = i;
					return true;
				}
			}
			return true;
		}
		else
		{
			//使用数量
			int32 UseCount = 0;

			//是否进行突破
			if (BookData->Level.CurrentEx == BookData->Level.CurrentTotalEx)
			{
				//使用限制,必须大一级才能使用
				if (BookData->Level.LV + 1 == NewData.M_CardSkillBookMaterial_ExLevel)
				{
					//使用数量+1
					UseCount++;
					//提升等级
					BookData->Level.AddEx(NewData.M_CardSkillBookMaterial_ExAdd);
				}
				else {
					OutCount = 0;
					return false;
				}
			}
			else if (BookData->Level.LV == 0 && NewData.M_CardSkillBookMaterial_ExLevel == 1)
			{
				//使用数量+1
				UseCount++;
				//提升等级
				BookData->Level.AddEx(NewData.M_CardSkillBookMaterial_ExAdd + BookData->Level.CurrentTotalEx);
			}


			//+上突破数量
			if (UseCount < 2)
			{
				UseCount++;
			}

			//4级
			if (BookData->Level.LV >= 4)
			{
				for (int32 i = UseCount; i <= OutCount; i++)
				{
					if (BookData->Level.AddEx(NewData.M_CardSkillBookMaterial_ExAdd))
					{
						//设置使用的次数
						OutCount = i;
						return true;
					}
				}
				return true;
			}
			else {
				int32 Max = OutCount;
				//经验值限制
				for (int32 i = UseCount; i <= Max; i++)
				{
					//加上经验值
					BookData->Level.CurrentEx += NewData.M_CardSkillBookMaterial_ExAdd;
					if (BookData->Level.CurrentEx >= BookData->Level.CurrentTotalEx)
					{
						BookData->Level.CurrentEx = BookData->Level.CurrentTotalEx;
						//设置使用次数
						OutCount = i;
						return true;
					}
				}

				OutCount = Max;
				return true;
			}
		}

		OutCount = 0;
		return false;
	}

	OutCount = 0;
	return false;
}

bool ULocalSkillBookDataSaveGame::CheckSkillBook(const FCardSkillBookMaterial& NewData) {

	FSkillBookData* BookData = this->SkillBookBaseDatas.Find(NewData.M_CardSkillBookMaterial_ID);
	if (BookData)
	{
		//满级
		if (BookData->Level.IsMax())
		{
			return false;
		}

		if (BookData->Level.LV < 4)
		{
			//突破等级
			if (BookData->Level.CurrentEx == BookData->Level.CurrentTotalEx)
			{
				//使用限制,必须大一级才能使用
				if (BookData->Level.LV + 1 == NewData.M_CardSkillBookMaterial_ExLevel)
				{
					return true;
				}
				else {
					return false;
				}
			}
			else {
				if (BookData->Level.LV >= NewData.M_CardSkillBookMaterial_ExLevel
					||
					(
						BookData->Level.LV == 0 &&
						NewData.M_CardSkillBookMaterial_ExLevel == 1)
					)
				{
					return true;
				}
			}
		}
	}
	return false;
}


void ULocalSkillBookDataSaveGame::Update() {
	this->UpdateSKillBook();
}

void ULocalSkillBookDataSaveGame::UpdateSKillBook() {
	DataTableAssetData<FSkillBookData_Data> Data = GET_DATA(FSkillBookData_Data, Material, SkillBookData);
	TArray<FSkillBookData_Data> Datas;
	Data.GetValues(Datas);
	for (auto& Cur : Datas)
	{
		if (!this->SkillBookBaseDatas.Contains(Cur.M_FSkillBookData.ID))
		{
			this->SkillBookBaseDatas.Emplace(Cur.M_FSkillBookData.ID, Cur.M_FSkillBookData);
		}
	}
}

bool ULocalSkillBookDataHandle::CreateData_Implementation() {

	this->SaveGameObject = NewObject<ULocalSkillBookDataSaveGame>();

	this->Execute_SaveData(this, TEXT("新建-卡片技能书存档"));

	return true;
}