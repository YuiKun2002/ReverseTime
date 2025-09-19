// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "Data/GameBuff.h"
#include "GameSystem/GameDataSubsystem.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "GameStart/Flipbook/GameActorFlipbookBase.h"
//引入老鼠buff系统
#include "Data/Buff/MouseGameBuff.h"

void UGameBuff::BuffInit()
{
	//初始化默认buff数据
	if (this->GameBuffInforBaseData.CurBuffs.Num() == 0)
	{
		if (!IsValid(UGameDataSubsystem::GetGameDataSubsystemStatic()))
		{
			return;
		}


		//需要初始化GameVsCache缓存，否则无法加载缓存中的数据表，GameVS建议新建子系统解决
		//GetGameDataAssetCache<UGameDataAssetCache>(MoveTemp(GameDataNameAsset::GameDataAsset_GlobalAsset_GameVS::GlobalAsset_GameVS));

		//初始化buff默认资产
		UDataTable* Tab = UGameDataSubsystem::GetGameDataSubsystemStatic()->
			GetGameDataAssetDataTableByName_Imp(
				MoveTemp(GameDataNameAsset::GameDataAsset_GlobalAsset_GameVS::GlobalAsset_GameVS),
				MoveTemp(GameDataNameAsset::GameDataAsset_GlobalAsset_GameVS::Buff)
			);

		TArray<FGameBuffInforAssetDataTable> Datas;
		DataTableAssetData<FGameBuffInforAssetDataTable>DataTables(Tab);
		DataTables.GetValues(Datas);

		for (const auto& TempData : Datas)
		{
			this->GameBuffInforBaseData = TempData.BaseBuffAsset;
			break;
		}
	}
}

UGameBuff* UGameBuff::MakeGameBuff(UObject* NewBuffChar, EGameBuffCharTag NewBuffTag)
{
	UGameBuff* NewBuff = nullptr;//NewObject<UGameBuff>();

	switch (NewBuffTag)
	{
		//case EGameBuffCharTag::Card:NewBuff = NewObject<UCardGameBuff>(); break;
	case EGameBuffCharTag::Mouse:NewBuff = NewObject<UMouseGameBuff>(); break;
	default:NewBuff = NewObject<UGameBuff>(); break;
	}

	//初始化角色
	NewBuff->BuffChar = NewBuffChar;
	//初始化buff类型
	NewBuff->BuffTag = NewBuffTag;
	//初始化buff
	NewBuff->BuffInit();

	return NewBuff;
}

void UGameBuff::AddBuffBySubclass(
	EGameBuffTag NewTag,
	float NewBuffTime,
	const TSubclassOf<UBuffDynamicProperty>& Property,
	UObject* OwnerObject
)
{
	if (Property)
	{
		this->AddBuff(NewTag, NewBuffTime,
			UDynamicProperty::MakeDynamicPropertyByClass(Property, false), OwnerObject
		);
	}
	else {
		this->AddBuff(NewTag, NewBuffTime,
			nullptr, OwnerObject
		);
	}
}

