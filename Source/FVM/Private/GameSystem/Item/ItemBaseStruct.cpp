// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除
// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/ItemBaseStruct.h"
#include "Engine/Texture2D.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/Tools/ItemLoadManager.h"
#include "Data/Texture/TextureDataAssetCache.h"

//全局静态流加载实例句柄
//static TSharedPtr<FStreamableHandle> UItemBaseStruct::M_Globle_StreamableHandle;
//全局静态物品基础对象
//static UItemBaseStruct* UItemBaseStruct::M_Globle_UItemBaseStruct_Ptr = nullptr;

void FBaseItemSave::AddItem(int32 Level, int32 Count)
{
	int32* CurItemCount = this->ItemCount.Find(Level);
	if (CurItemCount)
	{
		//最大数量限制
		long long CurCount = *CurItemCount + Count;
		if (CurCount > INT32_MAX / 2)
		{
			*CurItemCount = INT32_MAX / 2;
		}
		else {
			*CurItemCount += Count;
		}
	}
	else {
		this->ItemCount.Emplace(Level, Count);
	}
}

bool FBaseItemSave::RemoveItem(int32 Level, int32 Count)
{
	int32* CurItemCount = this->ItemCount.Find(Level);
	if (CurItemCount) {
		*CurItemCount -= Count;

		if (*CurItemCount < 1)
		{
			this->ItemCount.Remove(Level);
		}
		return true;
	}
	return false;
}

int32 FBaseItemSave::GetItemCount(int32 Level) {
	int32* CurItemCount = this->ItemCount.Find(Level);
	if (CurItemCount)
	{
		return *CurItemCount;
	}
	return 0;
}

bool FBaseItemSave::IsValid() {
	if (this->ItemCount.Num())
	{
		return true;
	}
	return false;
}

void FBaseItemSave::ToArray(int32 ID, TArray<FItemViewBlock>& OutItemBlock, int32 OutCount) const
{
	//限制判断
	if (OutCount > 0 && OutItemBlock.Num() == OutCount)
	{
		return;
	}

	//#if WITH_EDITOR
	//	UE_LOG(LogTemp, Warning, TEXT("Ptr：%p，Name：%s，解析物品数据，ID：%d "), this, *FString(__FUNCTION__), ID);
	//#endif

	for (const auto& Data : this->ItemCount)
	{
		//数据块
		FItemViewBlock NewBlock;
		NewBlock.ItemID = ID;
		NewBlock.ItemLevel = Data.Key;
		NewBlock.BlockIndex = 0;

		//当前数量
		int32 CurCount = OutItemBlock.Num();
		//当前总数量
		int32 Total = CurCount + Data.Value;
		//新增数量
		int32 NewCount = 0;

		//数量限制
		if (OutCount <= 0)
		{
			//初始化索引都为 Index 的数据块
			for (int32 Count = 0; Count < Data.Value; ++Count)
			{
				NewBlock.BlockIndex = Count;
				OutItemBlock.Emplace(NewBlock);

			}
		}
		else {

			//算出新增个数：新增数量 - (当前总数量 - 限制数量) = 需要增加的数量
			NewCount = Total > OutCount ? Data.Value - (Total - OutCount) : Data.Value;

			if (NewCount == 0)
			{
				return;
			}

			//初始化索引都为 Index 的数据块
			for (int32 Count = 0; Count < NewCount; ++Count)
			{
				NewBlock.BlockIndex = Count;
				OutItemBlock.Emplace(NewBlock);
			}
		}
	}
}

