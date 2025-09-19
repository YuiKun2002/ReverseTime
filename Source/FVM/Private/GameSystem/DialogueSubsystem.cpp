// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/DialogueSubsystem.h"
#include "Engine/DataTable.h"
#include "Game/UI/UI_Dialogue.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameStart/VS/GameMapInstance.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/Tools/GameSystemFunction.h"

void UDialogueSubsystem::DialogueExecute(TSoftClassPtr<UDialogueExecute> DialogueExecute)
{
	UDialogueSubsystem* Dia = UFVMGameInstance::GetFVMGameInstance()->GetSubsystem<UDialogueSubsystem>();

	//如果当前正在执行的对话还未结束，则无法进行下一步
	if (IsValid(Dia->DialogueExecuteObj))
	{
		return;
	}

	const FString& AssetName = DialogueExecute.GetAssetName();
	UClass** CurLoadDia = Dia->ResourcePool.Find(AssetName);
	UClass* CurDiaClass = nullptr;
	if (CurLoadDia)
	{
		CurDiaClass = *CurLoadDia;
	}
	else {
		CurDiaClass = DialogueExecute.LoadSynchronous();
		if (CurDiaClass)
		{
			Dia->ResourcePool.Emplace(AssetName, CurDiaClass);
		}
		else {
			return;
		}
	}

	TSubclassOf<UDialogueExecute> Sub(CurDiaClass);
	if (IsValid(Sub.Get()))
	{
		UPlayerStructManager* Player = UFVMGameInstance::GetPlayerStructManager_Static();

		UDialogueExecute* CurAsset = Sub.GetDefaultObject();
		Dia->SetDialogueExecute(CurAsset);
		CurAsset->DiaSub = Dia;

		//查询剧情完成情况
		TMap<int32, int32> DiaResult = CurAsset->DialogueInit(Dia);
		bool bResult = true;
		for (const auto& Result : DiaResult)
		{
			if (Player->DialogueFinished.FindFinishDialogue(Result.Key, Result.Value))
			{
				bResult = false;
				break;
			}
		}

		if (bResult)
		{
			CurAsset->Execute(Dia);
		}
		else {
			Dia->SetDialogueExecute(nullptr);
		}
	}
}

UUI_Dialogue* UDialogueExecute::CreateDialogue(TSoftObjectPtr<UDialogueDataAsset> Dialogue, bool bEndDialogue)
{
	if (!IsValid(Dialogue.Get()))
	{
		Dialogue.LoadSynchronous();
	}

	UDialogueDataAsset* NewDia = Dialogue.Get();
	if (IsValid(NewDia))
	{
		this->DialogueChapter = NewDia->DialogueDataInfor.DialogueChapter;
		this->DialogueDataInfor = NewDia->DialogueDataInfor;
		return this->DiaSub->ShowDialogueWidget(NewDia->DialogueDataInfor.DialogueID, Dialogue, bEndDialogue);
	}

	return nullptr;
}

void UDialogueExecute::AddDialogue(
	FDialogueDelegate Delegate,
	TSoftObjectPtr<UDialogueDataAsset> Dialogue,
	bool bEndDialogue
)
{
	//检测当前是否存在对话
	if (IsValid(this->DiaSub->UI_Dialogue) && this->DiaSub->UI_Dialogue->GetDialogueState())
	{
		if (Delegate.IsBound())
		{
			FDialogueAddedDelegate NewDialogueAddedDelegate(
				Delegate,
				Dialogue,
				bEndDialogue
			);
			this->DialogueDataAssets.Enqueue(
				MoveTemp(NewDialogueAddedDelegate)
			);
		}
	}
	else {
		UUI_Dialogue* NewUI_Dialogue = this->CreateDialogue(Dialogue, bEndDialogue);
		if (Delegate.IsBound())
		{
			Delegate.Execute(NewUI_Dialogue);
		}
	}
}

void UDialogueExecute::DialogueEnd() const
{
	DiaSub->DialogueExecuteObj = nullptr;
}

int32 UDialogueExecute::GetDialogueChapter() const
{
	return this->DialogueChapter;
}

FDialogueDataInfor UDialogueExecute::GetDialogueData() const
{
	return this->DialogueDataInfor;
}

