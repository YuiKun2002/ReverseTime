// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Item/Equipment/EquipmentBaseStruct.h"
#include "GiftBoxStruct.generated.h"


//---------------------------------------------------------------------------------------------------------
// 
//    礼盒结构开始
//---------------------------------------------------------------------------------------------------------

UENUM(BlueprintType)
enum class EGiftBox_Item_Type : uint8
{
	E_Card UMETA(DisplayName = "Card"),
	E_Equipment UMETA(DisplayName = "Equipment"),
	E_Material UMETA(DisplayName = "Material"),
	E_Coin UMETA(DisplayName = "Coin"),
};


//礼盒数据视图
USTRUCT(BlueprintType)
struct FItemViewBlockGift : public FItemViewBlock
{
	GENERATED_USTRUCT_BODY();

	//物品名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName = FText();
};


//道具模式【全部发送】
USTRUCT(BlueprintType)
struct FGiftBox_ListItemMode {

	GENERATED_USTRUCT_BODY()

	//ID 为-1表示货币模式
	//货币模式：ID：-1，Level：货币类型(0,1,2,3....)，Index：数量
	//发送的道具列表，ID：唯一编号，Level：等级，Index：数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FItemViewBlockGift> Items;
};

//道具模式【随机发送】
USTRUCT(BlueprintType)
struct FGiftBox_RandomItemMode {

	GENERATED_USTRUCT_BODY()


	//随机发送的道具列表，Key：权重分配，Value：随机挑选其中一之一
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FGiftBox_ListItemMode> RandomItems;


	//随机挑选个数[0表示全部，其他表示具体个数，如果超过总数，则直接全部发送，(_Count_符号)关联此属性，用作基础数量 ]
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RandomSelectCount = 1;

};

//道具->礼盒
USTRUCT(BlueprintType)
struct FGiftBox : public FEquipmentBase {

	GENERATED_USTRUCT_BODY()

public:

	//列表模式，直接发送道具
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGiftBox_ListItemMode ListItemMode;

	//随机模式，随机发送一个道具
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGiftBox_RandomItemMode RandomItemMode;

	//使用道具时触发命令{进行使用道具的时候触发指令}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Cmd = FString();
	//打开礼盒界面立刻执行{打开礼包界面时，运行指令}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CmdExecute = FString();
};


//---------------------------------------------------------------------------------------------------------
// 
//    礼盒结构结束
//---------------------------------------------------------------------------------------------------------

//装备-礼盒数据
USTRUCT(BlueprintType)
struct FEquipment_GiftBox_Data : public FItemBaseStructData
{
	GENERATED_USTRUCT_BODY();
public:
	//物品结构
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGiftBox M_FEquipment;
public:
	virtual bool GetSourceData(FItemBase& OutData) override {
		OutData = M_FEquipment;
		return true;
	}
};

UCLASS()
class FVM_API UGiftBoxStruct : public UObject
{
	GENERATED_BODY()
public:
	static const FString& DataTablePath;
};
