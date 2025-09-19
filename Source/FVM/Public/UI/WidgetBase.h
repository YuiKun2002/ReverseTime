// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h" 
#include "Blueprint/UserWidget.h"
#include "GameSystem/GameDataName.h"
#include "GameSystem/Tools/ItemLoadManager.h"
#include "GameSystem/GameUserInterfaceSubsystem.h"
#include "WidgetBase.generated.h"

#define F __FUNCTION__

//红色
#define COLOR_RED FVector(1.f,0.f,0.f)
//绿色
#define COLOR_Greeen FVector(0.f,1.f,0.f)
//蓝色
#define COLOR_BLUE FVector(0.f,0.f,1.f)
//默认颜色1 浅蓝
#define COLOR_Def FVector(0.f,1.f,1.f)
//默认颜色2 落日黄
#define COLOR_Def2 FVector(1.f, 0.f, 1.f)
//白色
#define COLOR_WHITE FVector(1.f, 1.f, 1.f)
//偏黑色
#define COLOR_DARK FVector(0.2f, 0.2f, 0.2f)
//纯黑色
#define COLOR_BLACK FVector(0.f, 0.f, 0.f)

/*
	界面基类
*/
UCLASS()
class FVM_API UWidgetBase : public UUserWidget
{
	GENERATED_BODY()

	friend class UItemLoaderObject;
	friend class UGameUserInterfaceInstance;
	friend class UGameUserInterfaceSubsystem;

public:
	//设置按钮风格
	UFUNCTION(BlueprintCallable)
	static void SetButtonStyle(
		class UButton* _ButtonComponent,
		FString _ButtonTexturePath,
		bool _bRefresh = false
	);
	//设置按钮风格
	UFUNCTION(BlueprintCallable)
	static void SetButtonStyleFromTexture(
		class UButton* _ButtonComponent,
		class UTexture2D* Texture,
		bool _bRefresh = false
	);
	//设置按钮风格
	UFUNCTION(BlueprintCallable)
	static void SetButtonStyleSoft(
		class UButton* _ButtonComponent,
		TSoftObjectPtr<class UTexture2D> Texture,
		bool _bRefresh = false
	);
	//设置一个图片的原始大小赋予盒子大小
	UFUNCTION(BlueprintCallable)
	static void SetBoxResetSize(
		class USizeBox* _USizeBoxComponent,
		FString _ButtonTexturePath
	);
	//设置一个图片的原始大小赋予盒子大小
	UFUNCTION(BlueprintCallable)
	static void SetBoxResetSizeSoft(
		class USizeBox* _USizeBoxComponent,
		TSoftObjectPtr<class UTexture2D> Texture
	);
	//设置图片纹理(bHit = 是否可以点击图片  默认false)
	UFUNCTION(BlueprintCallable)
	static void SetImageBrush(
		class UImage* _ImageComponent,
		FString _ImageTexturePath,
		FVector _Color = FVector(1.f),
		float Alph = 1.f,
		bool _ResetSize = false,
		FVector _Scale = FVector(1.f, 1.f, 0.f),
		bool _bHit = false
	);
	static void SetImageBrushByTexture(
		class UImage* _ImageComponent,
		TSoftObjectPtr<class UTexture2D> _ImageTexturePath,
		FVector _Color = FVector(1.f),
		float Alph = 1.f,
		bool _ResetSize = false,
		FVector _Scale = FVector(1.f, 1.f, 0.f),
		bool _bHit = false
	);
	//对纹理大小进行限制，如果宽或者高超过这个大小，则缩放大小
	UFUNCTION(BlueprintCallable)
	static void SetImageBrushTextureConstMaxSize(
		class UImage* ImageInstance,
		class UTexture2D* Texture,
		FVector2D XY,
		float Scale
	);
	//设置图片颜色
	UFUNCTION(BlueprintCallable)
	static void SetImageColor(
		class UImage* _ImageComponent,
		FVector _Color = FVector(1.f),
		float Alph = 1.f);
	//从自配置文件中设置画布比例
	UFUNCTION(BlueprintCallable)
	static void SetWidgetScaleFromConfig(class UWidget* _Widget_Panel);
	//将UI转换成(CanvasSlot)并且拥有Progress行为
	UFUNCTION(BlueprintCallable)
	static void SetWidgetTransformProgress(
		class UWidget* _Widget_Panel,
		float X_Position,
		float XMaxSize,
		float Alpha);
	//设置画布缩放
	UFUNCTION(BlueprintCallable)
	void SetWidgetSacle(class UWidget* _Widget, const FVector2D& _Sacle);
	//让新的父类管理此类型
	UFUNCTION(BlueprintCallable)
	void SetParentClass(UWidgetBase* PClass);
public:
	//获取父类
	UFUNCTION(BlueprintPure)
	UWidgetBase* GetParentClass();

	//获取当前界面在屏幕中的渲染层级
	UFUNCTION(BlueprintPure)
	int32 GetRenderLayerWidget();

