#include <stdio.h>
#include <string.h>

//#include "util.h"
#include "tool.h"

#include "pc.h"
#include "t_music.h"
#include "chat.h"

#include "savedata.h"



// セーブデータ //
//
// 構造とサイズ
// 		SAVEDATA_VER		セーブデータのバージ??(4 byte)
// 		CDKEY				CDキー(12 byte)
// 		PASSWORD			パス?ード(12 byte)
// 		BA_SEL_PEN			戦闘?の入れ替えで選択できるペットと
//               			戦闘に参加するペットの設定
// 							Bit 0 ... 戦闘?に入れ替え可能
// 							Bit 1 ... 戦闘に参加させる
//							Bit 2 ... ?ー?運んでる
// 		STEREO_FLAG_SIZE	ステ?オ??ノ??の切り替え
// 		SE_VOL				ＳＥボ??ー?
// 		BGM_VOL				ＢＧＭボ??ー?
// 		BGM_PITCH			ＢＧＭのピッチ変更
// 		CHAT_COLOR			チ?ット文?色
// 		CHAT_LINE			チ?ット行?
//		CHAT_AREA_SIZE		チ?ットの聞こえる範囲

// サイズ
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
// 構造
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

// セーブデータバージ??
//   0x0001 ...  26+1 byte
//   0x0002 ...  87+1 byte
//   0x0003 ...  57+1 byte
//   0x0004 ...  57+1 byte
//   0x0005 ... 127+1 byte
#define SAVEDATA_VERSION	0x0005

// セーブファイ?名
#define SAVEFILE_NAME	"data\\savedata.dat"

// エ?ーコード
unsigned char savedataErrorCode;
// loadNowStateエ?ーコード
//  1 ... セーブファイ?が作成できない。
//  2 ... セーブファイ?が開けない。
//  3 ... セーブデータの読み?みに失敗。
//  4 ... セーブデータのサイズが違う。（壊れてる）
//  5 ... セーブデータのバージ??違い。


BOOL createSaveFile( void );


// セーブデータファイ? ///////////////////////////////////////////////////

//
// ?ーザ設定保存
//
BOOL saveUserSetting( void )
{
	setUserSetting( selectPcNo );
	setUserSoundOption();
	setUserChatOption();

	return saveNowState();
}


//
// ?ーザ設定読み?み
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
// セーブデータ保存
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

	// セーブデータの暗?化
	jEncode( (char *)tmpsavedatabuf, SAVEDATA_SIZE, 0,
		writebuffer, &writebufferlen, sizeof( writebuffer ) );

	// セーブデータ保存
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
// セーブデータ読み?み（起動?に1度だけ）
//
BOOL loadNowState( void )
{
	FILE *fp;
	char readbuffer[4000];
	int readbufferlen;
	int tmpsavedatalen;

	savedataErrorCode = 0;

	// セーブファイ?を開く
	if( (fp = fopen( SAVEFILE_NAME, "rb+" )) == NULL )
	{
		// 開けない?はファイ?が無いとみなし新規で作成する。
		if( createSaveFile() == FALSE )
		{
			savedataErrorCode = 1;
			return FALSE;
		}

		// 改めてセーブファイ?を開く
		if( (fp = fopen( SAVEFILE_NAME, "rb+" ))==NULL )
		{
			// 作ったファイ?が開けない
			savedataErrorCode = 2;
			return FALSE;
		}
	}

	// セーブデータ読み?み
	readbufferlen = fread( readbuffer, 1, sizeof( readbuffer ), fp );
	if( ferror( fp ) )
	{
		// 読み?みエ?ー
		savedataErrorCode = 3;
		fclose( fp );
		return FALSE;
	}
	fclose( fp );

	jDecode( readbuffer, readbufferlen, 0, (char *)savedatabuf, &tmpsavedatalen );

#if 1
	// ?バージ??のセーブファイ?を今のものに変換する
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
		// データサイズがちがう
		savedataErrorCode = 4;
		return FALSE;
	}

	// セーブデータバージ??のチェック
	if( *((unsigned short *)savedatabuf+SAVEDATA_VER) != SAVEDATA_VERSION )
	{
		// バージ??違い
		savedataErrorCode = 5;
		return FALSE;
	}

	return TRUE;
}



