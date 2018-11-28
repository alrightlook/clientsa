#include "system.h"
#include <winsock.h>
#include "netmain.h"
#include "lssproto_cli.h"
#include <time.h>

//���紦������Ҫ������Ҫ  Owatsu��־��ѡ���Ƿ��ڷ�����
unsigned int sockfd;	// SOCKET �� unsigned int��
//�Ƿ������־���server _ choosed��
int init_net;			// WSAStartup�������ɤ��������Υե饰�����äƤ���server_choosed�ˤʤ�Τ���
//�������Ƿ�ѡ�������ת�۵�
int server_choosed;		// ���`�Щ`���x�Ӥ���äƤ��뤫�ɤ����Υե饰  

char net_writebuf[NETBUFSIZ];
char net_readbuf[NETBUFSIZ];
int net_readbuflen;
int net_writebuflen;

BOOL disconnectServerFlag = FALSE;
BOOL oldDisconnectServerFlag = FALSE;

#ifdef _LOG_MSG///���ԣ����ͺͽ���������־��
char debugLogFileName[256] = "recvdata.txt";	// �ǥХå��ã������ťǩ`�������ե����룩
#endif

////�����ص�
struct gameserver gmsv[] =
{
	// �ڲ�������
//	{ "192.168.1.62", "9009" },
//	{ "192.168.1.63", "9013" }
#ifdef _DEBUG
	// �ⲿ������
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






//IP��ַ����������  �����ص� �����ǰ������ip portд�뵽gmsv����

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


// ���`�ॵ�`�Щ`���������ä루��λ��`������) 
//?��ȡ��Ϸ����������������������ϣ�/ / / /�˺�������I/O
// �����v����I/O�򤷤ʤ�
int getServerMax( void )
{
	return sizeof( gmsv )/sizeof( gmsv[0] );
}


// /��ȡ��Ϸ����������Ϣ   ����
//  ����:	int index		: ����0��  -1
//  		char *hostname	: �������� IP��ַ
//			short *port		: �˿ں�
//  ����ֵ:	 0 ... �ɹ�
//			-1 ... ʧ��
//
//  ����I/O����
int getServerInfo( int index, char *hostname, short *port )
{
	if( index < 0 || index >= sizeof( gmsv)/sizeof( gmsv[0] ) )
		return -1;

	strcpy( hostname, gmsv[index].ipaddr );
	*port = atoi( gmsv[index].port );

	return 0;
}



/*
	����`�׺��ӳ�����ơ�������ͥåȥ�`���I���򤹤��v����

  ���`�Щ`��Ԓ���򤹤�ΤϤ����v�������Ǥ��롣


  �����v����I/O�򤹤�������
ÿ��ѭ��������ʱ�����е����紦�����ܡ�̸������������ֻ��������ܡ��˹��ܵ�I / O�Ǻ���
 */
char rpc_linebuffer[32768];     // rpc��

extern int testCnt;

#ifdef _DEBUG
int CheckNetErrror( void ){
	
	int error = WSAGetLastError();
	// �ΤΥ���`���ж�
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
  ���ä�Web�˥����������ĤŤ��롣
  
  do_http_request_forever()

  static �ʉ�����Ĥ����Τǡ�����åɰ�ȫ�ǤϤ���ޤ���

  char *ip : IP���ɥ쥹(�ɥåȱ�ӛ�����֤Τߡ���ǰ���W���Τ������ޤ���)
  unsigned short port : �ݩ`�ȷ��š��դĤ���80��
  char *obj : ��������`�ɤ��륪�֥�������
  int sec : ���뤪���ˤ�뤫

  ��������� 0���Ϥʤ������K�ˡ�ؓ�ʤ�ʤˤ�����`
  ����������վ��Զ��֧��......���Ա������������̰߳�ȫ�ġ� ..... IP��ַ����������ֻ��Kimasen������̫���ˡ���.....�����˿ںš�ͨ��Ϊ80�� .....:���ض���....������ʲô����ٿ�ÿһ�룺����ɹ���0�������ʲô����Ĵ���
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

    /* ���`��΄I����Ӱ푤򤢤����ʤ���ˡ� ��Ӱ��Ataenai��Ϸ����  ������ʹÿһ��
       1���1�ؤ�Ф��褦�ˤ��� */
    if( http_call_last_time != time(NULL )){
        http_call_last_time = time(NULL);
    } else {
        return 0;
    }

    //fprintf(stderr,".");
    
    if( http_call_counter == 0 ){
        struct sockaddr_in sin;

        /* sec���1��connect���� */
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
				// �����ϥ֥��å�����Ϥ����ä���䤱��
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
            /* ���������ʤ��� */
            http_call_counter = 0;
        }
        //fprintf(stderr,"read %d\n",r );        
    }
    
