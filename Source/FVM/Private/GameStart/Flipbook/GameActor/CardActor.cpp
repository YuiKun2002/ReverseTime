
#include "GameStart/Flipbook/GameActor/CardActor.h"
#include "GameStart/Flipbook/GameActor/MouseActor.h"

#include "GameStart/VS/GameMapInstance.h"

#include "GameStart/VS/MapMeshe.h"

#include "GameStart/VS/UI/UI_MapMeshe.h"
#include "GameStart/VS/UI/UI_Card.h"

#include <Paper2D/Classes/PaperFlipbook.h>
#include <Paper2D/Classes/PaperFlipbookComponent.h>

#include <Components/BoxComponent.h>
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include <Components/StaticMeshComponent.h>

#include "GameStart/VS/Components/CardManagerComponent.h"
#include "GameStart/VS/Components/ResourceManagerComponent.h"
#include "GameStart/VS/Components/VSManagerComponent.h"
#include "GameStart/VS/Components/MesheControllComponent.h"

//卡片功能组件
#include "GameStart/VS/Components/Card/CardFunctionComponent.h"
#include "GameStart/VS/GameMapInstance.h"

//玩家背包数据
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/Tools/GameSystemFunction.h"


void ACardActor::SetCardData(const FItemCard& _CardData)
{
	/*最原始的数据*/

	/*会随着玩家的操作进行变化*/

	this->SetCardHP(_CardData.M_CardHP, 0.f);
	this->SetCardType(_CardData.M_ELineType, _CardData.M_CardLayer);
	this->M_CardProperty.M_CardName = _CardData.ItemName;
	this->M_CardProperty.M_CardDay = _CardData.M_CardDay;
	this->M_CardProperty.M_CardLayer = _CardData.M_CardLayer;
	this->M_SourceCardDataBase = _CardData;
	this->CardID = _CardData.ItemID;
}

UUI_Card* ACardActor::GetCardUI()
{
	return this->M_UUI_Card;
}

void ACardActor::SetCardDataObject(UCardDataObject* CardDataObject) {
	this->CurCardDataComponent = CardDataObject;
}

void ACardActor::SetCardUI(UUI_Card* _card_ui)
{
	this->M_UUI_Card = _card_ui;
}

FLine ACardActor::GetLine() const
{
	return this->M_CardProperty.M_Line;
}

int32 ACardActor::GetCardLayer() const
{
	return this->M_CardProperty.M_CardLayer;
}

ELineType ACardActor::GetLineType() const
{
	return this->M_CardProperty.M_ELineType;
}

void ACardActor::InitCardGradeToMap()
{
	int32& CurLevel = this->M_SourceCardDataBase.Level;
	if (CurLevel < 0)
	{
		CurLevel = 0;
	}
	else if (CurLevel > 16)
	{
		CurLevel = 16;
	}

	if (CurLevel > 3 && CurLevel < 17)
	{
		UDataTable* CurGradeDataTable = LoadObject<UDataTable>(this,
			TEXT("DataTable'/Game/Resource/BP/Data/CardData/1_CardGradeAnim.1_CardGradeAnim'")
		);

		if (FCard_GradeAnim_Data* CurGradeData = CurGradeDataTable->FindRow<FCard_GradeAnim_Data>(
			FName(FString::FromInt(CurLevel)), TEXT("Grade")))
		{
			UStaticMesh* CurStaticMesh = LoadObject<UStaticMesh>(this,
				TEXT("StaticMesh'/Game/Resource/BP/GameStart/Item/Mesh/Plane.Plane'"));
			this->M_CardGradeStaticMesh->SetStaticMesh(CurStaticMesh);

			UMaterialInstance* CurMaterial = Cast<UMaterialInstance>(
				CurGradeData->CardGradeResource.TryLoad());
			this->M_CardGradeStaticMesh->SetMaterial(0, CurMaterial);
			this->M_CardGradeStaticMesh->SetRelativeLocation(
				FVector(CurGradeData->RelativeLocation.X, 0.f, CurGradeData->RelativeLocation.Y));
			this->M_CardGradeStaticMesh->SetWorldScale3D(
				FVector(CurGradeData->Scale.X, CurGradeData->Scale.Y, 1.f));
		}

		this->M_CardGradeStaticMesh->SetHiddenInGame(false);

	}
	else {
		this->M_CardGradeStaticMesh->SetHiddenInGame(true);
	}
}

