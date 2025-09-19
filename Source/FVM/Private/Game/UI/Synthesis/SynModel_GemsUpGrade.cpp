// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Synthesis/SynModel_GemsUpGrade.h"
#include <Components/Button.h>
#include <Components/TextBlock.h>
#include <Components/CanvasPanel.h>
#include "GameSystem/TaskSubsystem.h"
#include "Data/EquipmentDataSturct.h"
#include "Game/UI/Tips/UI_SelectTip.h"
#include "Game/UI/UI_PlayerSynthesis.h"
#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/UI_PlayerBagMaterialGrid.h"
#include "Game/UI/Tips/UI_NotUnlockedItemTip.h"
#include "Data/Texture/TextureDataAssetCache.h"
#include "Game/UI/Synthesis/SynModel_WeaponGems.h"
#include "GameSystem/Item/Equipment/PlayerWeaponStruct.h"
#include "GameSystem/PlayerData/LocalEquipmentDataHandle.h"

FString UUI_SynModel_GemUpgradeWeaponBox::ToString_Implementation() {
	if (this->GetItemViewBlock().IsValid())
	{
#if WITH_EDITOR
		return GameDataStaticObject<
			UPlayerBagEquipView
		>()->GetItemName(this->GetItemViewBlock().ItemID) +
			FString(TEXT("\nItem ID ：")) +
			FString::FromInt(this->GetItemViewBlock().ItemID) +
			FString(TEXT("\nUI Index ：")) +
			FString::FromInt(this->GetUiIndex()) +
			FString(TEXT("\nBlock Index ：")) +
			FString::FromInt(this->GetItemViewBlock().BlockIndex);

#endif
		return GameDataStaticObject<
			UPlayerBagEquipView
		>()->GetItemName(this->GetItemViewBlock().ItemID);
	}

	return FString();
}
FString UUI_SynModel_GemUpgradeWeaponAttachBox::ToString_Implementation() {
	if (this->GetItemViewBlock().IsValid())
	{
#if WITH_EDITOR
		return GameDataStaticObject<
			UPlayerBagEquipView
		>()->GetItemName(this->GetItemViewBlock().ItemID) +
			FString(TEXT("\nItem ID ：")) +
			FString::FromInt(this->GetItemViewBlock().ItemID) +
			FString(TEXT("\nUI Index ：")) +
			FString::FromInt(this->GetUiIndex()) +
			FString(TEXT("\nBlock Index ：")) +
			FString::FromInt(this->GetItemViewBlock().BlockIndex);

#endif
		return GameDataStaticObject<
			UPlayerBagEquipView
		>()->GetItemName(this->GetItemViewBlock().ItemID);
	}

	return FString();
}
FString UUI_SynModel_GemUpgradeGemBox::ToString_Implementation() {
	if (this->GetItemViewBlock().IsValid())
	{
#if WITH_EDITOR
		return GameDataStaticObject<
			UPlayerBagEquipView
		>()->GetItemName(this->GetItemViewBlock().ItemID) +
			FString(TEXT("\nItem ID ：")) +
			FString::FromInt(this->GetItemViewBlock().ItemID) +
			FString(TEXT("\nUI Index ：")) +
			FString::FromInt(this->GetUiIndex()) +
			FString(TEXT("\nBlock Index ：")) +
			FString::FromInt(this->GetItemViewBlock().BlockIndex);

#endif
		return GameDataStaticObject<
			UPlayerBagEquipView
		>()->GetItemName(this->GetItemViewBlock().ItemID);
	}

	return FString();
}



void UUI_SynModel_GemUpgradeWeaponBox::OnClicked_Implementation() {
	this->Cancel();
	Cast<USynModel_GemsUpGrade>(this->GetParentClass())->InitPanel();
}

void UUI_SynModel_GemUpgradeWeaponAttachBox::OnClicked_Implementation() {
	Cast<USynModel_GemsUpGrade>(this->GetParentClass())->SelectGemSlot(this);
}

