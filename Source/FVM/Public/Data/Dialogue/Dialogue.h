// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Dialogue.generated.h"

//对话类型
UENUM(BlueprintType)
enum class EDialogueType : uint8 {
	Main UMETA(DisplayName = "Main"),
	Change UMETA(DisplayName = "Change"),
};

//对话数据类型
UENUM(BlueprintType)
enum class EDialogueDataType : uint8 {
	//主线类型
	MainLine UMETA(DisplayName = "MainLine"),
	//支线类型
	BranchLine UMETA(DisplayName = "BranchLine"),
};

//对话信息结构
USTRUCT(BlueprintType)
struct FDialogueDataInfor {
	GENERATED_BODY()
public:
	//对话ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DialogueID = 0;
	//章节
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DialogueChapter;
	//剧情标题
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DialogueTitle;
	//剧情目标位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DialogueTarget;
	//剧情总结
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DialogueSummary;
	//对话数据类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDialogueDataType DialogueDataType;
};

/*
游戏对话表
1.对话的核心数据表
*/
USTRUCT(BlueprintType)
struct FDialogueDataTable : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	//类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDialogueType Type;
	//角色名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CharName;
	//角色形象
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath CharImg;
	//角色位置[左|右]
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLeft = true;
	//对话内容
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DialogueContent;
	//对话选项[选项内容，跳转对话]
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, int32> DialogueSelect;
	//跳转对话
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 JumpDialogueIndex = -1;
	//背景
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> DialogueBG;
	//背景显示颜色
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector BGColor = FVector(1.f);
	//音频持续时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DialogueTime = 1.f;
	//播放的音频
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath CharVoicePath;
	//播放的音频2【针对主角】
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath Char2VoicePath;
	//背景音乐
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath BGMPath;
};

/*
* 剧情对话数据资产
存储剧情章节信息，以及对话的数据表
*/
UCLASS()
class FVM_API UDialogueDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	//对话信息结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDialogueDataInfor DialogueDataInfor;
	//数据表
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UDataTable> DialogueDataTable;
};

/*
适用于存储一个完整的对话资产信息
1.管理全部的对话表信息
*/
USTRUCT(BlueprintType)
struct FDialogueDataAssetDataTable : public FTableRowBase {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UDialogueDataAsset> DialogueDataAsset;
};

//章节关联名称[用表索引对应章节的名称 0 -> 穿越 ，1 -> 美味岛]
USTRUCT(BlueprintType)
struct FDialogueChapter : public FTableRowBase {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FString> DialogueChapter;
};

/*
通过索引来关联对应的剧情细节数据
1.包含标题
2.ID
........等等
*/
USTRUCT(BlueprintType)
struct FDialogueDataInfor_Player_Detail {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FDialogueDataInfor> DialogueData;
};

/*
适用于角色存档存储数据
1.任何完成的剧情都会存入此处
2.剧情查询也在此处查询
*/
USTRUCT(BlueprintType)
struct FDialogueDataInfor_Player {
	GENERATED_BODY()
public:
	//对话信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FDialogueDataInfor_Player_Detail> DialogueDataInfor;

	//当前局外播放的剧情信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FDialogueDataInfor_Player_Detail> DialogueDataInfor_Global;
	//当前关卡播放的剧情信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FDialogueDataInfor_Player_Detail> DialogueDataInfor_Level;
public:
	//新增局外完成的剧情
	void AddFinishDialogue_Global();
	void AddFinishDialogue_Temp_Global(
		const int32& DialogueChapter,
		const int32& DialogueID,
		const FDialogueDataInfor& Infor
	);

	//新增关卡完成的剧情
	void AddFinishDialogue_Level();
	void AddFinishDialogue_Temp_Level(
		const int32& DialogueChapter,
		const int32& DialogueID,
		const FDialogueDataInfor& Infor
	);

	//查询剧情是否完成
	bool FindFinishDialogue(
		const int32& DialogueChapter,
		const int32& DialogueID
	);
private:
	void AddFinishDialogueTemp(
		TMap<int32, FDialogueDataInfor_Player_Detail>& InputMap,
		const int32& DialogueChapter,
		const int32& DialogueID,
		const FDialogueDataInfor& Infor) {
		FDialogueDataInfor_Player_Detail* CurInfor = InputMap.Find(DialogueChapter);
		if (CurInfor)
		{
			CurInfor->DialogueData.Emplace(DialogueID, Infor);
		}
		else {
			FDialogueDataInfor_Player_Detail NewDetail;
			NewDetail.DialogueData.Emplace(DialogueID, Infor);
			InputMap.Emplace(DialogueChapter, NewDetail);
		}
	}

	void AddFinishDialogueTemp(TMap<int32, FDialogueDataInfor_Player_Detail>& InputMap) {
		for (const auto& Detail : InputMap)
		{
			for (const auto& Data : Detail.Value.DialogueData)
			{
				this->AddFinishDialogue(Detail.Key, Data.Key, Data.Value);
			}
		}
		InputMap.Empty();
	}

	//新增完成剧情
	void AddFinishDialogue(
		const int32& DialogueChapter,
		const int32& DialogueID,
		const FDialogueDataInfor& Infor
	);
};