/************************/
/*	menu.cpp			*/
/************************/
#include "system.h"
#include "font.h"
#include "ime_sa.h"
#include "anim_tbl.h"
#include "pc.h"
#include "battleMenu.h"
#include "battleProc.h"
#include "lssproto_cli.h"
#include "netmain.h"
#include "loadsprbin.h"
#include "savedata.h"
#include "t_music.h"
#include "menu.h"
#include "tool.h"
#include "map.h"
#include "field.h"
#include "pet_skillinfo.h"

// メニューの階層と当たり判定の数
#define MENU_SYSTEM_0	9
#define MENU_MAP_0		3
#define MENU_STATUS_0	12
#define MENU_PET_0		20
#define MENU_ITEM_0		40
#define MENU_MAIL_0		40
#define MENU_ALBUM_0	20
#define MENU_BTL_RESULT_0	1


// システム
#define MAX_CHAT_REGISTY_STR		8		// チャットの文字登録の数
#define MAX_CHAT_REGISTY_STR_LEN	26 		// チャットの文字登録の文字数
#define CAHT_REGISTY_STR_FILE_NAME 	"data\\chatreg.dat" 	// チャットの登録文字保存ファイル名
static int systemWndFontNo[ MENU_SYSTEM_0 ]; 			// 当たり判定番号
static unsigned int  systemWndNo;						// ウィンドウ番号
STR_BUFFER chatRegistryStr[ MAX_CHAT_REGISTY_STR ];		// チャットの文字登録の入力バッファー

// マップ
int mapWndFontNo[ MENU_MAP_0 ]; 	// 当たり判定番号
static unsigned int  mapWndNo;		// ウィンドウ番号
int MapWmdFlagBak;					// 戦闘後に自動でマップウィンド表示するフラグ

// ステータス
static int statusWndFontNo[ MENU_STATUS_0 ]; 	// 当たり判定番号
static int statusWndBtnFlag[ MENU_STATUS_0 ];	// ボタン凹凸フラグ
static unsigned int statusWndNo;				// ウィンドウ番号
STR_BUFFER shougouChange;						// プレイヤーの称号用文字列
int StatusUpPoint;								// ステータスアップポイント

// ペット
static int petWndFontNo[ MENU_PET_0 ]; 	// 当たり判定番号
static int petWndBtnFlag[ MENU_PET_0 ]; // ボタン凹凸フラグ
static unsigned int  petWndNo;			// ペットウィンドウ番号
int  petStatusNo;				// 詳細ウィンドウに表示されているペットの番号
static ACTION *pActPet;					// ペットのアクションポインタ
BOOL BattlePetReceiveFlag;				// 参戦ペット変更プロトコル送信フラグ
STR_BUFFER petNameChange;				// ペットの名前変更用文字列
int BattlePetReceivePetNo = -1;			// バトル受信中のペット番号
int SelectWazaNo;						// 選択した技番号

// アイテム
static int itemWndFontNo[ MENU_ITEM_0 ]; 	// 当たり判定番号
static int itemWndBtnFlag[ MENU_ITEM_0 ];	// ボタン凹凸フラグ
static unsigned int  itemWndNo;		
static int  jujutuNo = -1;					// 選択されている呪術番号
static int  itemNo = -1;					// 選択されているアイテム番号
static int  itemWndDropGold;				// 捨てるお金
static int  itemWndDropGoldInc;				// 捨てるお金増分
static int  itemWndDropGoldCnt;				// 捨てるお金増分カウンター
static ACTION *pActPet2;					// 合成ペットのアクションポインタ
BOOL ItemMixRecvFlag;						// 合成出来たフラグ
static int ItemMixPetNo = -1;				// 合成しているペットの番号
// アイテムバッファー
ITEM_BUFFER ItemBuffer[ MAX_ITEM ];

// メール
#define MAIL_HISTORY_FILE_NAME 	"data\\mail.dat" 	// 履歴保存ファイル名
typedef enum{
	MAIL_WND_VIEW,
	MAIL_WND_SELECT,
	MAIL_WND_SEND,
	MAIL_WND_PET_SEND,
	MAIL_WND_ITEM,
	MAIL_WND_HISTORY,
}MAIL_WND_TYPE;
static int mailWndFontNo[ MENU_MAIL_0 ]; 
static int mailWndBtnFlag[ MENU_MAIL_0 ];			// ボタン凹凸フラグ
static MAIL_WND_TYPE mailWndNo;						// 現在のメールウィンドウ番号
static MAIL_WND_TYPE mailWndNoBak;					// 前のメールウィンドウ番号
static int mailViewWndPageNo;						// 名刺一覧ウィンドウのページ番号
static int mailSendWndPageNo;						// 普通送信ウィンドウのページ番号
static int mailPetSendWndPageNo;					// ペット送信ウィンドウのページ番号
static int mailItemNo;								// ペット送信ウィンドウのアイテム番号
static int mailWndSendFlag[ MAX_ADR_BOOK ]; 		// 送信相手記憶フラグ
//int mailReceivePetGraNo;							// ペットメール受信時のペットのグラフィック番号
int mailHistoryWndPageNo;							// 履歴ウィンドウのページ番号
int mailHistoryWndSelectNo;							// 選択されている名刺番号
static int mailHistoryWndNowPageNo;					// 実際に表示する履歴番号
STR_BUFFER MailStr;									// 送る文章
MAIL_HISTORY MailHistory[ MAX_ADR_BOOK ];			// メールの履歴文字列
ACTION *pActLetter[ 4 ];							// 手紙アニメーションアクションポインタ
ACTION *pActMailItem;								// ペットメールで持っていくアイテム

// アルバム
#define ALBUM_FILE_NAME 	"data\\album.dat" 	// アルバム保存ファイル名
static int albumWndFontNo[ MENU_ALBUM_0 ]; 			
static int albumWndBtnFlag[ MENU_ALBUM_0 ]; 			
static unsigned int albumWndNo;						
static int albumWndPageNo;					// ページ番号
static int albumNo;						// アルバム番号
// アルバム用ペットデータテーブル構造体
PET_ALBUM_TBL PetAlbumTbl[] = {
	#include "petName.h"	// ペットのデータテーブル
};
PET_ALBUM PetAlbum[ MAX_PET_KIND ];		// アルバム構造体
int AlbumIdCnt = 0;					// 今回のアルバムファイル読み込み場所カウンタ
// ＩＤ暗号化テーブル
int IdEncryptionTbl[ 16 ] = { 48, 158, 98, 23, 134, 29, 92, 67, 
								70, 28, 235, 20, 189, 48, 57, 125 };

// 戦闘結果ウィンドウ
int resultWndFontNo[ MENU_BTL_RESULT_0 ];	// 当たり判定番号
int ResultWndTimer;							// 消えるまでの時間タイマー

// タスクバー  任务栏
#define TASK_BAR	7
#define TASK_BAR_X 320
#define TASK_BAR_Y 468
BOOL TaskBarFlag = FALSE;	// タスクバーフラグ  任务栏转折点
static int taskBarFontNo[ TASK_BAR ]; 
static int taskBarX = TASK_BAR_X, taskBarY = TASK_BAR_Y + 24;

// トグルフラグ 肘节转折点
unsigned int MenuToggleFlag;

// ウィンドウアクションポインタ 窗口的动作指针
ACTION *pActMenuWnd;
ACTION *pActMenuWnd2;
ACTION *pActMenuWnd3;
ACTION *pActMenuWnd4;
ACTION *pActYesNoWnd;

// 効果音ステレオモノラル文字列  效果音立体声单声道字符串
static char *monoStereoStr[] = { 	"　　　单声道　　　",
									"　　　立体声　　　"};

#define WINDOW_CREATE_FRAME 10	// ウィンドウが表示されるまでの時間

// ペットのコースから向きを取ってくる
extern unsigned char crs_change_tbl[];
// ペットの向きからコースを取ってくる
extern unsigned char crs_change_tbl2[];
// 反射テーブル
extern unsigned char crs_bound_tbl[][32];

// ウィンドウ表示タスク処理 ***************************************************/
void WindowDisp( ACTION *pAct )
{
	// 予備構造体のアドレス
	WINDOW_DISP *pYobi = ( WINDOW_DISP *)pAct->pYobi;
	int i, j;
	int x = pAct->x + 32, y = pAct->y + 24;
	
	// 行動番号で分岐
	switch( pAct->actNo ){
	
	case 0:	// ウィンドウ出現処理 窗口出现处理
	
		StockBoxDispBuffer( pYobi->mx - pYobi->nowX, 
							pYobi->my - pYobi->nowY, 
							pYobi->mx + pYobi->nowX,
							pYobi->my + pYobi->nowY, 
							pYobi->boxDispPrio, SYSTEM_PAL_BLACK, 0 );
		//StockBoxDispBuffer( pYobi->mx - pYobi->nowX + 2, 
		//					pYobi->my - pYobi->nowY + 2, 
		//					pYobi->mx + pYobi->nowX - 2,
		//					pYobi->my + pYobi->nowY - 2, 
		//					pYobi->boxDispPrio, SYSTEM_PAL_BLACK, 0 );
		// 増分プラス
		pYobi->nowX += pAct->dx;					
		pYobi->nowY += pAct->dy;					
		// カウンタープラス
		pYobi->cnt++;
		// リミットチェック
		if( pYobi->cnt >= WINDOW_CREATE_FRAME ){ 
			// 枠だけの時
			if( pYobi->wndType == -1 ) pAct->actNo = 1;
			else
			// 確認ウィンドウの時
			if( pYobi->wndType == -2 ) pAct->actNo = 3;
			else pAct->actNo = 2;
		}
		
		break;
		
	case 1:	// 枠だけの時
		pAct->hp = 1;
		
		break;
		
	case 2:	// ウィンドウ描画
	
		for( j = 0 ; j < pYobi->sizeY ; j++ ){	
			// 上表示
			if( j == 0 ){
				for( i = 0 ; i < pYobi->sizeX ; i++ ){	
					// 左端
					if( i == 0 ) StockDispBuffer( x, y, DISP_PRIO_MENU, pYobi->wndType + 0, pYobi->hitFlag );
					// 右端
					else if( i == pYobi->sizeX - 1 ) StockDispBuffer( x, y, DISP_PRIO_MENU, pYobi->wndType + 2, pYobi->hitFlag );
					// 真ん中
					else StockDispBuffer( x, y, DISP_PRIO_MENU, pYobi->wndType + 1, pYobi->hitFlag );
					x += 64; // 座標プラス
				}
			}else 
			// 下表示
			if( j == pYobi->sizeY - 1 ){
				for( i = 0 ; i < pYobi->sizeX ; i++ ){	
					// 左端
					if( i == 0 ) StockDispBuffer( x, y, DISP_PRIO_MENU, pYobi->wndType + 6, pYobi->hitFlag );
					// 右端
					else if( i == pYobi->sizeX - 1 ) StockDispBuffer( x, y, DISP_PRIO_MENU, pYobi->wndType + 8, pYobi->hitFlag );
					// 真ん中
					else StockDispBuffer( x, y, DISP_PRIO_MENU, pYobi->wndType + 7, pYobi->hitFlag );
					x += 64; // 座標プラス
				}
			}else 
			// 真ん中表示
			for( i = 0 ; i < pYobi->sizeX ; i++ ){	
				// 左端
				if( i == 0 ) StockDispBuffer( x, y, DISP_PRIO_MENU, pYobi->wndType + 3, pYobi->hitFlag );
				// 右端
				else if( i == pYobi->sizeX - 1 ) StockDispBuffer( x, y, DISP_PRIO_MENU, pYobi->wndType + 5, pYobi->hitFlag );
				// 真ん中
				else StockDispBuffer( x, y, DISP_PRIO_MENU, pYobi->wndType + 4, pYobi->hitFlag );
				x += 64; // 座標プラス
			}
			x = pAct->x + 32; 	// 左端に戻す
			y += 48; 		// 座標プラス
		}
		// タイトル描画 标题绘图
		if( pYobi->titleNo != 0 ) StockDispBuffer( pYobi->titleX, pYobi->titleY, DISP_PRIO_IME3, pYobi->titleNo, pYobi->hitFlag );
		//if( pYobi->titleNo != 0 ) StockDispBuffer( pYobi->titleX, pYobi->titleY, DISP_PRIO_IME3, CG_WND_TITLE_SYSTEM, pYobi->hitFlag );
		pAct->hp = 1;
		break;
		
	case 3:	// 確認ウィンドウ  确认窗口
		
		// 左クリックした時
		if( mouse.onceState & MOUSE_LEFT_CRICK ){
			// はいの時
			if( HitDispNo == pYobi->yesDispNo ){
				pYobi->yesNoResult = TRUE;
			}
			// いいえの時
			if( HitDispNo == pYobi->noDispNo ){
				pYobi->yesNoResult = FALSE;
			}
		}
		
		// 確認ウィンドウのベース表示
		StockDispBuffer( pYobi->mx, pYobi->my, DISP_PRIO_YES_NO_WND, CG_COMMON_WIN_YORO, 1 );
		// はいボタン
		pYobi->yesDispNo = StockDispBuffer( pYobi->mx, pYobi->my, DISP_PRIO_YES_NO_BTN, CG_COMMON_YES_BTN, 2);
		// いいえボタン
		pYobi->noDispNo = StockDispBuffer( pYobi->mx, pYobi->my, DISP_PRIO_YES_NO_BTN, CG_COMMON_NO_BTN, 2 );
		
		break;
	}
	
}

// ウィンドウ表示タスク作成 **************************************************************/
//	引数：	int x,y 			表示座標（左上）
//			int sizeX,sizeY 	ウィンドウのサイズパーツ単位（枠だけの時はピクセル単位）
//			int titleNo 		タイトルつけるときの画像番号、（無しのときはＮＵＬＬ）
//			int wndType 		０：石のウィンドウ（当たり判定あり）
//			 					１：木のウィンドウ（当たり判定あり）
//								２：石のウィンドウ（当たり判定無し）
//								３：木のウィンドウ（当たり判定無し）
//								４：確認ウィンドウ（当たり判定あり）
//								-1：枠だけ
//****************************************************************************************/
ACTION *MakeWindowDisp( int x, int y, int sizeX, int sizeY, int titleNo, int wndType )
{
	ACTION *pAct;
	WINDOW_DISP *pYobi;
	
	// アクションポインタ取得
	pAct = GetAction( PRIO_JIKI, sizeof( WINDOW_DISP ) );
	if( pAct == NULL ) return NULL;
	
	// 実行関数
	pAct->func = WindowDisp;
	// 表示優先度
	pAct->dispPrio = DISP_PRIO_MENU;
	// 当たり判定する
	pAct->atr |= ACT_ATR_HIT;
	// 表示しない
	pAct->atr |= ACT_ATR_HIDE;
	
	// 予備構造体のアドレス
	pYobi = ( WINDOW_DISP *)pAct->pYobi;
	
	// 座標を記憶
	pAct->x = x;
	pAct->y = y;
	pYobi->titleX = x + ( sizeX * 64 ) / 2;
	pYobi->titleY = pAct->y + 27;
	// サイズを記憶
	pYobi->sizeX = sizeX;
	pYobi->sizeY = sizeY;
	// タイトル番号記憶
	pYobi->titleNo = titleNo;
	pYobi->boxDispPrio = DISP_PRIO_MENU;
	// ウィンドウタイプ記憶
	if( wndType == 0 ) pYobi->wndType = CG_WND_G_0;
	else if( wndType == 1 ) pYobi->wndType = CG_WND2_G_0;
	else if( wndType == 2 ) pYobi->wndType = CG_WND_G_0;
	else if( wndType == 3 ) pYobi->wndType = CG_WND2_G_0;
	else if( wndType == 4 ){
		pYobi->wndType = -2;
		// 当たり判定初期化
		pYobi->yesDispNo = -2;
		pYobi->noDispNo = -2;
		pYobi->boxDispPrio = DISP_PRIO_YES_NO_WND;
	}
	else pYobi->wndType = -1;
	
	// YesNo フラグ初期化
	pYobi->yesNoResult = -1;
	
	// 当たり判定フラグ
	if( wndType >= 2 ) pYobi->hitFlag = FALSE;
	else pYobi->hitFlag = TRUE;
	
	// 中心座標計算
	// 枠だけの時、又は確認ウィンドウの時
	if( wndType == -1 || wndType == 4 ){
		pYobi->mx = sizeX / 2 + pAct->x;
		pYobi->my = sizeY / 2 + pAct->y;
	}else{	// 汎用ウィンドウの時
		pYobi->mx = ( sizeX * 64 ) / 2 + pAct->x;
		pYobi->my = ( sizeY * 48 ) / 2 + pAct->y;
	}
	// 増分計算
	pAct->dx = ( pYobi->mx - pAct->x ) / WINDOW_CREATE_FRAME;
	pAct->dy = ( pYobi->my - pAct->y ) / WINDOW_CREATE_FRAME;
	
	return pAct;
}
	
// アニメーションタスクの行動番号
enum{
	ANIM_DISP_PET,				// ペット表示
	ANIM_DISP_PET_SEND,			// ペットメール送信時、画面外へ移動
	ANIM_DISP_PET_RECIEVE,		// ペットメール受信時、画面内へ移動
	
	ANIM_DISP_PET_MAIN,			// ペットの行動選択処理
	ANIM_DISP_PET_WALK,			// 歩き
	ANIM_DISP_PET_STAND,		// 待機
	ANIM_DISP_PET_ATTACK,		// 攻撃
	ANIM_DISP_PET_GUARD,		// ガード
	ANIM_DISP_PET_DAMAGE,		// ダメージ
	ANIM_DISP_PET_DEAD,			// 死亡
	ANIM_DISP_PET_ITEM,			// ペットアイテム表示
	
	ANIM_DISP_LETTER_MOVE,		// 手紙移動
	ANIM_DISP_LETTER,			// 手紙アニメーション
	
	ANIM_DISP_MIX_PET_INIT,		// 合成ペット初期化
	ANIM_DISP_MIX_PET_MOVE,		// 合成ペット食べながら移動
	ANIM_DISP_MIX_PET_MOJI,		// 合成ペットがもじもじして、合成完了
	ANIM_DISP_MIX_PET_END,		// 合成ペット画面外へ
	
};

// アニメーションタスク表示処理 ***********************************************/
void AnimDisp( ACTION *pAct )
{
	int flag = FALSE;
	int x, y, i;
	
	// ペットの時
	switch( pAct->actNo ){
	
	case ANIM_DISP_PET:	// ペット表示
	
		// 左クリックで方向変換
		if( pAct->hitDispNo == HitDispNo && 
			mouse.autoState & MOUSE_LEFT_CRICK ){
			// 方向変換
			pAct->anim_ang++;
			// リミットチェック
			if( pAct->anim_ang >= 8 ) pAct->anim_ang = 0;
			play_se( 217, 320, 240 ); // クリック音
		}
		// アニメーション
		pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
		
		break;
		
	case ANIM_DISP_PET_SEND:	// ペットメール送信時、画面外へ移動
	
		// 動作番号
		pAct->anim_no = ANIM_WALK;
		// アニメーション向き( ０～７ )( 下が０で右回り )
		pAct->anim_ang = 2;
		// 方向
		pAct->crs = crs_change_tbl2[ pAct->anim_ang ];
		// カウンター
		pAct->dx++;
		// ２フレームに一回加速
		if( pAct->dx % 3 == 0 ){ 
			// スピード加速
			if( pAct->spd < 20 ){ 
				pAct->spd++;
			}
		}
		// ３フレームに一回加速
		if( pAct->dx % 5 == 0 ){ 
			if( pAct->dy >= 2 ) pAct->dy--;
		}
		// 移動処理
		gemini( pAct );
		
		// アイテムある時
		if( pActMailItem != NULL ){
			// アイテムに近づいたら
			//if( pAct->x - pActMailItem->x < 32 ){
			//	pActMailItem->x = pAct->x - 32;
			//}
			if( pAct->x <= pActMailItem->x ){
				pActMailItem->x = pAct->x;
			}
		}
		// 画面外へ消えたらウィンドウ閉じる
		if( pAct->x <= - 100 ){ 
			// ウィンドウ抹殺
			DeathAction( pActMenuWnd );
			pActMenuWnd = NULL;
			// ペット抹殺
			DeathAction( pActPet );
			pActPet = NULL;
			// メールアイテム抹殺
			DeathAction( pActMailItem );
			pActMailItem = NULL;
			// 一覧ウィンドウへ
			mailWndNo = MAIL_WND_VIEW;	
			// ウィンドウ閉じる音
			play_se( 203, 320, 240 );
			// 入力フォーカス戻す
			GetKeyInputFocus( &MyChatBuffer );
		}
		// アニメーション
		pattern( pAct, pAct->dy, ANM_LOOP );
		
		break;
		
	case ANIM_DISP_PET_RECIEVE:	// ペットメール受信時、画面内へ移動
	
		// 動作番号
		pAct->anim_no = ANIM_WALK;
		// アニメーション向き( ０～７ )( 下が０で右回り )
		pAct->anim_ang = 6;
		// 方向
		pAct->crs = crs_change_tbl2[ pAct->anim_ang ];
		// カウンター
		pAct->dx++;
		// スピード加速
		if( pAct->spd > 0 ){ 
			// ２フレームに一回加速
			if( pAct->dx % 3 == 0 ){ 
				pAct->spd--;
				// 止まった時、行動ランダム
				if( pAct->spd <= 0 ){ 
					pAct->actNo = ANIM_DISP_PET_MAIN;
					// アイテムある時
					if( pActMailItem != NULL ){
						pActMailItem->dispPrio = DISP_PRIO_BOX3;
					}
				}
			}
		}
		// ３フレームに一回加速
		if( pAct->dx % 8 == 0 ){ 
			// アニメーション加速
			if( pAct->dy < 12 ) pAct->dy++;
		}
		// 移動処理
		gemini( pAct );
		
		// アイテムある時
		if( pActMailItem != NULL ){
			// アイテムに近づいたら
			//if( pAct->x - pActMailItem->x < 32 ){
			//	pActMailItem->x = pAct->x - 32;
			//}
			if( pAct->x >= pActMailItem->x ){
				pActMailItem->x = pAct->x;
			}
		}
		// アニメーション
		pattern( pAct, pAct->dy, ANM_LOOP );
		
		break;
	
	case ANIM_DISP_PET_MAIN:	// ペットの行動選択処理
		
		// ５分の３で歩き
		if( Rnd( 0, 4 ) >= 2 ){ 
			pAct->actNo = ANIM_DISP_PET_WALK;
			// 方向ランダム
			pAct->anim_ang = Rnd( 0, 7 );
			// コース設定
			pAct->crs = crs_change_tbl2[ pAct->anim_ang ];
			// ランダム移動スピード
			pAct->spd = Rnd( 1, 12 );
			// アニメーションスピード
			pAct->dy = 13 - pAct->spd;
		}
		else pAct->actNo = Rnd( ANIM_DISP_PET_STAND, ANIM_DISP_PET_DEAD );
		
		// 前回のアニメーション忘れさせる
		pAct->anim_no_bak = -1;
		
		break;
		
	case ANIM_DISP_PET_WALK:	// 歩き
		
		// 動作番号
		pAct->anim_no = ANIM_WALK;
		
		// 移動処理
		gemini( pAct );
	
		// アニメーション
		if( pattern( pAct, pAct->dy, ANM_NO_LOOP ) ){
			// ランダム繰り返し
			if( Rnd( 0, 3 ) == 0 ){
				pAct->actNo = ANIM_DISP_PET_MAIN;
			}else{
				// 前回のアニメーション忘れさせる
				pAct->anim_no_bak = -1;
			}
		}
		
		// リミットチェック
		// 上
		if( pActPet->y < pActMenuWnd->y + 64 ){
			pActPet->y = pActMenuWnd->y + 64;
			// コース設定
			pAct->crs = crs_bound_tbl[ 0 ][ pAct->crs ];
			// 向き設定
			pAct->anim_ang = crs_change_tbl[ pAct->crs ];
		}
		// 下
		if( pActPet->y > pActMenuWnd->y + 424 - 32 ){
			pActPet->y = pActMenuWnd->y + 424 - 32;
			// コース設定
			pAct->crs = crs_bound_tbl[ 2 ][ pAct->crs ];
			// 向き設定
			pAct->anim_ang = crs_change_tbl[ pAct->crs ];
		}
		// 左
		if( pActPet->x < pActMenuWnd->x + 32 ){
			pActPet->x = pActMenuWnd->x + 32;
			// コース設定
			pAct->crs = crs_bound_tbl[ 3 ][ pAct->crs ];
			// 向き設定
			pAct->anim_ang = crs_change_tbl[ pAct->crs ];
		}
		// 右
		if( pActPet->x > pActMenuWnd->x + 272 - 32 ){
			pActPet->x = pActMenuWnd->x + 272 - 32;
			// コース設定
			pAct->crs = crs_bound_tbl[ 1 ][ pAct->crs ];
			// 向き設定
			pAct->anim_ang = crs_change_tbl[ pAct->crs ];
		}
		
		break;

	case ANIM_DISP_PET_STAND:	// 待機
		
		// 動作番号
		pAct->anim_no = ANIM_STAND;
	
		// アニメーション
		if( pattern( pAct, ANM_NOMAL_SPD, ANM_NO_LOOP ) ){
			// ランダム繰り返し
			//if( Rnd( 0, 1 ) == 0 ){
				pAct->actNo = ANIM_DISP_PET_MAIN;
			//}else{
				// 前回のアニメーション忘れさせる
			//	pAct->anim_no_bak = -1;
			//}
		}
	
		break;
	
	case ANIM_DISP_PET_ATTACK:	// 攻撃
		
		// 動作番号
		pAct->anim_no = ANIM_ATTACK;
	
		// アニメーション
		if( pattern( pAct, ANM_NOMAL_SPD, ANM_NO_LOOP ) ){
			// ランダム繰り返し
			if( Rnd( 0, 1 ) == 0 ){
				pAct->actNo = ANIM_DISP_PET_MAIN;
			}else{
				// 前回のアニメーション忘れさせる
				pAct->anim_no_bak = -1;
			}
		}
	
		break;
		
	case ANIM_DISP_PET_GUARD:	// ガード
		
		// 動作番号
		pAct->anim_no = ANIM_GUARD;
	
		// アニメーション
		if( pattern( pAct, ANM_NOMAL_SPD, ANM_NO_LOOP ) ){
			// ランダム繰り返し
			if( Rnd( 0, 1 ) == 0 ){
				pAct->actNo = ANIM_DISP_PET_MAIN;
			}else{
				// 前回のアニメーション忘れさせる
				//pAct->anim_no_bak = -1;
			}
		}
	
		break;
		
	case ANIM_DISP_PET_DAMAGE:	// ダメージ
		
		// 動作番号
		pAct->anim_no = ANIM_DAMAGE;
	
		// アニメーション
		if( pattern( pAct, ANM_NOMAL_SPD, ANM_NO_LOOP ) ){
			// ランダム繰り返し
			if( Rnd( 0, 1 ) == 0 ){
				pAct->actNo = ANIM_DISP_PET_MAIN;
			}else{
				// 前回のアニメーション忘れさせる
				pAct->anim_no_bak = -1;
			}
		}
	
		break;
		
	case ANIM_DISP_PET_DEAD:	// 死亡
		
		// 動作番号
		pAct->anim_no = ANIM_DEAD;
	
		// アニメーション
		if( pattern( pAct, ANM_NOMAL_SPD, ANM_NO_LOOP ) ){
			// ランダム繰り返し
			if( Rnd( 0, 2 ) == 0 ){
				pAct->actNo = ANIM_DISP_PET_MAIN;
			}else{
				// 前回のアニメーション忘れさせる
				//pAct->anim_no_bak = -1;
			}
		}
	
		break;
		
	case ANIM_DISP_PET_ITEM:	// 送信ペットアイテム表示
	
		// アニメーション
		pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
		
		break;
	
	case ANIM_DISP_LETTER_MOVE:	// 手紙移動
	
		// 移動
		if( pAct->x < 245 ) pAct->x += 5;
		// 手紙アニメーションへ
		else{ 
			pAct->actNo = ANIM_DISP_LETTER;
			// アニメーション
			pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
			pAct->anim_cnt += 8;
		}
		break;
	
	case ANIM_DISP_LETTER:		// 手紙アニメーション
	
		// アニメーション
		pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
		
		break;
		
	case ANIM_DISP_MIX_PET_INIT:	// 合成ペット初期化
	
		// ランダム移動スピード
		pAct->spd = 11;//Rnd( 6, 12 );
		// アニメーションスピード
		pAct->dy = 14 - pAct->spd;
		
		// 動作番号
		pAct->anim_no = ANIM_WALK;
		
		// 最初のポイントを探す
		for( i = 5 ; i < MAX_ITEM ; i++ ){
			if( ItemBuffer[ i ].mixFlag >= 1 ){ 
				pAct->dx = i;
				break;
			}
		}
		// 移動へ
		pAct->actNo = ANIM_DISP_MIX_PET_MOVE;
		break;
		
	case ANIM_DISP_MIX_PET_MOVE:	// 合成ペット食べながら移動
		
		// 目的地設定
		x = ItemBuffer[ pAct->dx ].defX;
		y = ItemBuffer[ pAct->dx ].defY;
		
		// レーダー
		radar( pAct, &x, &y );
		
		// 動作番号
		pAct->anim_no = ANIM_WALK;
		
		// 到着してなかったら
		if( y >= pAct->spd / 2 ){
			// コース設定
			pAct->crs = x;
			// 向き設定（目的地変更時に一回だけ）
			if( pAct->level == FALSE ){
				pAct->anim_ang = crs_change_tbl[ pAct->crs ];
				// アングル変換フラグＯＮ
				pAct->level = TRUE;
			}
			
			// 移動処理
			gemini( pAct );
			// アニメーション
			pattern( pAct, pAct->dy, ANM_LOOP );
			//pattern( pAct, 2, ANM_LOOP );
		}
		// 到着したら
		else{
			// アングル変換フラグＯＦＦ
			pAct->level = FALSE;
			// アイテム使う音
			play_se( 204, 320, 240 );
			
			// 最後まで行ったフラグＯＮの時
			if( pAct->delta == 1 ){
				// もじもじするへ
				pAct->actNo = ANIM_DISP_MIX_PET_MOJI;
				// 動作番号
				pAct->anim_no = ANIM_STAND;
				
				// ランダム移動スピード
				pAct->spd = 10;//Rnd( 6, 12 );
				// アニメーションスピード
				pAct->dy = 14 - pAct->spd;

				// 描画を下にする
				for( i = 5 ; i < MAX_ITEM ; i++ ){
					if( ItemBuffer[ i ].mixFlag >= 1 ){ 
						ItemBuffer[ i ].mixFlag = 4;
					}
				}
				
				// もじもじする回数ランダム
				pAct->delta = Rnd( 2, 15 );
				pAct->mp = 15;
				break;
			}
			
			// 移動スピード遅くする
			pAct->spd = ( int )( ( double )pAct->spd * 0.79 );
			// リミットチェック
			if( pAct->spd < 2 ) pAct->spd = 2;
			// アニメーションスピード
			pAct->dy = 13 - pAct->spd;
			
			// ペットにアイテムをくっつける
			ItemBuffer[ pAct->dx ].mixFlag = 3;
			
			// 次のポイントを探す
			for( i = pAct->dx + 1 ; i < MAX_ITEM ; i++ ){
				if( ItemBuffer[ i ].mixFlag >= 1 ){ 
					pAct->dx = i;
					break;
				}
			}
			
			// 最後まで行ったら
			if( i >= MAX_ITEM ){
				// アイテムが出きるところを探す
				for( i = 5 ; i < MAX_ITEM ; i++ ){
					// その欄にアイテムが無い時
					if( pc.item[ i ].useFlag == FALSE ){
						// そこで決定
						pAct->dx = i;
						break;
					}else
					// 今回合成するアイテム欄なら
					if( ItemBuffer[ i ].mixFlag >= 1 ){ 
						// そこで決定
						pAct->dx = i;
						break;
					}
				}
				// 最後まで行ったフラグＯＮ
				pAct->delta = 1;
				
			}
		}
		break;
		
	case ANIM_DISP_MIX_PET_MOJI:	// 合成ペットがもじもじして完成。
		
		// アニメーション
		pattern( pAct, 1, ANM_LOOP );
		//pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
	
		// 時間がきたら
		if( pAct->maxHp >= pAct->mp ) pAct->maxHp = 0;
		else{
			pAct->maxHp++;
			break;
		}
		
		// もじもじする
		switch( pAct->hp ){
			
			// 右向き
			case 0:
			case 3:
				// 向き設定
				pAct->anim_ang++;
				// リミットチェック
				if( pAct->anim_ang >= 8 ) pAct->anim_ang = 0;
				// もじもじ音
				if( pAct->hp == 0 ){ 
					// アイテム使う音
					play_se( 216, 320, 240 );
				}
				// カウントプラス
				if( pAct->hp == 3 ){ 
					pAct->dir++;
					pAct->hp = 0;
					// 首振りスピード変更
					pAct->mp--;
					if( pAct->mp < 4 ) pAct->mp = 4;
				}
				else pAct->hp++;
			break;
			
			// 左向き
			case 1:
			case 2:
				// 向き設定
				pAct->anim_ang--;
				// リミットチェック
				if( pAct->anim_ang < 0 ) pAct->anim_ang = 7;
				pAct->hp++;
			break;
		}
		
		// もじもじ終ったら
		if( pAct->dir >= pAct->delta ){
			// 送信する
			if( pAct->dir == pAct->delta ){
				pAct->dir++;
				// 文字初期化
				char moji[ 256 ];
				// 文字初期化
				moji[ 0 ] = NULL;
				// 合成するアイテムをチェック
				for( i = 5 ; i < 20 ; i++ ){
					// 合成フラグ立っている時
					if( ItemBuffer[ i ].mixFlag >= TRUE ){
						char work[ 256 ];
						// 文字列作成
						sprintf( work,"%d|", i );
						strcat( moji, work );
						// フラグＯＦＦ
						//ItemBuffer[ i ].mixFlag = FALSE;
					}
				}
				// 最後の|をＮＵＬＬにする
				moji[ strlen( moji ) - 1 ] = NULL;
				
				// ペットスキル使うを送信（今は合成しかない）
				lssproto_PS_send( sockfd, petStatusNo, SelectWazaNo, 0, moji );
				// 受信待ちフラグＯＮ
				ItemMixRecvFlag = TRUE;
			}	
			
			// 受信待ちフラグＯＮ
			if(	ItemMixRecvFlag == FALSE ){
				// アイテム使う音
				play_se( 115, 320, 240 );
				//play_se( 212, 320, 240 );
				// 合成フラグの初期化
				for( i = 5 ; i < MAX_ITEM ; i++ ){
					ItemBuffer[ i ].mixFlag = FALSE;
				}
				// ランダム移動スピード
				pAct->spd = 11;
				// アニメーションスピード
				pAct->dy = 14 - pAct->spd;
				// 画面外へ
				pAct->actNo = ANIM_DISP_MIX_PET_END;
			}
		}
		
		break;
		
	case ANIM_DISP_MIX_PET_END:	// 合成ペット画面外へ
	
		// 目的地設定
		x = 750;
		y = 240;
		
		// レーダー
		radar( pAct, &x, &y );
		
		// 動作番号
		pAct->anim_no = ANIM_WALK;
		
		// 到着してなかったら
		if( y >= pAct->spd / 2 ){
			// コース設定
			pAct->crs = x;
			
			// 向き設定（目的地変更時に一回だけ）
			if( pAct->level == FALSE ){
				pAct->anim_ang = crs_change_tbl[ pAct->crs ];
				// アングル変換フラグＯＮ
				pAct->level = TRUE;
			}
			
			// 移動処理
			gemini( pAct );
			// アニメーション
			pattern( pAct, pAct->dy, ANM_LOOP );
		}
		// 到着したら
		else{
			// アクション抹殺
			DeathAction( pAct );
			pActPet2 = NULL;
			// 合成ペット番号初期化
			ItemMixPetNo = -1;
		}
		break;
		
	}
	
	// ペット画面内で動いている時
	if( ANIM_DISP_PET_MAIN <= pAct->actNo && pAct->actNo <= ANIM_DISP_PET_DEAD ){
		// ペットを左クリックした時
		if( pAct->hitDispNo == HitDispNo && 
			mouse.autoState & MOUSE_LEFT_CRICK ){
			// 行動初期化
			pAct->actNo = ANIM_DISP_PET_WALK;
			// 方向ランダム
			pAct->anim_ang = Rnd( 0, 7 );
			// コース設定
			pAct->crs = crs_change_tbl2[ pAct->anim_ang ];
			// ランダム移動スピード
			pAct->spd = 15;
			//pAct->spd = Rnd( 6, 12 );
			// アニメーションスピード
			//pAct->dy = 13 - pAct->spd;
			pAct->dy = 2;
			play_se( 217, 320, 240 ); // クリック音
		}
	}
}

