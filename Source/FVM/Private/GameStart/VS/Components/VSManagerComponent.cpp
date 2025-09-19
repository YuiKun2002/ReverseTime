// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStart/VS/Components/VSManagerComponent.h"
#include "GameStart/VS/Components/MouseManagerComponent.h"
#include "GameStart/VS/Components/MesheControllComponent.h"
#include "GameSystem/GameDataName.h"
#include "GameStart/Flipbook/GameActor/GamePlayer.h"
#include "GameStart/VS/UI/UI_GamePlayerHead.h"
#include "GameStart/VS/UI/UI_VSCountDown.h"
#include "GameStart/VS/UI/UI_GameOver.h"
#include "GameStart/VS/MapMeshe.h"
#include "GameSystem/Item/ItemStruct.h"
#include "GameStart/VS/Components/CardManagerComponent.h"
#include "Data/EquipmentDataSturct.h"
#include "GameStart/VS/UI/UI_CardBar.h"

#include "GameStart/VS/GameMapInstance.h"

#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/GameMapStructManager.h"
#include "GameSystem/Tools/GameSystemFunction.h"

#include "GameSystem/MouseStructManager.h"
#include "GameSystem/DialogueSubsystem.h"

#include "Math/UnrealMathUtility.h"

#include "Game/UI/Tips/UI_BottomTip.h"

#include <Kismet/KismetSystemLibrary.h>


UVSManagerComponent::UVSManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	this->SetTickableWhenPaused(true);
}

void UVSManagerComponent::GameStartInit(class AGameMapInstance* GameMapInstance)
{
	//创建按钮
	this->M_UIBottomTip = Cast<UUI_BottomTip>(UGameSystemFunction::GetUserInterWidgetByName(
		MoveTemp(GameDataNameAsset::UI_GameVs::GameVs),
		MoveTemp(GameDataNameAsset::UI_GameVs::BottomTextTip)
	));

	//创建倒计时
	if (IsValid(AGameMapInstance::GetGameMapInstance()->GetGameMapStructManager()))
	{
		if (AGameMapInstance::GetGameMapInstance()->GetGameMapStructManager()->LevelConfig.LevelTimeWidthLevelConfig.bEnable)
		{
			this->UI_VSCountDown = Cast<UUI_VSCountDown>(UGameSystemFunction::GetUserInterWidgetByName(
				MoveTemp(GameDataNameAsset::UI_GameVs::GameVs),
				MoveTemp(GameDataNameAsset::UI_GameVs::GameTimer)
			));

			//开始倒计时
			this->UI_VSCountDown->Enable(
				AGameMapInstance::GetGameMapInstance()->GetGameMapStructManager()->LevelConfig.LevelTimeWidthLevelConfig.MaxTime
			);

			//添加到屏幕
			this->UI_VSCountDown->AddToViewport();
		}
	}

	//如果玩家没有放置->显示界面---提示玩家放置角色
	this->M_UI_PlayPlayerTipsWidget = UGameSystemFunction::GetUserInterWidgetByName(
		MoveTemp(GameDataNameAsset::UI_GameVs::GameVs),
		MoveTemp(GameDataNameAsset::UI_GameVs::PlayPlayerTip)
	);
	this->M_UI_PlayPlayerTipsWidget->AddToViewport();
}
void UVSManagerComponent::GameStart(class AGameMapInstance* GameMapInstance)
{
	//移除UI
	this->M_UI_PlayPlayerTipsWidget->RemoveFromParent();
}
void UVSManagerComponent::GameTick(float DeltaTime)
{
	if (AGameMapInstance::GetGameMapMouseManagerStatic()->IsMouseSpawnEnd())
	{
		if (!AGameMapInstance::GetGameMapMouseManagerStatic()->IsMouseExist())
		{
			//游戏结束
			this->GameWin();
		}
	}
}
void UVSManagerComponent::GameOverInit(class AGameMapInstance* GameMapInstance, bool bGameWin)
{

}
void UVSManagerComponent::GameOver(class AGameMapInstance* GameMapInstance, bool bGameWin)
{
	//关闭计时
	if (IsValid(this->UI_VSCountDown))
	{
		this->UI_VSCountDown->DisEnable();
	}

	//结算界面
	this->M_UI_GameOver = Cast<UUI_GameOver>(UGameSystemFunction::GetUserInterWidgetByName(
		MoveTemp(GameDataNameAsset::UI_GameVs::GameVs),
		MoveTemp(GameDataNameAsset::UI_GameVs::GameOver)
	));

	if (M_Win)
	{
		this->M_UI_GameOver->ShowOver1();
	}
	this->M_UI_GameOver->ShowOver2(M_Win);
	this->M_UI_GameOver->ShowOver3();
	this->M_UI_GameOver->AddToViewport();

	//播放音乐
	if (M_Win)
	{
		UFVMGameInstance::PlayBGM_Static(FString(TEXT("SoundWave'/Game/Resource/BP/Audio/GameBG/Win.Win'")), false);
	}
	else {
		UFVMGameInstance::PlayBGM_Static(FString(TEXT("SoundWave'/Game/Resource/BP/Audio/GameBG/Failed.Failed'")), false);
	}
}
void UVSManagerComponent::GameResourceUnload()
{

}

