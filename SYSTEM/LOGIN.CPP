//进入游戏设置先看
// 开始游戏/ /登录
//   登录，服务器选择过程什么呢，直至（或标题）
//
#include<string.h>

#include"system.h"
#include"process.h"
#include"netproc.h"
#include"netmain.h"
#include"pc.h"
#include"tool.h"
#include"character.h"
#include"loadsprbin.h"
#include"savedata.h"
#include"chat.h"
#include"ime_sa.h"
#include"menu.h"
#include"t_music.h"
#include"netmain.h"
#include"map.h"
#include"lssproto_cli.h"
#include"field.h"

#include"anim_tbl.h"

#include"login.h"

#include"produce.h"

#define SA_VERSION	"B"  //重要修改#define SA_VERSION	"VER 1.27"  字符串0结尾
//#define _SA_VERSION 'B'  服务端是这样的一个字节 无0结尾

char selectServerName[][64] =
{
	"测试一线",
	"测试二线",
	"测试三线",
	"测试四线"
};

char selectServerName2[][64] =
{
	"太陽 [SUN]",
	"月[MOON]",
	"太陽 [SUN]",

	"月[MOON]"

};

BOOL logOutFlag = FALSE;//退出标志


void initInputIdPassword( void );
int inputIdPassword( BOOL );

void initCommonMsgWin( void ); //初始化正常消息 
int commonMsgWin( char * );


void initCertifyIdPassword( void );//验证密? 
int certifyIdPassword( char *, char * );


void initSelectServer( void );
int selectServer( int, int );


void initConnecGameServer( void );
int connecGameServer( void );

void initDownloadCharList( void );
int downloadCharList( void );

void initDeleteCharacter( void );
int deleteCharacter( void );

int commonYesNoWindow( int, int );


void initStrBuffer( STR_BUFFER *, int, int, int, int, int );

void getStrSplit( char *, char *, int, int, int );//获取返回消息


void initUserCertifyErrorMsgWin( void );//初始化验证错误信息
int userCertifyErrorMsgWin( void );



///////////////////////////////////////////////////////////////////////////
//
// 俬俢丄僷僗儚乕僪擖椡張棟
/// /身份证，密码处理..

static STR_BUFFER idKey;					// 用户ID输入缓冲区.b僼傽
static STR_BUFFER passwd;					// 缓冲区的密码.?
static STR_BUFFER *idPasswordFocus[] =
	{ &idKey, &passwd };					// 僼僅乕僇僗愗傝懼偊傛偆儚乕僋僄儕傾
STR_BUFFER *idPasswordStr = &idKey;
short idKeyReturn = 0;
static int idPasswordGraId[] = { -2, -2 };	// 鼠标每幅图像，并确定
static int idPasswordFocusSw = 0;			// 切换输入焦点.,
static short idKeyBoxX, idKeyBoxY;			// ID显示坐标输入缓冲区.罆W
static short passwdBoxX, passwdBoxY;		// 缓冲区显示坐标密码.[僪偺


#define NO_FIRST_ID_PASSWD					// 




void idPasswordProc( void )
{
	BOOL flag = FALSE;
	int ret;
	static char msg[256];

	// 初期化
	if( SubProcNo == 0 )
	{
		SubProcNo++;
		initInputIdPassword();
		idKeyReturn = 0;
	}

	// 入力処理
	if( SubProcNo == 1 )
	{
		flag = TRUE;	// ＩＤ?パスワード入力可能フラグ  密码输入转折点？
	}

	ret = inputIdPassword( flag );

	if( ret == 1 )
	{
#ifdef NO_FIRST_ID_PASSWD
		// タイトル画面へ
		ChangeProc( PROC_TITLE_MENU );
		// 入力されたID,PASSWDをとりあえずファイルに保存
		setCdkey( idKey.buffer );
		setPassword( passwd.buffer );
		if( saveNowState() )
		{
		}
#else
		SubProcNo = 2;
#endif
		play_se( 217, 320, 240 );	// クリック音
	}
	else
	// 終了ボタンが押されたので終了処理
	if( ret == 2 )
	{
		play_se( 217, 320, 240 );	// クリック音
		// ＷＩＮＤＯＷＳに WM_CLOSE メッセージを送らせる
		PostMessage( hWnd, WM_CLOSE, 0, 0L );
	}
	else
	// ID,PASSWORDが空の時のエラーメッセージ
	if( ret < 0 )
	{
		SubProcNo = 100;
		strcpy( msg, "请输入账户密码" );
	}


	// ＩＤ?パスワード認証処理  ID密码认证处理？
	if( SubProcNo == 2 )
	{
		// 初期化
		initCertifyIdPassword();
		SubProcNo++;
	}
	if( SubProcNo == 3 )
	{
		ret = certifyIdPassword( idKey.buffer, passwd.buffer );

		// 認証完了
		if( ret == 1 )
		{
			// タイトル画面へ  标题画面上
			ChangeProc( PROC_TITLE_MENU );
			// 入力されたID,PASSWDをとりあえずファイルに保存
			//输入的ID，PASSWD首先保存到文件
			setCdkey( idKey.buffer );
			setPassword( passwd.buffer );
			if( saveNowState() )
			{
			}
			return;
		}
		else
		// ネットワークの準備が出来ていない 网络的准备。
		if( ret == -1 )
		{
			SubProcNo = 100;
			strcpy( msg, NET_ERRMSG_SOCKLIBERROR );
		}
		else
		// サーバと接続できない
		if( ret == -2 )
		{
			SubProcNo = 100;
			strcpy( msg, NET_ERRMSG_NOTCONNECT );
		}
		else
		// アカウントが無かった
		if( ret == -3 )
		{
			SubProcNo = 100;
			strcpy( msg, "IDとPASSWORDをもう一度確認してください。" );
		}
	}

	// エラーメッセージ
	if( SubProcNo == 100 )
	{
		initCommonMsgWin();
		SubProcNo++;
	}
	if( SubProcNo == 101 )
	{
		if( commonMsgWin( msg ) )
		{
			// ＯＫボタンが押された
			SubProcNo = 1;
		}
	}
}



// ＩＤ?パスワード入力バッファ初期化
void initInputIdPassword( void )
{
	int i;

	for( i = 0; i < sizeof( idPasswordGraId )/sizeof( int ); i++ )
	{
		idPasswordGraId[i] = -2;
	}

	// ID入力バッファ初期化
	idKeyBoxX = 320;
	idKeyBoxY = 183;
	initStrBuffer( &idKey, idKeyBoxX, idKeyBoxY, 8, FONT_PAL_WHITE, FONT_PRIO_BACK );
	// 現在のＩＤを読み込みバッファに設定
	getCdkey( idKey.buffer );
	idKey.cnt = strlen( idKey.buffer );

	// パスワード入力バッファ初期化
	passwdBoxX = 320;
	passwdBoxY = 214;
	initStrBuffer( &passwd, passwdBoxX, passwdBoxY, 12, FONT_PAL_WHITE, FONT_PRIO_BACK );
	// 現在のパスワードを読み込みバッファに設定
	getPassword( passwd.buffer );
	passwd.cnt = strlen( passwd.buffer );
	passwd.blindFlag = TRUE;

	//idPasswordFocus[0] = &idKey;
	//idPasswordFocus[1] = &passwd;

	// フォーカス設定
	idPasswordFocusSw = 0;
	GetKeyInputFocus( idPasswordFocus[idPasswordFocusSw] );
}


// 入力処理?画面表示
//
//  戻り値：	 0 ... 入力中
//				 1 ... 入力完了
//				 2 ... 終了ボタン
//				-1 ... 入力欄が空
int inputIdPassword( BOOL flag )
{
	int id;
	int selOkFlag;
	static BOOL flag2 = FALSE;
	static int oldId = 0;
	int ret = 0;
	int x1, y1, x2, y2, cx, cy;

	// グラフィックの当たり判定
	id = selGraId( idPasswordGraId, sizeof( idPasswordGraId )/sizeof( int ) );

	// ＯＫボタンが押された
	if( id == 0 )
	{
		// CDキー?パスワードが設定されているか
		if( strlen( idKey.buffer ) > 0 && strlen( passwd.buffer ) > 0 )
		{
			idKey.buffer[idKey.cnt] = '\0';
			passwd.buffer[passwd.cnt] = '\0';
			ret = 1;
		}
		else
		{
			// 設定されていない
			ret = -1;
		}
	}
	else
	// 終了ボタンが押された
	if( id == 1 )
	{
		ret = 2;
	}


	// ボタン等の選択可?不可の設定
	if( flag )
	{
		selOkFlag = 2;
	}
	else
	{
		selOkFlag = 0;
	}

	StockFontBuffer2( &idKey );
	StockFontBuffer2( &passwd );

	id = -1;

	if( selOkFlag )
	{
		x1 = idKeyBoxX - 4 -2;
		y1 = idKeyBoxY - 2 -1;
		x2 = x1 + 116 +4;
		y2 = y1 + 22 +2;
		if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX ) )
		{
			id = 0;
		}
		x1 = passwdBoxX - 4 -2;
		y1 = passwdBoxY - 2 -1;
		x2 = x1 + 116 +4;
		y2 = y1 + 22 +2;
		if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX ) )
		{
			id = 1;
		}
	}


	// 入力フォーカス変更
	if( idKeyReturn )
	{
		id = 1;
		idKeyReturn = 0;
	}

	if( joy_trg[ 1 ] & JOY_TAB )
	{
		if( oldId == 0 )
		{
			id = 1;
		}
		else
		if( oldId == 1 )
		{
			id = 0;
		}
	}

	if( !flag )
	{
		// フラグがFALSEなら入力できなくする
		GetKeyInputFocus( NULL );
		flag2 = TRUE;
	}
	else
	if( (0 <= id && id <= 1)
	 || flag2 )
	{
		if( flag2 )
			id = oldId;
		GetKeyInputFocus( idPasswordFocus[id] );
		flag2 = FALSE;
		oldId = id;
	}


	// ＯＫ
	cx = 317;
	cy = 287;
	x1 = cx - 50;
	y1 = cy - 30;
	x2 = cx + 50;
	y2 = cy + 30;
	if( MakeHitBox( x1, y1, x2,  y2, -1 ) && selOkFlag )
	{
		idPasswordGraId[0] =
			StockDispBuffer( cx, cy, DISP_PRIO_BG, CG_TITLE_ID_PASS_OK, 1 );
	}
	else
	{
		idPasswordGraId[0] = -2;
	}
	// 終了
	cx = 359;
	cy = 371;
	x1 = cx - 50;
	y1 = cy - 30;
	x2 = cx + 50;
	y2 = cy + 30;
	if( MakeHitBox( x1, y1, x2,  y2, -1 ) && selOkFlag )
	{
		idPasswordGraId[1] =
			StockDispBuffer( cx, cy, DISP_PRIO_BG, CG_TITLE_ID_PASS_QUIT, 1 );
	}
	else
	{
		idPasswordGraId[1] = -2;
	}


	// ＢＧ表示
	StockDispBuffer( 320, 240, DISP_PRIO_BG, CG_TITLE_ID_PASS, 0 );

	return ret;
}




// ＩＤ?パスワード認証処理初期化

static short certifyIdPasswordProcNo = 0;		// 認証処理進行状況
static short certifyIdPasswordServerIndex = 0;	// 認証処理使用サーバインデックス

void initCertifyIdPassword( void )
{
	certifyIdPasswordProcNo = 0;
}


// ＩＤ?パスワード認証処理
//  引数：		id   ... ＩＤ
//				pass ... パスワード
//
//  戻り値：	 0 ... 処理中
//				 1 ... 正しいと認証された
//				-1 ... ネットワークの準備が出来ていない
//				-2 ... サーバに接続できなかった
//				-3 ... アカウントが無かった
int certifyIdPassword( char *id, char *pass )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	int ret = 0;
	int ret2;
	char msg[] = "認証中";

	// 初期化
	if( certifyIdPasswordProcNo == 0 )
	{
		certifyIdPasswordProcNo = 1;

		// ウィンドウ作成
		w = (strlen( msg )*8+63)/64;
		if( w < 2 )
			w = 2;
		h = (16+47)/48;
		if( h < 2 )
			h = 2;
		x = (640-w*64)/2;
		y = (480-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );

		certifyIdPasswordServerIndex = 0;
	}

	// 認証処理
	// ネットワーク初期化
	if( certifyIdPasswordProcNo == 1 )
	{
		cleanupNetwork();
		if( initNet() )
		{
			certifyIdPasswordProcNo = 2;
		}
		else
		{
			// ネットワークの準備が出来ていない
			ret = -1;
		}
	}
	// サーバ接続
	if( certifyIdPasswordProcNo == 2 )
	{
		// 初期化
		selectServerIndex = certifyIdPasswordServerIndex;
		strcpy( userId, id );
		strcpy( userPassword, pass );
		initConnectServer();
		certifyIdPasswordProcNo = 3;
	}
	if( certifyIdPasswordProcNo == 3 )
	{
		ret2 = connectServer();
		// 接続完了
		if( ret2 == 1 )
		{
			certifyIdPasswordProcNo = 4;
		}
		else
		// エラー
		if( ret2 < 0 )
		{
			if( certifyIdPasswordServerIndex < getServerMax()-1 )
			{
				// 次のサーバで試してみる
				certifyIdPasswordServerIndex++;
				certifyIdPasswordProcNo = 1;
			}
			else
			{
				// 両サーバともつながらなかった
				ret = -2;
			}
		}
	}
	// キャラリストを取る
	if( certifyIdPasswordProcNo == 4 )
	{
		charListStart();
		certifyIdPasswordProcNo = 5;
	}
	if( certifyIdPasswordProcNo == 5 )
	{
		ret2 = charListProc();
		if( ret2 == 1 )
		{
			// ここまで来たら認証完了
			ret = 1;
		}
		else
		// タイムアウト
		if( ret2 == -1 )
		{
			ret = -2;
		}
		else
		// キャラリストが見つからなかった
		// IDかPASSWORDが間違っている
		if( ret2 == -2 )
		{
			ret = -3;
		}
	}

	// 結果がわかったらウィンドウ閉じる
	if( ret != 0 )
	{
		if( ptActMenuWin )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
		}
	}

	if( ptActMenuWin != NULL )
	{
		// ウィンドウ表示
		if( ptActMenuWin->hp >= 1 )
		{
			int len;
			int xx, yy;

			len = strlen( msg );
			xx = (w*64-len*8)/2;
			yy = (h*48-16)/2;
			StockFontBuffer( x+xx, y+yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0 );
		}
	}

	return ret;
}








// 標準メッセージウィンドウ（ＯＫボタンあり）
// 
static short commonMsgWinProcNo = 0;

// 初期化
void initCommonMsgWin( void )
{
	commonMsgWinProcNo = 0;
}

// 標準メッセージウィンドウ処理
//
//  引数：		msg ... 表示するメッセージ
//
//  戻り値：	0 ... 処理中
//				1 ... OKボタンが押された
int commonMsgWin( char *msg )
{
	static int fontId[] = { -2 };
	static ACTION *ptActMenuWin = NULL;
	int id;
	int i;
	static int x, y, w, h;
	int ret = 0;


	// 初期化
	if( commonMsgWinProcNo == 0 )
	{
		commonMsgWinProcNo = 1;

		for( i = 0; i < sizeof( fontId )/sizeof( int ); i++ )
		{
			fontId[i] = -2;
		}

		// ウィンドウ作成
		w = strlen( msg )*8/64+2;
		//if( w < 2 )
		//	w = 2;
		h = (36+32)/48;
		if( h < 2 )
			h = 2;
		x = (640-w*64)/2;
		y = (480-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );
	}


	// フォントの選択判定
	id = selFontId( fontId, sizeof( fontId )/sizeof( int ) );

	// ＯＫ
	if( id == 0 )
	{
		ret = 1;
		play_se( 217, 320, 240 );	// クリック音
	}


	// 結果がわかったらウィンドウ閉じる
	if( ret != 0 )
	{
		if( ptActMenuWin )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			return ret;
		}
	}

	if( ptActMenuWin != NULL )
	{
		// ウィンドウ表示
		if( ptActMenuWin->hp >= 1 )
		{
			int len;
			int xx;

			len = strlen( msg )+1;
			xx = (w*64-len*8)/2;
			StockFontBuffer( x+xx, y+30, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0 );

			xx = (w*64-strlen( "ＯＫ" )*8)/2;
			fontId[0] =
				StockFontBuffer( x+xx, y+56,
					FONT_PRIO_FRONT, FONT_PAL_YELLOW, "ＯＫ", 2 );
		}
	}

	return ret;
}








// 文字入力バッファを初期化する
//  引数:	STR_BUFFER *pt;	... 文字列バッファ構造体のポインタ
//			int x, y;		... 表示座標
//			int len;		... 入力バッファのサイズ
//			int color;		... 表示色
//			int prio;		... 表示優先度
void initStrBuffer( STR_BUFFER *pt, int x, int y, int len, int color, int prio )
{
	pt->x        = x;
	pt->y        = y;
	pt->len      = len;
	pt->color    = color;
	pt->fontPrio = prio;
}


// グラフィックの選択判定
//   マウスで選択され左クリックされた番号を返す
//  引数:	int *id;	... チェックするグラフィックＩＤ配列
//			int cnt;	... ＩＤの数
//  戻り値:	-1			... どれにもあたってない
int selGraId( int *id, int cnt )
{
	int i;

	// 左クリックされてなければ終了
	if( (mouse.onceState & MOUSE_LEFT_CRICK) == 0 )
		return -1;

	for( i = 0; i < cnt; i++ )
	{
		if( id[i] == HitDispNo )
		{
			return i;
		}
	}

	return -1;
}


// グラフィックの選択判定（リピート付き）
//   マウスで選択され左クリックされた番号を返す
//  引数:	int *id;	... チェックするグラフィックＩＤ配列
//			int cnt;	... ＩＤの数
//  戻り値:	-1			... どれにもあたってない
int selRepGraId( int *id, int cnt )
{
	int i;

	// 左クリックされてなければ終了
	if( (mouse.autoState & MOUSE_LEFT_CRICK) == 0 )
		return -1;

	for( i = 0; i < cnt; i++ )
	{
		if( id[i] == HitDispNo )
		{
			return i;
		}
	}

	return -1;
}


// グラフィックの選択判定
//   マウスで選択され左ボタンが押されている番号を返す
//  引数:	int *id;	... チェックするグラフィックＩＤ配列
//			int cnt;	... ＩＤの数
//  戻り値:	-1			... どれにもあたってない
int pushGraId( int *id, int cnt )
{
	int i;

	// 左が押されてなければ終了
	if( (mouse.state & MOUSE_LEFT_CRICK) == 0 )
		return -1;

	for( i = 0; i < cnt; i++ )
	{
		if( id[i] == HitDispNo )
		{
			return i;
		}
	}

	return -1;
}


// フォントの選択判定
//   マウスで選択され左クリックされた番号を返す
//  引数:	int *id;	... チェックするフォントＩＤ配列
//			int cnt;	... ＩＤの数
//  戻り値:	-1			... どれにもあたってない
int selFontId( int *id, int cnt )
{
	int i;

	// 左クリックされてなければ終了
	if( (mouse.onceState & MOUSE_LEFT_CRICK) == 0 )
		return -1;

	for( i = 0; i < cnt; i++ )
	{
		if( id[i] == HitFontNo )
		{
			return i;
		}
	}

	return -1;
}


// グラフィックのフォーカス判定
//   マウスカーソルのあたっている番号を返す
//  引数:	int *id;	... チェックするグラフィックＩＤ配列
//			int cnt;	... ＩＤの数
//  戻り値:	-1			... どれにもあたってない
int focusGraId( int *id, int cnt )
{
	int i;

	for( i = 0; i < cnt; i++ )
	{
		if( id[i] == HitDispNo )
		{
			return i;
		}
	}

	return -1;
}


// フォントのフォーカス判定
//   マウスカーソルのあたっている番号を返す
//  引数:	int *id;	... チェックするフォントＩＤ配列
//			int cnt;	... ＩＤの数
//  戻り値:	-1			... どれにもあたってない
int focusFontId( int *id, int cnt )
{
	int i;

	for( i = 0; i < cnt; i++ )
	{
		if( id[i] == HitFontNo )
		{
			return i;
		}
	}

	return -1;
}




// ユーザ認証失敗メッセージウィンドウ
// 
static short userCertifyErrorMsgWinProcNo = 0;

// 初期化
void initUserCertifyErrorMsgWin( void )
{
	userCertifyErrorMsgWinProcNo = 0;
}

// ユーザ認証失敗メッセージウィンドウ処理
//
//  戻り値：	0 ... 処理中
//				1 ... OKボタンが押された
int userCertifyErrorMsgWin( void )
{
	static int fontId[] = { -2 };
	static ACTION *ptActMenuWin = NULL;
	int id;
	int i;
	static int x, y, w, h;
	int ret = 0;
	char *msg[] =
	{
		"ユーザー認証に失敗しました。",
		"ご契約内容などを確認させていただきますので、",
		"ゲーマーズドリームカスタマケア(rian@gamersdream.ne.jp)",
		"までお問い合わせください。"
	};

	// 初期化
	if( userCertifyErrorMsgWinProcNo == 0 )
	{
		userCertifyErrorMsgWinProcNo = 1;

		for( i = 0; i < sizeof( fontId )/sizeof( int ); i++ )
		{
			fontId[i] = -2;
		}

		// ウィンドウ作成
		w = 8;
		h = 3;
		x = (640-w*64)/2;
		y = (480-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );
	}


	// フォントの選択判定
	id = selFontId( fontId, sizeof( fontId )/sizeof( int ) );

	// ＯＫ
	if( id == 0 )
	{
		ret = 1;
		play_se( 217, 320, 240 );	// クリック音
	}


	// 結果がわかったらウィンドウ閉じる
	if( ret != 0 )
	{
		if( ptActMenuWin )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			return ret;
		}
	}

	if( ptActMenuWin != NULL )
	{
		// ウィンドウ表示
		if( ptActMenuWin->hp >= 1 )
		{
			StockFontBuffer( x+140, y+20, FONT_PRIO_FRONT, FONT_PAL_RED, msg[0], 0 );
			StockFontBuffer( x+20, y+45, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[1], 0 );
			StockFontBuffer( x+20, y+65, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[2], 0 );
			StockFontBuffer( x+20, y+85, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[3], 0 );

			fontId[0] =
				StockFontBuffer( x+239, y+110,
					FONT_PRIO_FRONT, FONT_PAL_YELLOW, "ＯＫ", 2 );
		}
	}

	return ret;
}








