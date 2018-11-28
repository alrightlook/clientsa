/**** SYSTEM INCLUDE ****/
#include "system.h"
#include "winnls32.h"
#include <ime.h>
#include <imm.h>
#include "ime_sa.h"
#include "../resource.h"
#include "netmain.h"
#include "battleMap.h"
#include "battleProc.h"
#include "netmain.h"


#if 1
// 通常時
#define REALBIN_DIR		"data\\real_1.bin" 		// REAL.BINの場所
#define ADRNBIN_DIR		"data\\adrn_1.bin" 		// ADRN.BINの場所
#define SPRBIN_DIR		"data\\spr_1.bin" 		// SPR.BINの場所
#define SPRADRNBIN_DIR	"data\\spradrn_1.bin" 	// SPRADRN.BINの場所

#else

// グラフィックマシン用
#define REALBIN_DIR		"..\\realbin\\data\\real.bin" 		// REAL.BINの場所
#define ADRNBIN_DIR		"..\\realbin\\data\\adrn.bin" 		// ADRN.BINの場所
#define SPRBIN_DIR		"..\\spr\\spr.bin" 					// SPR.BINの場所
#define SPRADRNBIN_DIR	"..\\spr\\spradrn.bin" 				// SPRADRN.BINの場所

#endif


//---------------------------------------------------------------------------//
// 概要 ：グローバル変数定義                                                 //
//---------------------------------------------------------------------------//
HINSTANCE	hInst;			// インスタンスハンドル定義 实例窗口句柄定义
HWND hWnd;					// ウィンドウハンドル  窗口
int	CmdShow;				// WinMain関数の引数をグローバルにする  WinMain函数的论据和全球的
LPSTR CmdLine;				// WinMain関数のコマンドライン引数をグローバルにする  WinMain函数的命令行参数全球的
HANDLE hMutex;				// 同時起動チェックオブジェクトのハンドル  同时启动检查对象的方向盘

BOOL WindowMode = TRUE;		// ウィンドウモード/ / / /窗口模式
//BOOL WindowMode = FALSE;	// フルスクリーンモード/ /????????
							//	TRUE  : WindowMode
							//	FALSE : FullScreen
BOOL NoDelay = TRUE;		// TCP_NODELAY を使うかどうか。add by ringo  TCP_NODELAY是否使用 一种封包优化
int ResoMode = 0;			// 解像度モード（０：６４０＊４８０　１：３２０＊２４０　２：可変）分辨率模式
int LowResoCmdFlag = 0;		// 解像度変更コマンドフラグ  分辨率变更指令转折点

#ifdef _DEBUG
//BOOL offlineFlag = FALSE;	// オフラインフラグ 离线转折点  脱机模式
BOOL offlineFlag = FALSE;  //没用以后查原因
char DebugKey0[ 256 ];		// デバックキー０ 调试键0
char DebugKey1[ 256 ];		// デバックキー１
char DebugKey2[ 256 ];		// デバックキー２
#endif


char realBinName[256];
char adrnBinName[256];
char sprBinName[256];
char sprAdrnBinName[256];


/* コマンドライン解析   命令行解析 */
void AnalyzeCmdLine( void );
// ウィンドウモード切り替え関数宣言
//窗口模式转换函数宣言
void ChangeWindowMode( void );
// 解像度モード設定処理
//分辨率模式设定处理
void SetResoMode(int Mode);

// Windowsのメッセージをフックする  的信息的钩
//HRESULT CALLBACK HookProc( int nCode, WPARAM wParam, LPARAM lParam );				//メインプロシージャー

