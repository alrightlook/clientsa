#include <stdio.h>
#include <winsock.h>
#include <time.h>

#include"system.h"
#include "netmain.h"
#include "netproc.h"
#include "lssproto_cli.h"
#include "savedata.h"
#include "..\systeminc\process.h"
#include "pc.h"
#include "tool.h"
#include "map.h"
#include "character.h"
#include "action.h"
#include "battlemenu.h"
#include "battleProc.h"
#include "battleMap.h"
#include "menu.h"
#include "anim_tbl.h"
#include "login.h"
#include "handletime.h"
#include "field.h"
#include "t_music.h"

#define NETPROC_NOTSEND     0           // ?????? ·¢ËÍÇ°
#define NETPROC_SENDING     1           // ???????? ÖĞ
#define NETPROC_RECEIVED    2           //??????    ½ÓÊÜ

#define SET_SENDTIME( time )		time = GetTickCount()
#define CHECK_TIMEOUT( time, timeout )		\
			if( (GetTickCount() - (time)) > (timeout) )	\
			{											\
				return -1;								\
			}





#define SETSENDING  netproc_sending = NETPROC_SENDING;start_time=GetTickCount();
#define SETTIMEOUT( msg , state )  \
	if( ( GetTickCount() - start_time ) > TIMEOUT )	\
	{												\
		return;										\
	}

#define SETTIMEOUT2( msg )  \
	if( ( GetTickCount() - start_time ) > TIMEOUT )	\
	{												\
		sprintf( netprocErrmsg, msg );				\
		return -1;									\
	}

#define SETNEEDCHOOSE( mode )						\
	if( server_choosed == 0 )						\
	{												\
		return;										\
	}

/*
// ?????§f??­·????­È£@?StoneAge??1?????õ…¡p?
short selectServerIndex = -1;
// ??????????????????..??
short clientLoginStatus = 0;
// ??????™º???????????...??/ 
short charListStatus = 0;
// /????ID ??????
char userId[32];
char userPassword[32];
// ???????????
short charDelStatus = 0;
// ????
short newCharStatus = 0;
// ????
short charLoginStatus = 0;
// ????
short charLogoutStatus = 0;
*/
// Ö¸ÊıËùÑ¡ÔñµÄ·şÎñÆ÷£¨StoneAgeµÄ1»ò0µÄÇé¿öÏÂ
short selectServerIndex = -1;
// µÇÂ¼£¨µÇÂ¼Ò»¶ÎÊ±¼ä...£©
short clientLoginStatus = 0;
// ÈËÎïÁĞ±í×´Ì¬£¨ÊÕ¹ºÒ»...£©/ 
short charListStatus = 0;
// /ÓÃ»§ID ¡¤ÃÜÂë
char userId[32];
char userPassword[32];
// É¾³ıÈËÎï×´Ì¬
short charDelStatus = 0;
// ĞÂ½¨
short newCharStatus = 0;
// ¼ÇÂ¼
short charLoginStatus = 0;
// ÍË³ö
short charLogoutStatus = 0;

// GAMESTATE_CHOOSESERVER//??????????È?   ĞÅÏ¢£¬±ØĞëÅäÖÃ
// "¤ª¤ê¤ª¤ó"¤È¤«"¤Ú¤¬¤µ¤¹"¤ò¤¤¤ì¤ë¤³¤È¤Ë¤Ê¤ë¤À¤í¤¦¡£
char gamestate_chooseserver_name[128];
// GAMESTATE_LOGIN??????????È?   ĞÅÏ¢£¬±ØĞëÅäÖÃ
char gamestate_login_charname[128];
// GAMESTATE_DELETECHAR??????????È?   ĞÅÏ¢£¬±ØĞëÅäÖÃ
char gamestate_deletechar_charname[128];


// ?şFùÑä}?    ´íÎóÏûÏ¢
char netprocErrmsg[1024];

int netproc_sending = NETPROC_NOTSEND; 
DWORD start_time = 0;               // /?¥L??û¸??????§h????ÆŒ????????? TIMEOUT
//½ø³ÌÆô¶¯Ê±µÄËùÓĞÊ±¼ä¡£³¬Ê±Í¨¹ı TIMEOUT



// ¥µ©`¥Ğ©`ßx’k„IÀí ///////////////////////////////////////////////////////
//???­È£n??­»???///
//?????????­È? /?ıÌ???
// ¥µ©`¥Ğ½Ó¾A
// ³õÆÚ»¯
//·şÎñÆ÷Ñ¡Ôñ¹ı³Ì///
//Á¬½Óµ½·şÎñÆ÷/ /³õÊ¼»¯

int connectServerCounter = 0;

void initConnectServer( void )
{
	connectServerCounter = 0;
}


// ½Ó¾A„IÀí  /???•Ê???
//
// ?ì„? selectServerIndex ¡¢userId ¡¢userPassword 
//
//  ????	     0 ... ?¥i???
//				 1 ... ????OK
//				-1 ... ????
//				-2 ... ???????????????­È£`şL????????»P??õ…¡S??îÂ??ŠN???
//				-3 ... ??????????????
//				-4 ... IP?????????«Ì?
//				-5 ... connect????????
//				-6 ... select???­Ì???
//				-7 ... ????????????????  ????????
//´¦ÀíÁ¬½Ó
//
// ÉèÖÃ selectServerIndex AuserId AuserPassword 
//
//  ·µ»Ø	     0 ... ½øĞĞÖĞ
//				 1 ... Á¬½ÓOK
//				-1 ... ³¬Ê±
//				-2 ... ÓĞÃ»ÓĞÖ¸¶¨µÄ·şÎñÆ÷Ãû³Æ£¨Í¨³£ÕâÖÖÇé¿ö²»»á·¢Éú¡££©
//				-3 ... ÎŞ·¨´´½¨Ì×½Ó×Ö
//				-4 ... IPµØÖ·×ª»»ËğÊ§
//				-5 ... connectÁ¬½ÓÊ§°Ü
//				-6 ... selectÑ¡ÔñÊ§°Ü
//				-7 ... ¿Í»§¶ËµÇÂ¼Ê§°Ü¡£  ³¬¼¶ÖØµã
int connectServer( void )
{
	// ³õÆÚ»¯¤¬ÍêÁË¤·¤Æ¤Ê¤±¤ì¤ĞºÎ¤â¤·¤Ê¤¤
	if( !init_net )
		return 0;

	if( connectServerCounter == 0 )
	{
		//???????????????             ????
		//¼ÇÂ¼¿ªÊ¼Á¬½ÓÊ±¼             ¿´µ½
		start_time = GetTickCount();//»ñµÃ»úÆ÷Æô¶¯µ½ÏÖÔÚµÄÊ±¼ä
		//start_time = GetTickCount();//?­·??‘‚£cû¸?????????????

			// Ó¦¸ÃÎª·Ç×èÈûÌ×½Ó×Ö£¬Á¬½Óµ½¸Ã¹ú ·şÎñÆ  
		// ?????????ö²ã ????????????????????­È  
		char hostname[128];
		short pt;
		if( getServerInfo( selectServerIndex, hostname, &pt ) < 0 )
		{
			sprintf( netprocErrmsg, NET_ERRMSG_BADNAME );
			//connectServerCounter = 0;
			return -2;
		}

        // socket¤Ä¤¯ru
		sockfd = socket( AF_INET , SOCK_STREAM ,0 );
		if( sockfd == INVALID_SOCKET )
		{
			sprintf( netprocErrmsg, NET_ERRMSG_SOCKETERROR );
			//connectServerCounter = 0;
			return -3;
		}

        // ¥Î¥ó¥Ö¥í¥Ã¥­¥ó¥°¤Ë¤¹¤ë
#if 1
		unsigned long flg = 1;
		//ĞŞ¸ÄÌ×½Ó×ÖµÄ×èÈûÄ£Ê½ 1Îª·Ç×èÈû 0×èÈû
		ioctlsocket( sockfd, FIONBIO, &flg );//?????????????ö²â•??? 1?????ö²?0?ö²?
#endif
		// NoDelay ¤ÎˆöºÏ¤Ï¡¢setsockopt¤¹¤ë¡£ by ringo  ?ì„?setsockopt??????
		//ÉèÖÃsetsockoptµÄÊÂ¼ş
		extern BOOL NoDelay;
		if( NoDelay ){
			int flag = 1;
			if( setsockopt( sockfd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(int)) != 0 ){
				//MessageBox( NULL, "fuck", "ringo", MB_OK );
				return -100;
			}
		}
#if 0
#ifdef _DEBUG
		{
			extern int CheckNetErrror( void );
			unsigned char time = 1;
			int len = 1;
			char moji[256];
			// ?ì„?????????
			// ÉèÖÃ½ÓÊÕ³¬Ê±
			if( getsockopt( sockfd, IPPROTO_TCP, SO_RCVTIMEO, (char*) &time,  &len ) == SOCKET_ERROR ){
				CheckNetErrror();
				//return -100;
			}
			sprintf( moji, "Ê±¼ä:%u", time );
			MessageBox( NULL, moji, "ohta", MB_OK );
		}
#endif
#endif


		// ?ì„?????	// ÉèÖÃµØÖ·
		struct sockaddr_in sin;
		struct hostent *h;

		sin.sin_family = AF_INET;
		sin.sin_port = htons( pt );
		sin.sin_addr.s_addr = inet_addr( hostname );     /* accept only dot notaion  */
		if( sin.sin_addr.s_addr == -1 )
		{
			h = gethostbyname( hostname );
			if( h == NULL )
			{
				sprintf( netprocErrmsg, NET_ERRMSG_NOTGETADDR, hostname );
				closesocket( sockfd ); 
				//connectServerCounter = 0;
				return -4;
			}
			memcpy( (void*) &sin.sin_addr.s_addr , h->h_addr, sizeof( struct in_addr ) );
		}

		// Non blocking Connect
		int ret = connect( sockfd, (struct sockaddr*)&sin , sizeof( sin ) );
		if( ret == SOCKET_ERROR )
		{
			if( WSAGetLastError() == WSAEWOULDBLOCK )
			{
				// ±¾À´¤Ï¥Ö¥í¥Ã¥¯¤¹¤ë¤Ï¤º¤À¤Ã¤¿¤ó¤ä¤±¤É ?şFùÕ????????ì„? ´íÎóÓ¦¸ÃÓ¦¸Ã×èÖ¹
            }
			else
			{
				closesocket( sockfd );
				sprintf( netprocErrmsg, NET_ERRMSG_NOTCONNECT_S );
				return -5;
			}
		}

		// ¤³¤³¤Ş¤Ç¤Ç¤­¤¿¤é¥«¥¦¥ó¥¿¤ò1¤Ë¤¹¤ë  ????????  Á¬½Ó¼ÆÊı
		connectServerCounter = 1;
	}
	else
	if( connectServerCounter >= 1 && connectServerCounter <= 70 )
	{
		connectServerCounter++;
		if( connectServerCounter == 70 )
		{
			connectServerCounter = 69;
		}
		// select¤¹¤ë  ????  	// select‚·‚é  Ñ¡Ôñ
		fd_set rfds , wfds , efds;
		struct timeval tm;
		tm.tv_sec = 0;
		tm.tv_usec = 0;
		FD_ZERO( &rfds );
		FD_ZERO( &wfds );
		FD_ZERO( &efds );
		FD_SET( sockfd , &rfds );
		FD_SET( sockfd , &wfds );
		FD_SET( sockfd , &efds );
		int a = select(  2 , &rfds , &wfds , &efds,  &tm );
		if( FD_ISSET( sockfd, &wfds ) )
		{
			connectServerCounter = 71;
			//?????????ëÂ¡¥[ß„??¯£???§c???¤³¤ìÒÔ½µ¤Ï¥Í¥Ã¥È¥ï©`¥¯¥ë©`¥Á¥ó¤¬Éú¤­¤ë¡£¤½¤Î¤¿¤á¤Î¥Õ¥é¥°Á¢¤Æ
				//´ÓÏÖÔÚµÄÉú»îÍøÂç³ÌĞò
			server_choosed = 1;
		}
		if( FD_ISSET( sockfd , &efds ) )
		{
			sprintf( netprocErrmsg, NET_ERRMSG_NOTCONNECT );
			closesocket( sockfd ); 
			//connectServerCounter = 0;
			return -6; 
		}
    }
	else
	if( connectServerCounter >= 71 && connectServerCounter <= 80 )
	{
		// ClientLogin¤¹¤ë ????????   Á¬½Ó¡£¡£
		if( connectServerCounter == 71 )
		{
			// ¹Ø±ÕÔÚÏß×´Ì¬
			clientLoginStatus = 0;	// ????????????
			lssproto_ClientLogin_send( sockfd, userId, userPassword );
			netproc_sending = NETPROC_SENDING;
		}
		if( netproc_sending == NETPROC_RECEIVED )
		{
			//  recv ????????ÒÑ¾­³É¹¦
			if( clientLoginStatus )
			{
				connectServerCounter = 81;
			}
			else
			{
				netproc_sending = NETPROC_NOTSEND;
				sprintf( netprocErrmsg, NET_ERRMSG_LOGINFAIL );
				closesocket( sockfd ); 
				return -7;
			}
		}

		connectServerCounter++;
		if( connectServerCounter == 81 )
			connectServerCounter = 80;
    }
	else
	if( connectServerCounter >= 81 && connectServerCounter <= 98 )
	{
		// ¤Ş¤ï¤¹¤À¤±  ????×ª¶¯
		connectServerCounter ++;
    }
	else
	if( connectServerCounter == 99 )
	{
		//ChangeProc( PROC_CHAR_SELECT );
		// ÇĞ»»»Ø´«ËÍÓë·ñ ÎÒ¿ÉÒÔÌ¸Ì¸Ê²Ã´ 

		// ??????????????????????????? ¤Ş¤¿ËÍĞÅ¤·¤Æ¤¤¤Ê¤¤¥â©`¥É¤Ë‘ø¤¹¡£ºÎ»Ø¤·¤Æ¤â¤¤¤¤¤è¤¦¤Ë
		netproc_sending = NETPROC_NOTSEND;
		//connectServerCounter = 0;
		return 1;
	}

	// ÖÆÏŞ•rég¤¬¤¹¤®¤¿¤é¡¢¥¨¥é©`×´‘B¤ËÒÆĞĞ¡¢¤½¤Î¤¢¤È¥¨¥é©`½KÁË
	//???êû????ÊC??????í¢????????şFù¥??¯¶şFùÏ???
	//Ì«¶àµÄÊ±¼äÏŞÖÆ£¬¹ú¼Ò¹ı¶ÉµÄ´íÎó£¬Ôò´íÎóÍË³ö
	SETTIMEOUT2( NET_ERRMSG_CONNECTTIMEOUT );

	return 0;
}




// ¥¯¥é¥¤¥¢¥ó¥È¥í¥°¥¤¥ó ???????? ????
void clientLogin( void )
{
	char cdkey[64];
	char passwd[64];

	getCdkey( cdkey );
	getPassword( passwd );
	lssproto_ClientLogin_send( sockfd , cdkey , passwd );
	netproc_sending = NETPROC_SENDING;
}


// ¥í¥°¥¤¥óÊÜĞÅ„IÀí  ¿Í»§µÇÂ½ ÖØµã
void lssproto_ClientLogin_recv( int fd, char *result )
{
	if( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;	

		if( strcmp( result, OKSTR ) == 0 )
		{
			clientLoginStatus = 1;

			// ¬FÔÚ¤ÎÈÕ•r¤òÈ¡µÃ
			time( &serverAliveLongTime );
			serverAliveTime = localtime( &serverAliveLongTime );
		}
	}
}




