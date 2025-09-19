// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Synthesis/SynModel_CardChange.h"
#include <Components/Image.h>
#include <Components/Button.h> 
#include <Components/TextBlock.h>
#include "GameSystem/TaskSubsystem.h"
#include "GameSystem/FVMGameInstance.h"
#include "Game/UI/UI_PlayerSynthesis.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/Tools/GameSystemFunction.h"

void UUI_SynModel_TransferCardBox::OnClicked_Implementation() {

	if (this->GetItemViewBlock().IsValid())
	{
		this->Cancel();

		Cast<UUI_SynModel_TransferCardMainPanel>(Cast<UUI_PlayerSynthesis>(
			this->GetParentClass()
		)->GetSynthesisOperaterWidget(GET_UI_NAME(Synthesis, TransferCard)))->CancelItem();
	}
}

void UUI_SynModel_TransferCardMainPanel::InitWidget_Implementation() {

	UUI_PlayerSynthesis* Syn = Cast<UUI_PlayerSynthesis>(this->GetParentClass());

	this->TransferMainCardBox->SetParentClass(this->GetParentClass());
	this->TransferMainCardBox->Cancel();
	this->CurSelectID = -1;

	FItemViewBlock View;
	FItemCountViewBlock Count;
	this->SelectItem(View, Count, -1);

	Syn->SetCardAndGemsPanelShow(
		this, ESlateVisibility::Visible, 0, ESlateVisibility::Visible
	);
}

void UUI_SynModel_TransferCardMainPanel::SelectItem(const FItemViewBlock& Block, const FItemCountViewBlock& Count, int32 Index) {

	if (Block.ItemID == -1)
	{
		this->CancelItem();
		return;
	}
	else {
		ECardType Type;
		FItemCard* BasePtr = nullptr;
		if (GET_CACHE(Card)->GetCardData(Block.ItemID, Type, BasePtr))
		{
			if (BasePtr->TransferData.CardTransferID == -1)
			{
				UWidgetBase::CreateTipWidget(
					FString::Printf(TEXT("%s当前无法进行转职"), *Count.Name,
						BasePtr->TransferData.CardTransferLevel)
				);
				this->TransferMainCardBox->OnClicked();
			}
			else {
				//如果满足条件，则可以选择
				if (Block.ItemLevel >= BasePtr->TransferData.CardTransferLevel)
				{
					this->TransferCard_Butt->SetIsEnabled(true);

					//选择
					this->TransferMainCardBox->SetItemAttribute(
						GameDataStaticObject<UPlayerBagCardView>(), Block, Index
					);

					this->TargetCardID = BasePtr->TransferData.CardTransferID;

					int32 Level = this->TransferMainCardBox->GetItemViewBlock().ItemLevel;
					//设置保险金金额
					Cast<UUI_PlayerSynthesis>(
						this->GetParentClass()
					)->GetSynInsurancePanel()->SetSynInsurancePrice(
						this->LInsuranceCoinNum[Level]
					);
					//设置金币
					this->Coin = this->LCoinNum[Level];
					this->TransferCardCoinText->SetText(FText::FromString(FString::FromInt(this->Coin)));
					//设置概率
					this->Rate = 30.f;
					this->TransferCardRateText->SetText(FText::FromString(TEXT("30%")));

					//材料视图
					UPlayerBagMaterialView* MaterView = GameDataStaticObject<UPlayerBagMaterialView>();
					if (BasePtr->TransferData.CardTransferMaterials.Num() >= 2)
					{
						TArray<int32> LIndex = BasePtr->TransferData.CardTransferMaterials.Array();

						this->CurSelectTransferMaters.Empty(0);
						for (auto& Ids : BasePtr->TransferData.CardTransferMaterials)
						{
							this->CurSelectTransferMaters.Emplace(
								FItemViewBlock({ Ids,0,0 })
							);
						}

						FItemViewBlock Block0 = { LIndex[0] , 0 , 0 };
						//选择材料
						this->TransferMaterialBox_A->SetItemAttribute(
							MaterView,
							Block0,
							0
						);

						if (CAST_TYPE(FItemCountViewBlock, MaterView->GetItem(Block0.ItemID, 0)).Count == 0)
						{
							this->TransferMaterialBox_A->SetButtonColorIsDark();
							this->TransferCard_Butt->SetIsEnabled(false);
						}
						else {
							this->TransferMaterialBox_A->SetButtonColorIsDark(false);
						}


						FItemViewBlock Block1 = { LIndex[1] , 0 , 0 };
						this->TransferMaterialBox_B->SetItemAttribute(
							MaterView,
							Block1,
							0
						);

						if (CAST_TYPE(FItemCountViewBlock, MaterView->GetItem(Block1.ItemID, 0)).Count == 0)
						{
							this->TransferMaterialBox_B->SetButtonColorIsDark();
							this->TransferCard_Butt->SetIsEnabled(false);
						}
						else {
							this->TransferMaterialBox_B->SetButtonColorIsDark(false);
						}

						if (BasePtr->TransferData.CardTransferMaterials.Num() == 3)
						{
							FItemViewBlock Block2 = { LIndex[2] , 0 , 0 };
							this->TransferMaterialBox_C->SetItemAttribute(
								MaterView,
								Block2,
								0
							);

							if (CAST_TYPE(FItemCountViewBlock, MaterView->GetItem(Block2.ItemID, 0)).Count == 0)
							{
								this->TransferMaterialBox_C->SetButtonColorIsDark();
								this->TransferCard_Butt->SetIsEnabled(false);
							}
							else {
								this->TransferMaterialBox_C->SetButtonColorIsDark(false);
							}

							//设置概率
							this->Rate = 20.f;
							this->TransferCardRateText->SetText(FText::FromString(TEXT("20%")));

							this->TransferMaterialBox_C->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						}
						else {
							this->TransferMaterialBox_C->SetVisibility(ESlateVisibility::Hidden);
						}
					}
				}
				else {
					UWidgetBase::CreateTipWidget(
						FString::Printf(TEXT("%s需要达到%d星"), *Count.Name,
							BasePtr->TransferData.CardTransferLevel)
					);
					this->TransferMainCardBox->OnClicked();
				}
			}

		}
	}

	this->CurSelectID = Block.ItemID;
	this->CurSelectLevel = Block.ItemLevel;
	this->LoadItem();
}

