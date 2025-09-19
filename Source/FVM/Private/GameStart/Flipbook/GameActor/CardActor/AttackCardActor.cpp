// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameStart/Flipbook/GameActor/CardActor/AttackCardActor.h"
#include "GameStart/Flipbook/GameActor/MouseActor.h"

const FItemCardATK& AAttackCardActor::GetATKCardData()
{
	return this->ItemCardATK;
}

void AAttackCardActor::SetCurrentMouse(AMouseActor* MouseActor) {
	this->CurAttackMouseActor = MouseActor;
}

AMouseActor* AAttackCardActor::GetCurrentMouse() {
	return this->CurAttackMouseActor;
}

void AAttackCardActor::OnCardInitData(const FItemCard& NewBaseData) {

	Super::OnCardInitData(NewBaseData);

	//初始化数据
	this->ItemCardATK = this->GetCardDataObject()->GetCardATKData(this->CardID);
	this->ItemCardATK.Level = this->GetCardSourceData().Level;

	this->InitLineTracePosition(this->ItemCardATK.M_LineTraceSettings);
	this->CalculateLineTracePosition(this->GetLineTracePosition());

#if WITH_EDITOR
	if (UFVMGameInstance::GetDebug())
	{
		UE_LOG(LogTemp,
			Error,
			TEXT("[CardActor.cpp ACardActor::SetCardData_ATK]攻击卡片属性: [基础攻击]%.2f  [攻击倍率]%.2f  [二次伤害]%.2f "),
			this->ItemCardATK.M_CardATKBase,
			this->ItemCardATK.M_CardATKRate,
			this->ItemCardATK.M_SputteringATKRate
		);
	}
#endif

	//判断是否有攻击力提升功能
	//设置基础攻击力
	this->SetATK(this->ItemCardATK.M_CardATKBase);
	this->SetCurATK(this->ItemCardATK.M_CardATKBase);

	if (this->ItemCardATK.M_ECardUpGradeType == ECardUpGradeType::E_Atk_Up)
	{
		//攻击力计算
		this->SetCurATK(
			this->GetATK() +
			(this->GetATK() / 2.f) * this->ItemCardATK.M_CardATKRate *
			((float)(this->GetCardGrade())) *
			this->ItemCardATK.M_M_ECardUpGradeUpRate);
#if WITH_EDITOR
		if (UFVMGameInstance::GetDebug())
		{
			UE_LOG(LogTemp,
				Error,
				TEXT("[CardActor.cpp ACardActor::SetCardData_ATK]攻击卡片强化技能:|| [最终攻击力](%.2f) || 计算:[BaseATK (%.2f)] + [BaseATK / 2 (%.2f)] * [ATKRate (%.2f)] * [Grade (%d)] * [UpGradeRate (%.2f)]"),
				this->GetCurATK(), this->GetATK(),
				(this->GetATK() / 2.f),
				this->ItemCardATK.M_CardATKRate,
				this->GetCardGrade(),
				this->ItemCardATK.M_M_ECardUpGradeUpRate
			);
		}
#endif
	}


	//攻击速度提升  //设置攻击速度
	this->SetAttackSpeed(this->ItemCardATK.M_ShootSpeed);
	this->SetCurAttackSpeed(this->ItemCardATK.M_ShootSpeed);

	if (this->ItemCardATK.M_ECardSkillType == ECardSkillType::E_ATTACKSPEED)
	{
		this->SetCurAttackSpeed(
			this->ItemCardATK.M_ShootSpeed - this->ItemCardATK.M_ShootSpeed *
			UGameSystemFunction::GetPlayerSkillBooksLv(this->ItemCardATK.M_CardSkillBookName) * 0.1f
		);
#if WITH_EDITOR
		if (UFVMGameInstance::GetDebug())
		{
			FString Content = FString(TEXT("当前防御卡[") +
				this->ItemCardATK.ItemName + TEXT("]提升攻击速度=>") +
				FString::SanitizeFloat(this->GetCurrentAttackSpeed()));

			UGameSystemFunction::FVMLog(__FUNCTION__, Content);
		}
#endif
	}

	//设置投射延迟和投射个数
	this->SetAttackCount(this->ItemCardATK.M_ProjectionItemCount);
	this->SetAttackDelay(this->ItemCardATK.M_FirstShootSpeed, this->ItemCardATK.M_ShootDelay);
	this->SetCardData(this->ItemCardATK);
}
