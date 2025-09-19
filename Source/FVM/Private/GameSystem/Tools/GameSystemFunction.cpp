// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystem/Tools/GameSystemFunction.h"
#include <Components/Button.h>
#include "UI/WidgetBase.h"
#include "Data/GameLogSubsystem.h"

#include <Components/BoxComponent.h>
#include <Components/SphereComponent.h>

#include "GameStart/Flipbook/SpriteActor.h"
#include "Game/UI/Tips/UI_PlayerGetItemBox.h"
#include "GameStart/Flipbook/GameActorFlipbookBase.h"

#include "Paper2D/Classes/PaperFlipbook.h"

#include "GameStart/VS/GameMapInstance.h"
#include "GameStart/VS/Components/MesheControllComponent.h"
#include "GameStart/VS/MapMeshe.h"

#include "GameFramework/GameUserSettings.h"

#include "GameSystem/PlayerDataHandle.h"
#include "GameSystem/GameConfigSubsystem.h"
#include "GameSystem/PlayerDataSubsystem.h"
#include "GameSystem/GameUserInterfaceSubsystem.h"
#include "GameSystem/PlayerStructManager.h"

#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/KismetMathLibrary.h>

#include "Internationalization/Regex.h"

FDateTime UGameSystemFunction::GameSystem_Time;
FDateTime UGameSystemFunction::GameSystem_Time_Now;
FRandomStream UGameSystemFunction::Random_Now;
bool UGameSystemFunction::bEnable = false;

void UGameSystemFunction::UpdateTime()
{
	if (UGameSystemFunction::bEnable)
	{
		UGameSystemFunction::GameSystem_Time_Now = UGameSystemFunction::GameSystem_Time.Now();
		UGameSystemFunction::Random_Now.Initialize(UGameSystemFunction::GameSystem_Time_Now.GetHour() + UGameSystemFunction::GameSystem_Time_Now.GetMinute() + UGameSystemFunction::GameSystem_Time_Now.GetSecond() + UGameSystemFunction::GameSystem_Time_Now.GetMillisecond());
	}
}

void UGameSystemFunction::OpenUpdate()
{
	UGameSystemFunction::bEnable = true;
}

void UGameSystemFunction::CloseUpdate()
{
	UGameSystemFunction::bEnable = false;
}

void UGameSystemFunction::ClearTimeClip(FTimerManager& _TimeManager, FTimeClip& _Clip)
{
	_TimeManager.ClearTimer(_Clip.M_FTimerHandle);
	_Clip.M_Count = 0;
}

void UGameSystemFunction::SaveCurrentPlayerData(const FString& SaveLogMsg)
{
	if (IsValid(UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->GetPlayerDataInstance()))
	{
		UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->SavePlayerData(
			Cast<UObject>(UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->GetLocalPlayerDataHandle()),
			SaveLogMsg
		);
	}
}

UPlayerStructManager* UGameSystemFunction::LoadCurrentPlayerData(const FString& LoadLogMsg)
{
	if (IsValid(UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->GetPlayerDataInstance()))
	{
		ULocalPlayerDataHandle* Hd = UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->GetLocalPlayerDataHandle();
		if (UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->LoadPlayerData(
			Hd, LoadLogMsg
		))
		{
			return Cast<UPlayerStructManager>(Cast<IDataInterface>(Hd)->Execute_GetData(Hd));
		}
	}

	return nullptr;
}

void UGameSystemFunction::SaveCurrentGameConfigData()
{
	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__, TEXT("游戏配置数据已保存"));
	}

	UGameConfigSubsystem::GetGameConfigSubsystemStatic()->SaveLocalGameConfigInstance();
}

void UGameSystemFunction::SendMailToCurrentPlayer(int32 MailID, int32 Count)
{
	if (!IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return;
	}

	UPlayerStructManager* Player = UFVMGameInstance::GetFVMGameInstance()->GetPlayerStructManager();
	if (IsValid(Player))
	{
		FMailItemSave* Save = Player->Mails.Find(MailID);
		if (Save)
		{
			Save->Count += Count;
		}
		else {
			FMailItemSave NewSave;
			NewSave.Count = Count;
			NewSave.bRead = false;
			Player->Mails.Emplace(MailID, NewSave);

			//更新邮件提示
			UUI_MainFramework* Frame = GET_UI_CAST(UUI_MainFramework, Global, MainFrame);
			if (IsValid(Frame) && Frame->IsInViewport())
			{
				Frame->ShowMailTip();
			}
		}
	}
}

FMaterialBase* UGameSystemFunction::GetMaterialDataFromBag(TArray<FMaterialBase>& _Arrays, const FString& _ItemName)
{
	for (FMaterialBase& Items : _Arrays)
	{
		if (Items.ItemName.Equals(_ItemName))
			return &Items;
	}
	return nullptr;
}

void UGameSystemFunction::GetMaterialsArrayByType(
	TArray<FMaterialBase>& _ArraysSource,
	const TArray<EMaterialType>& _Type,
	TArray<FMaterialBase*>& OutArrays
)
{
	//更具类型添加材料
	for (const auto& LType : _Type)
	{
		for (FMaterialBase& Item : _ArraysSource)
		{
			if (Item.M_MaterialType == LType)
			{
				OutArrays.Emplace(&Item);
			}
		}
	}
}

FString UGameSystemFunction::GetCardGradeImagePath(int32 _Grade)
{
	// Texture2D'/Game/Resource/Texture/CardGrade/1.1'

	if (_Grade == 0)
		return FString();

	FString _GradePath = "Texture2D'/Game/Resource/Texture/CardGrade/" + FString::FormatAsNumber(_Grade) + "." + FString::FormatAsNumber(_Grade) + "'";
	return _GradePath;
}

bool UGameSystemFunction::GetRange(int32 _ValueMaxRange, int32 _EndMaxRange)
{
	int32 _Result = UGameSystemFunction::Random_Now.RandRange(1, _EndMaxRange);;

	//UGameSystemFunction::FVMLog(__FUNCTION__, FString::FromInt(_ValueMaxRange) + TEXT(" <= ") + FString::FromInt(_Result));

	if (_ValueMaxRange >= _Result)
		return true;

	return false;
}

int32 UGameSystemFunction::GetRange_Int(int32 _ValueMaxRange, int32 _EndMaxRange)
{
	return UGameSystemFunction::Random_Now.RandRange(1, _EndMaxRange);
}

int32 UGameSystemFunction::GetRandomRange(int32 Random_Min, int32 RandomNums)
{
	return UGameSystemFunction::Random_Now.RandRange(Random_Min, RandomNums);
}

float UGameSystemFunction::GetRandomRangeFloat(float Random_Min, float RandomNums)
{
	return UGameSystemFunction::Random_Now.FRandRange(Random_Min, RandomNums);
}

void UGameSystemFunction::TabSelect(const TArray<UButton*>& _ButtonArrays, const  FString& _UniformPath, const  TArray<FString>& _DefButtonStyle, const TArray<FString>& _ClickButtonStyle, int32 _Index)
{
	//确保索引相同
	if (_ButtonArrays.Num() == _DefButtonStyle.Num() && _DefButtonStyle.Num() == _ClickButtonStyle.Num())
	{
		int32 Count = _ButtonArrays.Num();

		if (_Index < 0 || _Index >= Count)
			return;

		//先全部使用默认样式
		for (int32 i = 0; i < Count; i++)
		{
			FString Path = FString(_UniformPath + _DefButtonStyle[i] + TEXT(".") + _DefButtonStyle[i] + TEXT("'"));
			UWidgetBase::SetButtonStyle(_ButtonArrays[i], Path, true);
			_ButtonArrays[i]->SetVisibility(ESlateVisibility::Visible);
		}

		//激活指定的选项卡
		FString Path = FString(_UniformPath + _ClickButtonStyle[_Index] + TEXT(".") + _ClickButtonStyle[_Index] + TEXT("'"));
		UWidgetBase::SetButtonStyle(_ButtonArrays[_Index], Path, true);
		_ButtonArrays[_Index]->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UGameSystemFunction::WidgetsSelect(const TArray<UWidget*>& _Widgets, ESlateVisibility _ShowType, int32 _Index)
{
	UGameSystemFunction::WidgetSelect<UWidget>(_Widgets, _ShowType, _Index);
}

void UGameSystemFunction::FVMLog(const FString& _FuncName, const FString& _Content)
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Error, TEXT("%s %s"), *_FuncName, *_Content);
	if (UGameLogSubsystem::GetGameLogSubsystemStatic())
	{
		UGameLogSubsystem::GetGameLogSubsystemStatic()->AddPrivate(
			RICHTEXT(_FuncName + TEXT(" ") + _Content)
		);
	}
#endif

#if UE_BUILD_DEVELOPMENT
	if (UGameLogSubsystem::GetGameLogSubsystemStatic())
	{
		UGameLogSubsystem::GetGameLogSubsystemStatic()->AddPrivate(
			RICHTEXT(_FuncName + TEXT(" ") + _Content)
		);
	}
#endif
}