// アニメーションタスク作成 ***********************************************/
ACTION *MakeAnimDisp( int x, int y, int sprNo, int mode )
{
	ACTION *pAct;
	
	// アクションポインタ取得
	pAct = GetAction( PRIO_JIKI, NULL );
	if( pAct == NULL ) return NULL;
	
	// 実行関数
	pAct->func = AnimDisp;
	// 当たり判定する
	pAct->atr |= ACT_ATR_HIT;
	// anim_tbl.h の番号
	pAct->anim_chr_no = sprNo;
	// 動作番号
	//pAct->anim_no = ANIM_WALK;
	pAct->anim_no = ANIM_STAND;
	// アニメーション向き( ０～７ )( 下が０で右回り )
	pAct->anim_ang = 1;
	/* 表示優先度 */
	pAct->dispPrio = DISP_PRIO_BOX3;
	/* 初期位置 */
	pAct->x = x;
	pAct->y = y;
	// モード設定
	pAct->actNo = mode;
	// モードによって初期化
	if( pAct->actNo == ANIM_DISP_PET ){
		// アニメーションスピード
		pAct->dy = 12;
	}else
	if( pAct->actNo == ANIM_DISP_PET_RECIEVE ){
		pAct->dy = 1;
		pAct->spd = 30;
	}
	
	// アニメーション
	pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
	
	return pAct;
}

// メールデータの保存 ****************************************************************/
BOOL SaveMailHistory( int no )
{
	FILE *fp; 
	
	// 書き込みファイルオープン
	if( ( fp = fopen( MAIL_HISTORY_FILE_NAME, "r+b" ) ) == NULL ){
		return FALSE;
	}
	
	// このキャラの書きこむ場所までファイルポインタを進ませる
	fseek( fp, sizeof( MAIL_HISTORY ) * MAX_ADR_BOOK * selectPcNo, SEEK_SET );
	
	// 今回書き込む名刺番号のところまでファイルポインタ進める
	fseek( fp, sizeof( MAIL_HISTORY ) * no, SEEK_CUR );
	
	// データ書き込み
	if( fwrite( &MailHistory[ no ], sizeof( MAIL_HISTORY ), 1, fp ) < 1 ){
	
		fclose( fp );// ファイルクローズ
		return FALSE;
	}
	
	// データ書き込み
	//if( fwrite( MailHistory, sizeof( MAIL_HISTORY ), MAX_ADR_BOOK, fp ) < MAX_ADR_BOOK ){
	//	return FALSE;
	//}
	// ファイルクローズ
	fclose( fp );
	
	return TRUE;
}

// メールデータの読み込み ****************************************************************/
BOOL LoadMailHistory( void )
{
	FILE *fp;
	MAIL_HISTORY work[ MAX_ADR_BOOK * 2 ];	// チェック用メール構造体

	// 読み込みファイルオープン
	if( ( fp = fopen( MAIL_HISTORY_FILE_NAME, "rb" ) ) == NULL ){
		// ない時、強制的に作成
		if( ( fp = fopen( MAIL_HISTORY_FILE_NAME, "wb" ) ) != NULL ){
			// データ書き込み
			fwrite( MailHistory, sizeof( MAIL_HISTORY ), MAX_ADR_BOOK, fp );
			fwrite( MailHistory, sizeof( MAIL_HISTORY ), MAX_ADR_BOOK, fp );
			fclose( fp );	// ファイルクローズ
		}
		
		return FALSE;
	}
	
	// ファイルサイズのチェック
	if( fread( work, sizeof( MAIL_HISTORY ), MAX_ADR_BOOK * 2, fp ) < MAX_ADR_BOOK * 2 ){
		// サイズが違う時
		// ない時、強制的に作成
		if( ( fp = fopen( MAIL_HISTORY_FILE_NAME, "wb" ) ) != NULL ){
#ifdef _DEBUG		
		MessageBox( hWnd, "以前のメールデータを削除しました。", "確認", MB_OK | MB_ICONSTOP );
#endif
			// データ書き込み
			fwrite( MailHistory, sizeof( MAIL_HISTORY ), MAX_ADR_BOOK, fp );
			fwrite( MailHistory, sizeof( MAIL_HISTORY ), MAX_ADR_BOOK, fp );
			fclose( fp );	// ファイルクローズ
		}
		
		return FALSE;
	}
	
	
	// 読みこむ場所までファイルポインタを進ませる
	fseek( fp, sizeof( MAIL_HISTORY ) * MAX_ADR_BOOK * selectPcNo, SEEK_SET );
	
	// データ読み込み
	if( fread( MailHistory, sizeof( MAIL_HISTORY ), MAX_ADR_BOOK, fp ) < MAX_ADR_BOOK ){
	
		fclose( fp );	// ファイルクローズ
		return FALSE;
	}
	// ファイルクローズ
	fclose( fp );
	
	return TRUE;
}

// チャットの登録文字の保存 ****************************************************************/
BOOL SaveChatRegistyStr( void )
{
	FILE *fp; 
	
	// 書き込みファイルオープン
	if( ( fp = fopen( CAHT_REGISTY_STR_FILE_NAME, "r+b" ) ) == NULL ){
#ifdef _DEBUG		
		MessageBox( hWnd, "チャットの登録文字のセーブに失敗！１", "確認", MB_OK | MB_ICONSTOP );
#endif
		return FALSE;
	}
	
	// データ書き込み
	if( fwrite( chatRegistryStr, sizeof( STR_BUFFER ), MAX_CHAT_REGISTY_STR, fp ) < MAX_CHAT_REGISTY_STR ){
	
#ifdef _DEBUG		
		MessageBox( hWnd, "チャットの登録文字のセーブに失敗！２", "確認", MB_OK | MB_ICONSTOP );
#endif
		fclose( fp );// ファイルクローズ
		return FALSE;
	}
	// ファイルクローズ
	fclose( fp );
	
	return TRUE;
}

// チャットの登録文字の読み込み ****************************************************************/
BOOL LoadChatRegistyStr( void )
{
	FILE *fp;
	int i;
	

	// 読み込みファイルオープン
	if( ( fp = fopen( CAHT_REGISTY_STR_FILE_NAME, "rb" ) ) == NULL ){
		// ない時、強制的に作成
		if( ( fp = fopen( CAHT_REGISTY_STR_FILE_NAME, "wb" ) ) != NULL ){
#ifdef _DEBUG		
		MessageBox( hWnd, "チャットの登録文字ファイルを作成します。", "確認", MB_OK );
#endif
			// チャットの文字登録の入力バッファーの初期化
			for( i = 0 ; i < 8 ; i++ ){
				// メール文章 初期設定
				chatRegistryStr[ i ].buffer[ 0 ] = NULL;
				chatRegistryStr[ i ].cnt = 0;
				// 文字列の長さ
				chatRegistryStr[ i ].len = MAX_CHAT_REGISTY_STR_LEN;
				// 一行の長さ
				chatRegistryStr[ i ].lineLen = 0;
				// 一行の間隔
				chatRegistryStr[ i ].lineDist = 20;
				// 文字列の色
				chatRegistryStr[ i ].color = 0;
				// 文字列の座標
				//chatRegistryStr[ i ].x = pActMenuWnd->x + 12;
				//chatRegistryStr[ i ].y = pActMenuWnd->y + 280 - 136 + i * 48;
				// 文字列の表示優先順位
				chatRegistryStr[ i ].fontPrio = FONT_PRIO_FRONT;
			}
			// データ書き込み
			fwrite( chatRegistryStr, sizeof( STR_BUFFER ), MAX_CHAT_REGISTY_STR, fp );
			fclose( fp );	// ファイルクローズ
		}
		
		return FALSE;
	}
	
	// データ読み込み
	if( fread( chatRegistryStr, sizeof( STR_BUFFER ), MAX_CHAT_REGISTY_STR, fp ) < MAX_CHAT_REGISTY_STR ){
		
#ifdef _DEBUG		
		MessageBox( hWnd, "チャットの登録文字のロードに失敗！", "確認", MB_OK | MB_ICONSTOP );
#endif
		fclose( fp );	// ファイルクローズ
		return FALSE;
	}
	// ファイルクローズ
	fclose( fp );
	
	return TRUE;
}

// アルバムの保存 ****************************************************************/
BOOL SaveAlbum( int no )
{
	FILE *fp; 
	int i;
	
	// 書き込みファイルオープン
	if( ( fp = fopen( ALBUM_FILE_NAME, "r+b" ) ) == NULL ){
#ifdef _DEBUG		
		MessageBox( hWnd, "アルバムファイルのセーブに失敗！１", "確認", MB_OK );
#endif
		return FALSE;
	}
	// 他人のアルバム分進める
	for( i = 0 ; i < AlbumIdCnt ; i++ ){
		// ＩＤカウントの分だけ進ませる
		fseek( fp, 16, SEEK_CUR );
		fseek( fp, sizeof( PET_ALBUM ) * MAX_PET_KIND, SEEK_CUR );
	}
	// ＩＤ分だけ進ませる
	fseek( fp, 16, SEEK_CUR );
	// 今回書き込むアルバム番号まで進める
	fseek( fp, sizeof( PET_ALBUM ) * no, SEEK_CUR );
	
	// データ書き込み
	if( fwrite( &PetAlbum[ no ], sizeof( PET_ALBUM ), 1, fp ) < 1 ){
	
#ifdef _DEBUG		
		MessageBox( hWnd, "アルバムファイルのセーブに失敗！２", "確認", MB_OK );
#endif
		fclose( fp );// ファイルクローズ
		return FALSE;
	}
	// ファイルクローズ
	fclose( fp );
	
	return TRUE;
}

// アルバムの読み込み ****************************************************************/
BOOL LoadAlbum( void )
{
	FILE *fp;
	char id[ 16 ];
	char id2[ 16 ];
	int i;
	
	// ＩＤカウンタ初期化
	AlbumIdCnt = 0;
	
	// 現在のＩＤ取得
	getCdkey( id );
	// 文字列の長さ取得
	id[ 15 ] = strlen( id );
	
	// 読み込みファイルオープン
	if( ( fp = fopen( ALBUM_FILE_NAME, "r+b" ) ) == NULL ){
		
		// ない時、強制的に作成
		if( ( fp = fopen( ALBUM_FILE_NAME, "wb" ) ) != NULL ){
#ifdef _DEBUG		
		//MessageBox( hWnd, "アルバムファイルを作成します。", "確認", MB_OK );
#endif
			// ＮＵＬＬのところを埋める
			id[ id[ 15 ] ] = Rnd( 1, 100 );
			// ＩＤの暗号化
			for( i = 0 ; i < 16 ; i++ ){
				id[ i ] ^= IdEncryptionTbl[ i ];
				
			}
			// ＩＤデータ書き込み
			fwrite( id, sizeof( id ), 1, fp );
			
			// アルバム構造体を適当に初期化
			for( i = 0 ; i < MAX_PET_KIND ; i++ ){
				int j;
				// 名前分ループ
				for( j = 0 ; j < PET_NAME_LEN + 1 ; j++ ){
					PetAlbum[ i ].name[ j ] = Rnd( 1, 255 );
					PetAlbum[ i ].freeName[ j ] = Rnd( 1, 255 );
				}
			
				PetAlbum[ i ].faceGraNo = Rnd( 1, 60000 );
				PetAlbum[ i ].level = Rnd( 1, 30000 );
				PetAlbum[ i ].faceGraNo = Rnd( 1, 30000 );
				PetAlbum[ i ].maxHp = Rnd( 1, 30000 );
				PetAlbum[ i ].str = Rnd( 1, 30000 );
				PetAlbum[ i ].quick = Rnd( 1, 30000 );
				PetAlbum[ i ].def = Rnd( 1, 30000 );
				
				PetAlbum[ i ].earth = Rnd( 1, 30000 );
				PetAlbum[ i ].water = Rnd( 1, 30000 );
				PetAlbum[ i ].fire = Rnd( 1, 30000 );
				PetAlbum[ i ].wind = Rnd( 1, 30000 );
			}
			
			// データ書き込み
			fwrite( PetAlbum, sizeof( PET_ALBUM ), MAX_PET_KIND, fp );
			fclose( fp );	// ファイルクローズ
		}
		
		return FALSE;
	}
	
	// ＩＤ探すまでループ（ 無い時は新規作成 ）
	while( 1 ){
		// データ読み込み
		if( fread( id2, sizeof( id2 ), 1, fp ) < 1 ){
#ifdef _DEBUG		
		//MessageBox( hWnd, "そのＩＤは保存されてません！新規作成します。", "確認", MB_OK | MB_ICONSTOP );
#endif
			// 失敗した時（ＥＯＦ）新規作成
			// ＮＵＬＬのところを埋める
			id[ id[ 15 ] ] = Rnd( 1, 100 );
			// ＩＤの暗号化
			for( i = 0 ; i < 16 ; i++ ){
				id[ i ] ^= IdEncryptionTbl[ i ];
				
			}
			// ＩＤデータ書き込み
			fwrite( id, sizeof( id ), 1, fp );
			// データ書き込み
			fwrite( PetAlbum, sizeof( PET_ALBUM ), MAX_PET_KIND, fp );
			fclose( fp );	// ファイルクローズ
			return FALSE;
		}
		// ＩＤの暗号化を戻す
		for( i = 0 ; i < 16 ; i++ ){
			id2[ i ] ^= IdEncryptionTbl[ i ];
		}
		// ＮＵＬＬ文字を埋める
		id2[ id2[ 15 ] ] = NULL;
		
		// 違うＩＤの時
		//if( strstr( id, id2 ) == 0 ){
		if( strcmp( id, id2 ) != 0 ){
#ifdef _DEBUG		
		//MessageBox( hWnd, "つぎのＩＤをさがします。", "確認", MB_OK | MB_ICONSTOP );
#endif
			// 次のＩＤの場所までファイルポインタを進ませる
			if( fseek( fp, sizeof( PET_ALBUM ) * MAX_PET_KIND, SEEK_CUR ) != 0 ){
#ifdef _DEBUG		
		//MessageBox( hWnd, "次のアルバムがないです", "確認", MB_OK | MB_ICONSTOP );
#endif
				fclose( fp );	// ファイルクローズ
				return FALSE;
			}
			
		}
		// あった時
		else{
#ifdef _DEBUG		
		//MessageBox( hWnd, "この人のアルバムを見つけました。", "確認", MB_OK | MB_ICONSTOP );
#endif
			// アルバムデータ読み込み
			if( fread( PetAlbum, sizeof( PET_ALBUM ), MAX_PET_KIND, fp ) < MAX_PET_KIND ){
		
#ifdef _DEBUG		
		//MessageBox( hWnd, "アルバムのロードに失敗！", "確認", MB_OK | MB_ICONSTOP );
#endif
				fclose( fp );	// ファイルクローズ
				return FALSE;
			}
			break;
		}
		// ＩＤカウンタ
		AlbumIdCnt++;
	}
	// ファイルクローズ
	fclose( fp );
	
	return TRUE;
}

// ログイン時の初期化（太田専用） **************************************************/
void InitOhtaParam( void )
{
	LoadMailHistory();					// メールデータの読み込み
	LoadChatHistoryStr();				// チャットの履歴文字の読み込み
	mailLamp = CheckMailNoReadFlag();	// 未読チェック
	StatusUpPoint = 0;					// ステータスアップポイント初期化
	battleResultMsg.useFlag = 0;		// 戦闘結果フラグ初期化
	MapWmdFlagBak = 0;
}

// メール未読チェック ****************************************************************/
BOOL CheckMailNoReadFlag( void )
{
	int i,j;
	
	// 名刺分ループ
	for( i = 0 ; i < MAX_ADR_BOOK ; i++ ){
		// 履歴分ループ
		for( j = 0 ; j < MAIL_MAX_HISTORY ; j++ ){
			// 未読メールがある時
			if( MailHistory[ i ].noReadFlag[ j ] >= TRUE ) return TRUE;
		}
	}
	return FALSE;
}

// 新規ペットチェック ****************************************************************/
void CheckNewPet( int sprNo )
{
	int tblNo = sprNo - 100250; // テーブルの番号
	int albumNo;				// アルバム番号
	
	// ペットのＳＰＲ番号だったら
	if( 0 <= tblNo && tblNo < MAX_PET_TBL ){
		// 今回登録するアルバムの番号
		albumNo = PetAlbumTbl[ tblNo ].albumNo;
		// テーブルにある時
		if( albumNo != -1 ){
			// フラグ立ってなかったら
			if( PetAlbum[ albumNo ].flag == 0 ){
				// 見たフラグＯＮ
				PetAlbum[ albumNo ].flag = 1;
				// 名前コピー
				strcpy( PetAlbum[ albumNo ].name, PetAlbumTbl[ tblNo ].name );
				// アルバムの保存
				SaveAlbum( albumNo );
			}
		}
	}
}

// 戦闘時の新規ペットチェック **********************************************************/
void CheckBattleNewPet( void )
{
	int i;
	
	// ２０体分ループ
	for( i = 0 ; i < 20 ; i++ ){
		// ポインタが無い時、戻る
		if( p_party[ i ] == NULL ) continue;
		// そこにいない時、戻る
		if( p_party[ i ]->func == NULL ) continue;
		// 新規ペットのチェック
		CheckNewPet( p_party[ i ]->anim_chr_no );
	}
}

// 文字列のセンターリング *****************************************************/
void CenteringStr( char *inStr, char *outStr, int max )
{
	int len, space, amari;
	
	// 長さを求める
	len = strlen( inStr );
	
	// リミットチェック
	if( len >= max ){ 
		// コピーするだけ
		strncpy( outStr, inStr, max );
		outStr[ max ] = NULL;	// 終端記号をいれる
		return;
	}
		
	// あまりを求める
	amari= ( max - len ) % 2;
	// 空白の数計算
	space = ( max - len ) / 2;
	
	// 最初に空白がある時
	if( space != 0 ){
		// 文字列作成
		sprintf( outStr, "%*c%s%*c", space, ' ', inStr, space + amari, ' ' );
	}else{
		// 文字列作成
		sprintf( outStr, "%s ", inStr );
	}
}

