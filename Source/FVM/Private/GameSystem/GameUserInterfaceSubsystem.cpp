// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/GameUserInterfaceSubsystem.h"
#include "GameSystem/GameAssetSubsystem.h"
#include "GameSystem/GameDataSubsystem.h"
#include "GameSystem/FVMGameInstance.h"
#include "Game/UI/UI_GameInformation.h"
#include "Game/UI/UI_DialogueInfor.h"
#include "GameSystem/TaskSubsystem.h"
#include "Game/UI/NewItemShowPanel.h"
#include "Game/UI/UI_MainFramework.h"
#include "Game/UI/UI_GameTask.h"

UClass* GetUserInterClass(GameDataCategoryName&& CategoryName, GameDataName&& Name) {
	return UGameSystemFunction::GetUserInterClassByName(
		MoveTempIfPossible(CategoryName),
		MoveTempIfPossible(Name));
}

UWidgetBase* GetUserInterWidget(GameDataCategoryName&& CategoryName, GameDataName&& Name) {
	return UGameSystemFunction::GetUserInterWidgetByName(
		MoveTempIfPossible(CategoryName),
		MoveTempIfPossible(Name));
}

bool UGameUserInterfaceInstance::AddTempNewUI(FName NewName, TSoftClassPtr<UWidgetBase> Resource)
{
	UWidgetBase** ResultByClass = this->InstanceUI.Find(NewName);
	if (ResultByClass)
	{
		if (IsValid(*ResultByClass))
		{
			if (UFVMGameInstance::GetDebug())
			{
				UGameSystemFunction::FVMLog(
					__FUNCTION__,
					NewName.ToString() + FString(TEXT("UI：")) + FString(TEXT("已存在！无法重新添加！"))
				);
			}

			return false;
		}
	}

	TSoftClassPtr<UWidgetBase>* ResultBySoftClass = this->UI.Find(NewName);
	if (ResultBySoftClass)
	{
		if (UFVMGameInstance::GetDebug())
		{
			UGameSystemFunction::FVMLog(
				__FUNCTION__,
				NewName.ToString() + FString(TEXT("UI：")) + FString(TEXT("已存在！无法重新添加！"))
			);
		}

		return false;
	}

	//新增UI
	this->UI.Emplace(NewName, Resource);

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(
			__FUNCTION__,
			NewName.ToString() + FString(TEXT("UI：")) + FString(TEXT("新增UI资源"))
		);
	}

	return true;
}

UWidgetBase* UGameUserInterfaceInstance::GetUI(FName Name)
{
	return this->GetUI_Imp(MoveTemp(Name));
}

UWidgetBase* UGameUserInterfaceInstance::GetUI_Imp(GameDataName&& Name)
{
	//从实例池中查询UI
	UWidgetBase** ResultWidget = this->InstanceUI.Find(Name);

	if (ResultWidget && IsValid(*ResultWidget))
	{
		return *ResultWidget;
	}


	//从资源池中加载UI
	TSoftClassPtr<UWidgetBase>* Result = this->UI.Find(Name);
	if (Result)
	{
		UClass* LoadRes = Result->LoadSynchronous();
		if (LoadRes)
		{
			UWidgetBase* NewWidget = CreateWidget<UWidgetBase>(GAMEINS(), LoadRes);
			if (IsValid(NewWidget))
			{
				if (UFVMGameInstance::GetDebug())
				{
					UGameSystemFunction::FVMLog(
						__FUNCTION__,
						Name.ToString() + FString(TEXT("UI：")) + FString(TEXT("被加载！"))
					);
				}

				//添加UI实例
				NewWidget->InstanceWidgetName = Name;
				NewWidget->InstanceWidgetCategoryName = this->InterfaceName;
				this->InstanceUI.Emplace(Name, NewWidget);

				return NewWidget;
			}
		}
	}

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(
			__FUNCTION__,
			Name.ToString() + FString(TEXT("UI：")) + FString(TEXT("资源不存在，无法被加载！"))
		);
	}

	return nullptr;
}

