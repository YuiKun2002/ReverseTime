// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStart/VS/Components/CardManagerComponent.h"
#include "GameStart/VS/MapMeshe.h"
#include "GameStart/VS/GameMapInstance.h"
#include "GameStart/VS/Components/MesheControllComponent.h"
#include "GameStart/VS/Components/Card/CardDataComponent.h"

#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/GameConfigManager.h"

//玩家背包数据
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/Tools/GameSystemFunction.h"

#include "GameStart/Flipbook/GameActor/CardActor.h"
#include "GameStart/VS/CardCoolDownManager.h"

#include "GameStart/VS/UI/UI_Card.h"
#include "GameStart/VS/UI/UI_MapMeshe.h"

#include <Kismet/KismetMathLibrary.h>

//铲子UI
#include "GameStart/VS/UI/UI_EradicateCard.h"

#include "GameStart/VS/UI/UI_CardBar.h"
#include <Components/HorizontalBox.h>
#include <Components/Image.h>
#include <Engine/Texture2D.h>

// Sets default values for this component's properties
UCardManagerComponent::UCardManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	this->SetTickableWhenPaused(true);
}

void UCardManagerComponent::GameStartInit(class AGameMapInstance* GameMapInstance)
{

	GAME_LOG(__FUNCTION__, TEXT("卡片管理器组件，游戏开始时，加载卡片拖拽所需的动画"), {
		GAME_LOG_LINE(TEXT("需要重做"));
		});

	UFVMGameInstance* GameIns = GAMEINS();
	UPlayerStructManager* PlayerIns;
	if (!IsValid(GameIns))
	{
		return;
	}

	PlayerIns = GameIns->LoadPlayerStructManager(GameIns->GetPlayerLoginName());

	if (!IsValid(PlayerIns))
	{
		return;
	}

	//初始化卡片默认动画
	//加载数据表
	/*UDataTable* Table = LoadObject<UDataTable>(0,
		TEXT("DataTable'/Game/Resource/BP/Data/CardData/0_CardIdleAnim.0_CardIdleAnim'"));
	TArray<FCard_KeyIdleAnim_Data*> LocalCard_KeyIdleAnim_Data;
	Table->GetAllRows("List", LocalCard_KeyIdleAnim_Data);
	for (auto Data : LocalCard_KeyIdleAnim_Data)
	{
		this->M_CardIdleAnim.Emplace((Data)->M_CardKeyIdleAnim.IdlePath.ToString());
	}*/

	//初始化卡片数据组件
	this->CardDataObject = NewObject<UCardDataObject>();

	//初始化卡片bar
	this->M_UUI_CardBar = CREATE_UI_WIDGET(UUI_CardBar, GameVs, CardBar);

	this->M_UUI_CardBar->Init();

	UCardDataAssetCache* CardCache = GET_CACHE(Card);

	for (auto CardDatas : PlayerIns->GetOtherData().GamePrepareSelectCardData)
	{
		FItemCard Data;
		Data.Level = CardDatas.ItemLevel;
		Data.ItemID = CardDatas.ItemID;
		if (CardCache->GetCardData(CardDatas.ItemID, Data))
		{
			this->AddNewCardData(CardDatas.ItemID, Data);
		}

	}

	GAME_LOG(__FUNCTION__, TEXT("卡片管理器组件，游戏开始时，关卡放置预制卡片"), {
	GAME_LOG_LINE(TEXT("需要重做"));
		});

	//初始化场地卡片
	//放置预备卡片
	//UMesheControllComponent* MesheCtlComp = AGameMapInstance::GetGameMapMesheControllManagerStatic();
	//for (AActor* MeshActor : MesheCtlComp->GetAllMapMeshes())
	//{
	//	if (AMapMeshe* CurMapMeshe = Cast<AMapMeshe>(MeshActor))
	//	{
	//		//如果没有UI则跳过
	//		if (!CurMapMeshe->GetUIMapMeshe())
	//		{
	//			continue;
	//		}

	//		TArray<FMesheColPrePlayCard> CurDatas = CurMapMeshe->GetPrePlayCard();
	//		//排序->从小到大，依次放置
	//		CurDatas.StableSort(
	//			[](const FMesheColPrePlayCard& A, const FMesheColPrePlayCard& B) {
	//				return A.CardLayer < B.CardLayer;
	//			}
	//		);

	//		for (const FMesheColPrePlayCard& CurCardData : CurDatas)
	//		{
	//			if (CurMapMeshe->GetUIMapMeshe()->CreateCard(
	//				CurMapMeshe->GetUIMapMeshe(),
	//				this,
	//				LoadClass<ACardActor>(0, *CurCardData.CardActorResource.ToString()),
	//				CurCardData.CardName,
	//				0,
	//				false,
	//				false
	//			))
	//			{
	//				CurMapMeshe->GetUIMapMeshe()->UpdateAllCardsCollision();
	//			}
	//		}
	//	}
	//}
}
void UCardManagerComponent::GameStart(class AGameMapInstance* GameMapInstance)
{
	this->ShowCardBar();
}
void UCardManagerComponent::GameTick(float DeltaTime)
{

}
void UCardManagerComponent::GameOverInit(class AGameMapInstance* GameMapInstance, bool bGameWin)
{
	this->HiddenCardBar();
}
void UCardManagerComponent::GameOver(class AGameMapInstance* GameMapInstance, bool bGameWin)
{

}
void UCardManagerComponent::GameResourceUnload()
{

}

