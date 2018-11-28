#define UNPACK2	0
#define WIN32_LEAN_AND_MEAN
#include <system.h>
//#include <windows.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include "common.h"
#include "loadrealbin.h"
#include "unpack.h"

#include <io.h>
#include <time.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <stdio.h>
#include <stdlib.h>


static char REALBINFILENAME[255];
static char ADDRBINFILENAME[255];
#define LINESTRINGS			255

ADRNBIN adrnbuff[MAX_GRAPHICS];
//���б�����������ʾ��  ��ȫ����ַ�ṹ���б� Ϊ���������ͼ��Ϣ���������ݵ�ַ��
unsigned long bitmapnumbertable[MAX_GRAPHICS];//�ʥ�Щ`����ӥåȥޥå׷��Ť������Ʃ`�֥� U2
//�Զ���ͼ����ʱ��ͼ�����ɫ��
unsigned char autoMapColorTbl[MAX_GRAPHICS];	// ���`�ȥޥå����ɕr�θ������ɫ�Ʃ`�֥�

FILE *Realbinfp, *Addrbinfp, *debughistory;
BITMAPINFO		tmpBitMapInfo;//λͼ��Ϣͷ


PBITMAPINFO 		PBiInfo;
PBYTE				PBits;
BITMAPFILEHEADER	header;//λͼ�ļ�ͷ׼����ȡλͼ��ʾ��

LONG	GetWidth(){return PBiInfo->bmiHeader.biWidth;}
LONG	GetHeight(){return PBiInfo->bmiHeader.biHeight;}
LONG	GetLineSize(){return (PBiInfo->bmiHeader.biWidth + 3) & ~3;}
DWORD	GetRealSize(){return GetWidth()*GetHeight();}
DWORD	GetImageSize(){return GetLineSize()*GetHeight();}

struct RECODEHEAD{
	unsigned short magicnumber;
	long size;
};




//�ե�������_���Ƥߤ�
//�o���_������TRUE�򷵤�   ��	REAL �ɹ�TRUE
BOOL initRealbinFileOpen(char *realbinfilename, char *addrbinfilename)
{
	
    strcpy(REALBINFILENAME, realbinfilename);
	strcpy(ADDRBINFILENAME, addrbinfilename);
	ADRNBIN tmpadrnbuff;
	//real.bin�_�����äѤʤ���  �򿪣�һֱ��
	if ((Realbinfp = fopen(REALBINFILENAME, "rb"))==NULL){
		return FALSE;
	}

	//adrn.bin�_�� ��
	if ((Addrbinfp = fopen(ADDRBINFILENAME, "rb"))==NULL){
		return FALSE;
	}
	//adrn.bin�Хåե����i���z�� �����ȡ
	while(!feof(Addrbinfp))
	{
		//fread ��ȡһ��BMP��ַ�ṹ��һ����ַ�ṹ����
		fread(&tmpadrnbuff, sizeof(tmpadrnbuff), 1, Addrbinfp);
		//�õ��Ľṹ�͵�����Ľṹָ��洢  ʵ����ָ��ָ���˱��õ��Ľṹ
		//bitmapno ͼƬ�Ľṹ�����ں���˳���0��ʼ
		adrnbuff[tmpadrnbuff.bitmapno] = tmpadrnbuff;


		// bitmapnumbertable[�ӥåȥޥå׷���] = ͨ������  [λͼ����] =���
		if ( tmpadrnbuff.attr.bmpnumber != 0)
		{
			//���ĵ�ͼ��Ų���0 �Ͱѽṹ��λ�ú��봫��λͼ�ĺ���
			bitmapnumbertable[tmpadrnbuff.attr.bmpnumber] = tmpadrnbuff.bitmapno;
#if 1
			// �O��������������Τ˂��e����������  �趨�ֶ�����������
			// 300�O�����ʤ��Τ��O������  ����300���趨300��
			if( (12802 <= tmpadrnbuff.attr.bmpnumber && tmpadrnbuff.attr.bmpnumber <= 12811)
			 || (10132 <= tmpadrnbuff.attr.bmpnumber && tmpadrnbuff.attr.bmpnumber <= 10136) )
			{
				adrnbuff[tmpadrnbuff.bitmapno].attr.hit =
					300 + (adrnbuff[tmpadrnbuff.bitmapno].attr.hit % 100);
			}//û�����Ǹ�ʲô����Ȼ���붼������˼  hit���洫˵��0����ͨ��1������
#endif
		}
		else
		{
#if 1
			bitmapnumbertable[tmpadrnbuff.attr.bmpnumber] = 0;
#else
			bitmapnumbertable[tmpadrnbuff.bitmapno] = tmpadrnbuff.bitmapno;
#endif
		}
	}
	//adrn.bin�]����  �ر�
	fclose(Addrbinfp);

	return TRUE;
}


