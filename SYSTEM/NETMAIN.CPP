#include "system.h"
#include <winsock.h>
#include "netmain.h"
#include "lssproto_cli.h"
#include <time.h>

//利大侃尖載嶷勣階窄嶷勣  Owatsu炎崗��僉夲頁倦壓捲暦匂
unsigned int sockfd;	// SOCKET は unsigned int。
//頁倦。宸倖炎崗朔貫server _ choosed。
int init_net;			// WSAStartupしたかどうか。このフラグがたってからserver_choosedになるのだ。
//捲暦匂頁倦僉夲潤崩議廬孵泣
int server_choosed;		// サ�`バ�`を�xびおわっているかどうかのフラグ  

char net_writebuf[NETBUFSIZ];
char net_readbuf[NETBUFSIZ];
int net_readbuflen;
int net_writebuflen;

BOOL disconnectServerFlag = FALSE;
BOOL oldDisconnectServerFlag = FALSE;

#ifdef _LOG_MSG///距編�┠∨雄予喨嬖�象晩崗��
char debugLogFileName[256] = "recvdata.txt";	// デバッグ喘��僕鞭佚デ�`タログファイル��
#endif

////階雫嶷泣
struct gameserver gmsv[] =
{
	// 坪何捲暦匂
//	{ "192.168.1.62", "9009" },
//	{ "192.168.1.63", "9013" }
#ifdef _DEBUG
	// 翌何捲暦匂
	{ "219.151.8.79","9065" },
	{ "192.168.1.106","9065" },// "127.0.0.1","9065" 
	//{ "219.151.8.79","9065" },
	{ "219.151.8.79","9065" },
		//{ "192.168.1.106","9065" },// "127.0.0.1","9065" 
	{  "219.151.8.79","9065"}

#else
	{ "","" },
	{ "","" }
#endif
};






//IP仇峽盾裂凋綜佩  階雫嶷泣 挫�駟念冤眞羌�ip port亟秘欺gmsv方怏

void ipAddressAnalyze( LPSTR CmdLine )
{
	char id;
	char *ipAdr;
	int i;

	ipAdr = CmdLine;
	while( 1 )
	{
		if( (ipAdr = strstr( ipAdr, "IP:" )  ) == NULL )
			break;

		ipAdr += 3;
		id = *ipAdr - '0';
		if( id < 0 || sizeof( gmsv )/sizeof( gmsv[0] ) <= id )
			return;

		ipAdr += 2;
		i = 0;
		while( *ipAdr != '\0' && *ipAdr != ':' )
		{
			if( i >= sizeof( gmsv[0].ipaddr )-1 )
				break;

			gmsv[id].ipaddr[i] = *ipAdr++;
			i++;
		}
		gmsv[id].ipaddr[i] = '\0';

		ipAdr++;
		i = 0;
		while( *ipAdr != '\0' && *ipAdr != ' ' && *ipAdr != '\t' )
		{
			if( i >= sizeof( gmsv[0].port )-1 )
				break;

			gmsv[id].port[i] = *ipAdr++;
			i++;
		}
	}
}


// ゲ�`ムサ�`バ�`の恷寄方を誼る��貧了ル�`チン喘) 
//?資函嗄老捲暦匂議恷寄方楚�┳９翩錬�/ / / /緩痕方音頁I/O
// この�v方はI/Oをしない
int getServerMax( void )
{
	return sizeof( gmsv )/sizeof( gmsv[0] );
}


// /資函嗄老捲暦匂議佚連   心欺
//  歌方:	int index		: 補秘0崛  -1
//  		char *hostname	: 麼字兆忖 IP仇峽
//			short *port		: 極笥催
//  卦指峙:	 0 ... 撹孔
//			-1 ... 払移
//
//  音頁I/O痕方
int getServerInfo( int index, char *hostname, short *port )
{
	if( index < 0 || index >= sizeof( gmsv)/sizeof( gmsv[0] ) )
		return -1;

	strcpy( hostname, gmsv[index].ipaddr );
	*port = atoi( gmsv[index].port );

	return 0;
}



/*
	�哀覃`プ柵び竃されて、あらゆるネットワ�`ク�I尖をする�v方。

  サ�`バ�`と��しをするのはこの�v方だけである。


  この�v方はI/Oをすごくする
耽倖儉桟瓜距喘扮��侭嗤議利大侃尖孔嬬。霧三欺捲暦匂��峪嗤宸倖孔嬬。緩孔嬬議I / O掲痕方
 */
char rpc_linebuffer[32768];     // rpc喘

extern int testCnt;