void UCardManagerComponent::SetCurrentCardMapMeshe(UUI_MapMeshe* _MapMeshe)
{
	this->M_CurrentPlayCardMapMeshe = _MapMeshe;
}

UUI_MapMeshe* UCardManagerComponent::GetCurrentCardMapMeshe()
{
	return this->M_CurrentPlayCardMapMeshe;
}

UClass* UCardManagerComponent::GetCurrentSelectActor()
{
	return this->M_CurrentSelectCardInstance;
}

ACardCoolDownManager* UCardManagerComponent::GetCurrentCoolDownManager()
{
	if (this->M_CurrentSelectCardID == -1)
	{
		return nullptr;
	}

	UUI_Card** Card = this->M_UICard.Find(this->M_CurrentSelectCardID);
	if (Card)
	{
		return (*Card)->GetCardCoolDownManager();
	}

	return nullptr;
}

void UCardManagerComponent::SetCardUITexture(UUI_Card* _CardUI, const FString& _CardBg)
{
	if (_CardUI)
	{
		//对应卡片的背景路径
		//UWidgetBase::SetButtonStyle();
		UWidgetBase::SetImageBrush(_CardUI->GetCardHead(), _CardBg, FVector(1.f), 1.f, false, FVector(1.f), true);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("卡片背景图片设置失败，因为当前卡片UI无效"));
	}
}

void UCardManagerComponent::SetLastCardIDAndClass(int32 LastCardID, UClass* LastCardIns) {
	this->M_LastSelectCardID = LastCardID;
	this->M_LastSelectCardClass = LastCardIns;
}

void UCardManagerComponent::SetEradicateCardInstance(class UUI_EradicateCard* EradicateCardIns) {
	this->M_UUI_EradicateCard = EradicateCardIns;
}

bool UCardManagerComponent::SelectCurrentActor(int32 ID, bool ForceSelect)
{
	//如果不是强制选择卡片
	if (!ForceSelect)
	{
		//如果当前选择的卡片名称和之前选择的卡片名称一样
		if (this->M_CurrentSelectCardID == ID)
		{
			//则取消选择
			this->CancelSelect();

			//卡片名称相同
			//UE_LOG(LogTemp, Error, TEXT("选择的卡片名称相同"));

			return false;
		}

		//则取消选择
		this->CancelSelect();

		//取消对铲子的选择
		this->CancelEradicate();
	}

#if WITH_EDITOR
	//打印当前选择的卡片
	if (UFVMGameInstance::GetDebug())
	{
		UGameSystemFunction::FVMLog(__FUNCTION__, FString(TEXT("当前选择了防御卡：") + FString::FromInt(ID)));
	}
#endif

	//播放BGM——S
	UFVMGameInstance::PlayBGM_S_Static(TEXT("TakeCard"), TEXT("ItemAudio"));

	//设置选择的名称
	this->M_CurrentSelectCardID = ID;

	//非强制选卡
	if (!ForceSelect)
	{
		//获取UICard
		if (this->GetUICard(this->M_CurrentSelectCardID))
		{
			UUI_Card* ForceSelectCard = this->GetUICard(ID);
			if (IsValid(ForceSelectCard))
			{
				//设置卡片选择的颜色
				ForceSelectCard->SelectColor();
				this->M_CurrentSelectCardInstance = ForceSelectCard->GetCardClassInstance();
				//设置当前卡片状态为选择
				ForceSelectCard->SetCardSelect(true);
				return true;
			}
		}

		return false;
	}

	UUI_Card* CardUI = this->GetUICard(this->M_CurrentSelectCardID);
	if (IsValid(CardUI))
	{
		this->M_CurrentSelectCardInstance = CardUI->GetCardClassInstance();
		//设置当前卡片状态为选择
		this->GetUICard(this->M_CurrentSelectCardID)->SetCardSelect(true);
	}

	return true;
}

