#pragma once

// #include "GameSystem/PlayerStructManager.h"

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/Dialogue/Dialogue.h"
#include "Data/MaterialDataStruct.h"
#include "Data/EquipmentDataSturct.h"
#include "Data/Mail/MailDataAssetCache.h"
#include "GameSystem/Item/PlayerRecord.h"
#include "Data/CardData/CardDataStruct.h"
#include "GameSystem/Item/PlayerStructBase.h"
#include "GameSystem/Item/Task/TaskStructBase.h"
#include "PlayerStructManager.generated.h"


class UPlayerStructManager* __GetPlayerStructManager_t();
//获取游戏角色数据实例
#define GET_PLAYERDATAINS() __GetPlayerStructManager_t()

//角色更新类
UCLASS(BlueprintType, Blueprintable)
class FVM_API UPlayerUpdateClass : public UObject, public IPlayerUpdateClassInterface
{
	GENERATED_BODY()
public:
	//更新接口
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Update(class UPlayerStructManager* PlayerDataIns);
	virtual void Update_Implementation(class UPlayerStructManager* PlayerDataIns) override;
};

// 	//角色进入地图的名称
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameConfig")
	//FString M_PlayerInGameMapName;

//角色货币更新代理
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerCoinUpdateDelegate);
/**
 *  角色存储结构
 */
UCLASS()
class FVM_API UPlayerStructManager : public USaveGame
{
	GENERATED_BODY()

public:
	//当角色货币发生变化时触发
	UPROPERTY(BlueprintAssignable, Category = "角色数据存档事件")
	FPlayerCoinUpdateDelegate OnCoinUpdateDelegate;

	//-----------------------------------------------角色主要处理-----------------------------------------------
public:
	//角色版本
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString M_GameVersion = "";
	// 0是女生  1是男生
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 M_PlayerGender;
	//角色名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString M_PlayerName = FString(TEXT("面包干&芙蓉"));
	//角色等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerEx M_FPlayerEx;
	//角色货币
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerCoin M_FPlayerCoin;
	//角色VIP
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerVIP M_FPlayerVIP;
	//其他数据
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerOtherData OtherData;
	//角色称号和描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerSign M_FPlayerSign;
	//背包界面中的背包容量
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerBagGirdGroup M_PlayerBagGirdGroup;
	//角色着装
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerSuit M_FPlayerSuit;
	//关卡开启[0新开启，1访问过]
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, int32> M_NewMapState;
	//关卡评级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FPlayerLevelGrade> M_PlayerLevelScore;
	//道具分解兑换抽奖信息存储
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FPlayerExchangeAndSplit> M_FPlayerExchangeAndSplitInfor;
public:
	/*-----------------------地图进入记录------------------------*/
	//玩家当前进入的地图
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerInMap M_PlayerInMap;
	/*-----------------------地图进入记录------------------------*/

	/*-----------------------剧情------------------------*/
	//角色剧情完成情况
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDialogueDataInfor_Player DialogueFinished;
	/*-----------------------剧情------------------------*/


	/*-----------------------历史累积------------------------*/

	//历史累积
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerRecordStruct PlayerRecord;
	/*-----------------------历史累积------------------------*/

	/*-----------------------任务------------------------*/
	//全部任何集合【全部可执行的任务】
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAllTask AllTasks;
	//任务完成情况
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLineTaskProgress LineTaskProgress;
	//每日任务的时间记录
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DayTaskTime = "20-6-2";
	/*-----------------------任务------------------------*/


	/*-----------------------防御卡------------------------*/

	//背包 _防御卡
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemCard> M_PlayerItems_Card;

	//背包，卡片:ID，数量等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FBaseItemSave> Cards;

	//卡片选择列表配置项目
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FPlayerSelectListConfig> CardsSelectListConfig;

	/*-----------------------防御卡------------------------*/


	/*-----------------------装备------------------------*/

	//背包，装备，ID，数量等级【源数据】
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FBaseItemSave> SourceEquipItems;

	//当前已经装备的物品【存储的是已经装备的物品】
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FEquipItemSave> bEquipItems;
	//当前存储的物品索引【非装备的物品】
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FEquipItemSave> bNotEquipItems;

	/*-----------------------装备------------------------*/

	/*-----------------------材料------------------------*/
	//背包，材料，ID，数量等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FBaseItemSave> Maters;
	/*-----------------------材料------------------------*/


	/*-----------------------其他------------------------*/
	//背包邮件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMail> M_PlayerMails;
	//邮件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FMailItemSave> Mails;
	//版本补偿
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPlayerVersionCompensate> M_GameVersionCompensate;
	/*-----------------------其他------------------------*/

public:
	//获取当前玩家的等级图片路径
	UFUNCTION(BlueprintCallable)
	FString GetGradeImagePath();
	//获取最后一个新出的地图
	UFUNCTION(BlueprintCallable)
	static FString GetLastNewLevelName();
public:
	//更新【进入游戏后自动调用】
	UFUNCTION(BlueprintCallable)
	void Update();
	/*---------------------------------货币操作---------------------------------*/

