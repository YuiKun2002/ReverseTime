// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#include "Data/Buff/MouseGameBuff.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/GameConfigManager.h"
#include "GameStart/Flipbook/SpriteActor.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "GameStart/Flipbook/GameActor/FlyItemActor.h"
#include "GameStart/Flipbook/GameActor/MouseActor.h"

UBuffObject* UMouseGameBuff::GetNewBuffObject(
	EGameBuffTag NewTag,
	float NewBuffTime,
	UBuffDynamicProperty* Property
)
{
	switch (NewTag)
	{
	case EGameBuffTag::Freeze:
		return UBuffObject::MakeNewBuffObject<UFreezeBuffMouse>(NewTag, NewBuffTime, Property, this); break;
	case EGameBuffTag::Burn:
		return UBuffObject::MakeNewBuffObject<UBurnBuffMouse>(NewTag, NewBuffTime, Property, this); break;
	case EGameBuffTag::SlowDown:
		return UBuffObject::MakeNewBuffObject<USlowDownBuffMouse>(NewTag, NewBuffTime, Property, this); break;
	case EGameBuffTag::Solidification:
		return UBuffObject::MakeNewBuffObject<USolidificationBuffMouse>(NewTag, NewBuffTime, Property, this); break;
	case EGameBuffTag::Accelerate:
		return UBuffObject::MakeNewBuffObject<UAccelerateBuffMouse>(NewTag, NewBuffTime, Property, this); break;
	default:
		return UBuffObject::MakeNewBuffObject<UBuffObject>(NewTag, NewBuffTime, Property, this); break;
	}
	return nullptr;
}

bool UMouseGameBuff::UpdateBuffColor(UBuffObject* NewBuffObject)
{
	if (Super::UpdateBuffColor(NewBuffObject))
	{
		return true;
	}

	//恢复材质
	Cast<AMouseActor>(this->GetBuffChar())->SetMaterial(
		LoadObject<UMaterialInstance>(this,
			TEXT("MaterialInstanceConstant'/Game/Resource/BP/Martials/Mouse/MI_MouseRender.MI_MouseRender'")
		));

	return false;
}

AMouseActor* UBuffMouseObject::GetBuffChar()
{
	return Cast<AMouseActor>(this->GetGameBuff()->GetBuffChar());
}

void UCtrlBuffLinkBuffMouse::BuffExec()
{
	Super::BuffExec();

	//如果没有限制级buff
	if (!this->GetGameBuff()->GetConstBuff())
	{
		this->SetTickRate(0.f);
		this->GetBuffChar()->GetRenderComponent()->SetPlayRate(0);
	}
}

void UCtrlBuffLinkBuffMouse::BuffEnd()
{
	Super::BuffEnd();

	//如果没有限制级buff
	if (!this->GetGameBuff()->GetConstBuff())
	{
		float MouseGameBuffTickRate = Cast<UMouseGameBuff>(this->GetGameBuff())->TargetTickRate;
		//恢复Tick
		this->SetTickRate(MouseGameBuffTickRate);
		this->GetBuffChar()->GetRenderComponent()->SetPlayRate(MouseGameBuffTickRate);
	}
}

bool UCtrlBuffLinkBuffMouse::GetConstbuff()
{
	return true;
}

void UFreezeBuffMouse::BuffInit(float BuffTime)
{
	Super::BuffInit(BuffTime);

	//检测是否存在buff【触发反应】
	if (this->GetGameBuff()->GetBuffExist())
	{
		//与灼烧触发反应
		if (this->SetBuffTimeOffset(this, this->GetBuffByTag(EGameBuffTag::Burn)))
		{
			FVMLOG(__FUNCTION__, TEXT("冻结与灼烧进行触发"));
		}
	}
}

