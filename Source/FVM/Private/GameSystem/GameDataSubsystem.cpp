// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除
#include "GameSystem/GameDataSubsystem.h"
#include "Data/GameLogSubsystem.h"
#include "Data/MaterialDataStruct.h"
#include "Data/EquipmentDataSturct.h"
#include "GameSystem/FVMGameInstance.h"
#include "Data/CardData/CardDataStruct.h"
#include "GameSystem/GameAssetSubsystem.h"
#include "GameSystem/GameUserInterfaceSubsystem.h"

//#if WITH_EDITOR
//#include "Subsystems/AssetEditorSubsystem.h"
//#endif

void UGameDataSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	//初始化游戏数据
	if (IsValid(UGameAssetSubsystem::GetGameAssetSubsystemStatic()))
	{
		this->GameDataAsset =
			Cast<UGameDataAsset>(
				UGameAssetSubsystem::GetGameAssetSubsystemStatic()->GetMainGameAsset()->
				GameData_DataTable.LoadSynchronous()
			);
	}

	if (IsValid(this->GameDataAsset))
	{
		//初始化UI子系统
		if (IsValid(UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic()))
		{
			UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic()->InitializeUserInterData(
				this->GameDataAsset
			);
		}
	}

	//初始化白名单

	// 指令缓存
	GET_CACHE(Instruction);
	// 卡片缓存
	GET_CACHE(Card);
	// 装备缓存
	GET_CACHE(Equip);
	// 材料缓存
	GET_CACHE(Material);

	UE_LOG(LogTemp, Warning, TEXT("游戏数据子系统，数据已经加载，UWorld启动"))
}

//初始化
void UGameDataStaticObject::Initialize() {
#if WITH_EDITOR
	GAME_LOG(__FUNCTION__, TEXT("游戏对象初始化"), {
		GAME_LOG_LINE(TEXT("Ptr：%p"), this);
		});
#endif
}
//卸载
void UGameDataStaticObject::DeInitialize() {
#if WITH_EDITOR
	GAME_LOG(__FUNCTION__, TEXT("游戏对象卸载"), {
		GAME_LOG_LINE(TEXT("Ptr：%p"), this);
		});
#endif
}

UGameDataSubsystem* UGameDataStaticObject::GetSubsystem() {
	if (this->GameDataSub)
	{
		return this->GameDataSub;
	}

	this->GameDataSub = UGameDataSubsystem::GetGameDataSubsystemStatic();

	return this->GameDataSub;
}

UDataTable* UGameDataAssetObject::GetDataByName(FName Name)
{
	return this->GetDataByName_Imp(MoveTemp(Name));
}

UDataTable* UGameDataAssetObject::GetDataByName_Imp(GameDataName&& Name)
{
	UDataTable** TargetTable = this->DataInstance.Find(Name);
	if (TargetTable && IsValid(*TargetTable))
	{
		return *TargetTable;
	}

	TSoftObjectPtr<UDataTable>* TargetSource = this->Datas.Find(Name);
	if (TargetSource)
	{
		UDataTable* NewData = TargetSource->LoadSynchronous();
		if (IsValid(NewData))
		{
			this->DataInstance.Emplace(Name, NewData);

			return NewData;
		}
	}

#if WITH_EDITOR
	UE_LOG(LogTemp, Error, TEXT("[UGameDataAssetObject::GetDataByName_Imp]数据表加载失败，数据表无效"));
#endif

	return nullptr;
}

void UGameDataAssetObject::Unload()
{
	for (auto& Instance : this->DataInstance)
	{
		if (IsValid(Instance.Value))
		{
			Instance.Value = nullptr;
		}
	}

	this->DataInstance.Empty();
}

UGameDataAssetObject* UGameDataAsset::GetDataByName(FName Name)
{
	return this->GetDataByName_Imp(MoveTemp(Name));
}

UGameDataAssetObject* UGameDataAsset::GetDataByName_Imp(GameDataCategoryName&& Name)
{
	UGameDataAssetObject** TargetTable = this->DataInstance.Find(Name);
	if (TargetTable && IsValid(*TargetTable))
	{
		return *TargetTable;
	}

	TSoftObjectPtr<UGameDataAssetObject>* TargetSource = this->Datas.Find(Name);
	if (TargetSource)
	{
		UGameDataAssetObject* NewData = TargetSource->LoadSynchronous();
		if (IsValid(NewData))
		{
			this->DataInstance.Emplace(Name, NewData);
			return NewData;
		}
	}

	return nullptr;
}