// ¥­¥ã¥é¥ê¥¹¥ÈÈ¡µÃ„IÀí ///////////////////////////////////////////////////

// ¥­¥ã¥é¤Î¥ê¥¹¥ÈÈ¡µÃ¥á¥Ã¥»©`¥¸¤òËÍ¤ë
void charListStart( void )
{
	int i;

	for( i = 0; i < MAXCHARACTER; i++ )
	{
		resetCharacterList( i );
	}

	charListStatus = 0;

	lssproto_CharList_send( sockfd );

	SETSENDING;
}


// ÊÜĞÅ´ı¤Á
//   ‘ø¤ê‚£º  0 ... ÊÜĞÅ´ı¤Á
//             1 ... ÊÜĞÅÍêÁË
//            -1 ... ¥¿¥¤¥à¥¢¥¦¥È
//            -2 ... ¥¨¥é©`¤¬¢¤Ã¤Æ¤­¤¿
//            -3 ... ¥á¥ó¥Æ¥Ê¥ó¥¹ÖĞ
//            -4 ... ¥æ©`¥¶ÕJÔ^Ê§”¡
int charListProc( void )
{
	if( netproc_sending == NETPROC_RECEIVED )
	{
		netproc_sending = NETPROC_NOTSEND;
		if( charListStatus == 1 )
		{
			return 1;
		}
		else
		if( charListStatus == 2 )
		{
			return -3;
		}
#if 0
		else
		if( charListStatus == 3 )
		{
			return -4;
		}
#endif
		else
		{
			return -2;
		}
	}

	SETTIMEOUT2( NET_ERRMSG_CHARLISTTIMEOUT );

	return 0;
}


// ¥µ©`¥Ğ©`¤Ë±£´æ¤µ¤ì¤Æ¤¤¤ë¥­¥ã¥é¥¯¥¿¤Î¥ê¥¹¥È¤ÎÊÜĞÅ
void lssproto_CharList_recv( int fd, char *result, char *data )
{
	if( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;

		if( strcmp( result, SUCCESSFULSTR ) != 0 )
		{
			if( strcmp( data, "OUTOFSERVICE" ) == 0 )
			{
				// ¬FÔÚ¥á¥ó¥Æ¥Ê¥ó¥¹ÖĞ
				charListStatus = 2;
			}
#if 0
			else
			if( strcmp( data, "???????" ) == 0 )
			{
				// ¥æ©`¥¶ÕJÔ^Ê§”¡
				charListStatus = 3;
			}
#endif
			return;
		}

		charListStatus = 1;

		// ŒgëH¤ËÇéˆó¤ò¥»¥Ã¥È¤¹¤ë
		char nm[1024], opt[1024];
		int i;
		for( i=0; i < MAXCHARACTER; i++ )
		{
			strcpy( nm ,"" );
			strcpy( opt , "" );
			getStringToken( data, '|', i*2+1, sizeof( nm )-1 , nm );
			getStringToken( data, '|', i*2+2, sizeof( opt )-1, opt );

			setCharacterList( nm , opt );
		}
	}
}



// Ö¸¶¨¥­¥ã¥é¤Ç¥í¥°¥¤¥ó ///////////////////////////////////////////////////
// ¥í¥°¥¤¥óé_Ê¼
void charLoginStart( void )
{
	charLoginStatus = 0;

	lssproto_CharLogin_send( sockfd, gamestate_login_charname );

	SETSENDING;
}


// ¥í¥°¥¤¥óÖĞ
//   ‘ø¤ê‚£º 0 ... ÊÜĞÅ´ı¤Á / 1 ... ÊÜĞÅÍêÁË / -1 ... ¥¿¥¤¥à¥¢¥¦¥È / -2 ... ¥¨¥é©`¤¬¢¤Ã¤Æ¤­¤¿
int charLoginProc( void )
{
	if( !server_choosed )
		return 0;

	if( netproc_sending == NETPROC_RECEIVED )
	{
		netproc_sending = NETPROC_NOTSEND;
		if( charLoginStatus == 1 )
		{
			return 1;
		}
		else
		{
			return -2;
		}
	}

	SETTIMEOUT2( NET_ERRMSG_LOGINTIMEOUT );

	return 0;
}


// ¥­¥ã¥é¥í¥°¥¤¥óÊÜĞÅ
void lssproto_CharLogin_recv( int fd, char* result, char* data )
{
	if( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;

		if( strcmp( result, SUCCESSFULSTR ) == 0 )
		{
			charLoginStatus = 1;
        }
	}
}




// ¥í¥°¥¢¥¦¥È /////////////////////////////////////////////////////////////
// ¥í¥°¥¢¥¦¥Èé_Ê¼
void charLogoutStart( void )
{
	charLogoutStatus = 0;

	lssproto_CharLogout_send( sockfd );

	SETSENDING;
}


// ¥í¥°¥¢¥¦¥ÈÖĞ
//   ‘ø¤ê‚£º	 0 ... ¥í¥°¥¢¥¦¥ÈÖĞ
//				 1 ... ¥í¥°¥¢¥¦¥ÈÍêÁË
//				-1 ... ¥¿¥¤¥à¥¢¥¦¥È
//				-2 ... ¥¨¥é©`¤¬¢¤Ã¤Æ¤­¤¿
int charLogoutProc( void )
{
	if( netproc_sending == NETPROC_RECEIVED )
	{
		netproc_sending = NETPROC_NOTSEND;
		if( charLogoutStatus == 1 )
		{
			return 1;
		}
		else
		{
			return -2;
		}
    }

	SETTIMEOUT2( NET_ERRMSG_LOGOUTTIMEOUT );

	return 0;
}


void lssproto_CharLogout_recv( int fd, char *result, char *data )
{
	if( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;
		if( strcmp( result, SUCCESSFULSTR ) == 0 )
		{
			charLogoutStatus = 1;
		}
	}
}




// ¥¹¥Æ©`¥¿¥¹Çéˆó¤òÊÜĞÅ¤¹¤ë ///////////////////////////////////////////////
//   ¥¹¥Æ©`¥¿¥¹Çéˆó¤òÊÜĞÅ¤¹¤ë¡£
//   ¤³¤Î¥³©`¥É¤Ï¥×¥í¥È¥³¥ë¥Õ¥¡¥¤¥ë¤ÎÄÚÈİ¤òÖÒŒg¤ËŒg×°¤·¤Æ¤¤¤Ê¤±¤ì¤Ğ¤Ê¤é¤Ê¤¤¡£
//   ¥³©`¥É¤¬¤Á¤ç¤Ã¤È¤Ç¤âß`·´¤·¤Æ¤¤¤¿¤é¤½¤ì¤Ï¥Ğ¥°¤Ç¤¢¤ë¤È¤·¤Æ¤è¤¤¡£
//×´Ì¬ĞÅÏ¢½ÓÊÕ////////
//×´Ì¬ĞÅÏ¢/ /½ÓÊÕ¡£/ /Õâ´úÂëĞ­ÒéÎÄ¼şµÄÄÚÈİÖÒÊµµØÊµÊ©×Å±ØĞë¡£
//´úÂëÉÔÎ¢Î¥·´ÁËµÄ»°£¬ÄÇÊÇ´íÎóµÄºÃ¡£

#define S_DELIM '|'						// ¥Ç¥ê¥ß¥¿  ¶¨½ç·û

