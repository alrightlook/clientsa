#include"system.h"
#include"action.h"
#include"pc.h"
#include"character.h"
#include"map.h"
#include"tool.h"
#include"anim_tbl.h"
#include"loadsprbin.h"
#include"netmain.h"
#include"lssproto_cli.h"
#include"savedata.h"
#include"menu.h"
#include"field.h"
#include"login.h"

// �ץ쥤��`�����ǩ`��
PC pc;
short maxPcNo;			// �����ꥹ�Ȥ˵��h����Ƥ륭������
short selectPcNo;		// ������x�k�r���x�k���줿�����ꥹ�Ȥη���
short prSendMode = 0;	// �У����ť�`�� 0 ... ���Ҫ�� / 1 ... ���gҪ��
short prSendFlag = 0;	// �Уҥץ��ȥ��� 1 ... ����

short jbSendFlag;		// �Α�ץ��ȥ�������
short duelSendFlag;		// �ǥ奨��I����
//short duelFlag;

int loginDp;			// pc���ڻ��r�˲��դ���ǥ奨��ݥ����

short helpFlag = 0;		// ���L�ФΥإ�ץܥ����״�B

// �ڥåȥǩ`��
PET pet[MAX_PET];


// ���g�ǩ`��
MAGIC magic[MAX_MAGIC];

// ���g���
PARTY party[MAX_PARTY];
short partyModeFlag = 0;	// �F�ڥѩ`�ƥ���`�ɤ�


// �����ꥹ����
CHARLISTTABLE chartable[MAXCHARACTER];


// ���ɥ쥹�֥å�
ADDRESS_BOOK addressBook[MAX_ADR_BOOK];


// ���L�Y����å��`��
BATTLE_RESULT_MSG battleResultMsg;


// �ڥåȤΥ�����
PET_SKILL petSkill[MAX_PET][MAX_SKILL];


char newCharacterName[CHAR_NAME_LEN+1];	// �¥�������ǰ
int newCharacterGraNo;				// �¥����λ��񷬺�
int newCharacterFaceGraNo;			// �¥�������񷬺�
int newCharacterVit;				// �¥���������
int newCharacterStr;				// �¥���������
int newCharacterTgh;				// �¥������B�ɤ�
int newCharacterDex;				// �¥��������礵
int newCharacterEarth;				// �¥����ε�����
int newCharacterWater;				// �¥�����ˮ����
int newCharacterFire;				// �¥����λ�����
int newCharacterWind;				// �¥������L����
int newCharacterHomeTown;			// �¥����γ�����




void swapInteger( int *, int * );
void swapShort( short *, short * );
void swapString( char *, char * );





// PC������I��Υ��ꥢ
void initPcAll( void )
{
	int i;

	memset( &pc, 0, sizeof( pc ) );
	memset( &pet, 0, sizeof( pet ) );
	memset( &magic, 0, sizeof( magic ) );
	memset( &party, 0, sizeof( party ) );
	memset( &petSkill, 0, sizeof( petSkill ) );

	pc.mailPetNo = -1;

	getUserSetting( selectPcNo );
	
	partyModeFlag = 0;

	for( i = 0; i < MAX_PARTY; i++ )
	{
		party[i].useFlag = 0;
		party[i].id = 0;
		party[i].ptAct = NULL;
	}


#if 1
	pc.faceGraNo = newCharacterFaceGraNo;
	pc.dp = loginDp;
#endif
}


