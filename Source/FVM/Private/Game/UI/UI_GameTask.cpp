// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/UI_GameTask.h"
#include "Engine/DataTable.h"
#include "GameSystem/TaskSubsystem.h"
#include "Data/CardData/MouseDataStruct.h"
#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/UI_GamePrepare.h"
#include "GameSystem/FVMGameInstance.h"
#include <Components/Button.h>
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Components/ScrollBox.h>
#include <Kismet/GameplayStatics.h>
#include <Components/RichTextBlock.h>
#include <Components/UniformGridPanel.h>
#include "GameSystem/GameUserInterfaceSubsystem.h"


bool UUI_GameTask::Initialize()
{
	Super::Initialize();

	this->TaskListScroll = this->GetWidgetComponent<UScrollBox>(this, TEXT("ScrollBox_83"));
	this->TaskListUniform = this->GetWidgetComponent<UUniformGridPanel>(this, TEXT("UniformGridPanel_133"));

	this->TaskItemListScroll = this->GetWidgetComponent<UScrollBox>(this, TEXT("ItemBox"));
	this->TaskItemListUniform = this->GetWidgetComponent<UUniformGridPanel>(this, TEXT("UniformGridPanel_99"));

	this->TTaskTitle = this->GetWidgetComponent<UTextBlock>(this, TEXT("TaskTitle"));
	this->TTaskContent = this->GetWidgetComponent<UTextBlock>(this, TEXT("TaskContent"));
	this->TTaskTarget = this->GetWidgetComponent<URichTextBlock>(this, TEXT("TaskTargetTitle"));
	this->TTaskEx = this->GetWidgetComponent<UTextBlock>(this, TEXT("Ex"));
	this->TTaskCoin = this->GetWidgetComponent<UTextBlock>(this, TEXT("Coin"));

	this->UTaskState = this->GetWidgetComponent<UImage>(this, TEXT("TaskStateImg"));
	this->UTaskFinishStateImage = this->GetWidgetComponent<UImage>(this, TEXT("TaskFinishImg"));

	this->TaskFinishBut = this->GetWidgetComponent<UButton>(this, TEXT("TaskFinish"));

	return true;
}

void UUI_GameTask::InitContent()
{
	//初始化任务显示
	if (!IsValid(this->TaskTimeLoadManager))
	{
		this->TaskTimeLoadManager = this->CreateItemLoaderObject(
			this->TaskListScroll,
			this->TaskListUniform,
			{ 12, 1,9999,0.01f,false }
		);
		this->TaskTimeLoadManager->OnNew.BindUObject(this, &UUI_GameTask::WidgetCreateTaskList);
		this->TaskTimeLoadManager->OnUpdate.BindUObject(this, &UUI_GameTask::WidgetRefreshTaskList);
	}

	//更新任务条目
	if (UTaskSubsystem::GetTaskSubsystemStatic()->GetUpdate())
	{
		this->CurSelect = nullptr;
		this->TaskItems.Empty();
		this->TaskDataBlockItems.Empty();
		UTaskSubsystem::GetTaskSubsystemStatic()->GetTasksList(this->TaskItems);

		//初始化任务数据
		for (const FTaskBase& Task : this->TaskItems)
		{
			FAllTaskDataBlock* TaskDataBlock = this->TaskDataBlockItems.Find(Task.GetTaskType());
			if (TaskDataBlock)
			{
				TaskDataBlock->AllTaskData.Emplace(Task.TaskID, Task);
			}
			else {
				FAllTaskDataBlock NewBlock;
				NewBlock.AllTaskData.Emplace(Task.TaskID, Task);
				this->TaskDataBlockItems.Emplace(Task.GetTaskType(), NewBlock);
			}
		}
	}

	//初始化任务列表
	this->CurListSelect = nullptr;
	this->TaskTimeLoadManager->UpdateDatatable(this->TaskItems);
	this->TaskTimeLoadManager->SetLoaderItemMaxCount(this->TaskItems.Num());
	this->TaskTimeLoadManager->Execute();

	if (IsValid(this->CurSelect))
	{
		this->CurSelect->Select();
	}
	else {
		if (IsValid(this->CurListSelect))
		{
			this->CurListSelect->Select();
		}
	}
}

