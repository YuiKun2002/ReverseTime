// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/GameDataSubsystem.h"
#include "GameSystem/Item/Equipment/GiftBoxStruct.h"
#include "GameSystem/Item/Equipment/EquipmentBaseStruct.h"
#include "MailDataAssetCache.generated.h"

//邮件结构
USTRUCT(BlueprintType)
struct FMail : public FItemBase {

	GENERATED_USTRUCT_BODY()

public:
	//邮件发送人
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MailSendPlayer;
	//邮件标题(输入的指令)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString M_MailTitle;
	//邮件主题(显示的标题)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MailTheme;
	//邮件内容
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MailContent;
	//邮件奖励显示
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGiftBox_ListItemMode ShowList;
	//发送指令
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Cmd;
};

//装备-邮件
USTRUCT(BlueprintType)
struct FEquipment_FMail_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//物品结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMail M_FEquipment;
public:
	bool GetSourceData(FItemBase& OutData) override
	{
		OutData = this->M_FEquipment;
		return true;
	}

	virtual void SetID(int32 ID) {
		FItemBaseStructData::SetID(ID);
		this->M_FEquipment.ItemID = ID;
	}
};

/**
 * 邮件缓存
 */
UCLASS()
class FVM_API UMailDataAssetCache : public UGameDataAssetCache
{
	GENERATED_BODY()

public:

	//获取 邮件数据
	UFUNCTION(BlueprintPure)
	void GetMailData(int32 ID, FMail& OutMailData,bool& bResult);

	//获取邮件的名称
	UFUNCTION(BlueprintPure)
	void GetAllMailData(TMap<int32, FName>& OutData);

private:

	//邮件初始化
	bool MailInit();

	//获取邮件数据表
	class UDataTable* GetMailDataTable();

	//邮件
	DataTableAssetData<FEquipment_FMail_Data> Maila;
	TMap<int32, FMail> MailDataTable;
	TMap<int32, FName> MailName;
};
