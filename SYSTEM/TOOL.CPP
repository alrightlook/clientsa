#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
/* WIN32_LEAN_AND_MEAN��define �����������õĺ������ñ���  WINDOWS.H */
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

#include "tool.h"


//���������ФΥ�`��λ�äޤǥݥ��󥿤򤺤餹 ָ����
int wordchk( char **pp )
{
	char *p = *pp;
	
	while ( *p != 0 ){
		switch (*p){
		case '\t':
			p++;
			break;
		case ' ':
			p++;
			break;
		default:
			
			*pp = p;
			return 1;

		}		
	}
	return 0;
}
//���������Ф��飱��`�ɷ֤ȤäƸ��˥ݥ��󥿤��Ƅ�  �Ǹ��ַ�����1�ַ���˵��ָ���ƶ�
int getword(char **pp , char *q)
{
	int i=0;
	char *p = *pp;
	
	wordchk( &p );
	
	for ( i=0 ; *p != ' ' && *p != '\t' && *p != '\0' ; p++ , q++ , i++ ){
		*q = *p;
	}
	*(q) = '\0';
	
	
	*pp = p;
	return i;
}
//һ�Х��`�ɥ�`����  �ͷţ�
void freadline( char *ptbuf , FILE *ptFile )
{
	char buf;
	int i;

	for (i=0 ; ; i++){
		if(feof( ptFile ) != 0 ){
			*ptbuf--;
			*ptbuf = '\0';
			break;
		}
		//	fseek( ptFile, 1, SEEK_CUR);
		fread( &buf ,1 , sizeof(unsigned char) , ptFile );
		if (buf == ' '){
			*ptbuf++ = buf;
//			;
		}else if (buf == '\t'){
			*ptbuf++ = buf;
//			;
		}else if (buf != 0x0d && buf != 0x0a){
			*ptbuf++ = buf;
		}else{
			*ptbuf++ = '\0';
			break;
		}
	}
	while ( feof(ptFile ) == 0 ){
		fread( &buf ,1 , sizeof(unsigned char) , ptFile );
		if (buf != 0x0d && buf != 0x0a ){
			fseek( ptFile, -1, SEEK_CUR);
			break;
		}
	}
}

//�������Ф�ͬ���ʤ餽���������򷵤������Ӥ���  q������һ���Ļ�������������p�˶�
int strstr2( char **pp , char *q )
{
	char *p = *pp;
	
	wordchk( &p );
	
	
	for (int i=1 ; *p++ == *q++ ; i++){
		
		if ( *q == 0 ){
			
			
			*pp = p;
			return i;
		}
		
	}
	return 0;
}


//���������Ф��飱��`�ɷ֤ȤäƤ���������Ȥ��Ʒ�����
int strint2( char **pp )
{
	char moji[1000]="";
	char *p = *pp;

	getword( &p , moji );

	*pp = p;
	return atoi(moji);
}


//���������Ф��飱��`�ɷ֤ȤäƤ����long�������Ȥ��Ʒ����� �Ǹ��ַ�����1�ַ���˵����������ֵ��Ϊ�ر�
long strlong2( char **pp )
{
	char moji[1000]="";
	char *p = *pp;

	getword( &p , moji );

	*pp = p;
	return atol(moji);
}


