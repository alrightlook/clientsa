/************************/
/*	process.c	进程重要处理事务	*/
/************************/
#include "system.h"
#include "../ohta/ohta.h"
#include "../dwaf/dwafTestProc.h"
#include "ime_sa.h"
#include "menu.h"
#include "pc.h"
#include "character.h"
#include "login.h"
#include "netproc.h"
#include "savedata.h"
#include "testView.h"
#include "battleProc.h"
#include "produce.h"
#include "lssproto_cli.h"
#include "netmain.h"
#include "battleMenu.h"
#include "t_music.h"
#include "field.h"
#include "handletime.h"

/* 命令处理隐藏*******************************************************************/
extern void kakushi_command(void);
/* 进程号 */
UINT ProcNo;
/* 子进程 */
UINT SubProcNo;

int palNo;//兄弟
int oldPalNo;

/* プロセス処理 ********************************************************************/
void Process( void )
{
	/* プロセス分岐 */
	switch( ProcNo ){
	
		case PROC_OPENNING:
		
			break;
			
		case PROC_INIT: 	/* 初期化プロセス */
		
			InitProc();
			
			break;
			

	// 身份证，密码处理
		case PROC_ID_PASSWORD:
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			idPasswordProc();
			RunAction();			// 执行动作
			StockTaskDispBuffer();	// 显示数据缓冲区的任务
			FlashKeyboardCursor();	// 处理光标闪烁键盘/ /
			ImeProc();				// 输入法有关的处理
			break;

		// 标题和菜单选择
		case PROC_TITLE_MENU:
			// /改变了退缓冲区
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			titleProc();
			TitleProduce();			// 职衔/ 
			RunAction();			// /执行动作
			StockTaskDispBuffer();	// 显示数据缓冲区的任务
			kakushi_command();		//隐藏的命令处理
			break;

		// 人物选择
		case PROC_CHAR_SELECT:		// 人物选择
			// 更改后的渲染处理
			BackBufferDrawType = DRAW_BACK_NORMAL;// 标准
			selectCharacterProc();
			break;



		// 创建人物
		case PROC_CHAR_MAKE:
			// 渲染处理
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			makeCharacterProc();
			break;

			
		// 开始登陆选定的人物
		case PROC_CHAR_LOGIN_START:
			// 转换人物
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			// charLoginStart();因此如何从网络上的数据备份缓冲区
			//初始化以保持其
			initMap();		// 地图
			initPcAll();	// PC的初始化信息（清除
			initCharObj();	// 初始化人物管理表
			initOpenServerWindow();	// 初始化的窗口中显示服务器的线路
			initFieldProc();		// 窗口初始化
			initMapEffect();		// 地图初始化的影响
			EncountFlag = FALSE;
			logOutFlag = FALSE;
			InitOhtaParam();	// 其他参数


			
			ChangeProc( PROC_CHAR_LOGIN );
			// ＢＧＭフェードアウト開始
			fade_out_bgm();//退出GM？
			break;

		case PROC_CHAR_LOGIN:
			// バックバッファー描画方法変更
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			characterLoginProc();
			break;

		// ログアウト
		case PROC_CHAR_LOGOUT:
			// 日志输出
//			BackBufferDrawType = DRAW_BACK_NORMAL; 
			characterLogoutProc();
			break;


		case PROC_GAME:     /*进程的游戏 */
			
			GameProc();
			break;

		case PROC_DISCONNECT_SERVER://断开游戏？
			switch( SubProcNo )
			{
				case 0:
					// 初期化
					// PCリセット  / /初始化/ /电脑复位
					resetPc();
					// 人物管理表
					initCharObj();
					// 取消动作
					DeathAllAction();


					//  /初始化生产
					ProduceInitFlag = TRUE;
					// 复制背景创建图像
					CopyBackBuffer();
					// 渲染方法
					BackBufferDrawType = DRAW_BACK_PRODUCE; 
					// 背景音乐
					fade_out_bgm();
					// 记录当前时间

					NowTime = GetTickCount();
					SubProcNo++;

					break;

				case 1:
					// 生成
					if( DrawProduce( PRODUCE_DOWN_ACCELE ) == TRUE )
					{
						BackBufferDrawType = DRAW_BACK_NORMAL; 
						SubProcNo++;
					}
					break;

				case 2:
					// 结束
					break;
			}
			// 断开警告Windows 窗口
			if( disconnectServer() )
			{
				// 初始化网络
				cleanupNetwork();
				// 恢复默认调色板
				PaletteChange( DEF_PAL, 0 );
				// 到标题画面
				ChangeProc( PROC_TITLE_MENU );
				// /消除行动
				DeathAllAction();
				disconnectServerFlag = FALSE;
				oldDisconnectServerFlag = FALSE;
				break;
			}
			RunAction();			// アクション走らせる  动作运行
			StockTaskDispBuffer();	// タスク表示データをバッファに溜める  显示数据任务缓冲蓄
			break;


		case PROC_BATTLE:     /* バトルプロセス */
		
			BattleProc();
			break;
			
#ifdef _DEBUG		
		//case PROC_OHTA_TEST:     /* 太田テストプロセス */
		
		//	OhtaTestProc();
		//	break;
			
		//case PROC_TAKE_TEST:     /* 竹内さんテストプロセス */
		
		//	TakeTestProc();
		//	break;

		//case PROC_DWAF_TEST:	// DWAFテスト
		//	dwafTestProc();
		//	break;

		case PROC_SPR_VIEW:		// 初始验证
			SprViewProc();
			break;
			
		case PROC_ANIM_VIEW:	// 动画验证
			AnimViewProc();
			break;
			
		case PROC_SE_TEST:	//  SE的审查过程
			SeTestProc();
			break;
#endif
		case PROC_ENDING:
			
			break;
			
	}
}

