// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "Engine/DataTable.h"
#include "UObject/Interface.h"
#include "GameSystem/GameConfigManager.h"
#include "UI_BagArrangement.generated.h"

//存储结构-背包道具类型整理数据结构
USTRUCT(BlueprintType)
struct FBagArrangementTypeData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

	//表结构的行名称表示类型

	//类型头像
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath TypeHead;

	//类型名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TypeName;

	//类型描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TypeContent;
};

/*
相同属性的基本类型
*/
class FVM_API IBagArrangementSharedInterface
{
public:
	IBagArrangementSharedInterface() {}
	virtual ~IBagArrangementSharedInterface() {}
	//设置类型
	void SetType(const int32& NewType);
protected:
	uint8 Type = 0;

};

/**
 * 物品整理项目设置菜单
 */
UCLASS()
class FVM_API UUI_BagArrangementItemMenu : public UWidgetBase, public IBagArrangementSharedInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void OnInit(bool bTop, bool bBotom);

protected:

	UFUNCTION(BlueprintCallable)
	void TopFunc();

	UFUNCTION(BlueprintCallable)
	void BottomFunc();

	UFUNCTION(BlueprintCallable)
	void MoveUpFunc();

	UFUNCTION(BlueprintCallable)
	void MoveDownFunc();

	UFUNCTION(BlueprintCallable)
	void ResetFunc();
};

/**
 * 物品整理项目类型
 */
UCLASS()
class FVM_API UUI_BagArrangementItem : public UWidgetBase, public IBagArrangementSharedInterface
{
	GENERATED_BODY()

public:
	//初始化项目
	void InitBagArrangementItem(
		class UTexture2D* Head,
		const FText& Name,
		const FText& Content
	);

protected:

	//设置项目
	UFUNCTION(BlueprintCallable)
	void SetItem(const FVector2D& Position, const FVector2D& ViewSize, float Scale);

protected:

	//类型头像
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* TypeHead = nullptr;

	//类型名称
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TypeName = nullptr;

	//类型描述内容
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TypeContent = nullptr;

	//设置按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Setting = nullptr;

};


/**
 * 物品整理设置
 */
UCLASS()
class FVM_API UUI_BagArrangement : public UWidgetBase
{
	GENERATED_BODY()

public:

	//初始化背包整理数据
	UFUNCTION(BlueprintCallable)
	void InitBagArrangement(int32 BagIndex, bool bShow);

	//整理顺序
	UFUNCTION(BlueprintCallable)
	void Arrangement(uint8 Type, int32 OperateState);

	//关闭设置界面按钮
	UFUNCTION(BlueprintPure)
	class UButton* GetCloseSetting();

	//获取类型
	UFUNCTION(BlueprintPure)
	uint8 GetFirstType();

	UFUNCTION(BlueprintPure)
	uint8 GetLastType();

protected:

	//关闭设置界面
	UFUNCTION(BlueprintCallable)
	void CloseSettingPanel();

protected:

	//滚动区域
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UScrollBox* ItemList = nullptr;

	//设置按钮
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* CloseSetting = nullptr;

private:
	void Fill(FName Name);

private:

	//总数据
	UPROPERTY()
	TMap<FName, FGamePlayerItemArrangeConfig> Maps;

	//当前名称
	UPROPERTY()
	FName CurName;

	//当前类型
	UPROPERTY()
	TArray<uint8> CurTypes;

	//第一个类型
	UPROPERTY()
	uint8 FistType = 0;

	//最后一个类型
	UPROPERTY()
	uint8 LastType = 0;
};
