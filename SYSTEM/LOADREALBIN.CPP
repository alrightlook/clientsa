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
//从列表拉倒桌面显示？  是全部地址结构体列表 为容易提出地图信息建立的数据地址表
unsigned long bitmapnumbertable[MAX_GRAPHICS];//ナンバーからビットマップ番号を引くテーブル U2
//自动地图作成时各图像的颜色表
unsigned char autoMapColorTbl[MAX_GRAPHICS];	// オートマップ作成時の各画像の色テーブル

FILE *Realbinfp, *Addrbinfp, *debughistory;
BITMAPINFO		tmpBitMapInfo;//位图信息头


PBITMAPINFO 		PBiInfo;
PBYTE				PBits;
BITMAPFILEHEADER	header;//位图文件头准备提取位图显示用

LONG	GetWidth(){return PBiInfo->bmiHeader.biWidth;}
LONG	GetHeight(){return PBiInfo->bmiHeader.biHeight;}
LONG	GetLineSize(){return (PBiInfo->bmiHeader.biWidth + 3) & ~3;}
DWORD	GetRealSize(){return GetWidth()*GetHeight();}
DWORD	GetImageSize(){return GetLineSize()*GetHeight();}

struct RECODEHEAD{
	unsigned short magicnumber;
	long size;
};