UClass* UGameUserInterfaceInstance::GetUIClass(FName Name)
{
	TSoftClassPtr<UWidgetBase>* Result = this->UI.Find(Name);
	if (Result)
	{
		UClass* WidClass = Result->LoadSynchronous();
		if (IsValid(WidClass))
		{
			if (UFVMGameInstance::GetDebug())
			{
				UGameSystemFunction::FVMLog(
					__FUNCTION__,
					Name.ToString() + FString(TEXT("UI Class：")) + FString(TEXT("被加载！"))
				);
			}
		}
		return WidClass;
	}

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(
			__FUNCTION__,
			Name.ToString() + FString(TEXT("UI：")) + FString(TEXT("资源不存在，无法被加载！"))
		);
	}

	return nullptr;
}

UWidgetBase* UGameUserInterfaceInstance::GetInstanceUI(FName Name)
{
	UWidgetBase** TargetUI = this->InstanceUI.Find(Name);

	if (TargetUI)
	{
		if (IsValid(*TargetUI))
		{
			return *TargetUI;
		}
	}

	return nullptr;
}

void UGameUserInterfaceInstance::ShowUI_ByName(FName Name, int32 ZOrder)
{
	this->ShowUI(this->GetUI(Name), ZOrder);
}

void UGameUserInterfaceInstance::ShowUI_ByName_Imp(GameDataName&& Name, int32 ZOrder)
{
	this->ShowUI(this->GetUI(Name), ZOrder);
}

bool UGameUserInterfaceInstance::ShowUI(UWidgetBase* WidgetInstance, int32 ZOrder /*= 0*/)
{
	if (IsValid(WidgetInstance))
	{
		if (WidgetInstance->IsInViewport())
		{
			return false;
		}
		else {
			WidgetInstance->AddToViewport(ZOrder);
			return true;
		}
	}

	return false;
}

void UGameUserInterfaceInstance::RemoveUI_ByName(FName Name)
{
	this->RemoveUI_ByName_Imp(MoveTemp(Name));
}

void UGameUserInterfaceInstance::RemoveUI_ByName_Imp(GameDataName&& Name)
{
	this->RemoveUI(this->GetIsInViewportUI_ByName(Name));
}

void UGameUserInterfaceInstance::RemoveUI(UWidgetBase* WidgetInstance)
{
	if (IsValid(WidgetInstance))
	{
		WidgetInstance->RemoveFromParent();
	}
}

bool UGameUserInterfaceInstance::UnloadUI_ByName(FName Name, bool bForceUnload /*= false*/)
{
	UWidgetBase** TargetUI = this->InstanceUI.Find(Name);
	if (TargetUI)
	{
		if (IsValid(*TargetUI))
		{
			if ((*TargetUI)->IsInViewport())
			{
				(*TargetUI)->RemoveFromParent();
			}
			this->InstanceUI.Remove(Name);

			UGameUserInterfaceSubsystem::ForceUnloadWidget((*TargetUI), bForceUnload);

			return true;
		}
	}

#if WITH_EDITOR
	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(
			__FUNCTION__,
			Name.ToString() + FString(TEXT("UI：")) + FString(TEXT("未被加载，无需标记"))
		);
	}
#endif

	return false;
}

void UGameUserInterfaceInstance::UnloadAllNotShowResource()
{
	TMap<FName, UWidgetBase*> TmepInstanceUI;
	TArray<UWidgetBase*> Wids;
	for (auto& TempUI : this->InstanceUI)
	{
		if (IsValid(TempUI.Value))
		{
			if (TempUI.Value->IsInViewport())
			{
				TmepInstanceUI.Emplace(TempUI.Key, TempUI.Value);
			}
			else {
				Wids.Emplace(TempUI.Value);

			}
		}
	}

	UGameUserInterfaceSubsystem::ForceUnloadWidgets(Wids);

	this->InstanceUI.Empty();
	this->InstanceUI.Append(TmepInstanceUI);
}

void UGameUserInterfaceInstance::ForceUnloadAllResource()
{
	TArray<UWidgetBase*> Wds;
	this->InstanceUI.GenerateValueArray(Wds);
	UGameUserInterfaceSubsystem::ForceUnloadWidgets(Wds);
	this->InstanceUI.Empty();
}

