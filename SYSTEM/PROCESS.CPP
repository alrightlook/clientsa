/************************/
/*	process.c	������Ҫ��������	*/
/************************/
#include "system.h"
#include "../ohta/ohta.h"
#include "../dwaf/dwafTestProc.h"
#include "ime_sa.h"
#include "menu.h"
#include "pc.h"
#include "character.h"
#include "login.h"
#include "netproc.h"
#include "savedata.h"
#include "testView.h"
#include "battleProc.h"
#include "produce.h"
#include "lssproto_cli.h"
#include "netmain.h"
#include "battleMenu.h"
#include "t_music.h"
#include "field.h"
#include "handletime.h"

/* ���������*******************************************************************/
extern void kakushi_command(void);
/* ���̺� */
UINT ProcNo;
/* �ӽ��� */
UINT SubProcNo;

int palNo;//�ֵ�
int oldPalNo;

/* �ץ������I�� ********************************************************************/
void Process( void )
{
	/* �ץ�������� */
	switch( ProcNo ){
	
		case PROC_OPENNING:
		
			break;
			
		case PROC_INIT: 	/* ���ڻ��ץ����� */
		
			InitProc();
			
			break;
			

	// ����֤�����봦��
		case PROC_ID_PASSWORD:
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			idPasswordProc();
			RunAction();			// ִ�ж���
			StockTaskDispBuffer();	// ��ʾ���ݻ�����������
			FlashKeyboardCursor();	// ���������˸����/ /
			ImeProc();				// ���뷨�йصĴ���
			break;

		// ����Ͳ˵�ѡ��
		case PROC_TITLE_MENU:
			// /�ı����˻�����
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			titleProc();
			TitleProduce();			// ְ��/ 
			RunAction();			// /ִ�ж���
			StockTaskDispBuffer();	// ��ʾ���ݻ�����������
			kakushi_command();		//���ص������
			break;

		// ����ѡ��
		case PROC_CHAR_SELECT:		// ����ѡ��
			// ���ĺ����Ⱦ����
			BackBufferDrawType = DRAW_BACK_NORMAL;// ��׼
			selectCharacterProc();
			break;



		// ��������
		case PROC_CHAR_MAKE:
			// ��Ⱦ����
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			makeCharacterProc();
			break;

			
		// ��ʼ��½ѡ��������
		case PROC_CHAR_LOGIN_START:
			// ת������
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			// charLoginStart();�����δ������ϵ����ݱ��ݻ�����
			//��ʼ���Ա�����
			initMap();		// ��ͼ
			initPcAll();	// PC�ĳ�ʼ����Ϣ�����
			initCharObj();	// ��ʼ�����������
			initOpenServerWindow();	// ��ʼ���Ĵ�������ʾ����������·
			initFieldProc();		// ���ڳ�ʼ��
			initMapEffect();		// ��ͼ��ʼ����Ӱ��
			EncountFlag = FALSE;
			logOutFlag = FALSE;
			InitOhtaParam();	// ��������


			
			ChangeProc( PROC_CHAR_LOGIN );
			// �£ǣͥե��`�ɥ������_ʼ
			fade_out_bgm();//�˳�GM��
			break;

		case PROC_CHAR_LOGIN:
			// �Хå��Хåե��`�軭�������
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			characterLoginProc();
			break;

		// ����������
		case PROC_CHAR_LOGOUT:
			// ��־���
//			BackBufferDrawType = DRAW_BACK_NORMAL; 
			characterLogoutProc();
			break;


		case PROC_GAME:     /*���̵���Ϸ */
			
			GameProc();
			break;

		case PROC_DISCONNECT_SERVER://�Ͽ���Ϸ��
			switch( SubProcNo )
			{
				case 0:
					// ���ڻ�
					// PC�ꥻ�å�  / /��ʼ��/ /���Ը�λ
					resetPc();
					// ���������
					initCharObj();
					// ȡ������
					DeathAllAction();


					//  /��ʼ������
					ProduceInitFlag = TRUE;
					// ���Ʊ�������ͼ��
					CopyBackBuffer();
					// ��Ⱦ����
					BackBufferDrawType = DRAW_BACK_PRODUCE; 
					// ��������
					fade_out_bgm();
					// ��¼��ǰʱ��

					NowTime = GetTickCount();
					SubProcNo++;

					break;

				case 1:
					// ����
					if( DrawProduce( PRODUCE_DOWN_ACCELE ) == TRUE )
					{
						BackBufferDrawType = DRAW_BACK_NORMAL; 
						SubProcNo++;
					}
					break;

				case 2:
					// ����
					break;
			}
			// �Ͽ�����Windows ����
			if( disconnectServer() )
			{
				// ��ʼ������
				cleanupNetwork();
				// �ָ�Ĭ�ϵ�ɫ��
				PaletteChange( DEF_PAL, 0 );
				// �����⻭��
				ChangeProc( PROC_TITLE_MENU );
				// /�����ж�
				DeathAllAction();
				disconnectServerFlag = FALSE;
				oldDisconnectServerFlag = FALSE;
				break;
			}
			RunAction();			// ����������ߤ餻��  ��������
			StockTaskDispBuffer();	// ��������ʾ�ǩ`����Хåե�������  ��ʾ�������񻺳���
			break;


		case PROC_BATTLE:     /* �Хȥ�ץ����� */
		
			BattleProc();
			break;
			
#ifdef _DEBUG		
		//case PROC_OHTA_TEST:     /* ̫��ƥ��ȥץ����� */
		
		//	OhtaTestProc();
		//	break;
			
		//case PROC_TAKE_TEST:     /* ���ڤ���ƥ��ȥץ����� */
		
		//	TakeTestProc();
		//	break;

		//case PROC_DWAF_TEST:	// DWAF�ƥ���
		//	dwafTestProc();
		//	break;

		case PROC_SPR_VIEW:		// ��ʼ��֤
			SprViewProc();
			break;
			
		case PROC_ANIM_VIEW:	// ������֤
			AnimViewProc();
			break;
			
		case PROC_SE_TEST:	//  SE��������
			SeTestProc();
			break;
#endif
		case PROC_ENDING:
			
			break;
			
	}
}

