/************************/
/*	battleMenu.cpp	????????	*/
/************************/
#include "system.h"
#include "font.h"
#include "ime_sa.h"
#include "battleMenu.h"
#include "lssproto_cli.h"
#include "netmain.h"
#include "pc.h"
#include "anim_tbl.h"
#include "battleProc.h"
#include "t_music.h"
#include "menu.h"
#include "map.h"

// 攻撃種類
enum{ 
	BATTLE_ATTACK,
	BATTLE_JUJUTU,
	BATTLE_CAPTURE,
	BATTLE_ITEM,
	BATTLE_WAZA,
};

// トグル
#define BATTLE_MENU				( 1 << 0 )
#define BATTLE_MENU_PET			( 1 << 1 )

// メニューの階層
#define BATTLE_MENU_SUU					9
#define BATTLE_MENU_FONTS				9
#define BATTLE_MENU_JUJUTSU_FONTS		6
#define BATTLE_MENU_ITEM_FONTS			6
#define BATTLE_MENU_PET_FONTS			7
#define BATTLE_MENU_WAZA_FONTS			9

// バトルコマンドサイズ
//#define BATTLE_COMMAND_SIZE			4096

// バトルボタンの数
#define BATTLE_BUTTON_SUU	9

// コマンド受付時間
//#define BATTLE_CNT_DOWN_TIME 300000	// 五分
#define BATTLE_CNT_DOWN_TIME  30000	// ３０秒
//#define BATTLE_CNT_DOWN_TIME  10000	// １０秒
//#define BATTLE_CNT_DOWN_TIME  100000	// 90秒
//#define BATTLE_CNT_DOWN_TIME 10000	// １０秒
//#define BATTLE_CNT_DOWN_TIME 3000	// ３秒

// バトルコマンド文字列
char BattleCmd[ BATTLE_COMMAND_SIZE ];
// バトルコマンドバックアップ
char BattleCmdBak[ BATTLE_BUF_SIZE ][ BATTLE_COMMAND_SIZE ];
// バトルコマンド文字列ポインター
int BattleCmdReadPointer;
int BattleCmdWritePointer;

// バトルコマンド文字列バックアップ
//char BattleCmdBak[ BATTLE_COMMAND_SIZE ];

// バトル状態文字列
char BattleStatus[ BATTLE_COMMAND_SIZE ];
// バトル状態文字列バックアップ
char BattleStatusBak[ BATTLE_BUF_SIZE ][ BATTLE_COMMAND_SIZE ];
// バトル状態文字列ポインター
int BattleStatusReadPointer;
int BattleStatusWritePointer;
// バトル状態文字列バックアップ
//char BattleStatusBak[ BATTLE_COMMAND_SIZE ];
// バトル状態文字列バックアップ
//char BattleStatusBak2[ BATTLE_COMMAND_SIZE ];

// 自分の配列番号
int BattleMyNo = 0;
// 戦闘時の自分のＭＰ
int BattleMyMp;
// 戦闘強制終了フラグ
int BattleEscFlag = FALSE;

// ペットスタメンカウンター
int BattlePetStMenCnt;

// ＢＰフラグ
int BattleBpFlag = FALSE;
// コマンド番号
int BattleCmdNo = -1;
// 呪術番号
int BattleJujutuNo;
// 技番号
int BattleWazaNo;
// 参戦ペット番号バックアップ
int battlePetNoBak = -2;
// 参戦ペット番号バックアップ２
int battlePetNoBak2 = -2;
// アイテム番号
int BattleItemNo;

// プレイヤー逃げたフラグ
int battlePlayerEscFlag = FALSE;

// コマンド入力済みフラグ
int BattleAnimFlag;
// ターン受信フラグ
BOOL BattleTurnReceiveFlag;
// 現在のクライアントターン番号
int BattleCliTurnNo;
// 現在のサーバーターン番号
int BattleSvTurnNo;

// 戦闘結果ウィンドウフラグ
BOOL BattleResultWndFlag = FALSE;

// カウントダウン
DWORD BattleCntDown;
// カウントダウンフラグチェック
BOOL BattleCntDownFlag = FALSE;

// バトルメニュー
static BOOL battleButtonFlag[ BATTLE_BUTTON_SUU ];
static int battleButtonDispNo[ BATTLE_BUTTON_SUU ];
static int battleMenuJujutsuFontNo[ BATTLE_MENU_JUJUTSU_FONTS ];
static int battleMenuItemFontNo[ BATTLE_MENU_ITEM_FONTS ];
static int battleMenuPetFontNo[ BATTLE_MENU_PET_FONTS ];
static int battleMenuWazaFontNo[ BATTLE_MENU_WAZA_FONTS ];
static int battleMenuFlag = FALSE;				// 現在出でいるメニューのフラグ（ビット）
static int battleMenuReturn = FALSE;			// メニュー画面外へ行くフラグ
static int battlePetMenuFlag = FALSE;			// ペットメニュー出すフラグ
static int battlePetButtonFlag = FALSE;			// ペットの技ボタンへこみフラグ
static int battleTimeUpFlag = FALSE;
static int battleTargetSelectFlag = FALSE;
int battleMenuFlag2 = FALSE;	// プロセスからの切り替え用
static int battleButtonBak = -1;		// 前回のボタン記憶
static int battleButtonBak2 = -1;		// 前回のボタン記憶２
static int battleWazaTargetBak = -1;		// 前回の技番号

// ウィンドウアクションポインタ
//static ACTION *pActJujutsuWnd;
//static ACTION *pActItemWnd;
static ACTION *pActInfoWnd;
static ACTION *pActWnd = NULL;

// バトルボタンの座標
static int buttonX, buttonY, buttonA;

// バトルメニュー初期化 ********************************************************/
void InitBattleMenu( void )
{
	int i;
	// ボタン
	for( i = 0 ; i < BATTLE_BUTTON_SUU ; i++ ) battleButtonDispNo[ i ] = -2;
	// 呪術
	for( i = 0 ; i < BATTLE_MENU_JUJUTSU_FONTS ; i++ ) battleMenuJujutsuFontNo[ i ] = -2;
	// アイテム
	for( i = 0 ; i < BATTLE_MENU_ITEM_FONTS ; i++ ) battleMenuItemFontNo[ i ] = -2;
	// ペット
	for( i = 0 ; i < BATTLE_MENU_PET_FONTS ; i++ ) battleMenuPetFontNo[ i ] = -2;
	// 技
	for( i = 0 ; i < BATTLE_MENU_WAZA_FONTS ; i++ ) battleMenuWazaFontNo[ i ] = -2;

	battleMenuFlag = FALSE;				// メニューフラグ初期化
	battleMenuReturn = TRUE;			// メニュー隠すフラグＯＦＦ
	battleButtonBak = -1;				// 前回のボタン記憶
	battleButtonBak2 = -1;				// 前回のボタン記憶２
	//battlePetNoBak = -2;				// 参戦ペット番号記憶
	BattleEscFlag = FALSE;				// 戦闘強制終了フラグ初期化
	// 戦闘結果ウィンドウフラグ
	BattleResultWndFlag = FALSE;
	// ボタンフラグ全初期化
	ClearBattleButton();
	// デフォルトで攻撃ボタンＯＮ
	battleButtonBak = 0;
	//DeathAction( pActWnd );
	pActWnd = NULL;				// ウィンドウポインタ初期化
	// インフォメーションウィンドウ抹殺
	//DeathAction( pActInfoWnd );
	pActInfoWnd = NULL;
	// ペットが入れ替わってなかったら
	if( battlePetNoBak != -2 ){
		battlePetButtonFlag = FALSE;			// ペットの技ボタン
	}else{
		battlePetButtonFlag = TRUE;			// ペットの技ボタン
	}
}

// 一人用か二人用かチェック ********************************************/
int CheckBattle1P2P( void )
{
	int i, cnt = 0;
	
	// ２０匹分ループ
	for( i = 0 ; i < 20 ; i++ ){
		// ポインタがない時戻る
		if( p_party[ i ]->func == NULL ) continue;
		// 死んでいる時
		//if( p_party[ i ]->hp <= 0 ) continue;
		// プレイヤーなら
		if( ( (ATR_EQU *)p_party[ i ]->pYobi )->pet_ok == TRUE ) cnt++;
	}
	if( cnt >= 2 ) return 2;
	else return 1;
}
// コマンド入力済みフラグのチェック ********************************************/
void CheckBattleAnimFlag( void )
{
	int i;
	
	// 更新が無ければ戻る
	//if( BattleAnimFlag == 0 ) return;
	
	// クライアントターンとサーバーターンが同じの時全員フラグ立てる
	if( BattleCliTurnNo != BattleSvTurnNo ){ 
		// ２０匹分ループ
		for( i = 0 ; i < 20 ; i++ ){
			// 最下位ビットを判定
			p_party[ i ]->atr |= ACT_ATR_BTL_CMD_END;
		}
		return;
	}
	// ２０匹分ループ
	for( i = 0 ; i < 20 ; i++ ){
		// 最下位ビットを判定
		if( ( BattleAnimFlag >> i ) & 0x00000001 ) p_party[ i ]->atr |= ACT_ATR_BTL_CMD_END;
		//BattleAnimFlag >>= 1;	// 右へ１ビットシフト
	}
	// 初期化
	//BattleAnimFlag = 0;
}

// コマンド入力済みフラグの初期化 ********************************************/
void InitBattleAnimFlag( void )
{
	int i;
	
	// ２０匹分ループ
	for( i = 0 ; i < 20 ; i++ ){
		// 最下位ビットを判定
		p_party[ i ]->atr &= ~ACT_ATR_BTL_CMD_END;
	}
}


// ペット所持数を調べる *******************************************************/
int CheckPetSuu( void )
{
	int i,cnt = 0;
	
	for( i = 0 ; i < 5 ; i++ ){
		if( pet[ i ].useFlag == TRUE ) cnt++;
	}
	return cnt;
}

// 当たり判定ボックス消す *****************************************************/
void ClearBoxFlag( void )
{
	int i;
	
	// 当たり判定ボックス消す
	for( i = 0 ; i < 20 ; i++ ){
		// ポインタがない時戻る
		if( p_party[ i ]->func == NULL ) continue;
		// 当たり判定ボックス表示フラグＯＦＦ
		p_party[ i ]->atr &= ~ACT_ATR_HIT_BOX;
		p_party[ i ]->atr &= ~ACT_ATR_HIT_BOX_ALL1;
		p_party[ i ]->atr &= ~ACT_ATR_HIT_BOX_ALL2;
		p_party[ i ]->atr &= ~ACT_ATR_HIT_BOX_ALL3;
		p_party[ i ]->atr |= ACT_ATR_HIT;
		// 当たり判定番号初期化
		p_party[ i ]->hitDispNo = -2;
	}
}

// ターゲットとの当たり判定 ***************************************************/
int CheckBattelTarget( void )
{
	int i;
	
	// キャラクターの数だけループ
	for( i = 0 ; i < 20 ; i++ ){
		// ポインタがない時戻る
		if( p_party[ i ]->func == NULL ) continue;
		// 選べない時戻る
		if( !( p_party[ i ]->atr & ACT_ATR_HIT_BOX ) &&
			!( p_party[ i ]->atr & ACT_ATR_HIT_BOX_ALL1 ) &&
			!( p_party[ i ]->atr & ACT_ATR_HIT_BOX_ALL2 ) &&
			!( p_party[ i ]->atr & ACT_ATR_HIT_BOX_ALL3 ) ) continue;
		// 当たっていたら
		if( p_party[ i ]->hitDispNo == HitDispNo ) return i;
	}	
	// 当たっていない時
	return -1;
}
// ボタンフラグ全初期化　*****************************************************/
void ClearBattleButton( void )
{
	// アイテム
	memset( battleButtonFlag, 0, sizeof( battleButtonFlag ) );
}

// バトルボタンをすべてＯＦＦにする *******************************************/
void BattleButtonOff( void )
{
	// ウィンドウある時
//	if( pActWnd != NULL ){ 
		// ウィンドウ閉じる音
//		play_se( 203, 320, 240 );
//	}else{
		// クリック音
		play_se( 217, 320, 240 );
//	}
	// ウィンドウアクション抹殺
	DeathAction( pActWnd );
	pActWnd = NULL;				// ウィンドウポインタ初期化
	// ターゲットセレクトフラグＯＦＦ
	battleTargetSelectFlag = FALSE;
	// コマンド番号
	BattleCmdNo = -1;
	// ボタンフラグ全初期化
	ClearBattleButton();
	// 当たり判定ボックス消す
	ClearBoxFlag();
}

// アイテム関連の初期化 *******************************************************/
void InitItem2( int x, int y )
{
	int i, j = 0, k = 0;
	
	// アイテム数ループ
	for( i = 5 ; i < MAX_ITEM ; i++ ){
	
		ItemBuffer[ i ].defX = x + 32 + j;
		ItemBuffer[ i ].defY = y + 56 + 48 + 48 + 48 -160 + k;
		ItemBuffer[ i ].x = ItemBuffer[ i ].defX;
		ItemBuffer[ i ].y = ItemBuffer[ i ].defY;
		ItemBuffer[ i ].bmpNo = 20000 + j / 48;
		ItemBuffer[ i ].dispPrio = DISP_PRIO_ITEM;
		
		j += 51; // Ｘ座標ずらす
		// リミットチェック
		if( j >= 48 * 5 ){
			j = 0;
			k += 48; // Ｙ座標ずらす
		}
	}
}

