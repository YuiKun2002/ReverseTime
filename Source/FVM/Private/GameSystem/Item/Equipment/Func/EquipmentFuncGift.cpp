// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/Equipment/Func/EquipmentFuncGift.h"
#include "Game/UI/UI_PlayerBagEquipmentGrid.h"
#include "Game/UI/Tips/UI_PlayerGetItemBox.h"
#include "Game/UI/Tips/UI_GiftItemTip.h"
#include "Game/UI/Tips/UI_ItemDesTip.h"
#include "Data/EquipmentDataSturct.h"
#include <Components/VerticalBox.h>
#include "Data/GameLogSubsystem.h"
#include "Game/UI/UI_PlayerBag.h"

void UEquipmentFuncGift::Execute(
	class UItemGridBase* EquipGridPtr,
	struct FItemBaseStructData* ItemBasePtr
) {

	//重写提示窗口
	if (!IsValid(this->GetTipWidget()))
	{
		this->TipUI = GET_UI_CAST(UUI_GiftItemTip, PlayerBag, GiftItemTip);
		TipUI->SetItemCountMax(
			Cast<UUI_PlayerBagEquipmentGrid>(EquipGridPtr)->GetItemCountViewBlock().Count
		);
		TipUI->SetEquipmentFuncGiftObject(this);
		TipUI->GetOkButtonComponent()->SetIsEnabled(true);
		this->SetTipWidget(this->TipUI);
	}

	Super::Execute(EquipGridPtr, ItemBasePtr);

	this->GetTipWidget()->SetHidden(false);

	this->GetTipWidget()->SetOkButtonTitleText(TEXT("打开"));

	this->GetTipWidget()->BindClickFunc(this, TEXT("Use"));

	//解析数据
	FEquipment_GiftBox_Data Data = CAST_TYPE(FEquipment_GiftBox_Data, ItemBasePtr);
	//解析列表【可直接发送给玩家的数据】
	Data.M_FEquipment.ListItemMode.Items.GenerateValueArray(this->ListData);
	this->RandomData = Data.M_FEquipment.RandomItemMode;

	if (!Data.M_FEquipment.Cmd.IsEmpty())
	{
		this->Cmd = Data.M_FEquipment.Cmd;
	}

	if (!Data.M_FEquipment.CmdExecute.IsEmpty())
	{
		this->ExecCmd = Data.M_FEquipment.CmdExecute;
	}

	//打开界面时触发指令
	this->UseInstruction(this->ExecCmd, 1);
	//显示内容
	this->ShowRandomDataContent();
	this->ShowListDataContent();
	//显示指令内容
	this->ShowCmdContent();
}

void UEquipmentFuncGift::Use() {

	if (IsValid(this->TipUI))
	{

		this->SendItem(this->TipUI->GetUseCount());


		UGameSystemFunction::SaveCurrentPlayerData(TEXT("开启礼包"));

		//清理缓存
		GameDataStaticObject<UPlayerBagEquipView>()->ClearCache();

		//获取背包UI
		GET_UI_CAST(UUI_PlayerBag, Global, PlayerBag)->LoadItemsData();
	}
}

