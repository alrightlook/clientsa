#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<direct.h>
#include<math.h>

#include"system.h"
#include"map.h"
#include"anim_tbl.h"
#include"pc.h"
#include"netproc.h"
#include"character.h"
#include"loadrealbin.h"
#include"loadsprbin.h"
#include"netmain.h"
#include"lssproto_cli.h"
#include"battleProc.h"
#include"t_music.h"
#include"field.h"
#include"login.h"
#include"menu.h"
#include"battleMenu.h"
#include "handletime.h"




//??魴?????????????????????狢?? 地图的现在地 玩家所在？

#define SEARCH_AREA		11		
// 空マップのチェック範囲（キャラからの距離）天空地图的检查范围（角色开始的距离)

enum
{
	MOUSE_CURSOR_MODE_NORMAL,//??????  鼠标 移动和静止？
	MOUSE_CURSOR_MODE_MOVE  //????
};

// ?昕?????????????????????  地图的现在地 玩家所在？

char nowFloorName[FLOOR_NAME_LEN+1];//地图名字
int nowFloor;                       //编号
int nowFloorGxSize, nowFloorGySize; //所在地图块坐标？
int nowGx, nowGy;
float nowX = (float)nowGx*GRID_SIZE, nowY = (float)nowGy*GRID_SIZE;
float nowVx, nowVy, nowSpdRate;//坐标 移动速度
int nextGx, nextGy;
int oldGx = -1, oldGy = -1;
int oldNextGx = -1, oldNextGy = -1;
int mouseMapGx, mouseMapGy;
int mouseMapX, mouseMapY;
#define MOVE_MODE_CHANGE_TIME	1000  //1秒检测
short mouseCursorMode = MOUSE_CURSOR_MODE_NORMAL;

int mapAreaX1, mapAreaY1, mapAreaX2, mapAreaY2;//4个坐标点
int mapAreaWidth, mapAreaHeight;//宽高

unsigned short tile[MAP_X_SIZE * MAP_Y_SIZE];	// 儅僢僾偺僞僀儖  地图的瓷砖
unsigned short parts[MAP_X_SIZE * MAP_Y_SIZE];	// 儅僢僾偺僷乕僣 地图的零件
unsigned short event[MAP_X_SIZE * MAP_Y_SIZE];	// 僀儀儞僩   活动
unsigned short hitMap[MAP_X_SIZE * MAP_Y_SIZE];	// 儅僢僾偁偨傝敾掕  地图一带判定  4层事件？



// マップデータ無し時の移動遅延処理用 地图无数据时的移动延迟处理用
//????????????????????????????
BOOL mapEmptyFlag;
short mapEmptyDir;
int mapEmptyGx, mapEmptyGy;
short getMapAreaX1[2], getMapAreaY1[2], getMapAreaX2[2], getMapAreaY2[2];
short getMapAreaCnt;
unsigned int mapEmptyStartTime;


// フロアが変更されたか ???????螻????莊???? 楼层被改变了吗？
BOOL floorChangeFlag = FALSE;


// ???????靨?????
//登录时必要的处理
BOOL loginFlag;

// ワープエフェクトを2重で実行しないためのフラグ
//经纱效果双重不实行标志为
//????2????????????
BOOL warpEffectFlag = FALSE;
BOOL warpEffectStart = FALSE;	//?????????????昻?
BOOL warpEffectOk = FALSE;		// ?睫?????

// ??????????????????????
float viewPointX;
float viewPointY;
int viewOffsetX = SCREEN_WIDTH_CENTER;//显示视野
int viewOffsetY = SCREEN_HEIGHT_CENTER;


// ????????
/// /移动处理/ /毫秒
#define MOVE_CLICK_WAIT_TIME	250		//????
short moveAddTbl[8][2] =
{
	{ -1,  1 }, // 0
	{ -1,  0 }, // 1
	{ -1, -1 }, // 2
	{  0, -1 }, // 3
	{  1, -1 }, // 4
	{  1,  0 }, // 5
	{  1,  1 }, // 6
	{  0,  1 }  // 7
};//移动方向和坐标的处理
BOOL moveStackFlag = FALSE;
int moveStackGx, moveStackGy;
#define MOVE_MAX	100
short moveRoute[MOVE_MAX];
short moveRouteCnt = 0;
short moveRouteGx[MOVE_MAX];
short moveRouteGy[MOVE_MAX];
char moveRouteDir[MOVE_MAX];


//?????????????鋗???
#define MOVE_MAX2	2			// サーバに送る移動経路の最大値 服务器送的移动路线的最大值
short moveRouteCnt2 = 0;
short moveRoute2[MOVE_MAX2];

short moveLastDir = -1;

// ?????鼠标的处理
BOOL mouseLeftCrick = FALSE;
BOOL mouseLeftOn = FALSE;
BOOL mouseRightCrick = FALSE;
BOOL mouseRightOn = FALSE;
unsigned int mouseLeftPushTime;
unsigned int beforeMouseLeftPushTime;


// ?????????????????????
//角色，零件的显示优先顺序决定用
enum
{
	CHAR_PARTS_PRIO_TYPE_CHAR,
	CHAR_PARTS_PRIO_TYPE_PARTS
};

typedef struct TAG_CHAR_PARTS_PRIORITY
{
	unsigned int graNo;
	int x, y;
	int dx, dy;
	int depth;
	float mx, my;
	short type;
	TAG_CHAR_PARTS_PRIORITY *pre;
	TAG_CHAR_PARTS_PRIORITY *next;
} CHAR_PARTS_PRIORITY;

#define MAX_CHAR_PRIO_BUF	2048
CHAR_PARTS_PRIORITY charPrioBufTop;
CHAR_PARTS_PRIORITY charPrioBuf[MAX_CHAR_PRIO_BUF];
int charPrioCnt;



// エンカウント処理用  ?犢??????  遇敌处理
short nowEncountPercentage;	// ?犢??   遇敌
short nowEncountExtra;		// ?犢?? ???? 
short minEncountPercentage;	// ?隯?  最小
short maxEncountPercentage;	// 最大エンカウント率
short sendEnFlag;			// EN??????鮃???  发送遇敌标志
short encountNowFlag;		// エンカウント中か  ???????  遇敌当中标志

// ????????
int eventId = 0;			// イベント管理ID  事件管理ID
short eventWarpSendFlag;	// ワープイベント送信フラグ 地图事件标志
short eventWarpSendId;		// ワープイベント送信時のID 发送	ID
short eventEnemySendFlag;	// 固定エンカウントイベント送信フラグ  遇敌标志
short eventEnemySendId;		// 固定エンカウントイベント送信時のID   固定敌人遇ID
short eventEnemyFlag;		// 固定敵にエンカウントしたら１ 固定敌人遇敌的话，1为标志


short etcEventFlag = 0;


// 自动制图用
#define AUTO_MAPPING_W		54
#define AUTO_MAPPING_H		54
BOOL autoMappingInitFlag = TRUE;
unsigned char autoMappingBuf[AUTO_MAPPING_H][AUTO_MAPPING_W];
#define AUTO_MAPPING_SEE_W		30
#define AUTO_MAPPING_SEE_H		30
//???????狄?????????  ??????????
unsigned int readMapAfterFrame = 10000;		// マップ読み込み後の経過フレーム数
unsigned short autoMapSeeFlagBuf[AUTO_MAPPING_SEE_H*AUTO_MAPPING_SEE_W];
BOOL autoMapSeeFlag = FALSE;
int autoMapSeeFloor;
int autoMapSeeGx, autoMapSeeGy;



// ???И??
void onceMoveProc( void );
void partyMoveProc( void );
void getPartyTbl( void );



// ????????????
//#define MAP_CACHE_PROC

#ifdef MAP_CACHE_PROC
//?????羞??鋗???????
#define MAX_MAP_CACHE_SIZE	3		// ??3??
#define MAP_CACHE_X_SIZE	800		//X 1?鋗昮?
#define MAP_CACHE_Y_SIZE	1200		// Y??1?鋗昮?
#define MAP_CACHE_X_BYTE	((MAP_CACHE_X_SIZE+7)/8)
#define MAP_CACHE_Y_BYTE	MAP_CACHE_Y_SIZE
int mapCacheFloorNo[MAX_MAP_CACHE_SIZE];
int mapCacheFloorGxSize[MAX_MAP_CACHE_SIZE];
int mapCacheFloorGySize[MAX_MAP_CACHE_SIZE];
unsigned char mapCacheFlag[MAX_MAP_CACHE_SIZE][MAP_CACHE_X_BYTE*MAP_CACHE_Y_BYTE];
int mapCacheUse;
unsigned int mapCacheLastTime[MAX_MAP_CACHE_SIZE];

void initMapCache( void );
BOOL checkMapCache( int, int, short *, int, int, int, int );
void clearMapCacheFlag( int );
BOOL checkMapCacheFlag( int, int, int, int, int, int, int );
BOOL checkMapCacheEvent( int, int, int, int );

#endif





BOOL readMap( int, int, int, int, int,
	unsigned short *, unsigned short *, unsigned short * );
void readHitMap( int, int, int, int,
	unsigned short *, unsigned short *, unsigned short *, unsigned short * );
void getRouteMap( void );
void shiftRouteMap( void );
void shiftRouteMap2( void );
int getDirData( int, int, int, int );
void checkAreaLimit( short *, short *, short *, short * );
void turnAround( void );
void turnAround2( int );


BOOL checkPrioPartsVsChar( CHAR_PARTS_PRIORITY *, CHAR_PARTS_PRIORITY * );
void insertCharPartsPrio( CHAR_PARTS_PRIORITY *, CHAR_PARTS_PRIORITY * );
void addCharPartsPrio( CHAR_PARTS_PRIORITY *, CHAR_PARTS_PRIORITY * );
void delCharPartsPrio( CHAR_PARTS_PRIORITY * );


BOOL createAutoMap( int, int, int );


///////////////////////////////////////////////////////////////////////////
// マップ処理初期化
void initMap( void )
{
#ifndef _DEBUG
	nowFloor = 0;
	nowFloorGxSize = 0;
	nowFloorGySize = 0;
	nowGx = 0;
	nowGy = 0;
	nowX = 0;
	nowY = 0;
	nextGx = 0;
	nextGy = 0;
	oldGx = 0, oldGy = 0;
	oldNextGx = 0, oldNextGy = 0;

	loginFlag = TRUE;
#else
	if( offlineFlag )
	{
		setMap( 400, 20, 20 );
		nowFloorGxSize = 100;
		nowFloorGySize = 100;
	}
	else
	{
		nowFloor = 0;
		nowFloorGxSize = 0;
		nowFloorGySize = 0;
		nowGx = 0;
		nowGy = 0;
		nowX = 0;
		nowY = 0;
		nextGx = 0;
		nextGy = 0;
		oldGx = 0, oldGy = 0;
		oldNextGx = 0, oldNextGy = 0;

		loginFlag = TRUE;
	}
#endif

	mapAreaX1 = nowGx+MAP_TILE_GRID_X1;
	mapAreaY1 = nowGy+MAP_TILE_GRID_Y1;
	mapAreaX2 = nowGx+MAP_TILE_GRID_X2;
	mapAreaY2 = nowGy+MAP_TILE_GRID_Y2;

	if( mapAreaX1 < 0 )
		mapAreaX1 = 0;
	if( mapAreaY1 < 0 )
		mapAreaY1 = 0;
	if( mapAreaX2 > nowFloorGxSize )
		mapAreaX2 = nowFloorGxSize;
	if( mapAreaY2 > nowFloorGySize )
		mapAreaY2 = nowFloorGySize;

	mapAreaWidth  = mapAreaX2 - mapAreaX1;
	mapAreaHeight = mapAreaY2 - mapAreaY1;

	nowVx = 0;
	nowVy = 0;
	nowSpdRate = 1;

	moveRouteCnt = 0;
	moveRouteCnt2 = 0;
	moveStackFlag = FALSE;

	mapEmptyFlag = FALSE;

	mouseCursorMode = MOUSE_CURSOR_MODE_NORMAL;

	nowEncountPercentage = 0;
	nowEncountExtra = 0;
	sendEnFlag = 0;
	encountNowFlag = 0;

	eventWarpSendFlag = 0;
	eventEnemySendFlag = 0;
	eventEnemyFlag = 0;

	etcEventFlag = 0;

	mouseLeftPushTime = 0;
	beforeMouseLeftPushTime = 0;

	warpEffectFlag = FALSE;
	warpEffectStart = FALSE;
	warpEffectOk = FALSE;

	autoMapSeeFlag = FALSE;

	eventWarpSendId = -1;
	eventEnemySendId = -1;

	moveLastDir = -1;

#ifdef MAP_CACHE_PROC
	initMapCache();
#endif
}


///////////////////////////////////////////////////////////////////////////
// マップ処理リセット
void resetMap( void )
{
	// 移動中なら移動先にいることにする
	nowGx = (int)(nowX/GRID_SIZE);
	nowGy = (int)(nowY/GRID_SIZE);
	nextGx = nowGx;
	nextGy = nowGy;
	nowX = (float)nowGx*GRID_SIZE;
	nowY = (float)nowGy*GRID_SIZE;
	oldGx = -1;
	oldGy = -1;
	oldNextGx = -1;
	oldNextGy = -1;

	mapAreaX1 = nowGx+MAP_TILE_GRID_X1;
	mapAreaY1 = nowGy+MAP_TILE_GRID_Y1;
	mapAreaX2 = nowGx+MAP_TILE_GRID_X2;
	mapAreaY2 = nowGy+MAP_TILE_GRID_Y2;

	if( mapAreaX1 < 0 )
		mapAreaX1 = 0;
	if( mapAreaY1 < 0 )
		mapAreaY1 = 0;
	if( mapAreaX2 > nowFloorGxSize )
		mapAreaX2 = nowFloorGxSize;
	if( mapAreaY2 > nowFloorGySize )
		mapAreaY2 = nowFloorGySize;

	mapAreaWidth  = mapAreaX2 - mapAreaX1;
	mapAreaHeight = mapAreaY2 - mapAreaY1;

	nowVx = 0;
	nowVy = 0;
	nowSpdRate = 1;

	viewPointX = nowX;
	viewPointY = nowY;

	moveRouteCnt = 0;
	moveRouteCnt2 = 0;
	moveStackFlag = FALSE;

	mouseCursorMode = MOUSE_CURSOR_MODE_NORMAL;

	mouseLeftPushTime = 0;
	beforeMouseLeftPushTime = 0;

	autoMapSeeFlag = FALSE;
}


///////////////////////////////////////////////////////////////////////////
// 新しいマップに来たらマップファイル作成しサイズを格納する
//新地图来制作尺寸地图文件存储
void createMap( int fl, int maxx, int maxy )
{
	FILE *fp;
	char floorname[255];
	int i = 0, j, k;
	short l = 0;
	int ox, oy;

	if( fl == 0 )
		return;

	sprintf( floorname, "map\\%d.dat", fl );
	if( (fp = fopen( floorname, "rb" ))==NULL )
	{
#if 0
#ifdef _DEBUG_MSG
		{
			char msg[256];
			sprintf( msg, "新規フロアファイル作成。[フロア:%d/x:%d/y:%d]", fl, maxx, maxy );
			StockChatBufferLine( msg, FONT_PAL_GRAY );
		}
#endif
#endif
		// ファイルが無いので作成する
		_mkdir( "map" );
		if( (fp = fopen( floorname, "wb" )) == NULL )
			return;
		fseek( fp, 0, SEEK_SET );
		fwrite( &maxx, sizeof( int ), 1, fp );
		fwrite( &maxy, sizeof( int ), 1, fp );
		for( i = 0; i < 3; i++ )
		{
			for( j = 0; j < maxx; j++ )
			{
				for( k=0; k < maxy; k++ )
				{
					fwrite( &l, sizeof(short), 1, fp );
				}
			}
		}
		fclose( fp );
	}
	else
	{
		// ファイルがある時サイズが違うならマップが変わってる
		fread( &ox, sizeof( int ), 1, fp );
		fread( &oy, sizeof( int ), 1, fp );
		if( maxx != ox || maxy != oy )
		{
#if 0
#ifdef _DEBUG_MSG
			{
				char msg[256];
				sprintf( msg, "マップサイズ変化。[フロア:%d/x:%d->%d/y:%d->%d]",
					fl, ox, maxx, oy, maxy );
				StockChatBufferLine( msg, FONT_PAL_GRAY );
			}
#endif
#endif
			fclose( fp );
			// サイズが違うので既存ファイルを消して新規にする
			if( (fp = fopen( floorname, "wb" )) == NULL )
				return;
			fwrite( &maxx, sizeof( int ), 1, fp );
			fwrite( &maxy, sizeof( int ), 1, fp );
			for( i = 0; i < 3; i++ )
			{
				for( j = 0; j < maxx; j++ )
				{
					for( k=0; k < maxy; k++ )
					{
						fwrite( &l, sizeof(short), 1, fp );
					}
				}
			}
		}
		fclose ( fp );
	}
}


// 画面内のエベントが再送されてきたらメモリも書きかえる
void setEventMemory( int x, int y, unsigned short ev )
{
	event[(y-mapAreaY1)*mapAreaWidth+(x-mapAreaX1)] = ev;
}


// マップデータの書き込み 地图数据的写入
//格网坐标（x1，y1)-( x2，y2）的范围的地图写入数据
//   グリッド座標(x1,y1)-(x2,y2)の範囲のマップデータを書き込む
//
//     tile, parts, event は線形メモリ構造になっている
//tile, parts, event 线性构造在内存。
//玩家数据在地图的位置 读取和写入 
BOOL writeMap( int floor, int x1, int y1, int x2, int y2,
	unsigned short *tile , unsigned short *parts, unsigned short *event )
{
	FILE *fp;
	char filename[255];
	int fWidth, fHeight, fOffset;
	int mWidth;
	int width, height;
	int fx, fy;
	int mx, my;
	int len, len2;
	int i, j;

#if 0
#ifdef _DEBUG_MSG
	{
		char msg[256];
		//sprintf( msg, "マップファイル書きこみ。[フロア:%d/(%d,%d)-(%d,%d)]",
		sprintf( msg, "地图文件写入。[楼层地图ID:%d/(%d,%d)-(%d,%d)]",

			floor, x1, y1, x2, y2 );
		StockChatBufferLine( msg, FONT_PAL_GRAY );
	}
#endif
#endif

	// 読み込みファイル名作成
	//读取文件名 地图ID
	sprintf( filename, "map\\%d.dat", floor);

	// ファイルオープン  文件开放读写格式
	if( (fp = fopen( filename, "rb+" ))==NULL )
	{
		// 失敗するのは最初でマップフォルダが無いから
		// だからフォルダを作る
		/// /失败的第一次没有地图文件夹文件夹/ / 所以制作
		//创建map目录
		_mkdir( "map" );
		// 読み込みデータも作る
		//读取数据也制造
		//　wb 只写打开或新建一个二进制文件；只允许写数据。 
		fp = fopen( filename, "wb" );
		fclose( fp );
		// 読み込みモードでもう一度オープン
		//读取模式再开放  rb+ 读写打开一个二进制文件，允许读数据
		if( (fp = fopen( filename, "rb+" ))==NULL )
			return FALSE;
	}


	fseek( fp, 0, SEEK_SET );
	//这里是地图格式了
	fread( &fWidth,  sizeof(int), 1, fp );
	fread( &fHeight, sizeof(int), 1, fp );

	mWidth = x2 - x1;
	width = mWidth;
	height = y2 - y1;

	mx = 0;
	fx = x1;
	if( x1 < 0 )
	{
		width += x1;
		fx = 0;
		mx -= x1;
	}
	if( x2 > fWidth )
	{
		width -= (x2 - fWidth);
	}
	my = 0;
	fy = y1;
	if( y1 < 0 )
	{
		height += y1;
		fy = 0;
		my -= y1;
	}
	if( y2 > fHeight )
	{
		height -= (y2 - fHeight);
	}


	fOffset = sizeof(int)*2;
	len = fy * fWidth + fx;
	len2 = my * mWidth + mx;
	for( i = 0; i < height; i++ )
	{
		fseek( fp, sizeof(short)*len+fOffset, SEEK_SET );
		fwrite( &tile[len2], sizeof(short)*width, 1, fp );
		len  += fWidth;
		len2 += mWidth;
	}

	fOffset += sizeof( short ) * (fWidth * fHeight);
	len = fy * fWidth + fx;
	len2 = my * mWidth + mx;
	for( i = 0; i < height; i++ )
	{
		fseek( fp, sizeof(short)*len+fOffset, SEEK_SET );
		fwrite( &parts[len2], sizeof(short)*width, 1, fp );
		len  += fWidth;
		len2 += mWidth;
	}

	fOffset += sizeof( short ) * (fWidth * fHeight);
	len = fy * fWidth + fx;
	len2 = my * mWidth + mx;
	for( i = 0; i < height; i++ )
	{
		// 読み込み?見たフラグを立てる
		//读取？看了转折点立
		for( j = 0; j < width; j++ )
		{
			event[len2+j] |= (MAP_SEE_FLAG | MAP_READ_FLAG);
			if( nowFloor == floor
			 && (mapAreaX1 <= x1+j && x1+j < mapAreaX2
			  && mapAreaY1 <= y1+i && y1+i < mapAreaY2) )
			{
				// 画面内のエベントが再送されてきたらメモリも書きかえる
				//画面内的エベント再送过来的话会被内存也写青蛙 再次写入内存
				setEventMemory( x1+j, y1+i, event[len2+j] );
			}
		}
		fseek( fp, sizeof(short)*len+fOffset, SEEK_SET );
		fwrite( &event[len2], sizeof(short)*width, 1, fp );
		len  += fWidth;
		len2 += mWidth;
	}

	fclose (fp);

	return TRUE;
}


// マップデータの読み込み
//
//   グリッド座標(x1,y1)-(x2,y2)の範囲のマップデータを読み込む
//地图数据的读取/ /格网坐标（1/ / x，y1)-( x，y两2）的范围的地图数据读取
//
BOOL readMap( int floor, int x1, int y1, int x2, int y2,
	unsigned short *tile, unsigned short *parts, unsigned short *event )
{
	FILE *fp;
	char filename[255];
	int fWidth, fHeight, fOffset;
	int mWidth;
	int width, height;
	int fx, fy;
	int mx, my;
	int len, len2;
	int i;


#if 0
#ifdef _DEBUG_MSG
	{
		char msg[256];
		sprintf( msg, "マップファイル読み込み。[フロア:%d/(%d,%d)-(%d,%d)]",
			floor, x1, y1, x2, y2 );
		StockChatBufferLine( msg, FONT_PAL_GRAY );
	}
#endif
#endif

	// 読み込みファイル名作成
	sprintf( filename, "map\\%d.dat", floor);

	// ファイルオープン
	if( (fp = fopen( filename, "rb" ))==NULL )
	{
		// 失敗するのは最初でマップフォルダが無いから
		// だからフォルダを作る
		_mkdir( "map" );
		// 読み込みデータも作る
		fp = fopen( filename, "wb" );
		fclose( fp );
		// 読み込みモードでもう一度オープン
		if( (fp = fopen( filename, "rb" ))==NULL )
			return FALSE;
	}

	memset( tile, 0, MAP_X_SIZE * MAP_Y_SIZE * sizeof( short ) );
	memset( parts, 0, MAP_X_SIZE * MAP_Y_SIZE * sizeof( short ) );
	memset( event, 0, MAP_X_SIZE * MAP_Y_SIZE * sizeof( short ) );


	fseek( fp, 0, SEEK_SET );
	fread( &fWidth,  sizeof(int), 1, fp );
	fread( &fHeight, sizeof(int), 1, fp );

	mWidth = x2 - x1;
	width = mWidth;
	height = y2 - y1;

	mx = 0;
	fx = x1;
	if( x1 < 0 )
	{
		width += x1;
		fx = 0;
		mx -= x1;
	}
	if( x2 > fWidth )
	{
		width -= (x2 - fWidth);
	}
	my = 0;
	fy = y1;
	if( y1 < 0 )
	{
		height += y1;
		fy = 0;
		my -= y1;
	}
	if( y2 > fHeight )
	{
		height -= (y2 - fHeight);
	}


	fOffset = sizeof(int)*2;
	len = fy * fWidth + fx;
	len2 = my * mWidth + mx;
	for( i = 0; i < height; i++ )
	{
		fseek( fp, sizeof(short)*len+fOffset, SEEK_SET );
		fread( &tile[len2], sizeof(short)*width, 1, fp );
		len  += fWidth;
		len2 += mWidth;
	}

	fOffset += sizeof( short ) * (fWidth * fHeight);
	len = fy * fWidth + fx;
	len2 = my * mWidth + mx;
	for( i = 0; i < height; i++ )
	{
		fseek( fp, sizeof(short)*len+fOffset, SEEK_SET );
		fread( &parts[len2], sizeof(short)*width, 1, fp );
		len  += fWidth;
		len2 += mWidth;
	}

	fOffset += sizeof( short ) * (fWidth * fHeight);
	len = fy * fWidth + fx;
	len2 = my * mWidth + mx;
	for( i = 0; i < height; i++ )
	{
		fseek( fp, sizeof(short)*len+fOffset, SEEK_SET );
		fread( &event[len2], sizeof(short)*width, 1, fp );
		len  += fWidth;
		len2 += mWidth;
	}

	fclose (fp);

	return TRUE;
}