// ���`�ȥޥå��û���ɫ�����ɤޤ����i���z��
//�Զ���ͼ��ͼ����ɫ���ɻ��ȡ
void initAutoMapColor( char *addrbinfilename )
{
	char *filename = "data\\auto.dat";

	// ���`�ȥޥå��û���ɫ�Υե����뤫���i�ߤ���
	//�Զ���ͼ��ͼ����ɫ���ļ��ж�
	if( readAutoMapColor( filename, addrbinfilename ) == 0 )
	{
		// �o���Τǥ��`�ȥޥå��û���ɫ������
		//��Ϊû���Զ���ͼ��ͼ����ɫ����
		makeAutoMapColor();
		writeAutoMapColor( filename, addrbinfilename );
	}
}


// ���`�ȥޥå��û���ɫ������
//�Զ���ͼ��ͼ����ɫ������
void makeAutoMapColor( void )
{
	unsigned int i;
	int chgTbl[20] =
	{
		166,  61,  80, 112,  96, 189, 112,  60, 164, 164,
		111,   0,   0,   0,   0,   0,   0,   0,   0,   0
	};

	for( i = 0; i < MAX_GRAPHICS; i++ )
	{
		if( adrnbuff[i].attr.bmpnumber != 0 )
		{
			if( 100 <= adrnbuff[i].attr.bmpnumber
			 && adrnbuff[i].attr.bmpnumber <= 19999 )
			{
				autoMapColorTbl[adrnbuff[i].attr.bmpnumber] =
					getAutoMapColor( adrnbuff[i].bitmapno );
			}
			else
			if( 60 <= adrnbuff[i].attr.bmpnumber
			 && adrnbuff[i].attr.bmpnumber <= 79 )
			{
				// ����Υ������ɫ���O������ ͼ��Ĵ�ש��ɫ����
				autoMapColorTbl[adrnbuff[i].attr.bmpnumber] =
					(unsigned char)chgTbl[adrnbuff[i].attr.bmpnumber-60];
			}
			else
			{
				autoMapColorTbl[adrnbuff[i].attr.bmpnumber] = 0;
			}
		}
		else
		{
			autoMapColorTbl[adrnbuff[i].attr.bmpnumber] = 0;
		}
	}
}


// ���`�ȥޥå��û���ɫ��ե�����˕����z��
//�Զ���ͼ��ͼ���ļ�д�����ɫ
//  ���ꂎ��  1 ... �ɹ�
//            0 ... ʧ��
int writeAutoMapColor( char *wFName, char *addrbinfilename )
{
	FILE *wfp, *rfp;
	int rfh;
	struct _stat statBuf;
	int adrnNo;
	unsigned int adrnTime;
	unsigned short autoMapColorVersion = 4;	
	char *tmpStr;


	// adrn.bin�ΥЩ`����󷬺ť����å�  �汾�ż��
	tmpStr = strstr( addrbinfilename, "adrn" );
	if( tmpStr == NULL )
		return 0;	// �����Пo���ä���K��� �����У����û�н���
	if( tmpStr[4] == '.' )
	{
		adrnNo = 0;	// �ǥХå��Щ`����� ���԰汾
	}
	else
	{
		adrnNo = -1;
		sscanf( tmpStr, "adrn_%d.bin", &adrnNo );
		if( adrnNo < 0 )
			return 0;
	}

	// adrn.bin�������ե����å�  �����ռ��
	if( (rfp = fopen( addrbinfilename, "rb" )) == NULL )
		return 0;

	rfh = _fileno( rfp );
	if( _fstat( rfh, &statBuf ) < 0 )
	{
		fclose( rfp );
		return 0;
	}
	adrnTime = statBuf.st_ctime;
	fclose( rfp );

	// �ǩ`����������
	if( (wfp = fopen( wFName, "wb" )) == NULL )
		return 0;

	fwrite( &autoMapColorVersion, sizeof( autoMapColorVersion ), 1, wfp );
	fwrite( &adrnNo, sizeof( adrnNo ), 1, wfp );
	fwrite( &adrnTime, sizeof( adrnTime ), 1, wfp );
	fwrite( &autoMapColorTbl, sizeof( autoMapColorTbl ), 1, wfp );

	fclose( wfp );

	return 1;
}


