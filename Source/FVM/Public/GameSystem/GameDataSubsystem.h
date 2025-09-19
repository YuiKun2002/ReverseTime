// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "UObject/Interface.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameSystem/GameDataName.h" //修改资产名称
#include "GameSystem/DataTableAssetData.h"//表格数据
#include "GameDataSubsystem.generated.h"

//获取UI的方法
// UGameSystemFunction::GetUserInterClassByName ......

/*常用获取道具的函数*/
//UCardDataAssetCache::GetData()	获取卡片数据



/* 常用宏数据获取  */
//GET_CACHE()						获取缓存类型 UCache*，缓存数据
//GET_DATA()						获取数据表格类型 DataTableAssetData<>，临时数据
//GET_DATA_ARRAY()					获取数据表中的行数据 TArray<T>，临时数据
//GET_ITEM()						通过ID号获取对应的物品数据，缓存数据
//CAST_TYPE()						转换一个结构体类型，返回的结果是不带指针的
//CAST_TYPE_PTR()					转换一个结构体类型，返回一个带指针类型的

/*常用名称获取*/
//GET_DEF_CATEGORYNAME()			获取一个静态[分类名称]，不带MoveTemp()
//GET_MOVE_CATEGORYNAME()			获取一个静态[分类名称]
//GET_DEF_NAME()					获取一个静态[名称]，不带MoveTemp()
//GET_MOVE_NAME()					获取一个静态[名称]
//GET_UI_CATEGORYNAME()				获取一个静态[分类名称]，适用于UI，不带MoveTemp()
//GET_UI_NAME()						获取一个静态[名称]，适用于UI，不带MoveTemp()

/*常用名称声明*/
//DEF_CATEGORYNAME()				创建一个静态[分类名称]
//DEF_NAME()						创建一个静态[名称]

/*常用的UI操作*/
//GET_UI_CLASS()					获取当前UI的UClass*实例
//GET_UI_CAST()						获取当前UI的实例对象，转换后，可以显示无需手动创建，并且唯一实例
//GET_UI()							获取当前UI的实例对象UWidgetBase*父对象实例，可以显示无需手动创建，并且唯一实例
//CREATE_UI()						创建一个全新的UI实例，并不会被管理器管理

/*游戏数据对象*/
//MAKE_GAMEDATAOBJECT()				创建一个游戏数据对象

/*其他*/
//GAMEINS()							获取全局游戏实例，GameInstance*
//GET_PLAYERDATAINS()				获取当前玩家的本地存档实例，UPlayerStructManager*
//GAME_LOG()						打印一个带有开始和结束提示的Log
//GAME_LOG_LINE()					打印一行Log信息



//获取临时数据表 DataTableAssetData<StructType>
#define GET_DATA(StructType,CategoryName,Name)\
_GetTempDataTableAssetData_<StructType>(\
GET_MOVE_CATEGORYNAME(CategoryName),\
GET_MOVE_NAME(CategoryName, Name)\
)\

//获取临时数据表数组
#define GET_DATA_ARRAY(StructType,CategoryName,Name)\
_GetTempDataTableAssetData_Array_<StructType>(\
GET_MOVE_CATEGORYNAME(CategoryName),\
GET_MOVE_NAME(CategoryName, Name)\
)\

//获取数据表数据
#define GET_SOURCE_DATA(StructType,Array,StructTable,CategoryName,Name)\
GetDataTableSourceData<StructType>(\
Array,\
StructTable,\
GET_MOVE_CATEGORYNAME(CategoryName),\
GET_MOVE_NAME(CategoryName, Name)\
)\

//获取数据表数据
#define GET_SOURCE_DATA_NO_MOVE(StructType,Array,StructTable,CategoryName,Name)\
GetDataTableSourceData<StructType>(\
Array,\
StructTable,\
GET_MOVE_CATEGORYNAME(CategoryName),\
Name\
)\

/*
	获取缓存数据
1.资产缓存类型
2.缓存类型的名称，例如：UTextureDataAssetCache 那么名称就是 Texture
返回就是：卡片类型的缓存类
*/
#define GET_CACHE(CategoryName)\
GetGameDataAssetCache_Temp<U##CategoryName##DataAssetCache>(GET_MOVE_CATEGORYNAME(CategoryName))\
\