// 耐久力メーター表示 ***********************************************************/
void HpMeterDisp( int no )
{
	int meterX = p_party[ no ]->x;
	int meterY = p_party[ no ]->y - 80;
	int graNo;
	
	// 存在しない又は死んでる時、戻る
	if( p_party[ no ]->func == NULL || p_party[ no ]->hp <= 0 ) return;
	// 旅行中の時
	if( p_party[ no ]->atr & ACT_ATR_TRAVEL ) return;
	
	// 自分との時
	if( no == BattleMyNo ){
		// 表示しない時
		//if( CheckBattle1P2P() == 1 
		//	&& BattleBpFlag & BATTLE_BP_PLAYER_MENU_NON 
		//	&& ( BattleBpFlag & BATTLE_BP_PET_MENU_NON || p_party[ BattleMyNo + 5 ]->hp <= 0 ) ) return;
		
		// Ｐ表示
		//StockFontBuffer( meterX - 12, meterY - 8, FONT_PRIO_BACK, FONT_PAL_GREEN, "Player", 0 );
		//StockFontBuffer( meterX - 12 - 20, meterY - 8, FONT_PRIO_BACK, FONT_PAL_GREEN, "P", 0 );
		// 枠表示
		StockDispBuffer( meterX, meterY, DISP_PRIO_IME1, CG_BATTLE_BAR_PLAYER_2, 0 );
		// 耐久力表示
		StockBoxDispBuffer( meterX - 21,
							meterY - 3,
							meterX - 21 + (int)( ( (double)p_party[ BattleMyNo ]->hp / (double)p_party[ BattleMyNo ]->maxHp ) * 40.0 ),
							meterY - 3,
							DISP_PRIO_IME2, SYSTEM_PAL_GREEN, 2 );
		StockBoxDispBuffer( meterX - 21,
							meterY - 2,
							meterX - 21 + (int)( ( (double)p_party[ BattleMyNo ]->hp / (double)p_party[ BattleMyNo ]->maxHp ) * 40.0 ),
							meterY - 2,
							DISP_PRIO_IME2, SYSTEM_PAL_GREEN, 2 );
		// 気力表示
		StockBoxDispBuffer( meterX - 21,
							meterY + 1,
							meterX - 21 + (int)( ( (double)p_party[ BattleMyNo ]->mp / 100.0 ) * 40.0 ),
							meterY + 1,
							DISP_PRIO_IME2, SYSTEM_PAL_YELLOW, 2 );
		StockBoxDispBuffer( meterX - 21,
							meterY + 2,
							meterX - 21 + (int)( ( (double)p_party[ BattleMyNo ]->mp / 100.0 ) * 40.0 ),
							meterY + 2,
							DISP_PRIO_IME2, SYSTEM_PAL_YELLOW, 2 );
	}else{	// 自分以外の時
		
		// 自分のペットの時
		//if( no == BattleMyNo + 5 ) StockFontBuffer( meterX - 12 - 20, meterY - 8, FONT_PRIO_BACK, FONT_PAL_YELLOW, "P", 0 );
		if( no == BattleMyNo + 5 ) graNo = CG_BATTLE_BAR_PET_2;
		else graNo = CG_BATTLE_BAR_PET;
		// 枠表示
		StockDispBuffer( meterX , meterY, DISP_PRIO_IME1, graNo, 0 );
		// 耐久力表示（１ライン目）
		StockBoxDispBuffer( meterX - 21,
							meterY - 1,
							meterX - 21 + (int)( ( (double)p_party[ no ]->hp / (double)p_party[ no ]->maxHp ) * 40.0 ),
							meterY - 1,
							DISP_PRIO_IME2, SYSTEM_PAL_GREEN, 2 );
		// 耐久力表示（２ライン目）
		StockBoxDispBuffer( meterX - 21,
							meterY + 0,
							meterX - 21 + (int)( ( (double)p_party[ no ]->hp / (double)p_party[ no ]->maxHp ) * 40.0 ),
							meterY + 0,
							DISP_PRIO_IME2, SYSTEM_PAL_GREEN, 2 );
	}
}

// ◎攻撃ボタン *****************************************************************/
void BattleButtonAttack( void )
{
	int i, j, k, bak, hitBoxAll;
	
	if( HitDispNo == battleButtonDispNo[ 0 ] || battleButtonBak2 == 0 ){
		// 左クリックされた時
		if( mouse.onceState & MOUSE_LEFT_CRICK || battleButtonBak2 == 0 ){
		
			bak = battleButtonFlag[ 0 ];	// バックアップ
			BattleButtonOff();				// とりあえず全てＯＦＦにする
			
			// ボタンがＯＦＦの時ＯＮにする
			if( bak == FALSE ){

				// ターゲットセレクトフラグＯＮ
				battleTargetSelectFlag = TRUE;
				// コマンド番号
				BattleCmdNo = BATTLE_ATTACK;
				// このボタンだけＯＮ
				battleButtonFlag[ 0 ] = TRUE;
				// 今回のボタン記憶
				battleButtonBak = 0;
				// 前回のボタン初期化
				battleButtonBak2 = -1;
				// ブーメランの時
				if( BattleBpFlag & BATTLE_BP_BOOMERANG ){
					// 初期化
					hitBoxAll = ACT_ATR_HIT_BOX_ALL1;
					j = 0;
					// ４ライン分処理
					for( i = 0 ; i < 4 ; i++ ){
						// 自分のラインで無い時
						if( !( j <= BattleMyNo && BattleMyNo < j + 5 ) ){
						//if( BattleMyNo < j || j + 5 < BattleMyNo ){
							// 当たり判定ボックス表示
							for( k = j ; k < j + 5 ; k++ ){
								// ポインタがない時戻る
								if( p_party[ k ]->func == NULL ) continue;
								// 死んでいる時
								if( p_party[ k ]->hp <= 0 ) continue;
								// 旅行中の時
								if( p_party[ k ]->atr & ACT_ATR_TRAVEL ) continue;
								// グループフラグＯＮ
								p_party[ k ]->atr |= hitBoxAll;
							}
							// 次のグループにする
							hitBoxAll <<= 1;
						}
						j += 5;
					}
				}else{
					// 当たり判定ボックス表示
					for( i = 0 ; i < 20 ; i++ ){
						// 自分なら戻る
						if( i == BattleMyNo ) continue;
						// ポインタがない時戻る
						if( p_party[ i ]->func == NULL ) continue;
						// 死んでいる時
						if( p_party[ i ]->hp <= 0 ) continue;
						// 当たり判定ボックス表示フラグＯＮ
						p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
					}
				}
#if 0
				// 当たり判定ボックス表示
				for( i = 0 ; i < 10 ; i++ ){
					// ポインタがない時戻る
					if( p_party[ i ]->func == NULL ) continue;
					// 死んでいる時
					if( p_party[ i ]->hp <= 0 ) continue;
					// 当たり判定ボックス表示フラグＯＮ
					//p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
					p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
				}
				// 当たり判定ボックス表示
				for( i = 10 ; i < 20 ; i++ ){
					// ポインタがない時戻る
					if( p_party[ i ]->func == NULL ) continue;
					// 死んでいる時
					if( p_party[ i ]->hp <= 0 ) continue;
					// 当たり判定ボックス表示フラグＯＮ
					//p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
					p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL2;
				}
#endif
			}
			
		}
		// 一行インフォ
		strcpy( OneLineInfoStr,"攻撃します。");
	}
}

