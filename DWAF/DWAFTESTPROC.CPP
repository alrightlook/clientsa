// Dwaf Test Process

#include "system.h"
#include "dwafTestProc.h"
#include "ime_sa.h"
#include "menu.h"
#include "pc.h"
#include "character.h"

void dwafTestProc( void )
{
	/* サブプロセス番号で分岐  子进程/号码分歧*/*/
	switch( SubProcNo ){
	
		case 0:

			InitChat();
			// 入力フォーカス取得  输入聚焦取得
			GetKeyInputFocus( &MyChatBuffer );
			SubProcNo++;
			break;
		
		case 1:

			drawMap();		// マップ表示  地图显示
			
			/* チャット処理   聊天处理*/
			ChatProc();
			// チャットバッファーをフォントバッファに溜める  聊天缓冲字体缓冲蓄
			ChatBufferToFontBuffer(); 
			// メニュー処理  菜单处理
			MenuProc();
			// ＩＭＥ関連処理
			ImeProc();		
			
			break;
	}
}

