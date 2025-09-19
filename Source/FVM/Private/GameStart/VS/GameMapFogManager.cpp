// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/VS/GameMapFogManager.h"
#include "GameStart/VS/Components/MesheControllComponent.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "GameStart/VS/GameMapInstance.h"

bool AGameMapFogBlock::GetDisplayResult()
{
	//清理无效的对象
	TArray<UObject*> Temp;
	for (UObject* TempObj : this->Objs)
	{
		if (IsValid(TempObj))
		{
			Temp.Emplace(TempObj);
		}
	}

	if (Temp.Num())
	{
		this->Objs.Empty();
		this->Objs = Temp;
	}

	//判断当前迷雾是否显示
	if (this->Objs.Num())
	{
		return false;
	}
	else {
		return true;
	}

	return false;
}

void AGameMapFogBlock::SetObj(UObject* NewObj, bool bAdd)
{
	this->bInit = true;

	//清理无效的对象
	TArray<UObject*> Temp;
	for (UObject* TempObj : this->Objs)
	{
		if (IsValid(TempObj))
		{
			Temp.Emplace(TempObj);
		}
	}

	if (Temp.Num())
	{
		this->Objs.Empty();
		this->Objs = Temp;
	}

	if (bAdd)
	{
		int32 Index = this->Objs.Find(NewObj);
		if (Index != -1)
		{
			this->Objs.RemoveAtSwap(Index);
		}
	}
	else {
		if (IsValid(NewObj))
		{
			if (this->Objs.Find(NewObj) == -1)
			{
				this->Objs.Emplace(NewObj);
			}
		}
	}

	//判断当前迷雾是否显示
	if (this->Objs.Num())
	{
		this->Dispaly(false);
	}
	else {
		this->Dispaly(true);
	}
}

void AGameMapFogBlock::TimeHidd(float time)
{
	if (this->bInit)
	{
		this->TimeDispaly(time);
	}
}

UGameMapFogManager::UGameMapFogManager()
{

}

void UGameMapFogManager::GameStartInit(class AGameMapInstance* GameMapInstance) {
	this->bUnload = false;
	this->bInit = false;
	this->FogBlockClass = LoadClass<AGameMapFogBlock>(nullptr,
		TEXT("Blueprint'/Game/Resource/Texture/Sprite/VS/Map/Fog/BP_FogBlock.BP_FogBlock_C'")
	);
}
void UGameMapFogManager::GameStart(class AGameMapInstance* GameMapInstance) {}
void UGameMapFogManager::GameTick(float DeltaTime) {}
void UGameMapFogManager::GameOverInit(class AGameMapInstance* GameMapInstance, bool bGameWin) {}
void UGameMapFogManager::GameOver(class AGameMapInstance* GameMapInstance, bool bGameWin) {}

void UGameMapFogManager::GameResourceUnload()
{
	this->bUnload = true;
	this->FogBlockClass = nullptr;
	if (this->bInit)
	{
		for (auto& Item : this->AllFogBlocks)
		{
			if (IsValid(Item))
			{
				Item->Destroy();
				Item = nullptr;
			}
		}
	}
}

bool UGameMapFogManager::InitAllBlock()
{
	//如果已经被释放
	if (this->bUnload)
	{
		return false;
	}

	//初始化迷雾块
	if (this->bInit)
	{
		return true;
	}
	this->bInit = true;

	//网格组件
	UMesheControllComponent* CurComp = AGameMapInstance::GetGameMapMesheControllManagerStatic();
	this->CurRowAndCol = CurComp->GetMapMeshRowAndCol();
	int32 TransI = 0;
	for (int32 i = 0; i < this->CurRowAndCol.Row; i++)
	{
		for (int32 j = 0; j < this->CurRowAndCol.Col + 1; j++)
		{
			AGameMapFogBlock* CurFogActor = AGameMapInstance::GetGameMapInstance()->
				GetWorld()->SpawnActor<AGameMapFogBlock>(this->FogBlockClass);

			if (j >= this->CurRowAndCol.Col)
			{
				int32 Col = (j >= this->CurRowAndCol.Col) ? this->CurRowAndCol.Col - 1 : j;
				CurFogActor->SetActorLocation(
					CurComp->GetMapMeshLocation(i, Col) +
					FVector(0.f, 90.f, 0.f)
				);
			}
			else
			{
				CurFogActor->SetActorLocation(
					CurComp->GetMapMeshLocation(i, j) + FVector(0.f, 30.f, 0.f)
				);
			}
			CurFogActor->SetTrans(11000 + TransI);
			this->AllFogBlocks.Emplace(CurFogActor);
			TransI++;
		}
	}

	return true;
}

void UGameMapFogManager::SingleBlock(UObject* NewObj, bool bShow, int32 RowIndex, int32 ColIndex)
{
	if (this->InitAllBlock())
	{
		AGameMapFogBlock* CurFogActor = this->AllFogBlocks[
			RowIndex * (this->CurRowAndCol.Col + 1) + ColIndex
		];
		if (IsValid(CurFogActor))
		{
			CurFogActor->SetObj(NewObj, bShow);
		}
	}
}

void UGameMapFogManager::RangeBlock(UObject* NewObj, bool bShow, FLine RangeRow, FLine RangeCol)
{
	if (!this->InitAllBlock())
	{
		return;
	}

	if (RangeRow.Row < 0)
	{
		RangeRow.Row = 0;
	}

	if (RangeRow.Col >= this->CurRowAndCol.Row)
	{
		RangeRow.Col = this->CurRowAndCol.Row - 1;
	}

	if (RangeCol.Row < 0)
	{
		RangeCol.Row = 0;
	}

	if (RangeCol.Col >= this->CurRowAndCol.Col)
	{
		RangeCol.Col = this->CurRowAndCol.Col - 1;
	}

	for (int32 i = RangeRow.Row; i <= RangeRow.Col; i++)
	{
		for (int32 j = RangeCol.Row; j <= RangeCol.Col; j++)
		{
			if (j + 1 == this->CurRowAndCol.Col)
			{
				this->SingleBlock(NewObj, bShow, i, j + 1);
			}
			this->SingleBlock(NewObj, bShow, i, j);
		}
	}
}

void UGameMapFogManager::FullFogBlock(UObject* NewObj, bool bShow)
{
	if (this->InitAllBlock())
	{
		for (AGameMapFogBlock* CurBlock : this->AllFogBlocks)
		{
			if (IsValid(CurBlock))
			{
				CurBlock->SetObj(NewObj, bShow);
			}
		}
	}
}

void UGameMapFogManager::FullTimeHiddBlock(float time)
{
	if (this->InitAllBlock())
	{
		for (AGameMapFogBlock* CurBlock : this->AllFogBlocks)
		{
			if (IsValid(CurBlock))
			{
				CurBlock->TimeHidd(time);
			}
		}
	}
}