/* �ץ���������I�� ***********************************************************/
void ChangeProc( int procNo )
{
	// �ץ��������Ť�ӛ��
	ProcNo = procNo;
	// ���֥ץ��������Ťγ��ڻ�
	SubProcNo = 0;
}
/* �ץ���������I�� ***********************************************************/
void ChangeProc( int procNo, int subProcNo )
{
	// �ץ��������Ť�ӛ��
	ProcNo = procNo;
	// ���֥ץ��������Ťγ��ڻ�
	SubProcNo = subProcNo;
}

/* ���`��ץ������I�� ********************************************************************/
void GameProc( void )
{
	static int now_bgm;
	static BOOL produceFlag;	// �ݳ��ե饰

	/* ���֥ץ��������ŤǷ�� */
	switch( SubProcNo ){
	
		case 0:
#ifdef _DEBUG
			// ���ե饤���`�ɤΕr�Υޥå׳��ڻ�
			if( offlineFlag )
			{
				initMap();		// �ޥå׳��ڻ�
			}
#endif

			//InitIme();		// �ɣͣ��v�B�γ��ڻ�
			ClrImeCompositionString(); // ������ɥ����ΣɣͣťХåե��`�򥯥ꥢ����
			InitChat();		/* ����å��v�B���ڻ� */
			// �ݳ����ť����ࣨż���Τߣ�
			//ProduceNo = Rnd( 0, PRODUCE_END - 1 ) & 0xfffffffe;
			produceFlag = FALSE;	// �ݳ��ե饰�ϣƣ�
			EncountFlag = FALSE;	// ���󥫥���ȥե饰�ϣƣ�
			//�ե��`��ɣ£ǣ������_ʼ
//			play_bgm(0);
			// ����Х���i���z��  ר���Ķ�ȡ  �ಾ
			LoadAlbum();
			
			SubProcNo++;

		// ���������ݳ�
		case 150:
			SubProcNo = 100;

		case 100:
			initPc();				// �ץ쥤��`���������
			resetFieldProc();		// �ե��`��ɄI���Υꥻ�å�
			restorePtActCharObjAll();	// ���������Ʃ`�֥�Υ��������ͻ�
			initOpenServerWindow();	// ���`�Ф�ָʾ�Ǳ�ʾ���륦����ɥ��γ��ڻ�
			InitMenu();		// ��˥�`�v�B�γ��ڻ�
//			BattleStatusBak[ 0 ] = NULL;// �Хȥ�״�B���ڻ�
//			BattleStatusReadPointer = BattleStatusWritePointer =0;
			// �����ե��`����ȡ��
			GetKeyInputFocus( &MyChatBuffer );
			SubProcNo++;

		case 101:
			if( loginFlag )
			{
				BackBufferDrawType = DRAW_BACK_NORMAL; 
				break;
			}
			SubProcNo++;

		case 102:
			// �ݳ����������
			// �Хåե����ڻ�
			paletteProc();

			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			initCharPartsPrio();	// �����?�ѩ`�Ĥ΃����λ�Q���I���γ��ڻ�
			RunAction();			// ����������ߤ餻��
			StockTaskDispBuffer();	// ��������ʾ�ǩ`����Хåե�������
			stop_bgm();				//�£ǣ�ֹͣ
			drawMap();				//	�ޥåױ�ʾ
			MenuProc();				// ��˥�`�I��
			// һ�Х���ե��ډT��Ĥ֤����\ɫ��
			//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
			ChatProc();				// ����åȄI��
			//���컺�����建����
			ChatBufferToFontBuffer(); // ����åȥХåե��`��ե���ȥХåե�������
			ImeProc();				// �ɣͣ��v�B�I��
			SortDispBuffer(); 	// ��ʾ�Хåե����`��
			// �Хå����`�ե������\�ǥ��ꥢ�`
			ClearBackSurface();	
			// �£ͣФ�Хå����`�ե����˥��å�
			PutBmp();	
			// �Хå����`�ե�������Хȥ륵�`�ե����إ��ԩ`
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// �Хåե����ڻ�
			DispBuffer.DispCnt = 0;
			FontCnt = 0;

		//	_asm{int 3}  ��� �����ͳ��� �ҹر�
		//	play_bgm( map_bgm_no );

			// �F�ڤΕr�g��ӛ��
			NowTime = GetTickCount();
			// �Хå��Хåե��`�軭�������
//			BackBufferDrawType = DRAW_BACK_NORMAL; 
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			// �ݳ���
			ProduceInitFlag = TRUE;
			DrawProduce( PRODUCE_BRAN_BIG );
			SubProcNo++;
			break;

		case 103:
			// �ݳ���
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_BRAN_BIG ) == TRUE )
			{
				SubProcNo = 3;
				produceFlag = TRUE;
				fieldInfoTime = GetTickCount();
			}
			break;


		// ��`���ݳ�
		case 200:
			// �£ǣͥե��`�ɥ������_ʼ