void FBaseItemSave::ToArrayCheckEquip(
	UPlayerStructManager* Player,
	int32 ID,
	const TMap<int32, FEquipItemSave>& EquipItemSave,
	TArray<FItemViewBlock>& OutItemBlockbEuip,
	TArray<FItemViewBlock>& OutItemBlockNormal,
	int32 OutCount) const
{
	//限制判断
	if (OutCount > 0 && OutItemBlockbEuip.Num() + OutItemBlockNormal.Num() == OutCount)
	{
		return;
	}

	//#if WITH_EDITOR
	//	UE_LOG(LogTemp, Warning, TEXT("Ptr：%p，Name：%s，解析物品数据，ID：%d "), this, *FString(__FUNCTION__), ID);
	//#endif

		//查询Index
	const FEquipItemSave* SaveData = EquipItemSave.Find(ID);
	//获取未装备的索引
	const FEquipItemSave* EquipSaveData = Player->GetEquipItem(ID, false);

	for (const auto& Data : this->ItemCount)
	{
		//数据块
		FItemViewBlock NewBlock;
		NewBlock.ItemID = ID;
		NewBlock.ItemLevel = Data.Key;
		NewBlock.BlockIndex = 0;

		//当前数量
		int32 CurCount = OutItemBlockbEuip.Num() + OutItemBlockNormal.Num();
		//当前总数量
		int32 Total = CurCount + Data.Value;
		//新增数量
		int32 NewCount = 0;

		//数量限制
		if (OutCount <= 0)
		{
			//初始化已经装备的索引
			//has Index
			TSet<int32> Indexs;
			if (SaveData) {
				SaveData->GetAllBlockIndex(Data.Key, Indexs);
				for (const auto& LastIndex : Indexs)
				{
					NewBlock.BlockIndex = LastIndex;
					OutItemBlockbEuip.Emplace(NewBlock);
				}
			}

			TSet<int32>  EquipIndexs;
			if (EquipSaveData) {
				EquipSaveData->GetAllBlockIndex(Data.Key, EquipIndexs);
				for (const auto& LastIndex : EquipIndexs)
				{
					NewBlock.BlockIndex = LastIndex;
					OutItemBlockNormal.Emplace(NewBlock);
				}
			}

			Indexs.Append(EquipIndexs);

			//减去已经装备的个数
			NewCount = Data.Value - Indexs.Num();

			//初始化未装备的索引
			int32 BlockIndex = 0;
			for (int32 Count = 0; Count < NewCount; ++BlockIndex)
			{
				if (Indexs.Contains(BlockIndex))
				{
					Indexs.Remove(BlockIndex);
				}
				else {
					NewBlock.BlockIndex = BlockIndex;
					OutItemBlockNormal.Emplace(NewBlock);
					++Count;
				}
			}
		}
		else {

			//算出新增个数：新增数量 - (当前总数量 - 限制数量) = 需要增加的数量
			NewCount = Total > OutCount ? Data.Value - (Total - OutCount) : Data.Value;

			if (NewCount == 0)
			{
				return;
			}

			//初始化已经装备的索引
			//has Index
			TSet<int32> Indexs;
			if (SaveData) {
				SaveData->GetAllBlockIndex(Data.Key, Indexs);
				for (const auto& LastIndex : Indexs)
				{
					NewBlock.BlockIndex = LastIndex;
					OutItemBlockbEuip.Emplace(NewBlock);
				}
			}

			TSet<int32>  EquipIndexs;
			if (EquipSaveData) {
				EquipSaveData->GetAllBlockIndex(Data.Key, EquipIndexs);
				for (const auto& LastIndex : EquipIndexs)
				{
					NewBlock.BlockIndex = LastIndex;
					OutItemBlockNormal.Emplace(NewBlock);
				}
			}

			Indexs.Append(EquipIndexs);

			//减去已经装备的个数
			NewCount -= Indexs.Num();

			//初始化未装备的索引
			int32 BlockIndex = 0;
			for (int32 Count = 0; Count < NewCount; ++BlockIndex)
			{
				if (Indexs.Contains(BlockIndex))
				{
					Indexs.Remove(BlockIndex);
				}
				else {
					NewBlock.BlockIndex = BlockIndex;
					OutItemBlockNormal.Emplace(NewBlock);
					++Count;
				}
			}
		}
	}
}

