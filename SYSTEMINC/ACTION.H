/************************/
/*	action.h			*/
/************************/
#ifndef _ACTION_H_
#define _ACTION_H_

/* 処理優先順位  prio の値 ****************************************************/
enum{
	PRIO_TOP,			/* 最優先 	*/

	PRIO_CHR,			/* キャラクタ等 */

	PRIO_BG,			/* ＢＧ 	*/
	PRIO_JIKI,			/* 自機 	*/
	PRIO_ENEMY,			/* 敵 		*/
	PRIO_ENEMY_TAMA,	/* 敵弾 	*/
	PRIO_JIKI_TAMA,		/* 自機弾 	*/
	PRIO_ITEM,			/* アイテム	*/
	PRIO_BOSS,			/* ボス		*/
	PRIO_GAME_OVER,		/* GAME OVER */
	PRIO_BTM = 255		/* 最後尾 	*/
};

/* アクションの状態 ***********************************************************/
enum{
	ACT_STATE_ACT = 0,	/* 通常状態 */
	ACT_STATE_DEAD		/* 死亡状態 */
};
/* 属性 ***********************************************************************/
#define ACT_ATR_HIDE 			( 1 <<  1 )	/* 非表示 */
#define ACT_ATR_HIT 			( 1 <<  2 )	/* 当たり判定する */
#define ACT_ATR_HIT_BOX 		( 1 <<  3 )	/* 当たり判定 + ボックス表示 */
#define ACT_ATR_INFO 			( 1 <<  4 )	/* 一行インフォ表示 */
#define ACT_ATR_HIDE2 			( 1 <<  5 )	/* 非表示+あたり判定あり */
#define ACT_ATR_BTL_CMD_END 	( 1 <<  6 )	/* 戦闘コマンド入力終了フラグ */
#define ACT_ATR_TYPE_PC 		( 1 <<  7 )	/* ＰＣ */
#define ACT_ATR_TYPE_OTHER_PC 	( 1 <<  8 )	/* 他のＰＣ */
#define ACT_ATR_TYPE_PET 		( 1 <<  9 )	/* ペット */
#define ACT_ATR_TYPE_ITEM 		( 1 << 10 )	/* アイテム */
#define ACT_ATR_TYPE_GOLD 		( 1 << 11 )	/* お金 */
#define ACT_ATR_TYPE_OTHER 		( 1 << 12 )	/* その他（ドア、町の人、看板） */
#define ACT_ATR_HIT_BOX_ALL1 	( 1 << 13 )	/* 当たり判定 + ボックス表示 複数選択１ */
#define ACT_ATR_HIT_BOX_ALL2 	( 1 << 14 )	/* 当たり判定 + ボックス表示 複数選択２ */
#define ACT_ATR_HIT_BOX_ALL3 	( 1 << 15 )	/* 当たり判定 + ボックス表示 複数選択３ */
#define ACT_ATR_TRAVEL 			( 1 << 16 )	/* 旅行中 */

typedef unsigned char UCHAR;
typedef int                 BOOL;
typedef unsigned int        UINT;
#define FALSE 0
#define TRUE 1
/* アクション構造体 *********动作结构*************************************************/
struct action{
	struct 	action *pPrev, *pNext;		/* 前後のアクションポインタ */
	void 	( *func )( struct action * );	/* 実行関数へのポインタ */
	void 	*pYobi;						/* 予備構造体ポインタ */
	void 	*pOther;					/* 汎用記憶ポインタ */
	UCHAR 	prio;						/* 処理の優先順位 */
	UCHAR 	dispPrio;					/* 表示の優先順位 */
	int 	x, y;						/* 表示座標 */
	int		hitDispNo;					/* 当たり判定用番号 */
	BOOL	deathFlag;					/* アクション死亡フラグ */
	
	int 	dx, dy;						/* 増分 */
	int 	dir;						/* 方向 */
	int 	delta;  					/* 合成ベクトル */
	
	char 	name[ 29 ];					/* 名前 */
	char 	freeName[ 13 ];				/* フリー名前 */
	int 	hp;							/* ＨＰ */
	int 	maxHp;						/* 最大ＨＰ */
	int 	mp;							/* ＭＰ */
	int 	maxMp;						/* 最大ＭＰ */
	int 	level;						/* ＬＥＶＥＬ */
	int 	status;						/* ステータス */
	int 	itemNameColor;				/* アイテムの名前の色 */
	
	int		bmpNo;						/* スプライト番号 */
	int		atr;						/* 属性 */
	int		state;						/* 状態 */
	int		actNo;						/* 行動番号（自由に使用可） */
	int		damage;						/* ダメージ */

	int		gx, gy;						// マップグリッド座標（現在地）
	int		nextGx, nextGy;				// マップグリッド座標（目的地）
	int		bufGx[10], bufGy[10];		// 移動先座標のバッファ
	short	bufCount;					// 移動座標が何個設定されているか
	short	walkFlag;					// 歩きアニメから立ちアニメへ切り替えタイミングフラグ
	float	mx, my;						// マップ座標
	float	vx, vy;						// 移動増分

// 属性
	short 	earth;						// 地 属性
	short 	water;						// 水 属性
	short 	fire;						// 火 属性
	short 	wind;						// 風 属性

// ｒａｄｅｒ用
	int		dirCnt;						// 方向変換カウンター

//ｇｅｍｉｎｉ用  速度 方向
	int		spd;						// 移動スピード( ０～６３ )( ４で１ドット )
	int		crs;						// 方向( ０～３１ )( 上が０で右回り )
	int		h_mini;						// 小数点横
	int		v_mini;						// 小数点縦

//ｐａｔｔｅｒｎ用 图案
	int		anim_chr_no;				// アニメーション番号（ anim_tbl.h の番号 )
	int		anim_chr_no_bak;			// 前回アニメーション番号
	int		anim_no;					// 動作番号
	int		anim_no_bak;				// 前回の動作番号
	int		anim_ang;					// アニメーション向き( ０～７ )( 下が０で右回り )
	int		anim_ang_bak;				// 前回のアニメーション向き
	int		anim_cnt;					// アニメーションカウンタ
	int		anim_frame_cnt;				// アニメーションフレームカウンタ
	int		anim_x;						// オフセットＸ座標 ( Sprbin + Adrnbin )
	int		anim_y;						// オフセットＹ座標 ( Sprbin + Adrnbin )
	int		anim_hit;					// 当たり判定
};

typedef struct action ACTION;


/* 最初と最後のアクションリストポインタ */
extern ACTION *pActTop;
extern ACTION *pActBtm;

/* 自機のポインタ */
extern ACTION *pJiki;

#ifdef _DEBUG		
/* アクション数カウント */
extern int ActCnt;
#endif

/* アクションリストに登録 *****************************************************/
ACTION *GetAction( UCHAR prio, UINT yobiSize );

/* アクションリスト初期化 *****************************************************/
void InitAction( void );

/* アクション走らせる *********************************************************/
void RunAction( void );

/* アクションリストから抹殺準備************************************************/
void DeathAction( ACTION *pAct );

/* 全アクション抹殺準備 *******************************************************/
void DeathAllAction( void );

/* アクションリストから完全抹殺 ***********************************************/
void ClearAction( ACTION *pAct );

/* アクション終了処理 *********************************************************/
void EndAction( void );

#endif

