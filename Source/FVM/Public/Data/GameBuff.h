// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Classes/Engine/DataTable.h"
#include "GameSystem/Tools/DynamicProperty.h"
#include "GameBuff.generated.h"

//全局默认变量
#define GAMEBUFF_VAR_DEFOBJECT TEXT(".")

//灼烧颜色
#define COLOR_BUFF_BURN FVector(1.f, 0.3f, 0.4f)
//减速颜色
#define COLOR_BUFF_SLOWDOWN FVector(0.5f, 1.f, 1.f)
//加速颜色
#define COLOR_BUFF_ACCELERATE FVector(1.f, 0.5f, 0.3f)
//禁锢颜色
#define COLOR_BUFF_SOLIDIFICATION FVector(0.9f, 0.9f, 0.9f)
//冻结颜色
#define COLOR_BUFF_FREEZE FVector(0.0f, 0.8f, 1.f)



//buff对象
UENUM(BlueprintType)
enum class EGameBuffTag : uint8
{
	//无效果
	Null UMETA(DisplayName = "Null"),
	//加速
	Accelerate UMETA(DisplayName = "Accelerate"),
	//减速
	SlowDown UMETA(DisplayName = "SlowDown"),
	//冻结
	Freeze UMETA(DisplayName = "Freeze"),
	//灼烧
	Burn UMETA(DisplayName = "Burn"),
	//禁锢
	Solidification UMETA(DisplayName = "Solidification")
};

//对象buff，针对谁的buff
UENUM(BlueprintType)
enum class EGameBuffCharTag : uint8
{
	//防御卡
	Card UMETA(DisplayName = "Card"),
	//老鼠
	Mouse UMETA(DisplayName = "Mouse")
};

//buff的信息结构
USTRUCT(BlueprintType)
struct FGameBuffData {
	GENERATED_USTRUCT_BODY()
public:
	//当前所有的buff
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurBuffTime;
	//当前所有的buff动态属性
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UBuffDynamicProperty> CurBuffProperty;
};

//buff的信息结构
USTRUCT(BlueprintType)
struct FGameBuffInfor {
	GENERATED_USTRUCT_BODY()
public:
	//当前所有的buff
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EGameBuffTag, FGameBuffData> CurBuffs;
};

//buff默认信息资产结构
USTRUCT(BlueprintType)
struct FGameBuffInforAssetDataTable : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameBuffInfor BaseBuffAsset;
};

UINTERFACE(MinimalAPI)
class UGameBuffInterface : public UInterface
{
	GENERATED_BODY()
};

class FVM_API IGameBuffInterface
{
	GENERATED_BODY()
protected:
	//初始化
	virtual void BuffInit(float BuffTime) = 0;
	//初始化更新
	virtual void BuffUpdate(class UBuffDynamicProperty* Property) = 0;
	//Buff执行
	virtual void BuffExec() = 0;
	//更新
	virtual void Tick(float BuffTime) = 0;
	//buff结束
	virtual void BuffEnd() = 0;
	//是否是负面状态的buff
	virtual bool GetDebuff() = 0;
	//是否是限制类型的buff[冻结，凝固]
	virtual bool GetConstbuff() = 0;
	//更新buff颜色
	virtual void UpdateColor() = 0;
	//回收
	virtual void BuffGC() = 0;
};

//Buff的动态属性
UCLASS(Blueprintable)
class FVM_API UBuffDynamicProperty : public UDynamicProperty {
	GENERATED_BODY()
public:
	//设置默认对象
	UFUNCTION(BlueprintCallable)
	void SetDefObject(UObject* Value);
	//获取默认对象
	UFUNCTION(BlueprintPure)
	void GetDefObject(UObject*& Value);

	//默认对象初始化
	UFUNCTION(BlueprintImplementableEvent)
	void OnDefObjectInit(UObject* Value);
};

//buff基本类型
UCLASS()
class FVM_API UBuffObject : public UObject, public IGameBuffInterface
{
	GENERATED_BODY()

	friend class UGameBuff;
	friend class UCardGameBuff;
	friend class UMouseGameBuff;
public:
	//获取Buff属性对象
	UFUNCTION(BlueprintCallable)
	UBuffDynamicProperty* GetDynamicProperty();

	//获取buff时间
	float GetCurTime() const;
	//获取buff标记
	const EGameBuffTag& GetCurTag() const;

	//创建一个buff实例
	template<class BuffType>
	static BuffType* MakeNewBuffObject(
		EGameBuffTag NewTag,
		float NewBuffTime,
		UBuffDynamicProperty* Property,
		UGameBuff* ParentBuff
	)
	{
		BuffType* CurNewBuff = NewObject<BuffType>();
		CurNewBuff->CurTag = NewTag;
		CurNewBuff->CurTime = NewBuffTime;
		CurNewBuff->CurBuffObject = ParentBuff;
		CurNewBuff->DynamicProperty = Property;
		return CurNewBuff;
	}
protected:
	//当buff第一次被添加时
	virtual void BuffInit(float BuffTime) override;
	//当buff多次被修改时，【二次新增,动态属性更新】
	virtual void BuffUpdate(class UBuffDynamicProperty* Property) override;
	//Buff执行
	virtual void BuffExec() override;
	//当游戏更新时
	virtual void Tick(float BuffTime) override;
	//当buff结束时
	virtual void BuffEnd() override;
	//获取这个buff是不是debuff
	virtual bool GetDebuff() override;
	//获取这个buff是不是固定(限制级)buff
	virtual bool GetConstbuff() override;
	//更新buff提供的颜色
	virtual void UpdateColor() override;
	//回收
	virtual void BuffGC() override;
protected:
	//获取游戏buff对象
	class UGameBuff* GetGameBuff();
	//获取buff
	UBuffObject* GetBuffByTag(EGameBuffTag NewTag);
	//设置更新率
	virtual void SetTickRate(float NewRate);
	//buff时间抵消[两个buff更具时间相互抵消，时间为0则结束buff],如果buff不存在则返回false
	bool SetBuffTimeOffset(UBuffObject* Buff1, UBuffObject* Buff2);
	//设置buff的时间
	void SetBuffCurTime(const EGameBuffTag& NewTag, const float& NewTime);
	//强制设置buff持续时间
	void SetForceCurTime(const float& NewTime);
private:
	//当前持续的时间
	UPROPERTY()
	float CurTime = 0.f;
	//当前buff的标签
	UPROPERTY()
	EGameBuffTag CurTag = EGameBuffTag::Burn;
	//当前buff对象
	UPROPERTY()
	class UGameBuff* CurBuffObject = nullptr;
	//当前动态属性
	UPROPERTY()
	class UBuffDynamicProperty* DynamicProperty = nullptr;
	//是否触发当前buff
	UPROPERTY()
	bool bTriggerBuff = true;
};

