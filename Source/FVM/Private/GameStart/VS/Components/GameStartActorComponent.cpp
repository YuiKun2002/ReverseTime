// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/VS/Components/GameStartActorComponent.h"
#include "GameSystem/GameStartSubsystem.h"
// Sets default values for this component's properties
UGameStartActorComponent::UGameStartActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGameStartActorComponent::BeginPlay()
{
	Super::BeginPlay();

	//绑定游戏结束时
	UGameStartSubsystem::GetGameStartSubsystemStatic()->OnGameStartSubsystemGameOverDelegate.AddDynamic(
		this, &UGameStartActorComponent::GameOver
	);
	//获取游戏状态
	this->bSafePtr_bGameStartState = &UGameStartSubsystem::GetGameStartSubsystemStatic()->GetbGameStartState();
	this->bSafePtr_bGameOverState = &UGameStartSubsystem::GetGameStartSubsystemStatic()->GetbGameOverState();
}


// Called every frame
void UGameStartActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (*this->bSafePtr_bGameStartState && !(*this->bSafePtr_bGameOverState))
	{
		this->GameTick(DeltaTime);
	}
}

void UGameStartActorComponent::GameTick(float DeltaTime)
{

}

bool UGameStartActorComponent::GetbGameOver()
{
	return (
		*this->bSafePtr_bGameStartState
		&&
		*this->bSafePtr_bGameOverState
		);
}

void UGameStartActorComponent::GameOver(bool bGameWin)
{
	this->SetTickableWhenPaused(true);
}
