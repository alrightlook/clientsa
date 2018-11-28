// directDraw.cpp 头文件

#ifndef _DIRECT_DRAW_H_
#define _DIRECT_DRAW_H_

// 最大调色板数量
#define MAX_PAL	15
// 默认调色板
#define DEF_PAL	0	//托盘一天

// 托盘交换时间（在一分钟内完成）
#define PAL_CHANGE_TIME			3600

// 托盘系统
#define SYSTEM_PAL_WHITE 		255//颜色定义白
#define SYSTEM_PAL_AQUA 		254
#define SYSTEM_PAL_PURPLE 		253
#define SYSTEM_PAL_BLUE 		252
#define SYSTEM_PAL_YELLOW 		251
#define SYSTEM_PAL_GREEN 		250
#define SYSTEM_PAL_RED 			249
#define SYSTEM_PAL_GRAY 		248
#define SYSTEM_PAL_BLUE3 		247
#define SYSTEM_PAL_GREEN3 		246//灰色

#define SYSTEM_PAL_WHITE2 		7
#define SYSTEM_PAL_AQUA2 		6
#define SYSTEM_PAL_PURPLE2 		5
#define SYSTEM_PAL_BLUE2 		4
#define SYSTEM_PAL_YELLOW2 		3
#define SYSTEM_PAL_GREEN2 		2
#define SYSTEM_PAL_RED2 		1
#define SYSTEM_PAL_BLACK 		0//黑
#define SYSTEM_PAL_BLUE4 		8
#define SYSTEM_PAL_GREEN4 		9

// 字体调色板
#define FONT_PAL_WHITE 		0
#define FONT_PAL_AQUA 		1
#define FONT_PAL_PURPLE 	2
#define FONT_PAL_BLUE 		3
#define FONT_PAL_YELLOW 	4
#define FONT_PAL_GREEN 		5
#define FONT_PAL_RED 		6
#define FONT_PAL_GRAY 		7
#define FONT_PAL_BLUE2 		8
#define FONT_PAL_GREEN2 	9


// 如何绘制后台缓冲区  背景？
enum{
	DRAW_BACK_NORMAL,	//正常
	DRAW_BACK_NON,		// 不画
	DRAW_BACK_PRODUCE,	// 生成
	DRAW_BACK_BATTLE,	// 战斗
};

//---------------------------------------------------------------------------//
// Direct Draw娗棟梡峔憿懱宆愰尵         管理用構造体型宣言                                      //
//---------------------------------------------------------------------------//
typedef struct
{
	LPDIRECTDRAW			lpDD;				// DirectDraw对象
	LPDIRECTDRAW2			lpDD2;				// DirectDraw2  能不能修改成高级？我修改原始是2
	LPDIRECTDRAWSURFACE		lpFRONTBUFFER;		// 前缓冲区
	LPDIRECTDRAWSURFACE		lpBACKBUFFER;		// 后
	LPDIRECTDRAWCLIPPER		lpCLIPPER;			// 剪贴
	DDSURFACEDESC			ddsd;				// 处理重新定义的表面结构体  原本是没2 我修改
	LPDIRECTDRAWPALETTE		lpPALETTE;			// 调色板
	int						xSize, ySize;		// 尺寸
} DIRECT_DRAW;

// 僷儗僢僩峔憿懱
extern PALETTEENTRY			Palette[256];

// 僷儗僢僩忬懺峔憿懱
typedef struct{
	int palNo;		// 调色板号码ID
	int time;		// 弶婜壔帪娫 初期化時間
	int flag;		// 弶婜壔僼儔僌 初期化转折点 有没初始化标志？
}PALETTE_STATE; //调色板结构

// 全局变量 /////////////////////////////////////////////////////////////
extern DIRECT_DRAW	*lpDraw;
// DirectDraw  初始化标志
extern BOOL DDinitFlag;	
// 字体处理
extern HFONT hFont;
// 调色板的结构体
extern PALETTE_STATE PalState;	
// 托盘改变标志
extern BOOL	PalChangeFlag;	
// 如何绘制后台缓冲区
extern int BackBufferDrawType;

//---------------------------------------------------------------------------//
// 娭悢僾儘僩僞僀僾愰尵                                                      //
//---------------------------------------------------------------------------//
// DirectDraw初始化
BOOL InitDirectDraw( void );

// 调色板初始化
BOOL InitPalette( void );

// 僶僢僋僒乕僼僃僗傪崟偱僋儕傾乕
void ClearBackSurface( void );

// 僒乕僼僃僗傪崟偱僋儕傾乕
void ClearSurface( LPDIRECTDRAWSURFACE lpSurface );

// 载入图片
LPBITMAPINFO LoadDirectDrawBitmap( char *pFile );

// 产生接口
LPDIRECTDRAWSURFACE CreateSurface( short sizeX, short sizeY, DWORD ColorKey, unsigned int VramOrSysram );

// 快进到后面的表面  快速
HRESULT DrawSurfaceFast( short bx, short by, LPDIRECTDRAWSURFACE lpSurface );
// 僶僢僋僒乕僼僃僗傊崅懍揮憲
HRESULT DrawSurfaceFast2( short bx, short by, RECT *rect, LPDIRECTDRAWSURFACE lpSurface );

// 僶僢僋僒乕僼僃僗傊揮憲
//HRESULT DrawSurface( short bx, short by, RECT *rect, LPDIRECTDRAWSURFACE lpSurface );

// 传递函数的表面和BMP的传递  StretchDIBits 使用
void DrawBitmapToSurface( LPDIRECTDRAWSURFACE lpSurface, int offsetX, int offsetY, LPBITMAPINFO pInfo );
// 内存的传递到接口 memcpy 傪巊梡 乯
void DrawBitmapToSurface2( LPDIRECTDRAWSURFACE lpSurface, int offsetX, int offsetY, int sizeX, int sizeY, LPBITMAPINFO pBmpInfo );

// 僒乕僼僃僗FLIP娭悢//表面flip功能
void Flip( void );

// 检查表面离开
BOOL CheckSurfaceLost( void );

// DirectDraw僆僽僕僃僋僩傪奐曻偡傞
void ReleaseDirectDraw( void );

// 绘制过程中采取的时间（调试）
void DrawDebugLine( unsigned char color );

// 绘制一个框 */
void DrawBox( RECT *rect, unsigned char color, BOOL fill );

// 僼僅儞僩僆僽僕僃僋僩偺嶌惉 */
void InitFont( int fontNo );

// 文本视图
void PutText( char fontPrio ); 

// 调色板 程序  托盘处理 ***************************************************************/
void PaletteProc( void );

// 僷儗僢僩僠僃儞僕 ***********************************************************/
void PaletteChange( int palNo, int time );

// 僆乕僩儅僢僺儞僌昞帵
void DrawAutoMapping( int x, int y, unsigned char *autoMap, int w, int h );
// 夋憸偺暯嬒傪庢偭偰僆乕僩儅僢僾偺怓傪嶌傞
int getAutoMapColor( unsigned int GraphicNo );
// 怓悢entry偺僷儗僢僩palette偺拞偐傜丄color偵堦斣嬤偄傕偺偺index傪曉偡丅
//从调色板中的颜色调色板条目数量，但返回最接近的颜色索引。
int getNearestColorIndex( COLORREF color, PALETTEENTRY *palette, int entry );

// 保存快照
void snapShot( void );
// 夋憸傪俛俵俹僼傽僀儖偵曐懚 保存为BMP图像文件
BOOL saveBmpFile( const char *, BYTE *,	int, int, int, int, int, RGBQUAD *, int );



void DrawMapEffect( void );


#endif