void UEquipmentFuncGift::SendItem(int32 UseCount) {

	if (UseCount <= 0)
	{
		return;
	}

	//使用指令
	this->UseInstruction(this->Cmd, UseCount);

	//所有获得的物品
	TMap<int32, TMap<int32, int32>> Items;
	//所有获得的货币
	TMap<int32, FItemViewBlock> Coins;


	UPlayerStructManager* Player = GET_PLAYERDATAINS();

	//发送列表道具
	for (const auto& ListItem : this->ListData)
	{
		if (ListItem.ItemID == -1 && ListItem.ItemLevel != -1)
		{
			FItemViewBlock* Block = Coins.Find(ListItem.ItemLevel);
			if (Block)
			{
				Block->BlockIndex += ListItem.BlockIndex * UseCount;
			}
			else {
				Coins.Emplace(ListItem.ItemLevel,
					FItemViewBlock{ ListItem.ItemID,
					ListItem.ItemLevel,
					ListItem.BlockIndex * UseCount });
			}

		}
		else {
			TMap<int32, int32>* TBlock = Items.Find(ListItem.ItemID);
			if (TBlock)
			{
				int32* Count = TBlock->Find(ListItem.ItemLevel);
				if (Count)
				{
					*Count += ListItem.BlockIndex * UseCount;
				}
				else {
					TBlock->Emplace(ListItem.ItemLevel, ListItem.BlockIndex * UseCount);
				}
			}
			else {
				TMap<int32, int32> TpBlock;
				TpBlock.Emplace(ListItem.ItemLevel, ListItem.BlockIndex * UseCount);
				Items.Emplace(ListItem.ItemID, TpBlock);
			}
		}
	}

	//计算概率
	TArray<int32> Values;//概率范围
	TMap<int32, int32> RandomKey;//概率范围映射【概率范围映射权重】
	TMap<int32, int32> KeyRandom;//权重映射【权重映射概率范围】
	int32 AddedValue = 0;//概率范围之和
	this->GenRandom(Values, RandomKey, KeyRandom, AddedValue);

	if (Values.Num())
	{

		//随机选取的次数
		for (int32 i = this->RandomData.RandomSelectCount * UseCount; i > 0; i--)
		{
			int32 LastValue = Values[Values.Num() - 1];

			int32 RandomValue = UGameSystemFunction::GetRandomRange(0, AddedValue);

			int32 CurRandomKey = -1;

			//随机数
			for (const auto& CurValue : Values)
			{
				//非最后一个概率
				if (CurValue >= RandomValue)
				{
					CurRandomKey = CurValue;
					break;
				}
				else {
					//判断是否随机到最后一位的范围
					if (CurValue == LastValue)
					{
						CurRandomKey = CurValue;
						break;
					}
				}
			}

			if (CurRandomKey == -1)
			{
				CurRandomKey = LastValue;
			}

			//发送对应数值的数据[通过映射进行转换]
			FGiftBox_ListItemMode* ListMode = this->RandomData.RandomItems.Find(*RandomKey.Find(CurRandomKey));
			if (ListMode)
			{

				//解析列表
				TArray<FItemViewBlockGift> Gifts;
				ListMode->Items.GenerateValueArray(Gifts);

				//随机列表
				if (Gifts.Num())
				{
					int32 RanIndex = UGameSystemFunction::GetRandomRange(0, Gifts.Num() - 1);

					FItemViewBlockGift& RefData = Gifts[RanIndex];

					if (RefData.ItemID == -1 && RefData.ItemLevel != -1)
					{
						FItemViewBlock* TBlock = Coins.Find(RefData.ItemLevel);
						if (TBlock)
						{
							TBlock->BlockIndex += RefData.BlockIndex;
						}
						else {
							Coins.Emplace(RefData.ItemLevel,
								FItemViewBlock{ RefData.ItemID,
								RefData.ItemLevel,
								RefData.BlockIndex });
						}
					}
					else {
						TMap<int32, int32>* TBlock = Items.Find(RefData.ItemID);
						if (TBlock)
						{
							int32* Count = TBlock->Find(RefData.ItemLevel);
							if (Count)
							{
								*Count += RefData.BlockIndex;
							}
							else {
								TBlock->Emplace(RefData.ItemLevel, RefData.BlockIndex);
							}
						}
						else {
							TMap<int32, int32> TpBlock;
							TpBlock.Emplace(RefData.ItemLevel, RefData.BlockIndex);
							Items.Emplace(RefData.ItemID, TpBlock);
						}
					}

				}
			}
		}
	}


	//发送道具
	for (const auto& Item : Items)
	{
		for (const auto& ItemLevel : Item.Value)
		{
			UGameSystemFunction::SendItemToBag(
				Item.Key,
				ItemLevel.Key,
				ItemLevel.Value,
				0U, false);

			UUI_PlayerGetItemBox::GetPlayerGetItemBoxTipWidget()->AddItem(Item.Key, ItemLevel.Key, ItemLevel.Value);
		}
	}


	//发送货币
	for (const auto& Item : Coins)
	{
		Player->AddCoin(Item.Value.BlockIndex, Item.Value.ItemLevel);

		UUI_PlayerGetItemBox::GetPlayerGetItemBoxTipWidget()->AddItemName(
			Player->GetCoin(Item.Value.ItemLevel).CoinName,
			-1, Item.Value.ItemLevel, Item.Value.BlockIndex);

	}

	//删除道具
	//Player->RemoveEquipItem(this->BlockData.ItemID, this->BlockData.ItemLevel, this->BlockData.BlockIndex, UseCount);
}