//
// マップ表示位置設定
//
void setMap( int floor, int gx, int gy )
{
	nowFloor = floor;

	setWarpMap( gx, gy );
}


//
// マップ位置移動変更
//
void setMoveMap( int gx, int gy )
{
	// 現在地に移動する（移動とは言わんが）なら何もしない。
	if( nowGx == gx && nowGy == gy )
		return;

	moveStackGx = gx;
	moveStackGy = gy;
	moveStackFlag = TRUE;
}


//
// マップ位置ワープ変更
//
void setWarpMap( int gx, int gy )
{
	nowGx = gx;
	nowGy = gy;
	nowX = (float)nowGx*GRID_SIZE;
	nowY = (float)nowGy*GRID_SIZE;
	nextGx = nowGx;
	nextGy = nowGy;
	nowVx = 0;
	nowVy = 0;
	nowSpdRate = 1;

	oldGx = -1;
	oldGy = -1;
	oldNextGx = -1;
	oldNextGy = -1;

	viewPointX = nowX;
	viewPointY = nowY;

	wnCloseFlag = 1;	// 汎用ウィンドウを閉じる
}


//
// マップ表示
//地图显示
void drawMap( void )
{
	int i, j;
	int x, y, tx, ty;
	S2 xx, yy, ww, hh;
	float dx, dy;
	U4 bmpNo;
	int rainFlag = 0;
	int snowFlag = 0;

	draw_map_bgm_flg = 0;

	readMapAfterFrame++;

	//现在的网格位置变了从文件读取
	// 現在のグリッド位置が変わったらファイルから読み込む
	if( nowGx != oldGx || nowGy != oldGy )
	{
		if( readMap( nowFloor, mapAreaX1, mapAreaY1, mapAreaX2, mapAreaY2,
				&tile[0], &parts[0], &event[0] ) )
		{
			// hitMap[]にあたり判定データを設定する 时判定数据设定
			readHitMap( mapAreaX1, mapAreaY1, mapAreaX2, mapAreaY2,
				&tile[0], &parts[0], &event[0], &hitMap[0] );

			if( mapEmptyFlag )
			{
				if( !checkEmptyMap( mapEmptyDir ) )
				{
					mapEmptyFlag = FALSE;
#if 0
#ifdef _DEBUG_MSG
					{
					char msg[256];
					//sprintf( msg, "データが来たので移動できるようにする。" );
			        sprintf( msg, "数据来了，能移动的。" );
					
					StockChatBufferLine( msg, FONT_PAL_GRAY );
					}
#endif
#endif
					autoMappingInitFlag = TRUE;	// オートマップ読み込み 自动地图读取

				}
				else
				{
#if 0
#ifdef _DEBUG_MSG
					{
					char msg[256];
					sprintf( msg, "まだデータがこないので待つ。还没等数据" );
					StockChatBufferLine( msg, FONT_PAL_GRAY );
					}
#endif
#endif
				}
			}
			else
			{
				autoMappingInitFlag = TRUE;	// オートマップ読み込み
			}

			//地图数据读取的经过框架清晰
			readMapAfterFrame = 0;	// マップデータを読み込んでからの経過フレームをクリア
		}
		else
		{
#if 0
#ifdef _DEBUG_MSG
			{
				char msg[256];
				sprintf( msg, "マップの読み込みに失敗しました。地图的读取失败了" );
				StockChatBufferLine( msg, FONT_PAL_GRAY );
			}
#endif
#endif
			return;
		}
	}

	if( ProcNo == PROC_GAME && SubProcNo == 3 )
	{
		//地图看下4帧后（处理分散）
		if( readMapAfterFrame == 4 )	// マップを読みこんで４フレーム後（処理を分散するため）
		{
			//去了所制定的转折点
			// 行った所のフラグを立てる
			readAutoMapSeeFlag();
		}
		else
		if( readMapAfterFrame == 8 )
		{
			// 立てたフラグを書き込む  立了转折点写入
			writeAutoMapSeeFlag();
		}
	}

	camMapToGamen( (float)(mapAreaX1*GRID_SIZE), (float)((mapAreaY2-1)*GRID_SIZE), &dx, &dy );

#if 1
	// 処理の順番が以下のように処理する  处理顺序以下处理
	//
	// [map]
	//               16
	//            15    14
	//         13    12    11
	//      10     9     8     7
	//          6     5     4
	//             3     2
	//                1
	tx = (int)dx;
	ty = (int)dy;

	int ti, tj;

	ti = mapAreaHeight-1;
	tj = 0;

	

	while( ti >= 0 )
	{
		i = ti;
		j = tj;

		x = tx;
		y = ty;

		while( i >= 0 && j >= 0 )
		{

			// タイル表示 瓷砖表示
			if( tile[i*mapAreaWidth+j] > CG_INVISIBLE )
			{
#if 0
				// 海タイルで音鳴らす(テスト)  海瓷砖声音响（考试）
				if( 193 <= tile[i*mapAreaWidth+j] && tile[i*mapAreaWidth+j] <= 196 )
				{
					play_environment( 0, x, y );
				}
				// 画面内なら処理する  如果画面内处理
				if( x >= -SURFACE_WIDTH/2
				 && x < DEF_APPSIZEX+SURFACE_WIDTH/2
				 && y >= -SURFACE_HEIGHT/2
				 && y < DEF_APPSIZEY+SURFACE_HEIGHT )
				{
					StockDispBuffer( x, y, DISP_PRIO_TILE, tile[i*mapAreaWidth+j], 0 );
				}
#else
				// 画面内なら処理する
				if( x >= -SURFACE_WIDTH/2
				 && x < DEF_APPSIZEX+SURFACE_WIDTH/2
				 && y >= -SURFACE_HEIGHT/2
				 && y < DEF_APPSIZEY+SURFACE_HEIGHT )
				{
					StockDispBuffer( x, y, DISP_PRIO_TILE, tile[i*mapAreaWidth+j], 0 );
				}
#endif
			}
			else
			{
				// 特殊なパーツ（音とか） 特殊的零件（声音）

				// ＳＥパーツなので鳴らす  SE零件，所以响
				if( 20 <= tile[i*mapAreaWidth+j] && tile[i*mapAreaWidth+j] <= 39 )
				{
					play_environment( tile[i*mapAreaWidth+j], x, y );
				}
				else
				// ＢＧＭパーツなので鳴らす  因为零件的BGM敲响
				if( 40 <= tile[i*mapAreaWidth+j] && tile[i*mapAreaWidth+j] <= 59 )
				{
					play_map_bgm( tile[i*mapAreaWidth+j] );
					draw_map_bgm_flg = 1;
				}
			}
			// パーツ表示  零件表示
			if( parts[i*mapAreaWidth+j] > CG_INVISIBLE )
			{
#if 0
				// 海タイルで音鳴らす(テスト)  海瓷砖声音响（考试）
				if( parts[i*mapAreaWidth+j] == 10011 )
				{
					play_environment( 2, x, y );
				} else
				if( parts[i*mapAreaWidth+j] == 10012 )
				{
					play_environment( 1, x, y );
				} else
				if( parts[i*mapAreaWidth+j] == 10203 )
				{
					play_environment( 4, x, y );
				} else
				if( parts[i*mapAreaWidth+j] == 10048 )
				{
					play_map_bgm(2);
					draw_map_bgm_flg = 1;
				}
#endif
				realGetNo( parts[i*mapAreaWidth+j], &bmpNo );
				// 画面内なら処理する
				realGetPos( bmpNo, &xx, &yy );
				realGetWH( bmpNo, &ww, &hh );
				xx += x;
				yy += y;
				if( xx < DEF_APPSIZEX && xx+ww-1 >= 0
				 && yy < DEF_APPSIZEY && yy+hh-1 >= 0 )
				{
					// 表示優先度決定のために登録する  表示优先度为了决定注册
					setPartsPrio( bmpNo, x, y, 0, 0,
						(float)(mapAreaX1+j)*GRID_SIZE, (float)(mapAreaY1+i)*GRID_SIZE );
				}
			}
			else
			{
				// 特殊なパーツ（音とか）
#if 0
// 強制的に雨（雪）を降らす  强制（雪）雨降下
if( nowFloor == 4000 )
{
	if( mapAreaX1+j == 76 && mapAreaY1+i == 90 )
	{
		parts[i*mapAreaWidth+j] = 85;
	}
	else
	if( mapAreaX1+j == 68 && mapAreaY1+i == 83 )
	{
		parts[i*mapAreaWidth+j] = 87;
	}
	else
	if( mapAreaX1+j == 68 && mapAreaY1+i == 76 )
	{
		parts[i*mapAreaWidth+j] = 89;
	}
}
#endif

				// ＳＥパーツなので鳴らす
				if( 20 <= parts[i*mapAreaWidth+j] && parts[i*mapAreaWidth+j] <= 39 )
				{
					play_environment( parts[i*mapAreaWidth+j], x, y );
				}
				else
				// ＢＧＭパーツなので鳴らす
				if( 40 <= parts[i*mapAreaWidth+j] && parts[i*mapAreaWidth+j] <= 59 )
				{
					play_map_bgm( parts[i*mapAreaWidth+j] );
					draw_map_bgm_flg = 1;
				}
				else
				// 雨パーツ
				if( 80 <= parts[i*mapAreaWidth+j] && parts[i*mapAreaWidth+j] <= 84 )
				{
					if( ABS( nowGx - (mapAreaX1+j) ) <= 5
					 && ABS( nowGy - (mapAreaY1+i) ) <= 5 )
					{
						if( rainFlag < parts[i*mapAreaWidth+j] - 79 )
						{
							rainFlag = parts[i*mapAreaWidth+j] - 79;
						}
					}
				}
				else
				// 雪パーツ
				if( 85 <= parts[i*mapAreaWidth+j] && parts[i*mapAreaWidth+j] <= 89 )
				{
					if( ABS( nowGx - (mapAreaX1+j) ) <= 5
					 && ABS( nowGy - (mapAreaY1+i) ) <= 5 )
					{
						if( snowFlag < parts[i*mapAreaWidth+j] - 84 )
						{
							snowFlag = parts[i*mapAreaWidth+j] - 84;
						}
					}
				}
			}

			i--;
			j--;
			x -= SURFACE_WIDTH;
		}

		if( tj < mapAreaWidth - 1 )
		{
			tj++;
			tx += SURFACE_WIDTH/2;
			ty -= SURFACE_HEIGHT/2;
		}
		else
		{
			ti--;
			tx -= SURFACE_WIDTH/2;
			ty -= SURFACE_HEIGHT/2;
		}
	}

#else
	// 処理の順番が以下のように処理する
	//
	// [map] 
	//               16
	//            15    12
	//         14    11     8
	//      13    10     7     4
	//          9     6     3
	//             5     2
	//                1

	x = (int)dx;
	y = (int)dy;

	for( i = mapAreaHeight-1; i >= 0; i-- )
	{
		tx = x;
		ty = y;
		for( j = 0; j < mapAreaWidth; j++ )
		{
			// タイル表示
			if( tile[i*mapAreaWidth+j] > CG_INVISIBLE )
			{
				// 画面内なら処理する
				if( x >= -SURFACE_WIDTH/2
				 && x < DEF_APPSIZEX+SURFACE_WIDTH/2
				 && y >= -SURFACE_HEIGHT/2
				 && y < DEF_APPSIZEY+SURFACE_HEIGHT )
				{
					StockDispBuffer( x, y, DISP_PRIO_TILE, tile[i*mapAreaWidth+j], 0 );
				}
			}

			// パーツ表示
			if( parts[i*mapAreaWidth+j] > CG_INVISIBLE )
			{
				realGetNo( parts[i*mapAreaWidth+j], &bmpNo );
				// 画面内なら処理する
				realGetPos( bmpNo, &xx, &yy );
				realGetWH( bmpNo, &ww, &hh );
				xx += x;
				yy += y;
				if( xx < DEF_APPSIZEX && xx+ww-1 >= 0
				 && yy < DEF_APPSIZEY && yy+hh-1 >= 0 )
				{
					// 表示優先度決定のために登録する
					setPartsPrio( bmpNo, x, y, 0, 0,
						(float)(mapAreaX1+j)*GRID_SIZE, (float)(mapAreaY1+i)*GRID_SIZE );
				}
			}
			x += SURFACE_WIDTH/2;
			y -= SURFACE_HEIGHT/2;
		}
		x = tx-SURFACE_WIDTH/2;
		y = ty-SURFACE_HEIGHT/2;
	}

#endif

	mapEffectRainLevel = rainFlag;
	mapEffectSnowLevel = snowFlag;

	stockCharParts();

	oldGx = nowGx;
	oldGy = nowGy;

#if 0
	if(!draw_map_bgm_flg){
		play_map_bgm(0);
		draw_map_bgm_flg = 1;
	}
#endif

}


// マップファイルを読み込みなおす
//   実際の表示は、drawMapで行われる
// /地图文件重新读取/ /实际的表示，drawMap举行
void redrawMap( void )
{
	oldGx = -1;
	oldGy = -1;
}


// マップファイルのチェックサムを調べる
//   戻り値：同じならTRUE / 違うならFALSE
/// /地图文件查校验/ /返回值：一样的话TRUE /不同的话FALSE
#if 1

//地图文件比特的排列顺序相反的列表
static unsigned char BitTable[] =	/*	ビットの並び順を逆にするテーブル	*/
{
		0x00 , 0x80 , 0x40 , 0xC0 , 0x20 , 0xA0 , 0x60 , 0xE0 , 
		0x10 , 0x90 , 0x50 , 0xD0 , 0x30 , 0xB0 , 0x70 , 0xF0 , 
		0x08 , 0x88 , 0x48 , 0xC8 , 0x28 , 0xA8 , 0x68 , 0xE8 , 
		0x18 , 0x98 , 0x58 , 0xD8 , 0x38 , 0xB8 , 0x78 , 0xF8 , 
		0x04 , 0x84 , 0x44 , 0xC4 , 0x24 , 0xA4 , 0x64 , 0xE4 , 
		0x14 , 0x94 , 0x54 , 0xD4 , 0x34 , 0xB4 , 0x74 , 0xF4 , 
		0x0C , 0x8C , 0x4C , 0xCC , 0x2C , 0xAC , 0x6C , 0xEC , 
		0x1C , 0x9C , 0x5C , 0xDC , 0x3C , 0xBC , 0x7C , 0xFC , 
		0x02 , 0x82 , 0x42 , 0xC2 , 0x22 , 0xA2 , 0x62 , 0xE2 , 
		0x12 , 0x92 , 0x52 , 0xD2 , 0x32 , 0xB2 , 0x72 , 0xF2 , 
		0x0A , 0x8A , 0x4A , 0xCA , 0x2A , 0xAA , 0x6A , 0xEA , 
		0x1A , 0x9A , 0x5A , 0xDA , 0x3A , 0xBA , 0x7A , 0xFA , 
		0x06 , 0x86 , 0x46 , 0xC6 , 0x26 , 0xA6 , 0x66 , 0xE6 , 
		0x16 , 0x96 , 0x56 , 0xD6 , 0x36 , 0xB6 , 0x76 , 0xF6 , 
		0x0E , 0x8E , 0x4E , 0xCE , 0x2E , 0xAE , 0x6E , 0xEE , 
		0x1E , 0x9E , 0x5E , 0xDE , 0x3E , 0xBE , 0x7E , 0xFE , 
		0x01 , 0x81 , 0x41 , 0xC1 , 0x21 , 0xA1 , 0x61 , 0xE1 , 
		0x11 , 0x91 , 0x51 , 0xD1 , 0x31 , 0xB1 , 0x71 , 0xF1 , 
		0x09 , 0x89 , 0x49 , 0xC9 , 0x29 , 0xA9 , 0x69 , 0xE9 , 
		0x19 , 0x99 , 0x59 , 0xD9 , 0x39 , 0xB9 , 0x79 , 0xF9 , 
		0x05 , 0x85 , 0x45 , 0xC5 , 0x25 , 0xA5 , 0x65 , 0xE5 , 
		0x15 , 0x95 , 0x55 , 0xD5 , 0x35 , 0xB5 , 0x75 , 0xF5 , 
		0x0D , 0x8D , 0x4D , 0xCD , 0x2D , 0xAD , 0x6D , 0xED , 
		0x1D , 0x9D , 0x5D , 0xDD , 0x3D , 0xBD , 0x7D , 0xFD , 
		0x03 , 0x83 , 0x43 , 0xC3 , 0x23 , 0xA3 , 0x63 , 0xE3 , 
		0x13 , 0x93 , 0x53 , 0xD3 , 0x33 , 0xB3 , 0x73 , 0xF3 , 
		0x0B , 0x8B , 0x4B , 0xCB , 0x2B , 0xAB , 0x6B , 0xEB , 
		0x1B , 0x9B , 0x5B , 0xDB , 0x3B , 0xBB , 0x7B , 0xFB , 
		0x07 , 0x87 , 0x47 , 0xC7 , 0x27 , 0xA7 , 0x67 , 0xE7 , 
		0x17 , 0x97 , 0x57 , 0xD7 , 0x37 , 0xB7 , 0x77 , 0xF7 , 
		0x0F , 0x8F , 0x4F , 0xCF , 0x2F , 0xAF , 0x6F , 0xEF , 
		0x1F , 0x9F , 0x5F , 0xDF , 0x3F , 0xBF , 0x7F , 0xFF
};
static unsigned short crctab16[] =	/*	crc　の計算テーブル	 计算表	*/
{
		0x0000,  0x1021,  0x2042,  0x3063,  0x4084,  0x50a5,  0x60c6,  0x70e7,
		0x8108,  0x9129,  0xa14a,  0xb16b,  0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
		0x1231,  0x0210,  0x3273,  0x2252,  0x52b5,  0x4294,  0x72f7,  0x62d6,
		0x9339,  0x8318,  0xb37b,  0xa35a,  0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
		0x2462,  0x3443,  0x0420,  0x1401,  0x64e6,  0x74c7,  0x44a4,  0x5485,
		0xa56a,  0xb54b,  0x8528,  0x9509,  0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
		0x3653,  0x2672,  0x1611,  0x0630,  0x76d7,  0x66f6,  0x5695,  0x46b4,
		0xb75b,  0xa77a,  0x9719,  0x8738,  0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
		0x48c4,  0x58e5,  0x6886,  0x78a7,  0x0840,  0x1861,  0x2802,  0x3823,
		0xc9cc,  0xd9ed,  0xe98e,  0xf9af,  0x8948,  0x9969,  0xa90a,  0xb92b,
		0x5af5,  0x4ad4,  0x7ab7,  0x6a96,  0x1a71,  0x0a50,  0x3a33,  0x2a12,
		0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,  0x9b79,  0x8b58,  0xbb3b,  0xab1a,
		0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,  0x2c22,  0x3c03,  0x0c60,  0x1c41,
		0xedae,  0xfd8f,  0xcdec,  0xddcd,  0xad2a,  0xbd0b,  0x8d68,  0x9d49,
		0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,  0x3e13,  0x2e32,  0x1e51,  0x0e70,
		0xff9f,  0xefbe,  0xdfdd,  0xcffc,  0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
		0x9188,  0x81a9,  0xb1ca,  0xa1eb,  0xd10c,  0xc12d,  0xf14e,  0xe16f,
		0x1080,  0x00a1,  0x30c2,  0x20e3,  0x5004,  0x4025,  0x7046,  0x6067,
		0x83b9,  0x9398,  0xa3fb,  0xb3da,  0xc33d,  0xd31c,  0xe37f,  0xf35e,
		0x02b1,  0x1290,  0x22f3,  0x32d2,  0x4235,  0x5214,  0x6277,  0x7256,
		0xb5ea,  0xa5cb,  0x95a8,  0x8589,  0xf56e,  0xe54f,  0xd52c,  0xc50d,
		0x34e2,  0x24c3,  0x14a0,  0x0481,  0x7466,  0x6447,  0x5424,  0x4405,
		0xa7db,  0xb7fa,  0x8799,  0x97b8,  0xe75f,  0xf77e,  0xc71d,  0xd73c,
		0x26d3,  0x36f2,  0x0691,  0x16b0,  0x6657,  0x7676,  0x4615,  0x5634,
		0xd94c,  0xc96d,  0xf90e,  0xe92f,  0x99c8,  0x89e9,  0xb98a,  0xa9ab,
		0x5844,  0x4865,  0x7806,  0x6827,  0x18c0,  0x08e1,  0x3882,  0x28a3,
		0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,  0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
		0x4a75,  0x5a54,  0x6a37,  0x7a16,  0x0af1,  0x1ad0,  0x2ab3,  0x3a92,
		0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,  0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
		0x7c26,  0x6c07,  0x5c64,  0x4c45,  0x3ca2,  0x2c83,  0x1ce0,  0x0cc1,
		0xef1f,  0xff3e,  0xcf5d,  0xdf7c,  0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
		0x6e17,  0x7e36,  0x4e55,  0x5e74,  0x2e93,  0x3eb2,  0x0ed1,  0x1ef0,
};
unsigned short CheckCRC( unsigned char *p , int size )
{
	unsigned short	crc = 0;
	int		i;
	
	for( i = 0 ; i < size ; i ++ ){
		crc = ( crctab16[ ( crc >> 8 ) & 0xFF ] 
					^ ( crc << 8 ) ^ BitTable[ p[ i ] ] );
	}
	return crc;
}


BOOL mapCheckSum( int floor, int x1, int y1, int x2, int y2,
	int tileSum, int partsSum, int eventSum )
{
	int tilesum = 0;
	int objsum = 0;
	int eventsum = 0;
	int databufferindex = 0;
	int width = x2 - x1;
	int height = y2 - y1;
	int i, j;
	unsigned short tile[MAP_X_SIZE*MAP_Y_SIZE];
	unsigned short parts[MAP_X_SIZE*MAP_Y_SIZE];
	unsigned short event[MAP_X_SIZE*MAP_Y_SIZE];


	// タイル?パーツデータを読み出す 计算主题瓷砖零件数据开始读？
#if 0
#ifdef _DEBUG_MSG
	{
	char msg[256];
	sprintf( msg, "チェックサムのために、" );
	StockChatBufferLine( msg, FONT_PAL_GRAY );
	}
#endif
#endif
	readMap( floor, x1, y1, x2, y2, tile, parts, event );

	for( i = 0 ; i < height; i ++ )
	{
        for( j = 0; j < width; j ++ )
		{
			event[i*width+j] &= 0x0fff;
        }
    }

	tilesum  = CheckCRC( (unsigned char *)tile,  27 * 27 * sizeof( short ) );
	objsum   = CheckCRC( (unsigned char *)parts, 27 * 27 * sizeof( short ) );
	eventsum = CheckCRC( (unsigned char *)event, 27 * 27 * sizeof( short ) );

	if( tileSum == tilesum && partsSum == objsum && eventSum == eventsum )
	{
#if 0
#ifdef _DEBUG_MSG
		{
			char msg[256];
			sprintf( msg, "地形のチェックサムが同じなのでファイルから読み込む。计算地形的检查和相同的，所以从文件读取" );
			StockChatBufferLine( msg, FONT_PAL_GRAY );
		}
#endif
#endif

		// ログイン時にマップを読み込むようにする 登录时读取的地图
		if( loginFlag )
		{
			redrawMap();
			loginFlag = FALSE;
		}

		return TRUE;
	}
	else
	{
#if 0
#ifdef _DEBUG_MSG
		{
			char msg[256];
			sprintf( msg, "地形のチェックサムが違うのでサーバへ要求。地形的检查和不同服务器去要求" );
			StockChatBufferLine( msg, FONT_PAL_GRAY );
		}
#endif
#endif
		lssproto_M_send( sockfd, floor, x1, y1, x2, y2 );

#if 0
		// もうこのフラグはいらないのでFALSEにする  已经这个转折点是不需要的，所以FALSE求
		if( loginFlag )
		{
			loginFlag = FALSE;
		}
#endif

		return FALSE;
	}
}
#else
BOOL mapCheckSum( int floor, int x1, int y1, int x2, int y2,
	int tileSum, int partsSum, int eventSum )
{
	int tilesum = 0;
	int objsum = 0;
	int eventsum = 0;
	int databufferindex = 0;
	int width = x2 - x1;
	int height = y2 - y1;
	int i, j;
	unsigned short tile[MAP_X_SIZE*MAP_Y_SIZE];
	unsigned short parts[MAP_X_SIZE*MAP_Y_SIZE];
	unsigned short event[MAP_X_SIZE*MAP_Y_SIZE];


	// タイル?パーツデータを読み出す  瓷砖零件数据开始读？
#if 0
#ifdef _DEBUG_MSG
	{
	char msg[256];
	sprintf( msg, "チェックサムのために、 为了校验，" );
	StockChatBufferLine( msg, FONT_PAL_GRAY );
	}
#endif
#endif
	readMap( floor, x1, y1, x2, y2, tile, parts, event );

	for( i = 0 ; i < height; i ++ )
	{
        for( j = 0; j < width; j ++ )
		{
			tilesum += (tile[i*width+j] % (27*27)) ^ databufferindex;
			objsum += (parts[i*width+j] % (27*27)) ^ databufferindex;
#if 1
			eventsum +=	((event[i*width+j] & 0x0fff) % (27*27))
				^ databufferindex;
#else
			eventsum += (event[i*width+j] % (27*27)) ^ databufferindex;
#endif
        	databufferindex++;
        }
    }

	if( tileSum == tilesum && partsSum == objsum && eventSum == eventsum )
	{
#if 0
#ifdef _DEBUG_MSG
		{
			char msg[256];
			sprintf( msg, "地形のチェックサムが同じなのでファイルから読み込む。地形的检查和相同的，所以从文件读取" );
			StockChatBufferLine( msg, FONT_PAL_GRAY );
		}
#endif
#endif

		// ログイン時にマップを読み込むようにする
		if( loginFlag )
		{
			redrawMap();
			loginFlag = FALSE;
		}

		return TRUE;
	}
	else
	{
#if 0
#ifdef _DEBUG_MSG
		{
			char msg[256];
			sprintf( msg, "地形のチェックサムが違うのでサーバへ要求。登录时读取的地图" );
			StockChatBufferLine( msg, FONT_PAL_GRAY );
		}
#endif
#endif
		lssproto_M_send( sockfd, floor, x1, y1, x2, y2 );

#if 0
		// もうこのフラグはいらないのでFALSEにする
		if( loginFlag )
		{
			loginFlag = FALSE;
		}
#endif

		return FALSE;
	}
}
#endif