// ◎呪術ボタン *****************************************************************/
void BattleButtonJujutsu( void )
{
	int i, x, y, bak;
	char moji[ 256 ];
	
	if( HitDispNo == battleButtonDispNo[ 1 ] || battleButtonBak2 == 1 ){
		// 左クリックされた時
		if( mouse.onceState & MOUSE_LEFT_CRICK || battleButtonBak2 == 1 ){
		
			bak = battleButtonFlag[ 1 ];	// バックアップ
			BattleButtonOff();				// とりあえず全てＯＦＦにする
		
			// ボタンがＯＦＦの時
			if( bak == FALSE ){
			
				// このボタンだけＯＮ
				battleButtonFlag[ 1 ] = TRUE;
				// コマンド番号
				BattleCmdNo = BATTLE_JUJUTU;
				// 文字の当たり判定初期化
				for( i = 0 ; i < BATTLE_MENU_JUJUTSU_FONTS ; i++ ) battleMenuJujutsuFontNo[ i ] = -2;
				// ウィンドウ表示タスク作成
				//pActWnd = MakeWindowDisp( 380, 120, 4, 7, CG_WND_TITLE_SYSTEM, 1 );
				// ウィンドウ表示タスク作成
				//pActWnd = MakeWindowDisp( 361, 144, 272, 280, 0, -1 );
				// ウィンドウ表示タスク作成
				pActWnd = MakeWindowDisp( 361, 144, 271, 281, 0, -1 );
				// 今回のボタン記憶
				battleButtonBak = 1;
				// 前回のボタン初期化
				battleButtonBak2 = -1;
			}
		}
		// 一行インフォ
		strcpy( OneLineInfoStr, "呪術を唱えます。");
	}
	// 呪術ウィンドウ処理
	if( battleButtonFlag[ 1 ] == TRUE ){
			// ウィンドウがある時
		if( pActWnd != NULL ){
			// ウィンドウが出来あがっていたら
			if( pActWnd->hp > 0 ){
				x = pActWnd->x + 74;
				y = pActWnd->y + 208;
				// 一行インフォ
				//strcpy( OneLineInfoStr, "呪術を選択してください。");
				// 呪術ウィンドウ
				StockDispBuffer( ( ( WINDOW_DISP *)pActWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActWnd->pYobi )->my + 10, DISP_PRIO_MENU, CG_JUJUTU_WND, 1 );
				// 呪術選択した時
				for( i = 0 ; i < 5 ; i++ ){
					// 文字に当たっている時かつ、この欄に呪術がある時
					if( HitFontNo == battleMenuJujutsuFontNo[ i ] && magic[ i ].useFlag == TRUE ){
						// 改行ポイント
						char *splitPoint = magic[ i ].memo;
						//char *splitPoint = "表示テストです。私の名前は太田哲生です。これからもよろしくお願いします。";
						// 消費ＭＰ表示
						//sprintf( moji,"%2d/%2d", magic[ i ].mp, BattleMyMp );
						sprintf( moji,"%2d/%2d", magic[ i ].mp, p_party[ BattleMyNo ]->mp );
						StockFontBuffer( pActWnd->x + 74 + 81, pActWnd->y + 183, FONT_PRIO_FRONT, 0, moji, 0 );
						// アイテムの名前表示
						//StockFontBuffer( pActMenuWnd2->x + 16, pActMenuWnd2->y + 331, FONT_PRIO_FRONT, pc.item[ i ].color, pc.item[ i ].name, 0 );	
						// 呪術の説明表示
						// 行数分ループ
						while( 1 ){
							// ２２文字を超えてたら分割
							if( strlen( splitPoint ) > 22 ){
								strncpy( moji, splitPoint, 22 );
								moji[ 22 ] = NULL;	// 終端記号をいれる
								// 全角の半分だった時
								if( GetStrLastByte( moji ) == 3 ){
									moji[ 21 ] = NULL;
									splitPoint += 21;
								}else{
									moji[ 22 ] = NULL;
									splitPoint += 22;
								}
								StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
							}else{
								strcpy( moji, splitPoint );
								StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 0 );
								break;
							}
						}
		#if 0
						// 文字列がある時
						if( magic[ i ].memo[ 0 ] != NULL ){
							// 呪術の説明
							strncpy( moji, magic[ i ].memo, 22 );
							moji[ 22 ] = NULL;
							StockFontBuffer( pActWnd->x + 74, pActWnd->y + 204, FONT_PRIO_FRONT, 0, moji, 0 );
							
							if( magic[ i ].memo[ 22 ] != NULL ){
								strncpy( moji, magic[ i ].memo + 22, 22 );
								moji[ 22 ] = NULL;
								StockFontBuffer( pActWnd->x + 74, pActWnd->y + 224, FONT_PRIO_FRONT, 0, moji, 0 );
							}
						}
		#endif
						//StockFontBuffer( pActWnd->x + 74, pActWnd->y + 206, FONT_PRIO_FRONT, 0, "呪術の説明欄です。ああ", 0 );
						//StockFontBuffer( pActWnd->x + 74, pActWnd->y + 226, FONT_PRIO_FRONT, 0, "呪術の説明欄です。ああ", 0 );
						// アイテム表示
						StockDispBuffer( pActWnd->x + 37, pActWnd->y + 220, DISP_PRIO_ITEM, pc.item[ i ].graNo, 0 );
						
						// 左クリックした時
						if( mouse.onceState & MOUSE_LEFT_CRICK ){
							// 使用不可又はＭＰ不足の時
							//if( magic[ i ].field == MAGIC_FIELD_MAP || magic[ i ].mp > BattleMyMp ){
							if( magic[ i ].field == MAGIC_FIELD_MAP || magic[ i ].mp > p_party[ BattleMyNo ]->mp ){
								// ＮＧ音
								play_se( 220, 320, 240 );
								
							}else{ // 使用可能な時
							
								// 呪術番号記憶
								BattleJujutuNo = i;
								// ターゲットセレクトフラグＯＮ
								battleTargetSelectFlag = TRUE;
								
								// ターゲットで分岐
								switch( magic[ i ].target ){
								//switch( 8 ){
								
								case MAGIC_TARGET_MYSELF:	// 自分のみ
								
									// 当たり判定ボックス表示フラグＯＮ
									p_party[ BattleMyNo ]->atr |= ACT_ATR_HIT_BOX;
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = TRUE;
									// ウィンドウアクション抹殺
									DeathAction( pActWnd );
									pActWnd = NULL;
									// ボタンフラグ全初期化
									ClearBattleButton();
									// コマンド番号
									BattleCmdNo = BATTLE_JUJUTU;
									// クリック音
									play_se( 217, 320, 240 );
									
									break;
									
								case MAGIC_TARGET_OTHER:	// 他の人（自分含む)
								
									// 当たり判定ボックス表示
									for( i = 0 ; i < 20 ; i++ ){
										// ポインタがない時戻る
										if( p_party[ i ]->func == NULL ) continue;
										// 死んでるやつは対象外の時
										if( magic[ BattleJujutuNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
										// 当たり判定ボックス表示フラグＯＮ
										p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
									}
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = TRUE;
									// ウィンドウアクション抹殺
									DeathAction( pActWnd );
									pActWnd = NULL;
									// ボタンフラグ全初期化
									ClearBattleButton();
									// コマンド番号
									BattleCmdNo = BATTLE_JUJUTU;
									// クリック音
									play_se( 217, 320, 240 );

									break;
									
								case MAGIC_TARGET_ALLMYSIDE:	// 味方全体
								
									// 右下グループの時
									if( BattleMyNo < 10 ){
										// 当たり判定ボックス表示
										for( i = 0 ; i < 10 ; i++ ){
											// ポインタがない時戻る
											if( p_party[ i ]->func == NULL ) continue;
											// 死んでるやつは対象外の時
											if( magic[ BattleJujutuNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
											// 旅行中の時
											if( p_party[ i ]->atr & ACT_ATR_TRAVEL ) continue;
											// グループフラグＯＮ
											p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
										}
									}
									// 左上グループの時
									else{
										// 当たり判定ボックス表示
										for( i = 10 ; i < 20 ; i++ ){
											// ポインタがない時戻る
											if( p_party[ i ]->func == NULL ) continue;
											// 死んでるやつは対象外の時
											if( magic[ BattleJujutuNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
											// 旅行中の時
											if( p_party[ i ]->atr & ACT_ATR_TRAVEL ) continue;
											// グループフラグＯＮ
											p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL2;
										}
									}
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = TRUE;
									// ウィンドウアクション抹殺
									DeathAction( pActWnd );
									pActWnd = NULL;
									// ボタンフラグ全初期化
									ClearBattleButton();
									// コマンド番号
									BattleCmdNo = BATTLE_JUJUTU;
									// クリック音
									play_se( 217, 320, 240 );
									break;
									
								case MAGIC_TARGET_ALLOTHERSIDE:	// 相手側全体
								
									// 左上グループの時
									if( BattleMyNo >= 10 ){
										// 当たり判定ボックス表示
										for( i = 0 ; i < 10 ; i++ ){
											// ポインタがない時戻る
											if( p_party[ i ]->func == NULL ) continue;
											// 死んでるやつは対象外の時
											if( magic[ BattleJujutuNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
											// 旅行中の時
											if( p_party[ i ]->atr & ACT_ATR_TRAVEL ) continue;
											// グループフラグＯＮ
											p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
										}
									}
									// 右下グループの時
									else{
										// 当たり判定ボックス表示
										for( i = 10 ; i < 20 ; i++ ){
											// ポインタがない時戻る
											if( p_party[ i ]->func == NULL ) continue;
											// 死んでるやつは対象外の時
											if( magic[ BattleJujutuNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
											// 旅行中の時
											if( p_party[ i ]->atr & ACT_ATR_TRAVEL ) continue;
											// グループフラグＯＮ
											p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL2;
										}
									}
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = TRUE;
									// ウィンドウアクション抹殺
									DeathAction( pActWnd );
									pActWnd = NULL;
									// ボタンフラグ全初期化
									ClearBattleButton();
									// コマンド番号
									BattleCmdNo = BATTLE_JUJUTU;
									// クリック音
									play_se( 217, 320, 240 );
									break;
									
								case MAGIC_TARGET_ALL:	// 全て
								
									// 当たり判定ボックス表示
									for( i = 0 ; i < 20 ; i++ ){
										// ポインタがない時戻る
										if( p_party[ i ]->func == NULL ) continue;
										// 死んでるやつは対象外の時
										if( magic[ BattleJujutuNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
										// 旅行中の時
										if( p_party[ i ]->atr & ACT_ATR_TRAVEL ) continue;
										// グループフラグＯＮ
										p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
									}
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = TRUE;
									// ウィンドウアクション抹殺
									DeathAction( pActWnd );
									pActWnd = NULL;
									// ボタンフラグ全初期化
									ClearBattleButton();
									// コマンド番号
									BattleCmdNo = BATTLE_JUJUTU;
									// クリック音
									play_se( 217, 320, 240 );
									break;
									
								case MAGIC_TARGET_WHOLEOTHERSIDE:	// どちらかサイド全体
								
									// 当たり判定ボックス表示
									for( i = 0 ; i < 10 ; i++ ){
										// ポインタがない時戻る
										if( p_party[ i ]->func == NULL ) continue;
										// 死んでるやつは対象外の時
										if( magic[ BattleJujutuNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
										// 旅行中の時
										if( p_party[ i ]->atr & ACT_ATR_TRAVEL ) continue;
										// グループフラグＯＮ
										p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
									}
									// 当たり判定ボックス表示
									for( i = 10 ; i < 20 ; i++ ){
										// ポインタがない時戻る
										if( p_party[ i ]->func == NULL ) continue;
										// 死んでるやつは対象外の時
										if( magic[ BattleJujutuNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
										// 旅行中の時
										if( p_party[ i ]->atr & ACT_ATR_TRAVEL ) continue;
										// グループフラグＯＮ
										p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL2;
									}
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = TRUE;
									// ウィンドウアクション抹殺
									DeathAction( pActWnd );
									pActWnd = NULL;
									// ボタンフラグ全初期化
									ClearBattleButton();
									// コマンド番号
									BattleCmdNo = BATTLE_JUJUTU;
									// クリック音
									play_se( 217, 320, 240 );
									break;
								}
							}
						}
					}
				}
				// 左クリックした時
				if( mouse.onceState & MOUSE_LEFT_CRICK ){
					// 戻る時
					if( HitDispNo == battleMenuJujutsuFontNo[ 5 ] ){
						BattleButtonOff();				
					}
				}
				// ウィンドウある時
				if( pActWnd != NULL ){
					int flag = FALSE;
					int col;
					// 座標設定
					x = pActWnd->x + 30;
					y = pActWnd->y + 35;
					
					// 呪術表示
					for( i = 0 ; i < 5 ; i++ ){
						// この欄に呪術がある時
						if( magic[ i ].useFlag == TRUE ){
							col = FONT_PAL_WHITE;	// 使用可能な色（白）
							//if( magic[ i ].mp > BattleMyMp ) col = FONT_PAL_RED;	// ＭＰ不足（赤）
							if( magic[ i ].mp > p_party[ BattleMyNo ]->mp ) col = FONT_PAL_RED;	// ＭＰ不足（赤）
							if( magic[ i ].field == MAGIC_FIELD_MAP ) col = FONT_PAL_GRAY;	// 使用不可な色（赤）
							// 文字表示バッファに溜める
							//sprintf( moji," %-16s ", magic[ i ].name );
							CenteringStr( magic[ i ].name, moji, MAGIC_NAME_LEN );
							battleMenuJujutsuFontNo[ i ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, col, moji, 2 ); y += 28;
							flag = TRUE;
						}
					}
					if( flag == FALSE ) StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, "呪術がありません", 0 );
					// 文字表示バッファに溜める
					battleMenuJujutsuFontNo[ 5 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActWnd->pYobi )->mx, pActWnd->y + 262 + 10, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
				}
			}
		}
	}
}

// ◎捕獲ボタン *****************************************************************/
void BattleButtonCapture( void )
{
	int i, bak;
	
	if( HitDispNo == battleButtonDispNo[ 2 ] || battleButtonBak2 == 2 ){
		// ペット所持数を調べる
		if( CheckPetSuu() >= 5 ){
			// 一行インフォ
			strcpy( OneLineInfoStr, "これ以上ペットを捕獲できません。");
			// 左クリックされた時
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				// ＮＧ音
				play_se( 220, 320, 240 );
			}
			// 前回のボタン初期化
			battleButtonBak2 = -1;
		}else{
			// 一行インフォ
			strcpy( OneLineInfoStr, "恐竜を捕獲します。");
		
			// 左クリックされた時
			if( mouse.onceState & MOUSE_LEFT_CRICK || battleButtonBak2 == 2 ){
			
				bak = battleButtonFlag[ 2 ];	// バックアップ
				BattleButtonOff();				// とりあえず全てＯＦＦにする
			
				// ボタンがＯＦＦの時
				if( bak == FALSE ){
					
					// ターゲットセレクトフラグＯＮ
					battleTargetSelectFlag = TRUE;
					// コマンド番号
					BattleCmdNo = BATTLE_CAPTURE;
					// このボタンだけＯＮ
					battleButtonFlag[ 2 ] = TRUE;
					// 今回のボタン記憶
					battleButtonBak = 2;
					// 前回のボタン初期化
					battleButtonBak2 = -1;
					// 当たり判定ボックス表示
					for( i = 0 ; i < 20 ; i++ ){
						// ポインタがない時戻る
						if( p_party[ i ]->func == NULL ) continue;
						// 味方なら戻る
						if( ( (ATR_EQU *)p_party[ i ]->pYobi )->group_flg == ( (ATR_EQU *)p_party[ BattleMyNo ]->pYobi )->group_flg ) continue;
						// 自分なら戻る
						if( i == BattleMyNo ) continue;
						// 死んでいる時
						if( p_party[ i ]->hp <= 0 ) continue;
						// 当たり判定ボックス表示フラグＯＮ
						p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
					}
				}
			}
		}
	}
}

// ◎ＨＥＬＰボタン *************************************************************/
void BattleButtonHelp( void )
{
	if( HitDispNo == battleButtonDispNo[ 3 ] && battleButtonFlag[ 3 ] == FALSE ){
		// 左クリックされた時
		// 一行インフォ
		strcpy( OneLineInfoStr, "助けを呼びます。");
		
		if( mouse.onceState & MOUSE_LEFT_CRICK ){
			// ヘルプなしの時
			if( NoHelpFlag == TRUE ){
				// ＮＧ音
				play_se( 220, 320, 240 );
			}else{
				// ＯＮの時
				if( helpFlag == TRUE ){
					// ＯＦＦにする時
					helpFlag = FALSE;
				}else{
					// ＯＮにする時
					helpFlag = TRUE;
				}
				// 参戦フラグ送信
				lssproto_HL_send( sockfd, helpFlag );
				// クリック音
				play_se( 217, 320, 240 );
			}
		}
	}
}

// ◎防御ボタン *****************************************************************/
void BattleButtonGuard( void )
{
	if( HitDispNo == battleButtonDispNo[ 4 ] && battleButtonFlag[ 4 ] == FALSE ){
		// 左クリックされた時
		if( mouse.onceState & MOUSE_LEFT_CRICK ){
			BattleButtonOff();	// とりあえず全てＯＦＦにする
			// このボタンだけＯＮ
			battleButtonFlag[ 4 ] = TRUE;
			// 防御送信
			lssproto_B_send( sockfd, "G" );
			// メニュー隠す
			battleMenuReturn = TRUE;
			// 今回のボタン記憶
			battleButtonBak = -1;
			// 前回のボタン初期化
			battleButtonBak2 = -1;
			// ウィンドウ閉じる音
			play_se( 203, 320, 240 );
		}
		// 一行インフォ
		strcpy( OneLineInfoStr, "防御します。");
	}
}

// ◎アイテムボタン *************************************************************/
void BattleButtonItem( void )
{
	int i, bak;
	char moji[ 256 ];
	int x, y;
	
	if( HitDispNo == battleButtonDispNo[ 5 ] || battleButtonBak2 == 5 ){
		// 左クリックされた時
		if( mouse.onceState & MOUSE_LEFT_CRICK || battleButtonBak2 == 5 ){

			bak = battleButtonFlag[ 5 ];	// バックアップ
			BattleButtonOff();				// とりあえず全てＯＦＦにする
		
			// ボタンがＯＦＦの時
			if( bak == FALSE ){
				// このボタンだけＯＮ
				battleButtonFlag[ 5 ] = TRUE;
				// コマンド番号
				BattleCmdNo = BATTLE_ITEM;
				// 文字の当たり判定初期化
				for( i = 0 ; i < BATTLE_MENU_ITEM_FONTS ; i++ ) battleMenuItemFontNo[ i ] = -2;
				// ウィンドウ表示タスク作成
				pActWnd = MakeWindowDisp( 361, 160, 271, 281, 0, -1 );
				// アイテム関連の初期化
				InitItem2( pActWnd->x, pActWnd->y );
				// 今回のボタン記憶
				battleButtonBak = 5;
				// 前回のボタン初期化
				battleButtonBak2 = -1;
			}
		}
		// 一行インフォ
		strcpy( OneLineInfoStr, "アイテムを使います。");
	}
	// アイテムウィンドウ処理
	if( battleButtonFlag[ 5 ] == TRUE ){
		// ウィンドウがある時
		if( pActWnd != NULL ){
			// ウィンドウが出来あがっていたら
			if( pActWnd->hp > 0 ){
				// アイテムウィンドウ下表示
				StockDispBuffer( ( ( WINDOW_DISP *)pActWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActWnd->pYobi )->my - 80, DISP_PRIO_MENU, CG_ITEM_WND_1, 1 );
				// アイテムウィンドウタイトル表示
				StockDispBuffer( ( ( WINDOW_DISP *)pActWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActWnd->pYobi )->my - 80, DISP_PRIO_MENU, CG_BTL_ITEM_WND_TITLE, 1 );
				
				// 左クリックした時
				if( mouse.onceState & MOUSE_LEFT_CRICK ){
					// 閉じる時
					if( HitDispNo == battleMenuItemFontNo[ 0 ] ){
						// とりあえず全てＯＦＦにする
						BattleButtonOff();				
					}
				}
#if 0				
				// アイテムループ
				for( i = 5 ; i < MAX_ITEM ; i++ ){
					// 左ダブルクリックでアイテム使用
					if( mouse.onceState & MOUSE_LEFT_DBL_CRICK ){
						// その欄にアイテムがあったら
						if( pc.item[ i ].useFlag == TRUE ){
							// 戦闘で使えるない時
							//if( ( pc.item[ i ].field == ITEM_FIELD_ALL || pc.item[ i ].field == ITEM_FIELD_BATTLE ) && pc.level >= pc.item[ i ].level ){
							if( pc.item[ i ].useFlag != TRUE || pc.item[ i ].field == ITEM_FIELD_MAP || pc.level < pc.item[ i ].level ){
								// ＮＧ音
								play_se( 220, 320, 240 );
							}else{ // 使用可能な時
							
								BattleItemNo = i;	// アイテム番号記憶
							
								// ターゲットで分岐
								switch( pc.item[ i ].target ){
								
								case ITEM_TARGET_MYSELF:				// 自分のみ
								
									// 当たり判定ボックス表示フラグＯＮ
									p_party[ BattleMyNo ]->atr |= ACT_ATR_HIT_BOX;
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = TRUE;
									// ウィンドウアクション抹殺
									DeathAction( pActWnd );
									pActWnd = NULL;
									// ボタンフラグ全初期化
									ClearBattleButton();
									// コマンド番号
									BattleCmdNo = BATTLE_ITEM;
									// クリック音
									play_se( 217, 320, 240 );
									break;
									
								case ITEM_TARGET_OTHER:					// 他の人（自分含む)
								
									// 当たり判定ボックス表示
									for( i = 0 ; i < 20 ; i++ ){
										// ポインタがない時戻る
										if( p_party[ i ]->func == NULL ) continue;
										// 死んでるやつは対象外の時
										if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
										// 当たり判定ボックス表示フラグＯＮ
										p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
									}
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = TRUE;
									// ウィンドウアクション抹殺
									DeathAction( pActWnd );
									pActWnd = NULL;
									// ボタンフラグ全初期化
									ClearBattleButton();
									// コマンド番号
									BattleCmdNo = BATTLE_ITEM;
									// クリック音
									play_se( 217, 320, 240 );
									break;
									
								case ITEM_TARGET_ALLMYSIDE:				// 味方全体
								
									// 右下グループの時
									if( BattleMyNo < 10 ){
										// 当たり判定ボックス表示
										for( i = 0 ; i < 10 ; i++ ){
											// ポインタがない時戻る
											if( p_party[ i ]->func == NULL ) continue;
											// 死んでるやつは対象外の時
											if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
											// グループフラグＯＮ
											p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
										}
									}
									// 左下グループの時
									else{
										// 当たり判定ボックス表示
										for( i = 10 ; i < 20 ; i++ ){
											// ポインタがない時戻る
											if( p_party[ i ]->func == NULL ) continue;
											// 死んでるやつは対象外の時
											if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
											// グループフラグＯＮ
											p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL2;
										}
									}
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = TRUE;
									// ウィンドウアクション抹殺
									DeathAction( pActWnd );
									pActWnd = NULL;
									// ボタンフラグ全初期化
									ClearBattleButton();
									// コマンド番号
									BattleCmdNo = BATTLE_ITEM;
									// クリック音
									play_se( 217, 320, 240 );
									break;
									
								case ITEM_TARGET_ALLOTHERSIDE:			// 相手側全体
									// 右下グループの時
									if( BattleMyNo >= 10 ){
										// 当たり判定ボックス表示
										for( i = 0 ; i < 10 ; i++ ){
											// ポインタがない時戻る
											if( p_party[ i ]->func == NULL ) continue;
											// 死んでるやつは対象外の時
											if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
											// グループフラグＯＮ
											p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
										}
									}
									// 左下グループの時
									else{
										// 当たり判定ボックス表示
										for( i = 10 ; i < 20 ; i++ ){
											// ポインタがない時戻る
											if( p_party[ i ]->func == NULL ) continue;
											// 死んでるやつは対象外の時
											if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
											// グループフラグＯＮ
											p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL2;
										}
									}
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = TRUE;
									// ウィンドウアクション抹殺
									DeathAction( pActWnd );
									pActWnd = NULL;
									// ボタンフラグ全初期化
									ClearBattleButton();
									// コマンド番号
									BattleCmdNo = BATTLE_ITEM;
									// クリック音
									play_se( 217, 320, 240 );
									break;
								
								case ITEM_TARGET_ALL:					// 全て
								
									// 当たり判定ボックス表示
									for( i = 0 ; i < 20 ; i++ ){
										// ポインタがない時戻る
										if( p_party[ i ]->func == NULL ) continue;
										// 死んでるやつは対象外の時
										if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
										// グループフラグＯＮ
										p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
									}
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = TRUE;
									// ウィンドウアクション抹殺
									DeathAction( pActWnd );
									pActWnd = NULL;
									// ボタンフラグ全初期化
									ClearBattleButton();
									// コマンド番号
									BattleCmdNo = BATTLE_ITEM;
									// クリック音
									play_se( 217, 320, 240 );
									break;
									
								case ITEM_TARGET_NONE:					// ターゲット無し
									//
									BattleButtonOff(); // とりあえず全てＯＦＦにする
									
									battleMenuReturn = TRUE;
									// アイテム使う音
									//play_se( 212, 320, 240 );
									// アイテム使う送信
									lssproto_ID_send( sockfd, nowGx, nowGy, i, 0 );
									
									// ウィンドウ閉じる音
									play_se( 203, 320, 240 );
									
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = FALSE;
									break;
									
								case ITEM_TARGET_OTHERWITHOUTMYSELF:	// 自分以外
								
									// 当たり判定ボックス表示
									for( i = 0 ; i < 20 ; i++ ){
										// ポインタがない時戻る
										if( p_party[ i ]->func == NULL ) continue;
										// 自分のペットなら戻る
										if( i == BattleMyNo ) continue;
										// 死んでるやつは対象外の時
										if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
										// グループフラグＯＮ
										p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
									}
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = TRUE;
									// ウィンドウアクション抹殺
									DeathAction( pActWnd );
									pActWnd = NULL;
									// ボタンフラグ全初期化
									ClearBattleButton();
									// コマンド番号
									BattleCmdNo = BATTLE_ITEM;
									// クリック音
									play_se( 217, 320, 240 );
									break;
									
								case ITEM_TARGET_WITHOUTMYSELFANDPET:	// 自分とペット以外
								
									// 当たり判定ボックス表示
									for( i = 0 ; i < 20 ; i++ ){
										// ポインタがない時戻る
										if( p_party[ i ]->func == NULL ) continue;
										// 自分のペットなら戻る
										if( i == BattleMyNo + 5 ) continue;
										// 自分なら戻る
										if( i == BattleMyNo ) continue;
										// 死んでるやつは対象外の時
										if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
										// グループフラグＯＮ
										p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
									}
									// ターゲットセレクトフラグＯＮ
									battleTargetSelectFlag = TRUE;
									// ウィンドウアクション抹殺
									DeathAction( pActWnd );
									pActWnd = NULL;
									// ボタンフラグ全初期化
									ClearBattleButton();
									// コマンド番号
									BattleCmdNo = BATTLE_ITEM;
									// クリック音
									play_se( 217, 320, 240 );
									break;
								}
								break;
							}
						}
					}
				}
#endif				
				// ウィンドウある時
				if( pActWnd != NULL ){
					// 座標設定
					x = pActWnd->x + 16;
					y = pActWnd->y + 191 + 7;
					
					// アイテムループ
					for( i = 5 ; i < MAX_ITEM ; i++ ){
						// 枠内に入っていたら
						if( MakeHitBox( ItemBuffer[ i ].defX - 24, ItemBuffer[ i ].defY - 24, 
										ItemBuffer[ i ].defX + 25, ItemBuffer[ i ].defY + 23, DISP_PRIO_IME3 ) == TRUE ){
							// その欄にアイテムがあったら
							if( pc.item[ i ].useFlag == TRUE ){ 
								int color = FONT_PAL_WHITE;
									
								// 改行ポイント
								char *splitPoint = pc.item[ i ].memo;
								//char *splitPoint = "表示テストです。私の名前は太田哲生です。これからもよろしくお願いします。";
								// 装備できるかチェック
								if( pc.level < pc.item[ i ].level ) color = FONT_PAL_RED;
								// アイテムの名前表示
								StockFontBuffer( pActWnd->x + 16, pActWnd->y + 332 - 160, FONT_PRIO_FRONT, color, pc.item[ i ].name, 0 );	
								// アイテムの説明表示
								// 行数分ループ
								while( 1 ){
									// ２８文字を超えてたら分割
									if( strlen( splitPoint ) > 28 ){
										strncpy( moji, splitPoint, 28 );
										moji[ 28 ] = NULL;	// 終端記号をいれる
										// 全角の半分だった時
										if( GetStrLastByte( moji ) == 3 ){
											moji[ 27 ] = NULL;
											splitPoint += 27;
										}else{
											moji[ 28 ] = NULL;
											splitPoint += 28;
										}
										StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
									}else{
										strcpy( moji, splitPoint );
										StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 0 );
										break;
									}
								}
							}
#if 1							
							// 左ダブルクリックでアイテム使用
							if( mouse.onceState & MOUSE_LEFT_DBL_CRICK ){
								// その欄にアイテムがあったら
								if( pc.item[ i ].useFlag == TRUE ){
									// 戦闘で使えるない時
									//if( ( pc.item[ i ].field == ITEM_FIELD_ALL || pc.item[ i ].field == ITEM_FIELD_BATTLE ) && pc.level >= pc.item[ i ].level ){
									if( pc.item[ i ].useFlag != TRUE || pc.item[ i ].field == ITEM_FIELD_MAP || pc.level < pc.item[ i ].level ){
										// ＮＧ音
										play_se( 220, 320, 240 );
									}else{ // 使用可能な時
									
										BattleItemNo = i;	// アイテム番号記憶
									
										// ターゲットで分岐
										switch( pc.item[ i ].target ){
										
										case ITEM_TARGET_MYSELF:				// 自分のみ
										
											// 当たり判定ボックス表示フラグＯＮ
											p_party[ BattleMyNo ]->atr |= ACT_ATR_HIT_BOX;
											// ターゲットセレクトフラグＯＮ
											battleTargetSelectFlag = TRUE;
											// ウィンドウアクション抹殺
											DeathAction( pActWnd );
											pActWnd = NULL;
											// ボタンフラグ全初期化
											ClearBattleButton();
											// コマンド番号
											BattleCmdNo = BATTLE_ITEM;
											// クリック音
											play_se( 217, 320, 240 );
											break;
											
										case ITEM_TARGET_OTHER:					// 他の人（自分含む)
										
											// 当たり判定ボックス表示
											for( i = 0 ; i < 20 ; i++ ){
												// ポインタがない時戻る
												if( p_party[ i ]->func == NULL ) continue;
												// 死んでるやつは対象外の時
												if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
												// 当たり判定ボックス表示フラグＯＮ
												p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
											}
											// ターゲットセレクトフラグＯＮ
											battleTargetSelectFlag = TRUE;
											// ウィンドウアクション抹殺
											DeathAction( pActWnd );
											pActWnd = NULL;
											// ボタンフラグ全初期化
											ClearBattleButton();
											// コマンド番号
											BattleCmdNo = BATTLE_ITEM;
											// クリック音
											play_se( 217, 320, 240 );
											break;
											
										case ITEM_TARGET_ALLMYSIDE:				// 味方全体
										
											// 右下グループの時
											if( BattleMyNo < 10 ){
												// 当たり判定ボックス表示
												for( i = 0 ; i < 10 ; i++ ){
													// ポインタがない時戻る
													if( p_party[ i ]->func == NULL ) continue;
													// 死んでるやつは対象外の時
													if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
													// グループフラグＯＮ
													p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
												}
											}
											// 左下グループの時
											else{
												// 当たり判定ボックス表示
												for( i = 10 ; i < 20 ; i++ ){
													// ポインタがない時戻る
													if( p_party[ i ]->func == NULL ) continue;
													// 死んでるやつは対象外の時
													if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
													// グループフラグＯＮ
													p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL2;
												}
											}
											// ターゲットセレクトフラグＯＮ
											battleTargetSelectFlag = TRUE;
											// ウィンドウアクション抹殺
											DeathAction( pActWnd );
											pActWnd = NULL;
											// ボタンフラグ全初期化
											ClearBattleButton();
											// コマンド番号
											BattleCmdNo = BATTLE_ITEM;
											// クリック音
											play_se( 217, 320, 240 );
											break;
											
										case ITEM_TARGET_ALLOTHERSIDE:			// 相手側全体
											// 右下グループの時
											if( BattleMyNo >= 10 ){
												// 当たり判定ボックス表示
												for( i = 0 ; i < 10 ; i++ ){
													// ポインタがない時戻る
													if( p_party[ i ]->func == NULL ) continue;
													// 死んでるやつは対象外の時
													if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
													// グループフラグＯＮ
													p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
												}
											}
											// 左下グループの時
											else{
												// 当たり判定ボックス表示
												for( i = 10 ; i < 20 ; i++ ){
													// ポインタがない時戻る
													if( p_party[ i ]->func == NULL ) continue;
													// 死んでるやつは対象外の時
													if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
													// グループフラグＯＮ
													p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL2;
												}
											}
											// ターゲットセレクトフラグＯＮ
											battleTargetSelectFlag = TRUE;
											// ウィンドウアクション抹殺
											DeathAction( pActWnd );
											pActWnd = NULL;
											// ボタンフラグ全初期化
											ClearBattleButton();
											// コマンド番号
											BattleCmdNo = BATTLE_ITEM;
											// クリック音
											play_se( 217, 320, 240 );
											break;
										
										case ITEM_TARGET_ALL:					// 全て
										
											// 当たり判定ボックス表示
											for( i = 0 ; i < 20 ; i++ ){
												// ポインタがない時戻る
												if( p_party[ i ]->func == NULL ) continue;
												// 死んでるやつは対象外の時
												if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
												// グループフラグＯＮ
												p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
											}
											// ターゲットセレクトフラグＯＮ
											battleTargetSelectFlag = TRUE;
											// ウィンドウアクション抹殺
											DeathAction( pActWnd );
											pActWnd = NULL;
											// ボタンフラグ全初期化
											ClearBattleButton();
											// コマンド番号
											BattleCmdNo = BATTLE_ITEM;
											// クリック音
											play_se( 217, 320, 240 );
											break;
											
										case ITEM_TARGET_NONE:					// ターゲット無し
											//
											BattleButtonOff(); // とりあえず全てＯＦＦにする
											
											battleMenuReturn = TRUE;
											// アイテム使う音
											//play_se( 212, 320, 240 );
											// アイテム使う送信
											lssproto_ID_send( sockfd, nowGx, nowGy, i, 0 );
											
											// ウィンドウ閉じる音
											play_se( 203, 320, 240 );
											
											// ターゲットセレクトフラグＯＮ
											battleTargetSelectFlag = FALSE;
											break;
											
										case ITEM_TARGET_OTHERWITHOUTMYSELF:	// 自分以外
										
											// 当たり判定ボックス表示
											for( i = 0 ; i < 20 ; i++ ){
												// ポインタがない時戻る
												if( p_party[ i ]->func == NULL ) continue;
												// 自分のペットなら戻る
												if( i == BattleMyNo ) continue;
												// 死んでるやつは対象外の時
												if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
												// グループフラグＯＮ
												p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
											}
											// ターゲットセレクトフラグＯＮ
											battleTargetSelectFlag = TRUE;
											// ウィンドウアクション抹殺
											DeathAction( pActWnd );
											pActWnd = NULL;
											// ボタンフラグ全初期化
											ClearBattleButton();
											// コマンド番号
											BattleCmdNo = BATTLE_ITEM;
											// クリック音
											play_se( 217, 320, 240 );
											break;
											
										case ITEM_TARGET_WITHOUTMYSELFANDPET:	// 自分とペット以外
										
											// 当たり判定ボックス表示
											for( i = 0 ; i < 20 ; i++ ){
												// ポインタがない時戻る
												if( p_party[ i ]->func == NULL ) continue;
												// 自分のペットなら戻る
												if( i == BattleMyNo + 5 ) continue;
												// 自分なら戻る
												if( i == BattleMyNo ) continue;
												// 死んでるやつは対象外の時
												if( pc.item[ BattleItemNo ].deadTargetFlag == FALSE ) if( p_party[ i ]->hp <= 0 ) continue;
												// グループフラグＯＮ
												p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
											}
											// ターゲットセレクトフラグＯＮ
											battleTargetSelectFlag = TRUE;
											// ウィンドウアクション抹殺
											DeathAction( pActWnd );
											pActWnd = NULL;
											// ボタンフラグ全初期化
											ClearBattleButton();
											// コマンド番号
											BattleCmdNo = BATTLE_ITEM;
											// クリック音
											play_se( 217, 320, 240 );
											break;
										}
										break;
									}
								}
							}
#endif
						}
						// その欄にアイテムがあったら
						//if( pc.item[ i ].useFlag == TRUE && ItemBuffer[ i ].dragFlag == FALSE ){ 
						if( pc.item[ i ].useFlag == TRUE ){ 
							// アイテム表示
							StockDispBuffer( ItemBuffer[ i ].defX, ItemBuffer[ i ].defY, ItemBuffer[ i ].dispPrio, pc.item[ i ].graNo, 0 );
						}
					}
					// ウィンドウあったら
					if( pActWnd != NULL ){
						// 文字表示バッファに溜める
						battleMenuItemFontNo[ 0 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActWnd->pYobi )->mx, pActWnd->y + 262, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
					}
				}
			}
		}
	}
}

// ◎ペット入れ替えボタン *******************************************************/
void BattleButtonPet( void )
{
	int i, bak;
	char moji[ 256 ];
	int x, y;
	int flag = FALSE;
	int atrFlag = FALSE;
	int atrGraNo[ 4 ];
	int color;
	
	if( HitDispNo == battleButtonDispNo[ 6 ] || battleButtonBak2 == 6 ){
		// 左クリックされた時
		if( mouse.onceState & MOUSE_LEFT_CRICK || battleButtonBak2 == 6 ){
		
			bak = battleButtonFlag[ 6 ];	// バックアップ
			BattleButtonOff();				// とりあえず全てＯＦＦにする
			
			// ボタンがＯＦＦの時
			if( bak == FALSE ){
				// このボタンだけＯＮ
				battleButtonFlag[ 6 ] = TRUE;
				// 文字の当たり判定初期化
				for( i = 0 ; i < BATTLE_MENU_PET_FONTS ; i++ ) battleMenuPetFontNo[ i ] = -2;
				// ウィンドウ表示タスク作成
				//pActWnd = MakeWindowDisp( 380, 144, 4, 5, CG_WND_TITLE_SYSTEM, 1 );
				// ウィンドウ表示タスク作成
				pActWnd = MakeWindowDisp( 376, 132, 256, 320, 0, -1 );
				// 今回のボタン記憶
				battleButtonBak = 6;
				// 前回のボタン初期化
				battleButtonBak2 = -1;
			}	
		}
		// 一行インフォ
		strcpy( OneLineInfoStr, "ペットを入れ替えます。");
	}
	// ペットウィンドウ処理
	if( battleButtonFlag[ 6 ] == TRUE ){
		if( pActWnd->hp > 0 ){
			
			// ペット入れ替えウィンドウの表示
			StockDispBuffer( ( ( WINDOW_DISP *)pActWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActWnd->pYobi )->my, DISP_PRIO_MENU, CG_BTL_PET_CHANGE_WND, 1 );
			// 左クリックした時
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				// ペットの数分ループ
				for( i = 0 ; i < 5 ; i++ ){
					// この欄にペットがいる時 かつ スタメンの時
					if( pet[ i ].useFlag == TRUE && pc.selectPetNo[ i ] == TRUE ){
						// ペット選択した時
						if( HitFontNo == battleMenuPetFontNo[ i ] ){
							// ペットを参加させる
							// 死んで無い時
							if( pet[ i ].hp > 0 && i != pc.battlePetNo ){
								// スタメンにする
								//pc.selectPetNo[ i ] = FALSE;
								// 文字列作成
								sprintf( moji, "S|%d", i );
								// ペット入れ替え又は出す
								lssproto_B_send( sockfd, moji );
								//play_se( 217, 320, 240 ); // クリック音
								// ウィンドウ閉じる音
								play_se( 203, 320, 240 );
								// ウィンドウアクション抹殺
								DeathAction( pActWnd );
								// ウィンドウポインタ初期化
								pActWnd = NULL;
								// メニュー隠す
								battleMenuReturn = TRUE;
								// 今回のボタン記憶
								battleButtonBak = -1;
								// 前回のボタン初期化
								battleButtonBak2 = -1;
							}else{
								// ＮＧ音
								play_se( 220, 320, 240 );
							}
						}
					}
				}
				// 戻す時
				if( HitDispNo == battleMenuPetFontNo[ 5 ] && pc.battlePetNo != -1 ){
					// ペットを参加させない
					lssproto_B_send( sockfd, "S|-1" );
					//play_se( 217, 320, 240 ); // クリック音
					// ウィンドウ閉じる音
					play_se( 203, 320, 240 );
					// ウィンドウアクション抹殺
					DeathAction( pActWnd );
					// ウィンドウポインタ初期化
					pActWnd = NULL;
					// メニュー隠す
					battleMenuReturn = TRUE;
					// 今回のボタン記憶
					battleButtonBak = -1;
					// 前回のボタン初期化
					battleButtonBak2 = -1;
				}
				// 閉じる時
				if( HitDispNo == battleMenuPetFontNo[ 6 ] ){
					// とりあえず全てＯＦＦにする
					BattleButtonOff();				
				}
			}
			// ウィンドウがある時
			if( pActWnd != NULL ){
				// 一行インフォ
				//strcpy( OneLineInfoStr, "ペットを選択してください。");
				// 座標設定
				x = pActWnd->x + 17, y = pActWnd->y + 33;
				// ペット分ループ
				for( i = 0 ; i < 5 ; i++ ){
					// この欄にペットがいる時 かつ スタメンの時
					if( pet[ i ].useFlag == TRUE && pc.selectPetNo[ i ] == TRUE ){
						atrFlag = FALSE;	// フラグ初期化
						color = FONT_PAL_AQUA;		// デフォルト色
						if( pet[ i ].hp <= 0 ) color = FONT_PAL_RED;	// 死んでる時
						if( i == pc.battlePetNo ) color = FONT_PAL_YELLOW;	// 戦闘に参加している時
						// 名前の文字列作成
						// あだ名がある時
						if( pet[ i ].freeName[ 0 ] != NULL )
							//sprintf( moji, "%s ", pet[ i ].freeName );
							CenteringStr( pet[ i ].freeName, moji, PET_NAME_LEN );
						else
							//sprintf( moji, "%s ", pet[ i ].name );
							CenteringStr( pet[ i ].name, moji, PET_NAME_LEN );
						battleMenuPetFontNo[ i ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, color, moji, 2 );
						
						// 属性表示
						if( pet[ i ].earth > 0 ){	// 地
							// 大の時
							if( pet[ i ].earth > 50 ) atrGraNo[ atrFlag ] = CG_ATR_ICON_EARTH_BIG;
							// 小の時
							else atrGraNo[ atrFlag ] = CG_ATR_ICON_EARTH_SML;
							atrFlag++; // フラグＯＮ
						}
						if( pet[ i ].water > 0 ){	// 水
							// 大の時
							if( pet[ i ].water > 50 ) atrGraNo[ atrFlag ] = CG_ATR_ICON_WATER_BIG;
							// 小の時
							else atrGraNo[ atrFlag ] = CG_ATR_ICON_WATER_SML;
							atrFlag++; // フラグＯＮ
						}
						if( pet[ i ].fire > 0 ){	// 火
							// 大の時
							if( pet[ i ].fire > 50 ) atrGraNo[ atrFlag ] = CG_ATR_ICON_FIRE_BIG;
							// 小の時
							else atrGraNo[ atrFlag ] = CG_ATR_ICON_FIRE_SML;
							atrFlag++; // フラグＯＮ
						}
						if( pet[ i ].wind > 0 ){	// 風
							// 大の時
							if( pet[ i ].wind > 50 ) atrGraNo[ atrFlag ] = CG_ATR_ICON_WIND_BIG;
							// 小の時
							else atrGraNo[ atrFlag ] = CG_ATR_ICON_WIND_SML;
							atrFlag++; // フラグＯＮ
						}
						// 一個目
						if( atrFlag > 0 ) StockDispBuffer( pActWnd->x + 214, y + 8, DISP_PRIO_IME3, atrGraNo[ 0 ], 0 );
						// 二個目
						if( atrFlag > 1 ) StockDispBuffer( pActWnd->x + 214 + 16 , y + 8, DISP_PRIO_IME3, atrGraNo[ 1 ], 0 );
						 y += 26;
						
						// ＬＶとＨＰ
						sprintf( moji, "%3d　  　 %4d %4d", pet[ i ].level, pet[ i ].hp, pet[ i ].maxHp );
						StockFontBuffer( x + 27, y, FONT_PRIO_FRONT, color, 	moji, 0 ); y += 25;
						// 忠誠度
						sprintf( moji, "%3d", pet[ i ].ai );
						StockFontBuffer( x + 60, y, FONT_PRIO_FRONT, color, 	moji, 0 ); y += 36;
						flag = TRUE;
					}
				}
				//y += 16;
				// ペットがいる時
				if( flag == TRUE ){ 
					// ペットが参戦している時
					if( pc.battlePetNo != -1 ){ 
						battleMenuPetFontNo[ 5 ] = StockDispBuffer( pActWnd->x + 70, pActWnd->y + 299, DISP_PRIO_IME3, CG_BTL_PET_RETURN_BTN, 2 );
						// 閉じるボタン
						battleMenuPetFontNo[ 6 ] = StockDispBuffer( pActWnd->x + 187, pActWnd->y + 299, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
					}else{
						// 閉じるボタン
						battleMenuPetFontNo[ 6 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActWnd->pYobi )->mx, pActWnd->y + 299, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
					}
				}else{
					// ペットがいない時
					StockFontBuffer( x + 11, y, FONT_PRIO_FRONT, 0, "参戦するペットがいません", 0 );
					// 閉じるボタン
					battleMenuPetFontNo[ 6 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActWnd->pYobi )->mx, pActWnd->y + 299, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
				}
			}
		}
	}
}

// ◎逃げるボタン ***************************************************************/
void BattleButtonEscape( void )
{
	if( HitDispNo == battleButtonDispNo[ 7 ] && battleButtonFlag[ 7 ] == FALSE ){
		// 左クリックされた時
		if( mouse.onceState & MOUSE_LEFT_CRICK ){
			BattleButtonOff();				// とりあえず全てＯＦＦにする
			// このボタンだけＯＮ
			battleButtonFlag[ 7 ] = TRUE;
			// 逃げ送信
			lssproto_B_send( sockfd, "E" );
			// プレイヤー逃げたフラグ
			battlePlayerEscFlag = TRUE;
			// メニュー隠す
			battleMenuReturn = TRUE;
			// 今回のボタン記憶
			battleButtonBak = -1;
			// 前回のボタン初期化
			battleButtonBak2 = -1;
			// ウィンドウ閉じる音
			play_se( 203, 320, 240 );
		}
		// 一行インフォ
		strcpy( OneLineInfoStr, "逃げます。");
	}
}

// 技ヒットボックスをセットする **************************************************/
void BattleSetWazaHitBox( int no )
{
	char moji[ 256 ];
	int i;
	
	// 技番号記憶
	BattleWazaNo = no;
	// ターゲットセレクトフラグＯＮ
	battleTargetSelectFlag = TRUE;
	
	//PETSKILL_TARGET_MYSELF,				自分のみ			
	//PETSKILL_TARGET_OTHER,				他の人（自分含む)
	//PETSKILL_TARGET_ALLMYSIDE,			味方全体			
	//PETSKILL_TARGET_ALLOTHERSIDE,			相手側全体
	//PETSKILL_TARGET_ALL,					全て
	//PETSKILL_TARGET_NONE,					誰も選択出来ない。防御やための時
	//PETSKILL_TARGET_OTHERWITHOUTMYSELF,	他の人（自分含まない) 
	
	// ターゲットで分岐
	switch( petSkill[ battlePetNoBak ][ no ].target ){
	
	case PETSKILL_TARGET_MYSELF:	// ペットのみ
	
		// 当たり判定ボックス表示フラグＯＮ
		p_party[ BattleMyNo + 5 ]->atr |= ACT_ATR_HIT_BOX;
		// ターゲットセレクトフラグＯＮ
		battleTargetSelectFlag = TRUE;
		// ウィンドウアクション抹殺
		DeathAction( pActWnd );
		pActWnd = NULL;
		// ボタンフラグ全初期化
		ClearBattleButton();
		// コマンド番号
		BattleCmdNo = BATTLE_WAZA;
		// クリック音
		play_se( 217, 320, 240 );
		// 技番号バックアップ
		battleWazaTargetBak = no;
		
		break;
		
	case PETSKILL_TARGET_OTHER:	// 他の人（自分含む)
	
		// 当たり判定ボックス表示
		for( i = 0 ; i < 20 ; i++ ){
			// ポインタがない時戻る
			if( p_party[ i ]->func == NULL ) continue;
			// 死んでいる時
			if( p_party[ i ]->hp <= 0 ) continue;
			// 当たり判定ボックス表示フラグＯＮ
			p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
		}
		// ターゲットセレクトフラグＯＮ
		battleTargetSelectFlag = TRUE;
		// ウィンドウアクション抹殺
		DeathAction( pActWnd );
		pActWnd = NULL;
		// ボタンフラグ全初期化
		ClearBattleButton();
		// コマンド番号
		BattleCmdNo = BATTLE_WAZA;
		// クリック音
		play_se( 217, 320, 240 );
		// 技番号バックアップ
		battleWazaTargetBak = no;
    
	break;
		
	case PETSKILL_TARGET_ALLMYSIDE:	// 味方全体
	
		// 右下グループの時
		if( BattleMyNo < 10 ){
			// 当たり判定ボックス表示
			for( i = 0 ; i < 10 ; i++ ){
				// ポインタがない時戻る
				if( p_party[ i ]->func == NULL ) continue;
				// 死んでいる時
				if( p_party[ i ]->hp <= 0 ) continue;
				// グループフラグＯＮ
				p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
			}
		}
		// 左下グループの時
		else{
			// 当たり判定ボックス表示
			for( i = 10 ; i < 20 ; i++ ){
				// ポインタがない時戻る
				if( p_party[ i ]->func == NULL ) continue;
				// 死んでいる時
				if( p_party[ i ]->hp <= 0 ) continue;
				// グループフラグＯＮ
				p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL2;
			}
		}
		// ターゲットセレクトフラグＯＮ
		battleTargetSelectFlag = TRUE;
		// ウィンドウアクション抹殺
		DeathAction( pActWnd );
		pActWnd = NULL;
		// ボタンフラグ全初期化
		ClearBattleButton();
		// コマンド番号
		BattleCmdNo = BATTLE_WAZA;
		// クリック音
		play_se( 217, 320, 240 );
		// 技番号バックアップ
		battleWazaTargetBak = no;
		break;
		
	case PETSKILL_TARGET_ALLOTHERSIDE:	// 相手側全体
	
		// 右下グループの時
		if( BattleMyNo >= 10 ){
			// 当たり判定ボックス表示
			for( i = 0 ; i < 10 ; i++ ){
				// ポインタがない時戻る
				if( p_party[ i ]->func == NULL ) continue;
				// 死んでいる時
				if( p_party[ i ]->hp <= 0 ) continue;
				// グループフラグＯＮ
				p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
			}
		}
		// 左下グループの時
		else{
			// 当たり判定ボックス表示
			for( i = 10 ; i < 20 ; i++ ){
				// ポインタがない時戻る
				if( p_party[ i ]->func == NULL ) continue;
				// 死んでいる時
				if( p_party[ i ]->hp <= 0 ) continue;
				// グループフラグＯＮ
				p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL2;
			}
		}
		// ターゲットセレクトフラグＯＮ
		battleTargetSelectFlag = TRUE;
		// ウィンドウアクション抹殺
		DeathAction( pActWnd );
		pActWnd = NULL;
		// ボタンフラグ全初期化
		ClearBattleButton();
		// コマンド番号
		BattleCmdNo = BATTLE_WAZA;
		// クリック音
		play_se( 217, 320, 240 );
		// 技番号バックアップ
		battleWazaTargetBak = no;
		break;
		
	case PETSKILL_TARGET_ALL:	// 全て
	
		// 当たり判定ボックス表示
		for( i = 0 ; i < 20 ; i++ ){
			// ポインタがない時戻る
			if( p_party[ i ]->func == NULL ) continue;
			// 死んでいる時
			if( p_party[ i ]->hp <= 0 ) continue;
			// グループフラグＯＮ
			p_party[ i ]->atr |= ACT_ATR_HIT_BOX_ALL1;
		}
		// ターゲットセレクトフラグＯＮ
		battleTargetSelectFlag = TRUE;
		// ウィンドウアクション抹殺
		DeathAction( pActWnd );
		pActWnd = NULL;
		// ボタンフラグ全初期化
		ClearBattleButton();
		// コマンド番号
		BattleCmdNo = BATTLE_WAZA;
		// クリック音
		play_se( 217, 320, 240 );
		// 技番号バックアップ
		battleWazaTargetBak = no;
		break;
		
	case PETSKILL_TARGET_NONE:	// 誰も選択出来ない。防御など。
	
		BattleButtonOff(); // とりあえず全てＯＦＦにする
		battleMenuReturn = TRUE;
		// 文字列作成
		sprintf( moji,"W|%X|%X", BattleWazaNo, BattleMyNo + 5 );
		// 技送信
		lssproto_B_send( sockfd, moji );
	
		// ウィンドウ閉じる音
		play_se( 203, 320, 240 );
		
		// 技番号バックアップ
		battleWazaTargetBak = -1;
		// ターゲットセレクトフラグＯＮ
		battleTargetSelectFlag = FALSE;
		break;
		
	case PETSKILL_TARGET_OTHERWITHOUTMYSELF:	// 他の人（自分含まない) 
	
		// 当たり判定ボックス表示
		for( i = 0 ; i < 20 ; i++ ){
			// ポインタがない時戻る
			if( p_party[ i ]->func == NULL ) continue;
			// 自分のペットなら戻る
			if( i == BattleMyNo + 5 ) continue;
			// 死んでいる時
			if( p_party[ i ]->hp <= 0 ) continue;
			// グループフラグＯＮ
			p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
		}
		// ターゲットセレクトフラグＯＮ
		battleTargetSelectFlag = TRUE;
		// ウィンドウアクション抹殺
		DeathAction( pActWnd );
		pActWnd = NULL;
		// ボタンフラグ全初期化
		ClearBattleButton();
		// コマンド番号
		BattleCmdNo = BATTLE_WAZA;
		// クリック音
		play_se( 217, 320, 240 );
		// 技番号バックアップ
		battleWazaTargetBak = no;
		break;
		
	case PETSKILL_TARGET_WITHOUTMYSELFANDPET:	// 自分とペット以外 
	
		// 当たり判定ボックス表示
		for( i = 0 ; i < 20 ; i++ ){
			// ポインタがない時戻る
			if( p_party[ i ]->func == NULL ) continue;
			// 自分のペットなら戻る
			if( i == BattleMyNo + 5 ) continue;
			// 自分なら戻る
			if( i == BattleMyNo ) continue;
			// 死んでいる時
			if( p_party[ i ]->hp <= 0 ) continue;
			// グループフラグＯＮ
			p_party[ i ]->atr |= ACT_ATR_HIT_BOX;
		}
		// ターゲットセレクトフラグＯＮ
		battleTargetSelectFlag = TRUE;
		// ウィンドウアクション抹殺
		DeathAction( pActWnd );
		pActWnd = NULL;
		// ボタンフラグ全初期化
		ClearBattleButton();
		// コマンド番号
		BattleCmdNo = BATTLE_WAZA;
		// クリック音
		play_se( 217, 320, 240 );
		// 技番号バックアップ
		battleWazaTargetBak = no;
		break;
	}
}

// ◎技ボタン *****************************************************************/
void BattleButtonWaza( void )
{
	int i, bak;
	int x, y;
	char moji[ 256 ];
	
	if( HitDispNo == battleButtonDispNo[ 8 ] || battlePetButtonFlag == TRUE ){
		// 左クリックされた時
		if( mouse.onceState & MOUSE_LEFT_CRICK || battlePetButtonFlag == TRUE ){
	//if( HitDispNo == battleButtonDispNo[ 8 ] || battleWazaTargetBak == -1 ){
		// 左クリックされた時
	//	if( mouse.onceState & MOUSE_LEFT_CRICK || battleWazaTargetBak == -1 ){
		
			bak = battleButtonFlag[ 8 ];	// バックアップ
			BattleButtonOff();				// とりあえず全てＯＦＦにする
		
			// ボタンがＯＦＦの時
			if( bak == FALSE ){
			
				// このボタンだけＯＮ
				battleButtonFlag[ 8 ] = TRUE;
				// バックアップ初期化
				battleWazaTargetBak = -1;
				// コマンド番号
				//BattleCmdNo = BATTLE_WAZA;
				// 文字の当たり判定初期化
				for( i = 0 ; i < BATTLE_MENU_WAZA_FONTS ; i++ ) battleMenuWazaFontNo[ i ] = -2;
				// ウィンドウ表示タスク作成
				//pActWnd = MakeWindowDisp( 380, 120, 4, 7, CG_WND_TITLE_SYSTEM, 1 );
				// ウィンドウ表示タスク作成
				//pActWnd = MakeWindowDisp( 361, 144, 272, 280, 0, -1 );
				// ウィンドウ表示タスク作成
				//pActWnd = MakeWindowDisp( 361, 144, 271, 281, 0, -1 );
				// ウィンドウ表示タスク作成
				//pActWnd = MakeWindowDisp( 376, 100, 4, 7, CG_WND_TITLE_SYSTEM, 1 );
				pActWnd = MakeWindowDisp( 364, 96, 272, 348, 0, -1 );
			}
		}
		// ペットボタン初期化
		battlePetButtonFlag = FALSE;
		// 一行インフォ				 
		strcpy( OneLineInfoStr, "技を使います。");
	}
	
	// 技ウィンドウ処理
	if( battleButtonFlag[ 8 ] == TRUE ){
		// ウィンドウがある時
		if( pActWnd != NULL ){
			// ウィンドウが出来あがっていたら
			if( pActWnd->hp > 0 ){
				// ペット技ウィンドウ
				StockDispBuffer( ( ( WINDOW_DISP *)pActWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActWnd->pYobi )->my, DISP_PRIO_MENU, CG_PET_WAZA_WND, 1 );
				// 持てる技の数分ループ
				for( i = 0 ; i < pet[ battlePetNoBak ].maxSkill ; i++ ){
				//for( i = 0 ; i < 7 ; i++ ){
					// 技ベースを表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActWnd->pYobi )->my, DISP_PRIO_IME3, CG_PET_WAZA_BAR_1 + i, 1 );
				}
				// 左クリックした時
				if( mouse.onceState & MOUSE_LEFT_CRICK ){
					// ウィンドウを閉じる
					if( HitDispNo == battleMenuWazaFontNo[ 7 ] ){
						// とりあえず全てＯＦＦにする
						BattleButtonOff();				
						// ウィンドウ抹殺
						//DeathAction( pActWnd );
						//pActWnd = NULL;
						// ウィンドウ閉じる音
						//play_se( 203, 320, 240 );
					}
				}
				
				// ウィンドウある時
				if( pActWnd != NULL ){
					
					// 座標設定
					x = pActWnd->x + 32;
					y = pActWnd->y + 252;
					
					// 説明欄
					for( i = 0 ; i < pet[ battlePetNoBak ].maxSkill ; i++ ){
					//for( i = 0 ; i < 7 ; i++ ){
						// その欄に技ある時
						if( petSkill[ battlePetNoBak ][ i ].useFlag == TRUE ){
							if( HitFontNo == battleMenuWazaFontNo[ i ] ){
								// 改行ポイント
								char *splitPoint = petSkill[ battlePetNoBak ][ i ].memo;
								//char *splitPoint = "表示テストです。私の名前は太田哲生です。これからもよろしくお願いします。";
								// 行数分ループ
								while( 1 ){
									// ２４文字を超えてたら分割
									if( strlen( splitPoint ) > 24 ){
										strncpy( moji, splitPoint, 24 );
										moji[ 24 ] = NULL;	// 終端記号をいれる
										// 全角の半分だった時
										if( GetStrLastByte( moji ) == 3 ){
											moji[ 23 ] = NULL;
											splitPoint += 23;
										}else{
											moji[ 24 ] = NULL;
											splitPoint += 24;
										}
										StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 20;
									}else{
										strcpy( moji, splitPoint );
										StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 0 );
										break;
									}
								}
								// 左クリックした時
								if( mouse.onceState & MOUSE_LEFT_CRICK ){
									// 戦闘で使える時
									if(	petSkill[ battlePetNoBak ][ i ].field != PETSKILL_FIELD_MAP ){
										// 技ヒットボックスをセットする
										BattleSetWazaHitBox( i );
									}else{
										int flag = 0;
										int j;
										
										// 戦闘で使えない技しか持ってないかチェック
										for( j = 0 ; j < pet[ battlePetNoBak ].maxSkill ; j++ ){
											// その欄に技ある時
											if( petSkill[ battlePetNoBak ][ j ].useFlag == TRUE ){
												// 戦闘で使える時
												if(	petSkill[ battlePetNoBak ][ j ].field != PETSKILL_FIELD_MAP ) flag++;
											}
										}
										// 戦闘で使える技を持ってる時
										if( flag > 0 ){
											// ＮＧ音
											play_se( 220, 320, 240 );
										}
										// 持ってない時
										else{
											BattleButtonOff(); // とりあえず全てＯＦＦにする
											battleMenuReturn = TRUE;
											// ペットは何もしないを送信
											lssproto_B_send( sockfd, "W|FF|FF" );
	    					
											// ウィンドウ閉じる音
											play_se( 203, 320, 240 );
											
											// 技番号バックアップ
											battleWazaTargetBak = -1;
											// ターゲットセレクトフラグＯＮ
											battleTargetSelectFlag = FALSE;
										}
									}
								}
							}
						}
					}
					
					// ウィンドウがある時
					if( pActWnd != NULL ){
						// 座標設定
						x = pActWnd->x + 40;
						y = pActWnd->y + 32;
						
						// あだ名がある時
						if( pet[ battlePetNoBak ].freeName[ 0 ] != NULL )
							//sprintf( moji,"%s", pet[ battlePetNoBak ].freeName );
							CenteringStr( pet[ battlePetNoBak ].freeName, moji, PET_NAME_LEN );
						else
							//sprintf( moji,"%s", pet[ battlePetNoBak ].name );
							CenteringStr( pet[ battlePetNoBak ].name, moji, PET_NAME_LEN );
							StockFontBuffer( x - 28, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 26; x += 18;
						
						// 技表示
						for( i = 0 ; i < pet[ battlePetNoBak ].maxSkill ; i++ ){
						//for( i = 0 ; i < 7 ; i++ ){
							// その欄に技ある時
							if( petSkill[ battlePetNoBak ][ i ].useFlag == TRUE ){
								int color = FONT_PAL_GRAY;
								sprintf( moji,"　　 %-22s",petSkill[ battlePetNoBak ][ i ].name );
							//	sprintf( moji,"ペットの技です。ああああ" );
								
								// 戦闘で使える時
								if(	petSkill[ battlePetNoBak ][ i ].field != PETSKILL_FIELD_MAP ) color = FONT_PAL_WHITE;
								battleMenuWazaFontNo[ i ] = StockFontBuffer( x - 43, y, FONT_PRIO_FRONT, color, moji, 2 );
							}
							y += 25;
						}
						
						// 座標設定
						y = pActWnd->y + 330;
						
						// 文字表示バッファに溜める
						battleMenuWazaFontNo[ 7 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActWnd->pYobi )->mx, y, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
						//battleMenuWazaFontNo[ 7 ] = StockFontBuffer( x + 100, y, FONT_PRIO_FRONT, FONT_PAL_AQUA, 	"　閉 じ る　", 2 );
					}
				}
			}
		}
	}
}

// ターゲットセレクト *********************************************************/
void BattleTargetSelect( void )
{
	char moji[ 256 ];
	int targetNo;
	int no;
	
	// システムメニュー出てない時
	if( !( MenuToggleFlag & JOY_ESC ) ){
		// インフォメーションウィンドウない時
		if( pActInfoWnd == NULL ){
			// マウスが画面下半分にあったら
			if( mouse.nowPoint.y < 228 ){ 
				// ウィンドウ表示タスク作成
				pActInfoWnd = MakeWindowDisp( 210, 356, 3, 2, 0, 1 );
			}else{ 
				// ウィンドウ表示タスク作成
				pActInfoWnd = MakeWindowDisp( 210, 4, 3, 2, 0, 1 );
			}
		}else{	
			// ウィンドウ出来あがっていたら
			if( pActInfoWnd->hp > 0 ){
				// 文字表示バッファに溜める
				StockFontBuffer( pActInfoWnd->x + 38, pActInfoWnd->y + 28, FONT_PRIO_FRONT, FONT_PAL_YELLOW, 	"ターゲットを", 0 );
				StockFontBuffer( pActInfoWnd->x + 38, pActInfoWnd->y + 52, FONT_PRIO_FRONT, FONT_PAL_YELLOW, 	"選択して下さい", 0 );
			}
			// マウスが画面下半分にあったら
			if( mouse.nowPoint.y > 300 ) pActInfoWnd->y = 4;
			if( mouse.nowPoint.y < 156 ) pActInfoWnd->y = 356;
		}
	}
		
	// 左クリックされた時
	if( mouse.onceState & MOUSE_LEFT_CRICK ){
		// ヒットチェック
		if( ( targetNo = CheckBattelTarget() ) != -1 ){
			// 攻撃の種類で分岐
			switch( BattleCmdNo ){
			
			case BATTLE_ATTACK:		// 通常攻撃
			
				// 文字列作成
				sprintf( moji, "H|%X", targetNo );
				// 通常攻撃
				lssproto_B_send( sockfd, moji );

				break;
			
			case BATTLE_CAPTURE:		// 捕獲
			
				// 文字列作成
				sprintf( moji, "T|%X", targetNo );
				// 捕獲送信
				lssproto_B_send( sockfd, moji );
				
				break;
				
			case BATTLE_JUJUTU:			// 呪術
				
				// ターゲットで分岐
				switch( magic[ BattleJujutuNo ].target ){
				
				case MAGIC_TARGET_MYSELF:	// 自分のみ
				case MAGIC_TARGET_OTHER:	// 他の人（自分含む)
				
					// 文字列作成
					sprintf( moji,"J|%X|%X", BattleJujutuNo, targetNo );
					// 呪術送信
					lssproto_B_send( sockfd, moji );
					// クリック音
					play_se( 217, 320, 240 );

					break;
					
				case MAGIC_TARGET_ALLMYSIDE:	// 味方全体
				
					// 右下グループの時
					if( BattleMyNo < 10 ) no = 20;
					else no = 21;
					// 文字列作成
					sprintf( moji,"J|%X|%X", BattleJujutuNo, no );
					// 呪術送信
					lssproto_B_send( sockfd, moji );
					// クリック音
					play_se( 217, 320, 240 );
					
					break;
					
				case MAGIC_TARGET_ALLOTHERSIDE:	// 相手側全体
				
					// 右下グループの時
					if( BattleMyNo < 10 ) no = 21;
					else no = 20;
					// 文字列作成
					sprintf( moji,"J|%X|%X", BattleJujutuNo, no );
					// 呪術送信
					lssproto_B_send( sockfd, moji );
					// クリック音
					play_se( 217, 320, 240 );
					
					break;
					
				case MAGIC_TARGET_ALL:	// 全て
				
					// 文字列作成
					sprintf( moji,"J|%X|%X", BattleJujutuNo, 22 );
					// 呪術送信
					lssproto_B_send( sockfd, moji );
					// クリック音
					play_se( 217, 320, 240 );
					
					break;
					
				case MAGIC_TARGET_WHOLEOTHERSIDE:    // どちらかサイド全体
				
					// 右下グループの時
					if( targetNo < 10 ) no = 20;
					else no = 21;
					// 文字列作成
					sprintf( moji,"J|%X|%X", BattleJujutuNo, no );
					// 呪術送信
					lssproto_B_send( sockfd, moji );
					// クリック音
					play_se( 217, 320, 240 );
					
					break;
				}
				
				break;
				
			case BATTLE_ITEM:			// アイテム
				
				// ターゲットで分岐
				switch( pc.item[ BattleItemNo ].target ){
				
				case ITEM_TARGET_MYSELF:				// 自分のみ
				case ITEM_TARGET_OTHER:					// 他の人（自分含む)
				case ITEM_TARGET_OTHERWITHOUTMYSELF:	// 他の人（自分含まない) 
				case ITEM_TARGET_WITHOUTMYSELFANDPET:	// 自分とペット以外
				
					// 文字列作成
					sprintf( moji,"I|%X|%X", BattleItemNo, targetNo );
					//sprintf( moji,"H|%X", targetNo );
					// アイテム使う送信
					lssproto_B_send( sockfd, moji );
				
					// クリック音
					play_se( 217, 320, 240 );

					break;
					
				case ITEM_TARGET_ALLMYSIDE:	// 味方全体
				
					// 右下グループの時
					if( BattleMyNo < 10 ) no = 20;
					else no = 21;
					// 文字列作成
					sprintf( moji,"I|%X|%X", BattleItemNo, no );
					// アイテム使う送信
					lssproto_B_send( sockfd, moji );
					// クリック音
					play_se( 217, 320, 240 );
					
					break;
					
				case ITEM_TARGET_ALLOTHERSIDE:	// 相手側全体
				
					// 右下グループの時
					if( BattleMyNo < 10 ) no = 21;
					else no = 20;
					// 文字列作成
					sprintf( moji,"I|%X|%X", BattleItemNo, no );
					// アイテム使う送信
					lssproto_B_send( sockfd, moji );
					// クリック音
					play_se( 217, 320, 240 );
					
					break;
					
				case ITEM_TARGET_ALL:	// 全て
				
					// 文字列作成
					sprintf( moji,"I|%X|%X", BattleItemNo, 22 );
					// アイテム使う送信
					lssproto_B_send( sockfd, moji );
					// クリック音
					play_se( 217, 320, 240 );
					
					break;
				}
				
				break;
				
			case BATTLE_WAZA:			// ペットの技
				
				// ターゲットで分岐
				switch( petSkill[ battlePetNoBak ][ BattleWazaNo ].target ){
				
				case PETSKILL_TARGET_MYSELF:				// 自分のみ
				case PETSKILL_TARGET_OTHER:					// 他の人（自分含む)
				case PETSKILL_TARGET_OTHERWITHOUTMYSELF:	// 他の人（自分含まない) 
				case PETSKILL_TARGET_WITHOUTMYSELFANDPET:	// 自分とペット以外
					// 文字列作成
					sprintf( moji,"W|%X|%X", BattleWazaNo, targetNo );
					// 技送信
					lssproto_B_send( sockfd, moji );
				
					// クリック音
					play_se( 217, 320, 240 );

					break;
					
				case PETSKILL_TARGET_ALLMYSIDE:	// 味方全体
				
					// 右下グループの時
					if( BattleMyNo < 10 ) no = 20;
					else no = 21;
					// 文字列作成
					sprintf( moji,"W|%X|%X", BattleWazaNo, no );
					// 技送信
					lssproto_B_send( sockfd, moji );
					// クリック音
					play_se( 217, 320, 240 );
					
					break;
					
				case PETSKILL_TARGET_ALLOTHERSIDE:	// 相手側全体
				
					// 右下グループの時
					if( BattleMyNo < 10 ) no = 21;
					else no = 20;
					// 文字列作成
					sprintf( moji,"W|%X|%X", BattleWazaNo, no );
					// 技送信
					lssproto_B_send( sockfd, moji );
					// クリック音
					play_se( 217, 320, 240 );
					
					break;
					
				case PETSKILL_TARGET_ALL:	// 全て
				
					// 文字列作成
					sprintf( moji,"W|%X|%X", BattleWazaNo, 22 );
					// 技送信
					lssproto_B_send( sockfd, moji );
					// クリック音
					play_se( 217, 320, 240 );
					
					break;
				}
				
				break;
			}
			// ウィンドウ閉じる音
			play_se( 203, 320, 240 );
			// フラグ初期化
			battleTargetSelectFlag = FALSE;
			//battleMenuFlag = FALSE;
			// メニュー隠す
			battleMenuReturn = TRUE;
			// インフォメーションウィンドウ抹殺
			DeathAction( pActInfoWnd );
			pActInfoWnd = NULL;
		}
	}
}

// カウントダウン処理 **********************************************************/
void BattleCntDownDisp( void )
{
	char moji[ 16 ];
	char *work = moji;
	int x = 320 - 16, y = 240, i;
	// 残り時間
	int BattleCntDownRest = BattleCntDown - GetTickCount();
	
	// フラグチェック
	if( BattleCntDownFlag == FALSE ) return;
	
	// タイムオーバーの時
	if( BattleCntDownRest <= 0 ){
		// フラグ初期化
		BattleCntDownFlag = FALSE;
		BattleCntDownRest = 0;
		// 当たり判定ボックス消す
		ClearBoxFlag();
		// ウィンドウアクション抹殺
		DeathAction( pActWnd );
		// ウィンドウポインタ初期化
		pActWnd = NULL;				
		// ボタンフラグ全初期化
		ClearBattleButton();
		// このボタンだけＯＮ
		//battleButtonFlag[ 4 ] = TRUE;

		// プレイヤーの行動選択中の時、両方何も出来ない
		if( battleMenuFlag & BATTLE_MENU && battleMenuReturn == FALSE ){
			// プレイヤー何もしない送信
			lssproto_B_send( sockfd, "N" );
			
			// 参戦してない時、又はペット死んでいる時
			if( battlePetNoBak == -1 || 
				p_party[ BattleMyNo + 5 ]->hp <= 0 ){
				// 何も送らない
			}else{
				// ペット何もしないを送信
				lssproto_B_send( sockfd, "W|FF|FF" );
			}
		}else{
			// ペットの行動選択中の時
			if( ( battleMenuFlag & BATTLE_MENU && battleMenuReturn == TRUE ) ||
				( battleMenuFlag & BATTLE_MENU_PET && battleMenuReturn == FALSE ) ){ 
				// 参戦してない時、又はペット死んでいる時
				if( battlePetNoBak == -1 || 
					p_party[ BattleMyNo + 5 ]->hp <= 0 ){
					// 何も送らない
				}else{
					// ペット何もしないを送信
					lssproto_B_send( sockfd, "W|FF|FF" );
				}
			}
		}
		
		// フラグ初期化
		battleTargetSelectFlag = FALSE;
		battleMenuReturn = TRUE;
		//battleMenuFlag = FALSE;
		// ウィンドウ閉じる音
		play_se( 203, 320, 240 );
		// 今回のボタン記憶
		battleButtonBak = battleButtonBak2;
		// 前回の技番号初期化
		//battleWazaTargetBak = -1
		// インフォメーションウィンドウ抹殺
		DeathAction( pActInfoWnd );
		pActInfoWnd = NULL;
	}
	// 文字列に変換（二桁表示）
	sprintf( moji, "%2d", BattleCntDownRest / 1000 );
	
	// 文字の数だけループ
	for( i = 0 ; i < 2 ; i++ ){
		// 空白でない時
		if( *work != 0x20 ){
			// 表示
			StockDispBuffer( x, y, DISP_PRIO_IME1, *work - '0' + CG_CNT_DOWN_0, 0 );
		}
		// Ｘ座標移動
		x += 32;
		// ポインタ進める
		work++;
	}
}

// バトルメニュー処理 *********************************************************/
void BattleMenuProc( void )
{
	char moji[ 256 ];
	int i;
	
	// 観戦モードの時
	if( BattleMyNo >= 20 ){
		// プレイヤー何もしないを送信
		lssproto_B_send( sockfd, "N" );
		// コマンド入力済みフラグの初期化
		InitBattleAnimFlag();
		// ペット何もしないを送信
		//lssproto_B_send( sockfd, "W|FF|FF" );
		// 次のプロセスへ
		SubProcNo++;
		
		return;	// 強制的に戻る
	}
	
	// 右下グループの時
	if( BattleMyNo < 10 ){
		for( i  = 0 ; i < 10 ; i++ ) HpMeterDisp( i ); // 耐久力メーター表示
	}else 
	// 右下グループの時
	if( BattleMyNo >= 10 ){ 
		for( i = 10 ; i < 20 ; i++ ) HpMeterDisp( i ); // 耐久力メーター表示
	}
	// プレイヤーかペットが行動できる時
	if( p_party[ BattleMyNo ]->hp > 0 && 
		!( BattleBpFlag & BATTLE_BP_PLAYER_MENU_NON && BattleBpFlag & BATTLE_BP_PET_MENU_NON ) ){
		
		// ターゲットセレクトフラグＯＮの時
		if( battleTargetSelectFlag ){ 
			BattleTargetSelect();
			// システムメニュー出ていたら抹殺
			if( MenuToggleFlag & JOY_ESC ){
				// インフォメーションウィンドウ抹殺
				DeathAction( pActInfoWnd );
				pActInfoWnd = NULL;
			}
		}else{
			// インフォメーションウィンドウ抹殺
			DeathAction( pActInfoWnd );
			pActInfoWnd = NULL;
		}
		
		// プレイヤーの上に Player 表示
		//if( p_party[ BattleMyNo ]->func != NULL ){
		
#if 0
#if 0
		// 自分の耐久力メーター表示
		HpMeterDisp( BattleMyNo );
		// ペットの耐久力メーター表示
		HpMeterDisp( BattleMyNo + 5 );
#else
		// 人数分ループ
		for( i = 0 ; i < 20 ; i++ ){
			// 耐久力メーター表示
			HpMeterDisp( i );
		}
#endif
#endif
	}
	
	// プレイヤーメニュー出せと言われた時
	if( battleMenuFlag2 == TRUE ){
		
		buttonX = 815;  // 初期Ｘ座標
		buttonY = 65;	// 初期Ｙ座標
		buttonA = 25;	// 初期加速度

		// コマンド入力済みフラグの初期化
		InitBattleAnimFlag();
		// コマンド入力済みフラグのチェック
		//CheckBattleAnimFlag();
		// プレイヤー逃げたフラグ
		battlePlayerEscFlag = FALSE;

		// カウントダウンフラグ初期化
		BattleCntDownFlag = TRUE;
		// コマンド受付時間初期化
		BattleCntDown = GetTickCount() + BATTLE_CNT_DOWN_TIME;
		
		// ペットが入れ替わってなかったら
		if( pc.battlePetNo == battlePetNoBak ){
			// 技ターゲット番号あったら
			if( battleWazaTargetBak != -1 ){
				// 自動ボタン凸
				battlePetButtonFlag = FALSE;
			}else{
				// 自動ボタン凹（防御とか選んだ時）
				battlePetButtonFlag = TRUE;
			}
		}else{
			// 自動ボタン凹
			battlePetButtonFlag = TRUE;
			// バックアップ初期化
			battleWazaTargetBak = -1;
		}
		
		// 自分が死んでいる時
		if( p_party[ BattleMyNo ]->hp <= 0 ||
			// プレイヤーメニューなしの時はコマンド入力無し
			BattleBpFlag & BATTLE_BP_PLAYER_MENU_NON ){ 
			
			// バトルメニューフラグＯＦＦ	
			//battleMenuFlag &= ~BATTLE_MENU;
			
			// プレイヤーメニューなしの時はコマンド入力無し
			if( BattleBpFlag & BATTLE_BP_PLAYER_MENU_NON ){ 
				// プレイヤー何もしないを送信
				lssproto_B_send( sockfd, "N" );
			}
			// 行動終了フラグ立てる
			p_party[ BattleMyNo ]->atr |= ACT_ATR_BTL_CMD_END;
			// ペットメニューへ
			battlePetMenuFlag = TRUE;
			
		}else{
		
			// バトルメニューフラグＯＮ	
			battleMenuFlag |= BATTLE_MENU; 
			// 初期化
			battleMenuReturn = FALSE; 
			// タイムアップフラグ初期化
			battleTimeUpFlag = FALSE;
			// ウィンドウ開く音
			play_se( 202, 320, 240 );
			// 文字の当たり判定初期化
			for( i = 0 ; i < BATTLE_BUTTON_SUU ; i++ ) battleButtonDispNo[ i ] = -2;
			// 当たり判定ボックス消す
			ClearBoxFlag();
			// 前回のボタン記憶
			battleButtonBak2 = battleButtonBak;
		}
		// 参戦ペット番号バックアップ（プレイヤーの行動で参戦番号が変わるためバックアップ取る）
		battlePetNoBak = pc.battlePetNo;
		battlePetNoBak2 = pc.battlePetNo;
		// フラグＯＦＦ
		battleMenuFlag2 = FALSE;
		// フラグ初期化
		BattleBpFlag &= ~BATTLE_BP_PLAYER_MENU_NON;
	}
	
	// プレイヤーメニュー *************************************************************/
	if( battleMenuFlag & BATTLE_MENU ){ 
		// メニュー戻らない時
		if( battleMenuReturn == FALSE ){
			
			// ボタン出現
			if( buttonA > 0 ){
				buttonA--;	// 加速度プラス
				buttonX -= buttonA; // 移動処理
			}
			// ボタン処理
			BattleButtonAttack();	// 攻撃ボタン
			BattleButtonJujutsu();	// 呪術ボタン
			BattleButtonCapture();	// 捕獲ボタン
			BattleButtonHelp();		// ＨＥＬＰボタン
			BattleButtonGuard();	// 防御ボタン
			BattleButtonItem();		// アイテムボタン
			BattleButtonPet();		// ペット入れ替えボタン
			BattleButtonEscape();	// 逃げる
			
			// カウントダウン表示とリミットチェック
			//BattleCntDownDisp();
#if 0			
#ifdef _DEBUG
			// 汎用ボタン
			if( joy_trg[ 1 ] & JOY_F12 ){ 
				BattleButtonOff();				// とりあえず全てＯＦＦにする
				// 防御送信
				lssproto_B_send( sockfd, "@" );
				// フラグ初期化
				battleMenuFlag = FALSE;
				// 今回のボタン記憶
				battleButtonBak = -1;
				// 前回のボタン初期化
				battleButtonBak2 = -1;
				// ウィンドウ閉じる音
				play_se( 203, 320, 240 );
			}
#endif
#endif

			// 右クリックされた時バトルボタンをすべてＯＦＦにする
			if( mouse.onceState & MOUSE_RIGHT_CRICK ) BattleButtonOff();
			
		}else{	// ボタン画面外へ
			if( buttonA <= 25 ){
				buttonA++;	// 加速度プラス
				buttonX += buttonA; // 移動処理
			}else{
				// 画面外に消えたら
				// ボックスフラグ消す
				ClearBoxFlag();
				// フラグ初期化
				battleTargetSelectFlag = FALSE;
				battleMenuFlag = FALSE;
				
				// 時間切れでない時
				if( BattleCntDownFlag == TRUE ){
					// ペットメニューＯＮ
					battlePetMenuFlag = TRUE;
				}else{
					// 次のプロセスへ
					SubProcNo++;
				}
					
				// コマンド番号初期化
				BattleCmdNo = -1;
				// ボタンフラグ全初期化
				ClearBattleButton();
				// 行動終了フラグ立てる
				//p_party[ BattleMyNo ]->atr |= ACT_ATR_BTL_CMD_END;
				// ペットウィンドウＯＮ
				//battleMenuFlag |= BATTLE_MENU_PET;
				// 次のプロセスへ
				//SubProcNo++;
			}
		}
		// ボタンを表示バッファに溜める
		battleButtonDispNo[ 0 ] = StockDispBuffer( buttonX, buttonY, DISP_PRIO_IME3, CG_BTL_BUTTON_ATTACK_UP 	+ battleButtonFlag[ 0 ], 2 );
		battleButtonDispNo[ 1 ] = StockDispBuffer( buttonX, buttonY, DISP_PRIO_IME3, CG_BTL_BUTTON_JUJUTU_UP 	+ battleButtonFlag[ 1 ], 2 );
		battleButtonDispNo[ 2 ] = StockDispBuffer( buttonX, buttonY, DISP_PRIO_IME3, CG_BTL_BUTTON_CAPTURE_UP 	+ battleButtonFlag[ 2 ], 2 );
		battleButtonDispNo[ 3 ] = StockDispBuffer( buttonX, buttonY, DISP_PRIO_IME3, CG_BTL_BUTTON_HELP_UP 		+ helpFlag, 2 );
		battleButtonDispNo[ 4 ] = StockDispBuffer( buttonX, buttonY, DISP_PRIO_IME3, CG_BTL_BUTTON_GUARD_UP 	+ battleButtonFlag[ 4 ], 2 );
		battleButtonDispNo[ 5 ] = StockDispBuffer( buttonX, buttonY, DISP_PRIO_IME3, CG_BTL_BUTTON_ITEM_UP 		+ battleButtonFlag[ 5 ], 2 );
		battleButtonDispNo[ 6 ] = StockDispBuffer( buttonX, buttonY, DISP_PRIO_IME3, CG_BTL_BUTTON_PET_UP 		+ battleButtonFlag[ 6 ], 2 );
		battleButtonDispNo[ 7 ] = StockDispBuffer( buttonX, buttonY, DISP_PRIO_IME3, CG_BTL_BUTTON_ESCAPE_UP 	+ battleButtonFlag[ 7 ], 2 );
		StockDispBuffer( buttonX, buttonY, DISP_PRIO_MENU, CG_BTL_BUTTON_BASE, 0 );
		// ペットがいっぱいの時×を表示
		if( CheckPetSuu() >= 5 ){
			StockDispBuffer( buttonX + 28, buttonY - 18, DISP_PRIO_IME4, CG_BTL_BUTTON_CROSS, 0 );
		}
		// ヘルプなしの時×を表示
		if( NoHelpFlag == TRUE ){
			StockDispBuffer( buttonX + 28 + 54, buttonY - 18, DISP_PRIO_IME4, CG_BTL_BUTTON_CROSS, 0 );
		}
	}
	
	// ペットメニュー出せと言われた時
	if( battlePetMenuFlag == TRUE ){
		
		buttonX = 815;  // 初期Ｘ座標
		buttonY = 48;	// 初期Ｙ座標
		buttonA = 25;	// 初期加速度

		// 参戦してない時、又はペット死んでいる時
		if( battlePetNoBak <= -1 || 
			p_party[ BattleMyNo + 5 ]->hp <= 0 ){
			
			// カウントダウンフラグＯＦＦ
			BattleCntDownFlag = FALSE;
			
			// ペットが入れ替わっていたら
			//if( pc.battlePetNo != battlePetNoBak ){
				// 自動ボタン凹
			//	battlePetButtonFlag = TRUE;
				// バックアップ初期化
			//	battleWazaTargetBak = -1;
			//}
			
			// 次のプロセスへ
			SubProcNo++;
		}else
		// メニュー出さない時、プレイヤー死んでる時
		if( BattleBpFlag & BATTLE_BP_PET_MENU_NON || 
			//BattleBpFlag & BATTLE_BP_PLAYER_MENU_NON || 
			p_party[ BattleMyNo ]->hp <= 0 || 
			battlePlayerEscFlag == TRUE ){
			
			// ペット何もしないを送信
			lssproto_B_send( sockfd, "W|FF|FF" );
			// カウントダウンフラグＯＦＦ
			BattleCntDownFlag = FALSE;
			// ペットが入れ替わっていたら
			//if( pc.battlePetNo != battlePetNoBak ){
				// 自動ボタン凹
			//	battlePetButtonFlag = TRUE;
				// バックアップ初期化
			//	battleWazaTargetBak = -1;
			//}
			// 次のプロセスへ
			SubProcNo++;
		}else{
			// ペットメニューフラグＯＮ	
			battleMenuFlag |= BATTLE_MENU_PET;
			// 初期化
			battleMenuReturn = FALSE; 
			// 自動ボタンへっこみ
			//battlePetButtonFlag = TRUE;

			// ウィンドウ開く音
			play_se( 202, 320, 240 );
			// 文字の当たり判定初期化
			//for( i = 0 ; i < BATTLE_BUTTON_SUU ; i++ ) battleButtonDispNo[ i ] = -2;
			// 当たり判定ボックス消す
			ClearBoxFlag();
	
			// 技番号がある時
			if( battleWazaTargetBak != -1 ){
				// 技ヒットボックスをセットする
				BattleSetWazaHitBox( BattleWazaNo );
			}
	
		}
		// フラグ初期化
		//BattleBpFlag &= ~BATTLE_BP_PLAYER_MENU_NON;
		BattleBpFlag &= ~BATTLE_BP_PET_MENU_NON;
		// フラグＯＦＦ
		battlePetMenuFlag = FALSE;
	}
	
	// ペットメニュー *************************************************************/
	if( battleMenuFlag & BATTLE_MENU_PET ){ 
		
		// メニュー戻らない時
		if( battleMenuReturn == FALSE ){
			
			// ボタン出現
			if( buttonA > 0 ){
				buttonA--;	// 加速度プラス
				buttonX -= buttonA; // 移動処理
			}
			// ボタン処理
			BattleButtonWaza();	// 技ボタン
			// 汎用ボタン
			//if( joy_trg[ 1 ] & JOY_F12 ){ 
			//	BattleButtonOff();				// とりあえず全てＯＦＦにする
				// 防御送信
			//	lssproto_B_send( sockfd, "@" );
				// フラグ初期化
			//	battleMenuFlag = FALSE;
				// 今回のボタン記憶
			//	battleButtonBak = -1;
				// 前回のボタン初期化
			//	battleButtonBak2 = -1;
				// ウィンドウ閉じる音
			//	play_se( 203, 320, 240 );
			//}
			// 右クリックされた時バトルボタンをすべてＯＦＦにする
			//if( mouse.onceState & MOUSE_RIGHT_CRICK ) BattleButtonOff();
			
			// 右クリックされた時バトルボタンをすべてＯＦＦにする
			if( mouse.onceState & MOUSE_RIGHT_CRICK ){ 
				
				// 技ボタンＯＮ
				battlePetButtonFlag = TRUE;
				// バックアップ初期化
				battleWazaTargetBak = -1;
			}
		}else{	// ボタン画面外へ
			if( buttonA <= 25 ){
				buttonA++;	// 加速度プラス
				buttonX += buttonA; // 移動処理
			}else{
				// 画面外に消えたら
				// ボックスフラグ消す
				ClearBoxFlag();
				// フラグ初期化
				battleTargetSelectFlag = FALSE;
				battleMenuFlag = FALSE;
				// コマンド番号初期化
				//BattleCmdNo = -1;
				// ボタンフラグ全初期化
				//ClearBattleButton();
				// 行動終了フラグ立てる
				p_party[ BattleMyNo + 5 ]->atr |= ACT_ATR_BTL_CMD_END;
				// 次のプロセスへ
				SubProcNo++;
			}
		}
		// 現在選択している技表示
		// 技があったら
		if( battleWazaTargetBak != -1 ){
			sprintf( moji,"%-24s",petSkill[ battlePetNoBak ][ BattleWazaNo ].name );
			//sprintf( moji,"%s","０１２３４５６７８９１２" );
			//CenteringStr( petSkill[ battlePetNoBak ][ BattleWazaNo ].name, moji, SKILL_NAME_LEN );
			StockFontBuffer( buttonX - 100, buttonY - 7, FONT_PRIO_FRONT, 0, moji, 0 );
			// キャンセルボタン表示
			battleButtonDispNo[ 8 ] = StockDispBuffer( buttonX, buttonY, DISP_PRIO_IME3, CG_PET_BTL_BUTTON_CANCEL_UP, 2 );
		}else{
			// ボタンを表示
			battleButtonDispNo[ 8 ] = StockDispBuffer( buttonX, buttonY, DISP_PRIO_IME3, CG_PET_BTL_BUTTON_WAZA_UP + battleButtonFlag[ 8 ], 2 );
		}
		// ボタンのベース
		StockDispBuffer( buttonX, buttonY, DISP_PRIO_MENU, CG_PET_BTL_BUTTON_BASE, 0 );
	}
	
	// カウントダウン表示とリミットチェック
	BattleCntDownDisp();
}

