#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include"action.h"
#include"pc.h"

enum
{
	CHR_STATUS_P 		= 0x0001,			// ��
	CHR_STATUS_N 		= 0x0002,			// ��w
	CHR_STATUS_Q 		= 0x0004,			// ���a
	CHR_STATUS_S 		= 0x0008,			// ʯ��
	CHR_STATUS_D 		= 0x0010,			// ���
	CHR_STATUS_C 		= 0x0020,			// ����
	CHR_STATUS_W 		= 0x0040,			// �����Ϥ�i���뤫
	CHR_STATUS_H 		= 0x0080,			// �ߤ������뤫
	CHR_STATUS_LEADER	= 0x0100,			// ��`���`
	CHR_STATUS_PARTY	= 0x0200,			// �ѩ`�ƥ���һ�T
	CHR_STATUS_BATTLE	= 0x0400,			// ���L��
	CHR_STATUS_USE_MAGIC= 0x0800,			// ���gʹ��
	CHR_STATUS_HELP		= 0x1000,			// �إ��
	CHR_STATUS_FUKIDASHI= 0x2000,			// ��������
	CHR_STATUS_WATCH	= 0x4000			// �Q����
};


enum
{											// ��������r�ηN�
	CHAROBJ_TYPE_NPC		= 0x0001,		// NPC
	CHAROBJ_TYPE_ITEM		= 0x0002,		// �����ƥ�
	CHAROBJ_TYPE_MONEY		= 0x0004,		// ����
	CHAROBJ_TYPE_USER_NPC	= 0x0008,		// ���ˤΥ���饯��
	CHAROBJ_TYPE_LOOKAT		= 0x0010,		// Ҋ���
	CHAROBJ_TYPE_ALL		= 0x00FF		// ȫ��
};


// ���`���ϤǤι�������
enum
{
	CHAR_TYPENONE,			// �ΤǤ�ʤ�
	CHAR_TYPEPLAYER,		// �ץ쥤��`
	CHAR_TYPEENEMY,			// ��
	CHAR_TYPEPET,			// �ڥå�
	CHAR_TYPEDOOR,			// �ɥ�
	CHAR_TYPEBOX,			// ����
	CHAR_TYPEMSG,			// ����
	CHAR_TYPEWARP,			// ��`�ץ��`��
	CHAR_TYPESHOP,			// ��
	CHAR_TYPEHEALER,		// �ҩ`��`
	CHAR_TYPEOLDMAN,		// �L��
	CHAR_TYPEROOMADMIN,		// ���Ӯb��
	CHAR_TYPETOWNPEOPLE,	// �ޤ��ΤҤ�
	CHAR_TYPEDENGON,		// ���԰�
	CHAR_TYPEADM,			// ���԰�
	CHAR_TYPETEMPLE,		// Temple master
	CHAR_TYPESTORYTELLER,	// �Z�겿
	CHAR_TYPERANKING,		// ���Ӯb��󥭥󥰱�ʾ�ΣУ�
	CHAR_TYPEOTHERNPC,		// �������Η�������ˤʤ�ʤ�NPC
	CHAR_TYPEPRINTPASSMAN,	// �ɥ��Υѥ���`�ɱ�ʾ����NPC
	CHAR_TYPENPCENEMY,		// �̶���
	CHAR_TYPEACTION,		// ���������˷��ꤹ��NPC
	CHAR_TYPEWINDOWMAN,		// ������ɥ���ʾ����NPC���ƥ��Ȥ���)
	CHAR_TYPESAVEPOINT,		// ���`�֥ݥ����
	CHAR_TYPEWINDOWHEALER,	// ������ɥ������פΥҩ`��`
	CHAR_TYPEITEMSHOP,		// ����
	CHAR_TYPESTONESHOP,		// ʯ�P�ݣ��ڥåȤμ��ݣ�
	CHAR_TYPEDUELRANKING,	// DUEL��󥭥�NPC
	CHAR_TYPENUM
};


