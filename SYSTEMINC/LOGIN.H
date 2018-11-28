#ifndef _LOGIN_H_
#define _LOGIN_H_


enum
{
	WINDOW_MESSAGETYPE_MESSAGE,					// 只有消息  儊僢僙乕僕偺傒
	WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,		// 消息      儊僢僙乕僕偲堦峴擖椡
	WINDOW_MESSAGETYPE_SELECT,					// 选择器    慖戰僂傿儞僪僂
	WINDOW_MESSAGETYPE_PETSELECT,				// 宠物       儁僢僩慖戰僂傿儞僪僂
	WINDOW_MESSAGETYPE_PARTYSELECT,				// 家族？？   拠娫慖戰僂傿儞僪僂
	WINDOW_MESSAGETYPE_PETANDPARTYSELECT,		// 家族宠     儁僢僩丆拠娫慖戰僂傿儞僪僂
	WINDOW_MESSAGETYPE_ITEMSHOPMENU,			// 菜单 物品商店   偍揦偺儊僯儏乕僂僀儞僪僂
	WINDOW_MWSSAGETYPE_ITEMSHOPMAIN,			// 这个店  当前商店  偍揦偺儊僀儞僂僀儞僪僂
	WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN,		// 商店橱窗中的专业项目的主要购买主窗口 攦偄庢傝愱栧傾僀僥儉壆偺儊僀儞僂僀儞僪僂
	WINDOW_MESSAGETYPE_PETSKILLSHOP,			// 窗口店宠物技能  儁僢僩偺媄壆偝傫僂僀儞僪僂
	WINDOW_MESSAGETYPE_WIDEMESSAGE,				// /只有信息窗口（大）/  儊僢僙乕僕偺傒乮戝偒偄曽乯
	WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT	// 线路输入和信息（大） 儊僢僙乕僕偲堦峴擖椡乮戝偒偄曽乯
};

#define	WINDOW_BUTTONTYPE_NONE		(0)
#define	WINDOW_BUTTONTYPE_OK		(1 << 0)
#define	WINDOW_BUTTONTYPE_CANCEL	(1 << 1)
#define	WINDOW_BUTTONTYPE_YES		(1 << 2)
#define	WINDOW_BUTTONTYPE_NO		(1 << 3)
#define	WINDOW_BUTTONTYPE_PRE		(1 << 4)
#define	WINDOW_BUTTONTYPE_NEXT		(1 << 5)

extern char selectServerName[][64];
extern char selectServerName2[][64];

extern BOOL logOutFlag;

extern int windowTypeWN;
extern short wnCloseFlag;


void idPasswordProc( void );

void titleProc( void );
void selectCharacterProc( void );
void characterLoginProc( void );

void makeCharacterProc( void );

void characterLogoutProc( void );


void initOpenServerWindow( void );
void openServerWindow( int, int, int, int, char * );
void openServerWindowProc( void );

int selGraId( int *, int );
int selRepGraId( int *, int );
int pushGraId( int *, int );
int selFontId( int *, int );
int focusGraId( int *, int );
int focusFontId( int *, int );

#endif
