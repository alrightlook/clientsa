/**** SYSTEM INCLUDE ****/
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<direct.h>
#include<errno.h>

#include "system.h"
#include "loadrealbin.h"
#include "map.h"

//托盘变更的间隔（窗口模式）
#define PAL_CHANGE_INTERVAL_WIN		120		// パレット変更の間隔（ウィンドウモード）
//托盘变更的间隔（全屏模式）
#define PAL_CHANGE_INTERVAL_FULL	60		// パレット変更の間隔（フルスクリーンモード）


//---------------------------------------------------------------------------//
// グローバル変数定義                                                        //
//---------------------------------------------------------------------------//
DIRECT_DRAW	*lpDraw;		// DirectDrawオブジェクトポインタ  DirectDraw对象
BOOL DDinitFlag = FALSE;	// DirectDraw初期化フラグ 初始化标志
HFONT hFont;				// フォントハンドル 字体句柄

PALETTEENTRY	Palette[256];	// パレット構造体  
PALETTE_STATE 	PalState;		// パレット状態構造体  
BOOL 			PalChangeFlag;	// パレットチェンジフラグ  

// パレットファイルネーム   调色板文件名
char *palFileName[] = {	
	#include "palName.h"
};

/* DirectDraw 初期化 **********************************************************/
BOOL InitDirectDraw( void )
{
	DDSCAPS ddscaps;		// セカンダリサーフェス定義用構造体  ?朎豸??蠏??????盪?
	
	// DirectDrawの初期化処理 失敗したら NULL を返す  ,NULL?桄??????????????雎?
	if( DirectDrawCreate( NULL, &lpDraw->lpDD, NULL ) != DD_OK )
	{//?????????? ?????????????? ?????????鰕?
		if( DirectDrawCreate( (GUID *)DDCREATE_EMULATIONONLY, &lpDraw->lpDD, NULL ) != DD_OK )
		//if( DirectDrawCreate( (GUID *)DDCREATE_HARDWAREONLY, &lpDraw->lpDD, NULL ) != DD_OK )
		{
			MessageBox( hWnd, "DirectDrawCreate Error", "確認", MB_OK | MB_ICONSTOP );
			return FALSE;
		}
	}

	// DirectDraw2拡張モードへ移行  ??????????????
	if( lpDraw->lpDD->QueryInterface( IID_IDirectDraw2, (LPVOID *)&lpDraw->lpDD2 ) != DD_OK )//???? IID_IDirectDraw2  ??????
	{
		MessageBox( hWnd, "QueryInterface Error", "確認", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	
	// ????????DirectDraw
	lpDraw->lpDD->Release();

////// ウィンドウモードの時 //////窗口方式的时候////////////////////////////////////////////
//	_asm{int 3}
	if( WindowMode )
	{
		// このアプリケーションの権限レベルを設定(ウィンドウモード）
		//??????????????????????????????????? ?????????????????? ????
		//窗口 模式的设置
		if( lpDraw->lpDD2->SetCooperativeLevel( hWnd ,DDSCL_NORMAL ) != DD_OK )//???? IID_IDirectDraw2  ??????
		//DDSCL_NORMAL ?桄??????????????????Windows???????????????????DDSCL_ALLOWMODEX ??DDSCL_EXCLUSIVE ??DDSCL_FULLSCREEN ????????
        //if( lpDraw->lpDD2->SetCooperativeLevel( hWnd ,DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN ) != DD_OK )


		// このアプリケーションの権限レベルを設定(排他レベル & フルスクリーン画面）
		//if( lpDraw->lpDD2->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ) != DD_OK )
		//if( lpDraw->lpDD2->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE ) != DD_OK )
		{
			MessageBox( hWnd, "SetCooperativeLevel Error", "確認", MB_OK | MB_ICONSTOP );
			return FALSE;
		}
		
		// ディスプレイ表示モードを変更 显示器显示模式的变更  开启后能动态设置 但是 这个位数 还是不能修改
		//lpDraw->lpDD2->SetDisplayMode( 1152, 864, 8 ,0 ,0 );
		//lpDraw->lpDD2->SetDisplayMode( 800, 600, 16,0 ,0 );
		
		// 表(プライマリ)バッファの初期化
		ZeroMemory( &lpDraw->ddsd, sizeof( lpDraw->ddsd ) ); 
		lpDraw->ddsd.dwSize = sizeof( lpDraw->ddsd );
#if 0
		{	// オーバーレイ対応チェック  覆盖对应检查  放开后模式不对
			HRESULT ret;
			ret = lpDraw->lpDD2->GetCaps( (DDCAPS_DX6*)&ddscaps, NULL );//获取硬件设备驱动程序的能力检测覆盖支持 DDCAPS_DX6*
			if( ret != DD_OK || !( ddscaps.dwCaps & DDCAPS_OVERLAY ) )
			{
				MessageBox( hWnd, "オーバーレイに非対応  叠加，非对应", "確認", MB_OK | MB_ICONSTOP );
			}else{ 
				MessageBox( hWnd, "オーバーレイに対応", "確認", MB_OK | MB_ICONSTOP );
			}
		}
#endif
		// ddsCaps構造体を有効化 & dwBackBufferCount変数を有効
		lpDraw->ddsd.dwFlags = DDSD_CAPS;
		
		// プライマリサーフェス(表画面)として設定 ,このサーフェスがFlipを使用できる。,複数のサーフェスの記述を有効 
		lpDraw->ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		
		// プライマリーサーフェスの作成 使用 DirectDraw 接口对象创建 DirectDrawSurface 主页面对象.

		if( lpDraw->lpDD2->CreateSurface( &lpDraw->ddsd, &lpDraw->lpFRONTBUFFER, NULL ) != DD_OK )//显示平面
		{
			MessageBox( hWnd, "プライマリサーフェスの作成に失敗しました。", "確認", MB_OK | MB_ICONSTOP );
			return FALSE;
		}
		
		// クリッパーの作成  ???????????莊??靨?????????//???? IID_IDirectDraw2  ??????
		if( lpDraw->lpDD2->CreateClipper( 0,&lpDraw->lpCLIPPER, NULL ) != DD_OK )
		{
			MessageBox( hWnd, "クリッパーの作成に失敗しました。", "確認", MB_OK | MB_ICONSTOP );
			return FALSE;
		}
		// クリッパーにウィンドウのハンドルをセットする  剪钳窗口的方向盘设置
		lpDraw->lpCLIPPER->SetHWnd( 0, hWnd );
		// プライマリサーフェスにクリッパーを関連付ける  ?????簑?
		lpDraw->lpFRONTBUFFER->SetClipper( lpDraw->lpCLIPPER );
		
		// 裏(バック)バッファの初期化
		ZeroMemory( &lpDraw->ddsd, sizeof( lpDraw->ddsd ) ); 
		lpDraw->ddsd.dwSize = sizeof( lpDraw->ddsd );
        lpDraw->ddsd.dwFlags           = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
       	lpDraw->ddsd.ddsCaps.dwCaps    = DDSCAPS_OFFSCREENPLAIN;
       	lpDraw->ddsd.dwWidth           = lpDraw->xSize;
       	lpDraw->ddsd.dwHeight          = lpDraw->ySize;
       	if( lpDraw->lpDD2->CreateSurface( &lpDraw->ddsd, &lpDraw->lpBACKBUFFER, NULL ) != DD_OK )//???? IID_IDirectDraw2  ??????
		{
			MessageBox( hWnd, "バックバッファの作成に失敗しました。", "確認", MB_OK | MB_ICONSTOP );
			return FALSE;
		}
		//MessageBox( hWnd, "DirectDraw 初期化終了", "確認", MB_OK | MB_ICONSTOP );
		
	
////// フルスクリーンモードの時 //////////////////////////////////////////////
	}
	else
	{
		// このアプリケーションの権限レベルを設定(排他レベル & フルスクリーン画面）
		//if( lpDraw->lpDD2->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ) != DD_OK )
		//全屏模式
		if( lpDraw->lpDD2->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX ) != DD_OK )// DDSCL_ALLOWREBOOT  ????
		{
			MessageBox( hWnd, "SetCooperativeLevel Error", "確認", MB_OK | MB_ICONSTOP );
			return FALSE;
		}
		// ディスプレイ表示モードを変更
		//lpDraw->lpDD2->SetDisplayMode( lpDraw->xSize, lpDraw->ySize, 8 ,0 ,0 ); ????????8????????32??
		
		//设置显示模式
		
		lpDraw->lpDD2->SetDisplayMode( lpDraw->xSize, lpDraw->ySize, 8,0 ,0 );//第三个参数16我修改没用窗口模式下 0是系统默认的
		//lpDraw->lpDD2->SetDisplayMode( 1024, 768, 8,0 ,0 );//第三个参数16我修改没用窗口模式下 0是系统默认的
		
		//lpDraw->lpDD2->SetDisplayMode( lpDraw->xSize, lpDraw->ySize, 32,0 ,0 );
		//lpDraw->lpDD2->SetDisplayMode( 1024,768, 16 ,0 ,0 );
		
		// 表(プライマリ)バッファの初期化
		// ddsd 構造体の内容をゼロクリアする
		ZeroMemory( &lpDraw->ddsd, sizeof( lpDraw->ddsd ) );
		
		// ddsd 構造体に作成するサーフェスの情報を格納する
		// ddsd 構造体のサイズの指定（ バージョンアップに対応させるため ）
		lpDraw->ddsd.dwSize = sizeof( lpDraw->ddsd );
		
		//?靹??豢??獎?
		// ddsCaps  ?盪??蟲???   & dwBackBufferCount ?菽?
		lpDraw->ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		
		// プライマリサーフェス(表画面)として設定 ,このサーフェスがFlipを使用できる。,複数のサーフェスの記述を有効 
		lpDraw->ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		
		// バックバッファ数を１つセット
		lpDraw->ddsd.dwBackBufferCount = 1;
		
		// プライマリーサーフェスの実作成処理
		//???? IID_IDirectDraw2  ??????
		if( lpDraw->lpDD2->CreateSurface( &lpDraw->ddsd, &lpDraw->lpFRONTBUFFER, NULL ) != DD_OK )
		{
			MessageBox( hWnd, "プライマリサーフェスの作成に失敗しました。", "確認", MB_OK | MB_ICONSTOP );
			return FALSE;
		}
		
		// 裏バッファの初期化
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		// 裏バッファをプライマリサーフェスに接続
		lpDraw->lpFRONTBUFFER->GetAttachedSurface( &ddscaps, &lpDraw->lpBACKBUFFER );
		
	}
	
	// DirectDraw初期化フラグをTUREにする
	DDinitFlag = TRUE;	
	
	// 設定したDirectDraw管理構造体をリターン
	return TRUE;
		
}

//---------------------------------------------------------------------------//
// 概要 ：指定リソースによるパレット初期化関数 ???????????????????袒?????????                              //
// 引数 ：DIRECT_DRAW *lpDraw : DirectDraw???晳盪?                         //
//        LPBITMAPINFO pInfo  : BITMAP?????盪?                             //
//调色板颜色不对 不能正常支持系统颜色 要修改--------------------------------------------------------------------------//


BOOL InitPalette( void )
{
	
	int i;
	FILE *fp; // ファイルポインタ 文件指针

	//获得系统调色板 自己对照下他们的写死的 发现前面10个完全一致
	//PALETTEENTRY PP[256];

	//GetSystemPaletteEntries ((HDC__ *)hWnd,246,255,PP);
	
	// ウィンドウシステムパレット
	//PALETTEENTRY pal[ 20 ]={
	PALETTEENTRY pal[ 32 ]={
		// 最初の１０色
		{0x00 ,0x00, 0x00, PC_NOCOLLAPSE | PC_RESERVED}, // 0:黒
		{0x80 ,0x00, 0x00, PC_NOCOLLAPSE | PC_RESERVED}, // 1:暗い赤
		{0x00 ,0x80, 0x00, PC_NOCOLLAPSE | PC_RESERVED}, // 2:暗い緑
		{0x80 ,0x80, 0x00, PC_NOCOLLAPSE | PC_RESERVED}, // 3:暗い黄
		{0x00 ,0x00, 0x80, PC_NOCOLLAPSE | PC_RESERVED}, // 4:暗い青
		{0x80 ,0x00, 0x80, PC_NOCOLLAPSE | PC_RESERVED}, // 5:暗い紫
		{0x00 ,0x80, 0x80, PC_NOCOLLAPSE | PC_RESERVED}, // 6:暗い水色
		{0xc0 ,0xc0, 0xc0, PC_NOCOLLAPSE | PC_RESERVED}, // 7:暗い白
		{0xc0 ,0xdc, 0xc0, PC_NOCOLLAPSE | PC_RESERVED}, // 8:？
		{0xa6 ,0xca, 0xf0, PC_NOCOLLAPSE | PC_RESERVED}, // 9:？
		
		// 新システムパレット 新系统调色板
		{ 0xde, 0x00, 0x00, PC_NOCOLLAPSE | PC_RESERVED },
		{ 0xff, 0x5f, 0x00, PC_NOCOLLAPSE | PC_RESERVED },
		{ 0xff, 0xff, 0xa0, PC_NOCOLLAPSE | PC_RESERVED },
		{ 0x00, 0x5f, 0xd2, PC_NOCOLLAPSE | PC_RESERVED },
		{ 0x50, 0xd2, 0xff, PC_NOCOLLAPSE | PC_RESERVED },
		{ 0x28, 0xe1, 0x28, PC_NOCOLLAPSE | PC_RESERVED },
		
		// 新システムパレット
		{ 0xf5, 0xc3, 0x96, PC_NOCOLLAPSE | PC_RESERVED },
		{ 0xe1, 0xa0, 0x5f, PC_NOCOLLAPSE | PC_RESERVED },
		{ 0xc3, 0x7d, 0x46, PC_NOCOLLAPSE | PC_RESERVED },
		{ 0x9b, 0x55, 0x1e, PC_NOCOLLAPSE | PC_RESERVED },
		{ 0x46, 0x41, 0x37, PC_NOCOLLAPSE | PC_RESERVED },
		{ 0x28, 0x23, 0x1e, PC_NOCOLLAPSE | PC_RESERVED },
		
		// 最後の１０色
		{0xff ,0xfb, 0xf0, PC_NOCOLLAPSE | PC_RESERVED}, // 246:？
		{0xa0 ,0xa0, 0xa4, PC_NOCOLLAPSE | PC_RESERVED}, // 247:？
		{0x80 ,0x80, 0x80, PC_NOCOLLAPSE | PC_RESERVED}, // 248:灰色
		{0xff ,0x00, 0x00, PC_NOCOLLAPSE | PC_RESERVED}, // 249:赤
		{0x00 ,0xff, 0x00, PC_NOCOLLAPSE | PC_RESERVED}, // 250:緑
		{0xff ,0xff, 0x00, PC_NOCOLLAPSE | PC_RESERVED}, // 251:黄
		{0x00 ,0x00, 0xff, PC_NOCOLLAPSE | PC_RESERVED}, // 252:青
		{0xff ,0x00, 0xff, PC_NOCOLLAPSE | PC_RESERVED}, // 253:紫
		{0x00 ,0xff, 0xff, PC_NOCOLLAPSE | PC_RESERVED}, // 254:水色
		{0xff ,0xff, 0xff, PC_NOCOLLAPSE | PC_RESERVED}  // 255:白
	};
	
	// システムパレットの設定 系统调色板的设定
	for( i = 0; i < 10; i++ ){
		Palette[i].peBlue  	= pal[i].peBlue;
		Palette[i].peGreen 	= pal[i].peGreen;
		Palette[i].peRed 	= pal[i].peRed;
		Palette[i].peFlags 	= PC_EXPLICIT;
		
		Palette[i+246].peBlue  	= pal[i+22].peBlue;
		Palette[i+246].peGreen 	= pal[i+22].peGreen;
		Palette[i+246].peRed 	= pal[i+22].peRed;
		Palette[i+246].peFlags 	= PC_EXPLICIT;
	}
	
	// 新システムパレットの設定
	for( i = 0; i < 6; i++ )
	{
		Palette[i+10].peBlue  	= pal[i+10].peBlue;
		Palette[i+10].peGreen 	= pal[i+10].peGreen;
		Palette[i+10].peRed 	= pal[i+10].peRed;
		Palette[i+10].peFlags 	= PC_NOCOLLAPSE | PC_RESERVED;
		
		Palette[i+240].peBlue  	= pal[i+16].peBlue;
		Palette[i+240].peGreen 	= pal[i+16].peGreen;
		Palette[i+240].peRed 	= pal[i+16].peRed;
		Palette[i+240].peFlags 	= PC_NOCOLLAPSE | PC_RESERVED;
	}
	
	// 初期化されてない時（一番最初だけ）
	if( PalState.flag == FALSE ){
	
		// パレットファイルオープン 托盘文件开放
	
		fp = fopen( palFileName[ DEF_PAL ], "rb" );
		
		// 自由に使えるパレットの設定  自由使用托盘的设定
		//for( i = 10; i < 246; i++ ){
		for( i = 16; i < 240; i++ )
		{
			// パレットの読み込み
			Palette[i].peBlue  	= fgetc( fp );
			Palette[i].peGreen 	= fgetc( fp );
			Palette[i].peRed 	= fgetc( fp );
			
			// ウィンドウモードの時  窗口方式的时候
			//我开启//if( WindowMode ){
			if( WindowMode ){
				Palette[i].peFlags = PC_NOCOLLAPSE | PC_RESERVED;
				//我开启
				Palette[i].peFlags = PC_RESERVED;
				//我开启3行
			}else{
				Palette[i].peFlags = PC_EXPLICIT;
			}
		}
		// ファイル閉じる
		fclose( fp );
		
	}
		Palette[168].peBlue  	= 0;
		Palette[168].peGreen 	= 0;
		Palette[168].peRed 		= 0;
		//Palette[168].peFlags 	= PC_EXPLICIT;
	
#if 0
	else
	{
	
		// 自由に使えるパレットの設定  自由使用调色板的设定
		//for( i = 10; i < 246; i++ ){
		for( i = 16; i < 240; i++ ){
			// ウィンドウモードの時  窗口方式的时候
			if( WindowMode )
			{
				Palette[i].peFlags = PC_NOCOLLAPSE | PC_RESERVED;
			}
			else
			{
				Palette[i].peFlags = PC_EXPLICIT;
			}
		}
	}
#endif
	// 8Bit指数Palette变数初期化，结果lpPALETTE返回。
	// 8BitインデックスをPalette変数で初期化し、結果をlpPALETTEへ返す。
	//DDPCAPS_8BIT
	
	lpDraw->lpDD2->CreatePalette( DDPCAPS_8BIT , Palette, &lpDraw->lpPALETTE, NULL );
	// != DD_OK
	if( lpDraw->lpPALETTE == NULL )
	//if( lpDraw->lpPALETTE != DD_OK )  //这里成功也不是DD_OK？？？
	{
		MessageBox(hWnd, "托盘未能创建指调色板", "Error", MB_OK| MB_ICONSTOP );
		return FALSE;
	}

	// プライマリサーフェスのパレットに上記取得パレットをアタッチする
	//if( lpDraw->lpFRONTBUFFER->SetPalette( lpDraw->lpPALETTE ) != DD_OK ){
		//MessageBox(hWnd, "パレットのアッタチに失敗", "Error", MB_OK);
	//	MessageBox(hWnd, "画面を２５６色モードにして下さい", "Error", MB_OK);
	//	return FALSE;
	//}
	
	// プライマリサーフェスのパレットに上記取得パレットをアタッチする
	//???????????蠡??????蕷uraimarisafesu
	//while( lpDraw->lpFRONTBUFFER->SetPalette( lpDraw->lpPALETTE ) != DD_OK )
	while( lpDraw->lpFRONTBUFFER->SetPalette( lpDraw->lpPALETTE ) != DD_OK )
	{  
		//MessageBox(hWnd, "パレットのアッタチに失敗", "Error", MB_OK);
		if( MessageBox(hWnd, "???256色调色板失败   ", "????", MB_RETRYCANCEL | MB_ICONEXCLAMATION ) == IDCANCEL) return FALSE;
	}
	
	// パレットの中身を設定
	//lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
	
	// 初期化フラグをＯＮ
	PalState.flag = TRUE;
	
	return TRUE;
}

// パレットチェンジ ***********************************************************/
void PaletteChange( int palNo, int time )
{
	// リミットチェック
	if( palNo >= MAX_PAL ) return;
	
	// パレット番号
	PalState.palNo = palNo;
	// パレット変更時間
	PalState.time = time;
	// リミットチェック
	if( PalState.time <= 0 ) PalState.time = 1;
}

// パレット処理 ***************************************************************/
void PaletteProc( void )
{
	FILE *fp; // ファイルポインタ
	static PALETTEENTRY	pal[256];	// パレット構造体
	static float	dRed[256];		// パレット増分
	static float	dGreen[256];	// パレット増分
	static float	dBlue[256];		// パレット増分
	static float	dRedBak[256];	// 小数点パレット
	static float	dGreenBak[256];	// 小数点パレット
	static float	dBlueBak[256];	// 小数点パレット
	static int 	timeCnt;			// タイムカウンター
	static int 	changeCnt;			// チェンジカウンター
	static int 	palNoBak = 0;		// カウンター
	static int 	openFlag = FALSE;	// ファイルオープンフラグ
	int i;
	
	// パレットが変更されてない時
	if( palNoBak == PalState.palNo && openFlag == FALSE ) return;
	
	// オープンしてなかったら
	if( palNoBak != PalState.palNo ){
		// パレットファイルオープン
		fp = fopen( palFileName[ PalState.palNo ], "rb" );
	
		// 自由に使えるパレットの設定
		//for( i = 10; i < 246; i++ ){
		for( i = 16; i < 240; i++ ){
			pal[i].peBlue  	= fgetc( fp );
			pal[i].peGreen 	= fgetc( fp );
			pal[i].peRed 	= fgetc( fp );
			
			// 168の色は( 0, 0, 0 )に変換  168的颜色（0，00），转换 估计是不是背景色
			if( i == 168 ){
				pal[168].peBlue  	= 0;
				pal[168].peGreen 	= 0;
				pal[168].peRed 		= 0;
			}
			
			// 小数パレット
			dBlueBak[ i ] 	= 	Palette[i].peBlue;
			dGreenBak[ i ] 	= 	Palette[i].peGreen;
			dRedBak[ i ] 	= 	Palette[i].peRed;
			// 増分計算
			dBlue[ i ] 	= 	( float )( pal[ i ].peBlue - Palette[i].peBlue ) 	/ ( float )PalState.time;
			dGreen[ i ] = 	( float )( pal[ i ].peGreen - Palette[i].peGreen )	/ ( float )PalState.time;
			dRed[ i ] 	= 	( float )( pal[ i ].peRed - Palette[i].peRed ) 		/ ( float )PalState.time;
//我开启
//#if 0
			// ウィンドウモードの時
			if( WindowMode ){
				Palette[i].peFlags = PC_NOCOLLAPSE | PC_RESERVED;
			}else{
				Palette[i].peFlags = PC_EXPLICIT;
			}
//#endif
		}
		
	
		fclose( fp );				// ファイル閉じる
		timeCnt = 0;				// 内部的にパレットをセットするカウンター初期化
		changeCnt = 0;				// 実際にパレットをセットするカウンター初期化;
		palNoBak = PalState.palNo;	// バックアップ
		openFlag = TRUE;			// フラグＯＮ
	}
	
	// カウンター
	timeCnt++;
	// 変更する時
	if( timeCnt <= PalState.time ){	
		// 自由に使えるパレットの設定
		//for( i = 10 ; i < 246 ; i++ ){
		for( i = 16; i < 240; i++ ){
			// 小数点パレット
			dBlueBak[ i ] 	+= 	dBlue[ i ];
			dGreenBak[ i ]	+=	dGreen[ i ];
			dRedBak[ i ]	+=	dRed[ i ];
			// 整数パレット
			Palette[i].peBlue  	= ( BYTE )dBlueBak[ i ];
			Palette[i].peGreen 	= ( BYTE )dGreenBak[ i ];
			Palette[i].peRed 	= ( BYTE )dRedBak[ i ];
		}
	}else{ 
			
		// 最後のフレームにに完全なパレットの値をいれる
		// 自由に使えるパレットの設定
		//for( i = 10; i < 246; i++ ){
		for( i = 16; i < 240; i++ ){
			Palette[i].peBlue  	= pal[ i ].peBlue;
			Palette[i].peGreen 	= pal[ i ].peGreen;
			Palette[i].peRed 	= pal[ i ].peRed;
		}
		openFlag = FALSE;	// フラグ初期化
	}
	
	// １０フレームに一回だけ表示（重過ぎるため）
	if( changeCnt == 0 || openFlag == FALSE ){
		// パレットチェンジフラグＯＮ
		PalChangeFlag = TRUE;
		// パレットの中身を設定
		//lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
	}
	// カウンタープラス
	changeCnt++;
	// ウィンドウモードの時
	if( WindowMode ){
		if( changeCnt >= PAL_CHANGE_INTERVAL_WIN ) changeCnt = 0;
	}else{
		if( changeCnt >= PAL_CHANGE_INTERVAL_FULL ) changeCnt = 0;
	}
}	

//---------------------------------------------------------------------------//
// 概要 ：垂直同期線中にバックサーフェスからフロントサーフェスへコピー       //
// 引数 ：なし                         										 //
// 戻値 ：なし                                                               //
//---------------------------------------------------------------------------//
void Flip( void )
{
	// ウィンドウモードの時
	if( WindowMode == TRUE ){
	
		RECT 	clientRect;  // クライアント領域の矩形領域を格納
		POINT 	clientPoint; // クライアント領域の左上のスクリーン座標位置を格納
		RECT 	moveRect;  	 // 転送元の矩形領域を格納
		
		// ポイント構造体の初期化
		clientPoint.x = clientPoint.y = 0;
		// クライアント領域のサイズを取得
		GetClientRect( hWnd, &clientRect );
		// クライアント領域の左上のスクリーン座標を取得
		ClientToScreen( hWnd, &clientPoint ); 
		// クライアント領域全体のスクリーン座標を求める ( clientRect に格納される )
		OffsetRect( &clientRect, clientPoint.x, clientPoint.y );
		// 転送元の矩形領域をセット
		SetRect(&moveRect, 0, 0, lpDraw->xSize, lpDraw->ySize );
		// プライマリーサーフェスへ一発転送 ( これがウィンドウモードのフリップ )
		lpDraw->lpFRONTBUFFER->Blt( &clientRect, lpDraw->lpBACKBUFFER, &moveRect, DDBLT_WAIT, NULL );
	
	}else{  // フルスクリーンモードの時
	
		// 画面のフリップ
		lpDraw->lpFRONTBUFFER->Flip( NULL, DDFLIP_WAIT );
		
	}
	return;
}

//---------------------------------------------------------------------------//
// 概要 ：バックサーフェスクリア関数                                         //
// 引数 ：DIRECT_DRAW *lpDraw : DirectDraw管理構造体                         //
// 戻値 ：なし                                                               //
//---------------------------------------------------------------------------//
void ClearBackSurface( void )
{
	DDBLTFX ddbltfx;

	ZeroMemory( &ddbltfx, sizeof( DDBLTFX ) );
	ddbltfx.dwSize = sizeof( DDBLTFX );
	lpDraw->lpBACKBUFFER->Blt( NULL, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddbltfx );

	return;
}

//---------------------------------------------------------------------------//
// 概要 ：サーフェスクリア関数                 		                         //
// 引数 ：DIRECT_DRAW *lpDraw : DirectDraw管理構造体                         //
// 戻値 ：なし                                                               //
//---------------------------------------------------------------------------//
void ClearSurface( LPDIRECTDRAWSURFACE lpSurface )
{
	DDBLTFX ddbltfx;

	ZeroMemory( &ddbltfx, sizeof( DDBLTFX ) );
	ddbltfx.dwSize = sizeof( DDBLTFX );
	ddbltfx.dwFillColor = DEF_COLORKEY; // 透明色を設定
	
	lpSurface->Blt( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );

	return;
}

//---------------------------------------------------------------------------//
// 概要 ：ビットマップファイルの読み込み及びメモリー確保   位图加载文件及存储器确保                   //
// 引数 ：char * pFile  : ビットマップファイル名   位图文件名                            //
// 戻り値：LPBITMAPINFO : NULL .メモリ確保Orファイル失敗                     //
//                NULL以外.LPBITMAPINFOアドレス 
//返回值：LPBITMAPINFO: NULL.内存确保Or文件/ /
//失败NULL以外. / / LPBITMAPINFO地址                             //
//---------------------------------------------------------------------------//
LPBITMAPINFO LoadDirectDrawBitmap( char *pFile )
{
	HFILE hFile;
	OFSTRUCT ofSt;
	BITMAPFILEHEADER BmpFileHeader;
	LPBITMAPINFO lpBmpInfo;

	//ファイル読み込み
	if( ( hFile = OpenFile( pFile, &ofSt, OF_READ ) ) == HFILE_ERROR )
		return (LPBITMAPINFO)NULL; // File Open Error

	//ビットマップファイルヘッダー読み込み
	_hread( hFile, &BmpFileHeader, sizeof(BITMAPFILEHEADER) );

	//ビットマップ用領域確保
	if( (lpBmpInfo = (LPBITMAPINFO)GlobalAllocPtr( GPTR, BmpFileHeader.bfSize )) == NULL ){
		MessageBox( hWnd, "位图用领域确保失败", "確認", MB_OK | MB_ICONSTOP );
		return (LPBITMAPINFO)NULL; //Memory Error
	}
	
	//ビットマップ読み出し 位图读出
	_hread( hFile, (void *)lpBmpInfo, BmpFileHeader.bfSize );

	//ファイルクローズ 文件关闭
	_lclose( hFile );
	
	// ＢＭＰ
	//bmp  的图像数据为止的偏移
	BmpOffBits = BmpFileHeader.bfOffBits - sizeof( BITMAPFILEHEADER );
	
	return lpBmpInfo;
}

//---------------------------------------------------------------------------//
// 概要 ：パターン用サーフェスの作成                                         //
// 引数 ：short bxsize           : 作成?????横幅(????)                       //
//        short bysize           : 作成?????縦幅(????)                       //
//        DWORD ColorKey         : 透明色とする色番号(0～255)   不被复制的颜色             //
// 戻り値：正常終了 ... サーフェスのアドレス / 失敗 ... NULL                 //
//---------------------------------------------------------------------------//
LPDIRECTDRAWSURFACE CreateSurface( short bxsize, short bysize, DWORD ColorKey, unsigned int VramOrSysram )
{
	DDCOLORKEY ddck;  //色彩键码 不被复制的颜色 
	LPDIRECTDRAWSURFACE lpSurface;


	lpSurface = (LPDIRECTDRAWSURFACE)GlobalAllocPtr( GPTR, (DWORD)sizeof( LPDIRECTDRAWSURFACE ) );

	// パターン用バッファの生成
	// ddsCaps構造体を  有効化 & dwWidth,dwHeight指定有効
	lpDraw->ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	// この、サーフェスはフロントでもバックでもない、サーフェスであることを指定
	lpDraw->ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | VramOrSysram;
	//lpDraw->ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	//lpDraw->ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
	// サーフェスの幅を指定
	lpDraw->ddsd.dwWidth = bxsize;
	// サーフェスの高さを指定
	lpDraw->ddsd.dwHeight = bysize;
	// パターン用サーフェスの実定義
	//???? IID_IDirectDraw2  ??????
    if( lpDraw->lpDD2->CreateSurface( &lpDraw->ddsd, &lpSurface, NULL ) != DD_OK )
		return (LPDIRECTDRAWSURFACE)NULL;

	// 透明色カラーキーの設定
	ddck.dwColorSpaceLowValue =  ColorKey;
	ddck.dwColorSpaceHighValue = ColorKey;
	lpSurface->SetColorKey( DDCKEY_SRCBLT, &ddck );

	return lpSurface;
}


//---------------------------------------------------------------------------//
// 概要 ：ビットマップを指定サーフェスへ描画                                 //
// 引数 ：LPDIRECTDRAWSURFACE lpSurface : 描画サーフェス                     //
//        short Xpoint        : 読み取るＢＭＰ横位置                         //
//        short Ypoint        :	読み取るＢＭＰ縦位置                         //
//        LPBITMAPINFO pInfo  : 描画するビットマップ構造体                   //
// 戻値 ：なし                                                               //
//---------------------------------------------------------------------------//
// ＢＭＰをサーフェスへ転送関数 （ StretchDIBits を使用 ）
void DrawBitmapToSurface( LPDIRECTDRAWSURFACE lpSurface, int offsetX, int offsetY, LPBITMAPINFO pBmpInfo )
{
	HDC hDcDest;
	//HDC hDcBmp;
	//HBITMAP hBmp;
	
	//DDSURFACEDESC ddsd;	// サーフェス構造体
    //char *pDest;		// 転送先のポインタ
    //char *pSource; 		// 転送元のポインタ
	//int i;
	
	// 指定サーフェス番号へのビットマップ画像の転送
	lpSurface->GetDC( &hDcDest );
	StretchDIBits( 	hDcDest, 
					0, 0, 
					//pBmpInfo->bmiHeader.biWidth, pBmpInfo->bmiHeader.biHeight, 
					SurfaceSizeX, SurfaceSizeY,
					offsetX, offsetY, 
					//pBmpInfo->bmiHeader.biWidth, pBmpInfo->bmiHeader.biHeight,
					SurfaceSizeX, SurfaceSizeY,
					(void *)((BYTE *)pBmpInfo + (sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256)), 
					pBmpInfo, 
					
					//DIB_PAL_COLORS,	
					DIB_RGB_COLORS, 
					//DIB_PAL_INDICES,
					
					SRCCOPY );
					//NOTSRCCOPY );
					//DSTINVERT );
					//BLACKNESS );

	lpSurface->ReleaseDC( hDcDest );
	
	return;
}

// ＢＭＰをサーフェスへ転送関数 （ memcpy を使用 ）****************************/
void DrawBitmapToSurface2( LPDIRECTDRAWSURFACE lpSurface, int offsetX, int offsetY, int sizeX, int sizeY, LPBITMAPINFO pBmpInfo )
{
	DDSURFACEDESC ddsd;	// サーフェス構造体
    char *pDest;		// 転送先のポインタ
    char *pSource; 		// 転送元のポインタ
    //char *pDestBak;		// 転送先のポインタバックアップ
    //char *pSourceBak; 	// 転送元のポインタバックアップ
	int surfacePitch;	// サーフェスの横幅記憶
	int bmpWidth;		// ＢＭＰの横幅記憶
	int i;
	
	// サーフェスポインタがNULLの時戻る
	if( lpSurface == NULL ) return;
	
	// 構造体の初期化
	ZeroMemory( &ddsd, sizeof( DDSURFACEDESC ) );
	ddsd.dwSize = sizeof( DDSURFACEDESC );
	
	// アクセスするサーフェスをロックする( 同時に ddsd に情報を入れてもらう )
	if( lpSurface->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) != DD_OK ){
		//MessageBox( hWnd, "サーフェスのロックに失敗しました", "確認", MB_OK | MB_ICONSTOP );
		return; 
	}	
	
	// サーフェスの左上隅のアドレス
	pDest = ( char *)( ddsd.lpSurface );
	
#if 0	// ＢＭＰファイルから読み込むとき

	// 転送元のアドレス
	pSource = ( char *)pBmpInfo + BmpOffBits
				+ offsetY * pBmpInfo->bmiHeader.biWidth + offsetX;
				
#else	// Realbin から読み込むとき

	// 転送元のアドレス
	pSource = pRealBinBits
				+ offsetY * RealBinWidth + offsetX;

#endif
				
	// サーフェスの横幅記憶
	surfacePitch = ddsd.lPitch;
#if 0
	// ＢＭＰの横幅記憶
	bmpWidth = pBmpInfo->bmiHeader.biWidth;
	
#else // Realbin 読み込むとき
	// ＢＭＰの横幅記憶
	bmpWidth = RealBinWidth;
#endif

	//３２０＊２４０ドットモードの時
	if(ResoMode == 1){
		//転送元アドレスを１ライン分ずらす
		pSource -= bmpWidth;
		//ループ回数１／２
		sizeY >>= 1;
	}
	// 縦のサイズ分ループする
	for( i = 0 ; i < sizeY ; i++ ){
		// ＢＭＰをサーフェスへコピー

		//３２０＊２４０ドットモードの時
		if(ResoMode == 1){
			_asm{
			mov		edi,[pDest]		//転送先アドレス  转移地址
			mov		esi,[pSource]	//転送元アドレス
			mov		eax,[sizeX]		//ループカウンター
//			mov		ah,al			//
			shr		ax,1
			mov		cx,ax
			inc		esi				//転送元アドレスを１ドットずらす

	loop_100:
			mov		al,[esi]		//１バイト取りだし
//			cmp		al,240			//ピンクの透明か
//			jne		loop_200		//そうでなければ次へ

//			xor		al,al			//０の透明に変更

//loop_200:
			mov		[edi],al		//１バイト書きこみ
			inc		esi				//転送元アドレス更新
			inc		esi				//転送元アドレス更新
			inc		edi				//転送先アドレス更新
//			dec		ah				//終了か
			dec		cx				//終了か
			jne		loop_100		//そうでなければ繰り返す
			}
			// サーフェスの横幅分アドレスを戻す（ 逆から転送しているため ）
			pSource -= bmpWidth*2;
		} else {
			//３２０＊２４０ドットモード以外の時
			memcpy( pDest, pSource, sizeX );
			// サーフェスの横幅分アドレスを戻す（ 逆から転送しているため ）
			pSource -= bmpWidth;
		}
		// サーフェスの横幅分アドレスを進める
		pDest += surfacePitch;
    }

	// アクセスするサーフェスをアンロックする
	if( lpSurface->Unlock( NULL ) != DD_OK ){
		//MessageBox( hWnd, "サーフェスのアンロックに失敗しました", "確認", MB_OK | MB_ICONSTOP );
		return; 
	}	
	
	return;
}

