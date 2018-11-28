#ifndef _NETMAIN_H_
#define _NETMAIN_H_

// ゲームサーバーの管理用構造体
struct gameserver
{
	char ipaddr[128];		// IPアドレス
	char port[64];			// ポート
};


#define SUCCESSFULSTR	"successful"        // プロトコルで使う文字定数群
#define FAILEDSTR		"failed" 
#define OKSTR           "ok"

#define TIMEOUT (600*1000)                   //  ネットワークのすべてのところでつかうあきらめ時間

#define NETBUFSIZ (1024*16)

extern char net_writebuf[];
extern char net_readbuf[];
extern int net_readbuflen;
extern int net_writebuflen;

extern unsigned int sockfd;
extern int server_choosed;
extern int init_net;

extern struct gameserver gmsv[];

#ifdef _LOG_MSG
extern char debugLogFileName[];
#endif


extern BOOL disconnectServerFlag;
extern BOOL oldDisconnectServerFlag;


void ipAddressAnalyze( LPSTR );
int getServerMax( void );
int getServerInfo( int, char *, short * );


void networkLoop(void);
void networkMycharWalk( int gx , int gy , char *direction );

int initNet(void);
void cleanupNetwork( void );
int appendReadBuf( char *buf , int size );
int appendWriteBuf( int index , char *buf , int size );
int shiftReadBuf(  int size );
int shiftWriteBuf( int size );
int getLineFromReadBuf( char *output  , int maxlen );
int sendn( unsigned int s , char *buffer , int len );



int getServerInfoByServerName( char *servername , char *hostname , short *port );


// エラーメッセージ
#define NET_ERRMSG_SOCKLIBERROR		"SOCKLIBERROR" //"ネットワークの準備が出来ていません。" 网络的准备都没有
#define NET_ERRMSG_BADNAME          "服务器名不正确" //"サーバ名が不正です。" 服务器名不正确。“
#define NET_ERRMSG_SOCKETERROR      "SOCKETERROR" //"ネットワークソケット(TCP)を作成することができません。"
#define NET_ERRMSG_NOTGETADDR		"服务器的IP地址没能够得到(%s)" //"サーバのIPアドレスを得ることができません。(%s)"
#define NET_ERRMSG_NOTCONNECT_S		"服务器连接错误" //"サーバに接続する処理を開始することができません。"
#define NET_ERRMSG_NOTCONNECT		"不能连接到服务器" //"サーバに接続することができません。 "
#define NET_ERRMSG_CONNECTTIMEOUT	"连接服务器超时" //"サーバ接続中にタイムアウトしました。" 
#define NET_ERRMSG_LOGINTIMEOUT		"进入游戏超时" //"ログイン処理中にタイムアウトしました。"
#define NET_ERRMSG_CHARLISTTIMEOUT	"人物列表获取超时" //"キャラクタリストを取得中にタイムアウトしました。"
#define NET_ERRMSG_LOGOUTTIMEOUT	"退出处理超时" //"ログアウト処理中にタイムアウトしました。"
#define NET_ERRMSG_LOGINFAIL		"登陆失败" //"ログイン出来ませんでした。"
#define NET_ERRMSG_CREATECHARTIMEOUT	"角色创造超时" //"キャラクタを作成中にタイムアウトしました。"
#define NET_ERRMSG_DELETECHARTIMEOUT    "人物删除超时" //"キャラクタを削除中にタイムアウトしました。"



#define GAMESTATE_ERRMSG_LOGOUTDENY        "退出失败" //"ログアウト処理がサーバに拒否されました。"