void UUI_GameTask::GetTaskItems()
{
	if (IsValid(this->CurSelect))
	{
		//领取奖励
		for (const FTaskItemStruct& Item : this->TaskItemDatas)
		{
			UGameSystemFunction::AddGameinstructions(this, TEXT("/") + Item.ItemIns, false);
		}

		//任务完成领取，移动任务位置
		UTaskSubsystem::MoveTaskIndexSingle(
			this->CurSelect->TaskIndex,
			UFVMGameInstance::GetPlayerStructManager_Static()->LineTaskProgress
		);

		//设置状态
		FAllTaskDataBlock* CurTaskBlock = this->TaskDataBlockItems.Find(this->CurSelect->TaskIndex.TaskType);
		if (CurTaskBlock)
		{
			FTaskBase* CurTaskBaseData = CurTaskBlock->AllTaskData.Find(this->CurSelect->TaskIndex.ID);
			if (CurTaskBaseData)
			{
				CurTaskBaseData->TaskState = 3u;
			}
		}
		this->CurSelect->Select();

		UFVMGameInstance::GetPlayerStructManager_Static()->AddPlayerEx(this->PlayerEx);
		UFVMGameInstance::GetPlayerStructManager_Static()->AddCoin(this->PlayerCoin, 0u);

		//显示UI
		UGameUserInterfaceSubsystem::ShowNewItemShowPanel(this, this->TaskItemDatas);

		//执行函数
		//获取任务执行对象
		const TSoftClassPtr<UTaskFuncObjectExe>& CurObj = this->TaskFuncObjectExe;
		if (UClass* CurNewClass = CurObj.LoadSynchronous())
		{
			if (UGameUserInterfaceSubsystem::GetGameTaskUIInViewportSub())
			{
				UGameUserInterfaceSubsystem::RemoveGameTaskUIViewportSub();

				if (!IsValid(UUI_GamePrepare::GetGamePrepareUiStatic()))
				{
					UGameSystemFunction::LoadLastMap();
				}
			}

			NewObject<UTaskFuncObjectExe>(UTaskFuncObjectExe::StaticClass(), CurNewClass)->OnInit();
		}

		UGameSystemFunction::SaveCurrentPlayerData(
			__FUNCTION__ +
			FString(TEXT("领取任务，领取名称：")) +
			this->CurSelect->TaskTitle->GetText().ToString()
		);
	}
}

UWidget* UUI_GameTask::WidgetCreateTaskList(const UItemLoadDataBlock* const _Data, int32 _Index)
{
	UUI_GameTaskItem* NewUI = CreateWidget<UUI_GameTaskItem>(
		this, LoadClass<UUI_GameTaskItem>(nullptr,
			TEXT("WidgetBlueprint'/Game/Resource/BP/Game/UI/GameTask/BP_TaskItem.BP_TaskItem_C'"))
	);

	FTaskBase CurTask = _Data->GetValue<FTaskBase>();
	FTaskIndex NewIndex;
	NewIndex.ID = CurTask.TaskID;
	NewIndex.TaskType = CurTask.TaskType;
	NewUI->SetTaskState(this, NewIndex, CurTask.TaskState, CurTask.TaskName, CurTask.TaskOtherTitle);

	if (!IsValid(this->CurListSelect))
	{
		this->CurListSelect = NewUI;
	}

	return NewUI;
}

void UUI_GameTask::WidgetRefreshTaskList(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget)
{
	UUI_GameTaskItem* NewUI = Cast<UUI_GameTaskItem>(_UWidget);
	FTaskBase CurTask = _Data->GetValue<FTaskBase>();
	FTaskIndex NewIndex;
	NewIndex.ID = CurTask.TaskID;
	NewIndex.TaskType = CurTask.TaskType;
	NewUI->SetTaskState(this, NewIndex, CurTask.TaskState, CurTask.TaskName, CurTask.TaskOtherTitle);

	if (!IsValid(this->CurListSelect))
	{
		this->CurListSelect = NewUI;
	}
}

UWidget* UUI_GameTask::WidgetCreateTaskItemList(const UItemLoadDataBlock* const _Data, int32 _Index)
{
	UUI_GameTaskItemShow* NewUI = CreateWidget<UUI_GameTaskItemShow>(
		this, LoadClass<UUI_GameTaskItemShow>(nullptr,
			TEXT("WidgetBlueprint'/Game/Resource/BP/Game/UI/GameTask/BP_TaskItemStruct.BP_TaskItemStruct_C'"))
	);
	FTaskItemStruct CurTask = _Data->GetValue<FTaskItemStruct>();
	NewUI->Set(CurTask.ImagePath, CurTask.ItemCount, CurTask.ItemName);
	return NewUI;
}