/*
获取物品数据
@Param1 道具的ID号
@Param2 返回对应的类型
@Param3 获取返回的结构数据(数据表源数据类型，例如：FCard_ATK_Data)
@Param4 返回对应的分类名称
*/
#define GET_ITEM(ID,OutType,OutStructTypeData,OutRef_FName_CATEGORYNAME)\
_GetItemByID_(ID,OutType,OutStructTypeData,OutRef_FName_CATEGORYNAME)\
\

/*结构体类型转换，带指针*/
#define CAST_TYPE_PTR(Type,Ptr) ((Type*)(Ptr))
/*结构体类型转换，不带指针*/
#define CAST_TYPE(Type,Ptr) (*CAST_TYPE_PTR(Type,Ptr))

////查询数据
//#define FINDDATABYID(ID,OutData,MapSourceData) FindDataByID(ID,OutData,MapSourceData)
////添加数据
//#define ADDDATABYID(ID,OutData,MapSourceData) AddDataByID(ID,OutData,MapSourceData)
////移除数据
//#define REMOVEDATABYID(ID,MapSourceData) RemoveDataByID(ID,MapSourceData)

////物品表格ID结构
//USTRUCT(BlueprintType)
//struct FItemTableID {
//	GENERATED_USTRUCT_BODY()
//protected:
//	//可使用的ID号
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	TSet<int32> CanUseID;
//	//已经被使用的ID号
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	TSet<int32> CurUseID;
//	//最大ID号
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	int32 MaxID = 0;
//protected:
//	int32 GetID() { return -1; }
//
//
//};

#define MAKE_GAMEDATAOBJECT(Type)\
{	\
UGameDataSubsystem* Sub = UGameDataSubsystem::GetGameDataSubsystemStatic();	\
if(Sub)\
{\
	Sub->AddGameDataObject(NewObject<Type>());\
}\
}	\
\

//标题打印
#define TITLE_LOG(Content) UE_LOG(LogTemp,Warning,TEXT("------------------[%s]------------------"),*FString(Content))

//打印一行
#define GAME_LOG_LINE(Content, ...) UE_LOG(LogTemp,Log,Content, ##__VA_ARGS__)

//游戏日志打印
#define GAME_LOG(FuncName,TitleName,Code)\
{\
UE_LOG(LogTemp,Log,TEXT(" \n"))\
UE_LOG(LogTemp,Log,TEXT(" \n"))\
TITLE_LOG(FuncName);\
TITLE_LOG(FString(FString(TitleName)+TEXT("[开始]")));\
Code;\
TITLE_LOG(FString(FString(TitleName)+TEXT("[结束]")));\
UE_LOG(LogTemp,Log,TEXT(" \n"))\
UE_LOG(LogTemp,Log,TEXT(" \n"))\
}\
\

//获取物品id函数
#define GETITEMBYIDFUNC(Name)\
{\
	OutNameFlag = GET_MOVE_CATEGORYNAME(Name);\
	UGameDataSubsystem* TempGameDataSubsystem = UGameDataSubsystem::GetGameDataSubsystemStatic();\
	if (!IsValid(TempGameDataSubsystem))\
	{\
		return false;\
	}\
	\
	UGameDataAssetCache* Cache = TempGameDataSubsystem->GetGameDataAssetCache(OutNameFlag);\
	if (IsValid(Cache))\
	{\
		IFindItemInterface* Inter = Cast<IFindItemInterface>(Cache);\
		if (Inter)\
		{\
			if (Inter->GetItemByID(ItemID, OutType, OutData))\
			{\
				return true;\
			}\
		}\
	}\
}\
\

////时间测试，记录当前逻辑运行时间
//#define TIME_TEST(Name,Code) \
//{\
//double ThisTime##Name = 0.f;\
//{\
//	SCOPE_SECONDS_COUNTER(ThisTime##Name);\
//	Code;\
//	UE_LOG(LogTemp, Log, TEXT("当前逻辑的运行时间 %.2f ms"), ThisTime##Name);\
//}}\
//