///////////////////////////////////////////////////////////////////////////
//
// ??簪???
//
void titleProc( void )
{
	int ret;
	static char msg[256];

	// 初期化
	if( SubProcNo == 0 )
	{
		char str[128];
		SubProcNo++;
		sprintf( str, "%s %s", DEF_APPNAME, SA_VERSION );
		SetWindowText( hWnd, str );
		disconnectServerFlag = FALSE;
		ProduceInitFlag = TRUE;
		initSelectServer();
	}

	// サーバ選択処理
	if( SubProcNo == 1 )
	{
		ret = selectServer( 256, 270 );
		// サーバ決定
		if( ret == 1 )
		{
			certifyIdPasswordServerIndex = selectServerIndex;
			getCdkey( userId );
			getPassword( userPassword );
			SubProcNo++;
		}
		else
		if( ret == 2 )
		{
			// ＷＩＮＤＯＷＳに WM_CLOSE メッセージを送らせる
			PostMessage( hWnd, WM_CLOSE, 0, 0L );
		}
	}

	// ゲームサーバへ接続
	if( SubProcNo == 2 )
	{
		// 初期化
		initConnecGameServer();
		SubProcNo++;
	}
	if( SubProcNo == 3 )
	{
		// 接続処理
		ret = connecGameServer();
		// 接続完了
		if( ret == 1 )
		{
			ChangeProc( PROC_CHAR_SELECT );
//			return;
		}
		else
		// ネットワークの準備が出来てない
		if( ret == -1 )
		{
			SubProcNo = 100;
			strcpy( msg, NET_ERRMSG_SOCKLIBERROR );
			// ネットワーク初期化
			cleanupNetwork();
		}
		else
		// サーバに接続できなかった
		if( ret == -2 )
		{
			SubProcNo = 100;
			strcpy( msg, NET_ERRMSG_NOTCONNECT );
			// ネットワーク初期化
			cleanupNetwork();
		}
	}


	// エラーメッセージ
	if( SubProcNo == 100 )
	{
		initCommonMsgWin();
		SubProcNo++;
	}
	if( SubProcNo == 101 )
	{
		if( commonMsgWin( msg ) )
		{
			char str[128];
			// ＯＫボタンが押された
			SubProcNo = 1;
			sprintf( str, "%s %s", DEF_APPNAME, SA_VERSION );
			SetWindowText( hWnd, str );
		}
	}


	// タイトルＢＧ表示
	StockDispBuffer( 320, 240, DISP_PRIO_BG, CG_TITLE, 0 );
	// バージョン表示
	StockFontBuffer( 540, 424, FONT_PRIO_BACK, FONT_PAL_BLUE, SA_VERSION, 0 );
}


// 显示服务器选择线路？

ACTION *ptActSelectServerWin = NULL;

// ????
void initSelectServer( void )
{
	ptActSelectServerWin = NULL;
}


//
//  选择     	0 ... 选择
//				1 ... 确定
//				2 ... 退出
int selectServer( int x, int y )
{
	static int fontId[] = { -2, -2, -2 };
	int id;
	int i;
	int ret = 0;

	if( ptActSelectServerWin == NULL )
	{
		ptActSelectServerWin = MakeWindowDisp( x, y, 2, 3, NULL, 1 );

		for( i = 0; i < sizeof( fontId )/sizeof( int ); i++ )
		{
			fontId[i] = -2;
		}
	}

	if( ptActSelectServerWin != NULL )
	{
		// フォントの選択判定
		id = selFontId( fontId, sizeof( fontId )/sizeof( int ) );

		// メニュー０が押された
		// メニュー１が押された
//#ifdef _DEBUG
#if 1
		if( id == 0 || id == 1 )
#else
		if( id == 0 )
#endif
		{
			char title[256];

#ifdef _DEBUG
			sprintf( title, "%s %s [%s  %s:%s]", DEF_APPNAME, SA_VERSION,
				selectServerName[id],
				gmsv[id].ipaddr, gmsv[id].port );
#else
			sprintf( title, "%s %s [%s]", DEF_APPNAME, SA_VERSION, selectServerName[id] );
#endif
			SetWindowText( hWnd, title );

			selectServerIndex = id;

			DeathAction( ptActSelectServerWin );
			ptActSelectServerWin = NULL;
			play_se( 217, 320, 240 );	// クリック音
			return 1;
		}
//#ifndef _DEBUG
#if 0
		else
		if( id == 1 )
		{
			play_se( 220, 320, 240 );	// ＮＧ音
		}
#endif
		else
		// 終了ボタンが押された
		if( id == 2 )
		{
			DeathAction( ptActSelectServerWin );
			ptActSelectServerWin = NULL;
			play_se( 217, 320, 240 );	// クリック音
			return 2;
		}

		if( ptActSelectServerWin->hp >= 1 )
		{
			fontId[0] =
				StockFontBuffer( x+22, y+28,
					FONT_PRIO_FRONT, FONT_PAL_YELLOW, selectServerName[0], 2 );
//#ifdef _DEBUG
#if 1
			//这里增加线路
			fontId[1] =
				StockFontBuffer( x+22, y+47,   //66
					FONT_PRIO_FRONT, FONT_PAL_YELLOW, selectServerName[1], 2 );

			fontId[2] =
				StockFontBuffer( x+22, y+66,
				FONT_PRIO_FRONT, FONT_PAL_YELLOW, selectServerName[2], 2 );

			fontId[3] =
				StockFontBuffer( x+22, y+85,
				FONT_PRIO_FRONT, FONT_PAL_YELLOW, selectServerName[3], 2 );





#else
			fontId[1] =
				StockFontBuffer( x+22, y+66,
					FONT_PRIO_FRONT, FONT_PAL_GRAY, selectServerName[1], 2 );
#endif

			fontId[2] =
				StockFontBuffer( x+22, y+104,
					FONT_PRIO_FRONT, FONT_PAL_YELLOW, "结束游戏　 ", 2 );
		}
	}

	return 0;
}





//  /初始化服务器连接过程/

static short connecGameServerProcNo = 0;

void initConnecGameServer( void )
{
	connecGameServerProcNo = 0;
}



// /处理服务器连接
//
//  返回：	     0 ... 检测
//				 1 ... 完成连接/ 
//				-1 ... 网络错误
//				-2 ... 无法连接到服务器
int connecGameServer( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	int ret = 0;
	int ret2;
	static char msg[256];

	// 初期化  ????/ // /
	if( connecGameServerProcNo == 0 )
	{
		connecGameServerProcNo = 1;

		sprintf( msg, "%s连接中", selectServerName2[selectServerIndex] );

		// 连接框架大小设置
		//w = (strlen( msg )*9+63)/64;
		w = (strlen( msg )*9+63)/64+3;
		if( w < 2 )
			w = 2;
		h = (16+47)/48;
		if( h < 2 )
			h = 2;
		x = (640-w*64)/2;
		y = (480-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );
	}

	// 	// 初始化网络
	if( connecGameServerProcNo == 1 )
	{
		cleanupNetwork();
		if( initNet() )
		{
			connecGameServerProcNo = 2;
		}
		else
		{
			ret = -1;
		}
	}
	// 初始化服务器连接
	if( connecGameServerProcNo == 2 )
	{
		initConnectServer();
		connecGameServerProcNo = 3;
	}
	if( connecGameServerProcNo == 3 )
	{
		ret2 = connectServer();
		if( ret2 == 1 )
		{
			ret = 1;
		}
		else
		if( ret2 < 0 )
		{
			ret = -2;
		}
	}

	if( ret != 0 )
	{
		if( ptActMenuWin )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
		}
	}

	if( ptActMenuWin != NULL )
	{
		// 显示窗口
		if( ptActMenuWin->hp >= 1 )
		{
			int len;
			int xx, yy;

			len = strlen( msg );
			xx = (w*64-len*8)/2;
			yy = (h*48-16)/2;
			StockFontBuffer( x+xx, y+yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0 );
		}
	}

	return ret;
}








///////////////////////////////////////////////////////////////////////////
//
// キャラ選択画面（削除も出来る） 角色选择画面（删除角色也可以）
//
void selectCharacterProc( void )
{
	int ret;
	int i, j;
	int login;
	int x;
	char msg[256];
	static int btnGraId[] = { -2, -2, -2, -2, -2, -2, -2 };
	int btnUseFlag = 0;
	int attrColor[4][2] =
	{
		{ SYSTEM_PAL_GREEN,  SYSTEM_PAL_GREEN2  },
		{ SYSTEM_PAL_AQUA,   SYSTEM_PAL_AQUA2   },
		{ SYSTEM_PAL_RED,    SYSTEM_PAL_RED2    },
		{ SYSTEM_PAL_YELLOW, SYSTEM_PAL_YELLOW2 }
	};
	int x1, y1, x2, y2;

	// キャラクタリスト取得処理初期化  角色列表取得处理初始化
	if( SubProcNo == 0 )
	{
		SubProcNo++;

		initDownloadCharList();
	}
	// キャラクタリスト取得処理  角色列表取得处理
	if( SubProcNo == 1 )
	{
		ret = downloadCharList();
		if( ret == 1 )
		{
			// 取得完了
			SubProcNo = 10;
			play_bgm(2);
		}
		else
		if( ret == -3 )
		{
			// メンテナンス中 维护中
			SubProcNo = 200;
			// ネットワーク初期化
			cleanupNetwork();
		}
		else
		if( ret == -4 )
		{
			// ユーザ認証失敗
			SubProcNo = 400;
			// ネットワーク初期化
			cleanupNetwork();
		}
		else
		if( ret < 0 )
		{
			// エラー発生  发生错误
			SubProcNo = 100;
			// ネットワーク初期化
			cleanupNetwork();
		}
	}

	if( SubProcNo == 10 )
	{
		for( i = 0; i < sizeof( btnGraId )/sizeof( int ); i++ )
		{
			btnGraId[i] = -2;
		}
		SubProcNo++;
	}
	if( SubProcNo == 11 )
	{
		btnUseFlag = 2;
	}

	// 削除処理初期化
	if( SubProcNo == 20 )
	{
		ret = commonYesNoWindow( 320, 240 );
		// はい 是
		if( ret == 1 )
		{
			SubProcNo++;
		}
		else
		// いいえ 不是
		if( ret == 2 )
		{
			SubProcNo = 10;
		}
	}
	if( SubProcNo == 21 )
	{
		initDeleteCharacter();
		SubProcNo++;
	}
	// 削除処理
	if( SubProcNo == 22 )
	{
		ret = deleteCharacter();
		if( ret == 1 )
		{
			resetCharacterList( selectPcNo );
			clearUserSetting( selectPcNo );
			if( saveNowState() )
			{
			}
			if( maxPcNo > 0 )
				maxPcNo--;
			SubProcNo = 10;
		}
		else
		if( ret == -2 )
		{
			SubProcNo = 300;
		}
	}

	// エラー
	if( SubProcNo == 100 )
	{
		initCommonMsgWin();
		SubProcNo++;
	}
	if( SubProcNo == 101 )
	{
		if( commonMsgWin( "角色列表的取得失败" ) )
		{
			// ＯＫボタンが押された  按下了按钮OK
			ChangeProc( PROC_TITLE_MENU );
		}
	}

	// メンテナンス中エラー  维护中错误
	if( SubProcNo == 200 )
	{
		initCommonMsgWin();
		SubProcNo++;
	}
	if( SubProcNo == 201 )
	{
		if( commonMsgWin( "服务器维护" ) )
		{
			// ＯＫボタンが押された
			ChangeProc( PROC_TITLE_MENU );
		}
	}

	// ユーザ認証失敗処理  用户认证失败处理
	if( SubProcNo == 400 )
	{
		initUserCertifyErrorMsgWin();
		SubProcNo++;
	}
	if( SubProcNo == 401 )
	{
		if( userCertifyErrorMsgWin() )
		{
			// ＯＫボタンが押された
			ChangeProc( PROC_TITLE_MENU );
		}
	}

	// キャラ削除失敗処理  人物删除失败处理
	if( SubProcNo == 300 )
	{
		initCommonMsgWin();
		SubProcNo++;
	}
	if( SubProcNo == 301 )
	{
		if( commonMsgWin( "人物删除失败" ) )
		{
			// ＯＫボタンが押された
			SubProcNo = 10;
		}
	}


	ret = selGraId( btnGraId, sizeof( btnGraId )/sizeof( int ) );
	// 戻りボタン押した 返回按钮。
	if( ret == 0 )
	{
		fade_out_bgm();

		ChangeProc( PROC_TITLE_MENU );
		play_se( 217, 320, 240 );	// クリック音
	}
	else
	// 新規作成ボタン押された 新建人物按钮按下了
	if( 1 <= ret && ret <= 2 )
	{
		selectPcNo = ret - 1;
		ChangeProc( PROC_CHAR_MAKE );
		play_se( 217, 320, 240 );	// クリック音
	}
	else
	// ログインボタン押された   登录按钮
	if( 3 <= ret && ret <= 4 )
	{
		char name[CHAR_NAME_LEN+1];

		// 使用キャラ決定
		strcpy( name, chartable[ret-3].name );
		sjisStringToEucString( name );
		strcpy( gamestate_login_charname, name );
		selectPcNo = ret - 3;

		// 初期化時にpc.faceGraNoに入れるため
		newCharacterFaceGraNo = chartable[selectPcNo].faceGraNo;
		loginDp = chartable[selectPcNo].dp;

		ChangeProc( PROC_CHAR_LOGIN_START );
		play_se( 217, 320, 240 );	// クリック音
	}
	else
	// 削除ボタン押された
	if( 5 <= ret && ret <= 6 )
	{
		char name[CHAR_NAME_LEN+1];

		// 削除キャラ決定
		strcpy( name, chartable[ret-5].name );
		sjisStringToEucString( name );
		strcpy( gamestate_deletechar_charname, name );
		selectPcNo = ret - 5;
		SubProcNo = 20;
		play_se( 217, 320, 240 );	// クリック音
	}


	if( SubProcNo >= 10 )
	{
		// パラメータの表示
		for( i = 0; i < MAXCHARACTER; i++ )
		{
			if( existCharacterListEntry( i ) )
			{
				// 顔画像
				StockDispBuffer( 168+i*304, 84, DISP_PRIO_CHAR, chartable[i].faceGraNo, 0 );

				// 名前  名字显示错误
				x = (144-strlen( chartable[i].name )*9)/2;
				//重点这个函数处理有问题导致中文全部错误
			
				StockFontBuffer( 93+i*304+x, 127,
					FONT_PRIO_BACK, FONT_PAL_WHITE, chartable[i].name, 0 );
				

				// レベル
				sprintf( msg, "%3d", chartable[i].level );
				StockFontBuffer( 104+i*304, 152,
					FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0 );

				// ＭＡＸ ＨＰ
				sprintf( msg, "%3d", chartable[i].hp );
				StockFontBuffer( 104+i*304, 174,
					FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0 );

				// 攻撃力
				sprintf( msg, "%3d", chartable[i].str );
				StockFontBuffer( 104+i*304, 196,
					FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0 );

				// 防御力
				sprintf( msg, "%3d", chartable[i].def );
				StockFontBuffer( 104+i*304, 218,
					FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0 );

				// 敏捷力
				sprintf( msg, "%3d", chartable[i].agi );
				StockFontBuffer( 104+i*304, 240,
					FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0 );

				// 魅力
				sprintf( msg, "%3d", chartable[i].app );
				StockFontBuffer( 104+i*304, 262,
					FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0 );

				// ＤＰ
				sprintf( msg, "%8d", chartable[i].dp );
				StockFontBuffer( 188+i*304, 262,
					FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0 );

				// 属性のメータ
				for( j = 0; j < 4; j++ )
				{
					if( chartable[i].attr[j] > 0 )
					{
						x1 = 208+i*304;
						y1 = 177+j*22;
						x2 = x1 + chartable[i].attr[j] * 8;
						y2 = y1 + 8;
						StockBoxDispBuffer( x1,   y1,   x2,   y2,   DISP_PRIO_IME2, attrColor[j][0], 1 );
						StockBoxDispBuffer( x1+1, y1+1, x2+1, y2+1, DISP_PRIO_IME1, attrColor[j][1], 0 );
						StockBoxDispBuffer( x1+2, y1+2, x2+2, y2+2, DISP_PRIO_IME1, attrColor[j][1], 0 );
					}
				}

				// ログイン回数
				login = chartable[i].login;
				if( login >= 1000 )
					login = 9999;	// カウンターストップ
				sprintf( msg, "%4d", login );
				StockFontBuffer( 156+i*304, 284,
					FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0 );

				// キャラいるなら
				// ログインボタン表示
				btnGraId[3+i] =
					StockDispBuffer( 100+i*304, 342,
						DISP_PRIO_CHAR, CG_CHR_SEL_LOGIN_BTN, btnUseFlag );
				// 削除ボタン表示
				btnGraId[5+i] =
					StockDispBuffer( 230+i*304, 342,
						DISP_PRIO_CHAR, CG_CHR_SEL_DEL_BTN, btnUseFlag );
			}
			else
			if( SubProcNo < 100 || 300 <= SubProcNo )
			{
				// キャラがいなければ新規ボタン表示
				btnGraId[1+i] =
					StockDispBuffer( 160+i*304,
						342, DISP_PRIO_CHAR, CG_CHR_SEL_NEW_BTN, btnUseFlag );
			}
		}

		// 戻るボタン
		btnGraId[0] =
			StockDispBuffer( 320, 432, DISP_PRIO_CHAR, CG_CHR_SEL_BACK_BTN, btnUseFlag );
	}

	// ＢＧ表示
	StockDispBuffer( 320, 240, DISP_PRIO_BG, CG_CHR_SEL_BG, 0 );

	RunAction();			// アクション走らせる
	StockTaskDispBuffer();	// タスク表示データをバッファに溜める
}



// キャラクタ削除処理

static short deleteCharacterProcNo = 0;

// 初期化
void initDeleteCharacter( void )
{
	deleteCharacterProcNo = 0;
}

// メイン処理
//
//  戻り値：	 0 ... 削除中
//				 1 ... 削除完了
//				-1 ... タイムアウト
//				-2 ... エラー
int deleteCharacter( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	int ret = 0;
	static char msg[] = "キャラクター削除中";

	// 初期化
	if( deleteCharacterProcNo == 0 )
	{
		deleteCharacterProcNo = 1;

		// ウィンドウ作成
		w = strlen( msg )*9/64+2;
		h = (16+47)/48;
		if( h < 2 )
			h = 2;
		x = (640-w*64)/2;
		y = (480-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );
	}

	// キャラリスト取得
	if( deleteCharacterProcNo == 1 )
	{
		delCharStart();
		deleteCharacterProcNo++;
	}
	else
	if( deleteCharacterProcNo == 2 )
	{
		ret = delCharProc();
	}

	if( ret != 0 )
	{
		if( ptActMenuWin )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
		}
	}

	if( ptActMenuWin != NULL )
	{
		// ウィンドウ表示
		if( ptActMenuWin->hp >= 1 )
		{
			int len;
			int xx, yy;

			len = strlen( msg );
			xx = (w*64-len*8)/2;
			yy = (h*48-16)/2;
			StockFontBuffer( x+xx, y+yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0 );
		}
	}

	return ret;
}



// キャラクタリスト取得処理

static short downloadCharListProcNo = 0;

// 初期化
void initDownloadCharList( void )
{
	downloadCharListProcNo = 0;
}

// メイン処理
//
//  戻り値：	 0 ... 受信中
//				 1 ... 受信完了
//				-1 ... タイムアウト
//				-2 ... エラー
//              -3 ... サーバメンテナンス
//              -4 ... ユーザ認証失敗
int downloadCharList( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	int ret = 0;
	static char msg[256];

	// 初期化
	if( downloadCharListProcNo == 0 )
	{
		downloadCharListProcNo = 1;

		strcpy( msg, "キャラクターリスト取得中" );

		// ウィンドウ作成
		w = strlen( msg )*8/64+2;
		h = (16+47)/48;
		if( h < 2 )
			h = 2;
		x = (640-w*64)/2;
		y = (480-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );
	}

	// キャラリスト取得
	if( downloadCharListProcNo == 1 )
	{
		charListStart();
		downloadCharListProcNo++;
	}
	else
	if( downloadCharListProcNo == 2 )
	{
		ret = charListProc();
	}

	if( ret != 0 )
	{
		if( ptActMenuWin )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
		}
	}

	if( ptActMenuWin != NULL )
	{
		// ウィンドウ表示
		if( ptActMenuWin->hp >= 1 )
		{
			int len;
			int xx, yy;

			len = strlen( msg );
			xx = (w*64-len*8)/2;
			yy = (h*48-16)/2;
			StockFontBuffer( x+xx, y+yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0 );
		}
	}

	return ret;
}


