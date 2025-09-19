// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/Instructions/GiftInstructions.h"
#include <Components/Button.h>
#include "GameSystem/Item/Equipment/Func/EquipmentFuncGift.h"
#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/Tips/UI_GiftItemTip.h"
#include "GameSystem/FVMGameInstance.h"

bool UGiftInstructions::ReadInstruction(const TArray<FString>& Instruction) {

	if (Instruction.Num() == 0)
	{
		return false;
	}

	//设置指令集合描述
	this->SetInstructions(GET_DEF_NAME(Instruction, Gift));

	//帮助指令
	if (this->GetSystem()->AddHelpInstructionAndPrint(Instruction[0], GET_DEF_NAME(Instruction, Gift)))
	{
		return false;
	}

	//发送帮助指令
	if (this->PrintHelpInstruction(TEXT("gift"), Instruction))
	{
		return false;
	}

	//可以对礼盒的UI界面里面的组件进行开启和关闭;;
	//gift ui set 【UI组件】【状态】;;
	//UI组件：;【use】使用按钮;【cancel】取消按钮;;
	//状态：;【1】开启;【0】关闭

	if (this->ExecuteInsHead(Instruction, TEXT("ui"),
		std::bind(&UGiftInstructions::UI, this, std::placeholders::_1)))
	{
		return true;
	}


	this->PrintErrorIns(TEXT("gift"), Instruction[0]);
	return false;
}

bool UGiftInstructions::UI(const TArray<FString>& Instruction) {

	if (Instruction.Num() < 1)
	{
		return false;
	}

	//设置UI组件的状态
	if (Instruction[0].Equals(TEXT("set")))
	{
		if (Instruction.Num() > 2)
		{
			return this->UI_Set(Instruction);
		}
		return false;
	}

	//编辑ui中礼盒的数据
	if (Instruction[0].Equals(TEXT("edit")))
	{
		if (Instruction.Num() > 2)
		{
			return this->UI_Edit(Instruction);
		}
		return false;
	}

	//移除ui中礼盒的数据
	if (Instruction[0].Equals(TEXT("rem")))
	{
		if (Instruction.Num() > 2)
		{
			return this->UI_Rem(Instruction);
		}
		return false;
	}

	return false;
}

bool UGiftInstructions::UI_Set(const TArray<FString>& Instruction) {
	UUI_GiftItemTip* TipUI = GET_UI_CAST(UUI_GiftItemTip, PlayerBag, GiftItemTip);
	if (IsValid(TipUI) && TipUI->IsInViewport())
	{
		if (Instruction[1].Equals(TEXT("use")))
		{
			int32 State = FCString::Atoi(*Instruction[2]);
			TipUI->GetOkButtonComponent()->SetIsEnabled((bool)State);
		}
		else if (Instruction[1].Equals(TEXT("cancel")))
		{
			int32 State = FCString::Atoi(*Instruction[2]);
			TipUI->GetCancelButtonComponent()->SetIsEnabled((bool)State);
		}
		else {
			return false;
		}
		return true;
	}
	return false;
}

bool UGiftInstructions::UI_Edit(const TArray<FString>& Instruction) {

	UUI_GiftItemTip* TipUI = GET_UI_CAST(UUI_GiftItemTip, PlayerBag, GiftItemTip);

	UEquipmentFuncGift* GiftObj = Cast<UEquipmentFuncGift>(TipUI->GetEquipmentFuncGiftObject());

	if (!IsValid(GiftObj))
	{
		return false;
	}

	if (IsValid(TipUI) && TipUI->IsInViewport())
	{
		//礼包指令修改
		if (this->UI_Edit_cmd(GiftObj, Instruction))
		{
			return true;
		}

		//礼包列表数据修改
		if (this->UI_Edit_ListData(GiftObj, Instruction))
		{
			return true;
		}

		//礼包列表数据修改
		if (this->UI_Edit_RandomListData(GiftObj, Instruction))
		{
			return true;
		}
	}
	return false;
}

bool UGiftInstructions::UI_Edit_cmd(UEquipmentFuncGift* GiftObj, const TArray<FString>& Instruction) {
	//编辑指令
	if (Instruction[1].Equals(TEXT("cmd")))
	{
		//使用深度搜索替换
		int32 Count = Instruction.Num();
		FString Temp;
		for (int32 i = 2; i < Count; i++)
		{
			Temp += Instruction[i] + TEXT(" ");
		}

		TArray<FString> Ins;
		this->DepthReadStrReplace(
			Temp, Ins, TEXT("+"), TEXT(","), 1
		);

		FString NewCmd;
		for (const FString& NewStr : Ins)
		{
			NewCmd += NewStr;
		}
		//覆盖命令
		GiftObj->GetCmd() = NewCmd;
	}
	else {
		return false;
	}
	return true;
}

