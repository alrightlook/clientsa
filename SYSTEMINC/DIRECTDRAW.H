// directDraw.cpp ͷ�ļ�

#ifndef _DIRECT_DRAW_H_
#define _DIRECT_DRAW_H_

// ����ɫ������
#define MAX_PAL	15
// Ĭ�ϵ�ɫ��
#define DEF_PAL	0	//����һ��

// ���̽���ʱ�䣨��һ��������ɣ�
#define PAL_CHANGE_TIME			3600

// ����ϵͳ
#define SYSTEM_PAL_WHITE 		255//��ɫ�����
#define SYSTEM_PAL_AQUA 		254
#define SYSTEM_PAL_PURPLE 		253
#define SYSTEM_PAL_BLUE 		252
#define SYSTEM_PAL_YELLOW 		251
#define SYSTEM_PAL_GREEN 		250
#define SYSTEM_PAL_RED 			249
#define SYSTEM_PAL_GRAY 		248
#define SYSTEM_PAL_BLUE3 		247
#define SYSTEM_PAL_GREEN3 		246//��ɫ

#define SYSTEM_PAL_WHITE2 		7
#define SYSTEM_PAL_AQUA2 		6
#define SYSTEM_PAL_PURPLE2 		5
#define SYSTEM_PAL_BLUE2 		4
#define SYSTEM_PAL_YELLOW2 		3
#define SYSTEM_PAL_GREEN2 		2
#define SYSTEM_PAL_RED2 		1
#define SYSTEM_PAL_BLACK 		0//��
#define SYSTEM_PAL_BLUE4 		8
#define SYSTEM_PAL_GREEN4 		9

// �����ɫ��
#define FONT_PAL_WHITE 		0
#define FONT_PAL_AQUA 		1
#define FONT_PAL_PURPLE 	2
#define FONT_PAL_BLUE 		3
#define FONT_PAL_YELLOW 	4
#define FONT_PAL_GREEN 		5
#define FONT_PAL_RED 		6
#define FONT_PAL_GRAY 		7
#define FONT_PAL_BLUE2 		8
#define FONT_PAL_GREEN2 	9


// ��λ��ƺ�̨������  ������
enum{
	DRAW_BACK_NORMAL,	//����
	DRAW_BACK_NON,		// ����
	DRAW_BACK_PRODUCE,	// ����
	DRAW_BACK_BATTLE,	// ս��
};

//---------------------------------------------------------------------------//
// Direct Draw�Ǘ��p�\���̌^�錾         �����Ø�����������                                      //
//---------------------------------------------------------------------------//
typedef struct
{
	LPDIRECTDRAW			lpDD;				// DirectDraw����
	LPDIRECTDRAW2			lpDD2;				// DirectDraw2  �ܲ����޸ĳɸ߼������޸�ԭʼ��2
	LPDIRECTDRAWSURFACE		lpFRONTBUFFER;		// ǰ������
	LPDIRECTDRAWSURFACE		lpBACKBUFFER;		// ��
	LPDIRECTDRAWCLIPPER		lpCLIPPER;			// ����
	DDSURFACEDESC			ddsd;				// �������¶���ı���ṹ��  ԭ����û2 ���޸�
	LPDIRECTDRAWPALETTE		lpPALETTE;			// ��ɫ��
	int						xSize, ySize;		// �ߴ�
} DIRECT_DRAW;

// �p���b�g�\����
extern PALETTEENTRY			Palette[256];

// �p���b�g��ԍ\����
typedef struct{
	int palNo;		// ��ɫ�����ID
	int time;		// ���������� ���ڻ��r�g
	int flag;		// �������t���O ���ڻ�ת�۵� ��û��ʼ����־��
}PALETTE_STATE; //��ɫ��ṹ

