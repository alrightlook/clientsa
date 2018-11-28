/**** SYSTEM INCLUDE ****/
#include "system.h"
#include "winnls32.h"
#include <ime.h>
#include <imm.h>
#include "ime_sa.h"
#include "../resource.h"
#include "netmain.h"
#include "battleMap.h"
#include "battleProc.h"
#include "netmain.h"


#if 1
// ͨ���r
#define REALBIN_DIR		"data\\real_1.bin" 		// REAL.BIN�Έ���
#define ADRNBIN_DIR		"data\\adrn_1.bin" 		// ADRN.BIN�Έ���
#define SPRBIN_DIR		"data\\spr_1.bin" 		// SPR.BIN�Έ���
#define SPRADRNBIN_DIR	"data\\spradrn_1.bin" 	// SPRADRN.BIN�Έ���

#else

// ����ե��å��ޥ�����
#define REALBIN_DIR		"..\\realbin\\data\\real.bin" 		// REAL.BIN�Έ���
#define ADRNBIN_DIR		"..\\realbin\\data\\adrn.bin" 		// ADRN.BIN�Έ���
#define SPRBIN_DIR		"..\\spr\\spr.bin" 					// SPR.BIN�Έ���
#define SPRADRNBIN_DIR	"..\\spr\\spradrn.bin" 				// SPRADRN.BIN�Έ���

#endif


//---------------------------------------------------------------------------//
// ��Ҫ �������`�Х�������x                                                 //
//---------------------------------------------------------------------------//
HINSTANCE	hInst;			// ���󥹥��󥹥ϥ�ɥ붨�x ʵ�����ھ������
HWND hWnd;					// ������ɥ��ϥ�ɥ�  ����
int	CmdShow;				// WinMain�v���������򥰥��`�Х�ˤ���  WinMain�������۾ݺ�ȫ���
LPSTR CmdLine;				// WinMain�v���Υ��ޥ�ɥ饤�������򥰥��`�Х�ˤ���  WinMain�����������в���ȫ���
HANDLE hMutex;				// ͬ�r���ӥ����å����֥������ȤΥϥ�ɥ�  ͬʱ����������ķ�����

BOOL WindowMode = TRUE;		// ������ɥ���`��/ / / /����ģʽ
//BOOL WindowMode = FALSE;	// �ե륹����`���`��/ /????????
							//	TRUE  : WindowMode
							//	FALSE : FullScreen
BOOL NoDelay = TRUE;		// TCP_NODELAY ��ʹ�����ɤ�����add by ringo  TCP_NODELAY�Ƿ�ʹ�� һ�ַ���Ż�
int ResoMode = 0;			// ����ȥ�`�ɣ����������������������������������������������ɉ䣩�ֱ���ģʽ
int LowResoCmdFlag = 0;		// ����ȉ�����ޥ�ɥե饰  �ֱ��ʱ��ָ��ת�۵�

#ifdef _DEBUG
//BOOL offlineFlag = FALSE;	// ���ե饤��ե饰 ����ת�۵�  �ѻ�ģʽ
BOOL offlineFlag = FALSE;  //û���Ժ��ԭ��
char DebugKey0[ 256 ];		// �ǥХå����`�� ���Լ�0
char DebugKey1[ 256 ];		// �ǥХå����`��
char DebugKey2[ 256 ];		// �ǥХå����`��
#endif


char realBinName[256];
char adrnBinName[256];
char sprBinName[256];
char sprAdrnBinName[256];


/* ���ޥ�ɥ饤�����   �����н��� */
void AnalyzeCmdLine( void );
// ������ɥ���`���Ф��椨�v������
//����ģʽת����������
void ChangeWindowMode( void );
// ����ȥ�`���O���I��
//�ֱ���ģʽ�趨����
void SetResoMode(int Mode);

// Windows�Υ�å��`����եå�����  ����Ϣ�Ĺ�
//HRESULT CALLBACK HookProc( int nCode, WPARAM wParam, LPARAM lParam );				//�ᥤ��ץ����`����`

