/************************/
/*	testView.cpp		*/
/************************/
#include "system.h"
#include "loadsprbin.h"
#include "anim_tbl.h"
#include "../oft/vg410.h"
#include "t_music.h"

#ifdef _DEBUG		

// ヒットマーク処理（会心）
extern void hit_mark_critical( ACTION *a0 );

// 表示バッファに溜める（ＳｐｒＶｉｅｗ用）
void StockDispBufferSprView( int x, int y, UCHAR dispPrio, int bmpNo )
{	
	// 今回保存する場所までアドレスを進ませる
	DISP_SORT 	*pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt;
	DISP_INFO 	*pDispInfo = DispBuffer.DispInfo + DispBuffer.DispCnt;
	
	// カウントオーバーの時
	if( DispBuffer.DispCnt >= DISP_BUFFER_SIZE ) return;
	
	// 表示優先ソート用構造体
	pDispSort->dispPrio = dispPrio;
	pDispSort->no = DispBuffer.DispCnt;
	// 表示情報構造体（ ソートしない内容 ）
	pDispInfo->x = x;
	pDispInfo->y = y;
	pDispInfo->bmpNo = bmpNo;
	pDispInfo->hitFlag = 2;

	// 表示カウンタープラス
	DispBuffer.DispCnt++;
}

/* スプライト確認プロセス ********************************************************************/
void SprViewProc( void )
{
	char szMoji[ 256 ];
	int bmpX;
	int bmpY;
	static int bmpNo = 0;
	int bmpNoBak;
	static int no = 0; // 増分番号
	static BOOL fontFlag = TRUE; // フォント表示フラグ
	// 増分テーブル
	int zoubunTbl[] ={   1,  5,  25,  100, 	500,  1000,
						-1, -5, -25, -100, -500, -1000 };
	static int palNo = 0; // パレット番号
	static int time = 600; // パレット番号
	
	/* サブプロセス番号で分岐 */
	switch( SubProcNo ){
	
		case 0:
			// バックバッファー描画方法で分岐
			//BackBufferDrawType = DRAW_BACK_NON
			// キーボードで操作する時
			// 進ませる
			if( joy_auto[ 0 ] & JOY_RIGHT ) bmpNo += zoubunTbl[ no ];
			// 戻る
			if( joy_auto[ 0 ] & JOY_LEFT ) bmpNo -= zoubunTbl[ no ];
			// 増分プラス
			if( joy_trg[ 0 ] & JOY_UP ){ 
				no++;
				if( no >= 6 ) no = 0; // リミットチェック
			}
			// 増分マイナス
			if( joy_trg[ 0 ] & JOY_DOWN ){
				no--;
				if( no <= -1 ) no = 5; // リミットチェック
			}
#if 0
			// マウスで操作する時
			// 左クリックで進む
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				bmpNo += zoubunTbl[ no ];
				// クリック音
				play_se( 201, 320, 240 );
			}
			// 真中クリックで早送り
			if( GetAsyncKeyState( VK_MBUTTON ) ){
				bmpNo += zoubunTbl[ no ];
			}
			
			// 右クリックで増分変更
			if( mouse.onceState & MOUSE_RIGHT_CRICK ){
			//if( mouse.state & MOUSE_RIGHT_CRICK ){
				no++;
				if( no == 6 ) no = 0; // リミットチェック
				// クリック音
				play_se( 201, 320, 240 );
			}
#endif			
			// リミットチェック
			if( bmpNo < 0 ) bmpNo = 0;
			if( bmpNo >= MAX_GRAPHICS - 25 ) bmpNo = MAX_GRAPHICS -25;
			
			// パレット変更
			if( joy_trg[ 0 ] & JOY_A ){	/* プラス */
				palNo--;
				if( palNo < 0 ) palNo = MAX_PAL - 1;
				// パレットチェンジ
				PaletteChange( palNo, 10 );
				// ウィンドウ開く音
				play_se( 202, 320, 240 );
			}
			if( joy_trg[ 0 ] & JOY_B ){	/* マイナス */
				palNo++;
				if( palNo >= MAX_PAL ) palNo = 0;
				// パレットチェンジ
				PaletteChange( palNo, 10 );
				// ウィンドウ開く音
				play_se( 202, 320, 240 );
			}
			// フォントフラグ変更
			if( joy_trg[ 0 ] & JOY_DEL ){	/* プラス */
				if( fontFlag ) fontFlag = 0;
				else fontFlag = 1;
				// アイテム使う音
				play_se( 212, 320, 240 );
			}
			
			// ＥＳＣで終了
			if( joy_trg[ 0 ] & JOY_ESC ){	/* マイナス */
				// ＷＩＮＤＯＷＳに WM_CLOSE メッセージを送らせる
				PostMessage( hWnd, WM_CLOSE, 0, 0L );
			}
			
			// ＢＭＰ番号バックアップ
			bmpNoBak = bmpNo;
			
			// 一画面分ループ（２５枚分）
			for( bmpY = 0 ; bmpY < 480 ; bmpY += 96 ){
				for( bmpX = 0 ; bmpX < 640 ; bmpX += 128 ){
					// フォントフラグＯＮの時
					if( fontFlag == TRUE ){
						// ＢＭＰ番号
						wsprintf( szMoji,"%7d", bmpNoBak );
						StockFontBuffer( bmpX, bmpY, FONT_PRIO_BACK, 0, szMoji, 0 );
					}
					// ＢＭＰ表示バッファにためる
					StockDispBufferSprView( bmpX, bmpY, 0, bmpNoBak++ );
				}
			}
			// フォントフラグＯＮの時
			if( fontFlag == TRUE ){
				// 増分表示
				wsprintf( szMoji, "PAL:%2d　増分:%6d", palNo, zoubunTbl[ no ] );
				StockFontBuffer( 640 - 16 * 12, 462, FONT_PRIO_FRONT, 0, szMoji, 0 );
					
				// 終了表示
				StockFontBuffer( 4, 462, FONT_PRIO_FRONT, 0, "ESC:終了　X or Y:PAL変更　DEL:文字 ON/OFF", 0 );
			}
			/* アクション走らせる */
			RunAction();
			// タスク表示データをバッファに溜める
			StockTaskDispBuffer();
			
			// 背景描画（紫色）
			StockBoxDispBuffer( 0, 0, 640, 480, DISP_PRIO_BG, SYSTEM_PAL_PURPLE , 1 );
			
			// 現在の時間を記憶
			NowTime = GetTickCount();

			break;
	}
}

