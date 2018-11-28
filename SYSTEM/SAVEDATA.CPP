#include <stdio.h>
#include <string.h>

//#include "util.h"
#include "tool.h"

#include "pc.h"
#include "t_music.h"
#include "chat.h"

#include "savedata.h"



// ���`�֥ǩ`�� //
//
// ����ȥ�����
// 		SAVEDATA_VER		���`�֥ǩ`���ΥЩ`��??(4 byte)
// 		CDKEY				CD���`(12 byte)
// 		PASSWORD			�ѥ�?�`��(12 byte)
// 		BA_SEL_PEN			���L?������椨���x�k�Ǥ���ڥåȤ�
//               			���L�˲μӤ���ڥåȤ��O��
// 							Bit 0 ... ���L?������椨����
// 							Bit 1 ... ���L�˲μӤ�����
//							Bit 2 ... ?�`?�\��Ǥ�
// 		STEREO_FLAG_SIZE	����?��??��??���Ф��椨
// 		SE_VOL				�ӣť�??�`?
// 		BGM_VOL				�£ǣͥ�??�`?
// 		BGM_PITCH			�£ǣͤΥԥå����
// 		CHAT_COLOR			��?�å���?ɫ
// 		CHAT_LINE			��?�å���?
//		CHAT_AREA_SIZE		��?�åȤ������빠��

// ������
enum
{
	SAVEDATA_VER_SIZE	= 2,
	CDKEY_SIZE			= 12,
	PASSWORD_SIZE		= 12,
	BA_SEL_PEN_SIZE		= 1,

	STEREO_FLAG_SIZE	= 1,
	SE_VOL_SIZE			= 1,
	BGM_VOL_SIZE		= 1,
	BGM_PITCH_SIZE		= 16,

	CHAT_COLOR_SIZE		= 1,
	CHAT_LINE_SIZE		= 1,
	CHAT_AREA_SIZE		= 1
};
// ����
enum
{
	SAVEDATA_VER		= 0,					
	CDKEY				= SAVEDATA_VER + SAVEDATA_VER_SIZE,
	PASSWORD			= CDKEY + CDKEY_SIZE,
	BA_SEL_PEN			= PASSWORD + PASSWORD_SIZE,

	STEREO_FLAG			= BA_SEL_PEN + BA_SEL_PEN_SIZE * MAX_PET * MAXCHARACTER,
	SE_VOL				= STEREO_FLAG + STEREO_FLAG_SIZE,
	BGM_VOL				= SE_VOL + SE_VOL_SIZE,
	BGM_PITCH			= BGM_VOL + BGM_VOL_SIZE,

	CHAT_COLOR			= BGM_PITCH + BGM_PITCH_SIZE,
	CHAT_LINE			= CHAT_COLOR + CHAT_COLOR_SIZE,
	CHAT_AREA			= CHAT_LINE + CHAT_LINE_SIZE,

	SAVEDATA_SIZE		= 127 /*CHAT_LINE + CHAT_LINE_SIZE*/
};

static unsigned char savedatabuf[SAVEDATA_SIZE];

// ���`�֥ǩ`���Щ`��??
//   0x0001 ...  26+1 byte
//   0x0002 ...  87+1 byte
//   0x0003 ...  57+1 byte
//   0x0004 ...  57+1 byte
//   0x0005 ... 127+1 byte
#define SAVEDATA_VERSION	0x0005

// ���`�֥ե���?��
#define SAVEFILE_NAME	"data\\savedata.dat"

// ��?�`���`��
unsigned char savedataErrorCode;
// loadNowState��?�`���`��
//  1 ... ���`�֥ե���?�����ɤǤ��ʤ���
//  2 ... ���`�֥ե���?���_���ʤ���
//  3 ... ���`�֥ǩ`�����i��?�ߤ�ʧ����
//  4 ... ���`�֥ǩ`���Υ��������`����������Ƥ룩
//  5 ... ���`�֥ǩ`���ΥЩ`��??�`����


BOOL createSaveFile( void );


// ���`�֥ǩ`���ե���? ///////////////////////////////////////////////////

//
// ?�`���O������
//
BOOL saveUserSetting( void )
{
	setUserSetting( selectPcNo );
	setUserSoundOption();
	setUserChatOption();

	return saveNowState();
}