// タイル?パーツから当たり判定情報を取り出す。 瓷砖？零件每信息从判定取出。
#if 1
void readHitMap( int x1, int y1, int x2, int y2,
	unsigned short *tile, unsigned short *parts, unsigned short *event, unsigned short *hitMap )
{
	int width, height;
	int i, j, k, l;
	S2 hit, hitX, hitY;
	U4 bmpNo;

	memset( hitMap, 0, MAP_X_SIZE * MAP_Y_SIZE * sizeof( short ) );

	width = x2 - x1;
	height = y2 - y1;

	if( width < 1 || height < 1 )
		return;

	// タイル
	for( i = 0; i < height; i++ )
	{
		for( j = 0; j < width; j++ )
		{
			// タイルの当たり判定
			if( tile[i*width+j] > CG_INVISIBLE
			 || (60 <= tile[i*width+j] && tile[i*width+j] <= 79) )
			{
				realGetNo( tile[i*width+j], &bmpNo );
				// 当たり判定の有無
				realGetHitFlag( bmpNo, &hit );
				// 当たり判定あるならバッファに設定
				if( hit == 0 && hitMap[i*width+j] != 2 )
				{
					hitMap[i*width+j] = 1;
				}
				else
				// hitが2の時はあたり判定なくす
				if( hit == 2 )
				{
					hitMap[i*width+j] = 2;
				}
			}
			else
			{
				// 0から11はデフォルトで見えないタイル
				switch( tile[i*width+j] )
				{
					case 0:	// 0.bmp(無いけど)タイルは当たり判定あるとする
						// 読み込みフラグがたっていない時は処理しない
						//（没有）瓷砖击中的判定。/ /读取转折点过去时，是没有不处理
						if( (event[i*width+j] & MAP_SEE_FLAG) == 0 )
							break;
					case 1:
					case 2:
					case 5:
					case 6:
					case 9:
					case 10:
						// 当たり判定が２の時は１を設定しない 判定2的时候每一不建立
						if( hitMap[i*width+j] != 2 )
						{
							hitMap[i*width+j] = 1;
						}
						break;

					case 4:
						hitMap[i*width+j] = 2;
						break;
				}
			}
		}
	}


	// パーツ
	for( i = 0; i < height; i++ )
	{
		for( j = 0; j < width; j++ )
		{
			// パーツの当たり判定
			if( parts[i*width+j] > CG_INVISIBLE )
			{
				realGetNo( parts[i*width+j], &bmpNo );
				// 当たり判定の有無
				realGetHitFlag( bmpNo, &hit );
				// 当たり判定あるならバッファに設定
				if( hit == 0 )
				{
					realGetHitPoints( bmpNo, &hitX, &hitY );
					for( k = 0; k < hitY; k++ )
					{
						for( l = 0; l < hitX; l++ )
						{
							if( (i - k) >= 0 && (j + l) < width
							 && hitMap[(i-k)*width+j+l] != 2 )
							{
								hitMap[(i-k)*width+j+l] = 1;
							}
						}
					}
				}
				else
				// 当たり判定が２の時はタイルが歩けなくても
				// 強制的に歩けるようにする
				//判定/ /每2的时候是瓷砖也不能走/ /强制走路的
				if( hit == 2 )
				{
					realGetHitPoints( bmpNo, &hitX, &hitY );
					for( k = 0; k < hitY; k++ )
					{
						for( l = 0; l < hitX; l++ )
						{
							if( (i - k) >= 0 && (j + l) < width )
							{
								hitMap[(i-k)*width+j+l] = 2;
							}
						}
					}
				}
			}
			else
			if( 60 <= parts[i*width+j] && parts[i*width+j] <= 79 )
			{
				realGetNo( parts[i*width+j], &bmpNo );
				// 当たり判定の有無
				realGetHitFlag( bmpNo, &hit );
				// 当たり判定あるならバッファに設定
				if( hit == 0 && hitMap[i*width+j] != 2 )
				{
					hitMap[i*width+j] = 1;
				}
				else
				// hitが2の時はあたり判定なくす
				if( hit == 2 )
				{
					hitMap[i*width+j] = 2;
				}
			}
			else
			{
				// 0から11はデフォルトで見えないタイル
				switch( parts[i*width+j] )
				{
					case 1:
					case 2:
					case 5:
					case 6:
					case 9:
					case 10:
						// 当たり判定が２の時は１を設定しない
						if( hitMap[i*width+j] != 2 )
						{
							hitMap[i*width+j] = 1;
						}
						break;

					case 4:
						hitMap[i*width+j] = 2;
						break;
				}
			}

			// 固定ＮＰＣイベントは障害物として扱う 固定NPC活动障碍物处理
			if( (event[i*width+j] & 0x0fff) == EVENT_NPC )
			{
				hitMap[i*width+j] = 1;
			}
		}
	}
}
#else
void readHitMap( int x1, int y1, int x2, int y2,
	unsigned short *tile, unsigned short *parts, unsigned short *event, unsigned short *hitMap )
{
	int width, height;
	int i, j, k, l;
	S2 hit, hitX, hitY;
	U4 bmpNo;

	memset( hitMap, 0, MAP_X_SIZE * MAP_Y_SIZE * sizeof( short ) );

	width = x2 - x1;
	height = y2 - y1;

	if( width < 1 || height < 1 )
		return;

	for( i = 0; i < height; i++ )
	{
		for( j = 0; j < width; j++ )
		{
			// 読み込みフラグがたっていない時は処理しない
			if( (event[i*width+j] & MAP_READ_FLAG) == 0 )
				continue;

			// タイルの当たり判定
			if( tile[i*width+j] > CG_INVISIBLE
#if 0
			 || (60 <= tile[i*width+j] && tile[i*width+j] <= 79) )
#else
			)
#endif
			{
				realGetNo( tile[i*width+j], &bmpNo );
				// 当たり判定の有無
				realGetHitFlag( bmpNo, &hit );
				// 当たり判定あるならバッファに設定
				if( hit == 0 && hitMap[i*width+j] != 2 )
				{
					hitMap[i*width+j] = 1;
				}
				else
				// hitが2の時はあたり判定なくす
				if( hit == 2 )
				{
					hitMap[i*width+j] = 2;
				}
			}
#if 0
			else
			{
				// 0から11はデフォルトで見えないタイル
				switch( tile[i*width+j] )
				{
					case 0:	// 0.bmp(無いけど)タイルは当たり判定あるとする
					case 1:
					case 2:
					case 5:
					case 6:
					case 9:
					case 10:
						// 当たり判定が２の時は１を設定しない
						if( hitMap[i*width+j] != 2 )
						{
							hitMap[i*width+j] = 1;
						}
						break;

					case 4:
						hitMap[i*width+j] = 2;
						break;
				}
			}
#endif

			// パーツの当たり判定
			if( parts[i*width+j] > CG_INVISIBLE )
			{
				realGetNo( parts[i*width+j], &bmpNo );
				// 当たり判定の有無
				realGetHitFlag( bmpNo, &hit );
				// 当たり判定あるならバッファに設定
				if( hit == 0 )
				{
					realGetHitPoints( bmpNo, &hitX, &hitY );
					for( k = 0; k < hitY; k++ )
					{
						for( l = 0; l < hitX; l++ )
						{
							if( (i - k) >= 0 && (j + l) < width
							 && hitMap[(i-k)*width+j+l] != 2 )
							{
								hitMap[(i-k)*width+j+l] = 1;
							}
						}
					}
				}
				else
				// 当たり判定が２の時はタイルが歩けなくても
				// 強制的に歩けるようにする
				if( hit == 2 )
				{
					realGetHitPoints( bmpNo, &hitX, &hitY );
					for( k = 0; k < hitY; k++ )
					{
						for( l = 0; l < hitX; l++ )
						{
							if( (i - k) >= 0 && (j + l) < width )
							{
								hitMap[(i-k)*width+j+l] = 2;
							}
						}
					}
				}
			}
#if 0
			else
			if( 60 <= parts[i*width+j] && parts[i*width+j] <= 79 )
			{
				realGetNo( parts[i*width+j], &bmpNo );
				// 当たり判定の有無
				realGetHitFlag( bmpNo, &hit );
				// 当たり判定あるならバッファに設定
				if( hit == 0 && hitMap[i*width+j] != 2 )
				{
					hitMap[i*width+j] = 1;
				}
				else
				// hitが2の時はあたり判定なくす
				if( hit == 2 )
				{
					hitMap[i*width+j] = 2;
				}
			}
#endif
#if 0
			else
			{
				// 0から11はデフォルトで見えないタイル
				switch( parts[i*width+j] )
				{
					case 1:
					case 2:
					case 5:
					case 6:
					case 9:
					case 10:
						// 当たり判定が２の時は１を設定しない
						if( hitMap[i*width+j] != 2 )
						{
							hitMap[i*width+j] = 1;
						}
						break;

					case 4:
						hitMap[i*width+j] = 2;
						break;
				}
			}
#endif

			// 固定ＮＰＣイベントは障害物として扱う
			if( (event[i*width+j] & 0x0fff) == EVENT_NPC )
			{
				hitMap[i*width+j] = 1;
			}
		}
	}
}
#endif


//
// あたり判定のチェック
//   戻り値：TRUE ... 進入禁止
//
BOOL checkHitMap( int gx, int gy )
{
	int x, y;

	x = gx - mapAreaX1;
	y = gy - mapAreaY1;

	// バッファ範囲外も進入禁止
	if( x < 0 || mapAreaWidth <= x
	 || y < 0 || mapAreaHeight <= y )
	{
		return TRUE;
	}

	// 当たり判定ありなら進入禁止
	if( hitMap[y*mapAreaWidth+x] == 1 )
	{
		return TRUE;
	}

	return FALSE;
}

//
// 移動先にデータが無い時は移動をとめてマップが来るのを待つ。
//前方没有移动数据时移动停止地图来等待。
BOOL checkEmptyMap( int dir )
{
	// キャラの位置から±11のところを調べてデータが無ければTRUEを返す
	//角色的位置开始±11的地方进行调查数据的话，那就TRUE回报
	int i;
	int gx, gy;
	int tx, ty;
	int len;
	BOOL flag = FALSE;


	if( mapAreaWidth < MAP_X_SIZE
	 || mapAreaHeight < MAP_Y_SIZE )
	{
		return FALSE;
	}

	getMapAreaCnt = 0;

	if( dir == 0 || dir == 1 || dir == 2 )
	{
		gx = nowGx - SEARCH_AREA;
		gy = nowGy - SEARCH_AREA;
		tx = -SEARCH_AREA - MAP_TILE_GRID_X1;
		ty = -SEARCH_AREA - MAP_TILE_GRID_Y1;
		len = SEARCH_AREA*2+1;
		for( i = 0; i < len; i++ )
		{
			if( (0 <= gx && gx < nowFloorGxSize)
			 && (0 <= gy && gy < nowFloorGySize) )
			{
				if( event[ty*mapAreaWidth+tx] == 0 )
				{
					getMapAreaX1[getMapAreaCnt] = gx - 1;
					getMapAreaY1[getMapAreaCnt] = gy - 1;
					getMapAreaX2[getMapAreaCnt] = gx + 1;
					getMapAreaY2[getMapAreaCnt] = gy + SEARCH_AREA*2+1;
					checkAreaLimit( &getMapAreaX1[getMapAreaCnt], &getMapAreaY1[getMapAreaCnt],
						&getMapAreaX2[getMapAreaCnt], &getMapAreaY2[getMapAreaCnt] );
					getMapAreaCnt++;
					flag = TRUE;
					break;
				}
			}
			gy++;
			ty++;
		}
	}
	if( dir == 2 || dir == 3 || dir == 4 )
	{
		gx = nowGx - SEARCH_AREA;
		gy = nowGy - SEARCH_AREA;
		tx = -SEARCH_AREA - MAP_TILE_GRID_X1;
		ty = -SEARCH_AREA - MAP_TILE_GRID_Y1;
		len = SEARCH_AREA*2+1;
		for( i = 0; i < len; i++ )
		{
			if( (0 <= gx && gx < nowFloorGxSize)
			 && (0 <= gy && gy < nowFloorGySize) )
			{
				if( event[ty*mapAreaWidth+tx] == 0 )
				{
					getMapAreaX1[getMapAreaCnt] = gx - 1;
					getMapAreaY1[getMapAreaCnt] = gy - 1;
					getMapAreaX2[getMapAreaCnt] = gx + SEARCH_AREA*2+1;
					getMapAreaY2[getMapAreaCnt] = gy + 1;
					checkAreaLimit( &getMapAreaX1[getMapAreaCnt], &getMapAreaY1[getMapAreaCnt],
						&getMapAreaX2[getMapAreaCnt], &getMapAreaY2[getMapAreaCnt] );
					getMapAreaCnt++;
					flag = TRUE;
					break;
				}
			}
			gx++;
			tx++;
		}
	}
	if( dir == 4 || dir == 5 || dir == 6 )
	{
		gx = nowGx + SEARCH_AREA;
		gy = nowGy - SEARCH_AREA;
		tx = SEARCH_AREA - MAP_TILE_GRID_X1;
		ty = -SEARCH_AREA - MAP_TILE_GRID_Y1;
		len = SEARCH_AREA*2+1;
		for( i = 0; i < len; i++ )
		{
			if( (0 <= gx && gx < nowFloorGxSize)
			 && (0 <= gy && gy < nowFloorGySize) )
			{
				if( event[ty*mapAreaWidth+tx] == 0 )
				{
					getMapAreaX1[getMapAreaCnt] = gx;
					getMapAreaY1[getMapAreaCnt] = gy - 1;
					getMapAreaX2[getMapAreaCnt] = gx + 2;
					getMapAreaY2[getMapAreaCnt] = gy + SEARCH_AREA*2+1;
					checkAreaLimit( &getMapAreaX1[getMapAreaCnt], &getMapAreaY1[getMapAreaCnt],
						&getMapAreaX2[getMapAreaCnt], &getMapAreaY2[getMapAreaCnt] );
					getMapAreaCnt++;
					flag = TRUE;
					break;
				}
			}
			gy++;
			ty++;
		}
	}
	if( dir == 6 || dir == 7 || dir == 0 )
	{
		gx = nowGx - SEARCH_AREA;
		gy = nowGy + SEARCH_AREA;
		tx = -SEARCH_AREA - MAP_TILE_GRID_X1;
		ty = SEARCH_AREA - MAP_TILE_GRID_Y1;
		len = SEARCH_AREA*2+1;
		for( i = 0; i < len; i++ )
		{
			if( (0 <= gx && gx < nowFloorGxSize)
			 && (0 <= gy && gy < nowFloorGySize) )
			{
				if( event[ty*mapAreaWidth+tx] == 0 )
				{
					getMapAreaX1[getMapAreaCnt] = gx - 1;
					getMapAreaY1[getMapAreaCnt] = gy;
					getMapAreaX2[getMapAreaCnt] = gx + SEARCH_AREA*2+1;
					getMapAreaY2[getMapAreaCnt] = gy + 2;
					checkAreaLimit( &getMapAreaX1[getMapAreaCnt], &getMapAreaY1[getMapAreaCnt],
						&getMapAreaX2[getMapAreaCnt], &getMapAreaY2[getMapAreaCnt] );
					getMapAreaCnt++;
					flag = TRUE;
					break;
				}
			}
			gx++;
			tx++;
		}
	}


	return flag;
}


// 範囲のリミットチェック
void checkAreaLimit( short *x1, short *y1, short *x2, short *y2 )
{
	if( *x1 < 0 )
		*x1 = 0;
	if( *y1 < 0 )
		*y1 = 0;
	if( *x2 > nowFloorGxSize )
		*x2 = nowFloorGxSize;
	if( *y2 > nowFloorGySize )
		*y2 = nowFloorGySize;
}


///////////////////////////////////////////////////////////////////////////
// ゲーム画面にグリッドカーソルを表示する  游戏画面显示网格光标
//
void drawGrid( void )
{
	float x, y;
	int xx, yy;

	extern int mapWndFontNo[];
	extern int resultWndFontNo[];

	// マップウィンドウのボタンの初期化  地图窗口的按钮的初期化
	if( (MenuToggleFlag & JOY_CTRL_M) == 0 )
	{
		mapWndFontNo[0] = -2;
	}
	// 戦闘結果ウィンドウのボタンの初期化  战斗结果窗口的按钮的初期化
	if( BattleResultWndFlag <= 0 )
	{
		resultWndFontNo[0] = -2;
	}


	camGamenToMap( (float)mouse.nowPoint.x, (float)mouse.nowPoint.y, &x, &y );
	mouseMapX = (int)x;
	mouseMapY = (int)y;
	mouseMapGx = (mouseMapX+GRID_SIZE/2)/GRID_SIZE;
	mouseMapGy = (mouseMapY+GRID_SIZE/2)/GRID_SIZE;
	xx = mouseMapGx*GRID_SIZE;
	yy = mouseMapGy*GRID_SIZE;
	camMapToGamen( (float)xx, (float)yy, &x, &y );


	// マウスカーソルがメニュー等の上にあるなら表示しない  鼠标菜单等上面的话不显示
#if 0
	if( mouse.level < DISP_PRIO_MENU
	 || mapWndFontNo[0] == HitDispNo
	 || resultWndFontNo[0] == HitDispNo )
#else
	if( mouse.level < DISP_PRIO_MENU )
#endif
	{
		// グリッドカーソルの表示
		if( mouseCursorMode == MOUSE_CURSOR_MODE_NORMAL )
		{
			StockDispBuffer( (int)x, (int)y, DISP_PRIO_GRID, CG_GRID_CURSOR, 0 );
		}
		else
		{
			StockDispBuffer( (int)x, (int)y, DISP_PRIO_GRID, 1610, 0 );
		}
	}


	// マウスの検知
	// fieldProc(); moveProc(); で参照されるのでそれまでに実行する
	mouseLeftCrick = FALSE;
	mouseLeftOn = FALSE;
	mouseRightCrick = FALSE;
	mouseRightOn = FALSE;
	if( (mouse.level < DISP_PRIO_MENU && mouse.itemNo == -1)
	 || mapWndFontNo[0] == HitDispNo
	 || resultWndFontNo[0] == HitDispNo )
	{
		if( (mouse.onceState & MOUSE_LEFT_CRICK) )
		{
			mouseLeftCrick = TRUE;
			mouseLeftPushTime = 0;
			beforeMouseLeftPushTime = GetTickCount();
		}
		if( (mouse.state & MOUSE_LEFT_CRICK) )
		{
			mouseLeftOn = TRUE;
			if( beforeMouseLeftPushTime > 0 )
			{
				mouseLeftPushTime = GetTickCount() - beforeMouseLeftPushTime;
			}
		}
		else
		{
			mouseLeftPushTime = 0;
			beforeMouseLeftPushTime = 0;
		}
		if( (mouse.onceState & MOUSE_RIGHT_CRICK) )
		{
			mouseRightCrick = TRUE;
		}
		if( (mouse.state & MOUSE_RIGHT_CRICK) )
		{
			mouseRightOn = TRUE;
		}

		// マップウィンドウのボタンが押されても移動しない様にする
		// 戦闘結果ウィンドウのボタンが押されても移動しない様にする
		if( mapWndFontNo[0] == HitDispNo
		 || resultWndFontNo[0] == HitDispNo )
			mouseLeftCrick = FALSE;
	}
	else
	{
		mouseCursorMode = MOUSE_CURSOR_MODE_NORMAL;
		mouseLeftPushTime = 0;
		beforeMouseLeftPushTime = 0;
	}
}


// 移動処理リセット
void resetMoveProc( void )
{
}


