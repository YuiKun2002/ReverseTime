// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameSystem/GameDataName.h" //修改资产名称
#include "GameSystem/GameDataSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameSystem/Item/Task/TaskStructBase.h"
#include "GameUserInterfaceSubsystem.generated.h"

class UWidgetBase;
class UAssetCategoryName;

class UClass* GetUserInterClass(GameDataCategoryName&& CategoryName, GameDataName&& Name);
/*
获取UI Class，获取当前UI的加载的Class*对象
@Param1 UI分类Tag
@Param2 UI名称
返回 UClass类型
*/
#define GET_UI_CLASS(CategoryName,Name)\
	GetUserInterClass(\
	MoveTemp(GET_UI_CATEGORYNAME(CategoryName)),\
	MoveTemp(GET_UI_NAME(CategoryName,Name))\
)\


class UWidgetBase* GetUserInterWidget(GameDataCategoryName&& CategoryName, GameDataName&& Name);
/*
获取UI 实例,如果当前UI没有被创建，则自动创建一次[被管理器管理唯一实例]
@Param1 UI类型Class
@Param2 UI分类Tag
@Param3 UI名称
返回 UI类型Class
*/
#define GET_UI_CAST(ClassType,CategoryName,Name)\
	Cast<ClassType>(GetUserInterWidget(\
	MoveTemp(GET_UI_CATEGORYNAME(CategoryName)),\
	MoveTemp(GET_UI_NAME(CategoryName,Name)))\
)\

/*
获取UI 实例(UWidgetBase),如果当前UI没有被创建，则自动创建一次[被管理器管理唯一实例]
@Param1 UI分类Tag
@Param2 UI名称
返回 UWidgetBase类型
*/
#define GET_UI(CategoryName,Name)\
GET_UI_CAST(UWidgetBase,CategoryName,Name)\


/*
创建UI Instance，创建一个全新的UI，被界面基础类实例自动管理
@Param1 界面基础类实例
@Param2 UI类型Class
@Param3 UI分类Tag
@Param4 UI名称
返回 UI类型Class

生命周期依附于 WidgetBaseClass

*/
#define CREATE_UI(WidgetBaseClass,ClassType,CategoryName,Name)\
UGameUserInterfaceSubsystem::CreateWidgetIns<ClassType>(\
GET_UI_CATEGORYNAME(CategoryName),GET_UI_NAME(CategoryName,Name),WidgetBaseClass,GET_UI_CLASS(CategoryName,Name))\

/*
创建UI Instance，创建一个全新的UI,需要手动管理，不会被WidgetBaseClass实例所管理
@Param1 UI类型Class
@Param2 UI分类Tag
@Param3 UI名称
返回 UI类型Class
*/
#define CREATE_UI_WIDGET(ClassType,CategoryName,Name)\
UGameUserInterfaceSubsystem::CreateWidgetIns<ClassType>(\
GET_UI_CATEGORYNAME(CategoryName),GET_UI_NAME(CategoryName,Name),GET_UI_CLASS(CategoryName,Name))\


//UI结构体
USTRUCT(BlueprintType)
struct FGameUserInterfaceTableRowBase : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	//更新的类条目【完全不建议后期修改名称】
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, TSoftClassPtr<UWidgetBase>> UI;
};

//UI集合结构体
USTRUCT(BlueprintType)
struct FGameUserInterfaceGroupTableRowBase : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	//UI的名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName UserInterName;

	//UI的资产
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UWidgetBase> UserInterSource;
};

//UI实例
UCLASS()
class FVM_API UGameUserInterfaceInstance : public UObject {
	GENERATED_BODY()

	friend class UWidgetBase;

public:
	//UI List Source 对应UI的软类资产引用
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, TSoftClassPtr<UWidgetBase>> UI;
	//UI Insatnce 已经加载的UI实例对象集合
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, UWidgetBase*> InstanceUI;
	// UI Instance Name
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName InterfaceName = FName();
public:
	//新增临时UI，游戏结束释放，不会被保存
	UFUNCTION(BlueprintCallable, Category = "新增UI")
	bool AddTempNewUI(FName NewName, TSoftClassPtr<UWidgetBase> Resource);

	//通过名称获取UI
	UFUNCTION(BlueprintPure, Category = "获取UI")
	UWidgetBase* GetUI(FName Name);
	UWidgetBase* GetUI_Imp(GameDataName&& Name);

	//通过名称获取UI的class
	UFUNCTION(BlueprintPure, Category = "获取UI")
	UClass* GetUIClass(FName Name);
	//通过名称获取已经被实例化的UI
	UFUNCTION(BlueprintPure, Category = "获取UI")
	UWidgetBase* GetInstanceUI(FName Name);
	//查询显示的UI
	UFUNCTION(BlueprintPure, Category = "获取UI")
	UWidgetBase* GetIsInViewportUI_ByName(FName Name);