/* プロセス変更処理 ***********************************************************/
void ChangeProc( int procNo )
{
	// プロセス番号を記憶
	ProcNo = procNo;
	// サブプロセス番号の初期化
	SubProcNo = 0;
}
/* プロセス変更処理 ***********************************************************/
void ChangeProc( int procNo, int subProcNo )
{
	// プロセス番号を記憶
	ProcNo = procNo;
	// サブプロセス番号の初期化
	SubProcNo = subProcNo;
}

/* ゲームプロセス処理 ********************************************************************/
void GameProc( void )
{
	static int now_bgm;
	static BOOL produceFlag;	// 演出フラグ

	/* サブプロセス番号で分岐 */
	switch( SubProcNo ){
	
		case 0:
#ifdef _DEBUG
			// オフラインモードの時のマップ初期化
			if( offlineFlag )
			{
				initMap();		// マップ初期化
			}
#endif

			//InitIme();		// ＩＭＥ関連の初期化
			ClrImeCompositionString(); // ウィンドウズのＩＭＥバッファーをクリアする
			InitChat();		/* チャット関連初期化 */
			// 演出番号ランダム（偶数のみ）
			//ProduceNo = Rnd( 0, PRODUCE_END - 1 ) & 0xfffffffe;
			produceFlag = FALSE;	// 演出フラグＯＦＦ
			EncountFlag = FALSE;	// エンカウントフラグＯＦＦ
			//フィールドＢＧＭ再生開始
//			play_bgm(0);
			// アルバムの読み込み  专辑的读取  相簿
			LoadAlbum();
			
			SubProcNo++;

		// ログイン演出
		case 150:
			SubProcNo = 100;

		case 100:
			initPc();				// プレイヤーキャラ作成
			resetFieldProc();		// フィールド処理のリセット
			restorePtActCharObjAll();	// キャラ管理テーブルのアクション復活
			initOpenServerWindow();	// サーバの指示で表示するウィンドウの初期化
			InitMenu();		// メニュー関連の初期化
//			BattleStatusBak[ 0 ] = NULL;// バトル状態初期化
//			BattleStatusReadPointer = BattleStatusWritePointer =0;
			// 入力フォーカス取得
			GetKeyInputFocus( &MyChatBuffer );
			SubProcNo++;

		case 101:
			if( loginFlag )
			{
				BackBufferDrawType = DRAW_BACK_NORMAL; 
				break;
			}
			SubProcNo++;

		case 102:
			// 演出画像の作成
			// バッファ初期化
			paletteProc();

			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			initCharPartsPrio();	// キャラ?パーツの優先順位決定処理の初期化
			RunAction();			// アクション走らせる
			StockTaskDispBuffer();	// タスク表示データをバッファに溜める
			stop_bgm();				//ＢＧＭ停止
			drawMap();				//	マップ表示
			MenuProc();				// メニュー処理
			// 一行インフォ欄塗りつぶし（黒色）
			//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
			ChatProc();				// チャット処理
			//聊天缓冲字体缓冲蓄
			ChatBufferToFontBuffer(); // チャットバッファーをフォントバッファに溜める
			ImeProc();				// ＩＭＥ関連処理
			SortDispBuffer(); 	// 表示バッファソート
			// バックサーフェスを黒でクリアー
			ClearBackSurface();	
			// ＢＭＰをバックサーフェスにセット
			PutBmp();	
			// バックサーフェスからバトルサーフェスへコピー
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// バッファ初期化
			DispBuffer.DispCnt = 0;
			FontCnt = 0;

		//	_asm{int 3}  这个 开启就出错 我关闭
		//	play_bgm( map_bgm_no );

			// 現在の時間を記憶
			NowTime = GetTickCount();
			// バックバッファー描画方法変更
//			BackBufferDrawType = DRAW_BACK_NORMAL; 
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			// 演出中
			ProduceInitFlag = TRUE;
			DrawProduce( PRODUCE_BRAN_BIG );
			SubProcNo++;
			break;

		case 103:
			// 演出中
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_BRAN_BIG ) == TRUE )
			{
				SubProcNo = 3;
				produceFlag = TRUE;
				fieldInfoTime = GetTickCount();
			}
			break;


		// ワープ演出
		case 200:
			// ＢＧＭフェードアウト開始
