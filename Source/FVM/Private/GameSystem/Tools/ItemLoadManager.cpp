// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Tools/ItemLoadManager.h"
#include "UI/WidgetBase.h"
#include <Components/ScrollBox.h>
#include <Components/CanvasPanelSlot.h>
#include <Components/UniformGridSlot.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Components/UniformGridPanel.h>
#include "GameSystem/Tools/GameSystemFunction.h"
#include "Game/UI/GeneralWidget/LoaderRangeWidget.h"

void UItemLoaderObject::BeginDestroy()
{
	Super::BeginDestroy();

	//解绑滚动事件
	if (IsValid(this->M_Load_Table_List))
	{
		this->M_Load_Table_List->OnUserScrolled.RemoveAll(this);
	}

	//销毁格子对象内存
	if (IsValid(this->M_Load_Uniform_Grid))
	{
		this->M_Load_Uniform_Grid->ClearChildren();
	}

	this->LoaderRangeWidget = nullptr;
	this->M_Load_Table_List = nullptr;
	this->M_Load_Uniform_Grid = nullptr;
	this->DataBlock = nullptr;
	this->ItemLoaderObjectPtr.Reset();
}

void UItemLoaderObject::OnUserScrolled(float _Value)
{
	//横向检测
	if (this->M_Ccroll_Condition_bHorizontal)
	{
		//往右翻页
		if (_Value >= this->M_Page_ScrollMaxWidth)
		{
			//如果数据没有显示完毕
			if (!this->M_ConditionPage_OverData)
			{
				//翻页次数加一
				this->M_Page_CurrentNum++;
				//重新排列
				this->Run(this->M_Page_LineWidth);
			}
		}
		else {
			if (_Value <= this->M_Page_LineWidth)
			{
				if (_Value < this->M_OnUserScrolled_Temp_UpOffset)
				{
					//往左翻页(当前页数必须大于0)
					if (this->M_Page_CurrentNum > 0)
					{
						//将数据显示完毕条件重置
						this->M_ConditionPage_OverData = false;
						//翻页次数减一
						this->M_Page_CurrentNum--;
						//重新排列
						this->Run(this->M_Page_ScrollMaxWidth);
					}
				}
				else {
					this->M_OnUserScrolled_Temp_UpOffset = _Value;
				}
			}
		}
	}
	else {
		//往下翻页
		if (_Value >= M_Page_ScrollMaxHight)
		{
			//如果数据没有显示完毕
			if (!this->M_ConditionPage_OverData)
			{
				//翻页次数加一
				this->M_Page_CurrentNum++;
				//重新排列
				this->Run(this->M_Page_LineHight);
			}
		}
		else {
			if (_Value <= this->M_Page_LineHight)
			{
				if (_Value <= this->M_OnUserScrolled_Temp_UpOffset)
				{
					//往上翻页(当前页数必须大于0)
					if (this->M_Page_CurrentNum > 0)
					{
						//将数据显示完毕条件重置
						this->M_ConditionPage_OverData = false;
						//翻页次数减一
						this->M_Page_CurrentNum--;
						//重新排列
						this->Run(this->M_Page_ScrollMaxHight);
					}
				}
				else {
					this->M_OnUserScrolled_Temp_UpOffset = _Value;
				}
			}
		}
	}
}

int32 UItemLoaderObject::GetPagetLastIndex() const
{

	if (this->M_Page_CurrentMaxCount + this->M_LoadIndex_First > this->M_Page_LoadMaxCount)
	{
		return this->M_Page_LoadMaxCount;
	}

	return this->M_Page_CurrentMaxCount + this->M_LoadIndex_First;
}

