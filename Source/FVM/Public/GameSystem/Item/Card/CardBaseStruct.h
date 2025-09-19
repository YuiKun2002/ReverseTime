// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameStart/VS/MapBaseType.h"
#include "GameSystem/GameDataSubsystem.h"
#include "GameSystem/Item/ItemBaseStruct.h"
#include "CardBaseStruct.generated.h"


//-----------------------------------------   卡片类型->拥有这个类型表示属于那一类

UENUM(BlueprintType)
enum class ECardType : uint8 {
	//攻击型
	E_ATK UMETA(Display = "ATK_Type"),
	//生产型
	E_SPAWN UMETA(Display = "Spawn_Type"),
	//防御型
	E_DEFENCE UMETA(Display = "Defence_Type"),
	//功能型
	E_Function UMETA(Display = "Function_Type"),
	//范围攻击型
	E_RangeATK UMETA(Display = "RangeATK_Type"),
	//辅助型
	E_Aid UMETA(Display = "Aid_Type"),

	//卡片最大值【有新类型记得更改】
	E_Max = E_RangeATK + 1u

};

//-----------------------------------------   卡片技能升级类型->拥有这个类型表示会得到这个类型的效果
UENUM(BlueprintType)
enum class ECardSkillType : uint8 {
	//无
	E_NULL UMETA(Display = "NULL_Type"),
	//生产量提升
	E_SpawnFlameNumUp UMETA(Display = "SpawnFlameNumUp_Type"),
	//冷却速度提升
	E_COLDDOWN UMETA(Display = "ColdDown_Type"),
	//攻击速度提升
	E_ATTACKSPEED UMETA(Display = "AttackSpeed_Type"),
};

//-----------------------------------------   卡片星星升级类型->表示每上升一个等级会随之提升某一项数据
UENUM(BlueprintType)
enum class ECardUpGradeType : uint8 {
	//攻击力提升
	E_Atk_Up UMETA(Display = "Atk_Up"),
	//生产速度提升
	E_SpawnSpeed_Up UMETA(Display = "SpawnSpeed_Up"),
	//冷却速度提升
	E_ColdDownSpeed_Up UMETA(Display = "ColdDownSpeed_Up"),
	//防御力(血量)提升
	E_Defence_Up UMETA(Display = "Defence_Up"),
};


//卡片转职
USTRUCT(BlueprintType)
struct FCardTransfer {

	GENERATED_USTRUCT_BODY()
	
	//转职的目标ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CardTransferID = -1;

	//转职所需材料ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<int32> CardTransferMaterials;

	//转职所需等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 CardTransferLevel = 9;
};

//等级提升结构
USTRUCT(BlueprintType)
struct FGrade {
	GENERATED_USTRUCT_BODY()
public:
	//当前经验值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_CurrentEx = 0.f;
	//当前经验值最大值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_CurrentTopEx = 100.f;
	//下一次经验值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_NextEx = 150.f;
	//经验值成长率
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_NextExUpRate = 0.2f;
	//当前等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_CurrentGrade = 0;
	//最大等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_MaxGrade = 10;
};

//----------------------------------------------卡片结构-----------------------------------------------------


//卡片碰撞类型
UENUM(BlueprintType)
enum class ECardCollisionType : uint8 {
	//默认(代表卡片所有)
	E_CardActor UMETA(Display = "CardActor"),
	//一般的默认卡
	E_CardActor2 UMETA(Display = "CardActor2"),
	//类似鱼刺的卡片
	E_CardActor3 UMETA(Display = "CardActor3")
};

//卡片种类类型
UENUM(BlueprintType)
enum class ECardCategoryTag : uint8 {
	//默认(代表卡片所有)
	Def UMETA(Display = "Def"),
	//管线
	Line UMETA(Display = "Line"),
	//投射物
	Projectile UMETA(Display = "Projectile"),
	//生产
	Spawn UMETA(Display = "Spawn"),
	//炸弹
	Bomb UMETA(Display = "Bomb"),
	//高墙面
	HighBlock UMETA(Display = "HighBlock"),
	//低墙面
	LowBlock UMETA(Display = "LowBlock"),
	//地形卡
	GroundBlock UMETA(Display = "GroundBlock"),
	//管线反弹
	LineRebound UMETA(Display = "LineRebound"),
	//投射加强
	ProjectileUp UMETA(Display = "ProjectileUp")
};