//			fade_out_bgm();

			// SプロトコルのCで warpEffectProc();が呼ばれて
			// 画面を作るのでここではしなくて良い
			SubProcNo++;

		case 201:
			// 演出中
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_CENTER_PRESSIN ) == TRUE )
			{
				SubProcNo++;
			}
			drawField();	// フィールド画面にいろいろ表示
			MenuProc();	// メニュー処理
			ImeProc();	// ＩＭＥ関連処理
			break;

		case 202:
			if( !warpEffectStart || !warpEffectOk )
			{
				BackBufferDrawType = DRAW_BACK_NORMAL; 
				drawField();	// フィールド画面にいろいろ表示
				MenuProc();	// メニュー処理
				ImeProc();	// ＩＭＥ関連処理
				break;
			}
			warpEffectStart = FALSE;
			warpEffectOk = FALSE;
			SubProcNo++;

		case 203:
			// 演出画像の作成
			// バッファ初期化  /演出图像的作成/ /缓冲初期化
			paletteProc();

			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			initCharPartsPrio();	// キャラ?パーツの優先順位決定処理の初期化
			RunAction();			// アクション走らせる
			StockTaskDispBuffer();	// タスク表示データをバッファに溜める
			redrawMap();
			drawMap();				//	マップ表示

#if 1
			// エフェクトのクリア
			if( (mapEffectRainLevel == 0 && oldMapEffectRainLevel != 0 )
			 || (mapEffectSnowLevel == 0 && oldMapEffectSnowLevel != 0 ) )
			{
				initMapEffect();
			}
#endif

			// 一行インフォ欄塗りつぶし（黒色）
			//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
			ChatProc();				// チャット処理
			ChatBufferToFontBuffer(); // チャットバッファーをフォントバッファに溜める
			SortDispBuffer(); 	// 表示バッファソート
			// バックサーフェスを黒でクリアー
			ClearBackSurface();	
#if 1
			if( (mapEffectRainLevel != 0 && oldMapEffectRainLevel == 0 )
			 || (mapEffectSnowLevel != 0 && oldMapEffectSnowLevel == 0 ) )
			{
				mapEffectProc2( 80 );		// マップエフェクト（雨?雪等）
			}
#endif
			// ＢＭＰをバックサーフェスにセット
			PutBmp();	
			// バックサーフェスからバトルサーフェスへコピー
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// バッファ初期化
			DispBuffer.DispCnt = 0;
			FontCnt = 0;