void FBaseItemSave::ToArrayZip(int32 ID, TArray<FItemViewBlock>& OutItemBlock, int32 OutCount) const
{
	//限制判断
	if (OutCount > 0 && OutItemBlock.Num() == OutCount)
	{
		return;
	}

	//#if WITH_EDITOR
		//UE_LOG(LogTemp, Warning, TEXT("Ptr：%p，Name：%s，解析物品数据，ID：%d "), this, *FString(__FUNCTION__), ID);
	//#endif


	for (const auto& Data : this->ItemCount)
	{
		//数据块[压缩数据，索引都为0，不会出现第二个]
		FItemViewBlock NewBlock;
		NewBlock.ItemID = ID;
		NewBlock.ItemLevel = Data.Key;
		NewBlock.BlockIndex = 0;

		//数量限制
		if (OutCount <= 0)
		{
			OutItemBlock.Emplace(MoveTempIfPossible(NewBlock));
		}
		else {
			//当前总数量小于需求数量时
			if (OutItemBlock.Num() < OutCount)
			{
				//添加物品
				OutItemBlock.Emplace(MoveTempIfPossible(NewBlock));
			}
			else {
				return;
			}
		}
	}
}
void FBaseItemSave::ToArrayZipCheckEquip(
	UPlayerStructManager* Player,
	int32 ID,
	const TMap<int32, FEquipItemSave>& bEquipItemSave,
	TArray<FItemViewBlock>& OutbEquipItems,
	TArray<FItemViewBlock>& OutbNotEquipItems,
	int32 OutCount) const {

	//限制判断
	if (OutCount > 0 && OutbEquipItems.Num() + OutbNotEquipItems.Num() == OutCount)
	{
		return;
	}

	//#if WITH_EDITOR
		//UE_LOG(LogTemp, Warning, TEXT("Ptr：%p，Name：%s，解析物品数据，ID：%d "), this, *FString(__FUNCTION__), ID);
	//#endif

	//查询Index【正在装备的物品】
	const FEquipItemSave* bEquipItems = bEquipItemSave.Find(ID);
	//获取未装备的索引【之前一定装备过】
	const FEquipItemSave* NotbEquipItems = Player->GetEquipItem(ID, false);

	for (const auto& Data : this->ItemCount)
	{
		//当前循环中等级数量总和
		int32 CurLoopLevelCountTotal = Data.Value;

		//数据块[压缩数据，索引都为0，不会出现第二个]
		FItemViewBlock NewBlock;
		NewBlock.ItemID = ID;
		NewBlock.ItemLevel = Data.Key;
		NewBlock.BlockIndex = 0;

		//数量限制
		if (OutCount <= 0)
		{
			//存储已经装备的物品索引
			TSet<int32> bEquipItemsIndex;
			//如果装备物品集合有效
			if (bEquipItems) {
				//在装备物品中通过等级获取对应的装备索引
				bEquipItems->GetAllBlockIndex(NewBlock.ItemLevel, bEquipItemsIndex);
				//遍历装备索引
				for (const auto& LastIndex : bEquipItemsIndex)
				{
					//依次加入到装备数组中
					NewBlock.BlockIndex = LastIndex;
					OutbEquipItems.Emplace(NewBlock);
				}
			}

			//存储未装备过的物品索引
			TSet<int32>  NotbEquipItemsIndex;
			//如果未装备物品集合有效
			if (NotbEquipItems) {
				//在未装备物品中通过等级获取对应的装备索引
				NotbEquipItems->GetAllBlockIndex(NewBlock.ItemLevel, NotbEquipItemsIndex);
				//遍历未装备索引
				for (const auto& LastIndex : NotbEquipItemsIndex)
				{
					//依次加入到未装备数组中
					NewBlock.BlockIndex = LastIndex;
					OutbNotEquipItems.Emplace(NewBlock);
				}
			}

			//将两中装备类型的物品集合
			bEquipItemsIndex.Append(NotbEquipItemsIndex);

			//如果当前物品对应等级的数量总和与未装备和装备的物品数量不匹配
			if (CurLoopLevelCountTotal != bEquipItemsIndex.Num())
			{
				//表示还有剩余物品还未进行初始化[BlockIndex]
				int32 BlockIndex = 0;
				while (true)
				{
					if (bEquipItemsIndex.Contains(BlockIndex))
					{
						bEquipItemsIndex.Remove(BlockIndex);
					}
					else {
						//添加到未装备的数组中
						NewBlock.BlockIndex = BlockIndex;
						OutbNotEquipItems.Emplace(NewBlock);
						break;
					}

					++BlockIndex;
				}
			}
		}
		else {

			//当前循环初始化后的BlockIndex数量总和
			int32 CurLoopbInitBlockIndexCountTotal = OutbEquipItems.Num() + OutbNotEquipItems.Num();

			//当前总数量小于需求数量时
			if (CurLoopbInitBlockIndexCountTotal < OutCount)
			{
				//初始化已经装备的索引，如果已经装备，则展开
				TSet<int32> bEquipItemsIndex;
				if (bEquipItems) {
					//获取索引
					bEquipItems->GetAllBlockIndex(NewBlock.ItemLevel, bEquipItemsIndex);
					//当前已有的总数+当前装备的数量小于等于需要返回的数量
					if (CurLoopbInitBlockIndexCountTotal + bEquipItemsIndex.Num() <= OutCount)
					{
						for (const auto& LastIndex : bEquipItemsIndex)
						{
							NewBlock.BlockIndex = LastIndex;
							OutbEquipItems.Emplace(NewBlock);
						}

						//更新当前总量
						CurLoopbInitBlockIndexCountTotal += bEquipItemsIndex.Num();
					}
					else {
						//新增BlockIndex数量
						int32 NewBlockIndexCountTotal = bEquipItemsIndex.Num();

						//计算出新增的最大数量，新增数量-超出数量 = 需要添加的数量。
						int32 NeedAddedCount =
							NewBlockIndexCountTotal -
							(CurLoopbInitBlockIndexCountTotal + NewBlockIndexCountTotal - OutCount);

						int32 LastIndexCount = 1;
						for (const auto& LastIndex : bEquipItemsIndex)
						{
							NewBlock.BlockIndex = LastIndex;
							OutbEquipItems.Emplace(NewBlock);
							if (LastIndexCount == NeedAddedCount)
							{
								//数量满足，无需再进行加载
								return;
							}
							++LastIndexCount;
						}
					}
				}

				TSet<int32>  NotbEquipItemsIndex;
				if (NotbEquipItems) {
					//获取索引
					NotbEquipItems->GetAllBlockIndex(NewBlock.ItemLevel, NotbEquipItemsIndex);
					//当前已有的总数+当前未装备的数量小于等于需要返回的数量
					if (CurLoopbInitBlockIndexCountTotal + NotbEquipItemsIndex.Num() <= OutCount)
					{
						for (const auto& LastIndex : NotbEquipItemsIndex)
						{
							NewBlock.BlockIndex = LastIndex;
							OutbNotEquipItems.Emplace(NewBlock);
						}

						//更新总量
						CurLoopbInitBlockIndexCountTotal += NotbEquipItemsIndex.Num();
					}
					else {
						//新增BlockIndex数量
						int32 NewBlockIndexCountTotal = bEquipItemsIndex.Num();
						//计算出新增的最大数量，新增数量-超出数量 = 需要添加的数量。
						int32 NeedAddedCount = NewBlockIndexCountTotal -
							(CurLoopbInitBlockIndexCountTotal + NewBlockIndexCountTotal - OutCount);

						int32 LastIndexCount = 1;
						for (const auto& LastIndex : NotbEquipItemsIndex)
						{
							NewBlock.BlockIndex = LastIndex;
							OutbNotEquipItems.Emplace(NewBlock);

							if (LastIndexCount == NeedAddedCount)
							{
								//数量满足，无需再进行加载
								return;
							}

							++LastIndexCount;
						}
					}

				}

				//将两中装备类型的物品集合
				bEquipItemsIndex.Append(NotbEquipItemsIndex);
				//如果当前物品对应等级的数量总和与未装备和装备的物品数量不匹配
				if (CurLoopLevelCountTotal != bEquipItemsIndex.Num())
				{
					//如果数量依旧小于返回数量，则继续添加一次
					if (bEquipItemsIndex.Num() + 1 < OutCount)
					{
						//如果还未装备，则压缩
						int32 BlockIndex = 0;
						while (true)
						{
							if (bEquipItemsIndex.Contains(BlockIndex))
							{
								bEquipItemsIndex.Remove(BlockIndex);
							}
							else {
								NewBlock.BlockIndex = BlockIndex;
								OutbNotEquipItems.Emplace(NewBlock);
								break;
							}

							++BlockIndex;
						}
					}
				}
			}
			else {
				return;
			}
		}
	}
}
int32 FEquipItemSave::GetAllCount() const {
	return this->Count;
}

