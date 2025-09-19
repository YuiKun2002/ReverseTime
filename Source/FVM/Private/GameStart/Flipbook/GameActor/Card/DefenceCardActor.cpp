// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/Flipbook/GameActor/Card/DefenceCardActor.h"

const FItemCardDefence& ADefenceCardActor::GetDefenceCardData()
{
	return this->ItemCardDefence;
}

void ADefenceCardActor::OnCardInitData(const FItemCard& NewBaseData)
{
	Super::OnCardInitData(NewBaseData);

	//初始化必要数据
	this->ItemCardDefence = this->GetCardDataObject()->GetCardDefenceData(this->CardID);
	this->ItemCardDefence.Level = this->GetCardSourceData().Level;

	if (this->ItemCardDefence.M_ECardUpGradeType == ECardUpGradeType::E_Defence_Up)
	{
		this->ItemCardDefence.M_CardHP =
			this->ItemCardDefence.M_CardHP +
			this->ItemCardDefence.M_CardHP * this->ItemCardDefence.M_DefenceUpRate *
			this->GetCardGrade() * this->ItemCardDefence.M_M_ECardUpGradeUpRate;
	}

	this->SetCardData(this->ItemCardDefence);
}
