// main.cpp ヘッダファイル

#ifndef _MAIN_H_
#define _MAIN_H_

//---------------------------------------------------------------------------//
// 概要 ：マクロ（define)宣言                                                //
//---------------------------------------------------------------------------//
//标题栏的名字
#define DEF_APPNAME		"魔力" // "ＳｔｏｎｅＡｇｅ" //タイトルバーの名前   好像没用窗口模式下 
#define DEF_APPSIZEX	640								//作成するWINDOWSの横幅640	
#define DEF_APPSIZEY	480							//作成するWINDOWSの縦幅480
//#define DEF_APPSIZEX	320								//作成するWINDOWSの横幅
//#define DEF_APPSIZEY	240								//作成するWINDOWSの縦幅

#define SCREEN_WIDTH_CENTER			DEF_APPSIZEX/2
#define SCREEN_HEIGHT_CENTER		DEF_APPSIZEY/2

//---------------------------------------------------------------------------//
// 概要 ：関数プロトタイプ宣言                                               //
//---------------------------------------------------------------------------//

int PASCAL WinMain( HINSTANCE 		hInstance, 	HINSTANCE 		hPrevInstance, 	LPSTR lpCmdLine, 	int nCmdShow );	// 初期化関数

LRESULT CALLBACK PASCAL WindMsgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );				//メインプロシージャー
BOOL SystemTask( void );																		// メッセージループ関数

//#define P( a ) 	printf( #a "\n")
//#define Pd( a ) printf( #a " = %d\n",a )
//#define Pf( a ) printf( #a " = %f\n",a )
//#define Pp( a ) printf( #a " = %p\n",a )

// グローバル変数
extern HWND hWnd;		// ウィンドウハンドル
extern BOOL WindowMode;		// ウィンドウモード
extern int ResoMode;
extern int LowResoCmdFlag;		// 解像度変更コマンドフラグ
extern int	CmdShow;	// WinMain関数の引数をグローバルにする
extern LPSTR CmdLine; 	// WinMain関数のコマンドライン引数をグローバルにする
extern HANDLE hMutex;	// 同時起動チェックオブジェクトのハンドル

extern char realBinName[];
extern char adrnBinName[];
extern char sprBinName[];
extern char sprAdrnBinName[];

#ifdef _DEBUG
extern BOOL offlineFlag;
extern char DebugKey0[ 256 ];		// デバックキー０
extern char DebugKey1[ 256 ];		// デバックキー１
extern char DebugKey2[ 256 ];		// デバックキー２
#endif

#endif