	//通过名称默认【无函数调用】显示UI
	UFUNCTION(BlueprintCallable, Category = "显示UI")
	void ShowUI_ByName(FName Name, int32 ZOrder = 0);
	void ShowUI_ByName_Imp(GameDataName&& Name, int32 ZOrder = 0);

	//通过对象直接显示UI
	UFUNCTION(BlueprintCallable, Category = "显示UI")
	bool ShowUI(UWidgetBase* WidgetInstance, int32 ZOrder = 0);

	//通过名称默认【无函数调用】移除显示UI
	UFUNCTION(BlueprintCallable, Category = "移除UI显示")
	void RemoveUI_ByName(FName Name);
	void RemoveUI_ByName_Imp(GameDataName&& Name);

	//通过对象直接移除显示UI
	UFUNCTION(BlueprintCallable, Category = "移除UI显示")
	void RemoveUI(UWidgetBase* WidgetInstance);

	/*
		通过名称直接释放UI
		@Param1 UI的唯一名称
		@Param2 不管是否在显示，强行释放
	*/
	UFUNCTION(BlueprintCallable, Category = "释放UI")
	bool UnloadUI_ByName(FName Name, bool bForceUnload = false);
	//释放所有没有显示的UI资源
	UFUNCTION(BlueprintCallable, Category = "释放UI")
	void UnloadAllNotShowResource();
	//强制释放全部的UI资源【如果有显示的UI也会被关闭】
	UFUNCTION(BlueprintCallable, Category = "释放UI")
	void ForceUnloadAllResource();

	//【系统结束】释放
	UFUNCTION()
	void Unload();
};

/**
 * 全局UI子系统
 */
UCLASS()
class FVM_API UGameUserInterfaceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class UWidgetBase;

public:
	//数据初始化
	UFUNCTION()
	void InitializeUserInterData(class UGameDataAsset* DataAsset);

	//获取子系统
	UFUNCTION(BlueprintPure, Category = "UI子系统")
	static UGameUserInterfaceSubsystem* GetGameUserInterfaceSubsystemStatic();
	//获取名称
	UFUNCTION(BlueprintPure, Category = "UI子系统 | 获取UI")
	FName GetUserInterCategoryName(TSoftClassPtr<UAssetCategoryName> ObjectType);
	//通过Name获取UI实例【使用前请确保Name有效】
	UFUNCTION(BlueprintPure, Category = "UI子系统 | 获取UI")
	UGameUserInterfaceInstance* GetUserInterInstance(FName Name);
	//新增临时UI实例,游戏结束释放，不会被保存
	UFUNCTION(BlueprintCallable, Category = "UI子系统 | 新增UI")
	bool AddTempUserInterInstance(FName Name);
	//直接移除UI实例并且释放当前实例的所有UI
	UFUNCTION(BlueprintCallable, Category = "UI子系统 | 移除UI")
	bool RemoveUserInterInstance(FName Name);
	//直接释放全部界面并且未显示的UI
	UFUNCTION(BlueprintCallable, Category = "UI子系统 | 释放UI")
	void UnloadAllNotShowResource();
	//直接释放全部的界面
	UFUNCTION(BlueprintCallable, Category = "UI子系统 | 释放UI")
	void UnloadAllResource();
