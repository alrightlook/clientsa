/************************/
/*	action.c			*/
/************************/
#include "system.h"



/* �Ϥ������Υꥹ�ȥݥ��� */
ACTION *pActTop;
ACTION *pActBtm;

#ifdef _DEBUG		
/* ������������������ */
int ActCnt;
#endif

/* ���������ꥹ�ȳ��ڻ� *****�����������ڻ�************************************************/
void InitAction( void )
{
	/* ���������Υݥ���ȡ�� */
	pActTop = ( ACTION * )calloc( 1, sizeof( ACTION ) );
	pActBtm = ( ACTION * )calloc( 1, sizeof( ACTION ) );

	pActTop->pPrev = NULL;	 	/* ǰ�Υꥹ�Ȥϟo�� */
	pActTop->pNext = pActBtm;	/* �ΤΥꥹ�Ȥ�����β */
	pActTop->func  = NULL;		/* �g���v���ϟo�� */
	pActTop->prio  = PRIO_TOP;	/* �����λ��� */
	pActTop->bmpNo = -1;		/* �£ͣз��ų��ڻ� */
	
	pActBtm->pPrev = pActTop;	/* ǰ�Υꥹ�Ȥ����^ */
	pActBtm->pNext = NULL;		/* �ΤΥꥹ�Ȥϟo�� */
	pActBtm->func  = NULL;		/* �g���v���ϟo�� */
	pActBtm->prio  = PRIO_BTM;	/* �����λ��� */
	pActBtm->bmpNo = -1;		/* �£ͣз��ų��ڻ� */
	
	//MessageBox( hWnd, "�����������ڻ����ˡ�", "�_�J", MB_OK );
}

/* ���������ꥹ�Ȥ˵��h *****************************************************/
ACTION *GetAction( UCHAR prio, UINT yobiSize )
{
	ACTION *pAct;
	ACTION *pActLoop;
	
	/* ����������������_�� */
	pAct = ( ACTION * )calloc( 1, sizeof( ACTION ) );
	if( pAct == NULL ){
//#ifdef _DEBUG		
		//MessageBox( hWnd, "GetAction Erorr!!", "�_�J", MB_OK );
		MessageBox( hWnd, "�ϩ`�ɥǥ������οդ����������ޤ���", "GetAction Erorr", MB_OK );
//#endif
		return NULL;
	}
	/* ��䘋����ʹ���r */
	if( yobiSize > 0 ){
		/* ��䘋�������_�� */
		pAct->pYobi = calloc( 1, yobiSize );
		// ʧ��������
		if( pAct->pYobi == NULL ){
			/* �������������Υ������ */
			free( pAct );
//#ifdef _DEBUG		
			//MessageBox( hWnd, "GetYobi Erorr!!", "�_�J", MB_OK );
			MessageBox( hWnd, "�ϩ`�ɥǥ������οդ����������ޤ���", "GetYobi Erorr", MB_OK );
//#endif
			return NULL;
		}
	}
	
	/* ���ڻ� */
	pAct->func  = NULL;			/* �g���v���ϟo�� */
	pAct->prio  = prio;			/* �����λ */
	pAct->bmpNo = -1;			/* �£ͣз��ų��ڻ� */
	pAct->hitDispNo = -2;		/* �ޥ������`���뵱�����ж����ų��ڻ� */
	
	/* ����혤ˁK�٤� */
	for( pActLoop = pActTop->pNext ; pActLoop != pActBtm->pNext ; 
		pActLoop = pActLoop->pNext ){
		
		/* ���ȶȤ��ͤ��r */
		if( pActLoop->prio > prio ){
			/* �����Ϥ�׷�� */
			/* ǰ�ᤫ��̤��Ƥ�餦 */
			pAct->pPrev = pActLoop->pPrev;	/* ǰ�Υꥹ�� */
			pAct->pNext = pActLoop;			/* �ΤΥꥹ�� */
			/* ǰ��˽̤��� */
			pActLoop->pPrev->pNext = pAct;	/* ǰ�Υꥹ�� */
			pActLoop->pPrev = pAct;			/* �ΤΥꥹ�� */
#ifdef _DEBUG		
			/* ������������������ */
   			ActCnt++;
#endif			
			break;
		}
	}
	
	return pAct;
}

/* ����������ߤ餻�� ******��������***************************************************/
void RunAction( void )
{
	ACTION *pActLoop = pActTop->pNext; 	/* ���^�Υꥹ�ȥݥ���ȡ��  ��ǰͷ������ָ��ȡ��*/
	ACTION *pActLoopBak;  /* �����r�Хå����å��� */
	
	// �I����`��	
	while(1){
	
		/* ����β������ޤǥ�`��  �����֮ǰѭ��*/	
		if( pActLoop == pActBtm ) break;

		/* �����Ƥ����� ������ŵĻ�*/
		if( pActLoop->deathFlag == FALSE )
		{
		
			/* ���ɥ쥹�����ä���g��   ��ַ�Ļ���ִ��*/
			if( pActLoop->func != NULL )
			{
				pActLoop->func( pActLoop );
			}
			
			/* �ΤΥݥ��󥿤򥻥å� */
			pActLoop = pActLoop->pNext;
			
		}
		else
		{	/* ����Ǥ����� */
		
			/* ǰ��˽̤��� */
			pActLoop->pPrev->pNext = pActLoop->pNext;
			pActLoop->pNext->pPrev = pActLoop->pPrev;
			/* �Хå����å� */
			pActLoopBak = pActLoop->pNext;
			/* �ꥹ�Ȥ���Ĩ�� */
			ClearAction( pActLoop );
			/* �Хå����åב��� */
			pActLoop = pActLoopBak;
		}
	}
}

/* ���������ꥹ�Ȥ���Ĩ���ʂ� ***********************************************/
void DeathAction( ACTION *pAct )
{
	if( pAct == NULL ) return;
	pAct->deathFlag = TRUE;
}

/* ���������ꥹ�Ȥ�����ȫĨ�� ***********************************************/
void ClearAction( ACTION *pAct )
{
	// ��䘋���夬����Ȥ�
	if( pAct->pYobi != NULL ){
		// ��䘋������_��
		free( pAct->pYobi );
	}
	// ���������������_��
	free( pAct );
	
#ifdef _DEBUG		
	/* ������������ޥ��ʥ� */
	ActCnt--;
#endif		

}

/* ȫ���������Ĩ�� ***********************************************************/
void DeathAllAction( void )
{
	ACTION *pActLoop = pActTop->pNext;
	
	/* ����ޤǤ�����K�� */
	while( pActLoop != pActBtm ){
		/* �����ե饰���Ƥ� */
		pActLoop->deathFlag = TRUE;
		/* �ΤΥݥ��󥿤� */
		pActLoop = pActLoop->pNext;
	}
	//MessageBox( hWnd, "DeathAllAction OK!!", "�_�J", MB_OK );
}

/* ���������K�˄I�� *********************************************************/
void EndAction( void )
{
	// ȫ���������Ĩ��
	DeathAllAction();
	//����������ߤ餻�루��ȫĨ����
	RunAction();
	
	/* ���������Υݥ����_�� */
	free( pActTop );
	free( pActBtm );
	
	//MessageBox( NULL, "����������_�����ˡ�", "�_�J", MB_OK );
}