bool UVSManagerComponent::CreatePlayer(
	bool bMainPlayer,
	FString PlayerName,
	int32 Row,
	int32 Col,
	int32 PlayerGrade,
	FSoftObjectPath PlayerHead,
	FPlayerSuitItem NewSuit,
	FString FirstWeaponName
)
{
	UMesheControllComponent* MesheCtr = AGameMapInstance::GetGameMapMesheControllManagerStatic();
	if (IsValid(MesheCtr))
	{
		FLine RowAndCol = MesheCtr->GetMapMeshRowAndCol();
		if (Row >= RowAndCol.Row || Col >= RowAndCol.Col)
		{
			return false;
		}

		//网格对象
		AMapMeshe* MesheActor = MesheCtr->GetMapMesh(
			Row,
			Col
		);

		if (!IsValid(MesheActor))
		{
			return false;
		}

		UUI_MapMeshe* UIMeshe = MesheActor->GetUIMapMeshe();
		if (!IsValid(UIMeshe))
		{
			return false;
		}

		//判断当前网格是否存在对象
		if (!UIMeshe->TestCardLayer(0))
		{
			return false;
		}

		//根据地形生成不同的平台【水上，水中，岩浆等地形】
		switch (MesheActor->GetLineType())
		{
		case ELineType::OnWater:
		{
			UFVMGameInstance::PlayBGM_S_Static("PlayCardToOnWater", "ItemAudio");
			if (UIMeshe->TestCardLayer(-1))
			{
				//木盘子
				UIMeshe->CreateFreeCard(96);
			}
		}break;
		case ELineType::Underwater:
		{
			UFVMGameInstance::PlayBGM_S_Static("PlayCardToOnWater", "ItemAudio");
			if (UIMeshe->TestCardLayer(3))
			{
				//苏打气泡
				UIMeshe->CreateFreeCard(107);
			}
		}break;
		case ELineType::Magma:
		{
			UFVMGameInstance::PlayBGM_S_Static("PlayCardToGround", "ItemAudio");
			if (UIMeshe->TestCardLayer(-1))
			{
				//棉花糖
				UIMeshe->CreateFreeCard(102);
			}
		}break;
		default:
		{
			UFVMGameInstance::PlayBGM_S_Static("PlayCardToGround", "ItemAudio");
		}break;
		}


		AGamePlayer* PlayerIns = MesheActor->GetWorld()->SpawnActor<AGamePlayer>(LoadClass<AGamePlayer>(0,
			TEXT("Blueprint'/Game/Resource/BP/GameStart/Item/Player/MyGamePlayer.MyGamePlayer_C'")));
		//添加ID
		UIMeshe->AddCardLayer(0, nullptr);
		UIMeshe->SetPlayerInstance(PlayerIns);
		//设置角色
		AGameMapInstance::GetGameMapVsManagerStatic()->RegistPlayer(PlayerName, PlayerIns);

		//设置旋转
		PlayerIns->SetActorRotation(FRotator(0.f, 90.f, 0.f));
		//初始化网格
		PlayerIns->InitMeshe(UIMeshe, MesheActor);

		if (bMainPlayer)
		{
			//初始化角色形象
			PlayerIns->InitPlayerData();
			//初始化武器
			PlayerIns->InitPlayerWeapon();
		}
		else {

			//设置新套装
			PlayerIns->SetPlayerSuit(NewSuit);

			GAME_LOG(__FUNCTION__, TEXT("设置武器数据"), {
				GAME_LOG_LINE(TEXT("需要重做"))
				});
			
			/*
			UEquipDataAssetCache* Cache = GetGameDataAssetCache_Temp<UEquipDataAssetCache>(
				MoveTemp(GameDataNameAsset::GameDataAsset_GlobalAsset_Equip::GlobalAsset_Equip)
			);
			//设置武器
			for (const auto& Data : Cache->GetWeaponFirst())
			{
				if (Data.M_FEquipment.ItemName.Equals(FirstWeaponName))
				{
					PlayerIns->LoadPlayerFirstWeapon(
						FirstWeaponName,
						Data.M_FEquipment.M_WeaponClassPath.ToString()
					);
					break;
				}
			}
			*/

			//武器无效
			if (!PlayerIns->PlayerFirstWeaponIsValid())
			{
				return false;
			}
		}

		//初始化排序
		PlayerIns->SetRenderLayer(UIMeshe->GetRenderLayer());
		//更新角色位置
		MesheActor->UpdatePlayerLocation();

		//更新网格碰撞
		UIMeshe->UpdateAllCardsCollision();
		//更新角色位置
		UIMeshe->GetMapMeshe()->UpdatePlayerLocation();

		//添加角色头像UI
		UUI_GamePlayerHead* UIPlayerHead = GET_UI_CAST(UUI_GamePlayerHead, GameVs, GamePlayerHead);
		UIPlayerHead->Init(bMainPlayer, PlayerHead, PlayerGrade, PlayerName);
		AGameMapInstance::GetGameMapCardManagerStatic()->GetUICardBar()->AddPlayerInfor(UIPlayerHead);

		return true;
	}

	return false;
}

