#ifndef __FAMILY_H__
#define __FAMILY_H__

#include "version.h"
#include "common.h"
#include "util.h"
#include "net.h"
#include "time.h"

#define FAMILY_MAXNUM			1000	// ��������
#ifdef _FMVER21
#define FAMILY_MAXMEMBER		200	// ��������
#define FAMILY_MAXCHANNELMEMBER		50	// Ƶ������
#else
#define FAMILY_MAXMEMBER		50	// ��������
#define FAMILY_MAXCHANNELMEMBER		10	// Ƶ������
#endif
#define FAMILY_MAXCHANNEL 		5	// ����Ƶ��

#define CHAR_MAXNAME			32
#define CHAR_MAXID			20
#define MINFMLEVLEFORPOINT      	3       // 3 ����ׯ԰��͵ȼ�
#define FMLEADERLV			30	// �峤�ȼ�

#ifdef _FAMILY_MANORNUM_CHANGE
#define FAMILY_FMPKFLOOR		15	// ����У�ͼ��
#else
#define FAMILY_FMPKFLOOR		9	// ����У�ͼ��
#define FMPOINTNUM			4       // �оݵ������������
#define MANORNUM                	4
#define FAMILY_MAXHOME			4	// ����ݵ�
#endif

enum
{
    FM_TOP_INTEGRATE = 1,    // DPTOP �ۺ�
    FM_TOP_ADV,              // DPTOP ð��
    FM_TOP_FEED,             // DPTOP ����
    FM_TOP_SYNTHESIZE,       // DPTOP �ϳ�
    FM_TOP_DEALFOOD,         // DPTOP ����
    FM_TOP_PK,               // DPTOP �У�
    FM_TOP_NUM,              // DPTOP ����
};

enum
{
	FM_FIX_ACCEPTFLAG = 1,
	FM_FIX_FMPK,
	FM_FIX_FMPET,
	FM_FIX_FMRULE,
	FM_FIX_DELFMTIME,
	FM_FIX_FMGOLD,
	FM_FIX_FMADV,
	FM_FIX_FMFEED,
	FM_FIX_FMSYNTHESIZE,
	FM_FIX_FMDEALFOOD,
	FM_FIX_FMLEADERCHANGE,
};

/*
 * �ӡ���    ��ְ�������缰 cdkey charname ��Ӯ��ë�������׻���
 * CHEKEYLEN, CHARNAMELEN
 * ë����������  �ʣ�
 */
    
void CHAR_Family(int fd, int index, char* message);
void ACAddFM(int fd, int result, int fmindex, int index);
void ACJoinFM(int fd, int result, int recv);
void ACLeaveFM(int fd, int result, int resultflag);
void ACDelFM(int fd, int result);
void ACShowFMList(int ret, int fmnum, char *data);
void ACFMDetail(int ret, char *data, int charfdid);
void ACShowMemberList(int result, int index, int fmnumm,
int fmacceptflag, int fmjoinnum, char *data);
void ACShowDpTop(int result,int num, char *data, int kindflag);
void ACShowPointList(int result, char *data);
void ACShowFMMemo(int result, int index, int num, int dataindex, char *data);

#ifdef _PERSONAL_FAME   // Arminius: �����������
void ACFMCharLogin(int fd, int ret, int index, int floor, int fmdp,
	int joinflag, int fmsetupflag, int flag, int charindex, int charfame
	);
#else
void ACFMCharLogin(int fd, int ret, int index, int floor, int fmdp,
	int joinflag, int fmsetupflag, int flag, int charindex);
#endif

void ACFMPointList(int ret, char *data);
void ACSetFMPoint(int ret, int r, int clifd);
void ACFMAnnounce(int ret, char *fmname, int fmindex, int index, int kindflag,
	char *data, int color);
void ACFixFMPK(int winindex, int loseindex, int data);
void ACFMJob( int fd, int ret, char* data1, char* data2 );

void FAMILY_Add(int fd, int meindex, char *message);
void FAMILY_Join(int fd, int meindex, char *message);
void FAMILY_Leave(int fd, int meindex, char *message);
void FAMILY_Detail(int fd, int meindex, char *message);
void FAMILY_CheckMember(int fd, int meindex, char *message);
void FAMILY_Channel(int fd, int meindex, char *message);
void FAMILY_Bank(int fd, int meindex, char *message);
void FAMILY_SetPoint(int fd, int meindex, char *message);
void FAMILY_Init(void);
void FAMILY_SetAcceptFlag(int fd, int meindex, char *message);
void FAMILY_FixRule( int fd, int meindex, char* message );
void FAMILY_RidePet( int fd, int meindex, char* message );
void FAMILY_LeaderFunc( int fd, int meindex, char* message );

#ifdef _ADD_FAMILY_TAX			   // WON ADD ����ׯ԰˰��	
void GS_ASK_TAX(void);
void FAMILY_FIX_TAX( int fd, int index, char* message);
#endif

void SetFMPetVarInit(int meindex);
void SetFMVarInit(int meindex);


int CheckFMLeader(int meindex);
int getFmLv(int playerindex);

void getNewFMList( void );
void checkFamilyIndex( void );

int CheckLeaderQ(int meindex);


// shan add Begin
struct FMMEMBER_LIST
{
//  int  fmindex;
    int  fmnum; 
    int  fmjoinnum;
//  BOOL use;                                       // 0->ûʹ��   1->ʹ��
    int  memberindex[FAMILY_MAXMEMBER];             
    char numberlistarray[FAMILY_MAXMEMBER][64];
    char memo[35][220];                             // family dengon
    int  accept;                                    // ��ļ��Ա���
    int  memonum;
    int  memoindex;
};
// ����֮������԰�
struct FMS_MEMO
{
    char memo[140][220];
    int  memonum;
    int  memoindex;
};
// ����ǿ�߱�
struct FMS_DPTOP
{
    int  num;															// ��¼�ж��ٸ�����(�ۺ�)
    char topmemo[FAMILY_MAXNUM][128];
    int  fmtopid[FAMILY_MAXNUM];          // ��������
#ifdef _FMVER21    
    int  fmtopdp[FAMILY_MAXNUM];          // �����ۺ�����
#endif    
    int  adv_num;                // ð��
    char adv_topmemo[30][96];    
    int  feed_num;               // ����
    char feed_topmemo[30][96];        
    int  syn_num;                // �ϳ�
    char syn_topmemo[30][96];        
    int  food_num;               // ����
    char food_topmemo[30][96];        
    int  pk_num;                 // �У�
    char pk_topmemo[30][96];        
};
// ����ݵ�
struct FM_POINTLIST
{
  char pointlistarray[FAMILY_MAXHOME][1024];	// Arminius: 32->1024
};
// ����PKͼ��
struct FM_PKFLOOR
{
    int fl;
};
// End

#define MAXFAMILYLIST 64*1024
extern char    familyListBuf[MAXFAMILYLIST];

void JoinMemberIndex( int charaindex, int fmindexi);


#endif