void UUI_GameTask::WidgetRefreshTaskItemList(const UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget)
{
	UUI_GameTaskItemShow* NewUI = Cast<UUI_GameTaskItemShow>(_UWidget);
	FTaskItemStruct CurTask = _Data->GetValue<FTaskItemStruct>();
	NewUI->Set(CurTask.ImagePath, CurTask.ItemCount, CurTask.ItemName);
}

void UUI_GameTask::LoadItems(const TArray<FTaskItemStruct>& Items)
{
	//初始化任务奖励显示
	if (!IsValid(this->TaskItemTimeLoadManager))
	{
		this->TaskItemTimeLoadManager = this->CreateItemLoaderObject(
			this->TaskItemListScroll,
			this->TaskItemListUniform,
			{ 3, 2,9999,0.01f,false }
		);
		this->TaskItemTimeLoadManager->OnNew.BindUObject(this, &UUI_GameTask::WidgetCreateTaskItemList);
		this->TaskItemTimeLoadManager->OnUpdate.BindUObject(this, &UUI_GameTask::WidgetRefreshTaskItemList);
	}

	if (Items.Num())
	{
		this->TaskItemDatas.Empty();
		this->TaskItemDatas.Append(Items);
		this->TaskItemListScroll->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		this->TaskItemListScroll->SetVisibility(ESlateVisibility::Collapsed);
	}
	this->TaskItemTimeLoadManager->UpdateDatatable(this->TaskItemDatas);
	this->TaskItemTimeLoadManager->Execute();
}

bool UUI_GameTaskItem::Initialize()
{
	Super::Initialize();

	this->MainTaskImg = this->GetWidgetComponent<UImage>(this, TEXT("Image_74"));
	this->TaskStateImg = this->GetWidgetComponent<UImage>(this, TEXT("Image_76"));

	this->TaskTitle = this->GetWidgetComponent<UTextBlock>(this, TEXT("TaskTitle"));
	this->TaskOtherTitle = this->GetWidgetComponent<UTextBlock>(this, TEXT("OtherTitle"));

	return true;
}