void UEquipmentFuncGift::ShowListDataContent() {

	for (const auto& ListItem : this->ListData)
	{
		//加载礼盒项目UI UUI_GiftItemTip_Item
		UUI_GiftItemTip_Item* Item = this->TipUI->GetGiftItemTipItem();
		Item->SetGiftItemItemData(ListItem.ItemID, ListItem.ItemLevel, ListItem.ItemName, ListItem.BlockIndex, 100.f);
		this->TipUI->AddGiftItemTipItem(Item);
	}

	if (this->ListData.Num())
	{
		this->TipUI->AddTipContent(
			RICHLINE +
			RICHTEXT_LABLE(TEXT("100%礼盒获取规则："), RichText_Game::T35TITLE)
		);

		this->TipUI->AddTipContent(
			RICHLINE + RICHTAB + RICHTEXT_LABLE(TEXT("提示1：可以直接获取全部的道具"), RichText_Game::T35PINKBOX)
		);
	}
}

void UEquipmentFuncGift::ShowRandomDataContent() {

	//计算概率
	TArray<int32> Values;//概率范围
	TMap<int32, int32> RandomKey;//概率范围映射【概率范围映射权重】
	TMap<int32, int32> KeyRandom;//权重映射【权重映射概率范围】
	int32 AddedValue = 0;//概率范围之和
	this->GenRandom(Values, RandomKey, KeyRandom, AddedValue);

	//变量条目
	bool bCp = false;
	int32 Index = 0;
	for (const auto& List : RandomData.RandomItems)
	{

		//不允许出现权重为0的条目
		if (List.Key <= 0)
		{
			continue;
		}

		//获取原来的数据
		float SourceF = SourceF = ((((*KeyRandom.Find(List.Key)) * 1.f) / AddedValue)) * 100.f + 0.0005;
		float TRandomFloat = (int32)(SourceF * 100.f);
		//当前条目总概率
		float Point = TRandomFloat / 100;
		//当前道具条目个数
		int32 ItemCount = List.Value.Items.Num();
		//获得条目
		for (const auto& Item : List.Value.Items)
		{

			// 0.33333  // 0.33,38  = 33.3%
			// 0.06666  // 0.66,71  = 66.7%
			// 0.60000  // 0.60,05  = 60.0%

			//加载礼盒项目UI UUI_GiftItemTip_Item
			UUI_GiftItemTip_Item* ItemUI = this->TipUI->GetGiftItemTipItem();

			//获取单个条目的概率
			if (ItemCount > 1)
			{
				//有多个道具组合
				bCp = true;

				ItemUI->SetGiftItemItemData(
					Item.Value.ItemID,
					Item.Value.ItemLevel,
					Item.Value.ItemName,
					Item.Value.BlockIndex,
					Point / ItemCount
				);
			}
			else {
				ItemUI->SetGiftItemItemData(
					Item.Value.ItemID,
					Item.Value.ItemLevel,
					Item.Value.ItemName,
					Item.Value.BlockIndex,
					Point
				);
			}

			this->TipUI->AddGiftItemTipItem(ItemUI);
		}

		Index++;
	}


	this->TipUI->SetTipContent(FString());

	if (Values.Num()) {
		if (IsValid(this->TipUI))
		{
			this->TipUI->AddTipContent(
				RICHLINE +
				RICHTEXT_LABLE(TEXT("随机礼盒抽取规则："), RichText_Game::T35TITLE)
			);

			this->TipUI->AddTipContent(
				RICHLINE + RICHTAB + RICHTEXT_LABLE(TEXT("提示1：随机抽取["), RichText_Game::T35PINKBOX) +
				RICHTEXT_LABLE(FString::FromInt(RandomData.RandomSelectCount), RichText_Game::T35RED) +
				RICHTEXT_LABLE(TEXT("]种道具！"), RichText_Game::T35PINKBOX) +
				RICHTEXT_LABLE(TEXT("(随机模式无保底)"), RichText_Game::T35RED)
			);

			if (RandomData.RandomSelectCount > 1)
			{
				RICHTEXT_LABLE(
					TEXT("	(如果抽取次数大于1，相当于一个礼盒可以随机两种道具，例如：AB，AC，BC，BB，AA.....这种等等)")
					, RichText_Game::T35PINKBOX);
			}

			if (bCp)
			{
				this->TipUI->AddTipContent(
					RICHLINE + RICHTAB +
					RICHTEXT_LABLE(
						TEXT("提示2：将概率相加，四舍五入取整，都为100%，出现小数点是因为浮点的精度问题，例如：0.4 可能显示为 0.390001"),
						RichText_Game::T35PINKBOX) + RICHLINE
				);


				this->TipUI->AddTipContent(
					RICHTEXT_LABLE(TEXT("相同概率的道具--总概率计算："), RichText_Game::T35TITLE)
				);

				this->TipUI->AddTipContent(
					RICHLINE + RICHTAB +
					RICHTEXT_LABLE(TEXT("例如："), RichText_Game::T35PINK) +
					RICHTEXT_LABLE(TEXT("组合概率"), RichText_Game::T35RED) +
					RICHTEXT_LABLE(TEXT("(x)乘以"), RichText_Game::T35PINK) +
					RICHTEXT_LABLE(TEXT(" (当前相同名称的道具数量"), RichText_Game::T35RED) +
					RICHTEXT_LABLE(TEXT("(/)除以"), RichText_Game::T35PINK) +
					RICHTEXT_LABLE(TEXT("组合总数"), RichText_Game::T35RED) +
					RICHTEXT_LABLE(TEXT(" = 当前同名的道具概率"), RichText_Game::T35PINK)
				);

				this->TipUI->AddTipContent(
					RICHLINE + RICHTAB +
					RICHTEXT_LABLE(TEXT("例如："), RichText_Game::T35PINK) +
					RICHTEXT_LABLE(TEXT("A：10% A：10% B：10% C：10%"), RichText_Game::T35RED) +
					RICHTEXT_LABLE(TEXT("那么组合概率为："), RichText_Game::T35PINK) +
					RICHTEXT_LABLE(TEXT("40%"), RichText_Game::T35RED) +
					RICHTEXT_LABLE(TEXT("代入：0.4 * 2 / 4 = 0.2(20%)"), RichText_Game::T35PINK) +
					RICHTEXT_LABLE(TEXT("A道具"), RichText_Game::T35RED) +
					RICHTEXT_LABLE(TEXT("总概率为："), RichText_Game::T35PINK) +
					RICHTEXT_LABLE(TEXT("20%"), RichText_Game::T35RED)
				);

				this->TipUI->AddTipContent(
					RICHLINE + RICHTAB +
					RICHTEXT_LABLE(TEXT("0.4 = "), RichText_Game::T35PINK) +
					RICHTEXT_LABLE(TEXT("10% * 4 / 100   "), RichText_Game::T35RED) +
					RICHTEXT_LABLE(TEXT("2 = "), RichText_Game::T35PINK) +
					RICHTEXT_LABLE(TEXT("所有A道具的数量和   "), RichText_Game::T35RED) +
					RICHTEXT_LABLE(TEXT("4 = "), RichText_Game::T35PINK) +
					RICHTEXT_LABLE(TEXT(" A + A + B + C 四个道具   "), RichText_Game::T35RED)
				);

				this->TipUI->AddTipContent(
					RICHLINE + RICHTAB +
					RICHTEXT_LABLE(
						TEXT("简化：概率 * 相同道具的数量（10% * 2 = 20%）"),
						RichText_Game::T35PINKBOX) + RICHLINE
				);
			}
		}
	}
}

