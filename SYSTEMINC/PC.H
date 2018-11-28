#ifndef _PC_H_ 
#define _PC_H_

#include"action.h"

#define MAXCHARACTER			2		// サーバーに保存する最大キャラ数
#define CHAR_NAME_LEN			16		// キャラの名前の長さ
#define CHAR_FREENAME_LEN		12		// ユーザが設定した称号の長さ
#define MAGIC_NAME_LEN			24		// 呪術名の長さ
#define MAGIC_MEMO_LEN			72		// 呪術の説明文の長さ
#define ITEM_NAME_LEN			28		// アイテム名の長さ
#define ITEM_NAME2_LEN			16		// アイテム名２の長さ
#define ITEM_MEMO_LEN			84		// アイテムの説明の長さ
#define PET_NAME_LEN			16		// ペットの名前の長さ


//#define CHARNAMELEN				256		// キャラリスト用

#define MAX_PET					5		// 最大ペット取得数

#define MAX_MAGIC				5		// 最大呪術数

#define MAX_PARTY				5		// 最大仲間数（自分を含む）

#define	MAX_ADR_BOOK_COUNT		4		// アドレスブックの１ページに表示する人数
#define	MAX_ADR_BOOK_PAGE		10		// アドレスブックの最大ページ数
#define MAX_ADR_BOOK			(MAX_ADR_BOOK_COUNT*MAX_ADR_BOOK_PAGE)	// アドレスブックの最大数

#define MAX_ITEM				20		// 最大アイテム数

#define RESULT_ITEM_COUNT		3		// 戦闘結果取得アイテム数
#define RESULT_ITEM_NAME_LEN	24		// 戦闘結果取得アイテム名の長さ
#define RESULT_CHR_EXP			4		// 戦闘結果ＥＸＰ取得メッセージの数

#define SKILL_NAME_LEN			24		// スキルの名前の長さ
#define SKILL_MEMO_LEN			72		// スキルのメモの長さ
#define MAX_SKILL				7		// １キャラが保持できる技の数


enum
{
	PC_ETCFLAG_PARTY		= 0x0001,	// 仲間ＯＫフラグビット
	PC_ETCFLAG_JOINT_BTL	= 0x0002,	// 参戦ＯＫフラグビット
	PC_ETCFLAG_DUEL			= 0x0004,	// 対戦ＯＫフラグビット
	PC_ETCFLAG_CHAT_MODE	= 0x0008,	// チャットモードフラグビット
	PC_ETCFLAG_MAIL			= 0x0010	// 名刺交換ＯＫフラグビット
};


#define ITEM_FLAG_PET_MAIL		( 1 << 0 ) 	// ペットメールで送信可能アイテム
#define ITEM_FLAG_MIX			( 1 << 1 ) 	// 根本的に合成できるか
#define ITEM_FLAG_COOKING_MIX	( 1 << 2 ) 	// 料理合成可能アイテム

typedef struct
{
	short useFlag;					// 使用フラグ
	char name[ITEM_NAME_LEN+1];		// アイテム名
	char name2[ITEM_NAME2_LEN+1];	// アイテム名２
	char memo[ITEM_MEMO_LEN+1];		// メモ
	int color;						// 名前の色
	int graNo;						// 画像番号
	int level;						// アイテムレベル
	short field;					// 使用できる場所
	short target;					// 対象
	short deadTargetFlag;			// 死んでるやつも対象フラグ
	short sendFlag;					// ペットメールで送信可能か
} ITEM;

typedef struct
{
	int graNo;						// グラフィック番号
	int faceGraNo;					// 顔画像番号
	int id;							// 管理用ID
	int dir;						// 向き
	int hp, maxHp;					// ＨＰ／最大ＨＰ
	int mp, maxMp;					// ＭＰ／最大ＭＰ
	int vital;						// 体力
	int str, tgh, dex;				// 力／打たれ強さ／素早さ
	int exp, maxExp;				// 経験値／次のレベルに必要な経験値
	int level;						// レベル
	int atk, def;					// 現在攻撃力／現在防御力
	int quick, charm, luck;			// 俊敏さ／魅力／運
	int earth, water, fire, wind;	// 地水火風
	int gold;						// お金
	int titleNo;					// 称号番号
	int dp;							// デュエルポイント
	char name[CHAR_NAME_LEN+1];		// 名前
	char freeName[CHAR_FREENAME_LEN+1];	// 自分でつける名前
	short nameColor;				// 名前の色
	unsigned short status;			// ステータス(ビットフラグ)
	unsigned short etcFlag;			// 仲間OK?戦闘途中参加OK?デュエルOK フラグ
	short battlePetNo;				// 戦闘に参加させるペットの入った配列番号（－１は参加させない）
	short selectPetNo[MAX_PET];		// 戦闘画面で選択できるペット
	short mailPetNo;				// メールを運ぶペットの番号
	int battleNo;					// 戦闘番号
	short sideNo;					// 戦闘時にどちら側か
	short helpMode;					// ヘルプモード
	ITEM item[MAX_ITEM];			// アイテム
	ACTION *ptAct;					// プレイヤーのアクション
} PC;