// PC�����ڻ�
void initPc( void )
{
	int walk = 0, height = 0;
	int i;

#ifndef _DEBUG
	createPc( pc.graNo, nowGx, nowGy, pc.dir );
#else
	if( offlineFlag )
	{
		createPc( SPR_pet004, nowGx, nowGy, pc.dir );
	}
	else
	{
		createPc( pc.graNo, nowGx, nowGy, pc.dir );
	}
#endif

	if( (pc.status & CHR_STATUS_W) )
	{
		walk = 1;
	}
	if( (pc.status & CHR_STATUS_H) )
	{
		height = 1;
	}
	setPcParam( pc.name, pc.freeName, pc.level, pc.nameColor, walk, height );

	if( pc.ptAct != NULL )
	{
		pc.ptAct->hp = pc.hp;
		pc.ptAct->maxHp = pc.maxHp;

		// ���g��󤬤��ä��饢�������ݥ��󥿤����
		for( i = 0; i < MAX_PARTY; i++ )
		{
			if( party[i].useFlag != 0 && party[i].id == pc.id )
			{
				party[i].ptAct = pc.ptAct;
				break;
			}
		}
	}


	prSendMode = 0;
	prSendFlag = 0;
	jbSendFlag = 0;
	duelSendFlag = 0;
	helpFlag = 0;
}


// PC���������
void createPc( int graNo, int gx, int gy, int dir )
{
	pc.graNo = graNo;
	pc.dir = dir;
	if( pc.ptAct == NULL )
	{
		pc.ptAct = createCharAction( graNo, gx, gy, dir );
		if( pc.ptAct != NULL )
		{
			pc.ptAct->atr |= ACT_ATR_TYPE_PC;
		}
	}
	else
	{
		setPcGraNo( graNo, dir );
		setPcWarpPoint( gx, gy );
		setPcPoint();
	}
}


// PC�����ꥻ�å�
void resetPc( void )
{
	if( pc.ptAct != NULL )
	{
		DeathAction( pc.ptAct );
		pc.ptAct = NULL;
	}

	// ��`���`�ީ`���������Ƥ���
	delPcLeader();
}


// PC����饰��ե��å������O��
void setPcGraNo( int graNo, int dir )
{
	pc.graNo = graNo;
	pc.dir = dir;

	if( pc.ptAct == NULL )
		return;

	pc.ptAct->anim_chr_no = graNo;
	pc.ptAct->anim_ang = dir;
}


// PC�����ID���O��
void setPcId( int id )
{
	pc.id = id;
}


// PC������Ƅ����O��
void setPcMovePoint( int nextGx, int nextGy )
{
//	if( pc.ptAct == NULL )
//		return;

	setMoveMap( nextGx, nextGy );
}


// PC������`�����O��
void setPcWarpPoint( int gx, int gy )
{
//	if( pc.ptAct == NULL )
//		return;

#if 0
	forceWarpMap( gx, gy );
#endif
	setWarpMap( gx, gy );
}


// PC�����ޥå�λ�ä�ͬ�ڤ���
void setPcPoint( void )
{
	if( pc.ptAct == NULL )
		return;

	pc.ptAct->mx = nowX;
	pc.ptAct->my = nowY;

	pc.ptAct->gx = nowGx;
	pc.ptAct->gy = nowGy;

	pc.ptAct->vx = nowVx;
	pc.ptAct->vy = nowVy;

	pc.ptAct->nextGx = nextGx;
	pc.ptAct->nextGy = nextGy;
}


// PC�������򤭤��O��
void setPcDir( int dir )
{
	pc.dir = dir;

	if( pc.ptAct == NULL )
		return;

	pc.ptAct->anim_ang = dir;
}


// ���˥�`������O��
void setPcAction( int act )
{
	if( pc.ptAct == NULL )
		return;

	pc.ptAct->anim_no = act;
	if( pc.ptAct->anim_no != ANIM_WALK )
	{
		pc.ptAct->anim_no_bak = -1;
	}
}


// ���˥�`������״�Bȡ��
int getPcAction( void )
{
	if( pc.ptAct == NULL )
		return -1;

	return pc.ptAct->anim_no;
}