// はい、いいえ、選択ウィンドウ
//
//  戻り値：	0 ... 処理中
//				1 ... はいボタンが押された
//				2 ... いいえボタンが押された
int commonYesNoWindow( int x, int y )
{
	int i;
	int ret = 0;
	int id;
	static int btnGraId[] = { -2, -2 };

	id = selGraId( btnGraId, sizeof( btnGraId )/sizeof( int ) );
	// はいボタンが押された
	if( id == 0 )
	{
		ret = 1;
		play_se( 217, 320, 240 );	// クリック音
	}
	else
	// いいえボタンが押された
	if( id == 1 )
	{
		ret = 2;
		play_se( 217, 320, 240 );	// クリック音
	}

	btnGraId[0] = StockDispBuffer( x, y, DISP_PRIO_YES_NO_BTN, CG_COMMON_YES_BTN, 2 );
	btnGraId[1] = StockDispBuffer( x, y, DISP_PRIO_YES_NO_BTN, CG_COMMON_NO_BTN, 2 );

	StockDispBuffer( x, y, DISP_PRIO_YES_NO_WND, CG_COMMON_WIN_YORO, 0 );

	if( ret != 0 )
	{
		for( i = 0; i < sizeof( btnGraId )/sizeof( int ); i++ )
		{
			btnGraId[i] = -2;
		}
	}

	return ret;
}


///////////////////////////////////////////////////////////////////////////
//
// キャラ作成
//

// キャラ画像選択用

#define	MaxSelectChar  	12
static ACTION *ptActSelChar[MaxSelectChar];
static int selectGraNoTbl[MaxSelectChar] =
{
	SPR_001em,  //SPR_001em,
	SPR_011em,
	SPR_021em,
	SPR_031em,
	SPR_041em,
	SPR_051em,
	SPR_061em,
	SPR_071em,
	SPR_081em,
	SPR_091em,
	SPR_101em,
	SPR_111em
};
int selectGraLocate[MaxSelectChar][2] =
{
	{ 198, 301 },
	{ 113, 258 },
	{ 248, 396 },
	{ 130, 370 },
	{ 137, 160 },
	{ 343, 379 },
	{ 479, 371 },
	{ 420, 310 },
	{ 376, 231 },
	{ 297, 198 },
	{ 381, 146 },
	{ 242, 116 }
};
int selectGraHitArea[MaxSelectChar][4] =
{
//	    x,   y,   w,   h
	{ -32, -56,  63,  65 },
	{ -21, -72,  42,  88 },
	{ -19, -78,  38,  94 },
	{ -20, -67,  41,  80 },
	{ -30, -80,  60, 100 },
	{ -36, -84,  72, 100 },
	{ -30, -51,  59,  64 },
	{ -19, -77,  40,  88 },
	{ -18, -72,  34,  83 },
	{ -22, -72,  43,  89 },
	{ -18, -78,  35,  90 },
	{ -20, -76,  40,  90 }//
};
static int selCharColor[MaxSelectChar];	// キャラの色
static int selCharDir[MaxSelectChar];	// キャラの向き
static int selCharGraNoProcNo;
static int nowSelCharGraNo;			// キャラの種類
static int nowSelCharMouthNo;		// キャラの口の形
static int nowSelCharEyeNo;			// キャラの目の形
static int nowSelCharStatusPoint;	// キャラのステータスに割り振れるポイント
static int nowSelCharStatus[4];		// キャラのステータス(体力、腕力、丈夫さ、素早さ)
static int nowSelCharAttrPoint;		// キャラの属性に割り振れるポイント
static int nowSelCharAttr[4];		// キャラの属性(地水火風)

// キャラの色変更ウィンドウ処理用
static int selCharGraColorWinProcNo;

// キャラクタのパラメータ設定用
static int editCharParamProcNo;
static STR_BUFFER selCharName;
static int selCharNameBoxX, selCharNameBoxY;

// 出身地の選択用
static int selHomeTownProcNo;
static int nowSelHomeTownNo;

// 新キャラ作成処理用
static int createCharProcNo;



void initMakeCharacter( void );

void initSelCharGraNo( void );
void initSelCharGraNo2( void );
int selCharGraNo( void );
void initSelCharGraColorWin( void );
int selCharGraColorWin( void );
void initEditCharParam( void );
void initEditCharParam2( void );
int editCharParam( void );
void initCreateChar( void );
int createChar( void );
void initSelHomeTown( void );
int selHomeTown( void );



// キャラ作成メイン処理初期化
void initMakeCharacter( void )
{
}

// キャラ作成メイン処理
void makeCharacterProc( void )
{
	int ret;

	// 初期化
	if( SubProcNo == 0 )
	{
		initMakeCharacter();
		SubProcNo++;
	}

	// キャラ画像選択の初期化
	if( SubProcNo == 1 )
	{
		initSelCharGraNo();
		SubProcNo = 3;
	}
	else
	if( SubProcNo == 2 )
	{
		initSelCharGraNo2();
		SubProcNo++;
	}
	// キャラ画像選択処理
	if( SubProcNo == 3 )
	{
		ret = selCharGraNo();
		// キャラ選択決定
		if( ret == 1 )
		{
			SubProcNo = 10;
		}
		else
		// 戻るボタン押された
		if( ret == 2 )
		{
			ChangeProc( PROC_CHAR_SELECT, 10 );
//			return;
		}
	}

	// キャラのパラメータ設定の初期化
	if( SubProcNo == 10 && ret == 1 )
	{
		initEditCharParam();
		SubProcNo = 12;
	}
	else
	if( SubProcNo == 11 )
	{
		initEditCharParam2();
		SubProcNo = 12;
	}
	if( SubProcNo == 12 )
	{
		ret = editCharParam();
		if( ret == 1 )
		{
			SubProcNo = 20;
		}
		// キャンセル
		if( ret == 2 )
		{
			SubProcNo = 2;
		}
	}

	// 出身地の選択
	if( SubProcNo == 20 )
	{
		initSelHomeTown();
		SubProcNo = 21;
	}
	if( SubProcNo == 21 )
	{
		ret = selHomeTown();
		// 決定
		if( ret == 1 )
		{
			SubProcNo = 30;
			// 以前のセーブデータを初期化
			resetCharacterList( selectPcNo );
			clearUserSetting( selectPcNo );
			if( saveNowState() )
			{
			}
		}
		else
		// キャンセル
		if( ret == 2 )
		{
			SubProcNo = 11;
		}
	}

	// キャラ作成プロトコルを送信
	if( SubProcNo == 30 )
	{
		initCreateChar();
		SubProcNo = 31;
	}
	if( SubProcNo == 31 )
	{
		ret = createChar();
		if( ret == 1 )
		{
			maxPcNo++;
			strcpy( gamestate_login_charname, newCharacterName );
			ChangeProc( PROC_CHAR_LOGIN_START );
			//return;
		}
		else
		if( ret == 2 )
		{
			SubProcNo = 100;
		}
		selHomeTown();	// 画面作るため
	}

	// エラー
	if( SubProcNo == 100 )
	{
		initCommonMsgWin();
		SubProcNo++;
	}
	if( SubProcNo == 101 )
	{
		//_asm{int 3}
		if( commonMsgWin( "人物生成失败还是成功？" ) )
		{
			// ＯＫボタンが押された OK按钮按下了
			ChangeProc( PROC_CHAR_SELECT, 10 );
			//return;
		}
		selHomeTown();	// 画面作るため  为了制作画面
	}

	RunAction();			// アクション走らせる  动作运行
	//显示数据任务缓冲蓄
	StockTaskDispBuffer();	// タスク表示データをバッファに溜める
}




// キャラ画像選択の初期化
void initSelCharGraNo( void )
{
	int i;

	selCharGraNoProcNo = 0;

	// キャラ画像選択の初期化
	nowSelCharGraNo = -1;	// 非選択状態は-1
	nowSelCharMouthNo = 0;
	nowSelCharEyeNo = 0;

	for( i = 0; i < MaxSelectChar; i++ )
	{
		selCharColor[i] = 0;	// キャラの色
		selCharDir[i] = 0;		// キャラの向き
	}

	// キャラのアクション作成
	for( i = 0; i < MaxSelectChar; i++ )
	{
		if( selectGraNoTbl[i] < 0 )
		{
			ptActSelChar[i] = NULL;
			continue;
		}

		ptActSelChar[i] = GetAction( PRIO_CHR, 0 );
		if( ptActSelChar[i] != NULL )
		{
			ptActSelChar[i]->anim_chr_no = selectGraNoTbl[i];
			ptActSelChar[i]->anim_no = ANIM_STAND;
			ptActSelChar[i]->dispPrio = DISP_PRIO_CHAR;
			ptActSelChar[i]->x = selectGraLocate[i][0];
			ptActSelChar[i]->y = selectGraLocate[i][1];
			ptActSelChar[i]->anim_ang = selCharDir[i];
		}
	}

}

// キャラ画像選択の初期化
// （前の処理から戻ってきたときの初期化。パラメータを残しておくため）
void initSelCharGraNo2( void )
{
	int i;

	selCharGraNoProcNo = 0;
	nowSelCharMouthNo = 0;
	nowSelCharEyeNo = 0;

	// キャラ画像選択の初期化
	nowSelCharGraNo = -1;	// 非選択状態は-1

	// キャラのアクション作成
	for( i = 0; i < MaxSelectChar; i++ )
	{
		if( selectGraNoTbl[i] < 0 )
		{
			ptActSelChar[i] = NULL;
			continue;
		}

		ptActSelChar[i] = GetAction( PRIO_CHR, 0 );
		if( ptActSelChar[i] != NULL )
		{
			ptActSelChar[i]->anim_chr_no = selectGraNoTbl[i];
			ptActSelChar[i]->anim_no = ANIM_STAND;
			ptActSelChar[i]->dispPrio = DISP_PRIO_CHAR;
			ptActSelChar[i]->x = selectGraLocate[i][0];
			ptActSelChar[i]->y = selectGraLocate[i][1];
			ptActSelChar[i]->anim_ang = selCharDir[i];
		}
	}
}

// キャラ画像選択処理
//
//  戻り値：	0 ... 処理中
//				1 ... 決定
//				2 ... キャンセル
int selCharGraNo( void )
{
	int i;
	int ret = 0;
	int ret2;
	int x1, y1, x2, y2;
	int id;
	static int selCharCanselGraId[] = { -2 };

	// 初期化
	if( selCharGraNoProcNo == 0 )
	{
		for( i = 0; i < sizeof( selCharCanselGraId )/sizeof( int ); i++ )
		{
			selCharCanselGraId[i] = -2;
		}
		selCharGraNoProcNo++;
	}

	// キャラ選択
	if( selCharGraNoProcNo == 1 )
	{
		for( i = 0; i < MaxSelectChar; i++ )
		{
			if( ptActSelChar[i] != NULL )
			{
				x1 = ptActSelChar[i]->x + selectGraHitArea[i][0];
				y1 = ptActSelChar[i]->y + selectGraHitArea[i][1];
				x2 = x1 + selectGraHitArea[i][2];
				y2 = y1 + selectGraHitArea[i][3];
				if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX ) )
				{
					ptActSelChar[i]->anim_no = ANIM_WALK;
					if( mouse.onceState & MOUSE_LEFT_CRICK )
					{
						// キャラが選択された
						selCharGraNoProcNo = 2;
						nowSelCharGraNo = i;
						play_se( 217, 320, 240 );	// クリック音
					}
				}
				else
				{
					ptActSelChar[i]->anim_no = ANIM_STAND;
				}
			}
		}
	}

	// カラー選択メニュー
	if( selCharGraNoProcNo == 2 )
	{
		initSelCharGraColorWin();
		selCharGraNoProcNo++;
	}
	if( selCharGraNoProcNo == 3 )
	{
		ret2 = selCharGraColorWin();
		// 決定
		if( ret2 == 1 )
		{
			ret = 1;
		}
		else
		// キャンセルされた
		if( ret2 == 2 )
		{
			selCharGraNoProcNo = 1;
			nowSelCharGraNo = -1;
		}
	}

	// グラフィックの当たり判定
	id = selGraId( selCharCanselGraId, sizeof( selCharCanselGraId )/sizeof( int ) );
	// 戻るボタン
	if( id == 0 )
	{
		ret = 2;
		play_se( 217, 320, 240 );	// クリック音
	}

	// キャラのアクション消す
	if( ret != 0 )
	{
		for( i = 0; i < MaxSelectChar; i++ )
		{
			if( ptActSelChar[i] != NULL )
			{
				DeathAction( ptActSelChar[i] );
				ptActSelChar[i] = NULL;
			}
		}
	}


	// キャラの表示
	for( i = 0; i < MaxSelectChar; i++ )
	{
		if( ptActSelChar[i] != NULL )
		{
			if( nowSelCharGraNo == i )
			{
				ptActSelChar[i]->anim_no = ANIM_WALK;
				x1 = ptActSelChar[i]->x + selectGraHitArea[i][0];
				y1 = ptActSelChar[i]->y + selectGraHitArea[i][1];
				x2 = x1 + selectGraHitArea[i][2];
				y2 = y1 + selectGraHitArea[i][3];
				StockBoxDispBuffer( x1, y1, x2, y2, DISP_PRIO_BOX, BoxColor, 0 );
				//MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX );
			}
			ptActSelChar[i]->anim_chr_no = selectGraNoTbl[i] + selCharColor[i]*5;
			ptActSelChar[i]->anim_ang = selCharDir[i];
			pattern( ptActSelChar[i], ANM_NOMAL_SPD, ANM_LOOP );
		}
	}

	if( selCharGraNoProcNo == 1 )
	{
		id = focusGraId( selCharCanselGraId, sizeof( selCharCanselGraId )/sizeof( int ) );
		if( id == 0 )
		{
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"前の画面に戻ります。", 0 );
		}
		else
		{
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"キャラクターを選択してください。", 0 );
		}

		// キャラ選択に戻る
		selCharCanselGraId[0] =
			StockDispBuffer( 64, 400, DISP_PRIO_BG, CG_CHR_MAKE_BACK_BTN, 2 );
	}

	if( ret == 0 )
	{
		// ＢＧ表示
		StockDispBuffer( 320, 240, DISP_PRIO_BG, CG_CHR_MAKE_SEL_BG, 0 );
	}

	RunAction();			// アクション走らせる
	StockTaskDispBuffer();	// タスク表示データをバッファに溜める

	return ret;
}




// キャラの色変更ウィンドウ処理初期化
void initSelCharGraColorWin( void )
{
	selCharGraColorWinProcNo = 0;
}

// キャラの色変更ウィンドウ処理初期化
//
//  戻り値：	0 ... 処理中
//				1 ... OKボタンが押された
//				2 ... キャンセルボタンが押された
int selCharGraColorWin( void )
{
	static int fontId[] = { -2, -2, -2, -2 };
	static ACTION *ptActMenuWin = NULL;
	int id;
	int i;
	static int x, y, w, h;
	int ret = 0;
	char msg[][16] =
	{
		"人物确定 ",
		"形象选择",
		"方向查看 ",
		"重选人物"
	};

	// 初期化
	if( selCharGraColorWinProcNo == 0 )
	{
		selCharGraColorWinProcNo = 1;

		for( i = 0; i < sizeof( fontId )/sizeof( int ); i++ )
		{
			fontId[i] = -2;
		}

		// ウィンドウ作成
		w = 2;
		h = 3;
		x = 496;
		y = 80;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );
	}

	// フォントの選択判定
	id = selFontId( fontId, sizeof( fontId )/sizeof( int ) );

	// 決定
	if( id == 0 )
	{
		ret = 1;
		play_se( 217, 320, 240 );	// クリック音
	}
	else
	// カラー選択
	if( id == 1 )
	{
		selCharColor[nowSelCharGraNo]++;
		if( selCharColor[nowSelCharGraNo] > 3 )
			selCharColor[nowSelCharGraNo] = 0;
		play_se( 217, 320, 240 );	// クリック音
	}
	else
	// 方向ボタン
	if( id == 2 )
	{
		selCharDir[nowSelCharGraNo]++;
		if( selCharDir[nowSelCharGraNo] > 7 )
			selCharDir[nowSelCharGraNo] = 0;
		play_se( 217, 320, 240 );	// クリック音
	}
	else
	// キャンセル
	if( id == 3 )
	{
		ret = 2;
		play_se( 217, 320, 240 );	// クリック音
	}

	// 結果がわかったらウィンドウ閉じる
	if( ret != 0 )
	{
		if( ptActMenuWin )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
		}
	}


	// カーソルのあたっているメニュー
	id = focusFontId( fontId, sizeof( fontId )/sizeof( int ) );

	// 決定
	if( id == 0 )
	{
		StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
			"選択したキャラクターに決定します。", 0 );
	}
	else
	// カラー選択
	if( id == 1 )
	{
		StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
			"キャラクターの色を変更します。", 0 );
	}
	else
	// 方向ボタン
	if( id == 2 )
	{
		StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
			"キャラクターを他の向きに変更します。", 0 );
	}
	else
	// キャンセル
	if( id == 3 )
	{
		StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
			"キャラクター選択に戻ります。", 0 );
	}

	if( ptActMenuWin != NULL )
	{
		// ウィンドウ表示
		if( ptActMenuWin->hp >= 1 )
		{
			for( i = 0; i < sizeof( fontId )/sizeof( int ); i++ )
			{
				fontId[i] =
					StockFontBuffer( x+22, y+i*22+32,
						FONT_PRIO_FRONT, FONT_PAL_YELLOW, msg[i], 2 );
			}
		}
	}

	return ret;
}




// キャラクタのパラメータ設定初期化
void initEditCharParam( void )
{
	editCharParamProcNo = 0;


	// 名前入力処理の初期化
	selCharNameBoxX = 104;
	selCharNameBoxY = 102;
	initStrBuffer( &selCharName, selCharNameBoxX, selCharNameBoxY,
		CHAR_NAME_LEN, FONT_PAL_WHITE, FONT_PRIO_BACK );
	selCharName.buffer[0] = '\0';
	selCharName.cnt = 0;
	GetKeyInputFocus( &selCharName );

	// 能力ポイント振り分け処理の初期化
	nowSelCharStatusPoint = 20;		// キャラのステータスに割り振れるポイント
	nowSelCharStatus[0]   =  0;		// キャラの体力
	nowSelCharStatus[1]   =  0;		// キャラの腕力
	nowSelCharStatus[2]   =  0;		// キャラの丈夫さ
	nowSelCharStatus[3]   =  0;		// キャラの素早さ
	nowSelCharAttrPoint   = 10;		// キャラの属性に割り振れるポイント
	nowSelCharAttr[0]     =  0;		// キャラの地属性
	nowSelCharAttr[1]     =  0;		// キャラの水属性
	nowSelCharAttr[2]     =  0;		// キャラの火属性
	nowSelCharAttr[3]     =  0;		// キャラの風属性
}

// キャラクタのパラメータ設定初期化２
void initEditCharParam2( void )
{
	editCharParamProcNo = 0;

	// 名前入力処理の初期化
	selCharNameBoxX = 104;
	selCharNameBoxY = 102;
	initStrBuffer( &selCharName, selCharNameBoxX, selCharNameBoxY,
		CHAR_NAME_LEN, FONT_PAL_WHITE, FONT_PRIO_BACK );
	GetKeyInputFocus( &selCharName );
}

