// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "Data/MaterialDataStruct.h"
#include "UI_CardSkillTable.generated.h"

/**
 *技能书选项卡
 */
UCLASS()
class FVM_API UUI_CardSkillTable : public UWidgetBase
{
	GENERATED_BODY()

public:
	//加载数据
	UFUNCTION(BlueprintCallable)
	void LoadData();
	//初始数据
	void InitData(FSkillBookData _Data);
protected:
	//头像
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* ItemHead = nullptr;
	//进度条
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* ItemPro_Img = nullptr;
	//物品名称
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ItemName = nullptr;
	//提示
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ItemTip = nullptr;
	//等级
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ItemLV = nullptr;
	//经验
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ItemEx = nullptr;
};
