/************************/
/*	mouse.c				*/
/************************/
#include "system.h"
#include "loadrealbin.h"
#include "anim_tbl.h"
#include "pc.h"
#include "menu.h"
#include "battleMenu.h"

// �ޥ����ε������ж�������
#define MOUSE_HIT_SIZE_X 48
#define MOUSE_HIT_SIZE_Y 48

// �ޥ����Υ��`�ȥ�ԩ`�Ȥ��g��
#define MOUSE_AUTO_REPEATE_TIME 100

// �ޥ���������
MOUSE mouse;
// �ޥ������`����˵����ä��ե���ȷ��ţ��ե�����ã�
int	HitFontNo;
// �ޥ������`����˵����ä���ʾ���ţ��£ͣ��ã�
int HitDispNo;
// һ�Х���ե�������
char OneLineInfoStr[ 256 ];

// ��ʾ�ܥå�����ɫ
int BoxColor;	


// �ޥ������γ��ڻ� ////////////////////////////////////////////////////////
void MouseInit( void )
{
	memset( &mouse, 0, sizeof( MOUSE ) );
	mouse.itemNo = -1; // �����ƥ෬�ų��ڻ�
}

// �F�ڤΥޥ�����λ�ä�ӛ������ //////////////////////////////////////////////
void MouseNowPoint( int x, int y )
{
	// ���ˤ򥻥å�
	//���������������ɥåȥ�`�ɤΕr
	if(ResoMode == 1){
		mouse.nowPoint.x = x * 2;
		mouse.nowPoint.y = y * 2;
	} else {
		mouse.nowPoint.x = x;
		mouse.nowPoint.y = y;
	}
}

// �ޥ�������ܥ���Ѻ���줿�r�Υޥ�����λ�ä�ӛ������ //////////////////////
void MouseCrickLeftDownPoint( int x, int y )
{
	// ״�B�򥻥å�
	mouse.state |= MOUSE_LEFT_CRICK;
	mouse.onceState |= MOUSE_LEFT_CRICK;
	mouse.autoState |= MOUSE_LEFT_CRICK;
	// ���ˤ򥻥å�
	mouse.crickLeftDownPoint.x = x;
	mouse.crickLeftDownPoint.y = y;
	// �ܥ���Ѻ����Ƥ���νU�^�r�g���O������ǰ�ʂ�
	mouse.beforeLeftPushTime = GetTickCount();
	mouse.leftPushTime = 0;
}

// �ޥ�������ܥ����x���줿�r�Υޥ�����λ�ä�ӛ������ ////////////////////////
void MouseCrickLeftUpPoint( int x, int y )
{
	// ״�B�򥻥å�
	mouse.state &= (~MOUSE_LEFT_CRICK);
	mouse.onceState |= MOUSE_LEFT_CRICK_UP;
	mouse.autoState &= (~MOUSE_LEFT_CRICK);
	// ���ˤ򥻥å�
	mouse.crickLeftUpPoint.x = x;
	mouse.crickLeftUpPoint.y = y;
	// �ܥ����x���줿��r�g��ꥻ�å�
	mouse.beforeLeftPushTime = 0;
	mouse.leftPushTime = 0;
}

// ����֥륯��å����줿�r�Υޥ�����λ�ä�ӛ������ //////////////////////////
void MouseDblCrickLeftUpPoint( int x, int y )
{
	// ״�B�򥻥å�
	//mouse.onceState |= ( MOUSE_LEFT_DBL_CRICK | MOUSE_LEFT_CRICK );
	mouse.state |= MOUSE_LEFT_CRICK;
	mouse.onceState |= MOUSE_LEFT_DBL_CRICK;
	mouse.onceState |= MOUSE_LEFT_CRICK;
	mouse.autoState |= MOUSE_LEFT_CRICK;
	// ���ˤ򥻥å�
	mouse.crickLeftDownPoint.x = x;
	mouse.crickLeftDownPoint.y = y;
	// �ܥ���Ѻ����Ƥ���νU�^�r�g���O������ǰ�ʂ�
	mouse.beforeLeftPushTime = GetTickCount();
	mouse.leftPushTime = 0;
}