bool UVSManagerComponent::CreatePlayerByData(FString PlayerName, int32 Row, int32 Col, int32 PlayerGrade, TSoftObjectPtr<UDataTable> PlayerDataPtr)
{
	if (!IsValid(PlayerDataPtr.Get()))
	{
		if (!IsValid(PlayerDataPtr.LoadSynchronous()))
		{
			return false;
		}
	}

	GAME_LOG(__FUNCTION__, TEXT("创建角色数据【服装】"), {
		GAME_LOG_LINE(TEXT("需要重做"))
		});
	/*
	UDataTable* Datas = PlayerDataPtr.Get();
	TArray<FTableRowBase*> SourceDatas;
	Datas->GetAllRows(TEXT("PlayerSuitDataList"), SourceDatas);

	for (FTableRowBase* CurData : SourceDatas)
	{
		FGamePlayerData TempData = *((FGamePlayerData*)(CurData));
		if (TempData.PlayerName.Equals(PlayerName))
		{
			// TempData.Suit
			FPlayerSuitItem TempSuitData;
			//当前是否有服装装备
			TempSuitData.M_bUse = true;
			//服装名称
			TempSuitData.M_SuitName = TempData.Suit.ItemName;
			//服装头像路径
			TempSuitData.M_SuitHeadPath = TempData.Suit.ItemTexturePath;
			//服装展示图片路径1
			TempSuitData.M_SuitFirstShowPath = TempData.Suit.M_PlayerEq_Path_First;
			//服装展示图片路径2
			TempSuitData.M_SuitSecondShowPath = TempData.Suit.M_PlayerEq_Path_Second;
			//服装默认动画1
			TempSuitData.M_SuitFirstDefAnimPath = TempData.Suit.M_PlayerEq_Animaion_Path_First;
			//服装默认动画2
			TempSuitData.M_SuitSecondDefAnim2Path = TempData.Suit.M_PlayerEq_Animaion_Path_Second;
			//服装攻击动画1
			TempSuitData.M_SuitFirstAttackAnimPath = TempData.Suit.M_PlayerEq_Animaion_Path_First_Attack;
			//服装攻击动画2
			TempSuitData.M_SuitSecondAttackAnim2Path = TempData.Suit.M_PlayerEq_Animaion_Path_Second_Attack;
			//服装体力加成
			TempSuitData.M_Suit_HP_Value = TempData.Suit.M_PlayerEq_HP_Value;
			//服装魅力加成
			TempSuitData.M_Suit_Charm_Value = TempData.Suit.M_PlayerEq_Charm_Value;
			//坐标偏移
			TempSuitData.M_PlayerEq_Offset = TempData.Suit.M_PlayerEq_Offset;
			//服装ID号
			TempSuitData.ItemID = TempData.Suit.ItemID;

			if (PlayerGrade <= 0)
			{
				PlayerGrade = 1;
			}
			else if (PlayerGrade > 60)
			{
				PlayerGrade = 60;
			}

			return UVSManagerComponent::CreatePlayer(
				false,
				PlayerName,
				Row,
				Col,
				PlayerGrade,
				TempData.Suit.M_PlayerEq_Path_First,
				TempSuitData,
				TempData.PlayerFirstWeaponName
			);
		}
	}
	*/
	return false;
}