void UUI_SynModel_GemUpgradeGemBox::OnClicked_Implementation() {
	if (this->GetItemViewBlock().IsValid())
	{
		this->Cancel();
		Cast<USynModel_GemsUpGrade>(this->GetParentClass())->InitPanel();
	}
}


void USynModel_GemsUpGrade::InitWidget_Implementation()
{
	this->Syn = Cast<UUI_PlayerSynthesis>(this->GetParentClass());

	//设置管理类型
	this->GemWeaponBox->SetParentClass(this);
	this->BP_Syn_GemBox1->SetParentClass(this);
	this->BP_Syn_GemBox1->BindGemSlot(0);
	this->BP_Syn_GemBox2->SetParentClass(this);
	this->BP_Syn_GemBox2->BindGemSlot(1);
	this->BP_Syn_GemBox3->SetParentClass(this);
	this->BP_Syn_GemBox3->BindGemSlot(2);
	this->BP_Syn_GemBox4->SetParentClass(this);
	this->BP_Syn_GemBox4->BindGemSlot(3);
	this->GemBox->SetParentClass(this);
	this->UpgradeCrystalBox->SetParentClass(this);
	this->UpGemCloverBox->SetParentClass(this);

	//初始化
	this->InitPanel();

	this->LoadWeaponAndGem();
	this->LoadCloverAndScrystal();

	//显示界面
	this->Syn->SetCardAndGemsPanelShow(this, ESlateVisibility::Visible, 1, ESlateVisibility::Visible);
}

void USynModel_GemsUpGrade::InitPanel() {

	this->UpgradeGem_Butt->SetIsEnabled(false);

	this->UpgradeCrystalBox->Cancel();
	this->UpGemCloverBox->Cancel();

	this->WeaponGemPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SingleGemPanel->SetVisibility(ESlateVisibility::Collapsed);

	this->CurSelectGemSlot = nullptr;
	this->CurSelectGemLevel = 0;
	this->CurSelectGemLevelMax = 0;
	this->UpRate = 1.f;
	this->bUpGem = false;

	//设置金币
	this->UpgradeGemCoinText->SetText(
		FText::FromString(TEXT("0"))
	);
	//设置保险金
	this->Syn->GetSynInsurancePanel()->SetSynInsurancePrice(0);
	//设置概率
	this->RateText->SetText(FText::FromString(TEXT("0.00%")));

	FItemViewBlock TempBock;
	FPlayerWeaponBase TempBockWeaponBase;
	this->SelectWeapon(0U, TempBock, TempBockWeaponBase);
}

void USynModel_GemsUpGrade::SelectWeapon(uint8 WeaponType,
	const FItemViewBlock& Block,
	const FPlayerWeaponBase& BlockWeaponBase) {
	if (Block.ItemID == -1)
	{
		this->GemWeaponBox->Cancel();
		this->BP_Syn_GemBox1->Cancel();
		this->BP_Syn_GemBox2->Cancel();
		this->BP_Syn_GemBox3->Cancel();
		this->BP_Syn_GemBox4->Cancel();
		this->LoadWeaponAndGem();
		this->LoadCloverAndScrystal();
	}
	else {
		this->WeaponGemPanel->SetVisibility(ESlateVisibility::Visible);
		this->SingleGemPanel->SetVisibility(ESlateVisibility::Collapsed);
		this->GemBox->Cancel();

		this->CurWeaponType = WeaponType;
		this->GemWeaponBox->SetItemAttribute(GameDataStaticObject<UPlayerBagEquipView>(),
			Block, 0);
		this->InitWeapon(BlockWeaponBase);
	}
}