    if( r > 0 && FD_ISSET( http_sock, &wfds ) && http_sent_request == 0 ){
        /* HTTP��Ҫ���MSS����ǳ���С�����Τǡ�
           1�ؤ�write�Ǥ��ޤ������ȁ������Ƥ��롣
           �褯�ʤ����`�ɤ������褯�ʤ��C�ܤ�g�F���褦�Ȥ��Ƥ���Τ�
           �����Τ��� */
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
		// �ͥåȥ�`�����жϤ��줿 ?�[���类�ж�
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
			// ���`�Щ`�ȤνӾA��;�Ф줿���Ȥ��ʾ����
			disconnectServerFlag = TRUE;
		}
	}
#endif
	
	// ���Ť���r
	if( FD_ISSET( sockfd , &rfds) ){
		char buf[4096];
        memset(buf,0,sizeof(buf));
		int len = recv( sockfd , buf , sizeof( buf) - 1, 0 );
		
#ifdef _DEBUG
		{	// ���Ť����r�g������z��
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
			// ���`�Щ`�ȤνӾA��;�Ф줿���Ȥ��ʾ����
			disconnectServerFlag = TRUE;
		} else {
			appendReadBuf( buf , len );
		}
	}

	while(1){

		if( net_readbuflen > 0 ){

			//�����һ�зֵ��������飬�Ƿ�LSRPC1�еĵ绰�ġ�
        // �����ǡ�1�з֤Υǩ`�������Ƥ��뤫�ɤ�������٤ơ�LSRPC�ǣ��Фκ��ӳ����򤹤�Τ���
		
			int r= getLineFromReadBuf( rpc_linebuffer  ,sizeof(rpc_linebuffer) );
		    if( r == 0 ){
//            PUSH( "CALLING LINE---------------");
				//void receivedataFromServer(void);//layout.h
				//receivedataFromServer();	// LS2�����¤�*ӡ��ʾ
				lssproto_ClientDispatchMessage( sockfd , rpc_linebuffer );
#ifdef _DEBUG
		{	// �ǥ����ѥå������r�g������z��
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
	
	// ���Ť���r
	if( FD_ISSET( sockfd , &wfds)){
		int len = send( sockfd, net_writebuf , net_writebuflen , 0 );
		/*����������ʤ�*/
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
			// ���`�Щ`�ȤνӾA��;�Ф줿���Ȥ��ʾ����
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
	// ���ä�Web�˥����������ĤŤ��롣  ���ü���Web���ʡ�
//	if( do_http_request_forever("203.182.168.20",80,
							//"/~stoneage/newest.txt",180 ) < 0 ){
							//"/~stoneage/newest.txt",60 ) < 0 ){
							//"/~stoneage/newest.txt",10 ) < 0 ){
							//"/~stoneage/slfjfjlsf",180 ) < 0 ){
							//"/~stoneage/slfjfjlsf",60 ) < 0 ){
							//"/~stoneage/slfjfjlsf",10 ) < 0 ){
//		MessageBox( hWnd, "�ף�⥢��������ʧ��", "�_�J", MB_OK | MB_ICONSTOP );
//	}
#endif
}










/*
   StoneAge���饤������åͥåȥ�`��������`����


�����������Хåե���󥰡���ͬ�ڡ��Ȥ������Ȥ��顢�Хåե��`����select�����
��ޤ����Ƥ�ʹ�����Ȥˤʤ롣���`�ॵ�`�Щ`���v���ƤϤ���Ǥ褤������Ȥ�
�ޤä����e�ˡ��ޥ������åɤǥ��󥿩`�ͥåȤ���HTTP�ǥե������ȤäƤ���
�ֶΤ����⤹�롣

  ???????�[�ײ�???ɯ�\??Ξ?????????�F??????????��????�_??��??????�\??ϒ??��???ɯ�b????�|??????????????�Ȣ�????��???????��??????�N??��???????????HTTP????


  �ͥåȥ�`����ʹ����褦�ˤ��뷽��

  1. ����Τۤ���initNet()����     
  2. ����`��networkLoop����褦�˥�`�פβ��֤˕��� 
  3. cli.cpp �ˤ��롢 gmsv ����������Ф˥��`�ॵ�`�Щ`������������ࡣ
  4. ���Ϥǡ��ͥåȥ�`����ʹ����褦�ˤʤäƤ��롣

  1�����ں�initNet����2 a.����ÿ��ѭ��Ȧ�Ĳ���NetworkLoop��д��
  Cli.cpp˵��gmsv��Ϸ��������Ϣд�Ľṹ���顣 4�������ʹ�������Ѿ���Ϊ
*/



//***************************************
// �ͥåȥ�`�����ڻ�
//***************************************
BOOL initNet( void )
{
	int ret;
	WSADATA wsadata;

	// winsock����ڻ�����
    ret = WSAStartup( MAKEWORD( 1, 1 ), &wsadata );
	if( ret != 0 )
		return FALSE;

    // // lssproto��ʼ��
	lssproto_InitClient( appendWriteBuf, 65536, sockfd );

#ifdef _LOG_MSG
	//��־�ļ�
	{
		// ����ļ�������
		FILE *fp;
		fp = fopen( debugLogFileName, "w" );
		fclose(fp);
	}
	lssproto_SetClientLogFiles( debugLogFileName , debugLogFileName );
#endif

#ifdef _DEBUG
		{	// ���Ť����r�g������z��ե��������Ҏ����
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
// �������磨��ֹ���ã�/ 
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

	// lsrpc�Υ���`�󥢥å��v������Ҫ�����ޤ��ʤ��Τ���
	// �ͥåȥ�`���Хåե��ϥ����`�Х�Ǥ�äƤ뤫�餤���Τ���
	//������Ȼû����Ҫ�����Ĺ��ܡ� / /�����������Ǻ��£���Ϊ���Ѿ���ȫ��Χ
	lssproto_CleanupClient();
}


// ��`�ɥХåե��˥ǩ`����׷��
//
//  ���ꂎ��	   0 ... �ɹ�
//				  -1 ... �Хåե������ʤ�
//				-100 ... ���`�Ф��x�k���Ƥ��ʤ�
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


// �饤�ȥХåե��˥ǩ`����׷��
//
//  ���ꂎ��	   0 ... �ɹ�
//				  -1 ... �Хåե������ʤ�
//				-100 ... ���`�Ф��x�k���Ƥ��ʤ�
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


// ��`�ɥХåե��Υǩ`����size�Х��ȷ�ǰ�˥��ե�
// ���������size�Х��ȷ֤Υǩ`����������룩
//
//  ���ꂎ��	   0 ... �ɹ�
//				  -1 ... �Хåե��򳬤��Ƥ���
//				-100 ... ���`���x�k����Ƥʤ�
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


// �饤�ȥХåե��Υǩ`����size�Х��ȷ�ǰ�˥��ե�
// ���������size�Х��ȷ֤Υǩ`����������룩
//
//  ���ꂎ��	   0 ... �ɹ�
//				  -1 ... �Хåե��򳬤��Ƥ���
//				-100 ... ���`���x�k����Ƥʤ�
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


// ��`�ɥХåե����飱�У��������֤ޤǤ������У���ȡ�����
//
//  ���ꂎ��	   0 ... �ɹ�
//				  -1 ... ���з֥ǩ`�����o��
//				-100 ... ���`�Ф��x�k����Ƥʤ�
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
			// �������{���������Ф����ᤫ��������� 0x0d�����ä�������
			for( j = i+1; j >= 0; j-- )
			{
				if( output[j] == 0x0d )
				{
					output[j] = '\0';
					break;
				}
			}

			// ���M�����֥��եȤ���
			shiftReadBuf( i+1 );

			// �Хåե�����Ȥˣ��Х��ȡ��̥����֤�Ĥ��롣
			// ���եȲ����Εr�ϥХåե���������Ɖ�����ʤ��Ȥ����˘��˻��Ť��Ƥ��롣
			net_readbuf[net_readbuflen] = '\0';

			return 0;
		}
	}

	return -1;
}


// �����åȤ˴_�g��len�Х������Ť���
//
//  ���ꂎ��	0���� ... ���Ť����Х�����
//				 -100 ... ���`�Ф��x�k����Ƥ��ʤ�
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