//
// ?�`���O���i��?��
//
BOOL loadUserSetting( void )
{
	if( loadNowState() )
	{
		getUserSoundOption();
		getUserChatOption();

		return TRUE;
	}

	return FALSE;
}

//
// ���`�֥ǩ`������
//
BOOL saveNowState( void )
{
	FILE *fp;
	char writebuffer[4000];
	int writebufferlen;
	unsigned char tmpsavedatabuf[SAVEDATA_SIZE];
	int i;

	for( i = 0; i < SAVEDATA_SIZE; i++ )
	{
		tmpsavedatabuf[i] = savedatabuf[i];
	}

	// ���`�֥ǩ`���ΰ�?��
	jEncode( (char *)tmpsavedatabuf, SAVEDATA_SIZE, 0,
		writebuffer, &writebufferlen, sizeof( writebuffer ) );

	// ���`�֥ǩ`������
	if( (fp = fopen( SAVEFILE_NAME, "wb+" )) == NULL )
	{
		return FALSE;
	}
	if( fwrite( writebuffer, 1, writebufferlen, fp ) < (unsigned int)writebufferlen )
	{
		return FALSE;
	}
	fclose( fp );

	return TRUE;
}



//
// ���`�֥ǩ`���i��?�ߣ�����?��1�Ȥ�����
//
BOOL loadNowState( void )
{
	FILE *fp;
	char readbuffer[4000];
	int readbufferlen;
	int tmpsavedatalen;

	savedataErrorCode = 0;

	// ���`�֥ե���?���_��
	if( (fp = fopen( SAVEFILE_NAME, "rb+" )) == NULL )
	{
		// �_���ʤ�?�ϥե���?���o���Ȥߤʤ���Ҏ�����ɤ��롣
		if( createSaveFile() == FALSE )
		{
			savedataErrorCode = 1;
			return FALSE;
		}

		// �Ĥ�ƥ��`�֥ե���?���_��
		if( (fp = fopen( SAVEFILE_NAME, "rb+" ))==NULL )
		{
			// ���ä��ե���?���_���ʤ�
			savedataErrorCode = 2;
			return FALSE;
		}
	}

	// ���`�֥ǩ`���i��?��
	readbufferlen = fread( readbuffer, 1, sizeof( readbuffer ), fp );
	if( ferror( fp ) )
	{
		// �i��?�ߥ�?�`
		savedataErrorCode = 3;
		fclose( fp );
		return FALSE;
	}
	fclose( fp );

	jDecode( readbuffer, readbufferlen, 0, (char *)savedatabuf, &tmpsavedatalen );

#if 1
	// ?�Щ`��??�Υ��`�֥ե���?���Τ�Τˉ�Q����
	if( tmpsavedatalen == 57
	 && *((unsigned short *)savedatabuf+SAVEDATA_VER) == 0x0004 )
	{
		savedatabuf[SAVEDATA_VER] = SAVEDATA_VERSION;
		savedatabuf[CHAT_AREA] = 3;
		saveNowState();
		return TRUE;
	}
#endif

	if( tmpsavedatalen != SAVEDATA_SIZE )
	{
		// �ǩ`����������������
		savedataErrorCode = 4;
		return FALSE;
	}

	// ���`�֥ǩ`���Щ`��??�Υ����å�
	if( *((unsigned short *)savedatabuf+SAVEDATA_VER) != SAVEDATA_VERSION )
	{
		// �Щ`��??�`��
		savedataErrorCode = 5;
		return FALSE;
	}

	return TRUE;
}



// ��Ҏ���`�֥ե���?����
BOOL createSaveFile( void )
{
	FILE *fp;
	char writebuffer[4000];
	int writebufferlen;
	int i;

	// ���`�֥ǩ`���Хåե���?��
	memset( savedatabuf, 0, SAVEDATA_SIZE );

	// ���`�֥ǩ`���Щ`��??�����Ƥ���
	*((unsigned short *)savedatabuf+SAVEDATA_VER) = SAVEDATA_VERSION;

	// ?�ڂ����O��
	savedatabuf[STEREO_FLAG]	= (unsigned char)stereo_flg;
	savedatabuf[SE_VOL]			= (unsigned char)t_music_se_volume;
	savedatabuf[BGM_VOL]		= (unsigned char)t_music_bgm_volume;
	for( i = 0; i < 16; i++ )
		savedatabuf[BGM_PITCH+i] = t_music_bgm_pitch[i];

	savedatabuf[CHAT_LINE]		= 10;
	savedatabuf[CHAT_AREA]		= DEF_VOICE;


	// ���`�֥ǩ`���ΰ�?��
	jEncode( (char *)savedatabuf, SAVEDATA_SIZE, 0,
		writebuffer, &writebufferlen, sizeof( writebuffer ) );

	if( (fp = fopen( SAVEFILE_NAME, "wb+" )) == NULL )
	{
		return FALSE;
	}
	if( fwrite( writebuffer, 1, writebufferlen, fp ) < (unsigned int)writebufferlen )
	{
		return FALSE;
	}

	fclose( fp );

	return TRUE;
}