//---------------------------------------------------------------------------//
// ��Ҫ ���ᥤ���`�ׄI���v��                                               //
// ���� ��HANDLE hInstance                                                   //
//        HANDLE hPrevInstance                                               //	
//        LPSTR lpCmdLine                                                    //
//        int   nCmdShow                                                     //
//---------------------------------------------------------------------------//
int PASCAL WinMain( HINSTANCE hInstance ,HINSTANCE hPrevInstance ,LPSTR lpCmdLine ,int nCmdShow )
{
	WNDCLASS wndclass;
	hInst = hInstance;		// ???????�Ǥ��ä�???????????��????????�˴���

	CmdShow = nCmdShow;		// WinMain�v���������򥰥��`�Х�ˤ���
	CmdLine = lpCmdLine;	// WinMain�v���Υ��ޥ�ɥ饤�������򥰥��`�Х�ˤ���	
	
#ifndef _DEBUG
//#ifdef _DEBUG
	// ͬ�r���ӥ����å����֥�����������  ͬʱ�������������� û�������Ժ�鿴��
	hMutex = CreateMutex( NULL, TRUE, DEF_APPNAME );
	// ͬ�r�˶��������Ϥ����r  ͬʱ����������ʱ��
	if( GetLastError() == ERROR_ALREADY_EXISTS ){
		MessageBox( hWnd, "�ӣ����������Ѿ��������޷����������", "�_�J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	// ���åץǩ`�Ȥ���g�Ф����������å�  �Ӽ�����ʵ������
	if( !strstr( CmdLine, "updated" ) ){
		MessageBox( hWnd, "�ӣ���������.�������������", "�_�J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
#endif

    //*** ������ɥ����饹������ ***���ڰ������
	//���������ʵ��������ʡ�ԵĻ����ڻ�
    if( !hPrevInstance )	// �⤷���Υ��󥹥��󥹤������Фʤ���ڻ�ʡ�� 
	{
		wndclass.style = CS_BYTEALIGNCLIENT | CS_DBLCLKS;	// ���饹��������
		wndclass.lpfnWndProc = WindMsgProc;		// ������ɥ��ץ����`����`���ɥ쥹���x
		wndclass.cbClsExtra = 0;				// ���饹�����a�㶨�x�ʤ�
		wndclass.cbWndExtra = 0;				// ������ɥ������a��ǩ`���o��
		wndclass.hInstance = hInstance;			// ���󥹥��󥹥ϥ�ɥ붨�x
//		wndclass.hIcon = LoadIcon(hInstance ,"ICON1");	// ????����ICON1����`��
	    wndclass.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE(SA_ICON2));
		//wndclass.hIcon = LoadIcon( NULL, IDI_APPLICATION);

	
#ifdef _DEBUG
		wndclass.hCursor = (LoadCursor(hInstance ,MAKEINTRESOURCE(SA_MOUSE)));	// ����ʸӡ���O��SA_MOUSE
#else		
		wndclass.hCursor = LoadCursor(NULL ,IDC_ARROW);	// ???????��ʸӡ���O��  ���趨
		//wndclass.hCursor = LoadCursor(NULL ,MAKEINTRESOURCE(SA_MOUSE));	// ???????��ʸӡ���O��  ���趨
#endif
		
		//���ڵı���ɫ
		wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);// ������ɥ��α���ɫ
		wndclass.lpszMenuName= NULL;			// ��˥�`�ʤ�  û�в˵�
		wndclass.lpszClassName = DEF_APPNAME;	// �����ȥ�Щ`�����O��  �����������趨
		if( !RegisterClass(&wndclass) )
			return FALSE;						// ���ɤ�ʧ��������K�ˤ���
	}

	// �i���z�ߥե����������O��  ��ȡ�ļ������趨
	strcpy( realBinName, REALBIN_DIR );			// REAL.BIN�Έ���
	strcpy( adrnBinName, ADRNBIN_DIR );			// ADRN.BIN�Έ���
	strcpy( sprBinName, SPRBIN_DIR );			// SPR.BIN�Έ���
	strcpy( sprAdrnBinName, SPRADRNBIN_DIR );	// SPRADRN.BIN�Έ���
	
	//���ޥ�ɥ饤�����   ������
	AnalyzeCmdLine();
	
	// ����ȥ�`���O��   �ֱ���ģʽ�趨
	SetResoMode( ResoMode );
	
	// ������ɥ�������  ���ڵ�����
	ChangeWindowMode();
	// ������ɥ�������
	//ChangeWindowMode();
	
	// ȫ�ƤΥ�����ɥ����饭�`�ܩ`��������Z��  �����еĴ��ڼ����������
	//SetFocus( hWnd );

	//{
		// ALT+TAB �o���ˤ���
	//	int nOldVal;
	//	SystemParametersInfo (SPI_SCREENSAVERRUNNING, TRUE, &nOldVal, 0);
	//}
	
	// Windows�Υ�å��`����եå��ץ����`����򥻥åȤ���
	///SetWindowsHookEx( WH_SYSMSGFILTER, HookProc, hInstance, 0 );
	//SetWindowsHookEx( WH_MSGFILTER, HookProc, hInstance, 0 );
	//SetWindowsHookEx( WH_KEYBOARD, HookProc, hInstance, 0 );
	//SetWindowsHookEx( WH_CALLWNDPROC, HookProc, hInstance, 0 );
	//SetWindowsHookEx( WH_CBT, HookProc, hInstance, 0 );
	
	// ????????
	GameMain();
	
	return FALSE;
}

#ifdef _DEBUG

// �ǥХå����`���� **���Լ�����*********************************************************/
void DebugKey( char *str )
{
	char *work;
	int i = 0;
	
	// ���ޥ�ɥ饤�󥪥ץ����� key0: ������Ȥ�
	//������ѡ��key0:��ʱ��
	if( ( work = strstr( str, "KEY0:" ) ) )
	{
		work += 5;	// �����ФΤ���Ȥ����ޤǥݥ����M���
		// : ������ޤǥ�`��
		//�����еĵط�Ϊֹ�ƽ�ָ��/ /:��ѭ��
		while( *work != ':' ){
			DebugKey0[ i ] = *work++;
			i++;
			if( i == 255 ){ 
				i = 0;
				break;
			}
		}
	}
	DebugKey0[ i ] = NULL;	// �K��ӛ��
	i = 0;
	// ���ޥ�ɥ饤�󥪥ץ����� key1: ������Ȥ�
	if( ( work = strstr( str, "KEY1:" ) ) )
	{
		work += 5;	// �����ФΤ���Ȥ����ޤǥݥ����M���
		// : ������ޤǥ�`��
		while( *work != ':' ){
			DebugKey1[ i ] = *work++;
			i++;
			if( i == 255 ){ 
				i = 0;
				break;
			}
		}
	}	
	DebugKey1[ i ] = NULL;	// �K��ӛ��
	i = 0;
	// ���ޥ�ɥ饤�󥪥ץ����� key1: ������Ȥ�
	if( ( work = strstr( str, "KEY2:" ) ) )
	{
		work += 5;	// �����ФΤ���Ȥ����ޤǥݥ����M���
		// : ������ޤǥ�`��
		while( *work != ':' ){
			DebugKey2[ i ] = *work++;
			i++;
			if( i == 255 ){ 
				i = 0;
				break;
			}
		}
	}	
	DebugKey2[ i ] = NULL;	// �K��ӛ��
}

#endif
/* ���ޥ�ɥ饤����� ****�����н���*****************************************************/
void AnalyzeCmdLine( void )
{
	char *addr;

	// �i���z�ߥե��������Ή��
	if( addr = strstr( CmdLine, "realbin:" ) )
	{
		sprintf( realBinName, "data\\real_%d.bin", atoi(addr+strlen("realbin:")) );
	}
	if( addr = strstr( CmdLine, "adrnbin:" ) )
	{
		sprintf( adrnBinName, "data\\adrn_%d.bin", atoi(addr+strlen("adrnbin:")) );
	}
	if( addr = strstr( CmdLine, "sprbin:" ) )
	{
		sprintf( sprBinName, "data\\spr_%d.bin", atoi(addr+strlen("sprbin:")) );
	}
	if( addr = strstr( CmdLine, "spradrnbin:" ) )
	{
		sprintf( sprAdrnBinName, "data\\spradrn_%d.bin", atoi(addr+strlen("spradrnbin:")) );
	}

	// ���ޥ�ɥ饤�󥪥ץ����� windowmode ������Ȥ�
	/// /?۫�M????�d?�Sindowmode / / nodelay???�G? ???????????????????ȝ?0.4???
	if( strstr( CmdLine, "windowmode" ) ) WindowMode = TRUE;
	
	// nodelay???�G? ???????????????????ȝ?0.4??? ���ץ������O������ͨ�Ŵ����������뤢��ˤ���r��
	if( strstr( CmdLine, "nodelay" ) ) NoDelay = FALSE;
	
#ifdef _DEBUG
	// ���ե饤���`�ɤ��O��  �ѻ�ģʽ���趨
	if( strstr( CmdLine, "offline" ) )
	{
		offlineFlag = TRUE;
	}
	else
	{
		offlineFlag = FALSE;
	}
	if( offlineFlag )
	{
		ProcNo = PROC_GAME;	
	}
	else
	{
		ProcNo = PROC_ID_PASSWORD;
	}
	if( strstr( CmdLine, "PROC_OHTA_TEST" ) ) 
	{
		ProcNo = PROC_OHTA_TEST;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "PROC_TAKE_TEST" ) )
	{
	 	ProcNo = PROC_TAKE_TEST;
		offlineFlag = TRUE;
	 }
	if( strstr( CmdLine, "PROC_DWAF_TEST" ) )
	{
	 	ProcNo = PROC_DWAF_TEST;
		offlineFlag = TRUE;
	 }
	if( strstr( CmdLine, "PROC_BATTLE" ) )
	{
		ProcNo = PROC_BATTLE;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "sprview" ) )
	{
	 	ProcNo = PROC_SPR_VIEW;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "animview" ) )
	{
	 	ProcNo = PROC_ANIM_VIEW;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "setest" ) )
	{
	 	ProcNo = PROC_SE_TEST;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "encountoff" ) )
	{
		EncountOffFlag = TRUE;
	}
	if( strstr( CmdLine, "lowreso" ) )
	{
		ResoMode = 1;
	}