// PC�����Υѥ��`���O��
void setPcParam( char *name, char *freeName, int level, int nameColor,
	int walk, int height )
{
	int nameLen;
	int freeNameLen;

	nameLen = strlen( name );
	if( nameLen <= CHAR_NAME_LEN )
	{
		strcpy( pc.name, name );
	}

	freeNameLen = strlen( freeName );
	if( freeNameLen <= CHAR_FREENAME_LEN )
	{
		strcpy( pc.freeName, freeName );
	}

	pc.level = level;
	pc.nameColor = nameColor;
	if( walk != 0 )		// �����Ϥ�i����
	{
		pc.status |= CHR_STATUS_W;
	}
	if( height != 0 )	// �ߤ�������
	{
		pc.status |= CHR_STATUS_H;
	}

	if( pc.ptAct == NULL )
		return;

	if( nameLen <= CHAR_NAME_LEN )
	{
		strcpy( pc.ptAct->name, name );
	}
	if( freeNameLen <= CHAR_FREENAME_LEN )
	{
		strcpy( pc.ptAct->freeName, freeName );
	}
	pc.ptAct->level = level;
	
}


// PC���������򥢥��������O�����ʤ���
void updataPcAct( void )
{
	if( pc.ptAct == NULL )
		return;

	if( strlen( pc.name ) <= CHAR_NAME_LEN )
	{
		strcpy( pc.ptAct->name, pc.name );
	}
	if( strlen( pc.freeName ) <= CHAR_FREENAME_LEN )
	{
		strcpy( pc.ptAct->freeName, pc.freeName );
	}
	pc.ptAct->level = pc.level;
	pc.ptAct->hp    = pc.hp;
	pc.ptAct->maxHp = pc.maxHp;
}


// PC�������`���`�ˤ���
void setPcLeader( void )
{
	pc.status |= CHR_STATUS_LEADER;
}


// PC�������`���`�����ᤵ���롣
void delPcLeader( void )
{
	pc.status &= (~CHR_STATUS_LEADER);
}


// PC������ѩ`�ƥ��`��һ�T�ˤ���
void setPcParty( void )
{
	pc.status |= CHR_STATUS_PARTY;
}


// PC������ѩ`�ƥ��`�����ᤵ���롣
void delPcParty( void )
{
	pc.status &= (~CHR_STATUS_PARTY);
}


// PC�������Q���`�ɤˤ���
void setPcWatch( void )
{
	pc.status |= CHR_STATUS_WATCH;
}


// PC�������Q���`�ɤ���Ԫ�ˤ�ɤ�
void delPcWatch( void )
{
	pc.status &= (~CHR_STATUS_WATCH);
}


// �F�ښi���Ƥ�ե饰�����Ƥ�
//���i���������Υ��˥�`�������Ф��椨�r�˲��գ�
void setPcWalkFlag( void )
{
	if( pc.ptAct == NULL )
		return;

	pc.ptAct->walkFlag = 1;
}

// �������˥�ˉ䤨����ե饰�򤪤���
//���i���������Υ��˥�`�������Ф��椨�r�˲��գ�
void delPcWalkFlag( void )
{
	if( pc.ptAct == NULL )
		return;

	pc.ptAct->walkFlag = 0;
}


// �i���ե饰�β���
int checkPcWalkFlag( void )
{
	if( pc.ptAct == NULL )
		return 0;

	return (int)pc.ptAct->walkFlag;
}


// ���gʹ�åե饰�����Ƥ�
void setPcUseMagic( void )
{
	pc.status |= CHR_STATUS_USE_MAGIC;
}


// ���gʹ�åե饰�򤪤���
void delPcUseMagic( void )
{
	pc.status &= (~CHR_STATUS_USE_MAGIC);
}


// PC�˴������������
void setPcFukidashi( unsigned int offTime )
{
	CHAREXTRA *ext;

	if( pc.ptAct == NULL )
		return;

	pc.status |= CHR_STATUS_FUKIDASHI;
	ext = (CHAREXTRA *)pc.ptAct->pYobi;
	ext->drawFukidashiTime = offTime + GetTickCount();
}