// CDKEY //////////////////////////////////////////////////////////////////

// CDKEY�򱣴��åХåե����ä�
void setCdkey( char *cdkey )
{
	int i;

	if( cdkey == NULL )
		return;

	for( i = 0; i < CDKEY_SIZE; i++ )
	{
		savedatabuf[CDKEY+i] = cdkey[i];
		if( cdkey[i] == '\0' )
			break;
	}
}


// CDKEY�򱣴��åХåե�����ȡ��
char *getCdkey( char *cdkey )
{
	int i;

	if( cdkey == NULL )
		return NULL;

	for( i = 0; i < CDKEY_SIZE; i++ )
	{
		cdkey[i] = savedatabuf[CDKEY+i];
		if( cdkey[i] == '\0' )
			break;
	}
	cdkey[i] = '\0';	// ��Τ��᤿���� cdkey�Хåե���CDKEY_SIZE+1�Х��ȱ�Ҫ

	return cdkey;
}



// �ѥ�?�`�� /////////////////////////////////////////////////////////////

// �ѥ�?�`�ɤ򱣴��åХåե�������
void setPassword( char *pass )
{
	int i;

	if( pass == NULL )
		return;

	for( i = 0; i < PASSWORD_SIZE; i++ )
	{
		savedatabuf[PASSWORD+i] = pass[i];
		if( pass[i] == '\0' )
			break;
	}
}


// �ѥ�?�`�ɤ򱣴��åХåե�����ȡ��
char *getPassword( char *pass )
{
	int i;

	if( pass == NULL )
		return NULL;

	for( i = 0; i < PASSWORD_SIZE; i++ )
	{
		pass[i] = savedatabuf[PASSWORD+i];
		if( pass[i] == '\0' )
			break;
	}
	pass[i] = '\0';	//  ��Τ��᤿����pass�Хåե���PASSWORD_SIZE+1�Х��ȱ�Ҫ

	return pass;
}


// ?�`���O�� /////////////////////////////////////////////////////////////

// ?�`���O���򱣴�Хåե�������
void setUserSetting( int no )
{
	int i;

	if( no < 0 || MAXCHARACTER <= no )
		return;

	// ���L�μӥڥåȥ�?��
	// ���L�μ��x�k�ڥåȥ�?��
	for( i = 0; i < MAX_PET; i++ )
	{
		if( pc.selectPetNo[i] != 0 )
		{
			savedatabuf[BA_SEL_PEN+MAX_PET*no+i] = 0x01;
		}
		else
		{
			savedatabuf[BA_SEL_PEN+MAX_PET*no+i] = 0x00;
		}
#if 0	// ���`�Ф����?��Τǥ��`�֤��ʤ�
		if( i == pc.battlePetNo )
		{
			savedatabuf[BA_SEL_PEN+MAX_PET*no+i] |= 0x02;
		}
#endif
		// ?�`?�줱�Ƥ��?��
		if( i == pc.mailPetNo )
		{
			savedatabuf[BA_SEL_PEN+MAX_PET*no+i] |= 0x04;
		}
	}
}


// ?�`���O���򱣴�Хåե�����ȡ��
void getUserSetting( int no )
{
	int i;

	if( no < 0 || MAXCHARACTER <= no )
		return;

	// ���L�μӥڥåȥ�?��
	// ���L�μ��x�k�ڥåȥ�?��
	pc.battlePetNo = -1;
	for( i = 0; i < MAX_PET; i++ )
	{
		if( (savedatabuf[BA_SEL_PEN+MAX_PET*no+i] & 0x01) )
		{
			pc.selectPetNo[i] = 1;
		}
		else
		{
			pc.selectPetNo[i] = 0;
		}
#if 0	// ���`�Ф����?��Τǥ��`�֤��ʤ�
		if( (savedatabuf[BA_SEL_PEN+MAX_PET*no+i] & 0x02) )
		{
			pc.battlePetNo = i;
		}
#endif
		// ?�`?�줱�Ƥ�
		if( (savedatabuf[BA_SEL_PEN+MAX_PET*no+i] & 0x04) )
		{
			pc.mailPetNo = i;
		}
	}
}