void UUI_SynModel_TransferCardMainPanel::CancelItem() {

	this->TransferCard_Butt->SetIsEnabled(false);
	this->TransferCardCoinText->SetText(FText::FromString(TEXT("0")));
	this->TransferCardRateText->SetText(FText::FromString(TEXT("0%")));
	this->TransferMaterialBox_A->Cancel();
	this->TransferMaterialBox_B->Cancel();
	this->TransferMaterialBox_C->Cancel();
	this->TransferMaterialBox_C->SetVisibility(ESlateVisibility::Hidden);

	this->TransferMaterialBox_A->SetButtonColorIsDark(false);
	this->TransferMaterialBox_B->SetButtonColorIsDark(false);
	this->TransferMaterialBox_C->SetButtonColorIsDark(false);
	this->CurSelectID = -1;
	this->CurSelectLevel = -1;
	this->CurSelectTransferMaters.Empty(0);

	this->LoadItem();
}

void UUI_SynModel_TransferCardMainPanel::LoadItem() {

	Cast<UUI_PlayerSynthesis>(
		this->GetParentClass()
	)->LoadPublicCards({ {this->CurSelectID,this->CurSelectLevel,0} }, 1);

	Cast<UUI_PlayerSynthesis>(this->GetParentClass())->LoadPublicMaterials(
		{ (uint8)EMaterialType::E_CardChangeJobMaterial },
		this->CurSelectTransferMaters,
		GET_UI_NAME(Synthesis, TransferCard)
	);
}