enum
{
	MAGIC_FIELD_ALL,		// すべての場所で使える
	MAGIC_FIELD_BATTLE,		// 戦闘中のみ
	MAGIC_FIELD_MAP			// 通常マップ上のみ
};

enum
{
	MAGIC_TARGET_MYSELF,				// 自分のみ
	MAGIC_TARGET_OTHER,					// 他の人（自分含む)
	MAGIC_TARGET_ALLMYSIDE,				// 味方全体
	MAGIC_TARGET_ALLOTHERSIDE,			// 相手側全体
	MAGIC_TARGET_ALL,					// 全て
	MAGIC_TARGET_NONE,					// ターゲット無し
	MAGIC_TARGET_OTHERWITHOUTMYSELF,	// 自分以外
	MAGIC_TARGET_WITHOUTMYSELFANDPET,	// 自分とペット以外
	MAGIC_TARGET_WHOLEOTHERSIDE,    	// どちらかのサイド全体
};

enum
{
	PETSKILL_FIELD_ALL,			// すべての場所で使える
	PETSKILL_FIELD_BATTLE,		// 戦闘中のみ
	PETSKILL_FIELD_MAP			// 通常マップ上のみ
};

enum
{
	PETSKILL_TARGET_MYSELF,				// 自分のみ
	PETSKILL_TARGET_OTHER,				// 他の人（自分含む)
	PETSKILL_TARGET_ALLMYSIDE,			// 味方全体
	PETSKILL_TARGET_ALLOTHERSIDE,		// 相手側全体
	PETSKILL_TARGET_ALL,				// 全て
	PETSKILL_TARGET_NONE,				// ターゲット無し
	PETSKILL_TARGET_OTHERWITHOUTMYSELF,	// 自分以外
	PETSKILL_TARGET_WITHOUTMYSELFANDPET	// 自分とペット以外
};

enum
{
	ITEM_FIELD_ALL,				// すべての場所で使える
	ITEM_FIELD_BATTLE,			// 戦闘中のみ
	ITEM_FIELD_MAP,				// 通常マップ上のみ
};

enum
{
	ITEM_TARGET_MYSELF,			// 自分のみ
	ITEM_TARGET_OTHER,			// 他の人（自分含む)
	ITEM_TARGET_ALLMYSIDE,		// 味方全体
	ITEM_TARGET_ALLOTHERSIDE,	// 相手側全体
	ITEM_TARGET_ALL,			// 全て
	ITEM_TARGET_NONE,				// ターゲット無し
	ITEM_TARGET_OTHERWITHOUTMYSELF,	// 自分以外
	ITEM_TARGET_WITHOUTMYSELFANDPET	// 自分とペット以外
};

typedef struct
{
	int graNo;						// 画像番号
	int hp, maxHp;					// ＨＰ／最大ＨＰ
	int mp, maxMp;					// ＭＰ／最大ＭＰ
	int exp, maxExp;				// 経験値／次のレベルに必要な経験値
	int level;						// レベル
	int atk, def;					// 現在攻撃力／現在防御力
	int quick;						// 俊敏さ
	int ai;							// 忠誠度
	int earth, water, fire, wind;	// 地水火風
	int maxSkill;					// 最大スキル数
	unsigned short status;			// ステータス(ビットフラグ)
	char name[CHAR_NAME_LEN+1];		// 種類名
	char freeName[PET_NAME_LEN+1];	// ユーザがつける名前
	short useFlag;					// この欄にペットがいるかフラグ
	short changeNameFlag;			// 名前をつけれるかフラグ
} PET;


typedef struct
{
	short useFlag;					// 使用フラグ
	int mp;							// 必要ＭＰ
	short field;					// 使用できる場所
	short target;					// 対象
	short deadTargetFlag;			// 死んでるやつも対象フラグ
	char name[MAGIC_NAME_LEN+1];	// 呪術名
	char memo[MAGIC_MEMO_LEN+1];	// 説明
} MAGIC;


