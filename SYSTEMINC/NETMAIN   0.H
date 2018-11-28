#ifndef _NETMAIN_H_
#define _NETMAIN_H_

// ƒQ[ƒ€ƒT[ƒo[‚ÌŠÇ——p\‘¢‘Ì
struct gameserver
{
	char ipaddr[128];		// IPƒAƒhƒŒƒX
	char port[64];			// ƒ|[ƒg
};


#define SUCCESSFULSTR	"successful"        // ƒvƒƒgƒRƒ‹‚Åg‚¤•¶š’è”ŒQ
#define FAILEDSTR		"failed" 
#define OKSTR           "ok"

#define TIMEOUT (600*1000)                   //  ƒlƒbƒgƒ[ƒN‚Ì‚·‚×‚Ä‚Ì‚Æ‚±‚ë‚Å‚Â‚©‚¤‚ ‚«‚ç‚ßŠÔ

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


// ƒGƒ‰[ƒƒbƒZ[ƒW
#define NET_ERRMSG_SOCKLIBERROR		"ºô¸ô©|¥¼·Ç³Æ¦n¡C" //"ƒlƒbƒgƒ[ƒN‚Ì€”õ‚ªo—ˆ‚Ä‚¢‚Ü‚¹‚ñB"
#define NET_ERRMSG_BADNAME          "¦øªA¾¹¦WºÙ¤£¥¿½T¡C" //"ƒT[ƒo–¼‚ª•s³‚Å‚·B"
#define NET_ERRMSG_SOCKETERROR      "µLªk°µ¦¨TCP¡C" //"ƒlƒbƒgƒ[ƒNƒ\ƒPƒbƒg(TCP)‚ğì¬‚·‚é‚±‚Æ‚ª‚Å‚«‚Ü‚¹‚ñB"
#define NET_ERRMSG_NOTGETADDR		"µLªkÀò±o¦øªA¾¹ªº¢×¢Ş¦ì¸m¡C(%s)" //"ƒT[ƒo‚ÌIPƒAƒhƒŒƒX‚ğ“¾‚é‚±‚Æ‚ª‚Å‚«‚Ü‚¹‚ñB(%s)"
#define NET_ERRMSG_NOTCONNECT_S		"µLªk¶}©l¶i¦æ¦øªA¾¹ªº³s±µ¡C" //"ƒT[ƒo‚ÉÚ‘±‚·‚éˆ—‚ğŠJn‚·‚é‚±‚Æ‚ª‚Å‚«‚Ü‚¹‚ñB"
#define NET_ERRMSG_NOTCONNECT		"µLªk³s±µ¦øªA¾¹¡C" //"ƒT[ƒo‚ÉÚ‘±‚·‚é‚±‚Æ‚ª‚Å‚«‚Ü‚¹‚ñB "
#define NET_ERRMSG_CONNECTTIMEOUT	"¦øªA¾¹³s±µ®É¶¡¤w¨ì¡C" //"ƒT[ƒoÚ‘±’†‚Éƒ^ƒCƒ€ƒAƒEƒg‚µ‚Ü‚µ‚½B" 
#define NET_ERRMSG_LOGINTIMEOUT		"µn¥X³B²z®É¶¡¤w¨ì¡C" //"ƒƒOƒCƒ“ˆ—’†‚Éƒ^ƒCƒ€ƒAƒEƒg‚µ‚Ü‚µ‚½B"
#define NET_ERRMSG_CHARLISTTIMEOUT	"Åª¨ú¤Hª«ªí®É¶¡¤w¨ì¡C" //"ƒLƒƒƒ‰ƒNƒ^ƒŠƒXƒg‚ğæ“¾’†‚Éƒ^ƒCƒ€ƒAƒEƒg‚µ‚Ü‚µ‚½B"
#define NET_ERRMSG_LOGOUTTIMEOUT	"µn¥X³B²z®É¶¡¤w¨ì¡C" //"ƒƒOƒAƒEƒgˆ—’†‚Éƒ^ƒCƒ€ƒAƒEƒg‚µ‚Ü‚µ‚½B"
#define NET_ERRMSG_LOGINFAIL		"µLªkµn¤J¦øªA¾¹¡C" //"ƒƒOƒCƒ“o—ˆ‚Ü‚¹‚ñ‚Å‚µ‚½B"
#define NET_ERRMSG_CREATECHARTIMEOUT	"¤Hª«»s§@®É¶¡¤w¨ì¡C" //"ƒLƒƒƒ‰ƒNƒ^‚ğì¬’†‚Éƒ^ƒCƒ€ƒAƒEƒg‚µ‚Ü‚µ‚½B"
#define NET_ERRMSG_DELETECHARTIMEOUT    "¤Hª««d°£®É¶¡¤w¨ì¡C" //"ƒLƒƒƒ‰ƒNƒ^‚ğíœ’†‚Éƒ^ƒCƒ€ƒAƒEƒg‚µ‚Ü‚µ‚½B"



#define GAMESTATE_ERRMSG_LOGOUTDENY        "¦øªA¾¹©Úµ´³B²zµn¥X¡C" //"ƒƒOƒAƒEƒgˆ—‚ªƒT[ƒo‚É‹‘”Û‚³‚ê‚Ü‚µ‚½B"

