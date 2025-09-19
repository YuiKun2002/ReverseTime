// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetBase.h"

#include "Engine/Texture2D.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

#include "Game/UI/Tips/UI_Tip.h"
#include "Game/UI/Tips/UI_SelectTip.h"
#include "Game/UI/Tips/UI_NotUnlockedItemTip.h"

#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/GameConfigManager.h"
#include "GameSystem/Tools/GameSystemFunction.h"

#include <Components/Image.h>
#include <Components/Button.h>
#include <Components/SizeBox.h>
#include <Components/RichTextBlock.h>
#include <Kismet/KismetStringLibrary.h>
#include <Components/CanvasPanelSlot.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include "Blueprint/GameViewportSubsystem.h"

/*

1.这个游戏是哔哩哔哩赖小宇ly一人制作的单机版游戏-美食大战老鼠。

2.这个游戏是免费游戏，作为Up的一个童年游戏，还是花了一定时间来制作的，因为是一个人做没有更多时间打磨细节希望理解

3. QQ:1033082401 群1:[924537454]   群2[:978043033]
	群3:[642884960] 群4:[1082582998] 群5:[674942175]

4.最后请支持正版游戏：4399美食大战老鼠官网[https://my.4399.com/yxmsdzls/]
*/

bool UWidgetBase::Initialize()
{
	return Super::Initialize();
}

void UWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UWidgetBase::BeginDestroy()
{
	Super::BeginDestroy();

	//标记加载器
	for (UItemLoaderObject*& CurItemLoaderObject : this->ItemLoaderObjects)
	{
		if (IsValid(CurItemLoaderObject))
		{
			CurItemLoaderObject->MarkAsGarbage();
			CurItemLoaderObject = nullptr;
		}
	}

	//释放界面
	UGameUserInterfaceSubsystem::ForceUnloadWidgets(this->Childs);

	this->ParentClass = nullptr;
}

UWidgetBase* UWidgetBase::CreateNewChildWidget(
	TSubclassOf<class UAssetCategoryName> CategoryWidgetName,
	TSubclassOf<class UAssetCategoryName> WidgetName
)
{
	FName A = CategoryWidgetName.GetDefaultObject()->GetCategoryName();
	FName B = WidgetName.GetDefaultObject()->GetCategoryName();
	UWidgetBase* NewWidget = this->CreateChildWidget<UWidgetBase>(
		UGameSystemFunction::GetUserInterClassByName(
			MoveTemp(A),
			MoveTemp(B)
		)
	);

	return NewWidget;
}

UWidgetBase* UWidgetBase::CreateNewChildStaticWidget(
	TSubclassOf<class UAssetCategoryName> CategoryWidgetName,
	TSubclassOf<class UAssetCategoryName> WidgetName
)
{
	/*return this->CreateChildWidgetByName<UWidgetBase>(
		MoveTemp(CategoryWidgetName.GetDefaultObject()->GetCategoryName()),
		MoveTemp(WidgetName.GetDefaultObject()->GetCategoryName())
	);*/

	return nullptr;
}

FString UWidgetBase::ToString_Implementation()
{
	return UGameSystemFunction::GetObjectName(this);
}