void UGameSystemFunction::AddGameLog(const UObject* WorldContext, const FString& RichLable_Msg)
{
	UGameLogSubsystem* Cur = UGameplayStatics::GetGameInstance(
		WorldContext
	)->GetSubsystem<UGameLogSubsystem>();
	if (IsValid(Cur))
	{
		Cur->AddCurrent(RichLable_Msg);
	}
}

void UGameSystemFunction::AddDebugLog(const UObject* WorldContext, const FString& RichLable_Msg)
{
	if (UFVMGameInstance::GetDebug())
	{
		UGameLogSubsystem* Cur = UGameplayStatics::GetGameInstance(
			WorldContext
		)->GetSubsystem<UGameLogSubsystem>();
		if (IsValid(Cur))
		{
			Cur->AddPrivate(RichLable_Msg);
		}
	}
}

bool UGameSystemFunction::AddGameinstructions(const UObject* WorldContext, const FString& InputContent, bool bTip)
{
	if (bTip)
	{
		UGameSystemFunction::AddGameLog(WorldContext, RICHTEXT_LABLE(InputContent, TEXT("Ins")));
	}

	UGameLogSubsystem* Cur = UGameplayStatics::GetGameInstance(
		WorldContext
	)->GetSubsystem<UGameLogSubsystem>();
	if (IsValid(Cur))
	{
		if (Cur->ExeInstructions(InputContent))
		{
			if (bTip)
			{
				UWidgetBase::CreateTipWidget(TEXT("指令执行成功"));
			}
		}
	}

	return false;
}

FString UGameSystemFunction::BlueprintAddLog(const FString& Content, const FString& Flag /*= FString(TEXT("t")*/)
{
	return FVMAddLogPanel(Content, Flag);
}

void UGameSystemFunction::LoadMap()
{
	if (UPlayerStructManager* Player = UFVMGameInstance::GetPlayerStructManager_Static())
	{
		UGameUserInterfaceSubsystem* UserInter = UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic();

		FString MapPah = FPlayerInMap::GetMapName(Player->M_PlayerInMap.CurrentInMap);

		//关闭当前进入的UI地图
		UserInter->GetUserInterInstance(
			GameDataNameAsset::UI_WorldMap::WorldMap
		)->RemoveUI_ByName(FName(Player->M_PlayerInMap.CurrentInMapUIName));
		//设置新的UI地图名称
		Player->M_PlayerInMap.CurrentInMapUIName = MapPah;
		//显示新的UI地图
		UserInter->GetUserInterInstance(
			GameDataNameAsset::UI_WorldMap::WorldMap
		)->ShowUI_ByName(FName(Player->M_PlayerInMap.CurrentInMapUIName));

		//重新显示框架
		UGameSystemFunction::GetUserInterInstanceByName(
			MoveTemp(GameDataNameAsset::UI_Global::Global)
		)->RemoveUI_ByName_Imp(
			MoveTemp(GameDataNameAsset::UI_Global::MainFrame)
		);

		UGameSystemFunction::GetUserInterInstanceByName(
			MoveTemp(GameDataNameAsset::UI_Global::Global)
		)->ShowUI_ByName_Imp(
			MoveTemp(GameDataNameAsset::UI_Global::MainFrame)
		);

		UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("{加载当前进入的地图UI}")));
	}
}

void UGameSystemFunction::LoadLastMap()
{
	if (UPlayerStructManager* Player = UFVMGameInstance::GetPlayerStructManager_Static())
	{
		UGameUserInterfaceSubsystem* UserInter = UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic();

		FString MapPah = Player->M_PlayerInMap.LastInMap == EGameVsMap::VM_Main ?
			FPlayerInMap::GetMapName(EGameVsMap::VM_Map_1) :
			FPlayerInMap::GetMapName(Player->M_PlayerInMap.LastInMap);

		//关闭当前进入的UI地图
		UGameSystemFunction::GetUserInterInstanceByName(
			MoveTemp(GameDataNameAsset::UI_WorldMap::WorldMap)
		)->RemoveUI_ByName(FName(Player->M_PlayerInMap.CurrentInMapUIName));

		//设置新的UI地图名称
		Player->M_PlayerInMap.CurrentInMapUIName = MapPah;

		//显示新的UI地图
		UGameSystemFunction::GetUserInterInstanceByName(
			MoveTemp(GameDataNameAsset::UI_WorldMap::WorldMap)
		)->ShowUI_ByName(FName(Player->M_PlayerInMap.CurrentInMapUIName));

		//重新显示框架
		UGameSystemFunction::GetUserInterInstanceByName(
			MoveTemp(GameDataNameAsset::UI_Global::Global)
		)->RemoveUI_ByName_Imp(
			MoveTemp(GameDataNameAsset::UI_Global::MainFrame)
		);

		UGameSystemFunction::GetUserInterInstanceByName(
			MoveTemp(GameDataNameAsset::UI_Global::Global)
		)->ShowUI_ByName_Imp(
			MoveTemp(GameDataNameAsset::UI_Global::MainFrame)
		);

		UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("{加载上一次进入的地图UI}")));
	}
}

void UGameSystemFunction::SetMap(EGameVsMap _Map)
{
	if (UPlayerStructManager* Player = UFVMGameInstance::GetPlayerStructManager_Static())
	{
		if (Player->M_PlayerInMap.CurrentInMap != _Map)
		{
			Player->M_PlayerInMap.CurrentInMap = _Map;

			if (UFVMGameInstance::GetDebug())
			{
				UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("设置当前的地图")));
			}
		}
	}
}

void UGameSystemFunction::SetLastMap(EGameVsMap _Map)
{
	if (UPlayerStructManager* Player = UFVMGameInstance::GetPlayerStructManager_Static())
	{
		if (Player->M_PlayerInMap.LastInMap != _Map)
		{
			Player->M_PlayerInMap.LastInMap = _Map;

			if (UFVMGameInstance::GetDebug())
			{
				UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("设置上一次的地图")));
			}
		}
	}
}

FString UGameSystemFunction::GetPlayerGradeImagePath()
{
	UPlayerStructManager* Player = UFVMGameInstance::GetFVMGameInstance()->GetPlayerStructManager();
	FString _Grade_Path = TEXT("Texture2D'/Game/Resource/Texture/PlayerGrade/") + FString::FromInt(Player->M_FPlayerEx.M_PlayerGrade) + TEXT(".") + FString::FromInt(Player->M_FPlayerEx.M_PlayerGrade) + TEXT("'");
	return _Grade_Path;
}

EGameVsMap UGameSystemFunction::GetPlayerInMap()
{
	if (UPlayerStructManager* Player = UFVMGameInstance::GetPlayerStructManager_Static())
	{
		return Player->M_PlayerInMap.LastInMap;
	}

	return EGameVsMap::VM_Main;
}


EGameVsMap UGameSystemFunction::GetPlayerCurrentMap()
{
	if (UPlayerStructManager* Player = UFVMGameInstance::GetPlayerStructManager_Static())
	{
		return Player->M_PlayerInMap.CurrentInMap;
	}

	return EGameVsMap::VM_Main;
}

EGameVsMap UGameSystemFunction::GetPlayerLastInMap()
{
	return EGameVsMap::VM_Map_1;
}


void UGameSystemFunction::ResetDataTable(class UDataTable* _datatable, int32 BeginID)
{
	if (!IsValid(_datatable))
	{
		return;
	}

	TArray<FTableRowBase*> Items;
	_datatable->GetAllRows(TEXT("List"), Items);

	int32 Be = BeginID;
	for (FTableRowBase*& CurData : Items)
	{
		FItemBaseStructData* Temp = (FItemBaseStructData*)(CurData);
		Temp->SetID(Be);

		Be++;
	}
	_datatable->MarkPackageDirty();
}

FString UGameSystemFunction::GetObjectName(const UObject* Obj)
{
	return UKismetSystemLibrary::GetObjectName(Obj);
}

