// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameStart/Flipbook/GameActor/CardActor/ATK/ATKCardActorBase.h"
#include "Data/CardData/CardDataStruct.h"
#include "AttackCardActor.generated.h"


/*
	攻击卡片基础功能
	【一般直线攻击卡片】小笼包，三线，双向等这种类型的基础卡片
*/

class UPaperFlipbook;
class AFlyItemActor;
class AMouseActor;

//攻击类型卡片【基础】
UCLASS()
class FVM_API AAttackCardActor : public AATKCardActorBase
{
	GENERATED_BODY()

public:
	//获取攻击卡片数据
	const FItemCardATK& GetATKCardData();
public:

	virtual void OnCardInitData(const FItemCard& NewBaseData) override;

public:
	//设置当前正在攻击的老鼠
	UFUNCTION(BlueprintCallable)
	void SetCurrentMouse(class  AMouseActor* MouseActor);

public:
	//获取当前正在攻击的老鼠
	UFUNCTION(BlueprintPure)
	class AMouseActor* GetCurrentMouse();

public:
	//卡片默认动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "卡片预设属性 | 动画")
	TSoftObjectPtr<UPaperFlipbook> CardActor_DefAnim;
	//卡片攻击动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "卡片预设属性 | 动画")
	TSoftObjectPtr<UPaperFlipbook> CardActor_AttackAnim;
	//卡片攻击的子弹
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "卡片预设属性 | 子弹对象类")
	TSoftClassPtr<AFlyItemActor> CardActor_BulletClassObj;
private:
	//卡片数据
	UPROPERTY()
	FItemCardATK ItemCardATK;
	//当前卡片正在攻击的老鼠对象实例
	UPROPERTY()
	class AMouseActor* CurAttackMouseActor = nullptr;
};
