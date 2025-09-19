#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//#include "RichText.generated.h"

//�����ھ���������ı�
namespace RichText_Game {

	//��ͨ�ı�[55��С]-��ɫ
	const FString T("t");

	//��ͨ�ı�-��ɫ
	const FString TBLUE("t_color_blue");

	//��ͨ�ı�-��ɫ
	const FString TBLACK("t_color_black");

	//��ͨ�ı�-��ɫ
	const FString TWHITE("t_color_white");

	//��ͨ�ı�-��ɫ
	const FString TPURPLE("t_color_purple");

	//��ͨ�ı� 20�����С-��ɫ
	const FString T20("t_20");

	//��ͨ�ı� 20�����С-��ɫ
	const FString T20BLUE("t_20_blue");

	//��ͨ�ı� 20�����С-��ɫ
	const FString T20GOLD("t_20_gold");

	//��ͨ�ı� 20�����С-��ɫ
	const FString T20PURPLE("t_20_purple");

	//�����ı� 24�����С-ǳ��-�������
	const FString T24TITLE("t_24_title");

	//�����ı� 24�����С-ǳ��-�������
	const FString T35TITLE("t_35_title");

	//��ͨ�ı� 30�����С-��ɫ
	const FString T30BLUE("t_30_blue");

	//��ͨ�ı� 30�����С-��ɫ
	const FString T30GOLD("t_30_gold");

	//��ͨ�ı� 30�����С-��ɫ
	const FString T30PURPLE("t_30_purple");

	//��ͨ�ı� 35�����С-ƫ��ɫ
	const FString T35PINK("t_35_pink");

	//��ͨ�ı� 35�����С-ƫ��ɫ-��һ��ƫ��ɫ���
	const FString T35PINKBOX("t_35_pink");

	//��ͨ�ı� 35�����С-��ɫ
	const FString T35RED("t_35_red");
}

//������Ϣ����
namespace RichText_MsgLog {

	//��ͨ�ı�
	const FString T("t");

	//��������ı�
	const FString PLAYER("player");

	//�ȼ��ı�
	const FString G("g");

	//��Ʒ�������ı�
	const FString C("c");

	//��Ϸָ��������ı�
	const FString INS("Ins");
}


//��Ϣ
inline FString FVMAddLogPanel(const FString& Content, const FString& Flag = FString(TEXT("t"))) {
	return FString(FString(TEXT("<")) + Flag + FString(TEXT(">")) + Content + FString(TEXT("</>")));
};

//����
#define RICHLINE FString("\r\n")
//�Ʊ��
#define RICHTAB FString("\t")

//���ı�-Ĭ������
#define RICHTEXT(StrContent) FVMAddLogPanel(StrContent)
//���ı�-��ǩ���壺RichText_xxxx::T
#define RICHTEXT_LABLE(StrContent,RichText_) FVMAddLogPanel(StrContent,RichText_)