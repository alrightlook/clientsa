#ifndef _TOOL_H_
#define _TOOL_H_

#include<stdio.h>
//�ƥ����ȥե����룱�з֤��i���z��  һ���ı��ļ���ȡ
void freadline( char *ptbuf , FILE *ptFile );
//���������ФΥ�`��λ�äޤǥݥ��󥿤򤺤餹  �Ǹ��ַ���ָ���λ�ô���
int wordchk( char **pp );
//�������Ф�ͬ���ʤ餽���������򷵤������Ӥ���
int strstr2( char **pp , char *q );
//���������Ф��飱��`�ɷ֤ȤäƸ��˥ݥ��󥿤��Ƅ�
int getword(char **pp , char *q);
//���������Ф��飱��`�ɷ֤Ȥ������Ȥ��Ʒ�����
int strint2( char **pp );
//���������Ф��飱��`�ɷ֤ȤäƤ����long�������Ȥ��Ʒ�����
long strlong2( char **pp );

//fpsӋ��
int GetFps(void);
void sjisStringToEucString( char *buffer );
void eucStringToSjisString( char *buffer );



int getStringToken( char *src , char delim , int count , int maxlen , char *out );
int getIntegerToken( char *src , char delim , int count );
double getDoubleToken( char *src , char delim , int count );
void chop( char *src );
void shiftIntArray( int *a , int siz , int count );

int a62toi( char * );
int getInteger62Token( char *, char, int );


void jEncode( char *, int, int, char *, int *, int );
void jDecode( char *, int, int, char *, int * );

int strncmpi( char *s1 , char *s2 , int len );
struct tagRECT intToRect( int left, int top, int right, int bottom );
void insertString( char *buffer, char *string, int whereToInsert );
void insertChar( char *buffer, char character, int whereToInsert );
int isOnlySpaceChars( char *data );

char*   makeStringFromEscaped( char* src );
char*   makeEscapeString( char* src , char* dest, int sizeofdest);
void makeSendString(char *src, char *dest, int sizeofdest);
char *makeRecvString(char *src);


#endif /*_TOOL_H_*/