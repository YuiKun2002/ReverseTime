// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/Instructions/PlayerInstructions.h"
#include "Data/DataDataAssetCache.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/PlayerDataSubsystem.h"
#include "Game/UI/Tips/UI_PlayerGetItemBox.h"


bool UPlayerInstructions::ReadInstruction(const TArray<FString>& Instruction)
{
	if (Instruction.Num() == 0)
	{
		return false;
	}

	//设置指令集合描述
	this->SetInstructions(GET_DEF_NAME(Instruction, Player));

	//帮助指令
	if (this->GetSystem()->AddHelpInstructionAndPrint(Instruction[0], GET_DEF_NAME(Instruction, Player)))
	{
		return false;
	}

	//发送帮助指令
	if (this->PrintHelpInstruction(TEXT("player"), Instruction))
	{
		return false;
	}

	//发送指令
	if (this->ExecuteInsHead(Instruction, TEXT("send"),
		std::bind(&UPlayerInstructions::SendIns, this, std::placeholders::_1)))
	{
		return true;
	}

	//删除指令
	if (this->ExecuteInsHead(Instruction, TEXT("rem"),
		std::bind(&UPlayerInstructions::ClearIns, this, std::placeholders::_1)))
	{
		return true;
	}

	//设置指令
	if (this->ExecuteInsHead(Instruction, TEXT("set"),
		std::bind(&UPlayerInstructions::SetIns, this, std::placeholders::_1)))
	{
		return true;
	}

	//回退指令
	if (this->ExecuteInsHead(Instruction, TEXT("return"),
		std::bind(&UPlayerInstructions::ReturnIns, this, std::placeholders::_1)))
	{
		return true;
	}


	this->PrintErrorIns(TEXT("player"), Instruction[0]);
	return false;
}

bool UPlayerInstructions::SendIns(const TArray<FString>& Instruction)
{
	if (Instruction.Num() == 0)
	{
		return false;
	}

	if (Instruction.Num() > 0)
	{
		if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("-list")))
		{

			//所有的ID
			TMap<FString, int32 > Ids;
			GET_CACHE(Data)->GetItemIDAndName(Ids);
			FString CurStr(MoveTempIfPossible(RICHTEXT(TEXT("所有的道具列表：")) + TEXT("\r\n")));
			for (auto& CurItem : Ids)
			{
				CurStr += MoveTempIfPossible(FString(RICHTEXT(TEXT("【")) +
					RICHTEXT_LABLE(FString::FromInt(CurItem.Value), RichText_MsgLog::G) +
					RICHTEXT(TEXT("】：")) +
					RICHTEXT(TEXT("【")) +
					RICHTEXT_LABLE(CurItem.Key, RichText_MsgLog::C) +
					RICHTEXT(TEXT("】")) +
					RICHLINE));
			}

			UGameLogSubsystem::GetGameLogSubsystemStatic()->AddCurrent(
				MoveTempIfPossible(CurStr)
			);

			return false;
		}

		//物品发送
		auto SendItem = [&](const int32& ID, int32 Level, int32 Count)->bool {

			if (Count <= 0)
			{
				Count = 1;
			}

			if (Count >= MAX_int32)
			{
				Count = MAX_int32 / 2;
			}

			if (Level <= 0)
			{
				Level = 0;
			}
			else if (Level >= 16)
			{
				Level = 16;
			}

			if (UGameSystemFunction::SendItemToBag(ID, Level, Count))
			{

				UUI_PlayerGetItemBox::GetPlayerGetItemBoxTipWidget()->AddItem(ID, Level, Count);

				return true;
			}

			return false;
			};

		if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("type")))
		{
			int32 InsCount = Instruction.Num() - 1;
			if (InsCount < 0)
			{
				InsCount = 0;
			}

			if (InsCount == 0)
			{
				return false;
			}

			auto SendItemByType = [&](const int32& Type, int32 Count, int32 Level) {

				if (Count < 0)
				{
					Count = 1;
				}

				if (Count >= MAX_int32)
				{
					Count = MAX_int32 / 2;
				}

				if (Level < 0)
				{
					Level = 0;
				}

				if (Level > 16)
				{
					Level = 16;
				}

				if (Type == 0)
				{
					TMap<int32, FString> Ids;
					TArray<int32> CurId;
					GET_CACHE(Equip)->GetAllItemID(Ids);
					Ids.GenerateKeyArray(CurId);
					for (const auto& Id : CurId)
					{
						UGameSystemFunction::SendItemToBag(Id, 0, Count, 1);
					}
				}

				if (Type == 1)
				{
					TMap<int32, FString> Ids;
					TArray<int32> CurId;
					GET_CACHE(Card)->GetAllItemID(Ids);
					Ids.GenerateKeyArray(CurId);
					for (const auto& Id : CurId)
					{
						UGameSystemFunction::SendItemToBag(Id, Level, Count, 2);
					}
				}

				if (Type == 2)
				{
					TMap<int32, FString> Ids;
					TArray<int32> CurId;
					GET_CACHE(Material)->GetAllItemID(Ids);
					Ids.GenerateKeyArray(CurId);
					for (const auto& Id : CurId)
					{
						UGameSystemFunction::SendItemToBag(Id, 0, Count, 3);
					}
				}

				};

			if (InsCount < 2)
			{
				SendItemByType(FCString::Atoi(*Instruction[1]), 1, 0);
				return true;
			}
			else if (InsCount < 3)
			{
				SendItemByType(
					FCString::Atoi(*Instruction[1]),
					FCString::Atoi(*Instruction[2]),
					0);
				return true;
			}
			else if (InsCount < 4)
			{
				SendItemByType(
					FCString::Atoi(*Instruction[1]),
					FCString::Atoi(*Instruction[2]),
					FCString::Atoi(*Instruction[3]));
				return true;
			}

			return false;
		}

		if (Instruction.Num() < 2)
		{
			return SendItem(
				FCString::Atoi(*Instruction[0]),
				0,
				1);
		}
		else if (Instruction.Num() < 3)
		{
			return SendItem(
				FCString::Atoi(*Instruction[0]),
				FCString::Atoi(*Instruction[1]),
				1);
		}
		else {
			return SendItem(
				FCString::Atoi(*Instruction[0]),
				FCString::Atoi(*Instruction[1]),
				FCString::Atoi(*Instruction[2]));
		}

	}

	return false;
}