UGameDataAssetCache* UGameDataAssetCache::MakeGameDataAssetCache(TSoftClassPtr<UGameDataAssetCache> BpGameDataAssetCache)
{
	UClass* TargetClass = BpGameDataAssetCache.LoadSynchronous();
	if (IsValid(TargetClass))
	{
		TSubclassOf<UGameDataAssetCache> SubClass(TargetClass);
		UGameDataAssetCache* CurrentCache = SubClass.GetDefaultObject();
		if (IsValid(CurrentCache))
		{
			UGameDataSubsystem* TempGameDataSubsystem = UGameDataSubsystem::GetGameDataSubsystemStatic();
			UGameDataAssetCache* CurCache = TempGameDataSubsystem->GetGameDataAssetCache_Imp(
				CurrentCache->GenName()
			);
			if (IsValid(CurCache))
			{
				return CurCache;
			}

			CurrentCache->InitCache();
			TempGameDataSubsystem->AddGameDataAssetCache_Imp(CurrentCache->GenName(), CurrentCache);
			return CurrentCache;
		}
	}

	return nullptr;
}

void UGameDataAssetCache::InitCache_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("[UGameDataAssetCache,%p]：调用了C++的默认初始化函数"), this);
}

void UGameDataAssetCache::Unload_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("[UGameDataAssetCache,%p]：调用了C++的默认资产缓存卸载函数"), this);
}

UGameDataSubsystem* UGameDataSubsystem::GetGameDataSubsystemStatic()
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		if (IsValid(UFVMGameInstance::GetFVMGameInstance()->GetWorld()))
		{
			return UFVMGameInstance::GetFVMGameInstance()->GetWorld()->GetSubsystem<UGameDataSubsystem>();
		}
	}

	return nullptr;
}

UGameDataAsset* UGameDataSubsystem::GetAsset()
{
	return this->GameDataAsset;
}

void UGameDataSubsystem::AddGameDataAssetCache(FName Name, UGameDataAssetCache* CacheIns)
{
	this->AddGameDataAssetCache_Imp(MoveTemp(Name), CacheIns);
}

void UGameDataSubsystem::AddGameDataAssetCache_Imp(GameDataCategoryName&& Name, UGameDataAssetCache* CacheIns)
{
	UGameDataAssetCache** TargetIns = this->GameDataAssetCache.Find(Name);
	if (!TargetIns)
	{
		this->GameDataAssetCache.Emplace(Name, CacheIns);
	}
}

UGameDataAssetCache* UGameDataSubsystem::GetGameDataAssetCache(FName Name)
{
	return this->GetGameDataAssetCache_Imp(MoveTemp(Name));
}

UDataTable* UGameDataSubsystem::GetGameDataAssetDataTableByName(TSubclassOf<UAssetCategoryName> DataCategoryName, TSubclassOf<UAssetCategoryName> Name)
{
	FName A = DataCategoryName.GetDefaultObject()->GetCategoryName();
	FName B = Name.GetDefaultObject()->GetCategoryName();
	return this->GetGameDataAssetDataTableByName_Imp(
		MoveTemp(A),
		MoveTemp(B)
	);
}

UDataTable* UGameDataSubsystem::GetGameDataAssetDataTableByName_Imp(GameDataCategoryName&& DataCategoryName, GameDataName&& Name)
{
	//获取资产中的缓存
	UGameDataAssetObject* Cache = this->GameDataAsset->GetDataByName_Imp(
		MoveTemp(DataCategoryName)
	);

	if (IsValid(Cache))
	{
		return (Cache)->GetDataByName_Imp(MoveTemp(Name));
	}

	return nullptr;
}

UGameDataAssetCache* UGameDataSubsystem::GetGameDataAssetCache_Imp(GameDataCategoryName&& Name)
{
	UGameDataAssetCache** TargetIns = this->GameDataAssetCache.Find(Name);
	if (TargetIns && IsValid(*TargetIns))
	{
		return *TargetIns;
	}
	return nullptr;
}

void UGameDataSubsystem::GetAllGameDataAssetCache(TArray<UGameDataAssetCache*>& OutDataAssetCache) {
	this->GameDataAssetCache.GenerateValueArray(OutDataAssetCache);
}

void UGameDataSubsystem::Deinitialize()
{

	//卸载UI子系统数据
	if (IsValid(UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic()))
	{
		UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic()->UnloadAllResource();
	}

	if (IsValid(this->GameDataAsset))
	{
		for (auto& Data : this->GameDataAsset->DataInstance)
		{
			if (IsValid(Data.Value))
			{
				Data.Value->Unload();
			}
		}

		this->GameDataAsset->DataInstance.Empty();
	}


	for (auto& Data : this->GameDataAssetCache)
	{
		if (IsValid(Data.Value))
		{
			Data.Value->Unload();
			Data.Value = nullptr;
		}
	}
	this->GameDataAssetCache.Empty();

	UE_LOG(LogTemp, Warning, TEXT("游戏数据子系统，数据已经被卸载，UWorld卸载"))
}

bool GetItemByiDFunc(const int32& ItemID, uint8& OutType, void*& OutData, FName& OutNameFlag)
{
	//初始化白名单[卡片]
	GETITEMBYIDFUNC(Card);
	//初始化白名单[装备]
	GETITEMBYIDFUNC(Equip);
	//初始化白名单[材料]
	GETITEMBYIDFUNC(Material);

	return false;
}