// 新規セーブファイ?作成
BOOL createSaveFile( void )
{
	FILE *fp;
	char writebuffer[4000];
	int writebufferlen;
	int i;

	// セーブデータバッファク?ア
	memset( savedatabuf, 0, SAVEDATA_SIZE );

	// セーブデータバージ??を入れておく
	*((unsigned short *)savedatabuf+SAVEDATA_VER) = SAVEDATA_VERSION;

	// ?期値の設定
	savedatabuf[STEREO_FLAG]	= (unsigned char)stereo_flg;
	savedatabuf[SE_VOL]			= (unsigned char)t_music_se_volume;
	savedatabuf[BGM_VOL]		= (unsigned char)t_music_bgm_volume;
	for( i = 0; i < 16; i++ )
		savedatabuf[BGM_PITCH+i] = t_music_bgm_pitch[i];

	savedatabuf[CHAT_LINE]		= 10;
	savedatabuf[CHAT_AREA]		= DEF_VOICE;


	// セーブデータの暗?化
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

// CDKEYを保存用バッファに置く
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


// CDKEYを保存用バッファから取る
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
	cdkey[i] = '\0';	// 念のためただし cdkeyバッファはCDKEY_SIZE+1バイト必要

	return cdkey;
}



// パス?ード /////////////////////////////////////////////////////////////

// パス?ードを保存用バッファに入れる
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


// パス?ードを保存用バッファから取る
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
	pass[i] = '\0';	//  念のためただしpassバッファはPASSWORD_SIZE+1バイト必要

	return pass;
}


// ?ーザ設定 /////////////////////////////////////////////////////////////

// ?ーザ設定を保存バッファに入れる
void setUserSetting( int no )
{
	int i;

	if( no < 0 || MAXCHARACTER <= no )
		return;

	// 戦闘参加ペットフ?グ
	// 戦闘参加選択ペットフ?グ
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
#if 0	// サーバに情報が?るのでセーブしない
		if( i == pc.battlePetNo )
		{
			savedatabuf[BA_SEL_PEN+MAX_PET*no+i] |= 0x02;
		}
#endif
		// ?ー?届けてるフ?グ
		if( i == pc.mailPetNo )
		{
			savedatabuf[BA_SEL_PEN+MAX_PET*no+i] |= 0x04;
		}
	}
}


// ?ーザ設定を保存バッファから取る
void getUserSetting( int no )
{
	int i;

	if( no < 0 || MAXCHARACTER <= no )
		return;

	// 戦闘参加ペットフ?グ
	// 戦闘参加選択ペットフ?グ
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
#if 0	// サーバに情報が?るのでセーブしない
		if( (savedatabuf[BA_SEL_PEN+MAX_PET*no+i] & 0x02) )
		{
			pc.battlePetNo = i;
		}
#endif
		// ?ー?届けてる
		if( (savedatabuf[BA_SEL_PEN+MAX_PET*no+i] & 0x04) )
		{
			pc.mailPetNo = i;
		}
	}
}


// ?ーザ設定をク?アして保存バッファに入れる
void clearUserSetting( int no )
{
	int i;

	if( no < 0 || MAXCHARACTER <= no )
		return;

	pc.battlePetNo = -1;

	// 戦闘参加ペットフ?グ
	// 戦闘参加選択ペットフ?グ
	for( i = 0; i < MAX_PET; i++ )
	{
		savedatabuf[BA_SEL_PEN+MAX_PET*no+i] = 0x00;
	}
}


// サウ?ドオプシ??設定を保存バッファに入れる
void setUserSoundOption( void )
{
	int i;

	savedatabuf[STEREO_FLAG]	= (unsigned char)stereo_flg;		// ステ?オ切り替えフ?グ
	savedatabuf[SE_VOL]			= (unsigned char)t_music_se_volume;	// SEボ??ー?
	savedatabuf[BGM_VOL]		= (unsigned char)t_music_bgm_volume;// BGMボ??ー?
	for( i = 0; i < 16; i++ )										// BGMピッチ
		savedatabuf[BGM_PITCH+i] = t_music_bgm_pitch[i];
}


// サウ?ドオプシ??設定を保存バッファから取る
void getUserSoundOption( void )
{
	int i;

	stereo_flg			= (int)savedatabuf[STEREO_FLAG];	// ステ?オ切り替えフ?グ
	t_music_se_volume	= (int)savedatabuf[SE_VOL];			// SEボ??ー?
	t_music_bgm_volume	= (int)savedatabuf[BGM_VOL];		// BGMボ??ー?
	for( i = 0; i < 16; i++ )								// BGMピッチ
		t_music_bgm_pitch[i] = savedatabuf[BGM_PITCH+i];
}


// チ?ットオプシ??設定を保存バッファに入れる
void setUserChatOption( void )
{
	savedatabuf[CHAT_COLOR]		= MyChatBuffer.color;				// チ?ット文?色
	savedatabuf[CHAT_LINE]		= (unsigned char)NowMaxChatLine;	// チ?ット行?
	savedatabuf[CHAT_AREA]		= NowMaxVoice;						// チ?ット範囲
}