void ACardActor::SetLine(const FLine& _Line)
{
	this->M_CardProperty.M_Line = _Line;
}

void ACardActor::SetCardType(const ELineType& _Type, const int32& _CardID)
{
	this->M_CardProperty.M_ELineType = _Type;
	this->M_CardProperty.M_CardLayer = _CardID;
}

float ACardActor::GetCurrentHP() const
{
	return this->M_FCardActor_HP.M_Life;
}

int32 ACardActor::GetCardID() const
{
	return this->CardID;
}

float ACardActor::GetTotalHP() const
{
	return this->M_FCardActor_HP.M_TotalLife;
}

bool ACardActor::GetbMovement() const
{
	return this->M_UUI_MapMeshe->GetMapMeshe()->GetbMovement();
}

FItemCard ACardActor::GetCardSourceData() const
{
	return this->M_SourceCardDataBase;
}

void ACardActor::SetCardHP(const float& _HP, float _HPUpRate)
{
	this->M_FCardActor_HP.M_TotalLife = _HP;
	this->M_FCardActor_HP.M_Life = this->M_FCardActor_HP.M_TotalLife;
}

void ACardActor::SetCardCurrentHP(const float& _HP)
{
	this->M_FCardActor_HP.M_Life = _HP;
}

bool ACardActor::GetMapDay() const
{
	return this->M_CardProperty.M_Day;
}

bool ACardActor::GetCardDay() const
{
	return this->M_CardProperty.M_CardDay;
}

bool ACardActor::GetbCardDeaded() const {
	return (this->M_FCardActor_HP.M_Life <= 0.f);
}

bool ACardActor::GetbCardActive() const
{
	return this->bCardActive;
}

void ACardActor::SetRenderLayer(int32 _Layer)
{
	// 木盘子 -1  0  0,1
	// 小笼包 0   1  2,3
	// 西瓜皮 1   2  4,5

	/*int32 CurLayer = _Layer - TranslucencyConst::GetLayer(TranslucencyConst::ETranslucency::EMeshe);
	switch ((CurLayer - 1))
	{
	case -1:
		CurLayer = _Layer;
		; break;
	case 0:
		CurLayer = _Layer + 1;
		; break;
	case 1:
		CurLayer = _Layer + 2;
		; break;
	default:
		CurLayer = _Layer + 3;
		break;
	}*/

	//卡片层级
	Super::SetRenderLayer(_Layer);
	//卡片等级层级
	this->M_CardGradeStaticMesh->SetTranslucentSortPriority(_Layer + 1);
}

int32 ACardActor::GetCardGrade() const
{
	return this->M_SourceCardDataBase.Level;
}

void ACardActor::OnMapMesheMovedUpdate_Implementation(
	const float& DeltaTime,
	const int32& _Layer,
	const FVector& _Location,
	const FLine& _Line
)
{
	this->SetActorLocation(_Location);
	this->SetRenderLayer(_Layer + this->M_SourceCardDataBase.M_CardLayer + 1);
	this->SetLine(_Line);
}

void ACardActor::OnMapMesheMoveBegin_Implementation(const int32& _Layer, const FLine& _Line)
{
	this->SetLine(_Line);
	this->SetRenderLayer(_Layer + this->M_SourceCardDataBase.M_CardLayer + 1);
}