// キャラクタのパラメータ設定
//
//  戻り値：	0 ... 処理中
//				1 ... 決定
//				2 ... キャンセル
int editCharParam( void )
{
	int ret = 0;
	char msg[64];
	int x1, y1, x2, y2;
	static int graId[] =
	{ -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	  -2, -2, -2, -2, -2, -2, -2, -2,
	  -2, -2, -2, -2 };
	int i;
	int id, id2, id3;
	int statusLocate[4][2] =
	{
		{ 111, 250 },
		{ 111, 278 },
		{ 111, 305 },
		{ 111, 334 }
	};
	int attrLocate[4][2] =
	{
		{ 297, 255 },
		{ 297, 279 },
		{ 297, 303 },
		{ 297, 327 }
	};
	int attrColor[4][2] =
	{
		{ SYSTEM_PAL_GREEN,  SYSTEM_PAL_GREEN2  },
		{ SYSTEM_PAL_AQUA,   SYSTEM_PAL_AQUA2   },
		{ SYSTEM_PAL_RED,    SYSTEM_PAL_RED2    },
		{ SYSTEM_PAL_YELLOW, SYSTEM_PAL_YELLOW2 }
	};
	int upDownBtnGraNo[][2] =
	{
		{ CG_PREV_BTN, CG_PREV_BTN_DOWN },
		{ CG_NEXT_BTN, CG_NEXT_BTN_DOWN }
	};
	int selUseFlag = 0;
	int btn1, btn2;



	GetKeyInputFocus( NULL );

	// 初期化
	if( editCharParamProcNo == 0 )
	{
		for( i = 0; i < sizeof( graId )/sizeof( int ); i++ )
		{
			graId[i] = -2;
		}
		editCharParamProcNo++;
	}

	// パラメータ設定処理
	id = -1;
	id3 = -1;
	if( editCharParamProcNo == 1 )
	{
		selUseFlag = 2;
		GetKeyInputFocus( &selCharName );

		// グラフィックの当たり判定
		//id = selGraId( graId, sizeof( graId )/sizeof( int ) );
		id = selRepGraId( graId, sizeof( graId )/sizeof( int ) );
		id3 = pushGraId( graId, sizeof( graId )/sizeof( int ) );


		// 決定
		if( id == 0 )
		{
			// 名前が入力されてない
			if( selCharName.cnt <= 0 )
			{
				editCharParamProcNo = 100;
			}
			else
			// ステータスの振り分けポイントが残ってる
			if( nowSelCharStatusPoint > 0 )
			{
				editCharParamProcNo = 104;
			}
			else
			// 属性の振り分けポイントが残ってる
			if( nowSelCharAttrPoint > 0 )
			{
				editCharParamProcNo = 106;
			}
			else
			// 同じキャラ名がいる
			{
				strcpy( newCharacterName, selCharName.buffer );
				newCharacterName[selCharName.cnt] = '\0';

				if( cmpNameCharacterList( selCharName.buffer ) )
				{
					// 同じ名前があるのでダメ
					editCharParamProcNo = 102;
				}
				else
				// 空白文字が含まれている名前はダメ
				if( strstr( newCharacterName, " " ) != NULL
				 || strstr( newCharacterName, "　" ) != NULL )
				{
					editCharParamProcNo = 108;
				}
				else
				{
					// 決定
					ret = 1;
					sjisStringToEucString( newCharacterName );
					newCharacterGraNo =
						selectGraNoTbl[nowSelCharGraNo]+selCharColor[nowSelCharGraNo]*5;
					newCharacterFaceGraNo =
						CG_CHR_MAKE_FACE+nowSelCharGraNo*100
						+selCharColor[nowSelCharGraNo]*25
						+nowSelCharEyeNo*5+nowSelCharMouthNo;
					newCharacterVit = nowSelCharStatus[0];
					newCharacterStr = nowSelCharStatus[1];
					newCharacterTgh = nowSelCharStatus[2];
					newCharacterDex = nowSelCharStatus[3];
					newCharacterEarth = nowSelCharAttr[0];
					newCharacterWater = nowSelCharAttr[1];
					newCharacterFire  = nowSelCharAttr[2];
					newCharacterWind  = nowSelCharAttr[3];
					loginDp = 0;
					play_se( 217, 320, 240 );	// クリック音
				}
			}
		}
		else
		// 戻る
		if( id == 1 )
		{
			ret = 2;
			play_se( 217, 320, 240 );	// クリック音
		}
		else
		// ステータスパラメータ振り分け
		if( 2 <= id && id <= 9 )
		{
			id2 = id - 2;
			// パラメータを減らす
			if( (id2 % 2) == 0 )
			{
				if( nowSelCharStatus[id2/2] > 0 )
				{
					nowSelCharStatus[id2/2]--;
					nowSelCharStatusPoint++;
					play_se( 217, 320, 240 );	// クリック音
				}
			}
			else
			// パラメータを増やす
			{
				if( nowSelCharStatusPoint > 0 )
				{
					nowSelCharStatus[id2/2]++;
					nowSelCharStatusPoint--;
					play_se( 217, 320, 240 );	// クリック音
				}
			}
		}
		else
		// 属性の振り分け
		if( 10 <= id && id <= 17 )
		{
			id2 = id - 10;
			// パラメータを減らす
			if( (id2 % 2) == 0 )
			{
				if( nowSelCharAttr[id2/2] > 0 )
				{
					nowSelCharAttr[id2/2]--;
					nowSelCharAttrPoint++;
					play_se( 217, 320, 240 );	// クリック音
				}
			}
			else
			// パラメータを増やす
			{
				int no, no2;

				no = id2/2;
				if( no < 2 )
				{
					no2 = no+2;
				}
				else
				{
					no2 = no-2;
				}

				if( nowSelCharAttr[no2] == 0 )
				{
					if( nowSelCharAttrPoint > 0 )
					{
						nowSelCharAttr[no]++;
						nowSelCharAttrPoint--;
						play_se( 217, 320, 240 );	// クリック音
					}
					else
					// 振り分けポイントが０で増やすボタンを押したらほかのところから
					// 持ってくる
					{
						// 現在押されたボタン以外にパラメータが振り分けられているところを探す
						for( i = 0; i < 4; i++ )
						{
							if( i != no && nowSelCharAttr[i] > 0 )
							{
								break;
							}
						}
						if( i < 4 )
						{
							nowSelCharAttr[no]++;
							nowSelCharAttr[i]--;
							play_se( 217, 320, 240 );	// クリック音
						}
					}
				}
			}
		}
		else
		if( id == 18 || id == 19 )
		{
			// パラメータを減らす
			if( id == 18 )
			{
				if( nowSelCharEyeNo > 0 )
				{
					nowSelCharEyeNo--;
				}
				else
				{
					nowSelCharEyeNo = 4;
				}
			}
			else
			// パラメータを増やす
			{
				if( nowSelCharEyeNo < 4 )
				{
					nowSelCharEyeNo++;
				}
				else
				{
					nowSelCharEyeNo = 0;
				}
			}
			play_se( 217, 320, 240 );	// クリック音
		}
		else
		if( id == 20 || id == 21 )
		{
			// パラメータを減らす
			if( id == 20 )
			{
				if( nowSelCharMouthNo > 0 )
				{
					nowSelCharMouthNo--;
				}
				else
				{
					nowSelCharMouthNo = 4;
				}
			}
			else
			// パラメータを増やす
			{
				if( nowSelCharMouthNo < 4 )
				{
					nowSelCharMouthNo++;
				}
				else
				{
					nowSelCharMouthNo = 0;
				}
			}
			play_se( 217, 320, 240 );	// クリック音
		}
	}


	// エラーメッセージ
	// 名前入力がまだ
	if( editCharParamProcNo == 100 )
	{
		initCommonMsgWin();
		editCharParamProcNo++;
		play_se( 220, 320, 240 );	// ＮＧ音
	}
	if( editCharParamProcNo == 101 )
	{
		if( commonMsgWin( "名前が入力されていません！" ) )
		{
			// ＯＫボタンが押された
			editCharParamProcNo = 1;
		}
	}
	// 同一キャラ名
	if( editCharParamProcNo == 102 )
	{
		initCommonMsgWin();
		editCharParamProcNo++;
		play_se( 220, 320, 240 );	// ＮＧ音
	}
	if( editCharParamProcNo == 103 )
	{
		if( commonMsgWin( "同じ名前のキャラクターがすでにいます！" ) )
		{
			// ＯＫボタンが押された
			editCharParamProcNo = 1;
		}
	}
	// ステータスの振り分けポイントが残ってる
	if( editCharParamProcNo == 104 )
	{
		initCommonMsgWin();
		editCharParamProcNo++;
		play_se( 220, 320, 240 );	// ＮＧ音
	}
	if( editCharParamProcNo == 105 )
	{
		if( commonMsgWin( "ステータスに全てのポイントを振り分けてください！" ) )
		{
			// ＯＫボタンが押された
			editCharParamProcNo = 1;
		}
	}
	// 属性の振り分けポイントが残ってる
	if( editCharParamProcNo == 106 )
	{
		initCommonMsgWin();
		editCharParamProcNo++;
		play_se( 220, 320, 240 );	// ＮＧ音
	}
	if( editCharParamProcNo == 107 )
	{
		if( commonMsgWin( "属性に全てのポイントを振り分けてください！" ) )
		{
			// ＯＫボタンが押された
			editCharParamProcNo = 1;
		}
	}
	// 名前が空白文字
	if( editCharParamProcNo == 108 )
	{
		initCommonMsgWin();
		editCharParamProcNo++;
		play_se( 220, 320, 240 );	// ＮＧ音
	}
	if( editCharParamProcNo == 109 )
	{
		if( commonMsgWin( "空白文字が含まれる名前は使えません！" ) )
		{
			// ＯＫボタンが押された
			editCharParamProcNo = 1;
		}
	}


	// 説明
	if( editCharParamProcNo == 1 )
	{
		id = focusGraId( graId, sizeof( graId )/sizeof( int ) );
		if( id == 0 )
		{
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"設定したキャラクターに決定します。", 0 );
		}
		else
		if( id == 1 )
		{
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"前の画面に戻ります。", 0 );
		}
		else
		if( 2 <= id && id <= 9 )
		{
			id -= 2;
			id /= 2;
			if( id == 0 )
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"主に、耐久力にかかわります。", 0 );
			}
			else
			if( id == 1 )
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"主に、攻撃時に相手に与えるダメージにかかわります。", 0 );
			}
			else
			if( id == 2 )
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"主に、相手から受けるダメージを防ぐのにかかわります。", 0 );
			}
			else
			if( id == 3 )
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"主に、戦闘時の行動の順番にかかわります。", 0 );
			}
		}
		else
		if( 10 <= id && id <= 17 )
		{
			id -= 10;
			id /= 2;
			if( id == 0 )
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"地属性。水属性の相手に対し戦闘で有利です。", 0 );
			}
			else
			if( id == 1 )
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"水属性。火属性の相手に対し戦闘で有利です。", 0 );
			}
			else
			if( id == 2 )
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"火属性。風属性の相手に対し戦闘で有利です。", 0 );
			}
			else
			if( id == 3 )
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"風属性。地属性の相手に対し戦闘で有利です。", 0 );
			}
		}
		else
		if( id == 18 || id == 19 )
		{
			if( nowSelCharGraNo != 0 && nowSelCharGraNo != 6 )
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"目の形を変更します。", 0 );
			}
			else
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"鼻の形を変更します。", 0 );
			}
		}
		else
		if( id == 20 || id == 21 )
		{
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"口の形を変更します。", 0 );
		}
		else
		{
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"名前、顔、ステータス、属性を設定してください。", 0 );
		}
	}



	StockFontBuffer2( &selCharName );

	// 顔グラフィック
	StockDispBuffer( 93, 172, DISP_PRIO_BG,
		CG_CHR_MAKE_FACE+nowSelCharGraNo*100+selCharColor[nowSelCharGraNo]*25
		+nowSelCharEyeNo*5+nowSelCharMouthNo,
			0 );

	// キャラのステータスに割り振れるポイント
	sprintf( msg, "%3d", nowSelCharStatusPoint );
	StockFontBuffer( 161, 224, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0 );

	// キャラのステータス
	for( i = 0; i < 4; i++ )
	{
		sprintf( msg, "%3d", nowSelCharStatus[i] );
		StockFontBuffer( statusLocate[i][0], statusLocate[i][1],
			FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0 );
	}


	// キャラの属性に割り振れるポイント
	sprintf( msg, "%3d", nowSelCharAttrPoint );
	StockFontBuffer( 350, 224, FONT_PRIO_BACK, FONT_PAL_WHITE, msg, 0 );


	// 属性のメータ
	for( i = 0; i < 4; i ++ )
	{
		if( nowSelCharAttr[i] > 0 )
		{
			x1 = attrLocate[i][0];
			y1 = attrLocate[i][1];
			x2 = x1 + nowSelCharAttr[i] * 8;
			y2 = y1 + 8;
			StockBoxDispBuffer( x1,   y1,   x2,   y2,   DISP_PRIO_IME2, attrColor[i][0], 1 );
			StockBoxDispBuffer( x1+1, y1+1, x2+1, y2+1, DISP_PRIO_IME1, attrColor[i][1], 0 );
			StockBoxDispBuffer( x1+2, y1+2, x2+2, y2+2, DISP_PRIO_IME1, attrColor[i][1], 0 );
		}
	}


	// キャラ作成終了
	graId[0] = StockDispBuffer(  96, 410, DISP_PRIO_CHAR, CG_CHR_MAKE_OK_BTN, selUseFlag );
	// キャラ選択へ戻る
	graId[1] = StockDispBuffer( 192, 410, DISP_PRIO_CHAR, CG_CHR_MAKE_BACK_BTN, selUseFlag );

	for( i = 0; i < 4; i++ )
	{
		btn1 = 0;
		btn2 = 0;
		if( 2 <= id3 && id3 <= 9
		 && (id3-2)/2 == i )
		{
			if( ((id3-2)%2) == 0 )
			{
				btn1 = 1;
			}
			else
			{
				btn2 = 1;
			}
		}
		// < ボタン
		graId[i*2+2] = StockDispBuffer( statusLocate[i][0]+51, statusLocate[i][1]+8,
						DISP_PRIO_CHAR, upDownBtnGraNo[0][btn1], selUseFlag );

		// > ボタン
		graId[i*2+3] = StockDispBuffer( statusLocate[i][0]+87, statusLocate[i][1]+8,
						DISP_PRIO_CHAR, upDownBtnGraNo[1][btn2], selUseFlag );
	}

	for( i = 0; i < 4; i++ )
	{
		if( i == 0 && nowSelCharAttr[i+2] == 0
		 || i == 1 && nowSelCharAttr[i+2] == 0
		 || i == 2 && nowSelCharAttr[i-2] == 0
		 || i == 3 && nowSelCharAttr[i-2] == 0 )
		{
			btn1 = 0;
			btn2 = 0;
			if( 10 <= id3 && id <= 17
			 && (id3-10)/2 == i )
			{
				if( ((id3-10)%2) == 0 )
				{
					btn1 = 1;
				}
				else
				{
					btn2 = 1;
				}
			}

			// < ボタン
			graId[i*2+10] = StockDispBuffer( attrLocate[i][0]+101, attrLocate[i][1]+5,
							DISP_PRIO_CHAR, upDownBtnGraNo[0][btn1], selUseFlag );

			// > ボタン
			graId[i*2+11] = StockDispBuffer( attrLocate[i][0]+137, attrLocate[i][1]+5,
							DISP_PRIO_CHAR, upDownBtnGraNo[1][btn2], selUseFlag );
		}
		else
		{
			graId[i*2+10] = -2;
			graId[i*2+11] = -2;
		}
	}

	for( i = 0; i < 2; i++ )
	{
		btn1 = 0;
		btn2 = 0;
		if( 18 <= id3 && id3 <= 21
		 && (id3-18)/2 == i )
		{
			if( ((id3-18)%2) == 0 )
			{
				btn1 = 1;
			}
			else
			{
				btn2 = 1;
			}
		}

		// < ボタン
		graId[i*2+18] =
			StockDispBuffer( 156, 172+i*24,
				DISP_PRIO_CHAR, upDownBtnGraNo[0][btn1], selUseFlag );
		// > ボタン
		graId[i*2+19] =
			StockDispBuffer( 260, 172+i*24,
				DISP_PRIO_CHAR, upDownBtnGraNo[1][btn2], selUseFlag );
	}

	if( nowSelCharGraNo != 0 && nowSelCharGraNo != 6 )
	{
		StockDispBuffer( 320, 240, DISP_PRIO_CHAR, CG_CHR_MAKE_EYE_SEL, 0 );
	}
	else
	{
		StockDispBuffer( 320, 240, DISP_PRIO_CHAR, CG_CHR_MAKE_NOSE_SEL, 0 );
	}

	// ＢＧ表示
	StockDispBuffer( 320, 240, DISP_PRIO_BG, CG_CHR_MAKE_BG, 0 );

	RunAction();			// アクション走らせる
	StockTaskDispBuffer();	// タスク表示データをバッファに溜める
	FlashKeyboardCursor();	// キーボードカーソル点滅処理
	ImeProc();				// ＩＭＥ関連処理

	return ret;
}




// 出身地の選択
// 初期化
void initSelHomeTown( void )
{
	selHomeTownProcNo = 0;
	nowSelHomeTownNo = 0;
}


// 出身地の選択処理
//
//  戻り値：	0 ... 処理中
//				1 ... 決定
//				2 ... キャンセル
//出生地的选择处理/ // /返回值：0...处理中/ /1...决定/ /2...取消
int selHomeTown( void )
{
	static int btnId[] = { -2, -2, -2, -2, -2 };
	static int id2;
	int id;
	int ret = 0;
	int ret2;
	char *homeTownName[] =
	{
		"サムギルの村",
		"マリナスの村",
		"ジャジャの村",
		"カルターナの村"
	};
	int selUseFlag = 0;
	int i;

	// 初期化
	if( selHomeTownProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}
		selHomeTownProcNo++;
	}

	// 出身地選択中
	if( selHomeTownProcNo == 1 )
	{
		selUseFlag = 2;
	}

	// 決定確認処理
	if( selHomeTownProcNo == 10 )
	{
		ret2 = commonYesNoWindow( 320, 240 );
		// はい
		if( ret2 == 1 )
		{
			ret = 1;
			selHomeTownProcNo = 11;	// ダミー
		}
		else
		// いいえ
		if( ret2 == 2 )
		{
			selHomeTownProcNo = 1;
		}
	}

	id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
	// キャンセル
	if( id == 0 )
	{
		ret = 2;
		play_se( 217, 320, 240 );	// クリック音
	}
	if( 1 <= id && id <= 4 )
	{
		nowSelHomeTownNo = id - 1;
		newCharacterHomeTown = nowSelHomeTownNo;
		selHomeTownProcNo = 10;
		play_se( 217, 320, 240 );	// クリック音
	}

	if( selHomeTownProcNo == 1 )
	{
		id = focusGraId( btnId, sizeof( btnId )/sizeof( int ) );
	}
	// サムギルの村の説明
#if 1
	if( id == 1 )
#else
	if( id2 == 1 )
#endif
	{
		StockFontBuffer( 444, 96,
			FONT_PRIO_BACK, FONT_PAL_YELLOW, homeTownName[0], 0 );
		StockFontBuffer( 386, 156,
			FONT_PRIO_BACK, FONT_PAL_BLUE, "サイナスの東にある村。", 0 );
		StockFontBuffer( 386, 176,
			FONT_PRIO_BACK, FONT_PAL_BLUE, "四つの村の中でも大きく、", 0 );
		StockFontBuffer( 386, 196,
			FONT_PRIO_BACK, FONT_PAL_BLUE, "きれいに整っている村。", 0 );
	}
	else
	// マリナスの村の説明
#if 1
	if( id == 2 )
#else
	if( id2 == 2 )
#endif
	{
		StockFontBuffer( 444, 96,
			FONT_PRIO_BACK, FONT_PAL_YELLOW, homeTownName[1], 0 );
		StockFontBuffer( 390, 166,
			FONT_PRIO_BACK, FONT_PAL_BLUE, "サイナスの西にある", 0 );
		StockFontBuffer( 390, 186,
			FONT_PRIO_BACK, FONT_PAL_BLUE, "きれいな浜辺に面した村。", 0 );
	}
	else
	// ジャジャの村の説明
	if( id == 3 )
	{
		StockFontBuffer( 444, 96,
			FONT_PRIO_BACK, FONT_PAL_YELLOW, homeTownName[2], 0 );
		StockFontBuffer( 390, 156,
			FONT_PRIO_BACK, FONT_PAL_BLUE, "ジャルガの東にあり、", 0 );
		StockFontBuffer( 390, 176,
			FONT_PRIO_BACK, FONT_PAL_BLUE, "四方を森に囲まれた", 0 );
		StockFontBuffer( 390, 196,
			FONT_PRIO_BACK, FONT_PAL_BLUE, "緑豊かな村。", 0 );
	}
	else
	// カルターナの村の説明
	if( id == 4 )
	{
		StockFontBuffer( 444, 96,
			FONT_PRIO_BACK, FONT_PAL_YELLOW, homeTownName[3], 0 );
		StockFontBuffer( 390, 156,
			FONT_PRIO_BACK, FONT_PAL_BLUE, "ジャルガの西にある村。", 0 );
		StockFontBuffer( 390, 176,
			FONT_PRIO_BACK, FONT_PAL_BLUE, "村はいくつもの小さな", 0 );
		StockFontBuffer( 390, 196,
			FONT_PRIO_BACK, FONT_PAL_BLUE, "島の上に存在する。", 0 );
	}
	else
	// キャンセルボタンの説明
	if( id == 0 )
	{
		StockFontBuffer( 390, 176,
			FONT_PRIO_BACK, FONT_PAL_BLUE, "前の画面に戻ります。", 0 );
	}
	else
	{
		StockFontBuffer( 390, 176,
			FONT_PRIO_BACK, FONT_PAL_BLUE, "出身地を選択してください。", 0 );
	}


	btnId[0] = StockDispBuffer( 412, 296, DISP_PRIO_CHAR, CG_CHR_MAKE_BACK_BTN, selUseFlag );

	btnId[1] = StockDispBuffer( 320, 240, DISP_PRIO_BG, CG_CHR_MAKE_HOME_NAME0, selUseFlag );
	btnId[2] = StockDispBuffer( 320, 240, DISP_PRIO_BG, CG_CHR_MAKE_HOME_NAME1, selUseFlag );
	btnId[3] = StockDispBuffer( 320, 240, DISP_PRIO_BG, CG_CHR_MAKE_HOME_NAME2, selUseFlag );
	btnId[4] = StockDispBuffer( 320, 240, DISP_PRIO_BG, CG_CHR_MAKE_HOME_NAME3, selUseFlag );

	// ＢＧ表示
	StockDispBuffer( 320, 240, DISP_PRIO_BG, CG_CHR_MAKE_SEL2_BG, 0 );

	return ret;
}




// 新キャラ作成処理
// 初期化
void initCreateChar( void )
{
	createCharProcNo = 0;
}


// キャラ作成処理
//  戻り値：	0 ... 処理中
//				1 ... 完了
//				2 ... 失敗
int createChar( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	int ret = 0;
	int ret2;
	char msg[] = "キャラクター作成中";

	// 初期化
	if( createCharProcNo == 0 )
	{
		createCharProcNo++;

		// ウィンドウ作成
		w = (strlen( msg )*9+63)/64;
		if( w < 2 )
			w = 2;
		h = 2;
		x = (640-w*64)/2;
		y = (480-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );
	}

	// 新キャラ作成開始
	if( createCharProcNo == 1 )
	{
		createNewCharStart();
		createCharProcNo++;
	}
	if( createCharProcNo == 2 )
	{
		ret2 = createNewCharProc();
		// 完了
		if( ret2 == 1 )
		{
			ret = 1;
		}
		else
		// 失敗
		if( ret2 < 0 )
		{
			ret = 2;
		}
	}


	// 結果がわかったらウィンドウ閉じる
	if( ret != 0 )
	{
		if( ptActMenuWin )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
		}
	}

	if( ptActMenuWin != NULL )
	{
		// ウィンドウ表示
		if( ptActMenuWin->hp >= 1 )
		{
			int len;
			int xx, yy;

			len = strlen( msg )/2;
			xx = (w*64-len*17)/2;
			yy = (h*48-16)/2;
			StockFontBuffer( x+xx, y+yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0 );
		}
	}

	return ret;
}