bool UPlayerInstructions::ClearIns(const TArray<FString>& Instruction)
{
	if (Instruction.Num() == 0)
	{
		return false;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("0")))
	{
		if (UPlayerDataSubsystem::GetPlayerDataSubsystemStatic())
		{

			//玩家存档
			UPlayerStructManager* Player = UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()
				->GetPlayerDataInstance();

			//移除所有带有没有索引的物品
			Player->RemoveAllEquipItems();

			//删除所有非装备的道具
			Player->RemoveAllEquips();

			UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("装备道具全部销毁操作")));
		}

		return true;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("1")))
	{

		if (UPlayerDataSubsystem::GetPlayerDataSubsystemStatic())
		{
			UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->GetPlayerDataInstance()->Cards.Empty();

			UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("卡片道具全部销毁操作")));
		}

		return true;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("2")))
	{

		if (UPlayerDataSubsystem::GetPlayerDataSubsystemStatic())
		{
			UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->GetPlayerDataInstance()->Maters.Empty();

			UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("材料道具全部销毁操作")));
		}

		return true;
	}


	return false;
}

bool UPlayerInstructions::SetIns(const TArray<FString>& Instruction)
{
	//执行设置技能书指令
	if (this->ExecuteInsHead(Instruction, TEXT("sk"),
		std::bind(&UPlayerInstructions::SetSKIns, this, std::placeholders::_1)))
	{
		return true;
	}

	//执行设置角色等级指令
	if (this->ExecuteInsHead(Instruction, TEXT("pg"),
		std::bind(&UPlayerInstructions::SetPGIns, this, std::placeholders::_1)))
	{
		return true;
	}

	//执行设置货币数量指令
	if (this->ExecuteInsHead(Instruction, TEXT("coin"),
		std::bind(&UPlayerInstructions::SetCoinIns, this, std::placeholders::_1)))
	{
		return true;
	}

	return false;
}

