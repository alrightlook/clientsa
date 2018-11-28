#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include"action.h"
#include"pc.h"

enum
{
	CHR_STATUS_P 		= 0x0001,			// 毒
	CHR_STATUS_N 		= 0x0002,			// 麻痺
	CHR_STATUS_Q 		= 0x0004,			// 沈黙
	CHR_STATUS_S 		= 0x0008,			// 石化
	CHR_STATUS_D 		= 0x0010,			// 暗闇
	CHR_STATUS_C 		= 0x0020,			// 混乱
	CHR_STATUS_W 		= 0x0040,			// この上を歩けるか
	CHR_STATUS_H 		= 0x0080,			// 高さがあるか
	CHR_STATUS_LEADER	= 0x0100,			// リーダー
	CHR_STATUS_PARTY	= 0x0200,			// パーティの一員
	CHR_STATUS_BATTLE	= 0x0400,			// 戦闘中
	CHR_STATUS_USE_MAGIC= 0x0800,			// 呪術使用
	CHR_STATUS_HELP		= 0x1000,			// ヘルプ
	CHR_STATUS_FUKIDASHI= 0x2000,			// 吹き出し
	CHR_STATUS_WATCH	= 0x4000			// 観戦中
};


enum
{											// キャラ管理時の種類
	CHAROBJ_TYPE_NPC		= 0x0001,		// NPC
	CHAROBJ_TYPE_ITEM		= 0x0002,		// アイテム
	CHAROBJ_TYPE_MONEY		= 0x0004,		// お金
	CHAROBJ_TYPE_USER_NPC	= 0x0008,		// 他人のキャラクタ
	CHAROBJ_TYPE_LOOKAT		= 0x0010,		// 見れる
	CHAROBJ_TYPE_ALL		= 0x00FF		// 全て
};


// サーバ上での管理番号
enum
{
	CHAR_TYPENONE,			// 何でもない
	CHAR_TYPEPLAYER,		// プレイヤー
	CHAR_TYPEENEMY,			// 敵
	CHAR_TYPEPET,			// ペット
	CHAR_TYPEDOOR,			// ドア
	CHAR_TYPEBOX,			// 宝箱
	CHAR_TYPEMSG,			// 看板
	CHAR_TYPEWARP,			// ワープゾーン
	CHAR_TYPESHOP,			// 店
	CHAR_TYPEHEALER,		// ヒーラー
	CHAR_TYPEOLDMAN,		// 長老
	CHAR_TYPEROOMADMIN,		// 不動産屋
	CHAR_TYPETOWNPEOPLE,	// まちのひと
	CHAR_TYPEDENGON,		// 伝言版
	CHAR_TYPEADM,			// 伝言版
	CHAR_TYPETEMPLE,		// Temple master
	CHAR_TYPESTORYTELLER,	// 語り部
	CHAR_TYPERANKING,		// 不動産ランキング表示ＮＰＣ
	CHAR_TYPEOTHERNPC,		// その他の検索対象にならないNPC
	CHAR_TYPEPRINTPASSMAN,	// ドアのパスワード表示するNPC
	CHAR_TYPENPCENEMY,		// 固定敵
	CHAR_TYPEACTION,		// アクションに反応するNPC
	CHAR_TYPEWINDOWMAN,		// ウィンドウ表示するNPC（テストかも)
	CHAR_TYPESAVEPOINT,		// セーブポイント
	CHAR_TYPEWINDOWHEALER,	// ウインドウタイプのヒーラー
	CHAR_TYPEITEMSHOP,		// お店
	CHAR_TYPESTONESHOP,		// 石盤屋（ペットの技屋）
	CHAR_TYPEDUELRANKING,	// DUELランキングNPC
	CHAR_TYPENUM
};