void UFreezeBuffMouse::BuffExec()
{
	Super::BuffExec();

	if (IsValid(this->IceBlock))
	{
		return;
	}

	AMouseActor* MouseAc = this->GetBuffChar();
	//创建冰块图层
	if (
		MouseAc->GetCurrentHP() > 0.f
		&&
		MouseAc->M_Proper_Condition.M_CurrentInType != ELineType::OnWater
		)
	{
		//创建冰块
		this->IceBlock = Cast<ASpriteActor>(UGameSystemFunction::CreateIceBlock(MouseAc, FVector(0.f, 0.f, -3.5f),
			MouseAc->GetRenderLayer() + 4, this->GetCurTime()));
	}
}

void UFreezeBuffMouse::BuffUpdate(UBuffDynamicProperty* Property)
{
	Super::BuffUpdate(Property);

	this->IceBlockTime = this->GetCurTime();

	Property->GetFloatProperty(
		FREEZEBUFFPRONAME, this->NewTime
	);

	if (IsValid(this->IceBlock))
	{
		this->IceBlock->SetLifeSpan(this->GetCurTime());
	}
}

void UFreezeBuffMouse::Tick(float BuffTime)
{
	Super::Tick(BuffTime);

	if (IsValid(this->IceBlock))
	{
		//当前时间
		const float& CurTTime = this->GetCurTime();

		//比例值
		float BlockRate = UKismetMathLibrary::Lerp(0.f, 1.f, CurTTime / this->IceBlockTime);

		//设置材质比例
		this->IceBlock->GetRenderComponent()->
			SetScalarParameterValueOnMaterials(
				this->IceBlock->GetRenderMaterialOpacityName()
				, BlockRate);
	}
}

void UFreezeBuffMouse::BuffEnd()
{
	Super::BuffEnd();

	if (IsValid(this->IceBlock))
	{
		//设置材质比例
		this->IceBlock->GetRenderComponent()->
			SetScalarParameterValueOnMaterials(
				this->IceBlock->GetRenderMaterialOpacityName()
				, 0.f);
	}

	UObject* Objs = nullptr;
	this->GetDynamicProperty()->GetDefObject(Objs);
	//触发减速buff
	this->GetGameBuff()->AddBuff(EGameBuffTag::SlowDown,
		this->NewTime,
		nullptr,
		Objs);
}

void UFreezeBuffMouse::BuffGC()
{
	if (IsValid(this->IceBlock))
	{
		//老鼠没有死亡的时候才会触发音效
		if (this->GetBuffChar()->GetCurrentHP() > 0.f)
		{
			//播放冻结结束的音效
			UFVMGameInstance::PlayBGM_S_Static(TEXT("Freeze_Over"), TEXT("ItemAudio"));
		}

		this->IceBlock->Destroy();
	}
}

void UFreezeBuffMouse::UpdateColor()
{
	this->GetBuffChar()->SetMaterial(
		LoadObject<UMaterialInstance>(this,
			TEXT("MaterialInstanceConstant'/Game/Resource/BP/Data/Fx/Ice/MI_IceMaterial_Inst.MI_IceMaterial_Inst'")
		));

	this->GetBuffChar()->SetFlipbookColor(COLOR_BUFF_FREEZE,
		this->GetBuffChar()->GetColorOpacity());
}

void USolidificationBuffMouse::UpdateColor()
{
	this->GetBuffChar()->SetMaterial(
		LoadObject<UMaterialInstance>(this,
			TEXT("MaterialInstanceConstant'/Game/Resource/BP/Data/Fx/Solidification/MI_SolidificationMaterial_Inst.MI_SolidificationMaterial_Inst'")
		));

	this->GetBuffChar()->SetFlipbookColor(COLOR_BUFF_SOLIDIFICATION,
		this->GetBuffChar()->GetColorOpacity());

	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		if (UFVMGameInstance::GetFVMGameInstance()->GetGameConfigManager()->M_GameEF.M_bEnable)
		{
			//播放粒子特效
			AObjectPoolClass* CurObj = Cast<AObjectPoolClass>(
				this->GetBuffChar()->GetMouseManager()->GetSolidifBuffFxObjPoolManager()->GetObjectActor()
			);
			CurObj->SetActorLocation(this->GetBuffChar()->GetActorLocation() + FVector(0.f, 0.f, 40.f));
		}
	}
}


