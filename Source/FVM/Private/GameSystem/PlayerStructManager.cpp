// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/UI_GamePrepare.h"
#include "GameSystem/TaskSubsystem.h"
#include "GameSystem/PlayerDataHandle.h"
#include "GameSystem/GameDataSubsystem.h"
#include "GameSystem/PlayerDataSubsystem.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "GameSystem/GameUserInterfaceSubsystem.h"
#include "GameSystem/PlayerData/LocalEquipmentDataHandle.h"

UPlayerStructManager* __GetPlayerStructManager_t() {
	return UFVMGameInstance::GetPlayerStructManager_Static();
}

int32 UPlayerStructManager::GetBagGridCount(int32 Index)
{
	return this->M_PlayerBagGirdGroup.GetBagCount(Index);
}

FString UPlayerStructManager::GetGradeImagePath()
{
	FString _Grade_Path =
		TEXT("Texture2D'/Game/Resource/Texture/PlayerGrade/") +
		FString::FromInt(this->M_FPlayerEx.M_PlayerGrade) +
		TEXT(".") +
		FString::FromInt(this->M_FPlayerEx.M_PlayerGrade) +
		TEXT("'");

	return _Grade_Path;
}

bool UPlayerStructManager::AddCoin(int32 _Num, uint8 _Type)
{
	bool b = FPlayerCoinAdd::Sell(this, _Num, _Type);
	if (this->OnCoinUpdateDelegate.IsBound())
	{
		this->OnCoinUpdateDelegate.Broadcast();
	}
	return b;
}

bool UPlayerStructManager::ReduceCoin(int32 _Num, uint8 _Type)
{
	bool b = FPlayerCoinAdd::Buy(this, _Num, _Type);
	if (this->OnCoinUpdateDelegate.IsBound())
	{
		this->OnCoinUpdateDelegate.Broadcast();
	}
	return b;
}

bool UPlayerStructManager::SendCoin(const FString& CoinName, int32 _Num)
{

	GAME_LOG(__FUNCTION__, TEXT("发送货币"), {
		GAME_LOG_LINE(TEXT("需要重写"))
		});

	/*const TArray<FItemBase>& Buffer = FPlayerCoinAdd::GetCoinNames();
	int32 i = 0;
	for (const FItemBase& Data : Buffer)
	{
		if (Data.ItemName.Equals(CoinName))
		{
			return this->AddCoin(_Num, i);
		}
		i++;
	}*/
	return false;
}

void UPlayerStructManager::SetCoin(const FString& CoinName, int32 _Num)
{
	GAME_LOG(__FUNCTION__, TEXT("设置货币"), {
		GAME_LOG_LINE(TEXT("需要重写"))
		});

	//if (_Num <= 0)
	//{
	//	_Num = 0;
	//}
	//else if (_Num >= 1000000000)
	//{
	//	_Num = 1000000000;
	//}

	//if (CoinName.Equals(TEXT("金币")) || CoinName.Equals(TEXT("0")))
	//{
	//	if (_Num + this->M_FPlayerCoin.M_Coin_0 <= MAX_int32)
	//	{
	//		this->M_FPlayerCoin.M_Coin_0 += _Num;
	//	}
	//}

	//if (CoinName.Equals(TEXT("礼券")) || CoinName.Equals(TEXT("1")))
	//{
	//	if (_Num + this->M_FPlayerCoin.M_Coin_1 <= MAX_int32)
	//	{
	//		this->M_FPlayerCoin.M_Coin_1 += _Num;
	//	}
	//}

	//if (CoinName.Equals(TEXT("点券")) || CoinName.Equals(TEXT("2")))
	//{
	//	if (_Num + this->M_FPlayerCoin.M_Coin_2 <= MAX_int32)
	//	{
	//		this->M_FPlayerCoin.M_Coin_2 += _Num;
	//	}
	//}

	//if (CoinName.Equals(TEXT("威望")) || CoinName.Equals(TEXT("3")))
	//{
	//	if (_Num + this->M_FPlayerCoin.M_Coin_3 <= MAX_int32)
	//	{
	//		this->M_FPlayerCoin.M_Coin_3 += _Num;
	//	}
	//}

	////UGameSystemFunction::SaveCurrentPlayerData();
	//this->Save(__FUNCTION__ + FString(TEXT("货币设置操作")));
}