// 指定された技を持っているかチェックする *************************************/
int CheckPetSkill( int skillId )
{
	int i, j;
	
	// ペットの数分ループ
	for( j = 0 ; j < MAX_PET ; j++ ){
		// その欄にペットがいたら
		if( pet[ j ].useFlag == TRUE ){
			// 技数分ループ
			for( i = 0 ; i < pet[ j ].maxSkill ; i++ ){
				// その欄に技ある時
				if( petSkill[ j ][ i ].useFlag == TRUE ){
					// 技があったら
					if( petSkill[ j ][ i ].skillId == skillId ){
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}	
	
// アイテム関連の初期化 *******************************************************/
void InitItem( int x, int y )
{
	int i, j = 0, k = 0;
	
	// 装備品
	// 頭
	ItemBuffer[ 0 ].defX = x + 136;
	ItemBuffer[ 0 ].defY = y + 59;
	ItemBuffer[ 0 ].x = ItemBuffer[ 0 ].defX;
	ItemBuffer[ 0 ].y = ItemBuffer[ 0 ].defY;
	ItemBuffer[ 0 ].bmpNo = 20000 + 0;
	ItemBuffer[ 0 ].dispPrio = DISP_PRIO_ITEM;
	// 体
	ItemBuffer[ 1 ].defX = x + 134;
	ItemBuffer[ 1 ].defY = y + 129;
	ItemBuffer[ 1 ].x = ItemBuffer[ 1 ].defX;
	ItemBuffer[ 1 ].y = ItemBuffer[ 1 ].defY;
	ItemBuffer[ 1 ].bmpNo = 20200 + 1;
	ItemBuffer[ 1 ].dispPrio = DISP_PRIO_ITEM;
	// 手
	ItemBuffer[ 2 ].defX = x + 61;
	ItemBuffer[ 2 ].defY = y + 129;
	ItemBuffer[ 2 ].x = ItemBuffer[ 2 ].defX;
	ItemBuffer[ 2 ].y = ItemBuffer[ 2 ].defY;
	ItemBuffer[ 2 ].bmpNo = 20400 + 2;
	ItemBuffer[ 2 ].dispPrio = DISP_PRIO_ITEM;
	
	// アクセサリ１
	//ItemBuffer[ 3 ].defX = x + 32;
	ItemBuffer[ 3 ].defX = x + 32;
	ItemBuffer[ 3 ].defY = y + 68;
	ItemBuffer[ 3 ].x = ItemBuffer[ 3 ].defX;
	ItemBuffer[ 3 ].y = ItemBuffer[ 3 ].defY;
	ItemBuffer[ 3 ].bmpNo = 20600 + 3;
	ItemBuffer[ 3 ].dispPrio = DISP_PRIO_ITEM;
	// アクセサリ２
	//ItemBuffer[ 4 ].defX = x + 82;
	ItemBuffer[ 4 ].defX = x + 83;
	ItemBuffer[ 4 ].defY = y + 68;
	ItemBuffer[ 4 ].x = ItemBuffer[ 4 ].defX;
	ItemBuffer[ 4 ].y = ItemBuffer[ 4 ].defY;
	ItemBuffer[ 4 ].bmpNo = 20800 + 4;
	ItemBuffer[ 4 ].dispPrio = DISP_PRIO_ITEM;
	
	// アイテム数ループ
	for( i = 5 ; i < MAX_ITEM ; i++ ){
	
		ItemBuffer[ i ].defX = x + 32 + j;
		ItemBuffer[ i ].defY = y + 56 + 48 + 48 + 48 + k;
		ItemBuffer[ i ].x = ItemBuffer[ i ].defX;
		ItemBuffer[ i ].y = ItemBuffer[ i ].defY;
		ItemBuffer[ i ].bmpNo = 20000 + j / 48;
		ItemBuffer[ i ].dispPrio = DISP_PRIO_ITEM;
		ItemBuffer[ i ].mixFlag = FALSE;
		
		j += 51; // Ｘ座標ずらす
		// リミットチェック
		if( j >= 48 * 5 ){
			j = 0;
			k += 48; // Ｙ座標ずらす
		}
	}
}

// 当たり判定ボックス作成 *****每箱作成判定*************************************************/
BOOL MakeHitBox( int x1, int y1, int x2, int y2, int dispPrio )
{
	// 四角のあたり判定
	if( mouse.nowPoint.x <= x2 && x1 <= mouse.nowPoint.x &&
		mouse.nowPoint.y <= y2 && y1 <= mouse.nowPoint.y ){
		// ボックス表示する時
		if( dispPrio >= 0 ){
			// ボックス表示データをバッファに溜める
			StockBoxDispBuffer( x1, y1, x2, y2, dispPrio, BoxColor, 0 );
		}
		return TRUE;
	}
	return FALSE;
}

// 手紙アニメーション抹殺 **************************************************************/
void DeathLetterAction( void )
{
	int i;
	
	for( i = 0 ; i < 4 ; i++ ){
		DeathAction( pActLetter[ i ] );
		pActLetter[ i ] = NULL;
	}
}

// 送信相手フラグ初期化 ***************************************************************/
void InitMailSendFlag( void )
{
	int i;
	
	for( i = 0 ; i < MAX_ADR_BOOK ; i++ ) mailWndSendFlag[ i ] = 0;
}

// メニューアクション抹殺（左側） *****************************************************/
void DeathMenuAction( void )
{
	// ウィンドウ抹殺
	DeathAction( pActMenuWnd );
	pActMenuWnd = NULL;
	// ペットの名前変更ウィンドウ
	DeathAction( pActMenuWnd3 );
	pActMenuWnd3 = NULL;
	// 入力フォーカス戻す
	GetKeyInputFocus( &MyChatBuffer );
	// ペット抹殺
	DeathAction( pActPet );
	pActPet = NULL;
	// メールアイテム抹殺
	DeathAction( pActMailItem );
	pActMailItem = NULL;
	// 確認ウィンドウ抹殺
	DeathAction( pActYesNoWnd );
	pActYesNoWnd = NULL;
	// 手紙アニメーション抹殺
	DeathLetterAction();
		
}
// メニューアクション抹殺（右側） *****************************************************/
void DeathMenuAction2( void )
{
	// ウィンドウ抹殺
	DeathAction( pActMenuWnd2 );
	pActMenuWnd2 = NULL;
	// 合成ペット抹殺
	DeathAction( pActPet2 );
	pActPet2 = NULL;
	// 合成しているペット番号初期化
	ItemMixPetNo = -1;
}

// メニュー関連の初期化 *******************************************************/
void InitMenu( void )
{

	int i;
	
	// システム
	for( i = 0 ; i < MENU_SYSTEM_0 ; i++ ) systemWndFontNo[ i ] = -2;
	systemWndNo = 0;
	// マップ
	for( i = 0 ; i < MENU_MAP_0 ; i++ ) mapWndFontNo[ i ] = -2;
	mapWndNo = 0;
	// ステータス
	for( i = 0 ; i < MENU_STATUS_0 ; i++ ) statusWndFontNo[ i ] = -2;
	for( i = 0 ; i < MENU_STATUS_0 ; i++ ) statusWndBtnFlag[ i ] = 0;
	// ペット
	for( i = 0 ; i < MENU_PET_0 ; i++ ) petWndFontNo[ i ] = -2;
	petWndNo = 0;
	// アイテム
	for( i = 0 ; i < MENU_ITEM_0 ; i++ ) itemWndFontNo[ i ] = -2;
	for( i = 0 ; i < MENU_ITEM_0 ; i++ ) itemWndBtnFlag[ i ] = 0;
	itemWndNo = 0;
	// メール
	for( i = 0 ; i < MENU_MAIL_0 ; i++ ) mailWndFontNo[ i ] = -2;
	for( i = 0 ; i < MAX_ADR_BOOK ; i++ ) mailWndSendFlag[ i ] = 0;
	mailWndNo = MAIL_WND_VIEW;
	// アルバム
	for( i = 0 ; i < MENU_ALBUM_0 ; i++ ) albumWndFontNo[ i ] = -2;
	albumWndNo = 0;
	//BattleResultWndFlag = FALSE;
	// タスクバー
	for( i = 0 ; i < TASK_BAR ; i++ ) taskBarFontNo[ i ] = -2;
	// 戦闘結果ウィンドウ
	for( i = 0 ; i < MENU_BTL_RESULT_0 ; i++ ) resultWndFontNo[ i ] = -2;
	// トグルフラグ
	MenuToggleFlag = 0;
	// メニューアクション抹殺（左側）
	DeathMenuAction();
	// メニューアクション抹殺（右側）
	DeathMenuAction2();
	// ドラッグアイテム初期化
	mouse.itemNo = -1;
	// アイテム選択番号初期化
	itemNo = -1;
	// 選択している呪術初期化
	jujutuNo = -1;	
	// 合成ペット番号初期化
	ItemMixPetNo = -1;
}

// メニュー関連の初期化２ *******************************************************/
void InitMenu2( void )
{
	// トグルフラグ
	MenuToggleFlag = 0;
	// メニューアクション抹殺（左側）
	DeathMenuAction();
	// メニューアクション抹殺（右側）
	DeathMenuAction2();
	// ドラッグアイテム初期化
	mouse.itemNo = -1;
	// アイテム選択番号初期化
	itemNo = -1;
	// 選択している呪術初期化
	jujutuNo = -1;	
	// 戦闘結果ウィンドイウ初期化
	BattleResultWndFlag = FALSE;
}

// メニュー出てるかチェック *******************************************************/
BOOL CheckMenuFlag( void )
{
	BOOL flag = FALSE;
	
	// トグルフラグ
	if( MenuToggleFlag != 0 ) flag = TRUE;
	// 戦闘結果ウィンドイウ初期化
	if( BattleResultWndFlag >= 1 ) flag = TRUE;
	// マップウィンドウバックアップある時
	if( MapWmdFlagBak >= TRUE ) flag = TRUE;
	
	return flag;
}

// ****************************************************************************/
// メニュー処理 ***************************************************************/
// ****************************************************************************/
void MenuProc( void )
{
	int x, y, i;
	//int x0, y0;
	char moji[ 256 ];
	// 座標知るためのボックス
	//MakeHitBox( mouse.nowPoint.x - 24, mouse.nowPoint.y - 24, 
	//			mouse.nowPoint.x + 23, mouse.nowPoint.y + 23, DISP_PRIO_IME3 );
	
	// システムメニューとキャンセル
	if( joy_trg[ 0 ] & JOY_ESC && checkFieldMenuFlag() == FALSE ){
		// ＩＭＥバッファーに文字が無い時
		if( ImeBufferBak2 == NULL ){
			// キャンセルの時
			if( MenuToggleFlag != 0 || BattleResultWndFlag >= 1 ){ 
				MenuToggleFlag = 0;
				// 戦闘結果ウィンドイウ初期化
				BattleResultWndFlag = FALSE;
				// ドラッグアイテム初期化
				mouse.itemNo = -1;
				// アイテム選択番号初期化
				itemNo = -1;
				// 選択している呪術初期化
				jujutuNo = -1;	
				// マップウィンドウ出さない時
				if( MapWmdFlagBak != TRUE ){
					// ウィンドウ閉じる音
					play_se( 203, 320, 240 );
				}
			}else{  // システムメニューの時
				if( ImeBufferBak2 == '\0' ){ 
				
					MenuToggleFlag ^= JOY_ESC;	// ＥＳＣキー
					// ウィンドウ開く音
					play_se( 202, 320, 240 );
					// システム
					for( i = 0 ; i < MENU_SYSTEM_0 ; i++ ) systemWndFontNo[ i ] = -2;
					systemWndNo = 0;		// ウィンドウ番号初期化
				
					// ウィンドウ開く時
					if( MenuToggleFlag & JOY_ESC ){
						MenuToggleFlag &= JOY_CTRL_I | JOY_CTRL_M;	// アイテムとマップウィンドウの時だけ残す
						MenuToggleFlag |= JOY_ESC;	// ＥＳＣキー
						// 戦闘結果ウィンドイウ初期化
						BattleResultWndFlag = FALSE;
						// ウィンドウ開く音
						play_se( 202, 320, 240 );
					}else{
						// ウィンドウ閉じる音
						play_se( 203, 320, 240 );
					}
				}
			}
			// メニューアクション抹殺
			DeathMenuAction();
			DeathMenuAction2();
			// パラメータセーブ
			saveUserSetting();
		}
	}
	// システムメニュークリックの時
	if( TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[ 3 ] ){ 
			
		MenuToggleFlag ^= JOY_ESC;	// ＥＳＣキー
		// システム
		for( i = 0 ; i < MENU_SYSTEM_0 ; i++ ) systemWndFontNo[ i ] = -2;
		systemWndNo = 0;		// ウィンドウ番号初期化
		// メニューアクション抹殺
		DeathMenuAction();
		// パラメータセーブ
		saveUserSetting();
		
		// ウィンドウ開く時
		if( MenuToggleFlag & JOY_ESC ){
			MenuToggleFlag &= JOY_CTRL_I | JOY_CTRL_M;	// アイテムとマップウィンドウの時だけ残す
			MenuToggleFlag |= JOY_ESC;	// ＥＳＣキー
			// 戦闘結果ウィンドイウ初期化
			BattleResultWndFlag = FALSE;
			// ウィンドウ開く音
			play_se( 202, 320, 240 );
		}else{
			// ウィンドウ閉じる音
			play_se( 203, 320, 240 );
		}
	}
	
	// ステータスウィンドウ
	if( ( joy_trg[ 0 ] & JOY_CTRL_S && ImeBufferBak2 == '\0' ) 
		|| ( TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[ 1 ] ) ){ 
		
		// 戦闘中の時
		if( ProcNo == PROC_BATTLE ){
			// ＮＧ音
			play_se( 220, 320, 240 );
		}else{
		
			MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S キー
			// ステータス
			for( i = 0 ; i < MENU_STATUS_0 ; i++ ) statusWndFontNo[ i ] = -2;
			for( i = 0 ; i < MENU_STATUS_0 ; i++ ) statusWndBtnFlag[ i ] = 0;
			statusWndNo = 0;
			// メニューアクション抹殺
			DeathMenuAction();
			// パラメータセーブ
			saveUserSetting();
	
			// ウィンドウ開く時
			if( MenuToggleFlag & JOY_CTRL_S ){
				MenuToggleFlag &= JOY_CTRL_I | JOY_CTRL_M;	// アイテムとマップウィンドウの時だけ残す
				MenuToggleFlag |= JOY_CTRL_S;	// ステータスウィンドウ
				// 戦闘結果ウィンドイウ初期化
				BattleResultWndFlag = FALSE;
				// ウィンドウ開く音
				play_se( 202, 320, 240 );
			}else{
				// ウィンドウ閉じる音
				play_se( 203, 320, 240 );
			}
		}
	}
	
	// ペットウィンドウ
	if( ( joy_trg[ 0 ] & JOY_CTRL_P && ImeBufferBak2 == '\0' )
		|| ( TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[ 2 ] ) ){ 
		
		// 戦闘中の時又はエンカウント演出中の時
		//if( ProcNo == PROC_BATTLE || ( ProcNo == PROC_GAME && SubProcNo == 5 ){
		if( ProcNo == PROC_BATTLE ){
			// ＮＧ音
			play_se( 220, 320, 240 );
		}else{
			
			MenuToggleFlag ^= JOY_CTRL_P;	// CTRL + P キー
			// ペット
			for( i = 0 ; i < MENU_PET_0 ; i++ ) petWndFontNo[ i ] = -2;
			petWndNo = 0;		// ウィンドウ番号初期化
			
			// メニューアクション抹殺
			DeathMenuAction();
			// パラメータセーブ
			saveUserSetting();
			
			// ウィンドウ開く時
			if( MenuToggleFlag & JOY_CTRL_P ){
				MenuToggleFlag &= JOY_CTRL_I | JOY_CTRL_M;	// アイテムとマップウィンドウの時だけ残す
				MenuToggleFlag |= JOY_CTRL_P;
				// 戦闘結果ウィンドイウ初期化
				BattleResultWndFlag = FALSE;
				// ウィンドウ開く音
				play_se( 202, 320, 240 );
			}else{
				// ウィンドウ閉じる音
				play_se( 203, 320, 240 );
			}
		}
	}
	// アイテムウィンドウ
	if( ( joy_trg[ 0 ] & JOY_CTRL_I && ImeBufferBak2 == '\0' ) 
		|| ( TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[ 4 ] ) ){ 
		
		// 戦闘中の時
		if( ProcNo == PROC_BATTLE ){
			// ＮＧ音
			play_se( 220, 320, 240 );
		}else{
			
			MenuToggleFlag ^= JOY_CTRL_I;	// CTRL + I キー
			// アイテムフォント番号初期化
			for( i = 0 ; i < MENU_ITEM_0 ; i++ ) itemWndFontNo[ i ] = -2;
			for( i = 0 ; i < MENU_ITEM_0 ; i++ ) itemWndBtnFlag[ i ] = 0;
			itemWndNo = 0;
			mouse.itemNo = -1;
			itemWndDropGold = 0;
			
			// メニューアクション抹殺
			if( BattleResultWndFlag >= 1 ) DeathMenuAction();
			// メニューアクション抹殺
			DeathMenuAction2();
			// パラメータセーブ
			saveUserSetting();
			
			// ウィンドウ開く時
			if( MenuToggleFlag & JOY_CTRL_I ){
				MenuToggleFlag &= ~JOY_CTRL_M;	// マップウィンドウだけ消す
				MenuToggleFlag |= JOY_CTRL_I;
				// 戦闘結果ウィンドイウ初期化
				BattleResultWndFlag = FALSE;
				// バックアップ初期化
				MapWmdFlagBak = FALSE;
				// ウィンドウ開く音
				play_se( 202, 320, 240 );
			}else{
				// ウィンドウ閉じる音
				play_se( 203, 320, 240 );
			}
		}
	}
	
	// マップウィンドウ
	if( ( joy_trg[ 0 ] & JOY_CTRL_M && ImeBufferBak2 == '\0' ) 
		|| ( TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[ 0 ] )
		|| ( MapWmdFlagBak == TRUE && BattleResultWndFlag == FALSE && EncountFlag == FALSE && ProcNo == PROC_GAME && SubProcNo == 3 )
		){ 
		
		// 戦闘中の時
		if( ProcNo == PROC_BATTLE ){
			// ＮＧ音
			play_se( 220, 320, 240 );
		}else{
			
			MenuToggleFlag ^= JOY_CTRL_M;	// CTRL + M キー
			// ペット
			for( i = 0 ; i < MENU_PET_0 ; i++ ) petWndFontNo[ i ] = -2;
			mapWndNo = 0;		// ウィンドウ番号初期化
			
			// メニューアクション抹殺
			if( BattleResultWndFlag >= 1 ) DeathMenuAction();
			DeathMenuAction2();
			// パラメータセーブ
			//saveUserSetting();
			
			// ウィンドウ開く時
			if( MenuToggleFlag & JOY_CTRL_M ){
				MenuToggleFlag &= ~JOY_CTRL_I;	// アイテムウィンドウだけ消す
				MenuToggleFlag |= JOY_CTRL_M;
				// 戦闘結果ウィンドイウ初期化
				BattleResultWndFlag = FALSE;
				// バックアップ初期化
				MapWmdFlagBak = FALSE;
				// ウィンドウ開く音
				play_se( 202, 320, 240 );
			}else{
				// ウィンドウ閉じる音	
				play_se( 203, 320, 240 );
			}
		}
	}

	// メールウィンドウ
	if( ( joy_trg[ 0 ] & JOY_CTRL_E && ImeBufferBak2 == '\0' )
		|| ( TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[ 5 ] ) ){ 
		
		// 戦闘中の時
		if( ProcNo == PROC_BATTLE ){
			// ＮＧ音
			play_se( 220, 320, 240 );
		}else{
			
			MenuToggleFlag ^= JOY_CTRL_E;	// CTRL + E キー
			// ペット
			for( i = 0 ; i < MENU_MAIL_0 ; i++ ) mailWndFontNo[ i ] = -2;
			mailWndNo = MAIL_WND_VIEW;	// ウィンドウ番号初期化
			mailViewWndPageNo = 0;		// 名刺一覧ウィンドウのページ番号初期化
			mailItemNo = -1;			// ペットメールアイテム番号初期化
			InitMailSendFlag();			// 送信相手フラグ初期化
			// メニューアクション抹殺
			DeathMenuAction();
			// パラメータセーブ
			saveUserSetting();
			
			// ウィンドウ開く時
			if( MenuToggleFlag & JOY_CTRL_E ){
				MenuToggleFlag &= JOY_CTRL_I | JOY_CTRL_M;	// アイテムとマップウィンドウの時だけ残す
				MenuToggleFlag |= JOY_CTRL_E;
				// 戦闘結果ウィンドイウ初期化
				BattleResultWndFlag = FALSE;
				// ウィンドウ開く音
				play_se( 202, 320, 240 );
			}else{
				// ウィンドウ閉じる音
				play_se( 203, 320, 240 );
			}
		}
	}
	
//#ifdef _DEBUG
	// アルバムウィンドウ
	if( ( joy_trg[ 0 ] & JOY_CTRL_A && ImeBufferBak2 == '\0' )
	 	|| ( TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[ 6 ] ) ){
		
		// 戦闘中の時
		if( ProcNo == PROC_BATTLE ){
			// ＮＧ音
			play_se( 220, 320, 240 );
		}else{
			
			MenuToggleFlag ^= JOY_CTRL_A;	// CTRL + A キー
			// ペット
			for( i = 0 ; i < MENU_ALBUM_0 ; i++ ) albumWndFontNo[ i ] = -2;
			mapWndNo = 0;		// ウィンドウ番号初期化
			
			// メニューアクション抹殺
			DeathMenuAction();
			// パラメータセーブ
			saveUserSetting();
			
			// ウィンドウ開く時
			if( MenuToggleFlag & JOY_CTRL_A ){
				MenuToggleFlag &= JOY_CTRL_I | JOY_CTRL_M;	// アイテムとマップウィンドウの時だけ残す
				MenuToggleFlag |= JOY_CTRL_A;
				// 戦闘結果ウィンドイウ初期化
				BattleResultWndFlag = FALSE;
				// ウィンドウ開く音
				play_se( 202, 320, 240 );
			}else{
				// ウィンドウ閉じる音
				play_se( 203, 320, 240 );
			}
		}
	}
//#else
	// アルバムウィンドウ
//	if( joy_trg[ 0 ] & JOY_CTRL_A && ImeBufferBak2 == '\0' ||	
//		TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[ 6 ] ){ 
			// ＮＧ音
//			play_se( 220, 320, 240 );
//	}
//#endif

// タスクバー処理 *************************************************************/

	// 一行インフォ欄塗りつぶし（黒色）
	//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
	StockDispBuffer( TASK_BAR_X, TASK_BAR_Y, DISP_PRIO_MENU, CG_TASK_BAR_BACK, 1 );
	// タスクバーをスクロール
	if( mouse.nowPoint.y >= 456 ){
		// スクロール（出現）
		if( taskBarY > TASK_BAR_Y ) taskBarY--;
		// タスクバーフラグＯＮ
		TaskBarFlag = TRUE;
	}else{
		// タスクバー出ている時
		if( TaskBarFlag == TRUE ){
			// スクロール（隠れる）
			if( taskBarY < TASK_BAR_Y + 24 ) taskBarY++;
			// 文字の当たり判定初期化
			if( taskBarY == TASK_BAR_Y + 24 ){
				for( i = 0 ; i < TASK_BAR ; i++ ) albumWndFontNo[ i ] = -2;
				TaskBarFlag = FALSE;
			}
		}
	}
	
	// タスクバーを表示
	//if( taskBarY != 480 ){
	if( TaskBarFlag == TRUE ){
		x = taskBarX, y = taskBarY;
		
		taskBarFontNo[ 0 ] = StockDispBuffer( x, y, DISP_PRIO_IME3, CG_TASK_BAR_MAP_UP 		+ ( ( MenuToggleFlag & JOY_CTRL_M ) ? 1 : 0 ), 2 );
		taskBarFontNo[ 1 ] = StockDispBuffer( x, y, DISP_PRIO_IME3, CG_TASK_BAR_STATUS_UP 	+ ( ( MenuToggleFlag & JOY_CTRL_S ) ? 1 : 0 ), 2 );
		taskBarFontNo[ 2 ] = StockDispBuffer( x, y, DISP_PRIO_IME3, CG_TASK_BAR_PET_UP 		+ ( ( MenuToggleFlag & JOY_CTRL_P ) ? 1 : 0 ), 2 );
		taskBarFontNo[ 4 ] = StockDispBuffer( x, y, DISP_PRIO_IME3, CG_TASK_BAR_ITEM_UP 	+ ( ( MenuToggleFlag & JOY_CTRL_I ) ? 1 : 0 ), 2 );
		taskBarFontNo[ 5 ] = StockDispBuffer( x, y, DISP_PRIO_IME3, CG_TASK_BAR_MAIL_UP 	+ ( ( MenuToggleFlag & JOY_CTRL_E ) ? 1 : 0 ), 2 );
		taskBarFontNo[ 6 ] = StockDispBuffer( x, y, DISP_PRIO_IME3, CG_TASK_BAR_ALBUM_UP 	+ ( ( MenuToggleFlag & JOY_CTRL_A ) ? 1 : 0 ), 2 );
		taskBarFontNo[ 3 ] = StockDispBuffer( x, y, DISP_PRIO_IME3, CG_TASK_BAR_SYSTEM_UP 	+ ( ( MenuToggleFlag & JOY_ESC ) ? 1 : 0 ), 2 );
		
	}
	//else{
	//	// タスクバーフラグＯＦＦ
	//	TaskBarFlag = FALSE;
	//}
		
//◎ システムウィンドウ処理 *****************************************************/

	if( MenuToggleFlag & JOY_ESC ){
		// ウィンドウ番号で分岐
		switch( systemWndNo ){
		
		case 0:	// システム メニューの時
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 4, 4, 3, 6, CG_WND_TITLE_SYSTEM, 1 );
				for( i = 0 ; i < MENU_SYSTEM_0 ; i++ ) systemWndFontNo[ i ] = -2;
				break;
			}else{
				if( pActMenuWnd->hp <= 0 ) break;
			}
			
			// 左クリックした時
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				// ログアウトの時
				if( HitFontNo == systemWndFontNo[ 0 ] ){
					// ウィンドウ抹殺
					DeathAction( pActMenuWnd );
					pActMenuWnd = NULL;
					systemWndNo = 1;
					// ウィンドウ開く音
					play_se( 202, 320, 240 );
					break;
				}
				// チャット設定の時
				if( HitFontNo == systemWndFontNo[ 1 ] ){
					// ウィンドウ抹殺
					DeathAction( pActMenuWnd );
					pActMenuWnd = NULL;
					systemWndNo = 2;
					// ウィンドウ開く音
					play_se( 202, 320, 240 );
					break;
				}
				// ＢＧＭ設定の時
				if( HitFontNo == systemWndFontNo[ 4 ] ){
					// ウィンドウ抹殺
					DeathAction( pActMenuWnd );
					pActMenuWnd = NULL;
					systemWndNo = 4;
					// ウィンドウ開く音
					play_se( 202, 320, 240 );
					break;
				}
				// 効果音設定の時
				if( HitFontNo == systemWndFontNo[ 3 ] ){
					// ウィンドウ抹殺
					DeathAction( pActMenuWnd );
					pActMenuWnd = NULL;
					systemWndNo = 3;
					// ウィンドウ開く音
					play_se( 202, 320, 240 );
					break;
				}
				// 閉じる時
				if( HitFontNo == systemWndFontNo[ 2 ] ){
					// ウィンドウ抹殺
					DeathAction( pActMenuWnd );
					pActMenuWnd = NULL;
					// システムウィンドウ閉じる
					MenuToggleFlag ^= JOY_ESC;
					// ウィンドウ閉じる音
					play_se( 203, 320, 240 );
					break;
				}
			}
			// 座標設定
			x = pActMenuWnd->x + 28;
			y = pActMenuWnd->y + 56;
			
			// 文字表示バッファに溜める 文字表示缓冲蓄
			systemWndFontNo[ 0 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	" 　退出游戏　 ", 2 );	y += 40;
			systemWndFontNo[ 1 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	"　 聊天設定　", 2 );	y += 40;
			systemWndFontNo[ 4 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	" 　ＢＧＭ設定　 ", 2 );	y += 40;
			systemWndFontNo[ 3 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	" 　効果音設定　 ", 2 );	y += 52;
			systemWndFontNo[ 2 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, FONT_PAL_AQUA, 	"  　关 闭  　", 2 );	y += 40;
			
			break;
			
		case 1:	// ログアウト メニューの時
			
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 4, 4, 3, 3, CG_WND_TITLE_LOGOUT, 1 );
				for( i = 0 ; i < MENU_SYSTEM_0 ; i++ ) systemWndFontNo[ i ] = -2;
				break;
			}else{
				if( pActMenuWnd->hp <= 0 ) break;
			}
			
			// 左クリックした時
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				// はいの時
				if( HitFontNo == systemWndFontNo[ 0 ] ){
					// ウィンドウ抹殺
					DeathAction( pActMenuWnd );
					pActMenuWnd = NULL;
					GameState = GAME_LOGIN;
					ChangeProc( PROC_CHAR_LOGOUT );
					// デフォルトパレットに戻す
					//PaletteChange( DEF_PAL, 0 );
					// ログアウト音
					play_se( 206, 320, 240 );
					break;
				}
				// いいえの時
				if( HitFontNo == systemWndFontNo[ 1 ] ){
					// ウィンドウ抹殺
					DeathAction( pActMenuWnd );
					pActMenuWnd = NULL;
					// ウィンド閉じる
					systemWndNo = 0;	
					// クリック音
					//play_se( 217, 320, 240 );
					// ウィンドウ閉じる音
					play_se( 203, 320, 240 );
					break;
				}
			}
			
			// 座標設定
			x = pActMenuWnd->x + 28;
			y = pActMenuWnd->y + 56;
			
			// 文字表示バッファに溜める
			systemWndFontNo[ 0 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	" 　　退出游戏　　 ", 2 );	y += 40;
			systemWndFontNo[ 1 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	" 　　返回游戏　　 ", 2 );	y += 40;
			break;
			
		case 2:	// チャットの設定 メニューの時
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 4, 4, 4, 8, CG_WND_TITLE_CHAT, 1 );
				for( i = 0 ; i < MENU_SYSTEM_0 ; i++ ) systemWndFontNo[ i ] = -2;
				break;
			}else{
				if( pActMenuWnd->hp <= 0 ) break;
			}
			
			// 左クリックした時（オート）
			if( mouse.autoState & MOUSE_LEFT_CRICK ){
				// チャット表示の行数　増やす時
				if( HitFontNo == systemWndFontNo[ 0 ] ){
					// 行数変更
					NowMaxChatLine++;
					// リミットチェック
					if( NowMaxChatLine > MAX_CHAT_LINE ){ 
						NowMaxChatLine = MAX_CHAT_LINE;
						// ＮＧ音
						play_se( 220, 320, 240 );
					}else{
						// クリック音
						play_se( 217, 320, 240 );
					}
				}
				// チャット表示の行数　減らす時
				if( HitFontNo == systemWndFontNo[ 1 ] ){
					// 行数変更
					NowMaxChatLine--;
					// リミットチェック
					if( NowMaxChatLine <= -1 ){
						NowMaxChatLine = 0;
						// ＮＧ音
						play_se( 220, 320, 240 );
					}else{
						// クリック音
						play_se( 217, 320, 240 );
					}
				}
				// チャット文字の色設定の時
				if( HitFontNo == systemWndFontNo[ 2 ] ){
					// 色変更
					MyChatBuffer.color++;	
					// リミットチェック
					if( MyChatBuffer.color >= 10 ) MyChatBuffer.color = 0;
					// クリック音
					play_se( 217, 320, 240 );
				}
				
				// 文字登録
				if( HitFontNo == systemWndFontNo[ 4 ] ){
					// ウィンドウ抹殺
					DeathAction( pActMenuWnd );
					pActMenuWnd = NULL;
					systemWndNo = 5;
					// ウィンドウ開く音
					play_se( 202, 320, 240 );
					break;
				}
				// チャット声の大きさ　増やす時
				if( HitFontNo == systemWndFontNo[ 5 ] ){
					// 大きさ変更
					NowMaxVoice++;
					// リミットチェック
					if( NowMaxVoice > MAX_VOICE ){ 
						NowMaxVoice = MAX_VOICE;
						// ＮＧ音
						play_se( 220, 320, 240 );
					}else{
						// クリック音
						play_se( 217, 320, 240 );
					}
				}
				// チャット声の大きさ　減らす時
				if( HitFontNo == systemWndFontNo[ 6 ] ){
					// 大きさ変更
					NowMaxVoice--;
					// リミットチェック
					if( NowMaxVoice <= 0 ){
						NowMaxVoice = 1;
						// ＮＧ音
						play_se( 220, 320, 240 );
					}else{
						// クリック音
						play_se( 217, 320, 240 );
					}
				}
			}
			// 左クリックした時
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				// 戻る時
				if( HitFontNo == systemWndFontNo[ 3 ] ){
					// ウィンドウ抹殺
					DeathAction( pActMenuWnd );
					pActMenuWnd = NULL;
					// チャット設定ウィンド閉じる
					systemWndNo = 0;	
					// ウィンドウ閉じる音
					play_se( 203, 320, 240 );
					// パラメータセーブ
					saveUserSetting();
					break;
				}
			}
			// 座標設定
			x = pActMenuWnd->x + 48 - 5;
			y = pActMenuWnd->y + 56;
																	  sprintf( moji,"◆現在の表示行数%3d 行◆",NowMaxChatLine );
									StockFontBuffer( x - 16, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0 );				y += 32;
			systemWndFontNo[ 0 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 					" 　　増　や　す　　 ", 2 );	y += 32;
			systemWndFontNo[ 1 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 					" 　　減　ら　す　　 ", 2 );	y += 32;
			systemWndFontNo[ 2 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, MyChatBuffer.color, 	"　 　文字色変更　 　", 2 );	y += 32;
			systemWndFontNo[ 4 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0				, 	"　 　 文字登録　 　 ", 2 );	y += 44;
			
																	  sprintf( moji,"◆現在の声の大きさ%3d ◆",NowMaxVoice );
									StockFontBuffer( x - 16, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0 );				y += 32;
			systemWndFontNo[ 5 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 					" 　　大きくする　　 ", 2 );	y += 32;
			systemWndFontNo[ 6 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 					" 　　小さくする　　 ", 2 );	y += 44;
			
			systemWndFontNo[ 3 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, FONT_PAL_AQUA, 		"　　　戻　　る　　　", 2 );	y += 40;
			
			break;
			
		case 3:	// 効果音の設定 メニューの時
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 4, 4, 4, 6, CG_WND_TITLE_SE, 1 );
				for( i = 0 ; i < MENU_SYSTEM_0 ; i++ ) systemWndFontNo[ i ] = -2;
				break;
			}else{
				if( pActMenuWnd->hp <= 0 ) break;
			}
			
			// 左クリックした時（オート）
			if( mouse.autoState & MOUSE_LEFT_CRICK ){
				// 音量大きくする時
				if( HitFontNo == systemWndFontNo[ 0 ] ){
					// 音量変更
					t_music_se_volume++;
					// リミットチェック
					if( t_music_se_volume > 15 ){ 
						t_music_se_volume = 15;
						// ＮＧ音
						play_se( 220, 320, 240 );
					}else{
						// クリック音
						play_se( 217, 320, 240 );
					}
				}
				// 音量大きくする時
				if( HitFontNo == systemWndFontNo[ 1 ] ){
					// 音量変更
					t_music_se_volume--;
					// リミットチェック
					if( t_music_se_volume < 0 ){
						t_music_se_volume = 0;
						// ＮＧ音
						play_se( 220, 320, 240 );
					}else{
						// クリック音
						play_se( 217, 320, 240 );
					}
				}
				// ステレオとモノラルを切り替える時
				if( HitFontNo == systemWndFontNo[ 2 ] ){
					// ステレオモノラル変更
					stereo_flg = !stereo_flg;	
					// クリック音
					play_se( 217, 320, 240 );
				}
			}
			// 左クリックした時
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				// 戻る時
				if( HitFontNo == systemWndFontNo[ 3 ] ){
					// ウィンドウ抹殺
					DeathAction( pActMenuWnd );
					pActMenuWnd = NULL;
					// チャット設定ウィンド閉じる
					systemWndNo = 0;	
					// ウィンドウ閉じる音
					play_se( 203, 320, 240 );
					// パラメータセーブ
					saveUserSetting();
					break;
				}
			}
			// 座標設定
			x = pActMenuWnd->x + 48 - 5;
			y = pActMenuWnd->y + 64;
																	  sprintf( moji,"◆　現在的音量%3d　 ◆",t_music_se_volume );
									StockFontBuffer( x - 8, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0 );				y += 40;
			systemWndFontNo[ 0 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	" 　　加大音量　　 ", 2 );	y += 40;
			systemWndFontNo[ 1 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	" 　　减小音量　　 ", 2 );	y += 40;
			systemWndFontNo[ 2 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, monoStereoStr[ stereo_flg ], 2 );	y += 52;
			systemWndFontNo[ 3 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, FONT_PAL_AQUA, 	"　　　返　　回　　　", 2 );	y += 40;
			
			break;
			
		case 4:	// ＢＧＭの設定 メニューの時
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 4, 4, 4, 8, CG_WND_TITLE_BGM, 1 );
				for( i = 0 ; i < MENU_SYSTEM_0 ; i++ ) systemWndFontNo[ i ] = -2;
				break;
			}else{
				if( pActMenuWnd->hp <= 0 ) break;
			}
			
			// 左クリックした時（オート）
			if( mouse.autoState & MOUSE_LEFT_CRICK ){
				// 音量大きくする時
				if( HitFontNo == systemWndFontNo[ 0 ] ){
					// 音量変更
					t_music_bgm_volume++;
					// リミットチェック
					if( t_music_bgm_volume > 15 ){ 
						t_music_bgm_volume = 15;
						// ＮＧ音
						play_se( 220, 320, 240 );
					}else{
						play_se( 217, 320, 240 );	// クリック音
						bgm_volume_change();		// 音量変更
					}
				}
				// 音量小さくする時
				if( HitFontNo == systemWndFontNo[ 1 ] ){
					// 音量変更
					t_music_bgm_volume--;
					// リミットチェック
					if( t_music_bgm_volume < 0 ){
						t_music_bgm_volume = 0;
						// ＮＧ音
						play_se( 220, 320, 240 );	
					}else{
						play_se( 217, 320, 240 );	// クリック音
						bgm_volume_change();		// 音量変更
					}
				}
				// テンポを速くする時
				if( HitFontNo == systemWndFontNo[ 3 ] ){
					// テンポプラス
					t_music_bgm_pitch[t_music_bgm_no]++;
					set_gbm_pitch();
					// リミットチェック
					if( t_music_bgm_pitch[t_music_bgm_no] > 8 ){ 
						t_music_bgm_pitch[t_music_bgm_no] = 8;
						// ＮＧ音
						play_se( 220, 320, 240 );
					}else{
						play_se( 217, 320, 240 );	// クリック音
						set_gbm_pitch();			// テンポ変更
					}
				}
				// テンポを遅くする
				if( HitFontNo == systemWndFontNo[ 4 ] ){
					// テンポプラス
					t_music_bgm_pitch[t_music_bgm_no]--;
					// リミットチェック
					if( t_music_bgm_pitch[t_music_bgm_no] < -8 ){ 
						t_music_bgm_pitch[t_music_bgm_no] = -8;
						// ＮＧ音
						play_se( 220, 320, 240 );	
					}else{
						play_se( 217, 320, 240 );	// クリック音
						set_gbm_pitch();			// テンポ変更
					}
				}
			}
			// 左クリックした時
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				// 戻る時
				if( HitFontNo == systemWndFontNo[ 2 ] ){
					// ウィンドウ抹殺
					DeathAction( pActMenuWnd );
					pActMenuWnd = NULL;
					// チャット設定ウィンド閉じる
					systemWndNo = 0;	
					// ウィンドウ閉じる音
					play_se( 203, 320, 240 );
					// パラメータセーブ
					saveUserSetting();
					break;
				}
			}
			// 座標設定
			x = pActMenuWnd->x + 48 - 5;
			y = pActMenuWnd->y + 64;
																	  sprintf( moji,"◆　現在の音量 %3d　◆",t_music_bgm_volume );
									StockFontBuffer( x - 8, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0 );				y += 40;
			systemWndFontNo[ 0 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	" 　　大きくする　　 ", 2 );	y += 40;
			systemWndFontNo[ 1 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	" 　　小さくする　　 ", 2 );	y += 48;
			if( t_music_bgm_pitch[t_music_bgm_no] == 0 ){			  
																	  sprintf( moji,"◆ 現在のテンポ ±0" );
									StockFontBuffer( x - 8 + 173, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "◆", 0 );
			}
			else													  sprintf( moji,"◆ 現在のテンポ %+3d ◆",t_music_bgm_pitch[t_music_bgm_no] );
									StockFontBuffer( x - 8, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0 );				y += 40;
			systemWndFontNo[ 3 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	"　　　速くする　　　", 2 );	y += 40;
			systemWndFontNo[ 4 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	"　　　遅くする　　　", 2 );	y += 52;
			systemWndFontNo[ 2 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, FONT_PAL_AQUA, 	"　　　返   回　　　", 2 );	y += 40;
			
			break;
			
		case 5:	// 文字登録の時 文字登录时
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 4, 4, 272, 430, CG_WND_TITLE_CHAT, -1 );
				for( i = 0 ; i < MENU_SYSTEM_0 ; i++ ) systemWndFontNo[ i ] = -2;
				// 入力フォーカス変更
				GetKeyInputFocus( &chatRegistryStr[ 0 ] );
				
				break;
			}else{
				if( pActMenuWnd->hp <= 0 ) break;
			}
			
			// 文字登録ウィンドウ表示
			StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_MENU, CG_CHAT_REGISTY_WND, 1 );
			// 左クリックした時
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				// 戻る時
				if( HitDispNo == systemWndFontNo[ 3 ] ){
					// ウィンドウ抹殺
					DeathAction( pActMenuWnd );
					pActMenuWnd = NULL;
					// 入力フォーカス戻す
					GetKeyInputFocus( &MyChatBuffer );
					// チャット設定ウィンド閉じる
					systemWndNo = 0;	
					// ウィンドウ閉じる音
					play_se( 203, 320, 240 );
					// パラメータセーブ
					saveUserSetting();
					// チャットの登録文字の保存
					SaveChatRegistyStr();
					break;
				}
			}
			// 座標設定
			x = pActMenuWnd->x + 18;
			y = pActMenuWnd->y + 58;
			
			// 登録数分ループ
			for( i = 0 ; i < 8 ; i++ ){
				// 文字列の座標
				chatRegistryStr[ i ].x = x + 2;
				chatRegistryStr[ i ].y = y;
				StockFontBuffer2( &chatRegistryStr[ i ] ); y += 43;
				// 枠内に入っていたら
				if( MakeHitBox( chatRegistryStr[ i ].x - 4, chatRegistryStr[ i ].y - 3, 
					chatRegistryStr[ i ].x + 234 + 3, chatRegistryStr[ i ].y + 16 + 3, DISP_PRIO_IME3 ) == TRUE ){
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// 入力フォーカス変更
						GetKeyInputFocus( &chatRegistryStr[ i ] );
						play_se( 217, 320, 240 ); // クリック音
					}
				}
			}
			// 戻るボタン
			systemWndFontNo[ 3 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, 410, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );
			
			break;
		}
	}
	