void UUI_SynModel_TransferCardMainPanel::Transfer() {

	if (!GET_PLAYERDATAINS()->CheckCoin(this->Coin, 0U))
	{
		UWidgetBase::CreateTipWidget(TEXT("金币不足，无法转职"));
		return;
	}

	bool bSelect = Cast<UUI_PlayerSynthesis>(
		this->GetParentClass()
	)->GetSynInsurancePanel()->GetSynInsuranceSelect();
	if (bSelect)
	{
		//支付保险金
		if (!Cast<UUI_PlayerSynthesis>(
			this->GetParentClass()
		)->GetSynInsurancePanel()->PaySynInsurance())
		{
			UWidgetBase::CreateTipWidget(TEXT("点券不足，无法转职"));
			return;
		}
	}

	float CurRate = UGameSystemFunction::GetRandomRangeFloat(0.001f, 100.f);
	bool bReSelect = true;
	if (this->Rate > 0.f && this->Rate >= CurRate)
	{
		//发送卡片
		UGameSystemFunction::SendItemToBag(
			this->TargetCardID, this->TransferMainCardBox->GetItemViewBlock().ItemLevel,
			1, 2U, false, true
		);
		//消耗卡片
		GET_PLAYERDATAINS()->ConsumeCard(
			this->TransferMainCardBox->GetItemViewBlock().ItemID,
			this->TransferMainCardBox->GetItemViewBlock().ItemLevel,
			1
		);
		//消耗材料
		if (this->TransferMaterialBox_A->GetItemViewBlock().IsValid())
		{
			//消耗材料
			GET_PLAYERDATAINS()->ConsumeMaterial(
				this->TransferMaterialBox_A->GetItemID(), 0
			);
		}
		if (this->TransferMaterialBox_B->GetItemViewBlock().IsValid())
		{
			//消耗材料
			GET_PLAYERDATAINS()->ConsumeMaterial(
				this->TransferMaterialBox_B->GetItemID(), 0
			);
		}
		if (this->TransferMaterialBox_C->GetItemViewBlock().IsValid())
		{
			//消耗材料
			GET_PLAYERDATAINS()->ConsumeMaterial(
				this->TransferMaterialBox_C->GetItemID(), 0
			);
		}

		//转职
		UWidgetBase::CreateTipWidget(TEXT("转职成功"));

		/*
			添加历史记录
		*/
		UPlayerRecord::AddCardTransferCount(1);
		/*
			调用任务系统-完成任务
		*/
		UTaskSubsystem::GetTaskSubsystemStatic()->ExecuteTasks(this);

		//对主卡进行取消选择
		this->TransferMainCardBox->OnClicked();

		//重新加载卡片
		this->LoadItem();
	}
	else {
		//选择了保险金
		if (bSelect)
		{
			//什么都不做
			bReSelect = false;

			//转职失败
			UWidgetBase::CreateTipWidget(TEXT("转职失败，不够好运！"));

			//支付货币
			GET_PLAYERDATAINS()->ReduceCoin(this->Coin, 0U);

			//存储
			UGameSystemFunction::SaveCurrentPlayerData(TEXT("卡片转职"));

			return;
		}
		else {
			if (this->TransferMaterialBox_A->GetItemViewBlock().IsValid())
			{
				//消耗材料
				GET_PLAYERDATAINS()->ConsumeMaterial(
					this->TransferMaterialBox_A->GetItemID(), 0
				);
			}
			if (this->TransferMaterialBox_B->GetItemViewBlock().IsValid())
			{
				//消耗材料
				GET_PLAYERDATAINS()->ConsumeMaterial(
					this->TransferMaterialBox_B->GetItemID(), 0
				);
			}
			if (this->TransferMaterialBox_C->GetItemViewBlock().IsValid())
			{
				//消耗材料
				GET_PLAYERDATAINS()->ConsumeMaterial(
					this->TransferMaterialBox_C->GetItemID(), 0
				);
			}

			//转职失败
			UWidgetBase::CreateTipWidget(TEXT("转职失败，不够好运！"));

			//重新选择主卡
			this->SelectItem(this->TransferMainCardBox->GetItemViewBlock(),
				this->TransferMainCardBox->GetItemCountViewBlock(),
				0);
			//重新加载卡片
			this->LoadItem();
		}
	}

	//支付货币
	GET_PLAYERDATAINS()->ReduceCoin(this->Coin, 0U);

	//存储
	UGameSystemFunction::SaveCurrentPlayerData(TEXT("卡片转职"));
}