// ?�`���O����?�����Ʊ���Хåե�������
void clearUserSetting( int no )
{
	int i;

	if( no < 0 || MAXCHARACTER <= no )
		return;

	pc.battlePetNo = -1;

	// ���L�μӥڥåȥ�?��
	// ���L�μ��x�k�ڥåȥ�?��
	for( i = 0; i < MAX_PET; i++ )
	{
		savedatabuf[BA_SEL_PEN+MAX_PET*no+i] = 0x00;
	}
}


// ����?�ɥ��ץ�??�O���򱣴�Хåե�������
void setUserSoundOption( void )
{
	int i;

	savedatabuf[STEREO_FLAG]	= (unsigned char)stereo_flg;		// ����?���Ф��椨��?��
	savedatabuf[SE_VOL]			= (unsigned char)t_music_se_volume;	// SE��??�`?
	savedatabuf[BGM_VOL]		= (unsigned char)t_music_bgm_volume;// BGM��??�`?
	for( i = 0; i < 16; i++ )										// BGM�ԥå�
		savedatabuf[BGM_PITCH+i] = t_music_bgm_pitch[i];
}


// ����?�ɥ��ץ�??�O���򱣴�Хåե�����ȡ��
void getUserSoundOption( void )
{
	int i;

	stereo_flg			= (int)savedatabuf[STEREO_FLAG];	// ����?���Ф��椨��?��
	t_music_se_volume	= (int)savedatabuf[SE_VOL];			// SE��??�`?
	t_music_bgm_volume	= (int)savedatabuf[BGM_VOL];		// BGM��??�`?
	for( i = 0; i < 16; i++ )								// BGM�ԥå�
		t_music_bgm_pitch[i] = savedatabuf[BGM_PITCH+i];
}


// ��?�åȥ��ץ�??�O���򱣴�Хåե�������
void setUserChatOption( void )
{
	savedatabuf[CHAT_COLOR]		= MyChatBuffer.color;				// ��?�å���?ɫ
	savedatabuf[CHAT_LINE]		= (unsigned char)NowMaxChatLine;	// ��?�å���?
	savedatabuf[CHAT_AREA]		= NowMaxVoice;						// ��?�åȹ���
}


// ��?�åȥ��ץ�??�O���򱣴�Хåե�����ȡ��
//chi���åȥ��ץ�??�趨���滺��ȡ
void getUserChatOption( void )
{
	MyChatBuffer.color	= savedatabuf[CHAT_COLOR];			// ��?�å���?ɫ  ������ɫ
	NowMaxChatLine		= (int)savedatabuf[CHAT_LINE];		// ��?�å���?  �� 
	NowMaxVoice			= savedatabuf[CHAT_AREA];			// ��?�åȹ��� ��Χ
}


















#if 0

static int savecharrecord = 0;




/*******************************************************

   �ѥ�?�`�ɹ�?

  ���椹�롢��ߤ���


*******************************************************/
/*
  �ѥ�?�`�ɱ���

  char *cdkey : CD key.  NULL�ݥ�?�����ä���CD���`�ϱ��椷�ʤ�
  char *passwd : �ѥ�?�`��   NULL�ݥ�?�����ä���ѥ�?�`�ɤϱ��椷�ʤ�
  return value:
  0 OK
  <0 Error

*/
int savePasswdFile( char *cdkey , char *passwd )
{
	if(cdkey!=NULL)
		setCdkey(cdkey);
	if(passwd!=NULL)
		setPassword(passwd);
	saveNowState();
	loadNowState();
	return 0;
}