///////////////////////////////////////////////////////////////////////////
//
// キャラログイン
//

void initCharLogin( void );
int charLogin( void );

void characterLoginProc( void )
{
	int ret;
	static char msg[256];

	if( SubProcNo == 0 )
	{
		// ログイン処理初期化
		initCharLogin();
		SubProcNo++;
	}


	if( SubProcNo == 1 )
	{
		// ログイン処理
		ret = charLogin();
		// ログイン成功
		if( ret == 1 )
		{
			ChangeProc( PROC_GAME );
		}
		else
		// タイムアウト
		if( ret == -1 )
		{
			SubProcNo = 100;
			strcpy( msg, NET_ERRMSG_LOGINTIMEOUT );
		}
		else
		// ログイン失敗
		if( ret == -2 )
		{
			SubProcNo = 100;
			strcpy( msg, "ログイン処理に失敗しました。" );
		}
	}

	// エラー表示
	if( SubProcNo == 100 )
	{
		initCommonMsgWin();
		SubProcNo++;
	}
	if( SubProcNo == 101 )
	{
		if( commonMsgWin( msg ) )
		{
			// ＯＫボタンが押された
			ChangeProc( PROC_TITLE_MENU );	// タイトルへ
		}
	}


	RunAction();			// アクション走らせる
	StockTaskDispBuffer();	// タスク表示データをバッファに溜める
}


static short charLoginProcNo = 0;

// 初期化
void initCharLogin( void )
{
	charLoginProcNo = 0;
}

// メイン処理
//
//  戻り値：	 0 ... ログイン中
//				 1 ... ログイン完了
//				-1 ... タイムアウト
//				-2 ... エラー
int charLogin( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	int ret = 0;
	static char msg[] = "ログイン中";

	// 初期化
	if( charLoginProcNo == 0 )
	{
		charLoginProcNo = 1;

		// ウィンドウ作成
		w = strlen( msg )*9/64+2;
		h = (16+47)/48;
		if( h < 2 )
			h = 2;
		x = (640-w*64)/2;
		y = (480-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );
	}

	// キャラログイン開始
	if( charLoginProcNo == 1 )
	{
		charLoginStart();
		charLoginProcNo++;
	}
	else
	if( charLoginProcNo == 2 )
	{
		ret = charLoginProc();
	}

	if( ret != 0 )
	{
		if( ptActMenuWin )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
		}
	}

	if( ptActMenuWin != NULL )
	{
		// ウィンドウ表示
		if( ptActMenuWin->hp >= 1 )
		{
			int len;
			int xx, yy;

			len = strlen( msg );
			xx = (w*64-len*8)/2;
			yy = (h*48-16)/2;
			StockFontBuffer( x+xx, y+yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0 );
		}
	}

	return ret;
}




static int produce_logout(void);
static int produce_vct_no = 0;

///////////////////////////////////////////////////////////////////////////
//
// キャラログアウト
//

void initCharLogout( void );
int charLogout( void );

void characterLogoutProc( void )
{
	int ret;
	static char msg[256];

	if( SubProcNo == 0 )
	{
		logOutFlag = TRUE;

		if(!produce_logout()){		//演出中なら
			return;
		}
		//ＶＣＴ番号クリア
		produce_vct_no = 0;

		// PCリセット
		resetPc();

		// キャラ管理テーブルリセット
		initCharObj();

		// アクション抹殺
		DeathAllAction();

		// ＢＧＭストップ
//		fade_out_bgm();

#ifdef _DEBUG
		if( !offlineFlag )
		{
			SubProcNo++;
		}
		else
		{
			// ＷＩＮＤＯＷＳに WM_CLOSE メッセージを送らせる
			PostMessage( hWnd, WM_CLOSE, 0, 0L );
		}
#else
		SubProcNo++;
#endif
	}

	if( SubProcNo == 1 )
	{
		initCharLogout();
		SubProcNo++;
	}
	else
	if( SubProcNo == 2 )
	{
		ret = charLogout();
		// ログアウト完了
		if( ret == 1 )
		{
			// ネットワーク初期化
			cleanupNetwork();
			// アクション抹殺
			DeathAllAction();
			ChangeProc( PROC_TITLE_MENU );
		}
		else
		// タイムアウト
		if( ret == -1 )
		{
			// ネットワーク初期化
			cleanupNetwork();
			SubProcNo = 100;
			strcpy( msg, NET_ERRMSG_LOGOUTTIMEOUT );
		}
		else
		// エラー
		if( ret == -2 )
		{
			// ネットワーク初期化
			cleanupNetwork();
			SubProcNo = 100;
			strcpy( msg, "ログアウト処理に失敗しました。" );
		}
	}

	// エラー表示
	if( SubProcNo == 100 )
	{
		initCommonMsgWin();
		SubProcNo++;
	}
	if( SubProcNo == 101 )
	{
		if( commonMsgWin( msg ) )
		{
			// ＯＫボタンが押された
			ChangeProc( PROC_TITLE_MENU );	// タイトルへ
		}
	}

	RunAction();			// アクション走らせる
	StockTaskDispBuffer();	// タスク表示データをバッファに溜める
}


static int charLogoutProcNo;

// ログアウト処理初期化
void initCharLogout( void )
{
	charLogoutProcNo = 0;
}


// ログアウト処理
//  戻り値：	 0 ... ログアウト中
//				 1 ... ログイアウト完了
//				-1 ... タイムアウト
//				-2 ... エラー
/// /退出处理/ /返回值：0...退出中/ /1...罗gui出局完毕/ / - 1...超时...错误/ / - 2
int charLogout( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	int ret = 0;
	static char msg[] = "正在退出ing....";

	// 初期化
	if( charLogoutProcNo == 0 )
	{
		charLogoutProcNo = 1;

		// ウィンドウ作成
		w = strlen( msg )*9/64+2;
		h = (16+47)/48;
		if( h < 2 )
			h = 2;
		x = (640-w*64)/2;
		y = (480-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );
	}

	// キャラログイン開始
	if( charLogoutProcNo == 1 )
	{
		charLogoutStart();
		charLogoutProcNo++;
	}
	else
	if( charLogoutProcNo == 2 )
	{
		ret = charLogoutProc();
	}

	if( ret != 0 )
	{
		if( ptActMenuWin )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
		}
	}

	if( ptActMenuWin != NULL )
	{
		// ウィンドウ表示
		if( ptActMenuWin->hp >= 1 )
		{
			int len;
			int xx, yy;

			len = strlen( msg );
			xx = (w*64-len*8)/2;
			yy = (h*48-16)/2;
			StockFontBuffer( x+xx, y+yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0 );
		}
	}

	return ret;
}





///////////////////////////////////////////////////////////////////////////
// サーバリクエスト汎用ウィンドウ
//
//   メッセージ:	半角40文字 × 8行
//
// パラメータだけ取りだし実際はopenServerWindowProcで処理する
/// /服务器要求通用窗口/ /:半角文字信息/ /40×8行/ /只取
//参数。实际上openServerWindowProc处理
int windowTypeWN = -1;	// ウィンドウタイプ  -1で未使用
int buttonTypeWN = 0;	// ボタンタイプ(ビット情報)
int indexWN;			// シーケンス番号
int idWN;				// キャラのＩＤ
char msgWN[25][70];		// 表示メッセージ等
int msgWNLen;			// 表示メッセージの１行の長さ
int msgWN_W;			// メッセージウィンドウの幅
int msgWN_H;			// メッセージウィンドウの高さ
ACTION *ptActMenuWin = NULL;
int selStartLine;		// 選択肢ウィンドウで何行目からを選択肢にするか
int wnOpenX, wnOpenY;	// 汎用ウィンドウのを開いたときのキャラ座標
short wnCloseFlag = 0;	// 汎用ウィンドウを閉じるフラグ


#define MAX_SHOP_ITEM	8	// ショップウィンドウに表示できるアイテム数
#define MAX_SHOP_PAGE	4	// ショップウィンドウに表示できるページ数
short shopWindowMode;		// ショップウィンドウのモード（0...販売モード,1...買取モード）
char shopWindow1Title[27];	// ショップウィンドウ１タイトル
char shopWindow1Msg[27];	// ショップウィンドウ１メッセージ

typedef struct
{
	int level;
	int price;
	int graNo;
	short sealFlag;
	short tbl;
	char name[ITEM_NAME_LEN+1];
	char info[3][29];
} ITEM_SHOP;

int menuIndexWN;									// シーケンス番号(メニューのシーケンス番号)
char shopWindow2Msg[2][45];							// ショップウィンドウ２メッセージ
short shopWondow2Page, shopWondow2MaxPage;			// 現在ページと最大ページ
ITEM_SHOP sealItem[MAX_SHOP_ITEM*MAX_SHOP_PAGE];	// 販売アイテム
short selShopItemNo;								// 選択したアイテム

char shopWindow3Msg[41];
short sealItemCnt;

char shopWindow4Msg[2][39];							// ショップウィンドウ４メッセージ
char shopWindow5Msg[2][39];							// ショップウィンドウ５メッセージ
char shopWindow6Msg[2][39];							// ショップウィンドウ６メッセージ

short nowUserItemCnt;
char shopWindow7Msg[2][39];							// ショップウィンドウ７メッセージ
char shopWindow8Msg[2][39];							// ショップウィンドウ８メッセージ



ITEM_SHOP userItem[MAX_SHOP_ITEM*MAX_SHOP_PAGE];	// ユーザが持っているアイテム

short buyOffFlag;									// 買取専門か？


#define MAX_SKILL_SHOP_SKILL	8					// ショップウィンドウに表示できるスキル数
#define MAX_SKILL_SHOP_PAGE		2					// ショップウィンドウのページ数
typedef struct
{
	int price;
	char name[SKILL_NAME_LEN+1];
	char info[3][25];
} SKILL_SHOP;

SKILL_SHOP sealSkill[MAX_SKILL_SHOP_SKILL*MAX_SKILL_SHOP_PAGE];
short skillShopWindowProcNo;
short selShopSkillNo;
short selShopSkillPetNo;
short selShopSkillSlotNo;


void initServerWindowType0( char * );
void initServerWindowType1( char * );
void initServerWindowType2( char * );
void initServerWindowType3( char * );
void initServerWindowType4( char * );
void initServerWindowType5( char * );
void serverWindowType0( int );
void serverWindowType1( void );
void serverWindowType2( void );
void serverWindowType3( void );
void serverWindowType4( void );
void serverWindowType5( void );
void serverWindowType6( void );



// 初期化
void initOpenServerWindow( void )
{
	windowTypeWN = -1;
	ptActMenuWin = NULL;
}


void openServerWindow( int windowtype, int buttontype, int index, int id, char *data )
{
	if( windowTypeWN < 0 )
	{
		wnOpenX = nowGx;
		wnOpenY = nowGy;
		wnCloseFlag = 0;

		windowTypeWN = windowtype;
		buttonTypeWN = buttontype;
		indexWN = index;
		idWN = id;

		buyOffFlag = 0;

		// データの解析
		if( windowTypeWN == WINDOW_MESSAGETYPE_MESSAGE
		 || windowTypeWN == WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT )
		{
			// 初期化
			// メッセージのみ
			// メッセージ＆１行入力
			initServerWindowType0( data );
		}
		else
		if( windowTypeWN == WINDOW_MESSAGETYPE_SELECT )
		{
			// 選択
			initServerWindowType1( data );
		}
		else
		if( windowTypeWN == WINDOW_MESSAGETYPE_ITEMSHOPMENU )
		{
			// ショップ専用ウィンドウ
			initServerWindowType2( data );
		}
		else
		if( windowTypeWN == WINDOW_MWSSAGETYPE_ITEMSHOPMAIN )
		{
			// ショップ専用ウィンドウ
			initServerWindowType3( data );
		}
		else
		// 買取専門ウィンドウ
		if( windowTypeWN == WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN )
		{
			// ショップ専用ウィンドウ
			initServerWindowType3( data );
			buyOffFlag = 1;
		}
		else
		// ペットの技屋さんウィンドウ
		if( windowTypeWN == WINDOW_MESSAGETYPE_PETSKILLSHOP )
		{
			initServerWindowType5( data );
		}
		else
		// 掲示板とか大きいウィンドウ
		if( windowTypeWN == WINDOW_MESSAGETYPE_WIDEMESSAGE
		 || windowTypeWN == WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT )
		{
			// 初期化
			// メッセージのみ
			// メッセージ＆１行入力
			initServerWindowType4( data );
		}

		// 他のウィンドウが出てたら消す
		if( CheckMenuFlag() )
		{
			InitMenu2();
		}
		closeEtcSwitch();
		closeCharActionAnimeChange();
	}
}






// タイプ０ウィンドウ
// メッセージのみ（全角２０文字×８行）
// メッセージ（全角２０文字×７行）＆１行入力
void serverWindowType0( int mode )
{
	static int winX, winY;
	static int winW, winH;
	static int fontId[] = { -2, -2, -2, -2, -2, -2 };
	static int btnLoc[6][2];
	static int btnCnt;
	static int msgLine;
	static STR_BUFFER input;
	char *btnTitle[] =
	{
		" O  K ",
		"CANCEL",
		"は  い",
		"いいえ",
		"戻  る",
		"次  へ"
	};
	int id;
	int i, j;
	int mask;
	int btn;


	static int totalMsgLine;
	static int lineSkip;


	if( ptActMenuWin == NULL )
	{
		winW = msgWN_W;
		winH = msgWN_H;
		winX = (640 - winW*64)/2;
		winY = (456 - winH*48)/2;
		ptActMenuWin = MakeWindowDisp( winX, winY, winW, winH, NULL, 1 );

		lineSkip = 20;
		totalMsgLine = (winH*48-56)/lineSkip;

		for( i = 0; i < sizeof( fontId )/sizeof( int ); i++ )
		{
			fontId[i] = -2;
		}

		mask = 1;
		btnCnt = 0;
		for( i = 0; i < 6; i++, mask <<= 1 )
		{
			if( buttonTypeWN & mask )
			{
				btnCnt++;
				if( btnCnt == 4 )
					break;
			}
		}
		// ボタンがあったら表示座標を求める
		if( btnCnt > 0 )
		{
			int w;
			w = winW*64/(btnCnt+1);
			for( i = 0; i < btnCnt; i++ )
			{
				btnLoc[i][0] = w * (i+1) - 27;
				btnLoc[i][1] = ((winH*48-56)/totalMsgLine)*(totalMsgLine-1)
					+(winH*48-totalMsgLine*lineSkip)/2+8;
			}
		}

		// モード０（メッセージ表示のみ）
		if( mode == 0 )
		{
			msgLine = totalMsgLine - 1;
			input.buffer[0] = '\0';
		}
		else
		if( mode == 1 )
		{
			msgLine = totalMsgLine - 2;
			input.buffer[0] = '\0';
			input.cnt = 0;
			initStrBuffer( &input,
				winX+(msgWN_W*64-msgWNLen*9)/2,
					winY+((winH*48-56)/totalMsgLine)*(totalMsgLine-2)+(winH*48-totalMsgLine*lineSkip)/2,
						msgWNLen, FONT_PAL_WHITE, FONT_PRIO_FRONT );
			GetKeyInputFocus( &input );
		}

		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}


	if( ptActMenuWin != NULL )
	{
		// フォントの選択判定
		id = selFontId( fontId, sizeof( fontId )/sizeof( int ) );

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || ((joy_trg[ 0 ] & JOY_ESC) && ImeBufferBak2 == NULL)
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			windowTypeWN = -1;
			wnCloseFlag = 0;
		}

		// ＯＫ
		if( 0 <= id && id < 6 )
		{
			char msg[256];
			btn = 1;
			btn <<= id;

			makeSendString( input.buffer, msg, sizeof( msg )-1 );
			lssproto_WN_send( sockfd, nowGx, nowGy, indexWN, idWN, btn, msg ) ;

			windowTypeWN = -1;
		}

		if( id >= 0 )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			if( mode == 1 )
			{
				GetKeyInputFocus( &MyChatBuffer );
			}
			return;
		}


		if( ptActMenuWin != NULL && ptActMenuWin->hp >= 1 )
		{
			for( i = 0; i < msgLine; i++ )
			{
				if( strlen( msgWN[i] ) > 0 )
				{
					StockFontBuffer( winX+(msgWN_W*64-msgWNLen*9)/2,
						winY+((winH*48-56)/totalMsgLine)*i+(winH*48-totalMsgLine*lineSkip)/2,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[i], 0 );
				}
			}
			j = 0;
			for( i = 0, mask = 1; i < 6; i++, mask <<= 1 )
			{
				fontId[i] = -2;
				if( buttonTypeWN & mask )
				{
					if( j < btnCnt )
					{
						fontId[i] =
							StockFontBuffer( winX+btnLoc[j][0], winY+btnLoc[j][1],
								FONT_PRIO_FRONT, FONT_PAL_YELLOW, btnTitle[i], 2 );
						j++;
					}
				}
			}
			// 入力欄の表示
			if( mode == 1 )
			{
				StockFontBuffer2( &input );
			}
		}
	}
}


// タイプ１ウィンドウ
// タイトル、選択肢（全角２０文字×８行）
void serverWindowType1( void )
{
	static int winX, winY;
	static int winW, winH;
	static int btnId[] = { -2, -2, -2, -2, -2, -2 };
	static int btnLoc[6][2];
	static int btnCnt;
	static int msgLine;
	char *btnTitle[] =
	{
		" O  K ",
		"CANCEL",
		"は  い",
		"いいえ",
		"戻  る",
		"次  へ"
	};
	int id, id2;
	int i, j;
	int mask;
	int btn;
	int x1, y1, x2, y2;

	if( ptActMenuWin == NULL )
	{
		winW = 7;
		winH = 5;
		winX = (640 - winW*64)/2;
		winY = (456 - winH*48)/2;
		ptActMenuWin = MakeWindowDisp( winX, winY, winW, winH, NULL, 1 );

		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		mask = 1;
		btnCnt = 0;
		for( i = 0; i < 6; i++, mask <<= 1 )
		{
			if( buttonTypeWN & mask )
			{
				btnCnt++;
				if( btnCnt == 4 )
					break;
			}
		}
		// ボタンがあったら表示座標を求める
		if( btnCnt > 0 )
		{
			int w;
			w = winW*64/(btnCnt+1);
			for( i = 0; i < btnCnt; i++ )
			{
				btnLoc[i][0] = w * (i+1) - 27;
				btnLoc[i][1] = 196;
			}
		}

		msgLine = 8;

		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}


	if( ptActMenuWin != NULL )
	{
		// ボタンの選択判定
		id = selFontId( btnId, sizeof( btnId )/sizeof( int ) );

		id2 = -1;
		if( ptActMenuWin->hp >= 1 )	// ウィンドウ出るまで選択枠出さない
		{
			for( i = selStartLine; i <  msgLine; i++ )
			{
				// 文字列無いところには枠出さない
				if( strlen( msgWN[i] ) > 0 )
				{
					x1 = winX+41;
					y1 = winY+18+i*21;
					x2 = x1 + 366;
					y2 = y1 + 20;
					if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
					{
						if( mouse.onceState & MOUSE_LEFT_CRICK )
						{
							id2 = i - selStartLine + 1;
						}
					}
				}
			}
		}

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			windowTypeWN = -1;
			wnCloseFlag = 0;
		}

		if( 0 <= id && id < 6
		 || 0 <= id2 && id2 < 8
		 || id == 100 )
		{
			char data[256];
			char msg[256];
			if( id == 100 )
			{
				btn |= WINDOW_BUTTONTYPE_CANCEL;
			}
			else
			if( 0 <= id && id < 6 )
			{
				btn = 1;
				btn <<= id;
			}
			else
			{
				btn = 0;
			}
			if( id2 < 0 )
				id2 = 0;
			sprintf( data, "%d", id2 );
			makeSendString( data, msg, sizeof( msg )-1 );
			lssproto_WN_send( sockfd, nowGx, nowGy, indexWN, idWN, btn, msg ) ;

			windowTypeWN = -1;
		}

		if( id >= 0 || id2 >= 0 )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			return;
		}


		if( ptActMenuWin->hp >= 1 )
		{
			for( i = 0; i < msgLine; i++ )
			{
				if( strlen( msgWN[i] ) > 0 )
				{
					StockFontBuffer( winX+44, winY+21+i*21,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[i], 0 );
				}
			}
			j = 0;
			for( i = 0, mask = 1; i < 6; i++, mask <<= 1 )
			{
				btnId[i] = -2;
				if( buttonTypeWN & mask )
				{
					if( j < btnCnt )
					{
						btnId[i] =
							StockFontBuffer( winX+btnLoc[j][0], winY+btnLoc[j][1],
								FONT_PRIO_FRONT, FONT_PAL_YELLOW, btnTitle[i], 2 );
						j++;
					}
				}
			}
		}
	}
}


