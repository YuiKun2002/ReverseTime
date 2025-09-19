// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameStart/VS/Components/GameStartActorComponent.h"
#include "GameStart/MapLauncher/MapLauncherBase.h"
#include "MeshesLauncherComponent.generated.h"

class AMapMeshe;
class UPaperFlipbook;
class UPaperFlipbookComponent;
class UPaperSprite;
class AGameActorFlipbookBase;
class AMapLauncherBase;

//网格发射器
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FVM_API UMeshesLauncherComponent : public UGameStartActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:
	//设置网格坐标
	UFUNCTION(BlueprintCallable)
	void SetMeshs(const TArray<FLine>& _Lines);
	//设置动画资源
	UFUNCTION(BlueprintCallable)
	void SetAnimationResPath_C(TSoftClassPtr<AGameActorFlipbookBase> _AnimRes);
	//应用设置
	UFUNCTION(BlueprintCallable)
	void ApplySetting();
private:
	//需要被发射的网格
	UPROPERTY()
	TMap<AMapMeshe*, AGameActorFlipbookBase*> M_Meshes;
	//动画资源
	UPROPERTY()
	UClass* M_AnimRes = nullptr;
private:
	//是否提交申请
	UPROPERTY()
	bool M_IsbApply = false;
	//发射器
	UPROPERTY()
	AMapLauncherBase* M_Owner = nullptr;
};
