/************************/
/*	process.h			*/
/************************/
#ifndef _PROCESS_H_
#define _PROCESS_H_

/* �ץ������ηN� */
enum{
	PROC_INIT,
	PROC_ID_PASSWORD,				// �ɣġ��ѥ���`�������I��
	PROC_TITLE_MENU,				// �����ȥ뻭���˥�`��ʾ
	PROC_CHAR_SELECT,				// ������x�k
	PROC_CHAR_MAKE,					// ���������
	PROC_CHAR_LOGIN_START,			// �������������_ʼ
	PROC_CHAR_LOGIN,				// ��������������
	PROC_CHAR_LOGOUT,				// ��������������
	PROC_OPENNING,
	PROC_GAME,						// ���`��ץ�����
	PROC_BATTLE,					// �Хȥ�ץ�����
	PROC_DISCONNECT_SERVER,			// �ͥåȥ�`�����жϤ��줿�r�΄I��

#ifdef _DEBUG		

	PROC_TAKE_TEST,					// ���ڥƥ���
	PROC_OHTA_TEST,					// ̫��ƥ���
	PROC_DWAF_TEST,					// �ɴ�ƥ���
	PROC_SPR_VIEW,					// ���ץ饤�ȴ_�J�ץ�����
	PROC_ANIM_VIEW,					// ���˥�`�����_�J�ץ�����
	PROC_SE_TEST,					// �ӣŴ_�J�ץ�����
	
#endif
	PROC_ENDING
	
};

/* �ץ��������� */
extern UINT ProcNo;
/* ���֥ץ��������� */
extern UINT SubProcNo;

extern int palNo;
extern int oldPalNo;


/* �ץ������I�� */
void Process( void );

/* �ץ���������I�� */
void ChangeProc( int procNo );
/* �ץ���������I�� */
void ChangeProc( int procNo, int subProcNo );

/* ���`��ץ������I�� */
void GameProc( void );

// ��`���ݳ���
void warpEffectProc( void );

void paletteProc( void );

#ifdef _DEBUG		

/* ���ڤ���ƥ��ȥץ����� */
void TakeTestProc( void );

/* ̫��ƥ��ȥץ����� */
void OhtaTestProc( void );

#endif

#endif