void lssproto_S_recv( int fd, char *data ) 
{
	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	switch( data[0] )
	{
		// ×ù˜Ë
		case 'C':
			{
			int fl;
			int maxx;
			int maxy;
			int gx;
			int gy;

			// ¥Õ¥í¥¢‰ä¸ü¥Õ¥é¥°(¥Ş¥Ã¥×¤òÕi¤ß¤³¤à¤è¤¦¤Ë)
			floorChangeFlag = TRUE;
			if( !loginFlag && ProcNo == PROC_GAME )	// ¥í¥°¥¤¥ó•r¤ÏŒgĞĞ¤·¤Ê¤¤
			{
				// ¥ï©`¥×¥¤¥Ù¥ó¥È¤Ç¤¹¤Ç¤ËŒgĞĞ¤µ¤ì¤Æ¤¤¤ì¤ĞºÎ¤â¤·¤Ê¤¤¡£
				if( !warpEffectFlag )
				{
					// ¥ï©`¥×Ñİ³öŒgĞĞ
					SubProcNo = 200;
					// ¬F•rµã¤Î»­Ãæ¤ò×÷¤ë
					warpEffectProc();
#if 1
					// ¥Ş¥Ã¥×¥¦¥£¥ó¥É¥¦¥Õ¥é¥°¥Ğ¥Ã¥¯¥¢¥Ã¥×
					if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
#endif
				}

				// PC¥­¥ã¥é¥ê¥»¥Ã¥È
				resetPc();
				warpEffectFlag = FALSE;
				warpEffectStart = TRUE;
			}


			// ¥«¥Æ¥´¥êÎÄ×Ö¤òïw¤Ğ¤¹//Êı¾İ·ÖÀàÌáÈ¡
			data++;

			fl   = getIntegerToken( data, S_DELIM, 1 );
			maxx = getIntegerToken( data, S_DELIM, 2 );
			maxy = getIntegerToken( data, S_DELIM, 3 );
			gx   = getIntegerToken( data, S_DELIM, 4 );
			gy   = getIntegerToken( data, S_DELIM, 5 );

			// ¥Ş¥Ã¥×Î»ÖÃÔO¶¨  µØÍ¼Î»ÖÃÉè¶¨
			setMap( fl, gx, gy );
			// ĞÂ¤·¤¤¥Ş¥Ã¥×¤Ê¤é¥Õ¥¡¥¤¥ë¤ò×÷¤ë  Èç¹û/ĞÂµØÍ¼ÎÄ¼ş  ÖÆ×÷/  
			createMap( fl, maxx, maxy );
			// ¥Ş¥Ã¥×¥µ¥¤¥º¤ÎÔO¶¨  ³ß´ç/ /µØÍ¼µÄÉè¶¨
			nowFloorGxSize = maxx;
			nowFloorGySize = maxy;


			// ¥­¥ã¥é¹ÜÀí¥Æ©`¥Ö¥ë¥ê¥»¥Ã¥È  ½ÇÉ«¹ÜÀí±í¸´Î»¶¨
			resetCharObj();

			// ¿Õ¥Ş¥Ã¥×¥Õ¥é¥°  ¿ÕµØÍ¼×ªÕÛµã£¿
			mapEmptyFlag = FALSE;

			// ¥¨¥ó¥«¥¦¥ó¥ÈÂÊ¤Î³õÆÚ»¯  ÓöµĞÂÊµÄ³õÆÚ»¯
			nowEncountPercentage = minEncountPercentage;
			nowEncountExtra = 0;

			resetMap();
			}
            break;

		// PC¥­¥ã¥é¤ÎIDÔO¶¨
		case 'D':
			// ¥«¥Æ¥´¥êÎÄ×Ö¤òïw¤Ğ¤¹
			data++;

			setPcId( getIntegerToken( data, S_DELIM, 1 ) );
			serverTime = getIntegerToken( data, S_DELIM, 2 );
			clientTime = time( NULL );
			// £Ó£Á•rég¤òÈ¡µÃ
			RealTimeToSATime( &SaTime );
			//£Ó£Á•rég¤Ç½ñ¤Î•régÇø·Ö¤òµÃ¤ë
			SaTimeZoneNo = getLSTime ( &SaTime );
			// ¥Ñ¥ì¥Ã¥È¥Á¥§¥ó¥¸
			PaletteChange( SaTimeZoneNo, 0 );
			
			break;

		// ¥Ñ¥é¥á©`¥¿
		case 'P':
			{
			char name[256];
			char freeName[256];
			//char status[256];

			// ¥«¥Æ¥´¥êÎÄ×Ö¤òïw¤Ğ¤¹
			data++;

			int i;
			int kubun;
			unsigned int mask;

			kubun		= getInteger62Token( data, S_DELIM, 1 );

			// È«¥Ñ¥é¥á©`¥¿ÊÜĞÅ
			if( kubun == 1 )
			{
				pc.hp		= getIntegerToken( data, S_DELIM, 2 );		// 0x00000002
				pc.maxHp	= getIntegerToken( data, S_DELIM, 3 );		// 0x00000004
				pc.mp		= getIntegerToken( data, S_DELIM, 4 );		// 0x00000008
				pc.maxMp	= getIntegerToken( data, S_DELIM, 5 );		// 0x00000010
				pc.vital	= getIntegerToken( data, S_DELIM, 6 );		// 0x00000020
				pc.str		= getIntegerToken( data, S_DELIM, 7 );		// 0x00000040
				pc.tgh		= getIntegerToken( data, S_DELIM, 8 );		// 0x00000080
				pc.dex		= getIntegerToken( data, S_DELIM, 9 );		// 0x00000100
				pc.exp		= getIntegerToken( data, S_DELIM, 10);		// 0x00000200
				pc.maxExp	= getIntegerToken( data, S_DELIM, 11 );		// 0x00000400
				pc.level	= getIntegerToken( data, S_DELIM, 12 );		// 0x00000800
				pc.atk		= getIntegerToken( data, S_DELIM, 13 );		// 0x00001000
				pc.def		= getIntegerToken( data, S_DELIM, 14 );		// 0x00002000
				pc.quick	= getIntegerToken( data, S_DELIM, 15 );		// 0x00004000
				pc.charm	= getIntegerToken( data, S_DELIM, 16 );		// 0x00008000
				pc.luck		= getIntegerToken( data, S_DELIM, 17 );		// 0x00010000
				pc.earth	= getIntegerToken( data, S_DELIM, 18 );		// 0x00020000
				pc.water	= getIntegerToken( data, S_DELIM, 19 );		// 0x00040000
				pc.fire		= getIntegerToken( data, S_DELIM, 20 );		// 0x00080000
				pc.wind		= getIntegerToken( data, S_DELIM, 21 );		// 0x00100000
				pc.gold		= getIntegerToken( data, S_DELIM, 22 );		// 0x00200000
				pc.titleNo	= getIntegerToken( data, S_DELIM, 23 );		// 0x00400000
				pc.dp		= getIntegerToken( data, S_DELIM, 24 );		// 0x00800000

				getStringToken( data, S_DELIM, 25, sizeof( name )-1, name );// 0x01000000
				makeRecvString( name );
				if( strlen( name ) <= CHAR_NAME_LEN )
					strcpy( pc.name, name );
				getStringToken( data, S_DELIM, 26, sizeof( freeName )-1, freeName );// 0x02000000
				makeRecvString( freeName );
				if( strlen( freeName ) <= CHAR_FREENAME_LEN )
					strcpy( pc.freeName, freeName );
				//getStringToken( data, S_DELIM, 27, sizeof( status )-1, status );// 0x04000000
				//setCharStatus( &pc.status, status );	// ¥¹¥Æ©`¥¿¥¹
			}
			else
			{
				mask = 2;
				i = 2;
				for( ; mask > 0; mask <<= 1 )
				{
					if( kubun & mask )
					{
						if( mask == 0x00000002 )
						{
							pc.hp = getIntegerToken( data, S_DELIM, i );// 0x00000002
							i++;
						}
						else
						if( mask == 0x00000004 )
						{
							pc.maxHp = getIntegerToken( data, S_DELIM, i );// 0x00000004
							i++;
						}
						else
						if( mask == 0x00000008 )
						{
							pc.mp = getIntegerToken( data, S_DELIM, i );// 0x00000008
							i++;
						}
						else
						if( mask == 0x00000010 )
						{
							pc.maxMp = getIntegerToken( data, S_DELIM, i );// 0x00000010
							i++;
						}
						else
						if( mask == 0x00000020 )
						{
							pc.vital = getIntegerToken( data, S_DELIM, i );// 0x00000020
							i++;
						}
						else
						if( mask == 0x00000040 )
						{
							pc.str = getIntegerToken( data, S_DELIM, i );// 0x00000040
							i++;
						}
						else
						if( mask == 0x00000080 )
						{
							pc.tgh = getIntegerToken( data, S_DELIM, i );// 0x00000080
							i++;
						}
						else
						if( mask == 0x00000100 )
						{
							pc.dex = getIntegerToken( data, S_DELIM, i );// 0x00000100
							i++;
						}
						else
						if( mask == 0x00000200 )
						{
							pc.exp = getIntegerToken( data, S_DELIM, i );// 0x00000200
							i++;
						}
						else
						if( mask == 0x00000400 )
						{
							pc.maxExp = getIntegerToken( data, S_DELIM, i );// 0x00000400
							i++;
						}
						else
						if( mask == 0x00000800 )
						{
							pc.level = getIntegerToken( data, S_DELIM, i );// 0x00000800
							i++;
						}
						else
						if( mask == 0x00001000 )
						{
							pc.atk = getIntegerToken( data, S_DELIM, i );// 0x00001000
							i++;
						}
						else
						if( mask == 0x00002000 )
						{
							pc.def = getIntegerToken( data, S_DELIM, i );// 0x00002000
							i++;
						}
						else
						if( mask == 0x00004000 )
						{
							pc.quick = getIntegerToken( data, S_DELIM, i );// 0x00004000
							i++;
						}
						else
						if( mask == 0x00008000 )
						{
							pc.charm = getIntegerToken( data, S_DELIM, i );// 0x00008000
							i++;
						}
						else
						if( mask == 0x00010000 )
						{
							pc.luck = getIntegerToken( data, S_DELIM, i );// 0x00010000
							i++;
						}
						else
						if( mask == 0x00020000 )
						{
							pc.earth = getIntegerToken( data, S_DELIM, i );// 0x00020000
							i++;
						}
						else
						if( mask == 0x00040000 )
						{
							pc.water = getIntegerToken( data, S_DELIM, i );// 0x00040000
							i++;
						}
						else
						if( mask == 0x00080000 )
						{
							pc.fire = getIntegerToken( data, S_DELIM, i );// 0x00080000
							i++;
						}
						else
						if( mask == 0x00100000 )
						{
							pc.wind = getIntegerToken( data, S_DELIM, i );// 0x00100000
							i++;
						}
						else
						if( mask == 0x00200000 )
						{
							pc.gold = getIntegerToken( data, S_DELIM, i );// 0x00200000
							i++;
						}
						else
						if( mask == 0x00400000 )
						{
							pc.titleNo = getIntegerToken( data, S_DELIM, i );// 0x00400000
							i++;
						}
						else
						if( mask == 0x00800000 )
						{
							pc.dp = getIntegerToken( data, S_DELIM, i );// 0x00800000
							i++;
						}
						else
						if( mask == 0x01000000 )
						{
							getStringToken( data, S_DELIM, i, sizeof( name )-1, name );// 0x01000000
							makeRecvString( name );
							if( strlen( name ) <= CHAR_NAME_LEN )
								strcpy( pc.name, name );
							i++;
						}
						else
						if( mask == 0x02000000 )
						{
							getStringToken( data, S_DELIM, i, sizeof( freeName )-1, freeName );// 0x02000000
							makeRecvString( freeName );
							if( strlen( freeName ) <= CHAR_FREENAME_LEN )
								strcpy( pc.freeName, freeName );
							i++;
						}
					}
				}
			}

			updataPcAct();

			if( (pc.status & CHR_STATUS_LEADER) != 0
			 && party[0].useFlag != 0 )
			{
				party[0].level = pc.level;
				party[0].maxHp = pc.maxHp;
				party[0].hp = pc.hp;
				strcpy( party[0].name, pc.name );
			}
			}
			break;

		// HP,MP,EXP
		case 'M':
			// ¥«¥Æ¥´¥êÎÄ×Ö¤òïw¤Ğ¤¹
			data++;

			pc.hp  = getIntegerToken( data, '|', 1 );
			pc.mp  = getIntegerToken( data, '|', 2 );
			pc.exp = getIntegerToken( data, '|', 3 );
			updataPcAct();

			if( (pc.status & CHR_STATUS_LEADER) != 0
			 && party[0].useFlag != 0 )
			{
				party[0].hp = pc.hp;
			}
			break;

		// ¥Ú¥Ã¥È¤Î¥Ñ¥é¥á©`¥¿
		case 'K':
			{
			char name[256];
			char freeName[256];
			//char status[256];
			int no;
			int kubun;
			no = data[1] - '0';

			// ¥«¥Æ¥´¥êÎÄ×Ö¤òïw¤Ğ¤¹
			data += 3;

			// ¥Ú¥Ã¥È¤¤¤Ê¤¤•r¤Ï½KÁË
			kubun = getInteger62Token( data, S_DELIM, 1 );
			if( kubun == 0 )
			{
				pet[no].useFlag = 0;
				break;
			}

			pet[no].useFlag = 1;

			int i;
			unsigned int mask;

			if( kubun == 1 )
			{
				pet[no].graNo	= getIntegerToken( data, S_DELIM, 2 );		// 0x00000002

				pet[no].hp		= getIntegerToken( data, S_DELIM, 3 );		// 0x00000004
				pet[no].maxHp	= getIntegerToken( data, S_DELIM, 4 );		// 0x00000008
				pet[no].mp		= getIntegerToken( data, S_DELIM, 5 );		// 0x00000010
				pet[no].maxMp	= getIntegerToken( data, S_DELIM, 6 );		// 0x00000020

				pet[no].exp		= getIntegerToken( data, S_DELIM, 7 );		// 0x00000040
				pet[no].maxExp	= getIntegerToken( data, S_DELIM, 8 );		// 0x00000080
				pet[no].level	= getIntegerToken( data, S_DELIM, 9 );		// 0x00000100

				pet[no].atk		= getIntegerToken( data, S_DELIM, 10);		// 0x00000200
				pet[no].def		= getIntegerToken( data, S_DELIM, 11 );		// 0x00000400
				pet[no].quick	= getIntegerToken( data, S_DELIM, 12 );		// 0x00000800
				pet[no].ai		= getIntegerToken( data, S_DELIM, 13 );		// 0x00001000

				pet[no].earth	= getIntegerToken( data, S_DELIM, 14 );		// 0x00002000
				pet[no].water	= getIntegerToken( data, S_DELIM, 15 );		// 0x00004000
				pet[no].fire	= getIntegerToken( data, S_DELIM, 16 );		// 0x00008000
				pet[no].wind	= getIntegerToken( data, S_DELIM, 17 );		// 0x00010000

				pet[no].maxSkill= getIntegerToken( data, S_DELIM, 18 );		// 0x00020000

				pet[no].changeNameFlag = getIntegerToken( data, S_DELIM, 19 );// 0x00040000

				getStringToken( data, S_DELIM, 20, sizeof( name )-1, name );// 0x00080000
				makeRecvString( name );
				if( strlen( name ) <= PET_NAME_LEN )
					strcpy( pet[no].name, name );
				getStringToken( data, S_DELIM, 21, sizeof( freeName )-1, freeName );// 0x00100000
				makeRecvString( freeName );
				if( strlen( freeName ) <= PET_NAME_LEN )
					strcpy( pet[no].freeName, freeName );
//				getStringToken( data, S_DELIM, 22, sizeof( status )-1, status );
//				setCharStatus( &pet[no].status, status );	// ¥¹¥Æ©`¥¿¥¹
			}
			else
			{
				mask = 2;
				i = 2;
				for( ; mask > 0; mask <<= 1 )
				{
					if( kubun & mask )
					{
						if( mask == 0x00000002 )
						{
							pet[no].graNo = getIntegerToken( data, S_DELIM, i );// 0x00000002
							i++;
						}
						else
						if( mask == 0x00000004 )
						{
							pet[no].hp = getIntegerToken( data, S_DELIM, i );// 0x00000004
							i++;
						}
						else
						if( mask == 0x00000008 )
						{
							pet[no].maxHp = getIntegerToken( data, S_DELIM, i );// 0x00000008
							i++;
						}
						else
						if( mask == 0x00000010 )
						{
							pet[no].mp = getIntegerToken( data, S_DELIM, i );// 0x00000010
							i++;
						}
						else
						if( mask == 0x00000020 )
						{
							pet[no].maxMp = getIntegerToken( data, S_DELIM, i );// 0x00000020
							i++;
						}
						else
						if( mask == 0x00000040 )
						{
							pet[no].exp = getIntegerToken( data, S_DELIM, i );// 0x00000040
							i++;
						}
						else
						if( mask == 0x00000080 )
						{
							pet[no].maxExp = getIntegerToken( data, S_DELIM, i );// 0x00000080
							i++;
						}
						else
						if( mask == 0x00000100 )
						{
							pet[no].level = getIntegerToken( data, S_DELIM, i );// 0x00000100
							i++;
						}
						else
						if( mask == 0x00000200 )
						{
							pet[no].atk = getIntegerToken( data, S_DELIM, i );// 0x00000200
							i++;
						}
						else
						if( mask == 0x00000400 )
						{
							pet[no].def = getIntegerToken( data, S_DELIM, i );// 0x00000400
							i++;
						}
						else
						if( mask == 0x00000800 )
						{
							pet[no].quick = getIntegerToken( data, S_DELIM, i );// 0x00000800
							i++;
						}
						else
						if( mask == 0x00001000 )
						{
							pet[no].ai = getIntegerToken( data, S_DELIM, i );// 0x00001000
							i++;
						}
						else
						if( mask == 0x00002000 )
						{
							pet[no].earth = getIntegerToken( data, S_DELIM, i );// 0x00002000
							i++;
						}
						else
						if( mask == 0x00004000 )
						{
							pet[no].water = getIntegerToken( data, S_DELIM, i );// 0x00004000
							i++;
						}
						else
						if( mask == 0x00008000 )
						{
							pet[no].fire = getIntegerToken( data, S_DELIM, i );// 0x00008000
							i++;
						}
						else
						if( mask == 0x00010000 )
						{
							pet[no].wind = getIntegerToken( data, S_DELIM, i );// 0x00010000
							i++;
						}
						else
						if( mask == 0x00020000 )
						{
							pet[no].maxSkill = getIntegerToken( data, S_DELIM, i );// 0x00020000
							i++;
						}
						else
						if( mask == 0x00040000 )
						{
							pet[no].changeNameFlag = getIntegerToken( data, S_DELIM, i );// 0x00040000
							i++;
						}
						else
						if( mask == 0x00080000 )
						{
							getStringToken( data, S_DELIM, i, sizeof( name )-1, name );// 0x00080000
							makeRecvString( name );
							if( strlen( name ) <= PET_NAME_LEN )
								strcpy( pet[no].name, name );
							i++;
						}
						else
						if( mask == 0x00100000 )
						{
							getStringToken( data, S_DELIM, i, sizeof( freeName )-1, freeName );// 0x00100000
							makeRecvString( freeName );
							if( strlen( freeName ) <= PET_NAME_LEN )
							strcpy( pet[no].freeName, freeName );
							i++;
						}
					}
				}
			}

			}
			break;

		// ¥¨¥ó¥«¥¦¥ó¥ÈÂÊ¤ÎÔO¶¨
		case 'E':
			{
			// ¥«¥Æ¥´¥êÎÄ×Ö¤òïw¤Ğ¤¹
			data++;

			minEncountPercentage = getIntegerToken( data, S_DELIM, 1 );
			maxEncountPercentage = getIntegerToken( data, S_DELIM, 2 );
			nowEncountPercentage = minEncountPercentage;
			}
			break;

		// …âĞg¤Î¥Ñ¥é¥á©`¥¿
		case 'J':
			{
			char name[256];
			char memo[256];
			int no;
			no = data[1] - '0';

			// ¥«¥Æ¥´¥êÎÄ×Ö¤òïw¤Ğ¤¹
			data += 3;

			// …âĞg¤¬¤Ê¤¤•r¤Ï½KÁË
			magic[no].useFlag = getIntegerToken( data, S_DELIM, 1 );
			if( magic[no].useFlag == 0 )
				break;

			magic[no].mp		= getIntegerToken( data, S_DELIM, 2 );
			magic[no].field		= getIntegerToken( data, S_DELIM, 3 );
			magic[no].target	= getIntegerToken( data, S_DELIM, 4 );
			if( magic[no].target >= 100 )
			{
				magic[no].target %= 100;
				magic[no].deadTargetFlag = 1;
			}
			else
			{
				magic[no].deadTargetFlag = 0;
			}

			getStringToken( data, S_DELIM, 5, sizeof( name )-1, name );
			makeRecvString( name );
			if( strlen( name ) <= sizeof( magic[no].name )-1 )
			{
				strcpy( magic[no].name, name );
			}
			getStringToken( data, S_DELIM, 6, sizeof( memo )-1, memo );
			makeRecvString( memo );
			if( strlen( memo ) <= sizeof( magic[no].memo )-1 )
			{
				strcpy( magic[no].memo, memo );
			}
			}
			break;

		// ÖÙég¤Î¥Ñ¥é¥á©`¥¿
		case 'N':
			{
			ACTION *ptAct;
			char name[256];
			int no;
			int kubun;
			int i;
			int checkPartyCount;


			no = data[1] - '0';

			// ¥«¥Æ¥´¥êÎÄ×Ö¤òïw¤Ğ¤¹
			data += 3;

			// ÖÙégÇéˆó¤¬¤Ê¤¤•r¤Ï½KÁË
			kubun = getInteger62Token( data, S_DELIM, 1 );
			if( kubun == 0 )
			{
				// ¤µ¤Ã¤­¤Ş¤ÇÖÙég¤¬¤¤¤¿¤éáá„IÀí¤·¤Æ½KÁË
				if( party[no].useFlag != 0 && party[no].id != pc.id )
				{
					ptAct = getCharObjAct( party[no].id );
					if( ptAct != NULL )
					{
						delCharParty( ptAct );
					}
				}
				party[no].useFlag	= 0;
				party[no].ptAct		= NULL;

				// Õl¤«¤¬’i¤±¤¿¤È¤­²Ğ¤ê¤¬×Ô·Ö¤À¤±¤Ê¤é¥Ñ©`¥Æ¥£¤Ï½âÉ¢
				checkPartyCount = 0;
				for( i = 0; i < MAX_PARTY; i++ )
				{
					if( party[i].useFlag != 0 )
						checkPartyCount++;
				}
				if( checkPartyCount <= 1 )
				{
					partyModeFlag = 0;
					clearPartyParam();
				}
				break;
			}

			// ¤³¤³¤òÍ¨¤ë¤È¤¤¤¦¤³¤È¤ÏPC¤Ï¥Ñ©`¥Æ¥£¤ËÈë¤Ã¤¿¤³¤È¤Ë¤Ê¤ë
			partyModeFlag = 1;
			prSendFlag = 0;		// ¤¿¤Ş¤ËPR¤Î·µÊÂ¤¬¤³¤Ê¤¤¤Î¤Ç¤³¤³¤Ç¥Õ¥é¥°¤òÏÂ¤í¤¹¡£

			party[no].useFlag	= 1;

			unsigned int mask;

			if( kubun == 1 )
			{
				party[no].id		= getIntegerToken( data, S_DELIM, 2 );	// 0x00000002
				party[no].level		= getIntegerToken( data, S_DELIM, 3 );	// 0x00000004
				party[no].maxHp		= getIntegerToken( data, S_DELIM, 4 );	// 0x00000008
				party[no].hp		= getIntegerToken( data, S_DELIM, 5 );	// 0x00000010
				party[no].mp		= getIntegerToken( data, S_DELIM, 6 );	// 0x00000020

				getStringToken( data, S_DELIM, 7, sizeof( name )-1, name );	// 0x00000040
				makeRecvString( name );
				if( strlen( name ) <= sizeof( party[no].name )-1 )
				{
					strcpy( party[no].name, name );
				}
				else
				{
					strcpy( party[no].name, "???" );
				}
			}
			else
			{
				mask = 2;
				i = 2;
				for( ; mask > 0; mask <<= 1 )
				{
					if( kubun & mask )
					{
						if( mask == 0x00000002 )
						{
							party[no].id = getIntegerToken( data, S_DELIM, i );// 0x00000002
							i++;
						}
						else
						if( mask == 0x00000004 )
						{
							party[no].level = getIntegerToken( data, S_DELIM, i );// 0x00000004
							i++;
						}
						else
						if( mask == 0x00000008 )
						{
							party[no].maxHp = getIntegerToken( data, S_DELIM, i );// 0x00000008
							i++;
						}
						else
						if( mask == 0x00000010 )
						{
							party[no].hp = getIntegerToken( data, S_DELIM, i );// 0x00000010
							i++;
						}
						else
						if( mask == 0x00000020 )
						{
							party[no].mp = getIntegerToken( data, S_DELIM, i );// 0x00000020
							i++;
						}
						else
						if( mask == 0x00000040 )
						{
							getStringToken( data, S_DELIM, i, sizeof( name )-1, name );// 0x00000040
							makeRecvString( name );
							if( strlen( name ) <= sizeof( party[no].name )-1 )
							{
								strcpy( party[no].name, name );
							}
							else
							{
								strcpy( party[no].name, "???" );
							}
							i++;
						}
					}
				}
			}

			if( party[no].id != pc.id )
			{
				ptAct = getCharObjAct( party[no].id );
				if( ptAct != NULL )
				{
					party[no].ptAct = ptAct;
					setCharParty( ptAct );
					// NPC¤¬¥ê©`¥À©`¤Î•r
					if( no == 0 )
					{
						setCharLeader( ptAct );
					}
#if 0
					else
					{
						// ¥ê©`¥À©`¤¬£Î£Ğ£Ã¤Î•r
						if( party[0].id != pc.id )
						{
							// ¥ê©`¥À©`¤Î¤¤¤ëˆöËù¤ËÒÆ„Ó
							if( party[0].ptAct != NULL )
							{
								stockCharMovePoint( ptAct, party[0].ptAct->gx, party[0].ptAct->gy );
							}
						}
						// ¥ê©`¥À©`¤¬£Ğ£Ã¤Î•r
						else
						{
							// ¥ê©`¥À©`¤Î¤¤¤ëˆöËù¤ËÒÆ„Ó
							stockCharMovePoint( ptAct, nowGx, nowGy );
						}
					}
#endif
				}
				else
				{
					party[no].ptAct = NULL;
				}
			}
			else
			{
				party[no].ptAct = pc.ptAct;
				setPcParty();

				// PC¤¬¥ê©`¥À©`¤Î•r
				if( no == 0 )
				{
					setPcLeader();
				}
#if 0
				// PC¤¬¥ê©`¥À©`ÒÔÍâ¤Î•r
				else
				{
					// ¥ê©`¥À©`¤Î¤¤¤ëˆöËù¤ËÒÆ„Ó
					if( party[0].ptAct != NULL )
					{
						stockCharMovePoint( pc.ptAct, party[0].ptAct->gx, party[0].ptAct->gy );
					}
				}
#endif
			}
			}
			break;

		// ¥¢¥¤¥Æ¥à¤ÎÇéˆó
		case 'I':
			{
			int i;
			int no;
			char name[256];
			char name2[256];
			char memo[256];

			// ¥«¥Æ¥´¥êÎÄ×Ö¤òïw¤Ğ¤¹
			data++;

			for( i = 0; i < MAX_ITEM; i++ )
			{
				no = i * 9;
				getStringToken( data, '|', no+1, sizeof( name ) - 1 , name );
				makeRecvString( name );

				// ÃûÇ°¤¬Ÿo¤¤•r¤Ï¥¢¥¤¥Æ¥à¤¬Ÿo¤¤¤È¤¹¤ë
				if( strlen( name ) == 0 )
				{
					pc.item[i].useFlag = 0;
					continue;
				}

				pc.item[i].useFlag = 1;
				if( strlen( name ) <= ITEM_NAME_LEN )
				{
					strcpy( pc.item[i].name, name );
				}
				getStringToken( data, '|', no+2, sizeof( name2 ) - 1, name2 );
				makeRecvString( name2 );
				if( strlen( name2 ) <= ITEM_NAME2_LEN )
				{
					strcpy( pc.item[i].name2, name2 );
				}
				pc.item[i].color = getIntegerToken( data, '|', no+3 );
				if( pc.item[i].color < 0 )
					pc.item[i].color = 0;
				getStringToken( data, '|', no+4, sizeof( memo ) - 1, memo );
				makeRecvString( memo );
				if( strlen( memo ) <= ITEM_MEMO_LEN )
				{
					strcpy( pc.item[i].memo, memo );
				}
				pc.item[i].graNo = getIntegerToken( data, '|', no+5 );
				pc.item[i].field = getIntegerToken( data, '|', no+6 );
				pc.item[i].target = getIntegerToken( data, '|', no+7 );
				if( pc.item[i].target >= 100 )
				{
					pc.item[i].target %= 100;
					pc.item[i].deadTargetFlag = 1;
				}
				else
				{
					pc.item[i].deadTargetFlag = 0;
				}
				pc.item[i].level = getIntegerToken( data, '|', no+8 );
				pc.item[i].sendFlag = getIntegerToken( data, '|', no+9 );
			}
			}
			break;

		// ¥Ú¥Ã¥È¤Î¼¼
		case 'W':
			{
			int i;
			int no, no2;
			char name[256];
			char memo[256];

			no = data[1] - '0';

			// ¥«¥Æ¥´¥êÎÄ×Ö¤òïw¤Ğ¤¹
			data += 3;

			for( i = 0; i < MAX_SKILL; i++ )
			{
				petSkill[no][i].useFlag = 0;
			}
			for( i = 0; i < MAX_SKILL; i++ )
			{
				no2 = i * 5;
				getStringToken( data, '|', no2+4, sizeof( name ) - 1, name );
				makeRecvString( name );
				// ÃûÇ°¤¬Ÿo¤¤•r¤Ï¥¹¥­¥ë¤¬¤Ê¤¤
				if( strlen( name ) == 0 )
				{
					continue;
				}

				petSkill[no][i].useFlag = 1;
				if( strlen( name ) <= SKILL_NAME_LEN )
				{
					strcpy( petSkill[no][i].name, name );
				}
				else
				{
					strcpy( petSkill[no][i].name, "??? name ???" );
				}
				petSkill[no][i].skillId = getIntegerToken( data, '|', no2+1 );
				petSkill[no][i].field = getIntegerToken( data, '|', no2+2 );
				petSkill[no][i].target = getIntegerToken( data, '|', no2+3 );
				getStringToken( data, '|', no2+5, sizeof( memo ) - 1, memo );
				makeRecvString( memo );
				if( strlen( name ) <= SKILL_MEMO_LEN )
				{
					strcpy( petSkill[no][i].memo, memo );
				}
				else
				{
					strcpy( petSkill[no][i].memo, "??? memo ???" );
				}
			}
			}
			break;


#if 0
    case 'S':       // ¥¹¥­¥ë
        {
            int c=0;
            for(int i=0;i<MAXSKILL;i++){
                pinfo.skill_kind_table[c] = getIntegerToken( data + 1, '|' , (i*2)+1 );
                pinfo.skill_level_table[c] = getIntegerToken( data + 1, '|' , (i*2)+1+1 );
                if( pinfo.skill_kind_table[c]!=-1){
                    c++;
                }else{
					break;
				}
            }
            pinfo.skill_kind_table[c] = SKILL_NUM;
            pinfo.skill_level_table[c] = 1;
			for( i = c+1; i < MAXSKILL+1; i++ )
			{
            	pinfo.skill_kind_table[i] = -1;
            	pinfo.skill_level_table[i] = 0;
			}
        }
        break;
    case 'T':       // ³ÆºÅ
        {
            for(int i=0;i<MAXTITLE;i++){
                char t[TITLESIZE];
                strcpy( t , "");
                getStringToken( data + 1, '|' , i+1 , sizeof(t)-1,t );
				makeRecvString(t);
                strcpy( pinfo.title[i] , t );
            }
        }
        break;
	case 'G':
		for (i=1;i<strlen(data);i++){
			if(i>MAXGEM)break;
			m[0] = data[i];
			m[1] = '\0';
			pinfo.knowngem[i-1] = atoi(m);
		}
		break;
#endif
    }
}