//---------------------------------------------------------------------------//
// 概要 ：メインループ処理関数                                               //
// 引数 ：HANDLE hInstance                                                   //
//        HANDLE hPrevInstance                                               //	
//        LPSTR lpCmdLine                                                    //
//        int   nCmdShow                                                     //
//---------------------------------------------------------------------------//
int PASCAL WinMain( HINSTANCE hInstance ,HINSTANCE hPrevInstance ,LPSTR lpCmdLine ,int nCmdShow )
{
	WNDCLASS wndclass;
	hInst = hInstance;		// ???????でもらった???????????を????????に代入

	CmdShow = nCmdShow;		// WinMain関数の引数をグローバルにする
	CmdLine = lpCmdLine;	// WinMain関数のコマンドライン引数をグローバルにする	
	
#ifndef _DEBUG
//#ifdef _DEBUG
	// 同時起動チェックオブジェクト作成  同时启动检查对象作成 没起作用以后查看下
	hMutex = CreateMutex( NULL, TRUE, DEF_APPNAME );
	// 同時に二つ立ち上げた時  同时两个启动的时候
	if( GetLastError() == ERROR_ALREADY_EXISTS ){
		MessageBox( hWnd, "ＳｔｏｎｅＡｇｅ已经启动，无法多次启动。", "確認", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	// アップデートから実行したかチェック  从检查更新实行了吗
	if( !strstr( CmdLine, "updated" ) ){
		MessageBox( hWnd, "ＳｔｏｎｅＡｇｅ.ｅｘｅ请启动。", "確認", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
#endif

    //*** ウインドウクラスの生成 ***窗口班的生成
	//如果其他的实例启动中省略的话初期化
    if( !hPrevInstance )	// もし他のインスタンスが起動中なら初期化省略 
	{
		wndclass.style = CS_BYTEALIGNCLIENT | CS_DBLCLKS;	// クラススタイル
		wndclass.lpfnWndProc = WindMsgProc;		// ウインドウプロシージャーアドレス定義
		wndclass.cbClsExtra = 0;				// クラス毎の補足定義なし
		wndclass.cbWndExtra = 0;				// ウインドウ毎の補足データ無し
		wndclass.hInstance = hInstance;			// インスタンスハンドル定義
//		wndclass.hIcon = LoadIcon(hInstance ,"ICON1");	// ????からICON1をロード
	    wndclass.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE(SA_ICON2));
		//wndclass.hIcon = LoadIcon( NULL, IDI_APPLICATION);

	
#ifdef _DEBUG
		wndclass.hCursor = (LoadCursor(hInstance ,MAKEINTRESOURCE(SA_MOUSE)));	// 光标を矢印に設定SA_MOUSE
#else		
		wndclass.hCursor = LoadCursor(NULL ,IDC_ARROW);	// ???????を矢印に設定  箭设定
		//wndclass.hCursor = LoadCursor(NULL ,MAKEINTRESOURCE(SA_MOUSE));	// ???????を矢印に設定  箭设定
#endif
		
		//窗口的背景色
		wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);// ウィンドウの背景色
		wndclass.lpszMenuName= NULL;			// メニューなし  没有菜单
		wndclass.lpszClassName = DEF_APPNAME;	// タイトルバー文字設定  标题栏文字设定
		if( !RegisterClass(&wndclass) )
			return FALSE;						// 作成に失敗したら終了する
	}

	// 読み込みファイル名の設定  读取文件名的设定
	strcpy( realBinName, REALBIN_DIR );			// REAL.BINの場所
	strcpy( adrnBinName, ADRNBIN_DIR );			// ADRN.BINの場所
	strcpy( sprBinName, SPRBIN_DIR );			// SPR.BINの場所
	strcpy( sprAdrnBinName, SPRADRNBIN_DIR );	// SPRADRN.BINの場所
	
	//コマンドライン解析   命令行
	AnalyzeCmdLine();
	
	// 解像度モード設定   分辨率模式设定
	SetResoMode( ResoMode );
	
	// ウィンドウの作成  窗口的作成
	ChangeWindowMode();
	// ウィンドウの作成
	//ChangeWindowMode();
	
	// 全てのウィンドウからキーボード入力を奪う  从所有的窗口键盘输入夺走
	//SetFocus( hWnd );

	//{
		// ALT+TAB 無効にする
	//	int nOldVal;
	//	SystemParametersInfo (SPI_SCREENSAVERRUNNING, TRUE, &nOldVal, 0);
	//}
	
	// Windowsのメッセージをフックプロジージャをセットする
	///SetWindowsHookEx( WH_SYSMSGFILTER, HookProc, hInstance, 0 );
	//SetWindowsHookEx( WH_MSGFILTER, HookProc, hInstance, 0 );
	//SetWindowsHookEx( WH_KEYBOARD, HookProc, hInstance, 0 );
	//SetWindowsHookEx( WH_CALLWNDPROC, HookProc, hInstance, 0 );
	//SetWindowsHookEx( WH_CBT, HookProc, hInstance, 0 );
	
	// ????????
	GameMain();
	
	return FALSE;
}

#ifdef _DEBUG

// デバックキー解析 **调试键解析*********************************************************/
void DebugKey( char *str )
{
	char *work;
	int i = 0;
	
	// コマンドラインオプションに key0: があるとき
	//命令行选项key0:的时候
	if( ( work = strstr( str, "KEY0:" ) ) )
	{
		work += 5;	// 文字列のあるところまでポインタ進める
		// : がくるまでループ
		//文字列的地方为止推进指针/ /:到循环
		while( *work != ':' ){
			DebugKey0[ i ] = *work++;
			i++;
			if( i == 255 ){ 
				i = 0;
				break;
			}
		}
	}
	DebugKey0[ i ] = NULL;	// 終端記号
	i = 0;
	// コマンドラインオプションに key1: があるとき
	if( ( work = strstr( str, "KEY1:" ) ) )
	{
		work += 5;	// 文字列のあるところまでポインタ進める
		// : がくるまでループ
		while( *work != ':' ){
			DebugKey1[ i ] = *work++;
			i++;
			if( i == 255 ){ 
				i = 0;
				break;
			}
		}
	}	
	DebugKey1[ i ] = NULL;	// 終端記号
	i = 0;
	// コマンドラインオプションに key1: があるとき
	if( ( work = strstr( str, "KEY2:" ) ) )
	{
		work += 5;	// 文字列のあるところまでポインタ進める
		// : がくるまでループ
		while( *work != ':' ){
			DebugKey2[ i ] = *work++;
			i++;
			if( i == 255 ){ 
				i = 0;
				break;
			}
		}
	}	
	DebugKey2[ i ] = NULL;	// 終端記号
}

#endif
/* コマンドライン解析 ****命令行解析*****************************************************/
void AnalyzeCmdLine( void )
{
	char *addr;

	// 読み込みファイル名の変更
	if( addr = strstr( CmdLine, "realbin:" ) )
	{
		sprintf( realBinName, "data\\real_%d.bin", atoi(addr+strlen("realbin:")) );
	}
	if( addr = strstr( CmdLine, "adrnbin:" ) )
	{
		sprintf( adrnBinName, "data\\adrn_%d.bin", atoi(addr+strlen("adrnbin:")) );
	}
	if( addr = strstr( CmdLine, "sprbin:" ) )
	{
		sprintf( sprBinName, "data\\spr_%d.bin", atoi(addr+strlen("sprbin:")) );
	}
	if( addr = strstr( CmdLine, "spradrnbin:" ) )
	{
		sprintf( sprAdrnBinName, "data\\spradrn_%d.bin", atoi(addr+strlen("spradrnbin:")) );
	}

	// コマンドラインオプションに windowmode があるとき
	/// /?郢﨧?????荢indowmode / / nodelay???釭? ???????????????????葷?0.4???
	if( strstr( CmdLine, "windowmode" ) ) WindowMode = TRUE;
	
	// nodelay???釭? ???????????????????葷?0.4??? オプションの設定。（通信待ち０．４秒ありにする時）
	if( strstr( CmdLine, "nodelay" ) ) NoDelay = FALSE;
	
#ifdef _DEBUG
	// オフラインモードの設定  脱机模式的设定
	if( strstr( CmdLine, "offline" ) )
	{
		offlineFlag = TRUE;
	}
	else
	{
		offlineFlag = FALSE;
	}
	if( offlineFlag )
	{
		ProcNo = PROC_GAME;	
	}
	else
	{
		ProcNo = PROC_ID_PASSWORD;
	}
	if( strstr( CmdLine, "PROC_OHTA_TEST" ) ) 
	{
		ProcNo = PROC_OHTA_TEST;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "PROC_TAKE_TEST" ) )
	{
	 	ProcNo = PROC_TAKE_TEST;
		offlineFlag = TRUE;
	 }
	if( strstr( CmdLine, "PROC_DWAF_TEST" ) )
	{
	 	ProcNo = PROC_DWAF_TEST;
		offlineFlag = TRUE;
	 }
	if( strstr( CmdLine, "PROC_BATTLE" ) )
	{
		ProcNo = PROC_BATTLE;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "sprview" ) )
	{
	 	ProcNo = PROC_SPR_VIEW;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "animview" ) )
	{
	 	ProcNo = PROC_ANIM_VIEW;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "setest" ) )
	{
	 	ProcNo = PROC_SE_TEST;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "encountoff" ) )
	{
		EncountOffFlag = TRUE;
	}
	if( strstr( CmdLine, "lowreso" ) )
	{
		ResoMode = 1;
	}
