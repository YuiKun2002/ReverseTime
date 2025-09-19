// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/Instructions/GameInstructions.h"
#include "Kismet\GameplayStatics.h"
#include "GameStart/VS/GameMapInstance.h"
#include "GameSystem/GameMapStructManager.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "GameStart/VS/Components/VSManagerComponent.h"

#include "GameStart/VS/UI/UI_Card.h"
#include "GameStart/VS/CardCoolDownManager.h"
#include "GameStart/VS/Components/CardManagerComponent.h"

#include "GameStart/VS/Components/MouseManagerComponent.h"

#include "GameStart/VS/MapMeshe.h"
#include "GameStart/VS/Components/MesheControllComponent.h"

#include "GameStart/VS/Components/ResourceManagerComponent.h"


bool UGameInstructions::ReadInstruction(const TArray<FString>& Instruction)
{
	if (Instruction.Num() == 0)
	{
		return false;
	}

	this->SetInstructions(GET_DEF_NAME(Instruction, Game));

	if (this->GetSystem()->AddHelpInstructionAndPrint(Instruction[0],
		GET_DEF_NAME(Instruction, Game)))
	{
		return false;
	}

	if (this->PrintHelpInstruction(TEXT("game"), Instruction))
	{
		return false;
	}

	if (this->SingleIns(Instruction))
	{
		return true;
	}

	if (this->DoubleIns(Instruction))
	{
		return true;
	}

	if (this->ThreeIns(Instruction))
	{
		return true;
	}

	// game play 小笼包,16 0,0
	// ml 老鼠血量等级 x
	// gr 游戏倍速 x
	// play (x,x) (x,x)
	// playm (x) (x)

	this->PrintErrorIns(TEXT("game"), Instruction[0]);
	return false;
}

AGameMapInstance* UGameInstructions::GetMap()
{
	if (IsValid(AGameMapInstance::GetGameMapInstance()))
	{
		if (!UGameStartSubsystem::GetbGameOver())
		{
			return AGameMapInstance::GetGameMapInstance();
		}
	}
	return nullptr;
}

bool UGameInstructions::SingleIns(const TArray<FString>& Instruction)
{
	if (Instruction.Num() == 0)
	{
		return false;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("p")))
	{
		if (this->GetMap())
		{
			UGameplayStatics::SetGamePaused(this->GetMap(), true);
			return true;

		}
		return false;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("s")))
	{
		if (this->GetMap())
		{
			UGameplayStatics::SetGamePaused(this->GetMap(), false);
			return true;
		}

		return false;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("w")))
	{
		if (this->GetMap())
		{
			if (IsValid(AGameMapInstance::GetGameMapVsManagerStatic()))
			{
				AGameMapInstance::GetGameMapVsManagerStatic()->GameWin();

				return true;
			}
		}

		return false;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("f")))
	{
		if (this->GetMap())
		{
			if (IsValid(AGameMapInstance::GetGameMapVsManagerStatic()))
			{
				AGameMapInstance::GetGameMapVsManagerStatic()->GameFailed();

				return true;
			}
		}

		return false;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("cs")))
	{
		/*if (this->GetMap())
		{
			if (IsValid(AGameMapInstance::GetGameMapCardManagerStatic()))
			{
				bool bRe = false;
				for (ACardCoolDownManager* Cur : AGameMapInstance::GetGameMapCardManagerStatic()->M_CardCoolDownManager)
				{
					if (IsValid(Cur))
					{
						Cur->SetNeverCoolDown();

						bRe = true;
					}
				}

				if (bRe)
				{
					return true;
				}
				else {
					return false;
				}
			}
		}*/

		return false;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("k")))
	{
		if (this->GetMap())
		{
			if (IsValid(AGameMapInstance::GetGameMapMouseManagerStatic()))
			{
				AGameMapInstance::GetGameMapMouseManagerStatic()->SetMouseSpawnKill();

				return true;
			}
		}

		return false;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("fmax")))
	{
		if (this->GetMap())
		{
			if (IsValid(AGameMapInstance::GetGameMapResourceManagerStatic()))
			{
				AGameMapInstance::GetGameMapResourceManagerStatic()->AddFlameNum(9999999);

				return true;
			}
		}
		return false;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("mp")))
	{
		if (this->GetMap())
		{
			if (IsValid(AGameMapInstance::GetGameMapMouseManagerStatic()))
			{
				AGameMapInstance::GetGameMapMouseManagerStatic()->SetMouseTimePause();

				return true;
			}
		}
		return false;
	}

	/*if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("cs")))
	{
		if (this->GetMap())
		{
			UGameplayStatics::SetGlobalTimeDilation(this->GetMap(),1);
		}
	}*/

	return false;
}

