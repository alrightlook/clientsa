// Dwaf Test Process

#include "system.h"
#include "dwafTestProc.h"
#include "ime_sa.h"
#include "menu.h"
#include "pc.h"
#include "character.h"

void dwafTestProc( void )
{
	/* ���֥ץ��������ŤǷ��  �ӽ���/�������*/*/
	switch( SubProcNo ){
	
		case 0:

			InitChat();
			// �����ե��`����ȡ��  ����۽�ȡ��
			GetKeyInputFocus( &MyChatBuffer );
			SubProcNo++;
			break;
		
		case 1:

			drawMap();		// �ޥåױ�ʾ  ��ͼ��ʾ
			
			/* ����åȄI��   ���촦��*/
			ChatProc();
			// ����åȥХåե��`��ե���ȥХåե�������  ���컺�����建����
			ChatBufferToFontBuffer(); 
			// ��˥�`�I��  �˵�����
			MenuProc();
			// �ɣͣ��v�B�I��
			ImeProc();		
			
			break;
	}
}
