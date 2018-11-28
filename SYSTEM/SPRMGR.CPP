/************************/
/*	sprmgr.c			*/
/************************/
#include "system.h"
#include "loadrealbin.h"
#include "loadsprbin.h"

//#define SURACE_INFO_MAX 512 	// サーフェスインフォ構造体の最大数
//#define SURACE_INFO_MAX 1024 	// 接口信息结构体的最大数量
#define SURACE_INFO_MAX 2048 	// サーフェスインフォ構造体の最大数
//#define SURACE_INFO_MAX 4096 	// サーフェスインフォ構造体の最大数

//表面和SYSRAM回程的分界线
#define VRAM_SYSRAM_POINT 0 	// VRAM サーフェスと SYSRAM サーフェスの境目
//表面上的BMP抹杀被允许为止的天数
#define SURACE_BMP_DEATH_DATE 2 // サーフェス上のＢＭＰの抹殺が許可されるまでの日数

// スプライトインフォ
SPRITE_INFO SpriteInfo[ MAX_GRAPHICS ];

// サーフェスインフォ
SURFACE_INFO SurfaceInfo[ SURACE_INFO_MAX ];

// バトルサーフェスのポインタ
LPDIRECTDRAWSURFACE lpBattleSurface; 	

// サーフェスを使用した日付   表面使用的日期
UINT SurfaceDate = 3;

// 現在のサーフェス検索位置
int SurfaceSearchPoint;

#ifdef _DEBUG		
// 現在使っているサーフェスの数カウント
int SurfaceUseCnt;

// 現在表示しているサーフェスの数カウント 现在显示的回程的数量计数
int SurfaceDispCnt;
#endif

// 作成したサーフェスの数カウント
int SurfaceCnt;

// VRAM に作成したサーフェスの数カウント
int VramSurfaceCnt;

// SYSTEMRAM に作成したサーフェスの数カウント
int SysramSurfaceCnt;

// 分割サーフェイスの幅
int SurfaceSizeX;
// 分割サーフェイスの高さ
int SurfaceSizeY;