UWidgetBase* UGameUserInterfaceInstance::GetIsInViewportUI_ByName(FName Name)
{
	UWidgetBase* TargetUI = this->GetInstanceUI(Name);

	if (IsValid(TargetUI))
	{
		if (TargetUI->IsInViewport())
		{
			return TargetUI;
		}
	}

	return nullptr;
}

void UGameUserInterfaceInstance::Unload()
{
	//卸载数据表
	this->UI.Empty();
	//卸载UI实例
	this->InstanceUI.Empty();
}

UGameUserInterfaceSubsystem* UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic()
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		return UFVMGameInstance::GetFVMGameInstance()->GetSubsystem<UGameUserInterfaceSubsystem>();
	}

	return nullptr;
}

FName UGameUserInterfaceSubsystem::GetUserInterCategoryName(TSoftClassPtr<UAssetCategoryName> ObjectType)
{
	UClass* CurClass = ObjectType.LoadSynchronous();
	if (IsValid(CurClass))
	{
		TSubclassOf<UAssetCategoryName> CurFunClass(CurClass);
		if (CurFunClass.GetDefaultObject())
		{
			return CurFunClass.GetDefaultObject()->GetCategoryName();
		}
	}

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__, TEXT("ObjectType 获取的{名称无效}"));
	}

	return FName();
}

UGameUserInterfaceInstance* UGameUserInterfaceSubsystem::GetUserInterInstance(FName Name)
{
	UGameUserInterfaceInstance** TargetIns = this->Resource.Find(Name);

	if (TargetIns)
	{
		if (IsValid(*TargetIns))
		{
			return *TargetIns;
		}
	}

	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__, TEXT("Name {无效}"));
	}

	return nullptr;
}

bool UGameUserInterfaceSubsystem::AddTempUserInterInstance(FName Name)
{
	UGameUserInterfaceInstance* OldInstance = this->GetUserInterInstance(Name);

	if (IsValid(OldInstance))
	{

#if WITH_EDITOR
		if (UFVMGameInstance::GetDebug())
		{
			UGameSystemFunction::FVMLog(__FUNCTION__,
				FString(TEXT("新增UI实例失败 ，")) +
				Name.ToString() +
				FString(TEXT("已存在！"))
			);
		}
#endif

		return false;
	}

	//创建新的UI实例对象
	UGameUserInterfaceInstance* NewUI = NewObject<UGameUserInterfaceInstance>();
	this->Resource.Emplace(Name, NewUI);

#if WITH_EDITOR
	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__,
			FString(TEXT("新增UI实例成功 ，实例名称：")) +
			Name.ToString()
		);
	}
#endif

	return true;
}

bool UGameUserInterfaceSubsystem::RemoveUserInterInstance(FName Name)
{
	UGameUserInterfaceInstance** Ins = this->Resource.Find(Name);
	if (Ins)
	{
		if (IsValid(*Ins))
		{
			(*Ins)->ForceUnloadAllResource();
			UGameUserInterfaceSubsystem::ForceUnloadWidget((*Ins));
			this->Resource.Remove(Name);
			return true;
		}
	}
#if WITH_EDITOR
	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__,
			FString(TEXT("UI实例标记失败 ，实例名称：")) +
			Name.ToString()
		);
	}
#endif

	return false;
}

void UGameUserInterfaceSubsystem::UnloadAllNotShowResource()
{
	for (const auto& Res : this->Resource)
	{
		if (IsValid(Res.Value))
		{
			Res.Value->UnloadAllNotShowResource();
		}
	}
}

void UGameUserInterfaceSubsystem::UnloadAllResource()
{
	for (const auto& Res : this->Resource)
	{
		if (IsValid(Res.Value))
		{
			Res.Value->ForceUnloadAllResource();
		}
	}
}

void UGameUserInterfaceSubsystem::ForceGC() {

	return;

	if (IsValid(this->GetWorld()))
	{
		if (!this->bStart)
		{
			this->GetWorld()->GetTimerManager().SetTimer(this->GCTimer, [this]() {
#if WITH_EDITOR
				UGameSystemFunction::FVMLog(
					__FUNCTION__,
					FString(TEXT("强行进行垃圾回收"))
				);
#endif

				this->bStart = false;

				this->GetWorld()->GetTimerManager().ClearTimer(this->GCTimer);

				GEngine->ForceGarbageCollection(true);

				}, 5.f, false);

			this->bStart = true;
		}

	}
}

