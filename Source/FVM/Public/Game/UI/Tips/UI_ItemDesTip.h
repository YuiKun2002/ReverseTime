// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include <Components/Button.h>
#include "UI_ItemDesTip.generated.h"

/**
 * 物品提示
 */

 // #include "Game/UI/Tips/UI_ItemDesTip.h"
 // WidgetBlueprint'/Game/Resource/BP/Game/UI/UI_Tip/BPUI_ItemDesTip.BPUI_ItemDesTip_C'

class UImage;
class UButton;
class USizeBox;
class UTextBlock;
class UVerticalBox;

/*
	描述提示UI
*/
UCLASS()
class FVM_API UUI_ItemDesTip : public UWidgetBase
{
	GENERATED_BODY()

public:
	//获取确认按钮代理
	FOnButtonClickedEvent& GetOkButtonDelegate();
	//获取取消按钮代理
	FOnButtonClickedEvent& GetCancelButtonDelegate();
public:

	//获取确认按钮
	UFUNCTION(BlueprintCallable)
	UButton* GetOkButtonComponent();
	//获取取消按钮
	UFUNCTION(BlueprintCallable)
	UButton* GetCancelButtonComponent();
	//获取展示列表
	UFUNCTION(BlueprintPure)
	class UVerticalBox* GetShowListBox();

	//设置隐藏属性，ok按钮和取消按钮
	UFUNCTION(BlueprintCallable)
	void SetHidden(bool bOkButt = true, bool bCancelButt = false);
	//设置确认按钮的文本
	UFUNCTION(BlueprintCallable)
	void SetOkButtonTitleText(FString _Content);
	//设置数量
	UFUNCTION(BlueprintCallable)
	void SetItemCount(int32 Count);
	//设置内容标题文本
	UFUNCTION(BlueprintCallable)
	void SetContentTitleText(FString _Content);
	//设置内容文本
	UFUNCTION(BlueprintCallable)
	void SetContentText(FString _Content);
	//设置道具图片
	UFUNCTION(BlueprintCallable)
	void SetItemHeadPath(FString _2DTexture_Path);
	//设置道具图片
	UFUNCTION(BlueprintCallable)
	void SetItemHead(class UTexture2D* Texture);

	//追加内容文本
	UFUNCTION(BlueprintCallable)
	void AddContentText(FString _Content);

	//绑定点击函数
	void BindClickFunc(UObject* Object, const FName& ClickFunc);

protected:
	//设置数量文本
	virtual void SetCountText(int32 Count);

protected:
	//标题名称
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Title = nullptr;
	//物品数量值
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ItemCount_Text = nullptr;
	//内容
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Content = nullptr;

	//确认按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Ok = nullptr;
	//取消按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Cancel = nullptr;

	//确认按钮标题名称
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* OkTitle = nullptr;


	//道具头像
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* ItemHead = nullptr;

	//显示盒子
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* ListBox = nullptr;
};
