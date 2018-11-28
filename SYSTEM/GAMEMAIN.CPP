/************************/	
/*	gamemain.cpp 	*/
/************************/
#include "system.h"
#include <winuser.h>
#include <winbase.h>
#include "loadrealbin.h"
#include "loadsprbin.h"
#include "../oft/sys2.h"
#include "ime_sa.h"
#include "anim_tbl.h"
#include "battleMap.h"
#include "netmain.h"
#include "savedata.h"
#include "produce.h"
#include "battleProc.h"
#include "t_music.h"
#include "menu.h"
#include "handletime.h"
#include "battleMenu.h"

#include "pc.h"
#include "map.h"

#define NO_DRAW_MAX_CNT 20	// 描画しない最大フレーム数  绘图不最大框架
//格掉的系统 SKIP指针
#define FRAME_SKIP	0		// コマ落ちシステム ＯＮ：１　ＯＦＦ：０  格掉的系统

// ウィンドウモード切り替え関数宣言 窗口模式转换函数宣言
void ChangeWindowMode( void );
// 解像度モード設定処理
void SetResoMode(int Mode);

//调试模式3行 以前关闭我开启
//#ifndef _DEBUG
//#define _DEBUG	// とりあえずデバッグモード  总之调试方式
//#endif

int testCnt;
static char no_wait_cnt = 0;

//---------------------------------------------------------------------------//
// グローバル変数定義       全局变量定义                                                 //
//---------------------------------------------------------------------------//
int	GameState;					// ゲームの状態 游戏的状态
double ProcTime = 16.666667;	// 処理に使う時間、後は描画時間 
//处理时间，使用后绘制的时间
//double ProcTime = 33.333333;				// 処理に使う時間、後は描画時間
double SystemTime = 16.666667;	// １シンクは 16 msc
//float SystemTime = 33.33333333;	// １シンクは 16 msc
double NowTime; 					// 現在の時間記憶
//绘图次数柜台
int	  NoDrawCnt = 1;			// 描画してない回数カウンター
//背部缓冲绘画方法
int BackBufferDrawType;			// バックバッファー描画方法

// フレームレート計算用  帧速率
int	  FrameRate;				// フレームレート  帧速率
//以每秒几张绘制计数
int	  DrawFrameCnt;				// 一秒間に何枚描画したかを記憶
DWORD DrawFrameTime;			// 一秒間を数えるカウンター 1秒计数
#ifdef _DEBUG
static int palNo = 0;			// パレット番号  字体号码
static int fontNo = 0;			// フォント番号
#endif

