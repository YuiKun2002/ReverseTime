// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/GameDataName.h"
#include "GameSystem/Item/ItemStruct.h"
#include "PlayerStructBase.generated.h"

//地图（结构关联（GameSystemFunction：LoadMap；SetMap））
UENUM(BlueprintType)
enum class EGameVsMap : uint8 {
	//美味镇
	VM_Main UMETA(DisplayName = "Main"),
	//美味岛
	VM_Map_1 UMETA(DisplayName = "Map_1"),
	//火山岛
	VM_Map_2 UMETA(DisplayName = "Map_2"),
	//火山遗迹
	VM_Map_3 UMETA(DisplayName = "Map_3"),
	//浮空岛
	VM_Map_4 UMETA(DisplayName = "Map_4"),
	//海底漩涡
	VM_Map_5 UMETA(DisplayName = "Map_5"),
};

//角色进入的地图
USTRUCT(BlueprintType)
struct FPlayerInMap {
	GENERATED_USTRUCT_BODY()
public:
	static const FString GetMapName(EGameVsMap Map)
	{
		switch (Map)
		{
		case EGameVsMap::VM_Main:
			return TEXT("MainMap");
		case EGameVsMap::VM_Map_1:
			return TEXT("Map1");
		case EGameVsMap::VM_Map_2:
			return TEXT("Map2");
		case EGameVsMap::VM_Map_3:
			return TEXT("Map3");
		case EGameVsMap::VM_Map_4:
			return TEXT("Map4");
		case EGameVsMap::VM_Map_5:
			return TEXT("Map5");
		}

		return TEXT("MainMap");
	}
public:
	//当前玩家进入的地图
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameVsMap CurrentInMap = EGameVsMap::VM_Main;

	//当前玩家上次进入的地图
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameVsMap LastInMap = EGameVsMap::VM_Main;

	//当前正在进入的地图UI名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CurrentInMapUIName = TEXT("");
};

USTRUCT(BlueprintType)
struct FPlayerEx {
	GENERATED_USTRUCT_BODY()
public:
	//当前经验值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_CurrentEx = 0.f;
	//当前经验值顶峰
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_CurrentExTop = 90.f;
	//经验值成长
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_ExUp = 10.f;
	//经验值成长率
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_ExUpRate = 0.2f;
	//最大经验值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_ExMax = 10000000.f;
	//角色等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_PlayerGrade = 1;
};

USTRUCT(BlueprintType)
struct FPlayerVIP
{
	GENERATED_USTRUCT_BODY()
public:
	//当前VIP值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_VIP = 0;
	//最大VIP
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 M_MaxVIP = 12;
	//当前进入VIP经验值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_CurrentEx = 0.f;
	//当前VIP最大经验值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_CurrentExTop = 100.f;
	//VIP成长值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_VIP_Up = 2.f;
	//VIP成长率
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_VIP_UpRate = 0.4f;
};

USTRUCT(BlueprintType)
struct FPlayerSign
{
	GENERATED_USTRUCT_BODY()
public:
	//角色等级称号
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString M_PlayerFlag;
	//角色加入的工会
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString M_TradeUnionName = TEXT("未加入工会");
	//成就名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString M_AchievementName = TEXT("");
};

USTRUCT(BlueprintType)
struct FDialogueShowInfor {
	GENERATED_BODY()
public:
	//剧情标题
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DialogueTitle;
	//剧情目标位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DialogueTarget;
};

USTRUCT(BlueprintType)
struct FCoin {

	GENERATED_USTRUCT_BODY()

	//货币值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CoinValue = 0;

	//货币名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CoinName;

};

//角色货币(存储信息)
USTRUCT(BlueprintType)
struct FPlayerCoin {

	GENERATED_USTRUCT_BODY()

	//货币
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FCoin> Coins;



	FORCEINLINE	FCoin* GetCoinRef(uint8 _Index)
	{
		return this->Coins.Find(_Index);
	}

	void InitCoin();

private:
	void MakeCoin(int32 Key, FCoin Data);
};

//货币计算
USTRUCT(BlueprintType)
struct FPlayerCoinAdd
{
	GENERATED_USTRUCT_BODY()