// チ?ットオプシ??設定を保存バッファから取る
//chi？ットオプシ??设定保存缓冲取
void getUserChatOption( void )
{
	MyChatBuffer.color	= savedatabuf[CHAT_COLOR];			// チ?ット文?色  聊天颜色
	NowMaxChatLine		= (int)savedatabuf[CHAT_LINE];		// チ?ット行?  行 
	NowMaxVoice			= savedatabuf[CHAT_AREA];			// チ?ット範囲 范围
}


















#if 0

static int savecharrecord = 0;




/*******************************************************

   パス?ード管?

  保存する、よみこむ


*******************************************************/
/*
  パス?ード保存

  char *cdkey : CD key.  NULLポイ?タだったらCDキーは保存しない
  char *passwd : パス?ード   NULLポイ?タだったらパス?ードは保存しない
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
  
    char *cdkey : cd key output  NULL ポイ?タだったら何もしない
    int cmaxlen : cdkey maxlen
    char *passwd : 出力。       NULL ポイ?タだったら何もしない
    int pmaxlen : ながさpasswd  

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
	アド?スブック?ッセージファイ?
*********************************************************/
/*
	イ?デックス指定で?ッセージを表示する
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
	イ?デックス指定で?ッセージ一?削?
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
	キ??指定で?ッセージ一?削?
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
	受け取った?ッセージを保存
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
	?グイ??にそのキ??が?ッセージを受けとってるかどうか知る
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
	状態保存
	保存するものは

  パス?ードとCDKEYが1組（半角12＋半角12バイトとする)
  以下のものを8組
	アクシ??ボタ?種類(12バイト)
	アクシ??ボタ?イ?デックス（24バイト）
	チ?ット表示行?(1バイト)
	チ?ットカ?ー(1バイト)
	?法を組みたてたことが?るか（200バイト）
	名前（12バイト）

  で、配列の
	cdkey		0
	password	12
	actionkind	24+300*i
	actionindex	36+300*i
	chatline	60+300*i
	chatcolor	61+300*i
	magicpad	62+300*i
	name		262+300*i
	actionbuttonposition 323+300*i

  をいじるってかんじ。

  そんで、エ?コードして、それ全?ファイ?に保存。
*********************************************************/
/***********************
	?き?み系
***********************/
/*
	アクシ??ボタ?の状態を保存用バッファに
	int i : 何番目のアクシ??ボタ?か
	int *kind : 何の種類か
	int *index : 何番目のものか
*/
void setActionbutton(int i, int kind, int index)
{
	savedatabuf[24+i    +savecharrecord*300] = (char)(kind+1);
	savedatabuf[36+i*2+1+savecharrecord*300] = (index+1) * 256;
	savedatabuf[36+i*2  +savecharrecord*300] = (index+1) % 256;
}
/*
	アクシ??ボタ?カーソ?の位置を保存用バッファに
	int i : 何番目にカーソ?が?るか
*/
void setActionbuttonCursorPos(int i)
{
	savedatabuf[323+savecharrecord*300] = (char)i;
}

/*
	チ?ットの状態を保存用バッファに
*/
void setChatState(unsigned char line, unsigned char color)
{
	savedatabuf[60+savecharrecord*300]=(char)line;
	savedatabuf[61+savecharrecord*300]=(char)color;
}
/*
	?法組みたて状況を保存用バッファに
*/
void setMagicPad(int i)
{
	savedatabuf[62+i+savecharrecord*300]=1;
}
/*
	名前保存
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
	読みこみ系
***********************/
/*
	アクシ??ボタ?の状態を保存用バッファから取る
	int i : 何番目のアクシ??ボタ?か
	int *kind : 何の種類か
	int *index : 何番目のものか
*/
void getActionbutton(int i, int *kind, int *index)
{
	*kind = (int)savedatabuf[24+i+savecharrecord*300]-1;
	*index = savedatabuf[36+i*2+1+savecharrecord*300]*256+savedatabuf[36+i*2+savecharrecord*300]-1;
}
/*
	アクシ??ボタ?カーソ?の位置を保存用バッファに
	戻り値　：　int  イ?デックス
*/
int getActionbuttonCursorPos(void)
{
	return savedatabuf[323+savecharrecord*300];
}
/*
	チ?ットの状態を保存用バッファから取る
	unsigned char line : 何行か
	unsigned char color : 何色か
*/
void getChatState(unsigned char *line, unsigned char *color)
{
	*line = savedatabuf[60+savecharrecord*300];
	*color = savedatabuf[61+savecharrecord*300];
}
/*
	?法組みたて状況を保存用バッファから取る
*/
char getMagicPad(int i)
{
	return savedatabuf[62+i+savecharrecord*300];
}
/*
	名前得る
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
	キ???ストを得た?バッファク?ア
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
	?ストの何番目のキ??で?グイ?したか
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
	//見つからなかったら
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
	?ストの何番目に?き見つけて作れたか
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

