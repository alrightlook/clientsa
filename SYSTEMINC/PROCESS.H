/************************/
/*	process.h			*/
/************************/
#ifndef _PROCESS_H_
#define _PROCESS_H_

/* プロセスの種類 */
enum{
	PROC_INIT,
	PROC_ID_PASSWORD,				// ＩＤ、パスワード入力処理
	PROC_TITLE_MENU,				// タイトル画面メニュー表示
	PROC_CHAR_SELECT,				// キャラ選択
	PROC_CHAR_MAKE,					// キャラ作成
	PROC_CHAR_LOGIN_START,			// キャラログイン開始
	PROC_CHAR_LOGIN,				// キャラログイン中
	PROC_CHAR_LOGOUT,				// キャラログアウト
	PROC_OPENNING,
	PROC_GAME,						// ゲームプロセス
	PROC_BATTLE,					// バトルプロセス
	PROC_DISCONNECT_SERVER,			// ネットワークが切断された時の処理

#ifdef _DEBUG		

	PROC_TAKE_TEST,					// 竹内テスト
	PROC_OHTA_TEST,					// 太田テスト
	PROC_DWAF_TEST,					// 松村テスト
	PROC_SPR_VIEW,					// スプライト確認プロセス
	PROC_ANIM_VIEW,					// アニメーション確認プロセス
	PROC_SE_TEST,					// ＳＥ確認プロセス
	
#endif
	PROC_ENDING
	
};

/* プロセス番号 */
extern UINT ProcNo;
/* サブプロセス番号 */
extern UINT SubProcNo;

extern int palNo;
extern int oldPalNo;


/* プロセス処理 */
void Process( void );

/* プロセス変更処理 */
void ChangeProc( int procNo );
/* プロセス変更処理 */
void ChangeProc( int procNo, int subProcNo );

/* ゲームプロセス処理 */
void GameProc( void );

// ワープ演出用
void warpEffectProc( void );

void paletteProc( void );

#ifdef _DEBUG		

/* 竹内さんテストプロセス */
void TakeTestProc( void );

/* 太田テストプロセス */
void OhtaTestProc( void );

#endif

#endif