bool UPlayerInstructions::SetSKIns(const TArray<FString>& Instruction)
{
	/*
	if (Instruction.Num() > 0)
	{
		if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("-list")))
		{
			FString CurStr = RICHTEXT(TEXT("技能书列表：")) + TEXT("\r\n");
			for (auto& CurSkill : UFVMGameInstance::GetPlayerStructManager_Static()->M_SkillBookDatas)
			{
				CurStr += RICHTEXT(TEXT("【")) + RICHTEXT_LABLE(CurSkill.Name, TEXT("c")) + RICHTEXT(TEXT("】")) + TEXT("\r\n");
			}

			this->GetSystem()->AddCurrent(
				CurStr
			);

			return false;
		}
	}


	//必须是两个指令
	if (Instruction.Num() > 1)
	{
		if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("-all")))
		{
			int32 UniGrade = FCString::Atoi(*Instruction[1]);

			for (auto& CurSkill : UFVMGameInstance::GetPlayerStructManager_Static()->M_SkillBookDatas)
			{
				CurSkill.M_LV = UniGrade;
				if (CurSkill.M_LV >= 9)
				{
					CurSkill.M_LV = 9;
				}
				else if (CurSkill.M_LV <= 0)
				{
					CurSkill.M_LV = 0;
				}
			}

			UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("设置技能书等级操作")));

			return true;
		}

		FString CurName = Instruction[0];
		for (auto& CurSkill : UFVMGameInstance::GetPlayerStructManager_Static()->M_SkillBookDatas)
		{
			if (CurSkill.Name.Equals(CurName))
			{
				int32 UniGrade = FCString::Atoi(*Instruction[1]);
				CurSkill.M_LV = UniGrade;
				if (CurSkill.M_LV >= 9)
				{
					CurSkill.M_LV = 9;
				}
				else if (CurSkill.M_LV <= 0)
				{
					CurSkill.M_LV = 0;
				}

				return true;
			}
		}
	}
	*/
	return false;
}

bool UPlayerInstructions::SetPGIns(const TArray<FString>& Instruction)
{
	if (Instruction.Num() > 0)
	{
		int32 Grade = FCString::Atoi(*Instruction[0]);
		if (Grade <= 0)
		{
			Grade = 1;
		}
		else if (Grade >= 60)
		{
			Grade = 60;
		}

		UFVMGameInstance::GetPlayerStructManager_Static()->M_FPlayerEx.M_PlayerGrade = Grade;

		return true;
	}

	return false;
}

bool UPlayerInstructions::SetCoinIns(const TArray<FString>& Instruction)
{
	if (Instruction.Num() > 1)
	{
		int32 Num = FCString::Atoi(*Instruction[1]);

		UFVMGameInstance::GetPlayerStructManager_Static()->SetCoin(Instruction[0], Num);

		return true;
	}

	return false;
}

bool UPlayerInstructions::ReturnIns(const TArray<FString>& Instruction)
{
	//执行设置技能书指令
	if (this->ExecuteInsHead(Instruction, TEXT("dia"),
		std::bind(&UPlayerInstructions::ReturnDialogue, this, std::placeholders::_1)))
	{
		return true;
	}

	return false;
}

bool UPlayerInstructions::ReturnDialogue(const TArray<FString>& Instruction)
{
	if (IsValid(UFVMGameInstance::GetPlayerStructManager_Static()))
	{
		/*if (UFVMGameInstance::GetPlayerStructManager_Static()->PlayerLastDialogueID > 0)
		{
			UFVMGameInstance::GetPlayerStructManager_Static()->PlayerLastDialogueID -= 1;
			UFVMGameInstance::GetPlayerStructManager_Static()->PlayerCurrentDialogueID =
				UFVMGameInstance::GetPlayerStructManager_Static()->PlayerLastDialogueID;

			UGameSystemFunction::AddGameLog(UFVMGameInstance::GetFVMGameInstance(), FALD(TEXT("剧情已回退！请前往查看进度！")));

			UFVMGameInstance::GetPlayerStructManager_Static()->Save(__FUNCTION__ + FString(TEXT("剧情回退操作")));
		}*/
	}

	return true;
}