typedef struct
{
	short use;				// ʹ���Ф�
	short type;				// �N�
	int id;					// ���`�Фι�������
	int graNo;				// ����ե��å�����
	int gx, gy;				// �ޥåפ�����
	int level;				// ��٥�
	int dir;				// ��
	int stockDir;			// �Ƅ�����򤭉��������Љ������
	int stockDirX, stockDirY;// stockDir���m�ä�������
	short nameColor;		// ��ǰ��ʾ�r��ɫ
	unsigned short status;	// ���Ʃ`����
	int classNo;			// �����ƥ��ã�����
	int money;				// ������
	char name[CHAR_NAME_LEN+1];			// ��ǰ
	char freeName[CHAR_FREENAME_LEN+1];	// ��`�����������ƺ�
	char info[64];			// ���Х���ե���`�����ACTION����줿�餤��ʤ����⣩
	int battleNo;			// ���L����
	short sideNo;			// ���L�r�ˤɤ��饵���ɤ������Ƥ��뤫
	short helpMode;			// �إ�ץ�`��
	int charType;			// �����������O����������
	short newFoundFlag;		// �°kҊ�ι֪���������å����뤫�Υե饰
	ACTION *ptAct;
} CHAROBJ;


typedef struct
{
	int charObjTblId;			// �����Ʃ`�֥�κη�Ŀ����äƤ��뤫
	ACTION *ptActLeaderMark;	// ��`���`�ީ`����ʾ��
	ACTION *ptActMagicEffect;	// ���g���ե�����
	unsigned int drawFukidashiTime;	// ��������������r�g
} CHAREXTRA;


typedef struct
{
	short mode;					// �ڥåȤ΄�����`��
	short moveDir;				// �Ƅӕr���򤫤�����
	short preDir;				// �ҤȤ�ǰ�������ä���
	short dirCnt;				// �B�A�������ä�����
	int preGx, preGy;			// �ҤȤ�ǰ�Υ���å�
	int walkCnt;				// �ƄӚi��
	unsigned int createTime;	// ���ɕr�g
	ACTION *ptAct;
	ACTION *ptAct2;
	float angle;
	int r;
	int id;						// ���`�Фι�������
} PETEXTRA;


void charMove( ACTION * );
void charMove2( ACTION * );
void _charMove( ACTION * );
void shiftBufCount( ACTION * );

ACTION *createCharAction( int, int, int, int );
void stockCharMovePoint( ACTION *, int, int );
void correctCharMovePoint( ACTION *, int, int );
void setCharMovePoint( ACTION *, int, int );
void _setCharMovePoint( ACTION *, int, int );
void setCharWarpPoint( ACTION *, int, int );
void setCharStatus( unsigned short *, char * );

ACTION *createCommmonEffectAction( int, int, int, int, int, int );
ACTION *createCommmonEffectNoLoop( int, int, int, int, int, int );
void drawCharStatus( ACTION * );

void changeCharAct( ACTION *, int, int, int, int, int, int, int );

void setCharLeader( ACTION * );
void delCharLeader( ACTION * );
void setCharParty( ACTION * );
void delCharParty( ACTION * );
void setCharWatch( ACTION * );
void delCharWatch( ACTION * );
void setCharBattle( ACTION *, int, short, short );
void delCharBattle( ACTION * );
void setCharUseMagic( ACTION * );
void delCharUseMagic( ACTION * );

void setCharFukidashi( ACTION *, unsigned int );


void charObjProc( void );
BOOL checkCharObjPoint( int, int, short );
int checkCharObjPointStatus( int, int, short, unsigned short );
int checkCharObjPointNotStatus( int, int, short, unsigned short );
void setNpcCharObj( int, int, int, int, int, char *, char *, int, int, int, int, int );
BOOL setReturnPetObj( int, int, int, int, int, char *, char *, int, int, int, int, int );
void setItemCharObj( int, int, int, int, int, int, char * );
void setMoneyCharObj( int, int, int, int, int, int, char * );
int getAtrCharType( int );
int getAtrCharObjType( int );
void delCharObj( int );
void initCharObj( void );
void resetCharObj( void );
void clearPtActCharObj( void );
void restorePtActCharObjAll( void );
void restorePtActCharObj( int );
void setMovePointCharObj( int, int, int );
ACTION *getCharObjAct( int );

int getCharObjBuf( void );

void initItemOverlapCheck( void );
BOOL itemOverlapCheck( unsigned int, int, int );

ACTION *createPetAction( int, int, int, int, int, int, int );
void ajustClientDir( short * );

#endif