// ¥Ş¥Ã¥×„IÀí /////////////////////////////////////////////////////////////
// ¥Ş¥Ã¥×¥Õ¥¡¥¤¥ë¤ÎÇéˆó¤Ç¥Á¥§¥Ã¥¯¥µ¥à¤òÈ¡¤ê¡¢
// ¥Ş¥Ã¥×¤¬‰ä¤ï¤Ã¤¿¤«¡¢Ÿo¤¤•r¤Ë¥Ş¥Ã¥×¤òÒªÇó¤¹¤ë¡£
void lssproto_MC_recv( int fd, int fl, int x1, int y1, int x2, int y2,
	int tileSum, int partsSum, int eventSum, char *data )
{
	char showString[512];
	char floorName[32];

	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

#ifdef DEBUGPUSH
    char msg[800];
    sprintf( msg , "µØĞÎ¤Î¥Á¥§¥Ã¥¯¥µ¥à¤ò¤¦¤±¤È¤Ã¤¿¡£FL%d %d,%d-%d,%d (%ud/%ud)",
		fl,x1,y1,x2,y2, tileSum, partsSum );
    PUSH( msg );
#endif

	getStringToken( data, '|', 1, sizeof( showString )-1, showString );

	makeRecvString( showString );
	// ¬FÔÚ¤¤¤ë¥Õ¥í¥¢ÒÔÍâ¤ÎÇéˆó¤Ï„IÀí¤·¤Ê¤¤
	if( nowFloor == fl )
	{
		getStringToken( showString, '|', 1, sizeof( floorName )-1, floorName );
		if( strlen( floorName ) <= FLOOR_NAME_LEN )
		{
			strcpy( nowFloorName, floorName );
		}
		else
		{
			strcpy( nowFloorName, "???" );
		}

		// ¥Ñ¥ì¥Ã¥È‰ä¸ü¤Î„IÀí
		char strPal[32];

		palNo = -2;

		getStringToken( showString, '|', 2 , sizeof( strPal )-1, strPal );
		if( strlen( strPal ) == 0 )
		{
			if( TimeZonePalChangeFlag == FALSE || loginFlag )
			{
				// ¥Ñ¥ì¥Ã¥È¥Á¥§¥ó¥¸
				palNo = -1;
			}
		}
		else
		{
			int pal;
			pal = atoi( strPal );
			if( pal >= 0 )
			{
				if( TimeZonePalChangeFlag == TRUE || loginFlag )
				{
					// ¹Ì¶¨¥Ñ¥ì¥Ã¥ÈÔO¶¨
					palNo = pal;
				}
			}
			else
			{
				if( TimeZonePalChangeFlag == FALSE || loginFlag )
				{
					// ¥Ñ¥ì¥Ã¥È¥Á¥§¥ó¥¸
					palNo = -1;
				}
			}
		}
	}

	if( mapCheckSum( fl, x1, y1, x2, y2, tileSum, partsSum, eventSum ) )
	{
		if( nowFloor == fl )
		{
			// ¥Á¥§¥Ã¥¯¥µ¥àÍ¬¤¸•r¤Î„IÀí
			floorChangeFlag = FALSE;
			if( warpEffectStart )
			{
				warpEffectOk = TRUE;
			}
		}
		// ¤â¤¦¤³¤Î¥Õ¥é¥°¤Ï¤¤¤é¤Ê¤¤¤Î¤ÇFALSE¤Ë¤¹¤ë
		loginFlag = FALSE;
	}
}


// ¥Ş¥Ã¥×¥Ç©`¥¿¤òÊÜ¤±È¡¤ê¥Õ¥¡¥¤¥ë¤Ë•ø¤¯
void lssproto_M_recv( int fd, int fl, int x1, int y1, int x2, int y2, char* data )
{
	char showString[512];
	char floorName[32];
	char tilestring[18192];
	char partsstring[18192];
	char eventstring[18192];
	unsigned short tile[2048] , parts[2048], event[2048];
	int i;
	char tmp[100];
	int flag;

	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

#ifdef DEBUGPUSH
    char msg[800];
    sprintf( msg , "¥µ©`¥Ğ©`¤«¤éµØĞÎ¤ò¤¦¤±¤È¤Ã¤¿¡£FL%d %d,%d-%d,%d" , fl,x1,y1,x2,y2 );
    PUSH( msg );
#endif

	getStringToken( data, '|', 1 , sizeof( showString )-1, showString );

	makeRecvString( showString );

	if( nowFloor == fl )
	{
		getStringToken( showString, '|', 1 , sizeof( floorName )-1, floorName );
		if( strlen( floorName ) <= FLOOR_NAME_LEN )
		{
			strcpy( nowFloorName, floorName );
		}
		else
		{
			strcpy( nowFloorName, "???" );
		}

		// ¥Ñ¥ì¥Ã¥È‰ä¸ü¤Î„IÀí
		char strPal[32];

		palNo = -2;

		getStringToken( showString, '|', 2 , sizeof( strPal )-1, strPal );
		if( strlen( strPal ) == 0 )
		{
			if( TimeZonePalChangeFlag == FALSE || loginFlag )
			{
				// ¥Ñ¥ì¥Ã¥È¥Á¥§¥ó¥¸
				palNo = -1;
			}
		}
		else
		{
			int pal;
			pal = atoi( strPal );
			if( pal >= 0 )
			{
				if( TimeZonePalChangeFlag == TRUE || loginFlag )
				{
					// ¹Ì¶¨¥Ñ¥ì¥Ã¥ÈÔO¶¨
					palNo = pal;
				}
			}
			else
			{
				if( TimeZonePalChangeFlag == FALSE || loginFlag )
				{
					// ¥Ñ¥ì¥Ã¥È¥Á¥§¥ó¥¸
					palNo = -1;
				}
			}
		}
	}

	getStringToken( data, '|', 2 , sizeof( tilestring )-1, tilestring );	
	getStringToken( data, '|', 3 , sizeof( partsstring )-1, partsstring );
	getStringToken( data, '|', 4 , sizeof( eventstring )-1, eventstring );

	for( i = 0; ; i++ )
	{
		flag = getStringToken( tilestring, ',', i+1, sizeof( tmp )-1, tmp );
		tile[i] = a62toi( tmp );

		getStringToken( partsstring, ',', i+1, sizeof( tmp )-1, tmp );
		parts[i] = a62toi( tmp );

		getStringToken( eventstring, ',', i+1, sizeof( tmp )-1, tmp );
		event[i] = a62toi( tmp );

		if( flag == 1 )
			break;
	}

	writeMap( fl, x1, y1, x2, y2, tile, parts, event );

	// ¥Ş¥Ã¥×¤¬¿Õ¤Î•r¥Ç©`¥¿¤¬¤­¤¿¤é¥Ş¥Ã¥×¤òÕi¤ßŞz¤à¤è¤¦¤Ë¤¹¤ë
	if( mapEmptyFlag
	 || floorChangeFlag )
	{
		if( nowFloor == fl )
		{
			redrawMap();
			floorChangeFlag = FALSE;
			if( warpEffectStart )
			{
				warpEffectOk = TRUE;
			}
		}
	}

	// ¤â¤¦¤³¤Î¥Õ¥é¥°¤Ï¤¤¤é¤Ê¤¤¤Î¤ÇFALSE¤Ë¤¹¤ë
	loginFlag = FALSE;
}




