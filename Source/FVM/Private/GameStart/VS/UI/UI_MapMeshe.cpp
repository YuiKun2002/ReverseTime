// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStart/VS/UI/UI_MapMeshe.h"
#include "GameStart/Flipbook/GameActor/CardActor.h"
#include "GameStart/Flipbook/GameActor/MouseActor.h"
#include "GameStart/Flipbook/GameActor/GamePlayer.h"

#include "GameStart/VS/Components/CardManagerComponent.h"
#include "GameStart/VS/Components/VSManagerComponent.h"
#include "GameStart/VS/CardCoolDownManager.h"
#include "GameStart/VS/GameMapInstance.h"
#include "GameStart/VS/MapMeshe.h"

#include "GameStart/VS/Components/ResourceManagerComponent.h"
#include "GameStart/VS/Components/MesheControllComponent.h"

#include "GameStart/VS/CreateCardBar.h"
#include "GameStart/VS/UI/UI_Card.h"

#include "GameSystem/FVMGameInstance.h"

#include <Kismet/GameplayStatics.h>

#include <Components/Image.h>

void UUI_MapMeshe::PlayCard_PlayBGM()
{
	if (
		this->M_ELineType == ELineType::OnWater
		||
		this->M_ELineType == ELineType::Underwater

		)
	{
		UFVMGameInstance::PlayBGM_S_Static("PlayCardToOnWater", "ItemAudio");
	}
	else {
		UFVMGameInstance::PlayBGM_S_Static("PlayCardToGround", "ItemAudio");
	}
}

//卡片放置
bool UUI_MapMeshe::PlayCard(
	UCardManagerComponent* CardManagerComponent,
	UClass* CardResourceInstance,
	const FItemCard& CardData,
	bool bResetCoolDown
)
{
	//判断当前卡片是否有自我替换能力
	if (CardData.ReplaceLayers.Num())
	{
		//卡片替换
		for (const auto& ReplaceLayer : CardData.ReplaceLayers) {
			//查询同层级的卡片
			ACardActor* CardActor = this->GetCardLayers(CardData.M_CardLayer);
			if (IsValid(CardActor))
			{
				//执行替换规则
				CardActor->ReplaceCard(CardResourceInstance, CardData);
				return true;
			}
		}
	}

#if WITH_EDITOR
	GAME_LOG(__FUNCTION__, TEXT("卡片创建"), {
						GAME_LOG_LINE(TEXT("开始创建新卡片：[%s]"),*(CardData.ItemName));
		});
#endif

	//测试卡片层级
	if (!this->TestCardLayer(CardData.M_CardLayer)) {

#if WITH_EDITOR
		GAME_LOG(__FUNCTION__, TEXT("卡片创建失败"), {
							GAME_LOG_LINE(TEXT("当前网格已经存在相同层级的卡片"));
			});
#endif

		AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(TEXT("当前位置已经存在防御卡，无法放置"));

		return false;
	}

	//设置当前放置的网格
	CardManagerComponent->SetCurrentCardMapMeshe(this);

	//获取卡片的UI栏
	UUI_Card* UICardIns = CardManagerComponent->GetUICard(CardData.ItemID);

	//初始化卡片的位置
	FTransform Trans = this->M_MapMesheTransform;
	FVector _Location = Trans.GetLocation();
	Trans.SetLocation(_Location);

	//准备生成世界卡片Actor的实例对象
	ACardActor* Card = Cast <ACardActor>(
		UGameplayStatics::BeginDeferredActorSpawnFromClass(
			this->M_MapMeshe->GetWorld(),
			CardResourceInstance, Trans)
	);
	//设置线路
	Card->SetLine(this->GetLine());
	//设置UIMapMesh
	Card->SetUIMapMesh(this);
	//设置UICard
	Card->SetCardUI(UICardIns);
	//设置卡片数据对象
	Card->SetCardDataObject(CardManagerComponent->GetCardDataObject());
	//初始化卡片数据
	Card->OnCardInitData(CardData);
	//产生阶段结束 【完成对象的生成，执行BeginPlay】
	UGameplayStatics::FinishSpawningActor(Card, Trans);

	//重置冷却时间
	if (bResetCoolDown)
	{
		//重置冷却
		if (IsValid(UICardIns))
		{
			UICardIns->GetCardCoolDownManager()->SetCoolDownFinish(false);
		}

		//对当前卡片进行取消选择
		CardManagerComponent->CancelSelect();
	}

	//添加层级
	if (this->AddCardLayer(CardData.M_CardLayer, Card))
	{
		//添加上一次卡片选择(限制到可以被复制的卡片Copy)
		if (CardData.M_bEnableCopy)
		{
			CardManagerComponent->SetLastCardIDAndClass(
				CardData.ItemID,
				CardResourceInstance
			);
		}
	}

	//判断是否是夜间卡片  并且 开启了咖啡粉自动激活
	if (!CardData.M_CardDay && CardData.M_AutoEnableCardDay)
	{
		GAME_LOG(__FUNCTION__, TEXT("卡片创建时，如果遇到咖啡粉，则自动激活"), {
		GAME_LOG_LINE(TEXT("应该改为放置技能，需要重新写"));
			});
		/*
		//寻找卡片列表的咖啡粉（如果找到则修改夜间卡片为白天）
		if (_CardMComp->GetUICard(119))
		{
			Card->CardDayTransform(true);
		}
		else if (!this->MesheCtr->
			GetGameMapStructManager()->LevelConfig.LevelIsDay
			)
		{
			//如果本事处于夜晚
			Card->CardDayTransform(true);
		}
		*/
	}


	//放置完成
	this->PlayFinish(Card);

	return true;
}