//ファイルを開いてみる
//無事開けたらTRUEを返す   打开	REAL 成功TRUE
BOOL initRealbinFileOpen(char *realbinfilename, char *addrbinfilename)
{
	
    strcpy(REALBINFILENAME, realbinfilename);
	strcpy(ADDRBINFILENAME, addrbinfilename);
	ADRNBIN tmpadrnbuff;
	//real.bin開き（っぱなし）  打开（一直）
	if ((Realbinfp = fopen(REALBINFILENAME, "rb"))==NULL){
		return FALSE;
	}

	//adrn.bin開き 打开
	if ((Addrbinfp = fopen(ADDRBINFILENAME, "rb"))==NULL){
		return FALSE;
	}
	//adrn.binバッファに読み込み 缓冲读取
	while(!feof(Addrbinfp))
	{
		//fread 读取一个BMP地址结构到一个地址结构变量
		fread(&tmpadrnbuff, sizeof(tmpadrnbuff), 1, Addrbinfp);
		//得到的结构送到申请的结构指针存储  实际是指针指向了本得到的结构
		//bitmapno 图片的结构体所在号码顺序从0开始
		adrnbuff[tmpadrnbuff.bitmapno] = tmpadrnbuff;


		// bitmapnumbertable[ビットマップ番号] = 通し番号  [位图号码] =编号
		if ( tmpadrnbuff.attr.bmpnumber != 0)
		{
			//最后的地图编号不是0 就把结构体位置号码传给位图的号码
			bitmapnumbertable[tmpadrnbuff.attr.bmpnumber] = tmpadrnbuff.bitmapno;
#if 1
			// 設定がおかしいものに個別で修正する  设定怪东西个别修正
			// 300設定がないので設定する  不到300？设定300个
			if( (12802 <= tmpadrnbuff.attr.bmpnumber && tmpadrnbuff.attr.bmpnumber <= 12811)
			 || (10132 <= tmpadrnbuff.attr.bmpnumber && tmpadrnbuff.attr.bmpnumber <= 10136) )
			{
				adrnbuff[tmpadrnbuff.bitmapno].attr.hit =
					300 + (adrnbuff[tmpadrnbuff.bitmapno].attr.hit % 100);
			}//没理解是干什么的虽然代码都明白意思  hit外面传说是0可以通过1不可以
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
	//adrn.bin閉じる  关闭
	fclose(Addrbinfp);

	return TRUE;
}


// オートマップ用画像色の作成または読み込み
//自动地图用图像颜色作成或读取
void initAutoMapColor( char *addrbinfilename )
{
	char *filename = "data\\auto.dat";

	// オートマップ用画像色のファイルから読みこむ
	//自动地图用图像颜色的文件中读
	if( readAutoMapColor( filename, addrbinfilename ) == 0 )
	{
		// 無いのでオートマップ用画像色を作成
		//因为没有自动地图用图像颜色作成
		makeAutoMapColor();
		writeAutoMapColor( filename, addrbinfilename );
	}
}


// オートマップ用画像色の作成
//自动地图用图像颜色的作成
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
				// 画像のタイルに色を設定する 图像的瓷砖颜色设置
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


// オートマップ用画像色をファイルに書き込む
//自动地图用图像文件写入的颜色
//  戻り値：  1 ... 成功
//            0 ... 失敗
int writeAutoMapColor( char *wFName, char *addrbinfilename )
{
	FILE *wfp, *rfp;
	int rfh;
	struct _stat statBuf;
	int adrnNo;
	unsigned int adrnTime;
	unsigned short autoMapColorVersion = 4;	
	char *tmpStr;


	// adrn.binのバージョン番号チェック  版本号检查
	tmpStr = strstr( addrbinfilename, "adrn" );
	if( tmpStr == NULL )
		return 0;	// 文字列無かったら終わる 文字列，如果没有结束
	if( tmpStr[4] == '.' )
	{
		adrnNo = 0;	// デバッグバージョン 调试版本
	}
	else
	{
		adrnNo = -1;
		sscanf( tmpStr, "adrn_%d.bin", &adrnNo );
		if( adrnNo < 0 )
			return 0;
	}

	// adrn.binの作成日チェック  作成日检查
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

	// データ書きこみ
	if( (wfp = fopen( wFName, "wb" )) == NULL )
		return 0;

	fwrite( &autoMapColorVersion, sizeof( autoMapColorVersion ), 1, wfp );
	fwrite( &adrnNo, sizeof( adrnNo ), 1, wfp );
	fwrite( &adrnTime, sizeof( adrnTime ), 1, wfp );
	fwrite( &autoMapColorTbl, sizeof( autoMapColorTbl ), 1, wfp );

	fclose( wfp );

	return 1;
}


// オートマップ用画像色をファイルから読みこむ
//  戻り値：  1 ... 成功
//            0 ... 失敗
int readAutoMapColor( char *wFName, char *addrbinfilename )
{
	FILE *rfp;
	int rfh;
	struct _stat statBuf;
	int adrnNo, rAdrnNo;
	unsigned int adrnTime, rAdrnTime;
	unsigned short autoMapColorVersion = 4, rAutoMapColorVersion;
	char *tmpStr;


	// adrn.binのバージョン番号取得  版本号取得
	tmpStr = strstr( addrbinfilename, "adrn" );
	if( tmpStr == NULL )
		return 0;	// 文字列無かったら終わる
	if( tmpStr[4] == '.' )
	{
		adrnNo = 0;	// デバッグバージョン
	}
	else
	{
		adrnNo = -1;
		sscanf( tmpStr, "adrn_%d.bin", &adrnNo );
		if( adrnNo < 0 )
			return 0;
	}

	// adrn.binの作成日取得
	if( (rfp = fopen( addrbinfilename, "rb" )) == NULL )
		return 0;

	rfh = _fileno( rfp );//取得参数stream指定的文件流所使用的文件描述符 返回某个数据流的文件句柄 
	if( _fstat( rfh, &statBuf ) < 0 )//获取由文件指针 handle 所打开文件的统计信息
	{
		fclose( rfp );
		return 0;
	}
	adrnTime = statBuf.st_ctime;
	fclose( rfp );

	// データ読み込み
	if( (rfp = fopen( wFName, "rb" )) == NULL )
		return 0;

	// バージョン読み込み
	if( fread( &rAutoMapColorVersion, sizeof( rAutoMapColorVersion ), 1, rfp ) != 1 )
	{
		// 読み込み失敗
		fclose( rfp );
		return 0;
	}
	if( autoMapColorVersion != rAutoMapColorVersion )
	{
		// バージョン違うので終わる
		fclose( rfp );
		return 0;
	}
	// adrn.binバージョン読み込み
	if( fread( &rAdrnNo, sizeof( rAdrnNo ), 1, rfp ) != 1 )
	{
		// 読み込み失敗
		fclose( rfp );
		return 0;
	}
	if( adrnNo != rAdrnNo )
	{
		// バージョン違うので終わる
		fclose( rfp );
		return 0;
	}
	// adrn.bin時間読み込み
	if( fread( &rAdrnTime, sizeof( rAdrnTime ), 1, rfp ) != 1 )
	{
		// 読み込み失敗
		fclose( rfp );
		return 0;
	}
	if( adrnTime != rAdrnTime )
	{
		// 時間違うので終わる
		fclose( rfp );
		return 0;
	}

	// 実データ読み込み
	if( fread( &autoMapColorTbl, sizeof( autoMapColorTbl ), 1, rfp ) != 1 )
	{
		// 読み込み失敗
		fclose( rfp );
		return 0;
	}
	fclose( rfp );

	return TRUE;
}




//ファイルを閉じようとする / /文件闭的/ /
//開くことが出来なかった場合でも、問題無し。能开了，即使没有问题。
void cleanupRealbin(void)
{
	fclose(Realbinfp);
#if 0
#ifdef _DEBUG
	fclose(debughistory);
#endif
#endif
}



//グラフィックナンバーより立ち位置をもらう  平面号码比得到站立的位置
//	戻値 -1:失敗 or 以外成功
BOOL realGetPos(U4 GraphicNo , S2 *x , S2 *y)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){*x=0;*y=0;return FALSE;}
	*x = adrnbuff[GraphicNo].xoffset;
	*y = adrnbuff[GraphicNo].yoffset;
	return TRUE;
}