#ifdef _DEBUG
int CheckNetErrror( void ){
	
	int error = WSAGetLastError();
	// 採のエラ�`か登協
	switch ( error ){
		case WSANOTINITIALISED:
			return error;
		case WSAENETDOWN:
			return error;
		case WSAEACCES:
			return error;
		case WSAEINPROGRESS:
			return error;
		case WSAEFAULT:
			return error;
		case WSAENETRESET:
			return error;
		case WSAENOBUFS:
			return error;
		case WSAENOTCONN:
			return error;
		case WSAENOTSOCK:
			return error;
		case WSAEOPNOTSUPP:
			return error;
		case WSAESHUTDOWN:
			return error;
		case WSAEWOULDBLOCK:
			return error;
		case WSAEMSGSIZE:
			return error;
		case WSAEINVAL:
			return error;
		case WSAECONNABORTED:
			return error;
		case WSAECONNRESET:
			return error;
		case WSAETIMEDOUT:
			return error;
		default:
			return error;
	}
	
	return error;
}
#endif

#ifdef _DEBUG

/*
  喟消にWebにアクセスしつづける。
  
  do_http_request_forever()

  static な�篳�をつかうので、スレッド芦畠ではありません。

  char *ip : IPアドレス(ドット燕��の方忖のみ。兆念は�Wいので哈きません)
  unsigned short port : ポ�`ト桑催。ふつうは80。
  char *obj : ダウンロ�`ドするオブジェクト
  int sec : 採昼おきにやるか

  かえりち�� 0參貧なら屎械�K阻。��ならなにかエラ�`
  写偬恵諒利嫋喟垓。屶竃......侭參延楚��遇音頁�潦粍家�議。 ..... IP仇峽�┫�泣方忖峪哈Kimasen兆忖頁湊蛸阻。��.....�此６某攤邸Ｍ┳Ｎ�80。 .....:和墮斤��....�債穽�焚担瑛醍休触耽匯昼�哉膵�撹孔賜0。泌惚嗤焚担減中議危列
 */
static int http_sock;
static int http_call_counter = 0;
static time_t http_call_last_time;
static int http_connecting = 0;
static int http_sent_request = 0;

int
do_http_request_forever( char *ip, unsigned short port,
                         char *obj, int sec )
{
    fd_set rfds,wfds;
    int r;
    struct timeval tm;

    /* ゲ�`ムの�I尖に唹��をあたえないよに、 万唹��Ataenai嗄老侃尖  距喘參聞耽匯昼
       1昼に1指よばれるようにする */
    if( http_call_last_time != time(NULL )){
        http_call_last_time = time(NULL);
    } else {
        return 0;
    }

    //fprintf(stderr,".");
    
    if( http_call_counter == 0 ){
        struct sockaddr_in sin;

        /* sec昼に1指connectする */
        if( time(NULL) % sec != 0 )return 0;
        
        http_sock = socket( AF_INET, SOCK_STREAM, 0 );
        if( http_sock < 0 )return -1;

		unsigned long flg = 1;
		ioctlsocket( http_sock, FIONBIO, &flg );
#if 0
        fprintf(stderr,"socket()\n" );
        flags = fcntl( http_sock, F_GETFL,0);
        if( fcntl( http_sock, F_SETFL, flags|O_NONBLOCK )< 0){
            close( http_sock );
            return -2;
        }
#endif
        memset( &sin, 0 , sizeof( sin ));
        sin.sin_addr.s_addr = inet_addr( ip );
        sin.sin_port = htons( port );
        sin.sin_family = AF_INET;
        
        if( connect( http_sock, (struct sockaddr*)&sin,
                     sizeof(sin))== SOCKET_ERROR ){
	
			if( WSAGetLastError() == WSAEWOULDBLOCK )
			{
				// 云栖はブロックするはずだったんやけど
            }
			else
			{
				closesocket( http_sock );
				return -5;
			}
		}

        http_call_counter ++;
        http_sent_request = 0;
        //fprintf(stderr,"connected\n" );
        return 0;
    }

    FD_ZERO( &rfds );
    FD_SET( http_sock, &rfds );
    FD_ZERO( &wfds );
    FD_SET( http_sock, &wfds );    

    tm.tv_usec = tm.tv_sec = 0;
    r = select( http_sock+1, &rfds, &wfds,(fd_set*)NULL,&tm);

    if( r > 0 && FD_ISSET( http_sock, &rfds ) ){
        char buf[1000];
        r = recv( http_sock, buf,sizeof(buf), 0);
        if( r <= 0 ){
            closesocket( http_sock );
            /* 恷兜からやりなおし */
            http_call_counter = 0;
        }
        //fprintf(stderr,"read %d\n",r );        
    }
    
    if( r > 0 && FD_ISSET( http_sock, &wfds ) && http_sent_request == 0 ){
        /* HTTPの勣箔はMSSよりも掲械に弌さいので、
           1指のwriteでうまくいくと��協している。
           よくないコ�`ドだが、よくない�C嬬を�g�Fしようとしているので
           いいのだ�� */
        int r;
        char fuck[1000];
        sprintf( fuck, "GET %s HTTP/1.0\r\n\r\n" , obj );

        r = send( http_sock, fuck, strlen( fuck ), 0 );

        if( r <= 0 ){
            closesocket(r );
            http_call_counter = 0;
            return -10;
        }
        //fprintf(stderr,"wrote %d\n",r );
        http_sent_request = 1;
    }
    return 0;
}
#endif