//◎ ステータスウィンドウ処理 ******状态窗口处理***********************************************/

	if( MenuToggleFlag & JOY_CTRL_S ){
	
		// ウィンドウ番号で分岐
		switch( statusWndNo ){
		
		case 0:	// 自分のステータス
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				//pActMenuWnd = MakeWindowDisp( 4, 4, 4, 8, CG_WND_TITLE_SYSTEM, 1 );
				
				pActMenuWnd = MakeWindowDisp( 4, 4, 272, 360, 0, -1 );
				
				// 当たり判定初期化
				for( i = 0 ; i < MENU_STATUS_0 ; i++ ) statusWndFontNo[ i ] = -2;
				for( i = 0 ; i < MENU_STATUS_0 ; i++ ) statusWndBtnFlag[ i ] = 0;
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					// プレイヤーステータスウィンドウ表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_MENU, CG_STATUS_WND, 1 );
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// 仲間のステータス一覧ウィンドウの時
						if( HitDispNo == statusWndFontNo[ 0 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							statusWndNo = 1;
							// ウィンドウ開く音
							play_se( 202, 320, 240 );
						}
						// 閉じる時
						if( HitDispNo == statusWndFontNo[ 1 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// システムウィンドウ閉じる
							MenuToggleFlag ^= JOY_CTRL_S;
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
					}
					
					// 称号変更
					if( HitDispNo == statusWndFontNo[ 6 ] ){
						// 左クリックした時
						if( mouse.onceState & MOUSE_LEFT_CRICK ){
							if( pActMenuWnd3 == NULL ){
								// ウィンドウ表示タスク作成
								//pActMenuWnd3 = MakeWindowDisp( 4, 4 + 280 + 56, 4, 2, NULL, 1 );
								pActMenuWnd3 = MakeWindowDisp( 4, 16 + 280 + 70, 272, 88, 0, -1 );
								// ウィンドウ開く音
								play_se( 202, 320, 240 );
								// 名前をコピー
								//strcpy( shougouChange.buffer, pet[ petStatusNo ].name );
								shougouChange.buffer[ 0 ] = NULL;
								shougouChange.cnt = 0;
								// 文字列の長さ
								shougouChange.len = 12;
								// 文字列の色
								shougouChange.color = 0;
								// 文字列の座標
								shougouChange.x = pActMenuWnd3->x + 38;
								shougouChange.y = pActMenuWnd3->y + 25;
								// 文字列の表示優先順位
								shougouChange.fontPrio = FONT_PRIO_FRONT;
								// ボタン凹
								statusWndBtnFlag[ 6 ] = TRUE;
							}else{
								// ウィンドウ抹殺
								DeathAction( pActMenuWnd3 );
								pActMenuWnd3 = NULL;
								// 入力フォーカス戻す
								GetKeyInputFocus( &MyChatBuffer );
								// ウィンドウ閉じる音
								play_se( 203, 320, 240 );
							}
							statusWndBtnFlag[ 6 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && statusWndBtnFlag[ 6 ] == TRUE ){
							// ボタン凹
							statusWndBtnFlag[ 6 ] = TRUE;
						}else{
							// ボタン凸
							statusWndBtnFlag[ 6 ] = FALSE;
						}
					}else{
						// ボタン凸
						statusWndBtnFlag[ 6 ] = FALSE;
					}
					// レベルアップできる時
					if( StatusUpPoint != 0 ){
						// ボタンの数ループ
						for( i = 2 ; i < 6 ; i++ ){
							// 素早さアップ
							if( HitDispNo == statusWndFontNo[ i ] ){
								// ボタン凹
								// 左クリックした時
								if( mouse.onceState & MOUSE_LEFT_CRICK ){
									// スキルアップ送信
									lssproto_SKUP_send( sockfd, i - 2 );
									// カウントマイナス
									StatusUpPoint--;
									// レベルアップ音
									play_se( 211, 320, 240 );
									// ボタン凹
									statusWndBtnFlag[ i ] = TRUE;
								}
								// 左クリックしてる時
								if( mouse.state & MOUSE_LEFT_CRICK && statusWndBtnFlag[ i ] == TRUE ){
									// ボタン凹
									statusWndBtnFlag[ i ] = TRUE;
								}else{
									// ボタン凸
									statusWndBtnFlag[ i ] = FALSE;
								}
							}else{
								// ボタン凸
								statusWndBtnFlag[ i ] = FALSE;
							}
						}
					}
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
					
					#define PET_WND_ATTR_X 159
					#define PET_WND_ATTR_Y 5
					
						int x2; // メーターの長さ

						// 座標設定
						x = pActMenuWnd->x + 20;
						y = pActMenuWnd->y + 28;
						
						//sprintf( moji,"%s", pc.name );
						// 文字列のセンターリング
						CenteringStr( pc.name, moji, CHAR_NAME_LEN );
						//sprintf( moji,"0123456789012345", pc.name );
						StockFontBuffer( x - 6, y, FONT_PRIO_FRONT, 0, moji, 0 );
						sprintf( moji,"%8d", pc.dp );
						//sprintf( moji,"01234560", pc.name );
						StockFontBuffer( x + 170, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 21;
						//sprintf( moji,"%s", pc.freeName );
						// 文字列のセンターリング
						CenteringStr( pc.freeName, moji, CHAR_FREENAME_LEN );
						//sprintf( moji,"012345678901", pc.name );
						StockFontBuffer( x + 2, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 21;
						sprintf( moji,"%3d",pc.level );
						//sprintf( moji,"999", pc.name );
						StockFontBuffer( x + 50, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 21;
						sprintf( moji, "%7d", pc.exp );
						//sprintf( moji,"9999999", pc.name );
						StockFontBuffer( x + 57, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 21;
						sprintf( moji, "%7d",pc.maxExp );
						//sprintf( moji,"9999999", pc.name );
						StockFontBuffer( x + 57, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 21;
						sprintf( moji, "%4d", pc.hp );
						//sprintf( moji,"9999", pc.name );
						StockFontBuffer( x + 48, y, FONT_PRIO_FRONT, 0, moji, 0 );
						sprintf( moji, "%4d", pc.maxHp );
						//sprintf( moji,"9999", pc.name );
						StockFontBuffer( x + 98, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 21;
						sprintf( moji, "%3d", pc.mp );
						//sprintf( moji,"999", pc.name );
						StockFontBuffer( x + 50, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 21;
						
						sprintf( moji, "%3d", pc.atk );
						//sprintf( moji,"999", pc.name );
						StockFontBuffer( x + 50, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 21;
						sprintf( moji, "%3d", pc.def );
						//sprintf( moji,"999", pc.name );
						StockFontBuffer( x + 50, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 21;
						sprintf( moji, "%3d", pc.quick );
						//sprintf( moji,"999", pc.name );
						StockFontBuffer( x + 50, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 21;
						sprintf( moji, "%3d", pc.charm );
						//sprintf( moji,"999", pc.name );
						StockFontBuffer( x + 50, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 21;
						y += 29;
						sprintf( moji, "%3d", pc.vital );
						//sprintf( moji,"999", pc.name );
						StockFontBuffer( x + 61, y, FONT_PRIO_FRONT, 0, moji, 0 );
						sprintf( moji, "%3d", pc.str );
						//sprintf( moji,"999", pc.name );
						StockFontBuffer( x + 61 + 120, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 20;
						sprintf( moji, "%3d", pc.tgh );
						//sprintf( moji,"999", pc.name );
						StockFontBuffer( x + 61, y, FONT_PRIO_FRONT, 0, moji, 0 );
						sprintf( moji, "%3d", pc.dex );
						//sprintf( moji,"999", pc.name );
						StockFontBuffer( x + 61 + 120, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 20;
						
						y += 17;
						// グループ
						statusWndFontNo[ 0 ] = StockDispBuffer( x + 59, y, DISP_PRIO_IME3, CG_STATUS_WND_GROUP_BTN, 2 );
						// 閉じる
						statusWndFontNo[ 1 ] = StockDispBuffer( x + 173, y, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
						// 顔グラフィック表示
						StockDispBuffer( pActMenuWnd->x + 210, pActMenuWnd->y + 119, DISP_PRIO_IME3, pc.faceGraNo, 0 );
						
						// 連勝
						//StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_IME3, CG_STATUS_WND_VICTORY_MARK, 0 );
						// レベルアップできる時
						if( StatusUpPoint != 0 ){
							// レベルアップポイント
							StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_IME3, CG_STATUS_WND_LV_UP_POINT, 0 );
							sprintf( moji, "%2d", StatusUpPoint );
							StockFontBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx + 50, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my + 88, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0 );
							
							// アップボタン
							statusWndFontNo[ 2 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_IME3, CG_STATUS_WND_UP_BTN_UP + statusWndBtnFlag[ 2 ], 2 );
							statusWndFontNo[ 3 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx + 120, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_IME3, CG_STATUS_WND_UP_BTN_UP + statusWndBtnFlag[ 3 ], 2 );
							statusWndFontNo[ 4 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my + 20, DISP_PRIO_IME3, CG_STATUS_WND_UP_BTN_UP + statusWndBtnFlag[ 4 ], 2 );
							statusWndFontNo[ 5 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx + 120, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my + 20, DISP_PRIO_IME3, CG_STATUS_WND_UP_BTN_UP + statusWndBtnFlag[ 5 ], 2 );
						}
						// 称号変更
						statusWndFontNo[ 6 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_IME3, CG_STATUS_WND_SHOUGOU_BTN_UP + statusWndBtnFlag[ 6 ], 2 );
						
						// 座標設定
						x = pActMenuWnd->x + 19;
						y = pActMenuWnd->y + 174;
						
						// 地属性ある時
						if( pc.earth > 0 ){
							x2 = ( int )( pc.earth * 0.8 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 0, y + PET_WND_ATTR_Y + 0, x + PET_WND_ATTR_X + 0 + x2, y + PET_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_GREEN, 1 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 1, y + PET_WND_ATTR_Y + 1, x + PET_WND_ATTR_X + 1 + x2, y + PET_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_GREEN2, 0 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 2, y + PET_WND_ATTR_Y + 2, x + PET_WND_ATTR_X + 2 + x2, y + PET_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_GREEN2, 0 );
						}
						y += 20;
						
						// 水属性ある時
						if( pc.water > 0 ){
							x2 = ( int )( pc.water * 0.8 );
							
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 0, y + PET_WND_ATTR_Y + 0, x + PET_WND_ATTR_X + 0 + x2, y + PET_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_AQUA, 1 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 1, y + PET_WND_ATTR_Y + 1, x + PET_WND_ATTR_X + 1 + x2, y + PET_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_AQUA2, 0 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 2, y + PET_WND_ATTR_Y + 2, x + PET_WND_ATTR_X + 2 + x2, y + PET_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_AQUA2, 0 );
						}
						y += 20;
						
						// 火属性ある時
						if( pc.fire > 0 ){
							x2 = ( int )( pc.fire * 0.8 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 0, y + PET_WND_ATTR_Y + 0, x + PET_WND_ATTR_X + 0 + x2, y + PET_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_RED, 1 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 1, y + PET_WND_ATTR_Y + 1, x + PET_WND_ATTR_X + 1 + x2, y + PET_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_RED2, 0 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 2, y + PET_WND_ATTR_Y + 2, x + PET_WND_ATTR_X + 2 + x2, y + PET_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_RED2, 0 );
						}
						y += 20;
						
						// 風属性ある時
						if( pc.wind > 0 ){
							x2 = ( int )( pc.wind * 0.8 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 0, y + PET_WND_ATTR_Y + 0, x + PET_WND_ATTR_X + 0 + x2, y + PET_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_YELLOW, 1 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 1, y + PET_WND_ATTR_Y + 1, x + PET_WND_ATTR_X + 1 + x2, y + PET_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_YELLOW2, 0 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 2, y + PET_WND_ATTR_Y + 2, x + PET_WND_ATTR_X + 2 + x2, y + PET_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_YELLOW2, 0 );
						}
						
					}
				}
			}
			// 称号変更ウィンドウがある時
			if( pActMenuWnd3 != NULL ){
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd3->hp > 0 ){
					// ペット詳細ウィンドウ表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd3->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd3->pYobi )->my, DISP_PRIO_MENU, CG_NAME_CHANGE_WND, 1 );
					// 入力フォーカス取得
					GetKeyInputFocus( &shougouChange );
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// 称号変更ＯＫボタン
						if( HitDispNo == statusWndFontNo[ 7 ] ){
							// リターンキー処理 
							KeyboardReturn();
						}
						// 称号変更キャンセルボタン
						if( HitDispNo == statusWndFontNo[ 8 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd3 );
							pActMenuWnd3 = NULL;
							// 入力フォーカス戻す
							GetKeyInputFocus( &MyChatBuffer );
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
					}
					// ウィンドウが出来あがっていたら
					if(	pActMenuWnd3 != NULL ){
						// 文字列の表示
						StockFontBuffer2( &shougouChange );
						
						// 座標設定
						x = pActMenuWnd3->x + 20;
						y = pActMenuWnd3->y + 60;
						statusWndFontNo[ 7 ] = StockDispBuffer( x + 60, y + 7, DISP_PRIO_IME3, CG_OK_BTN, 2 );
						statusWndFontNo[ 8 ] = StockDispBuffer( x + 170, y + 7, DISP_PRIO_IME3, CG_CANCEL_BTN, 2 );
					}
				}
			}
			break;
			
		case 1:	// 仲間のステータス一覧  伙伴的地位一覧
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				//pActMenuWnd = MakeWindowDisp( 16, 16, 4, 9, CG_WND_TITLE_SYSTEM, 1 );
				pActMenuWnd = MakeWindowDisp( 4, 0, 256, 456, 0, -1 );
				// 当たり判定初期化
				for( i = 0 ; i < MENU_STATUS_0 ; i++ ) statusWndFontNo[ i ] = -2;
				for( i = 0 ; i < MENU_STATUS_0 ; i++ ) statusWndBtnFlag[ i ] = 0;
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					// ステータス一覧ウィンドウ表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_MENU, CG_STATUS_WND_GROUP_WND, 1 );
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// 戻る時
						if( HitDispNo == statusWndFontNo[ 11 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							statusWndNo = 0;
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
					}
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
						int selectFlag;	// 選択ボックス表示するかフラグ
						// 座標設定
						x = pActMenuWnd->x + 17;
						y = pActMenuWnd->y + 25;
						
						// 選択ボックス表示する時
						if( itemNo != -1 || jujutuNo != -1 ) selectFlag = 2;
						else selectFlag = FALSE;
						
						// 文字表示バッファに溜める
						// 自分の名前
						//sprintf( moji,"%-16s", pc.name );
						CenteringStr( pc.name, moji, CHAR_NAME_LEN );
						statusWndFontNo[ 0 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji,selectFlag ); y += 19;
						sprintf( moji,"%4d", pc.mp );
						StockFontBuffer( x + 77, y, FONT_PRIO_FRONT, 0, 	moji, 0 );
						sprintf( moji,"%4d %4d", pc.hp, pc.maxHp );
						StockFontBuffer( x + 142, y, FONT_PRIO_FRONT, 0, 	moji, 0 ); y += 20;
						// ペット
						for( i = 0 ; i < 5 ; i++ ){
							// その欄にペットがいたら
							if( pet[ i ].useFlag == TRUE ){
								if( pet[ i ].freeName[ 0 ] != NULL )
									//sprintf( moji,"%-16s", pet[ i ].freeName );
									CenteringStr( pet[ i ].freeName, moji, PET_NAME_LEN );
								else
									//sprintf( moji,"%-16s", pet[ i ].name );
									CenteringStr( pet[ i ].name, moji, PET_NAME_LEN );
								statusWndFontNo[ i + 1 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji,selectFlag ); y += 20;
								//sprintf( moji,"Hp：%4d", pet[ i ].hp );
								sprintf( moji,"%4d %4d", pet[ i ].hp, pet[ i ].maxHp );
								StockFontBuffer( x + 142, y, FONT_PRIO_FRONT, 0, 	moji, 0 ); y += 20;
							}
						}
						y = 268;
						// 仲間
						for( i = 0 ; i < 5 ; i++ ){
							// 自分でない時かつその欄に仲間がいる時
							if( pc.id != party[ i ].id && party[ i ].useFlag == TRUE ){
								//sprintf( moji,"%-16s", party[ i ].name );
								CenteringStr( party[ i ].name, moji, CHAR_NAME_LEN );
								//sprintf( moji,"AAAAA", party[ i ].name );
								statusWndFontNo[ i + 6 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji,selectFlag ); y += 20;
								sprintf( moji,"%4d", party[ i ].mp );
								StockFontBuffer( x + 77, y, FONT_PRIO_FRONT, 0, 	moji, 0 );
								sprintf( moji,"%4d %4d", party[ i ].hp, party[ i ].maxHp );
								StockFontBuffer( x + 142, y, FONT_PRIO_FRONT, 0, 	moji, 0 ); y += 20;
								
							//sprintf( moji,"%-16s", pc.name );
							//StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 21;
							//sprintf( moji,"%4d", pc.mp );
							//StockFontBuffer( x + 77, y, FONT_PRIO_FRONT, 0, 	moji, 0 );
							//sprintf( moji,"%4d %4d", pc.hp, pc.maxHp );
							//StockFontBuffer( x + 142, y, FONT_PRIO_FRONT, 0, 	moji, 0 ); y += 19;
								
								//sprintf( moji,"%17s Hp：%4d", party[ i ].name, party[ i ].hp );
								//itemWndFontNo[ i ] = StockFontBuffer( pActMenuWnd2->x + 64, pActMenuWnd2->y + 32 + 340 + 16, FONT_PRIO_FRONT, 0, moji, 2 );	y += 20;
							}
						}
						// 文字表示バッファに溜める
						statusWndFontNo[ 11 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, 441, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );
					}
				}
			}
			break;
		}
	}
	
//◎ ペットウィンドウ処理 ********宠物窗口处理************************************************/

	if( MenuToggleFlag & JOY_CTRL_P ){
		// ウィンドウ番号で分岐
		switch( petWndNo ){
		
		case 0:	// ペット一覧ウィンドウ
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				//pActMenuWnd = MakeWindowDisp( 16, 16, 4, 8, CG_WND_TITLE_SYSTEM, 1 );
				
				pActMenuWnd = MakeWindowDisp( 4, 4, 272, 320, 0, -1 );
				
				// 当たり判定初期化
				for( i = 0 ; i < MENU_PET_0 ; i++ ) petWndFontNo[ i ] = -2;
				// 受信番号初期化
				BattlePetReceivePetNo = -1;
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					// ペット一覧ウィンドウ表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_MENU, CG_PET_WND_VIEW, 1 );
					
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
					}
					// スタメンカウント初期化
					BattlePetStMenCnt = 0;
					// スタメンチェック
					for( i = 0 ; i < 5 ; i++ ){
						if( pc.selectPetNo[ i ] == TRUE ) BattlePetStMenCnt++;
					}
					
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// ペットの数分ループ
						for( i = 0 ; i < 5 ; i++ ){
							// この欄にペットがいる時かつ、受信中でない時
							if( pet[ i ].useFlag == TRUE && BattlePetReceivePetNo != i ){
								// ペット選択した時
								if( HitDispNo == petWndFontNo[ i ] ){
									// メールの時
									if( i == pc.mailPetNo ){
										// メールなしにする
										pc.mailPetNo = -1;
										play_se( 217, 320, 240 ); // クリック音
									}else
									// 先発の時
									if( i == pc.battlePetNo && BattlePetReceiveFlag == FALSE ){
										// 先発なしにする
										lssproto_KS_send( sockfd, -1 );
										// 受信待ちフラグＯＮ
										BattlePetReceiveFlag = TRUE;
										// バトル受信中のペット番号記憶
										BattlePetReceivePetNo = i;
										
										// ２軍にする
										pc.selectPetNo[ i ] = FALSE;
										BattlePetStMenCnt--; // カウンターマイナス
										//pc.battlePetNo = -1;
										// メールボタンない時
										if( pc.mailPetNo == -1 ){
											// メールにする
											pc.mailPetNo = i;
										}
										play_se( 217, 320, 240 ); // クリック音
									}else
									// スタメンの時
									if( pc.selectPetNo[ i ] == TRUE ){
										// 先発がいなかったら
										if( pc.battlePetNo == -1 && BattlePetReceiveFlag == FALSE ){
											// 死んで無い時
											if( pet[ i ].hp > 0 ){ 
												// 先発送信
												lssproto_KS_send( sockfd, i );
												// 受信待ちフラグＯＮ
												BattlePetReceiveFlag = TRUE;
												// バトル受信中のペット番号記憶
												BattlePetReceivePetNo = i;
												play_se( 217, 320, 240 ); // クリック音
											}else{
												play_se( 220, 320, 240 ); // ＮＧ音
											}
										}else{
											// メールボタンない時
											if( pc.mailPetNo == -1 ){
												pc.mailPetNo = i;
											}
											// ２軍にする
											pc.selectPetNo[ i ] = FALSE;
											BattlePetStMenCnt--; // カウンターマイナス
											play_se( 217, 320, 240 ); // クリック音
										}
									}else
									// ２軍の時
									if( pc.selectPetNo[ i ] == FALSE ){
										// ベンチが空いてたら
										if( BattlePetStMenCnt < 3 ){
											// スタメンにする
											pc.selectPetNo[ i ] = TRUE;
											BattlePetStMenCnt++; // カウンタープラス
											play_se( 217, 320, 240 ); // クリック音
										}else{
											// メールボタンない時
											if( pc.mailPetNo == -1 ){
												pc.mailPetNo = i;
												play_se( 217, 320, 240 ); // クリック音
											}else{
												play_se( 220, 320, 240 ); // ＮＧ音
											}
										}
									}
								}
								// 詳細ウィンドウ選択した時
								if( HitFontNo == petWndFontNo[ i + 5 ] ){
									petStatusNo = i; // ペット番号記憶
									petWndNo = 1;
									// ウィンドウ抹殺
									DeathAction( pActMenuWnd );
									pActMenuWnd = NULL;
									// ウィンドウ開く音
									play_se( 202, 320, 240 );
								}
								
							}
						}
						// 閉じる
						if( HitDispNo == petWndFontNo[ 10 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// ペットウィンドウ閉じる
							MenuToggleFlag ^= JOY_CTRL_P;
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
							// パラメータセーブ
							saveUserSetting();
						}
						// 詳細ボタン
						if( HitDispNo == petWndFontNo[ 11 ] ){
							petStatusNo = 0; // ペット番号記憶
							// ペットいるかチェック
							for( i = 0 ; i < 5 ; i++ ){
								// この欄にペットがいる時
								if( pet[ i ].useFlag == TRUE ){ 
									petStatusNo = i;
									break;
								}
							}
							petWndNo = 1;
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// ウィンドウ開く音
							play_se( 202, 320, 240 );
						}
					}
					// 右クリックした時
					if( mouse.onceState & MOUSE_RIGHT_CRICK ){
						// ペットの数分ループ
						for( i = 0 ; i < 5 ; i++ ){
							// この欄にペットがいる時
							if( pet[ i ].useFlag == TRUE && eventWarpSendFlag == FALSE ){
								// ペット選択した時
								if( HitFontNo == petWndFontNo[ i + 5 ] ){
									// ペットが合成中じゃ無い時
									if( ItemMixPetNo != i ){
										// ペットを置く送信
										lssproto_DP_send( sockfd, nowGx, nowGy, i );
										// スタメンの時
										if( pc.selectPetNo[ i ] == TRUE ){
											pc.selectPetNo[ i ] = FALSE; // ２軍落ち
											BattlePetStMenCnt--; // カウンターマイナス
										}
										// 先発の時
										if( i == pc.battlePetNo ){
											// 先発なしにする
											lssproto_KS_send( sockfd, -1 );
										}
										// メールの時
										if( pc.mailPetNo == i ){
											pc.mailPetNo = -1;
										}
										play_se( 217, 320, 240 ); // クリック音
									}else{
										// ＮＧ音
										play_se( 220, 320, 240 );
									}
								}
							}
						}
					}
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
					
						int flag = FALSE;
						int color;
						int btnNo;
						int atrFlag = FALSE;
						int atrGraNo[ 4 ];
						
						// 一行インフォ
						//strcpy( OneLineInfoStr, "ペットを選択してください。");
						// 座標設定
						x = pActMenuWnd->x + 16 + 50, y = pActMenuWnd->y + 31;
						// ペット分ループ
						for( i = 0 ; i < 5 ; i++ ){
							// この欄にペットがいる時
							if( pet[ i ].useFlag == TRUE ){
							
								color = FONT_PAL_WHITE;		// デフォルト色
								//color = FONT_PAL_RED;		// デフォルト色
								btnNo = 0;					// デフォルト休み
								// スタメン入りの時
								if( pc.selectPetNo[ i ] == TRUE ){ 
									color = FONT_PAL_AQUA; 		
									//color = FONT_PAL_WHITE; 		
									btnNo = 2;
								}
								// 先発の時
								if( i == pc.battlePetNo ){
									color = FONT_PAL_YELLOW;
									btnNo = 1;
								}
								// メールの時
								if( i == pc.mailPetNo ){
									//color = FONT_PAL_PURPLE;
									color = FONT_PAL_GREEN;
									btnNo = 3;
								}
								// 名前の文字列作成
								// あだ名がある時
								if( pet[ i ].freeName[ 0 ] != NULL ){
									//sprintf( moji, "%s", pet[ i ].freeName );
									// 文字列のセンターリング
									CenteringStr( pet[ i ].freeName, moji, PET_NAME_LEN );
								}else{
									//sprintf( moji, "%s", pet[ i ].name );
									// 文字列のセンターリング
									CenteringStr( pet[ i ].name, moji, PET_NAME_LEN );
								}
								petWndFontNo[ i + 5 ] = StockFontBuffer( x + 3, y, FONT_PRIO_FRONT, color, moji, 2 ); y += 24;
									
								atrFlag = FALSE;
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
								if( atrFlag > 0 ) StockDispBuffer( pActMenuWnd->x + 228, y - 16, DISP_PRIO_IME3, atrGraNo[ 0 ], 0 );
								// 二個目
								if( atrFlag > 1 ) StockDispBuffer( pActMenuWnd->x + 228 + 16 , y - 16, DISP_PRIO_IME3, atrGraNo[ 1 ], 0 );
								 
								//petWndFontNo[ i + 5 ] = StockFontBuffer( x + 132 + 24 , y, FONT_PRIO_FRONT, color, "ＳＴ", 2 );
								petWndFontNo[ i ] = StockDispBuffer( x - 27, y - 14, DISP_PRIO_IME3, CG_PET_WND_REST_BTN + btnNo, 2 );
								sprintf( moji, "%3d   %4d %4d", pet[ i ].level, pet[ i ].hp, pet[ i ].maxHp );
								//sprintf( moji, "999   9999 9999", pet[ i ].level, pet[ i ].hp );
								//sprintf( moji, "　　LV %2d  HP %4d/%4d", pet[ i ].level, pet[ i ].hp, pet[ i ].maxHp );
								StockFontBuffer( x + 26 + 27, y, FONT_PRIO_FRONT, color, 	moji, 0 ); y += 27;
								flag = TRUE;
							}
						}
						// ペットがいない時
						if( flag != TRUE ){ 
							// 閉じる表示
							petWndFontNo[ 10 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, pActMenuWnd->y + 299, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
							StockFontBuffer( x + 10, y, FONT_PRIO_FRONT, 0, "ペットを持ってません", 0 ); y += 40;
						}else{
							// 閉じる表示
							//petWndFontNo[ 10 ] = StockFontBuffer( x + 44, y, FONT_PRIO_FRONT, FONT_PAL_AQUA,  "　　閉 じ る　　", 2 );
							petWndFontNo[ 10 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx + 56, pActMenuWnd->y + 299, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
							petWndFontNo[ 11 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx - 56, pActMenuWnd->y + 299, DISP_PRIO_IME3, CG_PET_WND_STATUS_BTN, 2 );
						}
					}
				}
			}
			break;
			
		case 1: // ペットの詳細ウィンドウ
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				//pActMenuWnd = MakeWindowDisp( 16, 16, 4, 7, CG_WND_TITLE_SYSTEM, 1 );
				
				pActMenuWnd = MakeWindowDisp( 4, 4, 272, 332, 0, -1 );
				
				// 当たり判定初期化
				for( i = 0 ; i < MENU_PET_0 ; i++ ) petWndFontNo[ i ] = -2;
				
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					// ペット詳細ウィンドウ表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_MENU, CG_PET_WND_DETAIL, 1 );
					// ペットアクションアクションない時
					if( pActPet == NULL ){
						// ペットアクション作成
						pActPet = MakeAnimDisp( pActMenuWnd->x + 200, pActMenuWnd->y + 144, pet[ petStatusNo ].graNo, ANIM_DISP_PET );
					}
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// ペット一覧ウィンドウに戻る
						if( HitDispNo == petWndFontNo[ 0 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// ペット抹殺
							DeathAction( pActPet );
							pActPet = NULL;
							// ペット一覧ウィンドウにする
							petWndNo = 0;	
							// 名前変更ウィンドウ抹殺
							DeathAction( pActMenuWnd3 );
							pActMenuWnd3 = NULL;
							// 入力フォーカス戻す
							GetKeyInputFocus( &MyChatBuffer );
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
						// ペットの技ボタン
						if( HitDispNo == petWndFontNo[ 4 ] ){
							// 技ウィンドウ番号
							petWndNo = 2;
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// ペット抹殺
							DeathAction( pActPet );
							pActPet = NULL;
							// ペット一覧ウィンドウにする
							petWndNo = 2;	
							// 名前変更ウィンドウ抹殺
							DeathAction( pActMenuWnd3 );
							pActMenuWnd3 = NULL;
							// 入力フォーカス戻す
							GetKeyInputFocus( &MyChatBuffer );
							// ウィンドウ開く音
							play_se( 202, 320, 240 );
						}
					}
					// 前のペットのステータス
					if( HitDispNo == petWndFontNo[ 1 ] || joy_auto[ 0 ] & JOY_A ){
						// 左クリックした時
						//if( mouse.onceState & MOUSE_LEFT_CRICK ){
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_A ){
							// 名前変更ウィンドウがある時
							if( pActMenuWnd3 != NULL ){
								// ウィンドウ抹殺
								DeathAction( pActMenuWnd3 );
								pActMenuWnd3 = NULL;
								// 入力フォーカス戻す
								GetKeyInputFocus( &MyChatBuffer );
								// ウィンドウ閉じる音
								play_se( 203, 320, 240 );
							}
							while( 1 ){
								petStatusNo--;
								// リミットチェック
								if( petStatusNo <= -1 ) petStatusNo = 4;
								if( pet[ petStatusNo ].useFlag == TRUE ) break;
							}
							// アニメーション番号変更
							pActPet->anim_chr_no = pet[ petStatusNo ].graNo;
							// クリック音
							play_se( 217, 320, 240 ); 
							petWndBtnFlag[ 1 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && petWndBtnFlag[ 1 ] == TRUE ){
							// ボタン凹
							petWndBtnFlag[ 1 ] = TRUE;
						}else{
							// ボタン凸
							petWndBtnFlag[ 1 ] = FALSE;
						}
					}else{
						// ボタン凸
						petWndBtnFlag[ 1 ] = FALSE;
					}
					// 次のペットのステータス
					if( HitDispNo == petWndFontNo[ 2 ] || joy_auto[ 0 ] & JOY_B ){
						// 左クリックした時
						//if( mouse.onceState & MOUSE_LEFT_CRICK ){
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_B ){
							// 名前変更ウィンドウがある時
							if( pActMenuWnd3 != NULL ){
								// ウィンドウ抹殺
								DeathAction( pActMenuWnd3 );
								pActMenuWnd3 = NULL;
								// 入力フォーカス戻す
								GetKeyInputFocus( &MyChatBuffer );
								// ウィンドウ閉じる音
								play_se( 203, 320, 240 );
							}
							while( 1 ){
								petStatusNo++;
								// リミットチェック
								if( petStatusNo >= 5 ) petStatusNo = 0;
								if( pet[ petStatusNo ].useFlag == TRUE ) break;
							}
							// アニメーション番号変更
							pActPet->anim_chr_no = pet[ petStatusNo ].graNo;
							// クリック音
							play_se( 217, 320, 240 ); 
							petWndBtnFlag[ 2 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && petWndBtnFlag[ 2 ] == TRUE ){
							// ボタン凹
							petWndBtnFlag[ 2 ] = TRUE;
						}else{
							// ボタン凸
							petWndBtnFlag[ 2 ] = FALSE;
						}
					}else{
						// ボタン凸
						petWndBtnFlag[ 2 ] = FALSE;
					}
					// 名前変更ボタン
					if( HitDispNo == petWndFontNo[ 3 ] ){
						// 左クリックした時
						if( mouse.onceState & MOUSE_LEFT_CRICK ){
							// 名付け親の時（変更可能）
							if( pet[ petStatusNo ].changeNameFlag == TRUE ){
								if( pActMenuWnd3 == NULL ){
									// ウィンドウ表示タスク作成
									//pActMenuWnd3 = MakeWindowDisp( 16, 16 + 280 + 56, 4, 2, NULL, 1 );
									pActMenuWnd3 = MakeWindowDisp( 4, 4 + 280 + 56, 272, 88, 0, -1 );
									// ウィンドウ開く音
									play_se( 202, 320, 240 );
									// 名前をコピー
									//strcpy( petNameChange.buffer, pet[ petStatusNo ].name );
									petNameChange.buffer[ 0 ] = NULL;
									petNameChange.cnt = 0;
									// 文字列の長さ
									petNameChange.len = 16;
									// 文字列の色
									petNameChange.color = 0;
									// 文字列の座標
									petNameChange.x = pActMenuWnd3->x + 22;
									petNameChange.y = pActMenuWnd3->y + 25;
									// 文字列の表示優先順位
									petNameChange.fontPrio = FONT_PRIO_FRONT;
								}else{	// ウィンドウあったら閉じる
									// ウィンドウ抹殺
									DeathAction( pActMenuWnd3 );
									pActMenuWnd3 = NULL;
									// 入力フォーカス戻す
									GetKeyInputFocus( &MyChatBuffer );
									// ウィンドウ閉じる音
									play_se( 203, 320, 240 );
								}		
								petWndBtnFlag[ 3 ] = TRUE;
								
							}else{
								// ＮＧ音
								play_se( 220, 320, 240 );
							}
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && petWndBtnFlag[ 3 ] == TRUE ){
							// ボタン凹
							petWndBtnFlag[ 3 ] = TRUE;
						}else{
							// ボタン凸
							petWndBtnFlag[ 3 ] = FALSE;
						}
					}else{
						// ボタン凸
						petWndBtnFlag[ 3 ] = FALSE;
					}
					// 写真を撮るボタン
					if( HitDispNo == petWndFontNo[ 7 ] ){
						// 左クリックした時
						if( mouse.onceState & MOUSE_LEFT_CRICK ){
							int tblNo = pet[ petStatusNo ].graNo - 100250; // テーブルの番号
							int albumNo;
							// エラーチェック
							if( 0 <= tblNo && tblNo < MAX_PET_TBL ){
							// テーブル分ループ
							//for( tblNo = 0 ; tblNo < MAX_PET_TBL ; tblNo++ ){
								// 今回登録するアルバムの番号
								albumNo = PetAlbumTbl[ tblNo ].albumNo;
								// テーブルにある時
								if( albumNo != -1 ){
									// 写真撮ったフラグＯＮ
									PetAlbum[ albumNo ].flag = 2;
									
									// あだ名がある時
									if( pet[ petStatusNo ].freeName[ 0 ] != NULL ){
										strcpy( PetAlbum[ albumNo ].freeName, pet[ petStatusNo ].freeName );
									}else{
										// 本名コピー
										strcpy( PetAlbum[ albumNo ].freeName, PetAlbumTbl[ tblNo ].name );
									}
									// 本名コピー
									strcpy( PetAlbum[ albumNo ].name, PetAlbumTbl[ tblNo ].name );
									// 顔グラフィック番号
									PetAlbum[ albumNo ].faceGraNo = PetAlbumTbl[ tblNo ].faceGraNo;
									// レベル
									PetAlbum[ albumNo ].level = pet[ petStatusNo ].level;
									// 最大ＨＰ
									PetAlbum[ albumNo ].maxHp = pet[ petStatusNo ].maxHp;
									// 腕力
									PetAlbum[ albumNo ].str = pet[ petStatusNo ].atk;
									// 素早さ
									PetAlbum[ albumNo ].quick = pet[ petStatusNo ].quick;
									// 防御
									PetAlbum[ albumNo ].def = pet[ petStatusNo ].def;
									
									// 地
									PetAlbum[ albumNo ].earth = pet[ petStatusNo ].earth;
									// 水
									PetAlbum[ albumNo ].water = pet[ petStatusNo ].water;
									// 火
									PetAlbum[ albumNo ].fire = pet[ petStatusNo ].fire;
									// 風
									PetAlbum[ albumNo ].wind = pet[ petStatusNo ].wind;
								
									// 写真を撮る音
									play_se( 201, 320, 240 );
									// 一行インフォに表示
									sprintf( moji,"%s の写真を撮った！", PetAlbum[ albumNo ].freeName );
									// チャット文字をバッファに溜める（一行）
									StockChatBufferLine( moji, FONT_PAL_WHITE );
									
									petWndBtnFlag[ 7 ] = TRUE;
									// アルバムの保存 
									SaveAlbum( albumNo );
									
								}else{
									// ＮＧ音
									play_se( 220, 320, 240 );
#ifdef _DEBUG		
		sprintf( moji,"グラフィック番号がおかしいかも %d",pet[ petStatusNo ].graNo );
		MessageBox( hWnd, moji, "確認", MB_OK | MB_ICONSTOP );
#endif
							//	}
								}
							}else{
								// ＮＧ音
								play_se( 220, 320, 240 );
#ifdef _DEBUG		
		sprintf( moji,"ペットのテーブル番号がおかしいかも %d",tblNo );
		MessageBox( hWnd, moji, "確認", MB_OK | MB_ICONSTOP );
#endif
							}
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && petWndBtnFlag[ 7 ] == TRUE ){
							// ボタン凹
							petWndBtnFlag[ 7 ] = TRUE;
						}else{
							// ボタン凸
							petWndBtnFlag[ 7 ] = FALSE;
						}
					}else{
						// ボタン凸
						petWndBtnFlag[ 7 ] = FALSE;
					}
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
					
					#define PET_WND_ATTR_X 159
					#define PET_WND_ATTR_Y 5
					
						int x2; // メーターの長さ
						// 座標設定
						x = pActMenuWnd->x + 20;
						y = pActMenuWnd->y + 35;
						
						// あだ名がある時
						if( pet[ petStatusNo ].freeName[ 0 ] != NULL )
							//sprintf( moji,"%s", pet[ petStatusNo ].freeName );
							// 文字列のセンターリング
							CenteringStr( pet[ petStatusNo ].freeName, moji, PET_NAME_LEN );
						else
							//sprintf( moji,"%s", pet[ petStatusNo ].name );
							// 文字列のセンターリング
							CenteringStr( pet[ petStatusNo ].name, moji, PET_NAME_LEN );
						StockFontBuffer( x - 7, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24; 
						//StockFontBuffer( x - 7, y, FONT_PRIO_FRONT, 0, "世代表示", 0 ); 
						// 名前変更
						petWndFontNo[ 3 ] = StockDispBuffer( x + 66, y + 7, DISP_PRIO_IME3, CG_NAME_CHANGE_BTN + petWndBtnFlag[ 3 ], 2 );
						y += 23;
						sprintf( moji,"%3d",pet[ petStatusNo ].level );
						StockFontBuffer( x + 50, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
						sprintf( moji, "%7d", pet[ petStatusNo ].exp );
						//sprintf( moji, "99999999", pet[ petStatusNo ].exp );
						StockFontBuffer( x + 57, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
						sprintf( moji, "%7d", pet[ petStatusNo ].maxExp );
						//sprintf( moji, "99999999", pet[ petStatusNo ].exp );
						StockFontBuffer( x + 57, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
						
						//sprintf( moji, "%4d %4d", pet[ petStatusNo ].hp, pet[ petStatusNo ].maxHp );
						//StockFontBuffer( x + 48, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
						
						
						//sprintf( moji, "%4d %4d", pet[ petStatusNo ].hp, pet[ petStatusNo ].maxHp );
						sprintf( moji, "%4d", pet[ petStatusNo ].hp );
						StockFontBuffer( x + 46, y, FONT_PRIO_FRONT, 0, moji, 0 );
						sprintf( moji, "%4d", pet[ petStatusNo ].maxHp );
						StockFontBuffer( x + 48 + 45, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
						
						// 地属性ある時
						if( pet[ petStatusNo ].earth > 0 ){
							x2 = ( int )( pet[ petStatusNo ].earth * 0.8 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 0, y + PET_WND_ATTR_Y + 0, x + PET_WND_ATTR_X + 0 + x2, y + PET_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_GREEN, 1 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 1, y + PET_WND_ATTR_Y + 1, x + PET_WND_ATTR_X + 1 + x2, y + PET_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_GREEN2, 0 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 2, y + PET_WND_ATTR_Y + 2, x + PET_WND_ATTR_X + 2 + x2, y + PET_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_GREEN2, 0 );
							//StockBoxDispBuffer( x + 126, y + 5, x + 126 + x2earth, y + 13, DISP_PRIO_IME3, SYSTEM_PAL_BLACK, 0 );
						}
						sprintf( moji, "%3d", pet[ petStatusNo ].atk );
						StockFontBuffer( x + 50, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
						
						// 水属性ある時
						if( pet[ petStatusNo ].water > 0 ){
							x2 = ( int )( pet[ petStatusNo ].water * 0.8 );
							//StockBoxDispBuffer( x + PET_WND_ATTR_X + 0, y + PET_WND_ATTR_Y + 0, x + PET_WND_ATTR_X + 0 + x2, y + PET_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_BLUE, 1 );
							//StockBoxDispBuffer( x + PET_WND_ATTR_X + 1, y + PET_WND_ATTR_Y + 1, x + PET_WND_ATTR_X + 1 + x2, y + PET_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_BLUE2, 0 );
							//StockBoxDispBuffer( x + PET_WND_ATTR_X + 2, y + PET_WND_ATTR_Y + 2, x + PET_WND_ATTR_X + 2 + x2, y + PET_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_BLUE2, 0 );
							
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 0, y + PET_WND_ATTR_Y + 0, x + PET_WND_ATTR_X + 0 + x2, y + PET_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_AQUA, 1 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 1, y + PET_WND_ATTR_Y + 1, x + PET_WND_ATTR_X + 1 + x2, y + PET_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_AQUA2, 0 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 2, y + PET_WND_ATTR_Y + 2, x + PET_WND_ATTR_X + 2 + x2, y + PET_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_AQUA2, 0 );
							//StockBoxDispBuffer( x + 126, y + 5, x + 126 + x2water, y + 13, DISP_PRIO_IME3, SYSTEM_PAL_BLACK, 0 );
						}
						sprintf( moji, "%3d", pet[ petStatusNo ].def );
						StockFontBuffer( x + 50, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
						
						// 火属性ある時
						if( pet[ petStatusNo ].fire > 0 ){
							x2 = ( int )( pet[ petStatusNo ].fire * 0.8 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 0, y + PET_WND_ATTR_Y + 0, x + PET_WND_ATTR_X + 0 + x2, y + PET_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_RED, 1 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 1, y + PET_WND_ATTR_Y + 1, x + PET_WND_ATTR_X + 1 + x2, y + PET_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_RED2, 0 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 2, y + PET_WND_ATTR_Y + 2, x + PET_WND_ATTR_X + 2 + x2, y + PET_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_RED2, 0 );
							//StockBoxDispBuffer( x + 126, y + 5, x + 126 + x2fire, y + 13, DISP_PRIO_IME3, SYSTEM_PAL_BLACK, 0 );
						}
						sprintf( moji, "%3d", pet[ petStatusNo ].quick );
						StockFontBuffer( x + 50, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
						
						// 風属性ある時
						if( pet[ petStatusNo ].wind > 0 ){
							x2 = ( int )( pet[ petStatusNo ].wind * 0.8 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 0, y + PET_WND_ATTR_Y + 0, x + PET_WND_ATTR_X + 0 + x2, y + PET_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_YELLOW, 1 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 1, y + PET_WND_ATTR_Y + 1, x + PET_WND_ATTR_X + 1 + x2, y + PET_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_YELLOW2, 0 );
							StockBoxDispBuffer( x + PET_WND_ATTR_X + 2, y + PET_WND_ATTR_Y + 2, x + PET_WND_ATTR_X + 2 + x2, y + PET_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_YELLOW2, 0 );
							//StockBoxDispBuffer( x + 126, y + 5, x + 126 + x2wind, y + 13, DISP_PRIO_IME3, SYSTEM_PAL_BLACK, 0 );
						}
						sprintf( moji, "%3d", pet[ petStatusNo ].ai );
						StockFontBuffer( x + 50, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
						
					//	sprintf( moji, "Fire ：%4d  Water：%4d", pet[ petStatusNo ].fire, pet[ petStatusNo ].water );
					//	StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
					//	sprintf( moji, "Earth：%4d  Wind ：%4d", pet[ petStatusNo ].earth, pet[ petStatusNo ].wind );
					//	StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
						// 文字表示バッファに溜める
						y += 12;
						// 写真を撮るボタン
						petWndFontNo[ 7 ] = StockDispBuffer( x + 52, y, DISP_PRIO_IME3, CG_ALBUM_WND_SNAP_BTN_UP + petWndBtnFlag[ 7 ], 2 );
						petWndFontNo[ 4 ] = StockDispBuffer( x + 178, y, DISP_PRIO_IME3, CG_PET_WND_WAZA_BTN, 2 ); y += 31;
						petWndFontNo[ 0 ] = StockDispBuffer( x + 100 + 70, y, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );
						petWndFontNo[ 1 ] = StockDispBuffer( x + 16 + 16 + 8, y, DISP_PRIO_IME3, CG_PREV_BTN + petWndBtnFlag[ 1 ], 2 );
						petWndFontNo[ 2 ] = StockDispBuffer( x + 50 + 16 + 18, y, DISP_PRIO_IME3, CG_NEXT_BTN + petWndBtnFlag[ 2 ], 2 );
					}
				}
			}
			
			// 名前変更ウィンドウがある時
			if( pActMenuWnd3 != NULL ){
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd3->hp > 0 ){
					// ペット詳細ウィンドウ表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd3->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd3->pYobi )->my, DISP_PRIO_MENU, CG_NAME_CHANGE_WND, 1 );
					// 入力フォーカス取得
					GetKeyInputFocus( &petNameChange );
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// 名前変更ＯＫボタン
						if( HitDispNo == petWndFontNo[ 5 ] ){
							// リターンキー処理
							KeyboardReturn();
						
						}
						// 名前変更キャンセルボタン
						if( HitDispNo == petWndFontNo[ 6 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd3 );
							pActMenuWnd3 = NULL;
							// 入力フォーカス戻す
							GetKeyInputFocus( &MyChatBuffer );
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
					}
					// ウィンドウが出来あがっていたら
					if(	pActMenuWnd3 != NULL ){
						// 文字列の表示
						StockFontBuffer2( &petNameChange );
						
						// 座標設定
						x = pActMenuWnd3->x + 20;
						y = pActMenuWnd3->y + 60;
						//petWndFontNo[ 5 ] = StockFontBuffer( x + 8, y + 8, FONT_PRIO_FRONT, FONT_PAL_YELLOW, 	" ＯＫ ", 2 );
						//petWndFontNo[ 6 ] = StockFontBuffer( x + 100, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, 	" キャンセル ", 2 );
						petWndFontNo[ 5 ] = StockDispBuffer( x + 60, y + 7, DISP_PRIO_IME3, CG_OK_BTN, 2 );
						petWndFontNo[ 6 ] = StockDispBuffer( x + 170, y + 7, DISP_PRIO_IME3, CG_CANCEL_BTN, 2 );
					}
				}
			}
			break;
			
		case 2: // ペットの技ウィンドウ
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				//pActMenuWnd = MakeWindowDisp( 4, 4, 4, 7, CG_WND_TITLE_SYSTEM, 1 );
				pActMenuWnd = MakeWindowDisp( 4, 4, 272, 348, 0, -1 );
				// 当たり判定初期化
				for( i = 0 ; i < MENU_PET_0 ; i++ ) petWndFontNo[ i ] = -2;
				
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					// ペット技ウィンドウ
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_MENU, CG_PET_WAZA_WND, 1 );
					// 持てる技の数分ループ
					for( i = 0 ; i < pet[ petStatusNo ].maxSkill ; i++ ){
					//for( i = 0 ; i < 7 ; i++ ){
						// 技ベースを表示
						StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_IME3, CG_PET_WAZA_BAR_1 + i, 1 );
					}
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// ペット詳細ウィンドウに戻る
						if( HitDispNo == petWndFontNo[ 7 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// ペット一覧ウィンドウにする
							petWndNo = 1;
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
					}
					// 前のペットのステータス
					if( HitDispNo == petWndFontNo[ 8 ] || joy_auto[ 0 ] & JOY_A ){
						// 左クリックした時
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_A ){
							while( 1 ){
								petStatusNo--;
								// リミットチェック
								if( petStatusNo <= -1 ) petStatusNo = 4;
								if( pet[ petStatusNo ].useFlag == TRUE ) break;
							}
							// 当たり判定初期化
							for( i = 0 ; i < MENU_PET_0 ; i++ ) petWndFontNo[ i ] = -2;
							// クリック音
							play_se( 217, 320, 240 ); 
							petWndBtnFlag[ 8 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && petWndBtnFlag[ 8 ] == TRUE ){
							// ボタン凹
							petWndBtnFlag[ 8 ] = TRUE;
						}else{
							// ボタン凸
							petWndBtnFlag[ 8 ] = FALSE;
						}
					}else{
						// ボタン凸
						petWndBtnFlag[ 8 ] = FALSE;
					}
					// 次のペットのステータス
					if( HitDispNo == petWndFontNo[ 9 ] || joy_auto[ 0 ] & JOY_B ){
						// 左クリックした時
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_B ){
							while( 1 ){
								petStatusNo++;
								// リミットチェック
								if( petStatusNo >= 5 ) petStatusNo = 0;
								if( pet[ petStatusNo ].useFlag == TRUE ) break;
							}
							// 当たり判定初期化
							for( i = 0 ; i < MENU_PET_0 ; i++ ) petWndFontNo[ i ] = -2;
							// クリック音
							play_se( 217, 320, 240 ); 
							petWndBtnFlag[ 9 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && petWndBtnFlag[ 9 ] == TRUE ){
							// ボタン凹
							petWndBtnFlag[ 9 ] = TRUE;
						}else{
							// ボタン凸
							petWndBtnFlag[ 9 ] = FALSE;
						}
					}else{
						// ボタン凸
						petWndBtnFlag[ 9 ] = FALSE;
					}
					
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
						
						// 座標設定
						x = pActMenuWnd->x + 32;
						y = pActMenuWnd->y + 252;
						
						// 説明欄
						for( i = 0 ; i < pet[ petStatusNo ].maxSkill ; i++ ){
						//for( i = 0 ; i < 7 ; i++ ){
							// その欄に技ある時
							if( petSkill[ petStatusNo ][ i ].useFlag == TRUE ){
								// 当たっている時
								if( HitFontNo == petWndFontNo[ i ] ){
									// 改行ポイント
									char *splitPoint = petSkill[ petStatusNo ][ i ].memo;
									//char *splitPoint = "1表示テストです。私の名前は太田哲生です。これからもよろしくお願いします。";
									// 技の説明表示
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
#ifdef _DEBUG						
									// 右クリックした時
									if( mouse.onceState & MOUSE_RIGHT_CRICK ){
										// フィールドで使える時
										if( petSkill[ petStatusNo ][ i ].field != PETSKILL_FIELD_BATTLE ){
											int j;
											int cnt = 0;
											switch( petSkill[ petStatusNo ][ i ].skillId ){
											
												case PETSKILL_MERGE: 	// 普通アイテム合成
													// 二つ以上フラグが立っているかをチェック
													for( j = 5 ; j < 20 ; j++ ){
														if( ItemBuffer[ j ].mixFlag == 1 ) cnt++;
													}
													break;
													
												case PETSKILL_MERGE2: 	// 料理アイテム合成
													// 二つ以上フラグが立っているかをチェック
													for( j = 5 ; j < 20 ; j++ ){
														if( ItemBuffer[ j ].mixFlag == 2 ) cnt++;
													}
													break;
											}
											
											// 二つ以上立っていたら
											if( cnt >= 2 ){
												// 文字初期化
												moji[ 0 ] = NULL;
												// 合成するアイテムをチェック
												for( j = 5 ; j < 20 ; j++ ){
													// 合成フラグ立っている時
													if( ItemBuffer[ j ].mixFlag >= TRUE ){
														char work[ 256 ];
														// 文字列作成
														sprintf( work,"%d|", j );
														strcat( moji, work );
														// フラグＯＦＦ
														ItemBuffer[ j ].mixFlag = FALSE;
													}
												}
												// 最後の|をＮＵＬＬにする
												moji[ strlen( moji ) - 1 ] = NULL;
												
												// ペットスキル使うを送信（今は合成しかない）
												lssproto_PS_send( sockfd, petStatusNo, i, 0, moji );
												
												// アイテム使う音
												play_se( 212, 320, 240 );
											}else{
												// ＮＧ音
												play_se( 220, 320, 240 );
											}
										}else{
											// ＮＧ音
											play_se( 220, 320, 240 );
										}
									}
#endif									
									// 左クリックした時
									if( mouse.onceState & MOUSE_LEFT_CRICK ){
										// フィールドで使える、アイテムウィンドウが開いている、ペットアクションが無い時
										if( petSkill[ petStatusNo ][ i ].field != PETSKILL_FIELD_BATTLE && pActPet2 == NULL && MenuToggleFlag & JOY_CTRL_I ){
											int j;
											int cnt = 0;
											// 合成の種類で分岐
											switch( petSkill[ petStatusNo ][ i ].skillId ){
											
												case PETSKILL_MERGE: 	// 普通アイテム合成
													// 二つ以上フラグが立っているかをチェック
													for( j = 5 ; j < MAX_ITEM ; j++ ){
														if( ItemBuffer[ j ].mixFlag == 1 ) cnt++;
													}
													break;
													
												case PETSKILL_MERGE2: 	// 料理アイテム合成
													// 二つ以上フラグが立っているかをチェック
													for( j = 5 ; j < MAX_ITEM ; j++ ){
														if( ItemBuffer[ j ].mixFlag == 2 ) cnt++;
													}
													break;
											}
											
											// 二つ以上立っていたら
											if( cnt >= 2 ){
											
												// ペットアクションアクションない時
												if( pActPet2 == NULL ){
													// ペットアクション作成
													pActPet2 = MakeAnimDisp( 750, 240, pet[ petStatusNo ].graNo, ANIM_DISP_MIX_PET_INIT );
													// 合成ペット番号記憶
													ItemMixPetNo = petStatusNo;
												}
												// 技番号記憶
												SelectWazaNo = i;
												// アイテム使う音
												play_se( 212, 320, 240 );
											}else{
												// ＮＧ音
												play_se( 220, 320, 240 );
											}
										}else{
											// ＮＧ音
											play_se( 220, 320, 240 );
										}
									}
								}
							}
						}
						
						// 座標設定
						x = pActMenuWnd->x + 40;
						y = pActMenuWnd->y + 32;
						
						// あだ名がある時
						if( pet[ petStatusNo ].freeName[ 0 ] != NULL )
							//sprintf( moji,"%s", pet[ petStatusNo ].freeName );
							CenteringStr( pet[ petStatusNo ].freeName, moji, PET_NAME_LEN );
						else
							//sprintf( moji,"%s", pet[ petStatusNo ].name );
							CenteringStr( pet[ petStatusNo ].name, moji, PET_NAME_LEN );
						StockFontBuffer( x - 28, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 26; x += 18;
						
						// 技表示
						for( i = 0 ; i < pet[ petStatusNo ].maxSkill ; i++ ){
							// その欄に技ある時
							if( petSkill[ petStatusNo ][ i ].useFlag == TRUE ){
								int color = FONT_PAL_GRAY;
								sprintf( moji,"　　 %-22s",petSkill[ petStatusNo ][ i ].name );
								//CenteringStr( petSkill[ petStatusNo ][ i ].name, moji, SKILL_NAME_LEN );
								//sprintf( moji,"　　 %s",moji );
								//sprintf( moji,"ペットの技です。ああああ" );
								// フィールドで使える時
								if(	petSkill[ petStatusNo ][ i ].field != PETSKILL_FIELD_BATTLE ) color = FONT_PAL_WHITE;
								petWndFontNo[ i ] = StockFontBuffer( x - 43, y, FONT_PRIO_FRONT, color, moji, 2 ); 
							}
							y += 25;
						}
						
						// 座標設定
						x = pActMenuWnd->x + 20;
						y = pActMenuWnd->y + 330;
						
						// 文字表示バッファに溜める
						petWndFontNo[ 7 ] = StockDispBuffer( x + 100 + 70, y, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );
						petWndFontNo[ 8 ] = StockDispBuffer( x + 16 + 16 + 8, y, DISP_PRIO_IME3, CG_PREV_BTN + petWndBtnFlag[ 8 ], 2 );
						petWndFontNo[ 9 ] = StockDispBuffer( x + 50 + 16 + 18, y, DISP_PRIO_IME3, CG_NEXT_BTN + petWndBtnFlag[ 9 ], 2 );
						
						//petWndFontNo[ 7 ] = StockFontBuffer( x + 100, y, FONT_PRIO_FRONT, FONT_PAL_AQUA, 	"　戻　　る　", 2 );
						//petWndFontNo[ 8 ] = StockFontBuffer( x + 16, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, 	" ＜ ", 2 );
						//petWndFontNo[ 9 ] = StockFontBuffer( x + 50, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, 	" ＞ ", 2 ); y += 32;
					}
				}
			}
			break;
		}
	}
	
//◎ アイテムウィンドウ処理 ****处理物品窗口***************************************************/
	if( MenuToggleFlag & JOY_CTRL_I ){
		// ウィンドウ番号で分岐
		switch( itemWndNo ){
		
		case 0:	// アイテム　メニューの時
		
			// ウィンドウがない時
			if( pActMenuWnd2 == NULL ){
				// ウィンドウ表示タスク作成
				pActMenuWnd2 = MakeWindowDisp( 365, 4, 271, 440, 0, -1 );
				// アイテム関連の初期化
				InitItem( pActMenuWnd2->x, pActMenuWnd2->y );
				// 選択されたアイテム番号初期化
				itemNo = -1;
				// 合成ペット番号初期化
				ItemMixPetNo = -1;
				// ペットメールのアイテム選択ウィンドウの時
				if( MenuToggleFlag & JOY_CTRL_E && mailWndNo == MAIL_WND_ITEM ){
					// アイテムウィンドウ閉じる
					MenuToggleFlag &= ~JOY_CTRL_E;
					DeathMenuAction();
				}
				
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd2->hp > 0 ){
					// アイテムウィンドウ上表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->my, DISP_PRIO_MENU, CG_ITEM_WND_0, 1 );
					// アイテムウィンドウ下表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->my, DISP_PRIO_MENU, CG_ITEM_WND_1, 1 );
					
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// 閉じる時
						if( HitDispNo == itemWndFontNo[ 0 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd2 );
							pActMenuWnd2 = NULL;
							// ペット抹殺
							DeathAction( pActPet2 );
							pActPet2 = NULL;
							// 合成ペット番号初期化
							ItemMixPetNo = -1;
							// アイテムウィンドウ閉じる
							MenuToggleFlag ^= JOY_CTRL_I;
							// ステータスウィンドウ開いていたら閉じる
							if( MenuToggleFlag & JOY_CTRL_S ){
							
								MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S キー
								// メニューアクション抹殺
								DeathMenuAction();
							}
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
							// アイテム選択番号初期化
							itemNo = -1;
						}
						// 呪術ウィンドウの時
						//if( HitDispNo == itemWndFontNo[ 1 ] ){
						if( HitDispNo == itemWndFontNo[ 1 ] ){
							itemWndNo = 1;
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd2 );
							pActMenuWnd2 = NULL;
							// ペット抹殺
							DeathAction( pActPet2 );
							pActPet2 = NULL;
							// 合成ペット番号初期化
							ItemMixPetNo = -1;
							// ウィンドウ開く音
							play_se( 202, 320, 240 );
							
							// ステータスウィンドウ開いてなかったら
							if( !( MenuToggleFlag & JOY_CTRL_S ) ){
							
								MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S キー
								// ステータス
								for( i = 0 ; i < MENU_STATUS_0 ; i++ ) statusWndFontNo[ i ] = -2;
								for( i = 0 ; i < MENU_STATUS_0 ; i++ ) statusWndBtnFlag[ i ] = 0;
								statusWndNo = 1;
								// メニューアクション抹殺
								DeathMenuAction();
								// パラメータセーブ
								saveUserSetting();
								
								MenuToggleFlag &= JOY_CTRL_I;	// アイテムウィンドウの時だけ残す
								MenuToggleFlag |= JOY_CTRL_S;	// ステータスウィンドウ
							}else{
								// ステータスウィンドウで一覧でない時
								if( statusWndNo != 1 ){
									// ウィンドウ抹殺
									DeathAction( pActMenuWnd );
									pActMenuWnd = NULL;
									statusWndNo = 1;
								}
							}
						}
						// アイテム選択されてる時
						if( itemNo != -1 && MenuToggleFlag & JOY_CTRL_S && statusWndNo == 1 ){
							// 人数分ループ
							for( i = 0 ; i < 11 ; i++ ){
								// アイテム使う人選択
								if( HitFontNo == statusWndFontNo[ i ] && eventWarpSendFlag == FALSE ){
									//アイテム使う送信
									lssproto_ID_send( sockfd, nowGx, nowGy, itemNo, i );
									// アイテム使う音
									play_se( 212, 320, 240 );
									// アイテム選択番号初期化
									itemNo = -1;
									
								}
							}
						}
					}
					
					// お金の落とすボタンの時
					if( HitDispNo == itemWndFontNo[ 2 ] ){
						// 左クリックした時
						if( mouse.onceState & MOUSE_LEFT_CRICK ){
							// 落とすお金があったら
							if( itemWndDropGold > 0 && eventWarpSendFlag == FALSE ){
								// ボタン凹にする
								itemWndBtnFlag[ 2 ] = TRUE;
								// クリック音
								play_se( 217, 320, 240 ); 
							}else{
								// ＮＧ音
								play_se( 220, 320, 240 );
							}
						}
						// ボタン凹かつ左クリック離した時
						if( mouse.onceState & MOUSE_LEFT_CRICK_UP && itemWndBtnFlag[ 2 ] == TRUE ){
							itemWndBtnFlag[ 2 ] = FALSE;
							// アイテム使う音
							play_se( 212, 320, 240 );
							// お金落とす送信
							lssproto_DG_send( sockfd, nowGx, nowGy, itemWndDropGold );
							itemWndDropGold = 0;
						}
					}else{
						// ボタン凸にする
						itemWndBtnFlag[ 2 ] = FALSE;
					}
					
					// お金増やすボタンの時
					if( HitDispNo == itemWndFontNo[ 3 ] ){
						// ボタン凹かつ左クリック離した時
						if( mouse.onceState & MOUSE_LEFT_CRICK_UP && itemWndBtnFlag[ 3 ] == TRUE ){
							
							itemWndBtnFlag[ 3 ] = FALSE;
							// アイテム使う音
							//play_se( 212, 320, 240 );
						}
						// ボタン凹の時
						if( itemWndBtnFlag[ 3 ] == TRUE ){
							// プラス
							itemWndDropGold += itemWndDropGoldInc;
							// 捨てるお金増分カウンタープラス
							itemWndDropGoldCnt++;
							// 増分変更の時
							if( itemWndDropGoldCnt >= 30 ){ 
								// 捨てるお金増分カウンター初期化
								itemWndDropGoldCnt = 0;		
								// 最初だけ１にする
								if( itemWndDropGoldInc == 0 ) itemWndDropGoldInc = 1;
								else{
									// １０倍にする
									itemWndDropGoldInc *= 5;
									// リミットチェック
									if( itemWndDropGoldInc > 10000 ) itemWndDropGoldInc = 10000;		
								}
							}
							// リミットチェック
							if( itemWndDropGold >= pc.gold ){ 
								itemWndDropGold = pc.gold;
								// ＮＧ音
								play_se( 220, 320, 240 );
							}else{
								// クリック音
								//play_se( 217, 320, 240 ); 
							
							}
						}
						// 左クリックした時
						if( mouse.onceState & MOUSE_LEFT_CRICK ){
							// プラス
							itemWndDropGold++;
							// リミットチェック
							if( itemWndDropGold >= pc.gold ){ 
								itemWndDropGold = pc.gold;
								// ＮＧ音
								play_se( 220, 320, 240 );
							}else{
								// 捨てるお金増分
								itemWndDropGoldInc = 0;		
								// 捨てるお金増分カウンター初期化
								itemWndDropGoldCnt = 0;		
								// ボタン凹にする
								itemWndBtnFlag[ 3 ] = TRUE;
								// クリック音
								play_se( 217, 320, 240 ); 
							}
						}
						
					}else{
						// ボタン凸にする
						itemWndBtnFlag[ 3 ] = FALSE;
					}
					
					// お金減らすボタンの時
					if( HitDispNo == itemWndFontNo[ 4 ] ){
						// ボタン凹かつ左クリック離した時
						if( mouse.onceState & MOUSE_LEFT_CRICK_UP && itemWndBtnFlag[ 4 ] == TRUE ){
							itemWndBtnFlag[ 4 ] = FALSE;
							// アイテム使う音
							//play_se( 212, 320, 240 );
						}
						// ボタン凹の時
						if( itemWndBtnFlag[ 4 ] == TRUE ){
							// マイナス
							itemWndDropGold -= itemWndDropGoldInc;
							// 捨てるお金増分カウンタープラス
							itemWndDropGoldCnt++;
							// 増分変更の時
							if( itemWndDropGoldCnt >= 30 ){ 
								// 捨てるお金増分カウンター初期化
								itemWndDropGoldCnt = 0;		
								// 最初だけ１にする
								if( itemWndDropGoldInc == 0 ) itemWndDropGoldInc = 1;
								else{
									// １０倍にする
									itemWndDropGoldInc *= 5;
									// リミットチェック
									if( itemWndDropGoldInc > 10000 ) itemWndDropGoldInc = 10000;		
								}
							}
							// リミットチェック
							if( itemWndDropGold < 0 ){ 
								itemWndDropGold = 0;
								// ＮＧ音
								play_se( 220, 320, 240 );
							}else{
								// クリック音
								//play_se( 217, 320, 240 ); 
							
							}
						}
						// 左クリックした時
						if( mouse.onceState & MOUSE_LEFT_CRICK ){
							// マイナス
							itemWndDropGold--;
							// リミットチェック
							if( itemWndDropGold <= 0 ){ 
								itemWndDropGold = 0;
								// ＮＧ音
								play_se( 220, 320, 240 );
							}else{
								// 捨てるお金増分
								itemWndDropGoldInc = 0;		
								// 捨てるお金増分カウンター初期化
								itemWndDropGoldCnt = 0;		
								// ボタン凹にする
								itemWndBtnFlag[ 4 ] = TRUE;
								// クリック音
								play_se( 217, 320, 240 ); 
							
							}
						}
					}else{
						// ボタン凸にする
						itemWndBtnFlag[ 4 ] = FALSE;
					}
					// ウィンドウある時
					if( pActMenuWnd2 != NULL ){
					
						x = pActMenuWnd2->x + 16;
						y = pActMenuWnd2->y + 351 + 7;
						
						// アイテム選択されている時
						if( itemNo != -1 ){
							// その欄にアイテムがあったら
							//if( pc.item[ i ].useFlag == TRUE ){ 
								// ボックス表示データをバッファに溜める
								StockBoxDispBuffer( ItemBuffer[ itemNo ].defX - 24, ItemBuffer[ itemNo ].defY - 24,
													ItemBuffer[ itemNo ].defX + 26, ItemBuffer[ itemNo ].defY + 23, 
													DISP_PRIO_IME4, SYSTEM_PAL_AQUA, 0 );
							//}else{
								// アイテム選択番号初期化
							//	itemNo != -1;
							//}
						}
						// アイテムループ
						for( i = 0 ; i < MAX_ITEM ; i++ ){
							// 枠内に入っていたら
							if( MakeHitBox( ItemBuffer[ i ].defX - 24, ItemBuffer[ i ].defY - 24, 
											ItemBuffer[ i ].defX + 26, ItemBuffer[ i ].defY + 23, DISP_PRIO_IME3 ) == TRUE ){
								// その欄にアイテムがあったら
								if( pc.item[ i ].useFlag == TRUE && ItemBuffer[ i ].mixFlag <= 2 ){ 
									// 改行ポイント
									char *splitPoint = pc.item[ i ].memo;
									int color = pc.item[ i ].color;
									
									// 装備できるかチェック
									if( pc.level < pc.item[ i ].level ) color = FONT_PAL_RED;
									//char *splitPoint = "表示テストです。私の名前は太田哲生です。これからもよろしくお願いします。";
									// アイテムの名前表示
									//StockFontBuffer( pActMenuWnd2->x + 16, pActMenuWnd2->y + 331, FONT_PRIO_FRONT, pc.item[ i ].color, pc.item[ i ].name, 0 );	
									StockFontBuffer( pActMenuWnd2->x + 16, pActMenuWnd2->y + 331, FONT_PRIO_FRONT, color, pc.item[ i ].name, 0 );	
									//StockFontBuffer( pActMenuWnd2->x + 16, pActMenuWnd2->y + 331, FONT_PRIO_FRONT, pc.item[ i ].color, pc.item[ i ].name, 0 );	
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
								// 合成中でなかったら
								if( pActPet2 == NULL ){
								
								// アイテムをドラッグしてなくて左クリックが押されていてマウスを動かした時
								if( mouse.onceState & MOUSE_LEFT_CRICK && mouse.itemNo == -1 ){
									// 合成フラグＯＦＦにする
									ItemBuffer[ i ].mixFlag = FALSE;
									// その欄にアイテムがあったら
									if( pc.item[ i ].useFlag == TRUE ){
										// ドラッグフラグＯＮ
										ItemBuffer[ i ].dragFlag = TRUE;
										mouse.itemNo = i;
										// アイテム選択番号初期化
										itemNo = -1;
									}
								}
								// 左クリック離した時
								if( mouse.onceState & MOUSE_LEFT_CRICK_UP ){
									// 何かドラッグしている時、交換する
									if( mouse.itemNo != -1 ){
										// 交換の時
										if( mouse.itemNo != i ){
											// 交換の送信
											lssproto_MI_send( sockfd, mouse.itemNo, i );
											// クリック音
											play_se( 217, 320, 240 );
										}
										// ドラッグフラグ初期化
										ItemBuffer[ mouse.itemNo ].dragFlag = FALSE;
										// マウスのアイテム番号初期化
										mouse.itemNo = -1; 
									}
								}
								// 左ダブルクリックでアイテム使用
								if( mouse.onceState & MOUSE_LEFT_DBL_CRICK ){
									// その欄にアイテムが無いかつフィールド上で使えない時
									if( pc.item[ i ].useFlag != TRUE || pc.item[ i ].field == ITEM_FIELD_BATTLE || pc.level < pc.item[ i ].level ){
										// ＮＧ音
										play_se( 220, 320, 240 );
									}else{ // 使用可能な時
										// ドラッグアイテム初期化
										mouse.itemNo = -1;
										// ターゲットで分岐
										switch( pc.item[ i ].target ){
											
										case ITEM_TARGET_MYSELF:	// 自分のみ
										
											// フラグＯＦＦの時
											if( eventWarpSendFlag == FALSE ){
												//アイテム使う送信
												lssproto_ID_send( sockfd, nowGx, nowGy, i, 0 );
												// アイテム使う音
												play_se( 212, 320, 240 );
											}
											
											break;
											
										case ITEM_TARGET_OTHER:	// 他の人（自分含む)
										
											// ターゲット選択ウィンドウへ
											//itemWndNo = 2;
											// ウィンドウ抹殺
										//	DeathAction( pActMenuWnd2 );
										//	pActMenuWnd2 = NULL;
											// ウィンドウ開く音
										//	play_se( 202, 320, 240 );
											// 選択されたアイテム番号記憶
											itemNo = i;
											// クリック音
											play_se( 217, 320, 240 );
											//jujutuNo = -1;
											
											// ステータスウィンドウ開いてなかったら
											if( !( MenuToggleFlag & JOY_CTRL_S ) ){
											
												MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S キー
												// ステータス
												for( i = 0 ; i < MENU_STATUS_0 ; i++ ) statusWndFontNo[ i ] = -2;
												for( i = 0 ; i < MENU_STATUS_0 ; i++ ) statusWndBtnFlag[ i ] = 0;
												statusWndNo = 1;
												// メニューアクション抹殺
												DeathMenuAction();
												// パラメータセーブ
												saveUserSetting();
												// ウィンドウ開く音
												play_se( 202, 320, 240 );
												
												MenuToggleFlag &= JOY_CTRL_I;	// アイテムウィンドウの時だけ残す
												MenuToggleFlag |= JOY_CTRL_S;	// ステータスウィンドウ
											}else{
												// ステータスウィンドウで一覧でない時
												if( statusWndNo != 1 ){
													// ウィンドウ抹殺
													DeathAction( pActMenuWnd );
													pActMenuWnd = NULL;
													statusWndNo = 1;
													// ウィンドウ開く音
													play_se( 202, 320, 240 );
												}
											}
											
											break;
#if 0											
										case ITEM_TARGET_ALLMYSIDE:	// 味方全体
										
											//アイテム使う送信
											lssproto_ID_send( sockfd, nowGx, nowGy, i, 0 );
											// アイテム使う音
											play_se( 212, 320, 240 );
											break;
											
										case ITEM_TARGET_ALLOTHERSIDE:	// 相手側全体
										
											//アイテム使う送信
											lssproto_ID_send( sockfd, nowGx, nowGy, i, 0 );
											// アイテム使う音
											play_se( 212, 320, 240 );
											break;
											
										case ITEM_TARGET_ALL:	// 全て
										
											//アイテム使う送信
											lssproto_ID_send( sockfd, nowGx, nowGy, i, 0 );
											// アイテム使う音
											play_se( 212, 320, 240 );
											break;
#endif
										}
									}
								}
//#ifdef _DEBUG		
								// アイテムドラッグしてないかつ、右クリックで合成表示
								if( mouse.onceState & MOUSE_RIGHT_CRICK  && mouse.itemNo == -1 ){
									int j;
									int cnt = 0;
									// その欄にアイテムがある時かつ、かつ装備してない時、合成できる時
									if( pc.item[ i ].useFlag == TRUE && i >= 5 && pc.item[ i ].sendFlag & ITEM_FLAG_MIX ){
										int j;
										int flag = 0;
										// 選択アイテム番号初期化
										if( itemNo != -1 ) itemNo = -1;
										// 何合成かを調べる
										for( j = 5 ; j < 20 ; j++ ){
											// 合成フラグ立っている時
											if( ItemBuffer[ j ].mixFlag >= TRUE ){
												flag = ItemBuffer[ j ].mixFlag;
												break;
											}
										}
										// 合成フラグＯＦＦの時
										if( ItemBuffer[ i ].mixFlag == FALSE ){
											// まだ何も選択されてなかったら
											if( flag == 0 ){
												// 料理の時
												if( pc.item[ i ].sendFlag & ITEM_FLAG_COOKING_MIX ){
													// 技を持っているかチェックする
													if( CheckPetSkill( PETSKILL_MERGE2 ) == TRUE ){
														ItemBuffer[ i ].mixFlag = 2;
														// クリック音
														play_se( 217, 320, 240 );
													}else{
														// ＮＧ音
														play_se( 220, 320, 240 );
													}
												}
												// 普通合成の時
												else{ 
													// 技を持っているかチェックする
													if( CheckPetSkill( PETSKILL_MERGE ) == TRUE ){
														ItemBuffer[ i ].mixFlag = 1;
														// クリック音
														play_se( 217, 320, 240 );
													}else{
														// ＮＧ音
														play_se( 220, 320, 240 );
													}
												}
											}else{
											
												// 料理の時
												if( pc.item[ i ].sendFlag & ITEM_FLAG_COOKING_MIX ){
													if( flag == 2 ){
														ItemBuffer[ i ].mixFlag = 2;
														// クリック音
														play_se( 217, 320, 240 );
													}else{
														// ＮＧ音
														play_se( 220, 320, 240 );
													}
													
												}
												// 普通合成の時
												else{
													if( flag == 1 ){
														ItemBuffer[ i ].mixFlag = 1;
														// クリック音
														play_se( 217, 320, 240 );
													}else{
														// ＮＧ音
														play_se( 220, 320, 240 );
													}
													
												}
											}
										}else{
											ItemBuffer[ i ].mixFlag = FALSE;
											// クリック音
											play_se( 217, 320, 240 );
										}
									}else{
										// ＮＧ音
										play_se( 220, 320, 240 );
									}
									// 二つ以上フラグが立っているかをチェック
									for( j = 5 ; j < 20 ; j++ ){
										if( ItemBuffer[ j ].mixFlag >= 1 ) cnt++;
									}
									// 二つ以上立っていたら
									if( cnt >= 2 ){
										// ステータスウィンドウ開いてなかったら
										if( !( MenuToggleFlag & JOY_CTRL_P ) ){
										
											MenuToggleFlag ^= JOY_CTRL_P;	// CTRL + S キー
											// ステータス
											for( j = 0 ; j < MENU_PET_0 ; j++ ) petWndFontNo[ j ] = -2;
											for( j = 0 ; j < MENU_PET_0 ; j++ ) petWndBtnFlag[ j ] = 0;
											petWndNo = 0;
											// メニューアクション抹殺
											DeathMenuAction();
											// パラメータセーブ
											saveUserSetting();
											// ウィンドウ開く音
											play_se( 202, 320, 240 );
											
											MenuToggleFlag &= JOY_CTRL_I;	// アイテムウィンドウの時だけ残す
											MenuToggleFlag |= JOY_CTRL_P;	// ステータスウィンドウ
										}
										
									//	else{
											// ステータスウィンドウで一覧でない時
									//		if( petWndNo != 0 ){
									//			// ウィンドウ抹殺
									//			DeathAction( pActMenuWnd );
									//			pActMenuWnd = NULL;
									//			petWndNo = 0;
												// ウィンドウ開く音
									//			play_se( 202, 320, 240 );
									//		}
									//	}
									}
								}
//#endif
								}
							}
							// その欄にアイテムがあったら
							//if( pc.item[ i ].useFlag == TRUE && ItemBuffer[ i ].dragFlag == FALSE ){ 
							if( pc.item[ i ].useFlag == TRUE ){
								// 合成中で無い時
								if( ItemBuffer[ i ].mixFlag <= 2 ){
									// アイテム表示
									StockDispBuffer( ItemBuffer[ i ].defX, ItemBuffer[ i ].defY, ItemBuffer[ i ].dispPrio, pc.item[ i ].graNo, 0 );
								}else
								// ペット食べ歩き中
								if( ItemBuffer[ i ].mixFlag == 3 ){
									// アイテム表示
									StockDispBuffer( pActPet2->x, pActPet2->y - 16, DISP_PRIO_DRAG, pc.item[ i ].graNo, 0 );
								}else
								// もじもじ中（合成）
								if( ItemBuffer[ i ].mixFlag == 4 ){
									// アイテム表示
									StockDispBuffer( pActPet2->x, pActPet2->y, ItemBuffer[ i ].dispPrio, pc.item[ i ].graNo, 0 );
								}
							}
							// 合成文字表示
							if( ItemBuffer[ i ].mixFlag >= TRUE ){ 
								// その欄にアイテムがあったら
								if( pc.item[ i ].useFlag == TRUE ){ 
									// 普通合成の時
									if( ItemBuffer[ i ].mixFlag == 1 ){
										// 合成
										StockFontBuffer( ItemBuffer[ i ].defX - 17, ItemBuffer[ i ].defY, FONT_PRIO_FRONT, 0, "合成", 0 );
									}else 
									// 調理合成の時
									if( ItemBuffer[ i ].mixFlag == 2 ){
										// 料理合成
										StockFontBuffer( ItemBuffer[ i ].defX - 17, ItemBuffer[ i ].defY, FONT_PRIO_FRONT, 0, "料理", 0 );
									}
								}else{
									// 合成フラグＯＦＦ
									ItemBuffer[ i ].mixFlag = TRUE;
								}
							}
						}
						
						// 左クリック離した時
						if( mouse.onceState & MOUSE_LEFT_CRICK_UP ){
							// 何かドラッグしている時
							if( mouse.itemNo != -1 ){
								// ウィンドウ外なら
								if( mouse.nowPoint.x <= pActMenuWnd2->x && mouse.itemNo != -1 && eventWarpSendFlag == FALSE ){
									// アイテムを置く送信
									lssproto_DI_send( sockfd,nowGx, nowGy, mouse.itemNo );
								}
								// ドラッグフラグ初期化
								ItemBuffer[ mouse.itemNo ].dragFlag = FALSE;
								// マウスのアイテム番号初期化
								mouse.itemNo = -1; 
								// クリック音
								play_se( 217, 320, 240 );
							}
						}
						// ドラッグしている時
						if( mouse.itemNo != -1 ){
							// 右クリックされた時キャンセル
							if( mouse.onceState & MOUSE_RIGHT_CRICK ){
								// 元の位置に戻す
								//ItemBuffer[ mouse.itemNo ].x = ItemBuffer[ mouse.itemNo ].defX;
								//ItemBuffer[ mouse.itemNo ].y = ItemBuffer[ mouse.itemNo ].defY;
								// ドラッグフラグ初期化
								ItemBuffer[ mouse.itemNo ].dragFlag = FALSE;
								// マウスのアイテム番号初期化
								mouse.itemNo = -1; 
								// クリック音
								play_se( 217, 320, 240 );
								
							}
							// アイテム表示
							//StockDispBuffer( ItemBuffer[ mouse.itemNo ].x, ItemBuffer[ mouse.itemNo ].y, ItemBuffer[ mouse.itemNo ].dispPrio, ItemBuffer[ mouse.itemNo ].bmpNo, 0 );
							StockDispBuffer( mouse.nowPoint.x, mouse.nowPoint.y,DISP_PRIO_DRAG, pc.item[ mouse.itemNo ].graNo, 0 );
						}
						// ウィンドウある時
						if( pActMenuWnd2 != NULL ){
							// 文字表示バッファに溜める
							// 閉じるボタン
							itemWndFontNo[ 0 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx, pActMenuWnd2->y + 422, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
							//itemWndFontNo[ 0 ] = StockDispBuffer( pActMenuWnd2->x + 24 + 48 * 3, pActMenuWnd2->y + 32, DISP_PRIO_IME3, CG_ITEM_WND_JUJUTU_BTN, 2 );
							// 呪術ボタン
							//itemWndFontNo[ 1 ] = StockFontBuffer( pActMenuWnd2->x + 24 + 48 * 3 + 20, pActMenuWnd2->y + 32 + 8, FONT_PRIO_FRONT, 0, 	" 呪 術 ", 2 );
							//itemWndFontNo[ 1 ] = StockDispBuffer( pActMenuWnd2->x + 24 + 48 * 3 + 20 + 32, pActMenuWnd2->y + 32, DISP_PRIO_IME3, CG_ITEM_WND_JUJUTU_BTN, 2 );
							itemWndFontNo[ 1 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->my, DISP_PRIO_IME3, CG_ITEM_WND_JUJUTU_BTN, 2 );
							// お金表示
							sprintf( moji,"%7d", pc.gold );
												 StockFontBuffer( pActMenuWnd2->x + 32 + 48 * 3 + 16, pActMenuWnd2->y + 87, FONT_PRIO_FRONT, 0, moji, 0 );
							// 落とすお金表示
							sprintf( moji,"%7d", itemWndDropGold );
												 StockFontBuffer( pActMenuWnd2->x + 32 + 48 * 3 + 16, pActMenuWnd2->y + 138, FONT_PRIO_FRONT, 0, moji, 0 );
							// お金落とすボタン
							itemWndFontNo[ 2 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->my, DISP_PRIO_IME3, CG_ITEM_WND_GOLD_DROP_BTN_UP + itemWndBtnFlag[ 2 ], 2 );
							// お金プラスボタン
							itemWndFontNo[ 3 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->my, DISP_PRIO_IME3, CG_ITEM_WND_GOLD_INC_BTN_UP + itemWndBtnFlag[ 3 ], 2 );
							// お金マイナスボタン
							itemWndFontNo[ 4 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->my, DISP_PRIO_IME3, CG_ITEM_WND_GOLD_DEC_BTN_UP + itemWndBtnFlag[ 4 ], 2 );
							//itemWndFontNo[ 4 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx + 20, ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->my, DISP_PRIO_IME3, CG_ITEM_WND_GOLD_INC_BTN_UP + itemWndBtnFlag[ 4 ], 2 );
						}
					}
				}
			}
			break;
			
		case 1:	// 呪術選択ウィンドウ
		
			// ウィンドウがない時
			if( pActMenuWnd2 == NULL ){
				// ウィンドウ表示タスク作成
				//pActMenuWnd2 = MakeWindowDisp( 384 - 16, 16, 4, 9, CG_WND_TITLE_SYSTEM, 1 );
				// ウィンドウ表示タスク作成
				pActMenuWnd2 = MakeWindowDisp( 364, 4, 272, 280, 0, -1 );
				// アイテム
				for( i = 0 ; i < MENU_ITEM_0 ; i++ ) itemWndFontNo[ i ] = -2;
				for( i = 0 ; i < MENU_ITEM_0 ; i++ ) itemWndBtnFlag[ i ] = 0;
				jujutuNo = -1;	// 選択している呪術初期化
				
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd2->hp > 0 ){
					x = pActMenuWnd2->x + 74;
					y = pActMenuWnd2->y + 208;
					// アイテムウィンドウ上表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->my + 10, DISP_PRIO_MENU, CG_JUJUTU_WND, 1 );
					// 呪術選択した時
					for( i = 0 ; i < 5 ; i++ ){
						if( HitFontNo == itemWndFontNo[ i ] ){
							// 改行ポイント
							char *splitPoint = magic[ i ].memo;
							//char *splitPoint = "表示テストです。私の名前は太田哲生です。これからもよろしくお願いします。";
							//char moji2[ 256 ] = "私の名前は太田哲生と申します。これかもよろしくお願いします。";
							// 文字表示バッファに溜める
							sprintf( moji,"%2d/%2d", magic[ i ].mp, pc.mp );
							StockFontBuffer( pActMenuWnd2->x + 74 + 81, pActMenuWnd2->y + 183, FONT_PRIO_FRONT, 0, moji, 0 );
							// 呪術の説明表示
							// アイテムの名前表示
							//StockFontBuffer( pActMenuWnd2->x + 16, pActMenuWnd2->y + 331, FONT_PRIO_FRONT, pc.item[ i ].color, pc.item[ i ].name, 0 );	
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
								strncpy( moji, magic[ i ].memo, 22 );
								moji[ 22 ] = NULL;
								StockFontBuffer( pActMenuWnd2->x + 74, pActMenuWnd2->y + 204, FONT_PRIO_FRONT, 0, moji, 0 );
								
								if( magic[ i ].memo[ 22 ] != NULL ){
									strncpy( moji, magic[ i ].memo + 22, 22 );
									moji[ 22 ] = NULL;
									StockFontBuffer( pActMenuWnd2->x + 74, pActMenuWnd2->y + 224, FONT_PRIO_FRONT, 0, moji, 0 );
								}
							}
#endif
							// アイテム表示
							StockDispBuffer( pActMenuWnd2->x + 37, pActMenuWnd2->y + 220, DISP_PRIO_ITEM, pc.item[ i ].graNo, 0 );
							
							// 左クリックした時
							if( mouse.onceState & MOUSE_LEFT_CRICK ){
								// 使用不可又はＭＰ不足の時
								if( magic[ i ].field == MAGIC_FIELD_BATTLE || magic[ i ].mp > pc.mp ){
									// ＮＧ音
									play_se( 220, 320, 240 );
								}else{ // 使用可能な時
									// ターゲットで分岐
									switch( magic[ i ].target ){
										
									case MAGIC_TARGET_MYSELF:	// 自分のみ
										
										// イベントフラグＯＦＦの時
										if( eventWarpSendFlag == FALSE ){
											// 呪術使用送信
											lssproto_MU_send( sockfd, nowGx, nowGy, i, 0 );
											// ターゲット選択ウィンドウへ
											//itemWndNo = 0;
											// ウィンドウ抹殺
											//DeathAction( pActMenuWnd2 );
											//pActMenuWnd2 = NULL;
											// 呪術音
											play_se( 100, 320, 240 );
											// ウィンドウ開く音
											//play_se( 202, 320, 240 );
										}
										break;
										
									case MAGIC_TARGET_OTHER:	// 他の人（自分含む)
									
										// ターゲット選択ウィンドウへ
										//itemWndNo = 2;
										// ウィンドウ抹殺
										//DeathAction( pActMenuWnd2 );
										//pActMenuWnd2 = NULL;
										// ウィンドウ開く音
										//play_se( 202, 320, 240 );
										// 選択された呪術番号記憶
										jujutuNo = i;
										// クリック音
										play_se( 217, 320, 240 );
										//itemNo = -1;
										break;
#if 0
									case MAGIC_TARGET_ALLMYSIDE:	// 味方全体
									
										// 呪術使用送信
										lssproto_MU_send( sockfd, nowGx, nowGy, i, 0 );
										// 呪術音
										play_se( 100, 320, 240 );
										break;
										
									case MAGIC_TARGET_ALLOTHERSIDE:	// 相手側全体
									
										// 呪術使用送信
										lssproto_MU_send( sockfd, nowGx, nowGy, i, 0 );
										// 呪術音
										play_se( 100, 320, 240 );
										break;
										
									case MAGIC_TARGET_ALL:	// 全て
									
										// 呪術使用送信
										lssproto_MU_send( sockfd, nowGx, nowGy, i, 0 );
										// 呪術音
										play_se( 100, 320, 240 );
										break;
#endif									
									}
								}
							}
						}
					}
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// 戻る時
						if( HitDispNo == itemWndFontNo[ 12 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd2 );
							pActMenuWnd2 = NULL;
							// システムウィンドウ閉じる
							//MenuToggleFlag ^= JOY_CTRL_I;
							itemWndNo = 0;
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
							// 選択している呪術初期化
							jujutuNo = -1;	
							
							// ステータスウィンドウ開いていたら閉じる
							if( MenuToggleFlag & JOY_CTRL_S ){
							
								MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S キー
								// メニューアクション抹殺
								DeathMenuAction();
							}
						}
						// 呪術選択されてる時
						if( jujutuNo != -1 && MenuToggleFlag & JOY_CTRL_S && statusWndNo == 1 ){
							// 人数分ループ
							for( i = 0 ; i < 11 ; i++ ){
								// 呪術使う人選択
								if( HitFontNo == statusWndFontNo[ i ] && eventWarpSendFlag == FALSE ){
									// 呪術使用送信
									lssproto_MU_send( sockfd, nowGx, nowGy, jujutuNo, i );
									// 呪術音
									play_se( 100, 320, 240 );
									// 残りＭＰ計算
									if( magic[ jujutuNo ].mp > pc.mp - magic[ jujutuNo ].mp ) jujutuNo = -1;;	// ＭＰ不足（赤）
									
								}
							}
						}
						
					}
					// ウィンドウある時
					if( pActMenuWnd2 != NULL ){
						int flag = FALSE;
						int col;
						// 座標設定
						x = pActMenuWnd2->x + 30;
						y = pActMenuWnd2->y + 35;
						
						// 呪術表示
						for( i = 0 ; i < 5 ; i++ ){
							if( magic[ i ].useFlag == TRUE ){
								col = FONT_PAL_WHITE;	// 使用可能な色（白）
								if( jujutuNo == i ) col = FONT_PAL_AQUA;	// 選択中の呪術
								if( magic[ i ].mp > pc.mp ) col = FONT_PAL_RED;	// ＭＰ不足（赤）
								if( magic[ i ].field == MAGIC_FIELD_BATTLE ) col = FONT_PAL_GRAY;	// 使用不可な色（赤）
								// 文字表示バッファに溜める
								//sprintf( moji," %-16s ", magic[ i ].name );
								CenteringStr( magic[ i ].name, moji, MAGIC_NAME_LEN );
								itemWndFontNo[ i ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, col, moji, 2 ); y += 28;
								flag = TRUE;
							}
						}
						if( flag == FALSE ) StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, "呪術がありません", 0 );
						// 文字表示バッファに溜める
						itemWndFontNo[ 12 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx, pActMenuWnd2->y + 262 + 10, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );
					}
				}
			}
			break;
			
		case 2:	// ターゲット選択ウィンドウ
		
			// ウィンドウがない時
			if( pActMenuWnd2 == NULL ){
				// ウィンドウ表示タスク作成
				//pActMenuWnd2 = MakeWindowDisp( 384 - 16, 16, 4, 9, CG_WND_TITLE_SYSTEM, 1 );
				// ウィンドウ表示タスク作成
				pActMenuWnd2 = MakeWindowDisp( 368, 4, 192, 304, 0, -1 );
				// アイテム
				for( i = 0 ; i < MENU_ITEM_0 ; i++ ) itemWndFontNo[ i ] = -2;
				for( i = 0 ; i < MENU_ITEM_0 ; i++ ) itemWndBtnFlag[ i ] = 0;
				
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd2->hp > 0 ){
					// アイテムウィンドウ上表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->my, DISP_PRIO_MENU, CG_ITEM_WND_SELECT_WND, 1 );
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// 使う人選択した時
						for( i = 0 ; i < 11 ; i++ ){
							if( HitFontNo == itemWndFontNo[ i ] && eventWarpSendFlag == FALSE ){
								// 呪術選択の時
								if( jujutuNo != -1 ){
									// 呪術使用送信
									lssproto_MU_send( sockfd, nowGx, nowGy, jujutuNo, i );
									// 呪術音
									play_se( 100, 320, 240 );
									// ＭＰ無くなった時戻る
									if( magic[ jujutuNo ].mp > pc.mp - magic[ jujutuNo ].mp ){
										// ウィンドウ抹殺
										DeathAction( pActMenuWnd2 );
										pActMenuWnd2 = NULL;
										itemWndNo = 1;
										// ウィンドウ閉じる音
										play_se( 203, 320, 240 );
									}
								}else{	// アイテムの時
									//アイテム使う送信
									lssproto_ID_send( sockfd, nowGx, nowGy, itemNo, i );
									// アイテム使う音
									play_se( 212, 320, 240 );
									// ウィンドウ抹殺
									DeathAction( pActMenuWnd2 );
									pActMenuWnd2 = NULL;
									itemWndNo = 0;
									// ステータスウィンドウ開いていたら閉じる
									if( MenuToggleFlag & JOY_CTRL_S ){
									
										MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S キー
										// メニューアクション抹殺
										DeathMenuAction();
									}
									// ウィンドウ閉じる音
									play_se( 203, 320, 240 );
								}
							}
						}
						// 戻る時
						if( HitDispNo == itemWndFontNo[ 11 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd2 );
							pActMenuWnd2 = NULL;
							// 呪術選択の時
							if( jujutuNo != -1 ) itemWndNo = 1;
							else{ 
								// アイテムウィンドウに戻る
								itemWndNo = 0;
								// ステータスウィンドウ開いていたら閉じる
								if( MenuToggleFlag & JOY_CTRL_S ){
								
									MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S キー
									// メニューアクション抹殺
									DeathMenuAction();
								}
							}
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
					}
					// ウィンドウある時
					if( pActMenuWnd2 != NULL ){
						// 座標設定
						x = pActMenuWnd2->x + 25;
						y = pActMenuWnd2->y + 30;
						
						// 文字表示バッファに溜める
						// 自分の名前
						sprintf( moji,"%-16s", pc.name );
						itemWndFontNo[ 0 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 2 ); y += 23;
						// ペット
						for( i = 0 ; i < 5 ; i++ ){
							// その欄にペットがいたら
							if( pet[ i ].useFlag == TRUE ){
								// あだ名がある時
								if( pet[ i ].freeName[ 0 ] != NULL )
									sprintf( moji,"%-16s", pet[ i ].freeName );
								else
									sprintf( moji,"%-16s", pet[ i ].name );
								itemWndFontNo[ i + 1 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 2 ); y += 23;
							}
						}
						y = 183;
						// 仲間
						for( i = 0 ; i < 5 ; i++ ){
							// 自分でない時かつその欄に仲間がいる時
							if( pc.id != party[ i ].id && party[ i ].useFlag == TRUE ){
								sprintf( moji,"%-16s", party[ i ].name );
								itemWndFontNo[ i + 6 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 2 ); y += 23;
							}
						}
						// 文字表示バッファに溜める
						itemWndFontNo[ 11 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx, pActMenuWnd2->y + 287, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );
					}
				}
			}
			break;
		}
	}
	
//◎ マップウィンドウ処理 **地图窗口处理*****************************************************/

	if( MenuToggleFlag & JOY_CTRL_M ){
	
		// ウィンドウ番号で分岐
		switch( mapWndNo ){
		
		case 0:	// マップウィンドウ
		
			// ウィンドウがない時
			if( pActMenuWnd2 == NULL ){
				// ウィンドウ表示タスク作成
				//pActMenuWnd2 = MakeWindowDisp( 0, 0, 3, 6, CG_WND_TITLE_SYSTEM, 1 );
				// ウィンドウ表示タスク作成
				pActMenuWnd2 = MakeWindowDisp( 388, 4, 248, 240, 0, -1 );
				//pActMenuWnd2 = MakeWindowDisp( 365, 4, 271, 440, 0, -1 );
				// 当たり判定初期化
				for( i = 0 ; i < MENU_MAP_0 ; i++ ) mapWndFontNo[ i ] = -2;
				
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd2->hp > 0 ){
					// マップウィンドウ表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->my, DISP_PRIO_MENU, CG_MAP_WND, 0 );
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// 閉じる時
						if( HitDispNo == mapWndFontNo[ 0 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd2 );
							pActMenuWnd2 = NULL;
							// ウィンドウ閉じる
							MenuToggleFlag ^= JOY_CTRL_M;
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
					}
					
					// ウィンドウある時
					if( pActMenuWnd2 != NULL ){
						// 座標設定
						x = pActMenuWnd2->x + 61;
						y = pActMenuWnd2->y + 186;
						
						// マップの名前表示
						StockFontBuffer( pActMenuWnd2->x + 22, pActMenuWnd2->y + 31, FONT_PRIO_FRONT, 0, nowFloorName, 0 );
						
						// 文字表示バッファに溜める
						sprintf( moji, "東 %3d", nowGx );
						StockFontBuffer( x, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0 );
						sprintf( moji, "南 %3d", nowGy );
						StockFontBuffer( x + 73, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0 ); y += 36;
						// 閉じるボタン
						//mapWndFontNo[ 0 ]= StockFontBuffer( x + 40, y, FONT_PRIO_FRONT, FONT_PAL_AQUA, 	"　閉 じ る　", 2 );
						mapWndFontNo[ 0 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd2->pYobi )->mx, y, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
					}
				}
			}
			break;
		}	
	}
	
//◎ メールウィンドウ処理 ****邮件窗口处理***************************************************/

	if( MenuToggleFlag & JOY_CTRL_E )
	{
	
		// ウィンドウ番号で分岐
		switch( mailWndNo ){
		
		case MAIL_WND_VIEW:	// 名刺一覧ウィンドウ
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				//pActMenuWnd = MakeWindowDisp( 0, 0, 3, 6, CG_WND_TITLE_SYSTEM, 1 );
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 4, 4, 272, 440, 0, -1 );
				// 当たり判定初期化
				for( i = 0 ; i < MENU_MAIL_0 ; i++ ) mailWndFontNo[ i ] = -2;
				// メールが届いているページ番号を探す（無い時はデフォルト）
				for( i = 0 ; i < MAX_ADR_BOOK ; i++ ){
					int j;
					// 履歴ページ分ループ
					for( j = 0 ; j < MAIL_MAX_HISTORY ; j++ ){
						// 未読チェック
						if( MailHistory[ i ].noReadFlag[ j ] >= TRUE ){
							// ページ番号の覚える
							mailViewWndPageNo = i / MAX_ADR_BOOK_COUNT;
							i =  MAX_ADR_BOOK;
							break;
						}
					}
				}
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					static int nowDelNo;
					// 名刺一覧ウィンドウ表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_MENU, CG_MAIL_WND, 1 );
					
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK && pActYesNoWnd == NULL ){
					
						int nowNo = mailViewWndPageNo * MAX_ADR_BOOK_COUNT;
						// ページ内ループ
						//for( i = mailViewWndPageNo * 4 ; i < mailViewWndPageNo * 4 + 4 ; i++ ){
						for( i = 0 ; i < 12 ; i += 3, nowNo++ ){
							// 名前がある所にデータが入っているとする
							if( addressBook[ nowNo ].useFlag == TRUE ){
								// ＯＮ／ＯＦＦボタン
								//if( HitDispNo == mailWndFontNo[ i ] ){
									// クリック音
								//	play_se( 217, 320, 240 );
								//}
								// 履歴ボタン
								//if( HitDispNo == mailWndFontNo[ i ] ){
								if( HitFontNo == mailWndFontNo[ i ] ){
									mailHistoryWndSelectNo = nowNo;
									// ウィンドウ抹殺
									DeathAction( pActMenuWnd );
									pActMenuWnd = NULL;
									// 手紙アニメーション抹殺
									DeathLetterAction();
									// 履歴ウィンドウへ
									mailWndNo = MAIL_WND_HISTORY;
									// ウィンドウ開く音
									play_se( 202, 320, 240 );
								}
								
								// 削除ボタン
								if( HitDispNo == mailWndFontNo[ i + 1 ] ){
									// 確認ウィンドウない時
									if( pActYesNoWnd == NULL ){
										// ウィンドウ開く音
										play_se( 202, 320, 240 );
										// 確認ウィンドウ表示タスク作成
										pActYesNoWnd = MakeWindowDisp( pActMenuWnd->x + 42, i * 32 + 54, 176, 56, 0, 4 );
										
										// 削除する番号を記憶
										nowDelNo = nowNo;
									}
								}
								// メールボタン
								if( HitDispNo == mailWndFontNo[ i + 2 ] ){
									// ウィンドウ抹殺
									DeathAction( pActMenuWnd );
									pActMenuWnd = NULL;
									// 手紙アニメーション抹殺
									DeathLetterAction();
									// 送信相手フラグ初期化
									InitMailSendFlag();
									// 送信相手だけフラグ立てる
									mailWndSendFlag[ nowNo ] = TRUE;
									// ウィンドウ番号バックアップ
									mailWndNoBak = mailWndNo;
									// メールペットが選択されていたら
									if( pc.mailPetNo != -1 ){
										// 選択ウィンドウへ
										mailWndNo = MAIL_WND_SELECT;
									}else{
										// 普通メール送信ウィンドウ
										mailWndNo = MAIL_WND_SEND;
									}
									// ウィンドウ開く音
									play_se( 202, 320, 240 );
								}
							}
						}
						// 閉じる時
						if( HitDispNo == mailWndFontNo[ 19 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// 手紙アニメーション抹殺
							DeathLetterAction();
							// 手紙アニメーション抹殺
							//for( i = 0 ; i < 4 ; i++ ){
							//	DeathAction( pActLetter[ i ] );
							//	pActLetter[ i ] = NULL;
							//}
							// ウィンドウ閉じる
							MenuToggleFlag ^= JOY_CTRL_E;
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
					}
					
					// 前に戻る
					if( ( HitDispNo == mailWndFontNo[ 26 ] || joy_auto[ 0 ] & JOY_A ) && pActYesNoWnd == NULL ){
						// 左クリックした時
						//if( mouse.onceState & MOUSE_LEFT_CRICK ){
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_A ){
							mailViewWndPageNo--; // ページ戻す
							if( mailViewWndPageNo < 0 ) mailViewWndPageNo = MAX_ADR_BOOK_PAGE - 1;
							// クリック音
							play_se( 217, 320, 240 );
							mailWndBtnFlag[ 26 ] = TRUE;
							// 手紙アニメーション抹殺
							DeathLetterAction();
						}
						// 左クリックしてる時
						if( ( mouse.state & MOUSE_LEFT_CRICK /*|| joy_auto[ 0 ] & JOY_A*/ ) && mailWndBtnFlag[ 26 ] == TRUE ){
							// ボタン凹
							mailWndBtnFlag[ 26 ] = TRUE;
						}else{
							// ボタン凸
							mailWndBtnFlag[ 26 ] = FALSE;
						}
					}else{
						// ボタン凸
						mailWndBtnFlag[ 26 ] = FALSE;
					}
					// 次に進む
					if( ( HitDispNo == mailWndFontNo[ 27 ] || joy_auto[ 0 ] & JOY_B ) && pActYesNoWnd == NULL ){
						// 左クリックした時
						//if( mouse.onceState & MOUSE_LEFT_CRICK ){
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_B ){
							mailViewWndPageNo++; // ページ進ませる
							if( mailViewWndPageNo >= MAX_ADR_BOOK_PAGE ) mailViewWndPageNo = 0;
							// クリック音
							play_se( 217, 320, 240 );
							mailWndBtnFlag[ 27 ] = TRUE;
							// 手紙アニメーション抹殺
							DeathLetterAction();
						}
						// 左クリックしてる時
						if( ( mouse.state & MOUSE_LEFT_CRICK /*|| joy_auto[ 0 ] & JOY_B*/ ) && mailWndBtnFlag[ 27 ] == TRUE ){
							// ボタン凹
							mailWndBtnFlag[ 27 ] = TRUE;
						}else{
							// ボタン凸
							mailWndBtnFlag[ 27 ] = FALSE;
						}
					}else{
						// ボタン凸
						mailWndBtnFlag[ 27 ] = FALSE;
					}
					
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
						int nowNo = mailViewWndPageNo * MAX_ADR_BOOK_COUNT;
						// 座標設定
						x = pActMenuWnd->x;
						y = pActMenuWnd->y;
						
						// ＯＮＬＩＮＥ，ＯＦＦＬＩＮＥグラフィック番号
						int graNo[ 3 ] = { 	CG_MAIL_WND_OFF_LINE_BTN,
											CG_MAIL_WND_ON_LINE_SUN_BTN,
											CG_MAIL_WND_ON_LINE_MOON_BTN
										};
										
						// ページ内ループ
						//for( i = mailViewWndPageNo * 4 ; i < mailViewWndPageNo * 4 + 4 ; i++ ){
						//for( i = 0 ; i < 4 ; i++ ){
						for( i = 0 ; i < 12 ; i += 3, nowNo++ ){
							// 名前がある所にデータが入っているとする
							if( addressBook[ nowNo ].useFlag == TRUE ){
								int j = 0;
								// 手紙アニメーション表示
								for( j = 0 ; j < MAIL_MAX_HISTORY ; j++ ){
									// 手紙アクションがない時
									if( pActLetter[ i / 3 ] == NULL ){
										if( MailHistory[ nowNo ].noReadFlag[ j ] >= TRUE ){
											// アニメーションタスク作成
											pActLetter[ i / 3 ] = MakeAnimDisp( x - 20, y + 38, SPR_mail, ANIM_DISP_LETTER_MOVE );
										}
									}
								}
								// 顔表示
								StockDispBuffer( x + 44, y + 68, DISP_PRIO_IME3, addressBook[ nowNo ].graNo, 0 );
								//StockDispBuffer( x + 44, y + 68, DISP_PRIO_IME3, pc.faceGraNo, 0 );
								// 名前表示
								CenteringStr( addressBook[ nowNo ].name, moji, CHAR_NAME_LEN );
								mailWndFontNo[ i ] = StockFontBuffer( x + 80, y + 30, FONT_PRIO_FRONT, 0, moji, 2 );
								//StockFontBuffer( x + 80, y + 30, FONT_PRIO_FRONT, 0, "0123456789012345", 0 );
								//StockFontBuffer( x + 80, y + 31, FONT_PRIO_FRONT, 0, "ああああああああ", 0 );
								// レベル表示
								sprintf( moji,"%3d", addressBook[ nowNo ].level );
								//sprintf( moji,"999", addressBook[ nowNo ].level );
								StockFontBuffer( x + 104, y + 60, FONT_PRIO_FRONT, 0, moji, 0 );
								// ペットの数表示
								//sprintf( moji,"%3d", addressBook[ nowNo ].level );
								//sprintf( moji,"999", addressBook[ nowNo ].level );
								//StockFontBuffer( x + 84, y + 88, FONT_PRIO_FRONT, 0, moji, 0 );
								// ＤＰ表示
								sprintf( moji,"%8d", addressBook[ nowNo ].dp );
								//sprintf( moji,"9999999", addressBook[ nowNo ].dp );
								//sprintf( moji,"9999", addressBook[ nowNo ].level );
								StockFontBuffer( x + 103, y + 88, FONT_PRIO_FRONT, 0, moji, 0 );
								// ＯＮ／ＯＦＦボタン
								//StockDispBuffer( x + 242, y + 58, DISP_PRIO_IME3, CG_MAIL_WND_ON_LINE_BTN + !( addressBook[ nowNo ].onlineFlag ), 0 );
								StockDispBuffer( x + 225, y + 60, DISP_PRIO_IME3, graNo[ addressBook[ nowNo ].onlineFlag ], 0 );
								// 履歴ボタン
								//mailWndFontNo[ i ] = StockDispBuffer( x + 242, y + 78, DISP_PRIO_IME3, CG_MAIL_WND_HISTORY_BTN, 2 );
								// メールボタン
								mailWndFontNo[ i + 2 ] = StockDispBuffer( x + 225, y + 80, DISP_PRIO_IME3, CG_MAIL_WND_MAIL_BTN, 2 );
								// 削除ボタン
								//mailWndFontNo[ i + 1 ] = StockDispBuffer( x + 242, y + 98, DISP_PRIO_IME3, CG_MAIL_WND_DELETE_BTN, 2 );
								mailWndFontNo[ i + 1 ] = StockDispBuffer( x + 225, y + 100, DISP_PRIO_IME3, CG_MAIL_WND_DELETE_BTN, 2 );
								
							}
							y += 96;
						}
						// 前ボタン
						mailWndFontNo[ 26 ] = StockDispBuffer( pActMenuWnd->x + 25 + 28 - 8, pActMenuWnd->y + 421, DISP_PRIO_IME3, CG_PREV_BTN + mailWndBtnFlag[ 26 ], 2 );
						// ページ番号表示
						sprintf( moji,"%2d", mailViewWndPageNo + 1 );
						StockFontBuffer( pActMenuWnd->x + 46 + 28 - 8, pActMenuWnd->y + 413, FONT_PRIO_FRONT, 0, moji, 0 );
						// 次ボタン
						mailWndFontNo[ 27 ] = StockDispBuffer( pActMenuWnd->x + 75 + 28, pActMenuWnd->y + 421, DISP_PRIO_IME3, CG_NEXT_BTN + mailWndBtnFlag[ 27 ], 2 );
						// メールボタン
						//mailWndFontNo[ 18 ] = StockDispBuffer( pActMenuWnd->x + 135, pActMenuWnd->y + 421, DISP_PRIO_IME3, CG_MAIL_WND_MAIL_BTN, 2 );
						// 閉じるボタン
						mailWndFontNo[ 19 ] = StockDispBuffer( pActMenuWnd->x + 220 - 28, pActMenuWnd->y + 421, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
					}
					
					// 確認ウィンドウある時
					if( pActYesNoWnd != NULL ){
						// はいの時
						if( ( ( WINDOW_DISP *)pActYesNoWnd->pYobi )->yesNoResult == TRUE ){
							// 削除するを送信
							lssproto_DAB_send( sockfd, nowDelNo );
							// アルバムデータを消す
							//memset( MailHistory[ nowDelNo ], 0, sizeof( MAIL_HISTORY ) );
							// クリック音
							play_se( 217, 320, 240 );
							// 確認ウィンドウ抹殺
							DeathAction( pActYesNoWnd );
							pActYesNoWnd = NULL;
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}else
						// いいえの時
						if( ( ( WINDOW_DISP *)pActYesNoWnd->pYobi )->yesNoResult == FALSE ){
							// 確認ウィンドウ抹殺
							DeathAction( pActYesNoWnd );
							pActYesNoWnd = NULL;
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
					}
				}
				
			}
			break;
			
		case MAIL_WND_SELECT:	// 選択ウィンドウ
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 40, 196, 3, 3, NULL, 0 );
				// 当たり判定初期化
				for( i = 0 ; i < MENU_MAIL_0 ; i++ ) mailWndFontNo[ i ] = -2;
				// いないペットチェック
				for( i = 0 ; i < 5 ; i++ ){
					// この欄にペットがいない時
					if( pet[ i ].useFlag == FALSE ){
						// メールの時
						if( i == pc.mailPetNo ){ 
							pc.mailPetNo = -1;
							// パラメータセーブ
							saveUserSetting();
						}
					}
				}
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// 普通メールの時
						if( HitFontNo == mailWndFontNo[ 0 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// 普通メール送信ウィンドウへ
							mailWndNo = MAIL_WND_SEND;	
							// ウィンドウ開く音
							play_se( 202, 320, 240 );
						}
						// ペットメールの時
						if( HitFontNo == mailWndFontNo[ 1 ] ){
							// メールペット選択されてる時
							if( pc.mailPetNo != -1 ){
								// ウィンドウ抹殺
								DeathAction( pActMenuWnd );
								pActMenuWnd = NULL;
								// ペットメール送信ウィンドウへ
								mailWndNo = MAIL_WND_PET_SEND;	
								// ウィンドウ開く音
								play_se( 202, 320, 240 );
							}else{
								// ＮＧ音
								play_se( 220, 320, 240 );
							}
						}
						// 戻る時
						if( HitDispNo == mailWndFontNo[ 2 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// 一覧ウィンドウへ
							mailWndNo = MAIL_WND_VIEW;	
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
					}
					
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
						int color = FONT_PAL_WHITE;
						
						// メールペット選択されて無い時
						if( pc.mailPetNo == -1 ) 
						color = FONT_PAL_RED;
						
						// 座標設定
						x = pActMenuWnd->x + 28;
						y = pActMenuWnd->y + 32;
						
						// 文字表示バッファに溜める
						mailWndFontNo[ 0 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	"　普　通メール　", 2 );	y += 32;
						mailWndFontNo[ 1 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, color, 	"　ペットメール　", 2 );	y += 48;
						mailWndFontNo[ 2 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, y, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );
					}
				}
			}
			break;
			
		case MAIL_WND_SEND:	// 普通メール送信ウィンドウ
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				//pActMenuWnd = MakeWindowDisp( 0, 0, 3, 6, CG_WND_TITLE_SYSTEM, 1 );
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 4, 4, 272, 304, 0, -1 );
				// 当たり判定初期化
				for( i = 0 ; i < MENU_MAIL_0 ; i++ ) mailWndFontNo[ i ] = -2;
				//mailPetSendWndPageNo = 0;		// 普通送信相手ページ番号初期化
				mailPetSendWndPageNo = mailViewWndPageNo;		// 普通送信相手ページ番号初期化
				//mailViewWndPageNo = 0;		// 名刺一覧ウィンドウのページ番号初期化
				
				// メール文章 初期設定
				//MailStr.buffer[ 0 ] = NULL;
				//MailStr.cnt = 0;
				// 文字列の長さ
				MailStr.len = 140;
				// 一行の長さ
				MailStr.lineLen = 28;
				// 一行の間隔
				MailStr.lineDist = 20;
				// 文字列の色
				MailStr.color = 0;
				// 文字列の座標
				MailStr.x = pActMenuWnd->x + 12;
				MailStr.y = pActMenuWnd->y + 280 - 136;
				// 文字列の表示優先順位
				MailStr.fontPrio = FONT_PRIO_FRONT;
				
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					// 送信ウィンドウ表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_MENU, CG_MAIL_WND_SEND_WND, 1 );
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						int nowNo = mailPetSendWndPageNo * MAX_ADR_BOOK_COUNT;
						// ページ内ループ
						//for( i = mailViewWndPageNo * 4 ; i < mailViewWndPageNo * 4 + 4 ; i++ ){
						for( i = 0 ; i < MAX_ADR_BOOK_COUNT ; i++ ){
							// 送信相手選択した時
							if( HitFontNo == mailWndFontNo[ i ] ){
								// 名前がある所にデータが入っているとする
								if( addressBook[ nowNo + i ].useFlag == TRUE ){
									// 選択されていなかったら
									if( mailWndSendFlag[ nowNo + i ] == FALSE ) mailWndSendFlag[ nowNo + i ] = TRUE;
									else mailWndSendFlag[ nowNo + i ] = FALSE;
									// クリック音
									play_se( 217, 320, 240 );
								}
							}
							
						}
						// 戻る時
						if( HitDispNo == mailWndFontNo[ 19 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// 前のウィンドウに戻る
							mailWndNo = mailWndNoBak;	
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
							// 入力フォーカス戻す
							GetKeyInputFocus( &MyChatBuffer );
						}
					}
					
					// 送信ボタン
					if( HitDispNo == mailWndFontNo[ 18 ] ){
						// 左クリックした時
						if( mouse.onceState & MOUSE_LEFT_CRICK ){
							int flag = FALSE;
							// 既に選択してるかチェック
							for( i = 0 ; i < MAX_ADR_BOOK ; i++ ){ 
								if( mailWndSendFlag[ i ] == TRUE ) flag = TRUE;
							}
							// 文字列がある時
							if( MailStr.buffer[ 0 ] != NULL && flag == TRUE ){
								// ２０人分ループ
								for( i = 0 ; i < MAX_ADR_BOOK ; i++ ){
									// フラグ立ってる時
									if( mailWndSendFlag[ i ] == TRUE ){
										char moji2[ 256 ];
										// とりあえずコピー
										strcpy( moji2, MailStr.buffer );
										// エスケープ処理
										makeSendString( moji2, moji, sizeof( moji ) );
										// メール送信
										lssproto_MSG_send( sockfd, i, moji, FONT_PAL_WHITE ); 
										// メール送信
										//lssproto_MSG_send( sockfd, i, MailStr.buffer, FONT_PAL_WHITE ); 
										
									}
								}
								// 文字列初期化
								//MailStr.buffer[ 0 ] = NULL;
								//MailStr.cnt = 0;
								// 送信音
								play_se( 101, 320, 240 );
								mailWndBtnFlag[ 18 ] = TRUE;
							}else{
								// ＮＧ音
								play_se( 220, 320, 240 );
							}
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[ 18 ] == TRUE ){
							// ボタン凹
							mailWndBtnFlag[ 18 ] = TRUE;
						}else{
							// ボタン凸
							mailWndBtnFlag[ 18 ] = FALSE;
						}
					}else{
						// ボタン凸
						mailWndBtnFlag[ 18 ] = FALSE;
					}
					
					// 前に戻る
					if( HitDispNo == mailWndFontNo[ 16 ] || joy_auto[ 0 ] & JOY_A ){
						// 左クリックした時
						//if( mouse.onceState & MOUSE_LEFT_CRICK ){
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_A ){
							mailPetSendWndPageNo--; // ページ戻す
							if( mailPetSendWndPageNo < 0 ) mailPetSendWndPageNo = MAX_ADR_BOOK_PAGE - 1;
							// クリック音
							play_se( 217, 320, 240 );
							mailWndBtnFlag[ 16 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[ 16 ] == TRUE ){
							// ボタン凹
							mailWndBtnFlag[ 16 ] = TRUE;
						}else{
							// ボタン凸
							mailWndBtnFlag[ 16 ] = FALSE;
						}
					}else{
						// ボタン凸
						mailWndBtnFlag[ 16 ] = FALSE;
					}
					// 次に進む
					if( HitDispNo == mailWndFontNo[ 17 ] || joy_auto[ 0 ] & JOY_B ){
						// 左クリックした時
						//if( mouse.onceState & MOUSE_LEFT_CRICK ){
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_B ){
							mailPetSendWndPageNo++; // ページ進ませる
							if( mailPetSendWndPageNo >= MAX_ADR_BOOK_PAGE ) mailPetSendWndPageNo = 0;
							// クリック音
							play_se( 217, 320, 240 );
							mailWndBtnFlag[ 17 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[ 17 ] == TRUE ){
							// ボタン凹
							mailWndBtnFlag[ 17 ] = TRUE;
						}else{
							// ボタン凸
							mailWndBtnFlag[ 17 ] = FALSE;
						}
					}else{
						// ボタン凸
						mailWndBtnFlag[ 17 ] = FALSE;
					}
					
					// 文章クリアボタン
					if( HitDispNo == mailWndFontNo[ 14 ] ){
						// 左クリックした時
						if( mouse.onceState & MOUSE_LEFT_CRICK ){
							// メール文章 初期設定
							MailStr.buffer[ 0 ] = NULL;
							MailStr.cnt = 0;
							// クリック音
							play_se( 217, 320, 240 );
							mailWndBtnFlag[ 14 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[ 14 ] == TRUE ){
							// ボタン凹
							mailWndBtnFlag[ 14 ] = TRUE;
						}else{
							// ボタン凸
							mailWndBtnFlag[ 14 ] = FALSE;
						}
					}else{
						// ボタン凸
						mailWndBtnFlag[ 14 ] = FALSE;
					}
					// 選択クリアボタン
					if( HitDispNo == mailWndFontNo[ 15 ] ){
						// 左クリックした時
						if( mouse.onceState & MOUSE_LEFT_CRICK ){
							// 送信相手フラグ初期化
							InitMailSendFlag();
							// クリック音
							play_se( 217, 320, 240 );
							mailWndBtnFlag[ 15 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[ 15 ] == TRUE ){
							// ボタン凹
							mailWndBtnFlag[ 15 ] = TRUE;
						}else{
							// ボタン凸
							mailWndBtnFlag[ 15 ] = FALSE;
						}
					}else{
						// ボタン凸
						mailWndBtnFlag[ 15 ] = FALSE;
					}
					
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
						int nowNo = mailPetSendWndPageNo * MAX_ADR_BOOK_COUNT;
						int color;
						
						// 座標設定
						x = pActMenuWnd->x;
						y = pActMenuWnd->y;
						// 入力フォーカス取得
						GetKeyInputFocus( &MailStr );
						// 文字列の表示
						StockFontBuffer2( &MailStr );
						
						// ページ内ループ
						//for( i = mailViewWndPageNo * 4 ; i < mailViewWndPageNo * 4 + 4 ; i++ ){
						//for( i = 0 ; i < 4 ; i++ ){
						for( i = 0 ; i < MAX_ADR_BOOK_COUNT ; i++ ){
							// 名前がある所にデータが入っているとする
							if( addressBook[ nowNo + i ].useFlag == TRUE ){
								color = FONT_PAL_WHITE;	// デフォルト色
								// 選択されている時
								if( mailWndSendFlag[ nowNo + i ] == TRUE ){
									// 黄色
									color = FONT_PAL_AQUA;
								}
								// 名前表示
								CenteringStr( addressBook[ nowNo + i ].name, moji, CHAR_NAME_LEN );
								mailWndFontNo[ i ] = StockFontBuffer( x + 111, y + 177 - 136, FONT_PRIO_FRONT, color, moji, 2 );
								//mailWndFontNo[ i ] = StockFontBuffer( x + 111, y + 177, FONT_PRIO_FRONT, color, "aaaaaaaaaaaaaaaa", 2 );
								//mailWndFontNo[ i ] = StockFontBuffer( x + 111, y + 177, FONT_PRIO_FRONT, color, "ああああああああ", 2 );
								
							}
							y += 23;
						}
						// 選択クリアボタン
						mailWndFontNo[ 15 ] = StockDispBuffer( pActMenuWnd->x + 51, pActMenuWnd->y + 211 - 136, DISP_PRIO_IME3, CG_MAIL_WND_CLEAR_BTN_UP + mailWndBtnFlag[ 15 ], 2 );
						// 前ボタン
						mailWndFontNo[ 16 ] = StockDispBuffer( pActMenuWnd->x + 26 - 8 + 4, pActMenuWnd->y + 241 - 132, DISP_PRIO_IME3, CG_PREV_BTN + mailWndBtnFlag[ 16 ], 2 );
						// ページ番号表示
						sprintf( moji,"%2d", mailPetSendWndPageNo + 1 );
						StockFontBuffer( pActMenuWnd->x + 47 - 8 + 4, pActMenuWnd->y + 233 - 132, FONT_PRIO_FRONT, 0, moji, 0 );
						// 次ボタン
						mailWndFontNo[ 17 ] = StockDispBuffer( pActMenuWnd->x + 76 + 4, pActMenuWnd->y + 241 - 132, DISP_PRIO_IME3, CG_NEXT_BTN + mailWndBtnFlag[ 17 ], 2 );
						// 文章クリアボタン
						mailWndFontNo[ 14 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx + 60, pActMenuWnd->y + 390 - 136, DISP_PRIO_IME3, CG_MAIL_WND_CLEAR_BTN_UP + mailWndBtnFlag[ 14 ], 2 );
						// 送信ボタン
						mailWndFontNo[ 18 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx - 60, pActMenuWnd->y + 421 - 136, DISP_PRIO_IME3, CG_SEND_BTN + mailWndBtnFlag[ 18 ] * 75, 2 );
						// 戻る
						mailWndFontNo[ 19 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx + 60, pActMenuWnd->y + 421 - 136, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );
					}
				}
			}
			break;
			
		case MAIL_WND_PET_SEND:	// ペットメール送信ウィンドウ
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				//pActMenuWnd = MakeWindowDisp( 0, 0, 3, 6, CG_WND_TITLE_SYSTEM, 1 );
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 4, 4, 272, 440, 0, -1 );
				// 当たり判定初期化
				for( i = 0 ; i < MENU_MAIL_0 ; i++ ) mailWndFontNo[ i ] = -2;
				// 送信相手フラグ初期化
				//for( i = 0 ; i < MAX_ADR_BOOK ; i++ ) mailWndSendFlag[ i ] = 0;
				//mailPetSendWndPageNo = 0;		// 普通送信相手ページ番号初期化
				mailPetSendWndPageNo = mailViewWndPageNo;		// 普通送信相手ページ番号初期化
				// メール文章 初期設定
				//MailStr.buffer[ 0 ] = NULL;
				//MailStr.cnt = 0;
				// 文字列の長さ
				MailStr.len = MAIL_STR_LEN;
				// 一行の長さ
				MailStr.lineLen = 28;
				// 一行の間隔
				MailStr.lineDist = 20;
				// 文字列の色
				MailStr.color = 0;
				// 文字列の座標
				MailStr.x = pActMenuWnd->x + 12;
				MailStr.y = pActMenuWnd->y + 280;
				// 文字列の表示優先順位
				MailStr.fontPrio = FONT_PRIO_FRONT;
				
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					// ペットアクションアクションない時
					if( pActPet == NULL ){
						// ペットアクション作成
						pActPet = MakeAnimDisp( pActMenuWnd->x + 190, pActMenuWnd->y + 120, pet[ pc.mailPetNo ].graNo, ANIM_DISP_PET );
					}
					// アイテムアクションない時かつ、アイテム選択されている時
					if( pActMailItem == NULL && mailItemNo != -1 ){
						// アイテムアクション作成
						pActMailItem = MakeAnimDisp( pActMenuWnd->x + 56, pActMenuWnd->y + 96, pc.item[ mailItemNo ].graNo, ANIM_DISP_PET_ITEM );
						pActMailItem->atr |= ACT_ATR_INFO;
						pActMailItem->atr |= ACT_ATR_TYPE_ITEM;
						pActMailItem->dispPrio = DISP_PRIO_DRAG;
						// アイテム名コピー
						strcpy( pActMailItem->name, pc.item[ mailItemNo ].name );
					}
					// 送信ウィンドウ表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_MENU, CG_MAIL_WND_PET_SEND_WND, 1 );
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						int nowNo = mailPetSendWndPageNo * MAX_ADR_BOOK_COUNT;
						// ページ内ループ
						//for( i = mailViewWndPageNo * 4 ; i < mailViewWndPageNo * 4 + 4 ; i++ ){
						for( i = 0 ; i < MAX_ADR_BOOK_COUNT ; i++ ){
							// 送信相手選択した時（ペットメールは一人にしか送れない）
							if( HitFontNo == mailWndFontNo[ i ] ){
								// 名前がある所にデータが入っているとする
								if( addressBook[ nowNo + i ].useFlag == TRUE ){
									int j, flag = FALSE;
									// 既に選択してるかチェック
									for( j = 0 ; j < MAX_ADR_BOOK ; j++ ){ 
										if( mailWndSendFlag[ j ] == TRUE ) flag = TRUE;
									}
									
									// 選択してなかったら
									if( flag == FALSE ){
										mailWndSendFlag[ nowNo + i ] = TRUE;
										// クリック音
										play_se( 217, 320, 240 );
									}else{ // 既に選択されていたら
										// 選択されているところをクリックした時
										if( mailWndSendFlag[ nowNo + i ] == TRUE ){ 
											mailWndSendFlag[ nowNo + i ] = FALSE;
											// クリック音
											play_se( 217, 320, 240 );
										}else{ // 選択されていないところをクリックした時
											// ＮＧ音
											play_se( 220, 320, 240 );
										}
									}
								}
							}
						}
						// 戻る時
						if( HitDispNo == mailWndFontNo[ 19 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// ペット抹殺
							DeathAction( pActPet );
							pActPet = NULL;
							// アイテム抹殺
							DeathAction( pActMailItem );
							pActMailItem = NULL;
							// 一覧ウィンドウへ
							mailWndNo = MAIL_WND_VIEW;	
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
							// 入力フォーカス戻す
							GetKeyInputFocus( &MyChatBuffer );
						}
						// アイテムボタン
						if( HitDispNo == mailWndFontNo[ 15 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// ペット抹殺
							DeathAction( pActPet );
							pActPet = NULL;
							// アイテム抹殺
							DeathAction( pActMailItem );
							pActMailItem = NULL;
							// 前のウィンドウに戻る
							mailWndNo = MAIL_WND_ITEM;	
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
							// 入力フォーカス戻す
							GetKeyInputFocus( &MyChatBuffer );
						}
					}
					// 送信ボタン
					if( HitDispNo == mailWndFontNo[ 18 ] ){
						
						// 左クリックした時
						if( mouse.onceState & MOUSE_LEFT_CRICK ){
							int flag = FALSE;
							// 既に選択してるかチェック
							for( i = 0 ; i < MAX_ADR_BOOK ; i++ ){ 
								if( mailWndSendFlag[ i ] == TRUE ) flag = TRUE;
							}
							// 文字列がある時
							//if( MailStr.buffer[ 0 ] != NULL && flag == TRUE && mailItemNo != -1 ){
							if( MailStr.buffer[ 0 ] != NULL && flag == TRUE 
								&& pActPet != NULL 
								&& pActPet->actNo == ANIM_DISP_PET
								&& pc.mailPetNo != ItemMixPetNo ){
								// ペットが合成中じゃ無い時
								//&& pActPet2 == NULL ){
								// ２０人分ループ
								for( i = 0 ; i < MAX_ADR_BOOK ; i++ ){
									// フラグ立ってる時
									if( mailWndSendFlag[ i ] == TRUE ){
										char moji2[ 256 ];
										// とりあえずコピー
										strcpy( moji2, MailStr.buffer );
										// エスケープ処理
										makeSendString( moji2, moji, sizeof( moji ) );
										// ペットメール送信
										lssproto_PMSG_send( sockfd, i, pc.mailPetNo, mailItemNo, moji2, FONT_PAL_WHITE ); 
										//lssproto_PMSG_send( sockfd, i, pc.mailPetNo, mailItemNo, MailStr.buffer, FONT_PAL_WHITE ); 
										// メールペット番号初期化
										pc.mailPetNo = -1;
										// 合成フラグ立っていたら
										ItemBuffer[ mailItemNo ].mixFlag = 0;
										// パラメータセーブ
										saveUserSetting();
										// ペットアクションアクションある時
										if( pActPet != NULL ){
											// ペット画面外へ
											pActPet->actNo = 1;
										}
									}
								}
								// 文字列初期化
								//MailStr.buffer[ 0 ] = NULL;
								//MailStr.cnt = 0;
								// 送信音
								play_se( 101, 320, 240 );
								mailWndBtnFlag[ 18 ] = TRUE;
							}else{
								// ＮＧ音
								play_se( 220, 320, 240 );
							}
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[ 18 ] == TRUE ){
							// ボタン凹
							mailWndBtnFlag[ 18 ] = TRUE;
						}else{
							// ボタン凸
							mailWndBtnFlag[ 18 ] = FALSE;
						}
					}else{
						// ボタン凸
						mailWndBtnFlag[ 18 ] = FALSE;
					}
					
					// 前に戻る
					if( HitDispNo == mailWndFontNo[ 16 ] || joy_auto[ 0 ] & JOY_A ){
						// 左クリックした時
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_A ){
							mailPetSendWndPageNo--; // ページ戻す
							if( mailPetSendWndPageNo < 0 ) mailPetSendWndPageNo = MAX_ADR_BOOK_PAGE - 1;
							// クリック音
							play_se( 217, 320, 240 );
							mailWndBtnFlag[ 16 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[ 16 ] == TRUE ){
							// ボタン凹
							mailWndBtnFlag[ 16 ] = TRUE;
						}else{
							// ボタン凸
							mailWndBtnFlag[ 16 ] = FALSE;
						}
					}else{
						// ボタン凸
						mailWndBtnFlag[ 16 ] = FALSE;
					}
					// 次に進む
					if( HitDispNo == mailWndFontNo[ 17 ] || joy_auto[ 0 ] & JOY_B ){
						// 左クリックした時
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_B ){
							mailPetSendWndPageNo++; // ページ進ませる
							if( mailPetSendWndPageNo >= MAX_ADR_BOOK_PAGE ) mailPetSendWndPageNo = 0;
							// クリック音
							play_se( 217, 320, 240 );
							mailWndBtnFlag[ 17 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[ 17 ] == TRUE ){
							// ボタン凹
							mailWndBtnFlag[ 17 ] = TRUE;
						}else{
							// ボタン凸
							mailWndBtnFlag[ 17 ] = FALSE;
						}
					}else{
						// ボタン凸
						mailWndBtnFlag[ 17 ] = FALSE;
					}
					
					// 文章クリアボタン
					if( HitDispNo == mailWndFontNo[ 14 ] ){
						// 左クリックした時
						if( mouse.onceState & MOUSE_LEFT_CRICK ){
							// メール文章 初期設定
							MailStr.buffer[ 0 ] = NULL;
							MailStr.cnt = 0;
							// クリック音
							play_se( 217, 320, 240 );
							mailWndBtnFlag[ 14 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[ 14 ] == TRUE ){
							// ボタン凹
							mailWndBtnFlag[ 14 ] = TRUE;
						}else{
							// ボタン凸
							mailWndBtnFlag[ 14 ] = FALSE;
						}
					}else{
						// ボタン凸
						mailWndBtnFlag[ 14 ] = FALSE;
					}
					
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
						int nowNo = mailPetSendWndPageNo * MAX_ADR_BOOK_COUNT;
						int color;
						
						// 座標設定
						x = pActMenuWnd->x;
						y = pActMenuWnd->y;
						// 入力フォーカス取得
						GetKeyInputFocus( &MailStr );
						
						// ペットの名前表示
						// あだ名が無い時
						if( pet[ pc.mailPetNo ].freeName[ 0 ] == NULL ){
							//StockFontBuffer( x + 120, y + 14, FONT_PRIO_FRONT, 0, pet[ pc.mailPetNo ].name, 0 );
							CenteringStr( pet[ pc.mailPetNo ].name, moji, PET_NAME_LEN );
						}else{
							//StockFontBuffer( x + 120, y + 14, FONT_PRIO_FRONT, 0, pet[ pc.mailPetNo ].freeName, 0 );
							CenteringStr( pet[ pc.mailPetNo ].freeName, moji, PET_NAME_LEN );
						}
						StockFontBuffer( x + 120, y + 14, FONT_PRIO_FRONT, 0, moji, 0 );
						//StockFontBuffer( x + 120, y + 14, FONT_PRIO_FRONT, 0, "0123456789012345", 0 );
						//StockFontBuffer( x + 120, y + 14, FONT_PRIO_FRONT, 0, moji"ああああああああ", 0 );
						
						// 文字列の表示
						StockFontBuffer2( &MailStr );
						
						// ページ内ループ
						//for( i = mailViewWndPageNo * 4 ; i < mailViewWndPageNo * 4 + 4 ; i++ ){
						//for( i = 0 ; i < 4 ; i++ ){
						for( i = 0 ; i < MAX_ADR_BOOK_COUNT ; i++ ){
							// 名前がある所にデータが入っているとする
							if( addressBook[ nowNo + i ].useFlag == TRUE ){
								color = FONT_PAL_WHITE;	// デフォルト色
								// 選択されている時
								if( mailWndSendFlag[ nowNo + i ] == TRUE ){
									// 黄色
									color = FONT_PAL_AQUA;
								}
								// 名前表示
								CenteringStr( addressBook[ nowNo + i ].name, moji, CHAR_NAME_LEN );
								mailWndFontNo[ i ] = StockFontBuffer( x + 111, y + 177, FONT_PRIO_FRONT, color, moji, 2 );
								//mailWndFontNo[ i ] = StockFontBuffer( x + 111, y + 177, FONT_PRIO_FRONT, color, "aaaaaaaaaaaaaaaa", 2 );
								//mailWndFontNo[ i ] = StockFontBuffer( x + 111, y + 177, FONT_PRIO_FRONT, color, "ああああああああ", 2 );
								
							}
							y += 23;
						}
						// アイテムボタン
						mailWndFontNo[ 15 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_IME3, CG_MAIL_WND_ITEM_BTN, 2 );
						// 前ボタン
						mailWndFontNo[ 16 ] = StockDispBuffer( pActMenuWnd->x + 25 - 8 + 4, pActMenuWnd->y + 241, DISP_PRIO_IME3, CG_PREV_BTN + mailWndBtnFlag[ 16 ], 2 );
						// ページ番号表示
						sprintf( moji,"%2d", mailPetSendWndPageNo + 1 );
						StockFontBuffer( pActMenuWnd->x + 46 - 8 + 4, pActMenuWnd->y + 233, FONT_PRIO_FRONT, 0, moji, 0 );
						// 次ボタン
						mailWndFontNo[ 17 ] = StockDispBuffer( pActMenuWnd->x + 75 + 4, pActMenuWnd->y + 241, DISP_PRIO_IME3, CG_NEXT_BTN + mailWndBtnFlag[ 17 ], 2 );
						// 文章クリアボタン
						mailWndFontNo[ 14 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx + 60, pActMenuWnd->y + 390, DISP_PRIO_IME3, CG_MAIL_WND_CLEAR_BTN_UP + mailWndBtnFlag[ 14 ], 2 );
						// 送信ボタン
						mailWndFontNo[ 18 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx - 60, pActMenuWnd->y + 421, DISP_PRIO_IME3, CG_SEND_BTN + mailWndBtnFlag[ 18 ] * 75, 2 );
						// 戻る
						mailWndFontNo[ 19 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx + 60, pActMenuWnd->y + 421, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );
					}
				}
			}
			break;

		case MAIL_WND_ITEM:	// アイテム選択ウィンドウ
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				//pActMenuWnd = MakeWindowDisp( 0, 0, 3, 6, CG_WND_TITLE_SYSTEM, 1 );
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 4, 24, 271, 281, 0, -1 );
				// 当たり判定初期化
				for( i = 0 ; i < MENU_MAIL_0 ; i++ ) mailWndFontNo[ i ] = -2;
				mailSendWndPageNo = 0;		// 送信相手ページ番号初期化
				// アイテム関連の初期化
				InitItem( pActMenuWnd->x, pActMenuWnd->y );
				// アイテムウィンドウの時
				if( MenuToggleFlag & JOY_CTRL_I && itemWndNo == 0 ){
					// アイテムウィンドウ閉じる
					MenuToggleFlag &= ~JOY_CTRL_I;
					DeathMenuAction2();
				}
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					// アイテムウィンドウ下表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my - 80, DISP_PRIO_MENU, CG_ITEM_WND_1, 1 );
					// アイテムウィンドウタイトル表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my - 80, DISP_PRIO_MENU, CG_BTL_ITEM_WND_TITLE, 1 );
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// 戻る時
						if( HitDispNo == mailWndFontNo[ 19 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// 一覧ウィンドウへ
							mailWndNo = MAIL_WND_PET_SEND;	
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
					}
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
						// 座標設定
						x = pActMenuWnd->x + 16;
						y = pActMenuWnd->y + 191 + 7;
						
						// アイテムループ
						for( i = 5 ; i < MAX_ITEM ; i++ ){
							// 枠内に入っていたら
							if( MakeHitBox( ItemBuffer[ i ].defX - 24, ItemBuffer[ i ].defY - 24 - 160, 
											ItemBuffer[ i ].defX + 25, ItemBuffer[ i ].defY + 23 - 160, DISP_PRIO_IME3 ) == TRUE ){
								// その欄にアイテムがあったら
								if( pc.item[ i ].useFlag == TRUE ){ 
									int color = FONT_PAL_WHITE;
										
									// 改行ポイント
									char *splitPoint = pc.item[ i ].memo;
									//char *splitPoint = "表示テストです。私の名前は太田哲生です。これからもよろしくお願いします。";
									// 装備できるかチェック
									if( pc.level < pc.item[ i ].level ) color = FONT_PAL_RED;
									// アイテムの名前表示
									StockFontBuffer( pActMenuWnd->x + 16, pActMenuWnd->y + 332 - 160, FONT_PRIO_FRONT, color, pc.item[ i ].name, 0 );	
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
								
								// 左ダブルクリックでアイテム使用
								if( mouse.onceState & MOUSE_LEFT_DBL_CRICK ){
									// その欄にアイテムがあって、送信可能のアイテムの時
									//if( pc.item[ i ].useFlag == TRUE && pc.item[ i ].sendFlag == TRUE ){
									if( pc.item[ i ].useFlag == TRUE && pc.item[ i ].sendFlag & ITEM_FLAG_PET_MAIL ){
										mailItemNo = i;	// アイテム番号記憶
										// ウィンドウ抹殺
										DeathAction( pActMenuWnd );
										pActMenuWnd = NULL;
										// 一覧ウィンドウへ
										mailWndNo = MAIL_WND_PET_SEND;	
										// ウィンドウ閉じる音
										play_se( 203, 320, 240 );
										// 入力フォーカス戻す
										GetKeyInputFocus( &MyChatBuffer );
									}else{
										// ＮＧ音
										play_se( 220, 320, 240 );
									}
								}
							}
							// その欄にアイテムがあったら
							//if( pc.item[ i ].useFlag == TRUE && ItemBuffer[ i ].dragFlag == FALSE ){ 
							if( pc.item[ i ].useFlag == TRUE ){ 
								// アイテム表示
								StockDispBuffer( ItemBuffer[ i ].defX, ItemBuffer[ i ].defY - 160, ItemBuffer[ i ].dispPrio, pc.item[ i ].graNo, 0 );
							}
						}
						// ウィンドウあったら
						if( pActMenuWnd != NULL ){
							// 文字表示バッファに溜める
							mailWndFontNo[ 19 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, pActMenuWnd->y + 262, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
						}
					}
				}
			}
			break;
			
		case MAIL_WND_HISTORY:	// 履歴ウィンドウ
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				//pActMenuWnd = MakeWindowDisp( 0, 0, 3, 6, CG_WND_TITLE_SYSTEM, 1 );
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 4, 4, 272, 424, 0, -1 );
				// 当たり判定初期化
				for( i = 0 ; i < MENU_MAIL_0 ; i++ ) mailWndFontNo[ i ] = -2;
				
				// 表示するページを設定（一番古い未読メールを選択する）
				// 履歴分ループ		
				for( mailHistoryWndPageNo = MAIL_MAX_HISTORY - 1 ; mailHistoryWndPageNo > 0 ; mailHistoryWndPageNo-- ){
					// リミットチェック
					if( mailHistoryWndPageNo < 0 ) mailHistoryWndPageNo = MAIL_MAX_HISTORY - 1;
					// 実際に表示する履歴番号
					mailHistoryWndNowPageNo = ( MailHistory[ mailHistoryWndSelectNo ].newHistoryNo + mailHistoryWndPageNo ) % MAIL_MAX_HISTORY;
					// 未読フラグ立っている時、抜ける
					if( MailHistory[ mailHistoryWndSelectNo ].noReadFlag[ mailHistoryWndNowPageNo ] >= TRUE ) break;
				}
