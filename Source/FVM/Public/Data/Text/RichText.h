#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//#include "RichText.generated.h"

//长用于绝大多数的文本
namespace RichText_Game {

	//普通文本[55大小]-棕色
	const FString T("t");

	//普通文本-蓝色
	const FString TBLUE("t_color_blue");

	//普通文本-黑色
	const FString TBLACK("t_color_black");

	//普通文本-白色
	const FString TWHITE("t_color_white");

	//普通文本-紫色
	const FString TPURPLE("t_color_purple");

	//普通文本 20字体大小-棕色
	const FString T20("t_20");

	//普通文本 20字体大小-蓝色
	const FString T20BLUE("t_20_blue");

	//普通文本 20字体大小-金色
	const FString T20GOLD("t_20_gold");

	//普通文本 20字体大小-紫色
	const FString T20PURPLE("t_20_purple");

	//标题文本 24字体大小-浅蓝-深蓝描边
	const FString T24TITLE("t_24_title");

	//标题文本 24字体大小-浅蓝-深蓝描边
	const FString T35TITLE("t_35_title");

	//普通文本 30字体大小-蓝色
	const FString T30BLUE("t_30_blue");

	//普通文本 30字体大小-金色
	const FString T30GOLD("t_30_gold");

	//普通文本 30字体大小-紫色
	const FString T30PURPLE("t_30_purple");

	//普通文本 35字体大小-偏粉色
	const FString T35PINK("t_35_pink");

	//普通文本 35字体大小-偏粉色-带一个偏紫色描边
	const FString T35PINKBOX("t_35_pink");

	//普通文本 35字体大小-红色
	const FString T35RED("t_35_red");
}

//用于消息窗口
namespace RichText_MsgLog {

	//普通文本
	const FString T("t");

	//玩家名称文本
	const FString PLAYER("player");

	//等级文本
	const FString G("g");

	//物品的名称文本
	const FString C("c");

	//游戏指令所需的文本
	const FString INS("Ins");
}


//信息
inline FString FVMAddLogPanel(const FString& Content, const FString& Flag = FString(TEXT("t"))) {
	return FString(FString(TEXT("<")) + Flag + FString(TEXT(">")) + Content + FString(TEXT("</>")));
};

//换行
#define RICHLINE FString("\r\n")
//制表符
#define RICHTAB FString("\t")

//富文本-默认字体
#define RICHTEXT(StrContent) FVMAddLogPanel(StrContent)
//富文本-标签字体：RichText_xxxx::T
#define RICHTEXT_LABLE(StrContent,RichText_) FVMAddLogPanel(StrContent,RichText_)