//			fade_out_bgm();

			// S�ץ��ȥ����C�� warpEffectProc();�����Ф��
			// ���������ΤǤ����ǤϤ��ʤ�������
			SubProcNo++;

		case 201:
			// �ݳ���
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_CENTER_PRESSIN ) == TRUE )
			{
				SubProcNo++;
			}
			drawField();	// �ե��`��ɻ���ˤ���������ʾ
			MenuProc();	// ��˥�`�I��
			ImeProc();	// �ɣͣ��v�B�I��
			break;

		case 202:
			if( !warpEffectStart || !warpEffectOk )
			{
				BackBufferDrawType = DRAW_BACK_NORMAL; 
				drawField();	// �ե��`��ɻ���ˤ���������ʾ
				MenuProc();	// ��˥�`�I��
				ImeProc();	// �ɣͣ��v�B�I��
				break;
			}
			warpEffectStart = FALSE;
			warpEffectOk = FALSE;
			SubProcNo++;

		case 203:
			// �ݳ����������
			// �Хåե����ڻ�  /�ݳ�ͼ�������/ /������ڻ�
			paletteProc();

			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			initCharPartsPrio();	// �����?�ѩ`�Ĥ΃����λ�Q���I���γ��ڻ�
			RunAction();			// ����������ߤ餻��
			StockTaskDispBuffer();	// ��������ʾ�ǩ`����Хåե�������
			redrawMap();
			drawMap();				//	�ޥåױ�ʾ