/* ＢＭＰをサーフェスへ割り当て  把对表面分配 ***********************************************/
void AllocateBmpToSurface( int bmpNo )
{
	//int bmpWidth  = lpBmpInfo->bmiHeader.biWidth;	// ＢＭＰの横サイズ
	//int bmpHeight = lpBmpInfo->bmiHeader.biHeight; 	// ＢＭＰの縦サイズ

	int bmpWidth  = RealBinWidth;	// ＢＭＰの横サイズ
	int bmpHeight = RealBinHeight; 	// ＢＭＰの縦サイズ
	
	int offsetX, offsetY; 			// 送るＢＭＰのオフセットポイント  送BMP的坐标点
	int sizeX, sizeY;				// 転送するサイズ
	int surfaceCntX;				// 必要なサーフェスの横枚数
	int surfaceCntY;				// 必要なサーフェスの横枚数
	int totalSurface;				// 必要なサーフェスの総数
	int totalSurfaceCnt = 0;		// 現在の確保したサーフェスの枚数（ ループカウンタ ）
	//周游检索了结束为了最初的位置记忆
	int	SurfaceSearchPointBak = SurfaceSearchPoint; // 一周検索したら終了するため最初の位置を記憶
	int amariSizeX = FALSE;		// 横に余りがあるかフラグ  旁边多有转折点
	int amariSizeY = FALSE;		// 縦に余りがあるかフラグ
	BOOL vramFullFlag = FALSE; 		// VRAMがいっぱいかどうか
	SURFACE_INFO *prevSurfaceInfo; 	// 前のサーフェスインフォ構造体のアドレス
	
	// 送るＢＭＰのオフセットポイント   送BMP的坐标点
	offsetX = 0; 
	offsetY = bmpHeight;
	
	// 必要なサーフェスの横枚数計算
	surfaceCntX = bmpWidth / SURFACE_WIDTH;
	
	// 横に余りがあったら半端サイズを記憶
	if( ( amariSizeX = bmpWidth % SURFACE_WIDTH ) ){ 
		surfaceCntX++;		// もうひとつ必要
	}
	
	// 必要なサーフェスの縦枚数計算
	surfaceCntY = bmpHeight / SURFACE_HEIGHT;
	
	// 縦に余りがあったら半端サイズを記憶
	if( ( amariSizeY = bmpHeight % SURFACE_HEIGHT ) ){ 
		surfaceCntY++;		// もうひとつ必要
	}
	// 必要なサーフェスの総数計算
	totalSurface  = surfaceCntX * surfaceCntY;
	
#if 0
	// 横幅を４のバウンダリにする
	if( ( lpBmpInfo->bmiHeader.biWidth & 3 ) ){
		lpBmpInfo->bmiHeader.biWidth += 4 - lpBmpInfo->bmiHeader.biWidth & 3;
	}
#else	// Realbin 読み込むとき
	// 横幅を４のバウンダリにする
	if( ( RealBinWidth & 3 ) ){
		RealBinWidth += 4 - RealBinWidth & 3;
	}
#endif

	// 空いているサーフェスを探す
	// サーフェスの数だけループ
	/// /接口的表面寻找/ /表面的数量循环
	while( 1 ){
		// 使用できる状態なら  可使用状态的话
		if( SurfaceInfo[ SurfaceSearchPoint ].date < SurfaceDate - SURACE_BMP_DEATH_DATE ){
#ifdef _DEBUG		
			// 現在使っているサーフェスの数カウント
			SurfaceUseCnt++;
#endif
			// 上書きする時
			if( SurfaceInfo[ SurfaceSearchPoint ].bmpNo != -1 ){
				// サーフェスインフォ構造体のアドレス
				SURFACE_INFO *lpSurfaceInfo;
				
				// サーフェスリストの先頭アドレスを記憶
				lpSurfaceInfo = SpriteInfo[ SurfaceInfo[ SurfaceSearchPoint ].bmpNo ].lpSurfaceInfo;
				
				// 前にいたＢＭＰとサーフェスのリンクを削除
				SpriteInfo[ SurfaceInfo[ SurfaceSearchPoint ].bmpNo ].lpSurfaceInfo = NULL;
				
				// サーフェスリストの初期化ループ
				for( ; lpSurfaceInfo != NULL;
					lpSurfaceInfo = lpSurfaceInfo->pNext ){
					
					// ＢＭＰ番号初期化
					lpSurfaceInfo->bmpNo = -1;
					
#ifdef _DEBUG		
					// 現在使っているサーフェスの数マイナス
					SurfaceUseCnt--;
#endif
				}
				
			}
			// サーフェスリストの先頭アドレスを記憶するとき
			if( SpriteInfo[ bmpNo ].lpSurfaceInfo == NULL ){
				// スプライトインフォに先頭アドレスを記憶する
				SpriteInfo[ bmpNo ].lpSurfaceInfo = &SurfaceInfo[ SurfaceSearchPoint ];
				
			}else{ // つなげるとき
			
				// 覚えておいた前のサーフェスインフォ構造体にアドレスを教える
				prevSurfaceInfo->pNext = &SurfaceInfo[ SurfaceSearchPoint ];
			}
			
			// ＢＭＰ番号を記憶する
			SurfaceInfo[ SurfaceSearchPoint ].bmpNo = bmpNo;
			
			// オフセット座標を覚える
			//３２０＊２４０ドットモードの時
			//胶印坐标记320×240像素/ /模式的时候
			if(ResoMode == 1){
				SurfaceInfo[ SurfaceSearchPoint ].offsetX = offsetX / 2;
				SurfaceInfo[ SurfaceSearchPoint ].offsetY = ( bmpHeight - offsetY ) / 2;
			} else {
				//３２０＊２４０ドットモード以外の時
				SurfaceInfo[ SurfaceSearchPoint ].offsetX = offsetX;
				SurfaceInfo[ SurfaceSearchPoint ].offsetY = bmpHeight - offsetY;
			}
			
			// 横に余りがあるときは転送する前にサーフェスを黒でクリアー
			if( offsetX >= bmpWidth - SURFACE_WIDTH && amariSizeX ){
				ClearSurface( SurfaceInfo[ SurfaceSearchPoint ].lpSurface );
				// 余りサイズ
				sizeX = amariSizeX;				
			}else sizeX = SURFACE_WIDTH;
			
			// 縦に余りがあるときは転送する前にサーフェスを黒でクリアー
			if( offsetY - SURFACE_HEIGHT <= 0 && amariSizeY ){
				// 先に黒でクリアーされてないとき
				if( sizeX != amariSizeX ){
					ClearSurface( SurfaceInfo[ SurfaceSearchPoint ].lpSurface );
				}
				// 余りサイズ
				sizeY = amariSizeY;
			}else sizeY = SURFACE_HEIGHT;
			
			// ＢＭＰをサーフェスへ転送
			DrawBitmapToSurface2( 	SurfaceInfo[ SurfaceSearchPoint ].lpSurface, 
									offsetX, 
									offsetY - 1, 
									sizeX,
									sizeY,
									lpBmpInfo );

			// 現在の確保したサーフェスの枚数をカウント
			totalSurfaceCnt++;

			// すべて確保し終わったら
			if( totalSurfaceCnt >= totalSurface ){
				//　pNext にＮＵＬＬを入れて終了
				SurfaceInfo[ SurfaceSearchPoint ].pNext = NULL;
				// 検索位置を進ませる
				SurfaceSearchPoint++;
				
#if 0			// VRAMを優先使用バージョン

				// VRAMのサーフェスを検索するとき
				if( vramFullFlag == FALSE ){
					// リミットチェック
					if( SurfaceSearchPoint >= VramSurfaceCnt ) SurfaceSearchPoint = 0;
				}else{
					// 検索位置を戻す
					SurfaceSearchPoint = SurfaceSearchPointBak;
				}
				
#else			// VRAMとSYSRAMを同レベルで使用バージョン

				// リミットチェック
				if( SurfaceSearchPoint >= SurfaceCnt ) SurfaceSearchPoint = 0;
#endif
				break;
				
			}else{
				// 今のサーフェスインフォ構造体のアドレスを覚える
				prevSurfaceInfo = &SurfaceInfo[ SurfaceSearchPoint ];
				
				// 右端まで送ったら
				if( offsetX >= bmpWidth - SURFACE_WIDTH ){ 
					offsetX = 0;
					offsetY -= SURFACE_HEIGHT;
				}else{ 
					offsetX += SURFACE_WIDTH;
				}
			}
		}
		// 検索位置を進ませる
		SurfaceSearchPoint++;
		
#if 0	// VRAMを優先使用バージョン

		// VRAMのサーフェスを検索するとき
		if( vramFullFlag == FALSE ){
			// VRAMサーフェスの最後まで検索したとき
			if( SurfaceSearchPoint >= VramSurfaceCnt ) SurfaceSearchPoint = 0;
			// 一周検索したらVRAMに空きなし状態であきらめる
			if( SurfaceSearchPoint == SurfaceSearchPointBak ){ 
				//MessageBox( hWnd, "ＶＲＡＭに空きがありません。", "確認", MB_OK );
				// 検索位置をSYSTEMサーフェスが存在するところへ移動
				SurfaceSearchPoint = VramSurfaceCnt + 1;
				vramFullFlag = TRUE;
			}
		}
		// SYSTEMRAMのサーフェスを検索するとき
		if( vramFullFlag == TRUE ){
			// 最後まで検索したら
			if( SurfaceSearchPoint >= SurfaceCnt ){ 
				// 検索位置を戻す
				SurfaceSearchPoint = SurfaceSearchPointBak;
				//MessageBox( hWnd, "サーフェス不足です。", "確認", MB_OK );
				break;
			}
		}
		
#else	// VRAMとSYSRAMを同レベルで使用バージョン
		
		// 最後まで検索したら
		if( SurfaceSearchPoint >= SurfaceCnt ){ 
			// 最初に戻る
			SurfaceSearchPoint = 0;
		}
		// 一周検索したらサーフェスに空きなし状態であきらめる
		if( SurfaceSearchPoint == SurfaceSearchPointBak ){ 
			//MessageBox( hWnd, "サーフェス不足です。", "確認", MB_OK );
			break;
		}
	}
#endif
}