//卡片关键字映射默认动画
USTRUCT(BlueprintType)
struct FCardKeyIdleAnimation {
	GENERATED_USTRUCT_BODY()
public:
	//副词
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath IdlePath;
};

//卡片预览图
USTRUCT(BlueprintType)
struct FCardpreViewHead {
	GENERATED_USTRUCT_BODY()
public:
	//卡片名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CardName;
	//预览动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath CardViewAnim;
	//卡片优先级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CardLayer = 0;
	//预览图偏移
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D CardViewOffset;
	//预览图缩放
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D CardViewScale = FVector2D(2.f);
};

//卡片基础底层
USTRUCT(BlueprintType)
struct FItemCard : public FItemBase {

	GENERATED_USTRUCT_BODY()

	//卡片种类
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardCategoryTag CardCategoryTag = ECardCategoryTag::Def;
	//卡片属于白天还是夜晚
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_CardDay = true;
	//自我替换（自我替换刷新生命，不需要）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_ReplaceSelf = false;
	//替换层级[如果设置了替换层级，对应的卡片会触发替换规则]
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<int32> ReplaceLayers;
	//是否允许被复制（将用于幻幻鸡类型的卡片变身）（后续不需要，幻幻鸡内设置）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_bEnableCopy = true;
	//携带咖啡粉则自动激活卡片（夜间转换）（后续不需要，咖啡粉内设置）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_AutoEnableCardDay = true;
	//卡片价格
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_CardPrice = 250;
	//冷却时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_CardColdDown = 3.f;
	//卡片星星等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0;
	//卡片生命值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_CardHP = 10.f;
	//卡片优先级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_CardLayer = 0;
	//卡片是否可以重叠(覆盖)【不需要】
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_CardOverlap = false;
	//卡片重叠对象名称【不需要】
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> M_CardOverlapID;
	//卡片价格是否随玩家放置个数线性增加
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_CardPriceAutoUp = false;
	//卡片每次增加25火苗
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_CardPriceUpNum = 25;
	//卡片转职
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCardTransfer TransferData;
	//卡片类型(用于结构转型)[不需要]
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardType M_ECardType = ECardType::E_ATK;
	//卡片技能升级后开启的能力[不需要]
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardSkillType M_ECardSkillType = ECardSkillType::E_ATTACKSPEED;
	//卡片技能ID-> 0 关联ID为0的防御卡，使用它的能力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 M_CardSkillFuncID = -1;
	//卡片对应的技能书名称【不需要】
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString M_CardSkillBookName = FString(TEXT("NULL"));
	//卡片星星升级类型->表示每上升一个等级会随之提升某一项数据
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardUpGradeType M_ECardUpGradeType = ECardUpGradeType::E_Atk_Up;
	//因为等级每次的提升比率
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_M_ECardUpGradeUpRate = 0.f;
	//卡片线路类型->顾名思义就是种植的当前网格，如果网格是【陆地】类型（当前卡片类型必须是:陆地）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELineType M_ELineType = ELineType::OnGround;
	//道具描述2
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemDescrible_2 = FString(TEXT("描述信息待完善"));
	//卡片对应的蓝图实例
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftClassPath CardActorResource;
	//卡片的大头像
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath CardActorHead;
	//确定卡片特殊性(标记特殊会在强化中表示高概率卡片)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_SpecialCard_SynthesisUpGrade = false;
	//是否启用碰撞
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool M_bEnableCardCollision = true;
	//卡片碰撞类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardCollisionType M_ECardCollisionType = ECardCollisionType::E_CardActor2;
	//卡片碰撞宽度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float M_CardCollsionWidth = 20.f;
	//射线设置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLineTraceSetting> M_LineTraceSettings;
	//网格检测设置
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	TArray<FLineCheckSetting> M_LineCheckSetting;

	//FLineCheckSetting
};