void UUI_GameTaskItem::SetTaskState(UUI_GameTask* CurTaskUI, FTaskIndex LTaskIndex, uint8 LTaskState, FString STaskName, FString STaskOtherTitle)
{
	this->TaskUI = CurTaskUI;
	this->TaskIndex = LTaskIndex;

	if (TaskIndex.TaskType == EGameTaskType::MainTask)
	{
		this->MainTaskImg->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else {
		this->MainTaskImg->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (LTaskState == 0u)
	{
		UWidgetBase::SetImageBrush(
			this->TaskStateImg,
			TEXT("Texture2D'/Game/Resource/Texture/UI/Game/Task/22.22'")
		);
	}
	else if (LTaskState == 1u) {
		UWidgetBase::SetImageBrush(
			this->TaskStateImg,
			TEXT("Texture2D'/Game/Resource/Texture/UI/Game/Task/25.25'")
		);
	}
	else if (LTaskState == 2u) {
		UWidgetBase::SetImageBrush(
			this->TaskStateImg,
			TEXT("Texture2D'/Game/Resource/Texture/UI/Game/Task/28.28'")
		);
	}
	else {
		UWidgetBase::SetImageBrush(
			this->TaskStateImg,
			TEXT("Texture2D'/Game/Resource/Texture/UI/Game/Task/31.31'")
		);
	}

	this->TaskTitle->SetText(FText::FromString(STaskName));
	this->TaskOtherTitle->SetText(FText::FromString(STaskOtherTitle));
}

void UUI_GameTaskItem::Select()
{
	this->TaskUI->CurSelect = this;

	//查询相关任务
	FAllTaskDataBlock* CurTaskBlock = this->TaskUI->TaskDataBlockItems.Find(this->TaskIndex.TaskType);
	if (CurTaskBlock)
	{
		//选择相关任务
		FTaskBase* CurTaskBaseData = CurTaskBlock->AllTaskData.Find(this->TaskIndex.ID);
		if (CurTaskBaseData)
		{
			if (CurTaskBaseData->TaskState == 0 || CurTaskBaseData->TaskState == 1)
			{
				UWidgetBase::SetImageBrush(
					this->TaskStateImg,
					TEXT("Texture2D'/Game/Resource/Texture/UI/Game/Task/25.25'")
				);

				CurTaskBaseData->TaskState = 1;

			}

			if (CurTaskBaseData->TaskState == 2 || CurTaskBaseData->TaskState == 3)
			{
				UWidgetBase::SetImageBrush(
					this->TaskUI->UTaskState,
					TEXT("Texture2D'/Game/Resource/Texture/UI/Game/Task/52.52'")
				);
			}
			else
			{
				UWidgetBase::SetImageBrush(
					this->TaskUI->UTaskState,
					TEXT("Texture2D'/Game/Resource/Texture/UI/Game/Task/55.55'")
				);
			}

			if (CurTaskBaseData->TaskState == 2)
			{
				this->TaskUI->TaskFinishBut->SetIsEnabled(true);

				UWidgetBase::SetImageBrush(
					this->TaskStateImg,
					TEXT("Texture2D'/Game/Resource/Texture/UI/Game/Task/28.28'")
				);
			}
			else {
				this->TaskUI->TaskFinishBut->SetIsEnabled(false);
			}

			if (CurTaskBaseData->TaskState == 3)
			{
				UWidgetBase::SetImageBrush(
					this->TaskStateImg,
					TEXT("Texture2D'/Game/Resource/Texture/UI/Game/Task/31.31'")
				);
				this->TaskUI->UTaskFinishStateImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else {
				this->TaskUI->UTaskFinishStateImage->SetVisibility(ESlateVisibility::Collapsed);
			}


			FTaskBase BaseData;
			UTaskSubsystem::GetTaskSubsystemStatic()->FindTask(
				this->TaskIndex.ID,
				this->TaskIndex.TaskType, BaseData
			);
			//赋予执行函数
			this->TaskUI->TaskFuncObjectExe = BaseData.ExeObject;

			//设置任务描述
			FString CurContent = BaseData.TaskContent;
			FString Content = CurContent.Replace(
				TEXT("-p"),
				*UTaskSubsystem::GetTaskSubsystemStatic()->GetPlayerName()
			);
			this->TaskUI->TTaskContent->SetText(FText::FromString(Content));
			this->TaskUI->TTaskTitle->SetText(FText::FromString(BaseData.TaskName));
			//设置任务目标
			FString Left = TEXT("<t>") + BaseData.TaskTarget + TEXT("</>");
			//替换
			FString LeftTargetContent = Left.Replace(TEXT("【"), TEXT("</><g>"));
			FString RightTargetContent = LeftTargetContent.Replace(TEXT("】"), TEXT("</><t>"));
			this->TaskUI->TTaskTarget->SetText(FText::FromString(RightTargetContent));
			this->TaskUI->TTaskEx->SetText(FText::FromString(FString::FromInt(BaseData.TaskEx)));
			this->TaskUI->TTaskCoin->SetText(FText::FromString(FString::FromInt(BaseData.TaskCoin)));

			this->TaskUI->PlayerEx = BaseData.TaskEx;
			this->TaskUI->PlayerCoin = BaseData.TaskCoin;

			this->TaskUI->TaskItemDatas = BaseData.TaskItems;
			this->TaskUI->LoadItems(BaseData.TaskItems);
		}
	}
}

bool UUI_GameTaskItemShow::Initialize()
{
	Super::Initialize();

	this->BgImg = this->GetWidgetComponent<UImage>(this, TEXT("Image_66"));

	this->NameText = this->GetWidgetComponent<UTextBlock>(this, TEXT("TextBlock_31"));
	this->CountText = this->GetWidgetComponent<UTextBlock>(this, TEXT("TextBlock"));

	return true;
}

void UUI_GameTaskItemShow::Set(const FSoftObjectPath& ImagePath, int32 ItemCount, FString ItemName)
{
	UWidgetBase::SetImageBrush(
		this->BgImg,
		ImagePath.ToString(), FVector(1.f), 1.f, true, FVector(1.f, 1.f, 0.f), false
	);
	this->NameText->SetText(FText::FromString(ItemName));
	if (ItemCount == 0)
	{
		this->CountText->SetVisibility(ESlateVisibility::Collapsed);
	}
	else {
		this->CountText->SetText(FText::FromString(TEXT("x") + FString::FromInt(ItemCount)));
		this->CountText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}