void UWidgetBase::PlayOperateAudio(bool _bCancel_Audio, bool _bWindowsMouseEnterAudio, bool _bWindows)
{
	if (UFVMGameInstance::GetFVMGameInstance()->GetGameConfigManager()->M_FMusic.M_bEnableSpecialAudio)
	{
		if (_bWindowsMouseEnterAudio)
		{
#if PLATFORM_WINDOWS
			switch (UFVMGameInstance::GetFVMGameInstance()->GetGameConfigManager()->M_FMusic.M_Music_SpecialAudio)
			{
			case EConfig_Music_SpecialAudio::EEAudio_1:UFVMGameInstance::PlayBGM_S_Static(TEXT("Mouse_Enter"), TEXT("GameOperationAudio")); break;
			case EConfig_Music_SpecialAudio::EEAudio_2:UFVMGameInstance::PlayBGM_S_Static(TEXT("Mouse_Enter2"), TEXT("GameOperationAudio")); break;
			}
#endif
			return;
		}

		//播放取消按钮的音效
		if (_bCancel_Audio)
		{
			switch (UFVMGameInstance::GetFVMGameInstance()->GetGameConfigManager()->M_FMusic.M_Music_SpecialAudio)
			{
			case EConfig_Music_SpecialAudio::EEAudio_1:UFVMGameInstance::PlayBGM_S_Static(TEXT("Cancel"), TEXT("GameOperationAudio")); break;
			case EConfig_Music_SpecialAudio::EEAudio_2:UFVMGameInstance::PlayBGM_S_Static(TEXT("Cancel2"), TEXT("GameOperationAudio")); break;
			}
		}
		else {
			//播放确认按钮的音效
			switch (UFVMGameInstance::GetFVMGameInstance()->GetGameConfigManager()->M_FMusic.M_Music_SpecialAudio)
			{
			case EConfig_Music_SpecialAudio::EEAudio_1:UFVMGameInstance::PlayBGM_S_Static(TEXT("Click"), TEXT("GameOperationAudio")); break;
			case EConfig_Music_SpecialAudio::EEAudio_2:UFVMGameInstance::PlayBGM_S_Static(TEXT("Click2"), TEXT("GameOperationAudio")); break;
			}
		}

	}
	else {
		if (!_bWindowsMouseEnterAudio)
		{
			UFVMGameInstance::PlayBGM_S_Static(UFVMGameInstance::GetFVMGameInstance()->GetGameConfigManager()->M_FMusic.M_Operating_BGM, TEXT("GameOperationAudio"));
		}
	}

}

void UWidgetBase::PlayOperateAudioDef()
{
	this->PlayOperateAudio();
}


void UWidgetBase::SetButtonStyle(UButton* _ButtonComponent, FString _ButtonTexturePath, bool _bRefresh)
{
	if (_ButtonTexturePath.Equals(""))
	{
		if (UFVMGameInstance::GetDebug())
			UE_LOG(LogTemp, Error, TEXT("UWidgetBase::SetButtonStyle：纹理路径无效"));
		return;
	}

	UTexture2D* Obj = UWidgetBase::WidgetLoadTexture2D(_ButtonTexturePath);

	UWidgetBase::SetButtonStyleFromTexture(_ButtonComponent, Obj, _bRefresh);
}

void UWidgetBase::SetButtonStyleFromTexture(UButton* _ButtonComponent, UTexture2D* Texture, bool _bRefresh) {
	if (_ButtonComponent)
	{
		FButtonStyle _Style = _ButtonComponent->GetStyle();
		FSlateBrush _Normal, _Hovered, _Pressed;

		_Normal.TintColor = FSlateColor(FLinearColor(FVector4(1.f, 1.f, 1.f, 1.f)));
		_Hovered.TintColor = FSlateColor(FLinearColor(FVector4(.7f, .7f, .7f, 1.f)));
		_Pressed.TintColor = FSlateColor(FLinearColor(FVector4(.5f, .5f, .5f, 1.f)));

		_Normal.DrawAs = ESlateBrushDrawType::Image;
		_Hovered.DrawAs = ESlateBrushDrawType::Image;
		_Pressed.DrawAs = ESlateBrushDrawType::Image;

		//如果纹理有效
		if (IsValid(Texture))
		{
			const FVector2D& LSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());

			_Hovered.SetImageSize(LSize);
			_Pressed.SetImageSize(LSize);
			_Normal.SetImageSize(LSize);
		}

		_Hovered.SetResourceObject(Texture);
		_Pressed.SetResourceObject(Texture);
		_Normal.SetResourceObject(Texture);

		_Style.SetNormal(_Normal);
		_Style.SetHovered(_Hovered);
		_Style.SetPressed(_Pressed);

		_ButtonComponent->SetStyle(_Style);

		if (_bRefresh)
		{
			//刷新显示状态
			const ESlateVisibility& Curen = _ButtonComponent->GetVisibility();

			_ButtonComponent->SetVisibility(ESlateVisibility::Collapsed);

			_ButtonComponent->SetVisibility(Curen);
		}
		//UE_LOG(LogTemp, Error, TEXT("%.2f  %.2f"), _ButtonComponent->GetDesiredSize().X, _ButtonComponent->GetDesiredSize().Y);
	}
}