//---------------------------------------------------------------------------//
// 概要 ：ウインドウ制御メイン処理関数                                       //
// 引数 ：HWND hWnd : ウィンドウの識別ハンドル                               //
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
// 概要 ：ウインドウ制御メイン処理関数                                       //
// 引数 ：HWND hWnd : ウィンドウの識別ハンドル                               //
//  概要：窗口控制主要处理函数/ // /参数：HWND hWnd:窗口的识别手柄
//---------------------------------------------------------------------------//
BOOL GameMain( void )
{
	// ゲーム開始処理
	if( InitGame() == FALSE ) return FALSE;
	
	// メインループ
	while( 1 )
	{
		// メッセージループコール  信息循环呼叫
		if( SystemTask() == FALSE ){
		
			EndGame(); // ゲーム終了処理
			
			return FALSE;
		}
		
		// ＳＡ時間を取得
		RealTimeToSATime( &SaTime );
		
		// 表示カウンター初期化（コマ落ちの時も初期化するためここでする）
		DispBuffer.DispCnt = 0;
		// フォントカウンター初期化  字体柜台初期化
		FontCnt = 0;
		
		// ネットワーク処理
#ifdef _DEBUG
		if( !offlineFlag )
			networkLoop();
#else
		networkLoop();
#endif
		
		// ＣＤＤＡ再生（ リピート ）重复
		cdda_play( cdda_no );
		
		joy_read(); 	// キーボード入力  键盘输入
#if 1
//		if( joy_trg[ 1 ] & JOY_PRINT_SCREEN )
		if( joy_trg[ 1 ] & JOY_F12 )
		{
			static unsigned int prePushTime = 0;

			// 連射抑制（0.5秒間隔）
			if( prePushTime + 500 <= GetTickCount() )
			{
				// 現在の画面をＢＭＰファイルに保存
				snapShot();
				prePushTime = GetTickCount();
			}
		}
#endif
		Process();		// タスク処理  任务处理
		
		MouseProc();	// マウス処理
		
		SortDispBuffer(); 	// 表示バッファソート 表示排序缓冲区
		
		HitMouseCursor();	// マウスカーソルのあたり判定
		
		PaletteProc();		// パレット処理
		

#ifdef _DEBUG
		// 解像度変更
		if( joy_trg[ 1 ] & JOY_F9  && WindowMode ){
#else			
		// 解像度変更
		if( joy_trg[ 1 ] & JOY_F9  && WindowMode && LowResoCmdFlag ){
#endif
			// 演出中でなければ
			if( BackBufferDrawType != DRAW_BACK_PRODUCE ){
				
				// DirectDrawオブジェクトがある時
				if( lpDraw != NULL ){
					// DirectDraw 開放
					ReleaseDirectDraw();
					// フォントオブジェクトを抹殺
					DeleteObject( hFont );
					// 解像度モード設定
					SetResoMode( (ResoMode + 1 ) & 1 );

					// ウィンドウモードでなければ
//					if( !WindowMode ){
						// ウィンドウモードフラグ変更
//						if( WindowMode == TRUE ) WindowMode = FALSE;
//						else WindowMode = TRUE;
//						WindowMode = TRUE;
//					}
					// DirectDraw 初期化
					InitDirectDraw();
					// オフスクリーンサーフェスの作成 
					InitOffScreenSurface();
					// フォントオブジェクトの作成 */
					InitFont( 0 );
					// ウィンドウモードなら
//					if( WindowMode ){
						// ウィンドウモード切り替え
						ChangeWindowMode();
//					}

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
//					continue;
				}
			}
		}
#ifdef _DEBUG
#if 0
		// パレット変更
		if( joy_trg[ 1 ] & JOY_F8 ){	/* プラス */
			palNo++;
			if( palNo >= MAX_PAL ) palNo = 0;
			// パレットチェンジ
			PaletteChange( palNo, 300 );
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		// フォント変更
		if( joy_trg[ 1 ] & JOY_F6 ){	/* プラス */
		}
		// フォント変更
		if( joy_trg[ 1 ] & JOY_F5 ){	/* プラス */
		// このアプリケーションの権限レベルを設定(排他レベル & フルスクリーン画面）
		//	if( lpDraw->lpDD2->SetCooperativeLevel( hWnd, DDSCL_NORMAL ) != DD_OK )
		//	{
		//		MessageBox( hWnd, "SetCooperativeLevel Error", "確認", MB_OK | MB_ICONSTOP );
			//return FALSE;
		//	}
		}
		
		// フォント変更
		if( joy_trg[ 1 ] & JOY_F7 ){	/* プラス */
			// メールデータの読み込み
			//LoadMailHistory();
			// 全ウィンドウに再描画要求
		//	SendMessage( HWND_BROADCAST, WM_PAINT, NULL, NULL );
		//	SendMessage( HWND_BROADCAST, WM_PALETTECHANGED, NULL, NULL );
		//	SendMessage( HWND_BROADCAST, WM_ERASEBKGND, NULL, NULL );
		//	SendMessage( HWND_BROADCAST, WM_PAINTCLIPBOARD, NULL, NULL );
		//	SendMessage( HWND_BROADCAST, WM_PAINTICON, NULL, NULL );
		
		// このアプリケーションの権限レベルを設定(排他レベル & フルスクリーン画面）
		//if( lpDraw->lpDD2->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ) != DD_OK )
		//{
		//	MessageBox( hWnd, "SetCooperativeLevel Error", "確認", MB_OK | MB_ICONSTOP );
			//return FALSE;
		//}
			//MessageBox( hWnd, "ＤｉｒｅｃｔＤｒａｗの初期化に失敗しました。", "確認", MB_OK | MB_ICONSTOP );
			//BattleResultWndFlag = 2;
			//BattleResultWndFlag = 1;
			fontNo++;
			if( fontNo >= 3 ) fontNo = 0;
			// フォントチェンジ
			InitFont( fontNo );
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		
		extern void lssproto_MSG_recv( int fd,int aindex,char* text ,int color);
		// フォント変更
		if( joy_trg[ 1 ] & JOY_F5 ){	/* プラス */
			char moji[ 256 ];
			static cnt = 0;
			// メールが届いたメッセージ作成
			sprintf( moji," 9/%02d 12:15|１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０", cnt, cnt++ );
			// 仮で届いた
			lssproto_MSG_recv( 0, 1, moji ,0 );
			// アイテム使う音
			//play_se( 212, 320, 240 );
			// メールデータの保存
			//SaveMailHistory();
		}
#endif		
#if 0		

		// Ａパレット変更
		if( joy_auto[ 0 ] & JOY_A ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		// Ｂパレット変更
		if( joy_auto[ 0 ] & JOY_B ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		
		// Ａパレット変更
		//if( joy_trg[ 0 ] & JOY_A ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		// Ｂパレット変更
		//if( joy_trg[ 0 ] & JOY_B ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		
		
		// パレット変更
		//if( joy_trg[ 1 ] & JOY_CTRL_MINUS ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		// Ａパレット変更
		//if( joy_trg[ 1 ] & JOY_CTRL_MINUS ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		// パレット変更
		//if( joy_trg[ 1 ] & JOY_CTRL_CIRCUMFLEX ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		// パレット変更
		if( joy_trg[ 1 ] & JOY_CTRL_YEN ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		// パレット変更
		//if( joy_trg[ 1 ] & JOY_PRINT_SCREEN ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		// パレット変更
		if( joy_trg[ 1 ] & JOY_RSHIFT ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		// パレット変更
		if( joy_trg[ 1 ] & JOY_LSHIFT ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		// パレット変更
		if( joy_trg[ 1 ] & JOY_TAB ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		// パレット変更
		//if( joy_trg[ 1 ] & JOY_RETURN ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		// パレット変更
		if( joy_trg[ 1 ] & JOY_CTRL_W ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
#endif		
		
		// ショートカットキー  快捷键
		if( joy_trg[ 0 ] & JOY_HOME ){
			strcpy( MyChatBuffer.buffer + MyChatBuffer.cnt, DebugKey0 );
			MyChatBuffer.cnt += strlen( DebugKey0 );
		}
		if( joy_trg[ 0 ] & JOY_END ){
			strcpy( MyChatBuffer.buffer + MyChatBuffer.cnt, DebugKey1 );
			MyChatBuffer.cnt += strlen( DebugKey1 );
		}
		if( joy_trg[ 0 ] & JOY_INS ){
			strcpy( MyChatBuffer.buffer + MyChatBuffer.cnt, DebugKey2 );
			MyChatBuffer.cnt += strlen( DebugKey2 );
		}
		
		// フレームレートの計算（外人が作成）  帧速率的计算（外国人作成
		DisplayFrameRate();
		
		// 情報表示
		{
			static int flag = 0;
			// PageUp 押した時情報表示（トグル）
			if( di_key[ DIK_PRIOR ] & 0x80 ){ 
				if( flag == 0 ) flag = 1;
				if( flag == 2 ) flag = 3;
			}else{
				if( flag == 1 ) flag = 2;
				if( flag == 3 ) flag = 0;
			}
			if( flag >= 1 ) InfoDisp();
		}
#endif	
	
#if FRAME_SKIP
		// 描画時間が残っていないときは描画しない ( すぐ処理に戻る )
		if( NowTime + ProcTime < GetTickCount() ){
			// 描画しなくてもいい時なら
			if( NoDrawCnt < NO_DRAW_MAX_CNT ){
				// システムタイムプラス（ 繰越 )
				NowTime += SystemTime;
				// 描画してない回数カウント
				NoDrawCnt++;
				continue;
			}
		}
#endif		
#ifdef _DEBUG		
		// 現在表示しているサーフェス数の初期化
		SurfaceDispCnt = 0;
		
		// 描画時間（緑）（ デバッグ用 ）
		DrawDebugLine( 250 ); 
#endif		
		// バックバッファー描画方法で分岐
		switch( BackBufferDrawType ){
		
			case DRAW_BACK_NON:		// 描画しない
				break;
				
			case DRAW_BACK_NORMAL:	// バックサーフェスを黒でクリアー
			
				ClearBackSurface();	
				break;
				
			case DRAW_BACK_PRODUCE:	// 演出中
				break;
				
			case DRAW_BACK_BATTLE:	// バトルマップ描画
			
				DrawBattleMap();
				break;
				
		}
		

#ifdef _DEBUG
		no_wait_cnt++;
		no_wait_cnt&=7;
		if( GetAsyncKeyState( 0x10 ) & 0x8000 ){		//ＳＨＩＦＴを押してたら
			if(!no_wait_cnt){
				PutBmp();	// ＢＭＰをバックサーフェスにセット
			}
		} else {
			PutBmp();	// ＢＭＰをバックサーフェスにセット
		}
#else
		
		PutBmp();	// ＢＭＰをバックサーフェスにセット
#endif	
		
#ifdef _DEBUG
			// 文字描画時間（青）（ デバッグ用 ）
			//DrawDebugLine( 252 ); 
#endif	
		// フォントをバックサーフェスにセット
		//PutText( FONT_PRIO_FRONT );	
		
		// ウィンドウモードでない時
		//if( !WindowMode ){
			// ＢＭＰ表示バッファの初期化
		//	DispBuffer.DispCnt = 0;
			// フォントカウンター初期化
		//	FontCnt = 0;
			// マウスカーソルの表示（最優先表示）
		//	StockDispBuffer( mouse.nowPoint.x + 8, mouse.nowPoint.y + 8, DISP_PRIO_TOP, 5200, 0 );
		//	PutBmp();	// ＢＭＰをバックサーフェスにセット
		//}
		
#ifdef _DEBUG		
		
		// 余った時間（黒）（ デバッグ用 ）
		DrawDebugLine( 0 );	
#endif		
		
#ifdef _DEBUG		
		if( GetAsyncKeyState( 0x10 ) & 0x8000 ){		//ＳＨＩＦＴを押してたら
			if(!no_wait_cnt){
				Flip();	// 垂直同期線時の切り替え処理
			}
		} else {
			Flip();	// 垂直同期線時の切り替え処理
		}
#else
		
		Flip();	// 垂直同期線時の切り替え処理
#endif		
		
#ifdef _DEBUG		
		// 処理時間（赤）（ デバッグ用 ）
		DrawDebugLine( 249 ); 
#endif
		
#if FRAME_SKIP
#ifdef _DEBUG		
		if( GetAsyncKeyState( 0x10 ) & 0x8000 ){		//ＳＨＩＦＴを押してたら
			// ノーウエイト
			NowTime = GetTickCount();
		} else {
			// 時間のずれを調整
//			while( NowTime >= GetTickCount() );
			while( NowTime >= GetTickCount() ){
				// ウィンドウモードなら
				if( WindowMode ){
					Sleep(1);
				}
			}
		}
#else
		// 時間のずれを調整
//		while( NowTime >= GetTickCount() );
		while( NowTime >= GetTickCount() ){
			// ウィンドウモードなら
			if( WindowMode ){
				Sleep(1);
			}
		}
		
#endif
#endif

		// パレットチェンジフラグＯＮの時
		if( PalChangeFlag == TRUE ){
			// 一度開放
		//    lpDraw->lpPALETTE->Release();
			// 8BitインデックスをPalette変数で初期化し、結果をlpPALETTEへ返す。
		//	lpDraw->lpDD2->CreatePalette( DDPCAPS_8BIT, Palette, &lpDraw->lpPALETTE, NULL );
		
		
			// パレットの中身を設定
			lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			PalChangeFlag = FALSE; // フラグ初期化
		}
		
		// システムタイムプラス
		NowTime += SystemTime;
		// 描画してない回数初期化
		NoDrawCnt = 1;
		
#ifdef _DEBUG		
		// 一秒間に描画した枚数をカウント
    	DrawFrameCnt++;
#endif
		// サーフェスを使用した日付を更新
		SurfaceDate++;
	}

	return FALSE;
}

#ifdef _DEBUG		
// 情報表示 *******************************************************************/
void InfoDisp( void )
{
	char szMoji[ 256 ];
	int x = 16;
	int y = 16;
	char c = 0;
	
	sprintf( szMoji,"ActionCnt        = %d", ActCnt );
	// フォント情報をバッファに溜める
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"DispCnt          = %d", DispBuffer.DispCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SurfaceCnt       = %d",SurfaceCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"VramSurfaceCnt   = %d",VramSurfaceCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SysramSurfaceCnt = %d",SysramSurfaceCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SurfaceUseCnt    = %d",SurfaceUseCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"SearchPoint      = %d",SurfaceSearchPoint );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SurfaceDispCnt   = %d",SurfaceDispCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SurfaceDate      = %d",SurfaceDate );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;c = 0;
	
	sprintf( szMoji,"FrameRate        = %d",FrameRate );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"HitDispNo        = %d",HitDispNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"HitFontNo        = %d",HitFontNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"MouseLevel       = %d",mouse.level );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"ProcNo           = %d",ProcNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"SubProcNo        = %d",SubProcNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"ActionSize       = %d",sizeof( ACTION ) );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"MouseX           = %d",mouse.nowPoint.x );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"MouseY           = %d",mouse.nowPoint.y );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"TimeZone         = %d",SaTime.hour );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16; c = 0;
	
	sprintf( szMoji,"PalNo            = %d",PalState.palNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"BattleMapNo      = %d",BattleMapNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"HogeCnt          = %d",testCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"EventEnemyFlag   = %d",eventEnemyFlag );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 0 ] = %d",pc.selectPetNo[ 0 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 1 ] = %d",pc.selectPetNo[ 1 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 2 ] = %d",pc.selectPetNo[ 2 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 3 ] = %d",pc.selectPetNo[ 3 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 4 ] = %d",pc.selectPetNo[ 4 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"BattlePetStMenCnt= %d",BattlePetStMenCnt );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"BattlePetReceiveFlag= %d",BattlePetReceiveFlag );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

}
#endif		

// フレームレートの計算（外人が作成）
void DisplayFrameRate( void )
{
	// 一秒たったら更新
    if( GetTickCount() - DrawFrameTime >= 1000 ){
	
		// フレームレイトを記憶
        FrameRate = DrawFrameCnt;
		// 今の時間を記憶
        DrawFrameTime = GetTickCount();
		// 一秒間に描画した枚数を初期化
        DrawFrameCnt = 0;
    }
}

// ロゴの表示 ***标志的表示**************************************************************/
void PutLogo( void )
{
	
	// ＢＭＰ表示バッファの初期化
	DispBuffer.DispCnt = 0;
	StockDispBuffer( 320, 240, DISP_PRIO_TOP, CG_TITLE_NOW_LOADING, 0 );
	ClearBackSurface();	// バックサーフェスを黒でクリアー  清理接口开始下次位图
	PutBmp();			// ＢＭＰをバックサーフェスにセット 设置位图
	Flip();				// 垂直同期線時の切り替え処理  垂直线时的转换处理
}	

// ゲーム開始処理 ////游戏开始处理//////////////////////////////////////////////////////////
BOOL InitGame( void )
{
	
	// DirectDraw 初期化  这里不熟悉以后学习里面有个可以不用16位色调换系统不用
	if( InitDirectDraw() == FALSE ){
		MessageBox( hWnd, "ＤｉｒｅｃｔＤｒａｗの初期化に失敗しました。", "確認", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	
	/* オフスクリーンサーフェスの作成  初始化 表面接口为显示做准备*/
	if( InitOffScreenSurface() == FALSE ){
		MessageBox( hWnd, "オフスクリーンサーフェイスの作成に失敗しました。", "確認", MB_OK | MB_ICONSTOP );
		return FALSE;
	}

	// パレット初期化 调色板重点要学习完 拿来用
	if( InitPalette() == FALSE ) return FALSE;
	
	//_asm{int 3}
	// Realbin 関連の初期化  重点了 这里到了地图图片数据文件
	if( initRealbinFileOpen( realBinName, adrnBinName ) == FALSE ) {
	//if( initRealbinFileOpen( "..\\real.bin", "..\\adrn.bin" ) == FALSE ) {
		MessageBox( hWnd, "Real.bin のオープンに失敗しました。", "確認", MB_OK | MB_ICONSTOP );
		//MessageBox( hWnd, "ＳａＵｐｄａｔｅ.ｅｘｅから起動してください。", "確認", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	
	// ロゴの表示
	PutLogo();
	
	// オートマップ初期化処理
	initAutoMapColor( adrnBinName );

	// SprBin 関連の初期化
	if( InitSprBinFileOpen( sprBinName, sprAdrnBinName ) == FALSE ){
		MessageBox( hWnd, "Spr.bin のオープンに失敗しました。", "確認", MB_OK | MB_ICONSTOP );
		return FALSE;
	}

	/* アクション初期化 */
	InitAction();
		
	// フォントオブジェクトの作成 */
	InitFont( 0 );
	//InitFont( 2 );
	
	// 乱数系列初期化
	srand( ( unsigned int )GetTickCount() );
	initRand2();
	
	// DirectSound 初期化
	t_music_init();

	// ダイレクトインプット初期化
	InitDInput();
	
	// ＣＤＤＡ再生
	//cdda_play( Rnd( 2, 34 ) );
	
	// ＩＭＥ関連の初期化
	InitIme();
	
	// マウス処理の初期化
	MouseInit();

	// CDキー、パスワード読み込み
	if( loadUserSetting() == FALSE )
	{
		char msg[1024];
		sprintf( msg, SAVE_ERRMSG_loadNowState, savedataErrorCode );
		MessageBox( hWnd, msg, "確認", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	
	// チャットの登録文字の読み込み  聊天的注册文字的读取
	LoadChatRegistyStr();				
	
	// 現在の時間を記憶
	NowTime = GetTickCount();
	DrawFrameTime = GetTickCount();

// デバッグ版でないとき	
#ifndef _DEBUG
	// マウスカーソルを隠す
	ShowCursor( FALSE );
#endif	
	return TRUE;
}	

// ゲーム終了処理 //////////////////////////////////////////////////////////////
void EndGame( void )
{
	//DirectDraw 開放
	ReleaseDirectDraw();
	// ダイレクトインプット開放
	EndDInput();
	// DirectSound 開放
	t_music_end();
	// ＣＤＤＡ停止
	cdda_stop();
	// Rialbin 閉じる
	cleanupRealbin();
	// フォントオブジェクトを抹殺
	DeleteObject( hFont );
	// ＩＭＥの終了
	EndIme();
	// アクション終了処理
	EndAction();
	// ネットワーク終了処理
	cleanupNetwork();
	// 同時起動チェックオブジェクト開放
	ReleaseMutex( hMutex );
	// パラメータセーブ
	saveUserSetting();
	// チャットの登録文字の保存
	SaveChatRegistyStr();
	
	// タイマー終了
//	timeKillEvent(timerId);
	
	//{
		// ALT+TAB 有効にする
	//	int nOldVal;
	//	SystemParametersInfo (SPI_SCREENSAVERRUNNING, FALSE, &nOldVal, 0);
	//}
}

