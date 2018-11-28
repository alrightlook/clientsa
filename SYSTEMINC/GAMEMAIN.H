// gamemain.cpp ヘッダファイル

#ifndef _GAMEMAIN_H_
#define _GAMEMAIN_H_

// ゲームの状態
enum{
	GAME_LOGIN,					// ログイン
	GAME_FIELD,					// フィールドマップ
	GAME_FIELD_TO_ENCOUNT,		// フィールドマップからエンカウント演出中
	GAME_ENCOUNT_TO_BATTLE,		// エンカウント演出から戦闘
	GAME_BATTLE				// エンカウント演出から戦闘
};


extern int	GameState;	// ゲームの状態
extern double NowTime;	// 現在の時間記憶

//---------------------------------------------------------------------------//
// 関数プロトタイプ宣言                                                      //
//---------------------------------------------------------------------------//
BOOL GameMain( void );				// ゲーム処理メイン関数

// ゲーム開始処理
BOOL InitGame( void );

// ゲーム終了処理
void EndGame( void );

#ifdef _DEBUG		
// 情報表示
void InfoDisp( void );
#endif

// フレームレートの計算（外人が作成）
void DisplayFrameRate( void );

#endif