public:
	//辅助函数-计时器GC
	void ForceGC();
	//辅助函数-检查是否是管理器管理的界面
	static bool CheckWidget(class UWidget* Ins);
	//辅助函数-界面基础添加-带有父界面
	static void WidgetBaseAddChild(FName CWidgetName, FName WidgetName, class UWidgetBase* Owner, class UObject*& NewClass, class UClass* Class);
	template <typename WidgetT = UUserWidget>
	static WidgetT* CreateWidgetIns(FName CWidgetName, FName WidgetName, class UWidgetBase* Owner, UClass* Class) {
		UObject* NewWidget = nullptr;
		UGameUserInterfaceSubsystem::WidgetBaseAddChild(CWidgetName, WidgetName, Owner, NewWidget, Class);
		return Cast<WidgetT>(NewWidget);
	}
	//辅助函数-界面基础添加
	static void WidgetBaseAddChild(FName CWidgetName, FName WidgetName, class UObject*& NewClass, class UClass* Class);
	template <typename WidgetT = UUserWidget>
	static WidgetT* CreateWidgetIns(FName CWidgetName, FName WidgetName, UClass* Class) {
		UObject* NewWidget = nullptr;
		UGameUserInterfaceSubsystem::WidgetBaseAddChild(CWidgetName, WidgetName, NewWidget, Class);
		return Cast<WidgetT>(NewWidget);
	}
	//强制释放界面对象，计时器释放（5s延迟）
	template<class Type = UWidgetBase>
	static void ForceUnloadWidget(Type* Instance, bool bForceGC = false)
	{
		if (IsValid(Instance))
		{
#if WITH_EDITOR
			if (Cast<class UWidgetBase>(Instance) || Cast<class UWidget>(Instance))
			{
				UGameSystemFunction::FVMLog(
					__FUNCTION__,
					FString::Printf(TEXT("[%p]"), Instance) +
					FString(TEXT("UI：")) +
					FString(TEXT("被标记为垃圾对象"))
				);
			}
			else {
				UGameSystemFunction::FVMLog(
					__FUNCTION__,
					FString::Printf(TEXT("[%p]"), Instance) +
					FString(TEXT("Object：")) +
					FString(TEXT("被标记为垃圾对象"))
				);
			}
#endif
			//如果此界面没有被子系统管理，则自行标记
			if (!UGameUserInterfaceSubsystem::CheckWidget(Cast<UWidget>(Instance)))
			{
				Instance->MarkAsGarbage();
				Instance = nullptr;
			}

			if (bForceGC)
			{
				UGameUserInterfaceSubsystem* Sym = UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic();
				if (IsValid(Sym))
				{
					Sym->ForceGC();
				}
			}
		}

	}
	//强制释放界面对象，计时器释放（5s延迟）
	template<class Type = TArray<UWidgetBase*>>
	static void ForceUnloadWidgets(Type& Widgets)
	{
		bool bGC = false;
		for (auto& Widget : Widgets)
		{
			if (IsValid(Cast<UWidget>(Widget)))
			{
				bGC = true;
#if WITH_EDITOR
				UGameSystemFunction::FVMLog(
					__FUNCTION__,
					FString::Printf(TEXT("[%p]"), Widget) +
					FString(TEXT("UI：")) +
					FString(TEXT("被标记为垃圾对象"))
				);
#endif
				//如果此界面没有被子系统管理，则自行标记
				if (!UGameUserInterfaceSubsystem::CheckWidget(Widget))
				{
					Widget->MarkAsGarbage();
					Widget = nullptr;
				}
			}
		}

		if (bGC)
		{
			UGameUserInterfaceSubsystem* Sym = UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic();
			if (IsValid(Sym))
			{
				Sym->ForceGC();
			}
		}
	}
	//强制释放界面对象，计时器释放（5s延迟）
	template<class Type = TArray<UWidgetBase*>>
	static void ForceUnloadWidgets(const Type& Widgets)
	{
		bool bGC = false;
		for (auto& Widget : Widgets)
		{
			if (IsValid(Cast<UWidget>(Widget)))
			{
				bGC = true;
#if WITH_EDITOR
				UGameSystemFunction::FVMLog(
					__FUNCTION__,
					FString::Printf(TEXT("[%p]"), Widget) +
					FString(TEXT("UI：")) +
					FString(TEXT("被标记为垃圾对象"))
				);
#endif

				//如果此界面没有被子系统管理，则自行标记
				if (!UGameUserInterfaceSubsystem::CheckWidget(Widget))
				{
					Widget->MarkAsGarbage();
				}
			}
		}

		if (bGC)
		{
			UGameUserInterfaceSubsystem* Sym = UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic();
			if (IsValid(Sym))
			{
				Sym->ForceGC();
			}
		}
	}

public:

	/*
		其他处理的UI，便捷调用函数
	*/

	//显示情报岛UI
	UFUNCTION(BlueprintCallable, Category = "UI子系统 | 情报岛UI")
	static void ShowGameInformationUISub(class UWidget* Widget);

	//显示任务UI
	UFUNCTION(BlueprintCallable, Category = "UI子系统 | 任务UI")
	static void ShowGameTaskUISub(class UWidget* Widget);
	//任务UI是否显示
	UFUNCTION(BlueprintCallable, Category = "UI子系统 | 任务UI")
	static bool GetGameTaskUIInViewportSub();
	//关闭任务UI的显示
	UFUNCTION(BlueprintCallable, Category = "UI子系统 | 任务UI")
	static void RemoveGameTaskUIViewportSub();
	//显示任务完成提示UI
	UFUNCTION(BlueprintCallable, Category = "UI子系统 | 任务UI")
	static void ShowTaskFinishTipUISub(class UWidget* Widget, class UGameInstance* Ins = nullptr);

	//显示新关卡UI
	UFUNCTION(BlueprintCallable, Category = "UI子系统 | 新增关卡提示UI")
	static void ShowNewLevelTipUISub(class UWidget* Widget);

	//显示新得到的物品UI提示
	UFUNCTION(BlueprintCallable, Category = "UI子系统 | 新增物品提示UI")
	static void ShowNewItemShowPanel(class UWidget* Widget, const TArray<FTaskItemStruct>& Data);

	//显示剧情进度UI
	UFUNCTION(BlueprintCallable, Category = "UI子系统 | 剧情进度UI")
	static void ShowDialogueInforUISub(class UWidget* Widget);

public:
	//初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//取消初始化
	virtual void Deinitialize() override;
public:
	//全部的UI资源
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, UGameUserInterfaceInstance*> Resource;
	//计时器
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle GCTimer;
	//计时器状态
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStart = false;
};