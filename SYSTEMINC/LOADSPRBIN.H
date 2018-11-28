#ifndef _SPRITE_H_
#define _SPRITE_H_

//スプライトの種類個数を定義している。
//?獎????蠏????鯡?????
#define mxSPRITE	32768

typedef unsigned char MOJI;
typedef unsigned char U1;
typedef          char S1;
typedef unsigned short U2;
typedef          short S2;
typedef unsigned long U4;
typedef          long S4;
typedef float  F4;
typedef double F8;

typedef enum {
	ANIM_ATTACK,
	ANIM_DAMAGE,
	ANIM_DEAD,
	ANIM_STAND,
	ANIM_WALK,
	ANIM_SIT,
	ANIM_HAND,
	ANIM_HAPPY,
	ANIM_ANGRY,
	ANIM_SAD,
	ANIM_GUARD,
	ANIM_NOD,
	ANIM_THROW,
	ANIM_LIST_
} ANIM_LIST;//??????惞?????????????????


// 1フレームの情報
typedef struct
{
	U4 BmpNo;				// bmp番号(real.binでの位置)
	S2 PosX,PosY;			// 表示位置のオフセット
	U2 SoundNo;				// サウンド番号
} FRAMELIST;


// １アニメーションの情報
typedef struct
{
	U2 dir;					// アニメーションの向き
	U2 no;					// 何のアニメーションか(0は立ちポーズとか)
	U4 dtAnim;				// アニメーションの実行時間
	U4 frameCnt;
	FRAMELIST *ptFramelist;	// １アニメーションの情報
} ANIMLIST;
//???????

// SPRの位置情報 ????????
typedef struct
{
	U4 sprNo;				// 処理上０から始まる(実際のSPR番号は65536ぐらいから)
	U4 offset;				// SPR.BINのファイルの読み込み位置
	U2 animSize;			// アニメーションの数
} SPRADRN;


typedef struct
{
	U2 animSize;				// アニメーションの数
	ANIMLIST	*ptAnimlist;	//アニメーションリストここがNULLなら直接BMPを表示
} SPRITEDATA;


typedef struct {
	U2			dtz;			//描画されるときの順番を決める
	U2			noSpritedata;	//スプライト番号
	U2			mdAnim;			//アニメーションモード
	U2			mdAnim2;		//次のアニメーションモード
	U2			dir;			//方向は？
	U2			ctLoop;			//何回ループしたか？
	float		ctAnim;			//アニメーションカウンター ０～１未満は不動小数点処理、整数はフレーム番号処理
	float		dtAnim;			//スピード	これが０であれば、sprファイルから引っ張る
} SPRITE;

extern SPRITEDATA SpriteData[mxSPRITE];


BOOL InitSprBinFileOpen( char *, char * );


#endif