/* オフスクリーンサーフェスの作成 *********************************************/
BOOL InitOffScreenSurface( void )
{
	int i;
	BOOL vramFullFlag = FALSE; // VRAMがいっぱいかどうか 是否有很多
	
	// サーフェスの数 表面的难到是显示器？
	SurfaceCnt = 0;
	// VRAM に作成したサーフェスの数
	VramSurfaceCnt = 0;
	// SYSTEMRAM に作成したサーフェスの数
	SysramSurfaceCnt = 0;
	
	// バトルサーフェスの作成
	if( ( lpBattleSurface = CreateSurface( DEF_APPSIZEX, DEF_APPSIZEY, DEF_COLORKEY, DDSCAPS_VIDEOMEMORY )) == NULL ){
		// バトルサーフェスの作成
		if( ( lpBattleSurface = CreateSurface( DEF_APPSIZEX, DEF_APPSIZEY, DEF_COLORKEY, DDSCAPS_SYSTEMMEMORY )) == NULL ){
			MessageBox( hWnd ,"バトルサーフェスの作成に失敗しました。" ,"確認",MB_OK | MB_ICONSTOP );
			return FALSE;
		}
	}
	// SURACE_INFO_MAX までサーフェスを作る   表面到制作*/
	for( i = 0 ; i < SURACE_INFO_MAX ; i++ ){
	
#ifdef _DEBUG		
#if VRAM_SYSRAM_POINT
		// 今だけ（ チェック用 )  只是现在（用）检查
		if( i == VRAM_SYSRAM_POINT ){
			 vramFullFlag = TRUE;
		}
#endif
#endif

		// VRAMにサーフェスを作成できるとき 表面上能作成的时候
		if( vramFullFlag == FALSE ){
			// DirectDraw用にBITMAPを格納するためのサーフェスを作成 用BITMAP来存储的表面作成
			if( ( SurfaceInfo[ i ].lpSurface = CreateSurface( SurfaceSizeX, SurfaceSizeY, DEF_COLORKEY, DDSCAPS_VIDEOMEMORY )) == NULL ){
				//MessageBox( hWnd ,"ＶＲＡＭサーフェスの作成に失敗しました。" ,"確認",MB_OK | MB_ICONSTOP );
				vramFullFlag = TRUE;
			}else{
				// VRAM に作成したサーフェスの数カウント
				VramSurfaceCnt++;
			}
		}
			
		// SYSTEMRAMにサーフェスを作成するとき  表面上制作的时候
		if( vramFullFlag == TRUE ){
			if( ( SurfaceInfo[ i ].lpSurface = CreateSurface( SurfaceSizeX, SurfaceSizeY, DEF_COLORKEY, DDSCAPS_SYSTEMMEMORY )) == NULL ){
				MessageBox( hWnd ,"ＳＹＳＲＡＭサーフェスの作成に失敗しました。" ,"確認",MB_OK | MB_ICONSTOP );
				return FALSE;
			}else{
				// SYSTEMRAM に作成したサーフェスの数カウント
				SysramSurfaceCnt++;
			}
		}
	}
	
	// 作成したサーフェスの数を記憶
	SurfaceCnt = i;
	
	/* サーフェスインフォ構造体の初期化  回程信息结构的初期化*/
	InitSurfaceInfo();
	/* スプライトインフォ構造体の初期化 */
	InitSpriteInfo();
		
	
	return TRUE;
}

