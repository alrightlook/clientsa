#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
/* WIN32_LEAN_AND_MEAN他define 本函数不常用的函数不用编译  WINDOWS.H */
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

#include "tool.h"


//その文字列のワード位置までポインタをずらす 指针检测
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
//その文字列から１ワード分とって更にポインタを移動  那个字符串到1字分来说更指针移动
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
//一行ロードルーチン  释放？
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

//ｑ文字列と同じならその文字数を返し、ｐを動かす  q文字列一样的话那文字数量，p运动
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


//その文字列から１ワード分とってそれを数値として返す。
int strint2( char **pp )
{
	char moji[1000]="";
	char *p = *pp;

	getword( &p , moji );

	*pp = p;
	return atoi(moji);
}


//その文字列から１ワード分とってそれをlong型数値として返す。 那个字符串到1字分来说，它已型数值作为回报
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
  SJISの文字列をEUCに変換する。
  buffer: SJISの文字列。このバッファを直接書き換えるので、文字列定数へのポインタ
  をいれたりしてはいけない。
  SJIS的字符串EUC转换。buffer: SJIS文字列。这个缓冲直接重写，
  所以字符串常数的指针如果不准。
**********************************************************************************/
void sjisStringToEucString( char *buffer )
{
	int sjis2euc( unsigned char *sjis, unsigned char *euc );

	/* 我修改
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
  SJISの1文字をEUCに変換する。
  sjis: SJISの文字
  euc:  EUCに変換された文字
  返り値：SJISでない文字(ASCII等)が渡されるとFALSE,SJISだったらTRUE.

  SJIS一文字EUC转换。sjis: SJIS的文字euc: EUC被转换的文字返回值：
  不SJIS文字（ASCII等）交付的FALSE，SJIS的话TRUE.
*****************************************************************************/
int sjis2euc( unsigned char *sjis, unsigned char *euc )
{
    int word;
    if( !( (0x81<=sjis[0]&&sjis[0]<=0x9f)||(0xc0<=sjis[0]) ) )	return	FALSE;
 // sjis ではなかった 那么没

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
  EUCの文字列をSJISに変換する。
  buffer: EUCの文字列。このバッファを直接書き換えるので、文字列定数をいれたり
  してはいけない。  EUC的字符串SJIS转换。buffer: EUC文字列。这个缓冲直接重写，所以可以字符串常数不可。
**********************************************************************************/
void eucStringToSjisString( char *buffer )
{
	/*
	int euc2sjis( unsigned char *c);

	//我去掉 字符串加密导致汉子 乱码
	
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
  EUCの1文字をSJISに変換する。
  euc: EUCの文字
  sjis:  SJISに変換された文字
  返り値：EUCでない文字(ASCII等)が渡されるとFALSE,EUCだったらTRUE.

  EUC一文字SJIS转换。euc: EUC的文字sjis: SJIS被转换的文字返回值：
  不EUC文字（ASCII等）交付的FALSE，EUC的话TRUE
*****************************************************************************/
int euc2sjis( unsigned char *c)
{
  if( !(c[0]&0x80) )    return FALSE; // eucではなかった  euc了
  c[1] -= (c[0] & 1) == 0 ? 2 : (c[1] < 0xe0 ? 0x61 : 0x60);
  c[0]  = (c[0] < 0xdf ? c[0] + 0x61 : c[0] + 0xe1) >> 1;
  return TRUE;
}




/*
   おおきい文字列からトークンをとりだす。そのときに指定したデリミタ（１文字）を
   つかうことができる。

  char *src : もとの文字列
  char delim : でりみた
  int count : なんこめか。１なら最初のとーくん
  int maxlen : out にとりだす最大文字列ながさ
  char *out : 出力


  戻り値  0：デリミタで終わった
          1:文字列終端で終わった
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
  文字列から文字列へ、文字列がおわるかデリミタがくるまでこぴーする

  char *src : 元の文字列
  char delim : デリミタ
  int maxlen : 最大コピーながさ
  char *out : 出力

  戻り値  0：デリミタで終わった
          1:文字列終端で終わった

  文字列文字列，字符串结束吗定界符来到P的
  char * src:元的文字列char delim:定界符int 
  maxlen:最大的がさ复制char * out:输出返回值0 :
  定界符就结束了一:字符串终止结束了
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
  整数のトークンをint値でとりだす。

  char *src : もとの文字列
  char delim: デリミタになる文字列
  int count :何個目のトークンか １なら最初のとーくん

  return value : 値
  整数的衍生物为int值捞出。char * src:原来的文字列
  char delim:定界符的字符串int count:什么个眼的代币1话最初的和阿return value:值

*/
int getIntegerToken( char *src , char delim , int count )
{
    char  s[100];
    getStringToken( src , delim ,count , sizeof(s )-1 , s );
	if(s[0] == '\0') sprintf(s, "-1");
    return atoi( s );
}
/*
  doubleのトークンをとりだす。
  char *src : もとの文字列
  char delim: デリミタになる文字列
  int count :何個目のトークンか １なら最初のとーくん

  return value: 値

*/
double getDoubleToken( char *src , char delim , int count )
{
    char s[100];
    getStringToken( src , delim , count , sizeof( s )-1 , s );
    return strtod( s , NULL );
        

}

/*
        chopする(UNIX形式の文字列)

  char *src : 元の文字列。  このバッファを直接編集するから注意。

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
intの配列をシフトするのだ

  int *a : 配列のアドレス
  int siz :　配列のサイズ
  int count : いくつぶんシフトするか
　配列の最後には０をうめるぞ


  int排列转移的。int * a:排列的地址int siz:排列的尺寸
  int count:几个分子转移吗排列的最后0填补了

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


// 62進文字列をintに変換する
//62进文字列int转换
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
  整数のトークンをint値でとりだす。

  char *src : もとの文字列
  char delim: デリミタになる文字列
  int count :何個目のトークンか １なら最初のとーくん

  return value : 値

  整数的衍生物为int值捞出。char * src:原来的文字列
  char delim:定界符的字符串
  int count:什么个眼的代币1话最初的和阿return value:值

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
//fps計算
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
  小文字、大文字に関係なく文字列を比較する。

  char *s1 : 文字列１
  char *s2 : 文字列２
  int len : 最大比較文字数。



  小写字母大写，关系比较文字列的。char * s1:字符串1char * s2:字符串2int len:最大比较文字数。
*/
int strncmpi( char *s1 , char *s2 , int len )
{
	for(int i=0;i<len;i++){
		if( s1[i] == '\0' || s2[i] == '\0' ) return 0;
		int c1 = tolower( s1[i] );  //把字符转换成小写字母,非字母字符不做出处理
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
  スペースだけの文字列だったら1、他の文字を含んでいたら
  0を返す。SJIS用。
  空间光的文字列的话1，其他文字含有的话0回报。SJIS用。
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
  文字列をバッファに挿入する。
  buffer: バッファ
  string: 挿入する文字列
  whereToinsert: どこに挿入するか

  字符串缓冲插入。buffer:缓冲: string的字符串插入whereToinsert:插入到哪里？
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
  文字をバッファに挿入する。
  buffer: バッファ
  character: 挿入する文字列
  whereToinsert: どこに挿入するか


  文字缓冲插入。buffer:缓冲character:插入的字符串whereToinsert:插入到哪里？
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
えすけーぷ（サーバからのばっちもん）
哎—ぷ色狼（从服务器的很棒的东西）
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
 * makeEscapeStringで作成された文字列を元に戻す
 * 引数
 *  src             char*       元になる文字列。これが書き替る。
 * 返り値
 *  src    を返す。(関数を入れこにしやすいように)


  在* makeEscapeString被制作了的字符串原状*参数src char * *元的字符串。
  这写的替る。*返回值* src回报。（函数放入这里方便）
 ----------------------------------------*/
char*   makeStringFromEscaped( char* src )
{
    int     i;
    int     srclen = strlen( src );
    int     searchindex=0;

    for( i = 0 ; i < srclen ; i ++ ){
        if( src[i] == '\\' ){
            int j;

            /*  次の文字に行く  下面的文字去 */
            i++;
            for( j = 0; j<sizeof(escapeChar)/sizeof(escapeChar[0]); j++)
                if( escapeChar[j].escapedchar == src[i] ){
                    /*  次がエスケープキャラだ    次逃脱角色。*/
                    src[searchindex++] = escapeChar[j].escapechar;
                    goto NEXT;
                }
            /*  エラーやけどそのままコピーしておく  */
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
 * エスケープする。
 * 引数
 *  src             char*       元になる文字列
 *  dest            char*       エスケープされる文字列
 *  sizeofdest      int         dest の サイズ
 * 返り値
 *  dest    を返す。(関数を入れこにしやすいように)
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
            /*  '\0'分が足りないのでここで終り   */
            break;

        for( j = 0; j<sizeof(escapeChar)/sizeof(escapeChar[0]); j++)
            if( src[i] == escapeChar[j].escapechar ){
                dirty=TRUE;
                escapechar= escapeChar[j].escapedchar;
                break;
            }

        if( dirty == TRUE ){
            /*  エスケープする文字だ    */
            if( destindex + 2 < sizeofdest ){
                /*  +2 というのは、 '\\' と 'n'の事だ   */
                /*  残り文字は十分だ    */
                dest[destindex] = '\\';
                dest[destindex+1] = escapechar;
                destindex+=2;
                dirty=TRUE;
                continue;       /*  次の文字に進む  */
            }else{
                /*  残りバッファが不足している  */
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
	sjisをeucにしてエスケープかます
	char *src :もと
	char *dest : 出力
	int sizeofdest : 出力の最大サイズ
*/
void makeSendString(char *src, char *dest, int sizeofdest)
{

	sjisStringToEucString(src);
	makeEscapeString(src ,dest, sizeofdest);
}

/*
	eucをsjisにしてエスケープ解除
	char *src :もと
	char *srcが直接書き換わる。
*/
char *makeRecvString(char *src)
{
	makeStringFromEscaped( src );
	eucStringToSjisString(src);
	return src;
}




/***************************************************************
	暗号化ルーチン関係 by Jun
***************************************************************/
/*
char *src	生のデータadress
int srclen	生のデータsrcの有効長さ
int keystring	エンコードするためのキー
char *encoded	エンコードされた文字列を格納するメモリのアドレス
int *encodedlen	エンコードされた文字列の長さをいれるアドレス 
int maxencodedlen 最大長さはきめられている
*/
void jEncode(char *src,int srclen,int key,char *encoded,int *encodedlen,int maxencodedlen)
{
	char sum=0;
	int i;
	if(srclen+1 > maxencodedlen){
		/* エンコード後は長さが1のびる。マックスをこえたらエンコードせず単にコピーするだけ*/
		*encodedlen = maxencodedlen;
		for(i=0;i<(*encodedlen);i++){
			encoded[i] = src[i];
		}
	}
	if(srclen+1 <= maxencodedlen){
		/*マックスこえないとき */
		*encodedlen=srclen+1;
		for(i=0;i<srclen;i++){
			sum = sum + src[i];
			/*先頭から7の倍数個目か2の倍数個目のところはビット反転*/
			if(((key%7) == (i%5))||((key%2) == (i%2))){
				src[i] = ~src[i];
			}
		}
		for(i=0;i<(*encodedlen);i++){
			if(abs((key%srclen)) > i){			
			encoded[i] = src[i] + sum*((i*i)%3);	/*てきとうにたしてる*/
			}
			/*key%srclenの位置に全ビットをたしたものを置く*/			
			if(abs((key%srclen)) == i){
				encoded[i] = sum;
			}
			if(abs((key%srclen)) < i){
				encoded[i] = src[i-1] + sum*((i*i)%7);	/*てきとうにたしてる*/
			}
		}
	}
}

/*
char *src	暗号化されたデータ
int srclen	暗号化されているデータの有効長さ
int key		元のデータのんと同じキー
char *decoded	デコードされた文字列。出力
int *decodedlen	デコードされた文字列の長さ
*/
void jDecode(char *src,int srclen,int key,char *decoded,int *decodedlen)
{
	/*エンコードの逆*/
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

