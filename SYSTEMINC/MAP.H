#ifndef _MAP_H_ 
#define _MAP_H_

// 儅僢僾僞僀儖昞帵帪偺僌儕僢僪斖埻乮拞怱偐傜偺憡懳斖埻乯
//当地图tile网格显示区域  当地图平铺显示区域电网 （范围相对于中心）
#if 1
#define MAP_TILE_GRID_X1	-20
#define MAP_TILE_GRID_X2	+17		// 不包括结束
#define MAP_TILE_GRID_Y1	-16
#define MAP_TILE_GRID_Y2	+21		// 抂偼娷傑側偄
#else
#define MAP_TILE_GRID_X1	-19
#define MAP_TILE_GRID_X2	+15		// 抂偼娷傑側偄
#define MAP_TILE_GRID_Y1	-14
#define MAP_TILE_GRID_Y2	+20		// 抂偼娷傑側偄
#endif

// 地图尺寸
#define MAP_X_SIZE	(MAP_TILE_GRID_X2 - MAP_TILE_GRID_X1)
#define MAP_Y_SIZE	(MAP_TILE_GRID_Y2 - MAP_TILE_GRID_Y1)

// 网格的大小
#define GRID_SIZE		64

// 移動速度
#define MOVE_SPEED			4
#define MOVE_FRAME			(GRID_SIZE/MOVE_SPEED)

// 事件编号
enum
{
	EVENT_NONE,			// 事件不会发生  
	EVENT_NPC,			// 屌掕NPC  固定NPC 
	EVENT_ENEMY,		// 持续遭遇敌人  就是遇敌
	EVENT_WARP,			// 经  转换地图？
	EVENT_DOOR,			// 门
	//搜索地图 如果是的话
	EVENT_ALTERRATIVE,	// 壗偐偁傞偐傕抦傟側偄偺偱儅僢僾偺梫媮傪昁偢偡傞
	EVENT_WARP_MONING,	// 挬尷掕儚乕僾  上午
	EVENT_WARP_NOON,	// 拫尷掕儚乕僾  中午
	EVENT_WARP_NIGHT,	// 栭尷掕儚乕僾  下午限制

	EVENT_END		// 最后一个事件编号
};

// 标志的地图
//如果有数据标志（β版）  读取
#define MAP_READ_FLAG	0x8000		// 僨乕僞偑偁傞偐偺僼儔僌乮兝僶乕僕儑儞乯
//性格的标志到该位置  可见
#define MAP_SEE_FLAG	0x4000		// 僉儍儔偑偦偺応強偵峴偭偨偐偺僼儔僌

// 地砖的名称的长度
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


extern BOOL mouseLeftCrick;//鼠标左键按下？
extern BOOL mouseLeftOn;
extern BOOL mouseRightCrick;
extern BOOL mouseRightOn;
extern unsigned int mouseLeftPushTime;//按压时间？
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
	MAP_EFFECT_TYPE_NONE,//无
	MAP_EFFECT_TYPE_RAIN,//雨
	MAP_EFFECT_TYPE_SNOW,//雪
	MAP_EFFECT_TYPE_STAR,//星
	MAP_EFFECT_TYPE_HOTARU//月
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