// タイプ２ウィンドウ
// ペット選択
void serverWindowType2( void )
{
	static int winX, winY;
	static int winW, winH;
	static int btnId[] = { -2, -2, -2, -2, -2, -2 };
	static int btnLoc[6][2];
	static int btnCnt;
	char *btnTitle[] =
	{
		" O  K ",
		"CANCEL",
		"は  い",
		"いいえ",
		"戻  る",
		"次  へ"
	};
	int id, id2;
	int i, j;
	int mask;
	int btn;
	int x1, y1, x2, y2;
	int len;

	if( ptActMenuWin == NULL )
	{
		winW = 7;
		winH = 5;
		winX = (640 - winW*64)/2;
		winY = (456 - winH*48)/2;
		ptActMenuWin = MakeWindowDisp( winX, winY, winW, winH, NULL, 1 );

		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		mask = 1;
		btnCnt = 0;
		for( i = 0; i < 6; i++, mask <<= 1 )
		{
			if( buttonTypeWN & mask )
			{
				btnCnt++;
				if( btnCnt == 4 )
					break;
			}
		}
		// ボタンがあったら表示座標を求める
		if( btnCnt > 0 )
		{
			int w;
			w = winW*64/(btnCnt+1);
			for( i = 0; i < btnCnt; i++ )
			{
				btnLoc[i][0] = w * (i+1) - 27;
				btnLoc[i][1] = 196;
			}
		}

		strcpy( msgWN[0], "＝＝＝　ペットを選択してください　＝＝＝" );

		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}


	if( ptActMenuWin != NULL )
	{
		// ボタンの選択判定
		id = selFontId( btnId, sizeof( btnId )/sizeof( int ) );

		id2 = -1;
		if( ptActMenuWin->hp >= 1 )	// ウィンドウ出るまで選択枠出さない
		{
			for( i = 1, j = 1; i <= MAX_PET; i++ )
			{
				// 文字列無いところには枠出さない
				if(pet[i-1].useFlag != 0 )
				{
					x1 = winX+41;
					y1 = winY+18+(j+1)*21;
					x2 = x1 + 366;
					y2 = y1 + 20;
					if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
					{
						if( mouse.onceState & MOUSE_LEFT_CRICK )
						{
							id2 = i;
						}
					}
					j++;
				}
			}
		}

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			windowTypeWN = -1;
			wnCloseFlag = 0;
		}

		if( 0 <= id && id < 6
		 || 0 <= id2 && id2 < 8 )
		{
			char data[256];
			char msg[256];
			if( 0 <= id && id < 6 )
			{
				btn = 1;
				btn <<= id;
			}
			else
			{
				btn = 0;
			}
			if( id2 < 0 )
				id2 = 0;
			sprintf( data, "%d", id2 );
			makeSendString( data, msg, sizeof( msg )-1 );
			lssproto_WN_send( sockfd, nowGx, nowGy, indexWN, idWN, btn, msg ) ;

			windowTypeWN = -1;
		}

		if( id >= 0 || id2 >= 0 )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			return;
		}


		if( ptActMenuWin != NULL && ptActMenuWin->hp >= 1 )
		{
			StockFontBuffer( winX+54, winY+21,
				FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[0], 0 );
			for( i = 1, j = 1; i <= MAX_PET; i++ )
			{
				len = strlen( pet[i-1].name );
				if( pet[i-1].useFlag != 0 )
				{
					len = strlen( pet[i-1].freeName );
					if( len > 0 )
					{
						strcpy( msgWN[1], pet[i-1].freeName );
					}
					else
					{
						len = strlen( pet[i-1].name );
						strcpy( msgWN[1], pet[i-1].name );
					}
#if 0
					StockFontBuffer( winX+152+(144-len*9)/2, winY+21+(j+1)*21,
#else
					StockFontBuffer( winX+62, winY+21+(j+1)*21,
#endif
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0 );

					sprintf( msgWN[1], "LV.%d", pet[i-1].level );
					StockFontBuffer( winX+260, winY+21+(j+1)*21,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0 );

					sprintf( msgWN[1], "MaxHP %d", pet[i-1].maxHp );
					StockFontBuffer( winX+314, winY+21+(j+1)*21,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0 );

					j++;
				}
			}
			j = 0;
			for( i = 0, mask = 1; i < 6; i++, mask <<= 1 )
			{
				btnId[i] = -2;
				if( buttonTypeWN & mask )
				{
					if( j < btnCnt )
					{
						btnId[i] =
							StockFontBuffer( winX+btnLoc[j][0], winY+btnLoc[j][1],
								FONT_PRIO_FRONT, FONT_PAL_YELLOW, btnTitle[i], 2 );
						j++;
					}
				}
			}
		}
	}
}


// タイプ３ウィンドウ
// 仲間選択
void serverWindowType3( void )
{
	static int winX, winY;
	static int winW, winH;
	static int btnId[] = { -2, -2, -2, -2, -2, -2 };
	static int btnLoc[6][2];
	static int btnCnt;
	char *btnTitle[] =
	{
		" O  K ",
		"CANCEL",
		"は  い",
		"いいえ",
		"戻  る",
		"次  へ"
	};
	int id, id2;
	int i, j;
	int mask;
	int btn;
	int x1, y1, x2, y2;
	int len;

	if( ptActMenuWin == NULL )
	{
		winW = 7;
		winH = 5;
		winX = (640 - winW*64)/2;
		winY = (456 - winH*48)/2;
		ptActMenuWin = MakeWindowDisp( winX, winY, winW, winH, NULL, 1 );

		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		mask = 1;
		btnCnt = 0;
		for( i = 0; i < 6; i++, mask <<= 1 )
		{
			if( buttonTypeWN & mask )
			{
				btnCnt++;
				if( btnCnt == 4 )
					break;
			}
		}
		// ボタンがあったら表示座標を求める
		if( btnCnt > 0 )
		{
			int w;
			w = winW*64/(btnCnt+1);
			for( i = 0; i < btnCnt; i++ )
			{
				btnLoc[i][0] = w * (i+1) - 27;
				btnLoc[i][1] = 196;
			}
		}

		strcpy( msgWN[0], "＝＝＝　仲間を選択してください　＝＝＝" );

		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}


	if( ptActMenuWin != NULL )
	{
		// ボタンの選択判定
		id = selFontId( btnId, sizeof( btnId )/sizeof( int ) );

		id2 = -1;
		if( ptActMenuWin->hp >= 1 )	// ウィンドウ出るまで選択枠出さない
		{
			for( i = 1; i <= MAX_PARTY; i++ )
			{
				// 文字列無いところには枠出さない
				if( partyModeFlag == 0 && i == 1 )
				{
					strcpy( msgWN[1], pc.name );
				}
				else
				if( party[i-1].useFlag != 0 )
				{
						strcpy( msgWN[1], party[i-1].name );
				}
				else
				{
					msgWN[1][0] = '\0';
				}
				len = strlen( msgWN[1] );
				if( len > 0 )
				{
					x1 = winX+41;
					y1 = winY+18+(i+1)*21;
					x2 = x1 + 366;
					y2 = y1 + 20;
					if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
					{
						if( mouse.onceState & MOUSE_LEFT_CRICK )
						{
							id2 = i + 5;
						}
					}
				}
			}
		}

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			windowTypeWN = -1;
			wnCloseFlag = 0;
		}

		if( 0 <= id && id < 6
		 || 0 <= id2 && id2 < 8 )
		{
			char data[256];
			char msg[256];
			if( 0 <= id && id < 6 )
			{
				btn = 1;
				btn <<= id;
			}
			else
			{
				btn = 0;
			}
			if( id2 < 0 )
				id2 = 0;
			sprintf( data, "%d", id2 );
			makeSendString( data, msg, sizeof( msg )-1 );
			lssproto_WN_send( sockfd, nowGx, nowGy, indexWN, idWN, btn, msg ) ;

			windowTypeWN = -1;
		}

		if( id >= 0 || id2 >= 0 )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			return;
		}


		if( ptActMenuWin != NULL && ptActMenuWin->hp >= 1 )
		{
			StockFontBuffer( winX+62, winY+21,
				FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[0], 0 );
			for( i = 1; i <= MAX_PARTY; i++ )
			{
				if( partyModeFlag == 0 && i == 1 )
				{
					strcpy( msgWN[1], pc.name );
				}
				else
				if( party[i-1].useFlag != 0 )
				{
					strcpy( msgWN[1], party[i-1].name );
				}
				else
				{
					msgWN[1][0] = '\0';
				}
				len = strlen( msgWN[1] );
				if( len > 0 )
				{
					StockFontBuffer( winX+152+(144-len*9)/2, winY+21+(i+1)*21,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0 );
				}
			}
			j = 0;
			for( i = 0, mask = 1; i < 6; i++, mask <<= 1 )
			{
				btnId[i] = -2;
				if( buttonTypeWN & mask )
				{
					if( j < btnCnt )
					{
						btnId[i] =
							StockFontBuffer( winX+btnLoc[j][0], winY+btnLoc[j][1],
								FONT_PRIO_FRONT, FONT_PAL_YELLOW, btnTitle[i], 2 );
						j++;
					}
				}
			}
		}
	}
}


// タイプ４ウィンドウ
// ペット＆仲間選択
void serverWindowType4( void )
{
	static int winX, winY;
	static int winW, winH;
	static int btnId[] = { -2, -2, -2, -2, -2, -2 };
	static int btnLoc[6][2];
	static int btnCnt;
	char *btnTitle[] =
	{
		" O  K ",
		"CANCEL",
		"は  い",
		"いいえ",
		"戻  る",
		"次  へ"
	};
	int id, id2;
	int i, j;
	int mask;
	int btn;
	int x1, y1, x2, y2;
	int len;

	if( ptActMenuWin == NULL )
	{
		winW = 7;
		winH = 5;
		winX = (640 - winW*64)/2;
		winY = (456 - winH*48)/2;
		ptActMenuWin = MakeWindowDisp( winX, winY, winW, winH, NULL, 1 );

		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		mask = 1;
		btnCnt = 0;
		for( i = 0; i < 6; i++, mask <<= 1 )
		{
			if( buttonTypeWN & mask )
			{
				btnCnt++;
				if( btnCnt == 4 )
					break;
			}
		}
		// ボタンがあったら表示座標を求める
		if( btnCnt > 0 )
		{
			int w;
			w = winW*64/(btnCnt+1);
			for( i = 0; i < btnCnt; i++ )
			{
				btnLoc[i][0] = w * (i+1) - 27;
				btnLoc[i][1] = 196;
			}
		}

		strcpy( msgWN[0], "＝　ペットか仲間を選択してください　＝" );

		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}


	if( ptActMenuWin != NULL )
	{
		// ボタンの選択判定
		id = selFontId( btnId, sizeof( btnId )/sizeof( int ) );

		id2 = -1;
		if( ptActMenuWin->hp >= 1 )	// ウィンドウ出るまで選択枠出さない
		{
			for( i = 1; i <= MAX_PET; i++ )
			{
				// 文字列無いところには枠出さない
				if(pet[i-1].useFlag != 0 )
				{
					x1 = winX+41;
						y1 = winY+18+(i+1)*21;
					x2 = x1 + 149;
					y2 = y1 + 20;
					if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
					{
						if( mouse.onceState & MOUSE_LEFT_CRICK )
						{
							id2 = i;
						}
					}
				}
			}
		}

		for( i = 1; i <= MAX_PARTY; i++ )
		{
			// 文字列無いところには枠出さない
			if( partyModeFlag == 0 && i == 1 )
			{
				strcpy( msgWN[1], pc.name );
			}
			else
			if( party[i-1].useFlag != 0 )
			{
				strcpy( msgWN[1], party[i-1].name );
			}
			else
			{
				msgWN[1][0] = '\0';
			}
			len = strlen( msgWN[1] );
			if( len > 0 )
			{
				x1 = winX+257;
				y1 = winY+18+(i+1)*21;
				x2 = x1 + 149;
				y2 = y1 + 20;
				if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
				{
					if( mouse.onceState & MOUSE_LEFT_CRICK )
					{
						id2 = i + 5;
					}
				}
			}
		}

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			windowTypeWN = -1;
			wnCloseFlag = 0;
		}

		if( 0 <= id && id < 6
		 || 0 <= id2 && id2 < 10 )
		{
			char data[256];
			char msg[256];
			if( 0 <= id && id < 6 )
			{
				btn = 1;
				btn <<= id;
			}
			else
			{
				btn = 0;
			}
			if( id2 < 0 )
				id2 = 0;
			sprintf( data, "%d", id2 );
			makeSendString( data, msg, sizeof( msg )-1 );
			lssproto_WN_send( sockfd, nowGx, nowGy, indexWN, idWN, btn, msg ) ;

			windowTypeWN = -1;
		}

		if( id >= 0 || id2 >= 0 )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			return;
		}


		if( ptActMenuWin != NULL && ptActMenuWin->hp >= 1 )
		{
			StockFontBuffer( winX+62, winY+21,
				FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[0], 0 );

			// ペット表示
			for( i = 1; i <= MAX_PET; i++ )
			{
				len = strlen( pet[i-1].name );
				if( pet[i-1].useFlag != 0 )
				{
					len = strlen( pet[i-1].freeName );
					if( len > 0 )
					{
						strcpy( msgWN[1], pet[i-1].freeName );
					}
					else
					{
						len = strlen( pet[i-1].name );
						strcpy( msgWN[1], pet[i-1].name );
					}
					StockFontBuffer( winX+44+(144-len*9)/2, winY+21+(i+1)*21,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0 );
				}
			}

			// パーティ表示
			for( i = 1; i <= MAX_PARTY; i++ )
			{
				if( partyModeFlag == 0 && i == 1 )
				{
					strcpy( msgWN[1], pc.name );
				}
				else
				if( party[i-1].useFlag != 0 )
				{
					strcpy( msgWN[1], party[i-1].name );
				}
				else
				{
					msgWN[1][0] = '\0';
				}
				len = strlen( msgWN[1] );
				if( len > 0 )
				{
					StockFontBuffer( winX+260+(144-len*9)/2, winY+21+(i+1)*21,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0 );
				}
			}
			j = 0;
			for( i = 0, mask = 1; i < 6; i++, mask <<= 1 )
			{
				btnId[i] = -2;
				if( buttonTypeWN & mask )
				{
					if( j < btnCnt )
					{
						btnId[i] =
							StockFontBuffer( winX+btnLoc[j][0], winY+btnLoc[j][1],
								FONT_PRIO_FRONT, FONT_PAL_YELLOW, btnTitle[i], 2 );
						j++;
					}
				}
			}
		}
	}
}



// タイプ５ウィンドウ

void initShopWindow1( void );
int shopWindow1( void );
void initShopWindow2( void );
int shopWindow2( void );
void initShopWindow3( void );
int shopWindow3( void );
void initShopWindow4( void );
int shopWindow4( void );
void initShopWindow5( void );
int shopWindow5( void );
void initShopWindow6( void );
int shopWindow6( void );
void initShopWindow7( void );
int shopWindow7( void );
void initShopWindow8( void );
int shopWindow8( void );


