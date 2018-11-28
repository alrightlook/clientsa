#ifndef _SAVEDATA_H_
#define _SAVEDATA_H_

BOOL saveUserSetting( void );
BOOL loadUserSetting( void );
void setUserSetting( int );
void getUserSetting( int );
void clearUserSetting( int );
void setUserSoundOption( void );
void getUserSoundOption( void );
void setUserChatOption( void );
void getUserChatOption( void );

BOOL saveNowState( void );
BOOL loadNowState( void );

void setCdkey( char * );
char *getCdkey( char * );
void setPassword( char * );
char *getPassword( char * );

extern unsigned char savedataErrorCode;


// エラーメッセージ

#define SAVE_ERRMSG_loadNowState	"保存的数据的读取失败了(code = %d)" //"保存的数据的读取失败了。(code = %d)"


#if 0

int savePasswdFile( char *, char * );
int loadPasswdFile( char *, int, char *, int );

void checkRecvMes(int i);
void saveRecvMes(int aindex, char *msg, int color);
void deleteMes(int aindex);
void checkRecvdMes(void);

void saveNowState(void);
void loadNowState(void);

void setCdkey(char *cdkey);
void setPassword(char *pass);
char *getCdkey(char *cdkey);
char *getPassword(char *pass);

void setActionbutton(int i, int kind, int index);
void getActionbutton(int i, int *kind, int *index);

void setActionbuttonCursorPos(int i);
int getActionbuttonCursorPos(void);

void setChatState(unsigned char line, unsigned char color);
void getChatState(unsigned char *line, unsigned char *color);

void setMagicPad(int i);
char getMagicPad(int i);

void clearSavedataBuffer(void);
void setSavedataRecord(char *name);
void getNullSavedataRecord(char *name);

#endif

#endif // _SAVEDATA_H_