// ボックスを描画 *****箱绘图*********************************************************/
void DrawBox( RECT *rect, unsigned char color, BOOL fill )
{
	DDSURFACEDESC ddsd;	// サーフェス構造体
    char *pDest;		// 転送先のポインタ
	int surfacePitch;	// サーフェスの横幅記憶
	int i;				// ループカウンタ
	int bottom;			// 下線までのアドレス数
	int w;				// 横幅
	int h;				// 縦幅
	
	//３２０＊２４０ドットモードの時
	if(ResoMode == 1){
		rect->top = rect->top / 2 - 1;
		rect->left = rect->left / 2 - 1;
		rect->right = rect->right / 2 + 1;
		rect->bottom = rect->bottom / 2 + 1;
	}
	
	// リミットチェック
	if( rect->left < 0 ) rect->left = 0;
	if( rect->right >= lpDraw->xSize ) rect->right = lpDraw->xSize;
	if( rect->top < 0 ) rect->top = 0;
	if( rect->bottom >= lpDraw->ySize ) rect->bottom = lpDraw->ySize;
	
	w = rect->right - rect->left; // 横幅
	h = rect->bottom - rect->top; // 縦幅
	
	// 幅がない時　戻る
	//if( w <= 2 || h <= 2 ) return;
	// ラインでなく幅がない時　戻る
	if( fill != 2 && ( w <= 2 || h <= 2 ) ) return;
	if( fill == 2 && w == 0 ) return;
	
	// 構造体の初期化
	ZeroMemory( &ddsd, sizeof( DDSURFACEDESC ) );
	ddsd.dwSize = sizeof( DDSURFACEDESC );
	
	// アクセスするサーフェスをロックする( 同時に ddsd に情報を入れてもらう )
	if( lpDraw->lpBACKBUFFER->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) != DD_OK ){
		//MessageBox( hWnd, "サーフェスのロックに失敗しました", "確認", MB_OK | MB_ICONSTOP );
		return; 
	}	
	
	// サーフェスの横幅記憶
	surfacePitch = ddsd.lPitch;
	
	// 塗り始めるサーフェスのアドレス
	pDest = ( char *)( ddsd.lpSurface ) + rect->top * surfacePitch + rect->left;

	// ボックスのとき
	if( fill == FALSE ){
	
		pDest++;	// 位置ドットずらす
		
		// 下線までのアドレス
		bottom = ( h - 1 ) * surfacePitch;
	
		// 横ループ
		for( i = 0; i < w - 2; i++ ){
			// 上の線を描画
			*( pDest + i ) = color;
			*( pDest + i + surfacePitch ) = color;
			// 下の線を描画
			*( pDest + i + bottom ) = color;
			*( pDest + i + bottom - surfacePitch ) = color;
    	}
	
		// 内側の四点を描画
		*( pDest + surfacePitch + surfacePitch + 1 ) = color;					// 左上
		*( pDest + surfacePitch + surfacePitch + w - 4 ) = color;				// 右上
		*( pDest + bottom - ( surfacePitch + surfacePitch ) + 1 ) = color;		// 左下
		*( pDest + bottom - ( surfacePitch + surfacePitch ) + w - 4 ) = color;	// 右下
		
		// サーフェスの横幅分アドレスを進める
		pDest += surfacePitch - 1;
		
		// 縦ループ
		for( i = 0 ; i < h - 2 ; i++ ){
			// 左の線を描画
			*pDest = color;
			*( pDest + 1 ) = color;
			// 右の線を描画
			*( pDest + w - 1 ) = color;
			*( pDest + w - 2 ) = color;
			// サーフェスの横幅分アドレスを進める
			pDest += surfacePitch;
    	}
	}else if( fill == 1 ){	// 塗りつぶしの時
		
		pDest++;	// 位置ドットずらす
		
		// 上のＬＩＮＥを描画
		memset( pDest, color, w - 2 );
		// サーフェスの横幅分アドレスを進める
		pDest += surfacePitch - 1;
		
		// 縦ループ
		for( i = 0 ; i < h - 2 ; i++ ){
			// ＬＩＮＥを描画
			memset( pDest, color, w );
			// サーフェスの横幅分アドレスを進める
			pDest += surfacePitch;
	    }
		// 下のＬＩＮＥを描画
		memset( pDest + 1, color, w - 2 );
		
	}else if( fill == 2 ){	// ラインの時
		
		// ＬＩＮＥを描画
		memset( pDest, color, w );
		// サーフェスの横幅分アドレスを進める
		//pDest += surfacePitch;
		// ＬＩＮＥを描画
		//memset( pDest, color, w );
	}	
	
	// アクセスするサーフェスをアンロックする
	if( lpDraw->lpBACKBUFFER->Unlock( NULL ) != DD_OK ){
		//MessageBox( hWnd, "サーフェスのアンロックに失敗しました", "確認", MB_OK | MB_ICONSTOP );
		return; 
	}	
	
	return;
}