FCoin UPlayerStructManager::GetCoin(uint8 Type) {
	FCoin* Co = this->M_FPlayerCoin.Coins.Find(Type);
	if (Co)
	{
		return *Co;
	}
	return FCoin({ 0,FText::FromString(TEXT("未知")) });
}

bool UPlayerStructManager::CheckCoin(int32 Price, uint8 Type) {
	if (this->GetCoin(Type).CoinValue >= Price)
	{
		return true;
	}
	return false;
}

void UPlayerStructManager::AddPlayerEx(float _ExValue)
{
	UPlayerStructManager* PlayerData = UFVMGameInstance::GetPlayerStructManager_Static();

	//如果角色60级则不在增加经验
	if (PlayerData->M_FPlayerEx.M_PlayerGrade >= 60)
	{
		PlayerData->M_FPlayerEx.M_CurrentEx = PlayerData->M_FPlayerEx.M_CurrentExTop;
		return;
	}

	//叠加当前经验值
	PlayerData->M_FPlayerEx.M_CurrentEx += _ExValue;

	bool bRe = false;
	bool bUp = false;

	//将溢出的经验值保留，并且等级提升一级
	if (PlayerData->M_FPlayerEx.M_CurrentEx >= PlayerData->M_FPlayerEx.M_CurrentExTop)
	{
		//最高等级限制 60
		if (PlayerData->M_FPlayerEx.M_PlayerGrade >= 60)
		{
			PlayerData->M_FPlayerEx.M_CurrentEx = PlayerData->M_FPlayerEx.M_CurrentExTop;
			return;
		}

		//减去经验
		PlayerData->M_FPlayerEx.M_CurrentEx -= PlayerData->M_FPlayerEx.M_CurrentExTop;

		//提升等级
		PlayerData->M_FPlayerEx.M_PlayerGrade += 1;
		UTaskSubsystem::GetTaskSubsystemStatic()->ExecuteTasks(PlayerData);

		bUp = true;

		//下一次所需经验值提升  100 + 10 * 0.4
		PlayerData->M_FPlayerEx.M_CurrentExTop +=
			(PlayerData->M_FPlayerEx.M_CurrentExTop + PlayerData->M_FPlayerEx.M_ExUp) *
			PlayerData->M_FPlayerEx.M_ExUpRate;

		//经验成长值[最高限制]
		if (PlayerData->M_FPlayerEx.M_ExUp >= 8000)
		{
			PlayerData->M_FPlayerEx.M_ExUp = 8000;
		}
		else {
			PlayerData->M_FPlayerEx.M_ExUp += PlayerData->M_FPlayerEx.M_CurrentExTop * 0.50f;
		}

		if (PlayerData->M_FPlayerEx.M_CurrentEx >= PlayerData->M_FPlayerEx.M_CurrentExTop)
		{
			bRe = true;
		}

		int32 Cur = PlayerData->M_FPlayerEx.M_PlayerGrade;
		if (Cur == 3)
		{
			UGameSystemFunction::AddGameinstructions(
				UFVMGameInstance::GetFVMGameInstance(), TEXT("/mail 新人礼盒"),
				false);
		}
		else if (Cur == 5)
		{
			UGameSystemFunction::AddGameinstructions(
				UFVMGameInstance::GetFVMGameInstance(), TEXT("/mail 5级礼盒"),
				false);
		}
		else if (Cur == 10)
		{
			UGameSystemFunction::AddGameinstructions(
				UFVMGameInstance::GetFVMGameInstance(), TEXT("/mail 10级礼盒"),
				false);
		}
		else if (Cur == 15)
		{
			UGameSystemFunction::AddGameinstructions(
				UFVMGameInstance::GetFVMGameInstance(), TEXT("/mail 15级礼盒"),
				false);
		}
		else if (Cur == 20)
		{
			UGameSystemFunction::AddGameinstructions(
				UFVMGameInstance::GetFVMGameInstance(), TEXT("/mail 20级礼盒"),
				false);
		}
		else if (Cur == 30)
		{
			UGameSystemFunction::AddGameinstructions(
				UFVMGameInstance::GetFVMGameInstance(), TEXT("/mail 30级礼盒"),
				false);
		}
		else if (Cur == 40)
		{
			UGameSystemFunction::AddGameinstructions(
				UFVMGameInstance::GetFVMGameInstance(), TEXT("/mail 40级礼盒"),
				false);
		}

		if (PlayerData->M_FPlayerEx.M_PlayerGrade <= 15)
		{
			if (PlayerData->M_FPlayerEx.M_CurrentExTop > 1000)
			{
				PlayerData->M_FPlayerEx.M_CurrentExTop = 1000;
			}
		}
		else if (PlayerData->M_FPlayerEx.M_PlayerGrade > 15 && PlayerData->M_FPlayerEx.M_PlayerGrade <= 30)
		{
			if (PlayerData->M_FPlayerEx.M_CurrentExTop > 2500)
			{
				PlayerData->M_FPlayerEx.M_CurrentExTop = 2500;
			}
		}
		else if (PlayerData->M_FPlayerEx.M_PlayerGrade > 30 && PlayerData->M_FPlayerEx.M_PlayerGrade <= 45)
		{
			if (PlayerData->M_FPlayerEx.M_CurrentExTop > 4000)
			{
				PlayerData->M_FPlayerEx.M_CurrentExTop = 4000;
			}
		}
		else {
			if (PlayerData->M_FPlayerEx.M_CurrentExTop > 8000)
			{
				PlayerData->M_FPlayerEx.M_CurrentExTop = 8000;
			}
		}
	}

	//如果当前经验值依旧大于所需经验值-则递归
	if (bRe)
	{
		UPlayerStructManager::AddPlayerEx(0);
	}
	else {
		if (bUp)
		{
			CreateWidget<UWidgetBase>(
				UFVMGameInstance::GetFVMGameInstance(),
				LoadClass<UWidgetBase>(0
					, TEXT("WidgetBlueprint'/Game/Resource/BP/Game/UI/UI_Tip/BP_PlayerGradeUp.BP_PlayerGradeUp_C'"))
			)->AddToViewport(100);

			if (UGameUserInterfaceSubsystem::GetGameTaskUIInViewportSub())
			{
				UGameUserInterfaceSubsystem::RemoveGameTaskUIViewportSub();

				if (!IsValid(UUI_GamePrepare::GetGamePrepareUiStatic()))
				{
					UGameSystemFunction::LoadLastMap();
				}
			}
		}
	}
}

