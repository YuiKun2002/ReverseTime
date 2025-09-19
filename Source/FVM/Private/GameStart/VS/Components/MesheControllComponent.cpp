#include "GameStart/VS/Components\MesheControllComponent.h"
#include "GameStart/VS/Components/MapMesheWidgetComponent.h"
#include "GameStart/VS/Animal/VSAnimalBase.h"
#include "GameStart/VS/GameMapInstance.h"
#include "GameStart/VS/MapMeshe.h"
#include "GameSystem/GameMapStructManager.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameStart/Flipbook/SpriteActor.h"
#include "GameStart/MapLauncher/MapLauncherBase.h"
#include <Kismet/GameplayStatics.h>
#include "GameSystem/Tools/GameSystemFunction.h"


UMesheControllComponent::UMesheControllComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	this->SetTickableWhenPaused(true);
}

void UMesheControllComponent::GameStartInit(class AGameMapInstance* GameMapInstance)
{
	this->MapMouseMesheManagerClass = LoadClass<AMapMouseMesheManager>(0,
		TEXT("Blueprint'/Game/Resource/BP/GameStart/VS/AMapMouseManager.AMapMouseManager_C'"));

	//加载(地形)Actor
	this->MapMesheClass = LoadClass<AMapMeshe>(0,
		TEXT("Blueprint'/Game/Resource/BP/GameStart/VS/AMapMeshe.AMapMeshe_C'")
	);

	//加载精灵Actor类
	this->SpriteActorClass = LoadClass<ASpriteActor>(0, TEXT("Class'/Script/FVM.SpriteActor'"));

	//初始化地图管理器
	this->M_GameMapStructManager = GameMapInstance->GetGameMapStructManagerStatic();

	//初始化动物资源
	for (
		const TSoftClassPtr<class AVSAnimalBase>& Animal :
		this->M_GameMapStructManager->GameMapStruct.RowAnimalClass
		)
	{
		this->AnimalsClass.Emplace(
			Animal.LoadSynchronous()
		);
	}

	//初始化地图发射器资源
	this->MapLauncherClass.Emplace(
		this->M_GameMapStructManager->LevelConfig.LevelLauncherPath.TryLoadClass<AMapLauncherBase>()
	);


	this->InitGameMapMeshe();
}

void UMesheControllComponent::GameStart(class AGameMapInstance* GameMapInstance)
{
	//初始化地图发射器
	for (UClass*& MapLaun : this->MapLauncherClass)
	{
		//创建发射器
		AMapLauncherBase* TMapLauncher = Cast<AMapLauncherBase>(
			UGameplayStatics::BeginDeferredActorSpawnFromClass(this->GetWorld(),
				TSubclassOf<AMapLauncherBase>(MapLaun),
				FTransform()));

		if (TMapLauncher)
		{
			//设置关卡配置
			TMapLauncher->SetLevelConfig(this->M_GameMapStructManager->LevelConfig);
			//设置地图配置
			TMapLauncher->SetMapConfig(this->M_GameMapStructManager->GameMapStruct);
			//设置地图网格
			TMapLauncher->SetMapMesheControllComponent(this);
		}

		//完成生成
		AMapLauncherBase* MapLauncher = Cast<AMapLauncherBase>(
			UGameplayStatics::FinishSpawningActor(TMapLauncher, FTransform())
		);

		//设置地图发射器
		this->MapLaunchers.Emplace(MapLauncher);
	}

	//初始化动物
	int32 AnimalPathIndex = 0;
	for (UClass*& Animal : this->AnimalsClass)
	{
		if (AnimalPathIndex < this->M_GameMapStructManager->GameMapStruct.M_Meshe.Num())
		{
			FTransform LocalTrans = this->GetMapMeshCurrentLeft(AnimalPathIndex)->GetActorTransform();
			LocalTrans.SetLocation(LocalTrans.GetLocation() + FVector(0.f, -65.f, 0.f));

			AVSAnimalBase* CurAnim = this->GetWorld()->SpawnActor<AVSAnimalBase>(
				Animal
			);

			if (!CurAnim)
			{
				AnimalPathIndex++;
				continue;
			}

			this->Animals.Emplace(CurAnim);
			CurAnim->SetRow(AnimalPathIndex);
			CurAnim->SetActorTransform(LocalTrans);
			CurAnim->InitRotation();
			CurAnim->SetRenderLayer(this->GetMapMeshCurrentRight(AnimalPathIndex)->GetRenderLayer());

			FVMLOG(__FUNCTION__, FString(TEXT("创建动物:") +
				UKismetSystemLibrary::GetObjectName(CurAnim)));
		}

		AnimalPathIndex++;
	}
}