#ifdef _LOG_MSG
	if( strstr( CmdLine, "RECVDATA:" ) )
	{
		char *pt = strstr( CmdLine, "RECVDATA:" )+9;
		char *name;
		int i;

		name = debugLogFileName;
		for( i = 0; i < 255; i++ )
		{
			if( *pt != ' ' && *pt != '\0' && *pt != '\t' )
			{
				*name++ = *pt++;
			}
			else
			{
				break;
			}
		}
		*name = '\0';
	}
#endif
	//  ???�V�g???�?
	DebugKey( CmdLine );
#else
	// �ǥե���ȤΈ��ϤϤ��� Ĭ�ϵ��������
	ProcNo = PROC_ID_PASSWORD;
#endif
	// �����е�IP��ַ����  ���ޥ�ɥ饤���IP���ɥ쥹����
	ipAddressAnalyze( CmdLine );
}
/* ������ɥ���`���Ф��椨 ***************************************************/
void ChangeWindowMode( void )
{
	RECT clientRect;		// ���饤����Ȥα�ʾ�I��
	DWORD windowStyle;		// ������ɥ���������

	// ������ɥ���`�ɤΕr
	if( WindowMode ){
	
		// ������ɥ���������
		windowStyle = WS_MINIMIZEBOX | WS_SYSMENU | 
			            //�ҿ���1
						WS_THICKFRAME |
						WS_CAPTION | WS_OVERLAPPED;
							
		// ������ɥ����ɕr�Ǥʤ��ä���
		if( DDinitFlag ){
			// �ޥ������`������ʾ
			//ShowCursor( TRUE );
		}
	}
	// �ե륹����`��Εr
	else{	
	
		// ������ɥ���������
		windowStyle = WS_VISIBLE | WS_POPUP;
		// �ޥ������`������L��
		//ShowCursor( FALSE );
	}
	
	// ���饤������I����O��
	SetRect( &clientRect, 0, 0, lpDraw->xSize, lpDraw->ySize );
	// ���饤������I��Υ��������饦����ɥ���������Ӌ�㤷 clientRect ������
	//�Ӵ��ڳߴ��С�ͻ�����ļ���clientRect�Ž�
	AdjustWindowRectEx( &clientRect, windowStyle, FALSE, NULL );
	// ������ɥ������ɤ���Ȥ�
	//if( !DDinitFlag ){	// DirectDraw�����ڻ�����Ƥʤ��Ȥ�
	if( hWnd == NULL ){	// DirectDraw�����ڻ�����Ƥʤ��Ȥ�
	
		// ������ɥ�������
		hWnd = CreateWindowEx( 	NULL, 
								DEF_APPNAME, 
								DEF_APPNAME, 
								windowStyle,
								//CW_USEDEFAULT, 
								//CW_USEDEFAULT, 
								0, 
								0, 
								clientRect.right - clientRect.left, 
								clientRect.bottom - clientRect.top, 
								NULL, 
								NULL, 
								hInst, 
								NULL );
	}
	// ������ɥ���`�ɤ�������Ȥ�
	else{	
		// ������ɥ���������Ή��
		SetWindowLong( hWnd, GWL_STYLE, windowStyle );
		// ���ɤ���������ɥ����ʾ
		ShowWindow( hWnd, CmdShow );
		// ������ɥ���λ�äȥ������Ή��
		SetWindowPos( 	hWnd, 
						HWND_NOTOPMOST, 
						0, 
						0, 
						clientRect.right - clientRect.left, 
						clientRect.bottom - clientRect.top, 
						//SWP_SHOWWINDOW );
						SWP_FRAMECHANGED );
	}
	
	// ���ɤ���������ɥ����ʾ
	ShowWindow( hWnd, CmdShow );

	// ������ɥ����ʾ����
	UpdateWindow( hWnd );
}