void networkLoop(void)
{
	
	if( init_net == FALSE )
		return;

	if( disconnectServerFlag && !oldDisconnectServerFlag )
	{
		// ネットワ�`クが俳僅された ?�[利大瓜俳僅
		ChangeProc( PROC_DISCONNECT_SERVER );
	}
	oldDisconnectServerFlag = disconnectServerFlag;
	if( disconnectServerFlag )
		return;

    if( server_choosed == 0 )return;
	fd_set rfds , wfds, efds;
	
	struct timeval tm;
	tm.tv_sec = 0;
	tm.tv_usec = 0;

	FD_ZERO( &rfds );
	FD_ZERO( &wfds );
	FD_ZERO( &efds );

	FD_SET( sockfd , &rfds );
	FD_SET( sockfd , &wfds );
	FD_SET( sockfd , &efds );

	int a = select(  2 , &rfds , &wfds , (fd_set*)NULL, &tm );
	
	
#if 0
	if( FD_ISSET( sockfd , &efds )){
		char buf[256]; 
		memset( buf , 0 , sizeof(buf));
		int len = recv( sockfd , buf , sizeof( buf ) -1 , 0 );
		if( len == SOCKET_ERROR ){
#ifdef _DEBUG
			CheckNetErrror();
#endif
			closesocket( sockfd );
			// サ�`バ�`との俊�Aが余俳れたことを燕幣する
			disconnectServerFlag = TRUE;
		}
	}
#endif
	
	// 鞭佚する�r
	if( FD_ISSET( sockfd , &rfds) ){
		char buf[4096];
        memset(buf,0,sizeof(buf));
		int len = recv( sockfd , buf , sizeof( buf) - 1, 0 );
		
#ifdef _DEBUG
		{	// 鞭佚した�r�gを��き�zむ
			FILE *fp = fopen( "recvtime.txt", "a+" );
			if( fp ) fprintf( fp, "%u RECV %d\n", GetTickCount(), len );
			if( fp ) fclose( fp );
			
		}
#endif	

		if( len == SOCKET_ERROR ){
#ifdef _DEBUG
			CheckNetErrror();
#endif
        	closesocket(sockfd);
			// サ�`バ�`との俊�Aが余俳れたことを燕幣する
			disconnectServerFlag = TRUE;
		} else {
			appendReadBuf( buf , len );
		}
	}

	while(1){

		if( net_readbuflen > 0 ){

			//壓宸戦��匯佩蛍議方象栖臥��頁倦LSRPC1佩議窮三議。
        // ここで、1佩蛍のデ�`タがきているかどうかしらべて、LSRPCで１佩の柵び竃しをするのだ。
		
			int r= getLineFromReadBuf( rpc_linebuffer  ,sizeof(rpc_linebuffer) );
		    if( r == 0 ){
//            PUSH( "CALLING LINE---------------");
				//void receivedataFromServer(void);//layout.h
				//receivedataFromServer();	// LS2の嘔和の*咫燕幣
				lssproto_ClientDispatchMessage( sockfd , rpc_linebuffer );
#ifdef _DEBUG
		{	// ディスパッチした�r�gを��き�zむ
			FILE *fp=fopen("recvtime.txt","a+");
			if( fp ) fprintf( fp, "%u DISPATCH %s\n", GetTickCount(), rpc_linebuffer );
			if( fp ) fclose( fp );
			
		}
#endif		
            
			}else {
				break;
			}
		} else {
			break;
		}
	}

	static unsigned int writetime = GetTickCount();
	
	// 僕佚する�r
	if( FD_ISSET( sockfd , &wfds)){
		int len = send( sockfd, net_writebuf , net_writebuflen , 0 );
		/*かきこんだなり*/
		if(len > 0){
			writetime = GetTickCount();
		}
#if 0
		{

            char msg[1990] , aho[2000];
            strncpy( aho , net_writebuf , len );
            aho[len]=0;
            sprintf(msg,"sent %db %d %s" , len , WSAGetLastError() ,aho);
            PUSH(msg);
    }
#endif
		if( len == SOCKET_ERROR ){
#ifdef _DEBUG
			CheckNetErrror();
#endif
        	closesocket(sockfd);
			// サ�`バ�`との俊�Aが余俳れたことを燕幣する
			disconnectServerFlag = TRUE;
		} else {
            shiftWriteBuf( len );
		}
	}
	if(writetime + 30*1000 <GetTickCount()){
//	if(writetime + 120*1000 <GetTickCount()){
		if(	init_net == TRUE){ 
			lssproto_Echo_send(sockfd, "hoge");
			testCnt++;
#if 0
			{
				FILE *fp;
				if( (fp = fopen( "debug.txt", "w" )) != NULL )
				{
					fprintf( fp, "%ld\n", GetTickCount() );
					fclose( fp );
				}
			}
#endif
		}
		
//#ifdef DEBUGPUSH
//		PUSH("Echo");
//#endif
	}
	
#ifdef _DEBUG
	// 喟消にWebにアクセスしつづける。  喟消写偬Web恵諒。
//	if( do_http_request_forever("203.182.168.20",80,
							//"/~stoneage/newest.txt",180 ) < 0 ){
							//"/~stoneage/newest.txt",60 ) < 0 ){
							//"/~stoneage/newest.txt",10 ) < 0 ){
							//"/~stoneage/slfjfjlsf",180 ) < 0 ){
							//"/~stoneage/slfjfjlsf",60 ) < 0 ){
							//"/~stoneage/slfjfjlsf",10 ) < 0 ){
//		MessageBox( hWnd, "Ｗｅｂアクセスに払��", "�_�J", MB_OK | MB_ICONSTOP );
//	}
#endif
}