//---------------------------------------------------------------------------//
// オートマッピングを描画                                                    //
//---------------------------------------------------------------------------//
// 引数：	int x, y;						左上表示座標
//			unsigned char *autoMap;			マップ表示用色情報  地图表示用颜色信息
//			int w, h;						autoMap のサイズ 尺寸
void DrawAutoMapping( int x, int y, unsigned char *autoMap, int w, int h )
{
	DDSURFACEDESC ddsd;	// サーフェス構造体 表面构造体
	char *ptDest;		// 転送先のポインタ 转递地址的指针
	char *tmpPtDest;	// ワーク
	char *tmpPtDest2;	// ワーク２
	int surfacePitch;	// サーフェスの横幅記憶  宽的记忆
	int i, j;			// ループカウンタ  循环计数器
	int color;
	static short pcFlush = 0;
	static unsigned int pcFlushTime = 0;
	int xx, yy;
	int ww, hh;
	int mul;

	xx = 18;
	yy = 118;
	ww = w;
	hh = h;
	mul = 1;

	//３２０＊２４０ドットモードの時  点状方式的时候
	if( ResoMode == 1 )
	{
		x  = x/2;
		y  = y/2;
		xx = xx/2+1;
		yy = yy/2-1;
		ww = ww/2;
		hh = hh/2;
		mul = 2;
	}


	// 構造体の初期化
	ZeroMemory( &ddsd, sizeof( DDSURFACEDESC ) );
	ddsd.dwSize = sizeof( DDSURFACEDESC );

	// アクセスするサーフェスをロックする( 同時に ddsd に情報を入れてもらう )
	if( lpDraw->lpBACKBUFFER->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) != DD_OK )
	{
		//MessageBox( hWnd, "サーフェスのロックに失敗しました", "確認", MB_OK | MB_ICONSTOP );
		return; 
	}
	// サーフェスの横幅記憶
	surfacePitch = ddsd.lPitch;

	// 塗り始めるサーフェスのアドレス
	ptDest = (char *)(ddsd.lpSurface) + (y+yy) * surfacePitch + (x+xx);
	tmpPtDest  = ptDest;
	tmpPtDest2 = ptDest;
	for( i = 0; i < hh; i++ )
	{
		ptDest = tmpPtDest;
		for( j = 0; j < ww; j++ )
		{
			color = autoMap[(i*mul)*w+(j*mul)];
			*(ptDest-1)				= color;
			*(ptDest)				= color;
			*(ptDest+1)				= color;
			*(ptDest-surfacePitch)	= color;
			ptDest -= (surfacePitch-2);
		}
		tmpPtDest += (surfacePitch+2);
	}

	// ＰＣキャラの表示（マップ中央で白黒点滅）
	ptDest = tmpPtDest2 + (surfacePitch+2)*hh/2 - (surfacePitch-2)*ww/2;
	if( pcFlush )
	{
		color = 255;
	}
	else
	{
		color = 0;
	}
	if( pcFlushTime+1000 <= GetTickCount() )
	{
		pcFlushTime = GetTickCount();
		pcFlush++;
		pcFlush &= 1;
	}
	*(ptDest-1)				= color;
	*(ptDest)				= color;
	*(ptDest+1)				= color;
	*(ptDest-surfacePitch)	= color;


	// アクセスするサーフェスをアンロックする
	if( lpDraw->lpBACKBUFFER->Unlock( NULL ) != DD_OK )
	{
		//MessageBox( hWnd, "サーフェスのアンロックに失敗しました", "確認", MB_OK | MB_ICONSTOP );
		return; 
	}

	return;
}