typedef struct
{
	short useFlag;					// 使用フラグ
	int id;							// 管理ＩＤ
	int level;						// レベル
	int maxHp;						// 最大ＨＰ
	int hp;							// ＨＰ
	int mp;							// ＭＰ
	char name[CHAR_NAME_LEN+1];		// 名前
	ACTION *ptAct;					// アクション
} PARTY;


typedef struct
{
	short useFlag;					// 使用フラグ
	short onlineFlag;				// オンラインどうか
	int level;						// レベル
	int dp;							// デュエルポイント
	int graNo;						// 画像番号
	char name[CHAR_NAME_LEN+1];		// 名前
} ADDRESS_BOOK;


typedef struct
{
	short petNo;					// ペット番号
	short levelUp;					// レベルアップフラグ
	int exp;						// 取得経験値
} BATTLE_RESULT_CHR;

typedef struct
{
	short useFlag;					// 使用フラグ
	BATTLE_RESULT_CHR resChr[RESULT_CHR_EXP];	// 戦闘結果経験値メッセージ
	char item[RESULT_ITEM_COUNT][RESULT_ITEM_NAME_LEN+1];		// 取得アイテム名
} BATTLE_RESULT_MSG;


typedef struct
{
	short useFlag;						// 使用フラグ
	short skillId;						// スキルＩＤ
	short field;						// 使用できる場所
	short target;						// 技の対象
	char name[SKILL_NAME_LEN+1];		// 技の名前
	char memo[SKILL_MEMO_LEN+1];		// メモ
} PET_SKILL;


typedef struct
{
	char name[CHAR_NAME_LEN+1];		// SJISで名前をいれる
	short level;					// レベル
	int login;						// ログイン回数

	int faceGraNo;					// 顔画像番号
	int hp;							// ＨＰ
	int str;						// 攻撃力
	int def;						// 防御力
	int agi;						// 敏捷力
	int app;						// 魅力
	int attr[4];					// 属性（地?水?火?風）
	int dp;							// デュエルポイント
} CHARLISTTABLE;


extern PC pc;
extern short maxPcNo;
extern short selectPcNo;
extern short prSendMode;
extern short prSendFlag;
extern short jbSendFlag;
extern short duelSendFlag;

extern int loginDp;

extern short helpFlag;


extern MAGIC magic[];

extern PET pet[];

extern PARTY party[];
extern short partyModeFlag;

extern ADDRESS_BOOK addressBook[];
extern BOOL addressBookFlag;

extern BATTLE_RESULT_MSG battleResultMsg;

extern PET_SKILL petSkill[][MAX_SKILL];

extern CHARLISTTABLE chartable[];

extern char newCharacterName[];
extern int newCharacterGraNo;
extern int newCharacterFaceGraNo;
extern int newCharacterVit;
extern int newCharacterStr;
extern int newCharacterTgh;
extern int newCharacterDex;
extern int newCharacterEarth;
extern int newCharacterWater;
extern int newCharacterFire;
extern int newCharacterWind;
extern int newCharacterHomeTown;

void initPcAll( void );
void initPc( void );
void createPc( int, int, int, int );
void resetPc( void );
void setPcMovePoint( int, int );
void setPcWarpPoint( int, int );
void setPcPoint( void );
void setPcGraNo( int, int );
void setPcId( int );
void setPcDir( int );
void setPcAction( int );
int getPcAction( void );
void setPcParam( char *, char *, int, int, int, int );
void updataPcAct( void );
void setPcLeader( void );
void delPcLeader( void );
void setPcParty( void );
void delPcParty( void );
void setPcWatch( void );
void delPcWatch( void );
void changePcAct( int, int, int, int, int, int, int );
void setPcWalkFlag( void );
void delPcWalkFlag( void );
int checkPcWalkFlag( void );
void setPcUseMagic( void );
void delPcUseMagic( void );
void setPcFukidashi( unsigned int );

void clearPartyParam( void );
void clearPtActPartyParam( void );

int existCharacterListEntry( int );
int cmpNameCharacterList( char * );
int setCharacterList( char *, char * );
int resetCharacterList( int );

void getItem( void );
void swapItem( int, int );

BOOL lookAtAround( void );


void initAddressBook( void );
void addressBookProc( void );

#endif