bool UGameSystemFunction::AddSphereTraceMulti(AActor* M_Owner, TArray<FHitResult>& OutActors, const FVector& _BeginOffset, const FVector& _EndOffset, float _Radius, ETraceTypeQuery _Type)
{

	if (!IsValid(M_Owner))
	{
		return false;
	}

	TArray<AActor*> Actors;
	if (UKismetSystemLibrary::SphereTraceMulti(
		M_Owner->GetWorld(),
		M_Owner->GetActorLocation() + _BeginOffset,
		M_Owner->GetActorLocation() + _EndOffset,
		_Radius, _Type, false, Actors, EDrawDebugTrace::ForDuration, OutActors, false
	))
	{
		return true;
	}

	return false;
}

bool UGameSystemFunction::AddSphereTrace(AActor* M_Owner, FHitResult& OutActor, const FVector& _BeginOffset, const FVector& _EndOffset, float _Radius, ETraceTypeQuery _Type)
{
	if (!M_Owner)
		return false;

	if (IsValid(M_Owner))
	{
		TArray<AActor*> Actors;
		if (UKismetSystemLibrary::SphereTraceSingle(
			M_Owner->GetWorld(),
			M_Owner->GetActorLocation() + _BeginOffset,
			M_Owner->GetActorLocation() + _EndOffset,
			_Radius,
			_Type, false, Actors, EDrawDebugTrace::ForDuration, OutActor, false)
			)
		{
			return true;
		}
	}

	return false;
}

bool UGameSystemFunction::AddLineTrance(
	const UWorld* World,
	const FVector& CurLoca,
	const FVector& BeginOffset,
	const FVector& EndOffset,
	ECollisionChannel CC,
	UObject* InputObj,
	TFunctionRef<void(UObject* Obj, AActor* CurHit)> Func
)
{
	//添加射线
	FHitResult Hit;

	if (UFVMGameInstance::GetDebug())
	{
		DrawDebugLine(World,
			CurLoca + BeginOffset,
			CurLoca + EndOffset,
			FColor(1.f, 0.f, 0.f, 1.f), false, 5.f);
	}

	if (World->LineTraceSingleByChannel(Hit,
		CurLoca + BeginOffset,
		CurLoca + EndOffset,
		CC))
	{
		Func(InputObj, Hit.GetActor());

		return true;
	}
	return false;
}

bool UGameSystemFunction::AddLineTranceMulti(
	const UWorld* World,
	const FVector& BeginLocation,
	const FVector& EndLocation,
	const TArray<TEnumAsByte<EObjectTypeQuery>> CC,
	UObject* InputObj,
	TFunctionRef<void(UObject* Obj, AActor* CurHit)> Func
)
{
	//添加射线
	TArray<FHitResult> OutHits;

	if (UFVMGameInstance::GetDebug())
	{
		DrawDebugLine(World,
			BeginLocation,
			EndLocation,
			FColor(1.f, 0.f, 0.f, 1.f), false, 5.f);
	}

	if (UKismetSystemLibrary::LineTraceMultiForObjects(
		World,
		BeginLocation, EndLocation, CC, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHits, true
	))
	{
		for (const FHitResult& CurHit : OutHits)
		{
			Func(InputObj, CurHit.GetActor());
		}

		return true;
	}
	return false;
}

void UGameSystemFunction::InitMouseMeshe(class UBoxComponent* Box, USceneComponent* BoxChildComp, FVector2D BoxRange, FVector2D Offset, bool bBack)
{
	if (IsValid(Box))
	{
		//FVector CurLocation = BoxChildComp->GetRelativeLocation();

		if (!IsValid(BoxChildComp))
		{
			return;
		}

		if (bBack)
		{
			Box->SetBoxExtent(FVector(20.f, BoxRange.X, BoxRange.Y), true);
			Box->AddLocalOffset(FVector(0.f, Offset.X, Offset.Y));
			BoxChildComp->AddLocalOffset(FVector(0.f, -Offset.X, -Offset.Y));
		}
		else {
			Box->SetBoxExtent(FVector(BoxRange.X, 20.f, BoxRange.Y), true);
			Box->AddLocalOffset(FVector(Offset.X, 0.f, Offset.Y));
			BoxChildComp->AddLocalOffset(FVector(-Offset.X, 0.f, -Offset.Y));
		}


		if (UFVMGameInstance::GetDebug())
		{
			Box->SetHiddenInGame(false);
			BoxChildComp->SetHiddenInGame(false);
		}
	}
}

void UGameSystemFunction::InintMouseCollisonByBox(class UBoxComponent* Box, FVector2D BoxRange /*= FVector2D(20.f)*/, bool bBack /*= false*/)
{
	if (IsValid(Box))
	{
		if (bBack)
		{
			Box->SetBoxExtent(FVector(20.f, BoxRange.X, BoxRange.Y), true);
		}
		else {
			Box->SetBoxExtent(FVector(BoxRange.X, 20.f, BoxRange.Y), true);
		}

		if (UFVMGameInstance::GetDebug())
		{
			Box->SetHiddenInGame(false);
		}
	}
}

void UGameSystemFunction::InintMouseCollisonBySphere(class USphereComponent* Sphere, float Radius)
{
	if (IsValid(Sphere))
	{
		Sphere->SetSphereRadius(Radius);

		if (UFVMGameInstance::GetDebug())
		{
			Sphere->SetHiddenInGame(false);
		}
	}
}

AActor* UGameSystemFunction::CreateIceBlock(AActor* M_Owner, const FVector& _Offset, const int32& _Layer, float _Time)
{
	if (IsValid(M_Owner))
	{
		//创建冰块图层
		ASpriteActor* Obj = M_Owner->GetWorld()->SpawnActor<ASpriteActor>(
			LoadClass<ASpriteActor>(0, TEXT("Class'/Script/FVM.SpriteActor'")));
		//Obj->SetTranslucency(8000);
		Obj->SetLifeSpan(_Time);
		Obj->SetSprite(LoadObject<UPaperSprite>(0,
			TEXT("PaperSprite'/Game/Resource/Texture/Sprite/VS/Sprite/Items/19/t/IceBlock/T_public_Sprite.T_public_Sprite'")
		));
		Obj->SetSpriteOffset(_Offset);
		Obj->SetSpriteLocation(M_Owner->GetActorLocation(), true);
		Obj->SetTranslucency(_Layer);
		return Obj;
	}
	return nullptr;
}

void UGameSystemFunction::CreateMapMatrix(FLine _Line, int32 _RowCount, int32 _ColCount, FVector _Offset, FString _FlipBookAnimPath)
{
	UPaperFlipbook* Anim = LoadObject<UPaperFlipbook>(0, *_FlipBookAnimPath);

	if (!Anim)
		return;

	int32 MaxRows = AGameMapInstance::GetGameMapInstance()->GetMesheControllComponent()->GetMapMeshRowAndCol().Row;
	int32 MaxCols = AGameMapInstance::GetGameMapInstance()->GetMesheControllComponent()->GetMapMeshRowAndCol().Col;

	//确定初始索引(确定偏移)
	int32 LInitRow = _Line.Row - _RowCount / 2;
	for (int32 i = 0; i < _ColCount; i++, LInitRow++)
	{
		//确定初始索引(确定偏移)
		int32 LInitCol = _Line.Col - _ColCount / 2;
		for (int32 j = 0; j < _ColCount; j++, LInitCol++)
		{
			if (LInitRow >= 0 && LInitCol >= 0)
				if (LInitRow < MaxRows && LInitCol < MaxCols)
				{

					AMapMeshe* Mesh = AGameMapInstance::GetGameMapInstance()->
						GetMesheControllComponent()->GetMapMesh(LInitRow, LInitCol);

					FVector Location = AGameMapInstance::GetGameMapInstance()->
						GetMesheControllComponent()->GetMapMeshLocation(LInitRow, LInitCol);

					AGameActorFlipbookBase* AAnims = Mesh->GetWorld()->SpawnActor<AGameActorFlipbookBase>(
						LoadClass<AGameActorFlipbookBase>(0,
							TEXT("Class'/Script/FVM.GameActorFlipbookBase'")));
					AAnims->SetRenderLayer(8000);
					AAnims->InitRotation();
					AAnims->GetMyActor()->SetFlipbook(Anim);
					AAnims->SetAnimationPlayEndDestroy();
					AAnims->SetActorLocation(Location + _Offset);
				}
		}
	}
}




