#ifndef __CHAR_DATA_H__
#define __CHAR_DATA_H__

#include "item.h"
#include "char.h"


#define CHAR_CLASS01_PASSLV     20

#define BATEBAN	2
#define FIRSTPOSITIONMAXINDEX   1
#define ELDERINDEXSTART     FIRSTPOSITIONMAXINDEX+BATEBAN+1

typedef struct tagLevelUpPattern
{
#if 1
    struct  Exptbl{
        int origin;
        int multi;
    }exptbl[3];
    int     hpupminpoint;   /*  hp ��ؤ����   쵰�*/
    int     hpupmaxpoint;   /*  hp ��ؤ����   쵰�*/

    int     needexp;        /*    ۢئ������    */
#else
    int     uppoint[5];
#endif
}LevelUpPattern;

/*====================���������Ƥ��ʧ��������====================*/
BOOL CHAR_getDefaultChar( Char*  nc, int imagenumber );

/*====================��    ��  �  į׸������====================*/
int CHAR_getNewImagenumberFromEquip( int basebaseimagenumber,
                                     ITEM_CATEGORY category );
/*====================  �  į  ٯ   ->   į����====================*/
//void CHAR_initSeekGraphicNumberFromString();
int CHAR_seekGraphicNumberFromString( char* string );



/*====================ƽ�ҷ¼����������====================*/
BOOL CHAR_getInitElderPosition( Char* ch,int hometown);
BOOL CHAR_getElderPosition( int elderindex, int* fl, int* x, int* y );

void CHAR_setInitValues( Char* ch );


/*====================  ����  ����====================*/
typedef enum
{
    CHAR_INVAREA,
    CHAR_CANNOTDROPAREA,
    CHAR_CANNOTMAGICAREA,
	CHAR_AREAKIND_NONE,
}CHAR_AREAKIND;

BOOL CHAR_initInvinciblePlace( char* filename );
BOOL CHAR_isCannotMagicArea( int floor , int x, int y, int magicnum );
BOOL CHAR_initAppearPosition( char* filename );
BOOL CHAR_isInvincibleArea( int floor , int x, int y );
BOOL CHAR_isAppearPosition( int floor, int *x, int *y);
BOOL CHAR_isCannotDropArea( int floor , int x, int y );


/* Ӯ  ���� */
BOOL CHAR_ElderSetPosition( int elderindex ,int fl,int x ,int y);



/*************************************************
������ì��ʧ������  
*************************************************/


int GetEnemyExp( int level );

int CHAR_GetLevelExp( int charaindex, int level );

int CHAR_LevelUpCheck( int charaindex , int toindex);

int CHAR_PetLevelUp( int petindex );

BOOL CHAR_checkFaceImageNumber( int imagenumber, int faceimagenumber);
BOOL CHAR_checkPlayerImageNumber( int imagenumber);

int CHAR_PetTakeLevelUp( int petindex, int lv);	//ǿ�Ƴ�������
int CHAR_PetTakeDrop( int petindex, int floor, int ox, int oy);	//ǿ�ƶ�����������

int PETTRANS_PetTransManStatus( int toindex, int petindex1, int petindex2);
#ifdef _PET_2TRANS
int NPC_PetTransManGetAns( int total1, int total2, int LV, int rank, int tran);
#else
int NPC_PetTransManGetAns( int total1, int total2, int LV, int rank);
#endif //_PET_2TRANS

#ifdef _CHIKULA_STONE
void CHAR_AutoChikulaStone( int charaindex, int Dflg);
#endif

int CHAR_findSurplusPetBox( int charaindex );

#ifdef _USER_EXP_CF
void setNeedLevelUpTbls(int level,int exp);
void setMaxLevel(int level);
#endif 
#ifdef _FM_METAMO
void CHAR_ReMetamo( int charaindex );
#endif 
#ifdef _NEW_PLAYER_RIDE
void CHAR_PlayerRide( int charaindex );
#endif 
#ifdef _USER_EXP_CF
BOOL LoadEXP( char* filename );
#endif 
#endif 