bool UVSManagerComponent::SpawnMainPlayer(FLine PlayerLine)
{
	if (IsValid(this->GetMainPlayer()))
	{
		return false;
	}

	//创建角色
	if (this->CreatePlayer(
		true,
		this->GetMainPlayerName(),
		PlayerLine.Row,
		PlayerLine.Col,
		0,
		FSoftObjectPath(),
		FPlayerSuitItem(),
		TEXT("")
	))
	{
		UGameStartSubsystem::GetGameStartSubsystemStatic()->GameStart();
		AGameMapInstance::GetGameMapInstance()->OnPlayPlayerDelegate.Broadcast();
		return true;
	}
	return false;
}

FString UVSManagerComponent::GetMainPlayerName()
{
	return UFVMGameInstance::GetPlayerStructManager_Static()->M_PlayerName;
}

bool UVSManagerComponent::GetbGameWin() const
{
	return this->M_Win;
}

AGamePlayer* UVSManagerComponent::GetPlayer(FString PlayerName)
{
	AGamePlayer** CurPlayer = this->M_PlayerInstance.Find(PlayerName);
	if (CurPlayer)
	{
		return *CurPlayer;
	}

	return nullptr;
}

AGamePlayer* UVSManagerComponent::GetMainPlayer()
{
	AGamePlayer** CurPlayer = this->M_PlayerInstance.Find(
		UFVMGameInstance::GetPlayerStructManager_Static()->M_PlayerName
	);
	if (CurPlayer)
	{
		return *CurPlayer;
	}

	return nullptr;
}

void UVSManagerComponent::RegistPlayer(FString PlayerName, AGamePlayer* NewPlayerIns)
{
	if (this->M_PlayerInstance.Find(PlayerName))
	{
		return;
	}

	this->M_PlayerInstance.Emplace(PlayerName, NewPlayerIns);
}

void UVSManagerComponent::ShowText(const FString& _Title)
{
	if (IsValid(this->M_UIBottomTip))
	{
		if (this->M_UIBottomTip->IsInViewport())
			this->M_UIBottomTip->RemoveFromParent();

		this->M_UIBottomTip->AddToViewport(1);
		this->M_UIBottomTip->SetTitleAndPlay(_Title);
	}
}

void UVSManagerComponent::UseLevelKey()
{
	this->M_bUseKey = true;
}

void UVSManagerComponent::GameWin()
{
	AGameMapInstance::GetGameMapVsManagerStatic()->M_Win = true;

	//保存对话
	UDialogueSubsystem::SaveCurrentDialogue_Level();

	UGameStartSubsystem::GetGameStartSubsystemStatic()->GameOver(true);
}

void UVSManagerComponent::GameFailed()
{
	AGameMapInstance::GetGameMapVsManagerStatic()->M_Win = false;
	UGameStartSubsystem::GetGameStartSubsystemStatic()->GameOver(false);
}

void UVSManagerComponent::GamePause()
{
	UGameplayStatics::SetGamePaused(UGameStartSubsystem::GetGameStartSubsystemStatic(), true);
}

void UVSManagerComponent::GameContinue()
{
	UGameplayStatics::SetGamePaused(UGameStartSubsystem::GetGameStartSubsystemStatic(), false);
}