void UUI_MapMeshe::PlayFinish(ACardActor* NewCard, bool bResetPlay)
{
	//播放音效
	this->PlayCard_PlayBGM();

	//如果是第一次放置成功
	if (!bResetPlay)
	{
		//减去费用
		AGameMapInstance::GetGameMapResourceManagerStatic()->SubtractFlameNum(
			NewCard->GetCardSourceData().M_CardPrice
		);
		//新增卡片放置名称
		UResourceManagerComponent::ResourceAddPlayCard(NewCard->GetCardSourceData().ItemName);
	}

	//加载卡片放置动画资源
	if (!IsValid(this->CardPlayAnimClass))
	{
		this->CardPlayAnimClass = LoadClass<AActor>(
			0,
			TEXT("Blueprint'/Game/Resource/Texture/Sprite/VS/Map/0/PlayGroundAnim/BP_放置动画.BP_放置动画_C'")
		);
	}

	if (IsValid(this->CardPlayAnimClass))
	{	//创建放置动画
		AActor* CurAnim = this->GetMapMeshe()->GetWorld()->SpawnActor(this->CardPlayAnimClass);

		//设置卡片最终位置
		CurAnim->SetActorLocation(NewCard->GetUIMapMesh()->GetMapMeshe()->GetActorLocation());
	}

	//卡片放置
	NewCard->PlayCard(this);
}

bool UUI_MapMeshe::PlayPlayer()
{
	return AGameMapInstance::GetGameMapVsManagerStatic()->SpawnMainPlayer(this->GetLine());
}

ACardActor* UUI_MapMeshe::GetCardLayers(int32 CardLayer) {
	ACardActor** CardIns = this->M_CardLayers.Find(CardLayer);
	if (CardIns)
	{
		return *CardIns;
	}
	return nullptr;
}

const TMap<int32, ACardActor*>& UUI_MapMeshe::GetCardLayers()
{
	return this->M_CardLayers;
}

void UUI_MapMeshe::GetAllCardsInstance(TArray<ACardActor*>& OutAllCards)
{
	this->M_CardLayers.GenerateValueArray(OutAllCards);
}

bool UUI_MapMeshe::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	this->M_Gird = this->GetWidgetComponent<UImage>(this, "Meshe");

	this->bPlayer = false;

	if (IsValid(AGameMapInstance::GetGameMapInstance()))
	{
		this->MesheCtr = AGameMapInstance::GetGameMapMesheControllManagerStatic();
	}

	return true;
}