bool UPlayerStructManager::CheckPlayerGrade(int32 CurGrade)
{

	UPlayerStructManager* PlayerData = UFVMGameInstance::GetPlayerStructManager_Static();
	if (IsValid(PlayerData))
	{
		if (PlayerData->M_FPlayerEx.M_PlayerGrade >= CurGrade)
		{
			return true;
		}
	}

	return false;
}

void UPlayerStructManager::AddPlayerLevelScore(const FString& _LevelName, const FPlayerLevelGrade& _Grade)
{
	UPlayerStructManager* PlayerData = UFVMGameInstance::GetPlayerStructManager_Static();

	const FPlayerLevelGrade* CurLevel = PlayerData->M_PlayerLevelScore.Find(_LevelName);
	if (CurLevel)
	{
		FPlayerLevelGrade CurGrade = *CurLevel;
		CurGrade.Init(_Grade.Time, _Grade.Score, _Grade.BadCardCount);
		CurGrade.InitScoreLevel(_Grade.BadCardCount);
		PlayerData->M_PlayerLevelScore.Emplace(_LevelName, CurGrade);
	}
	else {
		PlayerData->M_PlayerLevelScore.Emplace(_LevelName, _Grade);
	}
}

bool UPlayerStructManager::AddNewPlayerLevel(const FString& LevelName, int32 State)
{
	UPlayerStructManager* PlayerData = UFVMGameInstance::GetPlayerStructManager_Static();

	int32* Cur = PlayerData->M_NewMapState.Find(LevelName);
	bool bState = true;

	if (Cur)
	{
		bState = false;
		if (*Cur != 0)
		{
			return bState;
		}
	}

	PlayerData->M_NewMapState.Emplace(LevelName, State);

	if (bState)
	{
		UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("添加新开启的关卡操作")));
	}

	return bState;
}

TMap<int32, FBaseItemSave>& UPlayerStructManager::GetCardList() {
	return this->Cards;
}

TMap<int32, FBaseItemSave>& UPlayerStructManager::GetSourceEquipList() {
	return this->SourceEquipItems;
}

TMap<int32, FEquipItemSave>& UPlayerStructManager::GetbEquipList() {
	return this->bEquipItems;
}

TMap<int32, FEquipItemSave>& UPlayerStructManager::GetNotbEquipList() {
	return this->bNotEquipItems;
}

