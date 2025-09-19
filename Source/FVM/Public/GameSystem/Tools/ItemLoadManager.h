// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SharedPointer.h"
#include "ItemLoadManager.generated.h"

class UWidget;
class UScrollBox;
class UUniformGridSlot;
class UUniformGridPanel;


//物品加载器的物品基础对象
struct ItemLoaderItemBase {
	virtual void* GetValue() const = 0;
	virtual ~ItemLoaderItemBase() {}
};
//物品模板
template<typename ItemType>
struct ItemLoaderItem : public ItemLoaderItemBase {
	virtual  void* GetValue() const override {
		return (void*)(ObjDataPtr.Get());
	};

	ItemLoaderItem(const TSharedPtr<ItemType>& Ptr) :ObjDataPtr(Ptr) {}
private:
	//数据对象
	TSharedPtr<ItemType> ObjDataPtr;
};
/*
	物品加载存储对象
*/
struct ItemLoaderObject {
	//自定义删除
	~ItemLoaderObject() {
		for (struct ItemLoaderItemBase*& BaseData : this->Items)
		{
			delete BaseData;
			BaseData = nullptr;
		}
	}

	template<typename ItemType>
	void Set(
		const ItemType& NewData,
		const int32& Index
	) {
		if (Index < Items.Num())
		{
			if (struct ItemLoaderItemBase*& Ref = Items[Index])
			{
				delete Ref;
				Ref = nullptr;

				Ref = new ItemLoaderItem<ItemType>(MakeShareable<ItemType>(new ItemType(NewData)));
			}
			else {
				Items[Index] = new ItemLoaderItem<ItemType>(MakeShareable<ItemType>(new ItemType(NewData)));
			}
		}
		else {
			this->Items.Emplace(
				new ItemLoaderItem<ItemType>(MakeShareable<ItemType>(new ItemType(NewData)))
			);
		}
	}

	void* Get(const int32& Index) const {
		return this->Items[Index]->GetValue();
	}

	FORCEINLINE int32 Num() const { return this->Items.Num(); }

	FORCEINLINE void RemoveEnd() {
		this->Items.RemoveAtSwap(Items.Num() - 1);
	}

private:
	TArray<
		struct ItemLoaderItemBase*
	> Items;
};


//物品加载数据块
UCLASS()
class FVM_API UItemLoadDataBlock : public UObject
{
	GENERATED_BODY()
public:
	//设置数据
	FORCEINLINE void SetValue(const TSharedPtr<ItemLoaderObject>& RefValue, const int32& NewIndex)
	{
		this->ValueRef = RefValue; this->Index = NewIndex;
	}

	//获取数据
	template<class Type = void>
	FORCEINLINE Type GetValue() const {
		return *((Type*)(ValueRef->Get(this->Index)));
	}

	//获取数组的索引值
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetValue"))
	FORCEINLINE int32 GetIndex() const { return this->GetValue<int32>(); };
private:
	TSharedPtr<ItemLoaderObject> ValueRef;
	int32 Index = 0;
};


//新增
DECLARE_DELEGATE_RetVal_TwoParams(UWidget*, FItemLoaderNewDelegate, const  UItemLoadDataBlock* const, int32);
//更新
DECLARE_DELEGATE_ThreeParams(FItemLoaderUpdateDelegate, const  UItemLoadDataBlock* const, int32, UWidget*);
//更新前
DECLARE_DELEGATE(FItemLoaderUpdateBeforeDelegate);
//加载结束
DECLARE_DELEGATE(FItemLoaderEndDelegate);

//物品加载管理器初始化数据
USTRUCT(BlueprintType)
struct FItemLoadManagerData {

	GENERATED_USTRUCT_BODY()

	//加载器行数【如果是垂直模式Row至少是2】
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LoaderRow = 2;

	//加载器的列数【如果是横向模式Col至少是2】
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LoaderCol = 2;

	//加载的最大物品数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LoadItemMaxCount = 100;

	//加载物品的延迟时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LoadItemDelayTime = 0.01f;