// ショップ専用処理
void serverWindowType5( void )
{
	int ret;

	// 店メニュー
	if( shopWindowProcNo == 0 )
	{
		initShopWindow1();
		shopWindowProcNo++;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( shopWindowProcNo == 1 )
	{
		ret = shopWindow1();
	}

	// 店メイン
	if( shopWindowProcNo == 15 )
	{
		initShopWindow2();
		shopWindowProcNo = 11;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( shopWindowProcNo == 10 )
	{
		int i, j;

		// 所持アイテム数計算
		for( i = 5, j= 0; i < MAX_ITEM; i++ )
		{
			if( pc.item[i].useFlag == 0 )
				j++;
		}
		nowUserItemCnt = j;
	
		initShopWindow2();
		shopWindowProcNo++;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( shopWindowProcNo == 11 )
	{
		ret = shopWindow2();
		// 戻るが押されたら店メニューに戻る
		if( ret == 1 )
		{
#if 0
			windowTypeWN = WINDOW_MESSAGETYPE_ITEMSHOPMENU;
			shopWindowProcNo = 0;
#endif
#if 1
			char data[256];
			char msg[256];
			sprintf( data, "0" );
			makeSendString( data, msg, sizeof( msg )-1 );
			lssproto_WN_send( sockfd, nowGx, nowGy, indexWN, idWN, 0, msg ) ;
			windowTypeWN = -1;
#endif
		}
		else
		if( ret == 2 )
		{
			shopWindowProcNo = 30;
		}
		else
		if( ret == 3 )
		{
			shopWindowProcNo = 20;
		}
		if( ret == 4 )
		{
			shopWindowProcNo = 50;
		}
	}

	// アイテムレベル確認
	if( shopWindowProcNo == 20 )
	{
		initShopWindow4();
		shopWindowProcNo++;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( shopWindowProcNo == 21 )
	{
		ret = shopWindow4();
		if( ret == 1 )
		{
			shopWindowProcNo = 30;
		}
		else
		if( ret == 2 )
		{
			shopWindowProcNo = 15;
		}
	}

	// アイテム個数入力
	if( shopWindowProcNo == 30 )
	{
		initShopWindow3();
		shopWindowProcNo++;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( shopWindowProcNo == 31 )
	{
		ret = shopWindow3();
		if( ret == 1 )
		{
			shopWindowProcNo = 40;
		}
		else
		if( ret == 2 )
		{
			shopWindowProcNo = 15;
		}
	}

	// 買っていいか確認
	if( shopWindowProcNo == 40 )
	{
		initShopWindow5();
		shopWindowProcNo++;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( shopWindowProcNo == 41 )
	{
		ret = shopWindow5();
		if( ret == 1 )
		{
#if 1
			char data[256];
			char msg[256];
			sprintf( data, "%d|%d", selShopItemNo+1, sealItemCnt );
			makeSendString( data, msg, sizeof( msg )-1 );
			lssproto_WN_send( sockfd, nowGx, nowGy, indexWN, idWN, 0, msg ) ;
#endif
			nowUserItemCnt--;
			pc.gold -= sealItem[selShopItemNo].price*sealItemCnt;

			shopWindowProcNo = 15;
		}
		else
		if( ret == 2 )
		{
			shopWindowProcNo = 30;
		}
	}

	// アイテム一杯
	if( shopWindowProcNo == 50 )
	{
		initShopWindow6();
		shopWindowProcNo++;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( shopWindowProcNo == 51 )
	{
		ret = shopWindow6();
		if( ret == 1 )
		{
			shopWindowProcNo = 15;
		}
	}


	// 買い取りモード
	if( shopWindowProcNo == 100 )
	{
		initShopWindow7();
		shopWindowProcNo++;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( shopWindowProcNo == 101 )
	{
		ret = shopWindow7();
		if( ret == 1 )
		{
#if 1
			char data[256];
			char msg[256];
			sprintf( data, "0" );
			makeSendString( data, msg, sizeof( msg )-1 );
			lssproto_WN_send( sockfd, nowGx, nowGy, indexWN, idWN, 0, msg ) ;
			windowTypeWN = -1;
#endif
#if 0
			if( buyOffFlag == 0 )
			{
				windowTypeWN = WINDOW_MESSAGETYPE_ITEMSHOPMENU;
				shopWindowProcNo = 0;
			}
#endif
		}
		else
		if( ret == 2 )
		{
			shopWindowProcNo = 110;
		}
		else
		if( ret == 3 )
		{
			shopWindowProcNo = 120;
		}
	}

	// 売ってもいいか確認
	if( shopWindowProcNo == 110 )
	{
		initShopWindow5();
		shopWindowProcNo++;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( shopWindowProcNo == 111 )
	{
		ret = shopWindow5();
		if( ret == 1 )
		{
#if 1
			char data[256];
			char msg[256];
			sprintf( data, "%d|%d", userItem[selShopItemNo].tbl, userItem[selShopItemNo].price );
			makeSendString( data, msg, sizeof( msg )-1 );
			lssproto_WN_send( sockfd, nowGx, nowGy, indexWN, idWN, 0, msg ) ;
#endif
			// 売ったアイテムを項目から消す
			userItem[selShopItemNo].name[0] = '\0';
			// 売ったアイテムの値段分を財布に入れる
			pc.gold += userItem[selShopItemNo].price;
			shopWindowProcNo = 100;
#if 1
			// 空いてる空白欄を詰める
			int i, j, k;
			for( i = 0; i < MAX_SHOP_ITEM*MAX_SHOP_PAGE-1; i++ )
			{
				if( userItem[i].name[0] == '\0' )
				{
					for( j = i+1; j < MAX_SHOP_ITEM*MAX_SHOP_PAGE; j++ )
					{
						if( userItem[j].name[0] != '\0' )
						{
							strcpy( userItem[i].name, userItem[j].name );
							userItem[i].sealFlag = userItem[j].sealFlag;
							userItem[i].level = userItem[j].level;
							userItem[i].price = userItem[j].price;
							userItem[i].graNo = userItem[j].graNo;
							userItem[i].tbl = userItem[j].tbl;
							for( k = 0; k < 3; k++ )
							{
								strcpy( userItem[i].info[k], userItem[j].info[k] );
							}
							userItem[j].name[0] = '\0';
							break;
						}
					}
				}
			}
			for( i = 0; i < MAX_SHOP_ITEM*MAX_SHOP_PAGE; i++ )
			{
				if( userItem[i].name[0] == '\0' )
					break;
			}
			if( i > 0 )
			{
				shopWondow2MaxPage = (i+7)/MAX_SHOP_ITEM;
			}
			else
			{
				shopWondow2MaxPage = 1;
			}
			if( shopWondow2Page >= shopWondow2MaxPage )
				shopWondow2Page = shopWondow2MaxPage-1;
#endif
		}
		else
		if( ret == 2 )
		{
			shopWindowProcNo = 100;
		}
	}

	// 所持金が一杯になるよ
	if( shopWindowProcNo == 120 )
	{
		initShopWindow8();
		shopWindowProcNo++;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( shopWindowProcNo == 121 )
	{
		ret = shopWindow8();
		if( ret == 1 )
		{
			shopWindowProcNo = 100;
		}
	}
}


// ショップウィンドウ１

short shopWindow1ProcNo;

// 初期化
void initShopWindow1( void )
{
	shopWindow1ProcNo = 0;
}


// メイン
//   戻り値：0 ... 処理中
//           1 ... 買う
//           2 ... 売る
//           3 ... 出る
int shopWindow1( void )
{
	static int x, y, w, h;
	static int btnId[3];
	int id = 0;
	int i;
	int ret = 0;

	if( shopWindow1ProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 4;
		h = 3;
		x = (640-w*64)/2;
		y = (456-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );

		shopWindow1ProcNo++;
	}


	if( ptActMenuWin != NULL )
	{
		id = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンの選択判定
			id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
			ret = id + 1;
		}

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			ret = 3;	// 終わり
			wnCloseFlag = 0;
		}

		if( 0 <= id && id <= 2 )
		{
#if 1
			char data[256];
			char msg[256];
			sprintf( data, "%d", ret );
			makeSendString( data, msg, sizeof( msg )-1 );
			lssproto_WN_send( sockfd, nowGx, nowGy, menuIndexWN, idWN, 0, msg ) ;
#endif
		}

		if( id >= 0 )
		{
			if( ret == 3 )
			{
				play_se( 203, 320, 240 );	// ウィンドウ閉じる音
			}
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			windowTypeWN = -1;
			return ret;
		}

		if( ptActMenuWin->hp >= 1 )
		{
			int xx;

			xx = (w*64-strlen( shopWindow1Title )/2*17)/2;
			StockFontBuffer( x+xx, y+12, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Title, 0 );

			xx = (w*64-strlen( shopWindow1Msg )/2*17)/2;
			StockFontBuffer( x+xx, y+32, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Msg, 0 );

			btnId[0] =
				StockDispBuffer( x+w*64/2, y+16/2+66, DISP_PRIO_IME3, CG_BUY_BTN, 2 );
			btnId[1] =
				StockDispBuffer( x+w*64/2, y+16/2+90, DISP_PRIO_IME3, CG_SEAL_BTN, 2 );
			btnId[2] =
				StockDispBuffer( x+w*64/2, y+16/2+114, DISP_PRIO_IME3, CG_EXIT_BTN, 2 );
		}
	}

	return ret;
}




// ショップウィンドウ２

short shopWindow2ProcNo;

// 初期化
void initShopWindow2( void )
{
	shopWindow2ProcNo = 0;
}


// メイン
//   戻り値：0 ... 処理中
//           1 ... 戻る
//           2 ... 決定
//           3 ... レベル足りないアイテムだけど決定
//           4 ... アイテム一杯
int shopWindow2( void )
{
	static int x, y, w, h;
	static int btnId[3];
	int id;
	int pushId;
	int focusId;
	int selId;
	int i, j;
	int ret = 0;
	char tmsg[256];
	int prevBtnGraNo[] = { CG_PREV_BTN, CG_PREV_BTN_DOWN };
	int nextBtnGraNo[] = { CG_NEXT_BTN, CG_NEXT_BTN_DOWN };
	int itemColor[] = { FONT_PAL_WHITE, FONT_PAL_GRAY, FONT_PAL_RED };
	int color;
	int prevBtn, nextBtn;
	int x1, y1, x2, y2;

	if( shopWindow2ProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 432;
		h = 428;
		x = (640-w)/2;
		y = (456-h)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, -1 );

		shopWindow2ProcNo++;
	}


	if( ptActMenuWin != NULL )
	{
		id = -1;
		pushId = -1;
		focusId = -1;
		selId = -1;

		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンの選択判定
			//id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
			id = selRepGraId( btnId, sizeof( btnId )/sizeof( int ) );
			pushId = pushGraId( btnId, sizeof( btnId )/sizeof( int ) );

			j = shopWondow2Page*MAX_SHOP_ITEM;
			for( i = 0; i < MAX_SHOP_ITEM; i++ )
			{
				if( sealItem[j+i].name[0] != '\0' )
				{
					x1 = x+32;
					y1 = y+i*21+116;
					x2 = x1 + 372;
					y2 = y1 + 20;
					if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
					{
						if( mouse.onceState & MOUSE_LEFT_CRICK )
						{
							// 所持金が足りないものは選べない
							if( sealItem[j+i].price <= pc.gold
							 && sealItem[j+i].sealFlag == 0 )
							{
								selId = j+i;
							}
							else
							{
								play_se( 220, 320, 240 );	// ＮＧ音
							}
						}
						focusId = j+i;
					}
				}
			}
		}


		// 前ページボタン
		if( joy_con[0] & JOY_A )
		{
			pushId = 0;
			if( joy_auto[0] & JOY_A )
			{
				id = 0;
			}
		}
		if( pushId == 0 )
		{
			prevBtn = 1;
			if( id == 0 )
			{
				if( shopWondow2Page > 0 )
				{
					shopWondow2Page--;
					play_se( 217, 320, 240 );	// クリック音
				}
			}
		}
		else
		{
			prevBtn = 0;
		}
		// 次ページボタン
		if( joy_con[0] & JOY_B )
		{
			pushId = 1;
			if( joy_auto[0] & JOY_B )
			{
				id = 1;
			}
		}
		if( pushId == 1 )
		{
			nextBtn = 1;
			if( id == 1 )
			{
				if( shopWondow2Page+1 < shopWondow2MaxPage )
				{
					shopWondow2Page++;
					play_se( 217, 320, 240 );	// クリック音
				}
			}
		}
		else
		{
			nextBtn = 0;
		}


		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			wnCloseFlag = 0;
		}

		// id = 0, id = 1 はページボタンなので終わらない
		if( id >= 2
		 || selId >= 0 )
		{
			// 戻るボタン
			if( id == 2 )
			{
				ret = 1;
			}
			else
			if( id == 100 )
			{
				ret = 100;
			}
			else
			{
				selShopItemNo = selId;
				sealItemCnt = 1;
				if( nowUserItemCnt <= 0 )
				{
					ret = 4;
				}
				else
				if( sealItem[selId].level > pc.level )
				{
					ret = 3;
				}
				else
				{
					ret = 2;
				}
			}

			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			if( ret == 100 )
			{
				windowTypeWN = -1;
			}
			return ret;
		}

		if( ptActMenuWin->hp >= 1 )
		{
			int xx;

			// ショップタイトル
			xx = (w-strlen( shopWindow1Title )/2*17)/2;
			StockFontBuffer( x+xx, y+18, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Title, 0 );

			// ショップメッセージ
			for( i = 0; i < sizeof( shopWindow2Msg )/sizeof( shopWindow2Msg[0] ); i++ )
			{
				if( shopWindow2Msg[0] != '\0' )
				{
					StockFontBuffer( x+18, y+48+i*18, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						shopWindow2Msg[i], 0 );
				}
			}


			// 所持金
			sprintf( tmsg, "所持金  %8dＳ", pc.gold );
			StockFontBuffer( x+244, y+84, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0 );

			// ページ切り替え
			btnId[0] = StockDispBuffer( x +32, y+100, DISP_PRIO_IME3, prevBtnGraNo[prevBtn], 2 );
			btnId[1] = StockDispBuffer( x+200, y+100, DISP_PRIO_IME3, nextBtnGraNo[nextBtn], 2 );
			sprintf( tmsg, "%2d/%2d ページ", shopWondow2Page+1, shopWondow2MaxPage );
			StockFontBuffer( x +66, y+92, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0 );

			j = shopWondow2Page*MAX_SHOP_ITEM;
			for( i = 0; i < MAX_SHOP_ITEM; i++ )
			{
				// データが無ければスキップ
				if( strlen( sealItem[j+i].name ) == 0 )
					continue;

				// デフォルト、白
				color = itemColor[0];
				// レベルが足りない時は、赤
				if( sealItem[j+i].level > pc.level )
					color = itemColor[2];
				// 売らないフラグがたってたら、灰
				if( sealItem[j+i].sealFlag )
					color = itemColor[1];
				// お金が無い時も、灰
				if( sealItem[j+i].price > pc.gold )
				{
					color = itemColor[1];
				}

				// アイテム名
				StockFontBuffer( x+34, y+118+i*21,
					FONT_PRIO_FRONT, color, sealItem[j+i].name, 0 );
				// 金額
				sprintf( tmsg, "%8dＳ", sealItem[j+i].price );
				StockFontBuffer( x+312, y+118+i*21,
					FONT_PRIO_FRONT, color, tmsg, 0 );
			}

			if( focusId >= 0 )
			{
				// アイテム画像
				StockDispBuffer( x+64, y+350, DISP_PRIO_IME3, sealItem[focusId].graNo, 0 );

				for( i = 0; i < sizeof( sealItem[0].info )/sizeof( sealItem[0].info[0] ); i++ )
				{
					// 説明
					StockFontBuffer( x+120, y+308+i*20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						sealItem[focusId].info[i], 0 );
				}
			}

			// 戻るボタン
			btnId[2] = StockDispBuffer( x+216, y+402, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );

			// ウィンドウ
			StockDispBuffer( x+w/2, y+h/2, DISP_PRIO_MENU, CG_ITEMSHOP_WIN, 1 );
		}
	}

	return ret;
}


// ショップウィンドウ３
//   まとめ買いウィンドウ

short shopWindow3ProcNo;

// 初期化
void initShopWindow3( void )
{
	shopWindow3ProcNo = 0;
}


// メイン
//   戻り値：0 ... 処理中
//           1 ... 決定
//           2 ... 戻る
int shopWindow3( void )
{
	static int x, y, w, h;
	static int btnId[4];
	int upBtnGraNo[] = { CG_UP_BTN, CG_UP_BTN_DOWN };
	int downBtnGraNo[] = { CG_DOWN_BTN, CG_DOWN_BTN_DOWN };
	int upBtn, downBtn;
	int id;
	int pushId;
	int i;
	int ret = 0;
	char tmsg[256];

	if( shopWindow3ProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 440;
		h = 160;
		x = (640-w)/2;
		y = (456-h)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, -1 );

		shopWindow3ProcNo++;
	}


	if( ptActMenuWin != NULL )
	{
		id = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンの選択判定
			//id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
			id = selRepGraId( btnId, sizeof( btnId )/sizeof( int ) );
			pushId = pushGraId( btnId, sizeof( btnId )/sizeof( int ) );
		}

		// ダウンボタン
		if( joy_con[0] & JOY_A )
		{
			pushId = 0;
			if( joy_auto[0] & JOY_A )
			{
				id = 0;
			}
		}
		if( pushId == 0 )
		{
			downBtn = 1;
			if( id == 0 )
			{
				if( sealItemCnt > 1 )
				{
					sealItemCnt--;
					play_se( 217, 320, 240 );	// クリック音
				}
			}
		}
		else
		{
			downBtn = 0;
		}
		// アップボタン
		if( joy_con[0] & JOY_B )
		{
			pushId = 1;
			if( joy_auto[0] & JOY_B )
			{
				id = 1;
			}
		}
		if( pushId == 1 )
		{
			upBtn = 1;
			if( id == 1 )
			{
				if( sealItemCnt+1 <= nowUserItemCnt
				 && sealItem[selShopItemNo].price*(sealItemCnt+1) <= pc.gold )
				{
					sealItemCnt++;
					play_se( 217, 320, 240 );	// クリック音
				}
			}
		}
		else
		{
			upBtn = 0;
		}

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			wnCloseFlag = 0;
		}


		if( id >= 2 )
		{
			if( id == 2 )
			{
				ret = 1;
			}
			else
			if( id == 3 )
			{
				ret = 2;
			}
			else
			if( id == 100 )
			{
				ret = 100;
			}

			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			if( ret == 100 )
			{
				windowTypeWN = -1;
			}
			return ret;
		}


		if( ptActMenuWin->hp >= 1 )
		{
			int xx;

			xx = (w-strlen( shopWindow3Msg )/2*17)/2;
			StockFontBuffer( x+xx, y+20, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow3Msg, 0 );

			StockFontBuffer( x+16, y+52, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				sealItem[selShopItemNo].name, 0 );

			sprintf( tmsg, "%2d個", sealItemCnt );
			StockFontBuffer( x+300, y+52, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0 );


			sprintf( tmsg, "%8dＳ x %2d = %8dＳ",
				sealItem[selShopItemNo].price,
				sealItemCnt, sealItem[selShopItemNo].price*sealItemCnt );
			StockFontBuffer( x+146, y+82, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				tmsg, 0 );

			sprintf( tmsg, "所持金 %8dＳ", pc.gold );
			StockFontBuffer( x+248, y+120, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0 );

			btnId[0] = StockDispBuffer( x+364, y+60, DISP_PRIO_IME3, downBtnGraNo[downBtn], 2 );
			btnId[1] = StockDispBuffer( x+404, y+60, DISP_PRIO_IME3, upBtnGraNo[upBtn], 2 );

			btnId[2] = StockDispBuffer( x +64, y+120, DISP_PRIO_IME3, CG_OK_BTN, 2 );
			btnId[3] = StockDispBuffer( x+164, y+120, DISP_PRIO_IME3, CG_CANCEL_BTN, 2 );

			// ウィンドウ
			StockDispBuffer( x+w/2, y+h/2, DISP_PRIO_MENU, CG_ITEMSHOP_KOSU_WIN, 1 );
		}
	}

	return ret;
}


// ショップウィンドウ４
//   レベル足りない警告ウィンドウ
short shopWindow4ProcNo;

// 初期化
void initShopWindow4( void )
{
	shopWindow4ProcNo = 0;
}


// メイン
//   戻り値：0 ... 処理中
//           1 ... はい
//           2 ... いいえ
int shopWindow4( void )
{
	static int x, y, w, h;
	static int btnId[2];
	int id;
	int i;
	int ret = 0;

	if( shopWindow4ProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 6;
		h = 2;
		x = (640-w*64)/2;
		y = (456-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );

		shopWindow4ProcNo++;
	}


	if( ptActMenuWin != NULL )
	{
		id = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンの選択判定
			id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
		}

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			wnCloseFlag = 0;
		}


		if( id >= 0 )
		{
			if( id == 0 )
			{
				ret = 1;
			}
			else
			if( id == 1 )
			{
				ret = 2;
			}
			else
			if( id == 100 )
			{
				ret = 100;
			}

			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			if( ret == 100 )
			{
				windowTypeWN = -1;
			}
			return ret;
		}


		if( ptActMenuWin->hp >= 1 )
		{
			// ショップメッセージ
			for( i = 0; i < sizeof( shopWindow4Msg )/sizeof( shopWindow4Msg[0] ); i++ )
			{
				if( shopWindow4Msg[0] != '\0' )
				{
					StockFontBuffer( x+20, y+20+i*20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						shopWindow4Msg[i], 0 );
				}
			}

			btnId[0] = StockDispBuffer( x+128, y+72, DISP_PRIO_IME3, CG_YES_BTN, 2 );
			btnId[1] = StockDispBuffer( x+256, y+72, DISP_PRIO_IME3, CG_NO_BTN, 2 );
		}
	}

	return ret;
}


// ショップウィンドウ５
//   最終確認ウィンドウ
short shopWindow5ProcNo;

// 初期化
void initShopWindow5( void )
{
	shopWindow5ProcNo = 0;
}


// メイン
//   戻り値：0 ... 処理中
//           1 ... はい
//           2 ... いいえ
int shopWindow5( void )
{
	static int x, y, w, h;
	static int btnId[2];
	int id;
	int i;
	int ret = 0;

	if( shopWindow5ProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 6;
		h = 2;
		x = (640-w*64)/2;
		y = (456-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );

		shopWindow5ProcNo++;
	}


	if( ptActMenuWin != NULL )
	{
		id = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンの選択判定
			id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
		}

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			wnCloseFlag = 0;
		}


		if( id >= 0 )
		{
			if( id == 0 )
			{
				ret = 1;
			}
			else
			if( id == 1 )
			{
				ret = 2;
			}
			else
			if( id == 100 )
			{
				ret = 100;
			}

			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			if( ret == 100
			 || ret == 1 )
			{
				windowTypeWN = -1;
			}
			return ret;
		}


		if( ptActMenuWin->hp >= 1 )
		{
			// ショップメッセージ
			for( i = 0; i < sizeof( shopWindow5Msg )/sizeof( shopWindow5Msg[0] ); i++ )
			{
				if( shopWindow5Msg[0] != '\0' )
				{
					StockFontBuffer( x+20, y+20+i*20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						shopWindow5Msg[i], 0 );
				}
			}

			btnId[0] = StockDispBuffer( x+128, y+72, DISP_PRIO_IME3, CG_YES_BTN, 2 );
			btnId[1] = StockDispBuffer( x+256, y+72, DISP_PRIO_IME3, CG_NO_BTN, 2 );
		}
	}

	return ret;
}


// ショップウィンドウ６
//   アイテムが一杯ですウィンドウ
short shopWindow6ProcNo;

// 初期化
void initShopWindow6( void )
{
	shopWindow6ProcNo = 0;
}


// メイン
//   戻り値：0 ... 処理中
//           1 ... はい
int shopWindow6( void )
{
	static int x, y, w, h;
	static int btnId[1];
	int id;
	int i;
	int ret = 0;

	if( shopWindow6ProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 6;
		h = 2;
		x = (640-w*64)/2;
		y = (456-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );

		shopWindow6ProcNo++;
	}


	if( ptActMenuWin != NULL )
	{
		id = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンの選択判定
			id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
		}

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			wnCloseFlag = 0;
		}


		if( id >= 0 )
		{
			if( id == 0 )
			{
				ret = 1;
			}
			else
			if( id == 100 )
			{
				ret = 100;
			}

			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			if( ret == 100 )
			{
				windowTypeWN = -1;
			}
			return ret;
		}


		if( ptActMenuWin->hp >= 1 )
		{
			// ショップメッセージ
			for( i = 0; i < sizeof( shopWindow6Msg )/sizeof( shopWindow6Msg[0] ); i++ )
			{
				if( shopWindow6Msg[0] != '\0' )
				{
					StockFontBuffer( x+20, y+20+i*20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						shopWindow6Msg[i], 0 );
				}
			}

			btnId[0] = StockDispBuffer( x+192, y+72, DISP_PRIO_IME3, CG_YES_BTN, 2 );
		}
	}

	return ret;
}


// ショップウィンドウ７
//   買取ウィンドウ
short shopWindow7ProcNo;

// 初期化
void initShopWindow7( void )
{
	shopWindow7ProcNo = 0;
}


// メイン
//   戻り値：0 ... 処理中
//           1 ... 戻る
//           2 ... 決定
//           3 ... お金一杯
int shopWindow7( void )
{
	static int x, y, w, h;
	static int btnId[3];
	int id;
	int pushId;
	int focusId;
	int selId;
	int i, j;
	int ret = 0;
	char tmsg[256];
	int prevBtnGraNo[] = { CG_PREV_BTN, CG_PREV_BTN_DOWN };
	int nextBtnGraNo[] = { CG_NEXT_BTN, CG_NEXT_BTN_DOWN };
	int itemColor[] = { FONT_PAL_WHITE, FONT_PAL_GRAY, FONT_PAL_RED };
	int color;
	int prevBtn, nextBtn;
	int x1, y1, x2, y2;

	if( shopWindow7ProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 432;
		h = 428;
		x = (640-w)/2;
		y = (456-h)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, -1 );

		shopWindow7ProcNo++;
	}


	if( ptActMenuWin != NULL )
	{
		id = -1;
		pushId = -1;
		focusId = -1;
		selId = -1;

		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンの選択判定
			//id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
			id = selRepGraId( btnId, sizeof( btnId )/sizeof( int ) );
			pushId = pushGraId( btnId, sizeof( btnId )/sizeof( int ) );

			j = shopWondow2Page*MAX_SHOP_ITEM;
			for( i = 0; i < MAX_SHOP_ITEM; i++ )
			{
				if( userItem[j+i].name[0] != '\0' )
				{
					x1 = x+32;
					y1 = y+i*21+116;
					x2 = x1 + 372;
					y2 = y1 + 20;
					if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
					{
						if( mouse.onceState & MOUSE_LEFT_CRICK )
						{
							if( userItem[j+i].sealFlag == 0 )
							{
								selId = j+i;
							}
						}
						focusId = j+i;
					}
				}
			}
		}


		// 前ページボタン
		if( joy_con[0] & JOY_A )
		{
			pushId = 0;
			if( joy_auto[0] & JOY_A )
			{
				id = 0;
			}
		}
		if( pushId == 0 )
		{
			prevBtn = 1;
			if( id == 0 )
			{
				if( shopWondow2Page > 0 )
				{
					shopWondow2Page--;
					play_se( 217, 320, 240 );	// クリック音
				}
			}
		}
		else
		{
			prevBtn = 0;
		}
		// 次ページボタン
		if( joy_con[0] & JOY_B )
		{
			pushId = 1;
			if( joy_auto[0] & JOY_B )
			{
				id = 1;
			}
		}
		if( pushId == 1 )
		{
			nextBtn = 1;
			if( id == 1 )
			{
				if( shopWondow2Page+1 < shopWondow2MaxPage )
				{
					shopWondow2Page++;
					play_se( 217, 320, 240 );	// クリック音
				}
			}
		}
		else
		{
			nextBtn = 0;
		}


		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			wnCloseFlag = 0;
		}

		// id = 0, id = 1 はページボタンなので終わらない
		if( id >= 2
		 || selId >= 0 )
		{
			// 戻るボタン
			if( id == 2 )
			{
				ret = 1;
			}
			else
			if( id == 100 )
			{
				ret = 100;
			}
			else
			{
				selShopItemNo = selId;
				sealItemCnt = 1;
				if( userItem[j+i].price + pc.gold > 1000000 )
				{
					ret = 3;
				}
				else
				{
					ret = 2;
				}
			}

			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			if( ret == 100
			 || (buyOffFlag == 1 && ret == 1) )
			{
				windowTypeWN = -1;
			}
			return ret;
		}

		if( ptActMenuWin->hp >= 1 )
		{
			int xx;

			// ショップタイトル
			xx = (w-strlen( shopWindow1Title )/2*17)/2;
			StockFontBuffer( x+xx, y+18, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Title, 0 );

			// ショップメッセージ
			for( i = 0; i < sizeof( shopWindow7Msg )/sizeof( shopWindow7Msg[0] ); i++ )
			{
				if( shopWindow7Msg[0] != '\0' )
				{
					StockFontBuffer( x+18, y+48+i*18, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						shopWindow7Msg[i], 0 );
				}
			}


			// 所持金
			sprintf( tmsg, "所持金  %8dＳ", pc.gold );
			StockFontBuffer( x+244, y+84, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0 );

			// ページ切り替え
			btnId[0] = StockDispBuffer( x +32, y+100, DISP_PRIO_IME3, prevBtnGraNo[prevBtn], 2 );
			btnId[1] = StockDispBuffer( x+200, y+100, DISP_PRIO_IME3, nextBtnGraNo[nextBtn], 2 );
			sprintf( tmsg, "%2d/%2d ページ", shopWondow2Page+1, shopWondow2MaxPage );
			StockFontBuffer( x +66, y+92, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0 );

			j = shopWondow2Page*MAX_SHOP_ITEM;
			for( i = 0; i < MAX_SHOP_ITEM; i++ )
			{
				// データが無ければスキップ
				if( strlen( userItem[j+i].name ) == 0 )
					continue;

				// デフォルト、白
				color = itemColor[0];
				// 売らないフラグがたってたら、灰
				if( userItem[j+i].sealFlag )
					color = itemColor[1];

				// アイテム名
				StockFontBuffer( x+34, y+118+i*21,
					FONT_PRIO_FRONT, color, userItem[j+i].name, 0 );
				// 金額
				sprintf( tmsg, "%8dＳ", userItem[j+i].price );
				StockFontBuffer( x+312, y+118+i*21,
					FONT_PRIO_FRONT, color, tmsg, 0 );
			}

			if( focusId >= 0 )
			{
				// アイテム画像
				StockDispBuffer( x+64, y+350, DISP_PRIO_IME3, userItem[focusId].graNo, 0 );

				for( i = 0; i < sizeof( userItem[0].info )/sizeof( userItem[0].info[0] ); i++ )
				{
					// 説明
					StockFontBuffer( x+120, y+308+i*20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						userItem[focusId].info[i], 0 );
				}
			}

			// 戻るボタン
			if( buyOffFlag == 0 )
			{
				btnId[2] = StockDispBuffer( x+216, y+402, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );
			}
			else
			{
				btnId[2] = StockDispBuffer( x+216, y+402, DISP_PRIO_IME3, CG_EXIT_BTN, 2 );
			}

			// ウィンドウ
			StockDispBuffer( x+w/2, y+h/2, DISP_PRIO_MENU, CG_ITEMSHOP_WIN, 1 );
		}
	}

	return ret;
}