void UMesheControllComponent::GameTick(float DeltaTime)
{

}

void UMesheControllComponent::GameOverInit(class AGameMapInstance* GameMapInstance, bool bGameWin)
{

}

void UMesheControllComponent::GameOver(class AGameMapInstance* GameMapInstance, bool bGameWin)
{

}

void UMesheControllComponent::GameResourceUnload()
{
	this->MapMouseMesheManagerClass = nullptr;

	//释放动物
	for (AVSAnimalBase*& Anim : this->Animals)
	{
		if (IsValid(Anim))
		{
			Anim->Destroy();
		}
	}

	//释放发射器
	for (AMapLauncherBase*& MapLauncher : this->MapLaunchers)
	{
		if (IsValid(MapLauncher))
		{
			MapLauncher->Destroy();
		}
	}
}

UGameMapStructManager* const UMesheControllComponent::GetGameMapStructManager()
{
	return this->M_GameMapStructManager;
}

void UMesheControllComponent::ClearAllMeshes()
{
	this->M_CurMeshes.Empty();
	this->M_CurMeshesLocation.Empty();
	this->CurMapMouseMesheManager.Empty();
}


void UMesheControllComponent::SpawnMeshes()
{
	this->ClearAllMeshes();

	//初始化网格地图
	this->InitMeshesMap(this->M_GameMapStructManager->GameMapStruct.M_Meshe.Num(),
		this->M_GameMapStructManager->GameMapStruct.M_Meshe[0].M_Col.Num());

	//生成第一个网格位置
	FVector2D FristMeshPosition = this->M_GameMapStructManager->GameMapStruct.M_FirstRowMesheLocation;
	// 110是地图在场景的Z偏移。 -32是减去网格高度的一半。对齐中心点
	FristMeshPosition.Y += 110 - 32;

	//网格位置
	FTransform CurrentMesheLocation;

	float RowPadding = 0.f;
	//得到网格层渲染优先级【第Row层的优先级】
	int32 CurMesheTranslucency = GetRangeLayer(0, TranslucencyConst::ETranslucency::EMeshe);

	//初始化网格
	for (int32 Row = 0; Row < this->M_GameMapStructManager->GameMapStruct.M_Meshe.Num(); Row++)
	{
		for (int32 Col = 0; Col < this->M_GameMapStructManager->GameMapStruct.M_Meshe[Row].M_Col.Num(); Col++)
		{
			//设置坐标
			CurrentMesheLocation.SetLocation(
				FVector(550.f,
					FristMeshPosition.X + Col *
					this->M_GameMapStructManager->GameMapStruct.M_Meshe[Row].M_Col[Col].M_PaddingCol,
					FristMeshPosition.Y - RowPadding));

			//老鼠网格线路
			{
				//得到老鼠网格管理器
				AMapMouseMesheManager* CurMouseMapMesheManager =
					this->GetWorld()->SpawnActor<AMapMouseMesheManager>(
						this->MapMouseMesheManagerClass,
						CurrentMesheLocation);

				//设置线路
				CurMouseMapMesheManager->SetLine(FLine(Row, Col));
				//缓存
				this->CurMapMouseMesheManager.Emplace(CurMouseMapMesheManager);
			}

			//地图网格线路
			{
				UMesheControllComponent::CreateNewMapMeshe(
					this, CurrentMesheLocation,
					this->M_GameMapStructManager->GameMapStruct.M_Meshe[Row].M_Col[Col].M_ELineType,
					CurMesheTranslucency,
					FLine(Row, Col),
					this->M_GameMapStructManager->GameMapStruct.M_Meshe[Row].M_Col[Col].MesheColPrePlayCard,
					this->M_GameMapStructManager->GameMapStruct.M_Meshe[Row].M_Col[Col].M_MeshBG.M_bEnableBG,
					this->M_GameMapStructManager->GameMapStruct.M_Meshe[Row].M_Col[Col].M_MeshBG.M_BG_FullPath,
					this->M_GameMapStructManager->GameMapStruct.M_Meshe[Row].M_Col[Col].M_MeshBG.M_Offset
				);

				//最后一格的网格位置
				if (Col + 1 == this->M_GameMapStructManager->GameMapStruct.M_Meshe[Row].M_Col.Num())
				{
					//最后一格网格的位置
					this->M_LineLastLocation.Emplace(CurrentMesheLocation.GetLocation());
				}

			}

			//每列渲染优先级递增50
			CurMesheTranslucency += 50;
		}
		RowPadding += this->M_GameMapStructManager->GameMapStruct.M_Meshe[Row].M_PaddingRow;
	}

	//初始化移动网格
	for (int32 Row = 0; Row < this->M_GameMapStructManager->GameMapStruct.M_Meshe.Num(); Row++)
	{
		for (int32 Col = 0; Col < this->M_GameMapStructManager->GameMapStruct.M_Meshe
			[Row].M_Col.Num();
			Col++
			)
		{
			//当前网格是否支持移动
			if (this->M_GameMapStructManager->GameMapStruct.M_Meshe[Row].M_Col[Col].M_bMoveActive)
			{
				const FMesheCol& _Meshe = this->M_GameMapStructManager->GameMapStruct.M_Meshe
					[Row].M_Col[Col];
				this->GetMapMesh(Row, Col)->SetMesheMove(
					_Meshe.M_MoveingCloudDownTime,
					_Meshe.M_MoveingTime,
					_Meshe.M_MoveOffset,
					this->GetMapMesh(
						_Meshe.M_MoveOffset.Row,
						_Meshe.M_MoveOffset.Col
					)->GetActorLocation()
				);
			}
		}
	}


}

