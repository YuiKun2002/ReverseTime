// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/UI_BagArrangement.h"
#include <Components/Image.h>
#include <Components/Button.h>
#include <Components/TextBlock.h>
#include <Components/ScrollBox.h>
#include "GameSystem/GameConfigSubsystem.h"

void IBagArrangementSharedInterface::SetType(const int32& NewType) {
	this->Type = NewType;
}

void UUI_BagArrangementItemMenu::TopFunc() {
	GET_UI_CAST(UUI_BagArrangement, PlayerBag, Arrangement)->Arrangement(this->Type, 0);
}

void UUI_BagArrangementItemMenu::BottomFunc() {
	GET_UI_CAST(UUI_BagArrangement, PlayerBag, Arrangement)->Arrangement(this->Type, 1);
}

void UUI_BagArrangementItemMenu::MoveUpFunc() {
	GET_UI_CAST(UUI_BagArrangement, PlayerBag, Arrangement)->Arrangement(this->Type, 2);
}

void UUI_BagArrangementItemMenu::MoveDownFunc() {
	GET_UI_CAST(UUI_BagArrangement, PlayerBag, Arrangement)->Arrangement(this->Type, 3);
}

void UUI_BagArrangementItemMenu::ResetFunc() {
	GET_UI_CAST(UUI_BagArrangement, PlayerBag, Arrangement)->Arrangement(this->Type, -1);
}

void UUI_BagArrangementItem::InitBagArrangementItem(
	UTexture2D* Head,
	const FText& Name,
	const FText& Content
) {
	//设置头像
	this->SetImageBrushTextureConstMaxSize(this->TypeHead, Head, FVector2D(0.f, 60.f), 0.7f);
	this->TypeName->SetText(Name);
	this->TypeContent->SetText(Content);
}

void UUI_BagArrangementItem::SetItem(const FVector2D& Position, const FVector2D& ViewSize, float Scale) {

	UUI_BagArrangement* Ag = GET_UI_CAST(UUI_BagArrangement, PlayerBag, Arrangement);
	Ag->GetCloseSetting()->SetVisibility(ESlateVisibility::Visible);

	UUI_BagArrangementItemMenu* Menu = GET_UI_CAST(UUI_BagArrangementItemMenu, PlayerBag, ArrangementSetting);
	//类型
	Menu->SetType(this->Type);

	//初始化顺序
	if (Ag->GetFirstType() == this->Type)
	{
		Menu->OnInit(true, false);
	}
	else if (Ag->GetLastType() == this->Type) {
		Menu->OnInit(false, true);
	}
	else {
		Menu->OnInit(false, false);
	}

	// ViewSize视口大小无需 * 缩放比例
	// 界面才需要 缩放比例
	if (Position.Y + 363.f * Scale > ViewSize.Y)
	{
		Menu->SetPositionInViewport(
			FVector2D(
				Position.X,
				ViewSize.Y - 363.f * Scale
			)
		);
	}
	else {
		Menu->SetPositionInViewport(
			Position
		);
	}

	Menu->AddToViewport();
}

template<uint8 EnumMax>
void InitData(
	const FName& Name,
	TMap<FName, FGamePlayerItemArrangeConfig>& MapRef,
	FGamePlayerItemArrangeConfig* Config
) {
	if (Config)
	{
		//有排序数据
		if (Config->Types.Num())
		{
			//更新排序数据,补全不齐的类型
			uint8 Begin = 0U;
			for (; Begin < EnumMax; Begin += 1U)
			{
				if (!Config->Types.Contains(Begin))
				{
					Config->Types.Emplace(Begin);
				}
			}
		}
	}
	else {
		FGamePlayerItemArrangeConfig NewConfig;
		//新增类型
		uint8 Begin = 0U;
		for (; Begin < EnumMax; Begin += 1U)
		{
			NewConfig.Types.Emplace(Begin);
		}

		MapRef.Emplace(Name, NewConfig);
	}
}

void UUI_BagArrangement::InitBagArrangement(int32 BagIndex, bool bShow) {

	//UI


	UGameConfigSubsystem* Sys = UGameConfigSubsystem::GetGameConfigSubsystemStatic();
	if (!IsValid(Sys))
	{
		return;
	}

	Sys->GetGameConfigInstance()->GetPlayerItemArrangeConfig(this->Maps);

	switch (BagIndex)
	{
	case 1: {
		this->CurName = GET_DEF_NAME(Data, CardArrange);
		InitData<(uint8)ECardType::E_Max>(this->CurName, this->Maps, this->Maps.Find(this->CurName));
	}break;
	case 2: {
		this->CurName = GET_DEF_NAME(Data, MaterialArrange);
		InitData<(uint8)EMaterialType::E_Max>(this->CurName, this->Maps, this->Maps.Find(this->CurName));
	}break;
	default: {
		this->CurName = GET_DEF_NAME(Data, EquipArrange);
		InitData<(uint8)EEquipment::E_Max>(this->CurName, this->Maps, this->Maps.Find(this->CurName));
	}break;
	}

	Sys->GetGameConfigInstance()->SetPlayerItemArrangeConfig(this->Maps);

	//如果显示，则不需要进行保存
	if (bShow)
	{
		this->Fill(CurName);
		this->AddToViewport();
	}
	else {
		Sys->SaveLocalGameConfigInstance();
	}
}