// ���`�ȥޥå��û���ɫ��ե����뤫���i�ߤ���
//  ���ꂎ��  1 ... �ɹ�
//            0 ... ʧ��
int readAutoMapColor( char *wFName, char *addrbinfilename )
{
	FILE *rfp;
	int rfh;
	struct _stat statBuf;
	int adrnNo, rAdrnNo;
	unsigned int adrnTime, rAdrnTime;
	unsigned short autoMapColorVersion = 4, rAutoMapColorVersion;
	char *tmpStr;


	// adrn.bin�ΥЩ`����󷬺�ȡ��  �汾��ȡ��
	tmpStr = strstr( addrbinfilename, "adrn" );
	if( tmpStr == NULL )
		return 0;	// �����Пo���ä���K���
	if( tmpStr[4] == '.' )
	{
		adrnNo = 0;	// �ǥХå��Щ`�����
	}
	else
	{
		adrnNo = -1;
		sscanf( tmpStr, "adrn_%d.bin", &adrnNo );
		if( adrnNo < 0 )
			return 0;
	}

	// adrn.bin��������ȡ��
	if( (rfp = fopen( addrbinfilename, "rb" )) == NULL )
		return 0;

	rfh = _fileno( rfp );//ȡ�ò���streamָ�����ļ�����ʹ�õ��ļ������� ����ĳ�����������ļ���� 
	if( _fstat( rfh, &statBuf ) < 0 )//��ȡ���ļ�ָ�� handle �����ļ���ͳ����Ϣ
	{
		fclose( rfp );
		return 0;
	}
	adrnTime = statBuf.st_ctime;
	fclose( rfp );

	// �ǩ`���i���z��
	if( (rfp = fopen( wFName, "rb" )) == NULL )
		return 0;

	// �Щ`������i���z��
	if( fread( &rAutoMapColorVersion, sizeof( rAutoMapColorVersion ), 1, rfp ) != 1 )
	{
		// �i���z��ʧ��
		fclose( rfp );
		return 0;
	}
	if( autoMapColorVersion != rAutoMapColorVersion )
	{
		// �Щ`������`���ΤǽK���
		fclose( rfp );
		return 0;
	}
	// adrn.bin�Щ`������i���z��
	if( fread( &rAdrnNo, sizeof( rAdrnNo ), 1, rfp ) != 1 )
	{
		// �i���z��ʧ��
		fclose( rfp );
		return 0;
	}
	if( adrnNo != rAdrnNo )
	{
		// �Щ`������`���ΤǽK���
		fclose( rfp );
		return 0;
	}
	// adrn.bin�r�g�i���z��
	if( fread( &rAdrnTime, sizeof( rAdrnTime ), 1, rfp ) != 1 )
	{
		// �i���z��ʧ��
		fclose( rfp );
		return 0;
	}
	if( adrnTime != rAdrnTime )
	{
		// �r�g�`���ΤǽK���
		fclose( rfp );
		return 0;
	}

	// �g�ǩ`���i���z��
	if( fread( &autoMapColorTbl, sizeof( autoMapColorTbl ), 1, rfp ) != 1 )
	{
		// �i���z��ʧ��
		fclose( rfp );
		return 0;
	}
	fclose( rfp );

	return TRUE;
}




//�ե�������]���褦�Ȥ��� / /�ļ��յ�/ /
//�_�����Ȥ������ʤ��ä����ϤǤ⡢���}�o�����ܿ��ˣ���ʹû�����⡣
void cleanupRealbin(void)
{
	fclose(Realbinfp);
#if 0
#ifdef _DEBUG
	fclose(debughistory);
#endif
#endif
}



//����ե��å��ʥ�Щ`�������λ�ä��餦  ƽ�����ȵõ�վ����λ��
//	���� -1:ʧ�� or ����ɹ�
BOOL realGetPos(U4 GraphicNo , S2 *x , S2 *y)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){*x=0;*y=0;return FALSE;}
	*x = adrnbuff[GraphicNo].xoffset;
	*y = adrnbuff[GraphicNo].yoffset;
	return TRUE;
}