AMapMeshe* UMesheControllComponent::CreateNewMapMeshe(
	UMesheControllComponent* Comp,
	FTransform Trans,
	ELineType LineType,
	int32 Translucy,
	FLine Line,
	const TArray<FMesheColPrePlayCard>& PlayCard,
	bool bCreateBG,
	FSoftObjectPath BgPath,
	FVector BgOffset,
	bool bInitMeshe
)
{
	//得到地图网格
	AMapMeshe* M_TempMapMesh = Comp->GetWorld()->SpawnActor<AMapMeshe>(
		Comp->MapMesheClass,
		Trans);

	if (bInitMeshe)
	{
		//添加Actor到列表
		Comp->InitMesh(M_TempMapMesh, Trans.GetLocation());
	}

	//设置UI的旋转
	M_TempMapMesh->SetUUI_MapMeshRotatonPitchY(180.f);
	//设置行列
	M_TempMapMesh->SetLine(Line);
	//设置网格类型
	M_TempMapMesh->SetLineType(LineType);
	//初始化网格UI
	M_TempMapMesh->InitUUI_MapMesh(LineType, Translucy);
	//初始化预备放置卡片的数据
	M_TempMapMesh->SetPrePlayCard(PlayCard);
	//设置位置变化
	M_TempMapMesh->SetWidgetComponentTransform(Trans);

	//创建背景
	if (bCreateBG)
	{
		//创建背景
		ASpriteActor* Bg = Comp->GetWorld()->SpawnActor<ASpriteActor>(
			Comp->SpriteActorClass
		);
		if (Bg)
		{
			//设置精灵
			Bg->SetSprite(LoadObject<UPaperSprite>(0, *BgPath.ToString()));
			//设置偏移
			Bg->SetSpriteOffset(BgOffset);
			//设置坐标
			Bg->SetSpriteLocation(Trans.GetLocation(), true);
			//设置渲染优先级
			Bg->SetTranslucency(GetRangeLayer(0, TranslucencyConst::ETranslucency::EBgBlock));
			//将背景与网格关联
			M_TempMapMesh->SetMesheBg(Bg);
		}
	}

	return M_TempMapMesh;
}