void USynModel_GemsUpGrade::InitWeapon(const FPlayerWeaponBase& BlockWeaponBase)
{
	this->CurWeaponGemCountMax = BlockWeaponBase.M_WeaponGemCountMax;
	ULocalEquipmentDataHandle* PlayerWp = GET_PLAYERDATAINS()->GetLocalEquipmentDataHandle();
	ULocalEquipmentDataSaveGame* SaveData = Cast<ULocalEquipmentDataSaveGame>(
		PlayerWp->Execute_GetData(PlayerWp)
	);
	FPlayerWeaponEquipData* FindEquip = SaveData->bEuipWeaponItems.Find(this->GemWeaponBox->GetItemID());
	if (FindEquip)
	{
		//查询槽位
		FPlayerWeaponEquipGemSlotDataBlock* TargetDatr =
			FindEquip->Datas.Find(this->GemWeaponBox->GetItemViewBlock().BlockIndex);
		if (TargetDatr)
		{

			UUI_SynModel_GemUpgradeWeaponAttachBox* Lists[4] = {
			this->BP_Syn_GemBox1,this->BP_Syn_GemBox2,
			this->BP_Syn_GemBox3,this->BP_Syn_GemBox4,
			};

			//初始化已经开槽的位置
			for (auto Ptr = TargetDatr->Slot.CreateIterator(); Ptr; ++Ptr)
			{
				//宝石当前槽位已经被开启过，但是未嵌入宝石
				if (Ptr->GemID == -1 || Ptr->GemLevel == -1)
				{
					//取消对此槽位的选择
					Lists[Ptr.GetIndex()]->Cancel();
				}
				else {
					//设置宝石数据
					FItemViewBlock NewBlock = { Ptr->GemID,Ptr->GemLevel,0 };
					Lists[Ptr.GetIndex()]->SetItemAttribute(
						GameDataStaticObject<UPlayerBagEquipView>(),
						NewBlock, 0
					);
					Lists[Ptr.GetIndex()]->ShowGemSlot();

					if (!IsValid(this->CurSelectGemSlot))
					{
						this->SelectGemSlot(Lists[Ptr.GetIndex()]);
					}
				}
			}
		}
	}
}

void USynModel_GemsUpGrade::SelectGemSlot(UUI_SynModel_GemUpgradeWeaponAttachBox* GemSlot) {

	if (this->CurSelectGemSlot == Cast<UObject>(GemSlot))
	{
		return;
	}

	if (IsValid(this->CurSelectGemSlot))
	{
		Cast<UUI_SynModel_GemUpgradeWeaponAttachBox>(this->CurSelectGemSlot)->ShowGemSlot();
		this->CurSelectGemSlot = nullptr;
	}
	this->CurSelectGemSlot = GemSlot;
	GemSlot->SelectGemSlot();

	this->AddGem();
}

void USynModel_GemsUpGrade::SelectGemSlot(int32 MaxLevel, UItemGridBase* Gem) {
	this->WeaponGemPanel->SetVisibility(ESlateVisibility::Collapsed);
	this->SingleGemPanel->SetVisibility(ESlateVisibility::Visible);

	this->GemBox->SetItemAttributeCopy(Gem);
	this->CurSelectGemSlot = nullptr;

	this->CurSelectGemLevel = this->GemBox->GetItemViewBlock().ItemLevel;
	this->CurSelectGemLevelMax = MaxLevel;

	FPlayerWeaponEquipGemDataBlock Blcok;
	Blcok.GemID = this->GemBox->GetItemID();
	Blcok.GemLevel = this->CurSelectGemLevel;

	this->AddCrystal(Blcok);
}

int32 USynModel_GemsUpGrade::AddGem() {
	//查询武器中的宝石
	ULocalEquipmentDataHandle* PlayerWp = GET_PLAYERDATAINS()->GetLocalEquipmentDataHandle();
	ULocalEquipmentDataSaveGame* SaveData = Cast<ULocalEquipmentDataSaveGame>(
		PlayerWp->Execute_GetData(PlayerWp)
	);
	FPlayerWeaponEquipData* FindEquip = SaveData->bEuipWeaponItems.Find(this->GemWeaponBox->GetItemID());
	if (FindEquip)
	{
		FPlayerWeaponEquipGemSlotDataBlock* TargetDatr =
			FindEquip->Datas.Find(this->GemWeaponBox->GetItemViewBlock().BlockIndex);
		if (TargetDatr)
		{
			FPlayerWeaponEquipGemDataBlock& GemData =
				TargetDatr->Slot[Cast<UUI_SynModel_GemUpgradeWeaponAttachBox>(
					this->CurSelectGemSlot
				)->GetGemSlotIndex()];

			//拿到ID查询宝石细节数据
			EEquipment Type;
			FEquipmentBase* BasePtr = nullptr;
			if (GET_CACHE(Equip)->GetEquipData(GemData.GemID, Type, BasePtr))
			{
				FWeaponGem* GemSource = CAST_TYPE_PTR(FWeaponGem, BasePtr);

				//当前宝石的等级
				this->CurSelectGemLevel = GemData.GemLevel;
				this->CurSelectGemLevelMax = GemSource->M_GemLevelMax;

				//添加强化水晶
				this->AddCrystal(GemData);

				return GemData.GemID;
			}
		}
	}

	return -1;
}