/* アニメーションアクション処理 ***********************************************/
void AnimSpr( ACTION *pAct )
{
	char szMoji[ 256 ];
	int x = 32, y = 32;
	int c = 0,d0;
	ACTION *a1;
	static palNo = 0;
	static anim_spd = 0;
	static slowFlag = FALSE;
	static hitNo = 0;
	
	/* 状態で分岐 */
	switch( pAct->state ){
		/* 通常時 */
		case ACT_STATE_ACT:
			// スローでない時
			if( slowFlag == FALSE ){
				// スプライト番号変更（±１）
				if( joy_auto[ 0 ] & JOY_RIGHT ){	/* プラス */
					while( 1 ){
						pAct->anim_chr_no++;
						if( SpriteData[pAct->anim_chr_no - SPRSTART].ptAnimlist !=NULL ) break;
						if( pAct->anim_chr_no > 1000000 ){
							 pAct->anim_chr_no = 1000000;
							 break;
						}
					}
				}
				if( joy_auto[ 0 ] & JOY_LEFT ){		/* マイナス */
					while( 1 ){
						pAct->anim_chr_no--;
						if( SpriteData[pAct->anim_chr_no - SPRSTART].ptAnimlist !=NULL ) break;
						if( pAct->anim_chr_no <  SPRSTART ){ 
							pAct->anim_chr_no =  SPRSTART;
							break;
						}
					}
				}
				// スプライト番号変更（±１０００）
				if( joy_trg[ 0 ] & JOY_INS ){	/* プラス */
					pAct->anim_chr_no += 1000;
					while( 1 ){
						pAct->anim_chr_no++;
						if( SpriteData[pAct->anim_chr_no - SPRSTART].ptAnimlist !=NULL ) break;
						if( pAct->anim_chr_no > 1000000 ){ 
							pAct->anim_chr_no = 1000000;
							break;
						}
					}
				}
				if( joy_trg[ 0 ] & JOY_DEL ){	/* マイナス */
					pAct->anim_chr_no -= 1000;
					while( 1 ){
						pAct->anim_chr_no--;
						if( SpriteData[pAct->anim_chr_no - SPRSTART].ptAnimlist !=NULL ) break;
						if( pAct->anim_chr_no <  SPRSTART ){ 
							pAct->anim_chr_no =  SPRSTART;
							break;
						}
					}
				}
				// アニメーション有り無しチェック
				//while( 1 ){
				//if( SpriteData[ pAct->anim_chr_no ].ptAnimlist == NULL )
				//{
					
				
				// 方向変換
				if( joy_auto[ 0 ] & JOY_UP ){		/* プラス */
					pAct->anim_ang++;
					if( pAct->anim_ang >= 8 ) pAct->anim_ang = 0;
				}
				if( joy_auto[ 0 ] & JOY_DOWN ){		/* マイナス */
					pAct->anim_ang--;
					if( pAct->anim_ang < 0 ) pAct->anim_ang =  7;
				}
				
				// アニメーション番号表示
				if( joy_trg[ 0 ] & JOY_HOME ){		/* プラス */
					pAct->anim_no++;
					if( pAct->anim_no > 12 ) pAct->anim_no = 12;
				}
				if( joy_trg[ 0 ] & JOY_END ){	/* マイナス */
					pAct->anim_no--;
					if( pAct->anim_no < 0 ) pAct->anim_no =  0;
				}
				
				// スピード変更
				if( joy_trg[ 0 ] & JOY_A ){	/* プラス */
					anim_spd--;
					if( anim_spd < 0 ) anim_spd = 0;
				}
				if( joy_trg[ 0 ] & JOY_B ){	/* マイナス */
					anim_spd++;
					if( anim_spd >= 255 ) anim_spd = 255;
				}
			}
			// ＥＳＣで終了
			if( joy_trg[ 0 ] & JOY_ESC ){	/* マイナス */
				// ＷＩＮＤＯＷＳに WM_CLOSE メッセージを送らせる
				PostMessage( hWnd, WM_CLOSE, 0, 0L );
			}
			
			
			// コマ送りフラグＯＮ
			if( joy_trg[ 1 ] & JOY_F12 ){ 
				pAct->anim_frame_cnt = 0;
				slowFlag = TRUE;
			}
			// コマ戻しフラグＯＮ
			if( joy_trg[ 1 ] & JOY_F11 ){ 
				pAct->anim_cnt -= 2;
				// リミットチェック
				if( pAct->anim_cnt <= 0 ) pAct->anim_cnt = 0;
				pAct->anim_frame_cnt = 0;
				slowFlag = TRUE;
			}
			// コマ送りフラグＯＦＦ
			if( joy_trg[ 1 ] & JOY_F10 ) slowFlag = FALSE;
			
			if( slowFlag == FALSE || joy_trg[ 1 ] & JOY_F11 || joy_trg[ 1 ] & JOY_F12 ){
				// アニメーション
				pattern( pAct, anim_spd, ANM_LOOP );
			}
			
			// 当たり判定の時
			if( pAct->anim_hit >= 10000 ){
				hitNo = pAct->anim_hit; // 当たり判定番号記憶
				pAct->anim_hit = 0; // 初期化
				// ヒットマーク作成
				for( d0=5 ;d0>0 ;d0--){
					//ヒットマークセット
					a1 = GetAction( PRIO_JIKI, sizeof( ATR_EQU ) );
					if( a1 == NULL ) return;
					/* 表示優先度 */
					ATR_DISP_PRIO(a1) = DISP_PRIO_CHAR + 1;
					/* スプライト番号 */
					ATR_CHR_NO(a1) = CG_HIT_MARK_22;
					ATR_H_POS(a1) = 320;
					ATR_V_POS(a1) = 240;
					ATR_SPD(a1) = Rnd( 4, 7 );
					ATR_CRS(a1) = d0 * 6;
					ATR_FIRST_FLG(a1) = ATR_STIMER(a1) = 32;		//表示時間セット
					ATR_KAISHIN(a1) = 1;
					ATR_NAME(a1) = hit_mark_critical;
					ATR_CHR_NO(a1) = CG_HIT_MARK_01;
				}
			}
			// スプライト番号表示
			wsprintf( szMoji, "スプライト番号 =  %8d ： Left or Right ( ±1 )", pAct->anim_chr_no );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			StockFontBuffer( x + 236, y, FONT_PRIO_FRONT, 0, "： DEL  or INS ( ±1000 )", 0 ); y += 20;
			// 方向表示
			wsprintf( szMoji, "方　　　　　向 =  %8d ： Down or UP", pAct->anim_ang );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// アニメーション番号表示
			wsprintf( szMoji, "行　動　番　号 =  %8d ： END  or HOME", pAct->anim_no );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// 増分表示
			wsprintf( szMoji, "ス　ピ　ー　ド =  %8d ： Z    or X", anim_spd );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			
			// 効果音番号
			wsprintf( szMoji, "効果音　番　号 =  %8d", t_music_se_no );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// 効果音ある時
			if( t_music_se_no != -1 ){
				// 効果音の音量
				if( joy_trg[ 1 ] & JOY_F8 ){		/* プラス */
					tone_tbl[ t_music_se_no ].voice_volume++;
					if( tone_tbl[ t_music_se_no ].voice_volume >= 128 ) tone_tbl[ t_music_se_no ].voice_volume = 127;
				}
				if( joy_trg[ 1 ] & JOY_F7 ){	/* マイナス */
					tone_tbl[ t_music_se_no ].voice_volume--;
					if( tone_tbl[ t_music_se_no ].voice_volume <= -1 ) tone_tbl[ t_music_se_no ].voice_volume = 0;
				}
				
				// 効果音のピッチ
				if( joy_trg[ 1 ] & JOY_F6 ){	/* プラス */
					tone_tbl[ t_music_se_no ].voice_note++;
					if( tone_tbl[ t_music_se_no ].voice_note + tone_tbl[ t_music_se_no ].voice_rate >= 63 ) tone_tbl[ t_music_se_no ].voice_note = 62 - tone_tbl[ t_music_se_no ].voice_rate;
				}
				if( joy_trg[ 1 ] & JOY_F5 ){	/* マイナス */
					tone_tbl[ t_music_se_no ].voice_note--;
					if( tone_tbl[ t_music_se_no ].voice_note + tone_tbl[ t_music_se_no ].voice_rate <= 0 ) tone_tbl[ t_music_se_no ].voice_note = -tone_tbl[ t_music_se_no ].voice_rate + 1;
				}
				// 効果音音量
				wsprintf( szMoji, "効果音　音　量 =  %8d ： F7 or F8", tone_tbl[ t_music_se_no ].voice_volume );
				StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
				// 効果音ピッチ
				wsprintf( szMoji, "効果音　ピッチ =  %8d ： F5 or F6", tone_tbl[ t_music_se_no ].voice_note );
				StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			}else{
				// 効果音音量
				wsprintf( szMoji, "効果音　音　量 =  %8d ： F7 or F8", 0 );
				StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
				// 効果音ピッチ
				wsprintf( szMoji, "効果音　ピッチ =  %8d ： F5 or F6", 0 );
				StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			}
			
			// ＢＭＰ番号
			//wsprintf( szMoji, "ＢＭＰ　番　号 =  %8d", pAct->bmpNo );
			//StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// サイズ表示
			//wsprintf( szMoji, "ＢＭＰ　サイズ = %4d,%4d", SpriteInfo[ pAct->bmpNo ].width, SpriteInfo[ pAct->bmpNo ].height );
			//StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
						
			// 当たり判定番号
			wsprintf( szMoji, "当たり判定番号 =  %8d", hitNo );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			
			// コマ送りの説明
			wsprintf( szMoji, "F12：コマ送り　F11：コマ戻し　F10：ＯＦＦ" );
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			
			// 終了表示
			StockFontBuffer( 530, 460, FONT_PRIO_FRONT, 0, "終了：ESC", 0 );
			
			break;
		
		/* 死亡時 */	
		case ACT_STATE_DEAD:
		
			DeathAction( pAct );
			
			break;
	}
}

