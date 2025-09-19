// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/AssetManager.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StreamableManager.h"
#include "GameSystem/GameDataSubsystem.h"
#include "ItemBaseStruct.generated.h"

// this->M_UUI_PlayerInformationShow->SetPlayerSuitConst();

//---------------------------------------------------------------------------------------------------------
// 
//    射线结构开始->射线用于一些卡片-武器->来检测老鼠
//---------------------------------------------------------------------------------------------------------

//射线位置
UENUM(BlueprintType)
enum class ELineTracePosition : uint8 {
	//包括当前网格，从左边开始直到末尾
	E_Left  UMETA(DisplayName = "Left"),
	//包括当前网格，从右边开始直到末尾
	E_Right UMETA(DisplayName = "Right"),
	//检测当前左右两边
	E_LeftRight_M UMETA(DisplayName = "LeftRight_M"),
	//包括当前网格，从上边开始直到末尾
	E_Top UMETA(DisplayName = "Top"),
	//包括当前网格，从下边开始直到末尾
	E_Bottom UMETA(DisplayName = "Bottom"),
	//检测当前上下两边
	E_TopBottom_M UMETA(DisplayName = "TopBottom_M")
};

//射线检测类型
UENUM(BlueprintType)
enum class ELineTraceType : uint8 {
	//在格子走的老鼠
	E_MouseGround  UMETA(DisplayName = "MouseGround"),
	//在空中漂浮的老鼠
	E_MouseSky  UMETA(DisplayName = "MouseSky"),
	//陆地和空中
	E_MouseGroundAndSky  UMETA(DisplayName = "MouseGroundAndSky"),
	//在地下的老鼠
	E_MouseUnder  UMETA(DisplayName = "MouseUnder"),
	//任何类型
	E_MouseVisibility  UMETA(DisplayName = "MouseVisibility"),
};


//检测范围结构设置
USTRUCT(BlueprintType)
struct FLineCheckSetting {
	GENERATED_USTRUCT_BODY()
public:
	//检测类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELineTraceType M_ELineTraceType = ELineTraceType::E_MouseGround;
	//检测发射位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELineTracePosition M_ELineTracePosition = ELineTracePosition::E_Right;
	//检测长度[-1表示不限制长度]
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CheckLength = -1;
	//检测偏移[如果是左右检测(行偏移)如果是上下检测(列偏移)]
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CheckOffset = 0;
};

//射线偏移结构设置
USTRUCT(BlueprintType)
struct FLineTraceSetting {
	GENERATED_USTRUCT_BODY()
public:
	//开始位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector M_BeginLocation = FVector();
	//结束位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector M_EndLocation = FVector();
	//设置位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELineTracePosition M_ELineTracePosition = ELineTracePosition::E_Right;
	//开始固定格子长度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_bEnableSettingGridCount = false;
	//最长几个格子
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_GridCount = 1;
	//射线最终的偏移位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector M_LineTraceOffset = FVector();
	//检测类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELineTraceType M_ELineTraceType = ELineTraceType::E_MouseGround;
};

//---------------------------------------------------------------------------------------------------------
// 
//    射线结构结束
//---------------------------------------------------------------------------------------------------------


//角色版本补偿
USTRUCT(BlueprintType)
struct FPlayerVersionCompensate
{
	GENERATED_USTRUCT_BODY()
public:
	//发送状态
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_SendState = false;
	//发送的邮件名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_SendMailID = -1;
};

USTRUCT(BlueprintType)
struct FItemBase {
	GENERATED_USTRUCT_BODY()
public:
	FItemBase() {}
	FItemBase(const FString& _Name) :ItemName(_Name) {}
	FItemBase(const FString& _Name, const FString _IDs, const FString _TPath) :
		ItemName(_Name), ItemDescrible(_IDs), ItemTexturePath(_TPath) {}
public:
	//道具名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;
	//道具描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemDescrible;
	//显示的纹理路径
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath ItemTexturePath;
	//是否被认为是等待删除的对象
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWaitRemove = false;
	//物品ID号(可以不用设置)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID = -1;
};