void UGameBuff::AddBuff(
	EGameBuffTag NewTag,
	float NewBuffTime,
	UBuffDynamicProperty* Property,
	UObject* OwnerObject
)
{
	UBuffObject** CurBuff = this->CurBuffs.Find(NewTag);
	UBuffObject* NewBuffObj = nullptr;
	if (CurBuff)
	{
		/*
			之前已经存在的buff，再次执行
		*/
		if (IsValid(*CurBuff))
		{
			//更新buff持续时间
			if ((*CurBuff)->CurTime < NewBuffTime)
			{
				(*CurBuff)->CurTime = NewBuffTime;
				if (IsValid(Property))
				{
					Property->SetDefObject(OwnerObject);
					(*CurBuff)->BuffUpdate(Property);
				}
				else {
					UBuffDynamicProperty* NewProperty = this->GetDefBuffPropertyByTag(NewTag);
					NewProperty->SetDefObject(OwnerObject);
					(*CurBuff)->BuffUpdate(NewProperty);
				}
				(*CurBuff)->BuffExec();
				//新增buff
				NewBuffObj = (*CurBuff);
			}
		}
		else {
			return;
		}
	}
	else {
		/*
			新增buff，列表中不存在的buff
		*/
		this->OnExecuteBuffDelegate.ExecuteIfBound(NewTag, NewBuffTime);
		if (NewBuffTime > 0.f)
		{
			UBuffDynamicProperty* BuffProperty = Property;
			//如果属性无效，重新构造属性
			if (!IsValid(BuffProperty))
			{
				BuffProperty = this->GetDefBuffPropertyByTag(NewTag);
			}

			if (UFVMGameInstance::GetDebug())
			{
				FString PointStr = FString::Printf(TEXT("[Ptr：%p]"), BuffProperty);

				UGameSystemFunction::FVMLog(__FUNCTION__, FString(
					UGameSystemFunction::GetObjectName(BuffProperty) + PointStr
				));
			}

			//添加buff
			BuffProperty->SetDefObject(OwnerObject);
			UBuffObject* NewBuff = this->GetNewBuffObject(NewTag, NewBuffTime, BuffProperty);
			//初始化buff
			NewBuff->BuffInit(NewBuffTime);
			//初始化之后，判断是否可以触发buff
			if (NewBuff->bTriggerBuff)
			{
				NewBuff->BuffUpdate(BuffProperty);
				NewBuff->BuffExec();
				//设置限制buff的存在
				if (NewBuff->GetConstbuff())
				{
					this->bConstBuff = true;
				}
				NewBuffObj = NewBuff;
			}
			//不管有没有触发，都会加入列表，buffend由管理器来触发
			this->CurBuffs.Emplace(NewTag, NewBuff);
		}
		else {
			//如果添加了全新buff，但是最终结果是0，则表示无事发生
			return;
		}
	}
	this->OnBuffExecuteFinishedDelegate.ExecuteIfBound(this);
	//更新buff的颜色
	this->UpdateBuffColor(NewBuffObj);
}

void UGameBuff::AddBuffInfor(FGameBuffInfor NewBuff)
{
	for (const auto& Cur : NewBuff.CurBuffs)
	{
		TSubclassOf<UBuffDynamicProperty> Sub = Cur.Value.CurBuffProperty;
		if (Sub)
		{
			this->AddBuffBySubclass(Cur.Key, Cur.Value.CurBuffTime, *Sub);
		}
		else {
			this->AddBuffBySubclass(
				Cur.Key, Cur.Value.CurBuffTime,
				TSubclassOf<UBuffDynamicProperty>()
			);
		}
	}
}

void UGameBuff::ClearBuffs()
{
	for (auto& Cur : this->CurBuffs)
	{
		Cur.Value->BuffGC();
	}
	this->CurBuffs.Empty();

	AGameActorFlipbookBase* Char = Cast<AGameActorFlipbookBase>(this->GetBuffChar());
	if (IsValid(Char))
	{
		Char->GetRenderComponent()->SetPlayRate(1.f);
	}
	this->UpdateBuffColor(nullptr);
}

void UGameBuff::EndBuffs()
{
	for (const auto& Cur : this->CurBuffs)
	{
		this->SetBuffCurTime(Cur.Key, 0.f);
	}
}

void UGameBuff::DebugLog(const FString& ActorName, EGameBuffTag NewTag, bool Tirgger)
{
	//打印
	auto LLog = [&](const FString& ActorName, const FString& A, const FString& B) {
		if (Tirgger)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s：%s"), *ActorName, *A);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("~~~%s：%s"), *ActorName, *B);
		}
		};

	switch (NewTag)
	{
	case EGameBuffTag::Accelerate: LLog(ActorName, TEXT("触发加速"), TEXT("加速结束~~~~")); break;
	case EGameBuffTag::Burn: LLog(ActorName, TEXT("触发灼烧"), TEXT("灼烧结束~~~~")); break;
	case EGameBuffTag::Solidification: LLog(ActorName, TEXT("触发凝固"), TEXT("凝固结束~~~~")); break;
	case EGameBuffTag::Freeze: LLog(ActorName, TEXT("触发冻结"), TEXT("冻结结束~~~~")); break;
	case EGameBuffTag::SlowDown: LLog(ActorName, TEXT("触发减速"), TEXT("减速结束~~~~")); break;
	default:
		LLog(ActorName, TEXT("触发【其他buff】"), TEXT("【其他buff】结束~~~~"));
		break;
	}
}