int32 FEquipItemSave::GetItemCount(int32 Level) const{
	const FEquipItemBlockIndex* Index = this->BlockLevel.Find(Level);
	if (Index)
	{
		return Index->BlockIndex.Num();
	}
	return 0;
}

void FEquipItemSave::GetAllBlockIndex(int32 Level, TSet<int32>& OutBlockIndex)const {
	const FEquipItemBlockIndex* ItemBlockIndex = this->BlockLevel.Find(Level);
	if (ItemBlockIndex)
	{
		OutBlockIndex = ItemBlockIndex->BlockIndex;
	}
}

bool FEquipItemSave::Find(int32 Level, int32 BlockIndex) const {
	const FEquipItemBlockIndex* ItemBlockIndex = this->BlockLevel.Find(Level);
	if (ItemBlockIndex)
	{
		return ItemBlockIndex->BlockIndex.FindId(BlockIndex).IsValidId();
	}

	return false;
}

void FEquipItemSave::Add(int32 Level, int32 BlockIndex) {
	FEquipItemBlockIndex* ItemBlockIndex = this->BlockLevel.Find(Level);
	if (ItemBlockIndex)
	{
		ItemBlockIndex->BlockIndex.Emplace(BlockIndex);
	}
	else {
		FEquipItemBlockIndex NewIndex;
		NewIndex.BlockIndex.Emplace(BlockIndex);
		this->BlockLevel.Emplace(Level, NewIndex);
	}

	this->Count++;
}

