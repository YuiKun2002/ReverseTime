// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/Flipbook/GameStartActor.h"
#include "Components/ActorComponent.h"

void AGameStartActor::BeginPlay()
{
	//绑定游戏结束的委托
	this->OnGameOverDelegate.AddDynamic(this, &AGameStartActor::GameStartActorGameOver);

	Super::BeginPlay();	
}

void AGameStartActor::GameStartActorGameOver() {
	
	//停止所有组件的Tick功能

	this->M_Actor->SetTickableWhenPaused(true);

	for (auto CurComp : this->GetComponents())
	{
		CurComp->SetTickableWhenPaused(true);
	}
}