void UWidgetBase::SetButtonStyleSoft(UButton* _ButtonComponent, TSoftObjectPtr<class UTexture2D> Texture, bool _bRefresh /*= false*/)
{
	if (_ButtonComponent)
	{
		UTexture2D* Obj = UGameSystemFunction::LoadRes(Texture);
		if (!IsValid(Obj))
		{
			return;
		}

		FButtonStyle _Style = _ButtonComponent->GetStyle();
		FSlateBrush _Normal, _Hovered, _Pressed, _Dis;

		_Normal.TintColor = FSlateColor(FLinearColor(FVector4(1.f, 1.f, 1.f, 1.f)));
		_Hovered.TintColor = FSlateColor(FLinearColor(FVector4(.7f, .7f, .7f, 1.f)));
		_Pressed.TintColor = FSlateColor(FLinearColor(FVector4(.5f, .5f, .5f, 1.f)));
		_Dis.TintColor = FSlateColor(FLinearColor(FVector4(.3f, .3f, .3f, 1.f)));

		_Normal.DrawAs = ESlateBrushDrawType::Image;
		_Hovered.DrawAs = ESlateBrushDrawType::Image;
		_Pressed.DrawAs = ESlateBrushDrawType::Image;
		_Dis.DrawAs = ESlateBrushDrawType::Image;

		//如果纹理有效
		if (IsValid(Obj))
		{
			const FVector2D& LSize = FVector2D(Obj->GetSizeX(), Obj->GetSizeY());

			_Hovered.SetImageSize(LSize);
			_Pressed.SetImageSize(LSize);
			_Normal.SetImageSize(LSize);
			_Dis.SetImageSize(LSize);

			_Hovered.SetResourceObject(Obj);
			_Pressed.SetResourceObject(Obj);
			_Normal.SetResourceObject(Obj);
			_Dis.SetResourceObject(Obj);
		}

		_Style.SetNormal(_Normal);
		_Style.SetHovered(_Hovered);
		_Style.SetPressed(_Pressed);
		_Style.SetDisabled(_Dis);

		_ButtonComponent->SetStyle(_Style);

		if (_bRefresh)
		{
			//刷新显示状态
			const ESlateVisibility& Curen = _ButtonComponent->GetVisibility();

			_ButtonComponent->SetVisibility(ESlateVisibility::Collapsed);

			_ButtonComponent->SetVisibility(Curen);
		}
		//UE_LOG(LogTemp, Error, TEXT("%.2f  %.2f"), _ButtonComponent->GetDesiredSize().X, _ButtonComponent->GetDesiredSize().Y);
	}
}

void UWidgetBase::SetBoxResetSize(USizeBox* _USizeBoxComponent, FString _ButtonTexturePath)
{
	if (_USizeBoxComponent)
	{
		UTexture2D* LocalImg = LoadObject<UTexture2D>(0, *_ButtonTexturePath);
		if (LocalImg)
		{
			_USizeBoxComponent->SetWidthOverride(LocalImg->GetSizeX());
			_USizeBoxComponent->SetHeightOverride(LocalImg->GetSizeY());
		}
	}
}

void UWidgetBase::SetBoxResetSizeSoft(USizeBox* _USizeBoxComponent, TSoftObjectPtr<class UTexture2D> Texture)
{
	if (_USizeBoxComponent)
	{
		UTexture2D* LocalImg = UGameSystemFunction::LoadRes(Texture);
		if (LocalImg)
		{
			_USizeBoxComponent->SetWidthOverride(LocalImg->GetSizeX());
			_USizeBoxComponent->SetHeightOverride(LocalImg->GetSizeY());
		}
	}
}