// PC����������� /////////////////////////////////////////////////////////
void changePcAct( int x, int y, int dir, int action,
					int effectno, int effectparam1, int effectparam2 )
{
	switch( action )
	{
		// ����״�B
		case 0:
#if 0
#if 1
			setPcWarpPoint( x, y );
			setPcDir( dir );
#else
			// �ѩ`�ƥ����⤫����`���Εr�τI������
			if( partyModeFlag == 0 || (pc.status & CHR_STATUS_LEADER) != 0 )
			{
				setPcWarpPoint( x, y );
				setPcDir( dir );
			}
#endif
#endif
			setPcAction( ANIM_STAND );
			break;

		// ��`��
		case 31:
#if 1
			setPcWarpPoint( x, y );
			setPcDir( dir );
#else
			// �ѩ`�ƥ����⤫����`���Εr�τI������
			if( partyModeFlag == 0 || (pc.status & CHR_STATUS_LEADER) != 0 )
			{
				setPcWarpPoint( x, y );
				setPcDir( dir );
			}
#endif
			setPcAction( ANIM_STAND );
			break;

		// ����ܞ�Q
		case 30:
			setPcDir( dir );
			break;

		// �i��
		case 1:
#if 0
			// �ѩ`�ƥ����⤫����`���Εr�τI������
			if( partyModeFlag == 0 || (pc.status & CHR_STATUS_LEADER) != 0 )
			{
				setPcMovePoint( x, y );
				setPcDir( dir );
			}
#endif
			setPcAction( ANIM_WALK );
			break;

		// ����
		case 2:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_ATTACK );
			break;

		// Ͷ���루�ʤ����⣩
		case 3:
			break;

		// ����`��
		case 4:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_DAMAGE );
			break;

		// ����
		case 5:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_DEAD );
			break;

		// ħ��ʹ��
		case 6:
			break;

		// �����ƥ�ʹ��
		case 7:
			break;

		// ���ե�����
		case 8:		/* UsedMagic */
			break;

		// �����
		case 10:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_DEAD );
			break;

		// ����
		case 11:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_SIT );
			break;

		// �֤����
		case 12:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_HAND );
			break;

		// ϲ��
		case 13:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_HAPPY );
			break;

		// ŭ��
		case 14:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_ANGRY );
			break;

		// ������
		case 15:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_SAD );
			break;

		// ���`��
		case 16:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_GUARD );
			break;

		// �i��������������
		case 17:
			setPcDir( dir );
			setPcAction( ANIM_WALK );
			break;

		// ���ʤ���
		case 18:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_NOD );
			break;


		// ���L���
		case 20:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			break;

		// ��`���`���
		case 21:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			if( effectno == 1 )
			{
				// ��`���`�ީ`����ʾ
				setPcLeader();
#if 0
				if( pc.ptAct != NULL )
				{
					pc.ptAct->partyGx = -1;
					pc.ptAct->partyGy = -1;
				}
#endif
			}
			else
			{
				// ��`���`�ީ`����ȥ
				delPcLeader();
			}
			break;

		// �Q�����
		case 22:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			if( effectno == 1 )
			{
				setPcWatch();
			}
			else
			{
				delPcWatch();
			}
			break;
	}
}


// ���g���Υ��ꥢ /////////////////////////////////////////////////////////
void clearPartyParam( void )
{
	int i;

	for( i = 0; i < MAX_PARTY; i++ )
	{
		// ���ä��ޤǥѩ`�ƥ����ä������Υե饰�򤪤���
		if( party[i].useFlag != 0 )
		{
			if(  party[i].id == pc.id )
			{
				delPcParty();
				delPcLeader();
			}
			else
			{
				delCharParty( party[i].ptAct );
			}
		}
		party[i].useFlag = 0;
		party[i].id = 0;
		party[i].ptAct = NULL;
	}
}