bool UGameSystemFunction::HitMouse(UObject* CurHitMouseObj, float _HitValue, AMouseActor* _Mouse, FGameBuffInfor& _Buff, EFlyItemAttackType AttackType)
{
	if (IsValid(_Mouse))
	{
		//解析Buff信息
		_Mouse->ParseBuff_Information(_Buff, CurHitMouseObj);
		//被命中传入伤害数值
		if (_Mouse->BeHit(CurHitMouseObj, _HitValue, AttackType))
		{
			//设置老鼠状态->被击中
			_Mouse->SetbIsHurt(true);
			return true;
		}
	}
	return false;
}

AActor* UGameSystemFunction::CreateCheckMouseLineTrace(class UWorld* World, const TArray<FLineTraceSetting>& InputLineTraceSetting, ELineTraceType& OutCheckLineType)
{
	//线路检测【如果发现老鼠则返回】
	//如果检测到老鼠，符合条件直接返回当前老鼠

	//检测
	auto TempCheck = [&](const FLineTraceSetting& Line, const ELineTraceType& Type, AActor*& OutMouseActor)->bool {
		//创建一次单通道的射线检测
		return UGameSystemFunction::CheckMouseLineType(World,
			Line.M_BeginLocation, Line.M_EndLocation,
			UGameSystemFunction::GetMouseCollisionBoxTypeByLineType(Type),
			OutMouseActor);
		};

	//当前老鼠
	AActor* CurMouse = nullptr;
	for (const auto& Line : InputLineTraceSetting)
	{
		DrawDebugLine(World, Line.M_BeginLocation, Line.M_EndLocation, FColor::Red);

		//获取当前的线路碰撞
		ECollisionChannel CurCollision = UGameSystemFunction::GetMouseCollisionBoxTypeByLineType(Line.M_ELineTraceType);
		//检测线路
		if (TempCheck(Line, Line.M_ELineTraceType, CurMouse))
		{
			//如果当前线路检测成功有老鼠，并且是一个不确定的类型
			if (CurCollision == ECollisionChannel::ECC_GameTraceChannel9)
			{
				//进一步检测
				switch (Line.M_ELineTraceType)
				{
					//陆地空中混合
				case ELineTraceType::E_MouseGroundAndSky: {
					//优先检测空中的老鼠
					if (TempCheck(Line, ELineTraceType::E_MouseSky, CurMouse))
					{
						OutCheckLineType = ELineTraceType::E_MouseSky;
						//如果检测成功
						return CurMouse;
					}
					//检测陆地老鼠
					if (TempCheck(Line, ELineTraceType::E_MouseGround, CurMouse))
					{
						OutCheckLineType = ELineTraceType::E_MouseGround;
						//如果检测成功
						return CurMouse;
					}
				}; break;
				case ELineTraceType::E_MouseVisibility: {
					OutCheckLineType = ELineTraceType::E_MouseVisibility;
					return CurMouse;
				}break;
				}
			}
			else {
				OutCheckLineType = Line.M_ELineTraceType;
				//返回一个确定的类型老鼠
				return CurMouse;
			}
		}
	}

	return nullptr;
}

bool UGameSystemFunction::CheckMouseLineType(UWorld* World, const FVector& BeginLoc, const FVector& EndLoc, const ECollisionChannel& Type, AActor*& OutMouse)
{
	FHitResult TraceResult;
	FCollisionQueryParams TraceParams;
	FCollisionResponseParams TraceResParams;

	if (World->LineTraceSingleByChannel(
		TraceResult,
		BeginLoc,
		EndLoc,
		Type,
		TraceParams,
		TraceResParams))
	{
		OutMouse = TraceResult.GetActor();
		return true;
	}

	return false;
}

ETraceTypeQuery UGameSystemFunction::GetMouseCollisionTraceType(EMouseCollisionType _EMouseCollisionType)
{
	switch (_EMouseCollisionType)
	{
	case EMouseCollisionType::MouseGround:
		return ETraceTypeQuery::TraceTypeQuery4;
	case EMouseCollisionType::MouseSky:
		return ETraceTypeQuery::TraceTypeQuery5;
	case EMouseCollisionType::MouseUnder:
		return ETraceTypeQuery::TraceTypeQuery6;
	default:
		return ETraceTypeQuery::TraceTypeQuery11;
	}
	return ETraceTypeQuery::TraceTypeQuery11;
}

ECollisionChannel UGameSystemFunction::GetMouseCollisionBoxTypeByELineType(ELineType _EMouseLineType)
{
	switch (_EMouseLineType)
	{
	case ELineType::OnGround:return ECollisionChannel::ECC_GameTraceChannel2; break;
	case ELineType::Sky:return ECollisionChannel::ECC_GameTraceChannel3; break;
	case ELineType::Underground:return ECollisionChannel::ECC_GameTraceChannel4; break;
	default:
		return ECollisionChannel::ECC_GameTraceChannel9;
	}

	return ECollisionChannel::ECC_GameTraceChannel9;
}

ETraceTypeQuery UGameSystemFunction::GetMouseCollisionTraceTypeByELineType(ELineType _EMouseLineType)
{
	switch (_EMouseLineType)
	{
	case ELineType::OnGround:return ETraceTypeQuery::TraceTypeQuery4;; break;
	case ELineType::Sky:return ETraceTypeQuery::TraceTypeQuery5; break;
	case ELineType::Underground:return ETraceTypeQuery::TraceTypeQuery6; break;
	default:
		return ETraceTypeQuery::TraceTypeQuery11;
	}

	return ETraceTypeQuery::TraceTypeQuery11;
}

ECollisionChannel UGameSystemFunction::GetMouseCollisionBoxTypeByLineType(ELineTraceType _EMouseLineType)
{
	switch (_EMouseLineType)
	{
	case ELineTraceType::E_MouseGround:return ECollisionChannel::ECC_GameTraceChannel2; break;
	case ELineTraceType::E_MouseSky:return ECollisionChannel::ECC_GameTraceChannel3; break;
	case ELineTraceType::E_MouseUnder:return ECollisionChannel::ECC_GameTraceChannel4; break;
	default:
		return ECollisionChannel::ECC_GameTraceChannel9;
		break;
	}

	return ECollisionChannel::ECC_GameTraceChannel9;
}

ETraceTypeQuery UGameSystemFunction::GetMouseCollisonTraceTypeByLineType(ELineTraceType _EMouseLineType)
{
	return UEngineTypes::ConvertToTraceType(UGameSystemFunction::GetMouseCollisionBoxTypeByLineType(_EMouseLineType));
}

ETraceTypeQuery UGameSystemFunction::GetCardCollisionTraceType(ECardCollisionType _ECardCollisionType)
{
	switch (_ECardCollisionType)
	{
	case ECardCollisionType::E_CardActor2:
		return ETraceTypeQuery::TraceTypeQuery9;

	case ECardCollisionType::E_CardActor3:
		return ETraceTypeQuery::TraceTypeQuery10;
	default:
		return ETraceTypeQuery::TraceTypeQuery3;
	}
	return ETraceTypeQuery::TraceTypeQuery3;
}

ECollisionChannel UGameSystemFunction::GetCardCollisionBoxType(ECardCollisionType _ECardCollisionType)
{
	switch (_ECardCollisionType)
	{
	case ECardCollisionType::E_CardActor2:
		return ECollisionChannel::ECC_GameTraceChannel7;

	case ECardCollisionType::E_CardActor3:
		return ECollisionChannel::ECC_GameTraceChannel8;
	default:
		return ECollisionChannel::ECC_GameTraceChannel1;
	}
	return ECollisionChannel::ECC_GameTraceChannel1;
}