#ifdef _LOG_MSG
	if( strstr( CmdLine, "RECVDATA:" ) )
	{
		char *pt = strstr( CmdLine, "RECVDATA:" )+9;
		char *name;
		int i;

		name = debugLogFileName;
		for( i = 0; i < 255; i++ )
		{
			if( *pt != ' ' && *pt != '\0' && *pt != '\t' )
			{
				*name++ = *pt++;
			}
			else
			{
				break;
			}
		}
		*name = '\0';
	}
#endif
	//  ??????釤?
	DebugKey( CmdLine );
#else
	// デフォルトの場合はこれ 默认的情况这是
	ProcNo = PROC_ID_PASSWORD;
#endif
	// 命令行的IP地址解析  コマンドラインのIPアドレス解析
	ipAddressAnalyze( CmdLine );
}
/* ウィンドウモード切り替え ***************************************************/
void ChangeWindowMode( void )
{
	RECT clientRect;		// クライアントの表示領域
	DWORD windowStyle;		// ウィンドウスタイル

	// ウィンドウモードの時
	if( WindowMode ){
	
		// ウィンドウスタイル
		windowStyle = WS_MINIMIZEBOX | WS_SYSMENU | 
			            //我开启1
						WS_THICKFRAME |
						WS_CAPTION | WS_OVERLAPPED;
							
		// ウィンドウ作成時でなかったら
		if( DDinitFlag ){
			// マウスカーソルを表示
			//ShowCursor( TRUE );
		}
	}
	// フルスクリーンの時
	else{	
	
		// ウィンドウスタイル
		windowStyle = WS_VISIBLE | WS_POPUP;
		// マウスカーソルを隠す
		//ShowCursor( FALSE );
	}
	
	// クライアント領域の設定
	SetRect( &clientRect, 0, 0, lpDraw->xSize, lpDraw->ySize );
	// クライアント領域のサイズからウィンドウサイズを計算し clientRect へ入れる
	//从窗口尺寸大小客户领域的计算clientRect放进
	AdjustWindowRectEx( &clientRect, windowStyle, FALSE, NULL );
	// ウィンドウを作成するとき
	//if( !DDinitFlag ){	// DirectDrawが初期化されてないとき
	if( hWnd == NULL ){	// DirectDrawが初期化されてないとき
	
		// ウインドウの生成
		hWnd = CreateWindowEx( 	NULL, 
								DEF_APPNAME, 
								DEF_APPNAME, 
								windowStyle,
								//CW_USEDEFAULT, 
								//CW_USEDEFAULT, 
								0, 
								0, 
								clientRect.right - clientRect.left, 
								clientRect.bottom - clientRect.top, 
								NULL, 
								NULL, 
								hInst, 
								NULL );
	}
	// ウィンドウモードを変更するとき
	else{	
		// ウィンドウスタイルの変更
		SetWindowLong( hWnd, GWL_STYLE, windowStyle );
		// 作成したウインドウを表示
		ShowWindow( hWnd, CmdShow );
		// ウィンドウの位置とサイズの変更
		SetWindowPos( 	hWnd, 
						HWND_NOTOPMOST, 
						0, 
						0, 
						clientRect.right - clientRect.left, 
						clientRect.bottom - clientRect.top, 
						//SWP_SHOWWINDOW );
						SWP_FRAMECHANGED );
	}
	
	// 作成したウインドウを表示
	ShowWindow( hWnd, CmdShow );

	// ウィンドウを表示更新
	UpdateWindow( hWnd );
}