/*
  
    char *cdkey : cd key output  NULL �ݥ�?�����ä���Τ⤷�ʤ�
    int cmaxlen : cdkey maxlen
    char *passwd : ������       NULL �ݥ�?�����ä���Τ⤷�ʤ�
    int pmaxlen : �ʤ���passwd  

  return value:0 OK
  <0 Error

*/
int loadPasswdFile( char *cdkey , int cmaxlen , char *passwd ,int pmaxlen)
{
	if(cdkey!=NULL)
		cdkey = getCdkey(cdkey);
	if(passwd!=NULL)
		passwd = getPassword(passwd);
	return 0;
}


/*********************************************************
	����?���֥å�?�å��`���ե���?
*********************************************************/
/*
	��?�ǥå���ָ����?�å��`�����ʾ����
*/
void checkRecvMes(int aindex)
{
	int index, i,j;
	int checkflag=0;
	char m[800];
	FILE *fpin, *fpout;
	remove("data\\recvmes.bak");
	fpout = fopen("data\\recvmes.bak", "wb+");
	if((fpin = fopen("data\\recvmes.bin", "rb+"))==NULL){
		fclose(fpout);
		rename("data\\recvmes.bak","data\\recvmes.bin");
		return;
	}
	while(feof(fpin)==0){
		if(fread(&index,sizeof(int), 1, fpin)<1)break;
		fread(&i,sizeof(int), 1, fpin);
		fread(m, 800, 1, fpin);
		fread(&j,sizeof(int), 1, fpin);
		if(savecharrecord == index){
			if(i==aindex){
//				chatinputPushString(m, j);
			}else{
//				int ArrivalMessage(int i);
//				ArrivalMessage(aindex);
				fwrite(&index, sizeof(int), 1, fpout);
				fwrite(&i, sizeof(int), 1, fpout);
				fwrite(m, 800, 1, fpout);
				fwrite(&j, sizeof(int), 1, fpout);
				checkflag=1;
			}
		}else{
			fwrite(&index, sizeof(int), 1, fpout);
			fwrite(&i, sizeof(int), 1, fpout);
			fwrite(m, 800, 1, fpout);
			fwrite(&j, sizeof(int), 1, fpout);
		}
	}
	fclose (fpin);
	fclose (fpout);
	remove("data\\recvmes.bin");
	rename("data\\recvmes.bak","data\\recvmes.bin");
//	void setBlinkArrivalmessage(int i);//layout.cpp
//	if(checkflag==0)
//		setBlinkArrivalmessage(0);
}

/*
	��?�ǥå���ָ����?�å��`��һ?��?
*/
void deleteMes(int aindex)
{
	int index, i,j;
	int checkflag=0;
	char m[800];
	FILE *fpin, *fpout;
	remove("data\\recvmes.bak");
	fpout = fopen("data\\recvmes.bak", "wb+");
	if((fpin = fopen("data\\recvmes.bin", "rb+"))==NULL){
		fclose (fpout);
		rename("data\\recvmes.bak","data\\recvmes.bin");
		return;
	}
	while(feof(fpin)==0){
		if(fread(&index,sizeof(int), 1, fpin)<1)break;
		fread(&i,sizeof(int), 1, fpin);
		fread(m, 800, 1, fpin);
		fread(&j,sizeof(int), 1, fpin);
		if(savecharrecord == index){
			if(i!=aindex){
				fwrite(&index, sizeof(int), 1, fpout);
				fwrite(&i, sizeof(int), 1, fpout);
				fwrite(m, 800, 1, fpout);
				fwrite(&j, sizeof(int), 1, fpout);
				checkflag=1;
			}
		}else{
			fwrite(&index, sizeof(int), 1, fpout);
			fwrite(&i, sizeof(int), 1, fpout);
			fwrite(m, 800, 1, fpout);
			fwrite(&j, sizeof(int), 1, fpout);
		}
	}
	fclose (fpin);
	fclose (fpout);
	remove("data\\recvmes.bin");
	rename("data\\recvmes.bak","data\\recvmes.bin");
//	void setBlinkArrivalmessage(int i);//layout.cpp
//	if(checkflag==0)
//		setBlinkArrivalmessage(0);
}