/* サーフェスインフォ構造体の初期化　**************************************/
void InitSurfaceInfo( void )
{
	int i;
	
#ifdef _DEBUG		
	// 現在使っているサーフェスの数初期化
	SurfaceUseCnt = 0;
#endif
	
	// サーフェス検索位置初期化
	SurfaceSearchPoint = 0;
	
	// サーフェス数だけループ */
	for( i = 0 ; i < SurfaceCnt ; i++ ){
	
		// サーフェスインフォ構造体の初期化
		SurfaceInfo[ i ].bmpNo = -1;
		SurfaceInfo[ i ].date = 0;
		SurfaceInfo[ i ].pNext = NULL;
	}
}	

/* 接口 构造体初始化 構造体の初期化　**************************************/
void InitSpriteInfo( void )
{
	int i;
	
	// ＢＭＰの数だけループ */
	for( i = 0 ; i < MAX_GRAPHICS ; i++ ){
		// VRAM にいないことにする
		SpriteInfo[ i ].lpSurfaceInfo = NULL;
	}
}	

// ＢＭＰをロードする ***从硬盘加载******************************************************/
BOOL LoadBmp( int bmpNo )
{
	// ＶＲＡＭにいないときはハードディスクからロードする  没有硬盘从视频卡？
	if( SpriteInfo[ bmpNo ].lpSurfaceInfo == NULL )
	{
		// ＢＭＰ番号からイメージデータを返す( Realbin から読み込む )
		if( realGetImage( 	bmpNo, 
							( unsigned char **)&pRealBinBits, 
							&RealBinWidth, 
							&RealBinHeight ) == FALSE ){
							
			//MessageBox( hWnd, "イメージデータの読み込みに失敗しました。", "確認", MB_OK );
			return FALSE;
		}
		// ＢＭＰのサイズを記憶
		SpriteInfo[ bmpNo ].width = RealBinWidth;
		SpriteInfo[ bmpNo ].height = RealBinHeight;
		// ＢＭＰをサーフェスへ割り当て 
		AllocateBmpToSurface( bmpNo );
	}
	return TRUE;
}