void UGameSystemFunction::CalculateLineTracePosition(const FVector& _Location, const FLine& _Line, UUI_MapMeshe* _UI_Meshe, const FLineTraceSetting& _LineTraceSetting, TArray<FLineTraceSetting>& _LineTraceSettingOut)
{
	//基本坐标 
	FVector BaseLocation = _Location;
	BaseLocation.X -= 5.f;

	float TargetLY = _UI_Meshe->GetMapMeshCurrentLeft(_Line.Row).Y - 55.f;
	float TargetRY = _UI_Meshe->GetMapMeshCurrentRight(_Line.Row).Y + 55.f;
	FVector Top = _UI_Meshe->GetMapMeshCurrentTop(_Line.Col);
	FVector Bottom = _UI_Meshe->GetMapMeshCurrentBottom(_Line.Col);
	float GridCountMax = _LineTraceSetting.M_GridCount * (55.f);

	FLineTraceSetting _Item;
	_Item.M_ELineTraceType = _LineTraceSetting.M_ELineTraceType;

	switch (_LineTraceSetting.M_ELineTracePosition)
	{
	case ELineTracePosition::E_Left:
	{
		FVector LocationEnd = FVector(BaseLocation.X, TargetLY, BaseLocation.Z);
		_Item.M_BeginLocation = BaseLocation;
		_Item.M_EndLocation = LocationEnd;
	}
	break;
	case ELineTracePosition::E_Right:
	{
		FVector LocationEnd = FVector(BaseLocation.X, TargetRY, BaseLocation.Z);

		_Item.M_BeginLocation = BaseLocation;

		_Item.M_EndLocation = _LineTraceSetting.M_bEnableSettingGridCount ? BaseLocation + FVector(0.f, GridCountMax, 0.f) : LocationEnd;

		if (_Item.M_EndLocation.Y > LocationEnd.Y)
			_Item.M_EndLocation = LocationEnd;

	}
	break;
	case ELineTracePosition::E_LeftRight_M:
	{
		FVector LocationBegin = FVector(BaseLocation.X, TargetLY, BaseLocation.Z);
		FVector LocationEnd = FVector(BaseLocation.X, TargetRY, BaseLocation.Z);
		_Item.M_BeginLocation = LocationBegin;
		_Item.M_EndLocation = LocationEnd;
	}
	break;
	case ELineTracePosition::E_Top:
	{
		Top.X = BaseLocation.X;
		Top.Y = BaseLocation.Y;
		Top.Z += 45.f;

		_Item.M_BeginLocation = BaseLocation;
		//_Item.M_EndLocation = Top;
		_Item.M_EndLocation = _LineTraceSetting.M_bEnableSettingGridCount ? BaseLocation + FVector(0.f, 0.f, GridCountMax) : Top;

	}
	break;
	case ELineTracePosition::E_Bottom:
	{
		Bottom.X = BaseLocation.X;
		Bottom.Y = BaseLocation.Y;
		Bottom.Z -= 45.f;

		_Item.M_BeginLocation = BaseLocation;
		//_Item.M_EndLocation = Bottom;
		_Item.M_EndLocation = _LineTraceSetting.M_bEnableSettingGridCount ? BaseLocation + FVector(0.f, 0.f, -GridCountMax) : Bottom;

	}
	break;
	case ELineTracePosition::E_TopBottom_M:
	{
		Top.X = BaseLocation.X;
		Top.Y = BaseLocation.Y;
		Top.Z += 45.f;
		Bottom.X = BaseLocation.X;
		Bottom.Y = BaseLocation.Y;
		Bottom.Z -= 45.f;
		_Item.M_BeginLocation = Top;
		_Item.M_EndLocation = Bottom;
	}
	break;
	}

	//加上偏移量
	_Item.M_BeginLocation += _LineTraceSetting.M_LineTraceOffset;
	_Item.M_EndLocation += _LineTraceSetting.M_LineTraceOffset;

	//UE_LOG(LogTemp, Error, TEXT("%.2f  %.2f %.2f"), _LineTraceSetting.M_LineTraceOffset.X, _LineTraceSetting.M_LineTraceOffset.Y, _LineTraceSetting.M_LineTraceOffset.Z);

	_LineTraceSettingOut.Emplace(_Item);
}

void UGameSystemFunction::GetCurveLineByTimeLineBlue(FVector CurLocation, FVector TargetLocation, float MaxHight, const float& TimeTick, const float& HightTick, FVector& OutLocation)
{
	UGameSystemFunction::GetCurveLineByTimeLine(CurLocation, TargetLocation, MaxHight, TimeTick, HightTick, OutLocation);
}

void UGameSystemFunction::GetCurveLineByTimeLine(const FVector& CurLocation, const FVector& TargetLocation, const float& MaxHight, const float& TimeTick, const float& HightTick, FVector& OutLocation)
{
	FVector A = CurLocation;
	A.Z = HightTick * MaxHight + CurLocation.Z;

	FVector B = TargetLocation;
	B.Z = HightTick * MaxHight + TargetLocation.Z;

	OutLocation = UKismetMathLibrary::VLerp(A, B, TimeTick);
}

void UGameSystemFunction::SetGlobalGameTime(const UObject* WorldContextObject, float _TimeRate)
{
	UGameplayStatics::SetGlobalTimeDilation(WorldContextObject, _TimeRate);
}

void UGameSystemFunction::SetCaptureLook(const UObject* WorldContextObject, EMouseLockMode Lock)
{
	FInputModeGameAndUI GameUI;
	GameUI.SetHideCursorDuringCapture(false);
	GameUI.SetLockMouseToViewportBehavior(Lock);
	UGameplayStatics::GetPlayerController(WorldContextObject, 0)->SetInputMode(GameUI);
}

//发送补偿邮件
void UGameSystemFunction::SendVersionCompensate()
{
	//获取存档
	if (UPlayerStructManager* Player = UFVMGameInstance::GetFVMGameInstance()->GetPlayerStructManager())
	{
		TArray<FPlayerVersionCompensate_Data*> Datas;
		//获取数据表
		UGameSystemFunction::GetDataTableRows<FPlayerVersionCompensate_Data>(
			TEXT("DataTable'/Game/Resource/BP/Data/EquipmentData/GameVersionCompensate.GameVersionCompensate'"),
			Datas);

		//获取总数量
		int32 DataCount = Datas.Num();
		//获取背包存储数量
		int32 Count = Player->M_GameVersionCompensate.Num();

		if (UFVMGameInstance::GetDebug())
			UGameSystemFunction::FVMLog(__FUNCTION__,
				FString(TEXT("----------------------------------------------------------------")));

		if (UFVMGameInstance::GetDebug())
			UGameSystemFunction::FVMLog(__FUNCTION__,
				FString(TEXT("当前邮件数量：") +
					FString::FromInt(Count)) +
				TEXT("总量：") +
				FString::FromInt(DataCount));

		//防止出现版本更新时-已经领取的补偿-重复领取
		for (auto& Item : Player->M_GameVersionCompensate)
		{
			Item.M_SendState = true;
		}

		//计算需要添加多少补偿
		int32 Result = DataCount - Count;

		bool bSend = false;
		if (Result > 0)
		{

			if (UFVMGameInstance::GetDebug())
				UGameSystemFunction::FVMLog(__FUNCTION__, FString(TEXT("需要补发数量：") + FString::FromInt(Result)));

			for (int i = Count; i < DataCount; i++)
			{
				FPlayerVersionCompensate Data = Datas[i]->M_FPlayerVersionCompensate;
				Player->M_GameVersionCompensate.Emplace(Data);
			}


			for (auto& Item : Player->M_GameVersionCompensate)
			{
				if (!Item.M_SendState)
				{
					FMailItemSave* MailData = Player->GetMailList().Find(Item.M_SendMailID);
					if (MailData)
					{
						MailData->Count += 1;
					}
					else {
						FMailItemSave Data;
						Data.bRead = false;
						Data.Count = 1;
						Player->GetMailList().Emplace(Item.M_SendMailID, Data);
					}

					Item.M_SendState = true;

					if (UFVMGameInstance::GetDebug())
						UGameSystemFunction::FVMLog(__FUNCTION__,
							FString(TEXT("发送补偿方案：") +
								FString::FromInt(Item.M_SendMailID)));

					bSend = true;
				}
				else {
					if (UFVMGameInstance::GetDebug())
						UGameSystemFunction::FVMLog(__FUNCTION__,
							FString(TEXT("当前补偿方案失效：") +
								FString::FromInt(Item.M_SendMailID)));
				}
			}
		}

		if (UFVMGameInstance::GetDebug())
			UGameSystemFunction::FVMLog(__FUNCTION__,
				FString(TEXT("----------------------------------------------------------------")));

		if (bSend)
		{
			//保存
			UGameSystemFunction::SaveCurrentPlayerData(TEXT("邮件发送"));
		}
	}
}

//获取指定技能书的等级
int32 UGameSystemFunction::GetPlayerSkillBooksLv(const FString& _Name)
{
	/*for (const auto& Item : UFVMGameInstance::GetPlayerStructManager_Static()->M_SkillBookDatas)
	{
		if (_Name.Equals(Item.Name))
			return Item.Level.LV;
	}*/

	GAME_LOG(__FUNCTION__, TEXT("获取技能书等级"), {
	GAME_LOG_LINE(TEXT("需要重做"));
		});

	return 0;
}

