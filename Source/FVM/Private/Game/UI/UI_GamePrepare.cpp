// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/UI/UI_GamePrepare.h"
#include <Components/Button.h>
#include "GameSystem/GameConfigSubsystem.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/GameConfigManager.h"
#include "GameSystem/MouseStructManager.h"
#include "Game/UI/UI_TableTextBlock.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include <Components/UniformGridPanel.h>
#include <Components/HorizontalBox.h>
#include <Components/CanvasPanel.h>
#include <Components/VerticalBox.h>
#include <Components/Image.h>
#include <Kismet/KismetSystemLibrary.h>
#include "Game/UI/UI_PlayerShow.h"
#include "Data/Texture/TextureDataAssetCache.h"

UUI_GamePrepare* UUI_GamePrepare::GetGamePrepareUiStatic()
{
	if (GET_UI(Global, GamePrepare)->IsInViewport())
	{
		return Cast<UUI_GamePrepare>(GET_UI(Global, GamePrepare));
	}

	return nullptr;
}

void UUI_GamePrepare::LoadCardList(UFVMGameInstance* GameIns)
{
	if (this->CurrentSelectCardNum != 0)
	{
		return;
	}

	if (!IsValid(GameIns))
	{
		return;
	}

	//角色数据管理
	this->LocalPlayerData = GameIns->GetPlayerStructManager();

	/*
			数据准备
	*/

	//得到关卡配置
	const FLevelConfig& LLevelData = GameIns->GetGameMapStructManager()->LevelConfig;
	//拿到目前关卡运行的最大等级
	int32 LCurGradeMax = LLevelData.CardGradeMax > GP_CARDMAXLEVEL ?
		GP_CARDMAXLEVEL : LLevelData.CardGradeMax;

	//卡片视图
	UPlayerBagCardView* PlayerBagCardView = GameDataStaticObject<UPlayerBagCardView>();

	/*
			卡片列表渲染
	*/

	//卡片等级
	TArray<int32> Levels;
	//卡片生成总数量
	int32 SpawnTotal = 0;
	for (const auto& ItemData : PlayerBagCardView->GetItemTable())
	{
		//生成等级
		ItemData.Value.ItemCount.GenerateKeyArray(Levels);
		//排序
		Levels.Sort();

		int32 NewLevel = 0;
		if (Levels.Num())
		{
			//生成新的等级,获取最大的等级
			NewLevel = Levels[Levels.Num() - 1];
		}

		if (NewLevel > LCurGradeMax)
		{
			NewLevel = LCurGradeMax;
		}

		//生成UI
		UUI_PlayerBagCardGrid* TempCardGrid = CREATE_UI(this
			, UUI_PlayerBagCardGrid, PlayerBag, CardGrid);

		//设置数据视图块
		FItemViewBlock Block{ ItemData.Key,NewLevel };
		TempCardGrid->SetItemAttribute(PlayerBagCardView, Block, 0);

		TempCardGrid->SetCardOperateType(1);
		//加入到池中
		this->CardUiPool.Emplace(ItemData.Key, TempCardGrid);

		//加入列表
		this->CardGrid->AddChildToUniformGrid(TempCardGrid,
			(SpawnTotal / GP_LOADCARDUICOLSNUMS),
			SpawnTotal - GP_LOADCARDUICOLSNUMS *
			(SpawnTotal / GP_LOADCARDUICOLSNUMS));

		//更新生成数量
		SpawnTotal++;
	}
}

void UUI_GamePrepare::LoadCardSelectConfigList(UFVMGameInstance* GameIns)
{

	if (this->CurrentSelectCardNum != 0)
	{
		return;
	}

	if (!IsValid(GameIns))
	{
		return;
	}

	//如果卡片没有初始化则，无法被选择
	if (this->CardUiPool.Num() == 0)
	{
		return;
	}

	//角色数据管理
	this->LocalPlayerData = GameIns->GetPlayerStructManager();

	//获取角色其他数据
	FPlayerOtherData& OtherData = this->LocalPlayerData->GetOtherData();

	//获取到当前选择的配置项目
	FPlayerSelectListConfig* ConfigIns = this->LocalPlayerData->GetCardSelectConfig().Find(
		OtherData.PlayerSelectCardConfigPageName
	);

	//初始化选择列表,初始化UI，不添加到列表
	for (uint8 Index = 0u; Index < OtherData.PlayerSelectCardCount; Index += 1u)
	{
		UUI_PlayerBagCardGrid* TempCardGrid = CREATE_UI(this,
			UUI_PlayerBagCardGrid, PlayerBag, CardGrid);
		TempCardGrid->SetVisibility(ESlateVisibility::Hidden);

		TempCardGrid->SetCardOperateType(2);

		//加入到缓存区
		this->CardSelectUiPool.Emplace(((int32)Index), TempCardGrid);
		//加入到显示列表
		this->SelectCardList->AddChildToHorizontalBox(TempCardGrid);
	};


	//更新选择列表，添加到列表
	for (const auto& ItemData : ConfigIns->CardsConfig)
	{
		if (ItemData.ItemID != -1)
		{
			///卡片数据格子
			UUI_PlayerBagCardGrid** DataGrid = this->CardUiPool.Find(ItemData.ItemID);
			//查询数据，是否存在
			if (DataGrid)
			{
				this->OnSelectCard(*DataGrid);
			}
		}
	}
}