// �ޥ������ҥܥ���Ѻ���줿�r�Υޥ�����λ�ä�ӛ������ ////////////////////////
void MouseCrickRightDownPoint( int x, int y )
{
	// ״�B�򥻥å�
	mouse.state |= MOUSE_RIGHT_CRICK;
	mouse.onceState |= MOUSE_RIGHT_CRICK;
	mouse.autoState |= MOUSE_RIGHT_CRICK;
	// ���ˤ򥻥å�
	mouse.crickRightDownPoint.x = x;
	mouse.crickRightDownPoint.y = y;
	// �ܥ���Ѻ����Ƥ���νU�^�r�g���O������ǰ�ʂ�
	mouse.beforeRightPushTime = GetTickCount();
	mouse.rightPushTime = 0;
}

// �ޥ������ҥܥ����x���줿�r�Υޥ�����λ�ä�ӛ������ ////////////////////////
void MouseCrickRightUpPoint( int x, int y )
{
	// ״�B�򥻥å�
	mouse.state &= (~MOUSE_RIGHT_CRICK);
	mouse.onceState |= MOUSE_RIGHT_CRICK_UP;
	mouse.autoState &= (~MOUSE_RIGHT_CRICK);
	// ���ˤ򥻥å�
	mouse.crickRightUpPoint.x = x;
	mouse.crickRightUpPoint.y = y;
	// �ܥ����x���줿��r�g��ꥻ�å�
	mouse.beforeRightPushTime = 0;
	mouse.rightPushTime = 0;
}

// �ҥ��֥륯��å����줿�r�Υޥ�����λ�ä�ӛ������ //////////////////////////
void MouseDblCrickRightUpPoint( int x, int y )
{
	// ״�B�򥻥å�
	mouse.state |= MOUSE_RIGHT_CRICK;
	mouse.onceState |= MOUSE_RIGHT_DBL_CRICK;
	mouse.onceState |= MOUSE_RIGHT_CRICK;
	mouse.autoState |= MOUSE_RIGHT_CRICK;
	// ���ˤ򥻥å�
	mouse.crickRightDownPoint.x = x;
	mouse.crickRightDownPoint.y = y;
	// �ܥ���Ѻ����Ƥ���νU�^�r�g���O������ǰ�ʂ�
	mouse.beforeRightPushTime = GetTickCount();
	mouse.rightPushTime = 0;
}

