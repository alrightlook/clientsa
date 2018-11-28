#ifndef _LOGIN_H_
#define _LOGIN_H_


enum
{
	WINDOW_MESSAGETYPE_MESSAGE,					// ֻ����Ϣ  ���b�Z�[�W�̂�
	WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,		// ��Ϣ      ���b�Z�[�W�ƈ�s����
	WINDOW_MESSAGETYPE_SELECT,					// ѡ����    �I���E�B���h�E
	WINDOW_MESSAGETYPE_PETSELECT,				// ����       �y�b�g�I���E�B���h�E
	WINDOW_MESSAGETYPE_PARTYSELECT,				// ���壿��   ���ԑI���E�B���h�E
	WINDOW_MESSAGETYPE_PETANDPARTYSELECT,		// �����     �y�b�g�C���ԑI���E�B���h�E
	WINDOW_MESSAGETYPE_ITEMSHOPMENU,			// �˵� ��Ʒ�̵�   ���X�̃��j���[�E�C���h�E
	WINDOW_MWSSAGETYPE_ITEMSHOPMAIN,			// �����  ��ǰ�̵�  ���X�̃��C���E�C���h�E
	WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN,		// �̵�����е�רҵ��Ŀ����Ҫ���������� ���������A�C�e�����̃��C���E�C���h�E
	WINDOW_MESSAGETYPE_PETSKILLSHOP,			// ���ڵ���＼��  �y�b�g�̋Z������E�C���h�E
	WINDOW_MESSAGETYPE_WIDEMESSAGE,				// /ֻ����Ϣ���ڣ���/  ���b�Z�[�W�̂݁i�傫�����j
	WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT	// ��·�������Ϣ���� ���b�Z�[�W�ƈ�s���́i�傫�����j
};

#define	WINDOW_BUTTONTYPE_NONE		(0)
#define	WINDOW_BUTTONTYPE_OK		(1 << 0)
#define	WINDOW_BUTTONTYPE_CANCEL	(1 << 1)
#define	WINDOW_BUTTONTYPE_YES		(1 << 2)
#define	WINDOW_BUTTONTYPE_NO		(1 << 3)
#define	WINDOW_BUTTONTYPE_PRE		(1 << 4)
#define	WINDOW_BUTTONTYPE_NEXT		(1 << 5)

extern char selectServerName[][64];
extern char selectServerName2[][64];

extern BOOL logOutFlag;

extern int windowTypeWN;
extern short wnCloseFlag;


void idPasswordProc( void );

void titleProc( void );
void selectCharacterProc( void );
void characterLoginProc( void );

void makeCharacterProc( void );

void characterLogoutProc( void );


void initOpenServerWindow( void );
void openServerWindow( int, int, int, int, char * );
void openServerWindowProc( void );

int selGraId( int *, int );
int selRepGraId( int *, int );
int pushGraId( int *, int );
int selFontId( int *, int );
int focusGraId( int *, int );
int focusFontId( int *, int );

#endif