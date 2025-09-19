// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Data/GameBuff.h"
#include "MouseGameBuff.generated.h"

class AMouseActor;

//老鼠buff反应规则
/*
*
1.冻结->灼烧【相互抵消】

2.冻结结束->触发->减速

3.减速->灼烧【相互抵消】

4.减速->加速【共存，速度相互抵消】

5.冻结->凝固->(无法移动)减速->(无法移动)加速【共存，无法移动】

*/


/**
 * 老鼠专用Buff系统
 */
UCLASS()
class FVM_API UMouseGameBuff : public UGameBuff
{
	GENERATED_BODY()
	friend class UBuffObject;
public:
	//如果遇到控制类型的buff，需要恢复时的Tick值
	UPROPERTY()
	float TargetTickRate = 1.f;
protected:
	//生成一个新的buff对象
	virtual	UBuffObject* GetNewBuffObject(
		EGameBuffTag NewTag,
		float NewBuffTime,
		UBuffDynamicProperty* Property
	) override;

	//更新颜色
	virtual	bool UpdateBuffColor(UBuffObject* NewBuffObject) override;
};


//老鼠buff对象
UCLASS()
class FVM_API UBuffMouseObject : public UBuffObject
{
	GENERATED_BODY()
public:
	UFUNCTION()
	AMouseActor* GetBuffChar();
};

//冻结buff、凝固buff,控制buff,发生速度反应
UCLASS()
class FVM_API UCtrlBuffLinkBuffMouse : public UBuffMouseObject
{
	GENERATED_BODY()
protected:
	virtual void BuffExec() override;
	virtual void BuffEnd() override;
	virtual bool GetConstbuff() override;
};

//冻结buff属性名称
#define FREEZEBUFFPRONAME TEXT("FreezeBuffEndTime")
//冻结buff
UCLASS()
class FVM_API UFreezeBuffMouse : public UCtrlBuffLinkBuffMouse
{
	GENERATED_BODY()
protected:
	virtual void BuffInit(float BuffTime) override;
	virtual void BuffExec() override;
	virtual void BuffUpdate(UBuffDynamicProperty* Property) override;
	virtual void Tick(float BuffTime) override;
	virtual void BuffEnd() override;
	virtual void UpdateColor() override;
	virtual void BuffGC() override;
private:
	//冰块对象
	UPROPERTY()
	class ASpriteActor* IceBlock = nullptr;
	//buff结束减速buff的持续时间
	UPROPERTY()
	float NewTime;
	//冰块更新时间
	UPROPERTY()
	float IceBlockTime = 1.f;
};

//凝固buff
UCLASS()
class FVM_API USolidificationBuffMouse : public UCtrlBuffLinkBuffMouse
{
	GENERATED_BODY()
protected:
	virtual void UpdateColor() override;
};

#define BURNBUFFPRO2 TEXT("BuffDelay")
#define BURNBUFFPRO3 TEXT("ATKRate")

//灼烧buff
UCLASS()
class FVM_API UBurnBuffMouse : public UBuffMouseObject
{
	GENERATED_BODY()
protected:
	virtual void BuffInit(float BuffTime) override;
	virtual void BuffUpdate(UBuffDynamicProperty* Property) override;
	virtual void Tick(float BuffTime) override;
	virtual void UpdateColor() override;
private:
	//延迟多少触发
	UPROPERTY()
	float BuffDelay = 0.2f;
	//延迟计时器
	UPROPERTY()
	float BuffDelayTime = 0.f;
	//持续时间
	UPROPERTY()
	float CurBuffTime = 0.f;
	//伤害
	UPROPERTY()
	float ATK = 0.f;
	//触发
	UPROPERTY()
	bool bEnable = false;
};

//减速buff、加速buff,发生速度反应
UCLASS()
class FVM_API USlowDownLinkBuffMouse : public UBuffMouseObject
{
	GENERATED_BODY()
protected:
	//减速与加速反应
	void SlowDownAndAccelerate();
protected:
	//buff结束
	virtual void BuffEnd() override;
};


#define TICKRATEBUFF TEXT("Rate")
//减速buff
UCLASS()
class FVM_API USlowDownBuffMouse : public USlowDownLinkBuffMouse
{
	GENERATED_BODY()
public:
	UPROPERTY()
	float Rate = 0.35f;
protected:
	virtual void BuffInit(float BuffTime) override;
	virtual void BuffExec() override;
	virtual void BuffUpdate(UBuffDynamicProperty* Property) override;
	virtual void UpdateColor() override;
};

//加速buff
UCLASS()
class FVM_API UAccelerateBuffMouse : public USlowDownLinkBuffMouse
{
	GENERATED_BODY()
public:
	UPROPERTY()
	float Rate = 0.35f;
protected:
	virtual void BuffExec() override;
	virtual void BuffUpdate(UBuffDynamicProperty* Property) override;
	virtual bool GetDebuff() override;
	virtual void UpdateColor() override;
};