	static bool _Buy(FCoin* _Coin, const int32& _Num);
	static bool _Sell(FCoin* _Coin, const int32& _Num);
	static bool Buy(UPlayerStructManager* _Player, const int32& _Num, const uint8& _CoinGrade);
	static bool Sell(UPlayerStructManager* _Player, const int32& _Num, const uint8& _CoinGrade);
};


//角色背包的格子
USTRUCT(BlueprintType)
struct FPlayerBagGird
{
	GENERATED_USTRUCT_BODY()
public:
	//背景图片
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString M_GridImagePath;
	//容量
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_GridCount;
	//第几个界面
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_PanelNumber;
	//第几个背包
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_BagNumber;
	//Vector名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString M_VectorName;
	//道具名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString M_BagName;
	//索引
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemViewBlock BlockIndex;
};


//角色背包的格子组合(武器 防御卡 材料)
USTRUCT(BlueprintType)
struct FPlayerBagGirdGroup
{
	GENERATED_USTRUCT_BODY()

	//默认容量150
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 M_DefGridCount = 150;
	//装备背包格子
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPlayerBagGird> M_EquipmentGrid;
	//卡片背包格子
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPlayerBagGird> M_CardGrid;
	//材料背包格子
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPlayerBagGird> M_MaterialGrid;


	//获取背包容量（0表示获取装备背包 1表示卡片背包 2表示材料背包）
	int32 GetBagCount(uint8 _Index);
	//获取角色背包
	TArray<FPlayerBagGird*> GetUseBagArrays();
	static void ClearData(FPlayerBagGird& _Grid);

private:
	int32 GetBagCount(TArray<FPlayerBagGird>& _Arrays);
};



//角色拓展内容
USTRUCT(BlueprintType)
struct FPlayerExtraContent
{
	GENERATED_USTRUCT_BODY()
};


//角色其他数据
USTRUCT(BlueprintType)
struct FPlayerOtherData
{
	GENERATED_USTRUCT_BODY()

	FPlayerOtherData() {}
	FPlayerOtherData(
		int32 SelectCardCount,
		FString SelectCardConfigPageName
	) :
		PlayerSelectCardCount(SelectCardCount),
		PlayerSelectCardConfigPageName(SelectCardConfigPageName)
	{

	}

	//角色选择卡片的次数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerSelectCardCount = 5;
	//卡片选择配置名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerSelectCardConfigPageName;
	//角色进入地图的名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerSelectGameMapName;
	//角色准备环节选择的卡片数据
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemViewBlock> GamePrepareSelectCardData;
};


/*
	角色装备武器数据结构【存储在角色存档】
*/
USTRUCT(BlueprintType)
struct FPlayerWeapon
{
	GENERATED_USTRUCT_BODY()

	//武器头像路径
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString WeaponHeadPath = FString();

	//武器资源路径
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString WeaponResource_C_Path = FString();

	//武器数据视图
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FItemViewBlock WeaponBlock;

};

//角色武器【存储在角色存档】
USTRUCT(BlueprintType)
struct FPlayerWeapons
{
	GENERATED_USTRUCT_BODY()

	//主武器
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerWeapon FirstWeapon;

	//副武器
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerWeapon SecondWeapon;

	//超级武器
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerWeapon SuperWeapon;
};

//服装道具
USTRUCT(BlueprintType)
struct FPlayerSuitItem
{
	GENERATED_USTRUCT_BODY()
public:
	//数据视图
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FItemViewBlock Block;
	//头像路径【用于UI显示】
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString HeadPath;
	//前面2D模型显示【用于UI显示】
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString SuitFrontTextureShowPath;
	//后面2D模型显示【用于UI显示】
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString SuitBackTextureShowPath;
	//服装体力加成
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float M_Suit_HP_Value = 10.f;
	//服装魅力加成
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 M_Suit_Charm_Value = 10;
public:
	void CopyValue(const FPlayerEquipment& _Value, const FItemViewBlock& ItemViewBlock, FItemViewBlock& LastBlock)
	{
		LastBlock = this->Block;
		this->M_Suit_Charm_Value = _Value.M_PlayerEq_Charm_Value;
		this->M_Suit_HP_Value = _Value.M_PlayerEq_HP_Value;
		this->HeadPath = _Value.ItemTexturePath.ToString();
		this->SuitFrontTextureShowPath = _Value.M_PlayerEq_Path_First.ToString();
		this->SuitBackTextureShowPath = _Value.M_PlayerEq_Path_Second.ToString();
		this->Block = ItemViewBlock;
	}
};