/*
	静态游戏对象-类型静态单例
	1.可以直接通过内部函数获取数据子系统
	2.通过 GameDataStaticObject<Type>() 即可获取对象
	3.统一结构，继承自UGameDataStaticObject就可以使用数据子系统
	4.自带编译期检查
	5.不建议直接使用 XXXX::StaticObject()->  不方便区分和进行数据管理
*/
UCLASS(BlueprintType, Blueprintable)
class FVM_API UGameDataStaticObject : public UObject {

	GENERATED_BODY()

	friend class UGameDataSubsystem;

public:

	//获取数据子系统
	UFUNCTION(BlueprintPure)
	UGameDataSubsystem* GetSubsystem();

protected:
	//初始化
	virtual void Initialize();
	//卸载
	virtual void DeInitialize();
private:
	UPROPERTY()
	class UGameDataSubsystem* GameDataSub = nullptr;
};


//可视化名字类型分类[蓝图实现，全局的宏]
UCLASS(BlueprintType, Blueprintable)
class FVM_API UAssetCategoryName : public UObject {
	GENERATED_BODY()
public:
	//获取分类名称
	UFUNCTION(BlueprintImplementableEvent)
	FName GetCategoryName();
};

//游戏数据资产【全局唯一数据表结构】
UCLASS(BlueprintType, Blueprintable)
class FVM_API UGameDataAsset : public UDataAsset
{
	GENERATED_BODY()

	friend class UGameDataSubsystem;

public:
	//全部数据
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "数据集合")
	TMap<FName, TSoftObjectPtr<class UGameDataAssetObject>> Datas;
public:
	UFUNCTION(BlueprintPure, Category = "获取数据资产对象")
	class UGameDataAssetObject* GetDataByName(FName Name);
	class UGameDataAssetObject* GetDataByName_Imp(GameDataCategoryName&& Name);
private:
	//已经完成加载的数据资产对象
	UPROPERTY()
	TMap<FName, class UGameDataAssetObject*> DataInstance;
};

//数据资产对象【存储多个不同类型的数据表】
UCLASS(BlueprintType, Blueprintable)
class FVM_API UGameDataAssetObject : public UDataAsset
{
	GENERATED_BODY()
public:
	//全部数据
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "数据集合")
	TMap<FName, TSoftObjectPtr<UDataTable>> Datas;
public:
	UFUNCTION(BlueprintPure, Category = "获取数据表")
	UDataTable* GetDataByName(FName Name);
	UDataTable* GetDataByName_Imp(GameDataName&& Name);
	//卸载
	UFUNCTION()
	void Unload();
private:
	//已经完成加载的数据表格
	UPROPERTY()
	TMap<FName, UDataTable*> DataInstance;
};

//数据资产对象接口
UINTERFACE(MinimalAPI, Blueprintable)
class UFindItemInterface : public UInterface
{
	GENERATED_BODY()
};
class FVM_API IFindItemInterface
{
	GENERATED_BODY()

	//查询Map数据表
#define FIND_MAP_BODY_FINDINTERFACE(Map)\
{auto TargetData = Map.Find(ItemID);\
if (TargetData)\
{\
OutData = TargetData;\
return true;\
}}\
break\
\

//填充Map数据表
#define GETITEMBYID_IMP(Map,StructTable,CategoryName,Name)\
FillDataTable(\
Map,\
StructTable,\
GET_MOVE_CATEGORYNAME(CategoryName),\
GET_MOVE_NAME(CategoryName, Name)\
);\
FIND_MAP_BODY_FINDINTERFACE(Map)\
\

//声明一个可以 获取全部的物品【名称】和【ID】的函数
#define DECLAREGETALLITEMDATAFUNC(FuncName,CategoryName)\
template<typename MapType>\
void FuncName##Func(\
	TMap<int32, MapType>& InputMap,\
	DataTableAssetData<MapType>& Table,\
	FName&& Name,\
	TMap<int32, FString>& OutMap) {\
	FillDataTable(InputMap, Table, GET_MOVE_CATEGORYNAME(CategoryName), MoveTemp(Name));\
	for (auto TempData : InputMap)\
	{\
		FItemBase TempBaseData;\
		TempData.Value.GetSourceData(TempBaseData);\
		OutMap.Emplace(TempData.Key, TempBaseData.ItemName);\
	}\
}\
\

