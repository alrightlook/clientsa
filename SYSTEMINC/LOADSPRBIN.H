#ifndef _SPRITE_H_
#define _SPRITE_H_

//スプライトの�N����方を協�xしている。
//?��????��????�E?????
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
} ANIM_LIST;//??????��?????????????????


// 1フレ�`ムの秤��
typedef struct
{
	U4 BmpNo;				// bmp桑催(real.binでの了崔)
	S2 PosX,PosY;			// 燕幣了崔のオフセット
	U2 SoundNo;				// サウンド桑催
} FRAMELIST;


// １アニメ�`ションの秤��
typedef struct
{
	U2 dir;					// アニメ�`ションの�鬚�
	U2 no;					// 採のアニメ�`ションか(0は羨ちポ�`ズとか)
	U4 dtAnim;				// アニメ�`ションの�g佩�r�g
	U4 frameCnt;
	FRAMELIST *ptFramelist;	// １アニメ�`ションの秤��
} ANIMLIST;
//???????

// SPRの了崔秤�� ????????
typedef struct
{
	U4 sprNo;				// �I尖貧０から兵まる(�g�HのSPR桑催は65536ぐらいから)
	U4 offset;				// SPR.BINのファイルの�iみ�zみ了崔
	U2 animSize;			// アニメ�`ションの方
} SPRADRN;


typedef struct
{
	U2 animSize;				// アニメ�`ションの方
	ANIMLIST	*ptAnimlist;	//アニメ�`ションリストここがNULLなら岷俊BMPを燕幣
} SPRITEDATA;


typedef struct {
	U2			dtz;			//宙鮫されるときの��桑を�Qめる
	U2			noSpritedata;	//スプライト桑催
	U2			mdAnim;			//アニメ�`ションモ�`ド
	U2			mdAnim2;		//肝のアニメ�`ションモ�`ド
	U2			dir;			//圭�鬚錬�
	U2			ctLoop;			//採指ル�`プしたか��
	float		ctAnim;			//アニメ�`ションカウンタ�` ０゛１隆�困浪��嗤（�泣�I尖、屁方はフレ�`ム桑催�I尖
	float		dtAnim;			//スピ�`ド	これが０であれば、sprファイルから哈っ��る
} SPRITE;

extern SPRITEDATA SpriteData[mxSPRITE];


BOOL InitSprBinFileOpen( char *, char * );


#endif