void ACardActor::OnMapMesheMoveEnd_Implementation(const int32& _Layer, const FLine& _Line)
{
	this->SetLine(_Line);
	this->SetRenderLayer(_Layer + this->M_SourceCardDataBase.M_CardLayer + 1);
}

void ACardActor::BeginPlay()
{
	Super::BeginPlay();

	this->M_AGameMapInstance = AGameMapInstance::GetGameMapInstance();

	//设置卡片碰撞类型
	this->M_CardTypeBoxCollision->SetBoxExtent(FVector(this->GetCardSourceData().M_CardCollsionWidth));
	this->M_CardTypeBoxCollision->SetRelativeLocation(FVector::ZeroVector);

	this->SetActorRotation(FRotator(0.f, 90.f, 0.f));

#if WITH_EDITOR
	if (UFVMGameInstance::GetDebug())
	{
		this->M_CardTypeBoxCollision->SetHiddenInGame(false);
	}
#endif
}

void ACardActor::PlayCard(class UObject* TriggerObject)
{
	//触发卡片放置
	this->OnCardPlayed(TriggerObject);

	//增加卡片费用
	this->EditCardPrice(this->M_SourceCardDataBase.M_CardPriceUpNum);

	//设置卡片渲染顺序
	if (this->GetCardLayer() == -1)
	{
		//设置卡片排序优先级
		this->SetRenderLayer(0);
	}
	else {
		//设置卡片排序优先级
		this->SetRenderLayer(this->GetUIMapMesh()->GetPlayerRenderLayer() + this->GetCardLayer() + 1);
	}

	//绑定网格移动
	this->M_UUI_MapMeshe->GetMapMeshe()->AddGameStartActorMovement(this);

	//卡片激活
	if (this->GetCardDay() && this->GetMapDay() || this->GetCardDay() && !this->GetMapDay())
	{
		// (白天卡)直接激活
		bool bActiv = true;
		this->OnCardActivated(bActiv);
	}
	else if (!this->GetCardDay() && !this->GetMapDay())//(夜间卡)，只会在夜间激活
	{
		//激活
		bool bActiv = true;
		this->OnCardActivated(bActiv);
	}
	else {
		//其他情况默认 未激活
		bool bActiv = false;
		this->OnCardActivated(bActiv);
	}
}

void ACardActor::ActiveCard(bool bCardActiveState, UObject* TriggerObject) {

	this->OnCardActivated(bCardActiveState, TriggerObject);

	//设置卡片激活状态
	this->bCardActive = bCardActiveState;

	//更新卡片状态
	if (this->bCardActive)
	{
		this->SetFlipbookColor(FVector(1.f), 1.f);
	}
	else {
		this->SetFlipbookColor(FVector(0.2f), 1.f);
	}
}

void ACardActor::ReplaceCard(UClass* NewCardResourceClass, const FItemCard& NewCardData) {
	this->OnCardReplaced(NewCardResourceClass, NewCardData);
}

bool ACardActor::ReplyCard(float ReplyValue, UObject* TriggerObject) {

	this->OnCardReplyed(ReplyValue, TriggerObject);

	//更新生命值
	this->M_FCardActor_HP.M_Life = this->M_FCardActor_HP.M_Life + ReplyValue;

#if WITH_EDITOR
	if (UFVMGameInstance::GetDebug())
	{
		FString Content = FString(TEXT("【")) + FString::FromInt(this->GetCardID()) +
			FString(TEXT("】:")) +
			FString::SanitizeFloat(ReplyValue) + FString(TEXT("回复生命：HP:")) +
			FString::SanitizeFloat(this->M_FCardActor_HP.M_Life);

		UGameSystemFunction::FVMLog(__FUNCTION__, Content);
	}
#endif

	return true;
}