/*物品表结构
1.用于表示物品在数据表中的结构
2.使用此结构的物品，才可以被系统管理
3.ID号是被查询到的关键【GET_ITEM(ID)】
4.重写函数，可用于返回对应的数据

继承当前结构，必须重写【GetSourceData】返回数据，确保被查询时，返回正确的数据结果

*/
USTRUCT(BlueprintType)
struct FItemBaseStructData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();
public:
	//物品ID号
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID = 0;

	virtual bool GetSourceData(FItemBase& OutData)
	{
		return false;
	}

	//设置物品ID号【仅用于编辑器脚步工具】
	virtual void SetID(int32 ID) {
		this->ItemID = ID;
	}
};

/*物品视图数据块
1.一个结构表示一块物品的数据
2.主要用于视图生成的数据块
3.ID对应物品ID，用于查询
4.Level对应物品等级，拥有等级可以用于其他数据的变化
5.BlockIndex在相同的ID和等级下，使用索引区分
*/
USTRUCT(BlueprintType)
struct FItemViewBlock
{
	GENERATED_USTRUCT_BODY();

	//物品ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID = -1;

	//物品等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemLevel = -1;

	//块索引
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BlockIndex = -1;

	void SetNull() {
		ItemID = -1;
		ItemLevel = -1;
		BlockIndex = -1;
	}

	bool IsValid() const {

		if (ItemID == -1 || ItemLevel == -1 || BlockIndex == -1)
		{
			return false;
		}

		if (ItemID != -1 && ItemLevel != -1 && BlockIndex != -1)
		{
			return true;
		}

		return false;
	}

};

/*基本物品存储结构
1.主要用于玩家背包，一个结构表示一个物品组合【将数据折叠】
2.一个组合包括，等级和数量
3.可以直接生成一组数据，相当于【展开数据】
*/
USTRUCT(BlueprintType)
struct FBaseItemSave {
	GENERATED_USTRUCT_BODY()
public:
	//物品数量【Level：Count】
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, int32> ItemCount;
public:
	//新增物品
	void AddItem(int32 Level, int32 Count);
	//删除物品
	bool RemoveItem(int32 Level, int32 Count);
	//检测
	bool IsValid();
	//获取物品数量
	int32 GetItemCount(int32 Level);
	//转换到数组
	void ToArray(int32 ID, TArray<FItemViewBlock>& OutItemBlock, int32 OutCount = 0) const;
	//转换到数组[特供版本，专用于检测背包装备栏的装备情况，及时调整BlockIndex]
	void ToArrayCheckEquip(
		class UPlayerStructManager* Player,
		int32 ID,
		const TMap<int32, FEquipItemSave>& EquipItemSave,
		TArray<FItemViewBlock>& OutItemBlockbEuip,
		TArray<FItemViewBlock>& OutItemBlockNormal,
		int32 OutCount = 0) const;
	//转换到数组，压缩，[特供版本，专用于检测背包装备栏的装备情况，及时调整BlockIndex]
	void ToArrayZip(int32 ID, TArray<FItemViewBlock>& OutItemBlock, int32 OutCount = 0) const;
	void ToArrayZipCheckEquip(
		class UPlayerStructManager* Player,
		int32 ID,
		const TMap<int32, FEquipItemSave>& bEquipItemSave,
		TArray<FItemViewBlock>& OutbEquipItems,
		TArray<FItemViewBlock>& OutbNotEquipItems,
		int32 OutCount = 0) const;

};


//--------------------------------------------------------------------------------[装备道具存储索引块]
/*
	装备索引块
	1.BlockIndex表示当前所存储所有的索引块
*/
USTRUCT(BlueprintType)
struct FEquipItemBlockIndex {
	GENERATED_USTRUCT_BODY()

	//当前全部的索引
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<int32> BlockIndex;
};

/*
	装备索引块等级
	1.BlockLevel表示对应的物品不同等级对应的索引块
*/
USTRUCT(BlueprintType)
struct FEquipItemSave {
	GENERATED_USTRUCT_BODY()