const int32 UCardManagerComponent::GetCardFlameByID(int32 ID)
{
	FItemCard Data;
	if (this->GetCardDataByID(ID, Data))
	{
		return Data.M_CardPrice;
	}
	return 0;
}

bool UCardManagerComponent::GetCardDataByID(int32 ID, FItemCard& _CardFItemData) const
{
	const FItemCard* Data = this->M_CardData.Find(ID);
	if (Data)
	{
		_CardFItemData = *Data;
		return true;
	}
	return false;
}

FItemCard* UCardManagerComponent::GetCardDataByID(int32 ID)
{
	return this->M_CardData.Find(ID);
}

UUI_Card* const UCardManagerComponent::GetUICard(int32 ID)
{

	UUI_Card** UICard = this->M_UICard.Find(ID);
	if (UICard)
	{
		return *UICard;
	}

	return nullptr;
}

void UCardManagerComponent::GetCardsUI(TArray<UUI_Card*>& OutCardsUI)
{
	this->M_UICard.GenerateValueArray(OutCardsUI);
}

int32 UCardManagerComponent::GetCurrentSelectCardID() const
{
	return this->M_CurrentSelectCardID;
}

void UCardManagerComponent::CancelSelect()
{
	if (this->M_CurrentSelectCardID == -1)
	{
		return;
	}

	UUI_Card* CurSelectCard = this->GetUICard(this->M_CurrentSelectCardID);
	if (IsValid(CurSelectCard))
	{
		CurSelectCard->CancelColor();
		CurSelectCard->SetCardSelect(false);
		if (this->M_CurrentSelectCardInstance)
		{
			this->M_CurrentSelectCardInstance = nullptr;
		}
	}
	//清空名称
	this->M_CurrentSelectCardID = -1;
}

void UCardManagerComponent::SelectEradicate()
{
	this->M_CurrentSelectEradicate = true;
	this->CancelSelect();

	if (IsValid(this->M_UUI_EradicateCard))
	{
		this->M_UUI_EradicateCard->Select();
	}

	UFVMGameInstance::PlayBGM_S_Static(TEXT("TakeEradicate"), TEXT("ItemAudio"));
}

void UCardManagerComponent::CancelEradicate()
{
	this->M_CurrentSelectEradicate = false;

	if (IsValid(this->M_UUI_EradicateCard))
		this->M_UUI_EradicateCard->Cancel();
}