int32 USynModel_GemsUpGrade::EditGemLevel(int32 PlusNum) {

	if (this->GemBox->GetItemViewBlock().IsValid() && this->CurSelectGemSlot == nullptr)
	{
		int32 ID = this->GemBox->GetItemViewBlock().ItemID;
		// 8
		int32 Remove = this->GemBox->GetItemViewBlock().ItemLevel;

		// 8 + -1 = 7
		int32 Added = Remove + PlusNum;

		//删除旧的宝石，发送新的宝石
		GET_PLAYERDATAINS()->RemoveEquipItem(ID, Remove, -1, 1, 6);

		//发送新的宝石
		UGameSystemFunction::SendItemToBag(
			ID, Added, 1, 1, false, true
		);


		return ID;
	}
	else {
		//查询武器中的宝石
		ULocalEquipmentDataHandle* PlayerWp = GET_PLAYERDATAINS()->GetLocalEquipmentDataHandle();
		ULocalEquipmentDataSaveGame* SaveData = Cast<ULocalEquipmentDataSaveGame>(
			PlayerWp->Execute_GetData(PlayerWp)
		);
		FPlayerWeaponEquipData* FindEquip = SaveData->bEuipWeaponItems.Find(this->GemWeaponBox->GetItemID());
		if (FindEquip)
		{
			FPlayerWeaponEquipGemSlotDataBlock* TargetDatr =
				FindEquip->Datas.Find(this->GemWeaponBox->GetItemViewBlock().BlockIndex);
			if (TargetDatr)
			{
				FPlayerWeaponEquipGemDataBlock& GemData =
					TargetDatr->Slot[Cast<UUI_SynModel_GemUpgradeWeaponAttachBox>(
						this->CurSelectGemSlot
					)->GetGemSlotIndex()];

				GemData.GemLevel = GemData.GemLevel + PlusNum;

				PlayerWp->Execute_SaveData(PlayerWp, TEXT("宝石强化-宝石等级修改"));

				return GemData.GemID;
			}
		}
	}

	return -1;
}

void USynModel_GemsUpGrade::AddCrystal(FPlayerWeaponEquipGemDataBlock& GemData) {

	this->UpgradeGem_Butt->SetIsEnabled(true);

	auto SetCrystalImage = [&](int32 CrystialIndex) {
		this->CurCrystialID = this->Crystials[CrystialIndex];
		//设置水晶的图标
		this->UpgradeCrystalBox->SetItemAttribute(
			GameDataStaticObject<UPlayerBagMaterialView>(), {
			this->CurCrystialID,0,0
			}, 0
		);

		//从背包中查询水晶数量是否足够
		this->bUpGem = GET_PLAYERDATAINS()->FindMaterialCount(
			this->CurCrystialID, 0, this->NeedCrystalNums[GemData.GemLevel]);

		if (this->bUpGem)
		{
			this->UpgradeCrystalBox->SetButtonColorIsDark(false);
		}
		else {
			this->UpgradeCrystalBox->SetButtonColorIsDark();
		}
		};

	if (GemData.GemLevel < this->CurSelectGemLevelMax)
	{
		//等级小于11 使用普通水晶
		if (GemData.GemLevel < 11)
		{
			SetCrystalImage(0);
		}
		else {
			SetCrystalImage(1);
		}

		//设置金币
		this->UpgradeGemCoinText->SetText(
			FText::FromString(FString::FromInt(this->NeedCoin[GemData.GemLevel]))
		);
		//设置保险金
		this->Syn->GetSynInsurancePanel()->SetSynInsurancePrice(this->NeedInsuranceNums[GemData.GemLevel]);
		//设置概率
		float TargetRate = this->UpRate * this->GemsValue[this->CurSelectGemLevel];
		if (TargetRate > 100.f)
		{
			TargetRate = 100.f;
		}
		this->RateText->SetText(FText::FromString(
			FString::Printf(TEXT("%.2f"),
				TargetRate) + TEXT("%")
		));
		//加载水晶材料等
		this->LoadCloverAndScrystal();
		//设置水晶显示数量
		this->UpgradeCrystalBox->UpdateCount(this->NeedCrystalNums[GemData.GemLevel]);
	}
	else {
		this->UpgradeGem_Butt->SetIsEnabled(false);
	}
}

