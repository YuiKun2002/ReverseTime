// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "Game/UI/Synthesis/SynModel_Structures.h"
#include "Game/UI/Synthesis/SynthesisGeneralGrid.h"
#include "GameSystem/Item/Equipment/PlayerWeaponStruct.h"
#include "SynModel_WeaponGems.generated.h"


/**
 *武器开槽-武器格子
 */
UCLASS()
class FVM_API UUI_SynModel_WeaponBox : public UUI_SynModel_GeneralBox {

	GENERATED_BODY()

public:
	virtual FString ToString_Implementation() override;
	virtual void OnClicked_Implementation() override;
};

/**
 *武器开槽-宝石格子
 */
UCLASS()
class FVM_API UUI_SynModel_GemAttachmentBox : public UUI_SynModel_GeneralBox {

	GENERATED_BODY()

public:

	virtual FString ToString_Implementation() override;
	virtual void OnClicked_Implementation() override;

	//宝石槽位开锁
	void UnLockSlot();

	//宝石槽位锁定
	void LockSlot();

	//绑定道具ID
	void BindItemID(int32 ItemID, int32 SlotIndex);

	//宝石拆卸
	UFUNCTION(BlueprintCallable)
	void UnInstallGem();
	UFUNCTION()
	void SureUnInstallGemEventFunc();
	UFUNCTION()
	void CancelUnInstallGemEventFunc();
	UFUNCTION()
	void ForceUnInstallGemEventFunc();


	//开启开槽选项
	UFUNCTION(BlueprintImplementableEvent)
	void OpenSlotEvent(bool bState);

	//设置空显示
	UFUNCTION(BlueprintImplementableEvent)
	void EmptyGemSlot();

	//显示宝石主内容
	UFUNCTION(BlueprintImplementableEvent)
	void ShowGemSlot();

	//镶嵌
	UFUNCTION(BlueprintImplementableEvent)
	void AttachmentGem();

protected:

	virtual bool SetItemViewBlock(const FItemViewBlock* ItemViewBlock, class UTexture2D* ItemHeadTexture, int32 Index) override;


	//武器开槽
	UFUNCTION(BlueprintCallable)
	void OpenSlot();

	//检测开槽条件
	UFUNCTION(BlueprintCallable)
	bool CheckSlotCondition();

private:
	//绑定开槽物品
	int32 mItemID = -1;
	//绑定槽位索引
	int32 mSlot = 0;
	//默认锁定状态
	bool bLockState = true;

};


/**
 *武器开槽主界面
 */
UCLASS()
class FVM_API UUI_SynModel_WeaponGem : public UWidgetBase, public ISynthesisWidgetInterface
{
	GENERATED_BODY()

public:

	//初始化
	virtual void InitWidget_Implementation() override;

	//选择武器
	void SelectWeapon(uint8 WeaponType,
		const FItemViewBlock& Block,
		const FPlayerWeaponBase& BlockWeaponBase);

	//初始化武器中镶嵌的宝石
	void InitWeapon(const FPlayerWeaponBase& BlockWeaponBase);

	//检测宝石与武器是否兼容
	bool CheckTheGemMatchTheWeapon(int32 ConstWeaponID, uint8 GemType);

	//选择宝石
	void SelectGem(int32 ID, int32 Level);

	//宝石镶嵌
	UFUNCTION(BlueprintCallable)
	void InstallGem();

	//加载道具
	UFUNCTION(BlueprintCallable)
	void LoadItem();

	//加载武器和宝石
	UFUNCTION(BlueprintCallable)
	void LoadWeaponsAndGems(bool bNotLoad = false);

	FORCEINLINE	class UUI_SynModel_WeaponBox* GetWeaponBox() { return this->BP_Syn_WeaponBox; }

	FORCEINLINE	class UButton* GetAttachGemButt() { return this->AttachGem_Butt; }

	FORCEINLINE int32 GetWeaponGemCountMaxCache() { return this->CurWeaponGemCountMax; }

protected:

	//宝石A
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GemAttachmentBox* BP_Syn_GemBox1 = nullptr;

	//宝石B
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GemAttachmentBox* BP_Syn_GemBox2 = nullptr;

	//宝石C
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GemAttachmentBox* BP_Syn_GemBox3 = nullptr;

	//宝石D
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_GemAttachmentBox* BP_Syn_GemBox4 = nullptr;

	//武器框
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UUI_SynModel_WeaponBox* BP_Syn_WeaponBox = nullptr;

	//宝石安装费用文本
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* AttachGemCoinText = nullptr;

	//镶嵌
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* AttachGem_Butt = nullptr;

private:
	//当前宝石依附槽位的索引
	UPROPERTY()
	int32 CurAttachSlotIndex = 0;
	UPROPERTY()
	int32 CurWeaponGemCountMax = 0;
	UPROPERTY()
	uint8 CurWeaponType = 0U;
	UPROPERTY()
	class UUI_PlayerSynthesis* Syn = nullptr;
};