//角色服装
USTRUCT(BlueprintType)
struct FPlayerSuit
{
	GENERATED_USTRUCT_BODY()
public:
	//是否装备了套装（没有装备套装显示基础服装，装备套装隐藏基础服装的显示）
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool M_bPlayerSuit = false;
	//隐藏翅膀功能
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool M_HiddenFlyItem = false;
	//使用选择隐藏角色套装显示
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool M_ShowPlayerSuit = true;
public:
	//套装
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerSuitItem M_Suit;
public:
	//身体
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerSuitItem M_Body;
	//头
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerSuitItem M_Head;
	//头发
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerSuitItem M_Hair;
	//脸饰
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerSuitItem M_Face;
	//翅膀
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerSuitItem M_FlyItem;
	//眼镜
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerSuitItem M_Glasses;
	//眼睛
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerSuitItem M_Eye;
	//帽子
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerSuitItem M_Cap;
public:
	//角色武器
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPlayerWeapons M_PlayerWeapons;
};

//游戏角色结构
USTRUCT(BlueprintType)
struct FGamePlayerData : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	//角色名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName;
	//角色服装数据
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerEquipment Suit;
	//角色主武器名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerFirstWeaponName;
};

//关卡结算信息
USTRUCT(BlueprintType)
struct FPlayerLevelGrade
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Time = 9999;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Score = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ScoreLevel = TEXT("F");
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 BadCardCount = 9999;
public:
	//初始化
	void Init(int32 _Time, int32 _Score, int32 _BadCardCount)
	{
		if (this->Time > _Time)
		{
			this->Time = _Time;
		}

		if (this->Score < _Score)
		{
			this->Score = _Score;
		}

		if (this->BadCardCount > _BadCardCount)
		{
			this->BadCardCount = _BadCardCount;
		}
	}

	//初始化分数等级
	FString InitScoreLevel(int32 _BadCardCount)
	{
		FString Level = TEXT("");

		if (_BadCardCount >= 0 && _BadCardCount < 10)
		{
			Level = TEXT("SSS");
		}
		else if (_BadCardCount >= 10 && _BadCardCount < 15)
		{
			Level = TEXT("SS");
		}
		else if (_BadCardCount >= 15 && _BadCardCount < 20)
		{
			Level = TEXT("S");
		}
		else if (_BadCardCount >= 20 && _BadCardCount < 30)
		{
			Level = TEXT("A+");
		}
		else if (_BadCardCount >= 30 && _BadCardCount < 45)
		{
			Level = TEXT("A");
		}
		else if (_BadCardCount >= 45 && _BadCardCount < 50)
		{
			Level = TEXT("B+");
		}
		else if (_BadCardCount >= 50 && _BadCardCount < 65)
		{
			Level = TEXT("B");
		}
		else if (_BadCardCount >= 60 && _BadCardCount < 70)
		{
			Level = TEXT("C");
		}
		else {
			Level = TEXT("E");
		}

		//之前的卡片损坏小于现在的，数据不做修改
		if (this->BadCardCount < _BadCardCount)
		{
			return Level;
		}
		else {
			this->BadCardCount = _BadCardCount;
			this->ScoreLevel = Level;

			return Level;
		}

		return Level;
	}
};

//兑换商城的存储信息【富可敌国】
USTRUCT(BlueprintType)
struct FPlayerExchangeAndSplit
{
	GENERATED_USTRUCT_BODY()

public:
	//星星个数
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 StartCount = 1;
	//加成比例
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AddedRate = 0.f;
	//文字提示
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString TextLogResource;
	//显示版面位置
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D PanelTranlation;
};


/**
*角色更新接口
*/
UINTERFACE(MinimalAPI, Blueprintable)
class UPlayerUpdateClassInterface : public UInterface
{
	GENERATED_BODY()
};

class FVM_API IPlayerUpdateClassInterface
{
	GENERATED_BODY()
public:
	//更新接口
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Update(class UPlayerStructManager* PlayerDataIns);
	virtual void Update_Implementation(class UPlayerStructManager* PlayerDataIns);
};

//角色更新类数据表
USTRUCT(BlueprintType)
struct FPlayerUpdateClassTableRowBase : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	//更新的类条目
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, TSoftClassPtr<class UPlayerUpdateClass>> UpdateClass;
};