// 画像の平均を取ってオートマップの色を作る
//图像的平均采取自动地图制作的颜色
int getAutoMapColor( unsigned int GraphicNo )
{
	int index = 0;
	static int width, height;
	static unsigned char *graBuf;
	unsigned int red = 0, green = 0, blue = 0;
	unsigned int cnt = 0;
	int i, j;
	int color;

	// real.binの通し番号からイメージデータを読み込む  通过编号从图像数据读取
	if( realGetImage( GraphicNo, ( unsigned char **)&graBuf, &width, &height ) == FALSE )
	{
		return 0;
	}

#if 0
	// 画像の中心のＲＧＢ 图像的中心的RGB
	cnt = 1;
	index = graBuf[(height/2)*width+width/2];
	red   = Palette[index].peRed;
	green = Palette[index].peGreen;
	blue  = Palette[index].peBlue;
	
#else
	// 画像のＲＧＢ成分を平均する 平均的成分  图像的RGB成分平均的平均成分
	for( i = 0; i < height; i++ )
	{
		for( j = 0; j < width; j++ )
		{
			index = graBuf[i*width+j];
			//透射颜色不处理
			if( index != DEF_COLORKEY )	// 透過色は処理しない
			{
				red   += Palette[index].peRed;
				green += Palette[index].peGreen;
				blue  += Palette[index].peBlue;
				cnt++;
			}
		}
	}
#endif

	if( cnt == 0 )
		return 0;
	//平均颜色最接近的颜色的调色板寻找指数

	// 平均色に一番近い色のパレットインデックスを探す
	color = getNearestColorIndex( RGB( red/cnt, green/cnt, blue/cnt ), Palette, 256 );

	return color;
}