#if 0
#define GAMESTATE_ERRMSG_NEEDSERVERNAME   "服务器的名字没有指定" //"サーバの名前が指定されていません。"
#define GAMESTATE_ERRMSG_NEEDCHOOSE       "NEEDCHOOSE" //"サーバを選択してからでないとこの処理は実行できません。" 
#define GAMESTATE_ERRMSG_CANTSAVEUSERINFO "磁盘不足信息保存失败" //"ユーザ情報をファイルに保存できません。ディスクの空き容量が足りません。"
#define GAMESTATE_ERRMSG_CLIENTLOGINERROR "登陆失败" //"サーバにログインできません。"
#define GAMESTATE_ERRMSG_CLIENTLOGINTIMEOUT "登陆超时" //"サーバにログインする処理がタイムアウトしました。"
#define GAMESTATE_ERRMSG_CANNOTLOADPASSWD "CANNOTLOADPASSWD" //"まだユーザ情報が入力されていないので、パスワードを変更することはできません。まずユーザー情報を入力してください。"
#define GAMESTATE_ERRMSG_CANNOTSAVEPASSWD "CANNOTSAVEPASSWD" //"ユーザ情報をファイルに保存できなかったので、パスワード変更に失敗しました。"
#define GAMESTATE_ERRMSG_CHPASSERROR      "修改密码错误" //"パスワードの変更ができませんでした。"
#define GAMESTATE_ERRMSG_CHPASSTIMEOUT    "修改密码超时" //"パスワード変更処理がタイムアウトしました。"
#define GAMESTATE_ERRMSG_DELETECHARSERVERDENY    "删除角色被拒绝" //"キャラ削除処理がサーバに拒否されました。"
#define GAMESTATE_ERRMSG_CHARLISTDENY      "角色列表获取被拒绝" //"キャラクター表取得処理がサーバに拒否されました。"
#define GAMESTATE_ERRMSG_LOGINTIMEOUT			"登陆超时" //"ログイン処理がタイムアウトしました。"
#define GAMESTATE_ERRMSG_NOTIMPLEMENTED   "功能没实现" //"まだその機能は実装されてないなり"
#endif


#endif