#if 0
// Windowsのメッセージをフックする
HRESULT CALLBACK HookProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	int a = 0;
	
	switch( nCode ){
	//switch( wParam ){
		case MSGF_NEXTWINDOW:
		//case HCBT_SYSCOMMAND:
			a++;
			break;
		
		case HCBT_ACTIVATE:
			a++;
			break;
	}
	return 0;
}
#endif

//---------------------------------------------------------------------------//
// 概要 ：メッセージループ処理関数                                                     //
// 引数 ：なし   信息循环处理函数/ // /参数：无                                                            //
//---------------------------------------------------------------------------//
BOOL SystemTask( void )
{
	MSG msg;
	
	// ☆★☆★☆ メッセージループ ☆★☆★☆
	// メッセージがある間ループ  信息有间循环
	while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )){
		// メッセージを受け取る、WM_QUIT を受け取るとFALSEが返ってきて終了
		//if( GetMessage( &msg, NULL, 0, 0 ) == FALSE ) return FALSE; 
		// WM_QUIT を受け取ると終了
		if( msg.message == WM_QUIT ) return FALSE;
		
		TranslateMessage(&msg);		// 変換が必要なメッセージは変換する
		
		DispatchMessage(&msg);		// ウィンドウプロシージャへメッセージを送る
			
	}
	return TRUE;
}