//---------------------------------------------------------------------------//
// 色数entryのパレットpaletteの中から、colorに一番近いもののindexを返す。
//---------------------------------------------------------------------------//
int getNearestColorIndex( COLORREF color, PALETTEENTRY *palette, int entry )
{
	double distance, mindist;
	int min_index;
	int i;

	mindist = (palette[0].peRed - GetRValue(color))*(palette[0].peRed - GetRValue(color))
		      + (palette[0].peGreen - GetGValue(color))*(palette[0].peGreen - GetGValue(color))
			  +(palette[0].peBlue - GetBValue(color))*(palette[0].peBlue - GetBValue(color));
	min_index = 0;

	for( i = 16; i < entry - 16; i++ )
	{
		distance = (palette[i].peRed - GetRValue(color))*(palette[i].peRed - GetRValue(color))
			+ (palette[i].peGreen - GetGValue(color))*(palette[i].peGreen - GetGValue(color))
			+(palette[i].peBlue - GetBValue(color))*(palette[i].peBlue - GetBValue(color));
		if( distance < mindist )
		{
			min_index = i;
			mindist = distance;
		}
	}

	return min_index;
}



//---------------------------------------------------------------------------//
// マップエフェクトを描画                                                    //
//---------------------------------------------------------------------------//
void DrawMapEffect( void )
{
	DDSURFACEDESC ddsd;	// サーフェス構造体
	char *ptDest;		// 転送先のポインタ
	int surfacePitch;	// サーフェスの横幅記憶
	int ww, hh;
	MAP_EFFECT *buf;


	ww = DEF_APPSIZEX;
	hh = DEF_APPSIZEY;

	//３２０＊２４０ドットモードの時
	if( ResoMode == 1 )
	{
		ww /= 2;
		hh /= 2;
	}


	// 構造体の初期化
	ZeroMemory( &ddsd, sizeof( DDSURFACEDESC ) );
	ddsd.dwSize = sizeof( DDSURFACEDESC );

	// アクセスするサーフェスをロックする( 同時に ddsd に情報を入れてもらう )
	if( lpDraw->lpBACKBUFFER->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) != DD_OK )
	{
		//MessageBox( hWnd, "サーフェスのロックに失敗しました", "確認", MB_OK | MB_ICONSTOP );
		return; 
	}
	// サーフェスの横幅記憶
	surfacePitch = ddsd.lPitch;


	buf = useBufMapEffect;
	while( buf != (MAP_EFFECT *)NULL )
	{
		if( 0 <= buf->x && buf->x < ww-2
		 && 0 <= buf->y && buf->y < hh-2 )
		{
			ptDest = (char *)(ddsd.lpSurface)
				+ buf->y * surfacePitch + buf->x;
			if( buf->type == MAP_EFFECT_TYPE_RAIN )
			{
				*(ptDest-surfacePitch) = (char)143;
				*(ptDest) = (char)143;
				*(ptDest+surfacePitch+1) = (char)143;
				*(ptDest+surfacePitch*2+1) = (char)143;
			}
			else
			if( buf->type == MAP_EFFECT_TYPE_SNOW )
			{
				*(ptDest-surfacePitch-1) = (char)161;
				*(ptDest-surfacePitch) = (char)159;
				*(ptDest-surfacePitch+1) = (char)161;
				*(ptDest-1) = (char)159;
				*(ptDest) = (char)159;
				*(ptDest+1) = (char)159;
				*(ptDest+surfacePitch-1) = (char)161;
				*(ptDest+surfacePitch) = (char)159;
				*(ptDest+surfacePitch+1) = (char)161;
			}
			else
			if( buf->type == MAP_EFFECT_TYPE_STAR )
			{
				if( buf->mode == 0 )
				{
					// 星（白、丸）
					*(ptDest-surfacePitch-1) = (char)8;
					*(ptDest-surfacePitch) = (char)255;
					*(ptDest-surfacePitch+1) = (char)8;
					*(ptDest-1) = (char)255;
					*(ptDest) = (char)255;
					*(ptDest+1) = (char)255;
					*(ptDest+surfacePitch-1) = (char)8;
					*(ptDest+surfacePitch) = (char)255;
					*(ptDest+surfacePitch+1) = (char)8;
				}
				else
				if( buf->mode == 1 )
				{
					// 星（黄色、十字）
					*(ptDest-surfacePitch) = (char)251;
					*(ptDest-1) = (char)251;
					*(ptDest) = (char)251;
					*(ptDest+1) = (char)251;
					*(ptDest+surfacePitch) = (char)251;
				}
				else
				if( buf->mode == 2 )
				{
					// 星（黄色?白、４点）
					*(ptDest) = (char)251;
					*(ptDest+1) = (char)255;
					*(ptDest+surfacePitch) = (char)255;
					*(ptDest+surfacePitch+1) = (char)251;
				}
				else
				if( buf->mode == 3 )
				{
					// 星（白?黄色、４点）
					*(ptDest) = (char)255;
					*(ptDest+1) = (char)251;
					*(ptDest+surfacePitch) = (char)251;
					*(ptDest+surfacePitch+1) = (char)255;
				}
				else
				if( buf->mode == 4 )
				{
					// 星（黄色１点?橙３点）
					*(ptDest) = (char)251;
					*(ptDest+1) = (char)193;
					*(ptDest+surfacePitch) = (char)193;
					*(ptDest+surfacePitch+1) = (char)193;
				}
				else
				if( buf->mode == 5 )
				{
					// 星（薄黄色１）
					*(ptDest) = (char)198;
				}
				else
				if( buf->mode == 6 )
				{
					// 星（薄黄色２）
					*(ptDest) = (char)193;
				}
				else
				if( buf->mode == 7 )
				{
					// 星（薄黄色３）
					*(ptDest) = (char)208;
				}
				else
				if( buf->mode == 8 )
				{
					// 星（黄色）
					*(ptDest) = (char)251;
				}
			}
			else
			if( buf->type == MAP_EFFECT_TYPE_HOTARU )
			{
			}
		}

		buf = buf->next;
	}



	// アクセスするサーフェスをアンロックする
	if( lpDraw->lpBACKBUFFER->Unlock( NULL ) != DD_OK )
	{
		//MessageBox( hWnd, "サーフェスのアンロックに失敗しました", "確認", MB_OK | MB_ICONSTOP );
		return; 
	}

	return;
}