bool UGameUserInterfaceSubsystem::CheckWidget(UWidget* Ins) {
	UGameUserInterfaceSubsystem* Sym = UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic();
	if (IsValid(Sym))
	{
		UWidgetBase* Base = Cast<UWidgetBase>(Ins);
		UGameUserInterfaceInstance* Instance = Sym->GetUserInterInstance(Base->InstanceWidgetCategoryName);
		if (IsValid(Instance))
		{
			UWidgetBase** Target = Instance->InstanceUI.Find(Base->InstanceWidgetName);
			if (Target && IsValid(*Target))
			{
				if ((*Target)->GetClass() == Base->GetClass())
				{
					(*Target)->MarkAsGarbage();
					(*Target) = nullptr;
					Instance->InstanceUI.Remove(Base->InstanceWidgetName);
					Ins = nullptr;
					return true;
				}
			}
		}
	}
	return false;
}

void UGameUserInterfaceSubsystem::WidgetBaseAddChild(FName CWidgetName, FName WidgetName, UWidgetBase* Owner, UObject*& NewClass, UClass* Class) {
	NewClass = Cast<UObject>(CreateWidget<UWidgetBase>(Owner, Class));
	if (IsValid(NewClass))
	{	
		Cast<UWidgetBase>(NewClass)->InstanceWidgetName = WidgetName;
		Cast<UWidgetBase>(NewClass)->InstanceWidgetCategoryName = CWidgetName;
		Cast<UWidgetBase>(NewClass)->SetParentClass(Owner);
	}
	else {
#if WITH_EDITOR
		UGameSystemFunction::FVMLog(__FUNCTION__, FString(TEXT("WidgetBaseAddChild添加失败！")));
#endif
		NewClass = nullptr;
	}
}

void UGameUserInterfaceSubsystem::WidgetBaseAddChild(FName CWidgetName, FName WidgetName, UObject*& NewClass, UClass* Class) {
	NewClass = Cast<UObject>(CreateWidget<UWidgetBase>(GAMEINS(), Class));
	if (IsValid(NewClass))
	{
		Cast<UWidgetBase>(NewClass)->InstanceWidgetName = WidgetName;
		Cast<UWidgetBase>(NewClass)->InstanceWidgetCategoryName = CWidgetName;
	}
	else {
#if WITH_EDITOR
		UGameSystemFunction::FVMLog(__FUNCTION__, FString(TEXT("WidgetBaseAddChild添加失败！")));
#endif
		NewClass = nullptr;
	}
}

void UGameUserInterfaceSubsystem::ShowGameInformationUISub(UWidget* Widget)
{
	if (IsValid(UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic()))
	{
		UGameSystemFunction::GetUserInterInstanceByName(
			MoveTemp(GameDataNameAsset::UI_Global::Global)
		)->ShowUI_ByName_Imp(
			MoveTemp(GameDataNameAsset::UI_Global::GameInfor)
		);
	}
}

void UGameUserInterfaceSubsystem::ShowGameTaskUISub(class UWidget* Widget)
{
	if (IsValid(UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic()))
	{
		//添加到视口
		UUI_GameTask* Cur =
			Cast<UUI_GameTask>(
				UGameSystemFunction::GetUserInterInstanceByName(
					MoveTemp(GameDataNameAsset::UI_Global::Global)
				)->GetUI_Imp(
					MoveTemp(GameDataNameAsset::UI_Global::GameTask)
				)
			);

		if (IsValid(Cur))
		{
			if (!Cur->IsInViewport())
			{
				//加载任务系统
				UTaskSubsystem::GetTaskSubsystemStatic()->LoadTaskSystem();
				//初始化内容
				Cur->InitContent();
				//显示窗口
				Cur->AddToViewport();
			}
		}
	}
}

