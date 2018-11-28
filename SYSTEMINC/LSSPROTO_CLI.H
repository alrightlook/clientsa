/* output by ./lsgen.perl 0.41 ( 1998 May)
 * made Mon Nov  8 16:59:21 1999
 * user kawata
 * host indy
 * file /opt/kawata/sa/bin/output/lssproto_cli.h
 * util output/lssproto_util.c , output/lssproto_util.h
 * src  /opt/kawata/sa/bin/../doc/lssproto.html
 */
#ifndef _LSSPROTOCLI_H_
#define _LSSPROTOCLI_H_
#include "lssproto_util.h"

#ifdef MAXLSRPCARGS
#if ( MAXLSRPCARGS <= ( 13 + 1 ) )
#undef MAXLSRPCARGS
#define MAXLSRPCARGS ( 13 + 1 )
#endif
#else
#define MAXLSRPCARGS ( 13 + 1 )
#endif
void lssproto_W_send( int fd,int x,int y,char* direction ) ; /* ../doc/lssproto.html line 163 */
void lssproto_w_send( int fd,int x,int y,char* direction ) ; /* ../doc/lssproto.html line 195 */
void lssproto_EV_send( int fd,int event,int seqno,int x,int y,int dir ) ; /* ../doc/lssproto.html line 206 */
void lssproto_EV_recv( int fd,int seqno,int result ) ; /* ../doc/lssproto.html line 241 */
void lssproto_EN_send( int fd,int x,int y ) ; /* ../doc/lssproto.html line 255 */
void lssproto_DU_send( int fd,int x,int y ) ; /* ../doc/lssproto.html line 267 */
void lssproto_EN_recv( int fd,int result,int field ) ; /* ../doc/lssproto.html line 279 */
void lssproto_EO_send( int fd,int dummy ) ; /* ../doc/lssproto.html line 292 */
void lssproto_BU_send( int fd,int dummy ) ; /* ../doc/lssproto.html line 303 */
void lssproto_JB_send( int fd,int x,int y ) ; /* ../doc/lssproto.html line 314 */
void lssproto_LB_send( int fd,int x,int y ) ; /* ../doc/lssproto.html line 325 */
void lssproto_RS_recv( int fd,char* data ) ; /* ../doc/lssproto.html line 336 */
void lssproto_RD_recv( int fd,char* data ) ; /* ../doc/lssproto.html line 343 */
void lssproto_B_send( int fd,char* command ) ; /* ../doc/lssproto.html line 348 */
void lssproto_B_recv( int fd,char* command ) ; /* ../doc/lssproto.html line 371 */
void lssproto_SKD_send( int fd,int dir,int index ) ; /* ../doc/lssproto.html line 536 */
void lssproto_ID_send( int fd,int x,int y,int haveitemindex,int toindex ) ; /* ../doc/lssproto.html line 627 */
void lssproto_PI_send( int fd,int x,int y,int dir ) ; /* ../doc/lssproto.html line 656 */
void lssproto_DI_send( int fd,int x,int y,int itemindex ) ; /* ../doc/lssproto.html line 670 */
void lssproto_DG_send( int fd,int x,int y,int amount ) ; /* ../doc/lssproto.html line 687 */
void lssproto_DP_send( int fd,int x,int y,int petindex ) ; /* ../doc/lssproto.html line 700 */
void lssproto_I_recv( int fd,char* data ) ; /* ../doc/lssproto.html line 716 */
void lssproto_MI_send( int fd,int fromindex,int toindex ) ; /* ../doc/lssproto.html line 735 */
void lssproto_SI_recv( int fd,int fromindex,int toindex ) ; /* ../doc/lssproto.html line 757 */
void lssproto_MSG_send( int fd,int index,char* message,int color ) ; /* ../doc/lssproto.html line 782 */
void lssproto_MSG_recv( int fd,int aindex,char* text,int color ) ; /* ../doc/lssproto.html line 800 */
void lssproto_PMSG_send( int fd,int index,int petindex,int itemindex,char* message,int color ) ; /* ../doc/lssproto.html line 824 */
void lssproto_PME_recv( int fd,int objindex,int graphicsno,int x,int y,int dir,int flg,int no,char* cdata ) ; /* ../doc/lssproto.html line 845 */
void lssproto_AB_send( int fd ) ; /* ../doc/lssproto.html line 871 */
void lssproto_AB_recv( int fd,char* data ) ; /* ../doc/lssproto.html line 876 */
void lssproto_ABI_recv( int fd,int num,char* data ) ; /* ../doc/lssproto.html line 907 */
void lssproto_DAB_send( int fd,int index ) ; /* ../doc/lssproto.html line 915 */
void lssproto_AAB_send( int fd,int x,int y ) ; /* ../doc/lssproto.html line 929 */
void lssproto_L_send( int fd,int dir ) ; /* ../doc/lssproto.html line 942 */
void lssproto_TK_send( int fd,int x,int y,char* message,int color,int area ) ; /* ../doc/lssproto.html line 956 */
void lssproto_TK_recv( int fd,int index,char* message,int color ) ; /* ../doc/lssproto.html line 1005 */
void lssproto_MC_recv( int fd,int fl,int x1,int y1,int x2,int y2,int tilesum,int objsum,int eventsum,char* data ) ; /* ../doc/lssproto.html line 1037 */
void lssproto_M_send( int fd,int fl,int x1,int y1,int x2,int y2 ) ; /* ../doc/lssproto.html line 1065 */
void lssproto_M_recv( int fd,int fl,int x1,int y1,int x2,int y2,char* data ) ; /* ../doc/lssproto.html line 1085 */
void lssproto_C_send( int fd,int index ) ; /* ../doc/lssproto.html line 1122 */
void lssproto_C_recv( int fd,char* data ) ; /* ../doc/lssproto.html line 1129 */
void lssproto_CA_recv( int fd,char* data ) ; /* ../doc/lssproto.html line 1241 */
void lssproto_CD_recv( int fd,char* data ) ; /* ../doc/lssproto.html line 1291 */
void lssproto_R_recv( int fd,char* data ) ; /* ../doc/lssproto.html line 1301 */
void lssproto_S_send( int fd,char* category ) ; /* ../doc/lssproto.html line 1331 */
void lssproto_S_recv( int fd,char* data ) ; /* ../doc/lssproto.html line 1355 */
void lssproto_D_recv( int fd,int category,int dx,int dy,char* data ) ; /* ../doc/lssproto.html line 1701 */
void lssproto_FS_send( int fd,int flg ) ; /* ../doc/lssproto.html line 1731 */
void lssproto_FS_recv( int fd,int flg ) ; /* ../doc/lssproto.html line 1746 */
void lssproto_HL_send( int fd,int flg ) ; /* ../doc/lssproto.html line 1760 */
void lssproto_HL_recv( int fd,int flg ) ; /* ../doc/lssproto.html line 1771 */
void lssproto_PR_send( int fd,int x,int y,int request ) ; /* ../doc/lssproto.html line 1781 */
void lssproto_PR_recv( int fd,int request,int result ) ; /* ../doc/lssproto.html line 1794 */
void lssproto_KS_send( int fd,int petarray ) ; /* ../doc/lssproto.html line 1810 */
void lssproto_KS_recv( int fd,int petarray,int result ) ; /* ../doc/lssproto.html line 1821 */
void lssproto_AC_send( int fd,int x,int y,int actionno ) ; /* ../doc/lssproto.html line 1838 */
void lssproto_MU_send( int fd,int x,int y,int array,int toindex ) ; /* ../doc/lssproto.html line 1869 */
void lssproto_PS_send( int fd,int havepetindex,int havepetskill,int toindex,char* data ) ; /* ../doc/lssproto.html line 1893 */
void lssproto_PS_recv( int fd,int result,int havepetindex,int havepetskill,int toindex ) ; /* ../doc/lssproto.html line 1921 */
void lssproto_ST_send( int fd,int titleindex ) ; /* ../doc/lssproto.html line 1949 */
void lssproto_DT_send( int fd,int titleindex ) ; /* ../doc/lssproto.html line 1965 */
void lssproto_FT_send( int fd,char* data ) ; /* ../doc/lssproto.html line 1979 */
void lssproto_SKUP_recv( int fd,int point ) ; /* ../doc/lssproto.html line 1997 */
void lssproto_SKUP_send( int fd,int skillid ) ; /* ../doc/lssproto.html line 2005 */
void lssproto_KN_send( int fd,int havepetindex,char* data ) ; /* ../doc/lssproto.html line 2016 */
void lssproto_WN_recv( int fd,int windowtype,int buttontype,int seqno,int objindex,char* data ) ; /* ../doc/lssproto.html line 2033 */
void lssproto_WN_send( int fd,int x,int y,int seqno,int objindex,int select,char* data ) ; /* ../doc/lssproto.html line 2141 */
void lssproto_ClientLogin_send( int fd,char* cdkey,char* passwd ) ; /* ../doc/lssproto.html line 2190 */
void lssproto_ClientLogin_recv( int fd,char* result ) ; /* ../doc/lssproto.html line 2208 */
void lssproto_CreateNewChar_send( int fd,int dataplacenum,char* charname,int imgno,int faceimgno,int vital,int str,int tgh,int dex,int earth,int water,int fire,int wind,int hometown ) ; /* ../doc/lssproto.html line 2221 */
void lssproto_CreateNewChar_recv( int fd,char* result,char* data ) ; /* ../doc/lssproto.html line 2251 */
void lssproto_CharDelete_send( int fd,char* charname ) ; /* ../doc/lssproto.html line 2275 */
void lssproto_CharDelete_recv( int fd,char* result,char* data ) ; /* ../doc/lssproto.html line 2286 */
void lssproto_CharLogin_send( int fd,char* charname ) ; /* ../doc/lssproto.html line 2303 */
void lssproto_CharLogin_recv( int fd,char* result,char* data ) ; /* ../doc/lssproto.html line 2313 */
void lssproto_CharList_send( int fd ) ; /* ../doc/lssproto.html line 2328 */
void lssproto_CharList_recv( int fd,char* result,char* data ) ; /* ../doc/lssproto.html line 2333 */
void lssproto_CharLogout_send( int fd ) ; /* ../doc/lssproto.html line 2375 */
void lssproto_CharLogout_recv( int fd,char* result,char* data ) ; /* ../doc/lssproto.html line 2382 */
void lssproto_ProcGet_send( int fd ) ; /* ../doc/lssproto.html line 2396 */
void lssproto_ProcGet_recv( int fd,char* data ) ; /* ../doc/lssproto.html line 2402 */
void lssproto_PlayerNumGet_send( int fd ) ; /* ../doc/lssproto.html line 2414 */
void lssproto_PlayerNumGet_recv( int fd,int logincount,int player ) ; /* ../doc/lssproto.html line 2420 */
void lssproto_Echo_send( int fd,char* test ) ; /* ../doc/lssproto.html line 2432 */
void lssproto_Echo_recv( int fd,char* test ) ; /* ../doc/lssproto.html line 2443 */
void lssproto_Shutdown_send( int fd,char* passwd,int min ) ; /* ../doc/lssproto.html line 2454 */
int lssproto_InitClient( int(*)(int,char*,int)  , int bufsiz , int fd);
void lssproto_SetClientLogFiles( char *read , char *write);
void lssproto_CleanupClient(void);
int lssproto_ClientDispatchMessage(int fd ,char*line);
#endif
/* end of the generated client header code */