//����ե��å��ʥ�Щ`���饰��ե��å������ߤ��򷵤�
//	���� -1:ʧ�� or ����ɹ�
BOOL realGetWH(U4 GraphicNo , S2 *w , S2 *h)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){*w=0;*h=0;return FALSE;}

	*w = adrnbuff[GraphicNo].width;
	*h = adrnbuff[GraphicNo].height;

	return TRUE;
}

//����ե��å��ʥ�Щ`��굱���ꥵ���������롣
//	���� -1:ʧ�� or ����ɹ�
BOOL realGetHitPoints(U4 GraphicNo , S2 *HitX , S2 *HitY)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){*HitX=0;*HitY=0;return FALSE;}

	*HitX = adrnbuff[GraphicNo].attr.atari_x;
	*HitY = adrnbuff[GraphicNo].attr.atari_y;

	return TRUE;
}

//����ե��å��ʥ�Щ`��굱�����ж������롣
//	���� -1:ʧ�� or ����ɹ�
BOOL realGetHitFlag(U4 GraphicNo , S2 *Hit)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){
		*Hit=0;
		return FALSE;
	}

	*Hit = (adrnbuff[GraphicNo].attr.hit % 100);

	return TRUE;
}

//����ե��å��ʥ�Щ`����軭�����λ�Q������ȡ�������
//	���� -1:ʧ�� or ����ɹ�
BOOL realGetPrioType(U4 GraphicNo , S2 *prioType)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){
		*prioType=0;
		return FALSE;
	}

	*prioType = (adrnbuff[GraphicNo].attr.hit / 100);
	return TRUE;
}

//����ե��å��ʥ�Щ`���ߤ����Пo�����롣
//	���� -1:ʧ�� or ����ɹ�
BOOL realGetHeightFlag(U4 GraphicNo , S2 *Height)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){*Height=0;return FALSE;}
	*Height = adrnbuff[GraphicNo].attr.height;

	return TRUE;
}

//����ե��å��ʥ�Щ`���h������
//	���� -1:ʧ�� or ����ɹ�
int realGetSoundEffect(U4 GraphicNo)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS)return FALSE;
	return adrnbuff[bitmapnumbertable[GraphicNo]].attr.effect1;

}

//����ե��å��ʥ�Щ`���i������
//	���� -1:ʧ�� or ����ɹ�
BOOL realGetWalkSoundEffect(U4 GraphicNo )
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){return FALSE;}
	return adrnbuff[GraphicNo].attr.effect2;

}

//�����˥ӥåȥޥåץե����뷬��(�ե������������֤Τ�)���뤨��ͨ�����Ť򷵤�
//	���� -1:ʧ�� or ����ɹ�
BOOL realGetNo( U4 CharAction , U4 *GraphicNo )
{
	if(CharAction<0 || CharAction>=MAX_GRAPHICS){*GraphicNo=0;return FALSE;}
	*GraphicNo = bitmapnumbertable[CharAction];
	return TRUE;
}

//
//  ������ͨ�����Ť��뤨�ƥӥåȥޥåץե����뷬�Ť򷵤� ( realGetNo ����)
//  by Thai
int realGetBitmapNo( int num )
{

	if( num < 0 || num >= MAX_GRAPHICS ) return -1;
	return adrnbuff[num].attr.bmpnumber;

}




extern int unpack2(FILE *fp, char* p_out, long* bytelength);
extern int unpack3(char* bytedata, char* p_out, long* bytelength, int mode);

//////////////////////////////////////////////////////////������Ϊֹ���Ժ���/////////�����ޤǥǥХå��v��
#define REALGETIMAGEMAXSIZE 1000*1000
BYTE g_realgetimagebuf[REALGETIMAGEMAXSIZE];
BYTE g_realgetimagebuf2[REALGETIMAGEMAXSIZE];

