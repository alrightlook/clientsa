/************************/
/*	testView.cpp		*/
/************************/
#include "system.h"
#include "loadsprbin.h"
#include "anim_tbl.h"
#include "../oft/vg410.h"
#include "t_music.h"

#ifdef _DEBUG		

// �ҥåȥީ`���I�������ģ�
extern void hit_mark_critical( ACTION *a0 );

// ��ʾ�Хåե������루�ӣ��֣����ã�
void StockDispBufferSprView( int x, int y, UCHAR dispPrio, int bmpNo )
{	
	// ��ر��椹������ޤǥ��ɥ쥹���M�ޤ���
	DISP_SORT 	*pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt;
	DISP_INFO 	*pDispInfo = DispBuffer.DispInfo + DispBuffer.DispCnt;
	
	// ������ȥ��`�Щ`�Εr
	if( DispBuffer.DispCnt >= DISP_BUFFER_SIZE ) return;
	
	// ��ʾ���ȥ��`���Ø�����
	pDispSort->dispPrio = dispPrio;
	pDispSort->no = DispBuffer.DispCnt;
	// ��ʾ������壨 ���`�Ȥ��ʤ����� ��
	pDispInfo->x = x;
	pDispInfo->y = y;
	pDispInfo->bmpNo = bmpNo;
	pDispInfo->hitFlag = 2;

	// ��ʾ�����󥿩`�ץ饹
	DispBuffer.DispCnt++;
}

/* ���ץ饤�ȴ_�J�ץ����� ********************************************************************/
void SprViewProc( void )
{
	char szMoji[ 256 ];
	int bmpX;
	int bmpY;
	static int bmpNo = 0;
	int bmpNoBak;
	static int no = 0; // ���ַ���
	static BOOL fontFlag = TRUE; // �ե���ȱ�ʾ�ե饰
	// ���֥Ʃ`�֥�
	int zoubunTbl[] ={   1,  5,  25,  100, 	500,  1000,
						-1, -5, -25, -100, -500, -1000 };
	static int palNo = 0; // �ѥ�åȷ���
	static int time = 600; // �ѥ�åȷ���
	
	/* ���֥ץ��������ŤǷ�� */
	switch( SubProcNo ){
	
		case 0:
			// �Хå��Хåե��`�軭�����Ƿ��
			//BackBufferDrawType = DRAW_BACK_NON
			// ���`�ܩ`�ɤǲ�������r
			// �M�ޤ���
			if( joy_auto[ 0 ] & JOY_RIGHT ) bmpNo += zoubunTbl[ no ];
			// ����
			if( joy_auto[ 0 ] & JOY_LEFT ) bmpNo -= zoubunTbl[ no ];
			// ���֥ץ饹
			if( joy_trg[ 0 ] & JOY_UP ){ 
				no++;
				if( no >= 6 ) no = 0; // ��ߥåȥ����å�
			}
			// ���֥ޥ��ʥ�
			if( joy_trg[ 0 ] & JOY_DOWN ){
				no--;
				if( no <= -1 ) no = 5; // ��ߥåȥ����å�
			}
#if 0
			// �ޥ����ǲ�������r
			// �󥯥�å����M��
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				bmpNo += zoubunTbl[ no ];
				// ����å���
				play_se( 201, 320, 240 );
			}
			// ���Х���å������ͤ�
			if( GetAsyncKeyState( VK_MBUTTON ) ){
				bmpNo += zoubunTbl[ no ];
			}
			
			// �ҥ���å��ǉ��։��
			if( mouse.onceState & MOUSE_RIGHT_CRICK ){
			//if( mouse.state & MOUSE_RIGHT_CRICK ){
				no++;
				if( no == 6 ) no = 0; // ��ߥåȥ����å�
				// ����å���
				play_se( 201, 320, 240 );
			}
#endif			
			// ��ߥåȥ����å�
			if( bmpNo < 0 ) bmpNo = 0;
			if( bmpNo >= MAX_GRAPHICS - 25 ) bmpNo = MAX_GRAPHICS -25;
			
			// �ѥ�åȉ��
			if( joy_trg[ 0 ] & JOY_A ){	/* �ץ饹 */
				palNo--;
				if( palNo < 0 ) palNo = MAX_PAL - 1;
				// �ѥ�åȥ�����
				PaletteChange( palNo, 10 );
				// ������ɥ��_����
				play_se( 202, 320, 240 );
			}
			if( joy_trg[ 0 ] & JOY_B ){	/* �ޥ��ʥ� */
				palNo++;
				if( palNo >= MAX_PAL ) palNo = 0;
				// �ѥ�åȥ�����
				PaletteChange( palNo, 10 );
				// ������ɥ��_����
				play_se( 202, 320, 240 );
			}
			// �ե���ȥե饰���
			if( joy_trg[ 0 ] & JOY_DEL ){	/* �ץ饹 */
				if( fontFlag ) fontFlag = 0;
				else fontFlag = 1;
				// �����ƥ�ʹ����
				play_se( 212, 320, 240 );
			}
			
			// �ţӣäǽK��
			if( joy_trg[ 0 ] & JOY_ESC ){	/* �ޥ��ʥ� */
				// �ףɣΣģϣףӤ� WM_CLOSE ��å��`�����ͤ餻��
				PostMessage( hWnd, WM_CLOSE, 0, 0L );
			}
			
			// �£ͣз��ťХå����å�
			bmpNoBak = bmpNo;
			
			// һ����֥�`�ף�����ö�֣�
			for( bmpY = 0 ; bmpY < 480 ; bmpY += 96 ){
				for( bmpX = 0 ; bmpX < 640 ; bmpX += 128 ){
					// �ե���ȥե饰�ϣΤΕr
					if( fontFlag == TRUE ){
						// �£ͣз���
						wsprintf( szMoji,"%7d", bmpNoBak );
						StockFontBuffer( bmpX, bmpY, FONT_PRIO_BACK, 0, szMoji, 0 );
					}
					// �£ͣб�ʾ�Хåե��ˤ����
					StockDispBufferSprView( bmpX, bmpY, 0, bmpNoBak++ );
				}
			}
			// �ե���ȥե饰�ϣΤΕr
			if( fontFlag == TRUE ){
				// ���ֱ�ʾ
				wsprintf( szMoji, "PAL:%2d������:%6d", palNo, zoubunTbl[ no ] );
				StockFontBuffer( 640 - 16 * 12, 462, FONT_PRIO_FRONT, 0, szMoji, 0 );
					
				// �K�˱�ʾ
				StockFontBuffer( 4, 462, FONT_PRIO_FRONT, 0, "ESC:�K�ˡ�X or Y:PAL�����DEL:���� ON/OFF", 0 );
			}
			/* ����������ߤ餻�� */
			RunAction();
			// ��������ʾ�ǩ`����Хåե�������
			StockTaskDispBuffer();
			
			// �����軭����ɫ��
			StockBoxDispBuffer( 0, 0, 640, 480, DISP_PRIO_BG, SYSTEM_PAL_PURPLE , 1 );
			
			// �F�ڤΕr�g��ӛ��
			NowTime = GetTickCount();

			break;
	}
}