bool UUI_MapMeshe::AddCardLayer(const int32 CardLayer, ACardActor* _Card)
{

	//如果当前Layer == 0 并且 卡片对象无效，则表示当前对象是角色对象
	if (!IsValid(_Card) && CardLayer != 0)
	{
		return false;
	}

	this->M_CardLayers.Emplace(CardLayer, _Card);

	return true;
}

bool UUI_MapMeshe::TestCardLayer(const int32 CardLayer)
{

	//如果当前位置存在角色，则不会成功
	if (CardLayer == 0 && this->GetbHasPlayer())
	{
		return false;
	}

	//如果以及存在相同层级的卡片，则不会成功
	if (this->M_CardLayers.Contains(CardLayer))
	{
		return false;
	}

	return true;
}

void UUI_MapMeshe::KillAllCards()
{
	bool bHasPlayer = false;
	for (auto& Cur : this->M_CardLayers)
	{
		//如果角色下方存在底卡，则不会被消灭
		if (Cur.Key == -1)
		{
			if (IsValid(Cur.Value))
			{
				if (this->GetbHasPlayer())
				{
					bHasPlayer = true;
					continue;
				}
			}
		}

		if (IsValid(Cur.Value))
		{
			Cur.Value->KillCard();
			Cur.Value = nullptr;
		}
	}


	//判断是否还存留卡片
	if (bHasPlayer)
	{
		TMap<int32, ACardActor*> NewCardLayers;
		NewCardLayers.Emplace(-1, *this->M_CardLayers.Find(-1));
		NewCardLayers.Emplace(0, nullptr);

		this->M_CardLayers.Empty(NewCardLayers.Num());
		this->M_CardLayers.Append(NewCardLayers);

		//更新碰撞
		this->CloseAllCardsCollision();
		this->UpdateAllCardsCollision();
	}
	else {
		this->M_CardLayers.Empty();
	}
}

void UUI_MapMeshe::SetCardLayers(TMap<int32, ACardActor*>& LastCards)
{
	//标记类型不匹配的卡片
	for (auto& CurCard : LastCards)
	{
		if (IsValid(CurCard.Value))
		{
			//如果当前卡片类型不匹配当前网格类型，则进行卡片网格类型的更新
			if (CurCard.Value->GetLineType() != this->M_ELineType)
			{
				//如果返回true表示存在更新
				if (
					this->OnCardLayersUpdate(CurCard.Value->GetLineType(), CurCard.Key, CurCard.Value)
					)
				{
					continue;
				}
			}

			//新增卡片层
			this->AddCardLayer(CurCard.Key, CurCard.Value);
			CurCard.Value->SetUIMapMesh(this);
			CurCard.Value->SetLine(this->GetLine());
			//重新放置
			this->PlayFinish(CurCard.Value, true);
		}
		else {
			//如果当前存在角色
			if (this->GetbHasPlayer() && CurCard.Key == 0)
			{
#if WITH_EDITOR
				this->M_PlayerIns;

				GAME_LOG(__FUNCTION__, TEXT("设置新的网格，此网格存在角色，卡片层得到更新"), {
					GAME_LOG_LINE(TEXT("请重新设置匹配网格类型的角色位置！！！SetActorLocation"));
					});
#endif
			}
		}
	}

	//移除之前的卡片层
	LastCards.Empty();
}

bool UUI_MapMeshe::OnCardLayersUpdate(ELineType CardLineType, int32 CardLayer, ACardActor* CardActor)
{
	//如果当前卡片是All类型，则不需要更新
	if (CardActor->GetLineType() == ELineType::All)
	{
		return false;
	}

	return true;
}