	/*
	启用水平横向加载模式
	1.使用横向加载模式可以调整盒体的高度->不能小于加载的高度
	2.使用竖向加载模式可以调整盒体的宽度->不能小于加载的宽度
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLoaderModeHorizontal = false;
};

/**
 * 物品加载管理器
 * 1.可以将任何TArray<>中的数据给正确的加载
 * 2.创建少量界面用于加载大量数据
 * 3.例如：一千个物品数据，用40个UI界面全面展示【此加载器完美满足需求】
 * 4.滚动组件的大小决定了一个格子的大小
 */
UCLASS()
class FVM_API UItemLoaderObject : public UObject
{

	GENERATED_BODY()

	friend class UWidgetBase;

public:
	//新建需要被加载的界面
	FItemLoaderNewDelegate OnNew;
	//更新新的数据
	FItemLoaderUpdateDelegate OnUpdate;
	//更新数据之前
	FItemLoaderUpdateBeforeDelegate OnUpdateBefore;
	//加载结束触发
	FItemLoaderEndDelegate OnLoadEnd;

public:

	//设置加载器界面到当前加载器
	UFUNCTION(BlueprintCallable)
	void SetLoaderRangeWidget(class ULoaderRangeWidget* RangeWidgetIns);

	/*
	重新设置加载的行列
	1.会重新排列物品的显示
	*/
	UFUNCTION(BlueprintCallable)
	void SetLoaderRowCol(int32 Row, int32 Col);
	//设置道具最大加载数量
	UFUNCTION(BlueprintCallable)
	void SetLoaderItemMaxCount(int32 NewCount);
	//设置当前页数,将自动跳转对应位置
	UFUNCTION(BlueprintCallable)
	void SetLoaderPage(int32 NewPage, bool bExecute = false);
	//设置滑动区的UI平面位移
	UFUNCTION(BlueprintCallable)
	void SetLoaderTransfrom(
		FVector2D NewOffset,
		FVector2D NewScale = FVector2D(1.f, 1.f),
		float Angle = 0.f
	);
	//设置物品之间的间距
	UFUNCTION(BlueprintCallable)
	void SetLoaderWidgetMargin(float UniformMargin);
	//将数组的总数存入,解析为对应索引值
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "UpdateDataTable"))
	void UpdateDataTable_ArrayIndex(const int32& ArrayNum);
	//执行加载
	UFUNCTION(BlueprintCallable)
	void Execute();
	//更新数据表(模板) C++
	template<typename AnyType>
	void UpdateDatatable(const TArray<AnyType>& SourceItems)
	{
		//当前物品总数量
		this->M_Item_Count = SourceItems.Num();

		//自动扩容
		if (this->M_Page_LoadMaxCount < this->M_Item_Count)
		{
			this->M_Page_LoadMaxCount = this->M_Item_Count;
		}

		//如果当前总数组小于已存在的数组->删除
		if (this->ItemLoaderObjectPtr->Num() > SourceItems.Num())
		{
			while (this->ItemLoaderObjectPtr->Num() != SourceItems.Num())
			{
				this->ItemLoaderObjectPtr->RemoveEnd();
			}

			if (this->M_ConditionPage_OverData)
			{
				this->M_Condition_RefreshEndPage = true;
			}
		}
		else {
			if (this->M_ConditionPage_OverData)
			{
				this->M_Condition_RefreshEndPage = true;
				this->M_ConditionPage_OverData = false;
			}
		}

		int32 LocalIndex = 0;
		for (const auto& Item : SourceItems)
		{
			this->ItemLoaderObjectPtr->Set<AnyType>(Item, LocalIndex);
			LocalIndex++;
		}
	}
	//析构销毁
	virtual void BeginDestroy() override;