void UItemLoaderObject::Run(float _Offset)
{
	//重置展示数量
	this->M_Item_CurrentShowCount = 0;
	//设置需要显示的数据起始索引
	this->M_LoadIndex_First = this->GetPagetFirstIndex();
	//设置需要显示的数据末尾索引(如果只显示一列->那么额外增加一个最大显示)
	this->M_LoadIndex_Last = this->GetPagetLastIndex();

	//如果末尾的索引大于大于最大数量->重置末尾索引
	if (this->M_LoadIndex_Last == this->M_Page_LoadMaxCount)
	{
		//设置翻到最后一页
		this->M_ConditionPage_OverData = true;
	}

	//循环次数
	int32 LoopCount = this->M_LoadIndex_Last;
	//判断是否是最后一页数据：总数量小于需要加载的数量
	if (this->M_Item_Count < this->M_LoadIndex_Last)
	{
		LoopCount = this->M_Item_Count;
		this->M_ConditionPage_OverData = true;
	}

	for (int i = this->M_LoadIndex_First; i < LoopCount; i++)
	{
		this->M_Item_CurrentLoadCount = i + 1;
		this->M_Item_CurrentShowCount++;
	}

	//更新
	this->UpdateData(_Offset);
}

void UItemLoaderObject::Execute()
{
	if (!this->OnNew.IsBound() || !this->OnUpdate.IsBound())
	{
		return;
	}

	//如果数据没有加载完毕->加载数据
	if (!this->M_ConditionPage_OverData)
	{
		this->Run(this->M_Load_Table_List->GetScrollOffset());
	}
	else {
		if (this->M_Condition_RefreshEndPage)
		{
			this->M_Condition_RefreshEndPage = false;

			this->Run(this->M_Load_Table_List->GetScrollOffset());
		}
		else {
			this->UpdateData(this->M_Load_Table_List->GetScrollOffset());
		}
	}
}

void UItemLoaderObject::UpdateData(float _Offset)
{
	//判断 子UI缓冲对象数量  是否 适用于 现在的数据
	//如果不适合->添加新的子UI缓冲区(再一次调用Create函数)
	if (this->M_Load_Uniform_Grid->GetAllChildren().Num() < this->M_Item_CurrentShowCount)
	{
		//如果有对象->那么额外创建UI->之后刷新
		if (this->M_Load_Uniform_Grid->HasAnyChildren())
		{
			//创建新对象->先创建新的界面->然后再统一刷新
			this->CreateItem();
			this->RefreshItem(_Offset);
		}
		else {
			//如果没有任何界面->则直接创建界面
			this->CreateItem();
			this->OnLoadEnd.ExecuteIfBound();
		}
	}
	else {
		this->RefreshItem(_Offset);
	}
}

void UItemLoaderObject::CreateItem()
{
	for (int32 i = this->M_Load_Uniform_Grid->GetAllChildren().Num();
		i < this->M_Item_CurrentShowCount; i++)
	{
		//调用创建函数->返回界面
		this->DataBlock->SetValue(
			this->ItemLoaderObjectPtr, i);

		UWidget* LWidget = this->OnNew.Execute(this->DataBlock, i);

		if (LWidget)
		{
			//加入到统一格子
			if (this->M_Ccroll_Condition_bHorizontal)
			{
				this->M_Load_Uniform_Grid->AddChildToUniformGrid(LWidget,
					i % this->M_Page_CurrentColCount,
					i / this->M_Page_CurrentColCount);
			}
			else
			{
				this->M_Load_Uniform_Grid->AddChildToUniformGrid(LWidget,
					i / this->M_Page_CurrentColCount,
					i % this->M_Page_CurrentColCount);
			}
		}
	}
}