/* アニメーションアクション作成 ***********************************************/
ACTION *MakeAnimSpr( void )
{
	ACTION *pAct;
	
	/* アクションリストに登録 */
	pAct = GetAction( PRIO_JIKI, 0 );
	if( pAct == NULL ) return NULL;
	
	/* 実行関数 */
	pAct->func = AnimSpr;
	// anim_tbl.h の番号
	pAct->anim_chr_no = SPRSTART; 
	// 動作番号
	pAct->anim_no = 0;//ANIM_ATTACK;
	// アニメーション向き( ０～７ )( 下が０で右回り )
	pAct->anim_ang = 0;//Rnd( 0, 7 );
	// ボックス表示
	pAct->atr |= ACT_ATR_HIT_BOX;
	/* 表示優先度 */
	pAct->dispPrio = DISP_PRIO_CHAR;
	/* 初期位置 */
	pAct->x = 320;
	pAct->y = 360;
	
	// アニメーション
	pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
	
	return pAct;
}

/* アニメーション確認プロセス ********************************************************************/
void AnimViewProc( void )
{
	/* サブプロセス番号で分岐 */
	switch( SubProcNo ){
	
		case 0:
			
			// アニメーションアクション作成
			MakeAnimSpr();
			// 戦闘ＢＧＭ鳴らす
			play_bgm( 1 );
			// サブプロセス番号プラス
			SubProcNo++;
			
			break;
			
		case 1:
		
			/* アクション走らせる */
			RunAction();
			// タスク表示データをバッファに溜める
			StockTaskDispBuffer();
			
			// 背景描画（紫色）
			StockBoxDispBuffer( 0, 0, 640, 480, DISP_PRIO_BG, SYSTEM_PAL_PURPLE , 1 );
			
			break;
	}
}