///////////////////////////////////////////////////////////////////////////
// 移動処理
void moveProc( void )
{
	static unsigned int befortime = -1;


#ifdef _DEBUG
	{
		static short tglSw = 0;

		if( joy_trg[ 1 ] & JOY_F11 )
		{
			tglSw++;
			tglSw %= 8;
		}
		if( tglSw == 1 )
		{
			char msg[256];
			sprintf( msg, "EN send        : %d", sendEnFlag );
			StockFontBuffer( 240, 16, FONT_PRIO_FRONT, 0, msg, 0 );
			sprintf( msg, "EV (Warp) send : %d", eventWarpSendFlag );
			StockFontBuffer( 240, 32, FONT_PRIO_FRONT, 0, msg, 0 );
			sprintf( msg, "EV (Enemy) send: %d", eventEnemySendFlag );
			StockFontBuffer( 240, 48, FONT_PRIO_FRONT, 0, msg, 0 );
			sprintf( msg, "Empty          : %d", mapEmptyFlag );
			StockFontBuffer( 240, 64, FONT_PRIO_FRONT, 0, msg, 0 );
			sprintf( msg, "  nowGx      = %3d/ nowGy      = %3d", nowGx, nowGy );
			StockFontBuffer( 240, 80, FONT_PRIO_FRONT, 0, msg, 0 );
			sprintf( msg, "  mapEmptyGx = %3d/ mapEmptyGy = %3d", mapEmptyGx, mapEmptyGy );
			StockFontBuffer( 240, 96, FONT_PRIO_FRONT, 0, msg, 0 );
			sprintf( msg, "  mapEmptyDir = %3d", mapEmptyDir );
			StockFontBuffer( 240, 112, FONT_PRIO_FRONT, 0, msg, 0 );
		}
		else
		if( tglSw == 2 )
		{
			char msg[256];
			sprintf( msg, "nowEncountPercentage : %d", nowEncountPercentage );
			StockFontBuffer( 240, 16, FONT_PRIO_FRONT, 0, msg, 0 );
			sprintf( msg, "nowEncountExtra      : %d", nowEncountExtra );
			StockFontBuffer( 240, 32, FONT_PRIO_FRONT, 0, msg, 0 );
		}
		else
		if( tglSw == 3
		 || tglSw == 4 )
		{
			// あたり判定表示（デバッグ）
			// イベント判定表示（デバッグ）
			char msg[256];
			int x, y, xx, yy;

			xx = -MAP_TILE_GRID_X1;
			yy = -MAP_TILE_GRID_Y1;

			if( MAP_X_SIZE > mapAreaWidth )
			{
				if( mapAreaX1 == 0 )
				xx -= (MAP_X_SIZE - mapAreaWidth);
			}
			if( MAP_Y_SIZE > mapAreaHeight )
			{
				if( mapAreaY1 == 0 )
				yy -= (MAP_Y_SIZE - mapAreaHeight);
			}

			for( y = 0; y < mapAreaHeight && y < 26; y++ )
			{
				for( x = 0; x < mapAreaWidth; x++ )
				{
					if( tglSw == 3 )
					{
						sprintf( msg, "%d", hitMap[y*mapAreaWidth+x] );
					}
					else
					{
						sprintf( msg, "%d", (event[y*mapAreaWidth+x] & 0xfff) );
					}
					if( x == xx && y == yy )
					{
						StockFontBuffer( x*10, y*18, FONT_PRIO_FRONT, FONT_PAL_RED, msg, 0 );
					}
					else
					{
						StockFontBuffer( x*10, y*18, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0 );
					}
				}
			}
			sprintf( msg, "gx = %5d / gy = %5d / hit = %d",
				mouseMapGx, mouseMapGy, checkHitMap( mouseMapGx, mouseMapGy ) );
			StockFontBuffer( 332, 40, FONT_PRIO_FRONT, 0, msg, 0 );
		}
		else
		if( tglSw == 5 )
		{
			char msg[256];
			int x, y;

			x = mouseMapGx - mapAreaX1;
			y = mouseMapGy - mapAreaY1;
			sprintf( msg, "Tile  Bmp = %d", tile[y*mapAreaWidth+x] );
			StockFontBuffer( 240, 34, FONT_PRIO_FRONT, 0, msg, 0 );
			sprintf( msg, "Parts Bmp = %d", parts[y*mapAreaWidth+x] );
			StockFontBuffer( 240, 54, FONT_PRIO_FRONT, 0, msg, 0 );
			sprintf( msg, "Gx = %d / Gy = %d", mouseMapGx, mouseMapGy );
			StockFontBuffer( 240, 74, FONT_PRIO_FRONT, 0, msg, 0 );
		}
		else
		if( tglSw == 6 )
		{
			char msg[256];

			if( (joy_trg[ 0 ] & JOY_RIGHT) && mapEffectRainLevel < 5 )
			{
				mapEffectRainLevel++;
			}
			else
			if( (joy_trg[ 0 ] & JOY_LEFT) && mapEffectRainLevel > 0 )
			{
				mapEffectRainLevel--;
			}

			sprintf( msg, "mapEffectRainLevel = %d", mapEffectRainLevel );
			StockFontBuffer( 240, 34, FONT_PRIO_FRONT, 0, msg, 0 );
		}
		else
		if( tglSw == 7 )
		{
			char msg[256];

			if( (joy_trg[ 0 ] & JOY_RIGHT) && mapEffectSnowLevel < 5 )
			{
				mapEffectSnowLevel++;
			}
			else
			if( (joy_trg[ 0 ] & JOY_LEFT) && mapEffectSnowLevel > 0 )
			{
				mapEffectSnowLevel--;
			}

			sprintf( msg, "mapEffectSnowLevel = %d", mapEffectSnowLevel );
			StockFontBuffer( 240, 34, FONT_PRIO_FRONT, 0, msg, 0 );
		}
	}
#endif


#if 0
	// 呪術エフェクト
	if( joy_trg[ 1 ] & JOY_F9 )
	{
		setPcUseMagic();
		//createCommmonEffectNoLoop( SPR_effect01, nowGx, nowGy, 0, 0, DISP_PRIO_CHAR );
	}
#endif


#if 0
	{
		static char msg[256];
		sprintf( msg, "mouse.state = 0x%02x / mouse.onceState = 0x%02x",
			mouse.state, mouse.onceState );
		StockFontBuffer( 240, 16, FONT_PRIO_FRONT, 0, msg, 0 );
	}
#endif


#if 0
	// マウス情報表示（デバッグ）
	{
		static char msg[256];

		sprintf( msg, "moveStackGx = %5d", moveStackGx );
		StockFontBuffer( 300, 40, FONT_PRIO_FRONT, 0, msg, 0 );
		sprintf( msg, "moveStackGy = %5d", moveStackGy );
		StockFontBuffer( 300, 56, FONT_PRIO_FRONT, 0, msg, 0 );
		sprintf( msg, "mouseMapX = %5d/ mouseMapGx = %5d", mouseMapX, mouseMapGx );
		StockFontBuffer( 300, 72, FONT_PRIO_FRONT, 0, msg, 0 );
		sprintf( msg, "mouseMapY = %5d/ mouseMapGy = %5d", mouseMapY, mouseMapGy );
		StockFontBuffer( 300, 88, FONT_PRIO_FRONT, 0, msg, 0 );
		sprintf( msg, "mouseX = %5d", mouse.nowPoint.x );
		StockFontBuffer( 300, 104, FONT_PRIO_FRONT, 0, msg, 0 );
		sprintf( msg, "mouseY = %5d", mouse.nowPoint.y );
		StockFontBuffer( 300, 120, FONT_PRIO_FRONT, 0, msg, 0 );
		sprintf( msg, "leftPushTime  = %d", mouse.leftPushTime );
		StockFontBuffer( 300, 136, FONT_PRIO_FRONT, 0, msg, 0 );
		sprintf( msg, "RightPushTime = %d", mouse.rightPushTime );
		StockFontBuffer( 300, 152, FONT_PRIO_FRONT, 0, msg, 0 );
		sprintf( msg, "nextGx = %d/ nextGy = %d", nextGx, nextGy );
		StockFontBuffer( 300, 168, FONT_PRIO_FRONT, 0, msg, 0 );
	}
#endif


#if 0
	{
		static char msg[256];
		sprintf( msg, "partyModeFlag = %d", partyModeFlag );
		StockFontBuffer( 200, 40, FONT_PRIO_FRONT, 0, msg, 0 );
		sprintf( msg, "useflag = %d %d %d %d %d", 
			party[0].useFlag,
			party[1].useFlag,
			party[2].useFlag,
			party[3].useFlag,
			party[4].useFlag );
		StockFontBuffer( 200, 56, FONT_PRIO_FRONT, 0, msg, 0 );
		sprintf( msg, "ptAct = %p %p", party[0].ptAct, party[1].ptAct );
		StockFontBuffer( 200, 72, FONT_PRIO_FRONT, 0, msg, 0 );
		
	}
#endif


	// 各種イベントフラグのチェック
	if( sendEnFlag == 0
	 && eventWarpSendFlag == 0
	 && eventEnemySendFlag == 0 )
	{
		etcEventFlag = 0;
	}


	// 隣接する掲示板等を左クリックしたら
	// サーバに見るプロトコルを送る
	if( mouseLeftCrick )
	{
		if( lookAtAround() )
		{
			mouseLeftCrick = FALSE;
		}
	}

	// その場で向き変え＆アイテム拾い  右键  在那个场合的方向改变&道具捡起来
	if( mouseRightCrick )
	{
		// その場向き変え  方向改变
		turnAround();
		// アイテム拾う  道具捡起来
		getItem();

		//触发NPC 右键发送点消息？临时测试下
		chatStrSendForServer( "", 1 );
	}



	// 移動先の選択
	// 単独かリーダーの時だけ入力を受け付ける
	// 後、イベントとか起こっていないこと
	if( (partyModeFlag == 0 || (pc.status & CHR_STATUS_LEADER) != 0)
	 && etcSendFlag == 0
	 && etcEventFlag == 0 )
	{
		// 左ボタンを数秒押すと移動モードに移行
		if( mouseLeftPushTime >= MOVE_MODE_CHANGE_TIME )
		{
			mouseCursorMode = MOUSE_CURSOR_MODE_MOVE;
		}

		// 移動モードの処理
		if( mouseCursorMode == MOUSE_CURSOR_MODE_MOVE )
		{
			// クリックされたら通常モードに戻る
			if( mouseLeftCrick )
			{
				mouseCursorMode = MOUSE_CURSOR_MODE_NORMAL;
			}
			else
			// カーソルの指す位置を移動先とする
			{
				// マウスの連射を抑制
				if( befortime+MOVE_CLICK_WAIT_TIME <= GetTickCount() )
				{
					befortime = GetTickCount();
					moveStackGx = mouseMapGx;
					moveStackGy = mouseMapGy;
					moveStackFlag = TRUE;
				}
			}
		}
		else
		// 通常モードの処理
		if( mouseLeftCrick )
		{
			// カーソルの指す位置を移動先とする
			// マウスの連射を抑制
			if( befortime+MOVE_CLICK_WAIT_TIME <= GetTickCount() )
			{
				befortime = GetTickCount();
				moveStackGx = mouseMapGx;
				moveStackGy = mouseMapGy;
				moveStackFlag = TRUE;
			}
		}
	}



	// パーティー番号詰めて取り出す
	getPartyTbl();

	// 一人かリーダーの時の移動処理
	if( partyModeFlag == 0
	 || (pc.status & CHR_STATUS_LEADER) != 0 )
	{
		onceMoveProc();
	}
	else
	// パーティの子供の時の移動処理
	{
		partyMoveProc();
	}
	return;
}



short _encountFlag = 0;
short _warpEventFlag = 0;
short _enemyEventFlag = 0;

int  _enemyEventDir;


short justGoalFlag = 0;	// 目的地に着いた後すぐかどうか

short _partyTbl[MAX_PARTY];	// パーティー情報を詰めるテーブル

void _etcEventCheck( void );
BOOL _execEtcEvent( void );
void _getMoveRoute2( void );

void setPcMovePointToChar( int, int );
void setPartyMovePoint( void );
void _mapMove( void );
void _partyMapMove( void );
void _setMapMovePoint( int, int );


BOOL _checkEncount( void );
void _sendEncount( void );
void _sendMoveRoute( void );
BOOL _checkWarpEvent( int, int );
void _sendWarpEvent( void );
BOOL _checkEnemyEvent( int, int );
void _sendEnemyEvent( void );
void _checkEmptyMap( void );
BOOL checkEmptyMapData( int, int, int );



// プレーヤー一人で移動
void onceMoveProc( void )
{
	int dir;

	// ＰＣが基準に歩くときは加速する必要無い
	nowSpdRate = 1.0F;

	// マップ空なので移動処理をしない
	if( mapEmptyFlag )
		return;


	if( sendEnFlag == 0				// エンカウント
	 && eventWarpSendFlag == 0		// ワープイベント
	 && eventEnemySendFlag == 0		// 敵イベント
	)
	{
		// イベント等の実行
		if( _execEtcEvent() )
			return;

		// グリッドの中心にいる時の処理
		if( (float)nextGx*GRID_SIZE == nowX && (float)nextGy*GRID_SIZE == nowY )
		{
			// 移動先が設定されていたら移動経路を算出
			if( moveStackFlag
			 && moveRouteCnt2 == 0 )
			{
				moveStackFlag = FALSE;
				getRouteMap();
				if( moveRouteCnt == 0 )
				{
					// 移動経路探したが無かった時向きだけ変えておく
					turnAround();
				}
			}

			// 移動後に目的地じゃないときその方向を向ける
			if( moveRouteCnt == 0 && moveRouteCnt2 == 0 )
			{
				turnAround2( moveLastDir );
				moveLastDir = -1;
			}

			// サーバへ送るデータを取り出す
			if( moveRouteCnt > 0 && moveRouteCnt2 == 0 )
			{
				// 移動経路からサーバに送る最大数分取り出す
				_getMoveRoute2();

				if( moveRouteCnt2 > 0 )
				{
					// 取り出した経路上でイベント等が発生するかチェック
					_etcEventCheck();
				}

				// サーバに移動経路送る
				_sendMoveRoute();
			}


			// 移動経路があるので移動量を算出
			if( moveRouteCnt2 > 0 )
			{
				// マップが空ならフラグ立ててまつ
				_checkEmptyMap();

				// 移動先を設定し移動量を計算する
				dir = moveRoute2[0];
				shiftRouteMap2();
				setMapMovePoint( nowGx+moveAddTbl[dir][0], nowGy+moveAddTbl[dir][1] );

				// ＰＣがリーダーの時で移動した瞬間に
				// 次のパーティキャラに現在の座標を設定してやる
				setPcMovePointToChar( nowGx, nowGy );
			}
		}
	}

	// ＰＣがリーダーの時、他のパーティーの移動先を設定する
	setPartyMovePoint();


	// イベントとかのチェックが終わったら初期化しておく
	justGoalFlag = 0;

	// マップ空なので移動処理をしない
	if( mapEmptyFlag )
		return;

	// ＰＣ移動処理（移動量がある時だけ）
	_mapMove();

	// パーティーの移動処理
	_partyMapMove();

	mapAreaX1 = nowGx+MAP_TILE_GRID_X1;
	mapAreaY1 = nowGy+MAP_TILE_GRID_Y1;
	mapAreaX2 = nowGx+MAP_TILE_GRID_X2;
	mapAreaY2 = nowGy+MAP_TILE_GRID_Y2;

	if( mapAreaX1 < 0 )
		mapAreaX1 = 0;
	if( mapAreaY1 < 0 )
		mapAreaY1 = 0;
	if( mapAreaX2 > nowFloorGxSize )
		mapAreaX2 = nowFloorGxSize;
	if( mapAreaY2 > nowFloorGySize )
		mapAreaY2 = nowFloorGySize;

	mapAreaWidth  = mapAreaX2 - mapAreaX1;
	mapAreaHeight = mapAreaY2 - mapAreaY1;

	viewPointX = nowX;
	viewPointY = nowY;
}


// ＮＰＣがリーダーの時の処理
void partyMoveProc( void )
{
	int i;
	ACTION *ptAct, *ptActNext;

	// キャラがグリッドの中心に来てまだ移動先があるなら設定。
	// そして、後ろにパーティがいたら今の座標を設定してやる。
	for( i = 0; i < MAX_PARTY; i++ )
	{
		if( _partyTbl[i] >= 0 )
		{
			ptAct = party[_partyTbl[i]].ptAct;

			// ＮＰＣの時
			if( party[_partyTbl[i]].id != pc.id )
			{
				// グリッドの中心に来たら次の移動先を設定する
				if( (float)ptAct->nextGx*GRID_SIZE == ptAct->mx
				 && (float)ptAct->nextGy*GRID_SIZE == ptAct->my )
				{
					// バッファに次の座標があるので設定
					if( ptAct->bufCount > 0 )
					{
						// バッファがたまってたら加速する
						if( i == 0 )
						{
							nowSpdRate = 1.0F;
							if( ptAct->bufCount > 5 )
							{
								nowSpdRate = 2.0F;
							}
							else
							if( ptAct->bufCount >= 4 )
							{
								nowSpdRate = 1.6F;
							}
							else
							if( ptAct->bufCount >= 2 )
							{
								nowSpdRate = 1.2F;
							}
						}
						_setCharMovePoint( ptAct, ptAct->bufGx[0], ptAct->bufGy[0] );
						shiftBufCount( ptAct );
						if( _partyTbl[i+1] >= 0
						 && (i+1) < MAX_PARTY )
						{
							ptActNext = party[_partyTbl[i+1]].ptAct;
							stockCharMovePoint( ptActNext, ptAct->gx, ptAct->gy );
						}
					}
				}
			}
			// ＰＣの時
			else
			{
				// グリッドの中心に来たら次の移動先を設定する
				if( (float)nextGx*GRID_SIZE == nowX
				 && (float)nextGy*GRID_SIZE == nowY )
				{
					// バッファに次の座標があるので設定
					if( ptAct->bufCount > 0 )
					{
						_setMapMovePoint( ptAct->bufGx[0], ptAct->bufGy[0] );
						shiftBufCount( ptAct );
						if( _partyTbl[i+1] >= 0
						 && (i+1) < MAX_PARTY )
						{
							ptActNext = party[_partyTbl[i+1]].ptAct;
							stockCharMovePoint( ptActNext, ptAct->gx, ptAct->gy );
						}
					}
				}
			}
		}
		else
		{
			break;
		}
	}


	// 移動処理
	for( i = 0; i < MAX_PARTY; i++ )
	{
		if( _partyTbl[i] >= 0 )
		{
			if( party[_partyTbl[i]].id != pc.id )
			{
				_charMove( party[_partyTbl[i]].ptAct );
			}
			else
			{
				//mapMove2();
				_mapMove();
			}
		}
		else
		{
			break;
		}
	}

	mapAreaX1 = nowGx+MAP_TILE_GRID_X1;
	mapAreaY1 = nowGy+MAP_TILE_GRID_Y1;
	mapAreaX2 = nowGx+MAP_TILE_GRID_X2;
	mapAreaY2 = nowGy+MAP_TILE_GRID_Y2;

	if( mapAreaX1 < 0 )
		mapAreaX1 = 0;
	if( mapAreaY1 < 0 )
		mapAreaY1 = 0;
	if( mapAreaX2 > nowFloorGxSize )
		mapAreaX2 = nowFloorGxSize;
	if( mapAreaY2 > nowFloorGySize )
		mapAreaY2 = nowFloorGySize;

	mapAreaWidth  = mapAreaX2 - mapAreaX1;
	mapAreaHeight = mapAreaY2 - mapAreaY1;

	viewPointX = nowX;
	viewPointY = nowY;
}


///////////////////////////////////////////////////////////////////////////
// マップに移動先を設定
void _setMapMovePoint( int _nextGx, int _nextGy )
{
	float dx, dy;
	float len;
	float dir1;
	int dir;

	nextGx = _nextGx;
	nextGy = _nextGy;

	// 移動量を求める
	dx = nextGx*GRID_SIZE-nowX;
	dy = nextGy*GRID_SIZE-nowY;
	len = (float)sqrt( (double)(dx*dx+dy*dy) );
	if( len > 0 )
	{
		dx /= len;
		dy /= len;
	}
	else
	{
		dx = 0;
		dy = 0;
	}
	nowVx = dx * MOVE_SPEED;
	nowVy = dy * MOVE_SPEED;

	// PCキャラの方向設定
	if( dx != 0 || dy != 0 )
	{
		dir1 = Atan( dx, dy ) + 22.5F;
		AdjustDir( &dir1 );
		dir = (int)(dir1/45);
		setPcDir( dir );
		setPcWalkFlag();
	}

	// PCキャラをマップの位置と同期する
	setPcPoint();
}


///////////////////////////////////////////////////////////////////////////
// ＰＣがリーダーの時、移動した瞬間に
// 次のパーティキャラに現在の座標を設定してやる
void setPcMovePointToChar( int gx, int gy )
{
	if( partyModeFlag != 0 && (pc.status & CHR_STATUS_LEADER) != 0 )
	{
		if( _partyTbl[1] >= 0 )
		{
			stockCharMovePoint( party[_partyTbl[1]].ptAct, gx, gy );
		}
	}
}