bool ACardActor::AttackCard(float AttackValue, UObject* TriggerObject) {

	this->OnCardAttacked(AttackValue, TriggerObject);

	if (this->M_FCardActor_HP.M_Life <= 0.f)
	{
		return false;
	}

	//更新生命值
	this->M_FCardActor_HP.M_Life = this->M_FCardActor_HP.M_Life - AttackValue;

#if WITH_EDITOR
	if (UFVMGameInstance::GetDebug())
	{
		FString Content = FString(TEXT("【")) + FString::FromInt(this->GetCardID()) +
			FString(TEXT("】:")) +
			FString::SanitizeFloat(AttackValue) + FString(TEXT("受到攻击：HP:")) +
			FString::SanitizeFloat(this->M_FCardActor_HP.M_Life);

		UGameSystemFunction::FVMLog(__FUNCTION__, Content);
	}
#endif

	return true;
}

void ACardActor::KillCard(UObject* TriggerObject)
{

	this->OnCardDeathed(TriggerObject);

	//卡片摧毁[根据实际情况使用，Cast<XXX>]
	//UResourceManagerComponent::ResourceAddBadCard();

	if (IsValid(this->M_UUI_MapMeshe))
	{
		//网格必须存在
		if (this->M_UUI_MapMeshe->GetMapMeshe())
		{
			//修改卡片价格
			this->EditCardPrice(this->M_SourceCardDataBase.M_CardPriceUpNum * -1);

			//对网格解除移动相关的绑定
			this->M_UUI_MapMeshe->GetMapMeshe()->RemoveGameStartActorMovement(this);
		}
	}

	//解除绑定的委托
	this->OnCardPlayedDelegate.Clear();
	this->OnCardActivatedDelegate.Clear();
	this->OnCardAttackedDelegate.Clear();
	this->OnCardReplyedDelegate.Clear();
	this->OnCardDeathedDelegate.Clear();

	this->Destroy();
}

void ACardActor::SetUIMapMesh(UUI_MapMeshe* _MapMesh)
{
	this->M_UUI_MapMeshe = _MapMesh;
}

UUI_MapMeshe* ACardActor::GetUIMapMesh()
{
	return this->M_UUI_MapMeshe;
}

void ACardActor::OnCardInitData(const FItemCard& NewBaseData) {

	//初始化卡片等级
	this->M_SourceCardDataBase.Level = NewBaseData.Level;

	//初始化卡片等级动画
	this->InitCardGradeToMap();

	//获取白天和夜晚
	this->M_CardProperty.M_Day = AGameMapInstance::GetGameMapResourceManagerStatic()->GetDay();

	//如果选择关闭碰撞
	if (this->GetCardSourceData().M_bEnableCardCollision)
	{
		//设置卡片碰撞类型
		this->bCollisionActive = false;
		this->SetCardCollisonState(true);
	}
	else {
		this->bCollisionActive = true;
		this->SetCardCollisonState(false);
	}
}

void ACardActor::OnCardPlayed(UObject* TriggerObject)
{
	this->TriggerObjectInstance = TriggerObject;
	this->OnCardPlayedDelegate.Broadcast(TriggerObject);
}

void ACardActor::OnCardActivated(bool& bCardActiveState, UObject* TriggerObject)
{
	this->TriggerObjectInstance = TriggerObject;
	this->OnCardActivatedDelegate.Broadcast(bCardActiveState, TriggerObject);
}

void ACardActor::OnCardReplaced(UClass* NewCardResourceClass, const FItemCard& NewCardData) {
	this->OnCardReplacedDelegate.Broadcast(NewCardResourceClass, NewCardData);
}

void ACardActor::OnCardAttacked(float& AttackValue, class UObject* AttackObject)
{
	this->TriggerObjectInstance = AttackObject;
	this->OnCardAttackedDelegate.Broadcast(AttackValue, AttackObject);
}

void ACardActor::OnCardReplyed(float& ReplyValue, UObject* ReplyObject)
{
	this->TriggerObjectInstance = ReplyObject;
	this->OnCardReplyedDelegate.Broadcast(ReplyValue, ReplyObject);
}