#if 0
				// ペットアクションアクションない時
				if( pActPet == NULL ){
					// ペットアクション作成
					pActPet = MakeAnimDisp( pActMenuWnd->x - 204, pActMenuWnd->y + 160, pet[ pc.mailPetNo ].graNo, ANIM_DISP_PET_RECIEVE );
					pActPet->atr |= ACT_ATR_INFO;
					pActPet->atr |= ACT_ATR_TYPE_PET;
					pActPet->level = 32;
					strcpy( pActPet->name, "テストペット" );
				}
				// アイテムアクションない時かつ、アイテムが届いている時
				if( pActMailItem == NULL && MailHistory[ mailHistoryWndSelectNo ].itemGraNo[ mailHistoryWndNowPageNo ] != -1 ){
					// アイテムアクション作成
					pActMailItem = MakeAnimDisp( pActMenuWnd->x - 204, pActMenuWnd->y + 128, pc.item[ 5 ].graNo, ANIM_DISP_PET_ITEM );
					pActMailItem->atr |= ACT_ATR_INFO;
					pActMailItem->atr |= ACT_ATR_TYPE_ITEM;
					pActMailItem->dispPrio = DISP_PRIO_DRAG;
					// アイテム名コピー
					strcpy( pActMailItem->name, "届いたアイテム" );
				}