/* by nakamura */
/*********************************************************************************
  SJIS�������Ф�EUC�ˉ�Q���롣
  buffer: SJIS�������С����ΥХåե���ֱ�ӕ����Q����Τǡ������ж����ؤΥݥ���
  �򤤤줿�ꤷ�ƤϤ����ʤ���
  SJIS���ַ���EUCת����buffer: SJIS�����С��������ֱ����д��
  �����ַ���������ָ�������׼��
**********************************************************************************/
void sjisStringToEucString( char *buffer )
{
	int sjis2euc( unsigned char *sjis, unsigned char *euc );

	/* ���޸�
    char tmp[4];

    while( *buffer != '\0' ){
        if( sjis2euc( ( unsigned char* )buffer, ( unsigned char * )tmp ) ){
            buffer[0] = tmp [0];
            buffer[1] = tmp [1];
            buffer +=2;
        }else{
            buffer +=1;
        }
    }*/
}
/*****************************************************************************
  SJIS��1���֤�EUC�ˉ�Q���롣
  sjis: SJIS������
  euc:  EUC�ˉ�Q���줿����
  ���ꂎ��SJIS�Ǥʤ�����(ASCII��)���ɤ�����FALSE,SJIS���ä���TRUE.

  SJISһ����EUCת����sjis: SJIS������euc: EUC��ת�������ַ���ֵ��
  ��SJIS���֣�ASCII�ȣ�������FALSE��SJIS�Ļ�TRUE.
*****************************************************************************/
int sjis2euc( unsigned char *sjis, unsigned char *euc )
{
    int word;
    if( !( (0x81<=sjis[0]&&sjis[0]<=0x9f)||(0xc0<=sjis[0]) ) )	return	FALSE;
 // sjis �ǤϤʤ��ä� ��ôû

    word = sjis[0];
    if( word>=0xa0 )	word-= 0x40;
    euc[0] = ((word-0x81)<<1) + 0x21;
    word = sjis[1];
    if( word>=0x7f )	word--;
    word= word-0x40+0x21;
    if( word>=0x7f ){
        word = word-0x7f+0x21;
        euc[0]++;
    }
    euc[1] = word;
    euc[0] |= 0x80;
    euc[1] |= 0x80;
    return TRUE;
}



/*********************************************************************************
  EUC�������Ф�SJIS�ˉ�Q���롣
  buffer: EUC�������С����ΥХåե���ֱ�ӕ����Q����Τǡ������ж����򤤤줿��
  ���ƤϤ����ʤ���  EUC���ַ���SJISת����buffer: EUC�����С��������ֱ����д�����Կ����ַ����������ɡ�
**********************************************************************************/
void eucStringToSjisString( char *buffer )
{
	/*
	int euc2sjis( unsigned char *c);

	//��ȥ�� �ַ������ܵ��º��� ����
	
    while( *buffer != '\0' )
	{
        if( euc2sjis( (unsigned char *)buffer ) )
		{
            buffer +=2;
        }
		else
		{
            buffer +=1;
        }
    }*/
}
/*****************************************************************************
  EUC��1���֤�SJIS�ˉ�Q���롣
  euc: EUC������
  sjis:  SJIS�ˉ�Q���줿����
  ���ꂎ��EUC�Ǥʤ�����(ASCII��)���ɤ�����FALSE,EUC���ä���TRUE.

  EUCһ����SJISת����euc: EUC������sjis: SJIS��ת�������ַ���ֵ��
  ��EUC���֣�ASCII�ȣ�������FALSE��EUC�Ļ�TRUE
*****************************************************************************/
int euc2sjis( unsigned char *c)
{
  if( !(c[0]&0x80) )    return FALSE; // euc�ǤϤʤ��ä�  euc��
  c[1] -= (c[0] & 1) == 0 ? 2 : (c[1] < 0xe0 ? 0x61 : 0x60);
  c[0]  = (c[0] < 0xdf ? c[0] + 0x61 : c[0] + 0xe1) >> 1;
  return TRUE;
}