typedef struct
{
	short use;				// 使用中か
	short type;				// 種類
	int id;					// サーバの管理番号
	int graNo;				// グラフィック番号
	int gx, gy;				// マップの座標
	int level;				// レベル
	int dir;				// 向き
	int stockDir;			// 移動後に向き変更があれば変更する
	int stockDirX, stockDirY;// stockDirを適用する座標
	short nameColor;		// 名前表示時の色
	unsigned short status;	// ステータス
	int classNo;			// アイテム用（？）
	int money;				// お金用
	char name[CHAR_NAME_LEN+1];			// 名前
	char freeName[CHAR_FREENAME_LEN+1];	// ユーザが付けた称号
	char info[64];			// １行インフォメーション（ACTIONに入れたらいらないかも）
	int battleNo;			// 戦闘番号
	short sideNo;			// 戦闘時にどちらサイドに属しているか
	short helpMode;			// ヘルプモード
	int charType;			// アクションに設定する属性
	short newFoundFlag;		// 新発見の怪獣かをチェックするかのフラグ
	ACTION *ptAct;
} CHAROBJ;


typedef struct
{
	int charObjTblId;			// 管理テーブルの何番目に入っているか
	ACTION *ptActLeaderMark;	// リーダーマーク表示用
	ACTION *ptActMagicEffect;	// 呪術エフェクト
	unsigned int drawFukidashiTime;	// 吹き出しを出す時間
} CHAREXTRA;


typedef struct
{
	short mode;					// ペットの動作モード
	short moveDir;				// 移動時に向かう方向
	short preDir;				// ひとつ前に曲がった向き
	short dirCnt;				// 連続で曲がった回数
	int preGx, preGy;			// ひとつ前のグリッド
	int walkCnt;				// 移動歩数
	unsigned int createTime;	// 作成時間
	ACTION *ptAct;
	ACTION *ptAct2;
	float angle;
	int r;
	int id;						// サーバの管理番号
} PETEXTRA;


void charMove( ACTION * );
void charMove2( ACTION * );
void _charMove( ACTION * );
void shiftBufCount( ACTION * );

ACTION *createCharAction( int, int, int, int );
void stockCharMovePoint( ACTION *, int, int );
void correctCharMovePoint( ACTION *, int, int );
void setCharMovePoint( ACTION *, int, int );
void _setCharMovePoint( ACTION *, int, int );
void setCharWarpPoint( ACTION *, int, int );
void setCharStatus( unsigned short *, char * );

ACTION *createCommmonEffectAction( int, int, int, int, int, int );
ACTION *createCommmonEffectNoLoop( int, int, int, int, int, int );
void drawCharStatus( ACTION * );

void changeCharAct( ACTION *, int, int, int, int, int, int, int );

void setCharLeader( ACTION * );
void delCharLeader( ACTION * );
void setCharParty( ACTION * );
void delCharParty( ACTION * );
void setCharWatch( ACTION * );
void delCharWatch( ACTION * );
void setCharBattle( ACTION *, int, short, short );
void delCharBattle( ACTION * );
void setCharUseMagic( ACTION * );
void delCharUseMagic( ACTION * );

void setCharFukidashi( ACTION *, unsigned int );


void charObjProc( void );
BOOL checkCharObjPoint( int, int, short );
int checkCharObjPointStatus( int, int, short, unsigned short );
int checkCharObjPointNotStatus( int, int, short, unsigned short );
void setNpcCharObj( int, int, int, int, int, char *, char *, int, int, int, int, int );
BOOL setReturnPetObj( int, int, int, int, int, char *, char *, int, int, int, int, int );
void setItemCharObj( int, int, int, int, int, int, char * );
void setMoneyCharObj( int, int, int, int, int, int, char * );
int getAtrCharType( int );
int getAtrCharObjType( int );
void delCharObj( int );
void initCharObj( void );
void resetCharObj( void );
void clearPtActCharObj( void );
void restorePtActCharObjAll( void );
void restorePtActCharObj( int );
void setMovePointCharObj( int, int, int );
ACTION *getCharObjAct( int );

int getCharObjBuf( void );

void initItemOverlapCheck( void );
BOOL itemOverlapCheck( unsigned int, int, int );

ACTION *createPetAction( int, int, int, int, int, int, int );
void ajustClientDir( short * );

#endif