void UBurnBuffMouse::BuffInit(float BuffTime)
{
	Super::BuffInit(BuffTime);

	//检测是否存在buff
	if (this->GetGameBuff()->GetBuffExist())
	{
		if (this->SetBuffTimeOffset(this, this->GetBuffByTag(EGameBuffTag::Freeze)))
		{
			FVMLOG(__FUNCTION__, TEXT("灼烧与冻结进行触发"));
		}
		else if (this->SetBuffTimeOffset(this, this->GetBuffByTag(EGameBuffTag::SlowDown)))
		{
			FVMLOG(__FUNCTION__, TEXT("灼烧与减速进行触发"));
		}
	}
}

void UBurnBuffMouse::BuffUpdate(UBuffDynamicProperty* Property)
{
	Super::BuffUpdate(Property);

	this->CurBuffTime = this->GetCurTime();

	//初始化触发灼烧时间
	this->GetDynamicProperty()->GetFloatProperty(BURNBUFFPRO2, this->BuffDelay);

	//初始化持续伤害
	UObject* DefObj = nullptr;
	this->GetDynamicProperty()->GetDefObject(DefObj);
	if (AFlyItemActor* FlyItem = Cast<AFlyItemActor>(DefObj))
	{
		//初始化伤害倍率
		float TempRate = 0.1f;
		this->GetDynamicProperty()->GetFloatProperty(BURNBUFFPRO3, TempRate);
		this->ATK = FlyItem->GetATK() * TempRate;
	}
	else {
		this->ATK = 0.1f;
	}

	//防止刷新时，刷新间隔导致无法触发
	if (!this->bEnable)
	{
		this->BuffDelayTime = this->BuffDelay;

		if (this->ATK > 0.f)
		{
			this->bEnable = true;
		}
	}
}

void UBurnBuffMouse::Tick(float BuffTime)
{
	if (this->bEnable)
	{
		if (this->CurBuffTime > 0.f)
		{
			this->CurBuffTime -= BuffTime;
			this->BuffDelayTime -= BuffTime;

			if (this->BuffDelayTime <= 0.f) {

				this->BuffDelayTime = this->BuffDelay;

				//触发伤害
				if (IsValid(this->GetBuffChar()) && !this->GetBuffChar()->GetMouseIsDeath())
				{
					if (this->GetBuffChar()->BeHit(this, this->ATK, EFlyItemAttackType::Def))
					{
						this->GetBuffChar()->SetbIsHurt(true);

						FVMLOG(__FUNCTION__,
							TEXT("灼烧buff：【") +
							UGameSystemFunction::GetObjectName(this->GetBuffChar()) +
							TEXT("】灼烧伤害：") +
							FString::SanitizeFloat(this->ATK)
						);
					}
				}
			}
		}
		else {
			this->bEnable = false;
		}
	}
}

void UBurnBuffMouse::UpdateColor()
{
	//其他buff
	this->GetBuffChar()->SetMaterial(
		LoadObject<UMaterialInstance>(this,
			TEXT("MaterialInstanceConstant'/Game/Resource/BP/Martials/Mouse/MI_MouseRender.MI_MouseRender'")
		));

	this->GetBuffChar()->SetFlipbookColor(COLOR_BUFF_BURN,
		this->GetBuffChar()->GetColorOpacity());

	if (IsValid(UFVMGameInstance::GetFVMGameInstance()))
	{
		if (UFVMGameInstance::GetFVMGameInstance()->GetGameConfigManager()->M_GameEF.M_bEnable)
		{
			//播放粒子特效
			AObjectPoolClass* CurObj = Cast<AObjectPoolClass>(
				this->GetBuffChar()->GetMouseManager()->GetBurnBuffFxObjPoolManager()->GetObjectActor()
			);
			CurObj->SetActorLocation(this->GetBuffChar()->GetActorLocation() + FVector(0.f, 0.f, 40.f));
		}
	}
}