TMap<int32, FBaseItemSave>& UPlayerStructManager::GetMaterList() {
	return this->Maters;
}

TMap<int32, FMailItemSave>& UPlayerStructManager::GetMailList() {
	return this->Mails;
}

void UPlayerStructManager::FindbEquipItem(int32 ItemID, int32 Level, int32 BlockIndex, bool& OutbEquip, bool& OutNotbEquip)const {

	//初始化数据的道具
	OutbEquip = false;
	OutNotbEquip = false;

	//已经初始化数据并且装备的道具
	const FEquipItemSave* FindbEquipData = this->bEquipItems.Find(ItemID);
	if (FindbEquipData)
	{
		//通过等级查询
		const FEquipItemBlockIndex* Index = FindbEquipData->BlockLevel.Find(Level);
		if (Index)
		{
			//查询索引
			if (Index->BlockIndex.Contains(BlockIndex))
			{
				//索引存在，返回装备的数量1
				OutbEquip = true;
				return;
			}
		}

	}

	const FEquipItemSave* FindNotbEquipData = this->bNotEquipItems.Find(ItemID);
	if (FindNotbEquipData)
	{
		//通过等级查询
		const FEquipItemBlockIndex* Index = FindNotbEquipData->BlockLevel.Find(Level);
		if (Index)
		{
			//查询索引
			if (Index->BlockIndex.Contains(BlockIndex))
			{
				//索引存在，返回非装备的数量1
				OutNotbEquip = true;
			}
		}
	}
}

FEquipItemSave* UPlayerStructManager::GetEquipItem(int32 ItemID, bool GetbEquip) {
	if (GetbEquip)
	{
		return this->bEquipItems.Find(ItemID);
	}
	else {
		return this->bNotEquipItems.Find(ItemID);
	}
	return nullptr;
}

void UPlayerStructManager::AddEquipItem(int32 ItemID, int32 Level, int32 BlockIndex, bool AddbEquip) {
	if (AddbEquip)
	{
		//从装备库移除ID
		FEquipItemSave* FindEquipData = this->bNotEquipItems.Find(ItemID);
		if (FindEquipData)
		{
			FindEquipData->Remove(Level, BlockIndex);
		}

		//新增ID到装备栏
		FEquipItemSave* FindData = this->bEquipItems.Find(ItemID);
		if (FindData)
		{
			FindData->Add(Level, BlockIndex);
		}
		else {
			FEquipItemSave EquipData;
			EquipData.Add(Level, BlockIndex);
			this->bEquipItems.Emplace(ItemID, EquipData);
		}
	}
	else {

		//......同上

		FEquipItemSave* FindbEquipData = this->bEquipItems.Find(ItemID);
		if (FindbEquipData)
		{
			FindbEquipData->Remove(Level, BlockIndex);
		}

		FEquipItemSave* FindData = this->bNotEquipItems.Find(ItemID);
		if (FindData)
		{
			FindData->Add(Level, BlockIndex);
		}
		else {
			FEquipItemSave EquipData;
			EquipData.Add(Level, BlockIndex);
			this->bNotEquipItems.Emplace(ItemID, EquipData);
		}
	}



#if WITH_EDITOR

	GAME_LOG(__FUNCTION__, TEXT("装备添加"), {

	if (AddbEquip)
	{
		GAME_LOG_LINE(
		TEXT("【当前装备栏列表】，将新物品【ID：%d，Level：%d，BlockIndex：%d】，添加到装备栏")
		,ItemID,Level,BlockIndex
		);
	}
	else {
		GAME_LOG_LINE(
		TEXT("【当前装备栏列表】，将之前的物品【ID：%d，Level：%d，BlockIndex：%d】，从装备栏移除")
		,ItemID,Level,BlockIndex
		);
	}

	for (const auto& bEquip : this->bEquipItems)
	{
		for (const auto& bEquipData : bEquip.Value.BlockLevel)
		{
			for (const auto& bEquipDataBlock : bEquipData.Value.BlockIndex)
			{
				GAME_LOG_LINE(
					TEXT("ID：%d，Level：%d，BlockIndex：%d"),
					bEquip.Key,
					bEquipData.Key,
					bEquipDataBlock
				);
			}
		}
	}

	if (AddbEquip)
	{
		GAME_LOG_LINE(
		TEXT("【未装备的道具数据列表】，将存在的物品【ID：%d，Level：%d，BlockIndex：%d】，从非装备数据库中移除")
		,ItemID,Level,BlockIndex
		);
	}
	else {
		GAME_LOG_LINE(
		TEXT("【未装备的道具数据列表】，将之前的物品【ID：%d，Level：%d，BlockIndex：%d】，添加到非装备数据库")
		,ItemID,Level,BlockIndex
		);
	}

	for (const auto& bEquip : this->bNotEquipItems)
	{
		for (const auto& bEquipData : bEquip.Value.BlockLevel)
		{
			for (const auto& bEquipDataBlock : bEquipData.Value.BlockIndex)
			{
				GAME_LOG_LINE(
					TEXT("ID：%d，Level：%d，BlockIndex：%d"),
					bEquip.Key,
					bEquipData.Key,
					bEquipDataBlock
				);
			}
		}
	}

		});
#endif

}