int32 UGameSystemFunction::GetItemAddedCount(int32 ID, int32 Level, int32 AddCount, uint8 CheckType) {

	//自动判断
	if (CheckType == 0u)
	{
		uint8 Type;
		FItemBaseStructData* Data = nullptr;
		FName Tag = FName();
		if (GET_ITEM(ID, Type, Data, Tag))
		{
			if (Tag.IsEqual(GET_DEF_CATEGORYNAME(Equip)))
			{
				CheckType = 1u;
			}
			else if (Tag.IsEqual(GET_DEF_CATEGORYNAME(Card)))
			{
				CheckType = 2u;
			}
			else if (Tag.IsEqual(GET_DEF_CATEGORYNAME(Material)))
			{
				CheckType = 3u;
			}
			else {
				CheckType = 0u;
			}
		}
	}

	//装备
	if (CheckType == 1u)
		return UGameSystemFunction::GetEquipAddedCount(ID, Level, AddCount);

	//卡片
	if (CheckType == 2u)
		return UGameSystemFunction::GetCardAddedCount(ID, Level, AddCount);

	//材料
	if (CheckType == 3u)
		return UGameSystemFunction::GetMaterAddedCount(ID, Level, AddCount);

	return 0;
}

bool UGameSystemFunction::SendItemToBag(int32 ID, int32 Level, int32 Count, uint8 SendType, bool bSave, bool bShowTip) {

	//数量判断
	long long Max = Count;
	if (Max >= MAX_int32)
	{
		Count = MAX_int32 / 2;
	}
	else if (Count < 1)
	{
		Count = 1;
	}

	//发送装备
	if (SendType == 0u || SendType == 1u)
		if (UGameSystemFunction::SendEquipToBag(ID, Level, Count, bSave)) {
			if (bShowTip)
			{
				UUI_PlayerGetItemBox::GetPlayerGetItemBoxTipWidget()->AddItem(ID, Level, Count);
			}
			return true;
		}

	//发送卡片
	if (SendType == 0u || SendType == 2u)
		if (UGameSystemFunction::SendCardToBag(ID, Level, Count, bSave)) {
			if (bShowTip)
			{
				UUI_PlayerGetItemBox::GetPlayerGetItemBoxTipWidget()->AddItem(ID, Level, Count);
			}
			return true;
		}

	//发送材料
	if (SendType == 0u || SendType == 3u)
		if (UGameSystemFunction::SendMaterToBag(ID, Level, Count, bSave)) {
			if (bShowTip)
			{
				UUI_PlayerGetItemBox::GetPlayerGetItemBoxTipWidget()->AddItem(ID, Level, Count);
			}
			return true;
		}

	return false;
}

bool UGameSystemFunction::SendCardToBag(int32 ID, int32 Level, int32 Count, bool bSave) {

#if WITH_EDITOR
	FItemCard BaseData;
	if (UCardDataAssetCache::GetCardData(ID, BaseData))
	{
		GAME_LOG(__FUNCTION__, TEXT("卡片发送"), {
		FVMLOG(__FUNCTION__, FString(
			FString::FromInt(ID) +
			FString(
				TEXT("[") +
				BaseData.ItemName +
				TEXT("]")
			) +
			FString(
				TEXT("[") +
				FString(TEXT("等级:")) +
				FString::FromInt(Level) +
				TEXT("]")
			) +
			FString(
				TEXT("[") +
				FString(TEXT("数量:")) +
				FString::FromInt(Count) +
				TEXT("]")
			) +
			TEXT("发送到背包")
		));
			});

	}
	else {
		FVMLOG(__FUNCTION__, FString(
			FString::FromInt(ID) +
			TEXT("[当前卡片不存在]")
		));
		return false;
	}
#endif

	//获取卡片列表
	TMap<int32, FBaseItemSave>& Cards = GET_PLAYERDATAINS()->GetCardList();
	FBaseItemSave* CurItemData = Cards.Find(ID);
	if (CurItemData)
	{
		CurItemData->AddItem(Level, Count);
	}
	else {
		FBaseItemSave NewItem;
		NewItem.AddItem(Level, Count);
		Cards.Emplace(ID, NewItem);
	}

	if (bSave)
	{
		FString Msg;
#if WITH_EDITOR
		Msg = __FUNCTION__ +
			FString(TEXT("新增卡片[") +
				BaseData.ItemName +
				TEXT("]到背包,")) +

			FString(TEXT("ID:[")) +
			FString::FromInt(ID) +
			FString(TEXT("],")) +

			FString(TEXT("数量:[")) +
			FString::FromInt(Count) +
			FString(TEXT("],")) +

			FString(TEXT("等级:[")) +
			FString::FromInt(Level) +
			FString(TEXT("]"));
#endif
		UGameSystemFunction::SaveCurrentPlayerData(Msg);
	}

	return true;
}

int32 UGameSystemFunction::GetCardAddedCount(int32 ID, int32 Level, int32 AddCount) {
	TMap<int32, FBaseItemSave>& List = GET_PLAYERDATAINS()->GetCardList();
	FBaseItemSave* ItemSave = List.Find(ID);
	if (ItemSave)
	{
		//溢出个数
		long long Max = (ItemSave->GetItemCount(Level) + AddCount);
		if (Max > MAX_int32)
		{
			return (Max - MAX_int32) * -1.f;
		}

		//正常剩余
		return MAX_int32 - (ItemSave->GetItemCount(Level) + AddCount);
	}
	else {
		return MAX_int32;
	}
	return 0;
}

bool UGameSystemFunction::SendEquipToBag(int32 ID, int32 Level, int32 Count, bool bSave) {
#if WITH_EDITOR
	FEquipmentBase BaseData;
	if (UEquipDataAssetCache::GetEquipData(ID, BaseData))
	{
		GAME_LOG(__FUNCTION__, TEXT("装备发送"), {
		FVMLOG(__FUNCTION__, FString(
			FString::FromInt(ID) +
			FString(
				TEXT("[") +
				BaseData.ItemName +
				TEXT("]")
			) +
			FString(
				TEXT("[") +
				FString(TEXT("等级:")) +
				FString::FromInt(Level) +
				TEXT("]")
			) +
			FString(
				TEXT("[") +
				FString(TEXT("数量:")) +
				FString::FromInt(Count) +
				TEXT("]")
			) +
			TEXT("发送到背包")
		));
			});

	}
	else {
		FVMLOG(__FUNCTION__, FString(
			FString::FromInt(ID) +
			TEXT("[当前装备不存在]")
		));
		return false;
	}
#endif

	//获取装备列表
	TMap<int32, FBaseItemSave>& Equips = GET_PLAYERDATAINS()->GetSourceEquipList();
	FBaseItemSave* CurItemData = Equips.Find(ID);
	if (CurItemData)
	{
		CurItemData->AddItem(Level, Count);
	}
	else {
		FBaseItemSave NewItem;
		NewItem.AddItem(Level, Count);
		Equips.Emplace(ID, NewItem);
	}

	if (bSave)
	{
		FString Msg;
#if WITH_EDITOR
		Msg = __FUNCTION__ +
			FString(TEXT("新增装备[") +
				BaseData.ItemName +
				TEXT("]到背包,")) +

			FString(TEXT("ID:[")) +
			FString::FromInt(ID) +
			FString(TEXT("],")) +

			FString(TEXT("数量:[")) +
			FString::FromInt(Count) +
			FString(TEXT("],")) +

			FString(TEXT("等级:[")) +
			FString::FromInt(Level) +
			FString(TEXT("]"));
#endif
		UGameSystemFunction::SaveCurrentPlayerData(Msg);
	}

	return true;
}
int32 UGameSystemFunction::GetEquipAddedCount(int32 ID, int32 Level, int32 AddCount) {
	TMap<int32, FBaseItemSave>& List = GET_PLAYERDATAINS()->GetSourceEquipList();
	FBaseItemSave* ItemSave = List.Find(ID);
	if (ItemSave)
	{
		//溢出个数
		long long Max = (ItemSave->GetItemCount(Level) + AddCount);
		if (Max > MAX_int32)
		{
			return (Max - MAX_int32) * -1.f;
		}

		//正常剩余
		return MAX_int32 - (ItemSave->GetItemCount(Level) + AddCount);
	}
	else {
		return MAX_int32;
	}
	return 0;
}
//想背包发送指定的防御卡
bool UGameSystemFunction::SendCardToPlayerBag(const FString& _CardName, int32 _Lv)
{
	GAME_LOG(__FUNCTION__, TEXT("发送卡片到玩家背包"), {
	GAME_LOG_LINE(TEXT("需要删除这个函数，因为有代替函数"));
		});
	/*
	FItemCard CardData;
	if (UCardBaseStruct::SearchCardFromDataTable(_CardName, CardData))
	{
		CardData.M_CardGrade = _Lv;
		UFVMGameInstance::GetPlayerStructManager_Static()->M_PlayerItems_Card.Emplace(CardData);

		UGameLogSubsystem::AddPlayerGetCardLog(_CardName, _Lv);

		if (UFVMGameInstance::GetDebug())
		{
			UGameSystemFunction::FVMLog(__FUNCTION__, TEXT("发送防御卡：") + _CardName + TEXT("到背包"));
		}

		UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("发送防御卡：")) + _CardName + TEXT("到背包"));

		return true;
	}

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__, TEXT("发送防御卡失败：") + _CardName);
	}
	*/
	return false;
}