/*
   �������������Ф���ȩ`�����Ȥ���������ΤȤ���ָ�������ǥ�ߥ��������֣���
   �Ĥ������Ȥ��Ǥ��롣

  char *src : ��Ȥ�������
  char delim : �Ǥ�ߤ�
  int count : �ʤ󤳤ᤫ�����ʤ�����Τȩ`����
  int maxlen : out �ˤȤ������������Фʤ���
  char *out : ����


  ���ꂎ  0���ǥ�ߥ��ǽK��ä�
          1:�����нK�ˤǽK��ä�
*/
static int copyStringUntilDelim( char *src , char delim , int maxlen , char *out );
int getStringToken( char *src , char delim , int count , int maxlen , char *out )
{
    int c=1;
    for(int i=0;;i++){
        if( src[i]== '\0'){
            strcpy( out , "" );
            return 1;
        }
        if( i == 0 && count == 1 ){
            return copyStringUntilDelim( src , delim , maxlen , out );
        }
        if( src[i] == delim ){
            c++;
            if( c == count ){
                return copyStringUntilDelim( src+i+1 , delim ,maxlen , out );
            }
        }
    }
}
/*
  �����Ф��������Фء������Ф�����뤫�ǥ�ߥ�������ޤǤ��ԩ`����

  char *src : Ԫ��������
  char delim : �ǥ�ߥ�
  int maxlen : ��󥳥ԩ`�ʤ���
  char *out : ����

  ���ꂎ  0���ǥ�ߥ��ǽK��ä�
          1:�����нK�ˤǽK��ä�

  �����������У��ַ��������𶨽������P��
  char * src:Ԫ��������char delim:�����int 
  maxlen:���Ĥ�������char * out:�������ֵ0 :
  ������ͽ�����һ:�ַ�����ֹ������
*/
static int copyStringUntilDelim( char *src , char delim , int maxlen , char *out )
{
    for(int i=0;i<maxlen;i++){
        if( src[i] == delim){
            out[i] = '\0';
            return 0;
        }
        out[i] = src[i];
        if( out[i] == '\0' ) return 1;
    }
	out[i]='\0';
	return 1;
}
/*
  �����Υȩ`�����int���ǤȤ������

  char *src : ��Ȥ�������
  char delim: �ǥ�ߥ��ˤʤ�������
  int count :�΂�Ŀ�Υȩ`���� ���ʤ�����Τȩ`����

  return value : ��
  ������������Ϊintֵ�̳���char * src:ԭ����������
  char delim:��������ַ���int count:ʲô���۵Ĵ���1������ĺͰ�return value:ֵ

*/
int getIntegerToken( char *src , char delim , int count )
{
    char  s[100];
    getStringToken( src , delim ,count , sizeof(s )-1 , s );
	if(s[0] == '\0') sprintf(s, "-1");
    return atoi( s );
}
/*
  double�Υȩ`�����Ȥ������
  char *src : ��Ȥ�������
  char delim: �ǥ�ߥ��ˤʤ�������
  int count :�΂�Ŀ�Υȩ`���� ���ʤ�����Τȩ`����

  return value: ��

*/
double getDoubleToken( char *src , char delim , int count )
{
    char s[100];
    getStringToken( src , delim , count , sizeof( s )-1 , s );
    return strtod( s , NULL );
        

}

/*
        chop����(UNIX��ʽ��������)

  char *src : Ԫ�������С�  ���ΥХåե���ֱ�Ӿ������뤫��ע�⡣

*/
void chop( char *src )
{
    for(int i=0;;i++){
        if( src[i]==0)break;
        if( src[i]== '\n' && src[i+1] == '\0' ){
            src[i] = '\0';
            break;
        }
    }
}


/*
int�����Ф򥷥եȤ���Τ�

  int *a : ���ФΥ��ɥ쥹
  int siz :�����ФΥ�����
  int count : �����Ĥ֤󥷥եȤ��뤫
�����Ф�����ˤϣ��򤦤�뤾


  int����ת�Ƶġ�int * a:���еĵ�ַint siz:���еĳߴ�
  int count:��������ת�������е����0���

*/
void shiftIntArray( int *a , int siz , int count )
{
    int i;
    for(i=0;i<siz-count;i++){
        a[i]= a[i+count];
    }
    for(i=siz-count;i<siz;i++){
        a[i] = 0;
    }
}


// 62�M�����Ф�int�ˉ�Q����
//62��������intת��
// 0-9,a-z(10-35),A-Z(36-61)
int a62toi( char *a )
{
	int ret = 0;
	int fugo = 1;

	while( *a != NULL )
	{
		ret *= 62;
		if( '0' <= (*a) && (*a) <= '9' )
			ret += (*a)-'0';
		else
		if( 'a' <= (*a) && (*a) <= 'z' )
			ret += (*a)-'a'+10;
		else
		if( 'A' <= (*a) && (*a) <= 'Z' )
			ret += (*a)-'A'+36;
		else
		if( *a == '-' )
			fugo = -1;
		else
			return 0;
		a++;
	}
	return ret*fugo;
}