// ＢＭＰをロードする（グラフィック番号変換もする） **人物平面号码变换的 从硬盘*************************/
BOOL LoadBmp2( int bmpNo )
{
	// RIAL.BIN　番号にする
	realGetNo( bmpNo , (U4 *)&bmpNo );
	// ＶＲＡＭにいないときはハードディスクからロードする
	if( SpriteInfo[ bmpNo ].lpSurfaceInfo == NULL ){
		// ＢＭＰ番号からイメージデータを返す( Realbin から読み込む )
		//号的图像数据返回（Realbin读取）从
		if( realGetImage( 	bmpNo, 
							( unsigned char **)&pRealBinBits, 
							&RealBinWidth, 
							&RealBinHeight ) == FALSE ){
							
			//MessageBox( hWnd, "イメージデータの読み込みに失敗しました。", "確認", MB_OK );
			return FALSE;
		}
		// ＢＭＰのサイズを記憶
		SpriteInfo[ bmpNo ].width = RealBinWidth;
		SpriteInfo[ bmpNo ].height = RealBinHeight;
		// ＢＭＰをサーフェスへ割り当て 
		AllocateBmpToSurface( bmpNo );
	}
	return TRUE;
}


/* ビットマップの読み込み *****************************************************/
void OpenBmp( void )
{
	int i;
   	char *fileName[]=
	/* 0 */	  { "data\\spr\\自.bmp","data\\spr\\敵１.bmp","data\\spr\\敵１２.bmp","data\\spr\\敵２.bmp","data\\spr\\敵２２.bmp",
	/* 5 */		"data\\spr\\敵３２.bmp","data\\spr\\敵３.bmp","data\\spr\\敵４.bmp","data\\spr\\敵４２.bmp","data\\spr\\敵５.bmp",
	/*10 */		"data\\spr\\敵５２.bmp","data\\spr\\消す.bmp","data\\spr\\Ｍ.bmp","data\\spr\\Ｗ.bmp","data\\spr\\Ｓ.bmp",
	/*15 */		"data\\spr\\Ｍ２.bmp","data\\spr\\Ｗ２.bmp","data\\spr\\Ｓ２.bmp","data\\spr\\Ｍ３.bmp","data\\spr\\Ｗ３.bmp",
	/*20 */		"data\\spr\\Ｓ３.bmp","data\\spr\\爆発１.bmp","data\\spr\\爆発２.bmp","data\\spr\\爆発３.bmp", "data\\spr\\自弾.bmp",
	/*25 */		"data\\spr\\自弾消.bmp","data\\spr\\敵弾.bmp","data\\spr\\敵弾消.bmp","data\\spr\\黄色弾.bmp","data\\spr\\青色弾.bmp",
	/*30 */		"data\\spr\\風呂場.bmp","data\\spr\\pl01.bmp","data\\spr\\フォント.bmp","data\\spr\\st\\ken.bmp" };

	//BITMAPファイルの展開及びメモリー確保 文件的展开，确保及存储器
	//lpBmp[ 0 ] = LoadDirectDrawBitmap( fileName[ 0 ] ); 
	//if( lpBmp[ 0 ] == NULL ) MessageBox( hWnd, "ＢＭＰのオープンに失敗しました。", "確認", MB_OK );
	
	//if( !WindowMode ){
		// システムパレット初期化
	//	InitPalette( lpBmp[ 0 ] );
	//}
	
	// 作れるだけサーフェスを作る */
	for( i = 0 ; i < SURACE_INFO_MAX ; i++ ){
	
		// サーフェスインフォ構造体の初期化
		SurfaceInfo[ i ].bmpNo = -1;
		SurfaceInfo[ i ].pNext = NULL;
		
		// DirectDraw用にBITMAPを格納するためのサーフェスを作成
		if( ( SurfaceInfo[ i ].lpSurface = CreateSurface( SURFACE_WIDTH, SURFACE_HEIGHT, DEF_COLORKEY, 0 )) == NULL ){
			MessageBox( hWnd ,"サーフェスの初期化に失敗しました。" ,"確認",MB_OK | MB_ICONSTOP );
			break;
			//DestroyWindow( hWnd );
		}
		
		// サーフェスの数
		SurfaceCnt++;
		
		// BITMAPをサーフェス(座標[0,0])へ複写
		//DrawBitmapToSurface( lpSurface[ i ], 0, 0 ,lpBmp[ 0 ] );
		//SetRect( &Rect, 0, 0, lpBmp[ 0 ]->bmiHeader.biWidth, lpBmp[ 0 ]->bmiHeader.biHeight ); 
	}
}