// 処理にかかった時間を描画（ デバッグ用 ）************************************/
void DrawDebugLine( unsigned char color )
{
	DDSURFACEDESC ddsd;	// サーフェス構造体
    char *pDest;		// 転送先のポインタ
	int surfacePitch;	// サーフェスの横幅記憶
	int i, j, k, l;
	
	// ウィンドウモードなら戻る
	if( WindowMode ) return;
	
	// 構造体の初期化
	ZeroMemory( &ddsd, sizeof( DDSURFACEDESC ) );
	ddsd.dwSize = sizeof( DDSURFACEDESC );
	
	// アクセスするサーフェスをロックする( 同時に ddsd に情報を入れてもらう )
	if( lpDraw->lpFRONTBUFFER->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) != DD_OK ){
		//MessageBox( hWnd, "サーフェスのロックに失敗しました", "確認", MB_OK | MB_ICONSTOP );
		return; 
	}	
	//Ｘ座標
	j = 630;
	k = 480;
	l = 8;
	//３２０＊２４０ドットモードの時
	if(ResoMode == 1){
		j >>= 1;
		k >>= 1;
		l >>= 1;
	}
	// 塗り始めるサーフェスのアドレス
	pDest = ( char *)( ddsd.lpSurface ) + j;
	
	// サーフェスの横幅記憶
	surfacePitch = ddsd.lPitch;
	
	// 縦ループ
	for( i = 0 ; i < k ; i++ ){
		// ＬＩＮＥを描画
		memset( pDest, color, l );
		// サーフェスの横幅分アドレスを進める
		pDest += surfacePitch;
    }

	// アクセスするサーフェスをアンロックする
	if( lpDraw->lpFRONTBUFFER->Unlock( NULL ) != DD_OK ){
		MessageBox( hWnd, "サーフェスのアンロックに失敗しました", "確認", MB_OK | MB_ICONSTOP );
		return; 
	}	
	
	return;
}

//---------------------------------------------------------------------------//
// 概要 ：RECT座標が示す??????????から?????????へ高速転送                    //
// 引数 ：DIRECT_DRAW *lpDraw : DirectDraw管理構造体                         //
//        short  bx           : 描画先横位置                                 //
//        short  by           : 描画先縦位置                                 //
//        LPDIRECTDRAWSURFACE lpSurface : 描画元サーフェス                   //
// 戻値 ：DD_OK:正常終了                                                     //
//---------------------------------------------------------------------------//
HRESULT DrawSurfaceFast( short bx, short by, LPDIRECTDRAWSURFACE lpSurface )
{
	short x0, y0;
	long w, h;
	
	// 転送領域のセット
	//RECT rect = { 0, 0, SurfaceSizeX - 1, SurfaceSizeY - 1 }; // デバッグ枠付き
	RECT rect = { 0, 0, SurfaceSizeX, SurfaceSizeY };
	x0 = bx;
	y0 = by;
	w = rect.right - rect.left;
	h = rect.bottom - rect.top;

	// クリッピング処理
	//   （ちなみに RECT の右下座標のドットは表示されない）

	// 全く表示する部分が無ければ戻る
	if( bx >= lpDraw->xSize || bx + w <= 0 || by >= lpDraw->ySize || by + h <= 0 ){
		return DD_OK;
	}
	
	// 左端のチェック
	if( bx < 0 ){
		rect.left -= bx;
		x0 = 0;
	}
	// 右端のチェック
	if( bx + w > lpDraw->xSize ){
		rect.right -= bx + w - lpDraw->xSize;
	}
	// 上端のチェック
	if( by < 0 ){
		rect.top -= by;
		y0 = 0;
	}
	// 下端のチェック
	if( by + h > lpDraw->ySize ){
		rect.bottom -= by + h - lpDraw->ySize;
	}
	
#ifdef _DEBUG		
	// 現在表示しているサーフェスの数カウント
	SurfaceDispCnt++;
#endif
	
	// バックサーフェスへ高速転送
	return lpDraw->lpBACKBUFFER->BltFast( x0, y0, lpSurface, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );
	//return lpDraw->lpBACKBUFFER->BltFast( bx, by, lpSurface, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );
	//return lpDraw->lpBACKBUFFER->BltFast( x0, y0, lpSurface, &rect, DDBLTFAST_WAIT );
	//return lpDraw->lpBACKBUFFER->BltFast( x0, y0, lpSurface, &rect, DDBLTFAST_SRCCOLORKEY );
}

//---------------------------------------------------------------------------//
// 概要 ：RECT座標が示す??????????から?????????へ高速転送                    //
// 引数 ：DIRECT_DRAW *lpDraw : DirectDraw管理構造体                         //
//        short  bx           : 描画先横位置                                 //
//        short  by           : 描画先縦位置                                 //
//        RECT * rect         : 描画元領域構造体                             //
//        LPDIRECTDRAWSURFACE lpSurface : 描画元サーフェス                   //
// 戻値 ：DD_OK:正常終了                                                     //
//---------------------------------------------------------------------------//
HRESULT DrawSurfaceFast2( short bx, short by, RECT *rect, LPDIRECTDRAWSURFACE lpSurface )
{
	short x0, y0;
	long w, h;
	
	x0 = bx;
	y0 = by;
	w = rect->right - rect->left;
	h = rect->bottom - rect->top;

	// クリッピング処理
	//   （ちなみに RECT の右下座標のドットは表示されない）

	// 全く表示する部分が無ければ戻る
	if( bx >= lpDraw->xSize || bx + w <= 0 || by >= lpDraw->ySize || by + h <= 0 ){
		return DD_OK;
	}
	
	// 左端のチェック
	if( bx < 0 ){
		rect->left -= bx;
		x0 = 0;
	}
	// 右端のチェック
	if( bx + w > lpDraw->xSize ){
		rect->right -= bx + w - lpDraw->xSize;
	}
	// 上端のチェック
	if( by < 0 ){
		rect->top -= by;
		y0 = 0;
	}
	// 下端のチェック
	if( by + h > lpDraw->ySize ){
		rect->bottom -= by + h - lpDraw->ySize;
	}
	
#ifdef _DEBUG		
	// 現在表示しているサーフェスの数カウント
	SurfaceDispCnt++;
#endif
	
	// バックサーフェスへ高速転送
	//return lpDraw->lpBACKBUFFER->BltFast( x0, y0, lpSurface, rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );
	return lpDraw->lpBACKBUFFER->BltFast( x0, y0, lpSurface, rect, DDBLTFAST_WAIT );
}