/*
   StoneAgeクライアント喘ネットワ�`ク児云ル�`チン


秘薦、竃薦バッファリング、掲揖豚、ということから、バッファ�`原きselect秘竃薦
をまたしても聞うことになる。ゲ�`ムサ�`バ�`に�vしてはこれでよい。それとは
まったく�eに、マルチスレッドでインタ�`ネットからHTTPでファイルをとってくる
返粁を喘吭する。

  ???????�[閑欧???逼�\??��?????????�F??????????��????�_??��??????�\??��??欧???逼�b????�|??????????????�箱�????黠???????蹉??????�N??��???????????HTTP????


  ネットワ�`クを聞えるようにする圭隈

  1. 恷兜のほうでinitNet()する     
  2. �哀覃`プnetworkLoopするようにル�`プの何蛍に��く 
  3. cli.cpp にある、 gmsv ��夛悶の塘双にゲ�`ムサ�`バ�`の秤�鵑���きこむ。
  4. 參貧で、ネットワ�`クが聞えるようになっている。

  1。兜豚才initNet�┌�2 a.斤噐耽倖儉桟筈議何蛍NetworkLoop眉亟。
  Cli.cpp傍��gmsv嗄老捲暦匂佚連亟議潤更方怏。 4。低駅倬聞喘利大厮将撹葎
*/



//***************************************
// ネットワ�`ク兜豚晒
//***************************************
BOOL initNet( void )
{
	int ret;
	WSADATA wsadata;

	// winsockを兜豚晒する
    ret = WSAStartup( MAKEWORD( 1, 1 ), &wsadata );
	if( ret != 0 )
		return FALSE;

    // // lssproto兜兵晒
	lssproto_InitClient( appendWriteBuf, 65536, sockfd );

#ifdef _LOG_MSG
	//晩崗猟周
	{
		// 賠茅猟周議坪否
		FILE *fp;
		fp = fopen( debugLogFileName, "w" );
		fclose(fp);
	}
	lssproto_SetClientLogFiles( debugLogFileName , debugLogFileName );
#endif

#ifdef _DEBUG
		{	// 鞭佚した�r�gを��き�zむファイルを仟�ﾗ�撹
			FILE *fp = fopen( "recvtime.txt", "w" );
			if( fp ) fclose( fp );
		}
#endif	

	init_net = TRUE;
	disconnectServerFlag = FALSE;
	oldDisconnectServerFlag = FALSE;

	return TRUE;
}