void UItemLoaderObject::RefreshItem(float _Offset)
{
	//调用预刷新
	this->OnUpdateBefore.ExecuteIfBound();

	/*
	int32 InitIndex = 0;
	int32 EndRange = this->M_Item_CurrentShowCount;
	if (this->GetCurrentPage() != 0)
	{
		if (this->M_Ccroll_Condition_bHorizontal)
		{
			InitIndex = this->M_Page_CurrentRowCount;
			EndRange -= this->M_Page_CurrentRowCount;
		}
		else {
			InitIndex = this->M_Page_CurrentColCount;
			EndRange -= this->M_Page_CurrentColCount;
		}
	}*/

	//执行代理(设置数据表数据)
	for (int32 i = 0; i < this->M_Item_CurrentShowCount; i++)
	{
		UWidget* BasePtr = this->M_Load_Uniform_Grid->GetChildAt(i);

		//调用函数
		this->DataBlock->SetValue(
			this->ItemLoaderObjectPtr,
			this->M_LoadIndex_First + i
		);

		//执行代理函数->刷新
		this->OnUpdate.Execute(
			this->DataBlock,
			this->M_LoadIndex_First + i,
			BasePtr);

		//显示界面
		if (BasePtr->GetVisibility() != ESlateVisibility::Visible)
		{
			BasePtr->SetVisibility(ESlateVisibility::Visible);
		}
	}

	//将其他的界面隐藏
	if (this->M_ConditionPage_OverData)
	{
		for (
			int32 i = this->M_Item_CurrentShowCount;
			i < this->M_Load_Uniform_Grid->GetAllChildren().Num(); i++)
		{
			if (this->M_Load_Uniform_Grid->GetChildAt(i)->GetVisibility() != ESlateVisibility::Hidden)
			{
				this->M_Load_Uniform_Grid->GetChildAt(i)->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	//执行加载结束
	this->OnLoadEnd.ExecuteIfBound();

	this->M_Load_Table_List->SetScrollOffset(_Offset);
}

void UItemLoaderObject::SetItemLoadManager(
	class UScrollBox* ScrollBox,
	class UUniformGridPanel* UniformGridPanel,
	const FItemLoadManagerData& ItemLoadManagerData
) {

	//初始化数据块
	if (!IsValid(this->DataBlock))
	{
		this->DataBlock = NewObject<UItemLoadDataBlock>();
	}

	//初始化数据加载器对象
	if (!this->ItemLoaderObjectPtr.IsValid())
	{
		this->ItemLoaderObjectPtr = MakeShareable<ItemLoaderObject>(new ItemLoaderObject());
	}

	//初始化容器界面
	if (IsValid(this->M_Load_Uniform_Grid))
	{
		this->M_Load_Uniform_Grid->ClearChildren();
	}
	this->M_Load_Uniform_Grid = nullptr;
	this->M_Load_Uniform_Grid = UniformGridPanel;

	//如果之前有绑定事件，则移除此对象所绑定的滑动事件
	if (IsValid(this->M_Load_Table_List))
	{
		this->M_Load_Table_List->OnUserScrolled.RemoveAll(this);
		this->M_Load_Table_List = nullptr;
	}
	this->M_Load_Table_List = ScrollBox;
	this->M_Load_Table_List->OnUserScrolled.AddDynamic(this, &UItemLoaderObject::OnUserScrolled);

	//设置下拉模式
	this->M_Ccroll_Condition_bHorizontal = ItemLoadManagerData.bLoaderModeHorizontal;
	//设置默认页数
	this->SetLoaderPage(0, false);
	//设置加载最大个数：11 DataTable Count
	this->SetLoaderItemMaxCount(ItemLoadManagerData.LoadItemMaxCount);
	//设置滚动区偏移位置
	if (IsValid(this->LoaderRangeWidget))
	{
		this->M_ScrollOffset = this->LoaderRangeWidget->GetRenderTransform().Translation;
	}
	else {
		this->M_ScrollOffset = this->M_Load_Table_List->GetRenderTransform().Translation;
	}
	//设置物品直接的间隔
	this->M_MarginOffset = FVector2D(0.f);
	//设置行列
	this->SetLoaderRowCol(ItemLoadManagerData.LoaderRow, ItemLoadManagerData.LoaderCol);
}

void UItemLoaderObject::UpdateDataTable_ArrayIndex(const int32& ArrayNum) {

	TArray<int32> Index;
	Index.Empty(ArrayNum);

	//填充数据
	for (auto Ptr = Index.CreateIterator(); Ptr; ++Ptr)
	{
		(*Ptr) = Ptr.GetIndex();
	}

	this->UpdateDatatable<int32>(Index);
}

void UItemLoaderObject::SetLoaderRangeWidget(ULoaderRangeWidget* RangeWidgetIns) {
	this->LoaderRangeWidget = RangeWidgetIns;
	this->LoaderRangeWidget->GetLoaderObject()->SetItemLoadManager(
		this->LoaderRangeWidget->GetExternalArea(),
		this->LoaderRangeWidget->GetInternalArea(),
		this->LoaderRangeWidget->ItemLoaderData
	);
}

void UItemLoaderObject::SetLoaderRowCol(int32 _Row, int32 _Col)
{
	//如果设置了 横向模式
	if (this->M_Ccroll_Condition_bHorizontal)
	{
		Swap(_Row, _Col);
		this->M_Load_Table_List->SetOrientation(EOrientation::Orient_Horizontal);
	}
	else {
		this->M_Load_Table_List->SetOrientation(EOrientation::Orient_Vertical);
	}

	//设置多少列
	this->M_Page_CurrentColCount = _Col;
	//设置多少行
	this->M_Page_CurrentRowCount = _Row;
	//设置翻页基数:  5 * 1 - (1 * 1) = 4 Index
	this->M_Page_BaseNum = _Row * _Col - _Col * ((_Row == 1) ? 0 : 1);
	//设置一页最大显示个数: 5 * 1 * 2 + 1 = 11 Count
	this->M_Page_CurrentMaxCount = _Row * _Col * 2 + _Col;

	//获取滚动区设计大小
	FVector2D ScrollBoxSize = FVector2D::Zero();
	if (IsValid(this->LoaderRangeWidget))
	{
		ScrollBoxSize = UWidgetBase::WidgetToCanvasSlot(this->LoaderRangeWidget)->GetSize();
	}
	else {
		ScrollBoxSize = UWidgetBase::WidgetToCanvasSlot(this->M_Load_Table_List)->GetSize();
	}

	if (this->M_Ccroll_Condition_bHorizontal)
	{
		//设置行宽
		this->M_Page_LineWidth = ScrollBoxSize.X / _Row;
		//设置行高
		this->M_Page_LineHight = ScrollBoxSize.Y / _Col;
	}
	else {
		//设置行宽
		this->M_Page_LineWidth = ScrollBoxSize.X / _Col;
		//设置行高
		this->M_Page_LineHight = ScrollBoxSize.Y / _Row;
	}

	//设置一个滚动区域可显示的最大宽度
	this->M_Page_ScrollMaxWidth = ScrollBoxSize.X;
	//设置一个滚动区域可显示的最大高度
	this->M_Page_ScrollMaxHight = ScrollBoxSize.Y;

	//设置容器的大小
	if (!IsValid(this->LoaderRangeWidget))
	{
		UWidgetBase::WidgetToCanvasSlot(this->M_Load_Table_List)->SetSize(
			FVector2D(_Col * this->M_Page_LineWidth, _Row * this->M_Page_LineHight)
		);
	}

	//重置格子排列
	int32 LocalIndex = 0;
	UUniformGridSlot* Temp = nullptr;
	for (auto& Widget : this->M_Load_Uniform_Grid->GetAllChildren())
	{
		Temp = Cast<UUniformGridSlot>(Widget);
		if (IsValid(Temp))
		{
			if (this->M_Ccroll_Condition_bHorizontal)
			{
				Temp->SetRow(LocalIndex % this->M_Page_CurrentColCount);
				Temp->SetColumn(LocalIndex / this->M_Page_CurrentColCount);
			}
			else {
				Temp->SetRow(LocalIndex / this->M_Page_CurrentColCount);
				Temp->SetColumn(LocalIndex % this->M_Page_CurrentColCount);
			}
		}
		LocalIndex++;
	}
}

void UItemLoaderObject::SetLoaderItemMaxCount(int32 _NewCount)
{
	if (_NewCount > this->M_Item_Count)
	{
		this->M_Page_LoadMaxCount = this->M_Item_Count;
	}
	else {
		this->M_Page_LoadMaxCount = _NewCount;
	}
}

void UItemLoaderObject::SetLoaderPage(int32 _NewPage, bool bExecute)
{
	int32 LLocalEndPage = 0;
	if (this->M_Page_CurrentColCount != 0 || this->M_Page_CurrentRowCount != 0)
	{
		//得到最后一页
		LLocalEndPage = ((int32)(this->M_Page_LoadMaxCount /
			(this->M_Page_CurrentColCount * this->M_Page_CurrentRowCount))) - 1;
	}

	//设置页数
	if (LLocalEndPage < 0)
	{
		LLocalEndPage = 0;
	}
	else
	{
		if (_NewPage < LLocalEndPage)
		{
			LLocalEndPage = _NewPage;
		}
	}

	//设置页数
	this->M_Page_CurrentNum = LLocalEndPage;

	//如果需要执行加载
	if (bExecute)
	{
		if (this->M_Page_CurrentNum == 0)
		{
			this->Run(0.f);
		}
		else {
			if (this->M_Ccroll_Condition_bHorizontal)
			{
				this->Run(this->M_Page_LineWidth);
			}
			else {
				this->Run(this->M_Page_LineHight);
			}
		}
	}
	else {
		if (this->M_Page_CurrentNum == 0)
		{
			this->M_Load_Table_List->SetScrollOffset(0.f);
		}
	}
}

int32 UItemLoaderObject::GetGridIndexByUiIndex(int32 UiIndex) const {
	//确保此索引在显示范围内
	if (
		//大于起始索引
		UiIndex >= this->GetPagetFirstIndex()
		&&
		//小于最后范围
		UiIndex < this->GetPagetLastIndex()
		)
	{
		if (this->GetCurrentPage() == 0)
		{
			return UiIndex % this->GetShowCount();
		}
		else {
			int32 IndexOffset = UiIndex - this->GetPagetFirstIndex();
			int32 PageCount = this->GetShowCount() / 2;
			if (IndexOffset < PageCount)
			{
				return IndexOffset % PageCount;
			}
			else {
				int32 Index = IndexOffset % PageCount + PageCount;
				return Index >= this->GetShowCount() ? -1 : Index;
			}
		}
	}

	return -1;
}

void UItemLoaderObject::SetLoaderTransfrom(FVector2D _NewOffset, FVector2D _NewScale, float _Angle)
{
	FWidgetTransform Trans;
	Trans.Scale = _NewScale;
	Trans.Angle = _Angle;
	Trans.Translation = _NewOffset;

	if (IsValid(this->LoaderRangeWidget))
	{
		Trans.Shear = this->LoaderRangeWidget->GetRenderTransform().Shear;
		this->LoaderRangeWidget->SetRenderTransform(Trans);
	}
	else {
		Trans.Shear = this->M_Load_Table_List->GetRenderTransform().Shear;
		this->M_Load_Table_List->SetRenderTransform(Trans);
	}

}

void UItemLoaderObject::SetLoaderWidgetMargin(float UniformMargin)
{

	FMargin LMargin = 0.f;

	//如果是横向模式
	if (this->M_Ccroll_Condition_bHorizontal)
	{
		LMargin.Right = UniformMargin;
		this->M_Page_LineWidth += UniformMargin;
		this->M_Page_ScrollMaxWidth += UniformMargin * this->M_Page_CurrentRowCount;
	}
	else {
		LMargin.Bottom = UniformMargin;
		this->M_Page_LineHight += UniformMargin;
		this->M_Page_ScrollMaxHight += UniformMargin * this->M_Page_CurrentRowCount;
	}

	//设置间距
	this->M_Load_Uniform_Grid->SetSlotPadding(LMargin);

	if (IsValid(this->LoaderRangeWidget))
	{
		//设置容器的大小
		UWidgetBase::WidgetToCanvasSlot(this->LoaderRangeWidget)->SetSize(
			FVector2D(
				this->M_Page_ScrollMaxWidth,
				this->M_Page_ScrollMaxHight
			)
		);
	}
	else {
		//设置容器的大小
		UWidgetBase::WidgetToCanvasSlot(this->M_Load_Table_List)->SetSize(
			FVector2D(
				this->M_Page_ScrollMaxWidth,
				this->M_Page_ScrollMaxHight
			)
		);
	}
}
