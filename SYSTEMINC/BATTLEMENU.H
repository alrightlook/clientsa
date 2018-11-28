/************************/
/*	battleMenu.h		*/
/************************/
#ifndef _BATTLE_MENU_H_
#define _BATTLE_MENU_H_

// BattleBpFlagのビット情報
#define BATTLE_BP_JOIN				( 1 << 0 )		// 参戦
#define BATTLE_BP_PLAYER_MENU_NON	( 1 << 1 )		// プレイヤーメニューなし
#define BATTLE_BP_BOOMERANG			( 1 << 2 )		// ブーメラン
#define BATTLE_BP_PET_MENU_NON		( 1 << 3 )		// ペットメニューなし
#define BATTLE_BP_ENEMY_SURPRISAL	( 1 << 4 )		// 敵が不意打ついた時（不意つかれた時）
#define BATTLE_BP_PLAYER_SURPRISAL	( 1 << 5 )		// プレイヤーが不意打をついた時

// バトルコマンドサイズ
#define BATTLE_BUF_SIZE	4
#define BATTLE_COMMAND_SIZE			4096
// バトルコマンド文字列
extern char BattleCmd[ BATTLE_COMMAND_SIZE ];
extern char BattleCmdBak[ BATTLE_BUF_SIZE ][ BATTLE_COMMAND_SIZE ];
// バトルコマンド文字列ポインター
extern int BattleCmdReadPointer;
extern int BattleCmdWritePointer;
// バトルコマンド文字列バックアップ
//extern char BattleCmdBak[];
// バトル状態文字列
extern char BattleStatus[ BATTLE_COMMAND_SIZE ];
extern char BattleStatusBak[ BATTLE_BUF_SIZE ][ BATTLE_COMMAND_SIZE ];
// バトル状態文字列ポインター
extern int BattleStatusReadPointer;
extern int BattleStatusWritePointer;
// バトル状態文字列バックアップ
//extern char BattleStatusBak[];
// バトル状態文字列バックアップ
//extern char BattleStatusBak2[];
// バトルメニューフラグ
extern int battleMenuFlag2;
// 自分の配列番号
extern int BattleMyNo;
// 戦闘時の自分のＭＰ
extern int BattleMyMp;
// 戦闘強制終了フラグ
extern int BattleEscFlag;
// ペットスタメンカウンター
extern int BattlePetStMenCnt;
// 参戦ペット番号バックアップ
extern int battlePetNoBak;
// 参戦ペット番号バックアップ２
extern int battlePetNoBak2;

// ＢＰフラグ
extern int BattleBpFlag;
// コマンド入力済みフラグ
extern int BattleAnimFlag;
// ターン受信フラグ
extern BOOL BattleTurnReceiveFlag;
// 現在のクライアントターン番号
extern int BattleCliTurnNo;
// 現在のサーバーターン番号
extern int BattleSvTurnNo;

// 戦闘結果ウィンドウフラグ
extern BOOL BattleResultWndFlag;

// バトルメニュー処理 *********************************************************/
void BattleMenuProc( void );
// ボタンフラグ全初期化　*****************************************************/
void ClearBattleButton( void );
// バトルメニュー初期化 ********************************************************/
void InitBattleMenu( void );
// コマンド入力済みフラグのチェック ********************************************/
void CheckBattleAnimFlag( void );
// 一人用か二人用かチェック ********************************************/
int CheckBattle1P2P( void );
// 耐久力メーター表示 ***********************************************************/
void HpMeterDisp( int no );

#endif