public:
	//当前接口的名称
	virtual	const FName GetClassTag() = 0;
	//通过id获取数据
	virtual bool GetItemByID(int32 ItemID, uint8& OutType, void*& OutData) = 0;
	//获取所有的ID和名称
	virtual void GetAllItemID(TMap<int32, FString>& OutData) = 0;
};



// #include "GameSystem/GameDataSubsystem.h"
//缓存对象->持续到关卡被卸载
UCLASS(BlueprintType, Blueprintable)
class FVM_API UGameDataAssetCache : public UObject
{
	GENERATED_BODY()
public:
	//通过软类生成一个资产缓存
	UFUNCTION(BlueprintPure)
	static UGameDataAssetCache* MakeGameDataAssetCache(
		TSoftClassPtr<UGameDataAssetCache> BpGameDataAssetCache
	);
public:

	//初始化缓存
	UFUNCTION(BlueprintNativeEvent)
	void InitCache();
	virtual void InitCache_Implementation();

	//蓝图名称
	UFUNCTION(BlueprintImplementableEvent)
	FName GenName();

	//卸载数据
	UFUNCTION(BlueprintNativeEvent)
	void Unload();
	virtual void Unload_Implementation();
};



// DataTableAssetData<T>
// FTableRowBase
// #include "Engine/DataTable.h"

//数据表结构继承
USTRUCT(BlueprintType)
struct FVM_API FGameDataAssetCacheTempStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
};

// UCardDataAssetCache
// UEquipmentDataAssetCache
//继承缓存模板
UCLASS(BlueprintType, Blueprintable)
class FVM_API UGameDataAssetCacheTemp : public UGameDataAssetCache
{
	GENERATED_BODY()
public:

	//返回对应类型的数据，如果没有初始化则自行检测并且初始化数据
	TArray<FGameDataAssetCacheTempStruct> GetDataTable()
	{
		//使用方法例子如下
		/*return GET_SOURCE_DATA(FGameDataAssetCacheTempStruct,
			this->TempDataTable,
			this->TempSourceDataTable,
			GameVS,
			Buff
		);*/

		return this->TempDataTable;
	}

	//初始化数据表的其他方法，需要自行判断是否已经初始化
	void InitDataTableOtherFunc() {
		/*
		TArray<FGameDataAssetCacheTempStruct> LoadData;
		auto TempInitData = [&](GameDataName&& Name) {
			UGameSystemFunction::GetDataTableRows(
				UGameDataSubsystem::GetGameDataSubsystemStatic()->GetAsset()->
				GetDataByName(GET_MOVE_CATEGORYNAME(GameVS))->GetDataByName(Name), LoadData);
			};

		TempInitData(GET_MOVE_NAME(GameVS,Buff));
		*/
	}

private:
	//原生数据表
	DataTableAssetData<FGameDataAssetCacheTempStruct> TempSourceDataTable;
	//加载的数据表缓存数据
	UPROPERTY()
	TArray<FGameDataAssetCacheTempStruct> TempDataTable;
};

/**
 * 游戏数据子系统
 */