void UUI_GamePrepare::SaveLocalPlayerData() {

	//游戏实例
	UFVMGameInstance* GameIns = GAMEINS();

	if (!IsValid(this->LocalPlayerData))
	{
		this->LocalPlayerData = GameIns->LoadPlayerStructManager(GameIns->GetPlayerLoginName());
	}

	if (!IsValid(this->LocalPlayerData) || !IsValid(GameIns))
	{
		return;
	}

	//重新配置当前的卡片选择配置页面
	FPlayerOtherData& OtherData = this->LocalPlayerData->GetOtherData();

	//获取当前卡片配置数据
	TArray<UUI_PlayerBagCardGrid*> CurConfig;
	this->CardSelectUiPool.GenerateValueArray(CurConfig);
	//清理玩家选择卡片的缓存数据	
	OtherData.GamePrepareSelectCardData.Empty(CurConfig.Num());

	//获取到当前选择的配置项目
	FPlayerSelectListConfig* ConfigIns = this->LocalPlayerData->GetCardSelectConfig().Find(
		OtherData.PlayerSelectCardConfigPageName
	);

	if (!ConfigIns)
	{
		return;
	}

	ConfigIns->CardsConfig.Empty(CurConfig.Num());
	//解析选择的数据
	for (const auto& CardData : CurConfig)
	{
		if (CardData->GetVisibility() == ESlateVisibility::Hidden)
		{
			//如果当前选择是空的，则传入空数据占位
			ConfigIns->CardsConfig.Emplace(FItemViewBlock{ -1,-1 });
		}
		else {
			const FItemViewBlock& ItemView = CardData->GetItemViewBlock();
			OtherData.GamePrepareSelectCardData.Emplace(ItemView);
			ConfigIns->CardsConfig.Emplace(ItemView);
		}

	}

	//保存角色文件
	UGameSystemFunction::SaveCurrentPlayerData(TEXT("卡片选择，以及卡片选择配置"));
}

void UUI_GamePrepare::LoadMouseList()
{


}

void UUI_GamePrepare::OnSelectCard(UUI_PlayerBagCardGrid* CurSelectBagCardGrid)
{
	//获取角色其他数据
	FPlayerOtherData& OtherData = this->LocalPlayerData->GetOtherData();
	//如果当前选择的数量，小于最大数量，进行卡片选择和UI添加
	if (this->CurrentSelectCardNum >= OtherData.PlayerSelectCardCount)
	{
		return;
	}

	//可选择卡片的数量大小
	uint8 TotalSize = (uint8)this->CardSelectUiPool.Num();
	//查询数据，是否存在
	if (CurSelectBagCardGrid)
	{
		//查询卡片格子,查询空位
		UUI_PlayerBagCardGrid** SelectGrid = this->GetValidGrid(TotalSize);

		//如果有空位，则加入格子
		if (SelectGrid)
		{

			//设置数据
			(*SelectGrid)->SetItemAttributeCopy(CurSelectBagCardGrid);
			//设置显示
			(*SelectGrid)->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			//选定数据格子,设置为被选择
			CurSelectBagCardGrid->GetButton()->SetIsEnabled(false);

			//更新卡片选择次数
			this->CurrentSelectCardNum++;
		}
	}
}

void UUI_GamePrepare::OnRemoveCard(UUI_PlayerBagCardGrid* BagCardGrid)
{
	for (const auto& Grid : this->CardSelectUiPool)
	{
		if (Grid.Value->GetItemViewBlock().ItemID == BagCardGrid->GetItemViewBlock().ItemID)
		{

			this->CurrentSelectCardNum -= 1;

			//恢复，卡片选择
			UUI_PlayerBagCardGrid** GridIns = this->CardUiPool.Find(BagCardGrid->GetItemViewBlock().ItemID);
			if (GridIns)
			{
				(*GridIns)->GetButton()->SetIsEnabled(true);
			}


			//重置选择表的格子数据
			BagCardGrid->SetVisibility(ESlateVisibility::Hidden);
			//FItemViewBlock NewData{ -1,BagCardGrid->GetItemViewBlock().ItemLevel };
			//BagCardGrid->SetItemAttribute(GameDataStaticObject<UPlayerBagCardView>(), NewData, 0);
			return;
		}
	}
}

UUI_PlayerBagCardGrid** UUI_GamePrepare::GetValidGrid(uint8 TotalSize)
{
	for (uint8 i = 0; i < TotalSize; i += 1u)
	{
		UUI_PlayerBagCardGrid** Temp = this->CardSelectUiPool.Find(i);
		if (Temp)
		{
			//如果当前格子是隐藏的表示存在空位
			if ((*Temp)->GetVisibility() == ESlateVisibility::Hidden)
			{
				return Temp;
			}
		}
	}

	return nullptr;
}