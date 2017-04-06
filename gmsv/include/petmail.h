#ifndef  __PETMAIL_H__
#define __PETMAIL_H__
#include "version.h"
/*
 * ʸ������  ��ɬ��ƥ�ʣ�
 */
#define PETMAIL_OFFMSG_MAX          10000
#define PETMAIL_OFFMSG_TIMEOUT      ( 3 * 24 * 3600 )
#define PETMAIL_CHECK_OFFMSG_EXPIRE_INTERVAL  3600
#define PETMAIL_OFFMSG_TEXTLEN 512

/* ʸ���������Ｐ���ض���ѣ�  �˼��ѷ�Ի���ئ�����³��� */
#define		 PETMAIL_EFFECTMAX	1			

#define		PETMAIL_SPOOLFLOOR		777
#define		PETMAIL_SPOOLX			30
#define		PETMAIL_SPOOLY			30

#define		PETMAIL_LOOPINTERVAL1	500
#define		PETMAIL_LOOPINTERVAL2	5000

/* ��׷��̼���ƽ�ҷ³߼�����������ë������ */
typedef struct
{
    int use;
    time_t send_tm;     /* �����ǻ�����������ë˪����������+TIMEOUT�� */
    int color;							/*   ƽ�������� */
    char text[PETMAIL_OFFMSG_TEXTLEN];     /*   ƽ������     */
    char destcd[CDKEYLEN];				/* ˪����CDKEY */
    char destcharname[CHARNAMELEN];		/* ˪���Ƽ�ƽ�ҷ�   */
    char srccd[CDKEYLEN];				/* ˪���缰cdkey */
    char srccharname[CHARNAMELEN];		/* ˪���缰ƽ�ҷ�   */
	
} PETMAIL_offmsg;

void PETMAIL_Loopfunc( int index);
BOOL PETMAIL_CheckPlayerExist( int index, int mode);

BOOL PETMAIL_initOffmsgBuffer( int count );
BOOL PETMAIL_addOffmsg( int fromindex, char *tocdkey, char *tocharaname,
                            char *text, int color );
PETMAIL_offmsg *PETMAIL_getOffmsg( int offmsgindex);
BOOL PETMAIL_deleteOffmsg( int offmsgindex);
void PETMAIL_proc( void );
BOOL storePetmail( void);
BOOL PETMAIL_sendPetMail( int cindex, int aindex, 
					int havepetindex, int haveitemindex, char* text , int color );

#ifdef _PETMAIL_DEFNUMS
void CHAR_AutoPickupMailPet( int charaindex, int petindex );
#endif
int PETMAIL_getPetMailTotalnums( void);
void PETMAIL_delPetMailTotalnums( int numflg);
void PETMAIL_setPetMailTotalnums( int numflg);
int PETMAIL_CheckIsMyOffmsg( int fromindex, char *tocdkey, char *tocharaname);

#endif