//***************************************
// 潤崩利大��嶮峭癖喘��/ 
//***************************************
void cleanupNetwork( void )
{
	if( init_net == FALSE )
		return;

	init_net = FALSE;
	server_choosed = 0;

	disconnectServerFlag = FALSE;
	oldDisconnectServerFlag = FALSE;

	closesocket( sockfd );
	WSACleanup();

	// lsrpcのクリ�`ンアップ�v方が駅勣だがまだないのだ。
	// ネットワ�`クバッファはグロ�`バルでもってるからいいのだ。
	//徽万挽隼短嗤俶勣賠尖議孔嬬。 / /産喝曝利大頁挫並��咀葎麿厮将壓畠白袈律
	lssproto_CleanupClient();
}


// リ�`ドバッファにデ�`タを弖紗
//
//  ��り�｣�	   0 ... 撹孔
//				  -1 ... バッファが怎りない
//				-100 ... サ�`バを�x�kしていない
int appendReadBuf( char *buf, int size )
{
	if( server_choosed == 0 )
		return -100;

	if( (net_readbuflen + size) > NETBUFSIZ )
        return -1;

	memcpy( net_readbuf + net_readbuflen, buf, size );
	net_readbuflen += size;

	return 0;
}


// ライトバッファにデ�`タを弖紗
//
//  ��り�｣�	   0 ... 撹孔
//				  -1 ... バッファが怎りない
//				-100 ... サ�`バを�x�kしていない
int appendWriteBuf( int index, char *buf, int size )
{
	if( server_choosed == 0 )
		return -100;

	if( (net_writebuflen + size) > NETBUFSIZ )
		return -1;

	memcpy( net_writebuf + net_writebuflen, buf, size );
	net_writebuflen += size;

	return 0;
}


// リ�`ドバッファのデ�`タをsizeバイト蛍念にシフト
// ��恷兜からsizeバイト蛍のデ�`タが��される��
//
//  ��り�｣�	   0 ... 撹孔
//				  -1 ... バッファを階えている
//				-100 ... サ�`バ�x�kされてない
int shiftReadBuf( int size )
{
	int i;

	if( server_choosed == 0 )
		return -100;

	if( size >= NETBUFSIZ )
		return -1;

	for( i = size; i < NETBUFSIZ; i++ )
	{
		net_readbuf[i-size] = net_readbuf[i];
	}
	net_readbuflen -= size;

	return 0;
}


// ライトバッファのデ�`タをsizeバイト蛍念にシフト
// ��恷兜からsizeバイト蛍のデ�`タが��される��
//
//  ��り�｣�	   0 ... 撹孔
//				  -1 ... バッファを階えている
//				-100 ... サ�`バ�x�kされてない
int shiftWriteBuf( int size )
{
	int i;

	if( server_choosed == 0 )
		return -100;

	if( size >= NETBUFSIZ )
		return -1;

	for( i = size; i < NETBUFSIZ; i++ )
	{
		net_writebuf[i-size] = net_writebuf[i];
	}
	net_writebuflen -= size;

	return 0;
}


// リ�`ドバッファから１佩�┯槻侘鍔屬泙任領鍔崛丕�を函り竃す
//
//  ��り�｣�	   0 ... 撹孔
//				  -1 ... １佩蛍デ�`タが�oい
//				-100 ... サ�`バが�x�kされてない
int getLineFromReadBuf( char *output, int maxlen )
{
	int i, j;

	if( server_choosed == 0 )
		return -100;

	for( i = 0; i < net_readbuflen && i < (maxlen-1); i++ )
	{
		if( net_readbuf[i] == '\n' )
		{
			memcpy( output, net_readbuf, i );
			output[i] = '\0';
			// 竃薦を�{屁。猟忖双の恷瘁から�碧�して 0x0dがあったら��す
			for( j = i+1; j >= 0; j-- )
			{
				if( output[j] == 0x0d )
				{
					output[j] = '\0';
					break;
				}
			}

			// ���Mした蛍シフトする
			shiftReadBuf( i+1 );

			// バッファの翌�箸烹吋丱ぅ函▲魅詢鍔屬鬚弔韻襦�
			// シフト荷恬の�rはバッファの瘁ろは篤�欧気譴覆い箸いκ���に児づいている。
			net_readbuf[net_readbuflen] = '\0';

			return 0;
		}
	}

	return -1;
}


// ソケットに�_�gにlenバイト僕佚する
//
//  ��り�｣�	0參貧 ... 僕佚したバイト方
//				 -100 ... サ�`バが�x�kされていない
int sendn( SOCKET s, char *buffer, int len )
{
	int total = 0;
	int r;

	if( server_choosed == 0 )
		return -100;

	while( 1 )
	{
		r = send( s, buffer, len, 0 );
		if( r == SOCKET_ERROR )
			return  SOCKET_ERROR;
		total += r;
		if( total == len )
			return total;
	}
}