//执行buff
DECLARE_DELEGATE_TwoParams(ExecuteBuffDelegate, EGameBuffTag CurTag, float& CurBuffTime);
//退出buff
DECLARE_DELEGATE_OneParam(ExitBuffDelegate, EGameBuffTag CurTag);
//Buff全部执行完毕
DECLARE_DELEGATE_OneParam(GameBuffFinishedDelegate, class UGameBuff* CurBuffObject);


/**
 * 游戏buff
 */
 //Buff管理器基类
UCLASS()
class FVM_API UGameBuff : public UObject
{
	GENERATED_BODY()

	friend class UBuffObject;

public:
	//当执行buff时
	ExecuteBuffDelegate OnExecuteBuffDelegate;
	//当退出buff时
	ExitBuffDelegate OnExitBuffDelegate;
	//当前buff全部执行完毕
	GameBuffFinishedDelegate OnBuffExecuteFinishedDelegate;
public:
	//生成buff对象
	UFUNCTION(BlueprintCallable)
	static UGameBuff* MakeGameBuff(UObject* NewBuffChar, EGameBuffCharTag NewBuffTag);
	//添加新buff
	UFUNCTION(BlueprintCallable)
	void AddBuff(
		EGameBuffTag NewTag,
		float NewBuffTime,
		UBuffDynamicProperty* Property,
		UObject* OwnerObject = nullptr
	);
	//更新buff
	UFUNCTION(BlueprintCallable)
	void UpdateBuff(const float& DeltaTime);
public:
	//打印
	UFUNCTION(BlueprintCallable)
	void DebugLog(const FString& ActorName, EGameBuffTag NewTag, bool Tirgger);

	//添加buff
	UFUNCTION(BlueprintCallable)
	void AddBuffBySubclass(
		EGameBuffTag NewTag,
		float NewBuffTime,
		const TSubclassOf<UBuffDynamicProperty>& Property,
		UObject* OwnerObject = nullptr
	);
	//添加buff集合
	UFUNCTION(BlueprintCallable)
	void AddBuffInfor(FGameBuffInfor NewBuff);

	//直接清除所有buff【不在Tick】
	UFUNCTION(BlueprintCallable)
	void ClearBuffs();
	//直接结束所有Buff
	UFUNCTION(BlueprintCallable)
	void EndBuffs();
	//移除buff
	UFUNCTION(BlueprintCallable)
	void RemoveBuff(EGameBuffTag NewTag);

	//获取是否存在buff
	UFUNCTION(BlueprintCallable)
	bool GetBuffExist() const;
	//获取某个buff是否存在
	UFUNCTION(BlueprintCallable)
	bool GetBuffExistByTag(EGameBuffTag NewTag);
	//获取是否存在限制buff
	UFUNCTION(BlueprintCallable)
	bool GetConstBuff() const;
	//获取buff对象
	UFUNCTION(BlueprintCallable)
	UBuffObject* GetBuff(EGameBuffTag NewBuffTag);
	//获取tick的更新率
	UFUNCTION(BlueprintCallable)
	float GetTickRate() const;
	//获取buff的对象
	UFUNCTION(BlueprintCallable)
	UObject* GetBuffChar();
protected:
	//生成一个新的buff对象
	virtual	UBuffObject* GetNewBuffObject(EGameBuffTag NewTag, float NewBuffTime, UBuffDynamicProperty* Property);
	//更新buff颜色[false表示没有任何颜色更新]
	virtual	bool UpdateBuffColor(UBuffObject* NewBuffObject);
protected:
	//设置buff的时间
	void SetBuffCurTime(const EGameBuffTag& CurTag, const float& NewTime);
	//获取一个默认的buff属性
	UBuffDynamicProperty* GetDefBuffPropertyByTag(const EGameBuffTag& NewTag);
private:
	//buff集合
	UPROPERTY()
	TMap<EGameBuffTag, UBuffObject*> CurBuffs;
	//当前结束buff的集合
	UPROPERTY()
	TMap<EGameBuffTag, UBuffObject*> CurEndBuffs;
	//角色buff类型
	UPROPERTY()
	EGameBuffCharTag BuffTag = EGameBuffCharTag::Mouse;
	//buff对象
	UPROPERTY()
	UObject* BuffChar = nullptr;
	//是否存在限制类型的buff
	UPROPERTY()
	bool bConstBuff = false;
	//时间更新率
	UPROPERTY()
	float UpdateTickRate = 1.f;
	//基本buff基础数据
	UPROPERTY()
	FGameBuffInfor GameBuffInforBaseData;
private:
	void BuffInit();
};