//---------------------------------------------------------------------------//
// 概要 ：ウインドウメッセージ処理関数                                       //
// 引数 ：HWND hWnd:     ウィンドウの識別ハンドル                            //
//        UINT Message:  処理する制御メッセージ                              //
//        WPARAM wParam: 処理する制御メッセージ補足情報１                    //
//        LPARAM lParam: 処理する制御メッセージ補足情報２                    //
//---------------------------------------------------------------------------//
LRESULT CALLBACK WindMsgProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	switch( Message ){
		//case MSGF_NEXTWINDOW: // ALT+TAB 押したとき
			//SetActiveWindow( hWnd );
		//	{ 
		//		int a; 
		//		a++;
		//	}
		//	break;
		
/** システム関連メッセージ ***系统相关信息**************************************************/

		//这个应用软件激活或者非激活了的时候  当应用失去焦点或停止活动时
		case WM_ACTIVATE:	// このアプリケーションがアクティブ又は非アクティブになった時
			
			// ウィンドウがアクティブにされるとき 窗口被激活的时候或是被设置为焦点的时间
			if( ( wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE ) && lpDraw != NULL){
			
				// サーフェスが LOST していたら  表面有LOST了
				if( CheckSurfaceLost() == TRUE ){
					// DirectDraw 開放
					ReleaseDirectDraw();
					// 解像度モード設定
					SetResoMode( ResoMode );
					// DirectDraw 初期化
					if( InitDirectDraw() == FALSE ){
						MessageBox( hWnd, "ＤｉｒｅｃｔＤｒａｗの初期化に失敗しました。", "確認", MB_OK | MB_ICONSTOP );
						// ＷＩＮＤＯＷＳに WM_CLOSE メッセージを送らせる
						PostMessage( hWnd, WM_CLOSE, 0, 0L );
					}
					// ウィンドウモード切り替え
					//ChangeWindowMode();
					// オフスクリーンサーフェスの作成   清洁表面的作成  应该是界面
					InitOffScreenSurface();
					// パレット初期化
					if( InitPalette() == FALSE ){
						PostMessage( hWnd, WM_CLOSE, 0, 0L );
					}
					// バトルマップ読み込みとバトルサーフェスの画像作成
					//战斗地图，加载和战斗回程的画像作成
					if( ProcNo == PROC_BATTLE ){ 
						// バッファ初期化
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						ReadBattleMap( BattleMapNo );
						// バックサーフェスを黒でクリアー
						ClearBackSurface();	
						// ＢＭＰをバックサーフェスにセット
						PutBmp();	
						// バックサーフェスからバトルサーフェスへコピー  背部实体表面复制到战斗
						//函数复制一个后台缓存中的矩形区域到主页面上。

						lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
						// バッファ初期化
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						// 現在の時間を記憶
						NowTime = GetTickCount();
					}
				}
				// パレットオブジェクトがない時  托盘对象的时候
				if( lpDraw->lpPALETTE != NULL ){
					// ウィンドウモードの時  窗口方式的时候
					if( WindowMode ){
						// 作成しておいたパレットに変える  制作方面的托盘改变
						//调色板
						lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
					}
				}
				// マウスクリック入力の初期化  鼠标点击输入的初期化
				mouse.state = MOUSE_NO_CRICK;
				mouse.onceState = MOUSE_NO_CRICK;
				// 時間の遅れ忘れさせる  的时间晚让人忘了 获得当前时间系统启动--》
				NowTime = GetTickCount();
			}
			
			//ダイレクトインプットの修正
			//キーボードＯＫなら
			// /直接输入的修正/ /键盘OK的话
			if(keyboad_flg == TRUE)
			{		//キーボードＯＫなら
				// キーボードオブジェクトがあるとき  键盘对象的时候
				if(pDInputDevice != NULL){
					// ウィンドウが非アクティブにされるとき  窗口非被激活的时候
					if(wParam == WA_INACTIVE)
					{
						// DInput 停止
						DInputActiveFlag = FALSE;
						// キーバッファの初期化  键缓冲的初期化  实际创建键盘对象
						for( int i = 0 ; i < 256 ; i++ ) di_key[ i ] = 0;
						pDInputDevice->Unacquire();
					}
					else
					{
						
						// DInput 復活  获得鼠标  找鼠标的
						DInputActiveFlag = TRUE;
						pDInputDevice->Acquire();
					
					}
				}
			}

			//ジョイスティックＯＫなら  控制杆OK的话
			if(joy_flg == TRUE){		
				// ジョイスティックオブジェクトがあるとき  控制杆对象的时候
				if(pDInputDevice2 == NULL)
					break;

				// ウィンドウが非アクティブにされるとき  窗口非被激活的时候
				if(wParam == WA_INACTIVE){
					// DInput 停止
					DInputActiveFlag = FALSE;
					pDInputDevice2->Unacquire();
				}
				else{
					// DInput 復活
					DInputActiveFlag = TRUE;
					pDInputDevice2->Acquire();
				}
			}
			break;

//		case WM_ACTIVATEAPP: // 制御がこのアプリケーションに移った時
		
			// ALT+TAB切り替えにてLOSTした情報を修復
			//if( ( wParam ) && lpDraw != NULL ){
			//break;
			
		case WM_CLOSE:				// ウィンドウ閉じるとき
		
			DestroyWindow( hWnd );	// ウィンドウを破棄する、同時に
									// WM_DESTROY メッセージが送られる
			break;

		case WM_DESTROY:			// ウィンドウが破棄されたとき

			PostQuitMessage( 0 );	// WM_QUIT メッセージを送る ( 処理の終了 )
			break;
			
#if 0			
		case WM_PALETTECHANGED:		// パレットが変更されたとき
			// DirectDrawオブジェクトがない時
			if( lpDraw == NULL ) break;
			// パレットオブジェクトがない時
			if( lpDraw->lpPALETTE == NULL ) break;
			// ウィンドウモードの時
			//我开启
			if( WindowMode ){
				// 作成しておいたパレットに変える
				lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			}  //我开启
			break;
			//if( (HWND)wParam == hWnd )  break;
			
		case WM_PALETTEISCHANGING:		// パレットが変更されたとき
			// DirectDrawオブジェクトがない時
			if( lpDraw == NULL ) break;
			// パレットオブジェクトがない時
			if( lpDraw->lpPALETTE == NULL ) break;
			// ウィンドウモードの時
			//if( WindowMode ){
				// 作成しておいたパレットに変える
				lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			//}
			break;
		
		case WM_QUERYNEWPALETTE:	// パレット変更できるとき（ウィンドウモードの時のみ）
		
			// DirectDrawオブジェクトがない時
			if( lpDraw == NULL ) break;
			// パレットオブジェクトがない時
			if( lpDraw->lpPALETTE == NULL ) break;
			
			// ウィンドウモードの時
			//if( WindowMode ){
				// 作成しておいたパレットに変える
				lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			//}
			
			break;
#endif			
			
/** キー入力メッセージ *****键输入信息************************************************/

      	case WM_KEYDOWN:	// キー入力処理
		
			// サーフェスヴィジーの時
			///键盘输入处理维ー/ /回程的时候
			if( SurfaceBusyFlag == TRUE ){
				SurfaceBusyFlag = FALSE;
				//MessageBox( hWnd, "失去的图像数据修复。。", "確認", MB_OK | MB_ICONSTOP );
			
				// DirectDraw 開放
				ReleaseDirectDraw();
				// 解像度モード設定
				SetResoMode( ResoMode );
				// DirectDraw 初期化
				if( InitDirectDraw() == FALSE ){
					MessageBox( hWnd, "ＤｉｒｅｃｔＤｒａｗの初期化に失敗しました。", "確認", MB_OK | MB_ICONSTOP );
					// ＷＩＮＤＯＷＳに WM_CLOSE メッセージを送らせる
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// ウィンドウモード切り替え
				//ChangeWindowMode();
				// オフスクリーンサーフェスの作成 
				InitOffScreenSurface();
				// パレット初期化
				if( InitPalette() == FALSE ){
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// バトルマップ読み込みとバトルサーフェスの画像作成
				if( ProcNo == PROC_BATTLE ){ 
					// バッファ初期化
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					ReadBattleMap( BattleMapNo );
					// バックサーフェスを黒でクリアー
					ClearBackSurface();	
					// ＢＭＰをバックサーフェスにセット
					PutBmp();	
					// バックサーフェスからバトルサーフェスへコピー
					lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
					// バッファ初期化
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					// 現在の時間を記憶
					NowTime = GetTickCount();
				}
				// パレットオブジェクトがない時
				if( lpDraw->lpPALETTE != NULL ){
					// ウィンドウモードの時
					if( WindowMode ){
						// 作成しておいたパレットに変える
						lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
					}
				}
				// マウスクリック入力の初期化
				mouse.state = MOUSE_NO_CRICK;
				mouse.onceState = MOUSE_NO_CRICK;
				// 時間の遅れ忘れさせる
				NowTime = GetTickCount();
			}
			
            switch( wParam ){
                case VK_ESCAPE: // ＥＳＣキー押されたとき
					// ＷＩＮＤＯＷＳに WM_CLOSE メッセージを送らせる
					//ESC键被推的时候对WM _ CLOSE / / WINDOWS信息延缓
					//PostMessage( hWnd, WM_CLOSE, 0, 0L );
					break;
					
				case VK_RETURN:	// リターンキー押された時   
				
					/* リターンキー処理   按下回车键时回车键 /处理*/
					KeyboardReturn();
				
					break;
					
				case VK_BACK:	// BS で一文字削除  在一个字删除
				
					// ＢＳキー処理   / / BS/ / BS键处理
					KeyboardBackSpace();
				
					break;
					
				case VK_DELETE:	// チャットバッファクリア
				
					/* チャット文字を全てクリアする */
					ClearChatBuffer();
					
					break;
					
				case VK_F5:	// チャットバッファクリア
					
					/* チャット文字を全てクリアする  缓冲清除聊天/聊天文字全部清除 */
					wParam = wParam;
					
					break;
			}
			break;
			
		case WM_SYSKEYDOWN:	// ALTキー押されたとき  按下ALT键的时候
		
			// サーフェスヴィジーの時 实体维信息的时候
			if( SurfaceBusyFlag == TRUE ){
				SurfaceBusyFlag = FALSE;
				//MessageBox( hWnd, "失われた画像データを修復します。", "確認", MB_OK | MB_ICONSTOP );
			
				// DirectDraw 開放
				ReleaseDirectDraw();
				// 解像度モード設定
				SetResoMode( ResoMode );
				// DirectDraw 初期化
				if( InitDirectDraw() == FALSE ){
					MessageBox( hWnd, "ＤｉｒｅｃｔＤｒａｗの初期化に失敗しました。", "確認", MB_OK | MB_ICONSTOP );
					// ＷＩＮＤＯＷＳに WM_CLOSE メッセージを送らせる
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// ウィンドウモード切り替え
				//ChangeWindowMode();
				// オフスクリーンサーフェスの作成 
				InitOffScreenSurface();
				// パレット初期化
				if( InitPalette() == FALSE ){
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// バトルマップ読み込みとバトルサーフェスの画像作成
				if( ProcNo == PROC_BATTLE ){ 
					// バッファ初期化
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					ReadBattleMap( BattleMapNo );
					// バックサーフェスを黒でクリアー
					ClearBackSurface();	
					// ＢＭＰをバックサーフェスにセット
					PutBmp();	
					// バックサーフェスからバトルサーフェスへコピー
					lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
					// バッファ初期化
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					// 現在の時間を記憶
					NowTime = GetTickCount();
				}
				// パレットオブジェクトがない時
				if( lpDraw->lpPALETTE != NULL ){
					// ウィンドウモードの時
					if( WindowMode ){
						// 作成しておいたパレットに変える
						lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
					}
				}
				// マウスクリック入力の初期化
				mouse.state = MOUSE_NO_CRICK;
				mouse.onceState = MOUSE_NO_CRICK;
				// 時間の遅れ忘れさせる
				NowTime = GetTickCount();
			}
			
			switch( wParam )
			{
					//在窗口模式变更  ALT+RETURN
				//按下回车键时回车键 /处理*/
				case VK_RETURN:	// ALT+RETURN でウィンドウモード変更  在窗口模式变更
				
					// 演出中は抜ける  演出中穿过
					if( BackBufferDrawType == DRAW_BACK_PRODUCE ) break; 
					
					// DirectDrawオブジェクトがない時  对象的时候
					if( lpDraw == NULL ) break;
					
					// ウィンドウモードフラグ変更
					if( WindowMode == TRUE ) WindowMode = FALSE;
					else WindowMode = TRUE;
					
					// DirectDraw 開放
					ReleaseDirectDraw();

					// 解像度モード設定
					SetResoMode( ResoMode );

					// DirectDraw 初期化
					InitDirectDraw();
					// オフスクリーンサーフェスの作成 
					InitOffScreenSurface();
					// ウィンドウモード切り替え
					ChangeWindowMode();
					// 全ウィンドウに再描画要求
					//SendMessage( HWND_BROADCAST, WM_PAINT, NULL, NULL );
		
					// パレット初期化
					if( InitPalette() == FALSE ){
						PostMessage( hWnd, WM_CLOSE, 0, 0L );
					}
					// バトルマップ読み込みとバトルサーフェスの画像作成
					if( ProcNo == PROC_BATTLE ){ 
						// バッファ初期化
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						ReadBattleMap( BattleMapNo );
						// バックサーフェスを黒でクリアー
						ClearBackSurface();	
						// ＢＭＰをバックサーフェスにセット
						PutBmp();	
						// バックサーフェスからバトルサーフェスへコピー
						lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
						// バッファ初期化
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						// 現在の時間を記憶
						NowTime = GetTickCount();
					}	
					
					break;
				default:
					// 未処理メッセージをデフォルト処理関数へ渡して処理させる
					//未处理的信息处理函数默认交给处理
					return DefWindowProc( hWnd, Message, wParam, lParam );
			}
			break;
			
      	case WM_CHAR:	// 文字入力処理  文字输入处理
		
				// 文字バッファに溜める  文字缓冲蓄

			// StockChatBufferWord( WORD wParam );

				StockStrBufferChar( ( char )wParam );
			
			break;
			
/** マウス入力メッセージ *********鼠标输入信息********************************************/

		case WM_MOUSEMOVE:		// マウスを動かした時  移动鼠标的时候
		
			// 現在のマウスの位置を記憶する
			MouseNowPoint( LOWORD(lParam), HIWORD(lParam) );
			if( mouse.flag == TRUE ){ 
				ShowCursor( FALSE ); // マウスカーソルを消す
				mouse.flag = FALSE;
			}
			// サーフェスヴィジーの時
			if( SurfaceBusyFlag == TRUE ){
				SurfaceBusyFlag = FALSE;
				//MessageBox( hWnd, "失われた画像データを修復します。", "確認", MB_OK | MB_ICONSTOP );
			
				// DirectDraw 開放
				ReleaseDirectDraw();
				// 解像度モード設定
				SetResoMode( ResoMode );
				// DirectDraw 初期化
				if( InitDirectDraw() == FALSE ){
					MessageBox( hWnd, "ＤｉｒｅｃｔＤｒａｗの初期化に失敗しました。", "確認", MB_OK | MB_ICONSTOP );
					// ＷＩＮＤＯＷＳに WM_CLOSE メッセージを送らせる
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// ウィンドウモード切り替え
				//ChangeWindowMode();
				// オフスクリーンサーフェスの作成 
				InitOffScreenSurface();
				// パレット初期化
				if( InitPalette() == FALSE ){
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// バトルマップ読み込みとバトルサーフェスの画像作成
				if( ProcNo == PROC_BATTLE ){ 
					// バッファ初期化
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					ReadBattleMap( BattleMapNo );
					// バックサーフェスを黒でクリアー
					ClearBackSurface();	
					// ＢＭＰをバックサーフェスにセット
					PutBmp();	
					// バックサーフェスからバトルサーフェスへコピー
					lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
					// バッファ初期化
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					// 現在の時間を記憶
					NowTime = GetTickCount();
				}
				// パレットオブジェクトがない時
				if( lpDraw->lpPALETTE != NULL ){
					// ウィンドウモードの時
					if( WindowMode ){
						// 作成しておいたパレットに変える
						lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
					}
				}
				// マウスクリック入力の初期化
				mouse.state = MOUSE_NO_CRICK;
				mouse.onceState = MOUSE_NO_CRICK;
				// 時間の遅れ忘れさせる
				NowTime = GetTickCount();
			}

			break;
			
		case WM_NCMOUSEMOVE:	// マウスがウィンドウからはみ出た時  鼠标从窗口露出了的时候
		
			// マウスクリック入力の初期化
			mouse.state = MOUSE_NO_CRICK;
			mouse.onceState = MOUSE_NO_CRICK;
			if( mouse.flag == FALSE ){ 
				ShowCursor( TRUE ); // マウスカーソルを表示
				mouse.flag = TRUE;
			}
			
			break;
			
		case WM_LBUTTONDOWN:	// 左クリック(押した時)
		
			// 左クリックされた時のマウスの位置を記憶する
			MouseCrickLeftDownPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;
			
		case WM_LBUTTONUP:		// 左クリック(離した時)
		
			// 左クリックされた時のマウスの位置を記憶する
			MouseCrickLeftUpPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;

		case WM_LBUTTONDBLCLK:		// 左ダブルクリック
		
			// 左ダブルクリックされた時のマウスの位置を記憶する
			MouseDblCrickLeftUpPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;
			
		case WM_RBUTTONDOWN:	// 右クリック(押した時)
		
			// 左クリックされた時のマウスの位置を記憶する
			MouseCrickRightDownPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;
			
		case WM_RBUTTONUP:		// 右クリック(離した時)
		
			// 左クリックされた時のマウスの位置を記憶する
			MouseCrickRightUpPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;

		case WM_RBUTTONDBLCLK:		// 右ダブルクリック
		
			// 右ダブルクリックされた時のマウスの位置を記憶する
			MouseDblCrickRightUpPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;
			
		case WM_MBUTTONDOWN:	// 中クリック(押した時)
			break;

/** ＩＭＥ関連メッセージ処理 *******有关信息处理*******************************************/
			
			//的小窗口变化的时候
		case WM_IME_COMPOSITION: // ＩＭＥのちっちゃいウィンドウに変化がある時
	
			// WM_IME_COMPOSITION メッセージの処理
			ImeComposition( lParam );
			
			break;

			//文字来确定后。变换中的文字都在的时候也会乱码BS。/ / IME缓冲在空中的
		case WM_IME_ENDCOMPOSITION: // 文字を確定した直後に来る。変換中の文字を全部BSでけしたときもくる。
			// ＩＭＥバッファーを空にする
			*ImeBuffer = '\0';
			break;
		
		case WM_IME_NOTIFY:
			/* WM_IME_NOTIFY メッセージの処理   信息处理*/
			//ImeNotify( wParam ,lParam );
			// ここにDEFWINDOWPROCがあると、変換候補ウインドウを出すようだ。
			// ＩＭＥ入力モードの取得
			//这里有DEFWINDOWPROC，变换候补窗口发出。/ / IME输入方式的取得
			ImmGetConversionStatus(  hImc, &conversion, &sentence  );
			break;
			
		case WM_IME_CHAR:
			// ここにDefWindowProcがあると、WM_CHARを生成する
			//这里有DefWindowProc，WM _ CHAR生成
			break;
#if 0
		//case WM_IME_SETCONTEXT:
			//初期化したとき来てるのかな？フォーカスが移ったときも来るようだ。
			//DefWindowProc( hWnd, Message, wParam, lParam );
		//	break;
		case WM_IME_STARTCOMPOSITION:// IMEがONの状態で、文字を入力した直後に来る。
		//	DefWindowProc( hWnd, Message, wParam, lParam );
			break;
		case WM_IME_CONTROL:
			break;
		case WM_IME_SELECT: // IMEをえらんだとき来るらしい。バッファを初期化とかすればいいのかな？
			break;
		case WM_IME_KEYDOWN:   // IMEのキーコード
			// ここにDefWindowProcがあると、WM_KEYDOWNを生成する
			break;
#endif
	
		default:
			// 未処理メッセージをデフォルト処理関数へ渡して処理させる
			return DefWindowProc( hWnd, Message, wParam, lParam );
	}

	return 0;
}

/** 解像度モード設定処理 *****分辨率模式设定处理*********************************************/
void SetResoMode(int Mode){
	ResoMode = Mode;
	// DirectDraw管理構造体実体確保   初始化DIRECT_DRAW返回指针
	lpDraw = ( DIRECT_DRAW *)GlobalAllocPtr( GPTR, ( DWORD )sizeof( DIRECT_DRAW ) );
	if( lpDraw == NULL ){
		MessageBox( hWnd, "GlobalAllocPtr Error ( DIRECT_DRAW )", "確認", MB_OK | MB_ICONSTOP );
		return;
	}

	switch(Mode){
	case 0:
		//６４０＊４８０  ?640 480
		lpDraw->xSize = 640;//windows  窗口的大小 和Direct 没关系
		lpDraw->ySize = 480;
		SurfaceSizeX = 64;
		SurfaceSizeY = 48;
		break;
	case 1:
		//３２０＊２４０
		lpDraw->xSize = 320;
		lpDraw->ySize = 240;
		SurfaceSizeX = 32;
		SurfaceSizeY = 24;
		break;
	case 2:
		//可変
		lpDraw->xSize = 640;
		lpDraw->ySize = 480;
		SurfaceSizeX = 64;
		SurfaceSizeY = 48;
		break;
	}
}