	template <class _Cast, class T = UWidgetBase>
	_Cast* GetWidgetComponent(T* _Widget,
		const FString& _ComponentName,
		const FString& _FuncName = "")
	{
		if (!_Widget)
		{
			UE_LOG(LogTemp, Error, TEXT("[%s]:传入的_Widget是无效的"), *_FuncName);
			return nullptr;
		}

		if (_ComponentName.Equals(TEXT("")))
		{
			UE_LOG(LogTemp, Error, TEXT("你使用【空】的名称，来获取UI组件，返回nullptr"));
			return nullptr;
		}

		//获取组件
		_Cast* WidgetComponent = Cast<_Cast>(_Widget->GetWidgetFromName(FName(_ComponentName)));
		if (!WidgetComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("[%s]:%s这个名称在传入的Widget中不存在!"), *_FuncName, *_ComponentName);
			return nullptr;
		}
		return WidgetComponent;
	}
public:

	//向富文本框添加文字
	UFUNCTION(BlueprintCallable)
	static void AddTextToRichText(const FString& _Content, URichTextBlock* _RichTextBlock);

	//创建一个提示界面
	UFUNCTION(BlueprintCallable)
	static void CreateTipWidget(
		const FString& _Text,
		FVector _Color = FVector(0.f, 1.f, 1.f),
		float Alpha = 1.f
	);

	//创建一个选择提示界面
	UFUNCTION(BlueprintCallable)
	static class UUI_SelectTip* CreateSelectTipWidget(FText ContentText);

	/*
		创建一个未解锁的物品提示界面
		1.  CreateType展示的道具类型：0表示装备，1表示卡片，2表示材料，其他默认表示装备
		2.  ItemViewBlock 物品的视图数据
		3.  ContentText 显示的文本
	*/
	UFUNCTION(BlueprintCallable)
	static class UUI_NotUnlockedItemTip* CreateNotUnlockedItemTipWidget(
		int32 CreateType,
		int32 ItemID,
		int32 ItemLevel,
		FText ContentText
	);

	//生成一个物品加载器对象
	UFUNCTION(BlueprintCallable)
	class UItemLoaderObject* CreateItemLoaderObject(
		class UScrollBox* ScrollBox,
		class UUniformGridPanel* UniformGridPanel,
		const FItemLoadManagerData& ItemLoadManagerData = FItemLoadManagerData()
	);

	//创建子界面【可重复添加不做任何处理,自动添加并管理】
	UFUNCTION(BlueprintCallable)
	UWidgetBase* CreateNewChildWidget(
		TSubclassOf<class UAssetCategoryName> CategoryWidgetName,
		TSubclassOf<class UAssetCategoryName> WidgetName
	);

	//创建子界面【唯一实例，不可重复添加,自动添加并管理】
	//UFUNCTION(BlueprintCallable)
	UWidgetBase* CreateNewChildStaticWidget(
		TSubclassOf<class UAssetCategoryName> CategoryWidgetName,
		TSubclassOf<class UAssetCategoryName> WidgetName
	);
	//创建一个由此界面管理的界面【可重复添加不做任何处理】
	template <class type>
	type* CreateChildWidget(UClass* Instance) {
		type* NewWidget = CreateWidget<type>(this, Instance);
		if (Cast<UWidgetBase>(NewWidget))
		{
			NewWidget->ParentClass = this;
		}
		this->Childs.Emplace(NewWidget);
		return NewWidget;
	}

	//加载2D纹理
	UFUNCTION(BlueprintCallable)
	class UTexture2D* LoadTexture2D(const FString& _Path);

	//卸载当前界面资源
	UFUNCTION(BlueprintCallable)
	void UnloadWidget(bool bForceGC = false);

	//将组件转换成一个CanvasSlot
	UFUNCTION(BlueprintCallable)
	static UCanvasPanelSlot* WidgetToCanvasSlot(class UWidget* _Widget);
	//通过Widget加载2d纹理
	UFUNCTION(BlueprintCallable)
	static class UTexture2D* WidgetLoadTexture2D(const FString& _Path);

	//播放操作音效
	UFUNCTION(BlueprintCallable)
	void PlayOperateAudio(
		bool _bUseCancel_Audio = false,
		bool _bWindowsMouseEnterAudio = false,
		bool _bWindows = false);

	//播放操作音效->默认
	UFUNCTION(BlueprintCallable)
	void PlayOperateAudioDef();

public:
	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void BeginDestroy() override;

	//获取名字
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FString ToString();
protected:
	//返回名称
	virtual FString ToString_Implementation();
private:
	//父类对象
	UPROPERTY()
	class UWidgetBase* ParentClass = nullptr;
	//名称
	UPROPERTY()
	FName InstanceWidgetName = FName();
	//分类名称
	UPROPERTY()
	FName InstanceWidgetCategoryName = FName();
	//子类对象
	UPROPERTY()
	TSet<class UWidget*> Childs;
	//加载器对象
	UPROPERTY()
	TArray<class UItemLoaderObject*> ItemLoaderObjects;
};
