/************************/
/*	font.c				*/
/************************/
#include "system.h"

#define ASCII(a) a-'A'+10
#define ASCII_DEC(a) a-'0'+ 35

#define FONT_BUFFER_SIZE 1024 	// フォントバッファサイズ  字体缓冲区大小

// フォントバッファー  字体缓冲器
FONT_BUFFER FontBuffer[ FONT_BUFFER_SIZE ];

// フォントカウンター  字体柜台
int FontCnt = 0;

// 全角文字の横幅（ドット数）  点数
int FontZenkauWidth;
// 半角文字の横幅（ドット数）
int	FontHankakuWidth;

/* フォント情報をバッファに溜める  字体的信息缓冲蓄 *********************************************/
int StockFontBuffer( int x, int y, char fontPrio, int color, char *str, BOOL hitFlag )
{


	// カウントオーバーの時  字体计数 大于缓冲区的时间
	if( FontCnt >= FONT_BUFFER_SIZE ) return -2;
	//字体缓冲信息积蓄
	// フォントバッファーに情報を溜める
	FontBuffer[ FontCnt ].x = x;
	FontBuffer[ FontCnt ].y = y;
	FontBuffer[ FontCnt ].fontPrio = fontPrio;
	FontBuffer[ FontCnt ].color = color;
	FontBuffer[ FontCnt ].hitFlag = hitFlag;
	// 文字列コピー  文字列拷贝
	
	strcpy( FontBuffer[ FontCnt ].str, str );
	
	// フォントカウンタープラス 字体计数器加
	//FontCnt++;
	return FontCnt++;
	
}

/* フォント情報をバッファに溜める（構造体渡し）***字体的信息缓冲储（构造体交货）*****************************/
void StockFontBuffer2( STR_BUFFER *strBuffer )
{
	int lineDist = 0; // 左上からのＹオフセット座標 从左上角的Y坐标偏移
	int splitPoint = 0;
	char splitStr[ 256 ];
	
	// カウントオーバーの時
	if( FontCnt >= FONT_BUFFER_SIZE ){ 
		strBuffer->hitFontNo = -2;
		return;
	}
	
	// 改行する時
	if( strBuffer->lineLen != 0 ){
		// 一行に収まらない時
		while( strlen( strBuffer->buffer + splitPoint ) >= strBuffer->lineLen ){
		// 一行に収まらない時
		//if( strlen( strBuffer.buffer + splitPoint ) > strBuffer.lineLen ){
			// 分割文字列作成
			strncpy( splitStr, strBuffer->buffer + splitPoint, strBuffer->lineLen );
			// 終端記号を入れる
			*( splitStr + strBuffer->lineLen ) = NULL;
			// 最後の全角文字が分割されている時  最后的全角文字被分割的时候
			if( GetStrLastByte( splitStr ) == 3 ){ 
			
				// 一文字戻す 一字型恢复
				splitPoint = strBuffer->lineLen - 1 + splitPoint;
				// 終端記号を入れる
				*( splitStr + strBuffer->lineLen - 1 ) = NULL; 
			}
			else splitPoint = strBuffer->lineLen + splitPoint;
	
			// 文字表示バッファに溜める  文字表示缓冲蓄
			StockFontBuffer( strBuffer->x, strBuffer->y + lineDist, strBuffer->fontPrio, 0, splitStr, 0 );
			// Ｙ座標更新
			lineDist += strBuffer->lineDist;
		}
		// 余った文字列を表示 多余的文字表示
		StockFontBuffer( strBuffer->x, strBuffer->y + lineDist, strBuffer->fontPrio, 0, strBuffer->buffer + splitPoint, 0 );
		
		// 文字列いっぱいの時かつ、最後の行がＮＵＬＬの時  文字列满的时候且，最后一行的时候NULL
		if( strBuffer->cnt >= strBuffer->len &&  *( strBuffer->buffer + splitPoint ) == NULL ){
			// ＩＭＥ文字列の表示開始座標を求める  文字列开始寻求显示坐标
			strBuffer->imeX = strBuffer->x + GetStrWidth( strBuffer->buffer + splitPoint - strBuffer->lineLen );
			strBuffer->imeY = strBuffer->y + lineDist - strBuffer->lineDist;
		}else{
			// ＩＭＥ文字列の表示開始座標を求める
			strBuffer->imeX = strBuffer->x + GetStrWidth( strBuffer->buffer + splitPoint );
			strBuffer->imeY = strBuffer->y + lineDist;
		}
		
	}else{
	
		// フォントバッファーに情報を溜める
		FontBuffer[ FontCnt ].x = strBuffer->x;
		FontBuffer[ FontCnt ].y = strBuffer->y;
		FontBuffer[ FontCnt ].fontPrio = strBuffer->fontPrio;
		FontBuffer[ FontCnt ].color = strBuffer->color;
		FontBuffer[ FontCnt ].hitFlag = 0;	// 当たり判定は絶対しない
		// 文字列見えなくする時
		if( strBuffer->blindFlag >= TRUE ){
			int i;
			// 文字列作成
			for( i = 0 ; i < strBuffer->cnt ; i++ ){
				FontBuffer[ FontCnt ].str[ i ] = '*';
			}
			FontBuffer[ FontCnt ].str[ i ] = NULL;
			//sprintf( FontBuffer[ FontCnt ].str, "%*s", strBuffer->cnt, '*' );
		}else{
			// 文字列コピー
			strcpy( FontBuffer[ FontCnt ].str, strBuffer->buffer );
		}
	
		// ＩＭＥ文字列の表示開始座標を求める
		strBuffer->imeX = strBuffer->x + GetStrWidth( strBuffer->buffer );
		strBuffer->imeY = strBuffer->y;
		
		// フォントカウンタープラス
		strBuffer->hitFontNo = FontCnt++;
	}
}

// フォント表示関数 //字体显示函数/////////////////////////////////////////////////////////
void FontPrint( char *moji, int x, int y, int dispPrio )
{
	
	// 文字の数だけループ
	while( *moji ){
		// 空白のとき飛ばす
		if( *moji != 0x20 ){
			// 数字のとき 数字的时候
			if( '0' <= *moji && *moji <= '9' )
			{
				StockDispBuffer( x, y, dispPrio, ASCII_DEC( *moji ), 0 );
			}
			else if( 'A' <= *moji && *moji <= 'Z' ){
				// アルファベットの時  拉丁字母的时候
				//StockDispBuffer( dispPrio, x, y, 32, 0 );
			}

			else if( 'A' <= *moji && *moji <= 'Z' ){
				// アルファベットの時  拉丁字母的时候
				//StockDispBuffer( dispPrio, x, y, 32, 0 );
			}

		}
		// ポインタ進める
		moji++;
		// 表示座標プラス
		x += 16;
	}
}
	
// 数字フォント表示関数 //数字字体显示函数/////////////////////////////////////////////////////
void FontPrintDec( char *moji, int x, int y, int dispPrio )
{
	// 文字の数だけループ
	while( *moji ){
		if( *moji != 0x20 ){ 
			// 一文字ずつバッファにためる
			StockDispBuffer( x, y, dispPrio, 32, 0 );
		}
		// ポインタ進める
		moji++;
		// 表示座標プラス
		x += 16;
	}
}
	