/*
	��??ָ����?�å��`��һ?��?
*/
void deleteAllMes(int charindex)
{
	int index;
	int i,j;
	char m[800];
	FILE *fpin, *fpout;
	remove("data\\recvmes.bak");
	fpout = fopen("data\\recvmes.bak", "wb+");
	if((fpin = fopen("data\\recvmes.bin", "rb+"))==NULL){
		fclose(fpout);
		rename("data\\recvmes.bak","data\\recvmes.bin");
		return;
	}
	while(feof(fpin)==0){
		if(fread(&index,sizeof(int), 1, fpin)<1)break;
		fread(&i,sizeof(int), 1, fpin);
		fread(m, 800, 1, fpin);
		fread(&j,sizeof(int), 1, fpin);
		if(charindex != index){
			fwrite(&index, sizeof(int), 1, fpout);
			fwrite(&i, sizeof(int), 1, fpout);
			fwrite(m, 800, 1, fpout);
			fwrite(&j, sizeof(int), 1, fpout);
		}
	}
	fclose (fpin);
	fclose (fpout);
	remove("data\\recvmes.bin");
	rename("data\\recvmes.bak","data\\recvmes.bin");
}
/*
	�ܤ�ȡ�ä�?�å��`���򱣴�
*/
void saveRecvMes(int aindex, char *msg, int color)
{
//	int ArrivalMessage(int i);
//	if(ArrivalMessage(aindex) != 1) return;
	FILE *fp=fopen("data\\recvmes.bin", "ab+");
	fwrite(&savecharrecord, sizeof(int), 1, fp);
	fwrite(&aindex, sizeof(int), 1, fp);
	fwrite(msg, 800, 1, fp);
	fwrite(&color, sizeof(int), 1, fp);
	fclose(fp);
}

/*
	?����??�ˤ��Υ�??��?�å��`�����ܤ��ȤäƤ뤫�ɤ���֪��
*/
void checkRecvdMes(void)
{
	FILE *fp;
	int index;
	int i;
	char m[800];
	int j;
	if((fp = fopen("data\\recvmes.bin", "rb+"))==NULL)return;
	while(feof(fp)==0){
		if(fread(&index,sizeof(int), 1, fp)<1)break;
		fread(&i,sizeof(int), 1, fp);
		fread(m, 800, 1, fp);
		fread(&j,sizeof(int), 1, fp);
		if(savecharrecord == index){
//			int ArrivalMessage(int i);
//			if(ArrivalMessage(i) != 1) return;
//			void setBlinkArrivalmessage(int i);
//			setBlinkArrivalmessage(1);
		}
	}
	fclose (fp);
}
/*********************************************************
	״�B����
	���椹���Τ�

  �ѥ�?�`�ɤ�CDKEY��1�M�����12�����12�Х��ȤȤ���)
  ���¤Τ�Τ�8�M
	������??�ܥ�?�N�(12�Х���)
	������??�ܥ�?��?�ǥå�����24�Х��ȣ�
	��?�åȱ�ʾ��?(1�Х���)
	��?�åȥ�?�`(1�Х���)
	?����M�ߤ��Ƥ����Ȥ�?�뤫��200�Х��ȣ�
	��ǰ��12�Х��ȣ�

  �ǡ����Ф�
	cdkey		0
	password	12
	actionkind	24+300*i
	actionindex	36+300*i
	chatline	60+300*i
	chatcolor	61+300*i
	magicpad	62+300*i
	name		262+300*i
	actionbuttonposition 323+300*i

  �򤤤���äƤ��󤸡�

  ����ǡ���?���`�ɤ��ơ�����ȫ?�ե���?�˱��档
*********************************************************/
/***********************
	?��?��ϵ
***********************/
/*
	������??�ܥ�?��״�B�򱣴��åХåե���
	int i : �η�Ŀ�Υ�����??�ܥ�?��
	int *kind : �ΤηN�
	int *index : �η�Ŀ�Τ�Τ�
*/
void setActionbutton(int i, int kind, int index)
{
	savedatabuf[24+i    +savecharrecord*300] = (char)(kind+1);
	savedatabuf[36+i*2+1+savecharrecord*300] = (index+1) * 256;
	savedatabuf[36+i*2  +savecharrecord*300] = (index+1) % 256;
}
/*
	������??�ܥ�?���`��?��λ�ä򱣴��åХåե���
	int i : �η�Ŀ�˥��`��?��?�뤫
*/
void setActionbuttonCursorPos(int i)
{
	savedatabuf[323+savecharrecord*300] = (char)i;
}