UCLASS()
class FVM_API UGameDataSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	friend class UGameDataAsset;
	friend class UGameDataStaticObject;

	/*
	*
	全局资产【包括一下全部】：
	1.一个【名字】对应一个【数据资产】
	2.一个【数据资产】对应n个【数据资产对象】
	3.【数据资产对象】对应n个带有【唯一名字】的数据表

	数据资产：可以看成一个游戏中全部的核心数据，可以通过名字细分多个【数据资产对象】

	数据资产对象：可以看成一个细分大类，一个大类中可以细分多个具体的【数据表资产】

	数据表资产：数据表资产可以是不同的类型，于是获取数据表中的具体数据，就需要知道对应的数据表结构体。才能拿到对应存储的数据。


	数据缓存对象：(通过一个类封装对各种数据表中的数据进行处理和缓存，获取时不会进行转换，也不会二次加载数据表)
	1.通常的使用方法是将【同一个分类】的【全部数据表】进行一个【集合】
	2.并且【提供一个】可以【直接获取】数据表【结构转换后】的函数
	3.使用时，通过子系统获取，到当前缓存对象。即可使用当前缓存对象中的全部数据【无需做结构转换】
	4.如果子系统无法获取到，需要在合适的时机，对子系统进行缓存对象的添加
	5.便捷函数提供  GetGameDataAssetCache<T>  【获取】【添加】一体
	*/


	/*
	获取数据表数据的两种方法：

	1.通过资产获取

		1.
			DataTable*	 UGameDataSubsystem::GetGameDataSubsystemStatic()->GetAsset()->GetDataByName_Imp(
					Name
				)->GetDataByName_Imp(
					Name2
				);

		2.
			UDataTable*  UGameDataSubsystem::GetGameDataSubsystemStatic()->
				GetGameDataAssetDataTableByName_Imp(
					Name,
					Name2
				);

		3.
			DataTableAssetData<T>(UDataTable*)   接受一个数据表，可以快速进行数据结构的转换，并且获取内容

	2.通过缓存对象获取

		1.
			T* GetGameDataAssetCache<T>(Name);

			例子：
				UCardDataAssetCache* CardCache = GetGameDataAssetCache<UCardDataAssetCache>(Name);

				//以下缓存函数
				TArray<FCard_ATK_Data>&	CardCache->GetATKCard();
				TArray<FCard_Function_Data>& CardCache->GetFunctionCard();




	*/
public:
	UFUNCTION(BlueprintPure, Category = "游戏数据子系统")
	static UGameDataSubsystem* GetGameDataSubsystemStatic();

	//获取全局唯一资产，可以直接获取对应数据表【但是数据表结构需要自己转换，数据缓存需要自己处理】
	UFUNCTION(BlueprintPure, Category = "游戏数据子系统 | 获取资产")
	UGameDataAsset* GetAsset();

	//添加缓存对象【关卡被卸载时，自动清理缓存，数据缓存由子系统管理】
	UFUNCTION(BlueprintCallable, Category = "游戏数据子系统 | 缓存对象")
	void AddGameDataAssetCache(FName Name, UGameDataAssetCache* CacheIns);
	void AddGameDataAssetCache_Imp(GameDataCategoryName&& Name, UGameDataAssetCache* CacheIns);
	//获取缓存对象【关卡被卸载时，自动清理缓存，数据缓存由子系统管理】
	UFUNCTION(BlueprintPure, Category = "游戏数据子系统 | 缓存对象")
	UGameDataAssetCache* GetGameDataAssetCache(FName Name);
	UGameDataAssetCache* GetGameDataAssetCache_Imp(GameDataCategoryName&& Name);
	//获取所有的缓存实例
	UFUNCTION(BlueprintPure, Category = "游戏数据子系统 | 缓存对象")
	void GetAllGameDataAssetCache(TArray<UGameDataAssetCache*>& OutDataAssetCache);
	//获取缓存对象的数据表【关卡被卸载时，自动清理缓存，数据缓存需要自己处理】
	UFUNCTION(BlueprintPure, Category = "游戏数据子系统 | 缓存对象")
	UDataTable* GetGameDataAssetDataTableByName(TSubclassOf<UAssetCategoryName> DataCategoryName, TSubclassOf<UAssetCategoryName> Name);
	UDataTable* GetGameDataAssetDataTableByName_Imp(GameDataCategoryName&& DataCategoryName, GameDataName&& Name);
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
private:
	//游戏数据
	UPROPERTY()
	UGameDataAsset* GameDataAsset = nullptr;
	//资产缓存
	UPROPERTY()
	TMap<FName, UGameDataAssetCache*> GameDataAssetCache;
};

//获取临时数据表
template<class T>
DataTableAssetData<T> _GetTempDataTableAssetData_(FName&& Name1, FName&& Name2)
{
	UDataTable* TEMPDATATABLE = UGameDataSubsystem::GetGameDataSubsystemStatic()->
		GetAsset()->GetDataByName_Imp(MoveTemp(Name1))->GetDataByName_Imp(MoveTemp(Name2));
	return DataTableAssetData<T>(TEMPDATATABLE);
}