void UEquipmentFuncGift::ShowCmdContent() {

	if (!this->ExecCmd.IsEmpty())
	{
		this->TipUI->AddTipContent(
			RICHLINE +
			RICHTEXT_LABLE(TEXT("UI界面运行时触发的指令："), RichText_Game::T35TITLE)
		);

		this->TipUI->AddTipContent(
			RICHLINE + RICHTAB + RICHTEXT_LABLE(TEXT("提示1："), RichText_Game::T35PINKBOX) +
			RICHTEXT_LABLE(this->ExecCmd, RichText_Game::T35RED) + RICHLINE
		);
	}

	if (!this->Cmd.IsEmpty())
	{
		this->TipUI->AddTipContent(
			RICHLINE +
			RICHTEXT_LABLE(TEXT("使用道具时触发的指令："), RichText_Game::T35TITLE)
		);

		this->TipUI->AddTipContent(
			RICHLINE + RICHTAB + RICHTEXT_LABLE(TEXT("提示1："), RichText_Game::T35PINKBOX) +
			RICHTEXT_LABLE(this->Cmd, RichText_Game::T35RED) + RICHLINE +
			RICHTEXT_LABLE(TEXT("【提示：(_Count_ 符号)表示开启礼盒的次数 】"), RichText_Game::T35PINK)
		);

		this->TipUI->AddTipContent(
			RICHLINE + RICHTAB + RICHTEXT_LABLE(TEXT("提示2："), RichText_Game::T35PINKBOX) +
			RICHTEXT_LABLE(
				TEXT("部分指令使用时，会有明显的卡顿，例如：发送全部的道具。底层：检索相关的数据表解析物品的ID号，并且初始化后缓存，然后发送到背包，期间会有多次循环和查询、新增操作！。"),
				RichText_Game::T35PINK)
		);
	}
}

