// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameStart/Flipbook/GameStartActor.h"
#include "GameStart/Flipbook/GameActor/Mouse/Normal/NormalBase.h"
#include "Data/CardData/MouseDataStruct.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "MagicMasterMouse.generated.h"

/**
 * 魔笛鼠
 */

 //所有资产路径
USTRUCT(BlueprintType)
struct FMagicMasterResourceStruct {
	GENERATED_USTRUCT_BODY()
public:
	//正常形态
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UPaperFlipbook> Idle;
	//正常形态(攻击)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UPaperFlipbook> Attack;
	//吹笛
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UPaperFlipbook> Use;
	//死亡形态(死亡)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UPaperFlipbook> Death;

	//吹笛动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UPaperFlipbook> UseAnim;
	//加血动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UPaperFlipbook> AddHPAnim;
};

UCLASS()
class FVM_API AMagicMasterHpAddtionBuff : public AGameStartActor {
	GENERATED_BODY()
public:
	void Init(AMouseActor* MouseActor,TSoftObjectPtr<UPaperFlipbook> Anim);
	virtual void GameTick(float DeltaTime) override;
private:
	UPROPERTY()
		AMouseActor* CurMouse = nullptr;

	float CTime = 0.2f;
	float time = 0.f;
};

UCLASS()
class FVM_API AMagicMasterMouse : public ANormalBase
{
	GENERATED_BODY()
public:
	//网格碰撞组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* MMesheComponent = nullptr;
	//身体碰撞组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USphereComponent* MBodyComponent = nullptr;
	//魔笛动画显示位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UFlipbookBaseComponent* WeaponAnimLocation = nullptr;
	//老鼠动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "老鼠")
		FMagicMasterResourceStruct AnimRes;
	//使用魔笛武器的冷却时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "老鼠")
		float fUseWeaponTime = 15.f;
	//多少范围内的老鼠生效【半径】
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "老鼠")
		float fWeaponRangeRadius = 90.f;
	//回复自身血量的百分比
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "老鼠")
		float fReplySelfHpRate = 0.3f;
	//可以攻击卡片的什么类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "老鼠")
		ECardCollisionType MECardCollisionType = ECardCollisionType::E_CardActor2;
public:
	AMagicMasterMouse();
	//初始化
	virtual void BeginPlay() override;
	virtual void MouseInit() override;
	virtual void MouseTick(const float& DeltaTime) override;
	//动画播放完毕
	void OnAnimationPlayEnd();
	//更新状态
	void UpdateState();
public:
	//老鼠开始移动
	virtual void MoveingBegin() override;
	//移动逻辑函数
	virtual void MoveingUpdate(float DeltaTime) override;
	//当老鼠被命中时受到的伤害数值
	virtual bool BeHit(UObject* CurHitMouseObj,float HurtValue, EFlyItemAttackType AttackType) override;
	//老鼠开始攻击
	virtual void AttackedBegin() override;
	//老鼠攻击结束
	virtual void AttackedEnd() override;
	//当老鼠死亡时
	virtual void MouseDeathed() override;
private:
	//是否启动攻击线
	bool bEnableAttakLine = true;
	//是否处于创建动画状态
	bool bUse = false;

	//使用魔笛的延迟
	float fUseDelay = 0.f;
	float fWeaponRangeRadius_ = 90.f;
	float fReplySelfHpRate_ = 0.3f;
};
