/************************/
/*	mouse.h				*/
/************************/
#ifndef _MOUSE_H_
#define _MOUSE_H_

//#include "menu.h"

// ¥Þ¥¦¥¹¤Î×´‘B

#define	MOUSE_NO_CRICK						0		// Ñº¤µ¤ì¤Æ¤¤¤Ê¤¤
#define	MOUSE_LEFT_CRICK			( 1 << 0 )		// ×ó¥¯¥ê¥Ã¥¯×´‘B
#define	MOUSE_RIGHT_CRICK			( 1 << 1 )		// ÓÒ¥¯¥ê¥Ã¥¯×´‘B
#define	MOUSE_LEFT_CRICK_UP			( 1 << 2 )		// ×ó¥¯¥ê¥Ã¥¯ëx¤·¤¿×´‘B
#define	MOUSE_RIGHT_CRICK_UP		( 1 << 3 )		// ÓÒ¥¯¥ê¥Ã¥¯ëx¤·¤¿×´‘B
#define	MOUSE_LEFT_DBL_CRICK		( 1 << 4 )		// ×ó¥À¥Ö¥ë¥¯¥ê¥Ã¥¯×´‘B
#define	MOUSE_RIGHT_DBL_CRICK		( 1 << 5 )		// ÓÒ¥À¥Ö¥ë¥¯¥ê¥Ã¥¯×´‘B
//#define	MOUSE_LEFT_AUTO_CRICK		( 1 << 6 )		// ×ó¥ª©`¥È¥¯¥ê¥Ã¥¯×´‘B
//#define	MOUSE_RIGHT_AUTO_CRICK		( 1 << 7 )		// ÓÒ¥ª©`¥È¥¯¥ê¥Ã¥¯×´‘B

// ¥Þ¥¦¥¹¤Î×ù˜Ë¡¡˜‹ÔìÌå
typedef struct{
	int x, y;	// ¥Þ¥¦¥¹¤Î( £Ø,£Ù )×ù˜Ë
}MOUSE_POINT;

// ¥Þ¥¦¥¹˜‹ÔìÌå
typedef struct{
	MOUSE_POINT nowPoint;			// ¬FÔÚ¤Î¥Þ¥¦¥¹¤Î×ù˜Ë
	MOUSE_POINT crickLeftDownPoint;	// ×îáá¤Ë×ó¥¯¥ê¥Ã¥¯¤·¤¿¥Þ¥¦¥¹¤Î×ù˜Ë
	MOUSE_POINT crickLeftUpPoint;	// ×îáá¤Ë×ó¥¯¥ê¥Ã¥¯¤òëx¤·¤¿¥Þ¥¦¥¹¤Î×ù˜Ë
	MOUSE_POINT crickRightDownPoint;// ×îáá¤ËÓÒ¥¯¥ê¥Ã¥¯¤·¤¿¥Þ¥¦¥¹¤Î×ù˜Ë
	MOUSE_POINT crickRightUpPoint;	// ×îáá¤ËÓÒ¥¯¥ê¥Ã¥¯¤òëx¤·¤¿¥Þ¥¦¥¹¤Î×ù˜Ë
	int			state;				// ¥Þ¥¦¥¹¥Ü¥¿¥ó¤Î×´‘B¥Ó¥Ã¥È¥Õ¥£©`¥ë¥É
	int			onceState;			// ¥Þ¥¦¥¹¥Ü¥¿¥ó¤Î×´‘B¡£¤¿¤À¤·¡¢Ñº¤µ¤ì¤¿¥Õ¥ì©`¥à¤Ë¤À¤±‚Ž¤¬Èë¤ë
	int			autoState;			// ¥Þ¥¦¥¹¥Ü¥¿¥ó¤Î×´‘B¥ª©`¥È¥ê¥Ô©`¥È
	UINT 		beforeLeftPushTime;	// ×ó¥¯¥ê¥Ã¥¯¤·¤¿•r¤Î•rég¤òÓ›‘›
	UINT 		leftPushTime;		// ×ó¥¯¥ê¥Ã¥¯¤¬Ñº¤µ¤ì¤Æ¤¤¤ë•rég
	UINT 		beforeRightPushTime;// ÓÒ¥¯¥ê¥Ã¥¯¤·¤¿•r¤Î•rég¤òÓ›‘›
	UINT 		rightPushTime;		// ÓÒ¥¯¥ê¥Ã¥¯¤¬Ñº¤µ¤ì¤Æ¤¤¤ë•rég
	UCHAR		level;				// ¬FÔÚ¤Î¥ì¥Ù¥ë£¨£°¡«£²£µ£µ£©
	int 		itemNo;				// ¬FÔÚ¥É¥é¥Ã¥°ÖÐ¤Î¥¢¥¤¥Æ¥à¤Î·¬ºÅ
	BOOL flag;						// ¥Þ¥¦¥¹¥«©`¥½¥ë¤Î±íÊ¾£¯·Ç±íÊ¾¥Õ¥é¥°
}MOUSE;