bool FEquipItemSave::Remove(int32 Level, int32 BlockIndex) {

	FEquipItemBlockIndex* ItemBlockIndex = this->BlockLevel.Find(Level);

	if (ItemBlockIndex)
	{
		if (ItemBlockIndex->BlockIndex.FindId(BlockIndex).IsValidId())
		{
			ItemBlockIndex->BlockIndex.Remove(BlockIndex);
			this->Count--;
			return true;
		}
	}
	return false;
}

void FItemLevel::AddSteps(int32 CurMaxLV, float RateValue) {

	if (CurMaxLV > this->MaxLV)
	{
		CurMaxLV = this->MaxLV;
	}

	if (this->Steps.Num())
	{
		this->Steps.Emplace(CurMaxLV, RateValue);
	}
	else {
		this->NextAddTotalEx = RateValue;
		this->CurStepLV = CurMaxLV;
		this->Steps.Emplace(CurMaxLV, RateValue);
	}
}

bool FItemLevel::AddEx(int32 NewEx) {
	int32 CurEx = this->CurrentEx;
	int32 CurExRe = CurEx + NewEx;
	if (CurExRe >= this->CurrentTotalEx)
	{
		//设置剩余经验值
		this->CurrentEx = CurExRe - this->CurrentTotalEx;// 100 - 50
		//等级提升
		this->LV++;
		//最大等级
		if (this->LV >= this->MaxLV)
		{
			this->LV = this->MaxLV;
			return true;
		}
		//设置阶段
		if (this->CurStepLV != 0)
		{
			//进入下一个阶段
			if (this->LV >= this->CurStepLV)
			{
				TArray<int32> Lvs;
				this->Steps.GenerateKeyArray(Lvs);
				int32 Index = Lvs.Find(this->CurStepLV);
				if (Index != INDEX_NONE)
				{
					if (Index == Lvs.Num() - 1)
					{
						this->CurStepLV = 0;
					}
					else {
						this->CurStepLV = Lvs[Index + 1];
						float* Value = this->Steps.Find(this->CurStepLV);
						if (Value)
						{
							this->NextAddTotalEx = *Value;
						}
					}
				}
				else {
					this->CurStepLV = 0;
				}
			}
		}
		//提升总经验值
		this->CurrentTotalEx *= this->NextAddTotalEx;
		//递归经验增加
		this->AddEx(this->CurrentEx);
	}
	else {
		this->CurrentEx = CurExRe;
	}

	return false;
}

