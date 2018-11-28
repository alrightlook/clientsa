#include "system.h"
#include <winsock.h>
#include "netmain.h"
#include "lssproto_cli.h"
#include <time.h>

//网络处理很重要超乎重要  Owatsu标志，选择是否在服务器
unsigned int sockfd;	// SOCKET は unsigned int。
//是否。这个标志后从server _ choosed。
int init_net;			// WSAStartupしたかどうか。このフラグがたってからserver_choosedになるのだ。
//服务器是否选择结束的转折点
int server_choosed;		// サーバーを選びおわっているかどうかのフラグ  

char net_writebuf[NETBUFSIZ];
char net_readbuf[NETBUFSIZ];
int net_readbuflen;
int net_writebuflen;

BOOL disconnectServerFlag = FALSE;
BOOL oldDisconnectServerFlag = FALSE;

#ifdef _LOG_MSG///调试（发送和接收数据日志）
char debugLogFileName[256] = "recvdata.txt";	// デバッグ用（送受信データログファイル）
#endif

////超级重点
struct gameserver gmsv[] =
{
	// 内部服务器
//	{ "192.168.1.62", "9009" },
//	{ "192.168.1.63", "9013" }
#ifdef _DEBUG
	// 外部服务器
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






//IP地址解析命令行  超级重点 好像是把外面的ip port写入到gmsv数组

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


// ゲームサーバーの最大数を得る（上位ルーチン用) 
//?获取游戏服务器的最大数量（常规上）/ / / /此函数不是I/O
// この関数はI/Oをしない
int getServerMax( void )
{
	return sizeof( gmsv )/sizeof( gmsv[0] );
}


// /获取游戏服务器的信息   看到
//  参数:	int index		: 输入0至  -1
//  		char *hostname	: 主机名字 IP地址
//			short *port		: 端口号
//  返回值:	 0 ... 成功
//			-1 ... 失败
//
//  不是I/O函数
int getServerInfo( int index, char *hostname, short *port )
{
	if( index < 0 || index >= sizeof( gmsv)/sizeof( gmsv[0] ) )
		return -1;

	strcpy( hostname, gmsv[index].ipaddr );
	*port = atoi( gmsv[index].port );

	return 0;
}



/*
	毎ループ呼び出されて、あらゆるネットワーク処理をする関数。

  サーバーと話しをするのはこの関数だけである。


  この関数はI/Oをすごくする
每个循环被调用时，所有的网络处理功能。谈话到服务器，只有这个功能。此功能的I / O非函数
 */
char rpc_linebuffer[32768];     // rpc用

extern int testCnt;

#ifdef _DEBUG
int CheckNetErrror( void ){
	
	int error = WSAGetLastError();
	// 何のエラーか判定
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
  永久にWebにアクセスしつづける。
  
  do_http_request_forever()

  static な変数をつかうので、スレッド安全ではありません。

  char *ip : IPアドレス(ドット表記の数字のみ。名前は遅いので引きません)
  unsigned short port : ポート番号。ふつうは80。
  char *obj : ダウンロードするオブジェクト
  int sec : 何秒おきにやるか

  かえりち： 0以上なら正常終了。負ならなにかエラー
  继续访问网站永远。支出......所以变量，而不是线程安全的。 ..... IP地址（带点数字只引Kimasen名字是太慢了。）.....：。端口号。通常为80。 .....:下载对象....：你做什么蓖麻蒂卡每一秒：如果成功或0。如果有什么负面的错误
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

    /* ゲームの処理に影響をあたえないよに、 它影响Ataenai游戏处理  调用以使每一秒
       1秒に1回よばれるようにする */
    if( http_call_last_time != time(NULL )){
        http_call_last_time = time(NULL);
    } else {
        return 0;
    }

    //fprintf(stderr,".");
    
    if( http_call_counter == 0 ){
        struct sockaddr_in sin;

        /* sec秒に1回connectする */
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
				// 本来はブロックするはずだったんやけど
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
            /* 最初からやりなおし */
            http_call_counter = 0;
        }
        //fprintf(stderr,"read %d\n",r );        
    }
    
    if( r > 0 && FD_ISSET( http_sock, &wfds ) && http_sent_request == 0 ){
        /* HTTPの要求はMSSよりも非常に小さいので、
           1回のwriteでうまくいくと仮定している。
           よくないコードだが、よくない機能を実現しようとしているので
           いいのだ！ */
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
		// ネットワークが切断された ?网络被切断
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
			// サーバーとの接続が途切れたことを表示する
			disconnectServerFlag = TRUE;
		}
	}