// ??????????? /////////////////////////////////////////////////////
void lssproto_C_recv( int fd, char *data ) 
{
	int i, j;
	char bigtoken[2048];
	char smalltoken[2048];

	int id;
	int x;
	int y;
	int dir;
	int graNo;
	int level;
	int nameColor;
	char name[2048];
	char freeName[2048];
	int walkable;
	int height;
	int classNo;
	char info[1024];
	int money;
	int charType;
	ACTION *ptAct;

	// ?????­È£R??????¡c? ???¡pä}???
	if( logOutFlag )
		return;

	// ‘éêLÖĞ¤ÏŸoÒ•¤¹¤ë ???????????¡c? ???¡v???
	if( encountNowFlag )
	{
		return;
	}

	for( i = 0; ; i++ )
	{
		// ¥Ñ¥é¥á©`¥¿Ÿo¤¯¤Ê¤Ã¤¿¤é½K¤ï¤ë
		getStringToken( data, ',', i+1, sizeof( bigtoken )-1, bigtoken );
		if( strlen( bigtoken ) == 0 )
			break;

		// 11‚€Ä¿¤Î¥È©`¥¯¥ó¤¬¤¢¤Ã¤¿¤é¤½¤ì¤Ï¥­¥ã¥é¡£
		getStringToken( bigtoken , '|' , 11 , sizeof( smalltoken )-1, smalltoken );
		if( strlen( smalltoken ) > 0 )
		{
			// NPC¤ÎÇéˆó
			charType = getIntegerToken( bigtoken, '|', 1 );
			getStringToken( bigtoken, '|', 2, sizeof( smalltoken )-1, smalltoken );
			id = a62toi( smalltoken );
			getStringToken( bigtoken, '|', 3, sizeof( smalltoken )-1, smalltoken );
			x = atoi( smalltoken );
			getStringToken( bigtoken, '|', 4, sizeof( smalltoken )-1, smalltoken );
			y = atoi( smalltoken );
			getStringToken( bigtoken, '|', 5, sizeof( smalltoken )-1, smalltoken );
			dir = (atoi( smalltoken )+3)%8;
			getStringToken( bigtoken, '|', 6, sizeof( smalltoken )-1, smalltoken );
			graNo = atoi( smalltoken );
			getStringToken( bigtoken, '|', 7, sizeof( smalltoken )-1,smalltoken );
			level = atoi( smalltoken );
			nameColor = getIntegerToken( bigtoken, '|', 8 );
			getStringToken( bigtoken , '|' , 9 , sizeof( name )-1, name );
			makeRecvString( name );
			getStringToken( bigtoken , '|' , 10 , sizeof( freeName )-1, freeName );
			makeRecvString( freeName );
			getStringToken( bigtoken, '|', 11, sizeof( smalltoken )-1, smalltoken );
			walkable = atoi( smalltoken );
			getStringToken( bigtoken, '|', 12, sizeof( smalltoken )-1, smalltoken );
			height = atoi( smalltoken );

			if( pc.id == id )
			{
				// PC¥­¥ã¥é
				if( pc.ptAct == NULL )
				{
					// ¥¢¥¯¥·¥ç¥óŸo¤«¤Ã¤¿¤é×÷¤ë
					createPc( graNo, x, y, dir );
					updataPcAct();
				}
				else
				{
					// ¥¢¥¯¥·¥ç¥ó¤¢¤Ã¤¿¤éÔO¶¨¤À¤±
#if 0
					setPcGraNo( graNo, dir );
					setPcMovePoint( x, y );
#else
					// ¤¹¤Ç¤Ë¥¢¥¯¥·¥ç¥ó¤¢¤ë•r¤Ï¡¢
					// X,Y,DIR¤ÏŸoÒ•¤¹¤ë
					setPcGraNo( graNo, pc.dir );
#endif
				}
				setPcParam( name, freeName, level, nameColor, walkable, height );
				if( (pc.status & CHR_STATUS_LEADER) != 0
				 && party[0].useFlag != 0 )
				{
					party[0].level = pc.level;
					strcpy( party[0].name, pc.name );
				}
				// ÖÙégÇéˆó¤¬¤¢¤Ã¤¿¤é¥¢¥¯¥·¥ç¥ó¥İ¥¤¥ó¥¿¤ò¸üĞÂ
				for( j = 0; j < MAX_PARTY; j++ )
				{
					if( party[j].useFlag != 0 && party[j].id == id )
					{
						party[j].ptAct = pc.ptAct;
						setPcParty();
						// PC¤¬¥ê©`¥À©`¤Î•r
						if( j == 0 )
						{
							setPcLeader();
						}
						break;
					}
				}
			}
			else
			{
				// NPC
#if 0
				// ¥æ©`¥¶¤¬ÔO¶¨¤·¤¿ÃûÇ°¤¬¤¢¤ë¤Ê¤é¤½¤ì¤òÊ¹¤¦
				if( strlen( freeName ) > 0 )
				{
					strcpy( name, freeName );
				}
#endif
				setNpcCharObj( id, graNo, x, y, dir, name, freeName,
					level, nameColor, walkable, height, charType );
				ptAct = getCharObjAct( id );
				if( ptAct != NULL )
				{
					// ÖÙégÇéˆó¤¬¤¢¤Ã¤¿¤é¥¢¥¯¥·¥ç¥ó¥İ¥¤¥ó¥¿¤ò¸üĞÂ
					for( j = 0; j < MAX_PARTY; j++ )
					{
						if( party[j].useFlag != 0 && party[j].id == id )
						{
							party[j].ptAct = ptAct;
							setCharParty( ptAct );
							// NPC¤¬¥ê©`¥À©`¤Î•r
							if( j == 0 )
							{
								setCharLeader( ptAct );
							}
							break;
						}
					}
				}
			}

#ifdef DEBUGPUSH
//			PUSHVAR( "Charindex %d¡¡img %d¤Î¥­¥ã¥é¤ò¤À¤»¤È¥µ©`¥Ğ©`¤¬¤¤¤Ã¤¿¡£", id, graNo );
#endif
		}
		else
		{
			getStringToken( bigtoken, '|', 6, sizeof( smalltoken )-1, smalltoken );
			if( strlen( smalltoken ) > 0 )
			{
				// ¥¢¥¤¥Æ¥àÇéˆó
				getStringToken( bigtoken, '|', 1, sizeof(smalltoken)-1, smalltoken );
				id = a62toi( smalltoken );
				getStringToken( bigtoken, '|', 2, sizeof(smalltoken)-1, smalltoken );
				x = atoi( smalltoken );
				getStringToken( bigtoken, '|', 3, sizeof(smalltoken)-1, smalltoken );
				y = atoi( smalltoken );
				getStringToken( bigtoken, '|', 4, sizeof(smalltoken)-1, smalltoken );
				graNo = atoi( smalltoken );
				classNo = getIntegerToken( bigtoken, '|', 5 );
				getStringToken( bigtoken, '|', 6, sizeof( info )-1, info );
				makeRecvString( info );

				setItemCharObj( id, graNo, x, y, 0, classNo, info );
			}
			else
			{
				getStringToken( bigtoken, '|', 4, sizeof( smalltoken )-1, smalltoken );
				if( strlen( smalltoken ) > 0 )
				{
					// ¤ª½ğÇéˆó
					getStringToken( bigtoken, '|', 1, sizeof(smalltoken)-1, smalltoken );
					id = a62toi( smalltoken );
					getStringToken( bigtoken, '|', 2, sizeof(smalltoken)-1, smalltoken );
					x = atoi( smalltoken );
					getStringToken( bigtoken, '|', 3, sizeof(smalltoken)-1, smalltoken );
					y = atoi( smalltoken );
					getStringToken( bigtoken, '|', 4, sizeof(smalltoken)-1, smalltoken );
					money = atoi( smalltoken );

					sprintf( info, "%d Stone", money );

					// ¤ª½ğ¤ÎÁ¿¤Ç¥°¥é¥Õ¥£¥Ã¥¯¤ò‰ä¤¨¤ë¤«¤â
					if( money > 10000 )
					{
						setMoneyCharObj( id, 24050, x, y, 0, money, info );
					}
					else
					if( money > 1000 )
					{
						setMoneyCharObj( id, 24051, x, y, 0, money, info );
					}
					else
					{
						setMoneyCharObj( id, 24052, x, y, 0, money, info );
					}
				}
			}
		}
	}
}




// ¥­¥ã¥é¤Î¥¢¥¯¥·¥ç¥ó¤ÈÄ¨Ïû ///////////////////////////////////////////////
// ¥­¥ã¥é¤Î¥¢¥¯¥·¥ç¥ó
void lssproto_CA_recv( int fd, char *data ) 
{
	char bigtoken[2048];
	char smalltoken[2048];
	int alreadytellC[1024];
	int tellCindex = 0;
	int tellflag;
	int i, j;
	int charindex;
	int x;
	int y;
	int act;
	int dir;
	int effectno;
	int effectparam1;
	int effectparam2;
	ACTION *ptAct;


	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	// ‘éêLÖĞ¤ÏŸoÒ•¤¹¤ë
	if( encountNowFlag )
	{
		return;
	}

	for( i = 0; ; i++ )
	{
		getStringToken( data, ',', i+1, sizeof( bigtoken )-1, bigtoken );
		if( strlen( bigtoken ) == 0 )
			break;
		getStringToken( bigtoken, '|', 1, sizeof( smalltoken )-1, smalltoken );
		charindex = a62toi( smalltoken );
		getStringToken( bigtoken, '|', 2, sizeof( smalltoken )-1, smalltoken );
		x = atoi( smalltoken );
		getStringToken( bigtoken, '|', 3, sizeof( smalltoken )-1, smalltoken );
		y = atoi( smalltoken );
		getStringToken( bigtoken, '|', 4, sizeof( smalltoken )-1, smalltoken );
		act = atoi( smalltoken );
		getStringToken( bigtoken, '|', 5, sizeof( smalltoken )-1, smalltoken );
		dir = (atoi( smalltoken )+3)%8;
		getStringToken( bigtoken, '|', 6, sizeof( smalltoken )-1, smalltoken );
		effectno = atoi( smalltoken );
		effectparam1 = getIntegerToken( bigtoken, '|', 7 );
		effectparam2 = getIntegerToken( bigtoken, '|', 8 );


		if( pc.id == charindex )
		{
			// PC¥­¥ã¥é¤Î„Ó×÷‰ä¸ü
			if( pc.ptAct == NULL
			 || (pc.ptAct != NULL && pc.ptAct->anim_chr_no == 0) )
			{
				// ¥¢¥¯¥·¥ç¥ó³öÀ´¤Æ¤Ê¤¤•r¤ÏC¤òÒªÇó¤·¥¢¥¯¥·¥ç¥ó¤ò×÷¤ë
				lssproto_C_send( sockfd, charindex );
			}
			else
			{
				changePcAct( x, y, dir, act, effectno, effectparam1, effectparam2 );
			}
			continue;
		}

		ptAct = getCharObjAct( charindex );
		if( ptAct == NULL )
		{
			// Í¬¤¸ID¤¬Ñ}Êı¤¢¤Ã¤Æ¤â¥µ©`¥Ğ¤Ë†–¤¤ºÏ¤ï¤»¤ë¤Î¤Ï£±ID¤ÇÒ»»Ø¤Ë¤¹¤ë
			tellflag = 0;
			for( j = 0; j < tellCindex; j++ )
			{
				if( alreadytellC[j] == charindex )
				{
					tellflag = 1;
					break;
				}
			}
			if( tellflag == 0 && tellCindex < sizeof(alreadytellC) )
			{
				alreadytellC[tellCindex] = charindex;
				tellCindex++;

				lssproto_C_send( sockfd, charindex );
			}
		}
		else
		{
			changeCharAct( ptAct, x, y, dir, act, effectno, effectparam1, effectparam2 );
		}
	}
}


// ¥­¥ã¥é¤ÎÄ¨Ïû
void lssproto_CD_recv( int fd, char *data )
{
	int i, j;
	int id;

	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	for( i = 1; ; i++ )
	{
//		id = getInteger62Token( data, '|', i );
		id = getInteger62Token( data, ',', i );
		if( id == -1 )
			break;

		delCharObj( id );

		for( j = 0; j < MAX_PARTY; j++ )
		{
			if( party[j].useFlag != 0 && party[j].id == id )
			{
				party[j].ptAct = NULL;
				break;
			}
		}
	}
}




// ÒÆ„Ó ///////////////////////////////////////////////////////////////////
// ¥µ©`¥Ğ¤ËÒÆ„Ó¤¹¤ë¤³¤È¤ò»¤¨¤ë
void walkSendForServer( int x, int y, char *direction )
{
    lssproto_W_send( sockfd , x, y, direction );
}


// ¥µ©`¥Ğ¤ËÒÆ„Ó¤¹¤ë¤³¤È¤ò»¤¨¤ë
//  ¤³¤Á¤é¤Ï¥Á¥§¥Ã¥¯¥µ¥à¤òËÍ¤Ã¤Æ¤³¤Ê¤¤¤Î¤Ç£±¶È¤Ç¤â¤¤¤Ã¤¿Ëù¤Ç¤Ê¤¤¤ÈÊ¹¤¨¤Ê¤¤¡£
void noChecksumWalkSendForServer( int x, int y, char *direction )
{
    lssproto_w_send( sockfd , x, y, direction );
}


// ÒÆ„Ó¥³¥Ş¥ó¥É¤Î·µ´ğÊÜĞÅ
void lssproto_W_recv( int fd, int id, int x, int y )
{
	// ½ñ»Ø¤Ï¤¢¤¿¤êÅĞ¶¨¤È¤«¥¯¥é¥¤¥¢¥ó¥È¤Ç¤·¤Æ
	// ¥µ©`¥Ğ¤Ë¤Ï½Ì¤¨¤ë¤À¤±¤Ê¤Î¤ÇºÎ¤â¤·¤Ê¤¤¡£¤«¤â£¿
}




// ¥Á¥ã¥Ã¥ÈÎÄ×ÖÁĞËÍĞÅ //ÁÄÌì×Ö·û´®·¢ËÍ///////////////////////////////////////////////////
void chatStrSendForServer( char *str, int color )
{


	char dest[1024], m[1024];
	int x, y;


	x = nowGx;
	y = nowGy;

	makeSendString( str, dest, sizeof(dest) );
	sprintf( m, "P|%s", dest );
	lssproto_TK_send( sockfd, x, y, m, color, NowMaxVoice );
}