//グラフィックナンバーからグラフィック幅、高さを返す
//	戻値 -1:失敗 or 以外成功
BOOL realGetWH(U4 GraphicNo , S2 *w , S2 *h)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){*w=0;*h=0;return FALSE;}

	*w = adrnbuff[GraphicNo].width;
	*h = adrnbuff[GraphicNo].height;

	return TRUE;
}

//グラフィックナンバーより当たりサイズを求める。
//	戻値 -1:失敗 or 以外成功
BOOL realGetHitPoints(U4 GraphicNo , S2 *HitX , S2 *HitY)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){*HitX=0;*HitY=0;return FALSE;}

	*HitX = adrnbuff[GraphicNo].attr.atari_x;
	*HitY = adrnbuff[GraphicNo].attr.atari_y;

	return TRUE;
}

//グラフィックナンバーより当たり判定を求める。
//	戻値 -1:失敗 or 以外成功
BOOL realGetHitFlag(U4 GraphicNo , S2 *Hit)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){
		*Hit=0;
		return FALSE;
	}

	*Hit = (adrnbuff[GraphicNo].attr.hit % 100);

	return TRUE;
}

//グラフィックナンバーより描画優先順位決定法を取り出す。
//	戻値 -1:失敗 or 以外成功
BOOL realGetPrioType(U4 GraphicNo , S2 *prioType)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){
		*prioType=0;
		return FALSE;
	}

	*prioType = (adrnbuff[GraphicNo].attr.hit / 100);
	return TRUE;
}

//グラフィックナンバーより高さの有無を求める。
//	戻値 -1:失敗 or 以外成功
BOOL realGetHeightFlag(U4 GraphicNo , S2 *Height)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){*Height=0;return FALSE;}
	*Height = adrnbuff[GraphicNo].attr.height;

	return TRUE;
}

//グラフィックナンバーより環境音を
//	戻値 -1:失敗 or 以外成功
int realGetSoundEffect(U4 GraphicNo)
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS)return FALSE;
	return adrnbuff[bitmapnumbertable[GraphicNo]].attr.effect1;

}

//グラフィックナンバーより歩行音を
//	戻値 -1:失敗 or 以外成功
BOOL realGetWalkSoundEffect(U4 GraphicNo )
{
	if(GraphicNo<0 || GraphicNo>=MAX_GRAPHICS){return FALSE;}
	return adrnbuff[GraphicNo].attr.effect2;

}

//引数にビットマップファイル番号(ファイル名の数字のみ)を与えて通し番号を返す
//	戻値 -1:失敗 or 以外成功
BOOL realGetNo( U4 CharAction , U4 *GraphicNo )
{
	if(CharAction<0 || CharAction>=MAX_GRAPHICS){*GraphicNo=0;return FALSE;}
	*GraphicNo = bitmapnumbertable[CharAction];
	return TRUE;
}

//
//  引数に通し番号を与えてビットマップファイル番号を返す ( realGetNo の逆)
//  by Thai
int realGetBitmapNo( int num )
{

	if( num < 0 || num >= MAX_GRAPHICS ) return -1;
	return adrnbuff[num].attr.bmpnumber;

}