bool UGameInstructions::DoubleIns(const TArray<FString>& Instruction)
{
	if (Instruction.Num() <= 1)
	{
		return false;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("gr")))
	{
		if (this->GetMap())
		{
			float CurR = FCString::Atof(*Instruction[1]);
			if (CurR <= 0.1f)
			{
				CurR = 0.1f;
			}
			else if (CurR >= 10.f)
			{
				CurR = 10.f;
			}

			UGameplayStatics::SetGlobalTimeDilation(this->GetMap(), CurR);
			return true;

		}
		return false;
	}


	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("ml")))
	{
		if (this->GetMap())
		{
			if (IsValid(AGameMapInstance::GetGameMapInstance()))
			{
				if (IsValid(AGameMapInstance::GetGameMapInstance()->GetGameMapStructManager()))
				{
					int32 CurI = FCString::Atoi(*Instruction[1]);
					if (CurI <= 0)
					{
						CurI = 1;
					}

					AGameMapInstance::GetGameMapInstance()->GetGameMapStructManager()->LevelConfig.MouseLevel = CurI;

					return true;
				}
			}
		}

		return false;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("playm")))
	{
		if (this->GetSystem()->InstructionTest(Instruction[1], TEXT("-list")))
		{
			TArray<FMouse_Data> Cur;
			UGameSystemFunction::GetDataTableRows(
				TEXT("DataTable'/Game/Resource/BP/Data/MouseData/MouseData.MouseData'")
				, Cur);

			FString CurStr = RICHTEXT(TEXT("老鼠名称列表：")) + RICHLINE;
			for (auto& MouseD : Cur)
			{
				CurStr += RICHTEXT(TEXT("【")) +
					RICHTEXT_LABLE(MouseD.M_Mouse.M_MouseName,
						RichText_MsgLog::T) +
					RICHTEXT(TEXT("】")) +
					RICHLINE;
			}

			this->GetSystem()->AddCurrent(
				CurStr
			);


			return false;
		}
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("playm")))
	{
		if (this->GetMap())
		{
			if (IsValid(AGameMapInstance::GetGameMapMouseManagerStatic()))
			{
				//this->GetMap()->M_MouseManagerComponent->SetMouseSpawnKill();
				FString CurParam1 = Instruction[1];
				FString MouseName;
				FString Row;
				CurParam1.Split(TEXT(","), &MouseName, &Row);

				//线路判断[行]
				int32 CurRow = FCString::Atoi(*Row);
				if (CurRow < 0)
				{
					CurRow = 0;
				}
				else if (CurRow >= AGameMapInstance::GetGameMapMesheControllManagerStatic()->GetMapMeshRowAndCol().Row)
				{
					CurRow = AGameMapInstance::GetGameMapMesheControllManagerStatic()->GetMapMeshRowAndCol().Row - 1;
				}

				if (UMouseSpawnManager::MakeNewMouseByName(
					LoadObject<UDataTable>(nullptr,
						TEXT("DataTable'/Game/Resource/BP/Data/MouseData/MouseData.MouseData'")),
					MouseName, FLine(CurRow, 0)
				))
				{
					return true;
				}
				else {
					this->GetSystem()->AddCurrent(RICHTEXT(TEXT("当前老鼠不存在！！！")));
				}
			}
		}

		return false;
	}

	return false;
}

bool UGameInstructions::ThreeIns(const TArray<FString>& Instruction)
{
	if (Instruction.Num() <= 2)
	{
		return false;
	}

	UMesheControllComponent* MesheCtl = AGameMapInstance::GetGameMapMesheControllManagerStatic();

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("play")) && (Instruction[2].Equals(TEXT("-all"))))
	{
		if (this->GetMap() && IsValid(MesheCtl))
		{
			FString CurParam1 = *Instruction[1];
			FString CardName;
			FString CardGrade;
			CurParam1.Split(TEXT(","), &CardName, &CardGrade);

			for (int32 i = 0; i < MesheCtl->GetMapMeshRowAndCol().Row; i++)
			{
				for (int32 j = 0; j < MesheCtl->GetMapMeshRowAndCol().Col; j++)
				{
					this->PlayCard(CardName, CardGrade, i, j);
				}
			}

			return true;

		}
		return false;
	}

	if (this->GetSystem()->InstructionTest(Instruction[0], TEXT("play")))
	{
		if (this->GetMap() && IsValid(MesheCtl))
		{
			FString CurParam1 = *Instruction[1];
			FString CurParam2 = *Instruction[2];

			FString CardName;
			FString CardGrade;
			CurParam1.Split(TEXT(","), &CardName, &CardGrade);

			FString CardLeftRow;
			FString CardRightCol;
			CurParam2.Split(TEXT(","), &CardLeftRow, &CardRightCol);

			//线路判断[行]
			int32 CurRow = FCString::Atoi(*CardLeftRow);
			if (CurRow < 0)
			{
				CurRow = 0;
			}
			else if (CurRow >= MesheCtl->GetMapMeshRowAndCol().Row)
			{
				CurRow = MesheCtl->GetMapMeshRowAndCol().Row - 1;
			}

			//列
			int32 CurCol = FCString::Atoi(*CardRightCol);
			if (CurCol < 0)
			{
				CurCol = 0;
			}
			else if (CurCol >= MesheCtl->GetMapMeshRowAndCol().Col)
			{
				CurCol = MesheCtl->GetMapMeshRowAndCol().Col - 1;
			}

			return this->PlayCard(CardName, CardGrade, CurRow, CurCol);

		}
	}

	return false;
}