void UGameBuff::UpdateBuff(const float& DeltaTime)
{
	//更新buff时间
	for (auto& CurBuffObj : this->CurBuffs)
	{
		CurBuffObj.Value->Tick(DeltaTime);
		CurBuffObj.Value->CurTime -= DeltaTime;
		if (CurBuffObj.Value->CurTime <= 0.f)
		{
			this->CurEndBuffs.Emplace(CurBuffObj.Key, CurBuffObj.Value);
		}
	}

	//消耗结束的buff
	if (this->CurEndBuffs.Num())
	{
		//添加还存在的buff，替换之前的buff列表
		TMap<EGameBuffTag, UBuffObject*> NewBuffs;
		//刷新是否还存在限制类型的buff
		bool bConstBuffExist = false;
		for (auto CurBuffObj = this->CurBuffs.CreateConstIterator(); CurBuffObj; ++CurBuffObj)
		{
			if (CurBuffObj->Value->CurTime > 0.f)
			{
				if (CurBuffObj->Value->GetConstbuff())
				{
					bConstBuffExist = true;
				}
				NewBuffs.Emplace(CurBuffObj->Key, CurBuffObj->Value);
			}
		}

		//设置是否存在限制类型的buff
		this->bConstBuff = bConstBuffExist;
		//重新拷贝buff
		this->CurBuffs = NewBuffs;
		//重新更新buff的颜色
		this->UpdateBuffColor(nullptr);
		//如果不存在任何buff，则恢复翻书动画的Tick更新频率
		if (this->CurBuffs.Num() == 0)
		{
			AGameActorFlipbookBase* Char = Cast<AGameActorFlipbookBase>(this->GetBuffChar());
			if (IsValid(Char))
			{
				Char->GetRenderComponent()->SetPlayRate(1.f);
			}
		}

		//依次执行结束的buff
		for (auto& CurBuffObj : this->CurEndBuffs)
		{
			CurBuffObj.Value->BuffEnd();
			this->OnExitBuffDelegate.ExecuteIfBound(CurBuffObj.Key);
			CurBuffObj.Value->BuffGC();
		}
		//当前动作全部执行完毕
		this->OnBuffExecuteFinishedDelegate.ExecuteIfBound(this);
		//清理内容
		this->CurEndBuffs.Empty();
	}
}

bool UGameBuff::UpdateBuffColor(UBuffObject* NewBuffObject)
{
	if (IsValid(NewBuffObject))
	{
		NewBuffObject->UpdateColor();
		return true;
	}

	//触发最后一个buff对象
	TArray<UBuffObject*> BuffObjs;
	this->CurBuffs.GenerateValueArray(BuffObjs);
	if (BuffObjs.Num())
	{
		BuffObjs.Last()->UpdateColor();
		return true;
	}

	AGameActorFlipbookBase* Char = Cast<AGameActorFlipbookBase>(this->GetBuffChar());
	if (IsValid(Char))
	{
		Char->SetFlipbookColor(
			FVector(1.f, 1.f, 1.f
			), Char->GetFlipbookColor().A);
	}

	return false;
}

void UGameBuff::RemoveBuff(EGameBuffTag NewTag)
{
	this->SetBuffCurTime(NewTag, 0.f);
}

bool UGameBuff::GetBuffExist() const
{
	return (CurBuffs.Num() > 0);
}

bool UGameBuff::GetBuffExistByTag(EGameBuffTag NewTag)
{
	if (IsValid(this->GetBuff(NewTag)))
	{
		return true;
	}

	return false;
}

bool UGameBuff::GetConstBuff() const
{
	return this->bConstBuff;
}

UBuffObject* UGameBuff::GetBuff(EGameBuffTag NewBuffTag)
{
	UBuffObject** CurBuff = this->CurBuffs.Find(NewBuffTag);
	if (CurBuff && IsValid(*CurBuff))
	{
		return ((*CurBuff)->GetCurTime() > 0.f) ? *CurBuff : nullptr;
	}
	return nullptr;
}

float UGameBuff::GetTickRate() const
{
	return this->UpdateTickRate;
}

UObject* UGameBuff::GetBuffChar()
{
	return this->BuffChar;
}

UBuffObject* UGameBuff::GetNewBuffObject(EGameBuffTag NewTag, float NewBuffTime, UBuffDynamicProperty* Property)
{
	return NewObject<UBuffObject>();
}

void UGameBuff::SetBuffCurTime(const EGameBuffTag& CurTag, const float& NewTime)
{
	UBuffObject** CurBuffObj = this->CurBuffs.Find(CurTag);
	if (CurBuffObj && IsValid(*CurBuffObj))
	{
		(*CurBuffObj)->CurTime = NewTime;
	}
}