/* オブジェクトを開放する ****************************************************/
void ReleaseDirectDraw( void )
{
	int i;
	
	// 全てのオフスクリーンサーフェスを開放
	for( i = 0 ; i < SurfaceCnt ; i++ ){
		// サーフェスがあったら
		if( SurfaceInfo[ i ].lpSurface != NULL ){
			// リリース
			SurfaceInfo[ i ].lpSurface->Release();
			SurfaceInfo[ i ].lpSurface = NULL;
		}
	}
	// バトルサーフェスの開放
	if( lpBattleSurface != NULL ){
		// リリース
		lpBattleSurface->Release();
		lpBattleSurface = NULL;
	}
	
	// パレット解放
	if( lpDraw->lpPALETTE != NULL ){
		lpDraw->lpPALETTE->Release();
		lpDraw->lpPALETTE = NULL;
	}
	// クリッパー開放
	if( lpDraw->lpCLIPPER != NULL ){
		lpDraw->lpCLIPPER->Release();
		lpDraw->lpCLIPPER = NULL;
	}
	// バックサーフェス解放
	if( lpDraw->lpBACKBUFFER != NULL ){
		lpDraw->lpBACKBUFFER->Release();
		lpDraw->lpBACKBUFFER = NULL;
	}
	// プライマリサーフェス解放
	if( lpDraw->lpFRONTBUFFER != NULL ){
		lpDraw->lpFRONTBUFFER->Release();
		lpDraw->lpFRONTBUFFER = NULL;
	}
	// DirectDrawの開放 //???? IID_IDirectDraw2  ??????
	if( lpDraw->lpDD2 != NULL){ 
		lpDraw->lpDD2->Release();
		lpDraw->lpDD2 = NULL;
	}
	// DIRECT_DRAW 構造体開放
	if( lpDraw != NULL){ 
		GlobalFreePtr( lpDraw );
		lpDraw = NULL;
	}
	
	// DirectDraw 初期化フラグを FALSE にする
	DDinitFlag = FALSE;	
}

// サーフェスロストのチェック関数 ////回程失落的检查函数////释放接口////////////////////////////////////
BOOL CheckSurfaceLost( void )
{
	int i;
	BOOL SurfaceLostFlag = FALSE;
	
	// 全てのオフスクリーンサーフェスを復帰
	for( i = 0 ; i < SurfaceCnt ; i++ ){
		// サーフェスがあったら
		if( SurfaceInfo[ i ].lpSurface != NULL ){
			// サーフェスが壊れていたらリストア
			if( SurfaceInfo[ i ].lpSurface->IsLost() ){
				SurfaceLostFlag = TRUE;
			}
		}
	}
	
	// バトルサーフェスの復帰
	if( lpBattleSurface != NULL ){
		// リリース
		if( lpBattleSurface->IsLost() ){
			SurfaceLostFlag = TRUE;
		}
	}
	
	// バックサーフェス復帰
	if( lpDraw->lpBACKBUFFER != NULL ){
		// サーフェスが壊れていたらリストア
		if( lpDraw->lpBACKBUFFER->IsLost() ){
			SurfaceLostFlag = TRUE;
		}
	}
	// プライマリサーフェス復帰
	if( lpDraw->lpFRONTBUFFER != NULL ){
		// サーフェスが壊れていたらリストア
		if( lpDraw->lpFRONTBUFFER->IsLost() ){
			SurfaceLostFlag = TRUE;
		}
	}
	
	return SurfaceLostFlag;
}	

// フォントオブジェクトの作成 ***字体对象的作成**********************************************/
void InitFont( int fontNo )
{
	// フォントの名前
	char *fnotName[] = { "ＭＳ ゴシック", "ＭＳ 明朝", "ＤＦPOP体" };
	
	// フォントがあったら
	if( hFont != NULL ){
		// フォントオブジェクトを抹殺
		DeleteObject( hFont );
	}
	
	//３２０＊２４０ドットモードの時
	if(ResoMode == 1){
		//フォントの横幅設定
		FontZenkauWidth = FONT_SIZE;
		FontHankakuWidth = FONT_SIZE / 2;
		// MSゴシックにて指定サイズのフォントオブジェクトを作成
		hFont = CreateFont( 
							FONT_SIZE/2, 	/* フォントの論理高	*/ 
							0, 		/* 文字の論理平均幅	*/ 
							0, 		/* テキスト行の角度	*/ 
							0,		/* ベース ラインとx軸の角度	*/ 
							/* フォントの太さ	*/ 
							FW_DONTCARE,		// 0 
							/* イタリック体属性のフラグ	*/ 
							FALSE, 					
							/* 下線付き属性のフラグ	*/ 
							FALSE,					
							/* 打ち消し線付き属性のフラグ	*/ 
							FALSE,					
							/* 文字セット識別子	*/ 
							//DEFAULT_CHARSET, 	// 普通の文字
							//我修改
							GB2312_CHARSET,

							/* 出力精度	*/ 
							OUT_DEFAULT_PRECIS, 
							/* クリッピング精度	*/ 
							CLIP_DEFAULT_PRECIS,
							/* 出力品質	*/ 
							DEFAULT_QUALITY,	// フォントの外観は重要視されません。
							/* ピッチ */ 
							FIXED_PITCH |
							/* ファミリ	*/ 
							FF_ROMAN,			// ○可変ストローク幅を持つ? セリフ付きフォントです。MS(R) Serifなどがあります。
							fnotName[ fontNo ] ); 
	} else {
		//フォントの横幅設定
		FontZenkauWidth = FONT_SIZE + 1;
		FontHankakuWidth = FONT_SIZE / 2 + 1;
		// MSゴシックにて指定サイズのフォントオブジェクトを作成
		hFont = CreateFont( 
							FONT_SIZE, 	/* フォントの論理高	*/ 
							0, 		/* 文字の論理平均幅	*/ 
							0, 		/* テキスト行の角度	*/ 
							0,		/* ベース ラインとx軸の角度	*/ 

							/* フォントの太さ	*/ 
							//FW_DONTCARE,		// 0 
							//FW_THIN,			// 100 
							//FW_EXTRALIGHT,	// 200 
							//FW_LIGHT,			// 300 
							//FW_NORMAL,			// 400 
							//FW_MEDIUM,		// 500 
							//FW_SEMIBOLD,		// 600 
							FW_BOLD,			// 700 
							//FW_EXTRABOLD,		// 800 
							//FW_HEAVY,			// 900
						
							/* イタリック体属性のフラグ	*/ 
							FALSE, 					
							//TRUE,
							
							/* 下線付き属性のフラグ	*/ 
							FALSE,					
							//TRUE,
							
							/* 打ち消し線付き属性のフラグ	*/ 
							FALSE,					
							//TRUE,
						
							/* 文字セット識別子	*/ 
							//ANSI_CHARSET,		// 変な文字
							//UNICODE_CHARSET,	//定義されていない
							//SYMBOL_CHARSET,	// 変な文字
							//SHIFTJIS_CHARSET,	// 普通の文字
							//HANGEUL_CHARSET,	// 普通の文字
							//CHINESEBIG5_CHARSET,// 普通の文字
							//OEM_CHARSET,		// 変な文字
							//原始
							//DEFAULT_CHARSET, 	// 普通の文字
							//我修改
							GB2312_CHARSET,
							
							/* 出力精度	*/ 
							OUT_DEFAULT_PRECIS, 
							//OUT_STRING_PRECIS,
							//OUT_CHARACTER_PRECIS, 
							//OUT_STROKE_PRECIS,
							//OUT_TT_PRECIS,
							//OUT_DEVICE_PRECIS,
							//OUT_RASTER_PRECIS,
							//OUT_TT_ONLY_PRECIS,
							//OUT_OUTLINE_PRECIS,
	
							/* クリッピング精度	*/ 
							CLIP_DEFAULT_PRECIS,
							//CLIP_CHARACTER_PRECIS,
							//CLIP_STROKE_PRECIS,
							//CLIP_MASK,
							//CLIP_LH_ANGLES,
							//CLIP_TT_ALWAYS,
							//CLIP_EMBEDDED,
	
							/* 出力品質	*/ 
							DEFAULT_QUALITY,	// フォントの外観は重要視されません。
							//DRAFT_QUALITY,		// フォントの外観は重要視されます。
							//PROOF_QUALITY,		// フォントの外観は最も重要視されます。
							
							
							/* ピッチ */ 
							FIXED_PITCH |
							
							//DEFAULT_PITCH, 
							//VARIABLE_PITCH, 
							
							//| FF_ROMAN
							
							/* ファミリ	*/ 
							//FF_DECORATIVE,	// ○装飾付きフォントです。Old Englishなどがあります。
							//FF_DONTCARE,		// ×ファミリを特に指定しないフォント? またはファミリが不明のフォントです。
							//FF_MODERN,		// ×固定ストローク幅を持つ? セリフ付きまたはセリフなしのフォントです。Pica? Elite? Courier New(R) などがあります。
							FF_ROMAN,			// ○可変ストローク幅を持つ? セリフ付きフォントです。MS(R) Serifなどがあります。
							//FF_SCRIPT,		// ○手書き風に表示されるフォントです。Script? Cursiveなどがあります。
							//FF_SWISS,			// ○可変ストローク幅を持つ? セリフなし
											
							/* タイプフェイス名文字列のアドレス	*/ 				
							//"MSGOTHIC");			
							//"Times New Roman"); 				
							//"MS Scan Serif");
							//"Small Fonts"); 				
							//"HG正楷書体-PRO"); 				
							//"ＭＳ 明朝"); 		//○		
					//"ＭＳ ゴシック"); 	//○			
							//"MS UI Gothic"); 				
					//"ＤＦPOP体"); 		//○		
							//"HG丸?????M-PRO"); 	//×				
							//"ＨＧ?????E-PRO"); 	//×			
							//"ＤＦ特太ゴシック体"); 	//○			
							fnotName[ fontNo ] ); 
	}
}