#endif
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					static int nowDelNo;
					// 履歴ウィンドウ表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_MENU, CG_MAIL_WND_HISTORY_WND, 1 );
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// メールボタン
						if( HitDispNo == mailWndFontNo[ 18 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// 手紙アニメーション抹殺
							DeathLetterAction();
							// ペット抹殺
							DeathAction( pActPet );
							pActPet = NULL;
							// アイテム抹殺
							DeathAction( pActMailItem );
							pActMailItem = NULL;
							// ウィンドウ番号バックアップ
							mailWndNoBak = mailWndNo;
							// 送信相手フラグ初期化
							InitMailSendFlag();
							// 送信相手だけフラグ立てる
							mailWndSendFlag[ mailHistoryWndSelectNo ] = TRUE;
							// メールペットが選択されていたら
							if( pc.mailPetNo != -1 ){
								// 選択ウィンドウへ
								mailWndNo = MAIL_WND_SELECT;
							}else{
								// 普通メール送信ウィンドウ
								mailWndNo = MAIL_WND_SEND;
							}
							// ウィンドウ開く音
							play_se( 202, 320, 240 );
						}
						// 戻る時
						if( HitDispNo == mailWndFontNo[ 19 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// 手紙アニメーション抹殺
							DeathLetterAction();
							// ペット抹殺
							DeathAction( pActPet );
							pActPet = NULL;
							// アイテム抹殺
							DeathAction( pActMailItem );
							pActMailItem = NULL;
							// 一覧ウィンドウへ
							mailWndNo = MAIL_WND_VIEW;	
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
					}
					// 前に戻る
					if( HitDispNo == mailWndFontNo[ 16 ] || joy_auto[ 0 ] & JOY_A ){
						// 左クリックした時
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_A ){
							mailHistoryWndPageNo--; // ページ戻す
							if( mailHistoryWndPageNo < 0 ) mailHistoryWndPageNo = MAIL_MAX_HISTORY - 1;
							// 手紙アニメーション抹殺
							DeathLetterAction();
							// ペット抹殺
							DeathAction( pActPet );
							pActPet = NULL;
							// アイテム抹殺
							DeathAction( pActMailItem );
							pActMailItem = NULL;
							// クリック音
							play_se( 217, 320, 240 );
							mailWndBtnFlag[ 16 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[ 16 ] == TRUE ){
							// ボタン凹
							mailWndBtnFlag[ 16 ] = TRUE;
						}else{
							// ボタン凸
							mailWndBtnFlag[ 16 ] = FALSE;
						}
					}else{
						// ボタン凸
						mailWndBtnFlag[ 16 ] = FALSE;
					}
					// 次に進む
					if( HitDispNo == mailWndFontNo[ 17 ] || joy_auto[ 0 ] & JOY_B ){
						// 左クリックした時
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_B ){
							mailHistoryWndPageNo++; // ページ進ませる
							if( mailHistoryWndPageNo >= MAIL_MAX_HISTORY ) mailHistoryWndPageNo = 0;
							// 手紙アニメーション抹殺
							DeathLetterAction();
							// ペット抹殺
							DeathAction( pActPet );
							pActPet = NULL;
							// アイテム抹殺
							DeathAction( pActMailItem );
							pActMailItem = NULL;
							// クリック音
							play_se( 217, 320, 240 );
							mailWndBtnFlag[ 17 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[ 17 ] == TRUE ){
							// ボタン凹
							mailWndBtnFlag[ 17 ] = TRUE;
						}else{
							// ボタン凸
							mailWndBtnFlag[ 17 ] = FALSE;
						}
					}else{
						// ボタン凸
						mailWndBtnFlag[ 17 ] = FALSE;
					}
					
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
						// 座標設定
						x = pActMenuWnd->x;
						y = pActMenuWnd->y;
						char *splitPoint;
						// 実際に表示する履歴番号
						mailHistoryWndNowPageNo = ( MailHistory[ mailHistoryWndSelectNo ].newHistoryNo + mailHistoryWndPageNo ) % MAIL_MAX_HISTORY;
						// 未読フラグ立っている時
						if( MailHistory[ mailHistoryWndSelectNo ].noReadFlag[ mailHistoryWndNowPageNo ] >= TRUE ){
							// ペットメールの時
							if( MailHistory[ mailHistoryWndSelectNo ].noReadFlag[ mailHistoryWndNowPageNo ] > TRUE ){
								// ペットアクションアクションない時
								if( pActPet == NULL ){
									// ペットアクション作成
									pActPet = MakeAnimDisp( pActMenuWnd->x - 204, pActMenuWnd->y + 160, MailHistory[ mailHistoryWndSelectNo ].noReadFlag[ mailHistoryWndNowPageNo ], ANIM_DISP_PET_RECIEVE );
									pActPet->atr |= ACT_ATR_INFO;
									pActPet->atr |= ACT_ATR_TYPE_PET;
									// ペットのＬＶ
									pActPet->level = MailHistory[ mailHistoryWndSelectNo ].petLevel[ mailHistoryWndNowPageNo ];
									// ペットの名前
									strcpy( pActPet->name, MailHistory[ mailHistoryWndSelectNo ].petName[ mailHistoryWndNowPageNo ] );
								}
								// アイテムアクションない時かつ、アイテムが届いている時
								if( pActMailItem == NULL && MailHistory[ mailHistoryWndSelectNo ].itemGraNo[ mailHistoryWndNowPageNo ] != -1 ){
									// アイテムアクション作成
									pActMailItem = MakeAnimDisp( pActMenuWnd->x - 204, pActMenuWnd->y + 128, MailHistory[ mailHistoryWndSelectNo ].itemGraNo[ mailHistoryWndNowPageNo ], ANIM_DISP_PET_ITEM );
									pActMailItem->atr |= ACT_ATR_INFO;
									pActMailItem->atr |= ACT_ATR_TYPE_ITEM;
									pActMailItem->dispPrio = DISP_PRIO_DRAG;
									// アイテム名コピー
									strcpy( pActMailItem->name, "届いたアイテム" );
								}
							}
							// 手紙アニメーションない時
							if( pActLetter[ 0 ] == NULL ){
								// アニメーションタスク作成
								pActLetter[ 0 ] = MakeAnimDisp( x + 28, y + 244, SPR_mail, ANIM_DISP_LETTER );
							}
							// 既読にする
							MailHistory[ mailHistoryWndSelectNo ].noReadFlag[ mailHistoryWndNowPageNo ] = FALSE;
							// メールデータの保存
							SaveMailHistory( mailHistoryWndSelectNo );
							// 未読チェック
							mailLamp = CheckMailNoReadFlag();
							
						}
						// 改行ポイント
						splitPoint = MailHistory[ mailHistoryWndSelectNo ].str[ mailHistoryWndNowPageNo ];
						//char *splitPoint = "１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０";
						// 名前表示
						CenteringStr( addressBook[ mailHistoryWndSelectNo ].name, moji, CHAR_NAME_LEN );
						StockFontBuffer( x + 64, y + 34, FONT_PRIO_FRONT, 0, moji, 0 );
						// 日付表示
						StockFontBuffer( x + 148, y + 236, FONT_PRIO_FRONT, 0, MailHistory[ mailHistoryWndSelectNo ].dateStr[ mailHistoryWndNowPageNo ], 0 );
						//StockFontBuffer( x + 128, y + 240, FONT_PRIO_FRONT, 0, " 9/14 12:10", 0 );
						
						// 文章表示
						while( 1 ){
							// ２２文字を超えてたら分割
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
								StockFontBuffer( x + 12, y + 260, FONT_PRIO_FRONT, 0, moji, 0 ); y += 20;
							}else{
								strcpy( moji, splitPoint );
								StockFontBuffer( x + 12, y + 260, FONT_PRIO_FRONT, 0, moji, 0 );
								break;
							}
						}
						// 前ボタン
						mailWndFontNo[ 16 ] = StockDispBuffer( pActMenuWnd->x + 25 + 16 + 8, pActMenuWnd->y + 405, DISP_PRIO_IME3, CG_PREV_BTN + mailWndBtnFlag[ 16 ], 2 );
						// ページ番号表示
						sprintf( moji,"%2d", mailHistoryWndPageNo + 1 );
						StockFontBuffer( pActMenuWnd->x + 46 + 16 + 9, pActMenuWnd->y + 397, FONT_PRIO_FRONT, 0, moji, 0 );
						// 次ボタン
						mailWndFontNo[ 17 ] = StockDispBuffer( pActMenuWnd->x + 75 + 16 + 9 + 8, pActMenuWnd->y + 405, DISP_PRIO_IME3, CG_NEXT_BTN + mailWndBtnFlag[ 17 ], 2 );
						// メールボタン
						mailWndFontNo[ 18 ] = StockDispBuffer( pActMenuWnd->x + 196, pActMenuWnd->y + 371, DISP_PRIO_IME3, CG_MAIL_WND_MAIL_BTN, 2 );
						// 閉じるボタン
						mailWndFontNo[ 19 ] = StockDispBuffer( pActMenuWnd->x + 220 -16 - 8, pActMenuWnd->y + 405, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );
					}
				}
			}
			break;
			
			
		}
	}
	
