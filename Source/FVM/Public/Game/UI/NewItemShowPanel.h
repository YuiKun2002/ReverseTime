// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "GameSystem/Tools/ItemLoadManager.h"
#include "GameSystem/Item/Task/TaskStructBase.h"
#include "NewItemShowPanel.generated.h"

class UImage;
class UTextBlock;
class UScrollBox;
class UItemLoadDataBlock;
class UUniformGridPanel;

/**
 * 物品显示物品
 */
UCLASS()
class FVM_API UNewItemShow : public UWidgetBase
{
	GENERATED_BODY()
public:
	//初始化组件
	virtual bool Initialize() override;
	//设置数据
	void Set(const FString& ItemName, const FSoftObjectPath& ImgPath);
private:
	UPROPERTY()
	UTextBlock* Name = nullptr;
	UPROPERTY()
	UImage* Bg = nullptr;
};


/**
 * 物品显示界面
 */
UCLASS()
class FVM_API UNewItemShowPanel : public UWidgetBase
{
	GENERATED_BODY()
public:
	//加载
	void Load(const TArray<FTaskItemStruct>& Data);
protected:
	//创建任务列表
	UWidget* WidgetCreateList(const class UItemLoadDataBlock* const _Data, int32 _Index);
	//刷新任务列表
	void WidgetRefreshList(const class UItemLoadDataBlock* const _Data, int32 _Index, UWidget* _UWidget);

protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class ULoaderRangeWidget* BP_LoaderRangeWidget = nullptr;

private:
	UPROPERTY()
	TArray<FTaskItemStruct> Datas;
};