FTimerHandle UDialogueExecute::SetTimer(
	FTimerDynamicDelegate Delegate, float Time, bool bLooping, float InitialStartDelay, float InitialStartDelayVariance)
{
	FTimerHandle Handle;
	if (Delegate.IsBound())
	{
		const UWorld* const World = DiaSub->GetWorld();
		if (World)
		{
			InitialStartDelay += FMath::RandRange(-InitialStartDelayVariance, InitialStartDelayVariance);
			if (Time <= 0.f || (Time + InitialStartDelay) < 0.f)
			{
				FString ObjectName = GetNameSafe(Delegate.GetUObject());
				FString FunctionName = Delegate.GetFunctionName().ToString();
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s %s SetTimer passed a negative or zero time. The associated timer may fail to be created/fire! If using InitialStartDelayVariance, be sure it is smaller than (Time + InitialStartDelay)."), *ObjectName, *FunctionName), ELogVerbosity::Warning);
			}

			FTimerManager& TimerManager = World->GetTimerManager();
			Handle = TimerManager.K2_FindDynamicTimerHandle(Delegate);
			TimerManager.SetTimer(Handle, Delegate, Time, bLooping, (Time + InitialStartDelay));
		}
	}
	else
	{
		UE_LOG(LogBlueprintUserMessages, Warning,
			TEXT("SetTimer passed a bad function (%s) or object (%s)"),
			*Delegate.GetFunctionName().ToString(), *GetNameSafe(Delegate.GetUObject()));
	}

	return Handle;
}

void UDialogueExecute::ClearTimerHandle(FTimerHandle& Handle)
{
	if (Handle.IsValid())
	{
		DiaSub->GetWorld()->GetTimerManager().ClearTimer(Handle);
	}
}

void UDialogueSubsystem::SetDialogueExecute(UDialogueExecute* NewDialogueExecute)
{
	this->DialogueExecuteObj = NewDialogueExecute;
}

bool UDialogueSubsystem::DialogueAddedExecute()
{
	if (IsValid(this->DialogueExecuteObj))
	{
		if (!this->DialogueExecuteObj->DialogueDataAssets.IsEmpty())
		{
			FDialogueAddedDelegate Data;
			this->DialogueExecuteObj->DialogueDataAssets.Dequeue(Data);

			UUI_Dialogue* NewDialogue = DialogueExecuteObj->CreateDialogue(
				Data.DialogueDataAsset,
				Data.bEndDialogue
			);
			Data.DialogueAddedDelegate.Execute(NewDialogue);

			return true;
		}
	}

	return false;
}

void UDialogueSubsystem::SaveCurrentDialogue()
{
	//获取对话系统
	UDialogueSubsystem* Dia = UFVMGameInstance::GetFVMGameInstance()->GetSubsystem<UDialogueSubsystem>();

	if (IsValid(Dia->GetDialogueExecute()))
	{
		//添加局外临时信息
		UFVMGameInstance::GetPlayerStructManager_Static()->DialogueFinished.AddFinishDialogue_Global();

		UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("剧情完成")));
	}
}

void UDialogueSubsystem::SaveCurrentDialogue_Level()
{
	//获取对话系统
	UDialogueSubsystem* Dia = UFVMGameInstance::GetFVMGameInstance()->GetSubsystem<UDialogueSubsystem>();

	if (IsValid(Dia->GetDialogueExecute()))
	{
		//添加关卡临时信息
		UFVMGameInstance::GetPlayerStructManager_Static()->DialogueFinished.AddFinishDialogue_Level();

		UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("剧情完成")));
	}
}

UUI_Dialogue* UDialogueSubsystem::ShowDialogueWidget(
	int32 ID,
	const TSoftObjectPtr<UDialogueDataAsset>& Datas,
	bool bEndDialogue
)
{
	if (!IsValid(this->UI_Dialogue))
	{
		//初始化UI
		this->UI_Dialogue = CREATE_UI_WIDGET(UUI_Dialogue, Global, Dialogue);
	}

	//如何正在进行剧情，则无法覆盖当前剧情
	if (this->UI_Dialogue->GetDialogueState())
	{
		return this->UI_Dialogue;
	}

	if (!this->UI_Dialogue->IsInViewport())
	{
		//添加到视口
		this->UI_Dialogue->AddToViewport(500);
	}

	//设置剧本
	this->UI_Dialogue->SetDialogue(ID, Datas, bEndDialogue);

	//返回剧本
	return this->UI_Dialogue;
}