void USynModel_GemsUpGrade::SetRate(UItemGridBase* CopyCloverGrid, float Rate) {
	if (IsValid(CopyCloverGrid))
	{
		this->UpGemCloverBox->SetItemAttributeCopy(CopyCloverGrid);
	}
	else {
		this->UpGemCloverBox->Cancel();
		this->LoadCloverAndScrystal();
	}

	this->UpRate = Rate;

	float TargetRate = this->UpRate * this->GemsValue[this->CurSelectGemLevel];
	if (TargetRate > 100.f)
	{
		TargetRate = 100.f;
	}


	this->RateText->SetText(FText::FromString(
		FString::Printf(TEXT("%.2f"), TargetRate) + TEXT("%")
	));
}

void USynModel_GemsUpGrade::UpgradeGemBut() {

	if (!this->bUpGem)
	{
		this->CreateNotUnlockedItemTipWidget(2, this->CurCrystialID, 0,
			FText::FromString(
				TEXT("水晶不足，需要【") +
				FString::FromInt(this->NeedCrystalNums[CurSelectGemLevel]) +
				TEXT("】个")
			)
		);
		return;
	}

	UPlayerStructManager* Player = GET_PLAYERDATAINS();


	if (!Player->CheckCoin(this->NeedCoin[this->CurSelectGemLevel], 0U))
	{
		UWidgetBase::CreateTipWidget(TEXT("金币不足，无法强化"));
		return;
	}

	bool bSelect = this->Syn->GetSynInsurancePanel()->GetSynInsuranceSelect();
	if (bSelect)
	{
		//支付保险金
		if (!this->Syn->GetSynInsurancePanel()->PaySynInsurance())
		{
			UWidgetBase::CreateTipWidget(TEXT("点券不足，无法强化"));
			return;
		}
	}

	//检测是否有四叶草
	if (this->UpGemCloverBox->GetItemViewBlock().IsValid())
	{
		//消耗结果
		bool bRe = Player->ConsumeMaterial(
			this->UpGemCloverBox->GetItemViewBlock().ItemID, 0, 1
		);

		if (bRe)
		{
			//如果四叶草消耗完毕了，则取消对四叶草的选择
			if (!Player->FindMaterialCount(
				this->UpGemCloverBox->GetItemViewBlock().ItemID, 0, 1
			))
			{
				this->UpGemCloverBox->Cancel();
			}
		}
	}

	float CurRate = UGameSystemFunction::GetRandomRangeFloat(0.001f, 100.f);
	float TargetRate = this->UpRate * this->GemsValue[this->CurSelectGemLevel];
	if (TargetRate > 100.f)
	{
		TargetRate = 100.f;
	}
	bool bUpResult = false;

	if (TargetRate > 0.f && TargetRate >= CurRate)
	{
		bUpResult = true;

		//宝石等级提升
		UWidgetBase::CreateTipWidget(TEXT("强化成功"));

		//消耗水晶
		Player->ConsumeMaterial(this->CurCrystialID, 0, this->NeedCrystalNums[this->CurSelectGemLevel]);

		//修改宝石等级
		int32 CurGemID = this->EditGemLevel(1);

		if (this->GemBox->GetItemViewBlock().IsValid() && this->CurSelectGemSlot == nullptr)
		{
			//更新宝石数据
			FItemViewBlock NewBlock = { CurGemID,this->CurSelectGemLevel + 1,0 };
			this->GemBox->SetItemAttribute(
				GameDataStaticObject<UPlayerBagEquipView>(),
				NewBlock, 0
			);
			this->LoadWeaponAndGem();
		}
		else {
			//更新宝石数据
			FItemViewBlock NewBlock = { CurGemID,this->CurSelectGemLevel + 1,0 };
			Cast<UUI_SynModel_GemUpgradeWeaponAttachBox>(this->CurSelectGemSlot)->SetItemAttribute(
				GameDataStaticObject<UPlayerBagEquipView>(),
				NewBlock, 0
			);
		}

		/*
			添加历史记录
		*/
		UPlayerRecord::AddGemUpGrade(
			GameDataStaticObject<UPlayerBagEquipView>()->GetItemName(CurGemID),
			this->CurSelectGemLevel + 1
		);
		/*
			调用任务系统-完成宝石强化任务
		*/
		UTaskSubsystem::GetTaskSubsystemStatic()->ExecuteTasks(this);
	}
	else {
		UWidgetBase::CreateTipWidget(TEXT("强化失败，不够好运"));
		//强化失败
		if (!bSelect)
		{
			//消耗水晶
			Player->ConsumeMaterial(this->CurCrystialID, 0, this->NeedCrystalNums[this->CurSelectGemLevel]);

			if (this->CurSelectGemLevel >= 6)
			{
				//修改宝石等级
				int32 CurGemID = this->EditGemLevel(-1);

				if (this->GemBox->GetItemViewBlock().IsValid() && this->CurSelectGemSlot == nullptr)
				{
					//更新宝石数据
					FItemViewBlock NewBlock = { CurGemID,this->CurSelectGemLevel - 1,0 };
					this->GemBox->SetItemAttribute(
						GameDataStaticObject<UPlayerBagEquipView>(),
						NewBlock, 0
					);
					this->LoadWeaponAndGem();
				}
				else {
					//更新宝石数据
					FItemViewBlock NewBlock = { CurGemID,this->CurSelectGemLevel - 1,0 };
					Cast<UUI_SynModel_GemUpgradeWeaponAttachBox>(this->CurSelectGemSlot)->SetItemAttribute(
						GameDataStaticObject<UPlayerBagEquipView>(),
						NewBlock, 0
					);
				}

			}
		}
	}

	UGameSystemFunction::SaveCurrentPlayerData(TEXT("宝石强化"));


	if (!this->GemBox->GetItemViewBlock().IsValid() && this->CurSelectGemSlot != nullptr)
	{
		//重新选择宝石
		this->AddGem();
	}
	else {
		//重新更新概率
		this->SelectGemSlot(this->CurSelectGemLevelMax, this->GemBox);
	}
}

void USynModel_GemsUpGrade::LoadWeaponAndGem() {

	this->Syn->LoadPublicEquips(
		{ 3U,4U,5U,6U }, {}, GET_UI_NAME(Synthesis, UpgradeGem)
	);
}

void USynModel_GemsUpGrade::LoadCloverAndScrystal() {

	if (this->UpGemCloverBox->GetItemViewBlock().IsValid())
	{
		TArray<FItemViewBlock> Items;
		Items.Emplace(this->UpGemCloverBox->GetItemViewBlock());
		if (this->bUpGem)
		{
			Items.Emplace(FItemViewBlock({ this->CurCrystialID,0,0 }));
		}
		this->Syn->LoadPublicMaterials(
			{ 4U,7U }, Items, GET_UI_NAME(Synthesis, UpgradeGem)
		);
	}
	else {
		TArray<FItemViewBlock> Items;
		if (this->bUpGem)
		{
			Items.Emplace(FItemViewBlock({ this->CurCrystialID,0,0 }));
		}
		this->Syn->LoadPublicMaterials(
			{ 4U,7U }, Items, GET_UI_NAME(Synthesis, UpgradeGem)
		);
	}
}
