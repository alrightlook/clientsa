/************************/
/*	sprdisp.h			*/
/************************/
#ifndef _SPRDISP_H_
#define _SPRDISP_H_

#define DISP_BUFFER_SIZE 4096 	// 表示バッファサイズ

/* 表示優先順位  dispPrio の値 *************************************************/
enum{
	DISP_PRIO_BG 		= 0,	/* ＢＧ 	*/
	DISP_PRIO_TILE 		= 0,	// タイル（ゲーム画面のフィールド部）
	DISP_PRIO_CHAR 		= 10,	// キャラ
	DISP_PRIO_PARTS 	= 10,	// パーツ（建物等）
	DISP_PRIO_RESERVE 	= 20,
	DISP_PRIO_JIKI 		= 30,	/* 自機 	*/
	DISP_PRIO_GRID 		= 100,	// グリッドカーソル
	DISP_PRIO_BOX,				/* ボックス */
	DISP_PRIO_IME1,				/* ＩＭＥボックス（バック用　黒色） */
	DISP_PRIO_IME2,				/* ＩＭＥボックス（バック用　青色） */
								/* フォントバック */
	DISP_PRIO_MENU,				/* メニュー */
	DISP_PRIO_IME3,				/* ＩＭＥボックス（フロント用　黒色） */
	DISP_PRIO_IME4,				/* ＩＭＥボックス（フロント用　青色） */
	DISP_PRIO_BOX2,				/* ボックス２ */
	DISP_PRIO_ITEM,				/* アイテム */
								/* フォントフロント */
	DISP_PRIO_YES_NO_WND,		/* 確認ウィンドウ */
	DISP_PRIO_YES_NO_BTN,		/* 確認ボタン */
	DISP_PRIO_BOX3,				/* ボックス３ */
	DISP_PRIO_DRAG,				/* ドラッグ中 */
	DISP_PRIO_MOUSE,			/* マウスカーソル 	*/
	DISP_PRIO_TOP = 255			/* 最優先 	*/
};


// 表示情報構造体
typedef struct {
	int x, y;			// 表示座標
	int bmpNo;			// ＢＭＰ番号( ボックス表示の時は色番号 )
	ACTION *pAct;		// アクションポインタ 动作指针
	//每次判断  0 不能走？  1 可以 2显示盒子？
	BOOL hitFlag;		// 当たり判定するかフラグ　０：しない、１：する、２：ＢＯＸ表示
}DISP_INFO;

// 表示優先ソート用構造体  优先排序/ /表示用结构体
typedef struct {
	short no;			// バッファ格納時の番号 缓冲存储时的号码
	UCHAR	dispPrio; 	// 表示の優先順位
}DISP_SORT;

// 表示バッファ構造体
typedef struct{
	DISP_INFO 	DispInfo[ DISP_BUFFER_SIZE ];
	DISP_SORT 	DispSort[ DISP_BUFFER_SIZE ];
	short 		DispCnt;	// 現在の格納数  格子显示数
}DISP_BUFFER;

// 表示バッファ構造体
extern DISP_BUFFER 	DispBuffer;

// ＢＭＰ読み込み用ワーク領域へのポインタ
extern LPBITMAPINFO lpBmpInfo;
// ＢＭＰのイメージデータまでのオフセット
extern int BmpOffBits;

// Realbin 読み込み用ワーク領域へのポインタ
extern char *pRealBinBits;
// 今回 Realbin から読み込むＢＭＰのサイズ
extern int RealBinWidth, RealBinHeight;

// BitBltの時
extern HBITMAP	hBmp;


// サーフェスヴィジーフラグ
extern int SurfaceBusyFlag;

// 表示バッファソート ///////////////////////////////////////////////////////////
void SortDispBuffer( void );

/* キャラクターの描画 **************************************************/
void PutBmp( void );

// 表示データをバッファに溜める ///////////////////////////////////////////////////
int StockDispBuffer( int x, int y, UCHAR prio, int bmpNo, BOOL hitFlag );

// 表示データをバッファに溜める ///////////////////////////////////////////////////
// ただし中でrealGetNo()を呼ばない
int StockDispBuffer2( int x, int y, UCHAR prio, int bmpNo, BOOL hitFlag );

// タスク表示データをバッファに溜める ///////////////////////////////////////////////////
void StockTaskDispBuffer( void );

// タスク表示データをバッファに溜める ///////////////////////////////////////////////////
// ただしprio1からprio2までのものは処理しない
void StockTaskDispBuffer2( int prio1, int prio2 );

// ボックス表示データをバッファに溜める ***************************************/
void StockBoxDispBuffer( int x1, int y1, int x2, int y2, UCHAR dispPrio, int color, BOOL fill );

#endif