void UUI_MapMeshe::UpdateCardLocation(ACardActor* SingleCard /*= nullptr*/)
{
	if (IsValid(SingleCard))
	{
		int32 CurLayer = SingleCard->GetCardSourceData().M_CardLayer;
		SingleCard->SetActorLocation(this->GetMapMeshe()->GetActorLocation());
	}
	else {
		for (const auto& Cur : this->M_CardLayers)
		{
			if (IsValid(Cur.Value))
			{
				int32 CurLayer = Cur.Value->GetCardSourceData().M_CardLayer;
				Cur.Value->SetActorLocation(this->GetMapMeshe()->GetActorLocation());
			}
		}
	}
}

void UUI_MapMeshe::CloseAllCardsCollision()
{
	for (const auto& Cur : this->M_CardLayers)
	{
		if (IsValid(Cur.Value))
		{
			Cur.Value->SetCollisionEnable(false);
		}
	}
}

void UUI_MapMeshe::UpdateAllCardsCollision()
{
	int32 Layers[3] = { 1, 0,-1 };

	bool bHasPlayer = this->GetbHasPlayer();

	for (const auto& Layer : Layers)
	{
		//如果当前顺序到达-1，并且当前存在角色，则不会开启碰撞
		if (Layer == -1 && bHasPlayer)
		{
			return;
		}

		//查询卡片，开启碰撞
		ACardActor* const* CurCard = this->GetCardLayers().Find(Layer);

		if (CurCard)
		{
			if (Layer == 1 && bHasPlayer)
			{
#if WITH_EDITOR		
				GAME_LOG(__FUNCTION__, TEXT("网格更新碰撞，但是此网格存在角色"), {
					GAME_LOG_LINE(TEXT("当开发到角色碰撞时，请更新逻辑"));
					});
#endif
			}
			else {
				if (IsValid((*CurCard)))
				{
					(*CurCard)->SetCollisionEnable(true);
				}
			}


#if WITH_EDITOR
			if (UFVMGameInstance::GetDebug())
			{
				UGameSystemFunction::FVMLog(__FUNCTION__, FString(
					UGameSystemFunction::GetObjectName(this) +
					TEXT("开启碰撞")
				));
			}
#endif
			return;
		}
	}
}

FLine UUI_MapMeshe::GetLine() const
{
	return this->M_Line;
}

bool UUI_MapMeshe::EradicateCard()
{
	//销毁顺序
	TArray<int32> M_Card_Layer = { 5,4,2,0,1,-1,3 };

	for (auto PP = M_Card_Layer.CreateConstIterator(); PP; PP++)
	{
		return this->EradicateCardByLayer(*PP);
	}

	return false;
}

bool UUI_MapMeshe::EradicateCardByLayer(int32 CardLayer)
{
	ACardActor** CurCard = this->M_CardLayers.Find(CardLayer);

	if (CurCard)
	{
		if (IsValid(*CurCard))
		{
			(*CurCard)->KillCard(this);
			(*CurCard) = nullptr;
			this->M_CardLayers.Remove(CardLayer);
			this->CloseAllCardsCollision();
			this->UpdateAllCardsCollision();
			return true;
		}
	}

	return false;
}

void UUI_MapMeshe::SetColor(FVector _Vec, float _Alpha)
{
#if WITH_EDITOR
	UWidgetBase::SetImageColor(this->M_Gird, _Vec, _Alpha);
#endif
}

void UUI_MapMeshe::SetMapMeshe(AMapMeshe* _Meshe)
{
	this->M_MapMeshe = _Meshe;
}

void UUI_MapMeshe::SetPlayerInstance(AGamePlayer* NewPlayerIns)
{
	this->M_PlayerIns = NewPlayerIns;
	this->bPlayer = true;
}

void UUI_MapMeshe::SetLine(FLine CurLine)
{
	this->M_Line = CurLine;
}

void UUI_MapMeshe::SetCardCollisionByID(int32 ID)
{
	ACardActor** CurCard = this->M_CardLayers.Find(ID);
	if (CurCard && IsValid(*CurCard))
	{
		(*CurCard)->SetCollisionEnable(true);
	}
}