#if 1
			// ���ե����ȤΥ��ꥢ
			if( (mapEffectRainLevel == 0 && oldMapEffectRainLevel != 0 )
			 || (mapEffectSnowLevel == 0 && oldMapEffectSnowLevel != 0 ) )
			{
				initMapEffect();
			}
#endif

			// һ�Х���ե��ډT��Ĥ֤����\ɫ��
			//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
			ChatProc();				// ����åȄI��
			ChatBufferToFontBuffer(); // ����åȥХåե��`��ե���ȥХåե�������
			SortDispBuffer(); 	// ��ʾ�Хåե����`��
			// �Хå����`�ե������\�ǥ��ꥢ�`
			ClearBackSurface();	
#if 1
			if( (mapEffectRainLevel != 0 && oldMapEffectRainLevel == 0 )
			 || (mapEffectSnowLevel != 0 && oldMapEffectSnowLevel == 0 ) )
			{
				mapEffectProc2( 80 );		// �ޥåץ��ե����ȣ���?ѩ�ȣ�
			}
#endif
			// �£ͣФ�Хå����`�ե����˥��å�
			PutBmp();	
			// �Хå����`�ե�������Хȥ륵�`�ե����إ��ԩ`
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// �Хåե����ڻ�
			DispBuffer.DispCnt = 0;
			FontCnt = 0;

//			if( map_bgm_no != now_bgm )
//			{
//				stop_bgm();				//�£ǣ�ֹͣ
//				play_bgm( map_bgm_no );
//			}

			// �F�ڤΕr�g��ӛ��
			NowTime = GetTickCount();

			ProduceInitFlag = TRUE;

			SubProcNo++;

		case 204:
			// �ݳ���
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_CENTER_PRESSOUT ) == TRUE )
			{
				SubProcNo = 3;
				fieldInfoTime = GetTickCount();
			}
			drawField();	// �ե��`��ɻ���ˤ���������ʾ
			MenuProc();	// ��˥�`�I��
			ImeProc();	// �ɣͣ��v�B�I��
			break;


		case 1:
			initPc();				// �ץ쥤��`���������
			resetFieldProc();		// �ե��`��ɄI���Υꥻ�å�
			restorePtActCharObjAll();	// ���������Ʃ`�֥�Υ��������ͻ�
			initOpenServerWindow();	// ���`�Ф�ָʾ�Ǳ�ʾ���륦����ɥ��γ��ڻ�
			InitMenu();		// ��˥�`�v�B�γ��ڻ�
//			BattleStatusBak[ 0 ] = NULL;// �Хȥ�״�B���ڻ�
//			BattleStatusReadPointer = BattleStatusWritePointer =0;
			// �����ե��`����ȡ��
			GetKeyInputFocus( &MyChatBuffer );
			
			// �ݳ�����r
			if( produceFlag == TRUE ){
			// ���L�Y��������ɥ��ե饰�ϣ�
			//if( BattleResultWndFlag == TRUE ){

				// �ݳ����������
				// �Хåե����ڻ�
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				initCharPartsPrio();	// �����?�ѩ`�Ĥ΃����λ�Q���I���γ��ڻ�
				RunAction();			// ����������ߤ餻��
				StockTaskDispBuffer();	// ��������ʾ�ǩ`����Хåե�������
				stop_bgm();				//�£ǣ�ֹͣ
				drawMap();				//	�ޥåױ�ʾ
				//�£ǣͤ��ʤ����
				if(!draw_map_bgm_flg){
					//���ä��Σ£ǣ�����
					play_bgm(map_bgm_no = now_bgm);
					draw_map_bgm_flg = 1;
				}
				MenuProc();				// ��˥�`�I��
				// һ�Х���ե��ډT��Ĥ֤����\ɫ��
				//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
				ChatProc();				// ����åȄI��
				ChatBufferToFontBuffer(); // ����åȥХåե��`��ե���ȥХåե�������
				ImeProc();				// �ɣͣ��v�B�I��
				SortDispBuffer(); 	// ��ʾ�Хåե����`��
				// �Хå����`�ե������\�ǥ��ꥢ�`
				ClearBackSurface();	
				// �£ͣФ�Хå����`�ե����˥��å�
				PutBmp();	
				// �Хå����`�ե�������Хȥ륵�`�ե����إ��ԩ`
				lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
				// �Хåե����ڻ�
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				// �F�ڤΕr�g��ӛ��
				NowTime = GetTickCount();
				// �Хå��Хåե��`�軭�������
				BackBufferDrawType = DRAW_BACK_PRODUCE; 
				// �ݳ���
				DrawProduce( PRODUCE_4WAY_IN );
			}else{
				// �ݳ��ե饰�ϣ�
				produceFlag = TRUE;
				SubProcNo++;
			}
			SubProcNo++;
			
			break;
		
		case 2:	// �ե��`��ɥ����ݳ�
			
			// �ݳ���
			if( DrawProduce( PRODUCE_4WAY_IN ) == TRUE ){
				SubProcNo++;
				warpEffectStart = FALSE;
				warpEffectOk = FALSE;
			}
			break;
			
		case 3:
			// �Хå��Хåե��`�軭�������
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			
			paletteProc();
			
			initItemOverlapCheck();	// �����ƥ��ؤʤ�����å��I���γ��ڻ�
			drawGrid();				// ����åɥ��`�����ʾ
			fieldProc();			// �ե��`����v�B�I��
			moveProc();				// �ƄӄI��
			charObjProc();			// ��������I��
			initCharPartsPrio();	// �����?�ѩ`�Ĥ΃����λ�Q���I���γ��ڻ�
			addressBookProc();		// ���ɥ쥹�֥å��I��
			openServerWindowProc();	// ���`�Џ��ƥ�����ɥ�
			drawFieldInfoWin();		// ��`����Έ������
			
			/* ����������ߤ餻�� */
			RunAction();
			// ��������ʾ�ǩ`����Хåե�������
			// ָ������ϥ��ȥå����ʤ�
			StockTaskDispBuffer();

			mapEffectProc();		// �ޥåץ��ե����ȣ���?ѩ�ȣ�

			drawMap();		// �ޥåױ�ʾ
			drawField();	// �ե��`��ɻ���ˤ���������ʾ

			/* ����åȄI�� */
			ChatProc();
			// ����åȥХåե��`��ե���ȥХåե�������
			ChatBufferToFontBuffer(); 
			// ���`�ܩ`�ɥ��`������I��
			
			FlashKeyboardCursor();
			// ��˥�`�I��
			MenuProc();
			// �ɣͣ��v�B�I��
			ImeProc();		
			// �r�g���I���ȥѥ�åȥ����󥸄I��
			TimeZoneProc();
			
#ifdef _DEBUG
			// ���󥫥���ȥܥ���
			if( joy_trg[ 0 ] & JOY_P_DOWN ){
				if( pc.ptAct != NULL ){
					resetMap();					// �ޥåׄI���ꥻ�å�
					// ���󥫥��������
					lssproto_EN_send( sockfd, pc.ptAct->gx, pc.ptAct->gy );
					eventEnemyFlag = 0;
				}
			}