void UPlayerStructManager::RemoveEquipItem(
	int32 ItemID,
	int32 Level,
	int32 BlockIndex,
	int32 ItemCount,
	int32 Type
) {
	//从装备库移除ID
	FEquipItemSave* FindEquipData = this->bNotEquipItems.Find(ItemID);
	if (FindEquipData)
	{
		if (BlockIndex != -1 && FindEquipData->Remove(Level, BlockIndex))
		{
			FBaseItemSave* SaveData = this->SourceEquipItems.Find(ItemID);
			if (SaveData)
			{
				//一个索引只对应一个道具
				if (SaveData->RemoveItem(Level, 1))
				{
					if (Type == -1)
					{
						uint8 Max = (uint8)EEquipment::E_Max;
						for (uint8 Cur = 0U; Cur < Max; Cur += 1U)
						{
							//删除对应的详细数据
							if (this->RemoveEquipType(BlockIndex, Cur))
							{
#if WITH_EDITOR
								UE_LOG(LogTemp, Error, TEXT("Ptr：%p，Name：%s，RemoveCount：%d，[%d]删除成功"),
									this, *FString(__FUNCTION__), 1, ItemID);
#endif
								return;
							}
						}
					}
					else {
						//删除对应的详细数据
						if (this->RemoveEquipType(BlockIndex, (uint8)Type))
						{
#if WITH_EDITOR
							UE_LOG(LogTemp, Error, TEXT("Ptr：%p，Name：%s，RemoveCount：%d，[%d]删除成功"),
								this, *FString(__FUNCTION__), 1, ItemID);
#endif
							return;
						}
					}
				}
				else {
#if WITH_EDITOR
					UE_LOG(LogTemp, Error, TEXT("Ptr：%p，Name：%s，[%d]删除失败，当前物品数量为0"), this,
						*FString(__FUNCTION__), ItemID);
#endif
				}
			}
		}
		else {
#if WITH_EDITOR
			UE_LOG(LogTemp, Error, TEXT("Ptr：%p，Name：%s，[%d]当前物品不存在BlockIndex，直接删除数量"), this,
				*FString(__FUNCTION__), ItemID);
#endif

			FBaseItemSave* SaveData = this->SourceEquipItems.Find(ItemID);
			if (SaveData)
			{
				if (SaveData->RemoveItem(Level, ItemCount))
				{
#if WITH_EDITOR
					UE_LOG(LogTemp, Error, TEXT("Ptr：%p，Name：%s，RemoveCount：%d，[%d]删除成功"),
						this, *FString(__FUNCTION__), ItemCount, ItemID);
#endif
				}
			}
			else {
#if WITH_EDITOR
				UE_LOG(LogTemp, Error, TEXT("Ptr：%p，Name：%s，[%d]删除失败，当前物品数量为0"),
					this, *FString(__FUNCTION__), ItemID);
#endif
			}
		}
	}
	else {

		//要直接删除数量需要 装备的数量+未初始化的物品数量来进行判断

		int32 Count = 0;
		FEquipItemSave* FindbEquipData = this->bEquipItems.Find(ItemID);
		if (FindbEquipData)
		{
			Count = FindbEquipData->GetItemCount(Level);
		}


		//如果装备区域和非装备区域都是空的，则直接删除数量
		FBaseItemSave* Save = this->SourceEquipItems.Find(ItemID);
		if (Save && ItemCount - Count > 0)
		{
			if (Save->RemoveItem(Level, ItemCount - Count))
			{
#if WITH_EDITOR
				UE_LOG(LogTemp, Error, TEXT("Ptr：%p，Name：%s，Level:%d，RemoveCount：%d，[%d]删除成功"),
					this, *FString(__FUNCTION__), Level, ItemCount - Count, ItemID);
#endif
			}
			else {
#if WITH_EDITOR
				UE_LOG(LogTemp, Error, TEXT("Ptr：%p，Name：%s，[%d]删除失败，当前物品数量为0"),
					this, *FString(__FUNCTION__), ItemID);
#endif
			}
		}
		else {
#if WITH_EDITOR
			UE_LOG(LogTemp, Error, TEXT("Ptr：%p，Name：%s，[%d]删除失败，当前需要删除的数量为0，不能删除0个道具！"),
				this, *FString(__FUNCTION__), ItemID);
#endif
		}
	}
}