#if 0
// Windows�Υ�å��`����եå�����
HRESULT CALLBACK HookProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	int a = 0;
	
	switch( nCode ){
	//switch( wParam ){
		case MSGF_NEXTWINDOW:
		//case HCBT_SYSCOMMAND:
			a++;
			break;
		
		case HCBT_ACTIVATE:
			a++;
			break;
	}
	return 0;
}
#endif

//---------------------------------------------------------------------------//
// ��Ҫ ����å��`����`�ׄI���v��                                                     //
// ���� ���ʤ�   ��Ϣѭ����������/ // /��������                                                            //
//---------------------------------------------------------------------------//
BOOL SystemTask( void )
{
	MSG msg;
	
	// ������ ��å��`����`�� ������
	// ��å��`���������g��`��  ��Ϣ�м�ѭ��
	while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )){
		// ��å��`�����ܤ�ȡ�롢WM_QUIT ���ܤ�ȡ���FALSE�����äƤ��ƽK��
		//if( GetMessage( &msg, NULL, 0, 0 ) == FALSE ) return FALSE; 
		// WM_QUIT ���ܤ�ȡ��ȽK��
		if( msg.message == WM_QUIT ) return FALSE;
		
		TranslateMessage(&msg);		// ��Q����Ҫ�ʥ�å��`���ω�Q����
		
		DispatchMessage(&msg);		// ������ɥ��ץ����`����إ�å��`�����ͤ�
			
	}
	return TRUE;
}