/*
  �����Υȩ`�����int���ǤȤ������

  char *src : ��Ȥ�������
  char delim: �ǥ�ߥ��ˤʤ�������
  int count :�΂�Ŀ�Υȩ`���� ���ʤ�����Τȩ`����

  return value : ��

  ������������Ϊintֵ�̳���char * src:ԭ����������
  char delim:��������ַ���
  int count:ʲô���۵Ĵ���1������ĺͰ�return value:ֵ

*/
int getInteger62Token( char *src, char delim, int count )
{
    char  s[100];

    getStringToken( src, delim, count, sizeof(s)-1, s );
	if( s[0] == '\0' )
		sprintf( s, "-1" );
    return a62toi( s );
}


#if 0
//fpsӋ��
int GetFps(void)
{
	static int counter[2];
	static int lasttimer;
	int timer = GetTickCount()/1000;
	counter[timer%2]++;
	if (lasttimer != timer){
		lasttimer = timer;
		counter[timer%2] = 0;
	}
	return counter[(timer-1)%2];
}
#endif




/*
  С���֡������֤��v�S�ʤ������Ф���^���롣

  char *s1 : �����У�
  char *s2 : �����У�
  int len : �����^��������



  Сд��ĸ��д����ϵ�Ƚ������еġ�char * s1:�ַ���1char * s2:�ַ���2int len:���Ƚ���������
*/
int strncmpi( char *s1 , char *s2 , int len )
{
	for(int i=0;i<len;i++){
		if( s1[i] == '\0' || s2[i] == '\0' ) return 0;
		int c1 = tolower( s1[i] );  //���ַ�ת����Сд��ĸ,����ĸ�ַ�����������
		int c2 = tolower( s2[i] );
		if( c1 != c2 )return 1;
	}

	return 0;
}

RECT intToRect( int left, int top, int right, int bottom )
{
    RECT rc={left,top,right,bottom};
    return rc;
}

/*****************************************************************
  ���ک`�������������Ф��ä���1���������֤򺬤�Ǥ�����
  0�򷵤���SJIS�á�
  �ռ��������еĻ�1���������ֺ��еĻ�0�ر���SJIS�á�
******************************************************************/
int isOnlySpaceChars( char *data )
{

  int i = 0;
  int returnflag = 0;

  while( data[i] != '\0' ){

	returnflag = 0;
    if ((unsigned char)data[i]==' ')returnflag = 1;
	if ( IsDBCSLeadByte(data[i]) ){
		if((unsigned char)data[i]==0x81
		&&(unsigned char)data[i+1]==0x40){
			returnflag = 1;
		}
		i++;
	}
	if(returnflag == 0)return 0;
    
    i++;
  }
  if(i==0)return 0;
  return returnflag;
}


/****************************
  �����Ф�Хåե��˒��뤹�롣
  buffer: �Хåե�
  string: ���뤹��������
  whereToinsert: �ɤ��˒��뤹�뤫

  �ַ���������롣buffer:����: string���ַ�������whereToinsert:���뵽���
****************************/
void insertString( char *buffer, char *string, int whereToInsert )
{
    int stringLength, bufferLength, i;

    stringLength = strlen( string );
    bufferLength = strlen( buffer );

    for( i=0;i<=bufferLength-whereToInsert;i++ ){
        buffer[bufferLength+stringLength-i] = buffer[bufferLength-i];
    }
    for( i=0;i<stringLength;i++ ){
        buffer[whereToInsert+i] = string[i];
    }

}

