// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/VS/MapMeshe_Cloud.h"
#include "GameStart/Flipbook/SpriteActor.h"

void AMapMeshe_Cloud::SetCloudTag(ASpriteActor* _Cloud)
{
	this->M_CloudActor = _Cloud;

	this->M_CloudActor->SetActorHiddenInGame(true);
}

void AMapMeshe_Cloud::RemoveCloudTag()
{
	this->M_CloudActor->SetActorHiddenInGame(false);
}

// Called every frame
void AMapMeshe_Cloud::GameTick(float DeltaTime)
{
	Super::GameTick(DeltaTime);

	//同步坐标
	if (IsValid(this->M_CloudActor))
	{
		this->SetActorLocation(this->M_CloudActor->GetSpriteLocation() + FVector(-15.f, 0.f, 0.f));

		if (this->M_CloudActor->GetSpriteLocation().Y < -90.f)
		{
			this->RemoveCloudTag();
			this->Destroy();
		}
	}
}