//			if( map_bgm_no != now_bgm )
//			{
//				stop_bgm();				//ＢＧＭ停止
//				play_bgm( map_bgm_no );
//			}

			// 現在の時間を記憶
			NowTime = GetTickCount();

			ProduceInitFlag = TRUE;

			SubProcNo++;

		case 204:
			// 演出中
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_CENTER_PRESSOUT ) == TRUE )
			{
				SubProcNo = 3;
				fieldInfoTime = GetTickCount();
			}
			drawField();	// フィールド画面にいろいろ表示
			MenuProc();	// メニュー処理
			ImeProc();	// ＩＭＥ関連処理
			break;


		case 1:
			initPc();				// プレイヤーキャラ作成
			resetFieldProc();		// フィールド処理のリセット
			restorePtActCharObjAll();	// キャラ管理テーブルのアクション復活
			initOpenServerWindow();	// サーバの指示で表示するウィンドウの初期化
			InitMenu();		// メニュー関連の初期化
//			BattleStatusBak[ 0 ] = NULL;// バトル状態初期化
//			BattleStatusReadPointer = BattleStatusWritePointer =0;
			// 入力フォーカス取得
			GetKeyInputFocus( &MyChatBuffer );
			
			// 演出する時
			if( produceFlag == TRUE ){
			// 戦闘結果ウィンドウフラグＯＮ
			//if( BattleResultWndFlag == TRUE ){

				// 演出画像の作成
				// バッファ初期化
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				initCharPartsPrio();	// キャラ?パーツの優先順位決定処理の初期化
				RunAction();			// アクション走らせる
				StockTaskDispBuffer();	// タスク表示データをバッファに溜める
				stop_bgm();				//ＢＧＭ停止
				drawMap();				//	マップ表示
				//ＢＧＭがなければ
				if(!draw_map_bgm_flg){
					//さっきのＢＧＭ再生
					play_bgm(map_bgm_no = now_bgm);
					draw_map_bgm_flg = 1;
				}
				MenuProc();				// メニュー処理
				// 一行インフォ欄塗りつぶし（黒色）
				//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
				ChatProc();				// チャット処理
				ChatBufferToFontBuffer(); // チャットバッファーをフォントバッファに溜める
				ImeProc();				// ＩＭＥ関連処理
				SortDispBuffer(); 	// 表示バッファソート
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
				// バックバッファー描画方法変更
				BackBufferDrawType = DRAW_BACK_PRODUCE; 
				// 演出中
				DrawProduce( PRODUCE_4WAY_IN );
			}else{
				// 演出フラグＯＮ
				produceFlag = TRUE;
				SubProcNo++;
			}
			SubProcNo++;
			
			break;
		
		case 2:	// フィールドイン演出
			
			// 演出中
			if( DrawProduce( PRODUCE_4WAY_IN ) == TRUE ){
				SubProcNo++;
				warpEffectStart = FALSE;
				warpEffectOk = FALSE;
			}
			break;
			
		case 3:
			// バックバッファー描画方法変更
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			
			paletteProc();
			
			initItemOverlapCheck();	// アイテム重なりチェック処理の初期化
			drawGrid();				// グリッドカーソル表示
			fieldProc();			// フィールド関連処理
			moveProc();				// 移動処理
			charObjProc();			// キャラ管理処理
			initCharPartsPrio();	// キャラ?パーツの優先順位決定処理の初期化
			addressBookProc();		// アドレスブック処理
			openServerWindowProc();	// サーバ強制ウィンドウ
			drawFieldInfoWin();		// ワープ後の場所情報
			
			/* アクション走らせる */
			RunAction();
			// タスク表示データをバッファに溜める
			// 指定範囲はストックしない
			StockTaskDispBuffer();

			mapEffectProc();		// マップエフェクト（雨?雪等）

			drawMap();		// マップ表示
			drawField();	// フィールド画面にいろいろ表示

			/* チャット処理 */
			ChatProc();
			// チャットバッファーをフォントバッファに溜める
			ChatBufferToFontBuffer(); 
			// キーボードカーソル点滅処理
			
			FlashKeyboardCursor();
			// メニュー処理
			MenuProc();
			// ＩＭＥ関連処理
			ImeProc();		
			// 時間帯処理とパレットチェンジ処理
			TimeZoneProc();
			
#ifdef _DEBUG
			// エンカウントボタン
			if( joy_trg[ 0 ] & JOY_P_DOWN ){
				if( pc.ptAct != NULL ){
					resetMap();					// マップ処理リセット
					// エンカウント送信
					lssproto_EN_send( sockfd, pc.ptAct->gx, pc.ptAct->gy );
					eventEnemyFlag = 0;
				}
			}
