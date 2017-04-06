#ifndef __LOG_H__
#define __LOG_H__
#include <time.h>
typedef enum
{
    LOG_TALK,
    LOG_PROC,
    LOG_ITEM,
    LOG_STONE,
    LOG_PET,
    LOG_TENSEI,
    LOG_KILL,     //ttom 12/26/2000 kill the pet & items
    LOG_TRADE,	// CoolFish: 2001/4/19
    LOG_HACK,	// Arminius 2001/6/14
    LOG_SPEED,	// Nuke 0626
    LOG_FMPOP,	// CoolFish: 2001/9/12
    LOG_FAMILY, // Robin 10/02
    LOG_GM,     // Shan 
#ifdef _GAMBLE_ROULETTE
	LOG_GAMBLE,
#endif

	LOG_LOGIN,
	PETTRANS,
//Syu ����ׯ԰սʤ��Log
	LOG_FMPKRESULT,

// Syu ADD ��������������д�ȡLog (������������)
	LOG_BANKSTONELOG,

	LOG_ACMESS,
	LOG_PKCONTEND,
    LOG_TYPE_NUM,
}LOG_TYPE;

void closeAllLogFile( void );
BOOL initLog( char* filename );
void printl( LOG_TYPE logtype, char* format , ... );


void LogAcMess( int fd, char *type, char *mess );

void LogItem(
	char *CharName, /* ƽ�ҷ�����   */
	char *CharID,
	int ItemNo, 	/* ʧ��  ة  į */
	char *Key, 		/* ƽ�������� */
	int floor,		/* ��   */
	int x,
	int y,
    char *uniquecode, // shan 2001/12/14
	char *itemname, int itemID
);
void LogPkContend( char *teamname1, char *teamname2,
	int floor, int x, int y, int flg );

void LogPetTrans(
	char *cdkey, char *uniwuecde, char *uniwuecde2, char *CharName, int floor, int x, int y,
	int petID1, char *PetName1, int petLV, int petrank, int vital1, int str1, int tgh1, int dex1, int total1,
	int petID2, char *PetName2, int vital2, int str2, int tgh2, int dex2, int total2,
	int work0, int work1, int work2, int work3, int ans, int trans
);

void LogPet(
	char *CharName, /* ƽ�ҷ�����   */
	char *CharID,
	char *PetName,
	int PetLv,
	char *Key, 		/* ƽ�������� */
	int floor,		/* ��   */
	int x,
	int y,
	char *uniquecode  // shan 2001/12/14
);

void LogTensei(
	char *CharName, /* ƽ�ҷ�����   */
	char *CharID,
	char *Key, 		/* ƽ�������� */
	int level,		//��ì��
	int transNum,	//��Ϸ����
	int quest,		//�;޵�����
	int home,		//���컩
	int item,		//  ��ʧ��  ة  ��Ի��
	int pet,		//  ��ʸ����  ��Ի��
	int vital,		//  ���Vital
	int b_vital,	//  ��  vital
	int str,		//  ���str
	int b_str,		//  ��  str
	int tgh,		//  ���      
	int b_tgh,		//  ��        
	int dex,		//  ���      
	int b_dex		//  ��        
);
// Syu ADD ��������������д�ȡLog (������������)
void LogFamilyBankStone(
        char *CharName,
        char *CharId,  
        int Gold,      
		int MyGold,
        char *Key,     
		int floor,           
        int x,
        int y,
		int banksum
);

void LogStone(
				int TotalGold,
        char *CharName, /* ƽ�ҷ�����   */
        char *CharId,   /* �����ǡ�ID */
        int Gold,               /* �ź� */
		int MyGold,
        char *Key,              /* ƽ�������� */
        int floor,              /* ��   */
        int x,
        int y
);

void LogTalk(
	char *CharName, /* ƽ�ҷ�����   */
	char *CharID,
	int floor,		/* ��   */
	int x,
	int y,
	char *message
);                                
//ttom 12/26/2000 kill pet & items
void LogKill(
        char *CharName,
        char *CharId,
        char *CharPet_Item
);
//ttom

// CoolFish: 2001/4/19
void LogTrade(char *message);
// CoolFish: 2001/9/12
void LogFMPOP(char *message);

// Arminius 2001/6/14
enum
{
    HACK_NOTHING,
    HACK_GETFUNCFAIL,
    HACK_NOTDISPATCHED,
    HACK_CHECKSUMERROR, 
    HACK_HP,
	HACK_TYPE_NUM,
}HACK_TYPE;
void logHack(int fd, int errcode);
// Nuke 0626
void logSpeed(int fd);

void closeAllLogFile( void );
int openAllLogFile( void );

// Robin 10/02
void LogFamily(
	char *FMName,
	int fmindex,
        char *charName,
        char *charId,
	char *keyWord,
	char *data
);


// Shan 11/02
void LogGM(
        char *CharName,    //��ɫ����
        char *CharID,      //���ID
        char *Message,     //ָ������
        int  floor,
        int  x,
        int  y
);


void LogLogin(
        char *CharID,   //���ID
        char *CharName, //��ɫ����
		int  saveIndex,
		char *ipadress
);

void LogCreatFUPet(
	char *PetName, int petid, int lv, int hp,
	int vital, int str, int tgh, int dex,
	int fixstr, int fixtgh, int fixdex, int trans, int flg);

#ifdef _GAMBLE_ROULETTE

void LogGamble(
        char *CharName, //��ɫ����
        char *CharID,   //���ID
        char *Key,      //˵��
        int floor,
        int x,
        int y,
		int player_stone,	//��ӵ�н�Ǯ
		int Gamble_stone,	//��ע����
		int get_stone,		//���
		int Gamble_num,
		int flg	//flg = 1 ��� 2 ׯ��
);
#endif

void LogBankStone(
        char *CharName, /* ƽ�ҷ�����   */
        char *CharId, /* �����ǡ�ID */
		int	meindex,
        int Gold,               /* �ź� */
        char *Key,              /* ƽ�������� */
        int floor,              /* ��   */
        int x,
        int y,
		int my_gold,
		int my_personagold
);

//Syu ����ׯ԰սʤ��Log
void Logfmpk(
			 char *winner, int winnerindex, int num1,
			 char *loser, int loserindex, int num2, char *date, char *buf1, char *buf2, int flg);

void backupAllLogFile( struct tm *ptm );

void LogPetPointChange( 
	char * CharName, char *CharID, char *PetName, int petindex, int errtype,
	int PetLv, char *Key,int floor, int x, int y);

void LogPetFeed(
	char * CharName, char *CharID, char *PetName, int petindex,
	int PetLv, char *Key,int floor, int x, int y, char *ucode);

void warplog_to_file( void );
void warplog_from_file( void );

typedef struct {
	int floor;
	int incount;
	int outcount;
}tagWarplog;
#define MAXMAPNUM 700
extern tagWarplog warplog[MAXMAPNUM];

typedef struct {
	int floor1;
	int floor2;
	int count;
}tagWarpCount;
#define MAXMAPLINK 1000
extern tagWarpCount warpCount[MAXMAPLINK];

#endif