// �ޥ����I����///////////////////////////////////////////////////////////////
void MouseProc( void )
{
	static UINT leftPushTimeBak;
	static UINT rightPushTimeBak;
	
	// ״�B����ڻ�
	mouse.onceState = MOUSE_NO_CRICK;
	// ��ܥ����Ѻ���Ƥ���νU�^�r�g�򥻥å�
	if( mouse.beforeLeftPushTime > 0 )
	{
		mouse.leftPushTime = GetTickCount() - mouse.beforeLeftPushTime;
	}
	// �ҥܥ����Ѻ���Ƥ���νU�^�r�g�򥻥å�
	if( mouse.beforeRightPushTime > 0 )
	{
		mouse.rightPushTime = GetTickCount() - mouse.beforeRightPushTime;
	}
	// ���`�ȥ�ԩ`�ȄI��
	// �󥯥�å�Ѻ����Ƥ���r
	// ��������ǥ�ԩ`���_ʼ
	if( mouse.leftPushTime > 500 ){
		// �����һ�ؤΕr
		if( leftPushTimeBak == 0 ){
			mouse.autoState |= MOUSE_LEFT_CRICK; // �ӥåȣϣ�
			leftPushTimeBak = mouse.leftPushTime;	// �r�g��ӛ��
		}else
		// �r�g���������R�귵��
		if( mouse.leftPushTime - leftPushTimeBak >= MOUSE_AUTO_REPEATE_TIME ){
			mouse.autoState |= MOUSE_LEFT_CRICK; // �ӥåȣϣ�
			leftPushTimeBak = mouse.leftPushTime;	// �r�g��ӛ��
		}else{	// �����ˣϣƣƤˤ���
			mouse.autoState &= (~MOUSE_LEFT_CRICK);	// �ӥåȣϣƣ�
		}
	}else{ 
		mouse.autoState &= (~MOUSE_LEFT_CRICK);	// �ӥåȣϣƣ�
		leftPushTimeBak = 0;
	}
	
	// �󥯥�å�Ѻ����Ƥ���r
	// ��������ǥ�ԩ`���_ʼ
	if( mouse.rightPushTime > 500 ){
		// �����һ�ؤΕr
		if( rightPushTimeBak == 0 ){
			mouse.autoState |= MOUSE_RIGHT_CRICK; // �ӥåȣϣ�
			rightPushTimeBak = mouse.rightPushTime;	// �r�g��ӛ��
		}else
		// �r�g���������R�귵��
		if( mouse.rightPushTime - rightPushTimeBak >= MOUSE_AUTO_REPEATE_TIME ){
			mouse.autoState |= MOUSE_RIGHT_CRICK; // �ӥåȣϣ�
			rightPushTimeBak = mouse.rightPushTime;	// �r�g��ӛ��
		}else{	// �����ˣϣƣƤˤ���
			mouse.autoState &= (~MOUSE_RIGHT_CRICK);	// �ӥåȣϣƣ�
		}
	}else{ 
		mouse.autoState &= (~MOUSE_RIGHT_CRICK);	// �ӥåȣϣƣ�
		rightPushTimeBak = 0;
	}
	
	
	
	// �ҥ���å�Ѻ����Ƥ���r
	//if( mouse.rightPushTime > 0 ){
	//}else mouse.state &= (~MOUSE_RIGHT_AUTO_CRICK);
	// ������ɥ���`�ɤǤʤ��r
	//if( !WindowMode ){
	//if( 0 <= mouse.nowPoint.x && mouse.nowPoint.x < 640 &&
	//	0 <= mouse.nowPoint.y && mouse.nowPoint.y < 480 ){
	
#ifndef _DEBUG
	if( mouse.flag == FALSE ){ 
		// �ޥ������`����α�ʾ����ȱ�ʾ��
		StockDispBuffer( mouse.nowPoint.x + 16, mouse.nowPoint.y + 16, DISP_PRIO_MOUSE, CG_MOUSE_CURSOR, 0 );
	}
#endif
		
}
// ����`���x�k�����å� *********************************************************/
void CheckGroupSelect( int no )
{
	int i;
	DISP_INFO 	*pDispInfo;
	DISP_SORT 	*pDispSort;
	/* �£ͣб�ʾ�Хåե������������ʾ�΃����λ���ߤ������飩 */
	//for( i = DispBuffer.DispCnt - 1; i >= 0 ; i--, pDispSort-- ){
	for( i = 0 ; i < DispBuffer.DispCnt ; i++ ){
	
		//pDispInfo = DispBuffer.DispInfo[ i ];
		//pDispSort = DispBuffer.DispSort[ i ];
		pDispInfo = DispBuffer.DispInfo + i;
		pDispSort = DispBuffer.DispSort + i;
		
		// �ե饰�����äƤ�����
		if( pDispInfo->hitFlag == no ){
			// ��˥�`����Ϥ��ä���
			if( pDispSort->dispPrio >= DISP_PRIO_MENU ){
				// �ܥå�����ʾ�ǩ`����Хåե�������
				StockBoxDispBuffer( pDispInfo->x - 2, pDispInfo->y - 2, 
									pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width + 2, 
									pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height + 2, 
									DISP_PRIO_BOX2, BoxColor, 0 );
			}else{
				// �ܥå�����ʾ�ǩ`����Хåե�������
				StockBoxDispBuffer( pDispInfo->x - 2, pDispInfo->y - 2, 
									pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width + 2, 
									pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height + 2, 
									DISP_PRIO_BOX, BoxColor, 0 );
									//pDispSort->dispPrio, 250, 0 );
			}
		}
	}
}

#if 0
// ��ʾ�ܥå����Υ���`�Ʃ`�֥�
int BoxColorTbl[] = { 	
						97,97,97,97,97,
						98,98,98,98,98,
						99,99,99,99,99,
						100,100,100,100,100,
						101,101,101,101,101,
						102,102,102,102,102,
						103,103,103,103,103,
						104,104,104,104,104,
						105,105,105,105,105,
						106,106,106,106,106,
						107,107,107,107,107,
						//108,108,108,108,108,
						//109,109,109,109,109,
						//110,110,110,110,110,
						//111,111,111,111,111,
						//110,110,110,110,110,
						//109,109,109,109,109,
						//108,108,108,108,108,
						//107,107,107,107,107,
						106,106,106,106,106,
						105,105,105,105,105,
						104,104,104,104,104,
						103,103,103,103,103,
						102,102,102,102,102,
						101,101,101,101,101,
						100,100,100,100,100,
						99,99,99,99,99,
						98,98,98,98,98,
						};
#endif
#if 1
// ��ʾ�ܥå����Υ���`�Ʃ`�֥�
UCHAR BoxColorTbl[] = { 	
						//255, 255, 255, 255, 255,
						//8,8,8,8,8,
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						15,15,15,15,15,
						2,2,2,2,2, 
						15,15,15,15,15, 
						//250,250,250,250,250, 
						//8,8,8,8,8,
					};