//�ʥ�Щ`���饤��`���ǩ`���򷵤�   ѹ����ʽID�ŵ�ͼ�����ݷ���
//graphicNo ͼƬID   ����2������
#if 1
BOOL realGetImage(int graphicNo, unsigned char **bmpdata, int *width, int *height)
{
	//_asm{int 3};
	ADRNBIN adrdata;
	//RD_HEADER head;
//	static BYTE bitmapinfo[sizeof(BITMAPINFO)+sizeof(RGBQUAD)*256];
	if(graphicNo<0 || graphicNo>=MAX_GRAPHICS)return FALSE;

	//graphicNo���ɥ쥹ȡ�����
	adrdata=adrnbuff[graphicNo];

	//�����adrndata��real.bin�Υ��ɥ쥹�����
	//�趨�ļ�ָ�� ָ��Ҫ��ȡ��real�ĵ�ַ
	fseek(Realbinfp, adrdata.adder, SEEK_SET);//real.bin�ե�����ݥ��󥿺Ϥ碌 

    if( fread(&g_realgetimagebuf, adrdata.size, 1, Realbinfp ) != 1 )
	//��real�ļ���ȡһ��adrn����ָ���ı�ͼƬ���ݴ�С
		return FALSE;

	unsigned int len;
	*bmpdata = g_realgetimagebuf2;
	//����������ݴ��ݸ�bmpdata

	if( decoder( g_realgetimagebuf, bmpdata,
			(unsigned int*)width, (unsigned int*)height, &len ) == NULL )
	{
		return FALSE;
	}

	return TRUE;
}
#else
BOOL realGetImage(int graphicNo, unsigned char **bmpdata, int *width, int *height)
{
	ADRNBIN adrdata;
	RECODEHEAD head;
//	static BYTE bitmapinfo[sizeof(BITMAPINFO)+sizeof(RGBQUAD)*256];
	if(graphicNo<0 || graphicNo>=MAX_GRAPHICS)return FALSE;

	//graphicNo���ɥ쥹ȡ�����
	adrdata=adrnbuff[graphicNo];

	//�����adrndata��real.bin�Υ��ɥ쥹�����
	fseek(Realbinfp, adrdata.adder, SEEK_SET);//real.bin�ե�����ݥ��󥿺Ϥ碌 

    if( fread(&head, 8, 1 , Realbinfp ) != 1 ) return FALSE;
	if((head.magicnumber != 'AP')&&(head.magicnumber != 'UC')){
	    //�إå��`����
#if 0
#ifdef DEBUGPUSH
		PUSH("real.bin��Ҏ����");
#endif
#endif
		return FALSE;
	}
#if UNPACK2
	unpack2( Realbinfp, (char *)&g_realgetimagebuf[0], &head.size);

#else



#if 0
    if( fread(&g_realgetimagebuf, head.size, 1 , Realbinfp ) != 1 )return FALSE;
	//�إå��`���R�s�åޥ��å��ʥ�Щ`���ä��r��11/12�إå�����&�ǩ`���R�s�棩

	if( head.magicnumber =='AP') 
		unpack( (char *)&g_realgetimagebuf[0], &head.size , 1 );
#else
    if( fread(g_realgetimagebuf2, head.size, 1 , Realbinfp ) != 1 )return FALSE;
	//�إå��`���R�s�åޥ��å��ʥ�Щ`���ä��r��11/12�إå�����&�ǩ`���R�s�棩

	if( head.magicnumber =='AP') 
		unpack3( (char *)g_realgetimagebuf2,(char *)g_realgetimagebuf, &head.size , 1 );
#endif



#endif

#if 1
	*width = *( ( int *)g_realgetimagebuf );
	*height = *( ( int *)g_realgetimagebuf + 1 );
	*bmpdata = g_realgetimagebuf + 8;
#else // �R�s�ʤ�
	*width = 64;
	*height = head.size / 64;
	*bmpdata = g_realgetimagebuf2;
#endif
	
	return TRUE;
}
#endif

#if 0
BYTE g_linearbuf[REALGETIMAGEMAXSIZE];
//���Υ���ˤ���  �����ڴ��
BOOL Linear(void)
{
    //����
    LPBYTE  PReserv;

	int siz = GetImageSize();

	PReserv = g_linearbuf;

    //PBits�����ݤ򱣹�
    CopyMemory(PReserv, PBits, GetImageSize() );

    LPBYTE  PSource;
    int     SourcePitch;
    LPBYTE  PDest;

    if(GetHeight() > 0){
        //������ɥ�����ʽ
        PSource = PReserv + (GetHeight() - 1) * GetLineSize();
        SourcePitch = -GetLineSize();
    }else{
        //OS2��ʽ
        PSource = PReserv;
        SourcePitch = GetLineSize();
    }


    PDest = PBits = g_realgetimagebuf;

    for(int i = 0 ; i < GetHeight() ; i ++){
        CopyMemory( PDest, PSource, GetWidth() );
        PDest   += GetWidth();
        PSource += SourcePitch;
    }

    return TRUE;
}
#endif





