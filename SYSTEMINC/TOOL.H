#ifndef _TOOL_H_
#define _TOOL_H_

#include<stdio.h>
//テキストファイル１佩蛍を�iみ�zみ  匯佩猟云猟周響函
void freadline( char *ptbuf , FILE *ptFile );
//その猟忖双のワ�`ド了崔までポインタをずらす  椎倖忖憲堪峺寞議了崔危蝕
int wordchk( char **pp );
//ｑ猟忖双と揖じならその猟忖方を卦し、ｐを�咾�す
int strstr2( char **pp , char *q );
//その猟忖双から１ワ�`ド蛍とって厚にポインタを卞��
int getword(char **pp , char *q);
//その猟忖双から１ワ�`ド蛍とり方�､箸靴瞳気后�
int strint2( char **pp );
//その猟忖双から１ワ�`ド蛍とってそれをlong侏方�､箸靴瞳気后�
long strlong2( char **pp );

//fps��麻
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