//发送指定装备到背包
bool UGameSystemFunction::SendEquipmentToPlayerBag(const FString& _EquipmentName, int32 _Count, bool _bSave)
{
	GAME_LOG(__FUNCTION__, TEXT("发送装备到玩家背包"), {
	GAME_LOG_LINE(TEXT("需要删除这个函数，因为有代替函数"));
		});
	/*
	if (_Count > 1000000000)
		_Count = 1000000000;

	if (_Count < 1)
		_Count = 1;

	FEquipmentBase EquipmentData;

	if (UEquipmentBaseStruct::SearchEquipmentFromDataTable(_EquipmentName, EquipmentData))
	{
		//当前物品是可以叠加的道具
		if (EquipmentData.M_IsOverlap)
		{
			//查询结果
			bool LResult = false;

			//背包查询->是否有重复的->如果有则直接添加对应个数
			for (auto& Item : UFVMGameInstance::GetPlayerStructManager_Static()->M_PlayerItems_Equipment)
			{
				if (_EquipmentName.Equals(Item.ItemName))
				{
					//根据类型发送背包
					if (Item.M_IsOverlap)
					{
						if (Item.M_Count + _Count > 9999)
							Item.M_Count = 9999;

						Item.M_Count += _Count;
					}
					LResult = true;
					break;
				}
			}

			if (!LResult)
			{
				//根据类型发送背包
				EquipmentData.M_Count = _Count;
				//申请ID
				UFVMGameInstance::GetPlayerStructManager_Static()->AddEquipmentToPlayerBag(EquipmentData);
				UFVMGameInstance::GetPlayerStructManager_Static()->M_PlayerItems_Equipment.Emplace(EquipmentData);
			}
		}
		else {
			for (int32 i = 0; i < _Count; i++)
			{
				//申请ID
				UFVMGameInstance::GetPlayerStructManager_Static()->AddEquipmentToPlayerBag(EquipmentData);
				UFVMGameInstance::GetPlayerStructManager_Static()->M_PlayerItems_Equipment.Emplace(EquipmentData);
			}
		}

		//添加装备内容
		UGameLogSubsystem::AddPlayerGetEquipmentLog(EquipmentData.ItemName, _Count, EquipmentData.M_EquipmentGrade);

		if (_bSave)
		{
			UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("发送装备：")) + EquipmentData.ItemName + TEXT("到背包"));
		}

		if (UFVMGameInstance::GetDebug())
		{
			UGameSystemFunction::FVMLog(__FUNCTION__, TEXT("发送装备：") + EquipmentData.ItemName + TEXT("到背包"));
		}

		return true;
	}

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__, TEXT("发送装备失败：") + EquipmentData.ItemName);
	}*/

	return false;
}

bool UGameSystemFunction::SendMaterToBag(int32 ID, int32 Level, int32 Count, bool bSave) {

	//材料没有等级，固定0
	Level = 0;

#if WITH_EDITOR
	FMaterialBase BaseData;
	if (UMaterialDataAssetCache::GetMaterData(ID, BaseData))
	{
		GAME_LOG(__FUNCTION__, TEXT("材料发送"), {
		FVMLOG(__FUNCTION__, FString(
			FString::FromInt(ID) +
			FString(
				TEXT("[") +
				BaseData.ItemName +
				TEXT("]")
			) +
			FString(
				TEXT("[") +
				FString(TEXT("等级:")) +
				FString::FromInt(Level) +
				TEXT("]")
			) +
			FString(
				TEXT("[") +
				FString(TEXT("数量:")) +
				FString::FromInt(Count) +
				TEXT("]")
			) +
			TEXT("发送到背包")
		));
			});

	}
	else {
		FVMLOG(__FUNCTION__, FString(
			FString::FromInt(ID) +
			TEXT("[当前装备不存在]")
		));
		return false;
	}
#endif

	//获取材料列表
	TMap<int32, FBaseItemSave>& Maters = GET_PLAYERDATAINS()->GetMaterList();
	FBaseItemSave* CurItemData = Maters.Find(ID);
	if (CurItemData)
	{
		CurItemData->AddItem(Level, Count);
	}
	else {
		FBaseItemSave NewItem;
		NewItem.AddItem(Level, Count);
		Maters.Emplace(ID, NewItem);
	}

	if (bSave)
	{
		FString Msg;
#if WITH_EDITOR
		Msg = __FUNCTION__ +
			FString(TEXT("新增材料[") +
				BaseData.ItemName +
				TEXT("]到背包,")) +

			FString(TEXT("ID:[")) +
			FString::FromInt(ID) +
			FString(TEXT("],")) +

			FString(TEXT("数量:[")) +
			FString::FromInt(Count) +
			FString(TEXT("],")) +

			FString(TEXT("等级:[")) +
			FString::FromInt(Level) +
			FString(TEXT("]"));
#endif
		UGameSystemFunction::SaveCurrentPlayerData(Msg);
	}

	return true;
}
int32 UGameSystemFunction::GetMaterAddedCount(int32 ID, int32 Level, int32 AddCount) {
	TMap<int32, FBaseItemSave>& List = GET_PLAYERDATAINS()->GetMaterList();
	FBaseItemSave* ItemSave = List.Find(ID);
	if (ItemSave)
	{
		//溢出个数
		long long Max = (ItemSave->GetItemCount(Level) + AddCount);
		if (Max > MAX_int32)
		{
			return (Max - MAX_int32) * -1.f;
		}

		//正常剩余
		return MAX_int32 - (ItemSave->GetItemCount(Level) + AddCount);
	}
	else {
		return MAX_int32;
	}
	return 0;
}
//发送指定材料到背包
bool UGameSystemFunction::SendMaterialToPlayerBag(const FString& _MaterialName, int32 _Count, bool _bSave)
{
	GAME_LOG(__FUNCTION__, TEXT("发送材料到玩家背包"), {
	GAME_LOG_LINE(TEXT("需要删除这个函数，因为有代替函数"));
		});
	/*
	if (_Count > 1000000000)
		_Count = 1000000000;

	if (_Count < 1)
		_Count = 1;

	FMaterialBase MaterialBaseData;
	if (UMaterialBaseStruct::SearchMaterailFromDataTable(_MaterialName, MaterialBaseData))
	{
		//查询结果
		bool LResult = false;
		//背包查询->是否有重复的->如果有则直接添加对应个数
		for (auto& Item : UFVMGameInstance::GetPlayerStructManager_Static()->M_PlayerItems_Material)
		{
			if (_MaterialName.Equals(Item.ItemName))
			{
				//个数判断
				if (Item.M_Count + _Count > 1000000000)
				{
					Item.M_Count = 1000000000;
				}
				else {
					Item.M_Count += _Count;
				}

				LResult = true;
				break;
			}
		}

		//查询失败
		if (!LResult)
		{
			//未查询到
			MaterialBaseData.M_Count = _Count;
			UFVMGameInstance::GetPlayerStructManager_Static()->M_PlayerItems_Material.Emplace(MaterialBaseData);
		}

		if (UFVMGameInstance::GetDebug())
		{
			UGameSystemFunction::FVMLog(__FUNCTION__, TEXT("发送材料：") + MaterialBaseData.ItemName + TEXT("到背包"));
		}

		UGameLogSubsystem::AddPlayerGetMaterialLog(MaterialBaseData.ItemName, _Count);

		//保存数据
		if (_bSave)
		{
			UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("发送材料：")) + MaterialBaseData.ItemName + TEXT("到背包"));
		}

		return true;
	}

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__, TEXT("发送材料失败：") + MaterialBaseData.ItemName);
	}
	*/
	return false;
}