#endif
	
	// 受信する時
	if( FD_ISSET( sockfd , &rfds) ){
		char buf[4096];
        memset(buf,0,sizeof(buf));
		int len = recv( sockfd , buf , sizeof( buf) - 1, 0 );
		
#ifdef _DEBUG
		{	// 受信した時間を書き込む
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
			// サーバーとの接続が途切れたことを表示する
			disconnectServerFlag = TRUE;
		} else {
			appendReadBuf( buf , len );
		}
	}

	while(1){

		if( net_readbuflen > 0 ){

			//在这里，一行分的数据来查，是否LSRPC1行的电话的。
        // ここで、1行分のデータがきているかどうかしらべて、LSRPCで１行の呼び出しをするのだ。
		
			int r= getLineFromReadBuf( rpc_linebuffer  ,sizeof(rpc_linebuffer) );
		    if( r == 0 ){
//            PUSH( "CALLING LINE---------------");
				//void receivedataFromServer(void);//layout.h
				//receivedataFromServer();	// LS2の右下の*印表示
				lssproto_ClientDispatchMessage( sockfd , rpc_linebuffer );
#ifdef _DEBUG
		{	// ディスパッチした時間を書き込む
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
	
	// 送信する時
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
			// サーバーとの接続が途切れたことを表示する
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
	// 永久にWebにアクセスしつづける。  永久继续Web访问。
//	if( do_http_request_forever("203.182.168.20",80,
							//"/~stoneage/newest.txt",180 ) < 0 ){
							//"/~stoneage/newest.txt",60 ) < 0 ){
							//"/~stoneage/newest.txt",10 ) < 0 ){
							//"/~stoneage/slfjfjlsf",180 ) < 0 ){
							//"/~stoneage/slfjfjlsf",60 ) < 0 ){
							//"/~stoneage/slfjfjlsf",10 ) < 0 ){
//		MessageBox( hWnd, "Ｗｅｂアクセスに失敗", "確認", MB_OK | MB_ICONSTOP );
//	}
#endif
}










/*
   StoneAgeクライアント用ネットワーク基本ルーチン


入力、出力バッファリング、非同期、ということから、バッファー付きselect入出力
をまたしても使うことになる。ゲームサーバーに関してはこれでよい。それとは
まったく別に、マルチスレッドでインターネットからHTTPでファイルをとってくる
手段を用意する。

  ???????醋菠???莎??螢???????????????????????瞋??????????蠏??菠???莎????簗??????????????????筌???????恝????????獰???????????HTTP????


  ネットワークを使えるようにする方法

  1. 最初のほうでinitNet()する     
  2. 毎ループnetworkLoopするようにループの部分に書く 
  3. cli.cpp にある、 gmsv 構造体の配列にゲームサーバーの情報を書きこむ。
  4. 以上で、ネットワークが使えるようになっている。

  1。初期和initNet（）2 a.对于每个循环圈的部分NetworkLoop三写。
  Cli.cpp说，gmsv游戏服务器信息写的结构数组。 4。你必须使用网络已经成为
*/



//***************************************
// ネットワーク初期化
//***************************************
BOOL initNet( void )
{
	int ret;
	WSADATA wsadata;

	// winsockを初期化する
    ret = WSAStartup( MAKEWORD( 1, 1 ), &wsadata );
	if( ret != 0 )
		return FALSE;

    // // lssproto初始化
	lssproto_InitClient( appendWriteBuf, 65536, sockfd );

#ifdef _LOG_MSG
	//日志文件
	{
		// 清除文件的内容
		FILE *fp;
		fp = fopen( debugLogFileName, "w" );
		fclose(fp);
	}
	lssproto_SetClientLogFiles( debugLogFileName , debugLogFileName );
#endif

#ifdef _DEBUG
		{	// 受信した時間を書き込むファイルを新規作成
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
// 结束网络（终止适用）/ 
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

	// lsrpcのクリーンアップ関数が必要だがまだないのだ。
	// ネットワークバッファはグローバルでもってるからいいのだ。
	//但它仍然没有需要清理的功能。 / /缓冲区网络是好事，因为他已经在全球范围
	lssproto_CleanupClient();
}


// リードバッファにデータを追加
//
//  戻り値：	   0 ... 成功
//				  -1 ... バッファが足りない
//				-100 ... サーバを選択していない
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


// ライトバッファにデータを追加
//
//  戻り値：	   0 ... 成功
//				  -1 ... バッファが足りない
//				-100 ... サーバを選択していない
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


// リードバッファのデータをsizeバイト分前にシフト
// （最初からsizeバイト分のデータが消される）
//
//  戻り値：	   0 ... 成功
//				  -1 ... バッファを超えている
//				-100 ... サーバ選択されてない
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


// ライトバッファのデータをsizeバイト分前にシフト
// （最初からsizeバイト分のデータが消される）
//
//  戻り値：	   0 ... 成功
//				  -1 ... バッファを超えている
//				-100 ... サーバ選択されてない
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


// リードバッファから１行（改行文字までの文字列）を取り出す
//
//  戻り値：	   0 ... 成功
//				  -1 ... １行分データが無い
//				-100 ... サーバが選択されてない
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
			// 出力を調整。文字列の最後から検索して 0x0dがあったら消す
			for( j = i+1; j >= 0; j-- )
			{
				if( output[j] == 0x0d )
				{
					output[j] = '\0';
					break;
				}
			}

			// 消費した分シフトする
			shiftReadBuf( i+1 );

			// バッファの外側に１バイト、ヌル文字をつける。
			// シフト操作の時はバッファの後ろは破壊されないという仕様に基づいている。
			net_readbuf[net_readbuflen] = '\0';

			return 0;
		}
	}

	return -1;
}


// ソケットに確実にlenバイト送信する
//
//  戻り値：	0以上 ... 送信したバイト数
//				 -100 ... サーバが選択されていない
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