bool FItemLevel::IsMax() {
	return (this->LV == this->MaxLV);
}

float FItemLevel::GetExValue() {
	return this->CurrentEx / this->CurrentTotalEx;
}

void UItemBaseView::Arrangement(
	const TArray<uint8>& Type,
	TMap<int32, FBaseItemSave>& NewItems,
	bool bAddOtherData
) {
	NewItems = this->GetItemTable();
}

bool UItemBaseView::InitID(int32 ID, FItemBase& OutData) {
	//查询
	uint8 Type = 0;
	FName Name = this->GetTag();
	FItemBaseStructData* Data = nullptr;
	if (GET_ITEM(ID, Type, Data, Name))
	{
		if (Data->GetSourceData(OutData))
		{
			//新增到  纹理
			this->Cache->AddItemTextureCache(ID, OutData.ItemTexturePath);

			//新增到 名称
			this->ItemName.Emplace(ID, OutData.ItemName);

			return true;
		}
	}
	return false;
}

FItemViewIndexRange UItemBaseView::GetItemIndexRange(int32 ID, int32 Level,
	const TMap<int32, FBaseItemSave>& NewLoadItems, int32 JumpPosition) {

	/*
		 默认实现
		 1.将对应ID位置查询
		 2.累计数量作为索引的偏移

		例如：  itemID：20，itemCount：40，最终范围：[20,60)

	*/

	int32 IDBegin = JumpPosition;
	int32 BeginIndex = 0;
	const TMap<int32, FBaseItemSave>& Temp = NewLoadItems.Num() ? NewLoadItems : this->GetItemTable();
	for (const auto& Data : Temp)
	{
		//当ID不匹配时，直接将个数增加作为下一个物品的起始索引
		if (Data.Key != ID)
		{
			if (IDBegin != -1)
			{
				continue;
			}

			for (const auto& LocalLevel : Data.Value.ItemCount)
			{
				BeginIndex += LocalLevel.Value;
			}
		}
		else
		{
			//只有匹配到ID之后，就一定返回数据

			if (IDBegin != -1)
			{
				BeginIndex = IDBegin;
			}
			else {
				IDBegin = BeginIndex;
			}

			for (const auto& LocalLevel : Data.Value.ItemCount)
			{
				if (LocalLevel.Key == Level)
				{
					if (LocalLevel.Value > 0)
					{
						return FItemViewIndexRange(IDBegin, BeginIndex,
							BeginIndex + LocalLevel.Value - 1);
					}
				}
				else {
					BeginIndex += LocalLevel.Value;
				}
			}

			return FItemViewIndexRange(-1);
		}
	}

	return FItemViewIndexRange(-1);
}

UTexture2D* UItemBaseView::GetItemTexture(int32 ID) {

	if (!IsValid(this->Cache))
	{
		this->Cache = GET_CACHE(Texture);
	}

	UTexture2D* TextureIns = this->Cache->GetItemTexture(ID);
	if (TextureIns)
	{
		return TextureIns;
	}

	//初始化
	FItemBase Base;
	if (this->InitID(ID, Base))
	{
		return this->Cache->GetItemTexture(ID);
	}

	return nullptr;
}

FString UItemBaseView::GetItemName(int32 ID) {

	FString* NameIns = this->ItemName.Find(ID);
	if (NameIns)
	{
		return *NameIns;
	}

	if (!IsValid(this->Cache))
	{
		this->Cache = GET_CACHE(Texture);
	}

	//初始化
	FItemBase Base;
	if (this->InitID(ID, Base))
	{
		return Base.ItemName;
	}

	return FString();
}

FString UItemBaseView::GetItemDescribe(int32 ID)
{
	uint8 Type = 0;
	FName Name = this->GetTag();
	FItemBaseStructData* Data = nullptr;
	if (GET_ITEM(ID, Type, Data, Name))
	{
		FItemBase BaseData;
		if (Data->GetSourceData(BaseData))
		{
			return BaseData.ItemDescrible;
		}
	}

	return FString();
}