#endif
			// ���󥫥���Ȥ����r  ���е�ʱ��
			if( EncountFlag == TRUE ){
				resetPc();				// PC�ꥻ�å�
				resetCharObj();			// ���������Ʃ`�֥�ꥻ�å�
				resetMap();				// �ޥåׄI���ꥻ�å�
				clearPtActPartyParam();	// ���g���Υ��������ݥ��󥿤�����NULL�ˤ���
				fieldInfoTime = 0;		// ������󤬳��Ƥ�������
				drawFieldInfoWin();		// ��`����Έ������
				nowEncountPercentage = minEncountPercentage;// ���󥫥�����ʤ���С�ˑ���
				sendEnFlag = 0;
				encountNowFlag = 1;
				eventEnemySendFlag = 0;
				duelSendFlag = 0;
				jbSendFlag = 0;
				// �ޥåץ�����ɥ��ե饰�Хå����å�
				if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
				// ������ޤǤΕr�g�����ީ`
				ResultWndTimer = RESULT_WND_CLOSE_TIME;
				InitMenu();			// ��˥�`�v�B�γ��ڻ�
				BattleCmd[ 0 ] = NULL;		// �Хȥ륳�ޥ�ɳ��ڻ�
//				BattleCmdBak[ 0 ] = NULL;	// �Хȥ륳�ޥ�ɳ��ڻ�
//				BattleCmdReadPointer = BattleCmdWritePointer =0;
				BattleStatus[ 0 ] = NULL;	// �Хȥ�״�B���ڻ�
				//BattleStatusBak[ 0 ] = NULL;// �Хȥ�״�B���ڻ�
//				BattleStatusReadPointer = BattleStatusWritePointer =0;
				BattleTurnReceiveFlag = TRUE;	// ���`�����ťե饰�ϣ�
				// ���󥫥������
				play_se( 215, 320, 240 );
				// �F�ڤΣ£ǣͱ���
				now_bgm = t_music_bgm_no;
				// �£ǣ�ֹͣ
				stop_bgm();
				SubProcNo++;
			}
			break;
			
		case 4:	// �Хå��Хåե��`���ݳ��å��`�ե����˥��ԩ`
			// �Хȥ륵�`�ե����λ������� 
			CopyBackBuffer();
			// �Хå��Хåե��`�軭�������
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			SubProcNo++;
			
			break;
			
		case 5:	// ���󥫥�����ݳ�
		
			// �ݳ���
			if( DrawProduce( PRODUCE_HAGARE_OCHI_OUT ) == TRUE ){
			//if( GameState == GAME_ENCOUNT_TO_BATTLE ){ 
				// �ץ�����������
				ChangeProc( PROC_BATTLE );
			}
			MenuProc();	// ��˥�`�I��
			ImeProc();	// �ɣͣ��v�B�I��
			
			break;
	}
}


// ��`���ݳ���
void warpEffectProc( void )
{
	oldMapEffectRainLevel = mapEffectRainLevel;
	oldMapEffectSnowLevel = mapEffectSnowLevel;

	DispBuffer.DispCnt = 0;
	FontCnt = 0;

	// �ޥåץ�����ɥ��ե饰�Хå����å�
	if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
	InitMenu2();

	fieldProc();			// �ե��`����v�B�I��
	charObjProc();			// ��������I��
	initCharPartsPrio();	// �����?�ѩ`�Ĥ΃����λ�Q���I���γ��ڻ�
	openServerWindowProc();	// ���`�Џ��ƥ�����ɥ�

	fieldInfoTime = 0;		// ������󤬳��Ƥ�������
	drawFieldInfoWin();		// ��`����Έ������


	/* ����������ߤ餻�� */
	RunAction();
	// ��������ʾ�ǩ`����Хåե�������
	StockTaskDispBuffer();

	mapEffectProc();		// �ޥåץ��ե����ȣ���?ѩ�ȣ�

	redrawMap();
	drawMap();		// �ޥåױ�ʾ

	/* ����åȄI�� */
	ChatProc();
	// ����åȥХåե��`��ե���ȥХåե�������
	ChatBufferToFontBuffer(); 
	// �r�g���I���ȥѥ�åȥ����󥸄I��
//	TimeZoneProc();

	SortDispBuffer(); 	// ��ʾ�Хåե����`��

	// �Хå����`�ե������\�ǥ��ꥢ�`
	ClearBackSurface();	
	// �£ͣФ�Хå����`�ե����˥��å�
	PutBmp();	


	// �Хå����`�ե�������Хȥ륵�`�ե����إ��ԩ`
	lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );

	// �Хåե����ڻ�
	DispBuffer.DispCnt = 0;
	FontCnt = 0;

	// �F�ڤΕr�g��ӛ��
	NowTime = GetTickCount();

	ProduceInitFlag = TRUE;
}


void paletteProc( void )
{
	if( palNo == -1 )
	{
		// �ѥ�åȥ�����
		PaletteChange( SaTimeZoneNo, 0 );
		// �r�g���ǥѥ�åȥ����󥸤���r
		TimeZonePalChangeFlag = TRUE;
		palNo = -2;
	}
	else
	if( palNo >= 0 )
	{
		// �̶��ѥ�å��O��
		PaletteChange( palNo, 0 );// �ѥ�åȥ�����
		// �r�g���ǥѥ�åȥ����󥸤��ʤ��r
		TimeZonePalChangeFlag = FALSE;
		palNo = -2;
	}
}