// ȫ�ֱ��� /////////////////////////////////////////////////////////////
extern DIRECT_DRAW	*lpDraw;
// DirectDraw  ��ʼ����־
extern BOOL DDinitFlag;	
// ���崦��
extern HFONT hFont;
// ��ɫ��Ľṹ��
extern PALETTE_STATE PalState;	
// ���̸ı��־
extern BOOL	PalChangeFlag;	
// ��λ��ƺ�̨������
extern int BackBufferDrawType;

//---------------------------------------------------------------------------//
// �֐��v���g�^�C�v�錾                                                      //
//---------------------------------------------------------------------------//
// DirectDraw��ʼ��
BOOL InitDirectDraw( void );

// ��ɫ���ʼ��
BOOL InitPalette( void );

// �o�b�N�T�[�t�F�X�����ŃN���A�[
void ClearBackSurface( void );

// �T�[�t�F�X�����ŃN���A�[
void ClearSurface( LPDIRECTDRAWSURFACE lpSurface );

// ����ͼƬ
LPBITMAPINFO LoadDirectDrawBitmap( char *pFile );

// �����ӿ�
LPDIRECTDRAWSURFACE CreateSurface( short sizeX, short sizeY, DWORD ColorKey, unsigned int VramOrSysram );

// ���������ı���  ����
HRESULT DrawSurfaceFast( short bx, short by, LPDIRECTDRAWSURFACE lpSurface );
// �o�b�N�T�[�t�F�X�֍����]��
HRESULT DrawSurfaceFast2( short bx, short by, RECT *rect, LPDIRECTDRAWSURFACE lpSurface );

// �o�b�N�T�[�t�F�X�֓]��
//HRESULT DrawSurface( short bx, short by, RECT *rect, LPDIRECTDRAWSURFACE lpSurface );

// ���ݺ����ı����BMP�Ĵ���  StretchDIBits ʹ��
void DrawBitmapToSurface( LPDIRECTDRAWSURFACE lpSurface, int offsetX, int offsetY, LPBITMAPINFO pInfo );
// �ڴ�Ĵ��ݵ��ӿ� memcpy ���g�p �j
void DrawBitmapToSurface2( LPDIRECTDRAWSURFACE lpSurface, int offsetX, int offsetY, int sizeX, int sizeY, LPBITMAPINFO pBmpInfo );

// �T�[�t�F�XFLIP�֐�//����flip����
void Flip( void );

// �������뿪
BOOL CheckSurfaceLost( void );

// DirectDraw�I�u�W�F�N�g���J������
void ReleaseDirectDraw( void );

// ���ƹ����в�ȡ��ʱ�䣨���ԣ�
void DrawDebugLine( unsigned char color );

// ����һ���� */
void DrawBox( RECT *rect, unsigned char color, BOOL fill );

// �t�H���g�I�u�W�F�N�g�̍쐬 */
void InitFont( int fontNo );

// �ı���ͼ
void PutText( char fontPrio ); 

// ��ɫ�� ����  ���̴��� ***************************************************************/
void PaletteProc( void );

// �p���b�g�`�F���W ***********************************************************/
void PaletteChange( int palNo, int time );

// �I�[�g�}�b�s���O�\��
void DrawAutoMapping( int x, int y, unsigned char *autoMap, int w, int h );
// �摜�̕��ς�����ăI�[�g�}�b�v�̐F�����
int getAutoMapColor( unsigned int GraphicNo );
// �F��entry�̃p���b�gpalette�̒�����Acolor�Ɉ�ԋ߂����̂�index��Ԃ��B
//�ӵ�ɫ���е���ɫ��ɫ����Ŀ��������������ӽ�����ɫ������
int getNearestColorIndex( COLORREF color, PALETTEENTRY *palette, int entry );

// �������
void snapShot( void );
// �摜���a�l�o�t�@�C���ɕۑ� ����ΪBMPͼ���ļ�
BOOL saveBmpFile( const char *, BYTE *,	int, int, int, int, int, RGBQUAD *, int );



void DrawMapEffect( void );


#endif