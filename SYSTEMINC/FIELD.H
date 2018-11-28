#ifndef _FIELD_H_ 
#define _FIELD_H_

#include<time.h>

// ßBÉä¤·¤¿•r¤ËÊÜ¤±¸¶¤±¤ëég¸ô
#define FIELD_BTN_PUSH_WAIT			500		// 0.5Ãë

// ¥á©`¥ë¥é¥ó¥×¤Îµãœçég¸ô
#define FIELD_MAIL_LAMP_FLASH_TIME	1000	// 1Ãëég¸ô

enum
{
	FIELD_FUNC_MENU,
	FIELD_FUNC_CARD,
	FIELD_FUNC_PARTY,
	FIELD_FUNC_JOIN_BATTLE,
	FIELD_FUNC_DUEL,
	FIELD_FUNC_ACT,
	FIELD_FUNC_END
};

extern short etcSendFlag;

extern short actBtn;
extern short menuBtn;
extern short mailLamp;

extern unsigned int fieldInfoTime;

extern struct tm *serverAliveTime;
extern time_t serverAliveLongTime;


void initFieldProc( void );
void resetFieldProc( void );
void fieldProc( void );
void drawField( void );

BOOL checkFieldMenuFlag( void );

void closeCharActionAnimeChange( void );
void closeEtcSwitch( void );

int disconnectServer( void );

void drawFieldInfoWin( void );

void actionShortCutKeyProc( void );

#endif

