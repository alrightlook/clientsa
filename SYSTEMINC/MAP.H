#ifndef _MAP_H_ 
#define _MAP_H_

// マップタイル表示時のグリッド範囲（中心からの相対範囲）
//ｵｱｵﾘﾍｼtileﾍ�ｸ�ﾏﾔﾊｾﾇ�ﾓ�  ｵｱｵﾘﾍｼﾆｽﾆﾌﾏﾔﾊｾﾇ�ﾓ�ｵ醢� ｣ｨｷｶﾎｧﾏ犖ﾔﾓﾚﾖﾐﾐﾄ｣ｩ
#if 1
#define MAP_TILE_GRID_X1	-20
#define MAP_TILE_GRID_X2	+17		// ｲｻｰ�ﾀｨｽ睫�
#define MAP_TILE_GRID_Y1	-16
#define MAP_TILE_GRID_Y2	+21		// 端は含まない
#else
#define MAP_TILE_GRID_X1	-19
#define MAP_TILE_GRID_X2	+15		// 端は含まない
#define MAP_TILE_GRID_Y1	-14
#define MAP_TILE_GRID_Y2	+20		// 端は含まない
#endif

// ｵﾘﾍｼｳﾟｴ�
#define MAP_X_SIZE	(MAP_TILE_GRID_X2 - MAP_TILE_GRID_X1)
#define MAP_Y_SIZE	(MAP_TILE_GRID_Y2 - MAP_TILE_GRID_Y1)

// ﾍ�ｸ�ｵﾄｴ�ﾐ｡
#define GRID_SIZE		64

// ﾒﾆ�ﾓﾋﾙｶﾈ
#define MOVE_SPEED			4
#define MOVE_FRAME			(GRID_SIZE/MOVE_SPEED)

// ﾊﾂｼ�ｱ犲ﾅ
enum
{
	EVENT_NONE,			// ﾊﾂｼ�ｲｻｻ盥｢ﾉ�  
	EVENT_NPC,			// 固定NPC  ｹﾌｶｨNPC 
	EVENT_ENEMY,		// ｳﾖﾐ�ﾔ簽�ｵﾐﾈﾋ  ｾﾍﾊﾇﾓ�ｵﾐ
	EVENT_WARP,			// ｾｭ  ﾗｪｻｻｵﾘﾍｼ｣ｿ
	EVENT_DOOR,			// ﾃﾅ
	//ﾋﾑﾋ�ｵﾘﾍｼ ﾈ郢�ﾊﾇｵﾄｻｰ
	EVENT_ALTERRATIVE,	// 何かあるかも知れないのでマップの要求を必ずする
	EVENT_WARP_MONING,	// 朝限定ワープ  ﾉﾏﾎ�
	EVENT_WARP_NOON,	// 昼限定ワープ  ﾖﾐﾎ�
	EVENT_WARP_NIGHT,	// 夜限定ワープ  ﾏﾂﾎ醯ﾞﾖﾆ

	EVENT_END		// ﾗ�ｺ�ﾒｻｸ�ﾊﾂｼ�ｱ犲ﾅ
};

// ｱ�ﾖｾｵﾄｵﾘﾍｼ
//ﾈ郢�ﾓﾐﾊ�ｾﾝｱ�ﾖｾ｣ｨｦﾂｰ譽ｩ  ｶﾁﾈ｡
#define MAP_READ_FLAG	0x8000		// データがあるかのフラグ（βバージョン）
//ﾐﾔｸ�ｵﾄｱ�ﾖｾｵｽｸﾃﾎｻﾖﾃ  ｿﾉｼ�
#define MAP_SEE_FLAG	0x4000		// キャラがその場所に行ったかのフラグ

// ｵﾘﾗｩｵﾄﾃ�ｳﾆｵﾄｳ､ｶﾈ
#define FLOOR_NAME_LEN	24