/*
#define NET_ERRMSG_SOCKLIBERROR		"呼隔﹟ゼ非称" //"僱僢僩儚乕僋偺弨旛偑弌棃偰偄傑偣傫丅"
#define NET_ERRMSG_BADNAME          "狝竟嘿ぃタ絋" //"僒乕僶柤偑晄惓偱偡丅"
#define NET_ERRMSG_SOCKETERROR      "礚猭暗ΘTCP" //"僱僢僩儚乕僋僜働僢僩(TCP)傪嶌惉偡傞偙偲偑偱偒傑偣傫丅"
#define NET_ERRMSG_NOTGETADDR		"礚猭莉眔狝竟⒆⑥竚(%s)" //"僒乕僶偺IP傾僪儗僗傪摼傞偙偲偑偱偒傑偣傫丅(%s)"
#define NET_ERRMSG_NOTCONNECT_S		"礚猭秨﹍秈︽狝竟硈钡" //"僒乕僶偵愙懕偡傞張棟傪奐巒偡傞偙偲偑偱偒傑偣傫丅"
#define NET_ERRMSG_NOTCONNECT		"礚猭硈钡狝竟" //"僒乕僶偵愙懕偡傞偙偲偑偱偒傑偣傫丅 "
#define NET_ERRMSG_CONNECTTIMEOUT	"狝竟硈钡丁" //"僒乕僶愙懕拞偵僞僀儉傾僂僩偟傑偟偨丅" 
#define NET_ERRMSG_LOGINTIMEOUT		"祅矪瞶丁" //"儘僌僀儞張棟拞偵僞僀儉傾僂僩偟傑偟偨丅"
#define NET_ERRMSG_CHARLISTTIMEOUT	"弄丁" //"僉儍儔僋僞儕僗僩傪庢摼拞偵僞僀儉傾僂僩偟傑偟偨丅"
#define NET_ERRMSG_LOGOUTTIMEOUT	"祅矪瞶丁" //"儘僌傾僂僩張棟拞偵僞僀儉傾僂僩偟傑偟偨丅"
#define NET_ERRMSG_LOGINFAIL		"礚猭祅狝竟" //"儘僌僀儞弌棃傑偣傫偱偟偨丅"
#define NET_ERRMSG_CREATECHARTIMEOUT	"籹丁" //"僉儍儔僋僞傪嶌惉拞偵僞僀儉傾僂僩偟傑偟偨丅"
#define NET_ERRMSG_DELETECHARTIMEOUT    "玠埃丁" //"僉儍儔僋僞傪嶍彍拞偵僞僀儉傾僂僩偟傑偟偨丅"

  
	
	  #define GAMESTATE_ERRMSG_LOGOUTDENY        "狝竟┶荡矪瞶祅" //"儘僌傾僂僩張棟偑僒乕僶偵嫅斲偝傟傑偟偨丅"
	  
		#if 0
		#define GAMESTATE_ERRMSG_NEEDSERVERNAME   "⊿Τ﹚狝竟嘿" //"僒乕僶偺柤慜偑巜掕偝傟偰偄傑偣傫丅"
		#define GAMESTATE_ERRMSG_NEEDCHOOSE       "ぃ匡拒狝竟玥礚猭秈︽赣兜矪瞶" //"僒乕僶傪慖戰偟偰偐傜偱側偄偲偙偺張棟偼幚峴偱偒傑偣傫丅" 
		#define GAMESTATE_ERRMSG_CANTSAVEUSERINFO "礚猭玂ㄏノ薄厨合盒甧秖ぃì" //"儐乕僓忣曬傪僼傽僀儖偵曐懚偱偒傑偣傫丅僨傿僗僋偺嬻偒梕検偑懌傝傑偣傫丅"
		#define GAMESTATE_ERRMSG_CLIENTLOGINERROR "礚猭祅狝竟" //"僒乕僶偵儘僌僀儞偱偒傑偣傫丅"
		#define GAMESTATE_ERRMSG_CLIENTLOGINTIMEOUT "祅狝竟丁" //"僒乕僶偵儘僌僀儞偡傞張棟偑僞僀儉傾僂僩偟傑偟偨丅"
		#define GAMESTATE_ERRMSG_CANNOTLOADPASSWD "パ﹟ゼ块ㄏノ薄厨礚猭跑盞絏叫块ㄏノ薄厨" //"傑偩儐乕僓忣曬偑擖椡偝傟偰偄側偄偺偱丄僷僗儚乕僪傪曄峏偡傞偙偲偼偱偒傑偣傫丅傑偢儐乕僓乕忣曬傪擖椡偟偰偔偩偝偄丅"
		#define GAMESTATE_ERRMSG_CANNOTSAVEPASSWD "礚猭玂ㄏノ薄厨盞絏跑ア毖" //"儐乕僓忣曬傪僼傽僀儖偵曐懚偱偒側偐偭偨偺偱丄僷僗儚乕僪曄峏偵幐攕偟傑偟偨丅"
		#define GAMESTATE_ERRMSG_CHPASSERROR      "礚猭跑盞絏" //"僷僗儚乕僪偺曄峏偑偱偒傑偣傫偱偟偨丅"
		#define GAMESTATE_ERRMSG_CHPASSTIMEOUT    "盞絏跑丁" //"僷僗儚乕僪曄峏張棟偑僞僀儉傾僂僩偟傑偟偨丅"
		#define GAMESTATE_ERRMSG_DELETECHARSERVERDENY    "狝竟┶荡矪瞶玠埃" //"僉儍儔嶍彍張棟偑僒乕僶偵嫅斲偝傟傑偟偨丅"
		#define GAMESTATE_ERRMSG_CHARLISTDENY      "狝竟┶荡矪瞶弄" //"僉儍儔僋僞乕昞庢摼張棟偑僒乕僶偵嫅斲偝傟傑偟偨丅"
		#define GAMESTATE_ERRMSG_LOGINTIMEOUT			"祅矪瞶丁" //"儘僌僀儞張棟偑僞僀儉傾僂僩偟傑偟偨丅"
		#define GAMESTATE_ERRMSG_NOTIMPLEMENTED   "﹟ゼ杆竚赣" //"傑偩偦偺婡擻偼幚憰偝傟偰側偄側傝"
		#endif
*/