/* ���˥�`����󥢥������I�� ***********************************************/
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
	
	/* ״�B�Ƿ�� */
	switch( pAct->state ){
		/* ͨ���r */
		case ACT_STATE_ACT:
			// �����`�Ǥʤ��r
			if( slowFlag == FALSE ){
				// ���ץ饤�ȷ��ŉ����������
				if( joy_auto[ 0 ] & JOY_RIGHT ){	/* �ץ饹 */
					while( 1 ){
						pAct->anim_chr_no++;
						if( SpriteData[pAct->anim_chr_no - SPRSTART].ptAnimlist !=NULL ) break;
						if( pAct->anim_chr_no > 1000000 ){
							 pAct->anim_chr_no = 1000000;
							 break;
						}
					}
				}
				if( joy_auto[ 0 ] & JOY_LEFT ){		/* �ޥ��ʥ� */
					while( 1 ){
						pAct->anim_chr_no--;
						if( SpriteData[pAct->anim_chr_no - SPRSTART].ptAnimlist !=NULL ) break;
						if( pAct->anim_chr_no <  SPRSTART ){ 
							pAct->anim_chr_no =  SPRSTART;
							break;
						}
					}
				}
				// ���ץ饤�ȷ��ŉ����������������
				if( joy_trg[ 0 ] & JOY_INS ){	/* �ץ饹 */
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
				if( joy_trg[ 0 ] & JOY_DEL ){	/* �ޥ��ʥ� */
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
				// ���˥�`������Ф�o�������å�
				//while( 1 ){
				//if( SpriteData[ pAct->anim_chr_no ].ptAnimlist == NULL )
				//{
					
				
				// �����Q
				if( joy_auto[ 0 ] & JOY_UP ){		/* �ץ饹 */
					pAct->anim_ang++;
					if( pAct->anim_ang >= 8 ) pAct->anim_ang = 0;
				}
				if( joy_auto[ 0 ] & JOY_DOWN ){		/* �ޥ��ʥ� */
					pAct->anim_ang--;
					if( pAct->anim_ang < 0 ) pAct->anim_ang =  7;
				}
				
				// ���˥�`����󷬺ű�ʾ
				if( joy_trg[ 0 ] & JOY_HOME ){		/* �ץ饹 */
					pAct->anim_no++;
					if( pAct->anim_no > 12 ) pAct->anim_no = 12;
				}
				if( joy_trg[ 0 ] & JOY_END ){	/* �ޥ��ʥ� */
					pAct->anim_no--;
					if( pAct->anim_no < 0 ) pAct->anim_no =  0;
				}
				
				// ���ԩ`�ɉ��
				if( joy_trg[ 0 ] & JOY_A ){	/* �ץ饹 */
					anim_spd--;
					if( anim_spd < 0 ) anim_spd = 0;
				}
				if( joy_trg[ 0 ] & JOY_B ){	/* �ޥ��ʥ� */
					anim_spd++;
					if( anim_spd >= 255 ) anim_spd = 255;
				}
			}
			// �ţӣäǽK��
			if( joy_trg[ 0 ] & JOY_ESC ){	/* �ޥ��ʥ� */
				// �ףɣΣģϣףӤ� WM_CLOSE ��å��`�����ͤ餻��
				PostMessage( hWnd, WM_CLOSE, 0, 0L );
			}
			
			
			// �����ͤ�ե饰�ϣ�
			if( joy_trg[ 1 ] & JOY_F12 ){ 
				pAct->anim_frame_cnt = 0;
				slowFlag = TRUE;
			}
			// ���ޑ����ե饰�ϣ�
			if( joy_trg[ 1 ] & JOY_F11 ){ 
				pAct->anim_cnt -= 2;
				// ��ߥåȥ����å�
				if( pAct->anim_cnt <= 0 ) pAct->anim_cnt = 0;
				pAct->anim_frame_cnt = 0;
				slowFlag = TRUE;
			}
			// �����ͤ�ե饰�ϣƣ�
			if( joy_trg[ 1 ] & JOY_F10 ) slowFlag = FALSE;
			
			if( slowFlag == FALSE || joy_trg[ 1 ] & JOY_F11 || joy_trg[ 1 ] & JOY_F12 ){
				// ���˥�`�����
				pattern( pAct, anim_spd, ANM_LOOP );
			}
			
			// �������ж��Εr
			if( pAct->anim_hit >= 10000 ){
				hitNo = pAct->anim_hit; // �������ж�����ӛ��
				pAct->anim_hit = 0; // ���ڻ�
				// �ҥåȥީ`������
				for( d0=5 ;d0>0 ;d0--){
					//�ҥåȥީ`�����å�
					a1 = GetAction( PRIO_JIKI, sizeof( ATR_EQU ) );
					if( a1 == NULL ) return;
					/* ��ʾ���ȶ� */
					ATR_DISP_PRIO(a1) = DISP_PRIO_CHAR + 1;
					/* ���ץ饤�ȷ��� */
					ATR_CHR_NO(a1) = CG_HIT_MARK_22;
					ATR_H_POS(a1) = 320;
					ATR_V_POS(a1) = 240;
					ATR_SPD(a1) = Rnd( 4, 7 );
					ATR_CRS(a1) = d0 * 6;
					ATR_FIRST_FLG(a1) = ATR_STIMER(a1) = 32;		//��ʾ�r�g���å�
					ATR_KAISHIN(a1) = 1;
					ATR_NAME(a1) = hit_mark_critical;
					ATR_CHR_NO(a1) = CG_HIT_MARK_01;
				}
			}
			// ���ץ饤�ȷ��ű�ʾ
			wsprintf( szMoji, "���ץ饤�ȷ��� =  %8d �� Left or Right ( ��1 )", pAct->anim_chr_no );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			StockFontBuffer( x + 236, y, FONT_PRIO_FRONT, 0, "�� DEL  or INS ( ��1000 )", 0 ); y += 20;
			// �����ʾ
			wsprintf( szMoji, "�������������� =  %8d �� Down or UP", pAct->anim_ang );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// ���˥�`����󷬺ű�ʾ
			wsprintf( szMoji, "�С��ӡ������� =  %8d �� END  or HOME", pAct->anim_no );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// ���ֱ�ʾ
			wsprintf( szMoji, "�����ԡ��`���� =  %8d �� Z    or X", anim_spd );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			
			// ����������
			wsprintf( szMoji, "�������������� =  %8d", t_music_se_no );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// ����������r
			if( t_music_se_no != -1 ){
				// ������������
				if( joy_trg[ 1 ] & JOY_F8 ){		/* �ץ饹 */
					tone_tbl[ t_music_se_no ].voice_volume++;
					if( tone_tbl[ t_music_se_no ].voice_volume >= 128 ) tone_tbl[ t_music_se_no ].voice_volume = 127;
				}
				if( joy_trg[ 1 ] & JOY_F7 ){	/* �ޥ��ʥ� */
					tone_tbl[ t_music_se_no ].voice_volume--;
					if( tone_tbl[ t_music_se_no ].voice_volume <= -1 ) tone_tbl[ t_music_se_no ].voice_volume = 0;
				}
				
				// �������Υԥå�
				if( joy_trg[ 1 ] & JOY_F6 ){	/* �ץ饹 */
					tone_tbl[ t_music_se_no ].voice_note++;
					if( tone_tbl[ t_music_se_no ].voice_note + tone_tbl[ t_music_se_no ].voice_rate >= 63 ) tone_tbl[ t_music_se_no ].voice_note = 62 - tone_tbl[ t_music_se_no ].voice_rate;
				}
				if( joy_trg[ 1 ] & JOY_F5 ){	/* �ޥ��ʥ� */
					tone_tbl[ t_music_se_no ].voice_note--;
					if( tone_tbl[ t_music_se_no ].voice_note + tone_tbl[ t_music_se_no ].voice_rate <= 0 ) tone_tbl[ t_music_se_no ].voice_note = -tone_tbl[ t_music_se_no ].voice_rate + 1;
				}
				// ����������
				wsprintf( szMoji, "�������������� =  %8d �� F7 or F8", tone_tbl[ t_music_se_no ].voice_volume );
				StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
				// �������ԥå�
				wsprintf( szMoji, "���������ԥå� =  %8d �� F5 or F6", tone_tbl[ t_music_se_no ].voice_note );
				StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			}else{
				// ����������
				wsprintf( szMoji, "�������������� =  %8d �� F7 or F8", 0 );
				StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
				// �������ԥå�
				wsprintf( szMoji, "���������ԥå� =  %8d �� F5 or F6", 0 );
				StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			}
			
			// �£ͣз���
			//wsprintf( szMoji, "�£ͣС������� =  %8d", pAct->bmpNo );
			//StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// ��������ʾ
			//wsprintf( szMoji, "�£ͣС������� = %4d,%4d", SpriteInfo[ pAct->bmpNo ].width, SpriteInfo[ pAct->bmpNo ].height );
			//StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
						
			// �������ж�����
			wsprintf( szMoji, "�������ж����� =  %8d", hitNo );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			
			// �����ͤ���h��
			wsprintf( szMoji, "F12�������ͤꡡF11�����ޑ�����F10���ϣƣ�" );
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			
			// �K�˱�ʾ
			StockFontBuffer( 530, 460, FONT_PRIO_FRONT, 0, "�K�ˣ�ESC", 0 );
			
			break;
		
		/* �����r */	
		case ACT_STATE_DEAD:
		
			DeathAction( pAct );
			
			break;
	}
}

/* ���˥�`����󥢥���������� ***********************************************/
ACTION *MakeAnimSpr( void )
{
	ACTION *pAct;
	
	/* ���������ꥹ�Ȥ˵��h */
	pAct = GetAction( PRIO_JIKI, 0 );
	if( pAct == NULL ) return NULL;
	
	/* �g���v�� */
	pAct->func = AnimSpr;
	// anim_tbl.h �η���
	pAct->anim_chr_no = SPRSTART; 
	// ��������
	pAct->anim_no = 0;//ANIM_ATTACK;
	// ���˥�`�������( ������ )( �¤������һؤ� )
	pAct->anim_ang = 0;//Rnd( 0, 7 );
	// �ܥå�����ʾ
	pAct->atr |= ACT_ATR_HIT_BOX;
	/* ��ʾ���ȶ� */
	pAct->dispPrio = DISP_PRIO_CHAR;
	/* ����λ�� */
	pAct->x = 320;
	pAct->y = 360;
	
	// ���˥�`�����
	pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
	
	return pAct;
}

/* ���˥�`�����_�J�ץ����� ********************************************************************/
void AnimViewProc( void )
{
	/* ���֥ץ��������ŤǷ�� */
	switch( SubProcNo ){
	
		case 0:
			
			// ���˥�`����󥢥����������
			MakeAnimSpr();
			// ���L�£ǣ��Q�餹
			play_bgm( 1 );
			// ���֥ץ��������ťץ饹
			SubProcNo++;
			
			break;
			
		case 1:
		
			/* ����������ߤ餻�� */
			RunAction();
			// ��������ʾ�ǩ`����Хåե�������
			StockTaskDispBuffer();
			
			// �����軭����ɫ��
			StockBoxDispBuffer( 0, 0, 640, 480, DISP_PRIO_BG, SYSTEM_PAL_PURPLE , 1 );
			
			break;
	}
}

/* �ӣŴ_�J�ץ����� ********************************************************************/
void SeTestProc( void )
{
	char szMoji[ 256 ];
	int x = 128, y = 196;
	static int seNo = 1, bgmFlag;
	
	/* ���֥ץ��������ŤǷ�� */
	switch( SubProcNo ){
	
		case 0:
			
			// ���֥ץ��������ťץ饹
			SubProcNo++;
			
			break;
			
		case 1:
			
			// �������Q�餹
			if( joy_trg[ 0 ] & JOY_A ) play_se( seNo, 320, 240 );
			// �ե��`��ɣ£ǣ��Q�餹
			if( joy_trg[ 0 ] & JOY_B ){ 
				if( bgmFlag == FALSE ){
					bgmFlag = TRUE;
					play_bgm( 0 );
				}else{
					bgmFlag = FALSE;
					stop_bgm();
				}
			}
			
			// ���������ŉ����������
			if( joy_auto[ 0 ] & JOY_RIGHT ){	/* �ץ饹 */
				while( 1 ){
					seNo++;
					if( seNo >= TONE_MAX ) seNo = 1;
					if( tone_tbl[ seNo ].voice_volume != 0 ) break;
				}
			}
			if( joy_auto[ 0 ] & JOY_LEFT ){		/* �ޥ��ʥ� */
				while( 1 ){
					seNo--;
					if( seNo < 0 ) seNo = TONE_MAX - 1;
					if( tone_tbl[ seNo ].voice_volume != 0 ) break;
				}
			}
			// ������������
			if( joy_auto[ 1 ] & JOY_F8 ){		/* �ץ饹 */
				tone_tbl[ seNo ].voice_volume++;
				if( tone_tbl[ seNo ].voice_volume >= 128 ) tone_tbl[ seNo ].voice_volume = 127;
			}
			if( joy_auto[ 1 ] & JOY_F7 ){	/* �ޥ��ʥ� */
				tone_tbl[ seNo ].voice_volume--;
				if( tone_tbl[ seNo ].voice_volume <= -1 ) tone_tbl[ seNo ].voice_volume = 0;
			}
			
			// �������Υԥå�
			if( joy_auto[ 1 ] & JOY_F6 ){	/* �ץ饹 */
				tone_tbl[ seNo ].voice_note++;
				if( tone_tbl[ seNo ].voice_note + tone_tbl[ seNo ].voice_rate >= 63 ) tone_tbl[ seNo ].voice_note = 62 - tone_tbl[ seNo ].voice_rate;
			}
			if( joy_auto[ 1 ] & JOY_F5 ){	/* �ޥ��ʥ� */
				tone_tbl[ seNo ].voice_note--;
				if( tone_tbl[ seNo ].voice_note + tone_tbl[ seNo ].voice_rate <= 0 ) tone_tbl[ seNo ].voice_note = -tone_tbl[ seNo ].voice_rate + 1;
			}
			
			// �ţӣäǽK��
			if( joy_trg[ 0 ] & JOY_ESC ){	/* �ޥ��ʥ� */
				// �ףɣΣģϣףӤ� WM_CLOSE ��å��`�����ͤ餻��
				PostMessage( hWnd, WM_CLOSE, 0, 0L );
			}
			// ����������
			wsprintf( szMoji, "�������������� =  %4d �� Left or Right", seNo );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// ����������
			wsprintf( szMoji, "�������������� =  %4d �� F7   or F8", tone_tbl[ seNo ].voice_volume );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// �������ԥå�
			wsprintf( szMoji, "���������ԥå� =  %4d �� F5   or F6", tone_tbl[ seNo ].voice_note );
			StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			// �������Q�餹�ܥ���
			wsprintf( szMoji, "Z���������Q�餹��X���£ǣ��Q�餹" );
			StockFontBuffer( 16, 460, FONT_PRIO_FRONT, 0, szMoji, 0 ); y += 20;
			
			// �K�˱�ʾ
			StockFontBuffer( 530, 460, FONT_PRIO_FRONT, 0, "�K�ˣ�ESC", 0 );
			
			break;
	}
}

#endif
