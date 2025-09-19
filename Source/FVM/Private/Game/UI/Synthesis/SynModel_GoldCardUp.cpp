// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/Synthesis/SynModel_GoldCardUp.h"
#include <Components/Button.h>
#include "GameSystem/TaskSubsystem.h"
#include "Game/UI/UI_PlayerSynthesis.h"
#include "Game/UI/UI_PlayerBagCardGrid.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/Item/Material/CardChangeJobMaterialStruct.h"

void USynModel_GoldCardUp::InitWidget_Implementation()
{
	this->Syn = Cast<UUI_PlayerSynthesis>(this->GetParentClass());

	this->CardMaterialBox1->SetParentClass(this);
	this->CardMaterialBox2->SetParentClass(this);
	this->EvolveMaterialBox->SetParentClass(this);
	this->CardBox->SetParentClass(this);

	this->Init();

	//显示界面
	this->Syn->SetCardAndGemsPanelShow(this, ESlateVisibility::Visible, 0);
}

void USynModel_GoldCardUp::Init() {

	this->TargetLevel = -1;
	this->MaterialID = -1;

	this->CardMaterialBox1->Cancel();
	this->CardMaterialBox2->Cancel();
	this->EvolveMaterialBox->Cancel();
	this->CardBox->Cancel();

	this->EvolveCard_Butt->SetIsEnabled(false);

	this->LoadCard();
	this->LoadMaterial();
}

void USynModel_GoldCardUp::Cancel() {
	this->TargetLevel = -1;
	this->MaterialID = -1;
	this->EvolveCard_Butt->SetIsEnabled(false);
	this->EvolveMaterialBox->Cancel();
	this->CardBox->Cancel();

	TArray<FItemViewBlock> SelectBlock;
	if (this->CardMaterialBox1->GetItemViewBlock().IsValid())
	{
		SelectBlock.Emplace(this->CardMaterialBox1->GetItemViewBlock());
	}
	if (this->CardMaterialBox2->GetItemViewBlock().IsValid())
	{
		SelectBlock.Emplace(this->CardMaterialBox2->GetItemViewBlock());
	}
	this->Syn->LoadPublicCards(SelectBlock, 2);
	this->LoadMaterial();
}

void USynModel_GoldCardUp::SelectCard(UUI_PlayerBagCardGrid* CardIns) {

	bool bSelect = false;
	TArray<FItemViewBlock> SelectBlock;

	if (this->CardMaterialBox1->GetItemViewBlock().IsValid())
	{
		SelectBlock.Emplace(this->CardMaterialBox1->GetItemViewBlock());
	}
	else
	{
		bSelect = true;
		this->CardMaterialBox1->SetItemAttributeCopy(CardIns);
		SelectBlock.Emplace(this->CardMaterialBox1->GetItemViewBlock());
	}

	if (bSelect)
	{
		if (this->CardMaterialBox2->GetItemViewBlock().IsValid())
		{
			SelectBlock.Emplace(this->CardMaterialBox2->GetItemViewBlock());
		}
	}
	else {
		if (this->CardMaterialBox2->GetItemViewBlock().IsValid())
		{
			SelectBlock.Emplace(this->CardMaterialBox2->GetItemViewBlock());
		}
		else
		{
			this->CardMaterialBox2->SetItemAttributeCopy(CardIns);
			SelectBlock.Emplace(this->CardMaterialBox2->GetItemViewBlock());
		}
	}

	//选择卡片
	this->Syn->LoadPublicCards(SelectBlock, 2, false);


	//如果有两个元素表示完成了选择，添加材料
	if (SelectBlock.Num() == 2)
	{
		//获取进化匹配表格
		DataTableAssetData<FMaterial_GoldCardUp_Data> Tab = GET_DATA(FMaterial_GoldCardUp_Data, Material, GoldCardUpData);

		if (Tab.Find(FName(FString::FromInt(SelectBlock[1].ItemID))))
		{
			FMaterial_GoldCardUp_Data FindData = Tab.GetDataByName(FName(FString::FromInt(SelectBlock[1].ItemID)));

			FMaterial_GoldCardUpNeedMaterData* Target = FindData.ItemID.Find(SelectBlock[0].ItemID);

			//查询成功，可以进行进化
			if (Target)
			{
				FItemViewBlock Block{ Target->NeedItemID,0,0 };
				this->EvolveMaterialBox->SetItemAttribute(
					GameDataStaticObject<UPlayerBagMaterialView>(), Block, 0
				);
				this->MaterialID = Target->NeedItemID;

				//查询材料
				if (GET_PLAYERDATAINS()->FindMaterialCount(this->MaterialID, 0, 1))
				{
					this->EvolveCard_Butt->SetIsEnabled(true);
					this->EvolveMaterialBox->SetButtonColorIsDark(false);
					//加载选择的材料
					this->Syn->LoadPublicMaterials({ 2U }, { Block }, GET_UI_NAME(Synthesis, GoldCardEvolve));
				}
				else {
					this->EvolveMaterialBox->SetButtonColorIsDark();
				}

				//目标卡片等级
				this->TargetLevel = SelectBlock[0].ItemLevel >= SelectBlock[1].ItemLevel ?
					SelectBlock[0].ItemLevel : SelectBlock[1].ItemLevel;
				this->TargetCardID = Target->TargetItemID;
				FItemViewBlock CardBlock{ this->TargetCardID,this->TargetLevel,0 };
				this->CardBox->SetItemAttribute(GameDataStaticObject<UPlayerBagCardView>(), CardBlock, 0);

			}
		}
	}

}

void USynModel_GoldCardUp::GoldCardEvolveFunc() {


	UPlayerStructManager* Player = GET_PLAYERDATAINS();

	//消耗卡片
	Player->ConsumeCard(this->CardMaterialBox1->GetItemViewBlock().ItemID,
		this->CardMaterialBox1->GetItemViewBlock().ItemLevel);
	Player->ConsumeCard(this->CardMaterialBox2->GetItemViewBlock().ItemID,
		this->CardMaterialBox2->GetItemViewBlock().ItemLevel);

	//消耗材料
	Player->ConsumeMaterial(this->MaterialID, 0);

	//发送卡片
	UGameSystemFunction::SendItemToBag(this->TargetCardID, this->TargetLevel, 1, 2U, false, true);

	//保存
	UGameSystemFunction::SaveCurrentPlayerData(TEXT("金卡进化"));

	this->Init();
}

void USynModel_GoldCardUp::LoadCard() {

	/*TArray<FItemViewBlock> SelectBlock;

	if (this->CardMaterialBox1->GetItemViewBlock().IsValid())
	{
		SelectBlock.Emplace(this->CardMaterialBox1->GetItemViewBlock());
	}

	if (this->CardMaterialBox2->GetItemViewBlock().IsValid())
	{
		SelectBlock.Emplace(this->CardMaterialBox2->GetItemViewBlock());
	}*/

	this->Syn->LoadPublicCards({}, 2);
}

void USynModel_GoldCardUp::LoadMaterial() {
	this->Syn->LoadPublicMaterials({ 2U }, {}, GET_UI_NAME(Synthesis, GoldCardEvolve));
}