// ショップウィンドウ８
//   所持金が一杯になりますウィンドウ
short shopWindow8ProcNo;

// 初期化
void initShopWindow8( void )
{
	shopWindow8ProcNo = 0;
}


// メイン
//   戻り値：0 ... 処理中
//           1 ... はい
int shopWindow8( void )
{
	static int x, y, w, h;
	static int btnId[1];
	int id;
	int i;
	int ret = 0;

	if( shopWindow8ProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 6;
		h = 2;
		x = (640-w*64)/2;
		y = (456-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );

		shopWindow8ProcNo++;
	}


	if( ptActMenuWin != NULL )
	{
		id = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンの選択判定
			id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
		}

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			wnCloseFlag = 0;
		}


		if( id >= 0 )
		{
			if( id == 0 )
			{
				ret = 1;
			}
			else
			if( id == 100 )
			{
				ret = 100;
			}

			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			if( ret == 100 )
			{
				windowTypeWN = -1;
			}
			return ret;
		}


		if( ptActMenuWin->hp >= 1 )
		{
			// ショップメッセージ
			for( i = 0; i < sizeof( shopWindow8Msg )/sizeof( shopWindow8Msg[0] ); i++ )
			{
				if( shopWindow8Msg[0] != '\0' )
				{
					StockFontBuffer( x+20, y+20+i*20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						shopWindow8Msg[i], 0 );
				}
			}

			btnId[0] = StockDispBuffer( x+192, y+72, DISP_PRIO_IME3, CG_YES_BTN, 2 );
		}
	}

	return ret;
}




// ペットスキルショップ専用処理

void initSkillShopWindow1( void );
int skillShopWindow1( void );
void initSkillShopWindow2( void );
int skillShopWindow2( void );
void initSkillShopWindow3( void );
int skillShopWindow3( void );
void initSkillShopWindow4( void );
int skillShopWindow4( void );

void serverWindowType6( void )
{
	int ret;

	// 店メニュー
	if( skillShopWindowProcNo == 0 )
	{
		initSkillShopWindow1();
		skillShopWindowProcNo++;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( skillShopWindowProcNo == 1 )
	{
		ret = skillShopWindow1();
		if( ret == 2 )
		{
			skillShopWindowProcNo = 10;
		}
	}

	// ペット選択
	if( skillShopWindowProcNo == 10 )
	{
		initSkillShopWindow2();
		skillShopWindowProcNo++;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( skillShopWindowProcNo == 11 )
	{
		ret = skillShopWindow2();
		// 戻る
		if( ret == 1 )
		{
			skillShopWindowProcNo = 0;
		}
		else
		if( ret == 2 )
		{
			skillShopWindowProcNo = 20;
		}
	}

	// スロット選択
	if( skillShopWindowProcNo == 20 )
	{
		initSkillShopWindow3();
		skillShopWindowProcNo++;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( skillShopWindowProcNo == 21 )
	{
		ret = skillShopWindow3();
		if( ret == 1 )
		{
			skillShopWindowProcNo = 10;
		}
		else
		if( ret == 2 )
		{
			skillShopWindowProcNo = 30;
		}
	}

	// 確認ウィンドウ
	if( skillShopWindowProcNo == 30 )
	{
		initSkillShopWindow4();
		skillShopWindowProcNo++;
		play_se( 202, 320, 240 );	// ウィンドウ開く音
	}
	if( skillShopWindowProcNo == 31 )
	{
		ret = skillShopWindow4();
		if( ret == 1 )
		{
#if 1
			char data[256];
			char msg[256];
			sprintf( data, "%d|%d|%d|%d",
				selShopSkillNo+1,
				selShopSkillPetNo,
				selShopSkillSlotNo,
				sealSkill[selShopSkillNo].price );
			makeSendString( data, msg, sizeof( msg )-1 );
			lssproto_WN_send( sockfd, nowGx, nowGy, indexWN, idWN, 0, msg ) ;
#endif
			pc.gold -= sealSkill[selShopSkillNo].price;

			windowTypeWN = -1;
		}
		else
		if( ret == 2 )
		{
			skillShopWindowProcNo = 20;
		}
	}
}


// ペットスキルメインウィンドウ
short skillShopWindow1ProcNo;
// 初期化
void initSkillShopWindow1( void )
{
	skillShopWindow1ProcNo = 0;
}

// メイン
//   戻り値：0 ... 処理中
//           1 ... 出る
//           2 ... 決定
int skillShopWindow1( void )
{
	static int x, y, w, h;
	static int btnId[3];
	int id;
	int pushId;
	int focusId;
	int selId;
	int i, j;
	int ret = 0;
	char tmsg[256];
	int prevBtnGraNo[] = { CG_PREV_BTN, CG_PREV_BTN_DOWN };
	int nextBtnGraNo[] = { CG_NEXT_BTN, CG_NEXT_BTN_DOWN };
	int itemColor[] = { FONT_PAL_WHITE, FONT_PAL_GRAY, FONT_PAL_RED };
	int color;
	int prevBtn, nextBtn;
	int x1, y1, x2, y2;

	if( skillShopWindow1ProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 432;
		h = 428;
		x = (640-w)/2;
		y = (456-h)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, -1 );

		skillShopWindow1ProcNo++;
	}


	if( ptActMenuWin != NULL )
	{
		id = -1;
		pushId = -1;
		focusId = -1;
		selId = -1;

		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンの選択判定
			//id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
			id = selRepGraId( btnId, sizeof( btnId )/sizeof( int ) );
			pushId = pushGraId( btnId, sizeof( btnId )/sizeof( int ) );


			j = shopWondow2Page*MAX_SKILL_SHOP_SKILL;
			for( i = 0; i < MAX_SKILL_SHOP_SKILL; i++ )
			{
				if( sealSkill[j+i].name[0] != '\0' )
				{
					x1 = x+32;
					y1 = y+i*21+116;
					x2 = x1 + 372;
					y2 = y1 + 20;
					if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
					{
						if( mouse.onceState & MOUSE_LEFT_CRICK )
						{
							// 所持金が足りないものは選べない
							if( sealSkill[j+i].price <= pc.gold )
							{
								selId = j+i;
							}
							else
							{
								play_se( 220, 320, 240 );	// ＮＧ音
							}
						}
						focusId = j+i;
					}
				}
			}
		}


		// 前ページボタン
		if( pushId == 0 )
		{
			prevBtn = 1;
			if( id == 0 )
			{
				if( shopWondow2Page > 0 )
				{
					shopWondow2Page--;
					play_se( 217, 320, 240 );	// クリック音
				}
			}
		}
		else
		{
			prevBtn = 0;
		}
		// 次ページボタン
		if( pushId == 1 )
		{
			nextBtn = 1;
			if( id == 1 )
			{
				if( shopWondow2Page+1 < shopWondow2MaxPage )
				{
					shopWondow2Page++;
					play_se( 217, 320, 240 );	// クリック音
				}
			}
		}
		else
		{
			nextBtn = 0;
		}


		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			wnCloseFlag = 0;
		}

		// id = 0, id = 1 はページボタンなので終わらない
		if( id >= 2
		 || selId >= 0 )
		{
			// 出るボタン
			if( id == 2 )
			{
				ret = 1;
				play_se( 203, 320, 240 );	// ウィンドウ閉じる音
			}
			else
			if( id == 100 )
			{
				ret = 100;
			}
			else
			{
				selShopSkillNo = selId;
				ret = 2;
			}

			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			if( ret == 100
			 || ret == 1 )
			{
				windowTypeWN = -1;
			}
			return ret;
		}

		if( ptActMenuWin->hp >= 1 )
		{
			int xx;

			// ショップタイトル
			xx = (w-strlen( shopWindow1Title )/2*17)/2;
			StockFontBuffer( x+xx, y+18, FONT_PRIO_FRONT, FONT_PAL_WHITE, shopWindow1Title, 0 );

			// ショップメッセージ
			for( i = 0; i < sizeof( shopWindow2Msg )/sizeof( shopWindow2Msg[0] ); i++ )
			{
				if( shopWindow2Msg[0] != '\0' )
				{
					StockFontBuffer( x+18, y+48+i*18, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						shopWindow2Msg[i], 0 );
				}
			}


			// 所持金
			sprintf( tmsg, "所持金  %8dＳ", pc.gold );
			StockFontBuffer( x+244, y+84, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0 );

			// ページ切り替え
			btnId[0] = StockDispBuffer( x +32, y+100, DISP_PRIO_IME3, prevBtnGraNo[prevBtn], 2 );
			btnId[1] = StockDispBuffer( x+200, y+100, DISP_PRIO_IME3, nextBtnGraNo[nextBtn], 2 );
			sprintf( tmsg, "%2d/%2d ページ", shopWondow2Page+1, shopWondow2MaxPage );
			StockFontBuffer( x +66, y+92, FONT_PRIO_FRONT, FONT_PAL_WHITE, tmsg, 0 );

			j = shopWondow2Page*MAX_SKILL_SHOP_SKILL;
			for( i = 0; i < MAX_SKILL_SHOP_SKILL; i++ )
			{
				// データが無ければスキップ
				if( strlen( sealSkill[j+i].name ) == 0 )
					continue;

				// デフォルト、白
				color = itemColor[0];
				// お金が無い時も、灰
				if( sealSkill[j+i].price > pc.gold )
				{
					color = itemColor[1];
				}

				// アイテム名
				StockFontBuffer( x+34, y+118+i*21,
					FONT_PRIO_FRONT, color, sealSkill[j+i].name, 0 );
				// 金額
				sprintf( tmsg, "%8dＳ", sealSkill[j+i].price );
				StockFontBuffer( x+312, y+118+i*21,
					FONT_PRIO_FRONT, color, tmsg, 0 );
			}

			if( focusId >= 0 )
			{
				for( i = 0; i < sizeof( sealSkill[0].info )/sizeof( sealSkill[0].info[0] ); i++ )
				{
					// 説明
					StockFontBuffer( x+27, y+308+i*20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						sealSkill[focusId].info[i], 0 );
				}
			}

			// 出るボタン
			btnId[2] = StockDispBuffer( x+216, y+402, DISP_PRIO_IME3, CG_EXIT_BTN, 2 );

			// ウィンドウ
			StockDispBuffer( x+w/2, y+h/2, DISP_PRIO_MENU, CG_SKILLSHOP_WIN, 1 );
		}
	}

	return ret;
}


// ペットスキル屋　ペット選択ウィンドウ
short skillShopWindow2ProcNo;
// 初期化
void initSkillShopWindow2( void )
{
	skillShopWindow2ProcNo = 0;
}

// メイン
//   戻り値：0 ... 処理中
//           1 ... 戻る
//           2 ... 決定
int skillShopWindow2( void )
{
	static int x, y, w, h;
	static int btnId[1];
	int id;
	int i, j;
	int ret = 0;
	int x1, y1, x2, y2;
	int len;

	if( skillShopWindow2ProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 7;
		h = 5;
		x = (640-w*64)/2;
		y = (456-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );

		skillShopWindow2ProcNo++;
	}

	if( ptActMenuWin != NULL )
	{
		id = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンの選択判定
			id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );

			for( i = 0, j = 0; i < MAX_PET; i++ )
			{
				// 文字列無いところには枠出さない
				if( pet[i].useFlag != 0 )
				{
					x1 = x+41;
					y1 = y+25+(j+1)*26;
					x2 = x1 + 366;
					y2 = y1 + 22;
					if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
					{
						if( mouse.onceState & MOUSE_LEFT_CRICK )
						{
							id = i + 1;
						}
					}
					j++;
				}
			}
		}

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			wnCloseFlag = 0;
		}

		if( id >= 0 )
		{
			// 戻るボタン
			if( id == 0 )
			{
				ret = 1;
			}
			else
			if( id == 100 )
			{
				ret = 100;
			}
			else
			{
				selShopSkillPetNo = id;
				ret = 2;
			}

			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			if( ret == 100 )
			{
				windowTypeWN = -1;
			}
			return ret;
		}

		if( ptActMenuWin->hp >= 1 )
		{
			StockFontBuffer( x+145, y+25,
				FONT_PRIO_FRONT, FONT_PAL_WHITE, "誰に覚えさせますか？", 0 );
			for( i = 0, j = 0; i < MAX_PET; i++ )
			{
				if( pet[i].useFlag != 0 )
				{
					len = strlen( pet[i].freeName );
					if( len > 0 )
					{
						strcpy( msgWN[1], pet[i].freeName );
					}
					else
					{
						len = strlen( pet[i].name );
						strcpy( msgWN[1], pet[i].name );
					}
#if 0
					StockFontBuffer( x+88+(144-len*9)/2, y+28+(j+1)*26,
#else
					StockFontBuffer( x+62, y+28+(j+1)*26,
#endif
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0 );


					sprintf( msgWN[1], "LV.%d", pet[i].level );
					StockFontBuffer( x+260, y+28+(j+1)*26,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0 );

					sprintf( msgWN[1], "MaxHP %d", pet[i].maxHp );
					StockFontBuffer( x+314, y+28+(j+1)*26,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msgWN[1], 0 );

					j++;
				}
			}
			btnId[0] = StockDispBuffer( x+w*64/2, y+204, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );
		}
	}

	return ret;
}


// ペットスキル屋　スロット選択ウィンドウ
short skillShopWindow3ProcNo;
// 初期化
void initSkillShopWindow3( void )
{
	skillShopWindow3ProcNo = 0;
}

// メイン
//   戻り値：0 ... 処理中
//           1 ... 戻る
//           2 ... 決定
int skillShopWindow3( void )
{
	static int x, y, w, h;
	static int btnId[1];
	int id;
	int i;
	int ret = 0;
	int x1, y1, x2, y2;
	char skillName[128];

	if( skillShopWindow3ProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 5;
		h = 6;
		x = (640-w*64)/2;
		y = (456-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );

		skillShopWindow3ProcNo++;
	}

	if( ptActMenuWin != NULL )
	{
		id = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンの選択判定
			id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );

			for( i = 0; i < MAX_SKILL && i < pet[selShopSkillPetNo-1].maxSkill; i++ )
			{
				x1 = x+41;
				y1 = y+33+(i+1)*26;
				x2 = x1 + 238;
				y2 = y1 + 22;
				if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
				{
					if( mouse.onceState & MOUSE_LEFT_CRICK )
					{
						id = i + 1;
					}
				}
			}
		}

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			wnCloseFlag = 0;
		}

		if( id >= 0 )
		{
			// 戻るボタン
			if( id == 0 )
			{
				ret = 1;
			}
			else
			if( id == 100 )
			{
				ret = 100;
			}
			else
			{
				selShopSkillSlotNo = id;
				ret = 2;
			}

			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			if( ret == 100 )
			{
				windowTypeWN = -1;
			}
			return ret;
		}

		if( ptActMenuWin->hp >= 1 )
		{
			StockFontBuffer( x+75, y+33,
				FONT_PRIO_FRONT, FONT_PAL_WHITE, "どの位置にしますか？", 0 );
			for( i = 0; i < MAX_SKILL && i < pet[selShopSkillPetNo-1].maxSkill; i++ )
			{
				sprintf( skillName, "技 %d: ", i+1 );
				if( petSkill[selShopSkillPetNo-1][i].useFlag != 0 )
				{
					strcat( skillName, petSkill[selShopSkillPetNo-1][i].name );
				}
				StockFontBuffer( x+46, y+36+(i+1)*26,
					FONT_PRIO_FRONT, FONT_PAL_WHITE, skillName, 0 );
			}
			btnId[0] = StockDispBuffer( x+160, y+256, DISP_PRIO_IME3, CG_RETURN_BTN, 2 );
		}
	}

	return ret;
}


// ショップウィンドウ５
//   最終確認ウィンドウ
short skillShopWindow4ProcNo;

// 初期化
void initSkillShopWindow4( void )
{
	skillShopWindow4ProcNo = 0;
}


// メイン
//   戻り値：0 ... 処理中
//           1 ... はい
//           2 ... いいえ
int skillShopWindow4( void )
{
	static int x, y, w, h;
	static int btnId[2];
	static char msg1[128];
	int id;
	int i;
	int ret = 0;

	if( skillShopWindow4ProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 8;
		h = 2;
		x = (640-w*64)/2;
		y = (456-h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );


		char name[32];
		if( strlen( pet[selShopSkillPetNo-1].freeName ) > 0 )
		{
			strcpy( name, pet[selShopSkillPetNo-1].freeName );
		}
		else
		{
			strcpy( name, pet[selShopSkillPetNo-1].name );
		}

		sprintf( msg1, "[%s]に[%s]を", name,
			sealSkill[selShopSkillNo].name );

		skillShopWindow4ProcNo++;
	}


	if( ptActMenuWin != NULL )
	{
		id = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンの選択判定
			id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
		}

		// 他のメニューが出るのでこっちのウィンドウを閉じる
		if( CheckMenuFlag()
		 || joy_trg[ 0 ] & JOY_ESC
		 || actBtn == 1
		 || menuBtn == 1
		 || disconnectServerFlag == TRUE
		 || wnCloseFlag == 1 )
		{
			id = 100;
			wnCloseFlag = 0;
		}


		if( id >= 0 )
		{
			if( id == 0 )
			{
				ret = 1;
			}
			else
			if( id == 1 )
			{
				ret = 2;
			}
			else
			if( id == 100 )
			{
				ret = 100;
			}

			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			if( ret == 100
			 || ret == 1 )
			{
				windowTypeWN = -1;
			}
			return ret;
		}


		if( ptActMenuWin->hp >= 1 )
		{
			// 確認メッセージ
			StockFontBuffer( x+20, y+20+0*20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				msg1, 0 );
			StockFontBuffer( x+20, y+20+1*20, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"覚えさせますか？", 0 );

			btnId[0] = StockDispBuffer( x+w*64/3, y+72, DISP_PRIO_IME3, CG_YES_BTN, 2 );
			btnId[1] = StockDispBuffer( x+w*64/3*2, y+72, DISP_PRIO_IME3, CG_NO_BTN, 2 );
		}
	}

	return ret;
}








// ログアウト演出
int produce_logout()
{
	switch(produce_vct_no){
	case 0:
		// バトルサーフェスの画像作成 
		CopyBackBuffer();
		// バックバッファー描画方法変更
		BackBufferDrawType = DRAW_BACK_PRODUCE; 
		// ＢＧＭフェードアウト開始
		fade_out_bgm();
		// 現在の時間を記憶
		NowTime = GetTickCount();
		produce_vct_no++;
		// メニュー初期化
		InitMenu();
		break;
	case 1:
		// 演出中
		if( DrawProduce( PRODUCE_BRAN_SMALL ) == TRUE ){
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			// デフォルトパレットに戻す
			PaletteChange( DEF_PAL, 0 );
			return 1;		//演出終了
		}
//		MenuProc();	// メニュー処理
//		ImeProc();	// ＩＭＥ関連処理
		break;
	}
	return 0;		//演出中
}