//获取指向玩家存档卡片的数据(建立在加载存档之后)
void UGameSystemFunction::GetPlayerCardDatas(TArray<FItemCard*>& _datas, int32 _Count)
{
	int32 _Num_ = 0;
	for (auto& Item : UFVMGameInstance::GetPlayerStructManager_Static()->M_PlayerItems_Card)
	{
		_datas.Emplace(&Item);

		if (_Count > 0)
		{
			_Num_++;
			if (_Num_ >= _Count)
				break;
		}

	}
}

//获取玩家存档卡片数据的引用(建立在加载存档之后)
TArray<FItemCard>& UGameSystemFunction::GetPlayerCardDatasByRef()
{
	return UFVMGameInstance::GetPlayerStructManager_Static()->M_PlayerItems_Card;
}

FString UGameSystemFunction::GetItemGradePath(int32 _Grade)
{
	if (_Grade >= 1 && _Grade <= 16)
		return FString(TEXT("Texture2D'/Game/Resource/Texture/CardGrade/") + FString::FromInt(_Grade) + TEXT(".") + FString::FromInt(_Grade) + TEXT("'"));

	return FString();
}

void UGameSystemFunction::AddString(FString& _CurrentString, const FString& _Content)
{
	if (!_CurrentString.Contains(_Content))
		_CurrentString += _Content;
}


bool UGameSystemFunction::CheckStringIsValid(const FString str, const FString Reg)
{
	FRegexPattern Pattern(Reg);
	FRegexMatcher regMatcher(Pattern, str);
	regMatcher.SetLimits(0, str.Len());
	return regMatcher.FindNext();
}


class UWidgetBase* UGameSystemFunction::GetUserInterWidgetByClass(TSoftClassPtr<class UAssetCategoryName> ObjectType, FName Name)
{
	UGameUserInterfaceSubsystem* UIStaticClass = UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic();
	if (IsValid(UIStaticClass))
	{
		const FName& CurName = UIStaticClass->GetUserInterCategoryName(ObjectType);
		return UIStaticClass->GetUserInterInstance(CurName)->GetUI(Name);
	}

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(
			__FUNCTION__,
			FString(TEXT("UI子系统无效"))
		);
	}

	return nullptr;
}

class UWidgetBase* UGameSystemFunction::GetUserInterWidgetByName(GameDataCategoryName&& CategoryName, GameDataName&& Name)
{
	UGameUserInterfaceSubsystem* UIStaticClass = UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic();
	if (IsValid(UIStaticClass))
	{
		return UIStaticClass->GetUserInterInstance(CategoryName)->GetUI(Name);
	}

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(
			__FUNCTION__,
			FString(TEXT("UI子系统无效"))
		);
	}

	return nullptr;
}

class UClass* UGameSystemFunction::GetUserInterClassByClass(TSoftClassPtr<class UAssetCategoryName> ObjectType, FName Name)
{
	UGameUserInterfaceSubsystem* UIStaticClass = UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic();
	if (IsValid(UIStaticClass))
	{
		const FName& CurName = UIStaticClass->GetUserInterCategoryName(ObjectType);
		return UIStaticClass->GetUserInterInstance(CurName)->GetUIClass(Name);
	}

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(
			__FUNCTION__,
			FString(TEXT("UI子系统无效"))
		);
	}

	return nullptr;
}

class UClass* UGameSystemFunction::GetUserInterClassByName(GameDataCategoryName&& CategoryName, GameDataName&& Name)
{
	UGameUserInterfaceSubsystem* UIStaticClass = UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic();
	if (IsValid(UIStaticClass))
	{
		return UIStaticClass->GetUserInterInstance(CategoryName)->GetUIClass(Name);
	}

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(
			__FUNCTION__,
			FString(TEXT("UI子系统无效"))
		);
	}

	return nullptr;
}

UGameUserInterfaceInstance* UGameSystemFunction::GetUserInterInstanceByName(GameDataCategoryName&& Name)
{
	UGameUserInterfaceSubsystem* UIStaticClass = UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic();
	if (IsValid(UIStaticClass))
	{
		return UIStaticClass->GetUserInterInstance(Name);
	}

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(
			__FUNCTION__,
			FString(TEXT("UI子系统无效"))
		);
	}

	return nullptr;
}

bool UGameSystemFunction::AddTempUserInterInstanceByName(FName Name)
{
	UGameUserInterfaceSubsystem* UIStaticClass = UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic();
	if (IsValid(UIStaticClass))
	{
		return UIStaticClass->AddTempUserInterInstance(Name);
	}

	return false;
}

UGameUserInterfaceInstance* UGameSystemFunction::GetUserInterInstanceByClass(TSoftClassPtr<class UAssetCategoryName> ObjectType)
{
	UGameUserInterfaceSubsystem* UIStaticClass = UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic();
	if (IsValid(UIStaticClass))
	{
		const FName& CurName = UIStaticClass->GetUserInterCategoryName(ObjectType);
		return UIStaticClass->GetUserInterInstance(CurName);
	}

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(
			__FUNCTION__,
			FString(TEXT("UI子系统无效"))
		);
	}

	return nullptr;
}

UTimeLineClass* UTimeLineClass::MakeTimeLineClass()
{
	UTimeLineClass* Cur = NewObject<UTimeLineClass>();
	Cur->OnTimelineFloatEvent.BindUFunction(Cur, TEXT("TimeUpdate"));
	Cur->OnTimelineFinishedEvent.BindUFunction(Cur, TEXT("TimeFinish"));
	Cur->Timeline.SetTimelineFinishedFunc(Cur->OnTimelineFinishedEvent);
	return Cur;
}

void UTimeLineClass::AddCurve(
	TSoftObjectPtr<UCurveFloat> NewCurve,
	UObject* NewObj,
	TFunction<void(UTimeLineClass* TimeLine, UObject*, float)> NewUpdateFunc,
	TFunction<void(UTimeLineClass* TimeLine, UObject*)> NewFinishFunc
)
{
	UCurveFloat* Cur = NewCurve.Get();
	if (!IsValid(Cur))
	{
		Cur = UGameSystemFunction::LoadRes(NewCurve);
		if (!IsValid(Cur))
		{

			return;
		}
	}

	this->Timeline.AddInterpFloat(Cur, this->OnTimelineFloatEvent);
	this->Curves.Emplace(Cur);
	this->CurveFuncsUpdate.Emplace(NewUpdateFunc);
	this->CurveFuncsFinish.Emplace(NewFinishFunc);
	this->AllObjs.Emplace(NewObj);
}


void UTimeLineClass::Tick(const float& DeltaTime)
{
	this->Timeline.TickTimeline(DeltaTime);
}

void UTimeLineClass::Play()
{
	this->Timeline.Play();
	this->bStart = true;
}

void UTimeLineClass::Reverse()
{
	this->Timeline.Reverse();
	this->bStart = false;
}

void UTimeLineClass::PlayFromStart()
{
	this->Timeline.PlayFromStart();
	this->bStart = true;
}

void UTimeLineClass::ReverseFromEnd()
{
	this->Timeline.ReverseFromEnd();
	this->bStart = false;
}

void UTimeLineClass::Stop()
{
	this->Timeline.Stop();
}

void UTimeLineClass::End()
{
	if (this->GetStart())
	{
		this->PlayFromStart();
		this->Stop();
	}
	else {
		this->ReverseFromEnd();
		this->Stop();
	}
	this->TimeFinish();
}

bool UTimeLineClass::GetStart()
{
	return this->bStart;
}

void UTimeLineClass::TimeUpdate()
{
	for (int32 i = 0; i < Curves.Num(); i++)
	{
		if (IsValid(Curves[i]))
		{
			//执行
			this->CurveFuncsUpdate[i](this, this->AllObjs[i], Curves[i]->GetFloatValue(Timeline.GetPlaybackPosition()));
		}
	}
}

void UTimeLineClass::TimeFinish()
{
	for (int32 i = 0; i < Curves.Num(); i++)
	{
		if (IsValid(Curves[i]))
		{
			//执行
			this->CurveFuncsFinish[i](this, this->AllObjs[i]);
		}
	}
}
