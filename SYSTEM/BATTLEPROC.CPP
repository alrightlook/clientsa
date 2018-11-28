/************************/
/*	battleProc.cpp		*/
/************************/
#include "system.h"
#include "ime_sa.h"
#include "map.h"
#include "menu.h"
#include "battlemap.h"
#include "battleProc.h"
#include "battleMenu.h"
#include "battleMap.h"
#include "produce.h"
#include "lssproto_cli.h"
#include "netmain.h"
#include "t_music.h"
#include "pc.h"
#include "character.h"
#include "handletime.h"
#include "savedata.h"

extern	ACTION* oft_test();
extern void damage_num( ACTION *a0 );
extern void WindowDisp( ACTION *a0 );
extern int piyo_point;
extern void set_bc( void );
extern void disp_kanji(ACTION *a0);

int flash_pal_cnt;
PALETTEENTRY	Palette2[256];	// パレット構造体

// エンカウントフラグ
BOOL EncountFlag = FALSE;
// デュエルフラグ
BOOL DuelFlag = FALSE;
// ヘルプ無しの時
BOOL NoHelpFlag = FALSE;
// エンカウントオフフラグ
BOOL EncountOffFlag = FALSE;

// 一人用か二人用かフラグ
BOOL Battle1P2PFlag;

// 不意打ち表示ウィンドウ
ACTION* pActSurprisalWnd;
ACTION* pActAudienceExitWnd;

// 全滅チェック処理
extern int check_all_dead( void );

/* 戦闘地震 *********************************************************/
void battle_quake( void )
{
	switch(quake_vct_no){
	case 0:		//処理無し
		break;
	case 1:		//地震開始
		quake_flg = 60;
		quake_vct_no = 2;
		break;
	case 2:		//地震中
		if(quake_flg >= 16){
			piyo_point += 16;
		} else {
			piyo_point += quake_flg;
		}
		piyo_point &= 63;
		if(!--quake_flg){		//地震終了なら
			quake_vct_no = 0;
		}
		break;
	}
}


// 不意打ち表示処理 **************************************************************/
void SurprisalDisp( void ){
	
	// 不意ついた時、つかれた時
	if( BattleBpFlag & BATTLE_BP_ENEMY_SURPRISAL || BattleBpFlag & BATTLE_BP_PLAYER_SURPRISAL ){
		// ウィンドウ無かったら
		if( pActSurprisalWnd == NULL ){
			// ウィンドウ表示タスク作成
			pActSurprisalWnd = MakeWindowDisp( 320 - 96, 240 - 48, 3, 2, NULL, 1 );
			// ウィンドウ開く音
			play_se( 202, 320, 240 );
		}
		// ウィンドウ出来あがっていたら
		if( pActSurprisalWnd->hp > 0 ){
			// 敵が不意をついた時（不意つかれた時）
			if( BattleBpFlag & BATTLE_BP_ENEMY_SURPRISAL ){
				StockFontBuffer( pActSurprisalWnd->x + 38, pActSurprisalWnd->y + 40, FONT_PRIO_FRONT, 0, 	"不意をつかれた", 0 );
			}
			// プレイヤーが不意をついた時
			if( BattleBpFlag & BATTLE_BP_PLAYER_SURPRISAL ){
				StockFontBuffer( pActSurprisalWnd->x + 38, pActSurprisalWnd->y + 40, FONT_PRIO_FRONT, 0, 	" 不意をついた ", 0 );
			}
		}
	}
}

// バトルプロセス サブプロセス番号
enum{
	BATTLE_SUBPROC_INIT,				// ０：初期化
	BATTLE_SUBPROC_IN_PRODUCE,			// １：バトルイン演出
	BATTLE_SUBPROC_RECEIVE_BC,			// ２：ＢＣ受信待ち
	BATTLE_SUBPROC_CHAR_IN,				// ３：登場演出
	BATTLE_SUBPROC_CMD_INPUT,			// ４：コマンド入力
	BATTLE_SUBPROC_RECEIVE_MOVIE,		// ５：ムービー受信待ち
	BATTLE_SUBPROC_MOVIE,				// ６：ムービー処理
	BATTLE_SUBPROC_OUT_PRODUCE_INIT,	// ７：終了演出初期化
	BATTLE_SUBPROC_OUT_PRODUCE,			// ８：終了演出
};
	