// ¥Æ¥­¥¹¥ÈÊÜĞÅ ///////////////////////////////////////////////////////////
void lssproto_TK_recv( int fd, int index, char *message, int color )
{
	char id[2];
	char msg[2024];
	ACTION *ptAct;

	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	getStringToken( message, '|', 1, sizeof( id  )-1, id );

	if( id[0] == 'P' )
	{
		getStringToken( message, '|', 2, sizeof( msg )-1, msg );
		makeRecvString( msg );
		StockChatBufferLine( msg, color );

		if( index >= 0 )
		{
			if( pc.ptAct != NULL && pc.id == index )
			{
				// 1000¥ß¥êÃë±íÊ¾
				setPcFukidashi( 1000 );
			}
			else
			{
				ptAct = getCharObjAct( index );
				if( ptAct != NULL )
				{
					// 1000¥ß¥êÃë±íÊ¾
					setCharFukidashi( ptAct, 1000 );
				}
			}
		}
	}



#if 0




	if(index == my_id_in_server){
		if(pinfo.ptaction != NULL){
			createPlayerCommonEffect( pinfo.ptaction, SPR_fukidasi, 0, 1, -5 );
		}
	}else{
		int goind = searchGameObjectIndex( index );
//		if( goind >= 0 ||gameobj[goind].ptaction!=NULL){
		if( goind >= 0 && gameobj[goind].ptaction!=NULL ){
			createPlayerCommonEffect( gameobj[goind].ptaction, SPR_fukidasi, 0, 1, -5 );
		}
	}

#ifdef DEBUGPUSH
	if(color<249)PUSHVAR("‰ä¤ÊÉ«¤Ç¥á¥Ã¥»©`¥¸À´¤¿£¨%d£©'%s'",color, message); 
#endif
	char id[2];
    char msg[2024];
	getStringToken( message , '|' , 1 , sizeof( id )-1, id);
	if(id[0] == 'P'){
		getStringToken( message , '|' , 2 , sizeof( msg )-1, msg);
		makeRecvString(msg);
		char cutmes[1024];
		int i, j;
		for(i=0, j=0;;i++,j++){
			cutmes[j] = msg[i];
			if(msg[i] == '\0'){
				chatinputPushString(cutmes, color);
				break;
			}
			if( IsDBCSTrailByte( msg, &msg[i+1] ) ){
				continue;
			}
			cutmes[j+1]='\0';
			if(GetStringWidth(cutmes)>600){
				j = -1;
				chatinputPushString(cutmes, color);
			}
		}
	}else if(id[0] == 'D'){
		int i;
		selectwindowid = getIntegerToken(message, '|', 2);
		void setDengonbanMessageNumber(int i);//netaction.cpp¤Ë±¾Ìå
		setDengonbanMessageNumber(getIntegerToken(message, '|', 3));
		for(i=0;i<5;i++){
			getStringToken( message , '|' , i+4 , sizeof( selectstringline[i] )-1, selectstringline[i]);
			makeRecvString(selectstringline[i]);
		}
		cantclosewindowflag = 1;
		OpenNPCWindow(NPC_BOARD_WINDOW);
	}else if(id[0] == 'W'){
		int i;
		char token[1024];
		getStringToken( message , '|' , 2 , sizeof( token )-1, token);
		selectwindowid = atoi(token);
		char mode[10];
		getStringToken( message , '|' , 3 , sizeof( mode )-1, mode);
		char caption[255];
		int flag;
		flag = getStringToken( message , '|' , 4 , sizeof( caption )-1, caption);
		makeRecvString(caption);
		strcpy(selectstringline[0],caption);
		for(i=1;;i++){
			selectstringlineindex[i] = getIntegerToken(message, '|', 3+i*2);
			flag = getStringToken( message , '|' , 4+i*2 , sizeof( caption )-1, caption);
			makeRecvString(caption);
			strcpy(selectstringline[i],caption);
			if(flag==1)break;
		}
		selectstringlinenumber=i;
		cantclosewindowflag = 1;
		if(mode[0] == 'G')OpenNPCWindow(NPC_KATARIBE_WINDOW);
		else if(mode[0] == 'N')OpenNPCWindow(NPC_WINDOW);
		else PUSHVAR("¥µ©`¥Ğ¤¬¥¦¥£¥ó¥É¥¦é_¤±¤È¤¤¤Ã¤¿¤¬Àí½â¤Ç¤­¤Ê¤«¤Ã¤¿", msg);
	}else if(id[0] == 'M'){
		int i = 0, j = 0, k = 0;
		if(strlen(message)>1024){
#ifdef DEBUGPUSH
			PUSH("¥µ©`¥Ğ¤«¤éÀ´¤¿¥á¥Ã¥»©`¥¸¤¬éL¤¹¤®¤ë¤Ê¤ê");
#endif
			return;
		}
		char token[1024];
		getStringToken( message , '|' , 2 , sizeof( token )-1, token);
		selectwindowid = atoi(token);
		char mode[10];
		getStringToken( message , '|' , 3 , sizeof( mode )-1, mode);
		char caption[1024], onelinestring[1024];
		getStringToken( message , '|' , 4 , sizeof( caption )-1, caption);
		makeRecvString(caption);
		for(i=0;;i++){
			if(caption[i] == '\n'){
				onelinestring[k] = '\0';
				memcpy(selectstringline[j], onelinestring, k+1);
				j++;
				k = 0;
			}else if (caption[i] == '\0'){
				onelinestring[k] = '\0';
				memcpy(selectstringline[j], onelinestring, k+1);
				j++;
				break;
			}else{
				onelinestring[k] = caption[i];
				k++;
			}
		}
		selectstringlinenumber=j;
		if(mode[0] == 'G')OpenNPCWindow(NPC_G_MESSAGE_WINDOW);
		else if(mode[0] == 'N')OpenNPCWindow(NPC_MESSAGE_WINDOW);
		else PUSHVAR("¥µ©`¥Ğ¤¬¥¦¥£¥ó¥É¥¦é_¤±¤È¤¤¤Ã¤¿¤¬Àí½â¤Ç¤­¤Ê¤«¤Ã¤¿", msg);
	}else if(id[0] == 'S'){
		int i;
#if 1
		// µê¤Î¥Ç©`¥¿¤¬À´¤¿¤Î¤Ç¥Ğ¥Ã¥Õ¥¡¤ò³õÆÚ»¯
		for( i = 0; i < MAXSHOPITEM; i++ )
		{
			shopitem[i].plice = 0;
			shopitem[i].stock = 0;
			shopitem[i].itm.name[0] = '\0';
			shopitem[i].itm.name2[0] = '\0';
			shopitem[i].itm.memo[0] = '\0';
			shopitem[i].itm.stage = 0;
			shopitem[i].itm.imgno = 0;
		}
#endif
		selectwindowid = getIntegerToken( message , '|' , 2 );	
		for(i=0;i<7;i++){
			getStringToken( message , '|' , 3+i , sizeof( shopmessage[i] )-1, shopmessage[i] );
			makeRecvString(shopmessage[i]);
		}
		shopgold = getIntegerToken( message , '|' , 10 );
		shopstock = getIntegerToken(message, '|', 11 );
		for(i=0;i<shopstock;i++){
			getStringToken( message , '|' , 12+i*5, sizeof(shopitem[i].itm.name)-1, shopitem[i].itm.name);
			makeRecvString(shopitem[i].itm.name);
			if(strlen(shopitem[i].itm.name)==0)break;
			shopitem[i].plice = getIntegerToken( message , '|' , 13+i*5);
			shopitem[i].itm.stage = getIntegerToken( message , '|' , 14+i*5);
			if(shopitem[i].itm.stage<0)shopitem[i].itm.stage=0;
			shopitem[i].itm.imgno = getIntegerToken( message , '|' , 15+i*5);
			getStringToken( message , '|' , 16+i*5, sizeof(shopitem[i].itm.memo)-1, shopitem[i].itm.memo);
			makeRecvString(shopitem[i].itm.memo);
			shopitem[i].stock = 1;
		}
		for (i=0;i<40;i++){
			charitemplice[i] = getIntegerToken(message, '|', 12+shopstock*5+i);
		}
		void setShoppage(int i);//layout.cpp
		setShoppage (0);
		OpenNPCWindow(NPC_SHOP_START_WINDOW);
		cantclosewindowflag = 1;
		PUSHVAR( "¥µ©`¥Ğ©`¤«¤é¡¢ '%s'¤È¤¤¤¦¥á¥Ã¥»©`¥¸¤­¤¿" , msg );
	}else if(id[0] == 'L'){//¤í¤¹¤È
		getStringToken( message , '|' , 2 , sizeof( selectstringline[0] )-1, selectstringline[0] );
			makeRecvString(selectstringline[0]);
		getStringToken( message , '|' , 3 , sizeof( selectstringline[1] )-1, selectstringline[1] );
			makeRecvString(selectstringline[1]);
		selectstringlineindex[1] = SYSTEM_CHARLOST;
		selectstringlinenumber=1;
		cantclosewindowflag = 1;
		OpenSystemWindow();
	}else if(id[0] == 'K'){
		void setSystemMessage(int i, char *mes, int index);//layout.cpp
		int i;
		char token[1024];
		getStringToken( message , '|' , 2 , sizeof( token )-1, token);
		selectwindowid = atoi(token);
		char caption[255];
		int flag;
		int index;
		flag = getStringToken( message , '|' , 3 , sizeof( caption )-1, caption);
		makeRecvString(caption);
		strcpy(selectstringline[0],caption);
		setSystemMessage( 0, caption, SYSTEM_SYSTEM);
		for(i=1;;i++){
			flag = getStringToken( message , '|' , i+3 , sizeof( caption )-1, caption);
			makeRecvString(caption);
			strcpy(selectstringline[i],caption);
			if(strcmp(selectstringline[i],"¥í¥°¥¢¥¦¥È")==0){
				index = SYSTEM_LOGOUT;
			}else{
				index = SYSTEM_SERVERPUSH_1+i-1;
			}
			selectstringlineindex[i]=index;
			setSystemMessage( i, caption, index);
			if(flag==1)break;
		}
		selectstringlinenumber=i;
		deadflag = 1;
		OpenSystemWindow();
//		OpenNPCWindow(NPC_WINDOW);
		PUSH("TK¤ÇËÀ¤ó¤À¤ÈÑÔ¤ï¤ì¤¿");
	}else if(id[0] == 'R'){
		deadflag = 0;
	}else{
		PUSH("TKÀ´¤¿¤±¤ÉÀí½â¤Ç¤­¤ó¤«¤Ã¤¿");
	}

#endif

}




// ĞÂ¥­¥ã¥é×÷³É ///////////////////////////////////////////////////////////
// ×÷³É¤·¤¿¥­¥ã¥é¥Ç©`¥¿¤ò¥µ©`¥Ğ¤ËËÍ¤ë
void createNewCharStart( void )
{
	newCharStatus = 0;

	// ¥µ©`¥Ğ¤Ç±£´æÎ»ÖÃ¤¬¹Ì¶¨¤Ë¤Ê¤Ã¤¿¤é¤³¤Ã¤Á¤Ë¤¹¤ë
	lssproto_CreateNewChar_send( sockfd, selectPcNo, newCharacterName,
		newCharacterGraNo, newCharacterFaceGraNo,
		newCharacterVit, newCharacterStr, newCharacterTgh, newCharacterDex,
		newCharacterEarth, newCharacterWater, newCharacterFire, newCharacterWind,
		newCharacterHomeTown );

	SETSENDING;
}


// ĞÂ¥­¥ã¥éµÇåhÍêÁË´ı¤Á
//   ‘ø¤ê‚£º 0 ... ÊÜĞÅ´ı¤Á / 1 ... ÊÜĞÅÍêÁË / -1 ... ¥¿¥¤¥à¥¢¥¦¥È / -2 ... ¥¨¥é©`¤¬¢¤Ã¤Æ¤­¤¿
int createNewCharProc( void )
{
	if( netproc_sending == NETPROC_RECEIVED )
	{
		netproc_sending = NETPROC_NOTSEND;
		if( newCharStatus == 1 )
		{
			return 1;
		}
		else
		{
			return -2;
		}
    }

	SETTIMEOUT2( NET_ERRMSG_CREATECHARTIMEOUT );

	return 0;
}


// ĞÂ¥­¥ã¥éµÇåhÍêÁËÊÜĞÅ
void lssproto_CreateNewChar_recv( int fd, char *result, char *data ) 
{
	if( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;

		if( strcmp( result, SUCCESSFULSTR ) == 0 )
		{
			newCharStatus = 1;
		}
    }
}








// ¥­¥ã¥éÏ÷³ı /////////////////////////////////////////////////////////////
// Ï÷³ı¥­¥ã¥é·¬ºÅËÍĞÅ
void delCharStart( void )
{
	charDelStatus = 0;

	lssproto_CharDelete_send( sockfd, gamestate_deletechar_charname );

	SETSENDING;
}


// ¥­¥ã¥éÏ÷³ı´ı¤Á
//   ‘ø¤ê‚£º 0 ... Ï÷³ı´ı¤Á / 1 ... Ï÷³ıÍêÁË / -1 ... ¥¿¥¤¥à¥¢¥¦¥È / -2 ... ¥¨¥é©`¤¬¢¤Ã¤Æ¤­¤¿
int delCharProc( void )
{
	if( netproc_sending == NETPROC_RECEIVED )
	{
		netproc_sending = NETPROC_NOTSEND;
		if( charDelStatus )
		{
			return 1;
		}
		else
		{
			return -2;
		}
	}

	SETTIMEOUT2( NET_ERRMSG_DELETECHARTIMEOUT );

	return 0;
}


// ¥­¥ã¥éÏ÷³ıÍêÁËÊÜĞÅ
void lssproto_CharDelete_recv( int fd, char *result, char *data )
{
	if( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;
		if( strcmp( result, SUCCESSFULSTR ) == 0 )
		{
			charDelStatus = 1;
    	}
    }
}



#if 0
// ¥Ñ¥¹¥ï©`¥É‰ä¸ü /////////////////////////////////////////////////////////

static short netprocChagePasswdSending = NETPROC_NOTSEND;
static BOOL netprocChagePasswdResult = FALSE;
static DWORD chagePasswdStartTime = 0;	// ¥¿¥¤¥à¥¢¥¦¥È¥Á¥§¥Ã¥¯

// ĞÂ¤·¤¤¥Ñ¥¹¥ï©`¥ÉËÍĞÅ£¨ĞÂÒµÇåh£©
//   CDKEY¤¬±ØÒª
void changePassword( char *_cdkey, char *_newPasswd )
{
	char cdkey[64];
	char newPasswd[64];
	char oldPasswd[64];

//	SETNEEDCHOOSE( GAMESTATE_CHPASS_ERROR_START );

	getPassword( oldPasswd );
	strcpy( cdkey, _cdkey );
	strcpy( newPasswd, _newPasswd );

    // ChangePasswd¤òËÍĞÅ¤¹¤ë¤ÈÍ¬•r¤ËClientLogin¤âËÍĞÅ¤¹¤ë( lssproto.html ²ÎÕÕ)
	netprocChagePasswdSending = NETPROC_SENDING;
	netprocChagePasswdResult = FALSE;
    lssproto_ClientLogin_send( sockfd, cdkey, newPasswd );
    lssproto_ChangePasswd_send( sockfd, oldPasswd, newPasswd );

	SET_SENDTIME( chagePasswdStartTime );
}


// ¥Ñ¥¹¥ï©`¥É‰ä¸ü½Y¹û´ı¤Á
// ‘ø¤ê‚£º0 ... ÍêÁË´ı¤Á/ 1 ... ÍêÁË(³É¹¦) / 2 ... ÍêÁË(Ê§”¡) / -1 ... ¥¿¥¤¥à¥¢¥¦¥È
int changePasswordDoing( void )
{
	if( netprocChagePasswdSending == NETPROC_RECEIVED )
	{
		netprocChagePasswdSending = NETPROC_NOTSEND;
		if( netprocChagePasswdResult )
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}

	CHECK_TIMEOUT( chagePasswdStartTime, TIMEOUT );
//	SETTIMEOUT( GAMESTATE_ERRMSG_CHPASSTIMEOUT , GAMESTATE_CHPASS_ERROR_START );

	return 0;
}


