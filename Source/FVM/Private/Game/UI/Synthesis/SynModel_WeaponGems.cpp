// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除
#include "Game/UI/Synthesis/SynModel_WeaponGems.h"
#include <Components/Button.h>
#include <Components/TextBlock.h>
#include "Data/EquipmentDataSturct.h"
#include "GameSystem/TaskSubsystem.h"
#include "Game/UI/Tips/UI_SelectTip.h"
#include "Game/UI/UI_PlayerSynthesis.h"
#include "GameSystem/PlayerStructManager.h"
#include "Game/UI/UI_PlayerBagMaterialGrid.h"
#include "Game/UI/Tips/UI_NotUnlockedItemTip.h"
#include "Data/Texture/TextureDataAssetCache.h"
#include "GameSystem/PlayerData/LocalEquipmentDataHandle.h"

FString UUI_SynModel_WeaponBox::ToString_Implementation() {
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

FString UUI_SynModel_GemAttachmentBox::ToString_Implementation() {
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

void UUI_SynModel_WeaponBox::OnClicked_Implementation() {

	this->Cancel();

	FItemViewBlock View;
	FPlayerWeaponBase BaseData;
	Cast<UUI_SynModel_WeaponGem>(this->GetParentClass())->SelectWeapon(0U, View, BaseData);

}

bool UUI_SynModel_GemAttachmentBox::SetItemViewBlock(const FItemViewBlock* ItemViewBlock, UTexture2D* ItemHeadTexture, int32 Index) {
	if (Super::SetItemViewBlock(ItemViewBlock, ItemHeadTexture, Index)) {
		this->bLockState = true;
		this->SetButtonImageMatchSize();
		this->ShowGemSlot();
		return true;
	}
	return false;
}

void UUI_SynModel_GemAttachmentBox::OnClicked_Implementation() {
	if (this->bLockState)
	{
		uint8 Type;
		FName Tag = GET_DEF_CATEGORYNAME(Equip);
		FItemBaseStructData* BasePtr = nullptr;
		if (GET_ITEM(this->GetItemViewBlock().ItemID, Type, BasePtr, Tag))
		{
			this->SetFuncObject(NewObject<UItemBaseGridFunc>(), BasePtr);
		}
	}
	else {
		//取消对当前宝石的选择
		FItemViewBlock Block;
		this->SetItemViewBlock(&Block, nullptr, 0);
		this->bLockState = true;
		this->EmptyGemSlot();

		//关闭镶嵌按钮
		Cast<UUI_SynModel_WeaponGem>(this->GetParentClass())->GetAttachGemButt()->SetIsEnabled(false);
	}
}

void UUI_SynModel_GemAttachmentBox::UnLockSlot() {
	this->bLockState = false;
}

void UUI_SynModel_GemAttachmentBox::LockSlot() {
	this->bLockState = true;
	if (this->GetItemViewBlock().IsValid())
	{
		this->SetButtonColorIsDark();
	}
}

void UUI_SynModel_GemAttachmentBox::BindItemID(int32 ItemID, int32 SlotIndex) {
	this->mItemID = ItemID;
	this->mSlot = SlotIndex;
}

void UUI_SynModel_GemAttachmentBox::UnInstallGem() {

	//查询角色背包是否有拆卸工具
	if (GET_PLAYERDATAINS()->FindMaterialCount(875, 0, 1))
	{
		//创建提示窗口
		UUI_SelectTip* Select = this->CreateSelectTipWidget(
			FText::FromString(TEXT("是否使用【拆卸改锥】进行宝石的拆卸？\r\n拆卸后宝石将直接返还至背包！"))
		);

		FScriptDelegate Ok, Cancel;
		Ok.BindUFunction(this, TEXT("SureUnInstallGemEventFunc"));
		Cancel.BindUFunction(this, TEXT("CancelUnInstallGemEventFunc"));
		Select->BindOk(MoveTempIfPossible(Ok));
		Select->BindCancel(MoveTempIfPossible(Cancel));
	}
	else {
		//创建提示窗口
		UUI_NotUnlockedItemTip* UnlockTip = this->CreateNotUnlockedItemTipWidget(
			2, 875, 0,
			FText::FromString(TEXT("使用【拆卸改锥】可以将宝石返还至背包！\r\n当前还未获得【拆卸改锥】！"))
		);
		FScriptDelegate Ok;
		Ok.BindUFunction(this, TEXT("CancelUnInstallGemEventFunc"));
		UnlockTip->BindOk(Ok);
	}
}

void UUI_SynModel_GemAttachmentBox::SureUnInstallGemEventFunc() {

	//消耗拆卸改锥
	GET_PLAYERDATAINS()->ConsumeMaterial(875, 0, 1);

	//直接摧毁宝石
	UUI_SynModel_WeaponBox* WBox = Cast<UUI_SynModel_WeaponGem>(this->GetParentClass())->GetWeaponBox();
	ULocalEquipmentDataHandle* PlayerWp = GET_PLAYERDATAINS()->GetLocalEquipmentDataHandle();
	ULocalEquipmentDataSaveGame* SaveData = Cast<ULocalEquipmentDataSaveGame>(
		PlayerWp->Execute_GetData(PlayerWp)
	);
	FPlayerWeaponEquipData* FindEquip = SaveData->bEuipWeaponItems.Find(WBox->GetItemID());
	if (FindEquip)
	{
		FPlayerWeaponEquipGemSlotDataBlock* TargetDatr =
			FindEquip->Datas.Find(WBox->GetItemViewBlock().BlockIndex);
		if (TargetDatr)
		{
			//返还宝石->发送宝石到背包
			UGameSystemFunction::SendItemToBag(
				TargetDatr->Slot[this->mSlot].GemID,
				TargetDatr->Slot[this->mSlot].GemLevel,
				1, 1U, false, true);

			TargetDatr->Slot[this->mSlot].GemID = -1;
			TargetDatr->Slot[this->mSlot].GemLevel = -1;
			PlayerWp->Execute_SaveData(PlayerWp, TEXT("宝石拆卸"));
		}
	}

	UGameSystemFunction::SaveCurrentPlayerData(TEXT("武器开槽-宝石拆卸返还至背包"));

	FItemViewBlock Block;
	this->SetItemViewBlock(&Block, nullptr, 0);
	this->bLockState = true;
	this->EmptyGemSlot();

	Cast<UUI_SynModel_WeaponGem>(this->GetParentClass())->LoadWeaponsAndGems();
}

void UUI_SynModel_GemAttachmentBox::CancelUnInstallGemEventFunc() {
	//创建提示窗口
	UUI_SelectTip* Select = this->CreateSelectTipWidget(
		FText::FromString(TEXT("直接拆卸宝石将不会返还至背包！确定要进行拆卸吗？"))
	);

	FScriptDelegate Ok;
	Ok.BindUFunction(this, TEXT("ForceUnInstallGemEventFunc"));
	Select->BindOk(MoveTempIfPossible(Ok));
}

void UUI_SynModel_GemAttachmentBox::ForceUnInstallGemEventFunc() {
	//直接摧毁宝石
	UUI_SynModel_WeaponBox* WBox = Cast<UUI_SynModel_WeaponGem>(this->GetParentClass())->GetWeaponBox();
	ULocalEquipmentDataHandle* PlayerWp = GET_PLAYERDATAINS()->GetLocalEquipmentDataHandle();
	ULocalEquipmentDataSaveGame* SaveData = Cast<ULocalEquipmentDataSaveGame>(
		PlayerWp->Execute_GetData(PlayerWp)
	);
	FPlayerWeaponEquipData* FindEquip = SaveData->bEuipWeaponItems.Find(WBox->GetItemID());
	if (FindEquip)
	{
		FPlayerWeaponEquipGemSlotDataBlock* TargetDatr =
			FindEquip->Datas.Find(WBox->GetItemViewBlock().BlockIndex);
		if (TargetDatr)
		{
			TargetDatr->Slot[this->mSlot].GemID = -1;
			TargetDatr->Slot[this->mSlot].GemLevel = -1;
			PlayerWp->Execute_SaveData(PlayerWp, TEXT("宝石直接摧毁"));
		}
	}

	UGameSystemFunction::SaveCurrentPlayerData(TEXT("武器开槽-宝石直接摧毁"));

	FItemViewBlock Block;
	this->SetItemViewBlock(&Block, nullptr, 0);
	this->bLockState = true;
	this->EmptyGemSlot();

	Cast<UUI_SynModel_WeaponGem>(this->GetParentClass())->LoadWeaponsAndGems();
}

void UUI_SynModel_GemAttachmentBox::OpenSlot() {

	//消耗材料
	GET_PLAYERDATAINS()->ConsumeMaterial(this->mItemID, 0, 1);

	//对武器进行开槽处理
	UUI_SynModel_WeaponBox* WBox = Cast<UUI_SynModel_WeaponGem>(this->GetParentClass())->GetWeaponBox();
	ULocalEquipmentDataHandle* PlayerWp = GET_PLAYERDATAINS()->GetLocalEquipmentDataHandle();
	ULocalEquipmentDataSaveGame* SaveData = Cast<ULocalEquipmentDataSaveGame>(
		PlayerWp->Execute_GetData(PlayerWp)
	);
	FPlayerWeaponEquipData* FindEquip = SaveData->bEuipWeaponItems.Find(WBox->GetItemID());
	if (FindEquip)
	{
		FPlayerWeaponEquipGemSlotDataBlock* TargetDatr =
			FindEquip->Datas.Find(WBox->GetItemViewBlock().BlockIndex);
		if (TargetDatr)
		{
			FPlayerWeaponEquipGemDataBlock NewSlot;
			TargetDatr->Slot.Emplace(NewSlot);
			PlayerWp->Execute_SaveData(PlayerWp, TEXT("武器开槽"));
		}
	}

	UGameSystemFunction::SaveCurrentPlayerData(TEXT("武器开槽-材料消耗"));

	FPlayerWeaponBase BaseData;
	BaseData.M_WeaponGemCountMax = Cast<UUI_SynModel_WeaponGem>(
		this->GetParentClass()
	)->GetWeaponGemCountMaxCache();
	Cast<UUI_SynModel_WeaponGem>(this->GetParentClass())->InitWeapon(BaseData);

	Cast<UUI_SynModel_WeaponGem>(this->GetParentClass())->LoadItem();

	UWidgetBase::CreateTipWidget(TEXT("开槽成功！"));
}

bool UUI_SynModel_GemAttachmentBox::CheckSlotCondition() {

	if (GET_PLAYERDATAINS()->FindMaterialCount(this->mItemID, 0, 1))
	{
		return true;
	}

	UWidgetBase::CreateNotUnlockedItemTipWidget(
		2, this->mItemID, 0,
		FText::FromString(TEXT("无法开槽，尚未获得") +
			GameDataStaticObject<
			UPlayerBagMaterialView
			>()->GetItemName(this->mItemID))
	);

	return false;
}

void UUI_SynModel_WeaponGem::InitWidget_Implementation() {

	this->Syn = Cast<UUI_PlayerSynthesis>(this->GetParentClass());

	this->BP_Syn_WeaponBox->SetParentClass(this);

	this->BP_Syn_GemBox1->SetParentClass(this);
	this->BP_Syn_GemBox1->BindItemID(871, 0);
	this->BP_Syn_GemBox2->SetParentClass(this);
	this->BP_Syn_GemBox2->BindItemID(872, 1);
	this->BP_Syn_GemBox3->SetParentClass(this);
	this->BP_Syn_GemBox3->BindItemID(873, 2);
	this->BP_Syn_GemBox4->SetParentClass(this);
	this->BP_Syn_GemBox4->BindItemID(874, 3);

	//取消操作
	FItemViewBlock TempBock;
	FPlayerWeaponBase TempBockWeaponBase;
	this->SelectWeapon(0U, TempBock, TempBockWeaponBase);

	//显示界面
	this->Syn->SetCardAndGemsPanelShow(this, ESlateVisibility::Visible, 1);
}

void UUI_SynModel_WeaponGem::SelectWeapon(uint8 WeaponType,
	const FItemViewBlock& Block,
	const FPlayerWeaponBase& BlockWeaponBase) {

	this->AttachGem_Butt->SetIsEnabled(false);
	this->AttachGemCoinText->SetText(FText::FromString("0"));

	if (Block.ItemID == -1)
	{
		this->BP_Syn_WeaponBox->Cancel();
		this->BP_Syn_GemBox1->Cancel();
		this->BP_Syn_GemBox2->Cancel();
		this->BP_Syn_GemBox3->Cancel();
		this->BP_Syn_GemBox4->Cancel();
		this->LoadItem();
		this->LoadWeaponsAndGems();
	}
	else {
		this->CurWeaponType = WeaponType;
		this->BP_Syn_WeaponBox->SetItemAttribute(GameDataStaticObject<UPlayerBagEquipView>(), Block, 0);
		this->InitWeapon(BlockWeaponBase);
		this->LoadWeaponsAndGems();
	}
}

void UUI_SynModel_WeaponGem::InitWeapon(const FPlayerWeaponBase& BlockWeaponBase) {
	this->CurWeaponGemCountMax = BlockWeaponBase.M_WeaponGemCountMax;
	ULocalEquipmentDataHandle* PlayerWp = GET_PLAYERDATAINS()->GetLocalEquipmentDataHandle();
	ULocalEquipmentDataSaveGame* SaveData = Cast<ULocalEquipmentDataSaveGame>(
		PlayerWp->Execute_GetData(PlayerWp)
	);
	FPlayerWeaponEquipData* FindEquip = SaveData->bEuipWeaponItems.Find(this->BP_Syn_WeaponBox->GetItemID());
	if (FindEquip)
	{
		//查询槽位
		FPlayerWeaponEquipGemSlotDataBlock* TargetDatr =
			FindEquip->Datas.Find(this->BP_Syn_WeaponBox->GetItemViewBlock().BlockIndex);
		if (TargetDatr)
		{

			//对宝石格子重新进行取消操作，防止上一次的数据没有被清理
			this->BP_Syn_GemBox1->Cancel(); this->BP_Syn_GemBox2->Cancel();
			this->BP_Syn_GemBox3->Cancel(); this->BP_Syn_GemBox4->Cancel();

			UUI_SynModel_GemAttachmentBox* Lists[4] = {
			this->BP_Syn_GemBox1,this->BP_Syn_GemBox2,
			this->BP_Syn_GemBox3,this->BP_Syn_GemBox4,
			};

			//初始化已经开槽的位置
			for (auto Ptr = TargetDatr->Slot.CreateIterator(); Ptr; ++Ptr)
			{
				//关闭开槽功能
				Lists[Ptr.GetIndex()]->OpenSlotEvent(false);

				//宝石当前槽位已经被开启过，但是未嵌入宝石
				if (Ptr->GemID == -1 || Ptr->GemLevel == -1)
				{
					//让UI显示空槽位
					Lists[Ptr.GetIndex()]->EmptyGemSlot();
				}
				else {
					//设置宝石数据
					FItemViewBlock NewBlock = { Ptr->GemID,Ptr->GemLevel,0 };
					Lists[Ptr.GetIndex()]->SetItemAttribute(
						GameDataStaticObject<UPlayerBagEquipView>(), NewBlock, 0
					);
					Lists[Ptr.GetIndex()]->AttachmentGem();
					Lists[Ptr.GetIndex()]->LockSlot();
				}
			}

			//初始化未开槽的位置
			if (TargetDatr->Slot.Num() == 0)
			{
				//如果从未开槽的武器，打开第一个位置的开槽功能
				Lists[0]->OpenSlotEvent(true);
			}
			else if (TargetDatr->Slot.Num() < this->CurWeaponGemCountMax)
			{
				//如果槽位不满足,最大槽位则对下一个槽位进行开启
				int32 Index = TargetDatr->Slot.Num();
				if (Index < this->CurWeaponGemCountMax && Index < 4)
				{
					Lists[Index]->OpenSlotEvent(true);
				}
			}
		}
	}
}

bool UUI_SynModel_WeaponGem::CheckTheGemMatchTheWeapon(int32 ConstWeaponID, uint8 GemType) {

	if (ConstWeaponID != -1)
	{
		//表示必须匹配当前武器才能通过
		if (this->BP_Syn_WeaponBox->GetItemViewBlock().ItemID == ConstWeaponID)
		{
			return true;
		}

		UWidgetBase::CreateNotUnlockedItemTipWidget(
			0, ConstWeaponID, 0,
			FText::FromString(
				TEXT("当前宝石是【") +
				GameDataStaticObject<
				UPlayerBagEquipView
				>()->GetItemName(ConstWeaponID) +
				TEXT("】武器的专属宝石！")
			)
		);

		return false;
	}


	switch (GemType)
	{
		//主武器宝石
	case 0U: {
		if (this->CurWeaponType != 3U)
		{
			UWidgetBase::CreateNotUnlockedItemTipWidget(
				0, this->BP_Syn_WeaponBox->GetItemViewBlock().ItemID, 0,
				FText::FromString(TEXT("当前宝石只能加在主武器上面！\r\n当前武器不兼容！"))
			);
		}
		else { return true; }
	}break;
		   //副武器宝石
	case 1U: {
		if (this->CurWeaponType != 4U)
		{
			UWidgetBase::CreateNotUnlockedItemTipWidget(
				0, this->BP_Syn_WeaponBox->GetItemViewBlock().ItemID, 0,
				FText::FromString(TEXT("当前宝石只能加在副武器上面！\r\n当前武器不兼容！"))
			);
		}
		else { return true; }
	}break;
		   //超级武器宝石
	case 2U: {
		if (this->CurWeaponType != 5U)
		{
			UWidgetBase::CreateNotUnlockedItemTipWidget(
				0, this->BP_Syn_WeaponBox->GetItemViewBlock().ItemID, 0,
				FText::FromString(TEXT("当前宝石只能加在超级武器上面！\r\n当前武器不兼容！"))
			);
		}
		else { return true; }
	}break;

		   //通用宝石，任意武器都可以镶嵌
	case 3U: return true;
	}

	return false;
}

void UUI_SynModel_WeaponGem::SelectGem(int32 ID, int32 Level) {

	UUI_SynModel_GemAttachmentBox* Lists[4] = {
			this->BP_Syn_GemBox1,this->BP_Syn_GemBox2,
			this->BP_Syn_GemBox3,this->BP_Syn_GemBox4,
	};

	ULocalEquipmentDataHandle* PlayerWp = GET_PLAYERDATAINS()->GetLocalEquipmentDataHandle();
	ULocalEquipmentDataSaveGame* SaveData = Cast<ULocalEquipmentDataSaveGame>(
		PlayerWp->Execute_GetData(PlayerWp)
	);
	FPlayerWeaponEquipData* FindEquip = SaveData->bEuipWeaponItems.Find(
		this->BP_Syn_WeaponBox->GetItemID());
	if (FindEquip)
	{
		//查询槽位
		FPlayerWeaponEquipGemSlotDataBlock* TargetDatr =
			FindEquip->Datas.Find(this->BP_Syn_WeaponBox->GetItemViewBlock().BlockIndex);
		if (TargetDatr)
		{
			int32 SlotMax = TargetDatr->Slot.Num() > 3 ? 4 : TargetDatr->Slot.Num();
			int32 SelectSlot = -1;
			for (int32 SlotIndex = 0; SlotIndex < SlotMax; ++SlotIndex)
			{
				//宝石当前槽位已经被开启过，但是未嵌入宝石
				if (TargetDatr->Slot[SlotIndex].GemID == -1 || TargetDatr->Slot[SlotIndex].GemLevel == -1)
				{
					if (SelectSlot == -1)
					{
						SelectSlot = SlotIndex;
					}
				}
				else if (TargetDatr->Slot[SlotIndex].GemID == ID)
				{
					UWidgetBase::CreateNotUnlockedItemTipWidget(
						0, ID, TargetDatr->Slot[SlotIndex].GemLevel,
						FText::FromString(TEXT("宝石重复！\r\n检测到此武器已经镶嵌过此宝石！"))
					);
					return;
				}
			}

			if (SelectSlot != -1)
			{
				//添加到槽位
					//设置宝石数据
				FItemViewBlock NewBlock = { ID,Level,0 };
				Lists[SelectSlot]->SetItemAttribute(
					GameDataStaticObject<UPlayerBagEquipView>(), NewBlock, 0
				);
				//解锁槽位
				Lists[SelectSlot]->UnLockSlot();
				//设置槽位索引
				this->CurAttachSlotIndex = SelectSlot + 1;
				//解锁宝石镶嵌功能
				this->AttachGem_Butt->SetIsEnabled(true);
				this->AttachGemCoinText->SetText(
					FText::FromString(
						FString::FromInt(1000 * this->CurAttachSlotIndex)
					)
				);
			}
		}
	}
}

void UUI_SynModel_WeaponGem::InstallGem() {

	UPlayerStructManager* Player = GET_PLAYERDATAINS();

	//检测货币
	if (!Player->CheckCoin(1000 * this->CurAttachSlotIndex, 0U))
	{
		UWidgetBase::CreateTipWidget(TEXT("金币不足，无法镶嵌宝石！"));
		return;
	}

	//扣除金币
	Player->ReduceCoin(1000 * this->CurAttachSlotIndex, 0U);

	UUI_SynModel_GemAttachmentBox* Lists[4] = {
	this->BP_Syn_GemBox1,this->BP_Syn_GemBox2,
	this->BP_Syn_GemBox3,this->BP_Syn_GemBox4,
	};

	const FItemViewBlock& CurView = Lists[this->CurAttachSlotIndex - 1]->GetItemViewBlock();
	//删除宝石
	Player->RemoveEquipItem(CurView.ItemID, CurView.ItemLevel, -1, 1, 6U);

	//修改武器细节数据
	ULocalEquipmentDataHandle* PlayerWp = Player->GetLocalEquipmentDataHandle();
	ULocalEquipmentDataSaveGame* SaveData = Cast<ULocalEquipmentDataSaveGame>(
		PlayerWp->Execute_GetData(PlayerWp)
	);
	FPlayerWeaponEquipData* FindEquip = SaveData->bEuipWeaponItems.Find(this->BP_Syn_WeaponBox->GetItemID());
	if (FindEquip)
	{
		//查询槽位
		FPlayerWeaponEquipGemSlotDataBlock* TargetDatr =
			FindEquip->Datas.Find(this->BP_Syn_WeaponBox->GetItemViewBlock().BlockIndex);
		if (TargetDatr)
		{
			TargetDatr->Slot[this->CurAttachSlotIndex - 1].GemID = CurView.ItemID;
			TargetDatr->Slot[this->CurAttachSlotIndex - 1].GemLevel = CurView.ItemLevel;
			Lists[this->CurAttachSlotIndex - 1]->AttachmentGem();
			Lists[this->CurAttachSlotIndex - 1]->LockSlot();
			this->GetAttachGemButt()->SetIsEnabled(false);
			PlayerWp->Execute_SaveData(PlayerWp, TEXT("宝石镶嵌-修改武器数据"));
		}
	}

	//执行任务
	UPlayerRecord::AddGemAttachCount();
	UTaskSubsystem::GetTaskSubsystemStatic()->ExecuteTasks(this);

	UGameSystemFunction::SaveCurrentPlayerData(TEXT("宝石镶嵌"));

	//重新加载宝石数据
	this->LoadWeaponsAndGems();
}

void UUI_SynModel_WeaponGem::LoadItem() {
	this->Syn->LoadPublicMaterials(
		{ 8U }, {}, FName()
	);
}

void UUI_SynModel_WeaponGem::LoadWeaponsAndGems(bool bNotLoad) {

	//武器-宝石界面切换加载
	//如果武器没有选择，则只加载武器
	//如果选择了武器，则只加载宝石

	if (this->BP_Syn_WeaponBox->GetItemViewBlock().IsValid())
	{
		this->Syn->LoadPublicEquips(
			{ 6U },
			{ },
			GET_UI_NAME(Synthesis, WeaponAndGem), bNotLoad
		);
	}
	else {
		this->Syn->LoadPublicEquips(
			{ 3U,4U,5U },
			{ },
			GET_UI_NAME(Synthesis, WeaponAndGem), bNotLoad
		);
	}
}