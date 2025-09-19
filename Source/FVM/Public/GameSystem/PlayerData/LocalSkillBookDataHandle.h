// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/PlayerDataSubsystem.h"
#include "GameSystem/Item/Material/CardSkillBookMaterialStruct.h"
#include "LocalSkillBookDataHandle.generated.h"

USTRUCT(BlueprintType)
struct FSkillBookDataMap {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FSkillBookData> SkillBookDatas;
};

/**
 * 本地角色技能书数据
 */
UCLASS()
class FVM_API ULocalSkillBookDataSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	//生成技能书数据列表
	void GenSkillBookListForWidget(ESkillBookTipCategory Type, TArray<FSkillBookData>& OutData);
	//添加技能书[返回实际扣除数量]
	bool AddSkillBookList(const FCardSkillBookMaterial& NewData, int32& OutCount);
	//检测当前技能书是否可以使用
	bool CheckSkillBook(const FCardSkillBookMaterial& NewData);
	//检测[玩家登录进入界面之前PlayerUpdate]
	void Update();
public:
	//技能书数据(存储信息表)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FSkillBookData> SkillBookBaseDatas;
private:
	void UpdateSKillBook();
};


/**
 * 本地角色技能书数据句柄
 */
UCLASS()
class FVM_API ULocalSkillBookDataHandle : public ULocalDataHandle
{
	GENERATED_BODY()

public:
	//创建新的数据
	virtual bool CreateData_Implementation() override;
	//获取存储Tag
	virtual const FString GetTag_Implementation() const override { return TEXT("SkillBook"); };
};