// ¥°¥í©`¥Ð¥ë¡¡¥Þ¥¦¥¹
extern MOUSE mouse;

// ¥Þ¥¦¥¹¥«©`¥½¥ë¤Ëµ±¤¿¤Ã¤¿¥Õ¥©¥ó¥È·¬ºÅ£¨¥Õ¥©¥ó¥ÈÓÃ£©
extern int HitFontNo;
// ¥Þ¥¦¥¹¥«©`¥½¥ë¤Ëµ±¤¿¤Ã¤¿¥¢¥¯¥·¥ç¥ó¥Ý¥¤¥ó¥¿Ó›‘›£¨£Â£Í£ÐÓÃ£©
extern int HitDispNo;

// Ò»ÐÐ¥¤¥ó¥Õ¥©ÎÄ×ÖÁÐ
extern char OneLineInfoStr[];

// ±íÊ¾¥Ü¥Ã¥¯¥¹¤ÎÉ«
extern int BoxColor;

// ¥Þ¥¦¥¹Çéˆó¤Î³õÆÚ»¯ ////////////////////////////////////////////////////////
void MouseInit( void );

// ¬FÔÚ¤Î¥Þ¥¦¥¹¤ÎÎ»ÖÃ¤òÓ›‘›¤¹¤ë //////////////////////////////////////////////
void MouseNowPoint( int x, int y );

// ¥Þ¥¦¥¹¤Î×ó¥Ü¥¿¥ó¤¬Ñº¤µ¤ì¤¿•r¤Î¥Þ¥¦¥¹¤ÎÎ»ÖÃ¤òÓ›‘›¤¹¤ë //////////////////////
void MouseCrickLeftDownPoint( int x, int y );

// ¥Þ¥¦¥¹¤ÎÓÒ¥Ü¥¿¥ó¤¬ëx¤µ¤ì¤¿•r¤Î¥Þ¥¦¥¹¤ÎÎ»ÖÃ¤òÓ›‘›¤¹¤ë //////////////////////
void MouseCrickLeftUpPoint( int x, int y );

// ¥Þ¥¦¥¹¤Î×ó¥Ü¥¿¥ó¤¬Ñº¤µ¤ì¤¿•r¤Î¥Þ¥¦¥¹¤ÎÎ»ÖÃ¤òÓ›‘›¤¹¤ë //////////////////////
void MouseCrickRightDownPoint( int x, int y );

// ¥Þ¥¦¥¹¤ÎÓÒ¥Ü¥¿¥ó¤¬ëx¤µ¤ì¤¿•r¤Î¥Þ¥¦¥¹¤ÎÎ»ÖÃ¤òÓ›‘›¤¹¤ë //////////////////////
void MouseCrickRightUpPoint( int x, int y );

// ×ó¥À¥Ö¥ë¥¯¥ê¥Ã¥¯¤µ¤ì¤¿•r¤Î¥Þ¥¦¥¹¤ÎÎ»ÖÃ¤òÓ›‘›¤¹¤ë //////////////////////////
void MouseDblCrickLeftUpPoint( int x, int y );

// ÓÒ¥À¥Ö¥ë¥¯¥ê¥Ã¥¯¤µ¤ì¤¿•r¤Î¥Þ¥¦¥¹¤ÎÎ»ÖÃ¤òÓ›‘›¤¹¤ë //////////////////////////
void MouseDblCrickRightUpPoint( int x, int y );

// ¥Þ¥¦¥¹„IÀí¡¡///////////////////////////////////////////////////////////////
void MouseProc( void );

/* ¥Þ¥¦¥¹¥«©`¥½¥ë¤Î¤¢¤¿¤êÅÐ¶¨ **************************************************/
void HitMouseCursor( void );

#endif

