// gamemain.cpp ヘッダファイル

#ifndef _GAMEMAIN_H_
#define _GAMEMAIN_H_

// ゲ�`ムの彜�B
enum{
	GAME_LOGIN,					// ログイン
	GAME_FIELD,					// フィ�`ルドマップ
	GAME_FIELD_TO_ENCOUNT,		// フィ�`ルドマップからエンカウント處竃嶄
	GAME_ENCOUNT_TO_BATTLE,		// エンカウント處竃から�蜉L
	GAME_BATTLE				// エンカウント處竃から�蜉L
};


extern int	GameState;	// ゲ�`ムの彜�B
extern double NowTime;	// �F壓の�r�g����

//---------------------------------------------------------------------------//
// �v方プロトタイプ傚冱                                                      //
//---------------------------------------------------------------------------//
BOOL GameMain( void );				// ゲ�`ム�I尖メイン�v方

// ゲ�`ム�_兵�I尖
BOOL InitGame( void );

// ゲ�`ム�K阻�I尖
void EndGame( void );

#ifdef _DEBUG		
// 秤�鷄輅�
void InfoDisp( void );
#endif

// フレ�`ムレ�`トの��麻��翌繁が恬撹��
void DisplayFrameRate( void );

#endif