void UUI_MapMeshe::SetUIMesheOccupancy(bool value, UObject* CurObj)
{
	if (IsValid(CurObj))
	{
		if (value)
		{
			this->M_MesheDis.Emplace(UGameSystemFunction::GetObjectName(CurObj), CurObj);
		}
		else {

			this->M_MesheDis.Remove(UGameSystemFunction::GetObjectName(CurObj));

			TMap<FString, UObject*> ValidObj;
			for (const auto& Cur : this->M_MesheDis)
			{
				//如果当前对象有效，表示有对象占用网格，标注不可用
				if (IsValid(Cur.Value))
				{
					ValidObj.Emplace(Cur.Key, Cur.Value);
				}
			}
			this->M_MesheDis.Empty(ValidObj.Num());
			this->M_MesheDis.Append(ValidObj);
		}
	}

	this->CheckMapMesheOccupancy();
}

bool UUI_MapMeshe::CheckMapMesheOccupancy()
{
	if (this->M_MesheDis.Num())
	{
		for (const auto& Cur : this->M_MesheDis)
		{
			//如果当前对象有效，表示有对象占用网格，标注不可用
			if (IsValid(Cur.Value))
			{
				this->bMesheEnabled = false;
				return false;
			}
		}
	}

	this->bMesheEnabled = true;
	return true;
}

TArray<UObject*> UUI_MapMeshe::GetMapMesheOccupancyObjects()
{
	TArray<UObject*> CurObjs;
	for (const auto& Cur : this->M_MesheDis)
	{
		CurObjs.Emplace(Cur.Value);
	}
	return CurObjs;
}

bool UUI_MapMeshe::GetbUIMesheOccupancy()
{
	return this->CheckMapMesheOccupancy();
}

bool UUI_MapMeshe::GetbHasPlayer() const
{
	return this->bPlayer;
}

bool UUI_MapMeshe::GetbMapMesheUsed() const
{
	if (IsValid(this->M_MapMeshe))
	{
		return this->M_MapMeshe->GetbUse();
	}
	return false;
}

bool UUI_MapMeshe::CreateNewCard(UCardManagerComponent* CardManagerComponent)
{
	//调用卡片创建虚函数【子类也实现】
	return  this->CreateCard(
		this,
		CardManagerComponent,
		CardManagerComponent->GetCurrentSelectCardID(),
		true,
		true);
}

bool UUI_MapMeshe::CreateFreeCard(int32 CardID, int32 CardLevel) {

	//优先查询卡片管理器中的卡片数据
	FItemCard CardData;
	if (AGameMapInstance::GetGameMapCardManagerStatic()->GetCardDataByID(CardID, CardData))
	{
		CardData.M_CardPrice = 0;

		//由于是卡片管理器中的数据，如果没有指定则使用原数据
		if (CardLevel != -1)
		{
			CardData.Level = CardLevel;
		}

		return this->PlayCard(
			AGameMapInstance::GetGameMapCardManagerStatic()
			, CardData.CardActorResource.TryLoadClass<ACardActor>()
			, CardData, false);
	}



	if (GET_CACHE(Card)->GetCardData(CardID, CardData))
	{

		CardData.M_CardPrice = 0;

		if (CardLevel != -1)
		{
			CardData.Level = CardLevel;
		}
		else {
			CardData.Level = 0;
		}

		return this->PlayCard(
			AGameMapInstance::GetGameMapCardManagerStatic()
			, CardData.CardActorResource.TryLoadClass<ACardActor>()
			, CardData, false);
	}
	return false;
}

