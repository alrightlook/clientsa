/************************/
/*	font.h				*/
/************************/
#ifndef _FONT_H_
#define _FONT_H_

// �ե���Ȥ�Փ���� ������߼���
#define FONT_SIZE 16

// ȫ�����֤κ�����ɥå���������
#define FONT_ZENKAU_WIDTH FONT_SIZE + 1
// ������֤κ�����ɥå�����
#define	FONT_HANKAKU_WIDTH	FONT_SIZE / 2 + 1

// ȫ�����֤κ�����ɥå�����
extern int FontZenkauWidth;
// ������֤κ�����ɥå�����
extern int FontHankakuWidth;

/* �ե���ȱ�ʾ�����λ  fontPrio �΂� ****************************************/
//������ʾ����˳���* * * * fontPrioֵ
enum{
	FONT_PRIO_BACK, 		/* ����˱�ʾ  ���棬��ʾ	*/
	FONT_PRIO_FRONT			/* ǰ�˱�ʾ 	*/
};

// �ե���ȥХåե��`������ ���建��ṹ
typedef struct{
	short x, y;			// ��ʾ����
	char color;			// ��ʾɫ
	char str[ 256 ]; 	// ������
	UCHAR	fontPrio; 	// ��ʾ�΃����λ
	BOOL hitFlag;		// �������ж����뤫�ե饰���ϣΣ������ϣƣƣ���  ÿ�ж���ת�۵�ON��1�ۿۣ�0
}FONT_BUFFER;

// �ե���ȥХåե��`  ���建����
extern FONT_BUFFER FontBuffer[];

// �ե���ȥ����󥿩`  �����̨
extern int FontCnt;

// �ե���ȱ�ʾ�v�� //  ������ʾ����/////////////////////////////////////////////////////////
void FontPrint( char *moji, int x, int y, int dispPrio );

// ���֥ե���ȱ�ʾ�v�� //  ����������ʾ����/////////////////////////////////////////////////////
void FontPrintDec( char *moji, int x, int y, int dispPrio );

/* �ե��������Хåե������� *********************************************/
//������Ϣ����* * * * * *�����* * 
int StockFontBuffer( int x, int y, char fontPrio, int color, char *str, BOOL hitFlag );

/* �ե��������Хåե������루������ɤ���********************************/
//������Ϣ���崢��������* * * * *������
void StockFontBuffer2( STR_BUFFER *strBuffer );

#endif