// ���g����ptAct��NULL�����Ƥ��
void clearPtActPartyParam( void )
{
	int i;

	for( i = 0; i < MAX_PARTY; i++ )
	{
		party[i].ptAct = NULL;
	}
}





// �����ꥹ�Ȥ˥ǩ`�����O������Ƥ��뤫�ʖ� /////////////////////////////
int existCharacterListEntry( int index )
{
	if( index < 0 || index >= MAXCHARACTER )
		return -1;

	if( chartable[index].name[0] != '\0' )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


// �����ꥹ�Ȥ�ͬһ�������뤫�����å�
int cmpNameCharacterList( char *name )
{
	int i;

	for( i = 0; i < MAXCHARACTER; i++ )
	{
		if( strcmp( name, chartable[i].name ) == 0 )
			return 1;
	}

	return 0;
}


// �����ꥹ�Ȥإǩ`���O���������z��Хåե���λ�äϥǩ`�����Ф˕����Ƥ��룩
int setCharacterList( char *name, char *opt )
{

	int index;

	makeRecvString( name );
	makeRecvString( opt );

	index = getIntegerToken( opt, '|', 1 );

	if( index < 0 || index >= MAXCHARACTER )
		return -1;

	memset( &chartable[index], 0, sizeof( CHARLISTTABLE ) );

	if( strlen( name ) <= CHAR_NAME_LEN )
	{

		strcpy( chartable[index].name, name );
	}
	else
	{
		strcpy( chartable[index].name, "???" );
	}
	chartable[index].faceGraNo	= getIntegerToken( opt, '|',  2 );
	chartable[index].level		= getIntegerToken( opt, '|',  3 );
	chartable[index].hp			= getIntegerToken( opt, '|',  4 );
	chartable[index].str		= getIntegerToken( opt, '|',  5 );
	chartable[index].def		= getIntegerToken( opt, '|',  6 );
	chartable[index].agi		= getIntegerToken( opt, '|',  7 );
	chartable[index].app		= getIntegerToken( opt, '|',  8 );
	chartable[index].dp			= getIntegerToken( opt, '|',  9 );
	chartable[index].attr[0]	= getIntegerToken( opt, '|', 10 )/10;
	chartable[index].attr[1]	= getIntegerToken( opt, '|', 11 )/10;
	chartable[index].attr[2]	= getIntegerToken( opt, '|', 12 )/10;
	chartable[index].attr[3]	= getIntegerToken( opt, '|', 13 )/10;
	chartable[index].login		= getIntegerToken( opt, '|', 14 );

	return 0;
}


// �����ꥹ�Ȥ����ݤ򤱤���
//   int index : �ɤΥ���餫��0  ~ 7
int resetCharacterList( int index )
{
	if( index < 0 || index >= MAXCHARACTER )
		return -1;

	memset( &chartable[index], 0, sizeof( CHARLISTTABLE ) );

	return 0;
}


// �����ƥ��ʰ�� /////////////////////////////////////////////////////////
void getItem( void )
{
	float tmpDir;
	float tmpX, tmpY;
	int dir;
	static unsigned int piSendTime = 0;

	// �����v�S�Υ�����ɥ����_���Ƥ�r��ʰ���ʤ��褦�ˤ���
	if( windowTypeWN == WINDOW_MESSAGETYPE_ITEMSHOPMENU
	 || windowTypeWN == WINDOW_MWSSAGETYPE_ITEMSHOPMAIN
	 || windowTypeWN == WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN )
		return;

	// �x�k�Ȥ���Ԫ�����O�Ӥ��륰��å�����τI�����ʤ�
	if( ABS( nowGx - mouseMapGx ) > 1
	 || ABS( nowGy - mouseMapGy ) > 1 )
		return;

	// �x�k�Ȥ˺Τ����뤫�{�٤Ƥʤ���ЄI�����ʤ�
	if( !checkCharObjPoint( mouseMapGx, mouseMapGy,
		CHAROBJ_TYPE_NPC|CHAROBJ_TYPE_ITEM|CHAROBJ_TYPE_MONEY ) )
		return;

	// �F�ڤΥޥ����η�����{�٤�
	tmpX = (float)(mouseMapGx - nowGx);
	tmpY = (float)(mouseMapGy - nowGy);
	tmpDir = Atan( tmpX, tmpY ) + 22.5F - 45.0F*3;
	AdjustDir( &tmpDir );
	dir = (int)(tmpDir/45);

	// �B������
	if( piSendTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )
	{
		// ���`�Ф��ͤ�
		lssproto_PI_send( sockfd, nowGx, nowGy, dir );
		piSendTime = GetTickCount();
	}
}


// �����ƥ������椨 /////////////////////////////////////////////////////
void swapItem( int from, int to )
{
	if( from < 0 || to < 0 )
		return;

	swapShort( &pc.item[from].useFlag, &pc.item[to].useFlag );
	swapString( pc.item[from].name, pc.item[to].name );
	swapString( pc.item[from].name2, pc.item[to].name2 );
	swapString( pc.item[from].memo, pc.item[to].memo );
	swapInteger( &pc.item[from].color, &pc.item[to].color );
	swapInteger( &pc.item[from].graNo, &pc.item[to].graNo );
	swapInteger( &pc.item[from].level, &pc.item[to].level );
	swapShort( &pc.item[from].field, &pc.item[to].field );
	swapShort( &pc.item[from].target, &pc.item[to].target );
	swapShort( &pc.item[from].deadTargetFlag, &pc.item[to].deadTargetFlag );
	swapShort( &pc.item[from].sendFlag, &pc.item[to].sendFlag );
}


// ����������줫
void swapInteger( int *a, int *b )
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}