// フォントをバックサーフェスにセット //////字体背部实体设置//////////////////////////////////
void PutText( char fontPrio )
{
	HDC  hDc;
	int i;
	//int j;
	BOOL colorFlag = FALSE;
	int	color;
	// フォントパレット
	static DWORD FontPal[]={
#if 1
			RGB(0xff ,0xff, 0xff ), // 0:白
			RGB(0x00 ,0xff, 0xff ), // 1:水色
			RGB(0xff ,0x00, 0xff ), // 2:紫
			RGB(0x00 ,0x00, 0xff ), // 3:青
			RGB(0xff ,0xff, 0x00 ), // 4:黄
			RGB(0x00 ,0xff, 0x00 ), // 5:緑
			RGB(0xff ,0x00, 0x00 ), // 6:赤
			RGB(0xa0 ,0xa0, 0xa4 ), // 7:灰色
			RGB(0xa6 ,0xca, 0xf0 ), // 8:淡い青
			RGB(0xc0 ,0xdc, 0xc0 )  // 9:淡い緑
#else
			0x80000000 | RGB(0xff ,0xff, 0xff ), // 0:白
			0x80000000 | RGB(0x00 ,0xff, 0xff ), // 1:水色
			0x80000000 | RGB(0xff ,0x00, 0xff ), // 2:紫
			0x80000000 | RGB(0x00 ,0x00, 0xff ), // 3:青
			0x80000000 | RGB(0xff ,0xff, 0x00 ), // 4:黄
			0x80000000 | RGB(0x00 ,0xff, 0x00 ), // 5:緑
			0x80000000 | RGB(0xff ,0x00, 0x00 ), // 6:赤
			0x80000000 | RGB(0xa0 ,0xa0, 0xa4 ), // 7:灰色
			0x80000000 | RGB(0xa6 ,0xca, 0xf0 ), // 8:淡い青
			0x80000000 | RGB(0xc0 ,0xdc, 0xc0 )  // 9:淡い緑
#endif		
		};
		
	// 表示する文字がなかったら
	if( FontCnt == 0 ) return;
	
	// バックサーフェスのデバイスコンテキストを取得
	lpDraw->lpBACKBUFFER->GetDC( &hDc );
	// マッピングモードの設定 ( デバイスピクセルにマッピング )
	//nOldMap = SetMapMode( hDc, MM_TEXT );
	//SetMapMode( hDc, MM_TEXT ); // しなくていいかも？
	// 背景表示モードの設定 ( 背景色を無視する )
	SetBkMode( hDc, TRANSPARENT );
	
	//hFont = ( HFONT )GetStockObject(DEFAULT_GUI_FONT);
	
	//hOldFont = ( HFONT )SelectObject( hDc, hFont ); // 以前のフォントを代入
	SelectObject( hDc, hFont ); // 以前のフォントを代入
	
	// 黒色をセット
	SetTextColor( hDc, 0 );
	// 溜めておいた文字をバックサーフェスに描画
	for( i = 0 ; i < FontCnt ; i++ ){
		// 前に表示か後ろに表示か  前表示后面表示吗？
		if( FontBuffer[ i ].fontPrio == fontPrio ){
			//３２０＊２４０ドットモードの時
			if(ResoMode == 1){
				// まず黒色の影を描画
				TextOut( hDc, FontBuffer[ i ].x/2 + 1, FontBuffer[ i ].y/2 + 1, FontBuffer[ i ].str, ( int )strlen( FontBuffer[ i ].str ) ); 
			} else {
				// まず黒色の影を描画
				TextOut( hDc, FontBuffer[ i ].x + 1, FontBuffer[ i ].y + 1, FontBuffer[ i ].str, ( int )strlen( FontBuffer[ i ].str ) ); 
			}
		}
	}
#if 0
	for( i = 0 ; i < FontCnt ; i++ ){
		// その上から指定された色で描画
		SetTextColor( hDc, FontPal[ FontBuffer[ i ].color ] );
		TextOut( hDc, FontBuffer[ i ].x, FontBuffer[ i ].y, FontBuffer[ i ].str, ( int )strlen( FontBuffer[ i ].str ) ); 
	}
#else
	// 色別にループ（ SetTextColorを呼ぶ回数が減るので処理が速い ）
	for( color = 0 ; color < 10 ; color++ ){
		// バッファー分ループ
		for( i = 0 ; i < FontCnt ; i++ ){
			// 前に表示か後ろに表示か
			if( FontBuffer[ i ].fontPrio == fontPrio ){
				// color の色が使われていたら
				if( FontBuffer[ i ].color == color ){
					// まだこの色がセットされていなかったら
					if( colorFlag == FALSE ){
						SetTextColor( hDc, FontPal[ color ] );
						colorFlag = TRUE;
					}
					//３２０＊２４０ドットモードの時
					if(ResoMode == 1){
						// その上から指定された色で描画
						TextOut( hDc, FontBuffer[ i ].x/2, FontBuffer[ i ].y/2, FontBuffer[ i ].str, ( int )strlen( FontBuffer[ i ].str ) ); 
					} else {
						// その上から指定された色で描画
						TextOut( hDc, FontBuffer[ i ].x, FontBuffer[ i ].y, FontBuffer[ i ].str, ( int )strlen( FontBuffer[ i ].str ) ); 
					}
				}
			}
		}
		colorFlag = FALSE;
	}
#endif
	
	// マッピングモードを指定
	//SetMapMode( hDc, nOldMap );
	// 以前のフォントを指定
	//SelectObject( hDc ,hOldFont );
	// フォントオブジェクトを抹殺
	//DeleteObject( hFont );
	// デバイスコンテクストを開放
	lpDraw->lpBACKBUFFER->ReleaseDC( hDc );
}



//---------------------------------------------------------------------------//
// スナップショットの保存  快照
//---------------------------------------------------------------------------//
void snapShot( void )
{
	DDSURFACEDESC ddsdDesc;
	RGBQUAD rgbpal[256];
	char fileName[256];
	int i;
	struct tm *nowTime;
	time_t longTime;
	FILE *fp;
	int w, h;
	POINT 	clientPoint; // クライアント領域の左上のスクリーン座標位置を格納

	// ポイント構造体の初期化
	clientPoint.x = 0;
	clientPoint.y = 0;
	// クライアント領域の左上のスクリーン座標を取得
	ClientToScreen( hWnd, &clientPoint ); 

	// 保存フォルダの作成
	if( _mkdir( "screenshot" ) != 0 )
	{
		if( errno != EEXIST )
			return;
	}

	// 保存ファイル名の作成
	time( &longTime );					// 現在の日時を取得
	nowTime = localtime( &longTime );

	for( i =  0; i < 1000; i ++ )
	{
		sprintf( fileName, "screenshot\\sa%02d%02d%02d_%03d.bmp",
			(nowTime->tm_year % 100), nowTime->tm_mon+1, nowTime->tm_mday, i );
 
		if( (fp = fopen( fileName, "r" )) != NULL )
		{
			// ファイルがあったら次のファイルに進む。
			fclose( fp );
			continue;
		}
		else
		{
			break;
		}
    }


	// パレット取得
//	if( WindowMode )
//	{
//	}
//	else
//	{
		// フルスクリーンモード
		for( i = 0; i < 256; i++ )
		{
			rgbpal[i].rgbRed   = Palette[i].peRed;
			rgbpal[i].rgbGreen = Palette[i].peGreen;
			rgbpal[i].rgbBlue  = Palette[i].peBlue;
			rgbpal[i].rgbReserved = 0;
		}
//	}


	memset( &ddsdDesc, 0, sizeof( DDSURFACEDESC ) );
	ddsdDesc.dwSize = sizeof( DDSURFACEDESC );
	if( lpDraw->lpFRONTBUFFER->Lock( NULL, &ddsdDesc, 0, NULL ) != DD_OK )
		return;

	if( ResoMode == 1 )
	{
		w = 320;
		h = 240;
	}
	else
	{
		w = 640;
		h = 480;
	}
	saveBmpFile( fileName, (BYTE*)ddsdDesc.lpSurface,
		clientPoint.x, clientPoint.y, w, h, ddsdDesc.lPitch, rgbpal, 256 );

	if( lpDraw->lpFRONTBUFFER->Unlock( NULL ) != DD_OK )
		return;
}


// 画像をＢＭＰファイルに保存  为BMP保存到文件
BOOL saveBmpFile( const char *filename, BYTE *buf,
	int x, int y, int width, int height, int srcpitch,
	RGBQUAD *rgbpal, int colorCnt )
{
	HANDLE fh;
	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;
	int linesize = (width+3)/4;
	DWORD writtensize;
	BYTE zero = 0;
	int i;

	fh = CreateFile( filename, GENERIC_WRITE, (DWORD)NULL,
			(LPSECURITY_ATTRIBUTES)NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL );

	if( fh == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}


	fileheader.bfType = 0x4D42;
	fileheader.bfSize = sizeof( BITMAPFILEHEADER )
		+sizeof( BITMAPINFOHEADER )+sizeof(RGBQUAD)*(colorCnt)+linesize*height;
	fileheader.bfReserved1 = 0;
	fileheader.bfReserved2 = 0;
	fileheader.bfOffBits = sizeof( BITMAPFILEHEADER )
		+sizeof( BITMAPINFOHEADER )+sizeof(RGBQUAD)*(colorCnt);

	WriteFile( fh, (void*)(&fileheader), sizeof( fileheader ), &writtensize, NULL );


	infoheader.biSize = sizeof( BITMAPINFOHEADER );
	infoheader.biWidth = width;
	infoheader.biHeight = height;
	infoheader.biPlanes = 1;
	infoheader.biBitCount = 8;
	infoheader.biCompression = NULL;
	infoheader.biSizeImage = sizeof( BITMAPINFOHEADER )
		+sizeof(RGBQUAD)*(colorCnt)+linesize*height;
	infoheader.biXPelsPerMeter = 0;
	infoheader.biYPelsPerMeter = 0;
	infoheader.biClrUsed = colorCnt;
	infoheader.biClrImportant = 0;

	WriteFile( fh, (void*)(&infoheader), sizeof(BITMAPINFOHEADER), &writtensize, NULL );
	WriteFile( fh, (void*)rgbpal, sizeof(RGBQUAD)*(colorCnt), &writtensize, NULL );

	for( i = 0; i < height; i++ )
	{
		WriteFile( fh, (void*)(buf+srcpitch*(height+y-1-i)+x), width, &writtensize, NULL );
		WriteFile( fh, (void*)(&zero), linesize-width, &writtensize, NULL );
	}

	CloseHandle( fh );

	return TRUE;
}