void UUI_BagArrangement::Arrangement(uint8 Type, int32 OperateState) {

	uint8 Index = this->CurTypes.Find(Type);

	if (Index == INDEX_NONE)
	{
		return;
	}

	switch (OperateState)
	{
		//置顶
	case 0: {
		if (Index == 0)
		{
			return;
		}

		TArray<uint8> NewArray = { Type };
		this->CurTypes.Remove(Type);
		NewArray.Append(this->CurTypes);
		this->CurTypes = MoveTempIfPossible(NewArray);
	}break;
		  //置底
	case 1: {
		if (Index == this->CurTypes.Num() - 1)
		{
			return;
		}

		this->CurTypes.Remove(Type);
		this->CurTypes.Emplace(Type);
	}break;
		  //向上
	case 2: {
		if (Index == 0)
		{
			return;
		}

		//交换
		uint8 CurType = this->CurTypes[Index - 1];
		this->CurTypes[Index - 1] = Type;
		this->CurTypes[Index] = CurType;
	}break;
		  //向下
	case 3: {
		if (Index == this->CurTypes.Num() - 1)
		{
			return;
		}

		uint8 CurType = this->CurTypes[Index + 1];
		this->CurTypes[Index + 1] = Type;
		this->CurTypes[Index] = CurType;
	}break;
		  //重置
	default: {
		this->CurTypes.Sort();
	}break;
	}

	//设置新的类型顺序
	FGamePlayerItemArrangeConfig* Config = this->Maps.Find(this->CurName);
	if (Config)
	{
		Config->Types = this->CurTypes;
	}

	UGameConfigSubsystem* Sys = UGameConfigSubsystem::GetGameConfigSubsystemStatic();
	Sys->GetGameConfigInstance()->SetPlayerItemArrangeConfig(this->Maps);
	Sys->SaveLocalGameConfigInstance();

	//重新加载列表
	this->Fill(this->CurName);

	//关闭界面
	this->CloseSettingPanel();
}

UButton* UUI_BagArrangement::GetCloseSetting() {
	return this->CloseSetting;
}

uint8 UUI_BagArrangement::GetFirstType() {
	return this->FistType;
}

uint8 UUI_BagArrangement::GetLastType() {
	return this->LastType;
}

void UUI_BagArrangement::CloseSettingPanel() {
	this->CloseSetting->SetVisibility(ESlateVisibility::Collapsed);
	GET_UI(PlayerBag, ArrangementSetting)->RemoveFromParent();
	GET_UI(PlayerBag, ArrangementSetting)->UnloadWidget(true);
}

void UUI_BagArrangement::Fill(FName Name) {
	//初始化滚动列表
	//数据表   EquipArrange  CardArrange  MaterialArrange

	UDataTable* TEMPDATATABLE = UGameDataSubsystem::GetGameDataSubsystemStatic()->
		GetAsset()->GetDataByName_Imp(GET_MOVE_CATEGORYNAME(Data))->GetDataByName(Name);
	DataTableAssetData<FBagArrangementTypeData> Data(TEMPDATATABLE);

	//获取数据
	TMap<FName, FBagArrangementTypeData> Datas = Data.GetDatas();

	FGamePlayerItemArrangeConfig* Config = this->Maps.Find(Name);
	if (Config)
	{

		//设置类型
		if (Config->Types.Num())
		{
			this->FistType = Config->Types[0];

			this->LastType = Config->Types[Config->Types.Num() - 1];
		}

		this->CurTypes = Config->Types;
		int32 Index = 0;
		int32 ListCount = this->ItemList->GetChildrenCount();
		for (int32 Type : Config->Types)
		{
			FBagArrangementTypeData* TypeData = Datas.Find(FName(FString::FromInt(Type)));
			if (TypeData)
			{

				//创建UI
				UUI_BagArrangementItem* Item = nullptr;
				if (Index < ListCount)
				{
					Item = Cast<UUI_BagArrangementItem>(this->ItemList->GetChildAt(Index));
				}
				else {
					Item = CREATE_UI(this, UUI_BagArrangementItem, PlayerBag, ArrangementItem);
					this->ItemList->AddChild(Item);
				}

				Item->SetType(Type);
				Item->InitBagArrangementItem(
					Cast<UTexture2D>(TypeData->TypeHead.TryLoad()),
					TypeData->TypeName,
					TypeData->TypeContent
				);
			}

			Index++;
		}
	}
}