#endif
			// エンカウントした時  遇敌的时候
			if( EncountFlag == TRUE ){
				resetPc();				// PCリセット
				resetCharObj();			// キャラ管理テーブルリセット
				resetMap();				// マップ処理リセット
				clearPtActPartyParam();	// 仲間情報のアクションポインタだけをNULLにする
				fieldInfoTime = 0;		// 場所情報が出てたら消す
				drawFieldInfoWin();		// ワープ後の場所情報
				nowEncountPercentage = minEncountPercentage;// エンカウント率を最小に戻す
				sendEnFlag = 0;
				encountNowFlag = 1;
				eventEnemySendFlag = 0;
				duelSendFlag = 0;
				jbSendFlag = 0;
				// マップウィンドウフラグバックアップ
				if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
				// 消えるまでの時間タイマー
				ResultWndTimer = RESULT_WND_CLOSE_TIME;
				InitMenu();			// メニュー関連の初期化
				BattleCmd[ 0 ] = NULL;		// バトルコマンド初期化
//				BattleCmdBak[ 0 ] = NULL;	// バトルコマンド初期化
//				BattleCmdReadPointer = BattleCmdWritePointer =0;
				BattleStatus[ 0 ] = NULL;	// バトル状態初期化
				//BattleStatusBak[ 0 ] = NULL;// バトル状態初期化
//				BattleStatusReadPointer = BattleStatusWritePointer =0;
				BattleTurnReceiveFlag = TRUE;	// ターン受信フラグＯＮ
				// エンカウント音
				play_se( 215, 320, 240 );
				// 現在のＢＧＭ保存
				now_bgm = t_music_bgm_no;
				// ＢＧＭ停止
				stop_bgm();
				SubProcNo++;
			}
			break;
			
		case 4:	// バックバッファーを演出用サーフェスにコピー
			// バトルサーフェスの画像作成 
			CopyBackBuffer();
			// バックバッファー描画方法変更
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			SubProcNo++;
			
			break;
			
		case 5:	// エンカウント演出
		
			// 演出中
			if( DrawProduce( PRODUCE_HAGARE_OCHI_OUT ) == TRUE ){
			//if( GameState == GAME_ENCOUNT_TO_BATTLE ){ 
				// プロセスチェンジ
				ChangeProc( PROC_BATTLE );
			}
			MenuProc();	// メニュー処理
			ImeProc();	// ＩＭＥ関連処理
			
			break;
	}
}


// ワープ演出用
void warpEffectProc( void )
{
	oldMapEffectRainLevel = mapEffectRainLevel;
	oldMapEffectSnowLevel = mapEffectSnowLevel;

	DispBuffer.DispCnt = 0;
	FontCnt = 0;

	// マップウィンドウフラグバックアップ
	if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
	InitMenu2();

	fieldProc();			// フィールド関連処理
	charObjProc();			// キャラ管理処理
	initCharPartsPrio();	// キャラ?パーツの優先順位決定処理の初期化
	openServerWindowProc();	// サーバ強制ウィンドウ

	fieldInfoTime = 0;		// 場所情報が出てたら消す
	drawFieldInfoWin();		// ワープ後の場所情報


	/* アクション走らせる */
	RunAction();
	// タスク表示データをバッファに溜める
	StockTaskDispBuffer();

	mapEffectProc();		// マップエフェクト（雨?雪等）

	redrawMap();
	drawMap();		// マップ表示

	/* チャット処理 */
	ChatProc();
	// チャットバッファーをフォントバッファに溜める
	ChatBufferToFontBuffer(); 
	// 時間帯処理とパレットチェンジ処理
//	TimeZoneProc();

	SortDispBuffer(); 	// 表示バッファソート

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

	ProduceInitFlag = TRUE;
}


void paletteProc( void )
{
	if( palNo == -1 )
	{
		// パレットチェンジ
		PaletteChange( SaTimeZoneNo, 0 );
		// 時間帯でパレットチェンジする時
		TimeZonePalChangeFlag = TRUE;
		palNo = -2;
	}
	else
	if( palNo >= 0 )
	{
		// 固定パレット設定
		PaletteChange( palNo, 0 );// パレットチェンジ
		// 時間帯でパレットチェンジしない時
		TimeZonePalChangeFlag = FALSE;
		palNo = -2;
	}
}

