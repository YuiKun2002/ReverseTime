// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Game/UI/UI_RoomCreate.h"
#include <Components/Image.h>
#include <Components/VerticalBox.h>
#include <Components/TextBlock.h>
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/GameConfigManager.h"
#include "GameSystem/MouseStructManager.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/Tools/GameSystemFunction.h"

bool UUI_RoomCreateRoomListItem::Initialize()
{
	Super::Initialize();


	this->SexImg = this->GetWidgetComponent<UImage>(this, TEXT("Sex"));
	this->SelectBoxImg = this->GetWidgetComponent<UImage>(this, TEXT("SelectBox"));

	this->RoomNumberText = this->GetWidgetComponent<UTextBlock>(this, TEXT("RoomNum"));
	this->RoomNameText = this->GetWidgetComponent<UTextBlock>(this, TEXT("RoomName"));

	return true;
}

void UUI_RoomCreateRoomListItem::Init(class UUI_RoomCreate* NewCurRoom)
{
	this->CurRoom = NewCurRoom;
}

void UUI_RoomCreateRoomListItem::SetData(FString SNumber, FString SRoomName, int32 ISex)
{
	this->RoomNumberText->SetText(FText::FromString(SNumber));
	this->RoomNameText->SetText(FText::FromString(SRoomName));

	//女生
	if (ISex == 0)
	{
		this->SetImageBrush(this->SexImg, this->PlayerGenderFemaleIcon.ToString());

		// TEXT("Texture2D'/Game/Resource/Texture/UI/RoomCreate/58.58'")
	}

	this->SelectBoxImg->SetVisibility(ESlateVisibility::Collapsed);
}

void UUI_RoomCreateRoomListItem::Select(FOnSelectLevel OnSelectLevel)
{
	if (IsValid(this->CurRoom))
	{
		//可优化，选择谁记录谁，谁取消，目前版本暂时不需要优化，条目不多
		for (UUI_RoomCreateRoomListItem* Cur : this->CurRoom->Alls)
		{
			Cur->Cancel();
		}
	}

	this->SelectBoxImg->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (this->OnSelctDelegate.IsBound())
	{
		this->OnSelctDelegate.Execute();

		UGameMapStructManager* Cur = GAMEINS()->GetGameMapStructManager();
		if (IsValid(Cur))
		{
			OnSelectLevel.ExecuteIfBound(Cur->LevelConfig);
		}
	}
}

void UUI_RoomCreateRoomListItem::Cancel()
{
	this->SelectBoxImg->SetVisibility(ESlateVisibility::Collapsed);
}

bool UUI_RoomCreate::Initialize()
{
	Super::Initialize();

	this->MapBg = this->GetWidgetComponent<UImage>(this, TEXT("Image_149"));
	this->MouseDefImg = this->GetWidgetComponent<UImage>(this, TEXT("DefMouseImg"));

	this->ShowRoomList = this->GetWidgetComponent<UVerticalBox>(this, TEXT("VerticalBox_23"));

	return true;
}

void UUI_RoomCreate::Init(
	FString MapName, TSubclassOf<UUI_RoomCreateRoomListItem> ListItemClass,
	FOnSelectLevel OnSelectLevel
)
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()->LoadGameMapStructManager(MapName)))
	{

		GET_PLAYERDATAINS()->GetOtherData().PlayerSelectGameMapName = MapName;

		UGameSystemFunction::SaveCurrentPlayerData(FString(TEXT("地图预备选择:") + MapName));

		//获取当前的地图管理器
		UGameMapStructManager* Cur = UFVMGameInstance::GetFVMGameInstance()->GetGameMapStructManager();
		//初始化原始地图配置数据
		this->SourceData = Cur->LevelConfig;

		//加载背景		
		this->SetImageBrush(this->MapBg, Cur->LevelConfig.LevelBGHead.ToString());

		//加载房间2
		UUI_RoomCreateRoomListItem* Cur2 = this->Gen(TEXT("【简单难度】欢乐大冒险,Let's go!"), ListItemClass);
		Cur2->OnSelctDelegate.BindUObject(this, &UUI_RoomCreate::Room2);
		//加载房间3
		UUI_RoomCreateRoomListItem* Cur3 = this->Gen(TEXT("【困难难度】关键材料掉落，掉落率增加0.5%"), ListItemClass);
		Cur3->OnSelctDelegate.BindUObject(this, &UUI_RoomCreate::Room3);
		//加载房间4
		UUI_RoomCreateRoomListItem* Cur4 = this->Gen(TEXT("【极限难度】等级限制，关键材料掉落，掉落率增加1%"), ListItemClass);
		Cur4->OnSelctDelegate.BindUObject(this, &UUI_RoomCreate::Room4);

		this->Alls.Emplace(Cur2);
		this->Alls.Emplace(Cur3);
		this->Alls.Emplace(Cur4);
		this->ShowRoomList->AddChildToVerticalBox(Cur2);
		this->ShowRoomList->AddChildToVerticalBox(Cur3);
		this->ShowRoomList->AddChildToVerticalBox(Cur4);

		//默认选择1
		Cur2->Select(OnSelectLevel);
	}
}

UUI_RoomCreateRoomListItem* UUI_RoomCreate::Gen(FString RoomName, TSubclassOf<UUI_RoomCreateRoomListItem> ListItemClass)
{
	UUI_RoomCreateRoomListItem* NewUI = CreateWidget<UUI_RoomCreateRoomListItem>(this, ListItemClass);

	NewUI->Init(this);
	NewUI->SetData(FString::FromInt(
		UGameSystemFunction::GetRandomRange(1000, 9999)
	), RoomName,
		UFVMGameInstance::GetPlayerStructManager_Static()->M_PlayerGender
	);

	return NewUI;
}

void UUI_RoomCreate::Room2()
{
	//获取当前的地图管理器
	UGameMapStructManager* Cur = GAMEINS()->GetGameMapStructManager();
	Cur->LevelConfig = this->SourceData;

	//移除忽略项目
	for (const FString& CurIg : this->SourceData.IgnoreItems)
	{
		Cur->LevelConfig.LevelItems.Remove(CurIg);
	}

	Cur->LevelConfig.MouseLevel = 1;
	Cur->LevelConfig.ItemsCountRate = 1;
}

void UUI_RoomCreate::Room3()
{
	//获取当前的地图管理器
	UGameMapStructManager* Cur = UFVMGameInstance::GetFVMGameInstance()->GetGameMapStructManager();
	Cur->LevelConfig = this->SourceData;
	Cur->LevelConfig.MouseLevel = Cur->LevelConfig.MouseLevel + 1;
	Cur->LevelConfig.ItemsCountRate = 1;
	Cur->LevelConfig.ItemsValue += 150.f * 0.05f;
}

void UUI_RoomCreate::Room4()
{
	//获取当前的地图管理器
	UGameMapStructManager* Cur = UFVMGameInstance::GetFVMGameInstance()->GetGameMapStructManager();
	Cur->LevelConfig = this->SourceData;
	Cur->LevelConfig.CardGradeMax = Cur->LevelConfig.CardGradeMin;
	Cur->LevelConfig.MouseLevel = Cur->LevelConfig.MouseLevel * 2;
	if (Cur->LevelConfig.MouseLevel < 3)
	{
		Cur->LevelConfig.MouseLevel = 3;
	}
	Cur->LevelConfig.ItemsCountRate = 1;
	Cur->LevelConfig.ItemsValue += 150.f * 0.1f;

}

