// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "Game/UI/Synthesis/SynModel_Structures.h"
#include "Game/UI/Synthesis/SynthesisGeneralGrid.h"
#include "GameSystem/Item/Equipment/PlayerWeaponStruct.h"
#include "SynModel_GemsUpGrade.generated.h"


/**
 *宝石强化-武器格子
 */
UCLASS()
class FVM_API UUI_SynModel_GemUpgradeWeaponBox : public UUI_SynModel_GeneralBox {

	GENERATED_BODY()

public:
	virtual FString ToString_Implementation() override;
	virtual void OnClicked_Implementation() override;
};

/**
 *宝石强化-武器镶嵌格子
 */
UCLASS()
class FVM_API UUI_SynModel_GemUpgradeWeaponAttachBox : public UUI_SynModel_GeneralBox {

	GENERATED_BODY()

public:
	virtual FString ToString_Implementation() override;
	virtual void OnClicked_Implementation() override;
	//绑定槽位
	FORCEINLINE	void BindGemSlot(int32 SlotIndex) { this->mSlotIndex = SlotIndex; }
	//获取槽位
	FORCEINLINE int32 GetGemSlotIndex() const { return this->mSlotIndex; }
public:
	//显示宝石主内容
	UFUNCTION(BlueprintImplementableEvent)
	void ShowGemSlot();
	//选择
	UFUNCTION(BlueprintImplementableEvent)
	void SelectGemSlot();

private:
	int32 mSlotIndex = 0;

};

/**
 *宝石强化-宝石格子
 */
UCLASS()
class FVM_API UUI_SynModel_GemUpgradeGemBox : public UUI_SynModel_GeneralBox {

	GENERATED_BODY()

public:

	virtual FString ToString_Implementation() override;
	virtual void OnClicked_Implementation() override;

};

/**
 *宝石强化-宝石数量
 */
UCLASS()
class FVM_API UUI_SynModel_GemCountBox : public UUI_SynModel_GeneralBox {

	GENERATED_BODY()

public:
	//更新数量
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCount(int32 NeedCount);

};



/**
 *  合成屋-宝石强化主界面
 */
UCLASS()
class FVM_API USynModel_GemsUpGrade : public UWidgetBase, public ISynthesisWidgetInterface
{
	GENERATED_BODY()

public:
	//初始化
	virtual void InitWidget_Implementation() override;

	//初始化界面
	void InitPanel();

	//加载武器宝石
	void LoadWeaponAndGem();

	//加载四叶草和水晶
	void LoadCloverAndScrystal();

	//选择武器
	void SelectWeapon(uint8 WeaponType,
		const FItemViewBlock& Block,
		const FPlayerWeaponBase& BlockWeaponBase);

	//初始化武器中镶嵌的宝石
	void InitWeapon(const FPlayerWeaponBase& BlockWeaponBase);

	//选择宝石槽位
	void SelectGemSlot(class UUI_SynModel_GemUpgradeWeaponAttachBox* GemSlot);

	//选择宝石
	void SelectGemSlot(int32 MaxLevel, class UItemGridBase* Gem);

	//添加宝石
	int32 AddGem();
	//修改宝石等级
	int32 EditGemLevel(int32 PlusNum);

	//添加水晶
	void AddCrystal(FPlayerWeaponEquipGemDataBlock& GemData);

	//设置概率
	UFUNCTION(BlueprintCallable)
	void SetRate(class UItemGridBase* CopyCloverGrid, float Rate);

	//水晶强化
	UFUNCTION(BlueprintCallable)
	void UpgradeGemBut();

	FORCEINLINE class UUI_SynModel_GemUpgradeWeaponBox* GetWeaponBox() { return this->GemWeaponBox; }

protected:

	//选择武器时的强化界面【默认开启此界面】
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UCanvasPanel* WeaponGemPanel = nullptr;
	//武器盒子
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GemUpgradeWeaponBox* GemWeaponBox = nullptr;

	//宝石槽位1
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GemUpgradeWeaponAttachBox* BP_Syn_GemBox1 = nullptr;
	//宝石槽位2
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GemUpgradeWeaponAttachBox* BP_Syn_GemBox2 = nullptr;
	//宝石槽位3
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GemUpgradeWeaponAttachBox* BP_Syn_GemBox3 = nullptr;
	//宝石槽位4
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GemUpgradeWeaponAttachBox* BP_Syn_GemBox4 = nullptr;

	//选择宝石时的强化界面
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UCanvasPanel* SingleGemPanel = nullptr;
	//宝石盒子
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GemUpgradeGemBox* GemBox = nullptr;

	//强化水晶盒子
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GemCountBox* UpgradeCrystalBox = nullptr;

	//强化四叶草盒子
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GeneralBox* UpGemCloverBox = nullptr;

	//强化概率文本
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* RateText = nullptr;

	//强化水晶的金币文本
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* UpgradeGemCoinText = nullptr;

	//强化水晶按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* UpgradeGem_Butt = nullptr;


private:

	UPROPERTY()
	class UUI_PlayerSynthesis* Syn = nullptr;

	//当前选择的宝石槽位
	UPROPERTY()
	UObject* CurSelectGemSlot = nullptr;
	//当前选择的宝石等级
	UPROPERTY()
	int32 CurSelectGemLevel = 0;
	//当前选择的宝石最大等级
	UPROPERTY()
	int32 CurSelectGemLevelMax = 0;
	//水晶ID 
	UPROPERTY()
	int32 CurCrystialID = 869;
	//强化倍率
	UPROPERTY()
	float UpRate = 1.f;
	//水晶是否足够
	UPROPERTY()
	bool bUpGem = false;

	UPROPERTY()
	uint8 CurWeaponType = 0U;
	UPROPERTY()
	int32 CurWeaponGemCountMax = 0;
private:
	//宝石强化概率
	const int32 GemsValue[17] = { 100,88,75,63,50,38,26,19,13,10,9,8,7,6,5,0,0 };
	//需要的水晶
	const int32 NeedCrystalNums[17] = { 5,10,15,25,35,55,75,100,130,160,200,245,300,355,420,666,0 };
	//需要的保险
	const int32 NeedInsuranceNums[17] = { 0,0,0,0,0,0,400,500,600,2500,3000,3500,4000,4500,5000,6000,0 };
	//强化金额
	const int32 NeedCoin[17] = { 100,200,400,1000,2000,2500,3000,3500,4000,4500,5000,6000,7000,8000,9000,10000 ,0 };
	//水晶ID
	const int32 Crystials[2] = { 869,870 };
};