//---------------------------------------------------------------------------//
// ��Ҫ ��������ɥ���å��`���I���v��                                       //
// ���� ��HWND hWnd:     ������ɥ����R�e�ϥ�ɥ�                            //
//        UINT Message:  �I������������å��`��                              //
//        WPARAM wParam: �I������������å��`���a�����                    //
//        LPARAM lParam: �I������������å��`���a�����                    //
//---------------------------------------------------------------------------//
LRESULT CALLBACK WindMsgProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	switch( Message ){
		//case MSGF_NEXTWINDOW: // ALT+TAB Ѻ�����Ȥ�
			//SetActiveWindow( hWnd );
		//	{ 
		//		int a; 
		//		a++;
		//	}
		//	break;
		
/** �����ƥ��v�B��å��`�� ***ϵͳ�����Ϣ**************************************************/

		//���Ӧ������������߷Ǽ����˵�ʱ��  ��Ӧ��ʧȥ�����ֹͣ�ʱ
		case WM_ACTIVATE:	// ���Υ��ץꥱ�`����󤬥����ƥ����֤Ϸǥ����ƥ��֤ˤʤä��r
			
			// ������ɥ��������ƥ��֤ˤ����Ȥ� ���ڱ������ʱ����Ǳ�����Ϊ�����ʱ��
			if( ( wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE ) && lpDraw != NULL){
			
				// ���`�ե����� LOST ���Ƥ�����  ������LOST��
				if( CheckSurfaceLost() == TRUE ){
					// DirectDraw �_��
					ReleaseDirectDraw();
					// ����ȥ�`���O��
					SetResoMode( ResoMode );
					// DirectDraw ���ڻ�
					if( InitDirectDraw() == FALSE ){
						MessageBox( hWnd, "�ģ������ģ����γ��ڻ���ʧ�����ޤ�����", "�_�J", MB_OK | MB_ICONSTOP );
						// �ףɣΣģϣףӤ� WM_CLOSE ��å��`�����ͤ餻��
						PostMessage( hWnd, WM_CLOSE, 0, 0L );
					}
					// ������ɥ���`���Ф��椨
					//ChangeWindowMode();
					// ���ե�����`�󥵩`�ե���������   �����������  Ӧ���ǽ���
					InitOffScreenSurface();
					// �ѥ�åȳ��ڻ�
					if( InitPalette() == FALSE ){
						PostMessage( hWnd, WM_CLOSE, 0, 0L );
					}
					// �Хȥ�ޥå��i���z�ߤȥХȥ륵�`�ե����λ�������
					//ս����ͼ�����غ�ս���س̵Ļ�������
					if( ProcNo == PROC_BATTLE ){ 
						// �Хåե����ڻ�
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						ReadBattleMap( BattleMapNo );
						// �Хå����`�ե������\�ǥ��ꥢ�`
						ClearBackSurface();	
						// �£ͣФ�Хå����`�ե����˥��å�
						PutBmp();	
						// �Хå����`�ե�������Хȥ륵�`�ե����إ��ԩ`  ����ʵ����渴�Ƶ�ս��
						//��������һ����̨�����еľ���������ҳ���ϡ�

						lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
						// �Хåե����ڻ�
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						// �F�ڤΕr�g��ӛ��
						NowTime = GetTickCount();
					}
				}
				// �ѥ�åȥ��֥������Ȥ��ʤ��r  ���̶����ʱ��
				if( lpDraw->lpPALETTE != NULL ){
					// ������ɥ���`�ɤΕr  ���ڷ�ʽ��ʱ��
					if( WindowMode ){
						// ���ɤ��Ƥ������ѥ�åȤˉ䤨��  ������������̸ı�
						//��ɫ��
						lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
					}
				}
				// �ޥ�������å������γ��ڻ�  ���������ĳ��ڻ�
				mouse.state = MOUSE_NO_CRICK;
				mouse.onceState = MOUSE_NO_CRICK;
				// �r�g���W�����줵����  ��ʱ������������ ��õ�ǰʱ��ϵͳ����--��
				NowTime = GetTickCount();
			}
			
			//�����쥯�ȥ���ץåȤ�����
			//���`�ܩ`�ɣϣˤʤ�
			// /ֱ�����������/ /����OK�Ļ�
			if(keyboad_flg == TRUE)
			{		//���`�ܩ`�ɣϣˤʤ�
				// ���`�ܩ`�ɥ��֥������Ȥ�����Ȥ�  ���̶����ʱ��
				if(pDInputDevice != NULL){
					// ������ɥ����ǥ����ƥ��֤ˤ����Ȥ�  ���ڷǱ������ʱ��
					if(wParam == WA_INACTIVE)
					{
						// DInput ֹͣ
						DInputActiveFlag = FALSE;
						// ���`�Хåե��γ��ڻ�  ������ĳ��ڻ�  ʵ�ʴ������̶���
						for( int i = 0 ; i < 256 ; i++ ) di_key[ i ] = 0;
						pDInputDevice->Unacquire();
					}
					else
					{
						
						// DInput �ͻ�  ������  ������
						DInputActiveFlag = TRUE;
						pDInputDevice->Acquire();
					
					}
				}
			}

			//���祤���ƥ��å��ϣˤʤ�  ���Ƹ�OK�Ļ�
			if(joy_flg == TRUE){		
				// ���祤���ƥ��å����֥������Ȥ�����Ȥ�  ���Ƹ˶����ʱ��
				if(pDInputDevice2 == NULL)
					break;

				// ������ɥ����ǥ����ƥ��֤ˤ����Ȥ�  ���ڷǱ������ʱ��
				if(wParam == WA_INACTIVE){
					// DInput ֹͣ
					DInputActiveFlag = FALSE;
					pDInputDevice2->Unacquire();
				}
				else{
					// DInput �ͻ�
					DInputActiveFlag = TRUE;
					pDInputDevice2->Acquire();
				}
			}
			break;

//		case WM_ACTIVATEAPP: // ���������Υ��ץꥱ�`�������Ƥä��r
		
			// ALT+TAB�Ф��椨�ˤ�LOST���������ޏ�
			//if( ( wParam ) && lpDraw != NULL ){
			//break;
			
		case WM_CLOSE:				// ������ɥ��]����Ȥ�
		
			DestroyWindow( hWnd );	// ������ɥ����Ɨ����롢ͬ�r��
									// WM_DESTROY ��å��`�����ͤ���
			break;

		case WM_DESTROY:			// ������ɥ����Ɨ����줿�Ȥ�

			PostQuitMessage( 0 );	// WM_QUIT ��å��`�����ͤ� ( �I���νK�� )
			break;
			
#if 0			
		case WM_PALETTECHANGED:		// �ѥ�åȤ�������줿�Ȥ�
			// DirectDraw���֥������Ȥ��ʤ��r
			if( lpDraw == NULL ) break;
			// �ѥ�åȥ��֥������Ȥ��ʤ��r
			if( lpDraw->lpPALETTE == NULL ) break;
			// ������ɥ���`�ɤΕr
			//�ҿ���
			if( WindowMode ){
				// ���ɤ��Ƥ������ѥ�åȤˉ䤨��
				lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			}  //�ҿ���
			break;
			//if( (HWND)wParam == hWnd )  break;
			
		case WM_PALETTEISCHANGING:		// �ѥ�åȤ�������줿�Ȥ�
			// DirectDraw���֥������Ȥ��ʤ��r
			if( lpDraw == NULL ) break;
			// �ѥ�åȥ��֥������Ȥ��ʤ��r
			if( lpDraw->lpPALETTE == NULL ) break;
			// ������ɥ���`�ɤΕr
			//if( WindowMode ){
				// ���ɤ��Ƥ������ѥ�åȤˉ䤨��
				lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			//}
			break;
		
		case WM_QUERYNEWPALETTE:	// �ѥ�åȉ���Ǥ���Ȥ���������ɥ���`�ɤΕr�Τߣ�
		
			// DirectDraw���֥������Ȥ��ʤ��r
			if( lpDraw == NULL ) break;
			// �ѥ�åȥ��֥������Ȥ��ʤ��r
			if( lpDraw->lpPALETTE == NULL ) break;
			
			// ������ɥ���`�ɤΕr
			//if( WindowMode ){
				// ���ɤ��Ƥ������ѥ�åȤˉ䤨��
				lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			//}
			
			break;
#endif			
			
/** ���`������å��`�� *****��������Ϣ************************************************/

      	case WM_KEYDOWN:	// ���`�����I��
		
			// ���`�ե����������`�Εr
			///�������봦��ά�`/ /�س̵�ʱ��
			if( SurfaceBusyFlag == TRUE ){
				SurfaceBusyFlag = FALSE;
				//MessageBox( hWnd, "ʧȥ��ͼ�������޸�����", "�_�J", MB_OK | MB_ICONSTOP );
			
				// DirectDraw �_��
				ReleaseDirectDraw();
				// ����ȥ�`���O��
				SetResoMode( ResoMode );
				// DirectDraw ���ڻ�
				if( InitDirectDraw() == FALSE ){
					MessageBox( hWnd, "�ģ������ģ����γ��ڻ���ʧ�����ޤ�����", "�_�J", MB_OK | MB_ICONSTOP );
					// �ףɣΣģϣףӤ� WM_CLOSE ��å��`�����ͤ餻��
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// ������ɥ���`���Ф��椨
				//ChangeWindowMode();
				// ���ե�����`�󥵩`�ե��������� 
				InitOffScreenSurface();
				// �ѥ�åȳ��ڻ�
				if( InitPalette() == FALSE ){
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// �Хȥ�ޥå��i���z�ߤȥХȥ륵�`�ե����λ�������
				if( ProcNo == PROC_BATTLE ){ 
					// �Хåե����ڻ�
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					ReadBattleMap( BattleMapNo );
					// �Хå����`�ե������\�ǥ��ꥢ�`
					ClearBackSurface();	
					// �£ͣФ�Хå����`�ե����˥��å�
					PutBmp();	
					// �Хå����`�ե�������Хȥ륵�`�ե����إ��ԩ`
					lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
					// �Хåե����ڻ�
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					// �F�ڤΕr�g��ӛ��
					NowTime = GetTickCount();
				}
				// �ѥ�åȥ��֥������Ȥ��ʤ��r
				if( lpDraw->lpPALETTE != NULL ){
					// ������ɥ���`�ɤΕr
					if( WindowMode ){
						// ���ɤ��Ƥ������ѥ�åȤˉ䤨��
						lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
					}
				}
				// �ޥ�������å������γ��ڻ�
				mouse.state = MOUSE_NO_CRICK;
				mouse.onceState = MOUSE_NO_CRICK;
				// �r�g���W�����줵����
				NowTime = GetTickCount();
			}
			
            switch( wParam ){
                case VK_ESCAPE: // �ţӣå��`Ѻ���줿�Ȥ�
					// �ףɣΣģϣףӤ� WM_CLOSE ��å��`�����ͤ餻��
					//ESC�����Ƶ�ʱ���WM _ CLOSE / / WINDOWS��Ϣ�ӻ�
					//PostMessage( hWnd, WM_CLOSE, 0, 0L );
					break;
					
				case VK_RETURN:	// �꥿�`�󥭩`Ѻ���줿�r   
				
					/* �꥿�`�󥭩`�I��   ���»س���ʱ�س��� /����*/
					KeyboardReturn();
				
					break;
					
				case VK_BACK:	// BS ��һ��������  ��һ����ɾ��
				
					// �£ӥ��`�I��   / / BS/ / BS������
					KeyboardBackSpace();
				
					break;
					
				case VK_DELETE:	// ����åȥХåե����ꥢ
				
					/* ����å����֤�ȫ�ƥ��ꥢ���� */
					ClearChatBuffer();
					
					break;
					
				case VK_F5:	// ����åȥХåե����ꥢ
					
					/* ����å����֤�ȫ�ƥ��ꥢ����  �����������/��������ȫ����� */
					wParam = wParam;
					
					break;
			}
			break;
			
		case WM_SYSKEYDOWN:	// ALT���`Ѻ���줿�Ȥ�  ����ALT����ʱ��
		
			// ���`�ե����������`�Εr ʵ��ά��Ϣ��ʱ��
			if( SurfaceBusyFlag == TRUE ){
				SurfaceBusyFlag = FALSE;
				//MessageBox( hWnd, "ʧ��줿����ǩ`�����ޏͤ��ޤ���", "�_�J", MB_OK | MB_ICONSTOP );
			
				// DirectDraw �_��
				ReleaseDirectDraw();
				// ����ȥ�`���O��
				SetResoMode( ResoMode );
				// DirectDraw ���ڻ�
				if( InitDirectDraw() == FALSE ){
					MessageBox( hWnd, "�ģ������ģ����γ��ڻ���ʧ�����ޤ�����", "�_�J", MB_OK | MB_ICONSTOP );
					// �ףɣΣģϣףӤ� WM_CLOSE ��å��`�����ͤ餻��
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// ������ɥ���`���Ф��椨
				//ChangeWindowMode();
				// ���ե�����`�󥵩`�ե��������� 
				InitOffScreenSurface();
				// �ѥ�åȳ��ڻ�
				if( InitPalette() == FALSE ){
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// �Хȥ�ޥå��i���z�ߤȥХȥ륵�`�ե����λ�������
				if( ProcNo == PROC_BATTLE ){ 
					// �Хåե����ڻ�
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					ReadBattleMap( BattleMapNo );
					// �Хå����`�ե������\�ǥ��ꥢ�`
					ClearBackSurface();	
					// �£ͣФ�Хå����`�ե����˥��å�
					PutBmp();	
					// �Хå����`�ե�������Хȥ륵�`�ե����إ��ԩ`
					lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
					// �Хåե����ڻ�
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					// �F�ڤΕr�g��ӛ��
					NowTime = GetTickCount();
				}
				// �ѥ�åȥ��֥������Ȥ��ʤ��r
				if( lpDraw->lpPALETTE != NULL ){
					// ������ɥ���`�ɤΕr
					if( WindowMode ){
						// ���ɤ��Ƥ������ѥ�åȤˉ䤨��
						lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
					}
				}
				// �ޥ�������å������γ��ڻ�
				mouse.state = MOUSE_NO_CRICK;
				mouse.onceState = MOUSE_NO_CRICK;
				// �r�g���W�����줵����
				NowTime = GetTickCount();
			}
			
			switch( wParam )
			{
					//�ڴ���ģʽ���  ALT+RETURN
				//���»س���ʱ�س��� /����*/
				case VK_RETURN:	// ALT+RETURN �ǥ�����ɥ���`�ɉ��  �ڴ���ģʽ���
				
					// �ݳ��Фϒi����  �ݳ��д���
					if( BackBufferDrawType == DRAW_BACK_PRODUCE ) break; 
					
					// DirectDraw���֥������Ȥ��ʤ��r  �����ʱ��
					if( lpDraw == NULL ) break;
					
					// ������ɥ���`�ɥե饰���
					if( WindowMode == TRUE ) WindowMode = FALSE;
					else WindowMode = TRUE;
					
					// DirectDraw �_��
					ReleaseDirectDraw();

					// ����ȥ�`���O��
					SetResoMode( ResoMode );

					// DirectDraw ���ڻ�
					InitDirectDraw();
					// ���ե�����`�󥵩`�ե��������� 
					InitOffScreenSurface();
					// ������ɥ���`���Ф��椨
					ChangeWindowMode();
					// ȫ������ɥ������軭Ҫ��
					//SendMessage( HWND_BROADCAST, WM_PAINT, NULL, NULL );
		
					// �ѥ�åȳ��ڻ�
					if( InitPalette() == FALSE ){
						PostMessage( hWnd, WM_CLOSE, 0, 0L );
					}
					// �Хȥ�ޥå��i���z�ߤȥХȥ륵�`�ե����λ�������
					if( ProcNo == PROC_BATTLE ){ 
						// �Хåե����ڻ�
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						ReadBattleMap( BattleMapNo );
						// �Хå����`�ե������\�ǥ��ꥢ�`
						ClearBackSurface();	
						// �£ͣФ�Хå����`�ե����˥��å�
						PutBmp();	
						// �Хå����`�ե�������Хȥ륵�`�ե����إ��ԩ`
						lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
						// �Хåե����ڻ�
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						// �F�ڤΕr�g��ӛ��
						NowTime = GetTickCount();
					}	
					
					break;
				default:
					// δ�I����å��`����ǥե���ȄI���v���ضɤ��ƄI��������
					//δ��������Ϣ��������Ĭ�Ͻ�������
					return DefWindowProc( hWnd, Message, wParam, lParam );
			}
			break;
			
      	case WM_CHAR:	// ���������I��  �������봦��
		
				// ���֥Хåե�������  ���ֻ�����

			// StockChatBufferWord( WORD wParam );

				StockStrBufferChar( ( char )wParam );
			
			break;
			
/** �ޥ���������å��`�� *********���������Ϣ********************************************/

		case WM_MOUSEMOVE:		// �ޥ�����Ӥ������r  �ƶ�����ʱ��
		
			// �F�ڤΥޥ�����λ�ä�ӛ������
			MouseNowPoint( LOWORD(lParam), HIWORD(lParam) );
			if( mouse.flag == TRUE ){ 
				ShowCursor( FALSE ); // �ޥ������`���������
				mouse.flag = FALSE;
			}
			// ���`�ե����������`�Εr
			if( SurfaceBusyFlag == TRUE ){
				SurfaceBusyFlag = FALSE;
				//MessageBox( hWnd, "ʧ��줿����ǩ`�����ޏͤ��ޤ���", "�_�J", MB_OK | MB_ICONSTOP );
			
				// DirectDraw �_��
				ReleaseDirectDraw();
				// ����ȥ�`���O��
				SetResoMode( ResoMode );
				// DirectDraw ���ڻ�
				if( InitDirectDraw() == FALSE ){
					MessageBox( hWnd, "�ģ������ģ����γ��ڻ���ʧ�����ޤ�����", "�_�J", MB_OK | MB_ICONSTOP );
					// �ףɣΣģϣףӤ� WM_CLOSE ��å��`�����ͤ餻��
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// ������ɥ���`���Ф��椨
				//ChangeWindowMode();
				// ���ե�����`�󥵩`�ե��������� 
				InitOffScreenSurface();
				// �ѥ�åȳ��ڻ�
				if( InitPalette() == FALSE ){
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// �Хȥ�ޥå��i���z�ߤȥХȥ륵�`�ե����λ�������
				if( ProcNo == PROC_BATTLE ){ 
					// �Хåե����ڻ�
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					ReadBattleMap( BattleMapNo );
					// �Хå����`�ե������\�ǥ��ꥢ�`
					ClearBackSurface();	
					// �£ͣФ�Хå����`�ե����˥��å�
					PutBmp();	
					// �Хå����`�ե�������Хȥ륵�`�ե����إ��ԩ`
					lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
					// �Хåե����ڻ�
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					// �F�ڤΕr�g��ӛ��
					NowTime = GetTickCount();
				}
				// �ѥ�åȥ��֥������Ȥ��ʤ��r
				if( lpDraw->lpPALETTE != NULL ){
					// ������ɥ���`�ɤΕr
					if( WindowMode ){
						// ���ɤ��Ƥ������ѥ�åȤˉ䤨��
						lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
					}
				}
				// �ޥ�������å������γ��ڻ�
				mouse.state = MOUSE_NO_CRICK;
				mouse.onceState = MOUSE_NO_CRICK;
				// �r�g���W�����줵����
				NowTime = GetTickCount();
			}

			break;
			
		case WM_NCMOUSEMOVE:	// �ޥ�����������ɥ�����Ϥ߳����r  ���Ӵ���¶���˵�ʱ��
		
			// �ޥ�������å������γ��ڻ�
			mouse.state = MOUSE_NO_CRICK;
			mouse.onceState = MOUSE_NO_CRICK;
			if( mouse.flag == FALSE ){ 
				ShowCursor( TRUE ); // �ޥ������`������ʾ
				mouse.flag = TRUE;
			}
			
			break;
			
		case WM_LBUTTONDOWN:	// �󥯥�å�(Ѻ�����r)
		
			// �󥯥�å����줿�r�Υޥ�����λ�ä�ӛ������
			MouseCrickLeftDownPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;
			
		case WM_LBUTTONUP:		// �󥯥�å�(�x�����r)
		
			// �󥯥�å����줿�r�Υޥ�����λ�ä�ӛ������
			MouseCrickLeftUpPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;

		case WM_LBUTTONDBLCLK:		// ����֥륯��å�
		
			// ����֥륯��å����줿�r�Υޥ�����λ�ä�ӛ������
			MouseDblCrickLeftUpPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;
			
		case WM_RBUTTONDOWN:	// �ҥ���å�(Ѻ�����r)
		
			// �󥯥�å����줿�r�Υޥ�����λ�ä�ӛ������
			MouseCrickRightDownPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;
			
		case WM_RBUTTONUP:		// �ҥ���å�(�x�����r)
		
			// �󥯥�å����줿�r�Υޥ�����λ�ä�ӛ������
			MouseCrickRightUpPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;

		case WM_RBUTTONDBLCLK:		// �ҥ��֥륯��å�
		
			// �ҥ��֥륯��å����줿�r�Υޥ�����λ�ä�ӛ������
			MouseDblCrickRightUpPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;
			
		case WM_MBUTTONDOWN:	// �Х���å�(Ѻ�����r)
			break;

/** �ɣͣ��v�B��å��`���I�� *******�й���Ϣ����*******************************************/
			
			//��С���ڱ仯��ʱ��
		case WM_IME_COMPOSITION: // �ɣͣŤΤ��ä��㤤������ɥ��ˉ仯������r
	
			// WM_IME_COMPOSITION ��å��`���΄I��
			ImeComposition( lParam );
			
			break;

			//������ȷ���󡣱任�е����ֶ��ڵ�ʱ��Ҳ������BS��/ / IME�����ڿ��е�
		case WM_IME_ENDCOMPOSITION: // ���֤�_������ֱ������롣��Q�Ф����֤�ȫ��BS�Ǥ������Ȥ��⤯�롣
			// �ɣͣťХåե��`��դˤ���
			*ImeBuffer = '\0';
			break;
		
		case WM_IME_NOTIFY:
			/* WM_IME_NOTIFY ��å��`���΄I��   ��Ϣ����*/
			//ImeNotify( wParam ,lParam );
			// ������DEFWINDOWPROC������ȡ���Q���a������ɥ�������褦����
			// �ɣͣ�������`�ɤ�ȡ��
			//������DEFWINDOWPROC���任�򲹴��ڷ�����/ / IME���뷽ʽ��ȡ��
			ImmGetConversionStatus(  hImc, &conversion, &sentence  );
			break;
			
		case WM_IME_CHAR:
			// ������DefWindowProc������ȡ�WM_CHAR�����ɤ���
			//������DefWindowProc��WM _ CHAR����
			break;
#if 0
		//case WM_IME_SETCONTEXT:
			//���ڻ������Ȥ����Ƥ�Τ��ʣ��ե��`�������Ƥä��Ȥ�������褦����
			//DefWindowProc( hWnd, Message, wParam, lParam );
		//	break;
		case WM_IME_STARTCOMPOSITION:// IME��ON��״�B�ǡ����֤���������ֱ������롣
		//	DefWindowProc( hWnd, Message, wParam, lParam );
			break;
		case WM_IME_CONTROL:
			break;
		case WM_IME_SELECT: // IME�򤨤����Ȥ�����餷�����Хåե�����ڻ��Ȥ�����Ф����Τ��ʣ�
			break;
		case WM_IME_KEYDOWN:   // IME�Υ��`���`��
			// ������DefWindowProc������ȡ�WM_KEYDOWN�����ɤ���
			break;
#endif
	
		default:
			// δ�I����å��`����ǥե���ȄI���v���ضɤ��ƄI��������
			return DefWindowProc( hWnd, Message, wParam, lParam );
	}

	return 0;
}

/** ����ȥ�`���O���I�� *****�ֱ���ģʽ�趨����*********************************************/
void SetResoMode(int Mode){
	ResoMode = Mode;
	// DirectDraw���혋����g��_��   ��ʼ��DIRECT_DRAW����ָ��
	lpDraw = ( DIRECT_DRAW *)GlobalAllocPtr( GPTR, ( DWORD )sizeof( DIRECT_DRAW ) );
	if( lpDraw == NULL ){
		MessageBox( hWnd, "GlobalAllocPtr Error ( DIRECT_DRAW )", "�_�J", MB_OK | MB_ICONSTOP );
		return;
	}

	switch(Mode){
	case 0:
		//��������������  ��?640 480
		lpDraw->xSize = 640;//windows  ���ڵĴ�С ��Direct û��ϵ
		lpDraw->ySize = 480;
		SurfaceSizeX = 64;
		SurfaceSizeY = 48;
		break;
	case 1:
		//��������������
		lpDraw->xSize = 320;
		lpDraw->ySize = 240;
		SurfaceSizeX = 32;
		SurfaceSizeY = 24;
		break;
	case 2:
		//�ɉ�
		lpDraw->xSize = 640;
		lpDraw->ySize = 480;
		SurfaceSizeX = 64;
		SurfaceSizeY = 48;
		break;
	}
}