bool UGiftInstructions::UI_Edit_ListData(class UEquipmentFuncGift* GiftObj, const TArray<FString>& Instruction) {
	//编辑列表数据
	if (Instruction[1].Equals(TEXT("list")))
	{
		// findname newname id level count

		if (Instruction.Num() > 6)
		{
			TArray<FItemViewBlockGift>& Ref = GiftObj->GetListData();

			FString CurName = Instruction[2];
			for (FItemViewBlockGift& RefGift : Ref)
			{
				if (RefGift.ItemName.ToString().Equals(CurName))
				{
					//修改新的名称
					RefGift.ItemName = FText::FromString(Instruction[3]);
					//修改id，等级，和数量
					RefGift.ItemID = FCString::Atoi(*Instruction[4]);
					RefGift.ItemLevel = FCString::Atoi(*Instruction[5]);
					RefGift.BlockIndex = FCString::Atoi(*Instruction[6]);

					return true;
				}
			}


			FItemViewBlockGift New;
			//修改新的名称
			New.ItemName = FText::FromString(Instruction[3]);
			//修改id，等级，和数量
			New.ItemID = FCString::Atoi(*Instruction[4]);
			New.ItemLevel = FCString::Atoi(*Instruction[5]);
			New.BlockIndex = FCString::Atoi(*Instruction[6]);
			Ref.Emplace(New);

			return true;
		}
	}
	else {
		return false;
	}
	return true;
}

bool UGiftInstructions::UI_Edit_RandomListData(class UEquipmentFuncGift* GiftObj, const TArray<FString>& Instruction) {
	//编辑列表数据
	if (Instruction[1].Equals(TEXT("random")))
	{

		// random selectcount 1
		if (Instruction.Num() > 3 && Instruction[2].Equals(TEXT("selectcount")))
		{
			return this->UI_Edit_RandomListData_setrandomcount(GiftObj, Instruction);
		}


		if (Instruction.Num() > 7)
		{

			// randomvalue itemname_find itemname_show id level count

			FGiftBox_RandomItemMode& Ref = GiftObj->GetRandomData();

			int32 RandomValue = FCString::Atoi(*Instruction[2]);
			FString FindName = Instruction[3];

			FItemViewBlockGift New;
			//修改新的名称
			New.ItemName = FText::FromString(Instruction[4]);
			//修改id，等级，和数量
			New.ItemID = FCString::Atoi(*Instruction[5]);
			New.ItemLevel = FCString::Atoi(*Instruction[6]);
			New.BlockIndex = FCString::Atoi(*Instruction[7]);

			FGiftBox_ListItemMode* list = Ref.RandomItems.Find(RandomValue);
			if (list)
			{
				list->Items.Emplace(FindName, New);
			}
			else {
				FGiftBox_ListItemMode NewModel;
				NewModel.Items.Emplace(FindName, New);
				Ref.RandomItems.Emplace(RandomValue, NewModel);
			}

			return true;
		}
	}
	return false;
}

bool UGiftInstructions::UI_Edit_RandomListData_setrandomcount(class UEquipmentFuncGift* GiftObj, const TArray<FString>& Instruction) {

	GiftObj->GetRandomData().RandomSelectCount = FCString::Atoi(*Instruction[3]);
	if (GiftObj->GetRandomData().RandomSelectCount < 0)
	{
		GiftObj->GetRandomData().RandomSelectCount = 1;
	}

	return true;
}

bool UGiftInstructions::UI_Rem(const TArray<FString>& Instruction) {

	UUI_GiftItemTip* TipUI = GET_UI_CAST(UUI_GiftItemTip, PlayerBag, GiftItemTip);

	UEquipmentFuncGift* GiftObj = Cast<UEquipmentFuncGift>(TipUI->GetEquipmentFuncGiftObject());

	if (!IsValid(GiftObj))
	{
		return false;
	}

	if (IsValid(TipUI) && TipUI->IsInViewport())
	{
		//移除列表
		if (this->UI_Rem_ListData(GiftObj, Instruction))
		{
			return true;
		}
	}

	return false;
}

bool UGiftInstructions::UI_Rem_ListData(class UEquipmentFuncGift* GiftObj, const TArray<FString>& Instruction) {

	//编辑列表数据
	if (Instruction[1].Equals(TEXT("list")))
	{
		// findname

		if (Instruction.Num() > 2)
		{
			TArray<FItemViewBlockGift>& Ref = GiftObj->GetListData();
			FString CurName = Instruction[2];
			int32 i = 0;
			for (FItemViewBlockGift& RefGift : Ref)
			{
				if (RefGift.ItemName.ToString().Equals(CurName))
				{
					Ref.RemoveAt(i);
					return true;
				}
				i++;
			}
		}

	}

	return false;
}

bool UGiftInstructions::UI_Rem_RandomListData(class UEquipmentFuncGift* GiftObj, const TArray<FString>& Instruction) {
	//编辑列表数据
	if (Instruction[1].Equals(TEXT("random")))
	{
		// randomvalue itemname_find

		if (Instruction.Num() == 3)
		{
			FGiftBox_RandomItemMode& Ref = GiftObj->GetRandomData();

			int32 Value = FCString::Atoi(*Instruction[2]);

			Ref.RandomItems.Remove(Value);

			return true;
		}
		else if (Instruction.Num() > 3)
		{
			FGiftBox_RandomItemMode& Ref = GiftObj->GetRandomData();

			int32 Value = FCString::Atoi(*Instruction[2]);
			FString Name = Instruction[3];

			FGiftBox_ListItemMode* mode = Ref.RandomItems.Find(Value);
			if (mode)
			{
				mode->Items.Remove(Name);

				return false;
			}

			return false;
		}

	}

	return false;
}