void ACardActor::OnCardDeathed(UObject* TriggerObject)
{
	this->TriggerObjectInstance = TriggerObject;
	this->OnCardDeathedDelegate.Broadcast(TriggerObject);
}

ACardActor::ACardActor()
{
	PrimaryActorTick.bCanEverTick = true;

	this->CurCheckMesheLine = FLine(-1);

	this->M_CardGradeStaticMesh = this->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardGradeMeshComponent"));
	this->M_CardGradeStaticMesh->SetupAttachment(this->GetGameLocation());
	this->M_CardGradeStaticMesh->SetWorldRotation(FRotator(0.f, 0.f, 90.f));

	this->M_CardTypeBoxCollision = this->CreateDefaultSubobject<UBoxComponent>(TEXT("CardTypeBox"));
	this->M_CardTypeBoxCollision->SetupAttachment(this->GetRootComponent());

	this->M_CardGradeStaticMesh->SetGenerateOverlapEvents(false);
	this->M_CardGradeStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	this->CardManagerComponent = AGameMapInstance::GetGameMapCardManagerStatic();
}

void ACardActor::GameTick(float DeltaSeconds)
{
	Super::GameTick(DeltaSeconds);
}

UCardDataObject* ACardActor::GetCardDataObject()
{
	return this->CurCardDataComponent;
}

UBoxComponent* ACardActor::GetBoxComponent()
{
	return this->M_CardTypeBoxCollision;
}

UObject* ACardActor::GetTriggerObject() {
	return this->TriggerObjectInstance;
}

void ACardActor::SetCardCollisonState(bool bActive)
{
	if (bActive)
	{
		if (!this->bCollisionActive)
		{
			this->bCollisionActive = true;
			this->bCollisionState = true;

			this->M_CardTypeBoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);

			this->M_CardTypeBoxCollision->SetCollisionResponseToChannel(
				UGameSystemFunction::GetCardCollisionBoxType(
					this->GetCardSourceData().M_ECardCollisionType),
				ECollisionResponse::ECR_Block);

			this->M_CardTypeBoxCollision->SetCollisionResponseToChannel(
				UGameSystemFunction::GetCardCollisionBoxType(ECardCollisionType::E_CardActor),
				ECollisionResponse::ECR_Block);

			this->M_CardTypeBoxCollision->SetBoxExtent(
				FVector(this->GetCardSourceData().M_CardCollsionWidth));
			this->M_CardTypeBoxCollision->SetRelativeLocation(FVector::ZeroVector);
		}
	}
	else {
		if (this->bCollisionActive)
		{
			this->bCollisionActive = false;
			this->bCollisionState = false;

			//设置卡片碰撞类型
			this->M_CardTypeBoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
		}
	}
}

void ACardActor::SetCollisionEnable(bool bValue)
{
	//如果是开启碰撞的卡片
	if (this->M_SourceCardDataBase.M_bEnableCardCollision)
	{
		if (bValue)
		{
			if (!this->bCollisionState)
			{
				this->bCollisionState = true;
				this->M_CardTypeBoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
		}
		else {
			if (this->bCollisionState)
			{
				this->bCollisionState = false;
				this->M_CardTypeBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}


void ACardActor::EditCardPrice(int32 EditValue) {
	//卡片自动增加+
	if (this->M_SourceCardDataBase.M_CardPriceAutoUp)
	{
		//调整卡片价格
		FItemCard* CurCardData = this->CardManagerComponent->GetCardDataByID(this->CardID);
		if (CurCardData)
		{
			UUI_Card* CurCardUI = this->CardManagerComponent->GetUICard(this->CardID);
			if (IsValid(CurCardUI))
			{
				//调整卡片价格
				int32 CardFlame = CurCardUI->GetCardFlame();
				int32 TargetFlame = CardFlame + EditValue;
				CurCardUI->SetCardFlame(TargetFlame, true);
				CurCardData->M_CardPrice = TargetFlame;
			}
		}
	}
}