#if 0
#define GAMESTATE_ERRMSG_NEEDSERVERNAME   "¨S¦³«ü©w¦øªA¾¹¦WºÙ¡C" //"ƒT[ƒo‚Ì–¼‘O‚ªw’è‚³‚ê‚Ä‚¢‚Ü‚¹‚ñB"
#define GAMESTATE_ERRMSG_NEEDCHOOSE       "¤£¥ı¿ï¾Ü¦øªA¾¹«hµLªk¶i¦æ¸Ó¶µ³B²z¡C" //"ƒT[ƒo‚ğ‘I‘ğ‚µ‚Ä‚©‚ç‚Å‚È‚¢‚Æ‚±‚Ìˆ—‚ÍÀs‚Å‚«‚Ü‚¹‚ñB" 
#define GAMESTATE_ERRMSG_CANTSAVEUSERINFO "µLªk«O¦s¨Ï¥ÎªÌªº±¡³ø¡CºÏºĞ®e¶q¤£¨¬¡C" //"ƒ†[ƒUî•ñ‚ğƒtƒ@ƒCƒ‹‚É•Û‘¶‚Å‚«‚Ü‚¹‚ñBƒfƒBƒXƒN‚Ì‹ó‚«—e—Ê‚ª‘«‚è‚Ü‚¹‚ñB"
#define GAMESTATE_ERRMSG_CLIENTLOGINERROR "µLªkµn¤J¦øªA¾¹¡C" //"ƒT[ƒo‚ÉƒƒOƒCƒ“‚Å‚«‚Ü‚¹‚ñB"
#define GAMESTATE_ERRMSG_CLIENTLOGINTIMEOUT "µn¤J¦øªA¾¹®É¶¡¤w¨ì¡C" //"ƒT[ƒo‚ÉƒƒOƒCƒ“‚·‚éˆ—‚ªƒ^ƒCƒ€ƒAƒEƒg‚µ‚Ü‚µ‚½B"
#define GAMESTATE_ERRMSG_CANNOTLOADPASSWD "¥Ñ©ó©|¥¼¿é¤J¨Ï¥ÎªÌ±¡³ø¡AµLªkÅÜ§ó±K½X¡C½Ğ¥ı¿é¤J¨Ï¥ÎªÌ±¡³ø¡C" //"‚Ü‚¾ƒ†[ƒUî•ñ‚ª“ü—Í‚³‚ê‚Ä‚¢‚È‚¢‚Ì‚ÅAƒpƒXƒ[ƒh‚ğ•ÏX‚·‚é‚±‚Æ‚Í‚Å‚«‚Ü‚¹‚ñB‚Ü‚¸ƒ†[ƒU[î•ñ‚ğ“ü—Í‚µ‚Ä‚­‚¾‚³‚¢B"
#define GAMESTATE_ERRMSG_CANNOTSAVEPASSWD "µLªk«O¦s¨Ï¥ÎªÌ±¡³ø¡A±K½XÅÜ§ó¥¢±Ñ¡C" //"ƒ†[ƒUî•ñ‚ğƒtƒ@ƒCƒ‹‚É•Û‘¶‚Å‚«‚È‚©‚Á‚½‚Ì‚ÅAƒpƒXƒ[ƒh•ÏX‚É¸”s‚µ‚Ü‚µ‚½B"
#define GAMESTATE_ERRMSG_CHPASSERROR      "µLªkÅÜ§ó±K½X¡C" //"ƒpƒXƒ[ƒh‚Ì•ÏX‚ª‚Å‚«‚Ü‚¹‚ñ‚Å‚µ‚½B"
#define GAMESTATE_ERRMSG_CHPASSTIMEOUT    "±K½XÅÜ§ó®É¶¡¤w¨ì¡C" //"ƒpƒXƒ[ƒh•ÏXˆ—‚ªƒ^ƒCƒ€ƒAƒEƒg‚µ‚Ü‚µ‚½B"
#define GAMESTATE_ERRMSG_DELETECHARSERVERDENY    "¦øªA¾¹©Úµ´³B²z«d°£¤Hª«¡C" //"ƒLƒƒƒ‰íœˆ—‚ªƒT[ƒo‚É‹‘”Û‚³‚ê‚Ü‚µ‚½B"
#define GAMESTATE_ERRMSG_CHARLISTDENY      "¦øªA¾¹©Úµ´³B²zÅª¨ú¤Hª«ªí¡C" //"ƒLƒƒƒ‰ƒNƒ^[•\æ“¾ˆ—‚ªƒT[ƒo‚É‹‘”Û‚³‚ê‚Ü‚µ‚½B"
#define GAMESTATE_ERRMSG_LOGINTIMEOUT			"µn¤J³B²z®É¶¡¤w¨ì¡C" //"ƒƒOƒCƒ“ˆ—‚ªƒ^ƒCƒ€ƒAƒEƒg‚µ‚Ü‚µ‚½B"
#define GAMESTATE_ERRMSG_NOTIMPLEMENTED   "©|¥¼¸Ë¸m¸Ó¥\¯à¡C" //"‚Ü‚¾‚»‚Ì‹@”\‚ÍÀ‘•‚³‚ê‚Ä‚È‚¢‚È‚è"
#endif


#endif
