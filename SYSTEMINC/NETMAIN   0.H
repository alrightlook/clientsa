#ifndef _NETMAIN_H_
#define _NETMAIN_H_

// �Q�[���T�[�o�[�̊Ǘ��p�\����
struct gameserver
{
	char ipaddr[128];		// IP�A�h���X
	char port[64];			// �|�[�g
};


#define SUCCESSFULSTR	"successful"        // �v���g�R���Ŏg�������萔�Q
#define FAILEDSTR		"failed" 
#define OKSTR           "ok"

#define TIMEOUT (600*1000)                   //  �l�b�g���[�N�̂��ׂĂ̂Ƃ���ł���������ߎ���

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


// �G���[���b�Z�[�W
#define NET_ERRMSG_SOCKLIBERROR		"�����|���ǳƦn�C" //"�l�b�g���[�N�̏������o���Ă��܂���B"
#define NET_ERRMSG_BADNAME          "���A���W�٤����T�C" //"�T�[�o�����s���ł��B"
#define NET_ERRMSG_SOCKETERROR      "�L�k����TCP�C" //"�l�b�g���[�N�\�P�b�g(TCP)���쐬���邱�Ƃ��ł��܂���B"
#define NET_ERRMSG_NOTGETADDR		"�L�k��o���A�����עަ�m�C(%s)" //"�T�[�o��IP�A�h���X�𓾂邱�Ƃ��ł��܂���B(%s)"
#define NET_ERRMSG_NOTCONNECT_S		"�L�k�}�l�i����A�����s���C" //"�T�[�o�ɐڑ����鏈�����J�n���邱�Ƃ��ł��܂���B"
#define NET_ERRMSG_NOTCONNECT		"�L�k�s�����A���C" //"�T�[�o�ɐڑ����邱�Ƃ��ł��܂���B "
#define NET_ERRMSG_CONNECTTIMEOUT	"���A���s���ɶ��w��C" //"�T�[�o�ڑ����Ƀ^�C���A�E�g���܂����B" 
#define NET_ERRMSG_LOGINTIMEOUT		"�n�X�B�z�ɶ��w��C" //"���O�C���������Ƀ^�C���A�E�g���܂����B"
#define NET_ERRMSG_CHARLISTTIMEOUT	"Ū���H�����ɶ��w��C" //"�L�����N�^���X�g���擾���Ƀ^�C���A�E�g���܂����B"
#define NET_ERRMSG_LOGOUTTIMEOUT	"�n�X�B�z�ɶ��w��C" //"���O�A�E�g�������Ƀ^�C���A�E�g���܂����B"
#define NET_ERRMSG_LOGINFAIL		"�L�k�n�J���A���C" //"���O�C���o���܂���ł����B"
#define NET_ERRMSG_CREATECHARTIMEOUT	"�H���s�@�ɶ��w��C" //"�L�����N�^���쐬���Ƀ^�C���A�E�g���܂����B"
#define NET_ERRMSG_DELETECHARTIMEOUT    "�H���d���ɶ��w��C" //"�L�����N�^���폜���Ƀ^�C���A�E�g���܂����B"



#define GAMESTATE_ERRMSG_LOGOUTDENY        "���A���ڵ��B�z�n�X�C" //"���O�A�E�g�������T�[�o�ɋ��ۂ���܂����B"

#if 0
#define GAMESTATE_ERRMSG_NEEDSERVERNAME   "�S�����w���A���W�١C" //"�T�[�o�̖��O���w�肳��Ă��܂���B"
#define GAMESTATE_ERRMSG_NEEDCHOOSE       "������ܦ��A���h�L�k�i��Ӷ��B�z�C" //"�T�[�o��I�����Ă���łȂ��Ƃ��̏����͎��s�ł��܂���B" 
#define GAMESTATE_ERRMSG_CANTSAVEUSERINFO "�L�k�O�s�ϥΪ̪������C�ϺЮe�q�����C" //"���[�U�����t�@�C���ɕۑ��ł��܂���B�f�B�X�N�̋󂫗e�ʂ�����܂���B"
#define GAMESTATE_ERRMSG_CLIENTLOGINERROR "�L�k�n�J���A���C" //"�T�[�o�Ƀ��O�C���ł��܂���B"
#define GAMESTATE_ERRMSG_CLIENTLOGINTIMEOUT "�n�J���A���ɶ��w��C" //"�T�[�o�Ƀ��O�C�����鏈�����^�C���A�E�g���܂����B"
#define GAMESTATE_ERRMSG_CANNOTLOADPASSWD "�ѩ�|����J�ϥΪ̱����A�L�k�ܧ�K�X�C�Х���J�ϥΪ̱����C" //"�܂����[�U��񂪓��͂���Ă��Ȃ��̂ŁA�p�X���[�h��ύX���邱�Ƃ͂ł��܂���B�܂����[�U�[������͂��Ă��������B"
#define GAMESTATE_ERRMSG_CANNOTSAVEPASSWD "�L�k�O�s�ϥΪ̱����A�K�X�ܧ󥢱ѡC" //"���[�U�����t�@�C���ɕۑ��ł��Ȃ������̂ŁA�p�X���[�h�ύX�Ɏ��s���܂����B"
#define GAMESTATE_ERRMSG_CHPASSERROR      "�L�k�ܧ�K�X�C" //"�p�X���[�h�̕ύX���ł��܂���ł����B"
#define GAMESTATE_ERRMSG_CHPASSTIMEOUT    "�K�X�ܧ�ɶ��w��C" //"�p�X���[�h�ύX�������^�C���A�E�g���܂����B"
#define GAMESTATE_ERRMSG_DELETECHARSERVERDENY    "���A���ڵ��B�z�d���H���C" //"�L�����폜�������T�[�o�ɋ��ۂ���܂����B"
#define GAMESTATE_ERRMSG_CHARLISTDENY      "���A���ڵ��B�zŪ���H�����C" //"�L�����N�^�[�\�擾�������T�[�o�ɋ��ۂ���܂����B"
#define GAMESTATE_ERRMSG_LOGINTIMEOUT			"�n�J�B�z�ɶ��w��C" //"���O�C���������^�C���A�E�g���܂����B"
#define GAMESTATE_ERRMSG_NOTIMPLEMENTED   "�|���˸m�ӥ\��C" //"�܂����̋@�\�͎�������ĂȂ��Ȃ�"
#endif


#endif