bool UGameUserInterfaceSubsystem::GetGameTaskUIInViewportSub()
{
	if (IsValid(UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic()))
	{
		//结果
		bool Result = IsValid(
			UGameSystemFunction::GetUserInterInstanceByName(
				MoveTemp(GameDataNameAsset::UI_Global::Global)
			)->GetIsInViewportUI_ByName(
				GameDataNameAsset::UI_Global::GameTask
			)
		);

		return Result;
	}
	return false;
}

void UGameUserInterfaceSubsystem::RemoveGameTaskUIViewportSub()
{
	if (IsValid(UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic()))
	{
		UGameSystemFunction::GetUserInterInstanceByName(
			MoveTemp(GameDataNameAsset::UI_Global::Global)
		)->ShowUI_ByName_Imp(
			MoveTemp(GameDataNameAsset::UI_Global::GameTask)
		);
	}
}

void UGameUserInterfaceSubsystem::ShowTaskFinishTipUISub(class UWidget* Widget, class UGameInstance* Insa)
{
	if (IsValid(UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic()))
	{
		UGameSystemFunction::GetUserInterInstanceByName(
			MoveTemp(GameDataNameAsset::UI_Global::Global)
		)->ShowUI_ByName_Imp(
			MoveTemp(GameDataNameAsset::UI_Global::TaskFinishTip), 200
		);
	}
}

void UGameUserInterfaceSubsystem::ShowNewLevelTipUISub(class UWidget* Widget)
{
	if (IsValid(UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic()))
	{
		UGameSystemFunction::GetUserInterInstanceByName(
			MoveTemp(GameDataNameAsset::UI_Global::Global)
		)->ShowUI_ByName_Imp(
			MoveTemp(GameDataNameAsset::UI_Global::NewLevelTip), 90
		);
	}
}

void UGameUserInterfaceSubsystem::ShowNewItemShowPanel(class UWidget* Widget, const TArray<FTaskItemStruct>& Data)
{
	if (IsValid(UGameUserInterfaceSubsystem::GetGameUserInterfaceSubsystemStatic()))
	{
		//添加到视口
		UNewItemShowPanel* Cur =
			Cast<UNewItemShowPanel>(
				UGameSystemFunction::GetUserInterInstanceByName(
					MoveTemp(GameDataNameAsset::UI_Global::Global)
				)->GetUI_Imp(
					MoveTemp(GameDataNameAsset::UI_Global::NewItemShow)
				)
			);

		//显示窗口
		if (!Cur->IsInViewport())
		{
			//加载数据
			Cur->Load(Data);
			//显示
			Cur->AddToViewport(80);
		}
	}
}

void UGameUserInterfaceSubsystem::ShowDialogueInforUISub(class UWidget* Widget)
{
	UGameSystemFunction::GetUserInterInstanceByName(
		MoveTemp(GameDataNameAsset::UI_Global::Global)
	)->ShowUI_ByName_Imp(
		MoveTemp(GameDataNameAsset::UI_Global::DialogueInfor)
	);
}

void UGameUserInterfaceSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
	//清理的定时器
	this->bStart = false;
}

void UGameUserInterfaceSubsystem::Deinitialize()
{
	for (const auto& Res : this->Resource)
	{
		Res.Value->Unload();
	}

	//清理的定时器
	this->GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	this->bStart = true;
}

void UGameUserInterfaceSubsystem::InitializeUserInterData(UGameDataAsset* DataAsset)
{
	if (this->Resource.Num())
	{
		return;
	}
	UDataTable* Table = DataAsset->GetDataByName_Imp(
		MoveTemp(GameDataNameAsset::GameDataAsset_GlobalAsset_UI::GlobalAsset_UI)
	)->GetDataByName_Imp(
		MoveTemp(GameDataNameAsset::GameDataAsset_GlobalAsset_UI::UserInter)
	);
	DataTableAssetData<FGameUserInterfaceTableRowBase> Datas(Table);
	for (const auto& CurData : Datas.GetDatas())
	{
		//初始化UI实例
		UGameUserInterfaceInstance* NewUI = NewObject<UGameUserInterfaceInstance>();
		//初始化分类名称
		NewUI->InterfaceName = CurData.Key;
		//初始化对应的UI软类资源
		NewUI->UI.Append(CurData.Value.UI);
		//添加到管理器
		this->Resource.Emplace(CurData.Key, NewUI);
	}
}