//◎ アルバムウィンドウ処理 ***********专辑窗口处理********************************************/

	if( MenuToggleFlag & JOY_CTRL_A ){
	
		// ウィンドウ番号で分岐
		switch( albumWndNo ){
		
		case 0:	// 自分のステータス
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 4, 4, 272, 448, 0, -1 );
				// 当たり判定初期化
				for( i = 0 ; i < MENU_ALBUM_0 ; i++ ) albumWndFontNo[ i ] = -2;
				// ページ番号初期化
				albumWndPageNo = 0;
				// アルバム番号初期化
				albumNo = -1;
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					// アルバムウィンドウ表示
					StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my, DISP_PRIO_MENU, CG_ALBUM_WND, 1 );
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// ページの先頭アルバム番号
						int nowTopNo = albumWndPageNo * 8;
						// １ページ分ループ
						for( i = 0 ; i < 8 ; i++ ){
							// ペット選んだ時、かつ写真とっているとき
							if( HitFontNo == albumWndFontNo[ i ] && PetAlbum[ nowTopNo + i ].flag >= 2 ){
							//if( HitFontNo == albumWndFontNo[ i ] ){
								// アルバム番号記憶
								albumNo = nowTopNo + i;
								// 新規でなくする
								PetAlbum[ albumNo ].flag = 3;
								// アルバムの保存
								SaveAlbum( albumNo );
								// クリック音
								play_se( 217, 320, 240 );
							}
						}
						// 閉じる時
						if( HitDispNo == albumWndFontNo[ 19 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// ウィンドウ閉じる
							MenuToggleFlag ^= JOY_CTRL_A;
							// ウィンドウ閉じる音
							play_se( 203, 320, 240 );
						}
					}
					
					// 前に戻る
					if( HitDispNo == albumWndFontNo[ 16 ] || joy_auto[ 0 ] & JOY_A ){
						// 左クリックした時
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_A){
							int limitPage = MAX_PET_KIND / MAX_PET_PAGE_SIZE;
							albumWndPageNo--; // ページ戻す
							// リミットチェック
							if( albumWndPageNo < 0 ) albumWndPageNo = limitPage;
							// クリック音
							play_se( 217, 320, 240 );
							albumWndBtnFlag[ 16 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && albumWndBtnFlag[ 16 ] == TRUE ){
							// ボタン凹
							albumWndBtnFlag[ 16 ] = TRUE;
						}else{
							// ボタン凸
							albumWndBtnFlag[ 16 ] = FALSE;
						}
					}else{
						// ボタン凸
						albumWndBtnFlag[ 16 ] = FALSE;
					}
					// 次に進む
					if( HitDispNo == albumWndFontNo[ 17 ] || joy_auto[ 0 ] & JOY_B ){
						// 左クリックした時
						if( mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[ 0 ] & JOY_B ){
							int limitPage = MAX_PET_KIND / MAX_PET_PAGE_SIZE;
							albumWndPageNo++; // ページ進ませる
							// リミットチェック
							if( albumWndPageNo > limitPage ) albumWndPageNo = 0;
							// クリック音
							play_se( 217, 320, 240 );
							albumWndBtnFlag[ 17 ] = TRUE;
						}
						// 左クリックしてる時
						if( mouse.state & MOUSE_LEFT_CRICK && albumWndBtnFlag[ 17 ] == TRUE ){
							// ボタン凹
							albumWndBtnFlag[ 17 ] = TRUE;
						}else{
							// ボタン凸
							albumWndBtnFlag[ 17 ] = FALSE;
						}
					}else{
						// ボタン凸
						albumWndBtnFlag[ 17 ] = FALSE;
					}
					
					
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
						#define ALBUM_WND_ATTR_X 34
						#define ALBUM_WND_ATTR_Y 0
						int x2;
						int nowTopNo;	// ページの先頭アルバム番号
						// 座標設定
						x = pActMenuWnd->x;
						y = pActMenuWnd->y + 28;
						
						// アルバム番号ある時
						if( albumNo != -1 ){
							// 名前文字列のセンターリング
							CenteringStr( PetAlbum[ albumNo ].freeName, moji, PET_NAME_LEN );
							//CenteringStr( "ＡＢＣＤＥＦＧＨ", moji, PET_NAME_LEN );
							//CenteringStr( "AAAAAAAAAAAAAAAA", moji, PET_NAME_LEN );
							StockFontBuffer( x + 13, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 22; 
							//StockFontBuffer( x - 7, y, FONT_PRIO_FRONT, 0, "世代表示", 0 ); 
							
							sprintf( moji,"%3d",PetAlbum[ albumNo ].level );
							//sprintf( moji,"999",PetAlbum[ albumNo ].level );
							StockFontBuffer( x + 36, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
							
							sprintf( moji, "%4d", PetAlbum[ albumNo ].maxHp );
							//sprintf( moji,"9999",PetAlbum[ albumNo ].level );
							StockFontBuffer( x + 36, y, FONT_PRIO_FRONT, 0, moji, 0 );
							
							sprintf( moji, "%3d", PetAlbum[ albumNo ].str );
							//sprintf( moji,"999",PetAlbum[ albumNo ].level );
							StockFontBuffer( x + 36 + 72, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 24;
							
							sprintf( moji, "%4d", PetAlbum[ albumNo ].quick );
							//sprintf( moji,"9999",PetAlbum[ albumNo ].level );
							StockFontBuffer( x + 36, y, FONT_PRIO_FRONT, 0, moji, 0 );
							
							sprintf( moji, "%3d", PetAlbum[ albumNo ].def );
							//sprintf( moji,"999",PetAlbum[ albumNo ].level );
							StockFontBuffer( x + 36 + 72, y, FONT_PRIO_FRONT, 0, moji, 0 ); y += 29;
							
							// 地属性ある時
							if( PetAlbum[ albumNo ].earth > 0 ){
								x2 = ( int )( PetAlbum[ albumNo ].earth * 0.8 );
								
								StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 0, y + ALBUM_WND_ATTR_Y + 0, x + ALBUM_WND_ATTR_X + 0 + x2, y + ALBUM_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_GREEN, 1 );
								StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 1, y + ALBUM_WND_ATTR_Y + 1, x + ALBUM_WND_ATTR_X + 1 + x2, y + ALBUM_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_GREEN2, 0 );
								StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 2, y + ALBUM_WND_ATTR_Y + 2, x + ALBUM_WND_ATTR_X + 2 + x2, y + ALBUM_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_GREEN2, 0 );
								//StockBoxDispBuffer( x + 126, y + 5, x + 126 + x2earth, y + 13, DISP_PRIO_IME3, SYSTEM_PAL_BLACK, 0 );
							}
							y += 20;
							
							// 水属性ある時
							if( PetAlbum[ albumNo ].water > 0 ){
								x2 = ( int )( PetAlbum[ albumNo ].water * 0.8 );
								
								//StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 0, y + ALBUM_WND_ATTR_Y + 0, x + ALBUM_WND_ATTR_X + 0 + x2, y + ALBUM_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_BLUE, 1 );
								//StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 1, y + ALBUM_WND_ATTR_Y + 1, x + ALBUM_WND_ATTR_X + 1 + x2, y + ALBUM_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_BLUE2, 0 );
								//StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 2, y + ALBUM_WND_ATTR_Y + 2, x + ALBUM_WND_ATTR_X + 2 + x2, y + ALBUM_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_BLUE2, 0 );
								
								StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 0, y + ALBUM_WND_ATTR_Y + 0, x + ALBUM_WND_ATTR_X + 0 + x2, y + ALBUM_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_AQUA, 1 );
								StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 1, y + ALBUM_WND_ATTR_Y + 1, x + ALBUM_WND_ATTR_X + 1 + x2, y + ALBUM_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_AQUA2, 0 );
								StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 2, y + ALBUM_WND_ATTR_Y + 2, x + ALBUM_WND_ATTR_X + 2 + x2, y + ALBUM_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_AQUA2, 0 );
								//StockBoxDispBuffer( x + 126, y + 5, x + 126 + x2water, y + 13, DISP_PRIO_IME3, SYSTEM_PAL_BLACK, 0 );
							}
							y += 20;
							
							// 火属性ある時
							if( PetAlbum[ albumNo ].fire > 0 ){
								x2 = ( int )( PetAlbum[ albumNo ].fire * 0.8 );
								
								StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 0, y + ALBUM_WND_ATTR_Y + 0, x + ALBUM_WND_ATTR_X + 0 + x2, y + ALBUM_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_RED, 1 );
								StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 1, y + ALBUM_WND_ATTR_Y + 1, x + ALBUM_WND_ATTR_X + 1 + x2, y + ALBUM_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_RED2, 0 );
								StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 2, y + ALBUM_WND_ATTR_Y + 2, x + ALBUM_WND_ATTR_X + 2 + x2, y + ALBUM_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_RED2, 0 );
								//StockBoxDispBuffer( x + 126, y + 5, x + 126 + x2fire, y + 13, DISP_PRIO_IME3, SYSTEM_PAL_BLACK, 0 );
							}
							y += 20;
							
							// 風属性ある時
							if( PetAlbum[ albumNo ].wind > 0 ){
								x2 = ( int )( PetAlbum[ albumNo ].wind * 0.8 );
								
								StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 0, y + ALBUM_WND_ATTR_Y + 0, x + ALBUM_WND_ATTR_X + 0 + x2, y + ALBUM_WND_ATTR_Y + 8, DISP_PRIO_IME4, SYSTEM_PAL_YELLOW, 1 );
								StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 1, y + ALBUM_WND_ATTR_Y + 1, x + ALBUM_WND_ATTR_X + 1 + x2, y + ALBUM_WND_ATTR_Y + 9, DISP_PRIO_IME3, SYSTEM_PAL_YELLOW2, 0 );
								StockBoxDispBuffer( x + ALBUM_WND_ATTR_X + 2, y + ALBUM_WND_ATTR_Y + 2, x + ALBUM_WND_ATTR_X + 2 + x2, y + ALBUM_WND_ATTR_Y + 10, DISP_PRIO_IME3, SYSTEM_PAL_YELLOW2, 0 );
								//StockBoxDispBuffer( x + 126, y + 5, x + 126 + x2wind, y + 13, DISP_PRIO_IME3, SYSTEM_PAL_BLACK, 0 );
							}
							
							// 顔グラフィック表示
							//StockDispBuffer( pActMenuWnd->x + 151 + 48, pActMenuWnd->y + 65 + 52, DISP_PRIO_IME3, 28001, 0 );
							StockDispBuffer( pActMenuWnd->x + 151 + 48, pActMenuWnd->y + 65 + 52, DISP_PRIO_IME3, PetAlbum[ albumNo ].faceGraNo, 0 );
						}
						
						y = pActMenuWnd->y + 215;
						
						// ページの先頭アルバム番号
						nowTopNo = albumWndPageNo * 8;
						// 一ページ分ループ
						for( i = 0 ; i < 8 ; i++ ){
							// リミットチェック
							if( nowTopNo + i >= MAX_PET_KIND ) break;
							
							// 新規フラグ立っていたら
							if( PetAlbum[ nowTopNo + i ].flag == 2 ){
								// 新規表示
								StockDispBuffer( x + 24, y + 9, DISP_PRIO_IME3, CG_ALBUM_WND_NEW_ICON, 0 );
							}
							// アルバムＮＯ表示
							sprintf( moji,"No.%3d",nowTopNo + i + 1 );
							//sprintf( moji,"No.999");
							StockFontBuffer( x + 46, y + 1, FONT_PRIO_FRONT, 0, moji, 0 );
							
							// ペットの名前表示
							// フラグ立ってる時
							if( PetAlbum[ nowTopNo + i ].flag != 0 ){
								// 見たとき
								if( PetAlbum[ nowTopNo + i ].flag == 1 ){
									CenteringStr( PetAlbum[ nowTopNo + i ].name, moji, PET_NAME_LEN );
									//CenteringStr( "ＡＢＣＤＥＦＧＨ", moji, PET_NAME_LEN );
									StockFontBuffer( x + 112, y, FONT_PRIO_FRONT, FONT_PAL_GRAY, moji, 0 );
									// 当たり判定番号初期化
									albumWndFontNo[ i ] = -2;
								}else
								// 写真撮った時
								if( PetAlbum[ nowTopNo + i ].flag >= 2 ){
									CenteringStr( PetAlbum[ nowTopNo + i ].name, moji, PET_NAME_LEN );
									//CenteringStr( "ＡＢＣＤＥＦＧＨ", moji, PET_NAME_LEN );
									albumWndFontNo[ i ] = StockFontBuffer( x + 112, y, FONT_PRIO_FRONT, 0, moji, 2 );
								}
							}else{
								// 当たり判定番号初期化
								albumWndFontNo[ i ] = -2;
							}
							y += 22;
						}
						
						// 前ボタン
						albumWndFontNo[ 16 ] = StockDispBuffer( pActMenuWnd->x + 25 + 32 + 10 + 40, pActMenuWnd->y + 400, DISP_PRIO_IME3, CG_PREV_BTN + albumWndBtnFlag[ 16 ], 2 );
						// ページ番号表示
						//sprintf( moji,"%2d", albumHistoryWndPageNo + 1 );
						//StockFontBuffer( pActMenuWnd->x + 46 + 16 + 9, pActMenuWnd->y + 397, FONT_PRIO_FRONT, 0, moji, 0 );
						// 次ボタン
						albumWndFontNo[ 17 ] = StockDispBuffer( pActMenuWnd->x + 75 + 32 + 9 + 10 + 40, pActMenuWnd->y + 400, DISP_PRIO_IME3, CG_NEXT_BTN + albumWndBtnFlag[ 17 ], 2 );
						// 閉じるボタン
						albumWndFontNo[ 19 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, pActMenuWnd->y + 448 - 19, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
					}
				}
			}
			break;
		}
	}