bool UUI_MapMeshe::CreateCard(
	UUI_MapMeshe* UI_MapMeshe,
	UCardManagerComponent* CardManagerComponent,
	int32 CardID,
	bool ResetCoolDown,
	bool bCheck
)
{
	//如果地形被禁止
	if (!this->CheckMapMesheOccupancy())
	{
		AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(TEXT("当前网格被占用，无法放置防御卡"));
		return false;
	}

	if (bCheck)
	{
		//如果选择了铲卡
		if (CardManagerComponent->GetbSelectEradicate())
		{
			this->EradicateCard();
			CardManagerComponent->CancelEradicate();

			AGameActorFlipbookBase* Eradicate_ = this->GetWorld()->SpawnActor<AGameActorFlipbookBase>(
				LoadClass<AGameActorFlipbookBase>(0,
					TEXT("Blueprint'/Game/Resource/BP/GameStart/VS/BPEradicate.BPEradicate_C'")
				),
				this->M_MapMesheTransform);

			Eradicate_->SetAnimationPlayEndDestroy();
			Eradicate_->SetFlipbookRotation(FRotator(0.f, 90.f, 0.f));

			switch (this->M_ELineType)
			{
			case ELineType::OnWater:
			case ELineType::Underwater: { UFVMGameInstance::PlayBGM_S_Static("PlayCardToOnWater", "ItemAudio"); }break;
			default: { UFVMGameInstance::PlayBGM_S_Static("PlayCardToGround", "ItemAudio"); }break;

			}
			return false;
		}

		//如果当前玩家选择了卡牌
		if (CardManagerComponent->GetCurrentSelectActor())
		{
			return true;
		}

		return false;
	}

	return true;
}

void UUI_MapMeshe::BoxOverlapBeginByAMapMeshe(AActor* _Actor)
{
	if (!IsValid(_Actor))
	{
		return;
	}

	//当老鼠进入网格时设置老鼠排序
	if (Cast<AMouseActor>(_Actor))
	{
		this->OnMouseEnter(Cast<AMouseActor>(_Actor));
	}
}

void UUI_MapMeshe::BoxOverlapEndByAMapMeshe(AActor* _Actor)
{
	//当老鼠进入网格时设置老鼠排序
	if (Cast<AMouseActor>(_Actor))
	{
		if (this->GetLine().Col == 0)
		{
			if (Cast<AMouseActor>(_Actor)->GetCurrentHP() > 0.f)
			{
				if (Cast<AMouseActor>(_Actor)->GetMouseLine().Row == this->M_Line.Row)
				{
					if (Cast<AMouseActor>(_Actor)->GetActorLocation().Y < this->GetMapMeshe()->GetActorLocation().Y)
					{
						Cast<AMouseActor>(_Actor)->InMapMeshe(ELineType::OnGround);
					}
				}
			}
		}
	}
}

void UUI_MapMeshe::TickMapMeshe(float DeltaTime)
{
}

AMapMeshe* UUI_MapMeshe::GetMapMeshe()
{
	return this->M_MapMeshe;
}

FVector UUI_MapMeshe::GetMapMeshCurrentTop(int32 _Col) const
{
	AMapMeshe* Meshe = this->MesheCtr->GetMapMeshCurrentTop(_Col);
	return Meshe->GetActorLocation();
}

FVector UUI_MapMeshe::GetMapMeshCurrentBottom(int32 _Col) const
{
	AMapMeshe* Meshe = this->MesheCtr->GetMapMeshCurrentBottom(_Col);
	return Meshe->GetActorLocation();
}

FVector UUI_MapMeshe::GetMapMeshCurrentLeft(int32 Row) const
{
	AMapMeshe* Meshe = this->MesheCtr->GetMapMeshCurrentLeft(Row);
	return Meshe->GetActorLocation();
}

FVector UUI_MapMeshe::GetMapMeshCurrentRight(int32 Row) const
{
	AMapMeshe* Meshe = this->MesheCtr->GetMapMeshCurrentRight(Row);
	//FVector A = Meshe->GetWidgetComponentTransform().GetLocation();
	//UE_LOG(LogTemp, Error, TEXT("%d %.2f %.2f %.2f"), Row,A.X, A.Y, A.Z);
	return Meshe->GetActorLocation();
}

ELineType UUI_MapMeshe::GetLineType() const {
	return this->M_ELineType;
}

FTransform UUI_MapMeshe::GetUIMapMesheTransform() const {
	return this->M_MapMesheTransform;
}

