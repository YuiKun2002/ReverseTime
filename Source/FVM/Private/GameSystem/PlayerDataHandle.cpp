// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/PlayerDataHandle.h"
#include "GameSystem/TaskSubsystem.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameSystem/Item/Task/TaskStructBase.h"
#include "GameSystem/PlayerData/LocalSkillBookDataHandle.h"
#include "GameSystem/PlayerData/LocalEquipmentDataHandle.h"

ULocalPlayerDataHandle* ULocalPlayerDataHandle::MakeLocalPlayerDataHandle(FString PlayerName, int32 PlayerGender)
{
	ULocalPlayerDataHandle* NewHandle = NewObject<ULocalPlayerDataHandle>();
	NewHandle->CreateLocalName(PlayerName);
	NewHandle->LocalPlayerGender = PlayerGender;
	return NewHandle;
}

ULocalDataHandle* ULocalPlayerDataHandle::CreatePlayerDataPageHandle(EPlayerDataConfigPage Config) {
	ULocalDataHandle* HD = nullptr;
	switch (Config)
	{
	case EPlayerDataConfigPage::SkillBook:
	{
		//技能书类型->食谱，结晶，技能书等等等
		HD = NewObject<ULocalSkillBookDataHandle>();
	}break;
	case EPlayerDataConfigPage::Equip: {
		//武器，宝石，等细节存储
		HD = NewObject<ULocalEquipmentDataHandle>();
	}break;
	}

	if (IsValid(HD))
	{
		HD->CreateLocalName(this->GetLocalNameBase());
	}

	return HD;
}

ULocalDataHandle* ULocalPlayerDataHandle::LoadPlayerDataPageHandle(EPlayerDataConfigPage Config) {

	ULocalDataHandle* HD = this->CreatePlayerDataPageHandle(Config);

	if (IsValid(HD))
	{
		if (HD->LocalFileIsExist())
		{
			HD->Execute_LoadData(HD, TEXT("存档加载！"));
		}
		else {
			HD->Execute_CreateData(HD);
		}

		return HD;
	}

	return nullptr;
}

bool ULocalPlayerDataHandle::RemovePlayerDataPageHandle(EPlayerDataConfigPage Config) {
	ULocalDataHandle* HD = this->CreatePlayerDataPageHandle(Config);
	if (IsValid(HD))
	{
		return HD->Execute_RemoveData(HD);
	}
	return false;
}

bool ULocalPlayerDataHandle::LoadData_Implementation(const FString& LoadLogMsg)
{
	this->SaveGameObject = nullptr;
	this->SaveGameObject = this->LoadLocalSaveGame(LoadLogMsg);

	if (IsValid(this->SaveGameObject))
	{

#if WITH_EDITOR
		UE_LOG(LogTemp, Error, TEXT("Ptr：%p，Name：%s，角色存档加载"),
			this->SaveGameObject, *FString(__FUNCTION__));
#endif

		//设置角色数据实例
		UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->SetPlayerDataInstance(
			Cast<UPlayerStructManager>(this->SaveGameObject)
		);
		//设置本地角色句柄
		UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->SetLocalPlayerDataHandle(this);

		return true;
	}

	return false;
}

bool ULocalPlayerDataHandle::CreateData_Implementation()
{
	//输入的文字是空的
	if (this->GetLocalNameBase().IsEmpty())
	{
		UWidgetBase::CreateTipWidget(TEXT("名称不能为空"), FVector(0.f, 1.f, 1.f));
		return false;
	}

	if (this->GetLocalNameBase().Len() > 8)
	{
		UWidgetBase::CreateTipWidget(TEXT("名字长度不能超过8位"), FVector(0.f, 1.f, 1.f));
		return false;
	}

	//查询本地存档是否存在
	if (this->LocalFileIsExist())
	{
		return this->Execute_LoadData(this, __FUNCTION__ + FString(TEXT("加载角色存档")));
	}
	else {

		//创建新的角色存档
		UPlayerStructManager* NewPlayerIns =
			UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->CreateEmptyPlayerInstance();
		//设置角色数据实例
		UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->SetPlayerDataInstance(NewPlayerIns);
		//设置本地角色句柄
		UPlayerDataSubsystem::GetPlayerDataSubsystemStatic()->SetLocalPlayerDataHandle(this);

		NewPlayerIns->M_PlayerName = this->GetLocalNameBase();
		NewPlayerIns->M_PlayerGender = LocalPlayerGender;
		NewPlayerIns->M_GameVersion = UFVMGameInstance::GetGameVersion();
		//更新角色其他数据
		NewPlayerIns->GetOtherData() = FPlayerOtherData(6, FString(TEXT("美味岛秘方")));
		//更新卡片选择配置
		NewPlayerIns->GetCardSelectConfig().Emplace(
			NewPlayerIns->GetOtherData().PlayerSelectCardConfigPageName,
			FPlayerSelectListConfig({})
		);
		//设置本地游戏实例对象
		this->SaveGameObject = NewPlayerIns;

		//初始化全部任务
		NewPlayerIns->AllTasks.InitTaskToNewPlayer(
			GET_CACHE(Task)->GetAllTaskData()
		);
		//初始化每日任务，周任务，月任务
		NewPlayerIns->AllTasks.InitDayTask(NewPlayerIns);

		//发送数据[装备]
		UGameSystemFunction::SendItemToBag(176, 0, 1, 1u, false);//全装备礼盒
		UGameSystemFunction::SendItemToBag(175, 0, 1, 1u, false);//全材料礼盒

		//发送数据[卡片]
		UGameSystemFunction::SendItemToBag(0, 0, 1, 2u, false);//小笼包
		UGameSystemFunction::SendItemToBag(80, 0, 1, 2u, false);//小火炉
		UGameSystemFunction::SendItemToBag(98, 0, 1, 2u, false);//土司面包
		//保存存档
		this->Execute_SaveData(this, __FUNCTION__ + FString(TEXT("保存角色存档")));
		return true;
	}

	return false;
}

bool ULocalPlayerDataHandle::RemoveData_Implementation() {

	uint8 Max = (uint8)EPlayerDataConfigPage::Max;
	for (uint8 Type = 0U; Type < Max; Type += 1U)
	{
		this->RemovePlayerDataPageHandle((EPlayerDataConfigPage)Type);
	}

	return  UGameplayStatics::DeleteGameInSlot(this->GetLocalName(), 0);
}