int xxx;
/* バトルプロセス処理 ********************************************************************/
void BattleProc( void )
{
	int i;
	//static no = Rnd( 0, BATTLE_MAP_FILES - 1 );	// 今だけランダムバトルマップ
	s_timer++;	// フレームタイマー


	xxx = 0;
	
	/* サブプロセス番号で分岐 */
	switch( SubProcNo ){
	
		case BATTLE_SUBPROC_INIT:				// 初期化
		
			// アクション抹殺
			DeathAllAction();
			
			slow_flg = 0;
			action_inf = 0;
			/* 自機 タスク作成 */
			oft_test();
			
			// メニュー関連の初期化
			InitMenu();
			// バトルメニュー初期化
			InitBattleMenu();
			// 不意打ちウィンド初期化
			//DeathAction( pActSurprisalWnd );
			pActSurprisalWnd = NULL;
			pActAudienceExitWnd = NULL;
			// プロデュース初期化
			ProduceInitFlag = TRUE;
			/* チャット関連初期化 */
			//InitChat();
			// 入力フォーカス取得
			GetKeyInputFocus( &MyChatBuffer );
			
			// 演出画像の作成
			// バッファ初期化
			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			// バトルマップ読み込みとバトルサーフェスの画像作成 */
			ReadBattleMap( BattleMapNo );
			//MenuProc();				// メニュー処理
			//PaletteProc();			// パレット処理
			ChatProc();				// チャット処理
			ChatBufferToFontBuffer(); // チャットバッファーをフォントバッファに溜める
			//ImeProc();				// ＩＭＥ関連処理
			//SortDispBuffer(); 	// 表示バッファソート
			// バックサーフェスを黒でクリアー
			ClearBackSurface();	
			// ＢＭＰをバックサーフェスにセット
			PutBmp();	
			// バックサーフェスからバトルサーフェスへコピー
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// バッファ初期化
			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			//固定敵かデュエルなら
			if(DuelFlag == TRUE || eventEnemyFlag == 1){
				//ボス戦闘ＢＧＭ再生
				play_bgm( 6 );
			} else {
				//通常戦闘ＢＧＭ再生
				play_bgm( 5 );
			}
			// 現在の時間を記憶
			NowTime = GetTickCount();
			// バックバッファー描画方法変更
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			// 演出中
			DrawProduce( PRODUCE_HAGARE_OCHI_IN );
			
			// 点滅を無くす為
			MenuProc();				// メニュー処理
			ImeProc();				// ＩＭＥ関連処理
			
			// メニューフラグＯＮ 
			battleMenuFlag2 = TRUE;
			
			// いないペットチェック
			for( i = 0 ; i < 5 ; i++ ){
				// この欄にペットがいない時
				if( pet[ i ].useFlag == FALSE ){
					// バトルを解除
					if( i == pc.battlePetNo ) pc.battlePetNo = -1;
					// メールの時
					if( i == pc.mailPetNo ) pc.mailPetNo = -1;
					// とりあえず２軍にする
					pc.selectPetNo[ i ] = FALSE;
					
				}
#if 0
				// バトルのときかつメールの時（ラグでなるときがある）
				if( pc.battlePetNo == i && pc.mailPetNo == i ){
					// メールなしにする
					pc.mailPetNo = -1;
					// とりあえず待機にする
					pc.selectPetNo[ i ] = TRUE;
				}
#endif
			}
			// パラメータセーブ
			saveUserSetting();
			
			// プロセス番号プラス
			SubProcNo++;
			break;
			
		case BATTLE_SUBPROC_IN_PRODUCE:			// バトルイン演出
		
			// 演出中
			if( DrawProduce( PRODUCE_HAGARE_OCHI_IN ) == TRUE ){
				// バッファ初期化
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				// バトルマップ読み込みとバトルサーフェスの画像作成 */
				ReadBattleMap( BattleMapNo );
				//no++;
				//if( no >= BATTLE_MAP_FILES ) no = 0;
				//no = Rnd( 0, BATTLE_MAP_FILES - 1 );
				// バックサーフェスを黒でクリアー
				ClearBackSurface();	
				//SortDispBuffer(); 	// 表示バッファソート
				// ＢＭＰをバックサーフェスにセット
				PutBmp();
				// バトルサーフェスを初期化
				//ClearSurface( lpBattleSurface );
				// バックサーフェスからバトルサーフェスへコピー
				lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
				// バッファ初期化
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				// 現在の時間を記憶
				NowTime = GetTickCount();
				
				// バックバッファー描画方法変更
				BackBufferDrawType = DRAW_BACK_BATTLE; 
				
				// バックバッファーに描く
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
				// パレット処理
				//PaletteProc();
				
				SubProcNo++;
			}else{
				MenuProc();	// メニュー処理
				ImeProc();	// ＩＭＥ関連処理
			}
		
			break;
			
		case BATTLE_SUBPROC_RECEIVE_BC:			// ＢＣ受信待ち
		
#ifdef _DEBUG
			if(offlineFlag == TRUE){
				strcpy(BattleStatus,"BC|3|"
				"0|ishikawa|title|18768|1|20|20|0|"
				"1|ishikawa|title|18768|1|20|20|0|"
				"2|ishikawa|title|18768|1|20|20|0|"
				"3|ishikawa|title|18768|1|20|20|0|"
				"4|ishikawa|title|18768|1|20|20|0|"
				"5|beron|title|18813|1|20|20|0|"
				"6|beron|title|18813|1|20|20|0|"
				"7|beron|title|18813|1|20|20|0|"
				"8|beron|title|18813|1|20|20|0|"
				"9|beron|title|18813|1|20|20|0|"
				"A|ishikawa|title|18813|1|20|20|0|"
				"B|ishikawa|title|18813|1|20|20|0|"
				"C|ishikawa|title|18813|1|20|20|0|"
				"D|ishikawa|title|18813|1|20|20|0|"
				"E|ishikawa|title|18813|1|20|20|0|"
				"F|beron|title|187BA|1|20|20|0|"
				"10|beron|title|187BA|1|20|20|0|"
				"11|beron|title|187BA|1|20|20|0|"
				"12|beron|title|187BA|1|20|20|0|"
				"13|beron|title|187BA|1|20|20|0|"
				);

	strcpy(BattleStatus,"BC|0|0|命の鐘の十常寺||186F7|1F|B6|B6|5|F|疾風のコズぴよ||18786|20|11D|11D|1|10|リンダ||187B2|1D|F8|F8|1|");
//	strcpy(BattleStatus,"BC|0|0|サガット|目標ガンゾ倒|186DF|E|2A8|2BC|4|F|ダックル||187B6|F|7D|7D|0|10|ダックル||187B6|10|1|91|0|");

				// ＢＣ設定
				set_bc();
				SubProcNo++;
				break;
			}
#endif
			
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
			
			// 参戦の時
			if( BattleBpFlag & BATTLE_BP_JOIN ){
				// 一行インフォ
				strcpy( OneLineInfoStr,"ターン終了待ち。");
			}
				
			// ２ターンめ以降はアクション走らせる
			if( EncountFlag == FALSE ){
				// 偽ラスタースクロール
				if( !( s_timer & 7 ) ){
					piyo_point++;
					piyo_point &= 63;
				}
				/* アクション走らせる */
				RunAction();
				// タスク表示データをバッファに溜める
				StockTaskDispBuffer();
			}
			
			// サーバーの応答待ち
#if 0
			if( BattleStatusBak[ 0 ] != NULL ){
				strcpy( BattleStatus, BattleStatusBak );
				BattleStatusBak[ 0 ] = NULL;
				// エンカウントフラグ初期化
				EncountFlag = FALSE;
				// ＢＣ設定
				set_bc();
				// 一人用か二人用かチェック
				Battle1P2PFlag = CheckBattle1P2P();
				SubProcNo++;

				//観戦モードなら
//				if(xxx == 0 && BattleMyNo >= 20){
//					BattleCmdBak[ 0 ] = NULL;
//					xxx = 1;
//				}
#else
			if( BattleStatusReadPointer != BattleStatusWritePointer ){
				strcpy( BattleStatus, BattleStatusBak[ BattleStatusReadPointer ] );
				BattleStatusReadPointer = ( BattleStatusReadPointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
				// エンカウントフラグ初期化
				EncountFlag = FALSE;
				// ＢＣ設定
				set_bc();
				// 戦闘中の新規ペットチェック
				CheckBattleNewPet();
				// 観戦モードでない時
				if( BattleMyNo < 20 ){
					// ＭＰを設定
					p_party[ BattleMyNo ]->mp = BattleMyMp;
				}
				
				// 一人用か二人用かチェック
				Battle1P2PFlag = CheckBattle1P2P();
				SubProcNo++;

				//観戦モードなら
//				if(xxx == 0 && BattleMyNo >= 20){
//					BattleCmdBak[ 0 ] = NULL;
//					xxx = 1;
//				}
#endif

			}
			// 観戦時に右クリックされた時
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// 観戦終了プロトコル送る
			//	lssproto_B_send( sockfd, "U" );
			//}
			// 観戦の時、ウィンドウの表示
			if( BattleMyNo >= 20 ){
				// ウィンドウ無かったら
				if( pActAudienceExitWnd == NULL ){
					// ウィンドウ表示タスク作成
					pActAudienceExitWnd = MakeWindowDisp( 444, 4, 3, 2, NULL, 1 );
					// ウィンドウ開く音
					play_se( 202, 320, 240 );
				}
			}
			
			break;
			
		case BATTLE_SUBPROC_CHAR_IN:			// 登場演出
			
			// 偽ラスタースクロール
			if( !( s_timer & 7 ) ){
				piyo_point++;
				piyo_point &= 63;
			}
			// 不意打ち表示処理
			SurprisalDisp();
			
			// コマンド入力済みフラグのチェック
			CheckBattleAnimFlag();
			/* アクション走らせる */
			RunAction();
			// タスク表示データをバッファに溜める
			StockTaskDispBuffer();
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
			
			// 登場終ったら
			if( action_inf == 3 ){
				action_inf = 0;
				// 戦闘参戦フラグＯＦＦ
				BattleBpFlag &= ~BATTLE_BP_JOIN; 
				// 全滅チェック処理
				if( check_all_dead() == 1 ){
					SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;
					break;
				}
				// 不意打ちウィンドウ出ていたら
				if( pActSurprisalWnd != NULL ){
					// ウィンドウ抹殺
					DeathAction( pActSurprisalWnd );
					pActSurprisalWnd = NULL;
					// 不意打ちフラグ初期化
					BattleBpFlag &= ~BATTLE_BP_ENEMY_SURPRISAL; 
					BattleBpFlag &= ~BATTLE_BP_PLAYER_SURPRISAL; 
					
				}
				// 次のプロセスへ
				SubProcNo++;
			}
			//戦闘脱出なら  战斗逃脱的话
			if( action_inf == 2 ){		
				SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;		//戦闘終了演出へ
				action_inf = 0;
				// バトルコマンド初期化
				BattleCmd[ 0 ] = NULL;
			}
			// 観戦時に右クリックされた時
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// 観戦終了プロトコル送る
			//	lssproto_B_send( sockfd, "U" );
			//}
			
			break;
			
		case BATTLE_SUBPROC_CMD_INPUT:			// コマンド入力
		
#ifdef _DEBUG
			if(offlineFlag == TRUE){
				SubProcNo++;
				break;
			}
#endif
			// 偽ラスタースクロール
			if( !( s_timer & 7 ) ){
				piyo_point++;
				piyo_point &= 63;
			}
			
			// バトルメニュー処理  
			BattleMenuProc();
			// コマンド入力済みフラグのチェック
			CheckBattleAnimFlag();
			
			/* アクション走らせる */
			RunAction();
			// タスク表示データをバッファに溜める
			StockTaskDispBuffer();
			
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
			
			// 観戦時に右クリックされた時
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// 観戦終了プロトコル送る
			//	lssproto_B_send( sockfd, "U" );
			//}
			
			break;
			
		case BATTLE_SUBPROC_RECEIVE_MOVIE:		// ムービー受信待ち
		
#ifdef _DEBUG
			if(offlineFlag == TRUE){
//				strcpy( BattleCmd,"BH|aCr0|f2|d10E|rC|f11|counter156|FF|bg|b|BH|aEr0|f2|dDE|rE|f30|counter0|FF|BH|a"
//				"Dr0|f2|d17E|FF|BE|e0|f1|");

//				strcpy( BattleCmd,"BS jibun_2 flg_0 BS jibun_2 flg_1 pet_186A2");
#if 0
				strcpy( BattleCmd,"BO jibun_0 teki_13 flg_0 damage_4 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				" BO jibun_1 teki_13 flg_8 damage_4 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				" BO jibun_2 teki_13 flg_20 damage_4 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				" BO jibun_3 teki_13 flg_4 damage_4 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				" BO jibun_4 teki_13 flg_0 damage_0 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				                 " BO jibun_0 teki_E flg_0 damage_4 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				" BO jibun_1 teki_E flg_8 damage_4 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				" BO jibun_2 teki_E flg_20 damage_4 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				" BO jibun_3 teki_E flg_4 damage_4 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				" BO jibun_4 teki_E flg_0 damage_0 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				                 " BO jibun_0 teki_9 flg_0 damage_4 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
				" BO jibun_1 teki_9 flg_8 damage_4 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
				" BO jibun_2 teki_9 flg_20 damage_4 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
				" BO jibun_3 teki_9 flg_4 damage_4 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
				" BO jibun_4 teki_9 flg_0 damage_0 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
//				                 " BO jibun_0 teki_4 flg_0 damage_4 teki_2 flg_0 damage_4                       teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
//				" BO jibun_1 teki_4 flg_8 damage_4 teki_2 flg_0 damage_4 teki_0 flg_0 damage_4 teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
//				" BO jibun_2 teki_4 flg_20 damage_4 teki_2 flg_0 damage_4 teki_0 flg_0 damage_4 teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
//				" BO jibun_3 teki_4 flg_4 damage_4 teki_2 flg_0 damage_4 teki_0 flg_0 damage_4 teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
//				" BO jibun_4 teki_4 flg_0 damage_0 teki_2 flg_0 damage_4 teki_0 flg_0 damage_4 teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
				                 " BO jibun_A teki_8 flg_0 damage_4 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				" BO jibun_B teki_8 flg_8 damage_4 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				" BO jibun_C teki_8 flg_20 damage_4 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				" BO jibun_D teki_8 flg_4 damage_4 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				" BO jibun_E teki_8 flg_0 damage_0 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				                 " BO jibun_A teki_3 flg_0 damage_4 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				" BO jibun_B teki_3 flg_8 damage_4 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				" BO jibun_C teki_3 flg_20 damage_4 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				" BO jibun_D teki_3 flg_4 damage_4 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				" BO jibun_E teki_3 flg_0 damage_0 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				                 " BO jibun_A teki_12 flg_0 damage_4 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF"
				" BO jibun_B teki_12 flg_8 damage_4 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF"
				" BO jibun_C teki_12 flg_20 damage_4 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF"
				" BO jibun_D teki_12 flg_4 damage_4 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF"
				" BO jibun_E teki_12 flg_0 damage_0 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF");
#endif
#if 1
				strcpy( BattleCmd,"BY teki_A jibun_0 flg_0 damage_0 jibun_1 flg_0 damage_0 jibun_2 flg_0 damage_0"
				" jibun_3 flg_0 damage_0 jibun_4 flg_0 damage_0 jibun_5 flg_0 damage_0"
				" jibun_6 flg_0 damage_0 jibun_7 flg_0 damage_0 jibun_8 flg_0 damage_0"
				" jibun_9 flg_0 damage_1 FF");

		strcpy( BattleCmd,"bn|f|BH|a0|rF|f20|d0|r0|f10|counter33|FF|BH|a10|r0|f2|d1|FF|");
//		strcpy( BattleCmd,"BH|a0|rF|fC|d18|FF|BH|a10|r0|f20|d0|r10|f31|counter1|FF|bg|f|");
#endif
				BattleMyNo = 1;
				SubProcNo++;
				break;
			}
#endif

			// 偽ラスタースクロール
			if( !( s_timer & 7 ) ){
				piyo_point++;
				piyo_point &= 63;
			}
			// コマンド入力済みフラグのチェック
			CheckBattleAnimFlag();
			/* アクション走らせる */
			RunAction();
			// タスク表示データをバッファに溜める
			StockTaskDispBuffer();

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
			
			// サーバーの応答待ち
#if 0
			if( BattleCmdBak[ 0 ] != NULL ){
				strcpy( BattleCmd, BattleCmdBak );
				BattleCmdBak[ 0 ] = NULL;
				action_inf = 0;
				SubProcNo++;
			}
#else
			if( BattleCmdReadPointer != BattleCmdWritePointer ){
				strcpy( BattleCmd, BattleCmdBak[ BattleCmdReadPointer ] );
				BattleCmdReadPointer = ( BattleCmdReadPointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
				action_inf = 0;
				SubProcNo++;
			}
#endif
			// 二人用の時
			if( Battle1P2PFlag == 2 ){
				// 一行インフォ
				strcpy( OneLineInfoStr,"他のプレイヤーの入力待ち。");
			}
			// 観戦の時
			if( BattleMyNo >= 20 ){
				// 人数分ループ
				for( i = 0 ; i < 20 ; i++ ){
					// 耐久力メーター表示
					HpMeterDisp( i );
				}
			}else{
				// 右下グループの時
				if( BattleMyNo < 10 ){
					for( i  = 0 ; i < 10 ; i++ ) HpMeterDisp( i ); // 耐久力メーター表示
				}else 
				// 右下グループの時
				if( BattleMyNo >= 10 ){ 
					for( i = 10 ; i < 20 ; i++ ) HpMeterDisp( i ); // 耐久力メーター表示
				}
			}
			
			// 観戦時に右クリックされた時
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// 観戦終了プロトコル送る
			//	lssproto_B_send( sockfd, "U" );
			//}
			break;
			
		case BATTLE_SUBPROC_MOVIE:				// ムービー処理
		
#ifdef _DEBUG
			// 人数分ループ
			//for( i = 0 ; i < 20 ; i++ ){
				// 耐久力メーター表示
			//	HpMeterDisp( i );
			//}
			if(offlineFlag == TRUE){
				if(joy_trg[0]&JOY_A){		//リセットなら
					DeathAllAction();
					SubProcNo = 0;
					BattleCmd[0] = NULL;
					break;
				}
			}
#endif
			//スロー中なら（死亡の時）
			if( slow_flg ){		
				if( !( s_timer & 31 ) ){	// ８フレームに一回
					piyo_point++;	// ラスタースクロール進ませる
					piyo_point &= 63;	// リミットチェック
				}
				if( s_timer & 3 ){		// ４フレームに一回（ここはいじるな！！！）
					damage_dispx();		// スロー中のダメージ値表示
				}else{
					RunAction();		/* アクション走らせる */
					battle_quake();		//戦闘地震
				}
			} else { // スローでない時
				if( !( s_timer & 7 ) ){	
					piyo_point++;
					piyo_point &= 63;
				}
				RunAction();		/* アクション走らせる */
				battle_quake();		//戦闘地震
			}
			
			//死亡フラッシュなら
			if( flash_vct_no ) die_flash();	//死亡フラッシュ
			
			// タスク表示データをバッファに溜める
			StockTaskDispBuffer();

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
			//ターン終了なら
			if( action_inf == 1 ){		
				//if( BattleBpFlag & BATTLE_BP_JOIN ){	// 参戦の時
				//	BattleBpFlag &= ~BATTLE_BP_JOIN;
				//	SubProcNo = BATTLE_SUBPROC_CMD_INPUT;		//メニューに戻る
				//}else{
					SubProcNo = BATTLE_SUBPROC_RECEIVE_BC;		//ＢＣに戻る
					BattleCliTurnNo++;							// クライアントターン進める
				//}
#ifdef _DEBUG
				if(offlineFlag == TRUE){
					SubProcNo = BATTLE_SUBPROC_RECEIVE_BC;		//メニューに戻らない
				}
				
#endif
				action_inf = 0;
				// バトルコマンド初期化
				BattleCmd[ 0 ] = NULL;
				// メニューフラグＯＮ 
				battleMenuFlag2 = TRUE;
			}
			
			//戦闘脱出なら
			if( action_inf == 2 ){		
				SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;		//戦闘終了演出へ
				action_inf = 0;
				// バトルコマンド初期化
				BattleCmd[ 0 ] = NULL;
			}
			
			// 観戦時に右クリックされた時
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// 観戦終了プロトコル送る
			//	lssproto_B_send( sockfd, "U" );
			//}
			break;
			
		case BATTLE_SUBPROC_OUT_PRODUCE_INIT:	// 終了演出初期化
			
			//ＢＧＭフェードアウト
			fade_out_bgm();
			// バトルサーフェスの画像作成 
			CopyBackBuffer();
			// バックバッファー描画方法変更
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			SubProcNo++;
			break;
			
		case BATTLE_SUBPROC_OUT_PRODUCE:		// 終了演出
			
			// 演出中
			if( DrawProduce( PRODUCE_4WAY_OUT ) == TRUE ){
				// プロセスチェンジ
				ChangeProc( PROC_GAME, 1 );
				// アクション全抹殺
				DeathAllAction();
				// キャラ管理テーブルのアクションを抹消
				clearPtActCharObj();
				// この後からのＣ、ＣＡプロトコルを受け付ける
				encountNowFlag = 0;
				// クライアントが最後のムービーを見終わったときに送る。
				lssproto_EO_send( sockfd, 0 );
				//フィールドのＢＧＭ再生
//				play_bgm(map_bgm);		//フィールドＢＧＭ再生
			//	SubProcNo++;
				// 戦闘結果ウィンドウ出す時（逃げた時、アルチメットの時出さない
				if( battleResultMsg.useFlag >= 1 ){ 
					BattleResultWndFlag = battleResultMsg.useFlag;	// メニューフラグＯＮ
					battleResultMsg.useFlag = FALSE; // 構造体のフラグ初期化
				}
				return;
			}
			break;
			
		case 12:	// 演出
			break;
	}
	
	// プロデュース中でないとき  演出中没有的时候
	if( BackBufferDrawType != DRAW_BACK_PRODUCE ){ 
		// 戦闘強制終了の時
		if( BattleEscFlag == TRUE ){	
			// 終了プロセスへ
			SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;
			// フラグ初期化
			BattleEscFlag = FALSE;
		}
		// 観戦時に右クリックされた時
		if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
			// 観戦終了プロトコル送る
			lssproto_B_send( sockfd, "U" );
		}
		// ウィンドウある時
		if( pActAudienceExitWnd != NULL ){
			// 観戦終了文字表示ウィンドウが出来あがっていたら
			if( pActAudienceExitWnd->hp > 0 ){
				// 文字を表示
				StockFontBuffer( pActAudienceExitWnd->x + 30, pActAudienceExitWnd->y + 28, FONT_PRIO_FRONT, 0, 	"右クリックで", 0 );
				StockFontBuffer( pActAudienceExitWnd->x + 30, pActAudienceExitWnd->y + 52, FONT_PRIO_FRONT, 0, 	"観戦を終了します", 0 );
				//StockFontBuffer( pActInfoWnd->x + 38, pActInfoWnd->y + 28, FONT_PRIO_FRONT, FONT_PAL_YELLOW, 	"ターゲットを", 0 );
				//StockFontBuffer( pActInfoWnd->x + 38, pActInfoWnd->y + 52, FONT_PRIO_FRONT, FONT_PAL_YELLOW, 	"選択して下さい", 0 );
			}
		}
	}
}