void UEquipmentFuncGift::UseInstruction(FString CurCmd, int32 Count) {

	//数量限制
	int32 BaseCount = RandomData.RandomSelectCount;
	if (BaseCount < 0)
	{
		BaseCount = 1;
	}

	if (Count * BaseCount >= INT32_MAX)
	{
		BaseCount = INT32_MAX - 40;
	}
	else {
		BaseCount = Count * BaseCount;
	}

	FString NewCmd = CurCmd.ToLower();

	//数量更新
	FString TargetCmd = NewCmd.Replace(TEXT("(_count_)"), *FString::FromInt(BaseCount));

	UGameLogSubsystem* Cur = UGameplayStatics::GetGameInstance(
		GAMEINS()->GetWorld()
	)->GetSubsystem<UGameLogSubsystem>();
	if (IsValid(Cur))
	{
		Cur->ExeInstructions(TargetCmd);
	}
}

void UEquipmentFuncGift::GenRandom(
	TArray<int32>& OutRandomValuesRange,
	TMap<int32, int32>& RandomValuesRangeToKey,
	TMap<int32, int32>& KeyToRandomValuesRange,
	int32& AddedValueSum
) {
	//计算概率
	TArray<int32> SourceValues;//权重【 100  10  58  200】【0 10 58 100 200】
	RandomData.RandomItems.GenerateKeyArray(SourceValues);

	//排序[小到大]
	SourceValues.Sort();

	//权重转换概率范围
	int32 TInex = 0;
	for (const auto& Value : SourceValues)
	{
		if (TInex == 0)
		{
			if (Value > 0)
			{
				AddedValueSum = Value;
				OutRandomValuesRange.Emplace(Value);
				RandomValuesRangeToKey.Emplace(Value, Value);
				KeyToRandomValuesRange.Emplace(Value, Value);
			}
		}
		else {
			//概率范围结果
			int32 Res = Value - SourceValues[TInex - 1];
			AddedValueSum += Res;
			OutRandomValuesRange.Emplace(Res);

			RandomValuesRangeToKey.Emplace(Res, Value);
			KeyToRandomValuesRange.Emplace(Value, Res);
		}

		TInex++;
	}

	//排序[小到大]
	OutRandomValuesRange.Sort();
}