	//当前全部的索引，Key ： Level，Value：Index
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FEquipItemBlockIndex> BlockLevel;


public:
	//获取物品数量
	int32 GetItemCount(int32 Level) const;
	//获取装备的数量
	int32 GetAllCount() const;
	//获取全部的索引
	void GetAllBlockIndex(int32 Level, TSet<int32>& OutBlockIndex)const;
	//查询索引是否存在
	bool Find(int32 Level, int32 BlockIndex) const;
	//新增索引
	void Add(int32 Level, int32 BlockIndex);
	//删除索引
	bool Remove(int32 Level, int32 BlockIndex);
private:
	UPROPERTY()
	int32 Count = 0;
};
//--------------------------------------------------------------------------------[装备道具存储索引块]


//邮件物品存储
USTRUCT(BlueprintType)
struct FMailItemSave {

	GENERATED_USTRUCT_BODY()

	//邮件数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 1;

	//邮件状态[已读，未读]
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRead = false;
};

//视图数据块[通过ID返回视图需要的数据]
USTRUCT(BlueprintType)
struct FItemBaseViewBlock {

	GENERATED_USTRUCT_BODY();

	//名称
	UPROPERTY()
	FString Name;
	//类型
	UPROPERTY()
	uint8 Type = 0U;
	//图形
	UPROPERTY()
	UTexture2D* Texture = nullptr;
};

//物品视图数据块[通过ID返回视图需要的数据]，带有数量的数据块【装备】【材料】
USTRUCT(BlueprintType)
struct FItemCountViewBlock : public FItemBaseViewBlock {

	GENERATED_USTRUCT_BODY();

	//数量
	UPROPERTY()
	int32 Count = -1;

};

//物品等级结构
USTRUCT(BlueprintType)
struct FItemLevel {

	GENERATED_USTRUCT_BODY()

	FItemLevel(
		const int32& NewLV,
		const int32& NewMaxLV,
		const int32& NewTotalEx,
		const float& NewAddTotalEx)
		:LV(NewLV), MaxLV(NewMaxLV), CurrentTotalEx(NewTotalEx), NextAddTotalEx(NewAddTotalEx)
	{}

	FItemLevel() {}

	//等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LV = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxLV = 20;

	//当前经验值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentEx = 0;

	//当前需要的总经验值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentTotalEx = 200;

	//下一个等级需要的加成经验值比例
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NextAddTotalEx = 1.5f;

public:

	//设置阶段，不同阶段不同经验值成长
	void AddSteps(int32 CurMaxLV, float RateValue);

	//添加经验值
	bool AddEx(int32 NewEx);

	//获取经验值进度
	float GetExValue();

	//是否是最大等级
	bool IsMax();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurStepLV = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, float> Steps;
};

/*物品视图索引范围*/
USTRUCT(BlueprintType)
struct FItemViewIndexRange {

	GENERATED_USTRUCT_BODY()

	FItemViewIndexRange() {}
	FItemViewIndexRange(int32 Init) :JumpPosition(Init), Begin(Init), End(Init) {}
	FItemViewIndexRange(int32 A, int32 B, int32 C) :JumpPosition(A), Begin(B), End(C) {}

	bool IsValid() { if (Begin < 0 || End < 0) { return false; }  return true; }

	//当前物品的ID位置索引
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 JumpPosition = -1;

	//范围开始
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Begin = -1;

	//范围结束
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 End = -1;

};


/*物品基础视图
1.可用于背包的数据加载，用于数据显示
2.例子类：PlayerCardView类型。专门显示角色背包中的卡片

使用：
1. 使用前需要进行一次 UpdateView 更新，作为解析缓存
2. GenerateItemView 会根据视图，进行解析，生成数组。解析时需要时间，和数组的拷贝【有一定的性能消耗】
3. ClearCache 使用一次清理，则会让 GenerateItemView 重新解析
*/
UCLASS()
class FVM_API UItemBaseView : public UGameDataStaticObject
{
	GENERATED_BODY()
public:

	//获取源数据
	FORCEINLINE virtual const TMap<int32, FBaseItemSave>& GetItemTable() const {
		static TMap<int32, FBaseItemSave> Temp; return Temp;
	}
	//获取类型的Tag
	FORCEINLINE virtual FName GetTag() { return FName(); }
	//获取块数据【必须实现】
	virtual FItemBaseViewBlock* GetItem(int32 ID, int32 Level) { return nullptr; };