// ¥Ñ¥¹¥ï©`¥É‰ä¸ü½Y¹ûÊÜĞÅ
void lssproto_ChangePasswd_recv( int fd, char *result, char *data )
{
	if( netprocChagePasswdSending == NETPROC_SENDING )
	{
		netprocChagePasswdSending = NETPROC_RECEIVED;
		if( strcmp( result, SUCCESSFULSTR ) == 0 )
		{
			netprocChagePasswdResult = TRUE;
		}
    }
}
#endif


// ÖÙégÒªÇó¤ÎÊÜĞÅ /////////////////////////////////////////////////////////
void lssproto_PR_recv( int fd, int request, int result )
{
	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	if( request == 1 && result == 1 )
	{
		// Õl¤«¤Î¥Ñ©`¥Æ¥£©`¤ËÈë¤ëÒªÇó¤¬Í¨¤Ã¤¿
		setPcParty();
	}
	else
	if( request == 0 && result == 1 )
	{
		// ¥Ñ©`¥Æ¥£©`¤«¤é³ıê ¤·¤¿
		delPcParty();
		delPcLeader();
		partyModeFlag = 0;
		clearPartyParam();
	}
	prSendFlag = 0;
}



// EV£¨¥¤¥Ù¥ó¥È£©ËÍĞÅáá¤ÎÊÜĞÅ´ı¤Á /////////////////////////////////////////
void lssproto_EV_recv( int fd, int seqno, int result )
{
	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	if( eventWarpSendId == seqno )
	{
		eventWarpSendFlag = 0;
		if( result == 0 )
		{
			// ¥ï©`¥×Ê§”¡¤·¤¿¤éÔª¤Î»­Ãæ¤ò±íÊ¾¤µ¤»¤ë
			redrawMap();
			floorChangeFlag = FALSE;
			// ŠÖÆµÄ¤Ë¥Õ¥§©`¥É¥¤¥ó¤µ¤»¤ë
			warpEffectStart = TRUE;
			warpEffectOk = TRUE;
		}
	}
	else
	if( eventEnemySendId == seqno )
	{
		if( result == 0 )
		{
			eventEnemySendFlag = 0;
		}
		//else
		//{
			// ¥Õ¥é¥°¥ª¥Õ¤Ïprocess.cpp¤Ç¤ä¤ë
		//}
	}
}


// ÖÙégOK¡¢‘éêLÍ¾ÖĞ²Î¼ÓOK¡¢£Ä£Õ£Å£Ì £Ï£Ë¤Î×´‘BÊÜĞÅ ////////////////////////
void lssproto_FS_recv( int fd, int flg )
{
	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	pc.etcFlag = (unsigned short)flg;
	pc.etcFlag &= ~PC_ETCFLAG_JOINT_BTL;	// ¤³¤Î¥Ó¥Ã¥È¤ÏÍ¨³££Ï£Æ£Æ
}



// ¥¢¥É¥ì¥¹¥Ö¥Ã¥¯¤ÎÄÚÈİÊÜĞÅ ///////////////////////////////////////////////
void lssproto_AB_recv( int fd, char *data )
{
	int i;
	int no;
	int nameLen;
	char name[256];
	int flag;
	int useFlag;

	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	for( i = 0; i < MAX_ADR_BOOK; i++ )
	{
		no = i * 6;

		useFlag = getIntegerToken( data, '|', no+1 );
		if( useFlag == -1 )
		{
			useFlag = 0;
		}
		if( useFlag <= 0 )
		{
#if 0
			if( addressBook[i].useFlag == 1 )
#else
			if( MailHistory[i].dateStr[MAIL_MAX_HISTORY-1][0] != '\0' )
#endif
			{
				memset( &MailHistory[i], 0, sizeof( MailHistory[0] ) ) ;
				SaveMailHistory( i );
				// Î´Õi¥Á¥§¥Ã¥¯
				mailLamp = CheckMailNoReadFlag();
				// ÊÖ¼ˆ¥¢¥Ë¥áÏûÈ¥
				DeathLetterAction();
			}
			addressBook[i].useFlag = 0;
			addressBook[i].name[0] = '\0';
			continue;
		}

		addressBook[i].useFlag = 1;

		flag = getStringToken( data, '|', no+2, sizeof( name )-1 , name );

		if( flag == 1 )
			break;

		makeRecvString( name );
		nameLen = strlen( name );
		if( 0 < nameLen && nameLen <= CHAR_NAME_LEN )
		{
			strcpy( addressBook[i].name, name );
		}
		addressBook[i].level = getIntegerToken( data, '|', no+3 );
		addressBook[i].dp = getIntegerToken( data, '|', no+4 );
		addressBook[i].onlineFlag = (short)getIntegerToken( data, '|', no+5 );
		addressBook[i].graNo = getIntegerToken( data, '|', no+6 );
	}
}


#if 1
// ¥¢¥É¥ì¥¹¥Ö¥Ã¥¯¤ÎÄÚÈİÊÜĞÅ£¨…gÌå£©////////////////////////////////////////
void lssproto_ABI_recv( int fd, int num, char* data )
{
	char name[256];
	int nameLen;
	int useFlag;

	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	useFlag = getIntegerToken( data, '|', 1 );
	if( useFlag == -1 )
	{
		useFlag = 0;
	}
	if( useFlag == 0 )
	{
#if 0
		if( addressBook[num].useFlag == 1 )
#else
		if( MailHistory[num].dateStr[MAIL_MAX_HISTORY-1][0] != '\0' )
#endif
		{
			memset( &MailHistory[num], 0, sizeof( MailHistory[0] ) ) ;
			SaveMailHistory( num );
			// Î´Õi¥Á¥§¥Ã¥¯
			mailLamp = CheckMailNoReadFlag();
			// ÊÖ¼ˆ¥¢¥Ë¥áÏûÈ¥
			DeathLetterAction();
		}
		addressBook[num].useFlag = useFlag;
		addressBook[num].name[0] = '\0';
		return;
	}

	addressBook[num].useFlag = useFlag;

	getStringToken( data, '|', 2, sizeof( name )-1 , name );
	makeRecvString( name );
	nameLen = strlen( name );
	if( 0 < nameLen && nameLen <= CHAR_NAME_LEN )
	{
		strcpy( addressBook[num].name, name );
	}
	addressBook[num].level = getIntegerToken( data, '|', 3 );
	addressBook[num].dp = getIntegerToken( data, '|', 4 );
	addressBook[num].onlineFlag = (short)getIntegerToken( data, '|', 5 );
	addressBook[num].graNo = getIntegerToken( data, '|', 6 );
}
#endif


// ‘éêL½Y¹û¥á¥Ã¥»©`¥¸ÊÜĞÅ /////////////////////////////////////////////////
void lssproto_RS_recv( int fd, char *data )
{
	int i;
	char token[2048];
	char item[2048];

	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	battleResultMsg.useFlag = 1;

	for( i = 0; i < RESULT_CHR_EXP; i++ )
	{
		getStringToken( data, ',', i+1, sizeof( token )-1, token );

		battleResultMsg.resChr[i].petNo = getIntegerToken( token, '|', 1 );
		battleResultMsg.resChr[i].levelUp = getIntegerToken( token, '|', 2 );
		battleResultMsg.resChr[i].exp = getInteger62Token( token, '|', 3 );
	}

	getStringToken( data, ',', i+1, sizeof( token )-1, token );
	getStringToken( token, '|', 1, sizeof( item )-1, item );
	makeRecvString( item );
	if( strlen( item ) <= RESULT_ITEM_NAME_LEN )
	{
		strcpy( battleResultMsg.item[0], item );
	}
	getStringToken( token, '|', 2, sizeof( item )-1, item );
	makeRecvString( item );
	if( strlen( item ) <= RESULT_ITEM_NAME_LEN )
	{
		strcpy( battleResultMsg.item[1], item );
	}
	getStringToken( token, '|', 3, sizeof( item )-1, item );
	makeRecvString( item );
	if( strlen( item ) <= RESULT_ITEM_NAME_LEN )
	{
		strcpy( battleResultMsg.item[2], item );
	}
}


// ¥Ç¥å¥¨¥ë½Y¹û¥á¥Ã¥»©`¥¸ÊÜĞÅ /////////////////////////////////////////////////
void lssproto_RD_recv( int fd, char *data )
{
	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	battleResultMsg.useFlag = 2;

	battleResultMsg.resChr[0].exp = getInteger62Token( data, '|', 1 );
	battleResultMsg.resChr[1].exp = getInteger62Token( data, '|', 2 );
}


// ¥¢¥¤¥Æ¥àÎ»ÖÃ‰ä¸üÊÜĞÅ ///////////////////////////////////////////////////
void lssproto_SI_recv( int fd, int from, int to )
{
	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	swapItem( from, to );
}


// ¥¢¥¤¥Æ¥àÇéˆóÊÜĞÅ ///////////////////////////////////////////////////////
void lssproto_I_recv( int fd, char *data )
{
	int i, j;
	int no;
	char name[256];
	char name2[256];
	char memo[256];

	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	for( j = 0; ; j++ )
	{
		no = j * 10;

		i = getIntegerToken( data, '|', no+1 );

		if( getStringToken( data, '|', no+2, sizeof( name ) - 1 , name ) == 1 )
			break;
		makeRecvString( name );

		// ÃûÇ°¤¬Ÿo¤¤•r¤Ï¥¢¥¤¥Æ¥à¤¬Ÿo¤¤¤È¤¹¤ë
		if( strlen( name ) == 0 )
		{
			pc.item[i].useFlag = 0;
			continue;
		}

		pc.item[i].useFlag = 1;
		if( strlen( name ) <= ITEM_NAME_LEN )
		{
			strcpy( pc.item[i].name, name );
		}
		getStringToken( data, '|', no+3, sizeof( name2 ) - 1, name2 );
		makeRecvString( name2 );
		if( strlen( name2 ) <= ITEM_NAME2_LEN )
		{
			strcpy( pc.item[i].name2, name2 );
		}
		pc.item[i].color = getIntegerToken( data, '|', no+4 );
		if( pc.item[i].color < 0 )
			pc.item[i].color = 0;
		getStringToken( data, '|', no+5, sizeof( memo ) - 1, memo );
		makeRecvString( memo );
		if( strlen( memo ) <= ITEM_MEMO_LEN )
		{
			strcpy( pc.item[i].memo, memo );
		}
		pc.item[i].graNo = getIntegerToken( data, '|', no+6 );
		pc.item[i].field = getIntegerToken( data, '|', no+7 );
		pc.item[i].target = getIntegerToken( data, '|', no+8 );
		if( pc.item[i].target >= 100 )
		{
			pc.item[i].target %= 100;
			pc.item[i].deadTargetFlag = 1;
		}
		else
		{
			pc.item[i].deadTargetFlag = 0;
		}
		pc.item[i].level = getIntegerToken( data, '|', no+9 );
		pc.item[i].sendFlag = getIntegerToken( data, '|', no+10 );
	}
}


// ¥¦¥£¥ó¥É¥¦„IÀí /////////////////////////////////////////////////////////
void lssproto_WN_recv( int fd,int windowtype,int buttontype,int seqno,int objindex,char* data )
{
	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	openServerWindow( windowtype, buttontype, seqno, objindex, data );
}


// ¥Ú¥Ã¥È¥á©`¥ëÑİ³ö ///////////////////////////////////////////////////////
void lssproto_PME_recv( int fd, int objindex,
	int graphicsno, int x, int y, int dir, int flg, int no, char *cdata )
{
	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	// ‘éêLÖĞ¤ÏŸoÒ•¤¹¤ë
	if( encountNowFlag )
	{
		return;
	}

	// ËÍĞÅÑİ³ö
	if( flg == 0 )
	{
		switch( no )
		{
			case 0:
				createPetAction( graphicsno, x, y, dir, 0, dir, -1 );
				break;

			case 1:
				createPetAction( graphicsno, x, y, dir, 2, 0, -1 );
				break;
		}
	}
	else
	// ÊÜĞÅÑİ³ö
	{
		char smalltoken[2048];
		int id;
		int x;
		int y;
		int dir;
		int graNo;
		int level;
		int nameColor;
		char name[2048];
		char freeName[2048];
		int walkable;
		int height;
		int charType;

		charType = getIntegerToken( cdata, '|', 1 );
		getStringToken( cdata, '|', 2, sizeof( smalltoken )-1, smalltoken );
		id = a62toi( smalltoken );
		getStringToken( cdata, '|', 3, sizeof( smalltoken )-1, smalltoken );
		x = atoi( smalltoken );
		getStringToken( cdata, '|', 4, sizeof( smalltoken )-1, smalltoken );
		y = atoi( smalltoken );
		getStringToken( cdata, '|', 5, sizeof( smalltoken )-1, smalltoken );
		dir = (atoi( smalltoken )+3)%8;
		getStringToken( cdata, '|', 6, sizeof( smalltoken )-1, smalltoken );
		graNo = atoi( smalltoken );
		getStringToken( cdata, '|', 7, sizeof( smalltoken )-1,smalltoken );
		level = atoi( smalltoken );
		nameColor = getIntegerToken( cdata, '|', 8 );
		getStringToken( cdata, '|' , 9 , sizeof( name )-1, name );
		makeRecvString( name );
		getStringToken( cdata, '|' , 10 , sizeof( freeName )-1, freeName );
		makeRecvString( freeName );
		getStringToken( cdata, '|', 11, sizeof( smalltoken )-1, smalltoken );
		walkable = atoi( smalltoken );
		getStringToken( cdata, '|', 12, sizeof( smalltoken )-1, smalltoken );
		height = atoi( smalltoken );

		// ¤¹¤Ç¤ËÇéˆó¤¬¤¢¤Ã¤¿¤éºÎ¤â¤·¤Ê¤¤
		if( setReturnPetObj( id, graNo, x, y, dir, name, freeName,
			level, nameColor, walkable, height, charType ) )
		{
			switch( no )
			{
				case 0:
					createPetAction( graphicsno, x, y, dir, 1, 0, objindex );
					break;

				case 1:
					createPetAction( graphicsno, x, y, dir, 3, 0, objindex );
					break;
			}
		}
	}
}


// Öi£¿ ///////////////////////////////////////////////////////////////////
void lssproto_IS_recv( int fd, char* cdata )
{
}




// EN£¨¥¨¥ó¥«¥¦¥ó¥È£©ËÍĞÅáá¤ÎÊÜĞÅ´ı¤Á /////////////////////////////////////
// result ¤Î‚	£±£ºÍ¨³£¥¨¥ó¥«¥¦¥ó¥È
//				£²£º¥Ç¥å¥¨¥ë
//				£´£ºÓQ‘é
//				£µ£º¥Ø¥ë¥×²»¿É
void lssproto_EN_recv( int fd,int result,int field )
{
	// ¥í¥°¥¢¥¦¥È¤Ï¤¸¤á¤¿¤é¥µ©`¥Ğ¤«¤é¤ÎÇéˆó¤ÏŸoÒ•¤¹¤ë
	if( logOutFlag )
		return;

	// ¥¨¥ó¥«¥¦¥ó¥Õ¥é¥°£Ï£Î
	if( result > 0 )
	{
		EncountFlag = TRUE;
		// ¥¨¥ó¥«¥¦¥ó¥È¤¬¹Ì¶¨”³¤«¥Ç¥å¥¨¥ë¤Î•r
		if( result == 6
		 || result == 2 )
		{
			eventEnemyFlag = 1;
		}
		else
		{
			eventEnemyFlag = 0;
		}
		// ¥¨¥é©`¥Á¥§¥Ã¥¯
		if( field < 0 || BATTLE_MAP_FILES <= field ){
			BattleMapNo = 0;	// ¥Ç¥Õ¥©¥ë¥È·¬ºÅ
		}else{
			BattleMapNo = field;	// ‘éêL¥Ş¥Ã¥×·¬ºÅ
		}
		// ¥Ç¥å¥¨¥ë¤Î•r
		if( result == 2 ) DuelFlag = TRUE;
		else DuelFlag = FALSE;
		// ¥Ø¥ë¥×Ÿo¤·¤Î•r
		if( result == 2 || result == 5 ) NoHelpFlag = TRUE;
		else NoHelpFlag = FALSE;
		
		// ¥Õ¥é¥°¥ª¥Õ¤Ïprocess.cpp¤Ç¤ä¤ë
		// sendEnFlag = 0;
		// duelSendFlag = 0;

		// ¥Ğ¥È¥ë¥³¥Ş¥ó¥É³õÆÚ»¯
		BattleStatusReadPointer = BattleStatusWritePointer =0;
		BattleCmdReadPointer = BattleCmdWritePointer =0;
	}
	else
	{
		// ½Y¹û¤â¤é¤Ã¤Æ¥¨¥ó¥«¥¦¥ó¥È¤Ç¤­¤Ê¤¤¤Ê¤é¤¹¤°¥Õ¥é¥°¥ª¥Õ
		sendEnFlag = 0;
		duelSendFlag = 0;
		jbSendFlag = 0;
	}
}


