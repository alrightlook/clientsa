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

//グラフィックナンバーより立ち位置をもらう
//	戻値 -1:失敗 or 以外成功
//图形号码比得到站立的位置/ /回值- 1:失败或以外成功
int realGetPos(U4 GraphicNo , S2 *x , S2 *y);

//グラフィックナンバーからグラフィック幅、高さを返す
//	戻値 -1:失敗 or 以外成功
//图形号平面宽、高回报
int realGetWH(U4 GraphicNo , S2 *w , S2 *h);

//グラフィックナンバーより当たりサイズを求める。
//	戻値 -1:失敗 or 以外成功 图形比要求每号尺寸。
int realGetHitPoints(U4 GraphicNo , S2 *HitX , S2 *HitY);

//グラフィックナンバーより当たり判定を求める。
//	戻値 -1:失敗 or 以外成功 图形号码比要求每判定 类型
int realGetHitFlag(U4 GraphicNo , S2 *Hit);

//グラフィックナンバーより描画優先順位決定法を取り出す。
//	戻値 -1:失敗 or 以外成功 图形号码比绘图优先顺序决定法取出。
BOOL realGetPrioType(U4 GraphicNo , S2 *prioType);

//グラフィックナンバーより高さの有無を求める。
//	戻値 -1:失敗 or 以外成功 
int realGetHeightFlag(U4 GraphicNo , S2 *Height);

//引数にビットマップファイル番号(ファイル名の数字のみ)を与えてグラフィックナンバー（通し番号）を返す
//	戻値 -1:失敗 or 以外成功
int realGetNo( U4 CharAction , U4 *GraphicNo );

//グラフィックナンバーより環境音を
//	戻り値　:　0 設定ない
int realGetSoundEffect(U4 GraphicNo);

//グラフィックナンバーより歩行音を
//	戻り値　:　0 設定ない
int realGetWalkSoundEffect(U4 GraphicNo);

//引数にグラフィックナンバー（通し番号）を与えてビットマップファイブ番号を返す
int realGetBitmapNo( int num );


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//地图路径结构 魔力的 MAP_ATTR只有5位 不知道干什么的，，，
typedef struct {
	unsigned char atari_x,atari_y;	//大きさ  尺寸
	unsigned short hit;				//歩けるか 0 可以走 1障碍物???
	short height;				//高さあるか 高度
	short broken;				//壊れるか  损坏
	short indamage;				//入HPダメージ  入HP损伤
	short outdamage;			//出ダメージ 伤害
	short inpoison;				//入毒 ??
	short innumb;				//入しびれ 麻木
	short inquiet;				//入沈黙  进入沉默  
	short instone;				//入石化  
	short indark;				//入暗闇 黑暗
	short inconfuse;			//入混乱  
	short outpoison;			//出毒
	short outnumb;				//出痺れ
	short outquiet;				//出沈黙
	short outstone;				//出石化
	short outdark;				//出暗闇
	short outconfuse;			//出混乱
	//效果1（画面看起来時音，画面效果等）?
	short effect1;				//エフェクト1（画面に見える時音、画面効果など）  ????1?????????????????賻??訒???
	//效果2（进入的时候声音，画面效果等）
	short effect2;				//エフェクト2（入ったとき音、画面効果など）  ????2?????羶?????賻??訒???
	unsigned short damy_a;
	unsigned short damy_b;
	unsigned short damy_c;
	unsigned int bmpnumber;		//画像番号 图片号
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
unsigned char atari_x,atari_y; //大きさ  尺寸  占地面积是物件所占的大小，1就表示占1格
unsigned short hit;    //歩けるか 0 可以走 1障碍物??? 0表示障碍物，1表示可以走上去
short height;    //高さあるか 高度
short broken;    //壊れるか  损坏
short indamage;    //入HPダメージ  入HP损伤
short outdamage;   //出ダメージ 伤害
short inpoison;    //入毒 ??
short innumb;    //入しびれ 麻木
short inquiet;    //入沈黙  进入沉默  
short instone;    //入石化  
short indark;    //入暗闇 黑暗
short inconfuse;   //入混乱  
short outpoison;   //出毒
short outnumb;    //出痺れ
short outquiet;    //出沈黙
short outstone;    //出石化
short outdark;    //出暗闇
short outconfuse;   //出混乱
//效果1（画面看起来時音，画面效果等）?
short effect1;    //エフェクト1（画面に見える時音、画面効果など）  ????1?????????????????賻??訒???
//效果2（进入的时候声音，画面效果等）
short effect2;    //エフェクト2（入ったとき音、画面効果など）  ????2?????羶?????賻??訒???
unsigned short damy_a;
unsigned short damy_b;
unsigned short damy_c;
unsigned int bmpnumber;  //画像番号 图片号ID   在地图文件里的编号
} MAP_ATTR;
struct ADRNBIN{
unsigned long bitmapno;  // 图片序列号  图片的编号
unsigned long adder;   // 指明图片在数据文件中的起始位置
unsigned long size; // 图片数据块的大小
int xoffset;   //显示图片时，横坐标偏移X
int yoffset;
unsigned int width;    //图片宽度;
unsigned int height;
MAP_ATTR attr;
};
*/