AMapMeshe* UMesheControllComponent::ReplaceMapMeshe(FLine Line, ELineType LineType)
{
	//获取组件
	UMesheControllComponent* CurComp = AGameMapInstance::GetGameMapInstance()->GetMesheControllComponent();
	if (!IsValid(CurComp))
	{
		return nullptr;
	}

	AMapMeshe* SourceMeshe = CurComp->GetMapMesh(Line.Row, Line.Col);
	if (SourceMeshe->GetLineType() == LineType)
	{
		return SourceMeshe;
	}

	FTransform NewMesheTrans;
	NewMesheTrans.SetLocation(SourceMeshe->GetActorLocation());

	TArray<FMesheColPrePlayCard> EmptyCard;

	AMapMeshe* NewMeshe =
		UMesheControllComponent::CreateNewMapMeshe(
			CurComp, NewMesheTrans,
			LineType, SourceMeshe->GetRenderLayer(),
			Line, EmptyCard, false, FSoftObjectPath(), FVector::ZeroVector, false
		);
	NewMeshe->SetMesheBg(SourceMeshe->M_MesheBG);
	//复制防御卡存储
	NewMeshe->CopyUIMapMesheData(SourceMeshe, SourceMeshe->GetUIMapMeshe());
	//复制网格
	CurComp->SetMapMesh(NewMeshe, Line.Row, Line.Col);

	return NewMeshe;
}

void UMesheControllComponent::ReplaceAllMapMeshe(ELineType CurLineType, ELineType TargetLineType)
{
	//获取组件
	UMesheControllComponent* CurComp = AGameMapInstance::GetGameMapInstance()->GetMesheControllComponent();
	if (!IsValid(CurComp))
	{
		return;
	}

	for (int32 Row = 0; Row < CurComp->GetMapMeshRowAndCol().Row; Row++)
	{
		for (int32 Col = 0; Col < CurComp->GetMapMeshRowAndCol().Col; Col++)
		{
			AMapMeshe* SourceMeshe = CurComp->GetMapMesh(Row, Col);
			if (SourceMeshe->GetLineType() == CurLineType)
			{
				FTransform NewMesheTrans;
				NewMesheTrans.SetLocation(SourceMeshe->GetActorLocation());

				TArray<FMesheColPrePlayCard> EmptyCard;
				AMapMeshe* NewMeshe =
					UMesheControllComponent::CreateNewMapMeshe(
						CurComp, NewMesheTrans,
						TargetLineType, SourceMeshe->GetRenderLayer(),
						FLine(Row, Col), EmptyCard,
						false, FSoftObjectPath(), FVector::ZeroVector, false
					);
				NewMeshe->SetMesheBg(SourceMeshe->M_MesheBG);
				//复制防御卡存储
				NewMeshe->CopyUIMapMesheData(SourceMeshe, SourceMeshe->GetUIMapMeshe());
				//复制网格
				CurComp->SetMapMesh(NewMeshe, Row, Col);
			}
		}
	}
}

TArray<AActor*> UMesheControllComponent::GetAllMapMeshes()
{
	return this->M_CurMeshes;
}

AMapLauncherBase* UMesheControllComponent::GetMapLauncherByIndex(int32 Index)
{
	if (Index > -1 && Index < this->MapLaunchers.Num())
	{
		return this->MapLaunchers[Index];
	}

	return nullptr;
}

void UMesheControllComponent::SetMapMesh(AMapMeshe* NewMapMeshe, int32 _Row, int32 _Col)
{
	AActor*& CurMapMeshe = this->M_CurMeshes[_Row * this->M_CurMapLine.Col + _Col];
	if (IsValid(CurMapMeshe))
	{
		//移除显示UI
		AMapMeshe* CurDel = Cast<AMapMeshe>(CurMapMeshe);
		if (IsValid(CurDel) && IsValid(CurDel->GetUIMapMeshe()))
		{
			CurDel->M_MapMesheWidgetComponent->SetWidget(nullptr);
		}
		CurMapMeshe->Destroy();
		CurMapMeshe = nullptr;
	}

	//设置新的网格
	CurMapMeshe = Cast<AActor>(NewMapMeshe);
}

AMapMeshe* UMesheControllComponent::GetMapMesh(int32 _Row, int32 _Col)
{
	return this->GetMesh(_Row, _Col);
}

AMapMeshe* UMesheControllComponent::GetMapMeshByLine(int32 _Row, int32 _Col)
{
	for (auto CurM : this->M_CurMeshes)
	{
		if (IsValid(Cast<AMapMeshe>(CurM)->GetUIMapMeshe()))
		{
			FLine CurLine = Cast<AMapMeshe>(CurM)->GetUIMapMeshe()->GetLine();
			if (CurLine.Row == _Row && CurLine.Col == _Col)
			{
				return Cast<AMapMeshe>(CurM);
			}
		}
	}

	return nullptr;
}