extern int unpack2(FILE *fp, char* p_out, long* bytelength);
extern int unpack3(char* bytedata, char* p_out, long* bytelength, int mode);

//////////////////////////////////////////////////////////到这里为止调试函数/////////ここまでデバッグ関数
#define REALGETIMAGEMAXSIZE 1000*1000
BYTE g_realgetimagebuf[REALGETIMAGEMAXSIZE];
BYTE g_realgetimagebuf2[REALGETIMAGEMAXSIZE];

//ナンバーからイメージデータを返す   压缩格式ID号的图像数据返回
//graphicNo 图片ID   后面2个宽高
#if 1
BOOL realGetImage(int graphicNo, unsigned char **bmpdata, int *width, int *height)
{
	//_asm{int 3};
	ADRNBIN adrdata;
	//RD_HEADER head;
//	static BYTE bitmapinfo[sizeof(BITMAPINFO)+sizeof(RGBQUAD)*256];
	if(graphicNo<0 || graphicNo>=MAX_GRAPHICS)return FALSE;

	//graphicNoアドレス取り出し
	adrdata=adrnbuff[graphicNo];

	//これでadrndataにreal.binのアドレスが入る
	//设定文件指针 指向要提取的real的地址
	fseek(Realbinfp, adrdata.adder, SEEK_SET);//real.binファイルポインタ合わせ 

    if( fread(&g_realgetimagebuf, adrdata.size, 1, Realbinfp ) != 1 )
	//从real文件读取一个adrn里面指定的本图片数据大小
		return FALSE;

	unsigned int len;
	*bmpdata = g_realgetimagebuf2;
	//把提出的数据传递给bmpdata

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

	//graphicNoアドレス取り出し
	adrdata=adrnbuff[graphicNo];

	//これでadrndataにreal.binのアドレスが入る
	fseek(Realbinfp, adrdata.adder, SEEK_SET);//real.binファイルポインタ合わせ 

    if( fread(&head, 8, 1 , Realbinfp ) != 1 ) return FALSE;
	if((head.magicnumber != 'AP')&&(head.magicnumber != 'UC')){
	    //ヘッダーが変
#if 0
#ifdef DEBUGPUSH
		PUSH("real.binが規格外");
#endif
#endif
		return FALSE;
	}
#if UNPACK2
	unpack2( Realbinfp, (char *)&g_realgetimagebuf[0], &head.size);

#else



#if 0
    if( fread(&g_realgetimagebuf, head.size, 1 , Realbinfp ) != 1 )return FALSE;
	//ヘッダーが圧縮用マジックナンバーだった時（11/12ヘッダ削除&データ圧縮版）

	if( head.magicnumber =='AP') 
		unpack( (char *)&g_realgetimagebuf[0], &head.size , 1 );
#else
    if( fread(g_realgetimagebuf2, head.size, 1 , Realbinfp ) != 1 )return FALSE;
	//ヘッダーが圧縮用マジックナンバーだった時（11/12ヘッダ削除&データ圧縮版）

	if( head.magicnumber =='AP') 
		unpack3( (char *)g_realgetimagebuf2,(char *)g_realgetimagebuf, &head.size , 1 );
#endif



#endif

#if 1
	*width = *( ( int *)g_realgetimagebuf );
	*height = *( ( int *)g_realgetimagebuf + 1 );
	*bmpdata = g_realgetimagebuf + 8;
#else // 圧縮なし
	*width = 64;
	*height = head.size / 64;
	*bmpdata = g_realgetimagebuf2;
#endif
	
	return TRUE;
}
#endif

#if 0
BYTE g_linearbuf[REALGETIMAGEMAXSIZE];
//線形メモリにする  线性内存的
BOOL Linear(void)
{
    //保管
    LPBYTE  PReserv;

	int siz = GetImageSize();

	PReserv = g_linearbuf;

    //PBitsの内容を保管
    CopyMemory(PReserv, PBits, GetImageSize() );

    LPBYTE  PSource;
    int     SourcePitch;
    LPBYTE  PDest;

    if(GetHeight() > 0){
        //ウィンドウズ形式
        PSource = PReserv + (GetHeight() - 1) * GetLineSize();
        SourcePitch = -GetLineSize();
    }else{
        //OS2形式
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
