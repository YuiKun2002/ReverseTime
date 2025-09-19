// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/Flipbook/GameStartActorBase.h"
#include "GameSystem/GameStartSubsystem.h"

// Sets default values
AGameStartActorBase::AGameStartActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameStartActorBase::BeginPlay()
{
	//获取游戏状态
	this->bSafePtr_bGameStartState = &UGameStartSubsystem::GetGameStartSubsystemStatic()->GetbGameStartState();
	this->bSafePtr_bGameOverState = &UGameStartSubsystem::GetGameStartSubsystemStatic()->GetbGameOverState();

	Super::BeginPlay();
}

// Called every frame
void AGameStartActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->bSafePtr_bGameStartState && this->bSafePtr_bGameOverState)
	{
		if (*this->bSafePtr_bGameStartState && !(*this->bSafePtr_bGameOverState))
		{
			this->GameTick(DeltaTime);
		}
		else if (*this->bSafePtr_bGameStartState && *this->bSafePtr_bGameOverState)
		{
			this->OnGameOverDelegate.Broadcast();
			this->OnGameOverDelegate.Clear();
			this->SetActorTickEnabled(false);
		}
	}
}

void AGameStartActorBase::GameTick(float DeltaTime)
{
	//game update
	this->BlueprintTick(DeltaTime);
}

void AGameStartActorBase::SetRenderLayer(int32 Layer) {
	this->GameActorLayer = Layer;
}

int32 AGameStartActorBase::GetRenderLayer() const {
	return this->GameActorLayer;
}

bool AGameStartActorBase::GetbGameOver()
{
	if (this->bSafePtr_bGameStartState && this->bSafePtr_bGameOverState)
	{
		return (*this->bSafePtr_bGameStartState && *this->bSafePtr_bGameOverState);
	}

	return true;
}
