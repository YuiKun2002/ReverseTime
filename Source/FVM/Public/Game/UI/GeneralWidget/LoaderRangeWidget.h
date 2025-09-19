// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameSystem/Tools/ItemLoadManager.h"
#include "LoaderRangeWidget.generated.h"

/**
 * 加载器范围界面
 * 1.可以可视化动态的设置加载，数量，行列，和大小等数据
 * 2.是物品加载器的可视化版本
 */
UCLASS()
class FVM_API ULoaderRangeWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual bool Initialize() override;

	virtual void BeginDestroy() override;

	//获取加载器对象
	FORCEINLINE	class UItemLoaderObject* GetLoaderObject() { return this->LoaderObject; };

	//获取滚动UI
	FORCEINLINE class UScrollBox* GetExternalArea() { return this->ExternalArea; };

	//获取格子UI
	FORCEINLINE class UUniformGridPanel* GetInternalArea() { return this->InternalArea; };

public:

	//加载器基础数据
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "LoaderRangeWidget", meta = (DisplayName = "LoaderData"))
	FItemLoadManagerData ItemLoaderData;

	//物品尺寸大小预览
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "LoaderRangeWidget", meta = (DisplayName = "PreviewItemData"))
	FVector2D ViewItemSize = FVector2D(100.f);

	//物品的间距
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "LoaderRangeWidget", meta = (DisplayName = "PreviewItemData"))
	float ViewItemMargin = 0.f;

protected:

	//滚动区域
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UScrollBox* ExternalArea = nullptr;

	//内容区域
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UUniformGridPanel* InternalArea = nullptr;

	//加载器
	UPROPERTY(BlueprintReadOnly)
	class UItemLoaderObject* LoaderObject = nullptr;

private:

	bool SetSize();

};