/*
	��?�åȤ�״�B�򱣴��åХåե���
*/
void setChatState(unsigned char line, unsigned char color)
{
	savedatabuf[60+savecharrecord*300]=(char)line;
	savedatabuf[61+savecharrecord*300]=(char)color;
}
/*
	?���M�ߤ���״�r�򱣴��åХåե���
*/
void setMagicPad(int i)
{
	savedatabuf[62+i+savecharrecord*300]=1;
}
/*
	��ǰ����
*/
void setName( char *name)
{
	eucStringToSjisString(name);
	int i;
	for (i=0;i<13;i++){
		savedatabuf[262+i+savecharrecord*300]=name[i];
	}
	sjisStringToEucString(name);
}
/***********************
	�i�ߤ���ϵ
***********************/
/*
	������??�ܥ�?��״�B�򱣴��åХåե�����ȡ��
	int i : �η�Ŀ�Υ�����??�ܥ�?��
	int *kind : �ΤηN�
	int *index : �η�Ŀ�Τ�Τ�
*/
void getActionbutton(int i, int *kind, int *index)
{
	*kind = (int)savedatabuf[24+i+savecharrecord*300]-1;
	*index = savedatabuf[36+i*2+1+savecharrecord*300]*256+savedatabuf[36+i*2+savecharrecord*300]-1;
}
/*
	������??�ܥ�?���`��?��λ�ä򱣴��åХåե���
	���ꂎ������int  ��?�ǥå���
*/
int getActionbuttonCursorPos(void)
{
	return savedatabuf[323+savecharrecord*300];
}
/*
	��?�åȤ�״�B�򱣴��åХåե�����ȡ��
	unsigned char line : ���Ф�
	unsigned char color : ��ɫ��
*/
void getChatState(unsigned char *line, unsigned char *color)
{
	*line = savedatabuf[60+savecharrecord*300];
	*color = savedatabuf[61+savecharrecord*300];
}
/*
	?���M�ߤ���״�r�򱣴��åХåե�����ȡ��
*/
char getMagicPad(int i)
{
	return savedatabuf[62+i+savecharrecord*300];
}
/*
	��ǰ�ä�
*/
char *getName(char *name)
{
	int i;
	for(i=0;i<13;i++)
	{
		name[i] = savedatabuf[262+i+savecharrecord*300];
	}
	return name;
}



/**********************************************
	��???���Ȥ�ä�?�Хåե���?��
**********************************************/
void clearSavedataBuffer(void)
{
	int i, j, k;
	int deleteflag[8];
	for(i=0;i<8;i++){
		deleteflag[i]=0;
	}
	char name[256], opt[1024];
	char getname[13];
	for(i=0;i<8;i++){
		getCharacterListEntry(i,name,opt);
		if(name[0]!=0){
			for(j=0;j<8;j++){
				for(k=0;k<13;k++)
				{
					getname[k] = savedatabuf[262+k+j*300];
				}
				if(strcmp(name, getname)==0){
					deleteflag[j]=1;
					break;
				}
			}
		}
	}
	for(i=0;i<8;i++){
		if(deleteflag[i]==0){
			memset(&savedatabuf[24+300*i], 0, 300);
			deleteAllMes(i);
		}
	}
}
/**********************************************
	?���Ȥκη�Ŀ�Υ�??��?����?������
**********************************************/
void setSavedataRecord(char *name )
{
	eucStringToSjisString(name);
	char getname[13];
	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<13;j++)
		{
			getname[j] = savedatabuf[262+j+i*300];
		}
		if(strcmp(name, getname)==0){
			savecharrecord = i;
			sjisStringToEucString(name);
			return;
		}
	}
	//Ҋ�Ĥ���ʤ��ä���
	for(i=0;i<8;i++){
		if(savedatabuf[262+i*300]=='\0'){
			savecharrecord = i;
			sjisStringToEucString(name);
			setName(name);
			eucStringToSjisString(name);
			break;
		}
	}
	if(i>7)savecharrecord=0;
	sjisStringToEucString(name);
}
/*
	?���Ȥκη�Ŀ��?��Ҋ�Ĥ������줿��
*/
void getNullSavedataRecord(char *name){
	int i;
	for(i=0;i<8;i++){
		if(savedatabuf[262+i*300]== '\0'){
			savecharrecord = i;
			setName(name);
//			saveNowState();
			return;
		}
	}
	savecharrecord = 7;
	setName(name);
	saveNowState();
	loadNowState();
	return ;

}
#endif