///////////////////////////////////////////////////////////////////////////
// ＰＣがリーダーの時、移動したら他のキャラも移動させる
void setPartyMovePoint( void )
{
	int i;
	ACTION *ptAct, *ptActNext;

	if( partyModeFlag != 0 && (pc.status & CHR_STATUS_LEADER) != 0 )
	{
		for( i = 1; i < MAX_PARTY; i++ )
		{
			if( _partyTbl[i] >= 0 )
			{
				ptAct = party[_partyTbl[i]].ptAct;

				// キャラがグリッドの中心か
				if( (float)ptAct->nextGx*GRID_SIZE == ptAct->mx
				 && (float)ptAct->nextGy*GRID_SIZE == ptAct->my )
				{
					// バッファに次の座標があるので設定
					if( ptAct->bufCount > 0 )
					{
						setCharMovePoint( ptAct, ptAct->bufGx[0], ptAct->bufGy[0] );
						shiftBufCount( ptAct );
						// 次のパーティに座標を渡す
						if( _partyTbl[i+1] >= 0
						 && (i+1) < MAX_PARTY )
						{
							ptActNext = party[_partyTbl[i+1]].ptAct;
							stockCharMovePoint( ptActNext, ptAct->gx, ptAct->gy );
						}
					}
				}
			}
			else
			{
				break;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////
// 移動の計算
void _mapMove( void )
{
	float nGx, nGy;
	float vx, vy;

	if( nowVx != 0 || nowVy != 0 )
	{
		nGx = (float)nextGx*GRID_SIZE;
		nGy = (float)nextGy*GRID_SIZE;

		vx = nowVx * nowSpdRate;
		vy = nowVy * nowSpdRate;

		// 現在地と目的地の距離が移動量以下なら目的地に到着
		if( pointLen2( nowX, nowY, nGx, nGy )
		 <= vx*vx+vy*vy )
		{
			// 目的地に着いたらパラメータ初期化
			nowX = nGx;
			nowY = nGy;
			nowVx = 0;
			nowVy = 0;
			justGoalFlag = 1;	// 今、目的地に着いた
		}
		else
		{
			// 移動
			nowX += vx;
			nowY += vy;
		}
		setPcAction( ANIM_WALK );
		setPcWalkFlag();

		if( pc.ptAct != NULL )
		{
			mapEffectMoveDir = pc.ptAct->anim_ang;
		}
	}
	else
	{
		// PCキャラのアニメーションがWALKの次のフレームならならSTANDにする
		if( checkPcWalkFlag() == 1 )
		{
			setPcAction( ANIM_STAND );
			delPcWalkFlag();
		}
		mapEffectMoveDir = -1;
	}

	nowGx = (int)(nowX/GRID_SIZE);
	nowGy = (int)(nowY/GRID_SIZE);

	// PCキャラをマップの位置と同期する
	setPcPoint();
}


///////////////////////////////////////////////////////////////////////////
// ＰＣがリーダーの時のパーティー
void _partyMapMove( void )
{
	int i;

	// ＰＣがリーダーの時で移動したら他のキャラも移動する
	if( partyModeFlag != 0 && (pc.status & CHR_STATUS_LEADER) != 0 )
	{
		for( i = 1; i < MAX_PARTY; i++ )
		{
			if( _partyTbl[i] >= 0 )
			{
				charMove2( party[_partyTbl[i]].ptAct );
			}
			else
			{
				break;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////
// パーティー情報を詰める
void getPartyTbl( void )
{
	int i, j;

	if( partyModeFlag != 0 )
	{
		// テーブル初期化
		for( i = 0; i < MAX_PARTY; i++ )
		{
			_partyTbl[i] = -1;
		}
		// パーティー番号を詰めて取り出す
		for( i = 0, j = 0; i < MAX_PARTY; i++ )
		{
			if( party[i].useFlag != 0 && party[i].ptAct != NULL )
			{
				_partyTbl[j] = i;
				j++;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////
// 各種エベントチェック＆実行

enum
{
	etcEventMode_None,
	etcEventMode_Warp,
	etcEventMode_LocalEncount,
	etcEventMode_Enemy
};

BOOL _etcEventFlag = FALSE;
short _etcEventStep = 0;
short _etcEventMode = etcEventMode_None;
short _eventWarpNo;


void _etcEventCheck( void )
{
	int i;
	int dir;
	int gx, gy;
	int ogx, ogy;
	BOOL breakFlag;
	float tmpDir;


	gx = nowGx;
	gy = nowGy;
	ogx = gx;
	ogy = gy;
	breakFlag = FALSE;

	// これから移動しようとする経路で何かが起こるかチェックする
	for( i = 0; i < moveRouteCnt2; i++ )
	{
		gx += moveAddTbl[moveRoute2[i]][0];
		gy += moveAddTbl[moveRoute2[i]][1];
		// 目的地の方向を調べる
		tmpDir = Atan( (float)(gx-ogx), (float)(gy-ogy) ) + 22.5F;
		AdjustDir( &tmpDir );
		dir = (int)(tmpDir/45);

		// 固定エンカウントチェック
		if( _checkEnemyEvent( gx, gy ) )
		{
			_etcEventFlag = TRUE;
			_etcEventStep = i;
			_etcEventMode = etcEventMode_Enemy;
			_enemyEventDir = dir - 3;
			if( _enemyEventDir < 0 )
				_enemyEventDir += 8;
			i--;
			breakFlag = TRUE;
			break;
		}
		else
		// ワープイベントチェック
		if( _checkWarpEvent( gx, gy ) )
		{
			_etcEventFlag = TRUE;
			_etcEventStep = i+1;
			_etcEventMode = etcEventMode_Warp;
			breakFlag = TRUE;
			break;
		}
		else
		// ローカルエンカウントチェック
		if( _checkEncount() )
		{
			_etcEventFlag = TRUE;
			_etcEventStep = i+1;
			_etcEventMode = etcEventMode_LocalEncount;
			breakFlag = TRUE;
			break;
		}
		ogx = gx;
		ogy = gy;
	}
	// 移動途中で何かが起こるのでそれ以降の移動経路はいらない
	if( i < MOVE_MAX2 && breakFlag )
	{
		moveRouteCnt2 = i+1;
		moveRouteCnt = 0;
	}
}


BOOL _execEtcEvent( void )
{
	if( _etcEventFlag )
	{
		if( justGoalFlag )
		{
			if( _etcEventStep > 0 )
				_etcEventStep--;
		}

		if( _etcEventStep <= 0 )
		{
			switch( _etcEventMode )
			{
				// ワープイベント発動
				case etcEventMode_Warp:
					_sendWarpEvent();
					break;

				// ローカルエンカウント発動
				case etcEventMode_LocalEncount:
					_sendEncount();
					break;

				// 敵イベントにあたった
				case etcEventMode_Enemy:
					_sendEnemyEvent();
					break;
			}

			_etcEventFlag = FALSE;
			_etcEventStep = 0;
			return TRUE;
		}
	}

	return FALSE;

}


///////////////////////////////////////////////////////////////////////////
// 移動経路からサーバに送る最大数分取り出す
void _getMoveRoute2( void )
{
	int i;

	for( i = 0; i < MOVE_MAX2 && moveRouteCnt > 0; i++ )
	{
		moveRoute2[moveRouteCnt2] = moveRoute[0];
		moveRouteCnt2++;
		shiftRouteMap();
	}
}


///////////////////////////////////////////////////////////////////////////
// サーバに移動経路送る
void _sendMoveRoute( void )
{
	int i;

	// データ無いなら何もしない
	if( moveRouteCnt2 <= 0 )
		return;

	// サーバに移動経路送る
	for( i = 0; i < moveRouteCnt2; i++ )
	{
		moveRouteDir[i] = cnvServDir( moveRoute2[i], 0 );
	}
#ifdef _DEBUG
	if( !offlineFlag )
#endif
	{
		// 誰の仲間でもない時に送る
		if( partyModeFlag == 0 || (pc.status & CHR_STATUS_LEADER) != 0 )
		{
			moveRouteDir[i] = '\0';

#ifdef MAP_CACHE_PROC
			if( checkMapCache( nowGx, nowGy, moveRoute2, moveRouteCnt2,
				nowFloor, nowFloorGxSize, nowFloorGySize ) )
			{
				// キャッシュがあったのでチェックサム無し
				noChecksumWalkSendForServer( nowGx, nowGy, moveRouteDir );
			}
			else
			{
				// キャッシュが無いのでチェックサムあり
				walkSendForServer( nowGx, nowGy, moveRouteDir );
			}
#else
			walkSendForServer( nowGx, nowGy, moveRouteDir );
#endif
		}
	}
#if 0
#ifdef _DEBUG_MSG
	{
		char msg[256];
		sprintf( msg, "サーバに移動経路送る(%d %d %s)", nowGx, nowGy, moveRouteDir );
		StockChatBufferLine( msg, FONT_PAL_GRAY );
	}
#endif
#endif
}


///////////////////////////////////////////////////////////////////////////
// エンカウント処理
//
// エンカウントしたかチェック
BOOL _checkEncount( void )
{
	BOOL ret = FALSE;

#ifdef _DEBUG
	// エンカウント無しフラグがたってたらエンカウントしない
	if( EncountOffFlag )
		return FALSE;
#endif

	// 自分がパーティーの一員でない時処理する
	if( partyModeFlag == 0 || (pc.status & CHR_STATUS_LEADER) != 0 )
	{
//		if( nowEncountPercentage > (rand()%100) )
		if( nowEncountPercentage > rand2() )
		{
			ret = TRUE;
			nowEncountExtra = 0;
		}
	}
	if( 6 > nowEncountExtra )
	{
		nowEncountExtra++;
	}
	else
	{
		if( maxEncountPercentage > nowEncountPercentage )
			nowEncountPercentage++;
	}

	return ret;
}

// サーバにエンカウントしたと送る
void _sendEncount( void )
{
	// 移動を止める
	resetMap();
	sendEnFlag = 1;
	etcEventFlag = 1;
	eventEnemyFlag = 0;

	// エンカウントしたとサーバーに言う
	lssproto_EN_send( sockfd, nowGx, nowGy );
}


///////////////////////////////////////////////////////////////////////////
// ワープイベント処理
//
// ワープイベントチェック
BOOL _checkWarpEvent( int gx, int gy )
{
	int x, y;
	int timeZoneNo;

	x = gx - mapAreaX1;
	y = gy - mapAreaY1;

	timeZoneNo = getLSTime ( &SaTime );

	_eventWarpNo = (event[y*mapAreaWidth+x] & 0x0fff);
	// ワープ
	if( _eventWarpNo == EVENT_WARP )
	{
		return TRUE;
	}
	else
	// 朝だけワープ
	if( _eventWarpNo == EVENT_WARP_MONING
	 && timeZoneNo == LS_MORNING )
	{
		return TRUE;
	}
	else
	// 昼だけワープ
	if( _eventWarpNo == EVENT_WARP_NOON
	 && (timeZoneNo == LS_NOON || timeZoneNo == LS_EVENING) )
	{
		return TRUE;
	}
	else
	// 夜だけワープ
	if( _eventWarpNo == EVENT_WARP_NIGHT
	 && timeZoneNo == LS_NIGHT )
	{
		return TRUE;
	}

	return FALSE;
}


// ワープイベント送信
void _sendWarpEvent( void )
{
	// 移動を止める
	resetMap();
	eventWarpSendFlag = 1;
	etcEventFlag = 1;
	eventWarpSendId = eventId;
	lssproto_EV_send( sockfd, _eventWarpNo, eventId, nowGx, nowGy, -1 );
	eventId++;
	wnCloseFlag = 1;	// 汎用ウィンドウを閉じる
	closeEtcSwitch();	// 各種設定メニューを閉じる
	closeCharActionAnimeChange();	// アクションウィンドウを閉じる

#if 1
	// ワープ演出実行
	SubProcNo = 200;
	// 現時点の画面を作る
	warpEffectProc();
	warpEffectFlag = TRUE;
	floorChangeFlag = TRUE;
#endif

	// マップウィンドウフラグバックアップ
	if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
}


///////////////////////////////////////////////////////////////////////////
// 固定エンカウントイベント処理
//
// イベントチェック
BOOL _checkEnemyEvent( int gx, int gy )
{
	int x, y;
	int ev;

	x = gx - mapAreaX1;
	y = gy - mapAreaY1;

	ev = (event[y*mapAreaWidth+x] & 0x0fff);
	// エンカウント
	if( ev == EVENT_ENEMY )
	{
		return TRUE;
	}

	return FALSE;
}


// 固定エンカウントイベント送信
void _sendEnemyEvent( void )
{
	// 移動を止める
	resetMap();
	eventEnemySendFlag = 1;
	etcEventFlag = 1;
	eventEnemySendId = eventId;
	lssproto_EV_send( sockfd, EVENT_ENEMY, eventId,	nowGx, nowGy, _enemyEventDir );
	eventId++;
	wnCloseFlag = 1;	// 汎用ウィンドウを閉じる
	eventEnemyFlag = 1;	// 固定敵にエンカウントしたら１にする
}


///////////////////////////////////////////////////////////////////////////
//
// 移動先にデータが無い時は移動をとめてマップが来るのを待つ。
//
void _checkEmptyMap( void )
{
	int dir;
	int i, j;
	int gx, gy;

	i = 0;

	gx = nowGx;
	gy = nowGy;
//	for( i = 0; i < moveRouteCnt2; i++ )
//	{
		dir = moveRoute2[i];

		// 移動先にデータが無ければフラグを立てて移動をとめる
		if( checkEmptyMapData( gx, gy, dir ) )
		{
			for( j = 0; j < getMapAreaCnt; j++ )
			{
#if 0
#ifdef _DEBUG_MSG
				char msg[256];

				sprintf( msg, "データ無いので移動とめる(%d,%d) dir = %d",
					nowGx, nowGy, dir );
				StockChatBufferLine( msg, FONT_PAL_GRAY );
				sprintf( msg, "データ(%d,%d)-(%d,%d)をサーバに要求",
					getMapAreaX1[j],
					getMapAreaY1[j],
					getMapAreaX2[j],
					getMapAreaY2[j] );
				StockChatBufferLine( msg, FONT_PAL_GRAY );
#endif
#endif
				// サーバに要求
				lssproto_M_send( sockfd, nowFloor,
					getMapAreaX1[j],
					getMapAreaY1[j],
					getMapAreaX2[j],
					getMapAreaY2[j] );
			}
			mapEmptyFlag = TRUE;
			mapEmptyDir = dir;
			mapEmptyGx = nowGx;
			mapEmptyGy = nowGy;
			mapEmptyStartTime = GetTickCount();	// マップ要求した時の時間を記憶
		}

//		gx += moveAddTbl[dir][0];
//		gy += moveAddTbl[dir][1];
//	}
}


BOOL checkEmptyMapData( int _gx, int _gy, int dir )
{
	// キャラの位置から±11のところを調べてデータが無ければTRUEを返す
	int i;
	int gx, gy;
	int tx, ty;
	int len;
	int egx, egy;
	BOOL flag = FALSE;


	if( mapAreaWidth < MAP_X_SIZE
	 || mapAreaHeight < MAP_Y_SIZE )
	{
		return FALSE;
	}

	getMapAreaCnt = 0;

	if( dir == 0 || dir == 1 || dir == 2 )
	{
		gx = _gx - SEARCH_AREA;
		gy = _gy - SEARCH_AREA;
		tx = -SEARCH_AREA - MAP_TILE_GRID_X1;
		ty = -SEARCH_AREA - MAP_TILE_GRID_Y1;
		len = SEARCH_AREA*2+1;
		egx = gx + 1;
		egy = gy + SEARCH_AREA*2+1;
		for( i = 0; i < len; i++ )
		{
			if( (0 <= gx && gx < nowFloorGxSize)
			 && (0 <= gy && gy < nowFloorGySize) )
			{
				if( (event[ty*mapAreaWidth+tx] & MAP_READ_FLAG) == 0 )
				{
					getMapAreaX1[getMapAreaCnt] = gx - 1;
					getMapAreaY1[getMapAreaCnt] = gy - 1;
					getMapAreaX2[getMapAreaCnt] = egx;
					getMapAreaY2[getMapAreaCnt] = egy;
					checkAreaLimit( &getMapAreaX1[getMapAreaCnt], &getMapAreaY1[getMapAreaCnt],
						&getMapAreaX2[getMapAreaCnt], &getMapAreaY2[getMapAreaCnt] );
					getMapAreaCnt++;
					flag = TRUE;
					break;
				}
			}
			gy++;
			ty++;
		}
	}
	if( dir == 2 || dir == 3 || dir == 4 )
	{
		gx = _gx - SEARCH_AREA;
		gy = _gy - SEARCH_AREA;
		tx = -SEARCH_AREA - MAP_TILE_GRID_X1;
		ty = -SEARCH_AREA - MAP_TILE_GRID_Y1;
		len = SEARCH_AREA*2+1;
		egx = gx + SEARCH_AREA*2+1;
		egy = gy + 1;
		for( i = 0; i < len; i++ )
		{
			if( (0 <= gx && gx < nowFloorGxSize)
			 && (0 <= gy && gy < nowFloorGySize) )
			{
				if( (event[ty*mapAreaWidth+tx] & MAP_READ_FLAG) == 0 )
				{
					getMapAreaX1[getMapAreaCnt] = gx - 1;
					getMapAreaY1[getMapAreaCnt] = gy - 1;
					getMapAreaX2[getMapAreaCnt] = egx;
					getMapAreaY2[getMapAreaCnt] = egy;
					checkAreaLimit( &getMapAreaX1[getMapAreaCnt], &getMapAreaY1[getMapAreaCnt],
						&getMapAreaX2[getMapAreaCnt], &getMapAreaY2[getMapAreaCnt] );
					getMapAreaCnt++;
					flag = TRUE;
					break;
				}
			}
			gx++;
			tx++;
		}
	}
	if( dir == 4 || dir == 5 || dir == 6 )
	{
		gx = _gx + SEARCH_AREA;
		gy = _gy - SEARCH_AREA;
		tx = SEARCH_AREA - MAP_TILE_GRID_X1;
		ty = -SEARCH_AREA - MAP_TILE_GRID_Y1;
		len = SEARCH_AREA*2+1;
		egx = gx + 2;
		egy = gy + SEARCH_AREA*2+1;
		for( i = 0; i < len; i++ )
		{
			if( (0 <= gx && gx < nowFloorGxSize)
			 && (0 <= gy && gy < nowFloorGySize) )
			{
				if( (event[ty*mapAreaWidth+tx] & MAP_READ_FLAG) == 0 )
				{
					getMapAreaX1[getMapAreaCnt] = gx;
					getMapAreaY1[getMapAreaCnt] = gy - 1;
					getMapAreaX2[getMapAreaCnt] = egx;
					getMapAreaY2[getMapAreaCnt] = egy;
					checkAreaLimit( &getMapAreaX1[getMapAreaCnt], &getMapAreaY1[getMapAreaCnt],
						&getMapAreaX2[getMapAreaCnt], &getMapAreaY2[getMapAreaCnt] );
					getMapAreaCnt++;
					flag = TRUE;
					break;
				}
			}
			gy++;
			ty++;
		}
	}
	if( dir == 6 || dir == 7 || dir == 0 )
	{
		gx = _gx - SEARCH_AREA;
		gy = _gy + SEARCH_AREA;
		tx = -SEARCH_AREA - MAP_TILE_GRID_X1;
		ty = SEARCH_AREA - MAP_TILE_GRID_Y1;
		len = SEARCH_AREA*2+1;
		egx = gx + SEARCH_AREA*2+1;
		egy = gy + 2;
		for( i = 0; i < len; i++ )
		{
			if( (0 <= gx && gx < nowFloorGxSize)
			 && (0 <= gy && gy < nowFloorGySize) )
			{
				if( (event[ty*mapAreaWidth+tx] & MAP_READ_FLAG) == 0 )
				{
					getMapAreaX1[getMapAreaCnt] = gx - 1;
					getMapAreaY1[getMapAreaCnt] = gy;
					getMapAreaX2[getMapAreaCnt] = egx;
					getMapAreaY2[getMapAreaCnt] = egy;
					checkAreaLimit( &getMapAreaX1[getMapAreaCnt], &getMapAreaY1[getMapAreaCnt],
						&getMapAreaX2[getMapAreaCnt], &getMapAreaY2[getMapAreaCnt] );
					getMapAreaCnt++;
					flag = TRUE;
					break;
				}
			}
			gx++;
			tx++;
		}
	}

	return flag;
}








// マップに移動先を設定
void setMapMovePoint( int _nextGx, int _nextGy )
{
	float dx, dy;
	float len;
	float dir1;
	float rate = 1.0F;
	int dir;

	nextGx = _nextGx;
	nextGy = _nextGy;

	// 移動量を求める
	dx = nextGx*GRID_SIZE-nowX;
	dy = nextGy*GRID_SIZE-nowY;
	len = (float)sqrt( (double)(dx*dx+dy*dy) );
	if( len > 0 )
	{
		dx /= len;
		dy /= len;
	}
	else
	{
		dx = 0;
		dy = 0;
	}
	nowVx = dx * MOVE_SPEED * rate;
	nowVy = dy * MOVE_SPEED * rate;

	// PCキャラの方向設定
	if( dx != 0 || dy != 0 )
	{
		dir1 = Atan( dx, dy ) + 22.5F;
		AdjustDir( &dir1 );
		dir = (int)(dir1/45);
		setPcDir( dir );
		setPcWalkFlag();
	}

	// PCキャラをマップの位置と同期する
	setPcPoint();
}


// マップに移動先を設定（加速処理あり）
void setMapMovePoint2( int _nextGx, int _nextGy )
{
	float dx, dy;
	float len;
	float dir1;
	float rate = 1.0F;
	int dir;

	if( pc.ptAct != NULL )
	{
		if( pc.ptAct->bufCount > 5 )
		{
			rate = 2.0F;
		}
		else
		if( pc.ptAct->bufCount >= 4 )
		{
			rate = 1.6F;
		}
		else
		if( pc.ptAct->bufCount >= 2 )
		{
			rate = 1.2F;
		}
	}

	nextGx = _nextGx;
	nextGy = _nextGy;

	// 移動量を求める
	dx = nextGx*GRID_SIZE-nowX;
	dy = nextGy*GRID_SIZE-nowY;
	len = (float)sqrt( (double)(dx*dx+dy*dy) );
	if( len > 0 )
	{
		dx /= len;
		dy /= len;
	}
	else
	{
		dx = 0;
		dy = 0;
	}
	nowVx = dx * MOVE_SPEED * rate;
	nowVy = dy * MOVE_SPEED * rate;

	// PCキャラの方向設定
	if( dx != 0 || dy != 0 )
	{
		dir1 = Atan( dx, dy ) + 22.5F;
		AdjustDir( &dir1 );
		dir = (int)(dir1/45);
		setPcDir( dir );
		setPcWalkFlag();
	}

	// PCキャラをマップの位置と同期する
	setPcPoint();
}


// マップ移動処理
void mapMove2( void )
{
	float dx, dy;

	// 移動中か？
	if( nowVx != 0 || nowVy != 0 )
	{
		// 目的地に着いたらパラメータ初期化
		dx = (float)nextGx*GRID_SIZE;
		dy = (float)nextGy*GRID_SIZE;
		if( pointLen2( nowX, nowY, dx, dy ) <= nowVx*nowVx+nowVy*nowVy )
		{
			nowX = dx;
			nowY = dy;
			nowVx = 0;
			nowVy = 0;
		}
		// 移動
		else
		{
			nowX += nowVx;
			nowY += nowVy;
		}
		setPcAction( ANIM_WALK );
		setPcWalkFlag();
	}
	else
	{
		// PCキャラのアニメーションがWALKの次のフレームならならSTANDにする
		if( checkPcWalkFlag() == 1 )
		{
			setPcAction( ANIM_STAND );
			delPcWalkFlag();
		}
	}

	nowGx = (int)(nowX/GRID_SIZE);
	nowGy = (int)(nowY/GRID_SIZE);

	// PCキャラをマップの位置と同期する
	setPcPoint();
}



///////////////////////////////////////////////////////////////////////////
// その場で向きを変える
//
void turnAround( void )
{
	float tmpDir;
	float tmpX, tmpY;
	int dir;
	char dir2[2];
	static unsigned int turnSendTime = 0;

	// PCキャラいなければ何もしない
	if( pc.ptAct == NULL )
		return;

	// 移動の途中でも何もしない
	if( nowVx != 0 || nowVy != 0 )
		return;

	// 何かのプロトコルを送ってたら何もしない
	if( etcSendFlag != 0 || etcEventFlag != 0 )
		return;


	// 現在のマウスの方向を調べる
	tmpX = (float)(mouseMapGx - nowGx);
	tmpY = (float)(mouseMapGy - nowGy);
	tmpDir = Atan( tmpX, tmpY ) + 22.5F;
	AdjustDir( &tmpDir );
	dir = (int)(tmpDir/45);


	// 向きが同じ時も何もしない
	if( pc.ptAct->anim_ang == dir )
		return;

	// 連射抑制
	if( turnSendTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )
	{
		setPcDir( dir );

		dir2[0] = cnvServDir( dir, 1 );
		dir2[1] = '\0';

		walkSendForServer( nowGx, nowGy, dir2 );
		turnSendTime = GetTickCount();
	}
}


void turnAround2( int dir )
{
	char dir2[2];
	static unsigned int turnSendTime = 0;

	// 方向が間違っていれば何もしない
	if( dir < 0 || 7 < dir )
		return;

	// PCキャラいなければ何もしない
	if( pc.ptAct == NULL )
		return;

	// 移動の途中でも何もしない
	if( nowVx != 0 || nowVy != 0 )
		return;

	// 何かのプロトコルを送ってたら何もしない
	if( etcSendFlag != 0 || etcEventFlag != 0 )
		return;

	// 向きが同じ時も何もしない
	if( pc.ptAct->anim_ang == dir )
		return;

	// 連射抑制
	if( turnSendTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )
	{
		setPcDir( dir );

		dir2[0] = cnvServDir( dir, 1 );
		dir2[1] = '\0';

		walkSendForServer( nowGx, nowGy, dir2 );
		turnSendTime = GetTickCount();
	}
}


///////////////////////////////////////////////////////////////////////////
// 移動経路を算出する
//
// 現在地からdir方向に1歩移動するためのデータを取得する
void getRouteData( int dir, int *x, int *y )
{
	switch( dir )
	{
		case 0:
			*x = -1;
			*y = 1;
			break;

		case 1:
			*x = -1;
			*y = 0;
			break;

		case 2:
			*x = -1;
			*y = -1;
			break;

		case 3:
			*x = 0;
			*y = -1;
			break;

		case 4:
			*x = 1;
			*y = -1;
			break;

		case 5:
			*x = 1;
			*y = 0;
			break;

		case 6:
			*x = 1;
			*y = 1;
			break;

		case 7:
			*x = 0;
			*y = 1;
			break;

		default:
			*x = 0;
			*y = 0;
			break;
	}
}


// (gx1,gy1)から(gx2,gy2)への向き
int getDirData( int gx1, int gy1, int gx2, int gy2 )
{
	float tmpDir;
	float tmpX, tmpY;

	// 目的地の方向を調べる
	tmpX = (float)(gx2 - gx1);
	tmpY = (float)(gy2 - gy1);
	tmpDir = Atan( tmpX, tmpY ) + 22.5F;
	AdjustDir( &tmpDir );

	return (int)(tmpDir/45);
}

// (gx1,gy2)を目的地とし(gx2,gy2)が隣接グリッドかどうか調べる
// 戻り値：TRUE ... 隣接グリッド
BOOL checkGridAround( int gx1, int gy1, int gx2, int gy2 )
{
	if( ((gx1 == gx2) && ABS( gy2 - gy1 ) == 1)
	 || (ABS( gx2 - gx1 ) == 1 && (gy1 == gy2))
	 || (ABS( gx2 - gx1 ) == 1 && ABS( gy2 - gy1 ) == 1 ) )
	{
		return TRUE;
	}

	return FALSE;
}


#if 1

void getRouteMap( void )
{
	int mx = moveStackGx, my = moveStackGy;
	int nowx = nowGx, nowy = nowGy;
	int dir;
	int dx, dy;
	float tmpDir;
	float tmpX, tmpY;
	int targetDir;
	int nowDir, checkDir1, checkDir2;
	int dirTbl[] = { 0, 1, -1 };
	int i;
	BOOL flag;

	moveLastDir = -1;

	if( nowx == mx && nowy == my )
		return;

	// 目的地の方向を調べる
	tmpX = (float)(mx - nowx);
	tmpY = (float)(my - nowy);
	tmpDir = Atan( tmpX, tmpY ) + 22.5F;
	AdjustDir( &tmpDir );
	targetDir = (int)(tmpDir/45);

	moveRouteCnt = 0;

	while( nowx != mx || nowy != my )
	{
		if( moveRouteCnt >= MOVE_MAX )
		{
			moveRouteCnt = 0;
			return;
		}

		// 目的地の方向を調べる
		tmpX = (float)(mx - nowx);
		tmpY = (float)(my - nowy);
		tmpDir = Atan( tmpX, tmpY ) + 22.5F;
		AdjustDir( &tmpDir );
		nowDir = (int)(tmpDir/45);

#if 1
		{
			checkDir1 = targetDir + 1;
			checkDir1 &= 7;
			checkDir2 = targetDir - 1;
			checkDir2 &= 7;
			if( nowDir != targetDir && nowDir != checkDir1 && nowDir != checkDir2 )
			{
				// 初期位置から目的地への方向と左右前方以外の方向には移動しない
				break;
			}
		}
#endif


		flag = TRUE;
		for( i = 0; i < sizeof( dirTbl )/sizeof( int ); i++ )
		{
			dir = nowDir + dirTbl[i];
			dir &= 7;
			checkDir1 = targetDir + 1;
			checkDir1 &= 7;
			checkDir2 = targetDir - 1;
			checkDir2 &= 7;
			if( dir != targetDir && dir != checkDir1 && dir != checkDir2 )
			{
				// 初期位置から目的地への方向と左右前方以外の方向には移動しない
				continue;
			}

#if 0
			// １歩でも動いた後、目的地と軸があったら終了
			if( moveRouteCnt > 0
			 && (nowx == mx || nowy == my) )
			{
				flag = TRUE;
				break;
			}
#endif

			getRouteData( dir, &dx, &dy );
			if( checkHitMap( nowx+dx, nowy+dy ) )
			{
				// 進入不可
				if( (nowx+dx) == mx && (nowy+dy) == my )
				{
#if 1
					if( (dir % 2) == 0 )
						continue;
#endif
					// 移動先が目的地なら終了
					flag = TRUE;
					break;
				}
				else
				{
					continue;
				}
			}
			// グリッド間を斜めに移動する時、前方左右斜め前の障害物によって動作が変わる
			if( (dir % 2) == 0 )
			{
				int dir2, dx2, dy2;
				int dir3, dx3, dy3;
				BOOL flag2, flag3;

				dir2 = dir + 1;
				dir2 &= 7;
				getRouteData( dir2, &dx2, &dy2 );
				dir3 = dir - 1;
				dir3 &= 7;
				getRouteData( dir3, &dx3, &dy3 );
				flag2 = checkHitMap( nowx+dx2, nowy+dy2 );
				flag3 = checkHitMap( nowx+dx3, nowy+dy3 );
				// 両方に障害物があるなら行き止まり
				if( flag2 && flag3 )
				{
					break;
				}
				// 右斜め前に障害物がある時
				// 左斜め前に回避
				if( flag2 )
				{
					moveRoute[moveRouteCnt] = dir3;
					moveRouteGx[moveRouteCnt] = nowx + dx3;
					moveRouteGy[moveRouteCnt] = nowy + dy3;
					moveRouteCnt++;
					dx = (nowx+dx) - (nowx+dx3);
					dy = (nowy+dy) - (nowy+dy3);
					dir = getDirData( 0, 0, dx, dy );
					nowx += dx3;
					nowy += dy3;
					flag = FALSE;
					break;
				}
				// 左斜め前に障害物がある時
				// 右斜め前に回避
				if( flag3 )
				{
					moveRoute[moveRouteCnt] = dir2;
					moveRouteGx[moveRouteCnt] = nowx + dx2;
					moveRouteGy[moveRouteCnt] = nowy + dy2;
					moveRouteCnt++;
					dx = (nowx+dx) - (nowx+dx2);
					dy = (nowy+dy) - (nowy+dy2);
					dir = getDirData( 0, 0, dx, dy );
					nowx += dx2;
					nowy += dy2;
					flag = FALSE;
					break;
				}
			}
			// ここまで来たら移動先が決定している
			flag = FALSE;
			break;
		}

		// 行き止まりなので終わる
		if( flag )
			break;


		nowx += dx;
		nowy += dy;
		moveRoute[moveRouteCnt] = dir;
		moveRouteGx[moveRouteCnt] = nowx;
		moveRouteGy[moveRouteCnt] = nowy;

		moveRouteCnt++;
	}

#if 1
	dx = 0;
	if( mx - nowx > 0 )
	{
		dx = 1;
	}
	else
	if( mx - nowx < 0 )
	{
		dx = -1;
	}
	if( dx != 0 )
	{
		while( !checkHitMap( nowx+dx, nowy ) && mx != nowx )
		{
			nowx += dx;
			moveRoute[moveRouteCnt] = getDirData( 0, 0, dx, 0 );
			moveRouteGx[moveRouteCnt] = nowx;
			moveRouteGy[moveRouteCnt] = nowy;
			moveRouteCnt++;
		}
	}

	dy = 0;
	if( my - nowy > 0 )
	{
		dy = 1;
	}
	else
	if( my - nowy < 0 )
	{
		dy = -1;
	}
	if( dy != 0 )
	{
		while( !checkHitMap( nowx, nowy+dy ) && my != nowy )
		{
			nowy += dy;
			moveRoute[moveRouteCnt] = getDirData( 0, 0, 0, dy );
			moveRouteGx[moveRouteCnt] = nowx;
			moveRouteGy[moveRouteCnt] = nowy;
			moveRouteCnt++;
		}
	}
#endif

	// 目的地の方向を調べる
	if( moveRouteCnt > 0
	 && (nowx != mx || nowy != my) )
	{
		tmpX = (float)(mx - nowx);
		tmpY = (float)(my - nowy);
		tmpDir = Atan( tmpX, tmpY ) + 22.5F;
		AdjustDir( &tmpDir );
		moveLastDir = (int)(tmpDir/45);
	}

	return;
}

#else

void getRouteMap( void )
{
	int mx = moveStackGx, my = moveStackGy;
	int nowx = nowGx, nowy = nowGy;
	int dir;
	int dx, dy;
	float tmpDir;
	float tmpX, tmpY;
	int targetDir;
	int nowDir, checkDir1, checkDir2;
	int dirTbl[] = { 0, 1, -1 };
	int i;
	BOOL flag;

	if( nowx == mx && nowy == my )
		return;

	// 目的地の方向を調べる
	tmpX = (float)(mx - nowx);
	tmpY = (float)(my - nowy);
	tmpDir = Atan( tmpX, tmpY ) + 22.5F;
	AdjustDir( &tmpDir );
	targetDir = (int)(tmpDir/45);

	moveRouteCnt = 0;

	while( nowx != mx || nowy != my )
	{
		if( moveRouteCnt >= MOVE_MAX )
		{
			moveRouteCnt = 0;
			return;
		}

		// 目的地の方向を調べる
		tmpX = (float)(mx - nowx);
		tmpY = (float)(my - nowy);
		tmpDir = Atan( tmpX, tmpY ) + 22.5F;
		AdjustDir( &tmpDir );
		nowDir = (int)(tmpDir/45);

		flag = TRUE;
		for( i = 0; i < sizeof( dirTbl )/sizeof( int ); i++ )
		{
			dir = nowDir + dirTbl[i];
			dir &= 7;
			checkDir1 = targetDir + 1;
			checkDir1 &= 7;
			checkDir2 = targetDir - 1;
			checkDir2 &= 7;
			if( dir != targetDir && dir != checkDir1 && dir != checkDir2 )
			{
				// 初期位置から目的地への方向と左右前方以外の方向には移動しない
				continue;
			}

			getRouteData( dir, &dx, &dy );
			if( checkHitMap( nowx+dx, nowy+dy ) )
			{
				// 進入不可
				if( (nowx+dx) == mx && (nowy+dy) == my )
				{
					// 移動先が目的地なら終了
					flag = TRUE;
					break;
				}
				else
				{
					continue;
				}
			}
			// グリッド間を斜めに移動する時、前方左右斜め前の障害物によって動作が変わる
			if( (dir % 2) == 0 )
			{
				int dir2, dx2, dy2;
				int dir3, dx3, dy3;
				BOOL flag2, flag3;

				dir2 = dir + 1;
				dir2 &= 7;
				getRouteData( dir2, &dx2, &dy2 );
				dir3 = dir - 1;
				dir3 &= 7;
				getRouteData( dir3, &dx3, &dy3 );
				flag2 = checkHitMap( nowx+dx2, nowy+dy2 );
				flag3 = checkHitMap( nowx+dx3, nowy+dy3 );
				// 両方に障害物があるなら行き止まり
				if( flag2 && flag3 )
				{
					break;
				}
				// 右斜め前に障害物がある時
				// 左斜め前に回避
				if( flag2 )
				{
					dir = dir3;
					dx = dx3;
					dy = dy3;
					flag = FALSE;
					break;
				}
				// 左斜め前に障害物がある時
				// 右斜め前に回避
				if( flag3 )
				{
					dir = dir2;
					dx = dx2;
					dy = dy2;
					flag = FALSE;
					break;
				}
			}
			// ここまで来たら移動先が決定している
			flag = FALSE;
			break;
		}

		// 行き止まりなので終わる
		if( flag )
			break;

		nowx += dx;
		nowy += dy;
		moveRoute[moveRouteCnt] = dir;
		moveRouteGx[moveRouteCnt] = nowx;
		moveRouteGy[moveRouteCnt] = nowy;

		moveRouteCnt++;
	}
	return;
}

#endif


// 登録されている移動ルートを前にシフト
void shiftRouteMap( void )
{
	int i;

	if( moveRouteCnt <= 0 )
		return;

	moveRouteCnt--;
	for( i = 0; i < moveRouteCnt; i++ )
	{
		moveRoute[i] = moveRoute[i+1];
		moveRouteGx[i] = moveRouteGx[i+1];
		moveRouteGy[i] = moveRouteGy[i+1];
	}
}

// 登録されている移動ルートを前にシフト
void shiftRouteMap2( void )
{
	int i;

	if( moveRouteCnt2 <= 0 )
		return;

	moveRouteCnt2--;
	for( i = 0; i < moveRouteCnt2; i++ )
	{
		moveRoute2[i] = moveRoute2[i+1];
	}
}



// 移動中に強制ワープした時、
//   ワープ地点が通過後の位置ならワープ、
//   移動先ならそこまで移動する。
void forceWarpMap( int gx, int gy )
{
	int i;

	// 移動中で無いなら何もしない
	if( moveRouteCnt <= 0 )
		return;

	for( i = 0; i < moveRouteCnt; i++ )
	{
		// ワープ地点が移動先ならそこまで移動する
		if( moveRouteGx[i] == gx && moveRouteGy[i] == gy )
		{
			moveRouteCnt = i + 1;
			return;
		}
	}

	// ワープ地点が通過後の位置ならワープ
	// (移動処理終わる)
	moveRouteCnt = 0;
	setMap( nowFloor, gx, gy );
}


// 方向をサーバに送るときの文字にする
// mode : 0 ... 移動用/ 1 ... 方向変えるだけ
char cnvServDir( int dir, int mode )
{
	char ret = 'f';

	if( 0 <= dir && dir <= 2 )
	{
		if( mode == 0 )
		{
			ret = 'f'+dir;
		}
		else
		{
			ret = 'F'+dir;
		}
	}
	else
	if( dir < 8 )
	{
		if( mode == 0 )
		{
			ret = 'a'+dir-3;
		}
		else
		{
			ret = 'A'+dir-3;
		}
	}

	return ret;
}



///////////////////////////////////////////////////////////////////////////
// キャラ、パーツの表示優先順位決定用
// 初期化
void initCharPartsPrio( void )
{
	charPrioCnt = 0;
	charPrioBufTop.pre = NULL;
	charPrioBufTop.next = NULL;
}


// マップパーツをバッファにためる
void setPartsPrio( int graNo, int x, int y, int dx, int dy, float mx, float my )
{
	int i;
	CHAR_PARTS_PRIORITY *ptc, *prePtc;
	BOOL flag;

	// バッファが無いので終わる
	if( charPrioCnt >= MAX_CHAR_PRIO_BUF )
		return;


	// バッファにパラメータセット
	charPrioBuf[charPrioCnt].graNo = graNo;
	charPrioBuf[charPrioCnt].x     = x;
	charPrioBuf[charPrioCnt].y     = y;
	charPrioBuf[charPrioCnt].dx    = dx;
	charPrioBuf[charPrioCnt].dy    = dy;
	charPrioBuf[charPrioCnt].mx    = mx;
	charPrioBuf[charPrioCnt].my    = my;
	charPrioBuf[charPrioCnt].type  = CHAR_PARTS_PRIO_TYPE_PARTS;
	charPrioBuf[charPrioCnt].pre   = NULL;
	charPrioBuf[charPrioCnt].next  = NULL;
	charPrioBuf[charPrioCnt].depth = y;

	if( charPrioCnt == 0 )
	{
		addCharPartsPrio( &charPrioBufTop, &charPrioBuf[charPrioCnt] );
	}
	else
	{
		// ほかに登録されたらソートする
		flag = FALSE;
		ptc = charPrioBufTop.next;
		for( i = 0; i < charPrioCnt && ptc != NULL; i++, ptc = ptc->next )
		{
			if( ptc->type == CHAR_PARTS_PRIO_TYPE_CHAR )
			{
				if( checkPrioPartsVsChar( ptc, &charPrioBuf[charPrioCnt] ) )
				{
					insertCharPartsPrio( ptc, &charPrioBuf[charPrioCnt] );
					flag = TRUE;
					break;
				}
			}
			prePtc = ptc;
		}
		if( !flag )
		{
			addCharPartsPrio( prePtc, &charPrioBuf[charPrioCnt] );
		}
	}

	charPrioCnt++;
}


// キャラをバッファにためる
void setCharPrio( int graNo, int x, int y, int dx, int dy, float mx, float my )
{
	int i;
	CHAR_PARTS_PRIORITY *ptc, *prePtc;
	BOOL flag;

	// バッファが無いので終わる
	if( charPrioCnt >= MAX_CHAR_PRIO_BUF )
		return;

	// バッファにパラメータセット
	charPrioBuf[charPrioCnt].graNo = graNo;
	charPrioBuf[charPrioCnt].x     = x;
	charPrioBuf[charPrioCnt].y     = y;
	charPrioBuf[charPrioCnt].dx    = dx;
	charPrioBuf[charPrioCnt].dy    = dy;
	charPrioBuf[charPrioCnt].mx    = mx;
	charPrioBuf[charPrioCnt].my    = my;
	charPrioBuf[charPrioCnt].type  = CHAR_PARTS_PRIO_TYPE_CHAR;
	charPrioBuf[charPrioCnt].pre   = NULL;
	charPrioBuf[charPrioCnt].next  = NULL;
	charPrioBuf[charPrioCnt].depth = y;

	if( charPrioCnt == 0 )
	{
		addCharPartsPrio( &charPrioBufTop, &charPrioBuf[charPrioCnt] );
	}
	else
	{
		// ほかに登録されたらソートする
		flag = FALSE;
		ptc = charPrioBufTop.next;
		for( i = 0; i < charPrioCnt && ptc != NULL; i++, ptc = ptc->next )
		{
			if( charPrioBuf[charPrioCnt].depth > ptc->depth )
			{
				insertCharPartsPrio( ptc, &charPrioBuf[charPrioCnt] );
				flag = TRUE;
				break;
			}
			prePtc = ptc;
		}
		if( !flag )
		{
			addCharPartsPrio( prePtc, &charPrioBuf[charPrioCnt] );
		}
	}

	charPrioCnt++;
}


// パーツとキャラの描画優先順位チェック
//   戻り値：パーツがキャラより手前なら TRUE
BOOL checkPrioPartsVsChar( CHAR_PARTS_PRIORITY *ptc, CHAR_PARTS_PRIORITY *ptp )
{
	short hit;
	short prioType;
	S2 w, h;

	// 描画優先順位決定タイプを取り出す
	realGetPrioType( ptp->graNo, &prioType );

	// あたり判定の有無
	realGetHitFlag( ptp->graNo, &hit );

	// 歩ける物体で prioType == 3 はタイル扱い
	if( hit != 0 && prioType == 3 )
	{
		return FALSE;
	}

	// タイプ１
	//  今は、デフォルトを使えばいい（これは必要ない）
	if( prioType == 1 )
	{
		if( ptc->mx <= ptp->mx
		 || ptc->my >= ptp->my )
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
#if 0
	else
	// タイプ２
	//  タイルと同じ扱いにする
	if( prioType == 2 )
	{
		// 古い設定（もういらないかも）
		if( (ptc->mx <= ptp->mx && ptc->my >= ptp->my)
		 || (ptc->mx < ptp->mx-GRID_SIZE || ptc->my > ptp->my+GRID_SIZE) )
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
#endif



	// デフォルトのプライオリティー
#if 1
	if( ptc->mx > ptp->mx && ptc->my < ptp->my )
	{
		return TRUE;
	}
	else
	{
	realGetHitPoints( ptp->graNo, &w, &h );
	if( ptc->x > ptp->x )
	{
		// PCが建物の右側にいる
		if( ptp->y-(w-1)*SURFACE_HEIGHT/2 <= ptc->y )
			return FALSE;
	}
	else
	if( ptc->x < ptp->x )
	{
		// PCが建物の左側にいる
		if( ptp->y-(h-1)*SURFACE_HEIGHT/2 <= ptc->y )
			return FALSE;
	}
	else
	{
		if( ptp->y <= ptc->y )
			return FALSE;
	}
	}
#else
	realGetHitPoints( ptp->graNo, &w, &h );
	if( ptc->x >= ptp->x )
	{
		// PCが建物の右側にいる
		if( ptp->y-(w-1)*SURFACE_HEIGHT/2 < ptc->y )
			return FALSE;
	}
	else
	{
		// PCが建物の左側にいる
		if( ptp->y-(h-1)*SURFACE_HEIGHT/2 < ptc->y )
			return FALSE;
	}
#endif

	return TRUE;
}


// pt1の場所にpt2を挿入する
void insertCharPartsPrio( CHAR_PARTS_PRIORITY *pt1, CHAR_PARTS_PRIORITY *pt2 )
{
	if( pt1 == NULL || pt2 == NULL )
		return;

	pt2->pre = pt1->pre;
	pt2->next = pt1;

	(pt1->pre)->next = pt2;
	pt1->pre = pt2;
}


// pt1の後ろにp2を追加する
void addCharPartsPrio( CHAR_PARTS_PRIORITY *pt1, CHAR_PARTS_PRIORITY *pt2 )
{
	if( pt1 == NULL || pt2 == NULL )
		return;

	pt2->pre = pt1;
	pt2->next = pt1->next;

	if( pt1->next != NULL )
	{
		(pt1->next)->pre = pt2;
	}
	pt1->next = pt2;
}


// pt1を削除
void delCharPartsPrio( CHAR_PARTS_PRIORITY *pt1 )
{
	if( pt1 == NULL )
		return;

	// 先頭のバッファは削除できない
	if( pt1->pre == NULL )
		return;

	(pt1->pre)->next = pt1->next;
	if( pt1->next != NULL )
	{
		(pt1->next)->pre = pt1->pre;
	}
}


// ソート後表示する
void stockCharParts( void )
{
	int i;
	CHAR_PARTS_PRIORITY *pt;

	pt = charPrioBufTop.next;
	if( pt == NULL )
		return;

	for( i = 0; i < charPrioCnt && pt != NULL; i++, pt = pt->next )
	{
		StockDispBuffer2( pt->x + pt->dx, pt->y + pt->dy,
			DISP_PRIO_PARTS, pt->graNo, 0 );
#if 0
		// デバッグ用
		if( pt->type == CHAR_PARTS_PRIO_TYPE_CHAR )
			break;
#endif
	}
}



///////////////////////////////////////////////////////////////////////////
// イベント処理
//

#if 0	// このイベントは障害物としてあたり判定であつかう
//
// NPCにぶつかったら止まるイベント
// （このイベントはプロトコルを送らない）
//
//   戻り値：TRUE  ... イベント発生
//           FALSE ... 何も無し
BOOL checkNpcEvent( int gx, int gy, int dx, int dy )
{
	int x, y;
	int ev;

	x = gx - mapAreaX1;
	y = gy - mapAreaY1;

	ev = (event[(y+dy)*mapAreaWidth+(x+dx)] & 0x0fff);
	// 移動先にNPC
	if( ev == EVENT_NPC )
	{
		// 移動を止める
		resetMap();
		return TRUE;
	}

	return FALSE;
}
#endif




///////////////////////////////////////////////////////////////////////////
// オートマッピング処理
//
void drawAutoMap( int x, int y )
{
	if( autoMappingInitFlag )
	{
		createAutoMap( nowFloor, nowGx, nowGy );
		autoMappingInitFlag = FALSE;
	}
	DrawAutoMapping( x, y, (unsigned char *)autoMappingBuf,
		AUTO_MAPPING_W, AUTO_MAPPING_H );
}


// (gx,gy)グリッド座標を中心としたオートマップカラーを
// autoMappingBufに格納する
BOOL createAutoMap( int floor, int gx, int gy )
{
	FILE *fp;
	char filename[255];
	int fWidth, fHeight, fOffset;
	int mWidth;
	int width, height;
	int fx, fy;
	int mx, my;
	int len, len2;
	int i, j;
	int x1, y1, x2, y2;
	int index;
	unsigned short tile[AUTO_MAPPING_W*AUTO_MAPPING_H];
	unsigned short parts[AUTO_MAPPING_W*AUTO_MAPPING_H];
	unsigned short event[AUTO_MAPPING_W*AUTO_MAPPING_H];

	memset( autoMappingBuf, 0, sizeof( autoMappingBuf ) );

	// 読み込みファイル名作成
	sprintf( filename, "map\\%d.dat", floor );

	// ファイルオープン
	if( (fp = fopen( filename, "rb" ))==NULL )
	{
		// 失敗するのは最初でマップフォルダが無いから
		// だからフォルダを作る
		_mkdir( "map" );
		// 読み込みデータも作る
		fp = fopen( filename, "wb" );
		fclose( fp );
		// 読み込みモードでもう一度オープン
		if( (fp = fopen( filename, "rb" ))==NULL )
			return FALSE;
	}

	memset( tile, 0, sizeof( tile ) );
	memset( parts, 0, sizeof( parts ) );
	memset( event, 0, sizeof( event ) );


	fseek( fp, 0, SEEK_SET );
	fread( &fWidth,  sizeof(int), 1, fp );
	fread( &fHeight, sizeof(int), 1, fp );

	x1 = gx - AUTO_MAPPING_W/2;
	y1 = gy - AUTO_MAPPING_H/2;
	x2 = x1 + AUTO_MAPPING_W;
	y2 = y1 + AUTO_MAPPING_H;


	mWidth = x2 - x1;
	width = mWidth;
	height = y2 - y1;

	mx = 0;
	fx = x1;
	if( x1 < 0 )
	{
		width += x1;
		fx = 0;
		mx -= x1;
	}
	if( x2 > fWidth )
	{
		width -= (x2 - fWidth);
	}
	my = 0;
	fy = y1;
	if( y1 < 0 )
	{
		height += y1;
		fy = 0;
		my -= y1;
	}
	if( y2 > fHeight )
	{
		height -= (y2 - fHeight);
	}


	fOffset = sizeof(int)*2;
	len = fy * fWidth + fx;
	len2 = my * mWidth + mx;
	for( i = 0; i < height; i++ )
	{
		fseek( fp, sizeof(short)*len+fOffset, SEEK_SET );
		fread( &tile[len2], sizeof(short)*width, 1, fp );
		len  += fWidth;
		len2 += mWidth;
	}

	fOffset += sizeof( short ) * (fWidth * fHeight);
	len = fy * fWidth + fx;
	len2 = my * mWidth + mx;
	for( i = 0; i < height; i++ )
	{
		fseek( fp, sizeof(short)*len+fOffset, SEEK_SET );
		fread( &parts[len2], sizeof(short)*width, 1, fp );
		len  += fWidth;
		len2 += mWidth;
	}

	fOffset += sizeof( short ) * (fWidth * fHeight);
	len = fy * fWidth + fx;
	len2 = my * mWidth + mx;
	for( i = 0; i < height; i++ )
	{
		fseek( fp, sizeof(short)*len+fOffset, SEEK_SET );
		fread( &event[len2], sizeof(short)*width, 1, fp );
		len  += fWidth;
		len2 += mWidth;
	}

	fclose (fp);


	// タイル設定
	for( i = 0; i < AUTO_MAPPING_H; i++ )
	{
		for( j = 0; j < AUTO_MAPPING_W; j++ )
		{
			// 行ったことの無い場所はオートマップに表示しない
			if( event[i*AUTO_MAPPING_W+j] & MAP_SEE_FLAG )
			{
				autoMappingBuf[i][j] =
					autoMapColorTbl[tile[i*AUTO_MAPPING_W+j]];
			}
		}
	}
	// パーツ設定
	for( i = 0; i < AUTO_MAPPING_H; i++ )
	{
		for( j = 0; j < AUTO_MAPPING_W; j++ )
		{
			// 行ったことの無い場所はオートマップに表示しない
			if( event[i*AUTO_MAPPING_W+j] & MAP_SEE_FLAG )
			{
				index = autoMapColorTbl[parts[i*AUTO_MAPPING_W+j]];
				if( index != 0 )
				{
					U4 bmpNo;
					S2 hit, hitX, hitY;
					int k, l;

					realGetNo( parts[i*AUTO_MAPPING_W+j], &bmpNo );
					// 当たり判定の有無
					realGetHitFlag( bmpNo, &hit );
					// 当たり判定あるならバッファに設定
					if( hit == 0 )
					{
						realGetHitPoints( bmpNo, &hitX, &hitY );
						for( k = 0; k < hitY; k++ )
						{
							for( l = 0; l < hitX; l++ )
							{
								if( (i - k) >= 0 && (j + l) < AUTO_MAPPING_W )
								{
									autoMappingBuf[i-k][j+l] = index;
								}
							}
						}
					}
					else
					{
						autoMappingBuf[i][j] = index;
					}
				}
			}
		}
	}

	return TRUE;
}


// 歩いた所のフラグを読みこむ
void readAutoMapSeeFlag( void )
{
	FILE *fp;
	char filename[255];
	int fWidth, fHeight, fOffset;
	int mWidth;
	int width, height;
	int fx, fy;
	int mx, my;
	int len, len2;
	int i, j;
	int x1, y1, x2, y2;


	// 読み込みファイル名作成
	sprintf( filename, "map\\%d.dat", nowFloor );

	// ファイルオープン
	if( (fp = fopen( filename, "rb" ))==NULL )
	{
		return;
	}

	memset( autoMapSeeFlagBuf, 0, sizeof( autoMapSeeFlagBuf ) );

	fseek( fp, 0, SEEK_SET );
	fread( &fWidth,  sizeof(int), 1, fp );
	fread( &fHeight, sizeof(int), 1, fp );

	x1 = nowGx - AUTO_MAPPING_SEE_W/2;
	y1 = nowGy - AUTO_MAPPING_SEE_H/2;
	x2 = x1 + AUTO_MAPPING_SEE_W;
	y2 = y1 + AUTO_MAPPING_SEE_H;


	mWidth = x2 - x1;
	width = mWidth;
	height = y2 - y1;

	mx = 0;
	fx = x1;
	if( x1 < 0 )
	{
		width += x1;
		fx = 0;
		mx -= x1;
	}
	if( x2 > fWidth )
	{
		width -= (x2 - fWidth);
	}
	my = 0;
	fy = y1;
	if( y1 < 0 )
	{
		height += y1;
		fy = 0;
		my -= y1;
	}
	if( y2 > fHeight )
	{
		height -= (y2 - fHeight);
	}


	fOffset = sizeof(int) * 2 + sizeof( short ) * (fWidth * fHeight) * 2;
	len = fy * fWidth + fx;
	len2 = my * mWidth + mx;
	for( i = 0; i < height; i++ )
	{
		fseek( fp, sizeof(short)*len+fOffset, SEEK_SET );
		fread( &autoMapSeeFlagBuf[len2], sizeof(short)*width, 1, fp );
		len  += fWidth;
		len2 += mWidth;
	}

	fclose (fp);

	// 通った所にフラグを立ててオートマップに表示されるようにする
	for( i = 0; i < AUTO_MAPPING_SEE_H; i++ )
	{
		for( j = 0; j < AUTO_MAPPING_SEE_W; j++ )
		{
			autoMapSeeFlagBuf[i*AUTO_MAPPING_SEE_W+j] |= MAP_SEE_FLAG;
		}
	}

	autoMapSeeFloor = nowFloor;
	autoMapSeeGx = nowGx;
	autoMapSeeGy = nowGy;

	autoMapSeeFlag = TRUE;
}


// 歩いた所のフラグを書き込む
void writeAutoMapSeeFlag( void )
{
	FILE *fp;
	char filename[255];
	int fWidth, fHeight, fOffset;
	int mWidth;
	int width, height;
	int fx, fy;
	int mx, my;
	int len, len2;
	int i;
	int x1, y1, x2, y2;


	// データがバッファに無いなら何もしない。
	if( !autoMapSeeFlag )
		return;

	autoMapSeeFlag = FALSE;

	// 読み込みファイル名作成
	sprintf( filename, "map\\%d.dat", autoMapSeeFloor );

	// ファイルオープン
	if( (fp = fopen( filename, "rb+" ))==NULL )
	{
		return;
	}


	fseek( fp, 0, SEEK_SET );
	fread( &fWidth,  sizeof(int), 1, fp );
	fread( &fHeight, sizeof(int), 1, fp );

	x1 = autoMapSeeGx - AUTO_MAPPING_SEE_W/2;
	y1 = autoMapSeeGy - AUTO_MAPPING_SEE_H/2;
	x2 = x1 + AUTO_MAPPING_SEE_W;
	y2 = y1 + AUTO_MAPPING_SEE_H;

	mWidth = x2 - x1;
	width = mWidth;
	height = y2 - y1;

	mx = 0;
	fx = x1;
	if( x1 < 0 )
	{
		width += x1;
		fx = 0;
		mx -= x1;
	}
	if( x2 > fWidth )
	{
		width -= (x2 - fWidth);
	}
	my = 0;
	fy = y1;
	if( y1 < 0 )
	{
		height += y1;
		fy = 0;
		my -= y1;
	}
	if( y2 > fHeight )
	{
		height -= (y2 - fHeight);
	}

	fOffset = sizeof(int) * 2 + sizeof( short ) * (fWidth * fHeight) * 2;
	len = fy * fWidth + fx;
	len2 = my * mWidth + mx;
	for( i = 0; i < height; i++ )
	{
		fseek( fp, sizeof(short)*len+fOffset, SEEK_SET );
		fwrite( &autoMapSeeFlagBuf[len2], sizeof(short)*width, 1, fp );
		len  += fWidth;
		len2 += mWidth;
	}

	fclose (fp);
}



///////////////////////////////////////////////////////////////////////////
// マップキャッシュ処理
#ifdef MAP_CACHE_PROC


// 初期化
void initMapCache( void )
{
	int i;

	for( i = 0; i < MAX_MAP_CACHE_SIZE; i++ )
	{
		mapCacheFloorNo[i] = -1;
		mapCacheFloorGxSize[i] = 0;
		mapCacheFloorGySize[i] = 0;
		clearMapCacheFlag( i );
		mapCacheLastTime[i] = 0;
	}

	mapCacheUse = 0;
}


// 移動先のマップがキャッシュされているかチェック
BOOL checkMapCache( int gx, int gy, short *buf, int size, int floor, int gxSize, int gySize )
{
	#define MAP_CACHE_RANGE		14
	#define MAP_CACHE_RANGE2	13
	int x1[2], y1[2], x2[2], y2[2];
	int tx1[2] = { 0xffff, 0xffff };
	int ty1[2] = { 0xffff, 0xffff };
	int tx2[2] = { 0, 0 };
	int ty2[2] = { 0, 0 };
	int dir;
	int dx, dy;
	int cnt;
	unsigned char bit;
	unsigned char tbit;
	int i;
	int mask;


	// 移動によって必要になるマップの範囲を計算する
	tbit = 0;
	for( cnt = 0; cnt < size; cnt++ )
	{
		dir = buf[cnt];

		bit = 0;
		if( 0 <= dir && dir <= 2 )
		{
			x1[0] = gx - MAP_CACHE_RANGE;
			x2[0] = x1[0] + 1;
			y1[0] = gy - MAP_CACHE_RANGE2;
			y2[0] = gy + MAP_CACHE_RANGE2 + 1;
			bit |= 1;
		}
		else
		if( 4 <= dir && dir <= 6 )
		{
			x1[0] = gx + MAP_CACHE_RANGE;
			x2[0] = x1[0] + 1;
			y1[0] = gy - MAP_CACHE_RANGE2;
			y2[0] = gy + MAP_CACHE_RANGE2 + 1;
			bit |= 1;
		}

		if( 2 <= dir && dir <= 4 )
		{
			x1[1] = gx - MAP_CACHE_RANGE2;
			x2[1] = gx + MAP_CACHE_RANGE2 + 1;
			y1[1] = gy - MAP_CACHE_RANGE;
			y2[1] = y1[1] + 1;
			bit |= 2;
		}
		else
		if( 6 <= dir && dir <= 7 || dir == 0 )
		{
			x1[1] = gx - MAP_CACHE_RANGE2;
			x2[1] = gx + MAP_CACHE_RANGE2 + 1;
			y1[1] = gy + MAP_CACHE_RANGE;
			y2[1] = y1[1] + 1;
			bit |= 2;
		}


		if( (bit & 1) != 0 )
		{
			if( x1[0] < tx1[0] )
				tx1[0] = x1[0];
			if( x2[0] > tx2[0] )
				tx2[0] = x2[0];
			if( y1[0] < ty1[0] )
				ty1[0] = y1[0];
			if( y2[0] > ty2[0] )
				ty2[0] = y2[0];
			tbit |= 1;
		}

		if( (bit & 2) != 0 )
		{
			if( x1[1] < tx1[1] )
				tx1[1] = x1[1];
			if( x2[1] > tx2[1] )
				tx2[1] = x2[1];
			if( y1[1] < ty1[1] )
				ty1[1] = y1[1];
			if( y2[1] > ty2[1] )
				ty2[1] = y2[1];
			tbit |= 2;
		}

		getRouteData( dir, &dx, &dy );
		gx += dx;
		gy += dy;
	}

	// マップの範囲がマップ内にあるかチェック
	bit = 0;
	for( i = 0, mask = 1; i < 2; i++, mask <<= 1 )
	{
		if( (tbit & mask) != 0 )
		{
			if( checkMapCacheFlag( floor, gxSize, gySize, tx1[i], ty1[i], tx2[i], ty2[i] ) )
			{
				if( !checkMapCacheEvent( tx1[i], ty1[i], tx2[i], ty2[i] ) )
				{
					bit |= mask;
				}
			}
		}
	}

	if( tbit == bit )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


// フラグのクリア
void clearMapCacheFlag( int no )
{
	if( no < 0 || MAX_MAP_CACHE_SIZE <= no )
		return;
	memset( mapCacheFlag[no], 0, MAP_CACHE_X_BYTE*MAP_CACHE_Y_BYTE );
}


// 指定位置のフラグをチェックする
//  戻り値：TRUE  ... キャッシュにある
//			FALSE ... キャッシュにない
BOOL checkMapCacheFlag( int floor, int gxSize, int gySize,
	int x1, int y1, int x2, int y2 )
{
	int xByteStart, xByteEnd;
	int yByteStart, yByteEnd;
	unsigned char bits;
	unsigned char mask;
	int i, j;
	BOOL ret = TRUE;
	BOOL thisFloorCacheFlag = FALSE;

	// 指定のフロア番号があるか調べる
	j = mapCacheUse;
	for( i = 0; i < MAX_MAP_CACHE_SIZE; i++ )
	{
		if( mapCacheFloorNo[j] == floor )
		{
			// フロアがあった
			mapCacheUse = j;
			thisFloorCacheFlag = TRUE;
			break;
		}

		j++;
		if( j >= MAX_MAP_CACHE_SIZE )
			j = 0;
	}

	// キャッシュに無い時
	if( !thisFloorCacheFlag )
	{
		BOOL flag;
		unsigned int tmpTime;

		// マップサイズが１００ｘ１００未満はキャッシュしない
		if( gxSize < 100 || gySize < 100 )
		{
			return FALSE;
		}

		// キャッシュするので空いてるところを探す
		flag = FALSE;
		for( i = 0; i < MAX_MAP_CACHE_SIZE; i++ )
		{
			if( mapCacheFloorNo[i] == -1 )
			{
				flag = TRUE;
				break;
			}
		}
		// 空きが無いので一番古いキャッシュを消す
		if( !flag )
		{
			i = 0;
			tmpTime = 0xffffffff;
			for( j = 0; j < MAX_MAP_CACHE_SIZE; j++ )
			{
				if( tmpTime > mapCacheLastTime[j] )
				{
					i = j;
					tmpTime = mapCacheLastTime[j];
				}
			}
		}

		// キャッシュ登録
		mapCacheUse = i;
		mapCacheFloorNo[i] = floor;
		mapCacheFloorGxSize[i] = gxSize;
		mapCacheFloorGySize[i] = gySize;
		clearMapCacheFlag( i );
		ret = FALSE;
	}

	// キャッシュへのアクセス時間登録
	mapCacheLastTime[mapCacheUse] = GetTickCount();


	if( x1 < 0 )
	{
		x1 = 0;
	}
	if( y1 < 0 )
	{
		y1 = 0;
	}
	if( x2 > mapCacheFloorGxSize[mapCacheUse] )
	{
		x2 = mapCacheFloorGxSize[mapCacheUse];
	}
	if( y2 > mapCacheFloorGySize[mapCacheUse] )
	{
		y2 = mapCacheFloorGySize[mapCacheUse];
	}

	// マップ範囲外
	if( x1 > x2 || y1 > y2 )
		return FALSE;

	xByteStart = x1/8;
	xByteEnd = x2/8;
	yByteStart = y1;
	yByteEnd = y2;

	for( i = xByteStart; i <= xByteEnd; i++ )
	{
		bits = 0xff;
		if( i == xByteStart )
		{
			mask = ((unsigned char)0xff >> (x1 % 8));
			bits &= mask;
		}
		if( i == xByteEnd )
		{
			mask = ((unsigned char)0xff << (7 - (x1 % 8)));
			bits &= mask;
		}
		for( j = yByteStart; j <= yByteEnd; j++ )
		{
			if( (mapCacheFlag[mapCacheUse][j*MAP_CACHE_X_BYTE+i] & bits) != bits )
			{
				// まだ来たことの無い所だった。
				ret = FALSE;
			}
			mapCacheFlag[mapCacheUse][j*MAP_CACHE_X_BYTE+i] |= bits;
		}
	}

	return ret;
}


// イベント情報内にEVENT_ALTERRATIVEがある時、TRUEを返す。
BOOL checkMapCacheEvent( int x1, int y1, int x2, int y2 )
{
	int x, y;
	int w, h;
	int i, j;

	if( x1 >= mapAreaX2 || x2 < 0
	 || y1 >= mapAreaY2 || y2 < 0 )
	{
		// チェックする場所が範囲外ならＴＲＵＥを返して
		// サーバにチェックサムを要求する
		return TRUE;
	}

	w = x2 - x1;
	h = y2 - y1;
	x = x1 - mapAreaX1;
	y = y1 - mapAreaY1;

	if( x < 0 )
	{
		w += x;
		x = 0;
	}
	if( y < 0 )
	{
		h += y;
		y = 0;
	}
	if( mapAreaX1+w > mapAreaX2 )
	{
		w -= (mapAreaX1+w-mapAreaX2);
	}
	if( mapAreaY1+h > mapAreaY2 )
	{
		h -= (mapAreaY1+h-mapAreaY2);
	}

	for( i = 0; i < h; i++, y++ )
	{
		for( j = 0; j < w; j++, x++ )
		{
			if( (event[y*mapAreaWidth+x] & 0x0fff) == EVENT_ALTERRATIVE )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}


#endif





///////////////////////////////////////////////////////////////////////////
// マップエフェクト


#define MAX_MAP_EFFECT_BUF	500

MAP_EFFECT masterBufMapEffect[MAX_MAP_EFFECT_BUF];
MAP_EFFECT *emptyBufMapEffect;
MAP_EFFECT *useBufMapEffect;

short mapEffectDrawFlag = 0;

short mapEffectRainLevel = 0;
short oldMapEffectRainLevel = 0;
short mapEffectRainCnt = 0;

short mapEffectSnowLevel = 0;
short oldMapEffectSnowLevel = 0;
short mapEffectSnowCnt = 0;

short mapEffectMoveDir = -1;

short mapEffectStarFlag = 0;
short mapEffectFallingStarFlag = 0;
int mapEffectFallingStarTime;

// マップエフェクトの初期化
void initMapEffect( void )
{
	int i;

	emptyBufMapEffect = &masterBufMapEffect[0];
	masterBufMapEffect[0].pre = (MAP_EFFECT *)NULL;
	for( i = 1; i < MAX_MAP_EFFECT_BUF; i++ )
	{
		masterBufMapEffect[i-1].next = &masterBufMapEffect[i];
		masterBufMapEffect[i].pre = &masterBufMapEffect[i-1];
	}
	masterBufMapEffect[i-1].next = (MAP_EFFECT *)NULL;

	useBufMapEffect = (MAP_EFFECT *)NULL;

	mapEffectRainLevel = 0;
	mapEffectRainCnt = 0;

	mapEffectSnowLevel = 0;
	mapEffectSnowCnt = 0;

	mapEffectMoveDir = -1;

	mapEffectStarFlag = 0;
	mapEffectFallingStarFlag = 1;
}


// エフェクトバッファを取得し表示設定をする
MAP_EFFECT *getMapEffectBuf( void )
{
	MAP_EFFECT *buf;

	// バッファが無いのでNULLを返す
	if( emptyBufMapEffect == (MAP_EFFECT *)NULL )
		return (MAP_EFFECT *)NULL;

	buf = emptyBufMapEffect;
	emptyBufMapEffect = emptyBufMapEffect->next;
	if( emptyBufMapEffect != (MAP_EFFECT *)NULL )
	{
		emptyBufMapEffect->pre = (MAP_EFFECT *)NULL;
	}

	buf->next = useBufMapEffect;
	buf->pre  = (MAP_EFFECT *)NULL;
	if( useBufMapEffect != (MAP_EFFECT *)NULL )
	{
		useBufMapEffect->pre = buf;
	}
	useBufMapEffect = buf;

	return buf;
}


// エフェクトバッファを開放する
void delMapEffectBuf( MAP_EFFECT *buf )
{
	if( buf == (MAP_EFFECT *)NULL )
		return;


	if( buf->pre != (MAP_EFFECT *)NULL )
	{
		buf->pre->next = buf->next;
	}
	if( buf->next != (MAP_EFFECT *)NULL )
	{
		buf->next->pre = buf->pre;
	}
	if( useBufMapEffect == buf )
	{
		useBufMapEffect = buf->next;
	}


	buf->next = emptyBufMapEffect;
	buf->pre = (MAP_EFFECT *)NULL;
	emptyBufMapEffect = buf;
	buf->type = MAP_EFFECT_TYPE_NONE;
}


// 移動による補正
void mapEffectDirHosei( int *x, int *y )
{
	if( mapEffectMoveDir < 0 )
		return;

	if( mapEffectMoveDir == 0 )
	{
		(*y) -= 2;
	}
	else
	if( mapEffectMoveDir == 1 )
	{
		(*x)++;
		(*y)--;
	}
	else
	if( mapEffectMoveDir == 2 )
	{
		(*x) += 2;
	}
	else
	if( mapEffectMoveDir == 3 )
	{
		(*x)++;
		(*y)++;
	}
	else
	if( mapEffectMoveDir == 4 )
	{
		(*y) += 2;
	}
	else
	if( mapEffectMoveDir == 5 )
	{
		(*x)--;
		(*y)++;
	}
	else
	if( mapEffectMoveDir == 6 )
	{
		(*x) -= 2;
	}
	else
	if( mapEffectMoveDir == 7 )
	{
		(*x)--;
		(*y)--;
	}
}



// sprdisp.cpp内のPutBmp()で呼ばれる
void drawMapEffect( void )
{
	if( mapEffectDrawFlag )
	{
		DrawMapEffect();
		mapEffectDrawFlag = 0;
	}
}


// 各エフェクトの計算
void mapEffectProc( void )
{
	mapEffectRain();
	mapEffectSnow();
#if 1
	mapEffectStar();
	mapEffectFallingStar();
#endif
}


void mapEffectProc2( int n )
{
	mapEffectRain2( n );
	mapEffectSnow2( n );
}


// 雨のエフェクト
void mapEffectRain( void )
{
	short mapEffectRainWaitTime = 0;
	short mapEffectRainFrameMax = 0;
	int hosei1 = 840;
	int hosei2 = -200;
	int ww = DEF_APPSIZEX;
	int hh = DEF_APPSIZEY;
	static unsigned int time = GetTickCount();
	MAP_EFFECT *buf;
	int i;


	// ３２０＊２４０ドットモードの時
	if( ResoMode == 1 )
	{
		ww /= 2;
		hh /= 2;
		hosei1 = 740;
		hosei2 = -100;
	}


	// 雨発生
	if( mapEffectRainLevel )
	{
		if( mapEffectRainLevel < 3 )
		{
			mapEffectRainWaitTime = (51 - 25*mapEffectRainLevel);
			if( time+mapEffectRainWaitTime < GetTickCount() )
			{
				MAP_EFFECT *buf = getMapEffectBuf();
				if( buf != (MAP_EFFECT *)NULL )
				{
					buf->type = MAP_EFFECT_TYPE_RAIN;
					buf->x = (rand() % hosei1) + hosei2;
					buf->y = 0;
					buf->dx = 2+(rand() % 2);
					buf->dy = 9;
					time = GetTickCount();
					mapEffectRainCnt++;
				}
			}
		}
		else
		{
			mapEffectRainFrameMax = (mapEffectRainLevel-2)*4;
			if( ResoMode == 1 )
			{
				mapEffectRainFrameMax /= 2;
			}
			for( i = 0; i < mapEffectRainFrameMax; i++ )
			{
				MAP_EFFECT *buf = getMapEffectBuf();
				if( buf != (MAP_EFFECT *)NULL )
				{
					buf->type = MAP_EFFECT_TYPE_RAIN;
					buf->x = (rand() % hosei1) + hosei2;
					buf->y = 0;
					buf->dx = 2+(rand() % 2);
					buf->dy = 9;
					time = GetTickCount();
					mapEffectRainCnt++;
				}
			}
		}
	}


	// 移動処理
	i = 0;
	buf = useBufMapEffect;
	while( buf != (MAP_EFFECT *)NULL )
	{
		if( buf->type == MAP_EFFECT_TYPE_RAIN )
		{
			buf->x += buf->dx;
			buf->y += buf->dy;
			mapEffectDirHosei( &buf->x, &buf->y );
			if( ww <= buf->x || hh <= buf->y )
			{
				// 移動終了
				MAP_EFFECT *buf2 = buf->next;
				delMapEffectBuf( buf );
				buf = buf2;
				mapEffectRainCnt--;
				continue;
			}
		}
		buf = buf->next;
		mapEffectDrawFlag = 1;
		i++;
	}

#if 0
	{
	char msg[256];
	sprintf( msg, "mapEffectRainCnt = %d / %d", mapEffectRainCnt, i );
	StockFontBuffer( 240, 54, FONT_PRIO_FRONT, 0, msg, 0 );
	}
#endif
}


// 雪のエフェクト
void mapEffectSnow( void )
{
	short mapEffectSnowWaitTime = 0;
	short mapEffectSnowFrameMax = 0;
	int hosei1 = 840;
	int hosei2 = -200;
	int ww = DEF_APPSIZEX;
	int hh = DEF_APPSIZEY;
	static unsigned int time = GetTickCount();
	MAP_EFFECT *buf;
	int i, j;


	// ３２０＊２４０ドットモードの時
	if( ResoMode == 1 )
	{
		ww /= 2;
		hh /= 2;
		hosei1 = 740;
		hosei2 = -100;
	}


	// 雪発生
	if( mapEffectSnowLevel )
	{
		if( mapEffectSnowLevel < 3 )
		{
			mapEffectSnowWaitTime = (51 - 25*mapEffectSnowLevel);
			if( time+mapEffectSnowWaitTime < GetTickCount() )
			{
				MAP_EFFECT *buf = getMapEffectBuf();
				if( buf != (MAP_EFFECT *)NULL )
				{
					buf->type = MAP_EFFECT_TYPE_SNOW;
					buf->x = (rand() % hosei1) + hosei2;
					buf->y = 0;
					j = (rand() % 2);
					if( j == 0 )
					{
						buf->dx = 0+(rand() % 2);
						buf->dy = 3+(rand() % 2);
					}
					else
					if( j == 1 )
					{
						buf->dx = 1;
						buf->dy = 4+(rand() % 2);
					}
					time = GetTickCount();
					mapEffectSnowCnt++;
				}
			}
		}
		else
		{
			mapEffectSnowFrameMax = (mapEffectSnowLevel-2)*2;
			if( ResoMode == 1 )
			{
				mapEffectSnowFrameMax /= 2;
			}
			for( i = 0; i < mapEffectSnowFrameMax; i++ )
			{
				MAP_EFFECT *buf = getMapEffectBuf();
				if( buf != (MAP_EFFECT *)NULL )
				{
					buf->type = MAP_EFFECT_TYPE_SNOW;
					buf->x = (rand() % hosei1) + hosei2;
					buf->y = 0;
					j = (rand() % 2);
					if( j == 0 )
					{
						buf->dx = 0+(rand() % 2);
						buf->dy = 3+(rand() % 2);
					}
					else
					if( j == 1 )
					{
						buf->dx = 1;
						buf->dy = 4+(rand() % 2);
					}
					time = GetTickCount();
					mapEffectSnowCnt++;
				}
			}
		}
	}


	// 移動処理
	i = 0;
	buf = useBufMapEffect;
	while( buf != (MAP_EFFECT *)NULL )
	{
		if( buf->type == MAP_EFFECT_TYPE_SNOW )
		{
			buf->x += buf->dx;
			buf->y += buf->dy;
			mapEffectDirHosei( &buf->x, &buf->y );
			if( ww <= buf->x || hh <= buf->y )
			{
				// 移動終了
				MAP_EFFECT *buf2 = buf->next;
				delMapEffectBuf( buf );
				buf = buf2;
				mapEffectSnowCnt--;
				continue;
			}
		}
		buf = buf->next;
		mapEffectDrawFlag = 1;
		i++;
	}

#if 0
	{
	char msg[256];
	sprintf( msg, "mapEffectSnowCnt = %d / %d", mapEffectSnowCnt, i );
	StockFontBuffer( 240, 54, FONT_PRIO_FRONT, 0, msg, 0 );
	}
#endif
}


// 星のエフェクト（地域限定）

void setEffectStar( MAP_EFFECT *buf, int gx, int gy, int ggx, int ggy, int type )
{
	if( buf )
	{
		buf->type = MAP_EFFECT_TYPE_STAR;
		buf->gx = gx;
		buf->gy = gy;
		buf->ggx = ggx;
		buf->ggy = ggy;
		buf->type2 = type;
	}
}

void calEffectStar( MAP_EFFECT *buf )
{
	float mx, my;

	// 画面表示位置
	camMapToGamen( (float)buf->gx*GRID_SIZE, (float)buf->gy*GRID_SIZE, &mx, &my );
	buf->x = (int)mx + buf->ggx;
	buf->y = (int)my + buf->ggy;
	//３２０＊２４０ドットモードの時
	if( ResoMode == 1 )
	{
		buf->x /= 2;
		buf->y /= 2;
	}
}


void mapEffectStar( void )
{
	static MAP_EFFECT *buf[30];
	int i;

	if( (nowFloor == 1200 || nowFloor == 20105 || nowFloor == 10920 || nowFloor == 20406)
	 && NOON_TO_EVENING+10 <= SaTime.hour && SaTime.hour < NIGHT_TO_MORNING+80 )
	{
		if( nowFloor == 1200 && mapEffectStarFlag == 0 )
		{
			for( i = 0; i < sizeof( buf )/sizeof( void *); i++ )
			{
				buf[i] = NULL;
			}
			buf[0] = getMapEffectBuf();
			setEffectStar( buf[0], 62, 28,   0,   0, 0 );
			buf[1] = getMapEffectBuf();
			setEffectStar( buf[1], 63, 27,  -5,  -5, 1 );
			buf[2] = getMapEffectBuf();
			setEffectStar( buf[2], 63, 28,  16, -10, 0 );
			buf[3] = getMapEffectBuf();
			setEffectStar( buf[3], 63, 29,  10,   5, 1 );
			buf[4] = getMapEffectBuf();
			setEffectStar( buf[4], 64, 29,   0,  -8, 1 );

			buf[5] = getMapEffectBuf();
			setEffectStar( buf[5], 65, 29,  -8,   5, 0 );
			buf[6] = getMapEffectBuf();
			setEffectStar( buf[6], 65, 30,  16,  10, 0 );
			buf[7] = getMapEffectBuf();
			setEffectStar( buf[7], 66, 31,   0,   0, 1 );
			buf[8] = getMapEffectBuf();
			setEffectStar( buf[8], 66, 30,   8, -12, 1 );
			buf[9] = getMapEffectBuf();
			setEffectStar( buf[9], 67, 31,  20,   0, 0 );

			buf[10] = getMapEffectBuf();
			setEffectStar( buf[10], 69, 34,  -8,  -8, 0 );
			buf[11] = getMapEffectBuf();
			setEffectStar( buf[11], 70, 34,   5,  10, 1 );
			buf[12] = getMapEffectBuf();
			setEffectStar( buf[12], 70, 35,   2,  24, 0 );
			buf[13] = getMapEffectBuf();
			setEffectStar( buf[13], 70, 36, -20, -16, 1 );
			buf[14] = getMapEffectBuf();
			setEffectStar( buf[14], 71, 36,   2,   4, 1 );

			buf[15] = getMapEffectBuf();
			setEffectStar( buf[15], 71, 37,   5, -10, 1 );
			buf[16] = getMapEffectBuf();
			setEffectStar( buf[16], 72, 37,   0,  12, 0 );


			mapEffectStarFlag = 1;
		}
		else
		if( nowFloor == 20105 && mapEffectStarFlag == 0 )
		{
			for( i = 0; i < sizeof( buf )/sizeof( void *); i++ )
			{
				buf[i] = NULL;
			}
			buf[0] = getMapEffectBuf();
			setEffectStar( buf[0], 21,  8,   5,   0, 1 );
			buf[1] = getMapEffectBuf();
			setEffectStar( buf[1], 20, 10,  -5,   5, 1 );
			buf[2] = getMapEffectBuf();
			setEffectStar( buf[2], 18, 11,   5,  10, 0 );
			buf[3] = getMapEffectBuf();
			setEffectStar( buf[3], 20, 12,   0,  -5, 1 );
			buf[4] = getMapEffectBuf();
			setEffectStar( buf[4], 23, 11,   0,   0, 0 );

			buf[5] = getMapEffectBuf();
			setEffectStar( buf[5], 23, 14,   0,   0, 0 );
			buf[6] = getMapEffectBuf();
			setEffectStar( buf[6], 25, 13,   8,   6, 1 );
			buf[7] = getMapEffectBuf();
			setEffectStar( buf[7], 25, 16,   0,   0, 1 );
			buf[8] = getMapEffectBuf();
			setEffectStar( buf[8], 26, 18,   4,  -8, 0 );
			buf[9] = getMapEffectBuf();
			setEffectStar( buf[9], 28, 16, -16,   0, 0 );

			buf[10] = getMapEffectBuf();
			setEffectStar( buf[10], 21, 14,  -5,   0, 1 );
			buf[11] = getMapEffectBuf();
			setEffectStar( buf[11], 27, 14,   0,   0, 1 );
			buf[12] = getMapEffectBuf();
			setEffectStar( buf[12], 24, 15,   4, -10, 1 );
			buf[13] = getMapEffectBuf();
			setEffectStar( buf[13], 28, 12,   5,  -8, 0 );
			buf[14] = getMapEffectBuf();
			setEffectStar( buf[14], 32, 14,   0,   0, 0 );

			buf[15] = getMapEffectBuf();
			setEffectStar( buf[15], 30, 17,   0,   0, 0 );
			buf[16] = getMapEffectBuf();
			setEffectStar( buf[16], 32, 19,   0,   0, 1 );
			buf[17] = getMapEffectBuf();
			setEffectStar( buf[17], 36, 19,   5,   2, 1 );
			buf[18] = getMapEffectBuf();
			setEffectStar( buf[18], 34, 22,  -4,   4, 0 );
			buf[19] = getMapEffectBuf();
			setEffectStar( buf[19], 36, 25,   1,   1, 0 );

			buf[20] = getMapEffectBuf();
			setEffectStar( buf[20], 39, 23,   0,   0, 0 );
			buf[21] = getMapEffectBuf();
			setEffectStar( buf[21], 38, 25,  -8,   2, 1 );
			buf[22] = getMapEffectBuf();
			setEffectStar( buf[22], 40, 24,  -5,  -5, 1 );
			buf[23] = getMapEffectBuf();
			setEffectStar( buf[23], 39, 27,   0,   0, 0 );
			buf[24] = getMapEffectBuf();
			setEffectStar( buf[24], 39, 30,   0,   0, 1 );

			buf[25] = getMapEffectBuf();
			setEffectStar( buf[25], 43, 29,   0,   0, 0 );
			buf[26] = getMapEffectBuf();
			setEffectStar( buf[26], 42, 32,   2,   0, 0 );
			buf[27] = getMapEffectBuf();
			setEffectStar( buf[27], 44, 33,   0,   0, 1 );
			buf[28] = getMapEffectBuf();
			setEffectStar( buf[28], 43, 34,   8,   2, 0 );
			buf[29] = getMapEffectBuf();
			setEffectStar( buf[29], 45, 36,   0,   0, 0 );


			mapEffectStarFlag = 1;
		}
		else
		if( nowFloor == 10920 && mapEffectStarFlag == 0 )
		{
			for( i = 0; i < sizeof( buf )/sizeof( void *); i++ )
			{
				buf[i] = NULL;
			}
			buf[0] = getMapEffectBuf();
			setEffectStar( buf[0], 71,  2,   0,   0, 1 );
			buf[1] = getMapEffectBuf();
			setEffectStar( buf[1], 71,  5,   0,   0, 0 );
			buf[2] = getMapEffectBuf();
			setEffectStar( buf[2], 70,  7,   0,   0, 1 );
			buf[3] = getMapEffectBuf();
			setEffectStar( buf[3], 73,  7,   0,   0, 1 );
			buf[4] = getMapEffectBuf();
			setEffectStar( buf[4], 75,  6,   0,   0, 0 );

			buf[5] = getMapEffectBuf();
			setEffectStar( buf[5], 75,  9,   0,   0, 0 );
			buf[6] = getMapEffectBuf();
			setEffectStar( buf[6], 75, 11,   0,   0, 1 );
			buf[7] = getMapEffectBuf();
			setEffectStar( buf[7], 77,  9,   0,   0, 1 );
			buf[8] = getMapEffectBuf();
			setEffectStar( buf[8], 76, 13,   0,   0, 0 );
			buf[9] = getMapEffectBuf();
			setEffectStar( buf[9], 79, 12,   0,   0, 0 );

			buf[10] = getMapEffectBuf();
			setEffectStar( buf[10], 78, 15,   0,   0, 1 );
			buf[11] = getMapEffectBuf();
			setEffectStar( buf[11], 80, 14,   0,   0, 0 );
			buf[12] = getMapEffectBuf();
			setEffectStar( buf[12], 79, 16,   0,   0, 1 );
			buf[13] = getMapEffectBuf();
			setEffectStar( buf[13], 80, 18,   0,   0, 0 );
			buf[14] = getMapEffectBuf();
			setEffectStar( buf[14], 83, 18,   0,   0, 0 );

			buf[15] = getMapEffectBuf();
			setEffectStar( buf[15], 83, 20,   0,   0, 1 );
			buf[16] = getMapEffectBuf();
			setEffectStar( buf[16], 84, 20,   0,   0, 1 );
			buf[17] = getMapEffectBuf();
			setEffectStar( buf[17], 84, 22,   0,   0, 0 );
			buf[18] = getMapEffectBuf();
			setEffectStar( buf[18], 85, 19,   0,   0, 0 );
			buf[19] = getMapEffectBuf();
			setEffectStar( buf[19], 87, 20,   0,   0, 1 );

			buf[20] = getMapEffectBuf();
			setEffectStar( buf[20], 89, 21,   0,   0, 0 );
			buf[21] = getMapEffectBuf();
			setEffectStar( buf[21], 89, 23,   0,   0, 1 );
			buf[22] = getMapEffectBuf();
			setEffectStar( buf[22], 89, 26,   0,   0, 1 );
			buf[23] = getMapEffectBuf();
			setEffectStar( buf[23], 91, 25,   0,   0, 0 );
			buf[24] = getMapEffectBuf();
			setEffectStar( buf[24], 91, 27,   0,   0, 1 );

			buf[25] = getMapEffectBuf();
			setEffectStar( buf[25], 93, 26,   0,   0, 1 );
			buf[26] = getMapEffectBuf();
			setEffectStar( buf[26], 93, 30,   0,   0, 0 );

			mapEffectStarFlag = 1;
		}
		else
		if( nowFloor == 20406 && mapEffectStarFlag == 0 )
		{
			for( i = 0; i < sizeof( buf )/sizeof( void *); i++ )
			{
				buf[i] = NULL;
			}
			buf[0] = getMapEffectBuf();
			setEffectStar( buf[0], 16,  8,   0,   0, 1 );
			buf[1] = getMapEffectBuf();
			setEffectStar( buf[1], 16, 10,   0,   0, 0 );
			buf[2] = getMapEffectBuf();
			setEffectStar( buf[2], 18, 10,   0,   0, 0 );
			buf[3] = getMapEffectBuf();
			setEffectStar( buf[3], 19, 12,   0,   0, 1 );
			buf[4] = getMapEffectBuf();
			setEffectStar( buf[4], 20, 12,   0,   0, 1 );

			buf[5] = getMapEffectBuf();
			setEffectStar( buf[5], 21, 14,   0,   0, 0 );
			buf[6] = getMapEffectBuf();
			setEffectStar( buf[6], 22, 16,   0,   0, 0 );
			buf[7] = getMapEffectBuf();
			setEffectStar( buf[7], 23, 16,   0,   0, 1 );
			buf[8] = getMapEffectBuf();
			setEffectStar( buf[8], 22, 11,   0,   0, 0 );
			buf[9] = getMapEffectBuf();
			setEffectStar( buf[9], 26, 15,   0,   0, 0 );

			buf[10] = getMapEffectBuf();
			setEffectStar( buf[10], 25, 18,   0,   0, 1 );
			buf[11] = getMapEffectBuf();
			setEffectStar( buf[11], 26, 17,   0,   0, 1 );
			buf[12] = getMapEffectBuf();
			setEffectStar( buf[12], 27, 13,   0,   0, 0 );
			buf[13] = getMapEffectBuf();
			setEffectStar( buf[13], 29, 14,   0,   0, 1 );
			buf[14] = getMapEffectBuf();
			setEffectStar( buf[14], 29, 12,   0,   0, 1 );

			buf[15] = getMapEffectBuf();
			setEffectStar( buf[15], 31, 17,   0,   0, 0 );
			buf[16] = getMapEffectBuf();
			setEffectStar( buf[16], 31, 19,   0,   0, 1 );
			buf[17] = getMapEffectBuf();
			setEffectStar( buf[17], 34, 18,   0,   0, 1 );
			buf[18] = getMapEffectBuf();
			setEffectStar( buf[18], 33, 21,   0,   0, 1 );
			buf[19] = getMapEffectBuf();
			setEffectStar( buf[19], 34, 23,   0,   0, 0 );

			buf[20] = getMapEffectBuf();
			setEffectStar( buf[20], 38, 26,   0,   0, 0 );
			buf[21] = getMapEffectBuf();
			setEffectStar( buf[21], 41, 25,   0,   0, 1 );
			buf[22] = getMapEffectBuf();
			setEffectStar( buf[22], 41, 26,   0,   0, 1 );
			buf[23] = getMapEffectBuf();
			setEffectStar( buf[23], 41, 32,   0,   0, 0 );
			buf[24] = getMapEffectBuf();
			setEffectStar( buf[24], 44, 31,   0,   0, 0 );

			buf[25] = getMapEffectBuf();
			setEffectStar( buf[25], 44, 33,   0,   0, 0 );
			buf[26] = getMapEffectBuf();
			setEffectStar( buf[26], 42, 43,   0,   0, 1 );
			buf[27] = getMapEffectBuf();
			setEffectStar( buf[27], 45, 36,   0,   0, 0 );
			buf[28] = getMapEffectBuf();
			setEffectStar( buf[28], 38, 21,   0,   0, 0 );
			buf[29] = getMapEffectBuf();
			setEffectStar( buf[29], 37, 19,   0,   0, 1 );

			mapEffectStarFlag = 1;
		}


		for( i = 0; i < sizeof( buf )/sizeof( void * ); i++ )
		{
			if( buf[i] )
			{
				calEffectStar( buf[i] );
				// 昼～夕１
				// 夜～朝３
				if( SaTime.hour < NOON_TO_EVENING+20
				 || NIGHT_TO_MORNING+70 < SaTime.hour )
				{
					buf[i]->mode = 5;
				}
				else
				// 昼～夕２
				// 夜～朝２
				if( (NOON_TO_EVENING+20 <= SaTime.hour && SaTime.hour < NOON_TO_EVENING+30)
				 || NIGHT_TO_MORNING+60 < SaTime.hour )
				{
					buf[i]->mode = 6;
				}
				else
				// 昼～夕３
				// 夜～朝１
				if( (NOON_TO_EVENING+30 <= SaTime.hour && SaTime.hour < NOON_TO_EVENING+40)
				 || NIGHT_TO_MORNING+40 < SaTime.hour )
				{
					buf[i]->mode = 8;
				}
				else
				{
					if( buf[i]->type2 == 0 )
					{
						// 昼～夕：少し星が大きくなる
						// 夜～朝：さらに星が小さくなる
						if( SaTime.hour < NOON_TO_EVENING+50
						 || NIGHT_TO_MORNING+30 < SaTime.hour )
						{
							buf[i]->mode = 4;
						}
						else
						// 昼～夕：さらに星が大きくなる
						// 夜～朝：少し星が小さくなる
						if( SaTime.hour < NOON_TO_EVENING+80
						 || NIGHT_TO_MORNING+20 < SaTime.hour )
						{
							buf[i]->mode = 1;
						}
						else
						{
							buf[i]->mode = (rand() % 2);
						}
					}
					else
					if( buf[i]->type2 == 1 )
					{
						buf[i]->mode = 8;
					}
				}
			}
		}
		mapEffectDrawFlag = 1;
	}
	else
	{
		if( mapEffectStarFlag )
		{
			for( i = 0; i < sizeof( buf )/sizeof( void * ); i++ )
			{
				delMapEffectBuf( buf[i] );
				buf[i] = NULL;
			}
			mapEffectStarFlag = 0;
		}
	}
}


// 流れ星エフェクト
void mapEffectFallingStar( void )
{
	static MAP_EFFECT *buf = NULL;

	if( mapEffectFallingStarFlag != 0
	 && (SaTime.hour < EVENING_TO_NIGHT || NIGHT_TO_MORNING < SaTime.hour) )
	{
		mapEffectFallingStarFlag = 0;
		mapEffectFallingStarTime = (rand() % 300)+400;
	}


#ifdef _DEBUG
//	if( (joy_trg[ 0 ] & JOY_RIGHT) && buf == NULL )
	if( !mapEffectFallingStarFlag && mapEffectFallingStarTime == SaTime.hour )
#else
	if( !mapEffectFallingStarFlag && mapEffectFallingStarTime == SaTime.hour )
#endif
	{
		if( nowFloor == 10920 )
		{
			buf = getMapEffectBuf();

			if( buf )
			{
				int i = (rand() % 5);

				switch( i )
				{
					case 0:
						setEffectStar( buf, 76, 6, 0, 0, 0 );
						buf->dx = 10;
						buf->dy = 6;
						buf->ey = 120;
						break;

					case 1:
						setEffectStar( buf, 77, 7, 0, 0, 0 );
						buf->dx = 8;
						buf->dy = 7;
						buf->ey = 120;
						break;

					case 2:
						setEffectStar( buf, 80, 13, 0, 0, 0 );
						buf->dx = -10;
						buf->dy = 9;
						buf->ey = 120;
						break;

					case 3:
						setEffectStar( buf, 84, 19, 0, 0, 0 );
						buf->dx = 12;
						buf->dy = 7;
						buf->ey = 90;
						break;

					case 4:
						setEffectStar( buf, 95, 26, 0, 0, 0 );
						buf->dx = -10;
						buf->dy = 7;
						buf->ey = 120;
						break;
				}
				// 星の種類
				if( (rand() % 10) < 6 )
				{
					buf->mode = 8;
				}
				else
				{
					buf->mode = 1;
				}
			}
			mapEffectFallingStarFlag = 1;
		}
		else
		if( nowFloor == 20105 )
		{
			buf = getMapEffectBuf();

			if( buf )
			{
				int i = (rand() % 8);
				i = 7;

				switch( i )
				{
					case 0:
						setEffectStar( buf, 22, 9, 0, 0, 0 );
						buf->dx = 10;
						buf->dy = 7;
						buf->ey = 120;
						break;

					case 1:
						setEffectStar( buf, 27, 15, 0, 0, 0 );
						buf->dx = -8;
						buf->dy = 9;
						buf->ey = 120;
						break;

					case 2:
						setEffectStar( buf, 31, 14, 0, 0, 0 );
						buf->dx = -10;
						buf->dy = 6;
						buf->ey = 120;
						break;

					case 3:
						setEffectStar( buf, 33, 18, 0, 0, 0 );
						buf->dx = 12;
						buf->dy = 8;
						buf->ey = 120;
						break;

					case 4:
						setEffectStar( buf, 38, 22, 0, 0, 0 );
						buf->dx = -11;
						buf->dy = 7;
						buf->ey = 120;
						break;

					case 5:
						setEffectStar( buf, 43, 27, 0, 0, 0 );
						buf->dx = -9;
						buf->dy = 6;
						buf->ey = 120;
						break;

					case 6:
						setEffectStar( buf, 41, 29, 0, 0, 0 );
						buf->dx = 12;
						buf->dy = 6;
						buf->ey = 120;
						break;

					case 7:
						setEffectStar( buf, 43, 33, 0, 0, 0 );
						buf->dx = 14;
						buf->dy = 8;
						buf->ey = 80;
						break;
				}
				// 星の種類
				if( (rand() % 10) < 6 )
				{
					buf->mode = 8;
				}
				else
				{
					buf->mode = 1;
				}
			}
			mapEffectFallingStarFlag = 1;
		}
		else
		if( nowFloor == 1200 )
		{
			buf = getMapEffectBuf();

			if( buf )
			{
				int i = (rand() % 3);
				i = 2;

				switch( i )
				{
					case 0:
						setEffectStar( buf, 63, 26, 0, 0, 0 );
						buf->dx = 10;
						buf->dy = 7;
						buf->ey = 80;
						break;

					case 1:
						setEffectStar( buf, 66, 30, 0, 0, 0 );
						buf->dx = -10;
						buf->dy = 6;
						buf->ey = 70;
						break;

					case 2:
						setEffectStar( buf, 70, 35, 0, 0, 0 );
						buf->dx = 8;
						buf->dy = 6;
						buf->ey = 40;
						break;
				}
				// 星の種類
				if( (rand() % 10) < 6 )
				{
					buf->mode = 8;
				}
				else
				{
					buf->mode = 1;
				}
			}
			mapEffectFallingStarFlag = 1;
		}
		else
		if( nowFloor == 20406 )
		{
			buf = getMapEffectBuf();

			if( buf )
			{
				int i = (rand() % 6);
				i = 5;

				switch( i )
				{
					case 0:
						setEffectStar( buf, 20, 11, 0, 0, 0 );
						buf->dx = -12;
						buf->dy = 9;
						buf->ey = 80;
						break;

					case 1:
						setEffectStar( buf, 23, 11, 0, 0, 0 );
						buf->dx = 10;
						buf->dy = 8;
						buf->ey = 120;
						break;

					case 2:
						setEffectStar( buf, 32, 14, 0, 0, 0 );
						buf->dx = -9;
						buf->dy = 6;
						buf->ey = 120;
						break;

					case 3:
						setEffectStar( buf, 36, 20, 0, 0, 0 );
						buf->dx = -10;
						buf->dy = 9;
						buf->ey = 120;
						break;

					case 4:
						setEffectStar( buf, 41, 24, 0, 0, 0 );
						buf->dx = 11;
						buf->dy = 7;
						buf->ey = 120;
						break;

					case 5:
						setEffectStar( buf, 44, 32, 0, 0, 0 );
						buf->dx = 10;
						buf->dy = 8;
						buf->ey = 120;
						break;
				}
				// 星の種類
				if( (rand() % 10) < 6 )
				{
					buf->mode = 8;
				}
				else
				{
					buf->mode = 1;
				}
			}
			mapEffectFallingStarFlag = 1;
		}
	}
	if( buf )
	{
		buf->ggx += buf->dx;
		buf->ggy += buf->dy;

		if( buf->ggy > buf->ey )
		{
			delMapEffectBuf( buf );
			buf = NULL;
		}
		else
		{
			calEffectStar( buf );
			mapEffectDrawFlag = 1;
		}
	}
}




// 雪のエフェクト
void mapEffectSnow2( int n )
{
	short mapEffectSnowWaitTime = 0;
	short mapEffectSnowFrameMax = 0;
	int hosei1 = 840;
	int hosei2 = -200;
	int ww = DEF_APPSIZEX;
	int hh = DEF_APPSIZEY;
	int time = 0;
	MAP_EFFECT *buf;
	int i, j, k;


	// ３２０＊２４０ドットモードの時
	if( ResoMode == 1 )
	{
		ww /= 2;
		hh /= 2;
		hosei1 = 740;
		hosei2 = -100;
	}


	for( k = 0; k < n; k++ )
	{

	// 雪発生
	if( mapEffectSnowLevel )
	{
		if( mapEffectSnowLevel < 3 )
		{
			mapEffectSnowWaitTime = (2-mapEffectSnowLevel)*2;
			if( time >= mapEffectSnowWaitTime )
			{
				MAP_EFFECT *buf = getMapEffectBuf();
				if( buf != (MAP_EFFECT *)NULL )
				{
					buf->type = MAP_EFFECT_TYPE_SNOW;
					buf->x = (rand() % hosei1) + hosei2;
					buf->y = 0;
					j = (rand() % 2);
					if( j == 0 )
					{
						buf->dx = 0+(rand() % 2);
						buf->dy = 3+(rand() % 2);
					}
					else
					if( j == 1 )
					{
						buf->dx = 1;
						buf->dy = 4+(rand() % 2);
					}
					time = 0;
					mapEffectSnowCnt++;
				}
			}
		}
		else
		{
			mapEffectSnowFrameMax = (mapEffectSnowLevel-2)*2;
			if( ResoMode == 1 )
			{
				mapEffectSnowFrameMax /= 2;
			}
			for( i = 0; i < mapEffectSnowFrameMax; i++ )
			{
				MAP_EFFECT *buf = getMapEffectBuf();
				if( buf != (MAP_EFFECT *)NULL )
				{
					buf->type = MAP_EFFECT_TYPE_SNOW;
					buf->x = (rand() % hosei1) + hosei2;
					buf->y = 0;
					j = (rand() % 2);
					if( j == 0 )
					{
						buf->dx = 0+(rand() % 2);
						buf->dy = 3+(rand() % 2);
					}
					else
					if( j == 1 )
					{
						buf->dx = 1;
						buf->dy = 4+(rand() % 2);
					}
					time = 0;
					mapEffectSnowCnt++;
				}
			}
		}
	}


	// 移動処理
	i = 0;
	buf = useBufMapEffect;
	while( buf != (MAP_EFFECT *)NULL )
	{
		if( buf->type == MAP_EFFECT_TYPE_SNOW )
		{
			buf->x += buf->dx;
			buf->y += buf->dy;
			mapEffectDirHosei( &buf->x, &buf->y );
			if( ww <= buf->x || hh <= buf->y )
			{
				// 移動終了
				MAP_EFFECT *buf2 = buf->next;
				delMapEffectBuf( buf );
				buf = buf2;
				mapEffectSnowCnt--;
				continue;
			}
		}
		buf = buf->next;
		mapEffectDrawFlag = 1;
		i++;
	}

	time++;
	}
}


// 雨のエフェクト
void mapEffectRain2( int n )
{
	short mapEffectRainWaitTime = 0;
	short mapEffectRainFrameMax = 0;
	int hosei1 = 840;
	int hosei2 = -200;
	int ww = DEF_APPSIZEX;
	int hh = DEF_APPSIZEY;
	int time = 0;
	MAP_EFFECT *buf;
	int i, k;


	// ３２０＊２４０ドットモードの時
	if( ResoMode == 1 )
	{
		ww /= 2;
		hh /= 2;
		hosei1 = 740;
		hosei2 = -100;
	}

	for( k = 0; k < n; k++ )
	{

	// 雨発生
	if( mapEffectRainLevel )
	{
		if( mapEffectRainLevel < 3 )
		{
			mapEffectRainWaitTime = (2 - mapEffectRainLevel)*2;
			if( time >= mapEffectRainWaitTime )
			{
				MAP_EFFECT *buf = getMapEffectBuf();
				if( buf != (MAP_EFFECT *)NULL )
				{
					buf->type = MAP_EFFECT_TYPE_RAIN;
					buf->x = (rand() % hosei1) + hosei2;
					buf->y = 0;
					buf->dx = 2+(rand() % 2);
					buf->dy = 9;
					time = 0;
					mapEffectRainCnt++;
				}
			}
		}
		else
		{
			mapEffectRainFrameMax = (mapEffectRainLevel-2)*4;
			if( ResoMode == 1 )
			{
				mapEffectRainFrameMax /= 2;
			}
			for( i = 0; i < mapEffectRainFrameMax; i++ )
			{
				MAP_EFFECT *buf = getMapEffectBuf();
				if( buf != (MAP_EFFECT *)NULL )
				{
					buf->type = MAP_EFFECT_TYPE_RAIN;
					buf->x = (rand() % hosei1) + hosei2;
					buf->y = 0;
					buf->dx = 2+(rand() % 2);
					buf->dy = 9;
					time = 0;
					mapEffectRainCnt++;
				}
			}
		}
	}


	// 移動処理
	i = 0;
	buf = useBufMapEffect;
	while( buf != (MAP_EFFECT *)NULL )
	{
		if( buf->type == MAP_EFFECT_TYPE_RAIN )
		{
			buf->x += buf->dx;
			buf->y += buf->dy;
			mapEffectDirHosei( &buf->x, &buf->y );
			if( ww <= buf->x || hh <= buf->y )
			{
				// 移動終了
				MAP_EFFECT *buf2 = buf->next;
				delMapEffectBuf( buf );
				buf = buf2;
				mapEffectRainCnt--;
				continue;
			}
		}
		buf = buf->next;
		mapEffectDrawFlag = 1;
		i++;
	}

	}
}




///////////////////////////////////////////////////////////////////////////
// マップの座標（グリッドではない）より画面座標にする。
//
void camMapToGamen( float sx , float sy , float *ex , float *ey )
{
	float x0, y0;
	float x, y;
	float tx = SURFACE_WIDTH/2;
	float ty = SURFACE_HEIGHT/2;

	x0 = ( sx - viewPointX ) / GRID_SIZE; 
	y0 = ( sy - viewPointY ) / GRID_SIZE; 

	x = + x0 * tx  +   y0 * tx ;
	y = - x0 * ty  +   y0 * ty ;

	//画面座標
	*ex = x + viewOffsetX;
	*ey = y + viewOffsetY;
}


//
// 画面座標よりマップの座標(グリッドではない)にする。
//
void camGamenToMap( float sx , float sy , float *ex , float *ey )
{
	float x0, y0;
	float x, y;

	// 画面座標
	x0 = sx - viewOffsetX; 
	y0 = sy - viewOffsetY; 

	x = x0 - (float)SURFACE_WIDTH / (float)SURFACE_HEIGHT * y0;
	y = x0 + (float)SURFACE_WIDTH / (float)SURFACE_HEIGHT * y0;

	*ex = x + viewPointX;
	*ey = y + viewPointY;
}