/* ＳＥ確認プロセス ********************************************************************/
void SeTestProc( void )
{
	char szMoji[ 256 ];
	int x = 128, y = 196;
	static int seNo = 1, bgmFlag;
	
	/* サブプロセス番号で分岐 */
	switch( SubProcNo ){
	
		case 0:
			
			// サブプロセス番号プラス
			SubProcNo++;
			
			break;
			
		case 1:
			
			// 効果音鳴らす
			if( joy_trg[ 0 ] & JOY_A ) play_se( seNo, 320, 240 );
			// フィールドＢＧＭ鳴らす
			if( joy_trg[ 0 ] & JOY_B ){ 
				if( bgmFlag == FALSE ){
					bgmFlag = TRUE;
					play_bgm( 0 );
				}else{
					bgmFlag = FALSE;
					stop_bgm();
				}
			}
			
			// 効果音番号変更（±１）
			if( joy_auto[ 0 ] & JOY_RIGHT ){	/* プラス */
				while( 1 ){
					seNo++;
					if( seNo >= TONE_MAX ) seNo = 1;
					if( tone_tbl[ seNo ].voice_volume != 0 ) break;
				}
			}
			if( joy_auto[ 0 ] & JOY_LEFT ){		/* マイナス */
				while( 1 ){
					seNo--;
					if( seNo < 0 ) seNo = TONE_MAX - 1;
					if( tone_tbl[ seNo ].voice_volume != 0 ) break;
				}
			}
			// 効果音の音量
			if( joy_auto[ 1 ] & JOY_F8 ){		/* プラス */
				tone_tbl[ seNo ].voice_volume++;
				if( tone_tbl[ seNo ].voice_volume >= 128 ) tone_tbl[ seNo ].voice_volume = 127;
			}
			if( joy_auto[ 1 ] & JOY_F7 ){	/* マイナス */
				tone_tbl[ seNo ].voice_volume--;
				if( tone_tbl[ seNo ].voice_volume <= -1 ) tone_tbl[ seNo ].voice_volume = 0;
			}
			
			// 効果音のピッチ
			if( joy_auto[ 1 ] & JOY_F6 ){	/* プラス */
				tone_tbl[ seNo ].voice_note++;
				if( tone_tbl[ seNo ].voice_note + tone_tbl[ seNo ].voice_rate >= 63 ) tone_tbl[ seNo ].voice_note = 62 - tone_tbl[ seNo ].voice_rate;
			}
			if( joy_auto[ 1 ] & JOY_F5 ){	/* マイナス */
				tone_tbl[ seNo ].voice_note--;
				if( tone_tbl[ seNo ].voice_note + tone_tbl[ seNo ].voice_rate <= 0 ) tone_tbl[ seNo ].voice_note = -tone_tbl[ seNo ].voice_rate + 1;
			}
			
			// ＥＳＣで終了
			if( joy_trg[ 0 ] & JOY_ESC ){	/* マイナス */
				// ＷＩＮＤＯＷＳに WM_CLOSE メッセージを送らせる
				PostMessage( hWnd, WM_CLOSE, 0, 0L );
			}
			// 効果音番号
			wsprintf( szMoji, "効果音　番　号 =  %4d ： Left or Right", seNo );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// 効果音音量
			wsprintf( szMoji, "効果音　音　量 =  %4d ： F7   or F8", tone_tbl[ seNo ].voice_volume );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// 効果音ピッチ
			wsprintf( szMoji, "効果音　ピッチ =  %4d ： F5   or F6", tone_tbl[ seNo ].voice_note );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// 効果音鳴らすボタン
			wsprintf( szMoji, "Z：効果音鳴らす　X：ＢＧＭ鳴らす" );
			StockFontBuffer( 16, 460, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			
			// 終了表示
			StockFontBuffer( 530, 460, FONT_PRIO_FRONT, 0, "終了：ESC", 0 );
			
			break;
	}
}

#endif