void USlowDownLinkBuffMouse::SlowDownAndAccelerate()
{
	//速度配比

	UGameBuff* Cur = this->GetGameBuff();

	//设置最终的Tick速度
	float& TickR = Cast<UMouseGameBuff>(Cur)->TargetTickRate;
	TickR = 1.f;

	//如果存在减速Buff，减去速度
	if (UBuffObject* SlowDown = this->GetBuffByTag(EGameBuffTag::SlowDown))
	{
		TickR = TickR - Cast<USlowDownBuffMouse>(SlowDown)->Rate;
	}

	//如果存在加速Buff，加上速度
	if (UBuffObject* Accelerate = this->GetBuffByTag(EGameBuffTag::Accelerate))
	{
		TickR = TickR + Cast<UAccelerateBuffMouse>(Accelerate)->Rate;
	}

	//如果有限制类型buff
	if (Cur->GetConstBuff())
	{
		return;
	}

	//设置最终的播放速度
	this->SetTickRate(TickR);
	this->GetBuffChar()->GetRenderComponent()->SetPlayRate(TickR);
}

void USlowDownLinkBuffMouse::BuffEnd()
{
	Super::BuffEnd();

	//减速影响画面播放速度，因此还需反应一次
	this->SlowDownAndAccelerate();
}

void USlowDownBuffMouse::BuffInit(float BuffTime)
{
	Super::BuffInit(BuffTime);

	//检测是否存在buff
	if (this->GetGameBuff()->GetBuffExist())
	{
		if (this->SetBuffTimeOffset(this, this->GetBuffByTag(EGameBuffTag::Burn)))
		{
			FVMLOG(__FUNCTION__, TEXT("减速与灼烧进行触发"));
		}
	}
}

void USlowDownBuffMouse::BuffExec()
{
	Super::BuffExec();

	this->SlowDownAndAccelerate();
}

void USlowDownBuffMouse::BuffUpdate(UBuffDynamicProperty* Property)
{
	Super::BuffUpdate(Property);

	Property->GetFloatProperty(TICKRATEBUFF, this->Rate);
}

void USlowDownBuffMouse::UpdateColor()
{
	this->GetBuffChar()->SetMaterial(
		LoadObject<UMaterialInstance>(this,
			TEXT("MaterialInstanceConstant'/Game/Resource/BP/Data/Fx/Ice/MI_SlowDownMaterial_Inst.MI_SlowDownMaterial_Inst'")
		));

	this->GetBuffChar()->SetFlipbookColor(COLOR_BUFF_SLOWDOWN,
		this->GetBuffChar()->GetColorOpacity());
}

void UAccelerateBuffMouse::BuffExec()
{
	Super::BuffExec();

	this->SlowDownAndAccelerate();
}

void UAccelerateBuffMouse::BuffUpdate(UBuffDynamicProperty* Property)
{
	Super::BuffUpdate(Property);

	Property->GetFloatProperty(TICKRATEBUFF, this->Rate);
}

bool UAccelerateBuffMouse::GetDebuff()
{
	return false;
}

void UAccelerateBuffMouse::UpdateColor()
{
	//其他buff
	this->GetBuffChar()->SetMaterial(
		LoadObject<UMaterialInstance>(this,
			TEXT("MaterialInstanceConstant'/Game/Resource/BP/Martials/Mouse/MI_MouseRender.MI_MouseRender'")
		));

	this->GetBuffChar()->SetFlipbookColor(COLOR_BUFF_ACCELERATE,
		this->GetBuffChar()->GetColorOpacity());
}