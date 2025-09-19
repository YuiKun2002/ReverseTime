// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Item/ItemBaseStruct.h"
#include "SynModel_Structures.generated.h"

//合成屋界面接口
UINTERFACE(MinimalAPI)
class USynthesisWidgetInterface : public UInterface
{
	GENERATED_BODY()
};
//合成屋界面接口
class FVM_API ISynthesisWidgetInterface
{
	GENERATED_BODY()

public:

	//初始化界面
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InitWidget();
	virtual void InitWidget_Implementation() {};

	//显示提示界面
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ShowHelpWidget();
	virtual void ShowHelpWidget_Implementation() {};
};


//合成屋-道具加载器结构
USTRUCT(BlueprintType)
struct FSynthesisItemLoader {

	GENERATED_USTRUCT_BODY()

	//加载器 
	UPROPERTY()
	class UItemLoaderObject* LoaderObject = nullptr;
	//格子初始化类型
	UPROPERTY()
	int32 GridType = 0;
	//格子初始化类型名称
	UPROPERTY()
	FName OperateTypeName = FName();
	//被加载的数据
	UPROPERTY()
	TMap<int32, FBaseItemSave> NewLoadItems;
	//被选择的道具
	UPROPERTY()
	TArray<FItemViewBlock> SelectBlockDatas;
};

