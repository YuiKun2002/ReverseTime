// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "Data/MaterialDataStruct.h"
#include "GameSystem/Item/ItemBaseGridFunc.h"
#include "MaterialFuncSkillBook.generated.h"

/**
 * 材料函数-技能书
 */
UCLASS()
class FVM_API UMaterialFuncSkillBook : public UItemBaseGridFunc
{
	GENERATED_BODY()

public:

	virtual void Execute(
		class UItemGridBase* GridPtr,
		struct FItemBaseStructData* ItemBaseDataPtr
	) override;

	void Preview();

protected:

	//使用
	UFUNCTION()
	void Use();

	//使用得到的技能书数据
	bool UseSkillBookData(FCardSkillBookMaterial _Data, int32& _BooksCount);

private:

	UPROPERTY()
	FCardSkillBooks_Data Data;

	UPROPERTY()
	class UUI_PlayerBagMaterialGrid* Grid = nullptr;

};