AMapMeshe* UMesheControllComponent::GetMapMeshCurrentTop(int32 _Col)
{
	return this->GetMesh(0, _Col);
}

AMapMeshe* UMesheControllComponent::GetMapMeshCurrentBottom(int32 _Col)
{
	return this->GetMesh(this->GetMapMeshRowAndCol().Row - 1, _Col);
}

AMapMeshe* UMesheControllComponent::GetMapMeshCurrentLeft(int32 Row)
{
	return this->GetMesh(Row, 0);
}

AMapMeshe* UMesheControllComponent::GetMapMeshCurrentRight(int32 Row)
{
	return this->GetMesh(Row, this->GetMapMeshRowAndCol().Col - 1);
}

AMapMeshe* UMesheControllComponent::GetHasCardMeshe(int32 Row)
{
	for (int32 i = 0; i < this->M_CurMapLine.Col; i++)
	{
		AMapMeshe* CurMeshe = this->GetMesh(Row, i);
		if (IsValid(CurMeshe) && IsValid(CurMeshe->GetUIMapMeshe()))
		{
			if (CurMeshe->GetUIMapMeshe()->GetCardLayers().Num() > 0)
			{
				return CurMeshe;
			}
		}
	}

	return nullptr;
}

AMapMouseMesheManager* UMesheControllComponent::GetMapMouseMesh(int32 _Row, int32 _Col)
{
	if (_Row < 0 || _Col < 0 || _Row >= this->GetMapMeshRowAndCol().Row || _Col >= this->GetMapMeshRowAndCol().Col)
	{
		return nullptr;
	}

	return this->CurMapMouseMesheManager[_Row * this->M_CurMapLine.Col + _Col];
}

AMapMouseMesheManager* UMesheControllComponent::GetMapMouseMeshCurrentTop(int32 _Col)
{
	return this->CurMapMouseMesheManager[_Col];
}

AMapMouseMesheManager* UMesheControllComponent::GetMapMouseMeshCurrentBottom(int32 _Col)
{
	return this->CurMapMouseMesheManager[this->GetMapMeshRowAndCol().Row - 1 * this->M_CurMapLine.Col + _Col];
}

AMapMouseMesheManager* UMesheControllComponent::GetMapMouseMeshCurrentLeft(int32 Row)
{
	return this->CurMapMouseMesheManager[Row * this->M_CurMapLine.Col];
}

AMapMouseMesheManager* UMesheControllComponent::GetMapMouseMeshCurrentRight(int32 Row)
{
	return this->CurMapMouseMesheManager[Row * this->M_CurMapLine.Col + this->GetMapMeshRowAndCol().Col - 1];
}

void UMesheControllComponent::InitGameMapMeshe()
{
	this->ClearAllMeshes();

	this->SpawnMeshes();
}

void UMesheControllComponent::InitMeshesMap(const int32& _Row, const int32& _Col)
{
	this->M_CurMapLine = FLine(_Row, _Col);
}

void UMesheControllComponent::InitMesh(AActor* _Mesh, const FVector& _MeshLocation)
{
	this->M_CurMeshes.Emplace(_Mesh);
	this->M_CurMeshesLocation.Emplace(_MeshLocation);
}

AMapMeshe* UMesheControllComponent::GetMesh(const int32& _Row, const int32& _Col)
{
	return Cast<AMapMeshe>(this->M_CurMeshes[_Row * this->M_CurMapLine.Col + _Col]);
}

FVector UMesheControllComponent::GetMapMeshLocation(const int32& _Row, const int32& _Col)
{
	if (_Col < 0 || _Row < 0)
	{
		return FVector::ZeroVector;
	}

	return this->M_CurMeshesLocation[_Row * this->M_CurMapLine.Col + _Col];
}

FVector UMesheControllComponent::GetMapMesheLastLocationByIndex(int32 Row)
{
	return this->M_LineLastLocation[Row];
}

FLine UMesheControllComponent::GetMapMeshRowAndCol()
{
	return this->M_CurMapLine;
}

int32 UMesheControllComponent::GetMesheCount() const
{
	return this->M_CurMeshes.Num();
}

bool UMesheControllComponent::CheckMesheIsValid(const int32& _Row, const int32& _Col)
{
	if (_Row * this->M_CurMapLine.Col + _Col < this->GetMesheCount())
	{
		return true;
	}

	return false;
}