bool UPlayerStructManager::RemoveEquipType(int32 BlockIndex, uint8 Type) {

	if (BlockIndex == -1)
	{
		return false;
	}

	if (IsValid(this->GetLocalEquipmentDataHandle()))
	{
		bool bRemoveResult = Cast<ULocalEquipmentDataSaveGame>(
			this->LocalEquipmentDataHandle->Execute_GetData(this->LocalEquipmentDataHandle)
		)->RemoveEquipType(BlockIndex, Type);

		if (bRemoveResult)
		{
			this->LocalEquipmentDataHandle->Execute_SaveData(
				this->LocalEquipmentDataHandle, TEXT("删除装备细节数据")
			);
		}
	}

	return false;
}

void UPlayerStructManager::RemoveAllEquips() {
	//当前装备的物品
	TMap<int32, FEquipItemSave> bEquipCopy = this->bEquipItems;
	//删除不带有索引的物品
	for (const auto& Item : bEquipCopy)
	{
		for (const auto& ItemLevel : Item.Value.BlockLevel)
		{
			//最大个数
			int32 Max = this->SourceEquipItems.Find(Item.Key)->GetItemCount(ItemLevel.Key);

			//减去带有索引的道具【未装备】
			FEquipItemSave* ItemSave = this->bNotEquipItems.Find(Item.Key);
			if (ItemSave)
			{
				Max -= ItemSave->BlockLevel.Num();
			}

			//删除所有没有索引装备的物品
			this->RemoveEquipItem(Item.Key, ItemLevel.Key, -1,
				Max - ItemLevel.Value.BlockIndex.Num());
		}
	}

	//重新赋值装备道具数量
	TMap<int32, FBaseItemSave> NewEquip;
	for (const auto& Item : bEquipCopy)
	{
		FBaseItemSave SaveData;
		for (const auto& ItemLevel : Item.Value.BlockLevel)
		{
			SaveData.AddItem(ItemLevel.Key, ItemLevel.Value.BlockIndex.Num());
		}

		NewEquip.Emplace(Item.Key, SaveData);
	}

	this->SourceEquipItems.Empty();
	this->SourceEquipItems.Append(MoveTempIfPossible(NewEquip));
}

void UPlayerStructManager::RemoveAllEquipItems() {

	TMap<int32, FEquipItemSave> EquipCopy = this->bNotEquipItems;
	TArray<FEquipItemSave> EquipCopyArray;
	EquipCopy.GenerateValueArray(EquipCopyArray);

	//删除带有索引但是未装备的物品
	for (const auto& Item : EquipCopyArray)//遍历所有的物品
	{

		//查询物品类型
		bool bFindType = false;
		//最大类型
		uint8 Max = (uint8)EEquipment::E_Max;
		//当前类型
		uint8 CurType = 0U;

		for (const auto& ItemLevel : Item.BlockLevel)//遍历当前物品所有的等级
		{
			for (const auto& BlockIndex : ItemLevel.Value.BlockIndex)//遍历当前物品的等级下所有的索引
			{
				if (bFindType)
				{
					this->RemoveEquipType(BlockIndex, CurType);
				}
				else {
					while (CurType != Max)
					{
						//删除所有物品详细数据
						if (this->RemoveEquipType(BlockIndex, CurType))
						{
							bFindType = true;
							break;
						}
						else {
							CurType += 1U;
						}
					}
				}
			}
		}
	}

	//清理所有的非装备物品
	this->bNotEquipItems.Empty();
}

bool UPlayerStructManager::FindCard(int32 ID, int32 Level, int32 FindCount) {

	FBaseItemSave* Save = this->Cards.Find(ID);
	if (Save)
	{
		if (Save->GetItemCount(Level) >= FindCount)
		{
			return true;
		}
	}
	return false;
}