//◎ 戦闘結果ウィンドウ ***********************************************************/
	if( BattleResultWndFlag >= 1 && ProcNo == PROC_GAME && SubProcNo == 3 ){
		// 時間がきたら
		if( -- ResultWndTimer <= 0 ){
			// ウィンドウ抹殺
			DeathAction( pActMenuWnd );
			pActMenuWnd = NULL;
			// システムウィンドウ閉じる
			BattleResultWndFlag = FALSE;
			// マップウィンドウ出さない時
			if( MapWmdFlagBak != TRUE ){
				// ウィンドウ閉じる音
				play_se( 203, 320, 240 );
			}
		}
		
		// 戦闘結果ウィンドウの種類で分岐
		switch( BattleResultWndFlag ){
		
		case 1:	// 経験値ウィンドウの時
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				int flag = 0;
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 320 - 160, 240 - 96 - 24, 5, 5, CG_WND_TITLE_RESULT, 2 );
				// 当たり判定初期化
				for( i = 0 ; i < MENU_BTL_RESULT_0 ; i++ ) resultWndFontNo[ i ] = -2;
				// レベルアップチェック
				for( i = 0 ; i < 4 ; i++ ){
					// レベルアップの時フラグＯＮ
					if( battleResultMsg.resChr[ i ].levelUp == TRUE ) flag = TRUE;
				}
				if( flag == TRUE ){
					// レベルアップ音
					play_se( 211, 320, 240 );
				}else{
					// ウィンドウ開く音
					play_se( 202, 320, 240 );
				}
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// 閉じる時
						if( HitDispNo == resultWndFontNo[ 0 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// システムウィンドウ閉じる
							BattleResultWndFlag = FALSE;
							// マップウィンドウ出さない時
							if( MapWmdFlagBak != TRUE ){
								// ウィンドウ閉じる音
								play_se( 203, 320, 240 );
							}
						}
					}
					
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
						int color;
						int flag, j;
						// 座標設定
						x = pActMenuWnd->x + 12;
						y = pActMenuWnd->y + 44;
						
						// 自分の名前
						//sprintf( moji,"%s", pc.name );
						CenteringStr( pc.name, moji, CHAR_NAME_LEN );
						//sprintf( moji,"0000000000000000", pc.name );
						StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 0 ); 
						color = FONT_PAL_GRAY;	// デフォルト色
						// レベルアップの時
						if( battleResultMsg.resChr[ 0 ].levelUp == TRUE ){
							// レベルアップ表示
							StockFontBuffer( x + 152, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "LvUp!", 0 ); 
						}
						// 経験値表示
						sprintf( moji,"Exp %+5d", battleResultMsg.resChr[ 0 ].exp );
						StockFontBuffer( x + 148 + 58, y, FONT_PRIO_FRONT, 0, moji, 0 ); 
						y += 20;
						
						// ペット分ループ
						for( i = 0 ; i < 5 ; i++ ){
							// この欄にペットがいる時 かつ スタメンの時
							if( pet[ i ].useFlag == TRUE && pc.selectPetNo[ i ] == TRUE ){
								// 名前の文字列作成
								// あだ名がある時
								if( pet[ i ].freeName[ 0 ] != NULL )
									//sprintf( moji, "%s ", pet[ i ].freeName );
									CenteringStr( pet[ i ].freeName, moji, PET_NAME_LEN );
								else
									//sprintf( moji, "%s ", pet[ i ].name );
									CenteringStr( pet[ i ].name, moji, PET_NAME_LEN );
								StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 0 );
								color = FONT_PAL_GRAY;	// デフォルト色
								flag = FALSE;			// フラグ初期化
								// 戦闘結果構造体の中から探す。
								for( j = 1 ; j < 4 ; j++ ){
									// あったら
									if( battleResultMsg.resChr[ j ].petNo == i ){
										// レベルアップの時
										if( battleResultMsg.resChr[ j ].levelUp == TRUE ){	// レベルアップの時
											// レベルアップ表示
											StockFontBuffer( x + 152, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW,"LvUp!", 0 ); 
										}
										// 経験値表示
										sprintf( moji,"Exp %+5d", battleResultMsg.resChr[ j ].exp );
										StockFontBuffer( x + 148 + 58, y, FONT_PRIO_FRONT, 0, moji, 0 ); 
										flag = TRUE;
									}
								}
								// 無かったら
								if( flag == FALSE ){
									// レベルアップ表示
									//StockFontBuffer( x + 148, y, FONT_PRIO_FRONT, color,"LvUp!", 0 ); 
									// 経験値表示
									sprintf( moji,"Exp %+4d", battleResultMsg.resChr[ j ].exp );
									StockFontBuffer( x + 148 + 58, y, FONT_PRIO_FRONT, 0, "Exp    +0", 0 ); 
								}
								y += 20;
							}
						}
						flag = FALSE;
						y = pActMenuWnd->y + 134;
						// 	アイテム: 表示
						StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, "アイテム：", 0 ); 
						// ゲットアイテムの表示
						for( i = 0 ; i < 3 ; i++ ){
							// アイテムがあったら
							if( battleResultMsg.item[ i ][ 0 ] != NULL ){
								StockFontBuffer( x + 85, y, FONT_PRIO_FRONT, 0, battleResultMsg.item[ i ], 0 ); 
								y += 22;
								flag = TRUE;
							}
						}
						// アイテム無い時
						if( flag == FALSE ){
							StockFontBuffer( x + 85, y, FONT_PRIO_FRONT, 0, "取得できませんでした。", 0 ); 
						}
						// 文字表示バッファに溜める
						resultWndFontNo[ 0 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, pActMenuWnd->y + 216, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
					}
				}
			}
			break;
			
		case 2:	// ＤＰウィンドウのとき
		
			// ウィンドウがない時
			if( pActMenuWnd == NULL ){
				int flag = 0;
				// ウィンドウ表示タスク作成
				pActMenuWnd = MakeWindowDisp( 320 - 96, 240 - 72, 3, 3, CG_WND_TITLE_RESULT, 2 );
				// 当たり判定初期化
				for( i = 0 ; i < MENU_BTL_RESULT_0 ; i++ ) resultWndFontNo[ i ] = -2;
				// ウィンドウ開く音
				play_se( 202, 320, 240 );
			}else{
				// ウィンドウが出来あがっていたら
				if( pActMenuWnd->hp > 0 ){
					// 左クリックした時
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						// 閉じる時
						if( HitDispNo == resultWndFontNo[ 0 ] ){
							// ウィンドウ抹殺
							DeathAction( pActMenuWnd );
							pActMenuWnd = NULL;
							// システムウィンドウ閉じる
							BattleResultWndFlag = FALSE;
							// マップウィンドウ出さない時
							if( MapWmdFlagBak != TRUE ){
								// ウィンドウ閉じる音
								play_se( 203, 320, 240 );
							}
						}
					}
					
					// ウィンドウある時
					if( pActMenuWnd != NULL ){
						int color = FONT_PAL_WHITE;
						//int flag, j;
						// 座標設定
						x = pActMenuWnd->x + 17;
						y = pActMenuWnd->y + 48;
						
						//color = FONT_PAL_GRAY;	// デフォルト色
						
						// ＤＰ表示
						sprintf( moji," Ｄ　Ｐ　%+8d", battleResultMsg.resChr[ 0 ].exp );
						//sprintf( moji," Ｄ　Ｐ　%+8d", -9999999 );
						//sprintf( moji," Ｄ　Ｐ　%+8d", -100 );
						StockFontBuffer( x, y, FONT_PRIO_FRONT, color, moji, 0 ); 
						y += 28;
						// ＤＰ表示
						sprintf( moji," 合　計　%8d", battleResultMsg.resChr[ 1 ].exp );
						//sprintf( moji," 合　計　%8d", 99999999 );
						//sprintf( moji," 合　計　%8d", 1100 );
						StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, moji, 0 ); 
						
						// 文字表示バッファに溜める
						resultWndFontNo[ 0 ] = StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx, pActMenuWnd->y + 120, DISP_PRIO_IME3, CG_CLOSE_BTN, 2 );
					}
				}
			}
			break;
		}
	}
}

