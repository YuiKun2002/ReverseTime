// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/PlayerStructBase.h"
#include "GameSystem/PlayerStructManager.h"


void FPlayerCoin::InitCoin() {
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning,
		TEXT("--------------------------------【%s】:货币初始化--------------------------------"),
		*FString(__FUNCTION__));
#endif

	MakeCoin(0, { 2000,FText::FromString(FString(TEXT("金币"))) });
	MakeCoin(1, { 600,FText::FromString(FString(TEXT("礼券"))) });
	MakeCoin(2, { 0,FText::FromString(FString(TEXT("点券"))) });
	MakeCoin(3, { 0,FText::FromString(FString(TEXT("威望"))) });

#if WITH_EDITOR
	UE_LOG(LogTemp, Warning,
		TEXT("--------------------------------【%s】:货币初始化完成--------------------------------"),
		*FString(__FUNCTION__));
#endif
}

void FPlayerCoin::MakeCoin(int32 Key, FCoin Data) {

	const FCoin* Temp = this->Coins.Find(Key);
	if (Temp)
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Warning,
			TEXT("%s：%d"),
			*FString(Temp->CoinName.ToString()), Temp->CoinValue);
#endif
	}
	else {
		this->Coins.Emplace(Key, Data);

#if WITH_EDITOR
		UE_LOG(LogTemp, Warning,
			TEXT("新增货币：%s，初始值：%d"),
			*FString(Data.CoinName.ToString()), Data.CoinValue);
#endif
	}
}

bool FPlayerCoinAdd::_Buy(FCoin* _Coin, const int32& _Num)
{
	if (!_Coin)
	{
		return false;
	}

	if (_Coin->CoinValue < 0)
		(*_Coin).CoinValue = 0;

	if (_Coin->CoinValue < _Num)
		return false;

	(*_Coin).CoinValue -= _Num;

	return true;
}
bool FPlayerCoinAdd::_Sell(FCoin* _Coin, const int32& _Num)
{
	if (!_Coin)
	{
		return false;
	}

	if (_Coin->CoinValue < 0)
		(*_Coin).CoinValue = 0;

	long long Max = _Coin->CoinValue + _Num;
	if (Max > 2147483600)
	{
		(*_Coin).CoinValue = 2147483600;
	}
	else {
		(*_Coin).CoinValue += _Num;
	}

	return true;
}

bool FPlayerCoinAdd::Buy(UPlayerStructManager* _Player, const int32& _Num, const uint8& _CoinGrade) {
	if (!_Player)
		return false;

	return _Buy(_Player->M_FPlayerCoin.GetCoinRef(_CoinGrade), _Num);
}

bool FPlayerCoinAdd::Sell(UPlayerStructManager* _Player, const int32& _Num, const uint8& _CoinGrade) {
	if (!_Player)
		return false;

	return _Sell(_Player->M_FPlayerCoin.GetCoinRef(_CoinGrade), _Num);
}

int32 FPlayerBagGirdGroup::GetBagCount(TArray<FPlayerBagGird>& _Arrays)
{
	int32 _LCount = 150;
	for (FPlayerBagGird& _bag : _Arrays)
	{
		_LCount += _bag.M_GridCount;
	}
	return _LCount;
}

int32 FPlayerBagGirdGroup::GetBagCount(uint8 _Index) {
	switch (_Index)
	{
		//卡片
	case 1:return GetBagCount(this->M_CardGrid);
		//材料
	case 2:return GetBagCount(this->M_MaterialGrid);
		//装备
	default:return GetBagCount(this->M_EquipmentGrid);
	}
}

TArray<FPlayerBagGird*> FPlayerBagGirdGroup::GetUseBagArrays() {
	TArray<FPlayerBagGird*> Items;

	for (FPlayerBagGird& Equipment : this->M_EquipmentGrid)
		if (!Equipment.M_VectorName.Equals(""))
			Items.Emplace(&Equipment);

	for (FPlayerBagGird& Card : this->M_CardGrid)
		if (!Card.M_VectorName.Equals(""))
			Items.Emplace(&Card);

	for (FPlayerBagGird& Material : this->M_MaterialGrid)
		if (!Material.M_VectorName.Equals(""))
			Items.Emplace(&Material);

	return Items;
}

void FPlayerBagGirdGroup::ClearData(FPlayerBagGird& _Grid)
{
	//背景图片
	_Grid.M_GridImagePath = "";
	//容量
	_Grid.M_GridCount = 0;
	//第几个界面
	_Grid.M_PanelNumber = -1;
	//第几个背包
	_Grid.M_BagNumber = -1;
	//Vector名称
	_Grid.M_VectorName = "";
	//道具名称
	_Grid.M_BagName = "";
}



void IPlayerUpdateClassInterface::Update_Implementation(class UPlayerStructManager* PlayerDataIns) {
	UE_LOG(LogTemp, Warning, TEXT("默认角色更新类-IPlayerUpdateClassInterface-更新"));
}
void UPlayerUpdateClass::Update_Implementation(class UPlayerStructManager* PlayerDataIns) {
	UE_LOG(LogTemp, Warning, TEXT("默认角色更新类-UPlayerUpdateClass-更新"));
}
