/************************/
/*	font.h				*/
/************************/
#ifndef _FONT_H_
#define _FONT_H_

// フォントの論理高 字体的逻辑高
#define FONT_SIZE 16

// 全角文字の横幅（ドット数）点数
#define FONT_ZENKAU_WIDTH FONT_SIZE + 1
// 半角文字の横幅（ドット数）
#define	FONT_HANKAKU_WIDTH	FONT_SIZE / 2 + 1

// 全角文字の横幅（ドット数）
extern int FontZenkauWidth;
// 半角文字の横幅（ドット数）
extern int FontHankakuWidth;

/* フォント表示優先順位  fontPrio の値 ****************************************/
//字体显示优先顺序的* * * * fontPrio值
enum{
	FONT_PRIO_BACK, 		/* 後ろに表示  后面，表示	*/
	FONT_PRIO_FRONT			/* 前に表示 	*/
};

// フォントバッファー構造体 字体缓冲结构
typedef struct{
	short x, y;			// 表示座標
	char color;			// 表示色
	char str[ 256 ]; 	// 文字列
	UCHAR	fontPrio; 	// 表示の優先順位
	BOOL hitFlag;		// 当たり判定するかフラグ　ＯＮ：１　ＯＦＦ：０  每判定？转折点ON：1折扣：0
}FONT_BUFFER;

// フォントバッファー  字体缓冲器
extern FONT_BUFFER FontBuffer[];

// フォントカウンター  字体柜台
extern int FontCnt;

// フォント表示関数 //  字体显示函数/////////////////////////////////////////////////////////
void FontPrint( char *moji, int x, int y, int dispPrio );

// 数字フォント表示関数 //  数字字体显示函数/////////////////////////////////////////////////////
void FontPrintDec( char *moji, int x, int y, int dispPrio );

/* フォント情報をバッファに溜める *********************************************/
//字体信息缓冲* * * * * *蓄积在* * 
int StockFontBuffer( int x, int y, char fontPrio, int color, char *str, BOOL hitFlag );

/* フォント情報をバッファに溜める（構造体渡し）********************************/
//字体信息缓冲储（构造体* * * * *交给）
void StockFontBuffer2( STR_BUFFER *strBuffer );

#endif











