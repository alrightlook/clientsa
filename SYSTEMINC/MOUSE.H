/************************/
/*	mouse.h				*/
/************************/
#ifndef _MOUSE_H_
#define _MOUSE_H_

//#include "menu.h"

// �ޥ�����״�B

#define	MOUSE_NO_CRICK						0		// Ѻ����Ƥ��ʤ�
#define	MOUSE_LEFT_CRICK			( 1 << 0 )		// �󥯥�å�״�B
#define	MOUSE_RIGHT_CRICK			( 1 << 1 )		// �ҥ���å�״�B
#define	MOUSE_LEFT_CRICK_UP			( 1 << 2 )		// �󥯥�å��x����״�B
#define	MOUSE_RIGHT_CRICK_UP		( 1 << 3 )		// �ҥ���å��x����״�B
#define	MOUSE_LEFT_DBL_CRICK		( 1 << 4 )		// ����֥륯��å�״�B
#define	MOUSE_RIGHT_DBL_CRICK		( 1 << 5 )		// �ҥ��֥륯��å�״�B
//#define	MOUSE_LEFT_AUTO_CRICK		( 1 << 6 )		// �󥪩`�ȥ���å�״�B
//#define	MOUSE_RIGHT_AUTO_CRICK		( 1 << 7 )		// �ҥ��`�ȥ���å�״�B

// �ޥ��������ˡ�������
typedef struct{
	int x, y;	// �ޥ�����( ��,�� )����
}MOUSE_POINT;

// �ޥ���������
typedef struct{
	MOUSE_POINT nowPoint;			// �F�ڤΥޥ���������
	MOUSE_POINT crickLeftDownPoint;	// ������󥯥�å������ޥ���������
	MOUSE_POINT crickLeftUpPoint;	// ������󥯥�å����x�����ޥ���������
	MOUSE_POINT crickRightDownPoint;// ������ҥ���å������ޥ���������
	MOUSE_POINT crickRightUpPoint;	// ������ҥ���å����x�����ޥ���������
	int			state;				// �ޥ����ܥ����״�B�ӥåȥե��`���
	int			onceState;			// �ޥ����ܥ����״�B����������Ѻ���줿�ե�`��ˤ����������
	int			autoState;			// �ޥ����ܥ����״�B���`�ȥ�ԩ`��
	UINT 		beforeLeftPushTime;	// �󥯥�å������r�Εr�g��ӛ��
	UINT 		leftPushTime;		// �󥯥�å���Ѻ����Ƥ���r�g
	UINT 		beforeRightPushTime;// �ҥ���å������r�Εr�g��ӛ��
	UINT 		rightPushTime;		// �ҥ���å���Ѻ����Ƥ���r�g
	UCHAR		level;				// �F�ڤΥ�٥루������������
	int 		itemNo;				// �F�ڥɥ�å��ФΥ����ƥ�η���
	BOOL flag;						// �ޥ������`����α�ʾ���Ǳ�ʾ�ե饰
}MOUSE;

// �����`�Х롡�ޥ���
extern MOUSE mouse;

// �ޥ������`����˵����ä��ե���ȷ��ţ��ե�����ã�
extern int HitFontNo;
// �ޥ������`����˵����ä����������ݥ���ӛ�����£ͣ��ã�
extern int HitDispNo;

// һ�Х���ե�������
extern char OneLineInfoStr[];

// ��ʾ�ܥå�����ɫ
extern int BoxColor;

// �ޥ������γ��ڻ� ////////////////////////////////////////////////////////
void MouseInit( void );

// �F�ڤΥޥ�����λ�ä�ӛ������ //////////////////////////////////////////////
void MouseNowPoint( int x, int y );

// �ޥ�������ܥ���Ѻ���줿�r�Υޥ�����λ�ä�ӛ������ //////////////////////
void MouseCrickLeftDownPoint( int x, int y );

// �ޥ������ҥܥ����x���줿�r�Υޥ�����λ�ä�ӛ������ //////////////////////
void MouseCrickLeftUpPoint( int x, int y );

// �ޥ�������ܥ���Ѻ���줿�r�Υޥ�����λ�ä�ӛ������ //////////////////////
void MouseCrickRightDownPoint( int x, int y );

// �ޥ������ҥܥ����x���줿�r�Υޥ�����λ�ä�ӛ������ //////////////////////
void MouseCrickRightUpPoint( int x, int y );

// ����֥륯��å����줿�r�Υޥ�����λ�ä�ӛ������ //////////////////////////
void MouseDblCrickLeftUpPoint( int x, int y );

// �ҥ��֥륯��å����줿�r�Υޥ�����λ�ä�ӛ������ //////////////////////////
void MouseDblCrickRightUpPoint( int x, int y );

// �ޥ����I����///////////////////////////////////////////////////////////////
void MouseProc( void );

/* �ޥ������`����Τ������ж� **************************************************/
void HitMouseCursor( void );

#endif