UBuffDynamicProperty* UGameBuff::GetDefBuffPropertyByTag(const EGameBuffTag& NewTag)
{
	FGameBuffData* TemData = this->GameBuffInforBaseData.CurBuffs.Find(NewTag);
	if (TemData && TemData->CurBuffProperty)
	{
		return UDynamicProperty::MakeDynamicPropertyByClass<
			UBuffDynamicProperty
		>(TemData->CurBuffProperty);
	}

	return UDynamicProperty::MakeDynamicPropertyByClass<
		UBuffDynamicProperty
	>();
}

void UBuffDynamicProperty::SetDefObject(UObject* Value)
{
	this->SetObjectProperty(GAMEBUFF_VAR_DEFOBJECT, Value);

	if (IsValid(Value))
	{
		this->OnDefObjectInit(Value);
	}
}

void UBuffDynamicProperty::GetDefObject(UObject*& Value)
{
	this->GetObjectProperty(GAMEBUFF_VAR_DEFOBJECT, Value);
}

void UBuffObject::BuffInit(float BuffTime)
{
	//如果当前buff时间小于新的时间，刷新buff的时间
	if (this->CurTime < BuffTime)
	{
		this->CurTime = BuffTime;
	}
}

void UBuffObject::BuffUpdate(UBuffDynamicProperty* Property) {
	if (this->DynamicProperty == Property)
	{
		return;
	}
	this->DynamicProperty = Property;
}

void UBuffObject::BuffExec() {}

void UBuffObject::Tick(float BuffTime) {}

void UBuffObject::BuffEnd() { this->CurTime = 0.f; }

bool UBuffObject::GetDebuff() { return true; }

bool UBuffObject::GetConstbuff() { return false; }

void UBuffObject::UpdateColor() {}

void UBuffObject::BuffGC() {}

class UGameBuff* UBuffObject::GetGameBuff() { return this->CurBuffObject; }

UBuffObject* UBuffObject::GetBuffByTag(EGameBuffTag NewTag)
{
	if (this->CurTag == NewTag)
	{
		if (this->CurTime > 0.f)
		{
			return this;
		}
		return nullptr;
	}

	UBuffObject** CurBuffObj = this->CurBuffObject->CurBuffs.Find(NewTag);
	if (CurBuffObj && IsValid(*CurBuffObj))
	{
		if ((*CurBuffObj)->CurTime > 0.f)
		{
			return (*CurBuffObj);
		}
	}

	return nullptr;
}

void UBuffObject::SetTickRate(float NewRate)
{
	if (NewRate < 0.f)
	{
		this->CurBuffObject->UpdateTickRate = 0.f;
	}
	else {
		this->CurBuffObject->UpdateTickRate = NewRate;
	}
}

bool UBuffObject::SetBuffTimeOffset(UBuffObject* Buff1, UBuffObject* Buff2)
{
	if (IsValid(Buff1) && IsValid(Buff2))
	{
		if (Buff1->CurTime > 0.f && Buff2->CurTime > 0.f)
		{
			float TempTime = Buff1->CurTime - Buff2->CurTime;
			if (TempTime > 0.f)
			{
				Buff2->CurTime = 0.f;
				Buff2->bTriggerBuff = false;
				Buff1->CurTime = TempTime;
			}
			else if (TempTime < 0.f) {
				Buff1->CurTime = 0.f;
				Buff1->bTriggerBuff = false;
				Buff2->CurTime = TempTime * -1.f;
			}
			else {
				Buff1->CurTime = 0.f;
				Buff1->bTriggerBuff = false;
				Buff2->CurTime = 0.f;
				Buff2->bTriggerBuff = false;
			}
			return true;
		}
	}
	return false;
}

void UBuffObject::SetBuffCurTime(const EGameBuffTag& NewTag, const float& NewTime)
{
	if (this->CurTag == NewTag)
	{
		if (NewTime <= 0.f)
		{
			this->CurTime = 0.f;
			this->bTriggerBuff = false;
		}
		else {
			this->CurTime = NewTime;
		}
		return;
	}
	this->CurBuffObject->SetBuffCurTime(NewTag, NewTime);
}

void UBuffObject::SetForceCurTime(const float& NewTime)
{
	this->CurTime = NewTime;
}

float UBuffObject::GetCurTime() const
{
	return this->CurTime;
}

const EGameBuffTag& UBuffObject::GetCurTag() const
{
	return this->CurTag;
}

UBuffDynamicProperty* UBuffObject::GetDynamicProperty()
{
	return this->DynamicProperty;
}