void UWidgetBase::SetImageBrush(UImage* _ImageComponent, FString _ImageTexturePath, FVector _Color, float Alph, bool _ResetSize, FVector _Scale, bool _bHit)
{
	if (_ImageComponent)
	{
		if (_ImageTexturePath.Equals(""))
		{
			_ImageComponent->SetVisibility(ESlateVisibility::Collapsed);
			return;
		}

		UTexture2D* LoadImg = UWidgetBase::WidgetLoadTexture2D(_ImageTexturePath);

		if (LoadImg)
		{
			FSlateBrush Temp = _ImageComponent->GetBrush();
			Temp.DrawAs = ESlateBrushDrawType::Image;
			Temp.SetResourceObject(LoadImg);
			_ImageComponent->SetBrush(Temp);
			UWidgetBase::SetImageColor(_ImageComponent, _Color, Alph);

			if (_ResetSize) {
				Temp.ImageSize = FVector2D(LoadImg->GetSizeX() * _Scale.X, LoadImg->GetSizeY() * _Scale.Y);
				UWidgetBase::WidgetToCanvasSlot(_ImageComponent)->SetSize(FVector2D(LoadImg->GetSizeX() * _Scale.X, LoadImg->GetSizeY() * _Scale.Y));
			}

			if (!_bHit)
				_ImageComponent->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
			_ImageComponent->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UWidgetBase::SetImageBrushByTexture(UImage* _ImageComponent, TSoftObjectPtr<UTexture2D> _ImageTexturePath, FVector _Color /*= FVector(1.f)*/, float Alph /*= 1.f*/, bool _ResetSize /*= false*/, FVector _Scale /*= FVector(1.f, 1.f, 0.f)*/, bool _bHit /*= false */)
{
	UWidgetBase::SetImageBrush(_ImageComponent, _ImageTexturePath.ToString(), _Color, Alph, _ResetSize, _Scale, _bHit);
}

void UWidgetBase::SetImageBrushTextureConstMaxSize(UImage* ImageInstance, UTexture2D* Texture, FVector2D XY, float Scale)
{
	if (IsValid(ImageInstance))
	{
		if (ImageInstance->GetBrush().GetResourceObject() != Cast<UObject>(Texture))
		{
			ImageInstance->SetBrushFromTexture(Texture, true);
			FSlateBrush TempBrush = ImageInstance->GetBrush();
			if (
				TempBrush.GetImageSize().Y > XY.Y
				||
				TempBrush.GetImageSize().X > XY.X
				)
			{
				TempBrush.SetImageSize(TempBrush.GetImageSize() * Scale);
			}
		}
	}
}

void UWidgetBase::SetImageColor(UImage* _ImageComponent, FVector _Color, float Alph)
{
	if (!_ImageComponent)
		return;

	FSlateBrush TempBrush = _ImageComponent->GetBrush();

	TempBrush.TintColor = FSlateColor(FLinearColor(_Color.X, _Color.Y, _Color.Z, Alph));

	_ImageComponent->SetBrush(TempBrush);
}

UTexture2D* UWidgetBase::WidgetLoadTexture2D(const FString& _Path)
{
	FStreamableManager& LStreamLoad = UAssetManager::GetStreamableManager();

	const FSoftObjectPath& Path = _Path;

	return Cast<UTexture2D>(LStreamLoad.LoadSynchronous(Path));
}

void UWidgetBase::CreateTipWidget(const FString& _Text, FVector _Color, float Alpha)
{
	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		UUI_Tip* Tip = CreateWidget<UUI_Tip>(UFVMGameInstance::GetFVMGameInstance(), LoadClass<UUI_Tip>(0,
			TEXT("WidgetBlueprint'/Game/Resource/BP/Game/UI/UI_Tip/BPUI_Tip.BPUI_Tip_C'")));
		Tip->SetTipText(_Text);
		Tip->SetTextColor(_Color, Alpha);
		Tip->AddToViewport();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("GameIns，is nullptr"));
	}
}

UUI_SelectTip* UWidgetBase::CreateSelectTipWidget(FText ContentText) {
	UUI_SelectTip* Tip = UUI_SelectTip::CreateSelectTip();
	Tip->SetContentText(ContentText);
	return Tip;
}

UUI_NotUnlockedItemTip* UWidgetBase::CreateNotUnlockedItemTipWidget(
	int32 CreateType,
	int32 ItemID,
	int32 ItemLevel,
	FText ContentText
) {

	UUI_NotUnlockedItemTip* Tip = UUI_NotUnlockedItemTip::CreateNotUnlockedItemTip(
		CreateType,
		{ ItemID,ItemLevel,0 },
		ContentText
	);
	return Tip;
}

UItemLoaderObject* UWidgetBase::CreateItemLoaderObject(
	class UScrollBox* ScrollBox,
	class UUniformGridPanel* UniformGridPanel,
	const FItemLoadManagerData& ItemLoadManagerData
) {

	UItemLoaderObject* Object = NewObject<UItemLoaderObject>();
	Object->SetItemLoadManager(ScrollBox, UniformGridPanel, ItemLoadManagerData);
	this->ItemLoaderObjects.Emplace(Object);
	return Object;
}

void UWidgetBase::SetWidgetScaleFromConfig(UWidget* _Widget_Panel)
{
	_Widget_Panel->SetRenderScale(FVector2D(UFVMGameInstance::GetFVMGameInstance()->GetGameConfigManager()->M_GameScreenScale.M_X, UFVMGameInstance::GetFVMGameInstance()->GetGameConfigManager()->M_GameScreenScale.M_Y));
}

void UWidgetBase::SetWidgetTransformProgress(UWidget* _Widget_Panel, float X_Position, float XMaxSize, float Alpha)
{
	UCanvasPanelSlot* Panel = UWidgetLayoutLibrary::SlotAsCanvasSlot(_Widget_Panel);
	if (Panel)
	{
		Panel->SetPosition(FVector2D(X_Position, Panel->GetPosition().Y));
		Panel->SetSize(FVector2D(Alpha * XMaxSize, Panel->GetSize().Y));
	}
}

void UWidgetBase::AddTextToRichText(const FString& _Content, URichTextBlock* _RichTextBlock)
{
	if (!_RichTextBlock)
		return;

	FString SContent = _RichTextBlock->GetText().ToString() + _Content + TEXT("<br><br>");

	_RichTextBlock->SetText(
		FText::FromString(UKismetStringLibrary::Replace(
			SContent, TEXT("<br>"), TEXT("\r\n"), ESearchCase::IgnoreCase)
		));
}

UCanvasPanelSlot* UWidgetBase::WidgetToCanvasSlot(UWidget* _Widget)
{
	return UWidgetLayoutLibrary::SlotAsCanvasSlot(_Widget);
}

void UWidgetBase::SetWidgetSacle(UWidget* _Widget, const FVector2D& _Sacle)
{
	if (_Widget)
	{
		_Widget->SetRenderScale(_Sacle);
	}
}

void UWidgetBase::SetParentClass(UWidgetBase* PClass) {

	if (IsValid(this->ParentClass) && this->ParentClass != PClass)
	{
		this->ParentClass->Childs.Remove(this);
		this->ParentClass = nullptr;
	}

	this->ParentClass = PClass;
	if (IsValid(this->ParentClass) && this->ParentClass != this)
	{
		this->ParentClass->Childs.Emplace(this);

		if (this->InstanceWidgetCategoryName.IsNone())
		{
			//设置分类
			this->InstanceWidgetCategoryName = PClass->InstanceWidgetCategoryName;
		}
	}
	else {
		this->ParentClass = nullptr;
#if WITH_EDITOR
		ensureMsgf(false, TEXT("The parent class of the current type is nullptr"));
#endif
	}
}

UTexture2D* UWidgetBase::LoadTexture2D(const FString& _Path)
{
	return UWidgetBase::WidgetLoadTexture2D(_Path);
}


void UWidgetBase::UnloadWidget(bool bForceGC)
{
	if (this->IsInViewport())
	{
		this->RemoveFromParent();
	}

	if (bForceGC)
	{
		//卸载当前界面
		UGameUserInterfaceSubsystem::ForceUnloadWidget(this, true);
	}
}

UWidgetBase* UWidgetBase::GetParentClass()
{
#if WITH_EDITOR
	// ensureAlwaysMsgf
	if (!IsValid(this->ParentClass))
	{
		FString CLog = FString::Printf(
			TEXT("The parent class %s=>%s of the current type is nullptr"),
			*this->InstanceWidgetCategoryName.ToString(),
			*this->InstanceWidgetName.ToString()
		);
		UE_LOG(LogTemp, Error, TEXT("%p：%s"), this, *CLog);
		ensureMsgf(false, TEXT("%s"), *CLog);
	}

#endif
	return this->ParentClass;
}

int32 UWidgetBase::GetRenderLayerWidget() {
	if (UGameViewportSubsystem* Subsystem = UGameViewportSubsystem::Get(GetWorld()))
	{
		FGameViewportWidgetSlot ViewportSlot;
		if (bIsManagedByGameViewportSubsystem)
		{
			ViewportSlot = Subsystem->GetWidgetSlot(this);
		}
		return ViewportSlot.ZOrder;
	}
	return 0;
}

