/************************/
/*	menu.h				*/
/************************/
#ifndef _MENU_H_
#define _MENU_H_

#include "pc.h"
#include "chat.h"


#define MAIL_STR_LEN 140		// メール文字の長さ
#define MAIL_DATE_STR_LEN 20	// 日付の文字列の長さ
#define MAIL_MAX_HISTORY 20		// 履歴の件数
#define MAX_CHAT_REGISTY_STR		8		// チャットの文字登録の数

// アイテムバッファー構造体
typedef struct{
	int 	x, y;			// 表示座標
	int 	defX, defY;		// デフォルト座標
	int 	bmpNo;			// 画像番号
	int 	dispPrio;		// 表示優先
	BOOL	dragFlag;		// ドラッグフラグ
	BOOL	mixFlag;		// 合成フラグ
}ITEM_BUFFER;

// メールの履歴構造体
typedef struct{
	char 	str[ MAIL_MAX_HISTORY ][ MAIL_STR_LEN + 1 ];			// メールの履歴文字列
	char 	dateStr[ MAIL_MAX_HISTORY ][ MAIL_DATE_STR_LEN + 1 ];	// 日付の文字列
	int 	noReadFlag[ MAIL_MAX_HISTORY ];							// 未読フラグ（受信ペットのグラフィック番号）
	int 	petLevel[ MAIL_MAX_HISTORY ];							// 受信ペットのグラフィック番号
	char 	petName[ MAIL_MAX_HISTORY ][ PET_NAME_LEN + 1 ];		// ペットの名前
	int 	itemGraNo[ MAIL_MAX_HISTORY ];							// 届いたアイテムのグラフィック番号
	int 	newHistoryNo;											// 最新履歴の番号
}MAIL_HISTORY;

// ウィンドウ表示アクション予備構造体
typedef struct{
	int sizeX,sizeY;	// サイズ
	int titleNo;		// タイトル番号
	int titleX, titleY;	// タイトルの座標
	int mx, my;			// 中心座標
	int nowX, nowY;		// 拡大中のサイズ
	int cnt;			// 中心座標
	int wndType;		// ウィンドウの種類
	int hitFlag;		// 当たり判定有り無し
	int yesDispNo;		// はいの当たり判定番号
	int noDispNo;		// いいえの当たり判定番号
	int boxDispPrio;	// 拡大ボックスの表示優先順位
	int yesNoResult;	// YesNoの結果（０：いいえ　１：はい）
}WINDOW_DISP;

#define MAX_PET_KIND 	124		// ペットの種類
#define MAX_PET_TBL 	181		// ペットテーブルのサイズ
#define MAX_PET_PAGE_SIZE 	8	// １ページに表示出きるペットの数。

// アルバム構造体
typedef struct{
	char name[ PET_NAME_LEN + 1 ];	// ペットの名前
	char freeName[ PET_NAME_LEN + 1 ];	// ペットのあだ名
	int faceGraNo;					// ペットの顔グラフィック番号
	int level;						// レベル
	int maxHp;						// 最大ＨＰ
	int str;						// 攻撃力
	int quick;						// 素早さ
	int def;						// 防御力
	int earth, water, fire, wind;	// 地水火風
	int flag;						// 登録フラグ（０：見たことない １：見た ２：写真撮った（新規） ３：登録済み ）
}PET_ALBUM;

// アルバム用ペットデータテーブル構造体
typedef struct{
	int albumNo;					// アルバムのとうろく番号
	char name[ PET_NAME_LEN + 1 ];	// ペットの名前
	int faceGraNo;					// ペットの顔グラフィック番号
}PET_ALBUM_TBL;


// タスクバーフラグ
extern BOOL TaskBarFlag;
// トグルフラグ
extern unsigned int MenuToggleFlag;
// アイテムバッファー
extern ITEM_BUFFER ItemBuffer[];
// ステータスアップポイント
extern int StatusUpPoint;
// 参戦ペット変更プロトコル送信フラグ
extern BOOL BattlePetReceiveFlag;		
// バトル受信中のペット番号
extern int BattlePetReceivePetNo; 
// メールの履歴文字列
extern MAIL_HISTORY MailHistory[];	
// チャットの文字登録の入力バッファー
extern STR_BUFFER chatRegistryStr[];
// メールの入力文字列
extern STR_BUFFER MailStr;		
// ウィンドウアクションポインタ
extern ACTION *pActMenuWnd3;
// チャットの文字登録の入力バッファー
extern STR_BUFFER chatRegistryStr[];
// プレイヤーの称号用文字列
extern STR_BUFFER shougouChange;
// ペットの名前変更用文字列
extern STR_BUFFER petNameChange;
// 詳細ウィンドウに表示されているペットの番号
extern int  petStatusNo;

extern int mailHistoryWndPageNo;	// 履歴ウィンドウのページ番号
extern int mailHistoryWndSelectNo;	// 選択されている名刺番号	
//extern int mailReceivePetGraNo;		// ペットメール受信時のペットのグラフィック番号

// 戦闘後に自動でマップウィンド表示するフラグ
extern int MapWmdFlagBak;

// 戦闘結果ウィンドウが消えるまでのフレーム数
#define RESULT_WND_CLOSE_TIME 	300	// ５秒
// 戦闘結果ウィンドウが消えるまでのタイマー
extern int ResultWndTimer;

// 合成出来たフラグ
extern BOOL ItemMixRecvFlag;

// メニュー関連の初期化 *******************************************************/
void InitMenu( void );

// メニュー関連の初期化２ *******************************************************/
void InitMenu2( void );

// メニュー出てるかチェック *******************************************************/
BOOL CheckMenuFlag( void );

// メニュー処理 ***************************************************************/
void MenuProc( void );

// ウィンドウ表示タスク作成 ***************************************************/
ACTION *MakeWindowDisp( int x, int y, int sizeX, int sizeY, int titleNo, int wndType );

// 当たり判定ボックス作成 ******************************************************/
BOOL MakeHitBox( int x1, int y1, int x2, int y2, int dispPrio );

// メールデータの保存 ****************************************************************/
BOOL SaveMailHistory( int no );

// メールデータの読み込み ****************************************************************/
BOOL LoadMailHistory( void );

// メール未読チェック ****************************************************************/
BOOL CheckMailNoReadFlag( void );

// ログイン時の初期化（太田専用） **************************************************/
void InitOhtaParam( void );

// 手紙アニメーション抹殺 **********************************************************/
void DeathLetterAction( void ); 

// チャットの登録文字の保存 ****************************************************************/
BOOL SaveChatRegistyStr( void );

// チャットの登録文字の読み込み ****************************************************************/
BOOL LoadChatRegistyStr( void );

// 文字列のセンターリング *****************************************************/
void CenteringStr( char *inStr, char *outStr, int max );

// 新規ペットのチェック ****************************************************************/
void CheckNewPet( int sprNo );

// 戦闘時の新規ペットチェック **********************************************************/
void CheckBattleNewPet( void );

// アルバムの読み込み ****************************************************************/
BOOL LoadAlbum( void );

// アルバムの保存 ****************************************************************/
BOOL SaveAlbum( int no );

#endif