public:
	/*
		通过UI索引获取对应列表的索引
		将物品在背包中的索引转换成加载器格子的索引位置
		1.输入一个索引
		返回一个当前页数所在的索引,如果不在显示范围内则返回【负数 [-1,-x]】
	*/
	UFUNCTION(BlueprintPure)
	int32 GetGridIndexByUiIndex(int32 UiIndex) const;
	/*
		获取加载器大小
		1.返回  行数 * 列数
	*/
	UFUNCTION(BlueprintPure)
	FORCEINLINE	int32 GetLoaderSize() const { return this->M_Page_CurrentColCount * this->M_Page_CurrentRowCount * 2; }
	//获取当前需要展示的数量
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetShowCount() const { return this->M_Item_CurrentShowCount; }
	//获取行
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetRow() const { return this->M_Page_CurrentRowCount; }
	//获取列
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetCol() const { return this->M_Page_CurrentColCount; }
	//获取总数量
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetTotal() const { return this->M_Item_Count; }
	//获取当前已加载的数量
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetCount() const { return this->M_Item_CurrentLoadCount; }
	//获取当前页数
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetCurrentPage() const { return this->M_Page_CurrentNum; }
	//获取页面首索引
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetPagetFirstIndex() const { return this->M_Page_CurrentNum * this->M_Page_BaseNum; }
	//获取当前位移位置
	UFUNCTION(BlueprintPure)
	FORCEINLINE FVector2D GetMainScrollTransfrom() const { return this->M_ScrollOffset; }
	//获取页面尾索引
	UFUNCTION(BlueprintPure)
	int32 GetPagetLastIndex() const;
private:
	//加载器界面
	UPROPERTY()
	class ULoaderRangeWidget* LoaderRangeWidget = nullptr;
	//加载列表的容器
	UPROPERTY()
	UScrollBox* M_Load_Table_List = nullptr;
	//加载列表的格子
	UPROPERTY()
	UUniformGridPanel* M_Load_Uniform_Grid = nullptr;
	//当前的数据
	UPROPERTY()
	UItemLoadDataBlock* DataBlock = nullptr;
	//存储区域
	TSharedPtr<struct ItemLoaderObject> ItemLoaderObjectPtr;
private:
	//滑动区的位移
	FVector2D M_ScrollOffset = FVector2D(0.f);
	//物品之间的间隔
	FVector2D M_MarginOffset = FVector2D(0.f, 0.f);

	//当前加载隐藏的道具数量
	int32 M_Hidden_Count = 0;
	//当前UI创建的数量->索引
	int32 M_CurrentCreateUINum = 0;

	//需要显示的数据起始索引
	int32 M_LoadIndex_First = 0;
	//需要显示的数据末尾索引
	int32 M_LoadIndex_Last = 0;

	//当前的页数
	int32 M_Page_CurrentNum = 0;
	//页数的基数
	int32 M_Page_BaseNum = 0;

	//当前页数最大的物品数量
	int32 M_Page_CurrentMaxCount = 0;
	//当前一行中有多少个物品(n列)
	int32 M_Page_CurrentColCount = 0;
	//最大显示多少行(n行)
	int32 M_Page_CurrentRowCount = 0;
	//能加载是最大数据->控制显示长度
	int32 M_Page_LoadMaxCount = 100;
	//当前已经加载的数量
	int32 M_Item_CurrentLoadCount = 0;
	//当需要展示的数量
	int32 M_Item_CurrentShowCount = 0;
	//当前缓存有效总数量
	int32 M_Item_Count = 0;


	//一行的行高
	float M_Page_LineHight = 0.f;
	//一个物品的宽度
	float M_Page_LineWidth = 0.f;
	//一个滚动区域可显示的最大高度
	float M_Page_ScrollMaxHight = 0.f;
	//一个滚动区域可显示的最大宽度
	float M_Page_ScrollMaxWidth = 0.f;

	//是否翻页到了最后
	bool M_ConditionPage_OverData = false;
	//刷新最后一页
	bool M_Condition_RefreshEndPage = false;
	//是否隐藏当前界面
	bool M_Condition_Widget_Hidden = false;
	//设置横向检测
	bool M_Ccroll_Condition_bHorizontal = false;
	//预刷新检测
	bool M_Ccroll_Condition_bHorizontalBeginRefresh = false;
	//记录玩家翻页时，是否是往上翻页
	float M_OnUserScrolled_Temp_UpOffset = 0.f;
private:
	//设置物品加载管理器
	void SetItemLoadManager(
		class UScrollBox* ScrollBox,
		class UUniformGridPanel* UniformGridPanel,
		const FItemLoadManagerData& ItemLoadManagerData = FItemLoadManagerData()
	);
	//当用户滑动时
	UFUNCTION()
	void OnUserScrolled(float _Value);
	//加载组合数据
	void Run(float _Offset);
	//更新数据
	void UpdateData(float _Offset);
	//刷新物品
	void RefreshItem(float _Offset);
	//创建物品
	void CreateItem();
};