//获取临时数据表数组返回
template<class T>
TArray<T> _GetTempDataTableAssetData_Array_(FName&& Name1, FName&& Name2)
{
	UDataTable* TEMPDATATABLE = UGameDataSubsystem::GetGameDataSubsystemStatic()->
		GetAsset()->GetDataByName_Imp(MoveTemp(Name1))->GetDataByName_Imp(MoveTemp(Name2));
	TArray<T> Temp;
	DataTableAssetData<T>(TEMPDATATABLE).GetValues(Temp);
	return Temp;
}

//获取游戏数据资产缓存【模板】
template<class CacheClassType>
CacheClassType* GetGameDataAssetCache_Temp(GameDataCategoryName&& Name)
{
	UGameDataSubsystem* TempGameDataSubsystem = UGameDataSubsystem::GetGameDataSubsystemStatic();
	if (!IsValid(TempGameDataSubsystem))
	{
		return nullptr;
	}

	UGameDataAssetCache* CurCache = TempGameDataSubsystem->GetGameDataAssetCache_Imp(
		MoveTemp(Name)
	);
	if (IsValid(CurCache))
	{
		CacheClassType* CurCacheType = Cast<CacheClassType>(CurCache);
		if (IsValid(CurCacheType))
		{
			return CurCacheType;
		}
	}

	CacheClassType* CurCacheType = NewObject<CacheClassType>();
	CurCacheType->InitCache();
	TempGameDataSubsystem->AddGameDataAssetCache_Imp(MoveTemp(Name), CurCacheType);

	return CurCacheType;
}

//查询物品ID函数，白名单
bool GetItemByiDFunc(const int32& ItemID, uint8& OutType, void*& OutData, FName& OutNameFlag);

////查询物品ID
template<typename TableStructType>
bool _GetItemByID_(const int32& ItemID, uint8& OutType, TableStructType*& OutData, FName& OutNameFlag) {
	if (OutNameFlag.IsEqual(FName()))
	{
		void* OutDataPtr = nullptr;
		//获取数据
		if (GetItemByiDFunc(ItemID, OutType, OutDataPtr, OutNameFlag))
		{
			OutData = (TableStructType*)(OutDataPtr);
			return true;
		}
	}
	else {
		UGameDataSubsystem* TempGameDataSubsystem = UGameDataSubsystem::GetGameDataSubsystemStatic();
		if (!IsValid(TempGameDataSubsystem))
		{
			return false;
		}

		UGameDataAssetCache* Cache = TempGameDataSubsystem->GetGameDataAssetCache(OutNameFlag);
		if (IsValid(Cache))
		{
			IFindItemInterface* Inter = Cast<IFindItemInterface>(Cache);
			if (Inter)
			{
				void* OutDataPtr = nullptr;
				if (Inter->GetItemByID(ItemID, OutType, OutDataPtr))
				{
					OutData = ((TableStructType*)(OutDataPtr));
					return true;
				}
			}
		}

	}

	return false;
}

//获取继承自 UGameDataStaticObject 对象的静态实例
template<typename GameDataStaticObjectType>
struct GameDataStaticObject {

	//GameDataStaticObject<int> A = GameDataStaticObject<int>();
	/*GameDataStaticObject<GameDataStaticObjectType> operator()() {
		return *this;
	}*/

	//隐式转换
	operator GameDataStaticObjectType* () {
		return GameDataStaticObject<GameDataStaticObjectType>::Get();
	}
	//重写 -> 运算符
	GameDataStaticObjectType* operator->() {
		return GameDataStaticObject<GameDataStaticObjectType>::Get();
	}
private:
	static GameDataStaticObjectType* Get() {
#if WITH_EDITOR
		static_assert(std::is_base_of<UGameDataStaticObject, GameDataStaticObjectType>::value,
			"This structure cannot use this type ! Must be a derived class of UGameDataStaticObject type"
			);
#endif
		return Cast<GameDataStaticObjectType>(
			GameDataStaticObjectType::StaticClass()->GetDefaultObject()
		);
	}
};
