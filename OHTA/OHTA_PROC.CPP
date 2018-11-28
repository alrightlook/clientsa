/************************/
/*	ohta_proc.cpp		*/
/************************/
#include "system.h"
#include "ohta.h"
#include "ime_sa.h"
#include "map.h"
#include "menu.h"

/* ̫��ץ������I�� ********************************************************************/
void OhtaTestProc( void )
{
	/* ���֥ץ��������ŤǷ�� */
	switch( SubProcNo ){
	
		case 0:
		
			/* �ԙC ���������� */
			MakeJiki();
			SubProcNo++;
			/* ����å��v�B���ڻ� */
			InitChat();
			// �����ե��`����ȡ��
			GetKeyInputFocus( &MyChatBuffer );
			
			break;
		
		case 1:
			// ���`�ܩ`�ɥ��`������I��
			FlashKeyboardCursor();
			
			// �ޥ�����٥뤬��˥�`���¤Εr
			if( mouse.level < DISP_PRIO_MENU && TaskBarFlag == FALSE ){
				drawGrid();		// ����åɥ��`�����ʾ
				moveProc();		// �ƄӄI��
			}
			
			/* ����������ߤ餻�� */
			RunAction();
			// ��������ʾ�ǩ`����Хåե�������
			StockTaskDispBuffer();
			
			drawMap();	// �ޥåױ�ʾ

			/* ����åȄI�� */
			ChatProc();
			// ����åȥХåե��`��ե���ȥХåե�������
			ChatBufferToFontBuffer(); 
			// ��˥�`�I��
			MenuProc();
			// �ɣͣ��v�B�I��
			ImeProc();		
			// �ѥ�åȄI��
			PaletteProc();
			
			break;
	}
}

