/************************/
/*	sprmgr.h			*/
/************************/
#ifndef _SPRMGR_H_
#define _SPRMGR_H_

//#define BMP_FILE			120000	// ＢＭＰファイルの数
#define MAX_GRAPHICS	 	130000	// 画像の最大数
#define	DEF_COLORKEY		0	// 透明色とする色番号 透明色的色号
//#define	DEF_COLORKEY		240	// 透明色とする色番号

#define SURFACE_WIDTH   64 	// 分割サーフェイスの幅 分割接口的宽度
#define SURFACE_HEIGHT  48  // 分割サーフェイスの高さ
//#define SURFACE_WIDTH   32 	// 分割サーフェイスの幅
//#define SURFACE_HEIGHT  24  // 分割サーフェイスの高さ

// サーフェスインフォ構造体 回程信息结构
struct surfaceInfo{

	LPDIRECTDRAWSURFACE lpSurface; 	// 指针

	int	bmpNo;						// 位图号码
	//这个值- 1的时候，这个表面是空。
	
	// この値が -1 の時はこのサーフェスは空である
	//使用了这个接口日期记忆
	UINT date;						// このサーフェスを使用した日付を記憶
	//显示位置所在X坐标
	short offsetX;					// 表示位置からのオフセットＸ座標
	short offsetY;					// 表示位置からのオフセットＹ座標

	surfaceInfo	*pNext;				// 次のサーフェスインフォ構造体へのポインタ
	
};

typedef struct surfaceInfo SURFACE_INFO;

// スプライトインフォ構造体  妖怪的信息结构？
typedef struct{

//	LPBITMAPINFO lpBmp; 			//位图信息结构的指针
	
	SURFACE_INFO *lpSurfaceInfo; 	// 使用しているサーフェスリストの先頭アドレス
	/// /使用的表面名单的前头地址/ /这个值NULL的时候这个BMP是VRAM没有
	// この値が NULL の時はこのＢＭＰは VRAM にはいない
	short width,height;				//BMP的尺寸
	
}SPRITE_INFO;



// 全局变量 ////////////////////////////////////////////////////////////

// スプライトインフォ  程序的接口
extern SPRITE_INFO SpriteInfo[];

// サーフェスインフォ
extern SURFACE_INFO SurfaceInfo[];

// バトルサーフェスのポインタ
//战斗图片指针？
extern LPDIRECTDRAWSURFACE lpBattleSurface;

// サーフェスの数  接口数量
extern int SurfaceCnt;

// VRAM に作成したサーフェスの数カウント
//全部接口 计数？
extern int VramSurfaceCnt;

// SYSTEMRAM に作成したサーフェスの数カウント
extern int SysramSurfaceCnt;

// サーフェスを使用した日付
//接口使用日期
extern UINT SurfaceDate;

// 現在のサーフェス検索位置
//接口位置检测
extern int SurfaceSearchPoint;

#ifdef _DEBUG		
// 現在使っているサーフェスの数カウント
extern int SurfaceUseCnt;

// 現在表示しているサーフェスの数カウント
extern int SurfaceDispCnt;
#endif
// 分割サーフェイスの幅
extern int SurfaceSizeX;
// 分割サーフェイスの高さ
extern int SurfaceSizeY;

// 関数の型宣言 //////////////////////////////////////////////////////////////

/* ビットマップの読み込み 位图加载  *****************************************************/
void OpenBmp( void );      

/* オフスクリーンサーフェスの作成 *********************************************/
BOOL InitOffScreenSurface( void );

/* サーフェスインフォ構造体の初期化　******************************************/
void InitSurfaceInfo( void );

/* スプライトインフォ構造体の初期化　**************************************/
void InitSpriteInfo( void );

/* ＢＭＰをサーフェスへ割り当て ***********************************************/
void AllocateBmpToSurface( int bmpNo );

// ＢＭＰをロードする *********************************************************/
BOOL LoadBmp( int bmpNo );
//编号转换？
// ＢＭＰをロードする（グラフィック番号変換もする） ***************************/
BOOL LoadBmp2( int bmpNo );

#endif