void UCardManagerComponent::AddNewCardData(int32 ID, const FItemCard& _CardData)
{
	if (this->M_UUI_CardBar)
	{
		UUI_Card* CardBoxWidget = CREATE_UI_WIDGET(UUI_Card, GameVs, CardUI);
		//卡片索引 = 卡片ID号
		CardBoxWidget->SetCardID(ID);
		//设置卡片能量
		CardBoxWidget->SetCardFlame(_CardData.M_CardPrice, _CardData.M_CardPriceAutoUp);
		//设置等级
		CardBoxWidget->SetCardLevelTexture(_CardData.Level);
		//是否可以被复制
		CardBoxWidget->SetbEnableCopy(_CardData.M_bEnableCopy);
		//设置卡片实例
		CardBoxWidget->SetCardClassInstance(
			_CardData.CardActorResource.TryLoadClass<ACardActor>()
		);
		//设置卡片UI的卡片头像
		this->SetCardUITexture(CardBoxWidget, _CardData.CardActorHead.ToString());
		//设置显示属性
		CardBoxWidget->SetVisibility(ESlateVisibility::Visible);

#if WITH_EDITOR	
		if (UFVMGameInstance::GetDebug())
		{
			FString LContent =
				FString(TEXT("当前卡片[") +
					_CardData.ItemName +
					TEXT("]加入放置栏位")
				);

			UGameSystemFunction::FVMLog(__FUNCTION__, LContent);
		}
#endif

		//当前冷却
		float _CardClodDown = 0.f;

		//计算冷却
		if (_CardData.M_ECardUpGradeType == ECardUpGradeType::E_ColdDownSpeed_Up)
		{
			_CardClodDown =
				_CardData.M_CardColdDown -
				(_CardData.M_CardColdDown *
					_CardData.Level *
					(_CardData.M_M_ECardUpGradeUpRate / 100.f)
					);
#if WITH_EDITOR	
			if (UFVMGameInstance::GetDebug())
			{
				FString LContent =
					FString(TEXT("当前卡片[") +
						_CardData.ItemName +
						TEXT("]等级提升技能：冷却速度=>") +
						FString::SanitizeFloat(_CardClodDown)
					);

				UGameSystemFunction::FVMLog(__FUNCTION__, LContent);
			}
#endif
		}
		else
		{
			_CardClodDown = _CardData.M_CardColdDown;
		}


		//技能书->技能冷却提升
		if (_CardData.M_ECardSkillType == ECardSkillType::E_COLDDOWN)
		{
			int32 Lv = UGameSystemFunction::GetPlayerSkillBooksLv(_CardData.M_CardSkillBookName);
			float _Result = _CardClodDown - _CardClodDown * Lv * 0.1f;
			_CardClodDown = _Result > 0.f ? _Result : 0.f;

#if WITH_EDITOR	
			if (UFVMGameInstance::GetDebug())
			{
				FString Content =
					FString(TEXT("当前卡片[") +
						_CardData.M_CardSkillBookName +
						TEXT("]技能书技能：减少冷却速度=>") +
						FString::SanitizeFloat(_CardClodDown)
					);

				UGameSystemFunction::FVMLog(__FUNCTION__, Content);
			}
#endif
		}

		//设置卡片冷却管理器
		ACardCoolDownManager* NewCardCoolDownManager = this->GetWorld()
			->SpawnActor<ACardCoolDownManager>();
		NewCardCoolDownManager->SetCoolDownTime(_CardClodDown);
		CardBoxWidget->SetCardCoolDownManager(NewCardCoolDownManager);

		//延展卡片栏UI，更新卡片栏位置
		this->M_UUI_CardBar->AddCard(CardBoxWidget);
		//将卡片添加到列表管理
		this->M_UICard.Emplace(ID, CardBoxWidget);
		//新增数据
		this->M_CardData.Emplace(ID, _CardData);
	}
}

bool UCardManagerComponent::AddNewCardDataAndOut(int32 ID, FItemCard& OutData, int32 NewLevel)
{
	ECardType CardType;
	FItemCard* NewData = nullptr;
	if (GET_CACHE(Card)->GetCardData(ID, CardType, NewData))
	{
		OutData = *NewData;
		OutData.Level = NewLevel;
		this->M_CardData.Emplace(ID, OutData);
		return true;
	}
	return false;
}

void UCardManagerComponent::HiddenCardBar()
{
	if (this->M_UUI_CardBar->IsInViewport())
	{
		this->M_UUI_CardBar->RemoveFromParent();
	}
}

void UCardManagerComponent::ShowCardBar()
{
	if (!this->M_UUI_CardBar->IsInViewport())
	{
		this->M_UUI_CardBar->AddToViewport();
	}
}

bool UCardManagerComponent::GetCardCoolDownFinish(int32 ID)
{
	UUI_Card** Card = this->M_UICard.Find(ID);
	if (Card)
	{
		return (*Card)->GetCardCoolDownManager()->GetIsCoolDownFinish();
	}

	//获取卡片冷却管理器
	return false;
}

UUI_CardBar* UCardManagerComponent::GetUICardBar()
{
	return this->M_UUI_CardBar;
}

UCardDataObject* UCardManagerComponent::GetCardDataObject() {
	return this->CardDataObject;
}

UClass* UCardManagerComponent::GetLastSelectCardClass()
{
	return this->M_LastSelectCardClass;
}

int32 UCardManagerComponent::GetLastSelectCardID()
{
	return this->M_LastSelectCardID;
}

ACardCoolDownManager* UCardManagerComponent::GetCardCoolDownManagerByID(int32 ID) {
	UUI_Card** Card = this->M_UICard.Find(ID);
	if (Card)
	{
		return (*Card)->GetCardCoolDownManager();
	}
	return nullptr;
}

bool UCardManagerComponent::GetbSelectEradicate() const
{
	return this->M_CurrentSelectEradicate;
}