#else
UCHAR BoxColorTbl[] = { 	
						255,255,255,255,255,255,255,255,255,255,
						255,255,255,255,255,255,255,255,255,255,
						7,7,7,7,7,7,7,7,7,7,
						248,248,248,248,248,248,248,248,248,248,
						0,0,0,0,0,0,0,0,0,0,
						248,248,248,248,248,248,248,248,248,248,
						7,7,7,7,7,7,7,7,7,7,
					};
#endif

/* �ޥ������`����Τ������ж� **************************************************/
void HitMouseCursor( void )
{	
	int i;					// ��`�ץ�����
	int strWidth;			// �����ФΥɥå���
	int hitFlag = FALSE;	// �������ж��ե饰
	static int cnt = 0;		// �����󥿩`
	int itemNameColor = FONT_PAL_WHITE;	// �����ƥ��ɫ
	
	DISP_SORT 	*pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt - 1;
	DISP_INFO 	*pDispInfo;
	//char moji[ 256 ];
	
	// ��ʾ�ܥå�����ɫ���
	if( cnt >= sizeof( BoxColorTbl ) - 1 ) cnt = 0;
	else cnt++;
	
	// ��ʾ�ܥå�����ɫ�O��
	BoxColor = BoxColorTbl[ cnt ];
	
	
	/* �ե���ȱ�ʾ�Хåե�������������`�Ȥ���ʤ��ΤǤɤ�����Ҋ�Ƥ�ͬ����*/
	for( i = 0 ; i < FontCnt ; i++ ){
	
		// �������ж����ʤ��r����
		if( FontBuffer[ i ].hitFlag == 0 ) continue;
		
		// �����Фκ�����ɥå��������{�٤�
		strWidth = GetStrWidth( FontBuffer[ i ].str );
		// �ĽǤΤ������ж�
		if( mouse.nowPoint.x <= FontBuffer[ i ].x + strWidth + 2 &&
			FontBuffer[ i ].x - 2 <= mouse.nowPoint.x &&
			mouse.nowPoint.y <= FontBuffer[ i ].y + FONT_SIZE  + 2 &&
			FontBuffer[ i ].y - 2 <= mouse.nowPoint.y ){
			
			// �����ä����Фη��Ť�ҙ����
			HitFontNo = i;
			
			// �ܥå������ʾ����r
			if( FontBuffer[ i ].hitFlag == 2 ){
				// �ܥå�����ʾ�ǩ`����Хåե�������
				StockBoxDispBuffer( FontBuffer[ i ].x - 3, FontBuffer[ i ].y - 4, 
									FontBuffer[ i ].x + strWidth + 2, 
									FontBuffer[ i ].y + FONT_SIZE + 4, 
									//DISP_PRIO_BOX2, 250, 0 );
									DISP_PRIO_BOX2, BoxColor, 0 );
									
				SortDispBuffer(); 	// ��ʾ�Хåե����`��
			}					
			// �����ä����Фη��Ť���ڻ�����
			HitDispNo = -1;
			// �ޥ�����٥�
			mouse.level = DISP_PRIO_MENU;
			return;		// �ե���Ȥ˵����ä��r�£ͣФ��ж��Ϥ��ʤ�
		}
	}
	// �����ä����Фη��Ť���ڻ�����
	HitFontNo = -1;
	
	/* �£ͣб�ʾ�Хåե������������ʾ�΃����λ���ߤ������飩 */
	for( i = DispBuffer.DispCnt - 1; i >= 0 ; i--, pDispSort-- ){
		// ��ʾ�ǩ`������äƤ��륢�ɥ쥹�˥��å�
		pDispInfo = DispBuffer.DispInfo + pDispSort->no;
		
		// �������ж����ʤ��r����
		if( pDispInfo->hitFlag == 0 ) continue;
		
		// ���֤Σ£ͣФ��ʤ��ä���@�A
		//if( SpriteInfo[ pDispInfo->bmpNo ].lpSurfaceInfo == NULL ) continue;
		// �֣ң��ͤˤ��ʤ��Ȥ��ϥϩ`�ɥǥ�����������`�ɤ���
		// �£ͣФ���`�ɤ���
		if( LoadBmp( pDispInfo->bmpNo ) == FALSE ) continue;
		
		// ���������Ǥʤ��r
		if( pDispInfo->pAct == NULL ){
			// �ĽǤΤ������ж�
			if( mouse.nowPoint.x <= pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width &&
				pDispInfo->x <= mouse.nowPoint.x &&
				mouse.nowPoint.y <= pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height &&
				pDispInfo->y <= mouse.nowPoint.y )
				
				hitFlag = TRUE;
		}else{
			// �ĽǤΤ������ж�������������ã�
			if( mouse.nowPoint.x <= pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width * 0.5 + MOUSE_HIT_SIZE_X * 0.5 &&
				pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width * 0.5 - MOUSE_HIT_SIZE_X * 0.5  <= mouse.nowPoint.x &&
				mouse.nowPoint.y <= pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height &&
				pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height - MOUSE_HIT_SIZE_Y <= mouse.nowPoint.y )
				
				hitFlag = TRUE;
		}
				
		// �����äƤ�����
		if( hitFlag == TRUE ){
			// ���ֱ�ʾ�Хåե�������
//			StockFontBuffer( pDispInfo->x, pDispInfo->y, FONT_PRIO_FRONT, 0, "Hit", 1 );
			
			// �����ä����Фη��Ť�ӛ��
			HitDispNo = pDispSort->no;
			
			// �ܥå������ʾ����r
			if( pDispInfo->hitFlag >= 2 ){
				// ��˥�`����Ϥ��ä���
				if( pDispSort->dispPrio >= DISP_PRIO_YES_NO_WND ){
					// �ܥå�����ʾ�ǩ`����Хåե�������
					StockBoxDispBuffer( pDispInfo->x - 2, pDispInfo->y - 2, 
										pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width + 2, 
										pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height + 2, 
										//DISP_PRIO_BOX2, 250, 0 );
										DISP_PRIO_BOX3, BoxColor, 0 );
				}else
				// ��˥�`����Ϥ��ä���
				if( pDispSort->dispPrio >= DISP_PRIO_MENU ){
					// �ܥå�����ʾ�ǩ`����Хåե�������
					StockBoxDispBuffer( pDispInfo->x - 2, pDispInfo->y - 2, 
										pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width + 2, 
										pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height + 2, 
										//DISP_PRIO_BOX2, 250, 0 );
										DISP_PRIO_BOX2, BoxColor, 0 );
				}else{
					// �ܥå�����ʾ�ǩ`����Хåե�������
					StockBoxDispBuffer( pDispInfo->x - 2, pDispInfo->y - 2, 
										pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width + 2, 
										pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height + 2, 
										//DISP_PRIO_BOX, 250, 0 );
										DISP_PRIO_BOX, BoxColor, 0 );
										//pDispSort->dispPrio, 250, 0 );
				}
				
				// ����`���x�k�����å����}���x�k�� 
				if( pDispInfo->hitFlag >= 3 ) CheckGroupSelect( pDispInfo->hitFlag );
				
				SortDispBuffer(); 	// ��ʾ�Хåե����`��
			}
			
			// �������Щ`���ʤ��r
			if( TaskBarFlag == FALSE ){
				// ���������ݥ��󥿤�����r
				if( pDispInfo->pAct != NULL ){
					// һ�Х���ե���ʾ�ϣΤΕr
					if( pDispInfo->pAct->atr & ACT_ATR_INFO ){
						// �ե��`����ϤΕr
						if( ProcNo == PROC_GAME ){
							// ����������
							//sprintf( OneLineInfoStr,"%s���ȼ���%d��������%4d/%4d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
							// һ�Х���ե���ʾ�ϣΤΕr
							// �ץ쥤��`�Εr
							if( pDispInfo->pAct->atr & ACT_ATR_TYPE_PC ){
								// �ƺŤ���r
								if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
									sprintf( OneLineInfoStr,"%s [%s] �ȼ���%d��������%d��%d��������%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp );
								}else{
									sprintf( OneLineInfoStr,"%s���ȼ���%d��������%d��%d��������%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp );
								}
							}else
							// ���Υץ쥤��`�Εr
							if( pDispInfo->pAct->atr & ACT_ATR_TYPE_OTHER_PC ){
								// �ƺŤ���r
								if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
									sprintf( OneLineInfoStr,"%s [%s]���ȼ���%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level );
								}else{
									sprintf( OneLineInfoStr,"%s���ȼ���%d", pDispInfo->pAct->name, pDispInfo->pAct->level );
								}
							}else
							// �ڥåȤΕr
							if( pDispInfo->pAct->atr & ACT_ATR_TYPE_PET ){
								// �ƺŤ���r
								if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
									sprintf( OneLineInfoStr,"%s���ȼ���%d", pDispInfo->pAct->freeName, pDispInfo->pAct->level );
								}else{
									sprintf( OneLineInfoStr,"%s���ȼ���%d", pDispInfo->pAct->name, pDispInfo->pAct->level );
								}
							}else
							// �����ƥ�Εr
							if( pDispInfo->pAct->atr & ACT_ATR_TYPE_ITEM ){
								// ��ǰ��ɫ�O��
								itemNameColor = pDispInfo->pAct->itemNameColor;
								sprintf( OneLineInfoStr,"%s", pDispInfo->pAct->name );
							}else
							// ����Εr
							if( pDispInfo->pAct->atr & ACT_ATR_TYPE_GOLD ){
								sprintf( OneLineInfoStr,"%s", pDispInfo->pAct->name );
							}else
							// �������Εr���ɥ�����ˡ����壩
							if( pDispInfo->pAct->atr & ACT_ATR_TYPE_OTHER ){
								sprintf( OneLineInfoStr,"%s", pDispInfo->pAct->name );
							}
						}else
						// ���L�Εr
						if( ProcNo == PROC_BATTLE ){
							// �Q���`�ɤ���ʤ��Εr
							if( BattleMyNo < 20 ){
								// �Է֤ʤ�ȣФȣͣб�ʾ
								if( ( (ATR_EQU *)pDispInfo->pAct->pYobi )->place_no == BattleMyNo ){
									// �ƺŤ���r
									if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
										//sprintf( OneLineInfoStr,"%s [%s]���ȼ���%d��������%d��%d��������%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, BattleMyMp );
										sprintf( OneLineInfoStr,"%s [%s]���ȼ���%d��������%d��%d��������%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pDispInfo->pAct->mp );
									}else{
										//sprintf( OneLineInfoStr,"%s���ȼ���%d��������%d��%d��������%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, BattleMyMp );
										sprintf( OneLineInfoStr,"%s���ȼ���%d��������%d��%d��������%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pDispInfo->pAct->mp );
									}
								}
								else
								// ζ���ʤ�ȣб�ʾ
								if( ( (ATR_EQU *)p_party[ BattleMyNo ]->pYobi )->group_flg == ( (ATR_EQU *)pDispInfo->pAct->pYobi )->group_flg ){
									// ����������
									//sprintf( OneLineInfoStr,"%s���ȼ���%d��������%4d/%4d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
									// �ƺŤ���r
									if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
										sprintf( OneLineInfoStr,"%s [%s]���ȼ���%d��������%d��%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
									}else{
										sprintf( OneLineInfoStr,"%s���ȼ���%d��������%d��%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
									}
								}else{
									// �ƺŤ���r
									if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
										sprintf( OneLineInfoStr,"%s [%s]���ȼ���%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level );
									}else{
										sprintf( OneLineInfoStr,"%s���ȼ���%d", pDispInfo->pAct->name, pDispInfo->pAct->level );
									}
								}
							}else{	// �Q���`�ɤΕr
								// �ƺŤ���r
								if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
									//sprintf( OneLineInfoStr,"%s [%s]���ȼ���%d��������%d��%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
									sprintf( OneLineInfoStr,"%s [%s]���ȼ���%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level );
								}else{
									//sprintf( OneLineInfoStr,"%s���ȼ���%d��������%d��%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
									sprintf( OneLineInfoStr,"%s���ȼ���%d", pDispInfo->pAct->name, pDispInfo->pAct->level );
								}
							}
						}
					}
				}
				// һ�Х���ե���ʾ
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, itemNameColor, OneLineInfoStr, 0 );
			}
			// �����г��ڻ�
			OneLineInfoStr[ 0 ] = NULL;
			
			// �ޥ�����٥�
			mouse.level = pDispSort->dispPrio;
			return;
		}
	}
	// �ޥ�����٥�
	mouse.level = DISP_PRIO_TILE;
	// �����ä����Фη��Ť���ڻ�����
	HitDispNo = -1;
	
	// �������Щ`���ʤ��r
	if( TaskBarFlag == FALSE ){
		// һ�Х���ե���ʾ
		StockFontBuffer( 8, 460, FONT_PRIO_FRONT, itemNameColor, OneLineInfoStr, 0 );
	}
	// �����г��ڻ�
	OneLineInfoStr[ 0 ] = NULL;
	
}