	//增加硬币
	UFUNCTION(BlueprintCallable)
	bool AddCoin(int32 _Num, uint8 _Type = 0);
	//减少硬币
	UFUNCTION(BlueprintCallable)
	bool ReduceCoin(int32 _Num, uint8 _Type = 0);
	//发送货币
	UFUNCTION(BlueprintCallable)
	bool SendCoin(const FString& CoinName, int32 _Num);
	//设置货币
	UFUNCTION(BlueprintCallable)
	void SetCoin(const FString& CoinName, int32 _Num);

	//判断货币
	UFUNCTION(BlueprintPure)
	bool CheckCoin(int32 Price, uint8 Type = 0);
	//获取货币
	UFUNCTION(BlueprintPure)
	FCoin GetCoin(uint8 Type = 0);
	/*---------------------------------货币操作---------------------------------*/

	//为当前角色添加经验
	UFUNCTION(BlueprintCallable)
	static void AddPlayerEx(float _ExValue);
	//检查角色等级是否满足条件
	UFUNCTION(BlueprintCallable)
	static bool CheckPlayerGrade(int32 CurGrade);
	//添加关卡评级
	UFUNCTION(BlueprintCallable)
	static void AddPlayerLevelScore(const FString& _LevelName, const FPlayerLevelGrade& _Grade);
	//添加关卡
	UFUNCTION(BlueprintCallable)
	static bool AddNewPlayerLevel(const FString& LevelName, int32 State = 0);
public:

	/*---------------------------------物品获取---------------------------------*/

	//获取卡片列表【总数量数据】
	UFUNCTION()
	TMap<int32, FBaseItemSave>& GetCardList();

	//获取装备列表【装备表总数量数据】
	UFUNCTION()
	TMap<int32, FBaseItemSave>& GetSourceEquipList();
	//获取当前玩家已经装备的物品列表【装备+数据】
	UFUNCTION()
	TMap<int32, FEquipItemSave>& GetbEquipList();
	//获取当前玩家非装备的物品列表【只有数据】
	UFUNCTION()
	TMap<int32, FEquipItemSave>& GetNotbEquipList();

	//获取材料列表
	UFUNCTION()
	TMap<int32, FBaseItemSave>& GetMaterList();

	//获取邮件列表
	UFUNCTION()
	TMap<int32, FMailItemSave>& GetMailList();

	/*---------------------------------物品获取---------------------------------*/


	//获取背包最大个数(150+背包拓展容量，并非道具总数量)
	UFUNCTION()
	int32 GetBagGridCount(int32 Index);


	/*---------------------------------物品操作---------------------------------*/

	/*
	查询物品装备情况，返回此道具装备状态
	1.物品ID
	2.物品等级
	3.块索引
	4.返回这个索引是否处于非装备的状态
	*/
	UFUNCTION()
	void FindbEquipItem(int32 ItemID, int32 Level, int32 BlockIndex, bool& OutbEquip,bool& OutNotbEquip) const;

	/*
	获取物品装备情况或者装备数据索引
	1.物品ID
	2.获取已经装备的物品
	*/
	FEquipItemSave* GetEquipItem(int32 ItemID, bool GetbEquip = true);

	/*
	添加装备物品
	1.物品ID
	2.物品等级
	3.块索引
	4.是否添加到装备栏
	*/
	UFUNCTION(BlueprintCallable)
	void AddEquipItem(int32 ItemID, int32 Level, int32 BlockIndex, bool AddbEquip = true);

	/*
	删除装备物品
	1.物品ID
	2.物品等级
	3.块索引
	4.删除类型: -1表示自动推导类型
	*/
	UFUNCTION(BlueprintCallable)
	void RemoveEquipItem(int32 ItemID, int32 Level, int32 BlockIndex, int32 ItemCount = 1, int32 Type = -1);
	/*
	删除装备物品类型
	1.类型(EquipType)

	对应类型的道具，存储的详细数据

	*/
	UFUNCTION(BlueprintCallable)
	bool RemoveEquipType(int32 BlockIndex, uint8 Type);

	//移除所有的未装备的物品
	UFUNCTION(BlueprintCallable)
	void RemoveAllEquips();

	//移除所有未装备但是有索引的物品
	UFUNCTION(BlueprintCallable)
	void RemoveAllEquipItems();

	//查询卡片
	UFUNCTION(BlueprintPure)
	bool FindCard(int32 ID, int32 Level, int32 FindCount = 1);

	//消耗卡片
	UFUNCTION(BlueprintCallable)
	bool ConsumeCard(int32 ID, int32 Level, int32 Count = 1);

	//消耗材料
	UFUNCTION(BlueprintCallable)
	bool ConsumeMaterial(int32 ID, int32 Level, int32 Count = 1);

	//查询材料数量
	UFUNCTION(BlueprintPure)
	bool FindMaterialCount(int32 ID, int32 Level, int32 Count = 1);

	/*---------------------------------物品操作---------------------------------*/

	//获取卡片选择配置表
	UFUNCTION()
	TMap<FString, FPlayerSelectListConfig>& GetCardSelectConfig();

	//获取其他数据【例如：因为某些事件，解锁什么类容】
	UFUNCTION()
	FPlayerOtherData& GetOtherData();

	//获取本地装备数据句柄
	class ULocalEquipmentDataHandle* GetLocalEquipmentDataHandle();
private:

	// 本地装备数据句柄
	UPROPERTY()
	class ULocalEquipmentDataHandle* LocalEquipmentDataHandle = nullptr;

};

