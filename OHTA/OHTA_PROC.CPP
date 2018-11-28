/************************/
/*	ohta_proc.cpp		*/
/************************/
#include "system.h"
#include "ohta.h"
#include "ime_sa.h"
#include "map.h"
#include "menu.h"

/* 太田プロセス処理 ********************************************************************/
void OhtaTestProc( void )
{
	/* サブプロセス番号で分岐 */
	switch( SubProcNo ){
	
		case 0:
		
			/* 自機 タスク作成 */
			MakeJiki();
			SubProcNo++;
			/* チャット関連初期化 */
			InitChat();
			// 入力フォーカス取得
			GetKeyInputFocus( &MyChatBuffer );
			
			break;
		
		case 1:
			// キーボードカーソル点滅処理
			FlashKeyboardCursor();
			
			// マウスレベルがメニュー以下の時
			if( mouse.level < DISP_PRIO_MENU && TaskBarFlag == FALSE ){
				drawGrid();		// グリッドカーソル表示
				moveProc();		// 移動処理
			}
			
			/* アクション走らせる */
			RunAction();
			// タスク表示データをバッファに溜める
			StockTaskDispBuffer();
			
			drawMap();	// マップ表示

			/* チャット処理 */
			ChatProc();
			// チャットバッファーをフォントバッファに溜める
			ChatBufferToFontBuffer(); 
			// メニュー処理
			MenuProc();
			// ＩＭＥ関連処理
			ImeProc();		
			// パレット処理
			PaletteProc();
			
			break;
	}
}


