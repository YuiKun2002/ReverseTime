// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/MapLauncher/MapLauncherBase.h"
#include "GameStart/VS/Components/MesheControllComponent.h"
#include "GameStart/Flipbook/SpriteActor.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "GameStart/VS/Components/MesheControllComponent.h"


void AMapLauncherBase::SetLevelConfig(FLevelConfig _data)
{
	this->LevelConfig = _data;
}

void AMapLauncherBase::SetMapConfig(FGameMapStruct _data)
{
	this->GameMapStruct = _data;
}

void AMapLauncherBase::SetMapMesheControllComponent(UMesheControllComponent* _UMesheControllComponent)
{
	this->M_UMesheControllComponent = _UMesheControllComponent;
}

FLevelConfig& AMapLauncherBase::GetLevelConfig()
{
	return this->LevelConfig;
}

FGameMapStruct& AMapLauncherBase::GetMapStructData()
{
	return this->GameMapStruct;
}

AMapMeshe* AMapLauncherBase::GetMapMeshes(const FLine& _Line)
{
	return this->M_UMesheControllComponent->GetMapMesh(_Line.Row,_Line.Col);
}

FLine AMapLauncherBase::GetMapMeshLine()
{
	return this->M_UMesheControllComponent->GetMapMeshRowAndCol();
}