bool UGameInstructions::PlayCard(FString CardName, FString CardGrade, int32 Row, int32 Col)
{
	/*
	//设置等级
	int32 CurGrade = FCString::Atoi(*CardGrade);
	if (CurGrade < 0)
	{
		CurGrade = 0;
	}
	else if (CurGrade > 16)
	{
		CurGrade = 16;
	}

	UMesheControllComponent* MesheCtl = AGameMapInstance::GetGameMapMesheControllManagerStatic();

	//线路判断[行]
	int32 CurRow = Row;
	if (CurRow < 0)
	{
		CurRow = 0;
	}
	else if (CurRow >= MesheCtl->GetMapMeshRowAndCol().Row)
	{
		CurRow = MesheCtl->GetMapMeshRowAndCol().Row - 1;
	}

	//列
	int32 CurCol = Col;
	if (CurCol < 0)
	{
		CurCol = 0;
	}
	else if (CurCol >= MesheCtl->GetMapMeshRowAndCol().Col)
	{
		CurCol = MesheCtl->GetMapMeshRowAndCol().Col - 1;
	}

	//新线路
	FLine NewLine(CurRow, CurCol);

	// game play 小笼包,16 0,0
	//UE_LOG(LogTemp, Error, TEXT("%d  %d"), CurRow, CurCol);

	//得到网格
	AMapMeshe* CurMapMeshe = MesheCtl->GetMapMesh(NewLine.Row, NewLine.Col);
	if (IsValid(CurMapMeshe) && IsValid(CurMapMeshe->GetUIMapMeshe()))
	{
		FItemCard CurCData;
		if (UCardBaseStruct::SearchCardFromDataTable(CardName, CurCData))
		{

			bool bRe = false;
			//查询卡片
			for (FItemCard& CurLocal : AGameMapInstance::GetGameMapCardManagerStatic()->M_CardData)
			{
				if (CurLocal.ItemName.Equals(CurCData.ItemName))
				{
					bRe = true;
					CurLocal.M_CardGrade = CurGrade;
					break;
				}
			}

			if (bRe)
			{
				//更新UI窗口显示的等级图标
				for (UUI_Card* CurUI : AGameMapInstance::GetGameMapCardManagerStatic()->M_UICard)
				{
					if (CurUI->GetCardID().Equals(CurCData.ItemName))
					{
						CurUI->SetCardGrade(CurGrade);
						break;
					}
				}
			}
			else {
				//设置等级
				CurCData.M_CardGrade = CurGrade;
				//新增卡片数据
				AGameMapInstance::GetGameMapCardManagerStatic()->M_CardData.Emplace(CurCData);
			}

			FMesheColPrePlayCard CurCardData;
			CurCardData.CardActorResource = CurCData.CardActorResource;
			CurCardData.CardLayer = CurCData.M_CardLayer;
			CurCardData.CardName = CardName;

			if (CurMapMeshe->GetUIMapMeshe()->CreateCard(
				CurMapMeshe->GetUIMapMeshe(),
				AGameMapInstance::GetGameMapCardManagerStatic(),
				LoadClass<ACardActor>(0, *CurCardData.CardActorResource.ToString()),
				CurCardData.CardName,
				0,
				false,
				false
			))
			{
				this->GetSystem()->AddCurrent(
					FALD(TEXT("您直接放置了")) + FAL(FString::FromInt(CurGrade) + TEXT("星的"), TEXT("g"))
					+ FALD(TEXT("【")) + FAL(CardName, TEXT("c")) + FALD(TEXT("】在")) +
					FAL(FString::FromInt(NewLine.Row + 1), TEXT("g")) + FALD(TEXT("行")) +
					FAL(FString::FromInt(NewLine.Col + 1), TEXT("g")) + FALD(TEXT("列"))
				);

				return true;
			}
			else {
				this->GetSystem()->AddCurrent(FALD(TEXT("卡片放置失败，可能是位置错误，可能是卡片名称错误")));
			}
		}
		else {
			this->GetSystem()->AddCurrent(FALD(TEXT("数据库中并不存在这张卡!!!!!!!!!")));
		}

	}
	*/
	return false;
}
