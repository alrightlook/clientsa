#ifndef _NETPROC_H_
#define _NETPROC_H_


extern short selectServerIndex;
extern char userId[];
extern char userPassword[];
extern short clientLoginStatus;
extern short charListStatus;
extern short charDelStatus;


extern char gamestate_chooseserver_name[];
extern char gamestate_login_charname[];
extern char gamestate_deletechar_charname[];

extern char netprocErrmsg[];

extern int connectServer2Counter;

void initConnectServer( void );
int connectServer( void );

void charListStart( void );
int  charListProc( void );
void charLoginStart( void );
int charLoginProc( void );
void clientLogin( void );

void createNewCharStart( void );
int createNewCharProc( void );

void charLogoutStart( void );
int charLogoutProc( void );

void walkSendForServer( int, int, char * );
void noChecksumWalkSendForServer( int, int, char * );

void chatStrSendForServer( char *, int );

void delCharStart( void );
int delCharProc( void );

#if 0
void changePassword( char *, char * );
int changePasswordDoing( void );
#endif


#if 0
extern char gamestate_userinfo_cdkey[128];
extern char gamestate_userinfo_passwd[128];
extern char gamestate_login_charname[128];
extern char gamestate_chpass_oldpass[128];
extern char gamestate_chpass_newpass[128];
extern int gamestate_createchar_hp;
extern int gamestate_createchar_mp;
extern int gamestate_createchar_str;
extern int gamestate_createchar_tough;
extern int gamestate_createchar_skills[128];
extern char gamestate_createchar_charname[128];
extern int gamestate_createchar_imgno;
extern char gamestate_deletechar_charname[128];



// 埲壓偺娭悢偼gamestate偺僗僀僢僠偐傜傛傃偩偝傟傞張棟娭悢偱偁傞丅
// 以下の関数はgamestateのスイッチからよびだされる処理関数である。

void chpassStart(void);
void chpassProc(void);
void loginStart(void);
void loginProc(void);
void deleteCharStart(void);
void deleteCharProc(void);
void createCharStart(void);
void createCharProc(void);
void logoutStart(void);
void logoutProc(void);
void charListStart(void);
void charListProc(void);
void userInfoStart(void);
void userInfoProc(void);


#endif

#endif  /* ifndef _NETPROC_H_ */
