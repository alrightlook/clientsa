#ifndef _TOOL_H_
#define _TOOL_H_

#include<stdio.h>
//テキストファイル１行分を読み込み  一行文本文件读取
void freadline( char *ptbuf , FILE *ptFile );
//その文字列のワード位置までポインタをずらす  那个字符串指针的位置错开
int wordchk( char **pp );
//ｑ文字列と同じならその文字数を返し、ｐを動かす
int strstr2( char **pp , char *q );
//その文字列から１ワード分とって更にポインタを移動
int getword(char **pp , char *q);
//その文字列から１ワード分とり数値として返す。
int strint2( char **pp );
//その文字列から１ワード分とってそれをlong型数値として返す。
long strlong2( char **pp );

//fps計算
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