void swapShort( short *a, short *b )
{
	short tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}


// �����Ф�����椨����������255���֤ޤǤ������У�
void swapString( char *a, char *b )
{
	char tmp[256];

	if( strlen( a ) > 255 || strlen( b ) > 255 )
		return;

	strcpy( tmp, a );
	strcpy( a, b );
	strcpy( b, tmp );
}


// �O�Ӥ��륰��åɤ�Ҋ�� /////////////////////////////////////////////////
BOOL lookAtAround( void )
{
	float tmpDir;
	float tmpX, tmpY;
	int dir;
	static unsigned int lSendTime = 0;

	// �����v�S�Υ�����ɥ����_���Ƥ�r��Ҋ�ʤ��褦�ˤ���
	if( windowTypeWN == WINDOW_MESSAGETYPE_ITEMSHOPMENU
	 || windowTypeWN == WINDOW_MWSSAGETYPE_ITEMSHOPMAIN
	 || windowTypeWN == WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN )
		return FALSE;

	// �x�k�Ȥ���Ԫ�����O�Ӥ��륰��å�����τI�����ʤ�
	if( ABS( nowGx - mouseMapGx ) > 1
	 || ABS( nowGy - mouseMapGy ) > 1 )
		return FALSE;

	// �x�k�Ȥ˺Τ����뤫�{�٤Ƥʤ���ЄI�����ʤ�
	if( !checkCharObjPoint( mouseMapGx, mouseMapGy, CHAROBJ_TYPE_LOOKAT ) )
		return FALSE;

	// �F�ڤΥޥ����η�����{�٤�
	tmpX = (float)(mouseMapGx - nowGx);
	tmpY = (float)(mouseMapGy - nowGy);
	if( tmpX == 0 && tmpY == 0 )	// ��ʾ����Ϥ��Է֤��󥯥�å�������ֹ���
		return FALSE;

	tmpDir = Atan( tmpX, tmpY ) + 22.5F - 45.0F*3;
	AdjustDir( &tmpDir );
	dir = (int)(tmpDir/45);

	// �B������
	if( lSendTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )
	{
		// ���`�Ф��ͤ�
		lssproto_L_send( sockfd, dir );
		lSendTime = GetTickCount();
	}

	return TRUE;
}