/****************************
  ���֤�Хåե��˒��뤹�롣
  buffer: �Хåե�
  character: ���뤹��������
  whereToinsert: �ɤ��˒��뤹�뤫


  ���ֻ�����롣buffer:����character:������ַ���whereToinsert:���뵽���
****************************/
void insertChar( char *buffer, char character, int whereToInsert )
{
    int bufferLength, i;

    bufferLength = strlen( buffer );

    for( i=0;i<=bufferLength-whereToInsert;i++ ){
        buffer[bufferLength+1-i] = buffer[bufferLength-i];
    }
    buffer[whereToInsert] = character;
}



/*************************************************************
�������`�ף����`�Ф���ΤФä����
������ɫ�ǣ��ӷ������ĺܰ��Ķ�����
*************************************************************/


typedef struct tagEscapeChar
{
    char     escapechar;
    char     escapedchar;
}EscapeChar;
static EscapeChar   escapeChar[]=
{
    { '\n',   'n' },
    { ',',    'c' },
    { '|',    'z' },
    { '\\',   'y' },
};
/*----------------------------------------
 * makeEscapeString�����ɤ��줿�����Ф�Ԫ�ˑ���
 * ����
 *  src             char*       Ԫ�ˤʤ������С����줬������롣
 * ���ꂎ
 *  src    �򷵤���(�v������줳�ˤ��䤹���褦��)


  ��* makeEscapeString�������˵��ַ���ԭ״*����src char * *Ԫ���ַ�����
  ��д����롣*����ֵ* src�ر����������������﷽�㣩
 ----------------------------------------*/
char*   makeStringFromEscaped( char* src )
{
    int     i;
    int     srclen = strlen( src );
    int     searchindex=0;

    for( i = 0 ; i < srclen ; i ++ ){
        if( src[i] == '\\' ){
            int j;

            /*  �Τ����֤��Ф�  ���������ȥ */
            i++;
            for( j = 0; j<sizeof(escapeChar)/sizeof(escapeChar[0]); j++)
                if( escapeChar[j].escapedchar == src[i] ){
                    /*  �Τ��������`�ץ�����    �����ѽ�ɫ��*/
                    src[searchindex++] = escapeChar[j].escapechar;
                    goto NEXT;
                }
            /*  ����`�䤱�ɤ��Τޤޥ��ԩ`���Ƥ���  */
            src[searchindex++] = src[i];
        }else
            src[searchindex++] = src[i];
    NEXT:
		;
    }
    src[searchindex] = '\0';
    return src;
}

/*----------------------------------------
 * �������`�פ��롣
 * ����
 *  src             char*       Ԫ�ˤʤ�������
 *  dest            char*       �������`�פ����������
 *  sizeofdest      int         dest �� ������
 * ���ꂎ
 *  dest    �򷵤���(�v������줳�ˤ��䤹���褦��)
 ----------------------------------------*/
char*   makeEscapeString( char* src , char* dest, int sizeofdest)
{
    int         i;
    int     srclen = strlen( src );
    int     destindex=0;

    for( i = 0 ; i < srclen ; i ++ ){
        BOOL dirty=FALSE;
        int     j;
        char    escapechar='\0';
        if( destindex + 1 >= sizeofdest )
            /*  '\0'�֤����ʤ��ΤǤ����ǽK��   */
            break;

        for( j = 0; j<sizeof(escapeChar)/sizeof(escapeChar[0]); j++)
            if( src[i] == escapeChar[j].escapechar ){
                dirty=TRUE;
                escapechar= escapeChar[j].escapedchar;
                break;
            }

        if( dirty == TRUE ){
            /*  �������`�פ������֤�    */
            if( destindex + 2 < sizeofdest ){
                /*  +2 �Ȥ����Τϡ� '\\' �� 'n'���¤�   */
                /*  �Ф����֤�ʮ�֤�    */
                dest[destindex] = '\\';
                dest[destindex+1] = escapechar;
                destindex+=2;
                dirty=TRUE;
                continue;       /*  �Τ����֤��M��  */
            }else{
                /*  �Ф�Хåե������㤷�Ƥ���  */
                dest[destindex] = '\0';
                return dest;
            }
        }else{
            dest[destindex] = src[i];
            destindex++;
        }

    }
    dest[destindex] = '\0';
    return dest;
}

