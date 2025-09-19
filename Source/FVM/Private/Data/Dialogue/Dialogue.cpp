// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/Dialogue/Dialogue.h"

void FDialogueDataInfor_Player::AddFinishDialogue_Global()
{
	this->AddFinishDialogueTemp(this->DialogueDataInfor_Global);
}

void FDialogueDataInfor_Player::AddFinishDialogue_Temp_Global(
	const int32& DialogueChapter,
	const int32& DialogueID,
	const FDialogueDataInfor& Infor
)
{
	this->AddFinishDialogueTemp(
		this->DialogueDataInfor_Global,
		DialogueChapter,
		DialogueID,
		Infor
	);
}

void FDialogueDataInfor_Player::AddFinishDialogue_Level()
{
	this->AddFinishDialogueTemp(this->DialogueDataInfor_Level);
}

void FDialogueDataInfor_Player::AddFinishDialogue_Temp_Level(
	const int32& DialogueChapter,
	const int32& DialogueID,
	const FDialogueDataInfor& Infor
)
{
	this->AddFinishDialogueTemp(
		this->DialogueDataInfor_Level,
		DialogueChapter,
		DialogueID,
		Infor
	);
}

void FDialogueDataInfor_Player::AddFinishDialogue(
	const int32& DialogueChapter,
	const int32& DialogueID,
	const FDialogueDataInfor& Infor
)
{
	FDialogueDataInfor_Player_Detail* CurInfor = this->DialogueDataInfor.Find(DialogueChapter);
	if (CurInfor)
	{
		CurInfor->DialogueData.Emplace(DialogueID, Infor);
	}
	else {
		FDialogueDataInfor_Player_Detail NewDetail;
		NewDetail.DialogueData.Emplace(DialogueID, Infor);
		this->DialogueDataInfor.Emplace(DialogueChapter, NewDetail);
	}
}

bool FDialogueDataInfor_Player::FindFinishDialogue(
	const int32& DialogueChapter,
	const int32& DialogueID
) {
	FDialogueDataInfor_Player_Detail* CurInfor = this->DialogueDataInfor.Find(DialogueChapter);
	if (CurInfor)
	{
		return CurInfor->DialogueData.Contains(DialogueID);
	}
	return false;
}