bool UPlayerStructManager::ConsumeCard(int32 ID, int32 Level, int32 Count) {
	FBaseItemSave* Save = this->Cards.Find(ID);
	if (Save)
	{
		return Save->RemoveItem(Level, Count);
	}
	return false;
}

bool UPlayerStructManager::ConsumeMaterial(int32 ID, int32 Level, int32 Count) {

	FBaseItemSave* Save = this->Maters.Find(ID);
	if (Save)
	{
		int32* CurCount = Save->ItemCount.Find(Level);
		if (CurCount)
		{
			if (Count <= *CurCount)
			{
				*CurCount -= Count;

				if (*CurCount < 1)
				{
					Save->ItemCount.Remove(Level);
					if (!Save->IsValid())
					{
						this->Maters.Remove(ID);
					}
				}

				return true;
			}
		}
	}

	return false;
}

bool UPlayerStructManager::FindMaterialCount(int32 ID, int32 Level, int32 Count) {
	FBaseItemSave* Save = this->Maters.Find(ID);
	if (Save)
	{
		int32* CurCount = Save->ItemCount.Find(Level);
		if (CurCount && Count <= *CurCount)
		{
			return true;
		}
	}
	return false;
}

TMap<FString, FPlayerSelectListConfig>& UPlayerStructManager::GetCardSelectConfig()
{
	return this->CardsSelectListConfig;
}

FPlayerOtherData& UPlayerStructManager::GetOtherData()
{
	return this->OtherData;
}

FString UPlayerStructManager::GetLastNewLevelName()
{
	UPlayerStructManager* PlayerData = UFVMGameInstance::GetPlayerStructManager_Static();

	if (PlayerData->M_NewMapState.Num())
	{
		TArray<FString> OutKeys;
		PlayerData->M_NewMapState.GetKeys(OutKeys);

		return OutKeys[OutKeys.Num() - 1];
	}

	return FString();
}

void UPlayerStructManager::Update()
{
	if (!IsValid(UGameDataSubsystem::GetGameDataSubsystemStatic()))
	{
		return;
	}

	UDataTable* Table = UGameDataSubsystem::GetGameDataSubsystemStatic()->GetAsset()->
		GetDataByName(
			MoveTemp(GameDataNameAsset::GameDataAsset_GlobalAsset_Player::GlobalAsset_Player)
		)->GetDataByName(
			MoveTemp(GameDataNameAsset::GameDataAsset_GlobalAsset_Player::Update)
		);
	DataTableAssetData<FPlayerUpdateClassTableRowBase> UpdataClassData(Table);

	//执行更新
	for (const auto& CurUpdateClass : UpdataClassData.GetDatas())
	{
		for (const auto& ClassPtr : CurUpdateClass.Value.UpdateClass)
		{
			UPlayerUpdateClass* CurNewPlayerUpdateClass = nullptr;
			UClass* CurClass = ClassPtr.Value.LoadSynchronous();
			if (IsValid(CurClass))
			{
				TSubclassOf<UPlayerUpdateClass> CurFunClass(CurClass);
				if (CurFunClass.GetDefaultObject())
				{
					CurNewPlayerUpdateClass = CurFunClass.GetDefaultObject();
					if (IsValid(CurNewPlayerUpdateClass))
					{
						IPlayerUpdateClassInterface* UpdateInterface = nullptr;
						UpdateInterface = Cast<IPlayerUpdateClassInterface>(CurNewPlayerUpdateClass);
						if (UpdateInterface)
						{
							UpdateInterface->Execute_Update(CurNewPlayerUpdateClass, this);
						}
					}
				}
			}
		}
	}

	UGameSystemFunction::SaveCurrentPlayerData(__FUNCTION__ + FString(TEXT("角色存档核心数据更新")));
}

ULocalEquipmentDataHandle* UPlayerStructManager::GetLocalEquipmentDataHandle() {

	if (!IsValid(this->LocalEquipmentDataHandle))
	{
		//加载装备数据句柄
		this->LocalEquipmentDataHandle = Cast<ULocalEquipmentDataHandle>(
			UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->
			GetLocalPlayerDataHandle()->
			LoadPlayerDataPageHandle(EPlayerDataConfigPage::Equip));
	}

	return this->LocalEquipmentDataHandle;
}