	//获取数据的索引范围[IndexA , IndexB]  

	/*
		获取数据的索引范围 [IndexA , IndexB]
		[JumpPosition表示移动索引到ID的当前位置开始计算]，-1表示从头开始
		请确保使用JumpPosition的值是正确的，否则不应该填写
	*/
	virtual FItemViewIndexRange GetItemIndexRange(
		int32 ID, int32 Level, const TMap<int32, FBaseItemSave>& NewLoadItems
		, int32 JumpPosition = -1);

	//获取物品介绍
	virtual FString GetItemDescribe(int32 ID);

	//获取物品名称
	FString GetItemName(int32 ID);
	//获取物品纹理
	class UTexture2D* GetItemTexture(int32 ID);
	/*
		从物品索引视图获取加载器UI的索引
		1.需要从第几页进行选择
		2.需要选择的物品数量
	*/
	TSet<int32> GetLoaderIndexFromItemViewBlock(
		class UItemLoaderObject* ItemLoader,
		const TArray<FItemViewBlock>& Blocks,
		const TMap<int32, FBaseItemSave>& NewLoadItems
	);

	/*
	 整理物品
	 1.类型的顺序
	 2.返回的存储结果
	 3.是否返回除筛选类型以外的数据，例如：筛选 1 3 4 以内。但是出现2，末尾也包含2的数据。
	*/
	virtual void Arrangement(
		const TArray<uint8>& Type, //筛选类型
		TMap<int32, FBaseItemSave>& NewItems,//筛选结果
		bool bAddOtherData = false//添加不符合条件的数据到末尾
	);
	//清理缓存
	virtual	void ClearCache();
	//生成列表视图
	virtual	TArray<FItemViewBlock> GenerateItemView(int32 GenerateMax);

protected:
	//初始化物品ID
	virtual	bool InitID(int32 ID, FItemBase& OutData);
	//整理模板
	template<typename ViewBlockType>
	void Arrangement_Temp(
		TMap<int32, ViewBlockType>& BlockDatas,
		const TArray<uint8>& Type,
		TMap<int32, FBaseItemSave>& NewItems,
		bool bAddOtherData
	) {
		TMap<uint8, TMap<int32, FBaseItemSave>> Maps;
		TMap<uint8, TMap<int32, FBaseItemSave>> OtherMaps;
		//确定顺序
		for (const uint8& ArrType : Type)
		{
			TMap<int32, FBaseItemSave> MapData;
			Maps.Emplace(ArrType, MapData);
		}
		//遍历  ID
		for (const auto& Data : this->GetItemTable())
		{
			//查找
			ViewBlockType* Item = BlockDatas.Find(Data.Key);

			if (!Item)
			{
				Item = static_cast<ViewBlockType*>(this->GetItem(Data.Key, 0));
			}

			if (Item)
			{
				//添加符合条件的数据
				if (Type.Contains(Item->Type))
				{
					TMap<int32, FBaseItemSave>* CurMaps = Maps.Find(Item->Type);
					if (CurMaps)
					{
						CurMaps->Emplace(Data.Key, Data.Value);
					}
				}
				else {

					if (!bAddOtherData)
					{
						continue;
					}

					//添加不符合条件的数据
					TMap<int32, FBaseItemSave>* CurMaps = OtherMaps.Find(Item->Type);
					if (CurMaps)
					{
						CurMaps->Emplace(Data.Key, Data.Value);
					}
					else {
						TMap<int32, FBaseItemSave> MapData;
						MapData.Emplace(Data.Key, Data.Value);
						OtherMaps.Emplace(Item->Type, MapData);
					}
				}
			}
			else {
				continue;
			}
		}
		TArray<TMap<int32, FBaseItemSave>> MapsArray;
		Maps.GenerateValueArray(MapsArray);
		for (const auto& NewMap : MapsArray)
		{
			NewItems.Append(NewMap);
		}
	}
protected:
	//物品名称
	UPROPERTY()
	TMap<int32, FString> ItemName;
	//纹理缓存
	UPROPERTY()
	class UTextureDataAssetCache* Cache = nullptr;
};