extern char nowFloorName[];
extern int nowFloor;
extern int nowFloorGxSize, nowFloorGySize;
extern int nowGx, nowGy;
extern float nowX, nowY;
extern float nowVx, nowVy, nowSpdRate;
extern int nextGx, nextGy;
extern int oldGx, oldGy;
extern int oldNextGx, oldNextGy;
extern int mouseMapGx, mouseMapGy;
extern int mouseMapX, mouseMapY;

extern BOOL mapEmptyFlag;

extern float viewPointX;
extern float viewPointY;
extern int viewOffsetX;
extern int viewOffsetY;

extern short moveAddTbl[8][2];


extern BOOL mouseLeftCrick;//ﾊ�ｱ�ﾗ�ｼ�ｰｴﾏﾂ｣ｿ
extern BOOL mouseLeftOn;
extern BOOL mouseRightCrick;
extern BOOL mouseRightOn;
extern unsigned int mouseLeftPushTime;//ｰｴﾑｹﾊｱｼ茱ｿ
extern unsigned int beforeMouseLeftPushTime;


extern short nowEncountPercentage;
extern short nowEncountExtra;
extern short minEncountPercentage;
extern short maxEncountPercentage;
extern short sendEnFlag;
extern short encountNowFlag;

extern short eventWarpSendFlag;
extern short eventWarpSendId;
extern short eventEnemySendFlag;
extern short eventEnemySendId;
extern short eventEnemyFlag;

extern BOOL loginFlag;

extern BOOL floorChangeFlag;

extern BOOL warpEffectFlag;
extern BOOL warpEffectStart;
extern BOOL warpEffectOk;


void initMap( void );
void resetMap( void );
void createMap( int, int, int );
BOOL writeMap( int, int, int, int, int,
	unsigned short *, unsigned short *, unsigned short * );
void setMap( int, int, int );
void setMoveMap( int, int );
void setWarpMap( int, int );
BOOL mapCheckSum( int, int, int, int, int, int, int, int );
void drawMap( void );
void redrawMap( void );
void drawGrid( void );
void moveProc( void );
BOOL checkEmptyMap( int );
void setMapMovePoint( int, int );
void setMapMovePoint2( int, int );
void mapMove2( void );

void getRouteData( int, int *, int * );

void forceWarpMap( int, int );
char cnvServDir( int, int );

void initCharPartsPrio( void );
void setPartsPrio( int, int, int, int, int, float, float );
void setCharPrio( int, int, int, int, int, float, float );
void stockCharParts( void );


#if 0
BOOL checkNpcEvent( int, int, int, int );
#endif

void drawAutoMap( int x, int y );
void readAutoMapSeeFlag( void );
void writeAutoMapSeeFlag( void );

BOOL checkHitMap( int, int );

void camMapToGamen( float, float, float *, float * );
void camGamenToMap( float, float, float *, float * );


#if 1

enum
{
	MAP_EFFECT_TYPE_NONE,//ﾎﾞ
	MAP_EFFECT_TYPE_RAIN,//ﾓ�
	MAP_EFFECT_TYPE_SNOW,//ﾑｩ
	MAP_EFFECT_TYPE_STAR,//ﾐﾇ
	MAP_EFFECT_TYPE_HOTARU//ﾔﾂ
};

typedef struct _MAP_EFFECT
{
	int x, y;
	int dx, dy;
	int gx, gy;
	int ggx, ggy;
	int ex, ey;
	_MAP_EFFECT *pre;
	_MAP_EFFECT *next;
	short type;
	short type2;
	short mode;
} MAP_EFFECT;

extern MAP_EFFECT *useBufMapEffect;

extern short mapEffectRainLevel;
extern short oldMapEffectRainLevel;
extern short mapEffectSnowLevel;
extern short oldMapEffectSnowLevel;
extern short mapEffectMoveDir;

void initMapEffect( void );
void drawMapEffect( void );
void mapEffectProc( void );
void mapEffectRain( void );
void mapEffectSnow( void );
void mapEffectStar( void );
void mapEffectFallingStar( void );

void mapEffectProc2( int );
void mapEffectRain2( int );
void mapEffectSnow2( int );


#endif



#endif
