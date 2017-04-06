#ifndef __HANDLETIME_H__ 
#define __HANDLETIME_H__

#include <sys/time.h>

#include "common.h"

#undef EXTERN
#ifdef __HANDLETIME_C__
#define EXTERN
#else  /*__HANDLETIME_C__*/
#define EXTERN extern
#endif

EXTERN struct timeval NowTime;
EXTERN int     DEBUG_ADJUSTTIME;

BOOL setNewTime( void );
#ifdef _ASSESS_SYSEFFICACY
void Assess_InitSysEfficacy( void);
void Assess_SysEfficacy( int flg);
void ASSESS_getSysEfficacy( float *TVsec);
#ifdef _ASSESS_SYSEFFICACY_SUB
void Assess_SysEfficacy_sub( int flg, int loop);
void ASSESS_getSysEfficacy_sub( float *TVsec, int loop);
#endif
#endif
// WON REM
//struct tm *localtime(const time_t *timep);

/*�����Ʊ��HiO��LS���������*/
/*    ����150(��)*60=9000  =750*12 ƥ   LS1     */
/*  LS1200����  LS1     */
/*  LS100  ƥ   LS1     */
typedef struct tagLSTIME
{
	int year;
	int day;
	int hour;
}LSTIME;

#define NIGHT_TO_MORNING	700
#define MORNING_TO_NOON		930
#define NOON_TO_EVENING		200
#define EVENING_TO_NIGHT	300

typedef enum
{
	LS_NIGHT = 0,
	LS_MORNING ,
	LS_NOON ,
	LS_EVENING ,
}LSTIME_SECTION;

void RealTimeToLSTime(long t , LSTIME *lstime);
/*void LSTimeToRealTime( LSTIME *lstime, long *t);*/
LSTIME_SECTION getLSTime (LSTIME *lstime);

#endif  