// ���ɥ쥹�֥å��I�� /////////////////////////////////////////////////////
BOOL addressBookFlag = FALSE;
ACTION *ptActAddressBookWin = NULL;
int addressBookX = 16;
int addressBookY = 16;
int addressBookPage = 0;
ACTION *ptActAddressBookChar[MAX_ADR_BOOK_COUNT];

// �x�k�Ǥ��������ФΣɣ�
int addressBookSelectStrItem[1+3*MAX_ADR_BOOK_COUNT];
// 0 ... �]����
// n*3+1 ... ����
// n*3+2 ... ����
// n*3+3 ... ����


BOOL addressBookSendMsgFlag = FALSE;
ACTION *ptActAddressBookSendMsgWin = NULL;


void addressBookWindow1( void );


// ���ɥ쥹�֥å��γ��ڻ�
void initAddressBook( void )
{
	int i;

	addressBookFlag = FALSE;
	ptActAddressBookWin = NULL;
	memset( &ptActAddressBookChar, 0, sizeof( ptActAddressBookChar ) );
	addressBookPage = 0;
	for( i = 0; i < sizeof( addressBookSelectStrItem )/sizeof( int ); i++ )
	{
		addressBookSelectStrItem[i] = -2;
	}

	addressBookSendMsgFlag = FALSE;
	ptActAddressBookSendMsgWin = NULL;
}


// ���ɥ쥹�֥å��I��
void addressBookProc( void )
{
	int i;
	int no;

	// ���ɥ쥹�֥å����_��
	if( addressBookFlag
	 && ptActAddressBookWin == NULL )
	{
		// �٩`��������ɥ��Υ��������
		ptActAddressBookWin = MakeWindowDisp( addressBookX, addressBookY, 4, 8, NULL, 1 );
		// ���֥����Υ��������
		for( i = 0; i < MAX_ADR_BOOK_COUNT; i++ )
		{
			no = addressBookPage*MAX_ADR_BOOK_COUNT+i;

			ptActAddressBookChar[i] = GetAction( PRIO_CHR, 0 );
			if( ptActAddressBookChar[i] != NULL )
			{
				ptActAddressBookChar[i]->anim_chr_no = addressBook[no].graNo;
				ptActAddressBookChar[i]->atr = ACT_ATR_HIDE;
				ptActAddressBookChar[i]->anim_no = ANIM_STAND;
				ptActAddressBookChar[i]->dispPrio = DISP_PRIO_ITEM;
				ptActAddressBookChar[i]->x = addressBookX + 40;
				ptActAddressBookChar[i]->y = addressBookY + i * 110 + 80;
				pattern( ptActAddressBookChar[i], ANM_NOMAL_SPD, ANM_LOOP );
			}
		}
	}
	// ���ɥ쥹�֥å����]����
	else
	if( !addressBookFlag
	 && ptActAddressBookWin != NULL )
	{
		DeathAction( ptActAddressBookWin );
		ptActAddressBookWin = NULL;
		for( i = 0; i < MAX_ADR_BOOK_COUNT; i++ )
		{
			if( ptActAddressBookChar[i] != NULL )
			{
				DeathAction( ptActAddressBookChar[i] );
				ptActAddressBookChar[i] = NULL;
			}
		}
	}
	// ���ɥ쥹�֥å���ʾ
	else
	if( addressBookFlag
	 && ptActAddressBookWin != NULL )
	{
		addressBookWindow1();
	}
}


