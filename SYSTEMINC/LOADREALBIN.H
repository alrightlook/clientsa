#ifndef _LOADREALBIN_H_
#define _LOADREALBIN_H_

#include <stdio.h>
//#include "common.h"

typedef unsigned char MOJI;
typedef unsigned char U1;
typedef          char S1;
typedef unsigned short U2;
typedef          short S2;
typedef unsigned long U4;
typedef          long S4;
typedef float  F4;
typedef double F8;

int realGetImage(int graphicNo, unsigned char **bmpdata, int *width, int *height);
int Linear(void);


int initRealbinFileOpen(char *realbinfilename, char *addrbinfilename);
void cleanupRealbin(void);

void initAutoMapColor( char * );
void makeAutoMapColor( void );
int writeAutoMapColor( char *, char * );
int readAutoMapColor( char *, char * );

extern unsigned char autoMapColorTbl[];

//extern	PBITMAPINFO         PBiInfo;
//extern	PBYTE               PBits;
//extern	BITMAPFILEHEADER    header;

//グラフィックナンバ�`より羨ち了崔をもらう
//	���� -1:払�� or 參翌撹孔
//夕侘催鷹曳誼欺嫋羨議了崔/ /指峙- 1:払移賜參翌撹孔
int realGetPos(U4 GraphicNo , S2 *x , S2 *y);

//グラフィックナンバ�`からグラフィック嫌、互さを卦す
//	���� -1:払�� or 參翌撹孔
//夕侘催峠中錐、互指烏
int realGetWH(U4 GraphicNo , S2 *w , S2 *h);

//グラフィックナンバ�`より輝たりサイズを箔める。
//	���� -1:払�� or 參翌撹孔 夕侘曳勣箔耽催樫雁。
int realGetHitPoints(U4 GraphicNo , S2 *HitX , S2 *HitY);

//グラフィックナンバ�`より輝たり登協を箔める。
//	���� -1:払�� or 參翌撹孔 夕侘催鷹曳勣箔耽登協 窃侏
int realGetHitFlag(U4 GraphicNo , S2 *Hit);

//グラフィックナンバ�`より宙鮫��枠��了�Q協隈を函り竃す。
//	���� -1:払�� or 參翌撹孔 夕侘催鷹曳紙夕單枠乏會畳協隈函竃。
BOOL realGetPrioType(U4 GraphicNo , S2 *prioType);

//グラフィックナンバ�`より互さの嗤�oを箔める。
//	���� -1:払�� or 參翌撹孔 
int realGetHeightFlag(U4 GraphicNo , S2 *Height);

//哈方にビットマップファイル桑催(ファイル兆の方忖のみ)を嚥えてグラフィックナンバ�`��宥し桑催��を卦す
//	���� -1:払�� or 參翌撹孔
int realGetNo( U4 CharAction , U4 *GraphicNo );

//グラフィックナンバ�`より�h廠咄を
//	��り�｡�:　0 �O協ない
int realGetSoundEffect(U4 GraphicNo);

//グラフィックナンバ�`より�i佩咄を
//	��り�｡�:　0 �O協ない
int realGetWalkSoundEffect(U4 GraphicNo);

//哈方にグラフィックナンバ�`��宥し桑催��を嚥えてビットマップファイブ桑催を卦す
int realGetBitmapNo( int num );


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//仇夕揃抄潤更 徴薦議 MAP_ATTR峪嗤5了 音岑祇孤焚担議������
typedef struct {
	unsigned char atari_x,atari_y;	//寄きさ  樫雁
	unsigned short hit;				//�iけるか 0 辛參恠 1嬾悪麗???
	short height;				//互さあるか 互業
	short broken;				//�欧譴襪�  鱒撒
	short indamage;				//秘HPダメ�`ジ  秘HP鱒彬
	short outdamage;			//竃ダメ�`ジ 彬墾
	short inpoison;				//秘蕎 ??
	short innumb;				//秘しびれ 醍直
	short inquiet;				//秘葡�a  序秘柿潮  
	short instone;				//秘墳晒  
	short indark;				//秘圧�� 菜圧
	short inconfuse;			//秘詞岱  
	short outpoison;			//竃蕎
	short outnumb;				//竃�wれ
	short outquiet;				//竃葡�a
	short outstone;				//竃墳晒
	short outdark;				//竃圧��
	short outconfuse;			//竃詞岱
	//丼惚1�┿�中心軟栖�r咄��鮫中丼惚吉��?
	short effect1;				//エフェクト1�┿�中に��える�r咄、鮫中�森�など��  ????1?????????????�g��????��??��???
	//丼惚2�┰�秘議扮昨蕗咄��鮫中丼惚吉��
	short effect2;				//エフェクト2��秘ったとき咄、鮫中�森�など��  ????2?????���a?�g��????��??��???
	unsigned short damy_a;
	unsigned short damy_b;
	unsigned short damy_c;
	unsigned int bmpnumber;		//鮫�餬�催 夕頭催
} MAP_ATTR;

struct ADRNBIN{
	unsigned long	bitmapno;
	unsigned long	adder;
	unsigned long	size;
	int	xoffset;
	int	yoffset;
	unsigned int width;
	unsigned int height;

	MAP_ATTR attr;

};

#endif /*_LOADREALBIN_H_*/

/*
typedef struct {
unsigned char atari_x,atari_y; //寄きさ  樫雁  媼仇中持頁麗周侭媼議寄弌��1祥燕幣媼1鯉
unsigned short hit;    //�iけるか 0 辛參恠 1嬾悪麗??? 0燕幣嬾悪麗��1燕幣辛參恠貧肇
short height;    //互さあるか 互業
short broken;    //�欧譴襪�  鱒撒
short indamage;    //秘HPダメ�`ジ  秘HP鱒彬
short outdamage;   //竃ダメ�`ジ 彬墾
short inpoison;    //秘蕎 ??
short innumb;    //秘しびれ 醍直
short inquiet;    //秘葡�a  序秘柿潮  
short instone;    //秘墳晒  
short indark;    //秘圧�� 菜圧
short inconfuse;   //秘詞岱  
short outpoison;   //竃蕎
short outnumb;    //竃�wれ
short outquiet;    //竃葡�a
short outstone;    //竃墳晒
short outdark;    //竃圧��
short outconfuse;   //竃詞岱
//丼惚1�┿�中心軟栖�r咄��鮫中丼惚吉��?
short effect1;    //エフェクト1�┿�中に��える�r咄、鮫中�森�など��  ????1?????????????�g��????��??��???
//丼惚2�┰�秘議扮昨蕗咄��鮫中丼惚吉��
short effect2;    //エフェクト2��秘ったとき咄、鮫中�森�など��  ????2?????���a?�g��????��??��???
unsigned short damy_a;
unsigned short damy_b;
unsigned short damy_c;
unsigned int bmpnumber;  //鮫�餬�催 夕頭催ID   壓仇夕猟周戦議園催
} MAP_ATTR;
struct ADRNBIN{
unsigned long bitmapno;  // 夕頭會双催  夕頭議園催
unsigned long adder;   // 峺苧夕頭壓方象猟周嶄議軟兵了崔
unsigned long size; // 夕頭方象翠議寄弌
int xoffset;   //�塋祥柴�扮��罪恫炎陶卞X
int yoffset;
unsigned int width;    //夕頭錐業;
unsigned int height;
MAP_ATTR attr;
};
*/