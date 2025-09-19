// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "Data/Dialogue/Dialogue.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DialogueSubsystem.generated.h"

class UDataTable;
class UDialogueDataAsset;
class UUI_Dialogue;


//对话代理，进行追加对话时，返回的对话UI
DECLARE_DYNAMIC_DELEGATE_OneParam(FDialogueDelegate, class UUI_Dialogue*, UIDialogue);

//对话追加结构
USTRUCT()
struct FDialogueAddedDelegate {
	GENERATED_USTRUCT_BODY()
public:
	FDialogueAddedDelegate() {}
	FDialogueAddedDelegate(
		FDialogueDelegate& DialogueDelegate,
		TSoftObjectPtr<UDialogueDataAsset>& DialogueDataAsset,
		bool EndDialogue
	) :DialogueAddedDelegate(MoveTemp(DialogueDelegate)),
		DialogueDataAsset(MoveTemp(DialogueDataAsset)),
		bEndDialogue(EndDialogue)
	{}
public:
	FDialogueDelegate DialogueAddedDelegate;
	UPROPERTY()
	TSoftObjectPtr<UDialogueDataAsset> DialogueDataAsset;
	UPROPERTY()
	bool bEndDialogue = false;
};

//对话执行资产，可编写对话执行顺序
UCLASS(Blueprintable)
class FVM_API UDialogueExecute : public UObject
{
	GENERATED_BODY()

	friend class UDialogueSubsystem;

public:
	//初始化执行【章节ID，对应的剧情ID】如果填写则会查询对应的完成情况，如果完成则初始化失败
	UFUNCTION(BlueprintImplementableEvent)
	TMap<int32, int32> DialogueInit(class UDialogueSubsystem* DialogueSub);
	//执行对话
	UFUNCTION(BlueprintImplementableEvent)
	void Execute(class UDialogueSubsystem* DialogueSub);
public:
	//创建一个对话
	UFUNCTION(BlueprintCallable)
	UUI_Dialogue* CreateDialogue(TSoftObjectPtr<UDialogueDataAsset> Dialogue, bool bEndDialogue = false);
	//追加对话
	UFUNCTION(BlueprintCallable)
	void AddDialogue(
		FDialogueDelegate Delegate,
		TSoftObjectPtr<UDialogueDataAsset> Dialogue,
		bool bEndDialogue = false
	);
	//对话结束
	UFUNCTION(BlueprintCallable)
	void DialogueEnd() const;
public:
	//获取剧情章节
	UFUNCTION(BlueprintPure)
	int32 GetDialogueChapter() const;
	//获取剧情数据
	UFUNCTION(BlueprintPure)
	FDialogueDataInfor GetDialogueData() const;
public:
	//创建定时器
	UFUNCTION(BlueprintCallable)
	FTimerHandle SetTimer(
		FTimerDynamicDelegate Delegate,
		float Time,
		bool bLooping,
		float InitialStartDelay = 0.f,
		float InitialStartDelayVariance = 0.f
	);

	//清除定时器
	UFUNCTION(BlueprintCallable)
	void ClearTimerHandle(UPARAM(ref) FTimerHandle& Handle);
private:
	UPROPERTY()
	class UDialogueSubsystem* DiaSub = nullptr;
	//剧情章节
	UPROPERTY()
	int32 DialogueChapter = 0;
	//剧情数据
	UPROPERTY()
	FDialogueDataInfor DialogueDataInfor;
	//对话资产合集[队列]
	TQueue<FDialogueAddedDelegate> DialogueDataAssets;
};


/**
 * 对话子系统
 */
UCLASS()
class FVM_API UDialogueSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class UDialogueExecute;

public:
	/*
		执行对话系统
	@Param1 对象执行对象
	*/
	UFUNCTION(BlueprintCallable)
	static void DialogueExecute(
		TSoftClassPtr<UDialogueExecute> DialogueExecute
	);

	UFUNCTION(BlueprintPure)
	const UDialogueExecute* GetDialogueExecute() const { return this->DialogueExecuteObj; }

	//保存当前剧情对话进度_关卡外
	UFUNCTION(BlueprintCallable)
	static void SaveCurrentDialogue();
	//保存当前剧情对话进度_关卡
	UFUNCTION(BlueprintCallable)
	static void SaveCurrentDialogue_Level();
public:
	//设置对话执行对象
	void SetDialogueExecute(UDialogueExecute* NewDialogueExecute);
	//执行追加的对话内容
	bool DialogueAddedExecute();
private:
	UUI_Dialogue* ShowDialogueWidget(
		int32 ID,
		const TSoftObjectPtr<UDialogueDataAsset>& Datas,
		bool bEndDialogue
	);
private:
	//对话UI
	UPROPERTY()
	UUI_Dialogue* UI_Dialogue = nullptr;
	//当前对话执行对象
	UPROPERTY()
	UDialogueExecute* DialogueExecuteObj = nullptr;
	//当前加载过的对话资产
	UPROPERTY()
	TMap<FString, UClass*> ResourcePool;
};
