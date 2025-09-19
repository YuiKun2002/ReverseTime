// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "GameSystem/Item/ItemStruct.h"
#include "GameSystem/PlayerStructManager.h"
#include "UI_PlayerShow.generated.h"

class UCanvasPanel;
class UButton;
class UImage;

/**
 * 角色形象显示界面
 */
UCLASS()
class FVM_API UUI_PlayerShow : public UWidgetBase
{
	GENERATED_BODY()

public:
	//不修改玩家数据
	UFUNCTION()
	void SetConstCurrentPlayer();
	//设置当前玩家数据
	UFUNCTION(BlueprintCallable)
	void SetCurrentPlayer(UPlayerStructManager* _CurrentPlayer);
public:
	//获取服装的限定条件
	UFUNCTION(BlueprintCallable)
	bool GetPlayerSuitConst();
	//获取是否是限定数据
	UFUNCTION()
	bool GetbConst();
	//获取服装数据
	FPlayerSuit* GetPlayerSuit();
public:
	//初始化角色形象
	UFUNCTION(BlueprintCallable)
	void InitPlayerSuit(uint8 PlayerGender = 0,FPlayerSuit LSuit = FPlayerSuit());
	//卸载角色着装
	UFUNCTION(BlueprintCallable)
	void UnInstallPlayerSuit(EPlayerEquipmentSlotPosition _LEPlayerEquipmentSlotPosition);
	//显示或者隐藏角色套装
	UFUNCTION(BlueprintCallable)
	void PlayerSuitHidden(bool _bShowSuit);
protected:	
	//服装界面
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* Player_Suit1_Panel = nullptr;
	//翅膀
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player_FlyItem = nullptr;
	//头发-后
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player_Head_Back = nullptr;
	//着装
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player_Body = nullptr;
	//头
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player_Head = nullptr;
	//眼睛
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player_eye = nullptr;
	//脸饰
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player_Face = nullptr;
	//眼镜
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player_eye_2 = nullptr;
	//头发-前
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player_Head_Front = nullptr;
	//帽子
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player_Head_Top = nullptr;

	//-角色套装---------------------------------------------------------------------------------
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//套装界面
	UCanvasPanel* Player_Suit2_Panel = nullptr;
	//套装图片
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player_Uni = nullptr;
	//-角色套装---------------------------------------------------------------------------------
private:
	//当前玩家存档
	UPROPERTY()
	UPlayerStructManager* M_CurrentPlayer = nullptr;
	//临时的服装数据据
	UPROPERTY()
	FPlayerSuit M_FPlayerSuitTemp;
	//如果为true无论怎么操作都不会影响角色存档的数据
	UPROPERTY()
	bool M_bConst = false;
};