TSet<int32> UItemBaseView::GetLoaderIndexFromItemViewBlock(
	UItemLoaderObject* ItemLoader,
	const TArray<FItemViewBlock>& Blocks,
	const TMap<int32, FBaseItemSave>& NewLoadItems
) {

	//函数内部结构->索引结构
	struct LocalItemViewIndexRange {
		struct LocalItemViewCount {
			FItemViewIndexRange Range;
			int32 IndexCount = 0;
		};
		TMap<int32, LocalItemViewCount> LevelRange;
		int32 JumpID = -1;
	};

	//返回展示的结果
	TSet<int32> Result;
	TMap<int32, LocalItemViewIndexRange> ItemBuffers;
	LocalItemViewIndexRange* CurLocalItemViewIndexRange = nullptr;
	LocalItemViewIndexRange::LocalItemViewCount* DataItemViewCount = nullptr;
	//用于判断是否需要加载新的数据
	int32 CurID = -1;
	int32 CurLevel = -1;
	//显示数量
	int32 ShowCount = ItemLoader->GetShowCount();
	//格子索引偏移位置
	int32 OffsetPisition = 0;
	bool bOffsetGird = false;

	//收集数据[将展开的数据进行压缩：例如：100个 -> 20个]
	for (auto Data = Blocks.CreateConstIterator(); Data; ++Data)
	{
		if (Data->ItemID == -1)
		{
			continue;
		}

		if (CurLocalItemViewIndexRange)
		{
			//判断与上一次的选择是否一样
			if (Data->ItemID == CurID && Data->ItemLevel == CurLevel)
			{
				DataItemViewCount->IndexCount += 1;
				continue;
			}
		}

		//重新记录当前元素的信息
		CurID = Data->ItemID;
		CurLevel = Data->ItemLevel;

		//从缓存提取
		CurLocalItemViewIndexRange = ItemBuffers.Find(Data->ItemID);
		if (CurLocalItemViewIndexRange)
		{
			//如果之前就有数据，则直接更新数量
			DataItemViewCount = CurLocalItemViewIndexRange->LevelRange.Find(Data->ItemLevel);
			if (DataItemViewCount)
			{
				DataItemViewCount->IndexCount += 1;
				continue;
			}
		}

		//如果什么数据都没有，则需要完全的新加载数据
		CurLocalItemViewIndexRange = ItemBuffers.Find(Data->ItemID);
		if (CurLocalItemViewIndexRange)
		{
			LocalItemViewIndexRange::LocalItemViewCount NewItem;
			NewItem.IndexCount = 1;
			NewItem.Range = this->GetItemIndexRange(
				Data->ItemID,
				Data->ItemLevel,
				NewLoadItems,
				CurLocalItemViewIndexRange->JumpID);
			if (CurLocalItemViewIndexRange->JumpID == -1)
			{
				CurLocalItemViewIndexRange->JumpID = NewItem.Range.JumpPosition;
			}

			DataItemViewCount = &CurLocalItemViewIndexRange->LevelRange.Emplace(Data->ItemLevel, NewItem);
		}
		else {
			//创建项目数据
			LocalItemViewIndexRange::LocalItemViewCount NewItem;
			NewItem.IndexCount = 1;
			NewItem.Range = this->GetItemIndexRange(
				Data->ItemID,
				Data->ItemLevel,
				NewLoadItems,
				-1);
			//创建数据块
			LocalItemViewIndexRange NewData;
			NewData.JumpID = NewItem.Range.JumpPosition;


			DataItemViewCount = &NewData.LevelRange.Emplace(
				Data->ItemLevel, MoveTempIfPossible(NewItem)
			);
			CurLocalItemViewIndexRange = &ItemBuffers.Emplace(Data->ItemID, MoveTempIfPossible(NewData));
		}
	}

	//开始选择UI索引
	for (auto Data = ItemBuffers.CreateConstIterator(); Data; ++Data)
	{
		for (const auto& Item : Data->Value.LevelRange)
		{
			/*
				测试，是否是必须需要这个偏移和BlockIndex对比
			*/

			/*
			UE_LOG(LogTemp, Error, TEXT("JumpID[%d]，Level[%d]，Range[%d,%d]，Count[%d]"),
				Data->Value.JumpID, Item.Key,
				Item.Value.Range.Begin, Item.Value.Range.End, Item.Value.IndexCount
			);
			*/

			//选择数据
			int32 RangeBegin = (Item.Value.Range.Begin <= ItemLoader->GetPagetFirstIndex())
				? ItemLoader->GetPagetFirstIndex() : Item.Value.Range.Begin;

			int32 RangeEnd = RangeBegin + Item.Value.IndexCount;

			//如果起始位置
			if (RangeEnd > Item.Value.Range.End)
			{
				RangeEnd = Item.Value.Range.End;
			}

			//将end范围缩小到能见区域
			if (
				RangeEnd >= ItemLoader->GetPagetLastIndex() ||
				RangeEnd < ItemLoader->GetPagetLastIndex()
				)
			{
				if (RangeEnd >= ItemLoader->GetPagetLastIndex())
				{
					//将结束范围缩小到最大显示范围
					RangeEnd = ItemLoader->GetPagetLastIndex() - 1;
				}
			}

			//结束判断
			if (RangeBegin > RangeEnd)
			{
				//此元素不再显示，跳过此元素
				continue;
			}

			//设置对应范围的格子为选择状态
			int32 BeginIndex = ItemLoader->GetGridIndexByUiIndex(RangeBegin);
			if (BeginIndex != -1)
			{
				if (
					!bOffsetGird &&
					ItemLoader->GetCurrentPage() != 0 &&
					BeginIndex < ItemLoader->GetCol()
					)
				{
					bOffsetGird = true;

					//如果最后的位置 - 当前可显示的结束位置
					//  102 - 102 - 6 = -6
					//  102 - 100 - 6 = -4
					//  102 - 2   - 6 = 94
					//  102 - 96  - 6 = 0
					OffsetPisition = Item.Value.Range.End - RangeEnd - ItemLoader->GetCol();
					if (OffsetPisition <= 0)
					{
						// 6 + -6 = 0//偏移位置 0
						// 6 + -4 = 2//偏移位置 2
						// 6 + 0  = 6//偏移位置 6
						OffsetPisition = ItemLoader->GetCol() + OffsetPisition;

						//OffsetPisition 会出现小于0的情况吗？必然不可能，起始索引大于结束索引则直接跳出当前循环
					}
					else {
						//如果剩余量很多，则直接偏移最大位置
						OffsetPisition = ItemLoader->GetCol();
					}
				}

				/*
				UE_LOG(LogTemp, Error,
					TEXT("CurPage:%d,PageFristIndex:%d,PageLastIndex:%d,PageShowCount:%d,OffsetPosition:%d,SelectUIIndexBegin:%d,SelectUIIndexEnd:%d,ItemArrayBegin:%d,ItemLevel:%d"),
					ItemLoader->GetCurrentPage(),
					ItemLoader->GetPagetFirstIndex(),
					ItemLoader->GetPagetLastIndex(),
					ShowCount,
					OffsetPisition,
					BeginIndex,
					BeginIndex + Item.Value.IndexCount,
					RangeBegin,
					Item.Key);*/

					//匹配索引起始位置和结束位置
				BeginIndex = BeginIndex + OffsetPisition;
				RangeEnd = BeginIndex + Item.Value.IndexCount;

				//添加范围数据
				for (; BeginIndex < RangeEnd; ++BeginIndex)
				{
					Result.Emplace(BeginIndex);
				}

				//只有首位选择的元素才会偏移，其他元素无需偏移
				OffsetPisition = 0;
			}
		}
	}

	return Result;
}

void UItemBaseView::ClearCache()
{
	//#if WITH_EDITOR
	//	UE_LOG(LogTemp, Warning, TEXT("Ptr：%p，Name：%s,清理物品物品数据缓存"), this, *FString(__FUNCTION__));
	//#endif
}

TArray<FItemViewBlock> UItemBaseView::GenerateItemView(int32 GenerateMax) {

	TArray<FItemViewBlock> Block;

	//遍历  ID
	for (const auto& Data : this->GetItemTable())
	{
		Data.Value.ToArray(Data.Key, Block, GenerateMax);
	}

	return Block;
}