// ���ɥ쥹�֥å���ʾ
void addressBookWindow1( void )
{
	int mode = 0;
	int i;
	int no;
	char msg[256];

	// �����Ф��x�k����Ƥʤ��������å�
	// �ޤ��󥯥�å�����Ƥʤ���ЄI�����ʤ�
	if( mouse.onceState & MOUSE_LEFT_CRICK )
	{
		for( i = 0; i < sizeof( addressBookSelectStrItem )/sizeof( int ); i++ )
		{
			if( addressBookSelectStrItem[i] == HitFontNo )
			{
				mode = i + 1;
				break;
			}
		}
	}

	// �]����ܥ����Ѻ���줿�ΤǽK���
	if( mode == 1 )
	{
		addressBookFlag = FALSE;
		return;
	}
	// �����ܥ����Ѻ���줿�ΤǤ����Ŀ����������
	else
	if( (mode-2) < MAX_ADR_BOOK_COUNT*3
	 && ((mode-2) % 3) == 2 )
	{
		lssproto_DAB_send( sockfd, addressBookPage*MAX_ADR_BOOK_COUNT+((mode-2)/3) );
	}

	// ������ɥ��_�����˥᤬�K��ä����������ʾ
	if( ptActAddressBookWin->hp >= 1 )
	{
		for( i = 0; i < MAX_ADR_BOOK_COUNT; i++ )
		{
			no = addressBookPage*MAX_ADR_BOOK_COUNT+i;

			if( ptActAddressBookChar[i] != NULL )
			{
				// ��ǰ���������˥ǩ`������äƤ���Ȥ���
				if( strlen( addressBook[no].name ) > 0 )
				{
					// ����ե��å����Ť���r������ʾ
					// ������ʾ
					ptActAddressBookChar[i]->atr &= (~ACT_ATR_HIDE);
					ptActAddressBookChar[i]->anim_chr_no = addressBook[no].graNo;
					// ��ǰ��ʾ
					sprintf( msg, "NAME. %s", addressBook[no].name );
					StockFontBuffer( addressBookX + 68, addressBookY + i * 110 + 28,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0 );
				
					// ��٥��ʾ
					sprintf( msg, "LV. %d", addressBook[no].level );
					StockFontBuffer( addressBookX + 68, addressBookY + i * 110 + 46,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0 );
					// ����饤���ʾ
					if( addressBook[i].onlineFlag != 0 )
					{
						StockFontBuffer( addressBookX + 130, addressBookY + i * 110 + 46,
							FONT_PRIO_FRONT, FONT_PAL_WHITE, "����饤��", 0 );
					}
					else
					{
						StockFontBuffer( addressBookX + 130, addressBookY + i * 110 + 46,
							FONT_PRIO_FRONT, FONT_PAL_GRAY, "���ե饤��", 0 );
					}
					// ���ťܥ����ʾ
					addressBookSelectStrItem[i*3+1] =
						StockFontBuffer( addressBookX + 68, addressBookY + i * 110 + 64,
							FONT_PRIO_FRONT, FONT_PAL_YELLOW, "����", 2 );
					// ���ťܥ����ʾ
					addressBookSelectStrItem[i*3+2] =
						StockFontBuffer( addressBookX + 112, addressBookY + i * 110 + 64,
							FONT_PRIO_FRONT, FONT_PAL_YELLOW, "����", 2 );
					// �����ܥ����ʾ
					addressBookSelectStrItem[i*3+3] =
						StockFontBuffer( addressBookX + 156, addressBookY + i * 110 + 64,
							FONT_PRIO_FRONT, FONT_PAL_YELLOW, "����", 2 );
				}
				else
				{
					ptActAddressBookChar[i]->atr |= ACT_ATR_HIDE;
					ptActAddressBookChar[i]->anim_chr_no = 0;
				}
			}
		}

		addressBookSelectStrItem[0] =
			StockFontBuffer( addressBookX + 56, addressBookY + 352,
				FONT_PRIO_FRONT, FONT_PAL_YELLOW, "�]����", 2 );
	}
}
