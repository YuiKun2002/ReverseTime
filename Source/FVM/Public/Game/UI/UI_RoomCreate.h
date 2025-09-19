// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"

//#include "D:\UnrealEngine\UE_5.0\Engine\Plugins\Online\OnlineSubsystem\Source\Public\Interfaces\OnlineSessionInterface.h"

//#include "OnlineServicesInterface/Public/Online/Sessions.h"
#include "GameSystem/GameMapStructManager.h"
#include "UI_RoomCreate.generated.h"

// D:\UnrealEngine\UE_5.0\Engine\Plugins\Online
// D:\UnrealEngine\UE_5.0\Engine\Plugins\Online\OnlineSubsystem\Source\Public

/**
 * 房间创建
 */

class UImage;
class UTextBlock;
class UVerticalBox;

DECLARE_DELEGATE(FSelctDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSelectLevel, FLevelConfig, LevelConfig);

//房间
UCLASS()
class FVM_API UUI_RoomCreateRoomListItem : public UWidgetBase
{
	GENERATED_BODY()
public:

	virtual bool Initialize() override;

	void Init(class UUI_RoomCreate* NewCurRoom);

	void SetData(FString SNumber, FString SRoomName, int32 ISex);

	UFUNCTION(BlueprintCallable)
	void Select(FOnSelectLevel OnSelectLevel);

	void Cancel();

protected:

	//角色性别女的图标
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath PlayerGenderFemaleIcon;

public:

	FSelctDelegate OnSelctDelegate;

private:
	//当前角色性别
	UPROPERTY()
	UImage* SexImg = nullptr;
	//选择框
	UPROPERTY()
	UImage* SelectBoxImg = nullptr;
	//房间号
	UPROPERTY()
	UTextBlock* RoomNumberText = nullptr;
	//房间名称
	UPROPERTY()
	UTextBlock* RoomNameText = nullptr;
	//房间对象
	UPROPERTY()
	class UUI_RoomCreate* CurRoom = nullptr;
};



//创建房间
UCLASS()
class FVM_API UUI_RoomCreate : public UWidgetBase
{
	GENERATED_BODY()

public:

	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable)
	void Init(FString MapName,
		TSubclassOf<UUI_RoomCreateRoomListItem> ListItemClass,
		FOnSelectLevel OnSelectLevel
	);

	UFUNCTION()
	UUI_RoomCreateRoomListItem* Gen(FString RoomName,
		TSubclassOf<UUI_RoomCreateRoomListItem> ListItemClass);
private:
	//地图背景
	UPROPERTY()
	UImage* MapBg = nullptr;
	//默认老鼠显示
	UPROPERTY()
	UImage* MouseDefImg = nullptr;
	//显示列表
	UPROPERTY()
	UVerticalBox* ShowRoomList = nullptr;
public:
	UPROPERTY()
	TArray<UUI_RoomCreateRoomListItem*> Alls;
	UPROPERTY()
	FLevelConfig SourceData;
public:
	void Room2();
	void Room3();
	void Room4();
};