/*
	sjis��euc�ˤ��ƥ������`�פ��ޤ�
	char *src :���
	char *dest : ����
	int sizeofdest : ��������󥵥���
*/
void makeSendString(char *src, char *dest, int sizeofdest)
{

	sjisStringToEucString(src);
	makeEscapeString(src ,dest, sizeofdest);
}

/*
	euc��sjis�ˤ��ƥ������`�׽��
	char *src :���
	char *src��ֱ�ӕ����Q��롣
*/
char *makeRecvString(char *src)
{
	makeStringFromEscaped( src );
	eucStringToSjisString(src);
	return src;
}




/***************************************************************
	���Ż���`�����v�S by Jun
***************************************************************/
/*
char *src	���Υǩ`��adress
int srclen	���Υǩ`��src���Є��L��
int keystring	���󥳩`�ɤ��뤿��Υ��`
char *encoded	���󥳩`�ɤ��줿�����Ф��{�������Υ��ɥ쥹
int *encodedlen	���󥳩`�ɤ��줿�����Ф��L���򤤤�륢�ɥ쥹 
int maxencodedlen ����L���Ϥ�����Ƥ���
*/
void jEncode(char *src,int srclen,int key,char *encoded,int *encodedlen,int maxencodedlen)
{
	char sum=0;
	int i;
	if(srclen+1 > maxencodedlen){
		/* ���󥳩`������L����1�ΤӤ롣�ޥå����򤳤����饨�󥳩`�ɤ����g�˥��ԩ`�������*/
		*encodedlen = maxencodedlen;
		for(i=0;i<(*encodedlen);i++){
			encoded[i] = src[i];
		}
	}
	if(srclen+1 <= maxencodedlen){
		/*�ޥå��������ʤ��Ȥ� */
		*encodedlen=srclen+1;
		for(i=0;i<srclen;i++){
			sum = sum + src[i];
			/*���^����7�α�����Ŀ��2�α�����Ŀ�ΤȤ����ϥӥåȷ�ܞ*/
			if(((key%7) == (i%5))||((key%2) == (i%2))){
				src[i] = ~src[i];
			}
		}
		for(i=0;i<(*encodedlen);i++){
			if(abs((key%srclen)) > i){			
			encoded[i] = src[i] + sum*((i*i)%3);	/*�Ƥ��Ȥ��ˤ����Ƥ�*/
			}
			/*key%srclen��λ�ä�ȫ�ӥåȤ򤿤�����Τ��ä�*/			
			if(abs((key%srclen)) == i){
				encoded[i] = sum;
			}
			if(abs((key%srclen)) < i){
				encoded[i] = src[i-1] + sum*((i*i)%7);	/*�Ƥ��Ȥ��ˤ����Ƥ�*/
			}
		}
	}
}

/*
char *src	���Ż����줿�ǩ`��
int srclen	���Ż�����Ƥ���ǩ`�����Є��L��
int key		Ԫ�Υǩ`���Τ��ͬ�����`
char *decoded	�ǥ��`�ɤ��줿�����С�����
int *decodedlen	�ǥ��`�ɤ��줿�����Ф��L��
*/
void jDecode(char *src,int srclen,int key,char *decoded,int *decodedlen)
{
	/*���󥳩`�ɤ���*/
	char sum;
	int i;
	*decodedlen=srclen-1;
	sum = src[abs(key%(*decodedlen))];

	for(i=0;i<srclen;i++){
		if(abs((key%(*decodedlen))) > i){
			decoded[i] = src[i] - sum*((i*i)%3);
		}

		if(abs((key%(*decodedlen))) < i){
			decoded[i-1] = src[i] - sum*((i*i)%7);
		}
	}
	for(i=0;i<(*decodedlen);i++){
		if(((key%7) == (i%5))||((key%2) == (i%2))){
			decoded[i] = ~decoded[i];
		}
	}
}
