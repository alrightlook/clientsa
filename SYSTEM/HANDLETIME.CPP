#define __HANDLETIME_C__

#include <system.h>
#include <time.h>
#include "handletime.h"

//#ifdef _DEBUG
//(180)秒（３分）で1日
//#define LSTIME_SECONDS_PER_DAY 180
//#else
//(750X12)秒（１時間半）で1日
#define LSTIME_SECONDS_PER_DAY 5400 /* LSTIME一日ぶんの現実の秒数 */
//#endif
/*
  LSTIME_SECONDS_PER_DAY を変えると、時間の進み具合を変えることができる。

  値            LS時間で一日あたりの現実の時間数
  9000 (標準)   2.5 [hour]
  900           0.25[hour] = 15[min]
  90            0.025[hour] = 1.5[min] = 90[sec]
  9             9[sec]

*/

#define LSTIME_HOURS_PER_DAY 1024 /* LSTIME一日ぶんのLSTIMEの時間数 */
#define LSTIME_DAYS_PER_YEAR 100 /* LSTIME一年ぶんのLSTIMEの日数 */


// ストーンエイジ時間
LSTIME SaTime;
long serverTime;
long clientTime;
int SaTimeZoneNo;	// 現在の時間帯
BOOL TimeZonePalChangeFlag; // 時間帯でパレットチェンジするフラグ
#if 0
/*------------------------------------------------------------
 * 一ループに一回呼ばれて、時間を進める。
 * 引数
 *  なし
 * 返り値
 *  成功    TRUE(1)
 *  失敗    FALSE(0)
 ------------------------------------------------------------*/
BOOL setNewTime( void )
{
    if( gettimeofday( &NowTime, (struct timezone*)NULL) != 0 )
        return FALSE;
    NowTime.tv_sec += DEBUG_ADJUSTTIME;
    return TRUE;
}
#endif

/*******************************************************************
今元旦にした。byHiO 1998/12/4 18:37
*******************************************************************/
static long era = (long)912766409 + 5400;
									/* SAの為にずらした */
									/* LS元年を定義するてきとーな数値。
                                    クライアントと同じじゃないとだめ。*/

/*******************************************************************
	マシン時間からLS時間にする
	long t : timeで出る
	LSTIME *lstime : LSTIME構造体へのポインタ
*******************************************************************/
void RealTimeToSATime( LSTIME *lstime )
{
	long lsseconds; /* LS元年からの秒数 */
    long lsdays; /* LS元年からの日数 */

	lsseconds = time( NULL ) + serverTime - clientTime - era; /* LS元年からの秒数 */

    /* 元年からの秒数を1年当りの秒数で割ると、年になる */
	lstime->year = (int)( lsseconds/(LSTIME_SECONDS_PER_DAY*LSTIME_DAYS_PER_YEAR) );

    lsdays = lsseconds/LSTIME_SECONDS_PER_DAY;/* まず元年からの日数を計算して */
	lstime->day  = lsdays % LSTIME_DAYS_PER_YEAR;/* 年当たりの日数で割ったあまりが日*/


    /*(750*12)秒で1日*/
    lstime->hour = (int)(lsseconds % LSTIME_SECONDS_PER_DAY )
/* ここまでで、一日が始まってから何秒たったか。 */
        * LSTIME_HOURS_PER_DAY / LSTIME_SECONDS_PER_DAY;
    /* 一日あたりの秒数で割ってから一日あたりの時間数をかけると現在何時
     なのかがわかる。*/

	return;
}

/*******************************************************************
	LS時間からマシン時間にする
	LSTIME *lstime : LSTIME構造体へのポインタ
	long *t : 実時間へのポインタ
*******************************************************************/
void LSTimeToRealTime( LSTIME *lstime, long *t)
{
	*t=(long)(
        ( lstime->hour*LSTIME_DAYS_PER_YEAR+lstime->day) /* 時間 */
               *LSTIME_HOURS_PER_DAY

        +     lstime->year)
        /*この関数はばぐっているように見える。nakamura      */


        *450;
	return;
}

/*******************************************************************
	LS時間で今の時間区分を得る
	戻り値 int : 夜0、朝1、昼2、夜3
	LSTIME *lstime : LSTIME構造体へのポインタ
*******************************************************************/
LSTIME_SECTION getLSTime (LSTIME *lstime)
{
	if (NIGHT_TO_MORNING < lstime->hour
        && lstime->hour <= MORNING_TO_NOON)
		return LS_MORNING;
	else if(NOON_TO_EVENING < lstime->hour
            && lstime->hour <= EVENING_TO_NIGHT)
		return LS_EVENING;
	else if(EVENING_TO_NIGHT < lstime->hour
            && lstime->hour <= NIGHT_TO_MORNING)
		return LS_NIGHT;
	else
		return LS_NOON;
}

// 時間帯処理とパレットチェンジ処理 ***********************************/
void TimeZoneProc( void )
{
	int	timeZoneNo;
	
	//ＳＡ時間で今の時間区分を得る
	timeZoneNo = getLSTime ( &SaTime );
	// 時間帯が変わった時
	if( SaTimeZoneNo != timeZoneNo ){
		SaTimeZoneNo = timeZoneNo;	// 時間帯更新
		// 時間帯でパレットチェンジする時
		if( TimeZonePalChangeFlag == TRUE ){
			PaletteChange( SaTimeZoneNo, PAL_CHANGE_TIME );// パレットチェンジ
		}
	}
}
	