// ‘éêLÖĞ¥Ø¥ë¥×¤ÎÊÜĞÅ
void lssproto_HL_recv( int fd, int flg )
{
	helpFlag = flg;
}





// ¥Ğ¥È¥ë¥³¥Ş¥ó¥ÉÊÜĞÅ /////////////////////////////////////////////////////
void lssproto_B_recv( int fd,char* command )
{

	// ¥Ğ¥È¥ë×´‘BÊÜĞÅ
	if( *( command + 1 ) == 'C'){
//		strcpy( BattleStatusBak, command );
		strcpy( BattleStatusBak[ BattleStatusWritePointer ], command );
		BattleStatusWritePointer = ( BattleStatusWritePointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
	}
	// ±¾ÈËÊÜĞÅ
	else if( *( command + 1 ) == 'P')
		sscanf( command + 3, "%X|%X|%X", &BattleMyNo, &BattleBpFlag, &BattleMyMp );
	// ¥³¥Ş¥ó¥ÉÈëÁ¦œg¤ß¥Õ¥é¥°ÊÜĞÅ
	else if( *( command + 1 ) == 'A'){
		sscanf( command + 3, "%X|%X", &BattleAnimFlag, &BattleSvTurnNo );
		// ¥¿©`¥óÊÜĞÅ¥Õ¥é¥°£Ï£Î¤Î•r
		if( BattleTurnReceiveFlag == TRUE ){ 
			BattleCliTurnNo = BattleSvTurnNo;	// ×î³õ¤Î¥¿©`¥ó·¬ºÅ¤òÒ™¤¨¤ë¡£
			BattleTurnReceiveFlag = FALSE;		// ¥Õ¥é¥°£Ï£Æ£Æ
		}
	}
	// ‘éêLŠÖÆ½KÁËÊÜĞÅ
	else if( *( command + 1 ) == 'U')
		BattleEscFlag = TRUE;
	// ‘éêL½Y¹ûÊÜĞÅ
	else {
//		strcpy( BattleCmdBak, command );
		strcpy( BattleCmdBak[ BattleCmdWritePointer ], command );
		BattleCmdWritePointer = ( BattleCmdWritePointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
	}
	
#ifdef _DEBUG_MSG
		StockChatBufferLine( command, FONT_PAL_GRAY );
#endif
	
}

// ‘éêL¤Ë³ö¤¹¥Ú¥Ã¥È¤òßx’k¤·¤¿
void lssproto_KS_recv( int fd,int petarray,int result )
{
	int cnt = 0; // ´ı™C¥«¥¦¥ó¥È
	int i;
	
	// ÊÜĞÅ´ı¤Á¥Õ¥é¥°³õÆÚ»¯
	BattlePetReceiveFlag = FALSE;
	// ¥Ğ¥È¥ëÊÜĞÅÖĞ¤Î¥Ú¥Ã¥È·¬ºÅ³õÆÚ»¯
	BattlePetReceivePetNo = -1;
	// ³É¹¦¤·¤¿•r
	if( result == TRUE ){ 
		// ¥Ú¥Ã¥È·¬ºÅ¥Ğ¥Ã¥¯¥¢¥Ã¥×³õÆÚ»¯
		battlePetNoBak = -2;
		// ÒÔÇ°¤Î¥Ğ¥È¥ë¥Ú¥Ã¥È¤ò´ı™C¤Ç¤Ê¤¯¤¹¤ë
		//if( pc.battlePetNo != -1 ) pc.selectPetNo[ pc.battlePetNo ] = FALSE;
		
		// ½ñ»Ø¤Î¥Ğ¥È¥ë¥Ú¥Ã¥È¤ò´ı™C¤Ë¤¹¤ë
		if( petarray != -1 ){ 
			// ´ı™C¤Ë¤¹¤ë
			pc.selectPetNo[ petarray ] = TRUE;
			// ¥á©`¥ë¥Ú¥Ã¥È¤À¤Ã¤¿¤é¡¢£Ï£Æ£Æ¤Ë¤¹¤ë
			if( pc.mailPetNo == petarray ) pc.mailPetNo = -1;
			// ´ı™C¥Ú¥Ã¥ÈÊı¥Á¥§¥Ã¥¯
			for( i = 0 ; i < 5 ; i++ ){
				if( pc.selectPetNo[ i ] == TRUE && i != petarray ) cnt++;
				// ÈËÊı¥ª¥Ğ©`¤Î•r¡¢£Ï£Æ£Æ¤Ë¤¹¤ë
				if( cnt >= 3 ){ 
					pc.selectPetNo[ i ] = FALSE;
					cnt--;
				}
			}
		}
		// ½ñ»Ø¤Î¥Ğ¥È¥ë¥Ú¥Ã¥È·¬ºÅÓ›‘›
		pc.battlePetNo = petarray;
	}
}

//	¥¹¥­¥ë¥¢¥Ã¥×¥İ¥¤¥ó¥È
void lssproto_SKUP_recv( int fd,int point )
{
	// ¥¹¥Æ©`¥¿¥¹¥¢¥Ã¥×¥İ¥¤¥ó¥ÈÊÜĞÅ
	StatusUpPoint = point;
}

// ¥¢¥É¥ì¥¹¥Ö¥Ã¥¯¥á¥Ã¥»©`¥¸¤òÊÜ¤±È¡¤Ã¤¿
void lssproto_MSG_recv( int fd,int aindex,char* text ,int color)
{
	char moji[ 256 ];
	int noReadFlag;
	
	// ¥á©`¥ë¥é¥ó¥×¥Õ¥é¥°£Ï£Î
	mailLamp = TRUE;
	
	// ×îĞÂÂÄšs·¬ºÅ¤Î¸üĞÂ
	MailHistory[ aindex ].newHistoryNo--;
	// ¥ê¥ß¥Ã¥È¥Á¥§¥Ã¥¯
	if( MailHistory[ aindex ].newHistoryNo <= -1 ) MailHistory[ aindex ].newHistoryNo = MAIL_MAX_HISTORY - 1;
	
	// ÈÕ•r¤òÈ¡¤ê³ö¤¹
	getStringToken( text, '|', 1, sizeof( MailHistory[ aindex ].dateStr[ MailHistory[ aindex ].newHistoryNo ] )-1, 
					MailHistory[ aindex ].dateStr[ MailHistory[ aindex ].newHistoryNo ] );
	// ÎÄÕÂ¤òÈ¡¤ê³ö¤¹
	getStringToken( text, '|', 2, sizeof( MailHistory[ aindex ].str[ MailHistory[ aindex ].newHistoryNo ] )-1, 
					MailHistory[ aindex ].str[ MailHistory[ aindex ].newHistoryNo ] );
	// ¥¨¥¹¥±©`¥×¤Ï¤º¤¹
	makeRecvString( MailHistory[ aindex ].str[ MailHistory[ aindex ].newHistoryNo ] );
	
	// ¥Ú¥Ã¥È¤Î¥°¥é¥Õ¥£¥Ã¥¯·¬ºÅÈ¡¤ê³ö¤¹
	noReadFlag = getIntegerToken( text, '|', 3 );
	// ¥Ú¥Ã¥È¥á©`¥ë¤Î•r
	if( noReadFlag != -1 ){
		// ¥Ú¥Ã¥È¤Î¥°¥é¥Õ¥£¥Ã¥¯·¬ºÅÓ›‘›
		MailHistory[ aindex ].noReadFlag[ MailHistory[ aindex ].newHistoryNo ] = noReadFlag;
		// ¥Ú¥Ã¥È¤Î¥ì¥Ù¥ë¤òÈ¡¤ê³ö¤¹
		MailHistory[ aindex ].petLevel[ MailHistory[ aindex ].newHistoryNo ] = getIntegerToken( text, '|', 4 );
		// ¥Ú¥Ã¥È¤ÎÃûÇ°¤òÈ¡¤ê³ö¤¹
		getStringToken( text, '|', 5, sizeof( MailHistory[ aindex ].petName[ MailHistory[ aindex ].newHistoryNo ] ), 
						MailHistory[ aindex ].petName[ MailHistory[ aindex ].newHistoryNo ] );
		// ¥Ú¥Ã¥È¤ÎÃûÇ°¤Î¥¨¥¹¥±©`¥×¤Ï¤º¤¹
		makeRecvString( MailHistory[ aindex ].petName[ MailHistory[ aindex ].newHistoryNo ] );
		// ¥¢¥¤¥Æ¥à¤Î¥°¥é¥Õ¥£¥Ã¥¯·¬ºÅ¤òÈ¡¤ê³ö¤¹
		MailHistory[ aindex ].itemGraNo[ MailHistory[ aindex ].newHistoryNo ] = getIntegerToken( text, '|', 6 );
		// ¥á©`¥ë¤¬½ì¤¤¤¿¥á¥Ã¥»©`¥¸×÷³É
		sprintf( moji,"%s ¤«¤é¥Ú¥Ã¥È¥á©`¥ë¤¬½ì¤¤¤¿£¡", addressBook[ aindex ].name );
	}
	// ÆÕÍ¨¥á©`¥ë¤Î•r
	else{	
		MailHistory[ aindex ].noReadFlag[ MailHistory[ aindex ].newHistoryNo ] = TRUE;
		// ¥á©`¥ë¤¬½ì¤¤¤¿¥á¥Ã¥»©`¥¸×÷³É
		sprintf( moji,"%s ¤«¤é¥á©`¥ë¤¬½ì¤¤¤¿£¡", addressBook[ aindex ].name );
	}
	
	// ¥Á¥ã¥Ã¥ÈÎÄ×Ö¤ò¥Ğ¥Ã¥Õ¥¡¤ËÁï¤á¤ë£¨Ò»ĞĞ£©
	StockChatBufferLine( moji, FONT_PAL_WHITE );
	
	// ÒŠ¤Æ¤ë¥Ú©`¥¸¤Î•r
	if( mailHistoryWndSelectNo == aindex ){
		// ÒŠ¤Æ¤ë¥Ú©`¥¸¤ò£±¥Ú©`¥¸¤º¤é¤¹
		mailHistoryWndPageNo++;
		// ¥ê¥ß¥Ã¥È¥Á¥§¥Ã¥¯
		if( mailHistoryWndPageNo >= MAIL_MAX_HISTORY ) mailHistoryWndPageNo = 0;
		// ÊÖ¼ˆ¥¢¥Ë¥á©`¥·¥ç¥óÄ¨š¢
	//	DeathLetterAction();
	}
	// ÊÜĞÅÒô
	play_se( 101, 320, 240 );
	// ¥á©`¥ë¥Ç©`¥¿¤Î±£´æ
	SaveMailHistory( aindex );
}


// ¥Ú¥Ã¥È¤Î¼¼¤òÊ¹¤¤½K¤ï¤Ã¤¿¤òÊÜĞÅ
void lssproto_PS_recv( int fd,int result,int havepetindex,int havepetskill,int toindex )
{
	char moji[ 256 ];
	
	// ¥Õ¥é¥°¤ò£Ï£Æ£Æ
	ItemMixRecvFlag = FALSE;
	
	// Ê§”¡¤·¤¿¤é
	if( result == 0 ){
		//Ò»ĞĞ¥¤¥ó¥Õ¥©¤Ë±íÊ¾
		sprintf( moji,"Ê§”¡¤·¤Ş¤·¤¿£¡");
		// ¥Á¥ã¥Ã¥ÈÎÄ×Ö¤ò¥Ğ¥Ã¥Õ¥¡¤ËÁï¤á¤ë£¨Ò»ĞĞ£©
		StockChatBufferLine( moji, FONT_PAL_WHITE );
	}
	
}



// ¥Ç¥Ğ¥Ã¥°ÓÃ³öÁ¦ /////////////////////////////////////////////////////////
void lssproto_Echo_recv( int fd, char *test )
{
#if 1
#ifdef _DEBUG_MSG

	// ¬FÔÚ¤ÎÈÕ•r¤òÈ¡µÃ
	time( &serverAliveLongTime );
	serverAliveTime = localtime( &serverAliveLongTime );

#endif
#endif
}







// ¥À¥ß©`ÊÜĞÅ„IÀí
void lssproto_PlayerNumGet_recv( int fd, int logincount, int player )
{
}


void lssproto_ProcGet_recv( int fd, char* data )
{
}




/*
   ¥ì©`¥À©`¤ÎÇéˆó¤ò¤¦¤±¤È¤ë¡£

*/
void lssproto_R_recv( int fd, char* data ) 
{
}


void lssproto_D_recv( int fd,int category,int dx,int dy,char* data ) 
{
}

/*
	ÒŠ¤¨¤ë¹ ‡ì¤Î¥²©`¥à¥ª¥Ö¥¸¥§¥¯¥È¥¤¥ó¥Ç¥Ã¥¯¥¹¤ÎÁ_ÁĞ
*/
void lssproto_ACI_recv( int fd, char* data )
{
}














































#if 0

// GAMESTATE_USERINFO¤Î¤¿¤á¤ËÔO¶¨¤·¤Ê¤±¤ì¤Ğ¤Ê¤é¤Ê¤¤Çéˆó¡£
char gamestate_userinfo_cdkey[128];
char gamestate_userinfo_passwd[128];


// GAMESTATE_CHPASS¤Î¤¿¤á¤ËÔO¶¨¤·¤Ê¤±¤ì¤Ğ¤Ê¤é¤Ê¤¤Çéˆó¡£
char gamestate_chpass_oldpass[128];
char gamestate_chpass_newpass[128];

// GAMESTATE_CREATECHAR¤Î¤¿¤á¤ËÔO¶¨¤·¤Ê¤±¤ì¤Ğ¤Ê¤é¤Ê¤¤Çéˆó¡£
// ¤¤¤Ş¤ÏHP¤·¤«ÔO¶¨¤Ç¤­¤Ê¤¤¤¬¤½¤Î¤¦¤ÁÈ«²¿ÔO¶¨¤Ç¤­¤ë¤è¤¦¤Ë¤¹¤ë
int gamestate_createchar_hp;
int gamestate_createchar_mp;
int gamestate_createchar_str;
int gamestate_createchar_tough;
int gamestate_createchar_skills[128];  // ÊË˜”•ø¤Ë¤¢¤ë¥¹¥­¥ë¤Î¥³©`¥É¤ò¤Ê¤é¤Ù¤Æ¤¤¤ì¤Æ¡¢×îáá¤Ï-1
char gamestate_createchar_charname[128];
int gamestate_createchar_imgno;


#endif

