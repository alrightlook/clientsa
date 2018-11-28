/************************/
/*	ime.h				*/
/************************/
#ifndef _IME_H_
#define _IME_H_

// ＩＭＥのハンドル  IME的方向盘
//Handle to input context.
extern HIMC hImc;

// ＩＭＥバッファー 缓冲器  我修改 _asm
extern char ImeBuffer[];
//extern char *ImeBuffer;

// ＩＭＥ文節バッファー   短语缓冲器
extern char ImeBlock[];
// ＩＭＥバッファーバックアップ  备份缓冲器
extern char ImeBufferBak2;

// ＩＭＥ入力モード取得用  输入方式取得用
extern DWORD conversion;
extern DWORD sentence;

// ＩＭＥ関連の初期化 *********************************************************/
void InitIme( void );

// ＩＭＥの終了 ***************************************************************/
void EndIme( void );

//*****************************************************************************/
// ＩＭＥのＯＮ、ＯＦＦ状態の確認 
// 戻り値　	ＯＮ　：TRUE
// 			ＯＦＦ：FALSE
//*****************************************************************************/
BOOL ImeOpenState( void );

//*****************************************************************************/
// ＷＩＮＤＯＷＳのＩＭＥバッファーをクリアする 
//*****************************************************************************/
void ClrImeCompositionString( void );

// WM_IME_COMPOSITION メッセージの処理 ****************************************/
void ImeComposition( LPARAM lParam );

// ＩＭＥ関連処理 *************************************************************/
void ImeProc( void );

/** ＩＭＥ関連メッセージ処理 **************************************************/
//void ImeMsgProc( UINT Message, WPARAM wParam, LPARAM lParam );

#endif