//bool UUI_MapMeshe::BeginPlayCard(
//	FItemCard& _Card,
//	UCardManagerComponent* _CardMComp,
//	UClass* _CardRes,
//	int32 _Price,
//	bool ResetCoolDown
//)
//{
//	//判断卡片是否重叠并且查询是否存在相同索引
//	if (_Card.M_CardOverlap && !this->TestCardLayer(_Card.M_CardLayer))
//	{
//		//判断当前卡片重叠范围的名称是否匹配			
//		//查询卡片 
//		ACardActor** _Reult = this->M_CardLayers.Find(_Card.M_CardLayer);
//
//		int32 LOverlapCardID = -1;
//
//		//如果查询成功并且这个卡片类型和当前网格类型匹配
//		if (_Reult && IsValid(*_Reult))
//		{
//			for (auto ID : _Card.M_CardOverlapID)
//			{
//				if ((*_Reult)->GetCardID() == ID)
//				{
//					//匹配成功
//					//寻找当前索引所对应的卡片(销毁)
//					(*_Reult)->KillCard();
//
//
//					//放置卡片
//					this->PlayCard(_CardMComp, _CardRes, _Card, _Price, ResetCoolDown);
//
//					return true;
//				}
//
//				LOverlapCardID = ID;
//			}
//		}
//
//		AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(
//			FString(TEXT("当前卡片需要在{")) +
//			FString::FromInt(LOverlapCardID) +
//			TEXT("} 这些卡的基础上放置")
//		);
//	}
//	else {
//		//卡片无需重叠
//		if (!_Card.M_CardOverlap && this->TestCardLayer(_Card.M_CardLayer))
//		{
//			this->PlayCard(_CardMComp, _CardRes, _Card, _Price, ResetCoolDown);
//
//			return true;
//		}
//	}
//
//	AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(TEXT("无法放置当前防御卡"));
//
//	return false;
//}

void UUI_MapMeshe::CardTypeDebug(const FString& _CardName, ELineType _LineType)
{
	if (!this->CheckMapMesheOccupancy())
	{
		AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(TEXT("当前地形被占用"));

		return;
	}

	switch (_LineType)
	{
	case ELineType::OnGround:
		AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(TEXT("当前卡片适合放在-陆地上"));
		break;
	case ELineType::Sky:
		AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(TEXT("当前卡片适合放在-空中"));
		break;
	case ELineType::OnWater:
		AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(TEXT("当前卡片适合放在-水上"));
		break;
	case ELineType::Underwater:
		AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(TEXT("当前卡片适合放在-水下"));
		break;
	case ELineType::Magma:
	{
		if (_CardName.Equals(TEXT("棉花糖")))
			AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(TEXT("当前卡片适合放在-岩浆或者填补缺失的云朵"));
		else
			AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(TEXT("当前卡片适合放在-岩浆"));
	}
	break;
	case ELineType::UnderwaterAndGround:
		AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(TEXT("当前卡片适合放在-陆地或者水下"));
		break;
	case ELineType::SkyAndGround:
		AGameMapInstance::GetGameMapVsManagerStatic()->ShowText(TEXT("当前卡片适合放在-空中或者陆地"));
		break;
	default:
		break;
	}
}

int32 UUI_MapMeshe::GetRenderLayer() const
{
	return this->M_Translucency;
}

int32 UUI_MapMeshe::GetPlayerRenderLayer() const
{
	if (this->bHasChar)
	{
		return this->M_PlayerIns->GetRenderLayer();
	}

	return this->GetRenderLayer();
}

void UUI_MapMeshe::SetRenderLayer(int32 _Translucency)
{
	this->M_Translucency = _Translucency;
	this->OnRenderLayerChanged(_Translucency);
}

void UUI_MapMeshe::OnMouseEnter(AMouseActor* Mouse)
{
	if (Mouse->GetCurrentHP() > 0.f)
	{
		if (Mouse->GetMouseLine().Row == this->M_Line.Row)
		{
			Mouse->SetRenderLayer(this->GetRenderLayer());
			Mouse->InMapMeshe(this->M_ELineType);
		}
	}
}

void UUI_MapMeshe::OnMouseEnd(AMouseActor* Mouse)
{

}
