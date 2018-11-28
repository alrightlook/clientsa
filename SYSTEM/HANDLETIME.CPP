#define __HANDLETIME_C__

#include <system.h>
#include <time.h>
#include "handletime.h"

//#ifdef _DEBUG
//(180)�루���֣���1��
//#define LSTIME_SECONDS_PER_DAY 180
//#else
//(750X12)�루���r�g�룩��1��
#define LSTIME_SECONDS_PER_DAY 5400 /* LSTIMEһ�դ֤�άF�g������ */
//#endif
/*
  LSTIME_SECONDS_PER_DAY ��䤨��ȡ��r�g���M�߾ߺϤ�䤨�뤳�Ȥ��Ǥ��롣

  ��            LS�r�g��һ�դ�����άF�g�Εr�g��
  9000 (�˜�)   2.5 [hour]
  900           0.25[hour] = 15[min]
  90            0.025[hour] = 1.5[min] = 90[sec]
  9             9[sec]

*/

#define LSTIME_HOURS_PER_DAY 1024 /* LSTIMEһ�դ֤��LSTIME�Εr�g�� */
#define LSTIME_DAYS_PER_YEAR 100 /* LSTIMEһ��֤��LSTIME������ */


// ���ȩ`�󥨥����r�g
LSTIME SaTime;
long serverTime;
long clientTime;
int SaTimeZoneNo;	// �F�ڤΕr�g��
BOOL TimeZonePalChangeFlag; // �r�g���ǥѥ�åȥ����󥸤���ե饰
#if 0
/*------------------------------------------------------------
 * һ��`�פ�һ�غ��Ф�ơ��r�g���M��롣
 * ����
 *  �ʤ�
 * ���ꂎ
 *  �ɹ�    TRUE(1)
 *  ʧ��    FALSE(0)
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
��Ԫ���ˤ�����byHiO 1998/12/4 18:37
*******************************************************************/
static long era = (long)912766409 + 5400;
									/* SA�Ξ�ˤ��餷�� */
									/* LSԪ����x����Ƥ��ȩ`��������
                                    ���饤����Ȥ�ͬ������ʤ��Ȥ��ᡣ*/

/*******************************************************************
	�ޥ���r�g����LS�r�g�ˤ���
	long t : time�ǳ���
	LSTIME *lstime : LSTIME������ؤΥݥ���
*******************************************************************/
void RealTimeToSATime( LSTIME *lstime )
{
	long lsseconds; /* LSԪ�꤫������� */
    long lsdays; /* LSԪ�꤫������� */

	lsseconds = time( NULL ) + serverTime - clientTime - era; /* LSԪ�꤫������� */

    /* Ԫ�꤫���������1�굱��������Ǹ��ȡ���ˤʤ� */
	lstime->year = (int)( lsseconds/(LSTIME_SECONDS_PER_DAY*LSTIME_DAYS_PER_YEAR) );

    lsdays = lsseconds/LSTIME_SECONDS_PER_DAY;/* �ޤ�Ԫ�꤫���������Ӌ�㤷�� */
	lstime->day  = lsdays % LSTIME_DAYS_PER_YEAR;/* �굱����������Ǹ�ä����ޤ꤬��*/


    /*(750*12)���1��*/
    lstime->hour = (int)(lsseconds % LSTIME_SECONDS_PER_DAY )
/* �����ޤǤǡ�һ�դ�ʼ�ޤäƤ�����뤿�ä����� */
        * LSTIME_HOURS_PER_DAY / LSTIME_SECONDS_PER_DAY;
    /* һ�դ�����������Ǹ�äƤ���һ�դ�����Εr�g���򤫤���ȬF�ںΕr
     �ʤΤ����狼�롣*/

	return;
}

/*******************************************************************
	LS�r�g����ޥ���r�g�ˤ���
	LSTIME *lstime : LSTIME������ؤΥݥ���
	long *t : �g�r�g�ؤΥݥ���
*******************************************************************/
void LSTimeToRealTime( LSTIME *lstime, long *t)
{
	*t=(long)(
        ( lstime->hour*LSTIME_DAYS_PER_YEAR+lstime->day) /* �r�g */
               *LSTIME_HOURS_PER_DAY

        +     lstime->year)
        /*�����v���ϤФ��äƤ���褦��Ҋ���롣nakamura      */


        *450;
	return;
}

/*******************************************************************
	LS�r�g�ǽ�Εr�g���֤�ä�
	���ꂎ int : ҹ0����1����2��ҹ3
	LSTIME *lstime : LSTIME������ؤΥݥ���
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

// �r�g���I���ȥѥ�åȥ����󥸄I�� ***********************************/
void TimeZoneProc( void )
{
	int	timeZoneNo;
	
	//�ӣ��r�g�ǽ�Εr�g���֤�ä�
	timeZoneNo = getLSTime ( &SaTime );
	// �r�g�������ä��r
	if( SaTimeZoneNo != timeZoneNo ){
		SaTimeZoneNo = timeZoneNo;	// �r�g������
		// �r�g���ǥѥ�åȥ����󥸤���r
		if( TimeZonePalChangeFlag == TRUE ){
			PaletteChange( SaTimeZoneNo, PAL_CHANGE_TIME );// �ѥ�åȥ�����
		}
	}
}
	
