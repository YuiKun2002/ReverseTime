// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/VS/PlayerCameraPosition.h"
#include "Components/SceneComponent.h"
#include "PaperSpriteComponent.h"

APlayerCameraPosition::APlayerCameraPosition()
{
	this->CameraLocation = CreateDefaultSubobject<USceneComponent>(TEXT("CameraLoca"));

	this->CameraLocation->SetupAttachment(this->GetRenderComponent());
}
