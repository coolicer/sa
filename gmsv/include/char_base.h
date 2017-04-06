#ifndef __CHAR_BASE_H__
#define __CHAR_BASE_H__

#include "version.h"
#include "skill.h"
#include "title.h"
/*#include "magicinfo.h"*/
#include "addressbook.h"

#define CHAR_DELIMITER "\n"

#define NONCHAR_DELIMITER "|"

#define STATUSSENDDELIMITER "|"

#define CHAR_MAXGOLDHAVE    (1000*10000)
#define CHAR_MAXBANKGOLDHAVE    (10000*10000)
#define CHAR_MAXFMBANKGOLDHAVE	(10000*10000*2)

#ifdef _PERSONAL_FAME	// Arminius 8.30: �����������
#define MAX_PERSONALFAME (10000 * 10000)
#endif
#define CHAR_SKILLMAXHAVE   5

#define CHAR_TITLEMAXHAVE   30

#define NULLSKILL   "0"

#define CHAR_MAXPETHAVE         5
#define CHAR_MAXPERSONAGOLD		50000000

#ifdef _ADD_POOL_ITEM			   // WON ADD ���ӿɼķŵĵ���
#define CHAR_MAXPOOLPETHAVE         15
#else
#define CHAR_MAXPOOLPETHAVE         10
#endif

#define CHAR_MAXPETSKILLHAVE       7
#define CHAR_PARTYMAX           5
#define	CHAR_MAXATTRIB			100
#define CHAR_MAXUPLEVEL			140
#define	CHAR_MAXDUELPOINT		100000000
#define CHAR_DEFAULTMAXAI		(60)
#define CHAR_MAXVARIABLEAI		(100*100)
#define CHAR_MINVARIABLEAI		(-100*100)
#define CHAR_POISONSTRING           "�ж�"
#define CHAR_RECOVERPOISONSTRING    "�ⶾ��"
#define CHAR_PARALYSISSTRING        "�����"
#define CHAR_RECOVERPARALYSISSTRING "��������"
#define CHAR_SILENCESTRING          "�޷���ħ����"
#define CHAR_RECOVERSILENCESTRING   "������ħ����"
#define CHAR_STONESTRING            "ʯ����"
#define CHAR_RECOVERSTONESTRING     "�������"
#define CHAR_DARKNESSSTRING         "���ܲ�����"
#define CHAR_RECOVERDARKNESSSTRING  "���ܳ�����"
#define CHAR_CONFUSIONSTRING        "����ֻ�"
#define CHAR_RECOVERCONFUSIONSTRING "�ָ�����"

#define CHAR_DEFAULTSEESIZ  23
#define MAP_CHAR_DEFAULTSEESIZ      (CHAR_DEFAULTSEESIZ+4)

typedef enum
{
    CHAR_HEAD,               
    CHAR_BODY, 
    CHAR_ARM,
    CHAR_DECORATION1,
    CHAR_DECORATION2,
    CHAR_EQUIPPLACENUM,
}CHAR_EquipPlace;


typedef struct tagCharHaveSkill
{
    int     use;
    Skill   skill;
}CHAR_HaveSkill;

typedef enum
{
    CHAR_ACTSTAND,
    CHAR_ACTWALK,
    CHAR_ACTATTACK,
    CHAR_ACTTHROW,
    CHAR_ACTDAMAGE,
    CHAR_ACTDEAD,
    CHAR_ACTMAGIC,
    CHAR_ACTITEM,
    CHAR_ACTEFFECT,
    CHAR_ACTDOWN = 10,
    CHAR_ACTSIT,
    CHAR_ACTHAND,
    CHAR_ACTPLEASURE,
    CHAR_ACTANGRY,
    CHAR_ACTSAD,
    CHAR_ACTGUARD,
    CHAR_ACTACTIONWALK,
    CHAR_ACTNOD,
    CHAR_ACTACTIONSTAND,
    CHAR_ACTBATTLE = 20,
    CHAR_ACTLEADER,
	CHAR_ACTBATTLEWATCH,
	CHAR_ACTPOPUPNAME,
	CHAR_ACTTURN = 30,
	CHAR_ACTWARP,
	CHAR_ACTTRADE,
#ifdef _MIND_ICON
	CHAR_MIND = 40,
#endif
#ifdef _ITEM_CRACKER
	CHAR_ITEM_CRACKER = 50,
#endif
}CHAR_ACTION;

typedef enum
{
    CHAR_CLIACTATTACK,
    CHAR_CLIACTDAMAGE,
    CHAR_CLIACTDOWN,
    CHAR_CLIACTSTAND,
    CHAR_CLIACTWALK,
    CHAR_CLIACTSIT,
    CHAR_CLIACTHAND,
    CHAR_CLIACTPLEASURE,
    CHAR_CLIACTANGRY,
    CHAR_CLIACTSAD,
    CHAR_CLIACTGUARD,
	CHAR_CLIACTNOD,
	CHAR_CLIACTTHROW,

}CHAR_CLIENTACTION;

typedef enum
{
    CHAR_TYPENONE,
    CHAR_TYPEPLAYER,
    CHAR_TYPEENEMY,
    CHAR_TYPEPET,
    CHAR_TYPEDOOR,
    CHAR_TYPEBOX,
    CHAR_TYPEMSG,
    CHAR_TYPEWARP,
    CHAR_TYPESHOP,
    CHAR_TYPEHEALER,
    CHAR_TYPEOLDMAN,
    CHAR_TYPEROOMADMIN,
    CHAR_TYPETOWNPEOPLE,
    CHAR_TYPEDENGON,
    CHAR_TYPEADM,
    CHAR_TYPETEMPLE,
    CHAR_TYPESTORYTELLER,
    CHAR_TYPERANKING,
    CHAR_TYPEOTHERNPC,
    CHAR_TYPEPRINTPASSMAN,
    CHAR_TYPENPCENEMY,
    CHAR_TYPEACTION,
    CHAR_TYPEWINDOWMAN,
    CHAR_TYPESAVEPOINT,
    CHAR_TYPEWINDOWHEALER,
	CHAR_TYPEITEMSHOP,
	CHAR_TYPESTONESHOP,
	CHAR_TYPEDUELRANKING,
	CHAR_TYPEWARPMAN,
	CHAR_TYPEEVENT,
	CHAR_TYPEMIC,
	CHAR_TYPELUCKYMAN,
	CHAR_TYPEBUS,
	CHAR_TYPECHARM,
	CHAR_TYPECHECKMAN,
	CHAR_TYPEJANKEN,
	CHAR_TYPETRANSMIGRATION,
	CHAR_TYPEFMWARPMAN,			// ����У˳�����Ա
	CHAR_TYPEFMSCHEDULEMAN,		// ����У˳��Ǽ�Ա
	CHAR_TYPEMANORSCHEDULEMAN,	// ׯ԰�У˳�ԤԼ��
#ifdef _GAMBLE_BANK
	CHAR_GAMBLEBANK,
#endif
#ifdef _NEW_WARPMAN
	CHAR_NEWNPCMAN,
#endif

#ifdef _GAMBLE_ROULETTE
	CHAR_GAMBLEROULETTE,
	CHAR_GAMBLEMASTER,
#endif
#ifdef _TRANSER_MAN
	CHAR_TRANSERMANS,
#endif

#ifdef _ITEM_NPCCHANGE
	CHAR_ITEMCHANGENPC,
#endif
#ifdef _CFREE_petskill
	CHAR_FREESKILLSHOP,
#endif
#ifdef _PETRACE
	CHAR_PETRACEMASTER,			// ���ﾺ��
	CHAR_PETRACEPET,
#endif

#ifdef _ALLDOMAN // (���ɿ�) Syu ADD ���а�NPC
  CHAR_TYPEALLDOMAN,
#endif

    CHAR_TYPEPETMAKER,              // petmaker

    CHAR_TYPENUM,
}CHAR_TYPE;

typedef enum
{
	CHAR_EVENT_NONE,
  CHAR_EVENT_NPC,
  CHAR_EVENT_ENEMY,
  CHAR_EVENT_WARP,
  CHAR_EVENT_DOOR,
	CHAR_EVENT_ALTERRATIVE,
	CHAR_EVENT_WARP_MORNING,
	CHAR_EVENT_WARP_NOON,
	CHAR_EVENT_WARP_NIGHT,
  CHAR_EVENTNUM,
}CHAR_EVENT;

typedef enum
{
    CHAR_PARTY_NONE,
    CHAR_PARTY_LEADER,
    CHAR_PARTY_CLIENT,
}CHAR_PARTY_MODE;

typedef enum
{
      CHAR_TRADE_FREE,      /* �ɽ��� */
      CHAR_TRADE_SENDING,   /* ���������� */
      CHAR_TRADE_TRADING,   /* ������ */
      CHAR_TRADE_LOCK,	    /* ���������� */
}CHAR_TRADE_MODE;
                  
typedef enum
{
    CHAR_PETMAIL_NONE,
    CHAR_PETMAIL_IDLE1,
    CHAR_PETMAIL_IDLE2,
    CHAR_PETMAIL_RETURNWAIT,
    CHAR_PETMAIL_IDLE3,
    CHAR_PETMAIL_IDLE4,
    CHAR_PETMAIL_IDLE5,
}CHAR_PETMAIL_MODE;

typedef enum
{
    CHAR_PETFOLLOW_NONE,
    CHAR_PETFOLLOW_NOW,
}CHAR_PETFOLLOW_MODE;

#ifdef _DROPSTAKENEW
typedef enum
{
	DICE_BIG = 1,		// ����_��
	DICE_SMALL,			// ����_С
	DICE_ALLEAT,		// ����_ͨ��
	PET_RACE1 = 5,		// ���ٳ�_����1
	PET_RACE2,			// ���ٳ�_����2
	PET_RACE3,			// ���ٳ�_����3
	ROULETTE1 = 11,	ROULETTE2, ROULETTE3, ROULETTE4, ROULETTE5,	ROULETTE6, ROULETTE7, 
	ROULETTE8, ROULETTE9, ROULETTE10, ROULETTE11, ROULETTE12, ROULETTE13, ROULETTE14,
	ROULETTE15,	ROULETTE16,	ROULETTE17,	ROULETTE18,	ROULETTE19,	ROULETTE20,	
	ROULETTE21 = 41, ROULETTE22, ROULETTE23, ROULETTE24,	ROULETTE25,	ROULETTE26,	
	ROULETTE27,	ROULETTE28,	ROULETTE29, ROULETTE30, ROULETTE31, ROULETTE32,
	ROULETTE33,	ROULETTE34,	ROULETTE35,	ROULETTE36,	ROULETTE37,	ROULETTE38,
	ROULETTE39,	ROULETTE40,
	ROULETTE41 = 71, ROULETTE42, ROULETTE43, ROULETTE44, ROULETTE45,
	ROULETTE51 = 101, ROULETTE52, ROULETTE53,	//����
	ROULETTE61 = 111, ROULETTE62,				//1��10 10��11
	PETFIGHT_WIN1 = 120,
	PETFIGHT_WIN2,
	PETFIGHT_DUEL,
}GAMBLE_TYPE;
#endif

#ifdef _ADD_POOL_ITEM				// (���ɿ�) WON ADD ���ӿɼķŵĵ���
#define CHAR_MAXPOOLITEMHAVE	30
#else
#define CHAR_MAXPOOLITEMHAVE	20
#endif

#define CHAR_MAXITEMNUM  15
#define CHAR_STARTITEMARRAY     CHAR_EQUIPPLACENUM
#define CHAR_MAXITEMHAVE (CHAR_STARTITEMARRAY+CHAR_MAXITEMNUM)

typedef enum
{
    CHAR_DATAPLACENUMBER,
    CHAR_BASEIMAGENUMBER,
    CHAR_BASEBASEIMAGENUMBER,
    CHAR_FACEIMAGENUMBER,
    CHAR_FLOOR,
    CHAR_X,
    CHAR_Y,
    CHAR_DIR,
    CHAR_LV,
    CHAR_GOLD,
    CHAR_HP,
    CHAR_MP,
    CHAR_MAXMP,
    CHAR_VITAL,
    CHAR_STR,
    CHAR_TOUGH,
    CHAR_DEX,
    CHAR_CHARM,
    CHAR_LUCK,
    CHAR_EARTHAT,
    CHAR_WATERAT,
    CHAR_FIREAT,
    CHAR_WINDAT,
    CHAR_DEFAULTPET,
    CHAR_CRITIAL,
    CHAR_COUNTER,
	CHAR_RARE,
    CHAR_RADARSTRENGTH,
    CHAR_CHATVOLUME,
    CHAR_MERCHANTLEVEL,
    CHAR_HEALERLEVEL,
    CHAR_SAVEPOINT = CHAR_HEALERLEVEL,
    CHAR_DETERMINEITEM,
    CHAR_INDEXOFEQTITLE,
    CHAR_POISON,
    CHAR_PARALYSIS,
    CHAR_SLEEP,
    CHAR_STONE,
    CHAR_DRUNK,
    CHAR_CONFUSION,
    CHAR_LOGINCOUNT,
    CHAR_NPCCREATEINDEX=CHAR_LOGINCOUNT,
    CHAR_DEADCOUNT,
    CHAR_WALKCOUNT,
    CHAR_TALKCOUNT,
    CHAR_DAMAGECOUNT,
    CHAR_GETPETCOUNT,
    CHAR_KILLPETCOUNT,
    CHAR_DEADPETCOUNT,
		CHAR_SENDMAILCOUNT,
		CHAR_MERGEITEMCOUNT,
		CHAR_DUELBATTLECOUNT,
		CHAR_DUELWINCOUNT,
		CHAR_DUELLOSECOUNT,
		CHAR_DUELSTWINCOUNT,
		CHAR_DUELMAXSTWINCOUNT,
    CHAR_WHICHTYPE,
    CHAR_WALKINTERVAL,
    CHAR_LOOPINTERVAL,
		CHAR_OLDEXP,
		CHAR_EXP,
    CHAR_LASTTALKELDER,
    CHAR_SKILLUPPOINT,
    CHAR_LEVELUPPOINT,
    CHAR_IMAGETYPE,
    CHAR_NAMECOLOR,
    CHAR_POPUPNAMECOLOR,
    CHAR_LASTTIMESETLUCK,
    CHAR_DUELPOINT,
    CHAR_ENDEVENT,				/* (0~31)		*/
    CHAR_ENDEVENT2,				/* (32~63)		*/
    CHAR_ENDEVENT3,				/* (64~96)		*/
#ifdef _NEWEVENT
	CHAR_ENDEVENT4,				/* (96~127) */
	CHAR_ENDEVENT5,				/* (128~159) */
	CHAR_ENDEVENT6,				/* (160~191) */
#endif
	CHAR_NOWEVENT,				/* (0~31)		*/
	CHAR_NOWEVENT2,				/* (32~63)		*/
	CHAR_NOWEVENT3,				/* (64~96)		*/
#ifdef _NEWEVENT
	CHAR_NOWEVENT4,				/* (96~127)		*/
	CHAR_NOWEVENT5,				/* (128~159)	*/
	CHAR_NOWEVENT6,				/* (160~191)	*/
#endif
	CHAR_TRANSMIGRATION,
	CHAR_TRANSEQUATION,
  CHAR_INITDATA,
  CHAR_SILENT,				/* char shutup time */    
  CHAR_FMINDEX,				// ���� index
  CHAR_FMLEADERFLAG,			/* �����Ա����
    							 * FMMEMBER_NONE	:û�м����κμ���
    							 * FMMEMBER_APPLY	:������������
    							 * FMMEMBER_LEADER	:�峤
    							 * FMMEMBER_MEMBER	:һ���Ա
    							 * FMMEMBER_ELDER	:����    
    							 * FMMEMBER_INVITE	:��˾			// ��ʱ����
    							 * FMMEMBER_BAILEE	:����			// ��ʱ����
    							 * FMMEMBER_VICELEADER  :���峤		// ��ʱ����
    							*/
  CHAR_FMSPRITE,		// �����ػ�����

  CHAR_BANKGOLD,
  CHAR_RIDEPET,
  CHAR_LEARNRIDE,
#ifdef _NEW_RIDEPETS
	CHAR_LOWRIDEPETS,
#endif
  CHAR_LIMITLEVEL,
    
#ifdef _PERSONAL_FAME	// Arminius 8.30: �����������
    CHAR_FAME,
#endif

#ifdef _NEWSAVE
    CHAR_SAVEINDEXNUMBER,	/*  SaveFile .0.char or .1.char */
#endif
 
#ifdef _ATTACK_MAGIC
		CHAR_EARTH_EXP,						// ��ҵĵ�ħ��������
		CHAR_WATER_EXP,						// ��ҵ�ˮħ��������
		CHAR_FIRE_EXP,						// ��ҵĻ�ħ��������
		CHAR_WIND_EXP,						// ��ҵķ�ħ��������
		CHAR_EARTH_RESIST,					// ��ҵĵ�ħ������
		CHAR_WATER_RESIST,					// ��ҵ�ˮħ������
		CHAR_FIRE_RESIST,					// ��ҵĻ�ħ������
		CHAR_WIND_RESIST,					// ��ҵķ�ħ������
		CHAR_EARTH_ATTMAGIC_EXP,			// ��ҵĵ�ħ�������Ⱦ���ֵ
		CHAR_WATER_ATTMAGIC_EXP,			// ��ҵ�ˮħ�������Ⱦ���ֵ
		CHAR_FIRE_ATTMAGIC_EXP,				// ��ҵĻ�ħ�������Ⱦ���ֵ
		CHAR_WIND_ATTMAGIC_EXP,				// ��ҵķ�ħ�������Ⱦ���ֵ
	 	CHAR_EARTH_DEFMAGIC_EXP,			// ��ҵĵ�ħ�����Ծ���ֵ
		CHAR_WATER_DEFMAGIC_EXP,			// ��ҵ�ˮħ�����Ծ���ֵ
		CHAR_FIRE_DEFMAGIC_EXP,				// ��ҵĻ�ħ�����Ծ���ֵ
		CHAR_WIND_DEFMAGIC_EXP,				// ��ҵķ�ħ�����Ծ���ֵ
#endif


#ifdef _GAMBLE_BANK
		CHAR_PERSONAGOLD,	//�ĳ���������
#endif
#ifdef _DROPSTAKENEW
		CHAR_GAMBLENUM,		//�ĳ�����
#endif
#ifdef _ADD_ACTION          //npc����
		CHAR_ACTIONSTYLE,
#endif

#ifdef _FM_JOINLIMIT
		CHAR_FMTIMELIMIT,
#endif

#ifdef _ALLDOMAN // (���ɿ�) Syu ADD ���а�NPC
		CHAR_HEROFLOOR, 
#endif
#ifdef _PETSKILL_BECOMEPIG
		CHAR_BECOMEPIG,
		CHAR_BECOMEPIG_BBI,//Ҫ��ɵ�ͼ��
#endif
		CHAR_LASTLEAVETIME, // Robin add ��������ʱ��

#ifdef _ITEM_ADDEXP2
		CHAR_ADDEXPPOWER,
		CHAR_ADDEXPTIME,
#endif

#ifdef _GM_ITEM
	CHAR_GMTIME,       // ���GM�������
#endif

#ifdef _VIP_SERVER
	CHAR_AMPOINT,      // ��Ա����
#endif

#ifdef _VIP_RIDE
	CHAR_VIPRIDE,      // ��Ա��ڼ�
#endif

		CHAR_LISTPAGE,

#ifdef _AUTO_PK
		CHAR_AUTOPK,       // �Զ���PK
#endif
#ifdef _BATTLE_PK
		CHAR_BATTLEPK,
#endif

#ifdef _PET_BEATITUDE
		CHAR_BEATITUDE,
#endif
#ifdef _RIDE_CF
	CHAR_LOWRIDEPETS1,
#endif
		CHAR_DATAINTNUM,

}CHAR_DATAINT;


typedef enum {
    CHAR_MODAI    			= CHAR_CHARM,    		/*  ʸ����  ���������Ĥ�Ұ�    */
	CHAR_VARIABLEAI 		= CHAR_LUCK,			/*  ʸ�����������  ����  ��ߨ�  ���װ�ƥ  �Ȼ�����   */
    CHAR_SLOT 				= CHAR_DEFAULTPET,		/* ��  ë��������  �µ���������  ʸ����������  */
   	CHAR_PETGETLV			= CHAR_CHATVOLUME,		/* ��������ľ����ì��  ʸ������������   */
    CHAR_PUTPETTIME			= CHAR_LOGINCOUNT,		/*  ʸ��������  ��  ��ľ������ */
	CHAR_MAILMODE 			= CHAR_RADARSTRENGTH,	/*   ʸ������������  ������˪������ */
    CHAR_ALLOCPOINT			= CHAR_LEVELUPPOINT,	/* ��ì��ʧ�����ݼ��Ի��Ի���̼���(ʸ������) */
    CHAR_PETMAILBUFINDEX	= CHAR_GETPETCOUNT, 	/* ʸ����������(offmsg)�߼�index */
	CHAR_PETMAILIDLETIME	= CHAR_SENDMAILCOUNT,	/* ʸ���������Ｐ�м�����ئ�� */
	CHAR_PETMAILFROMFLOOR	= CHAR_DUELBATTLECOUNT,	/* ������ë˪�����ݼ��׷�ʧ */
	CHAR_PETMAILFROMX		= CHAR_DUELWINCOUNT,	/* ������ë˪�����ݼ�  ��   */
	CHAR_PETMAILFROMY		= CHAR_DUELLOSECOUNT,	/* ������ë˪�����ݼ�  ��   */
	CHAR_PETMAILEFFECT		= CHAR_DUELSTWINCOUNT,	/* ʸ���������Ｐ���ؼ��ް�������  į */
	CHAR_PETSENDMAILCOUNT	= CHAR_TALKCOUNT,		/* ʸ������������ë���Ϸּ��� */
    CHAR_PETRANK			= CHAR_LASTTIMESETLUCK, /* ʸ�������¼���(ʸ������  ��  ��ۨ߷��޷¡��� */
	CHAR_PETID				= CHAR_DUELMAXSTWINCOUNT,/* ʸ������  ������������     */
    CHAR_PETFAMILY		= CHAR_FMLEADERFLAG,		// CoolFish: Family �ж��Ƿ�Ϊ�����ػ���
}CHAR_PET;


typedef enum
{
    CHAR_NAME,
    CHAR_OWNTITLE,
    CHAR_USERPETNAME = CHAR_OWNTITLE,
    CHAR_NPCARGUMENT,
    CHAR_CDKEY = CHAR_NPCARGUMENT,
    CHAR_OWNERCDKEY,
    CHAR_OWNERCHARANAME,
    CHAR_FMNAME,
#ifdef _UNIQUE_P_I
    CHAR_UNIQUECODE,
#endif
#ifdef _ACTION_GMQUE
	CHAR_GMQUESTR1,
#endif

#ifdef _ITEM_SETLOVER
	CHAR_LOVE,         // ����ж�
	CHAR_LOVERID,      // �����ʺ�
	CHAR_LOVERNAME,    // ��������
#endif
#ifdef _GM_ITEM
	CHAR_GMFUNCTION,   // ���GM����
#endif
       CHAR_NEWNAME,
    CHAR_DATACHARNUM,
}CHAR_DATACHAR;

typedef enum
{
    CHAR_ISATTACK,
    CHAR_ISATTACKED,
    CHAR_ISOVER,
    CHAR_ISOVERED,
    CHAR_HAVEHEIGHT,
    CHAR_ISVISIBLE,
    CHAR_ISTRANSPARENT,
    CHAR_ISFLYING,
    CHAR_ISDIE,
    CHAR_ISBIG,
    CHAR_ISSHOWBATTLEMSG,
    CHAR_ISPARTY,
    CHAR_ISWARP,
    CHAR_ISDUEL,
    CHAR_ISPARTYCHAT,
    CHAR_ISTRADECARD,
    CHAR_ISTRADE,
    CHAR_FLGNUM,
}CHAR_DATAFLG;

#define CHAR_FS_PARTY			(1 << 0)
#define CHAR_FS_BATTLE		(1 << 1)
#define CHAR_FS_DUEL    	(1 << 2)
#define CHAR_FS_PARTYCHAT	(1 << 3)	//����Ƶ������
#define CHAR_FS_TRADECARD	(1 << 4)
#define CHAR_FS_TRADE			(1 << 5)
typedef enum
{
    CHAR_WORKBATTLEMODE,
    CHAR_WORKBATTLEINDEX,
    CHAR_WORKBATTLESIDE,
    CHAR_WORKBATTLECOM1,
    CHAR_WORKBATTLECOM2,
    CHAR_WORKBATTLECOM3,
#ifdef _PSKILL_MDFYATTACK
	CHAR_WORKBATTLECOM4,
#endif
    CHAR_WORKBATTLEFLG,
	CHAR_WORKBATTLEWATCH,

    CHAR_WORKFIXEARTHAT,
    CHAR_WORKFIXWATERAT,
    CHAR_WORKFIXFIREAT,
    CHAR_WORKFIXWINDAT,
	CHAR_WORKMAXHP,
    CHAR_WORKMAXMP,

    CHAR_WORKATTACKPOWER,
    CHAR_WORKDEFENCEPOWER,
    CHAR_WORKQUICK,

    CHAR_WORKFIXVITAL,
    CHAR_WORKFIXSTR,
    CHAR_WORKFIXTOUGH,
    CHAR_WORKFIXDEX,

#ifdef _ITEMSET5_TXT
	CHAR_WORKFIXARRANGE,
	CHAR_WORKARRANGEPOWER,
	CHAR_WORKFIXSEQUENCE,
	CHAR_WORKSEQUENCEPOWER,
	CHAR_WORKATTACHPILE,
	CHAR_WORKHITRIGHT,	//��������
#endif
#ifdef _ITEMSET6_TXT
	CHAR_WORKNEGLECTGUARD,
#endif
    CHAR_WORKMODATTACK,
    CHAR_WORKMODDEFENCE,
    CHAR_WORKMODQUICK,

    CHAR_WORKMODCAPTURE,
    CHAR_WORKMODCHARM,

    CHAR_WORKFIXCHARM,
    CHAR_WORKFIXLUCK,
    CHAR_WORKFIXAI,
    CHAR_WORKFIXAVOID,

	CHAR_WORKDAMAGEABSROB,
	CHAR_WORKDAMAGEREFLEC,
	CHAR_WORKDAMAGEVANISH,
    CHAR_WORKPOISON,
    CHAR_WORKPARALYSIS,
    CHAR_WORKSLEEP,
    CHAR_WORKSTONE,
    CHAR_WORKDRUNK,
    CHAR_WORKCONFUSION,
    CHAR_WORKWEAKEN,             //����  vincent add  _MAGIC_WEAKEN
	CHAR_WORKDEEPPOISON,         //�綾  vincent add _MAGIC_DEEPPOISON
	CHAR_WORKBARRIER,            //ħ��  vincent add _MAGIC_BARRIER
	CHAR_WORKNOCAST,             //��Ĭ  vincent add _MAGIC_NOCAST  
    CHAR_WORKMODPOISON,
    CHAR_WORKMODPARALYSIS,
    CHAR_WORKMODSLEEP,
    CHAR_WORKMODSTONE,
    CHAR_WORKMODDRUNK,
    CHAR_WORKMODCONFUSION,
	CHAR_WORKMODWEAKEN,
	CHAR_WORKMODDEEPPOISON,
	CHAR_WORKMODBARRIER,		//ħ��
	CHAR_WORKMODNOCAST,			//��Ĭ
	CHAR_WORKMODCRITICAL,		//һ����ɱ
    CHAR_WORKULTIMATE,
    CHAR_WORKPARTYMODE,
    CHAR_WORKTRADEMODE,         /* ����ģʽ
    							 * 0: CHAR_TRADE_FREE   :�ɽ���
                                 * 1: CHAR_TRADE_SENDING:����������
                                 * 2: CHAR_TRADE_TRADING:������
                                 * 3: CHAR_TRADE_LOCK   :����������
                                */
    CHAR_WORKPARTYINDEX1,
    CHAR_WORKPARTYINDEX2,
    CHAR_WORKPARTYINDEX3,
    CHAR_WORKPARTYINDEX4,
    CHAR_WORKPARTYINDEX5,
    CHAR_WORKOBJINDEX,
    CHAR_WORKWALKSTARTSEC,
    CHAR_WORKWALKSTARTMSEC,
    CHAR_WORKLOOPSTARTSEC,
    CHAR_WORKLOOPSTARTMSEC,
    CHAR_WORKLASTATTACKCHARAINDEX,
    CHAR_WORKEVENTTYPE,
    CHAR_WORKGETEXP,
	CHAR_WORKMODCAPTUREDEFAULT,
    CHAR_WORKACTION,
	CHAR_WORKFD,
	CHAR_WORKFLG,
	CHAR_WORKGMLEVEL,
    CHAR_NPCWORKINT1,
    CHAR_NPCWORKINT2,
    CHAR_NPCWORKINT3,
    CHAR_NPCWORKINT4,
    CHAR_NPCWORKINT5,
    CHAR_NPCWORKINT6,
    CHAR_NPCWORKINT7,
    CHAR_NPCWORKINT8,
    CHAR_NPCWORKINT9,
    CHAR_NPCWORKINT10,
    CHAR_NPCWORKINT11,
#ifdef _PETRACE
	CHAR_NPCWORKINT12,
#endif
	CHAR_NPCWORKINT13,
    CHAR_WORKWARPCHECK,
    CHAR_TENSEICHECKED,    
    CHAR_WORKFMINDEXI,		// �������� index
    CHAR_WORKFMCHANNEL,		// ����Ƶ��
    CHAR_WORKFMCHANNELQUICK,
    CHAR_WORKFMFLOOR,		// ����ݵ�ͼ��
    CHAR_WORKFMDP,			// ����ģ�ֵ
    CHAR_WORKFMPKFLAG,		// �Ƿ����������У� 1:Yes
    CHAR_WORKFMSETUPFLAG,	// �����Ƿ�����ʽ����
    CHAR_WORKFMMANINDEX,	// FMWARPMAN Index
    CHAR_WORKFMCHARINDEX,	// �����Ա���� index
    CHAR_WORKBATTLEFLAG,    // -1(ս������)��1(��ս��)    

	CHAR_WORK_PET0_STAT,        
	CHAR_WORK_PET1_STAT,
	CHAR_WORK_PET2_STAT,
	CHAR_WORK_PET3_STAT,
	CHAR_WORK_PET4_STAT,	

    CHAR_WORKLOGINTIME,		// ��ҵ���ʱ��
    CHAR_WORKTALKCOUNT,
    CHAR_WORKTALKTIME,
    CHAR_WORKPETFOLLOW,		// �������
    CHAR_WORKPETFALL,		// ս������
    CHAR_WORKLEADERCHANGE,	// �峤��λindex
#ifdef _EQUIT_DEFMAGIC
	CHAR_EQUITDEFMAGIC_E,
	CHAR_EQUITDEFMAGIC_WA,
	CHAR_EQUITDEFMAGIC_F,
	CHAR_EQUITDEFMAGIC_WI,
	CHAR_EQUITQUIMAGIC,
#endif
#ifdef _PETMAIL_DEFNUMS
	CHAR_PETMAILNUMS,
	CHAR_PETMAILSENDTIME,
#endif
#ifdef _ITEM_METAMO
	CHAR_WORKITEMMETAMO,
#endif
#ifdef _ADD_DEAMGEDEFC
    CHAR_WORKOTHERDMAGE,
    CHAR_WORKOTHERDEFC,
#endif

#ifdef _LOSE_FINCH_ 
	CHAR_WORKSPETRELIFE,	//���︴������
#endif

#ifdef _CHIKULA_STONE
	CHAR_WORKCHIKULAHP,
	CHAR_WORKCHIKULAMP,
#endif
#ifdef _PETSKILL_SETDUCK
	CHAR_MYSKILLDUCKPOWER,
	CHAR_MYSKILLDUCK,
	CHAR_MYSKILLSTRPOWER,
	CHAR_MYSKILLSTR,
	CHAR_MYSKILLTGHPOWER,
	CHAR_MYSKILLTGH,
	CHAR_MYSKILLDEXPOWER,
	CHAR_MYSKILLDEX,
	CHAR_MAGICPETMP,
#endif
	CHAR_SKILLSTRPOWER,
	CHAR_SKILLDEXPOWER,
#ifdef _OTHER_MAGICSTAUTS
	CHAR_DEFMAGICSTATUS,
#ifdef _MAGIC_SUPERWALL
	CHAR_MAGICSUPERWALL,
#endif
	CHAR_OTHERSTATUSNUMS,
#endif
#ifdef _NPCCHANGE_PLAYERIMG
	CHAR_WORKNPCMETAMO,
#endif
	CHAR_PickAllPet,
#ifdef _DROPSTAKENEW
	CHAR_WORKSTAKEFLAG,
	CHAR_WORKSTAKETYPE1,
	CHAR_WORKSTAKETYPE2,
	CHAR_WORKSTAKETYPE3,
	CHAR_WORKSTAKETYPE4,
	CHAR_WORKSTAKETYPE5,
#endif

	CHAR_WORKTOXICATION,

#ifdef _SUIT_ITEM
	CHAR_WORKSUITITEM,
	CHAR_WORKROUNDHP,
	CHAR_WORKROUNDMP,
	CHAR_WORKSUITVIT,
	CHAR_WORKSUITSTR,
	CHAR_WORKSUITTGH,
	CHAR_WORKSUITDEX,
	CHAR_WORKSUITMODSTR,
#ifdef _SUIT_ADDENDUM
    CHAR_WORKRESIST, //�쳣������
	CHAR_WORKCOUNTER,//������
	CHAR_WORKMPOWER, //��ǿ��ʦ��ħ��( ��Ӱ��ķ���:��ɽȪ ������ ������ ������ ������ ������ ������ ����ĩ�� ����ǹ ��Ѫ���� ��Ѫ�� һ���Ѫ ������ )
#endif
#ifdef _SUIT_TWFWENDUM
	CHAR_WORK_EA,//��
	CHAR_WORK_WR,//ˮ
	CHAR_WORK_FI,//��
	CHAR_WORK_WI,//��
#endif
#ifdef _SUIT_ADDPART3
	CHAR_WORKDUCKPOWER,//��װ�ر�
    CHAR_WORKRENOCAST, //��Ĭ������
	CHAR_WORKSUITSTR_P,//������ ��λΪ%
	CHAR_WORKSUITTGH_P,//������ ��λΪ%
	CHAR_WORKSUITDEX_P,//������ ��λΪ%
#endif
#endif//_SUIT_ITEM

#ifdef _IMPRECATE_ITEM
	CHAR_WORKHURTMP,		//�˺� MP
	CHAR_WORKWISHESHP,		//ף�� hp
	CHAR_WORKWISHESMP,		//ף�� MP
	CHAR_WORKIMPRECATENUM1,
	CHAR_WORKIMPRECATENUM2,
	CHAR_WORKIMPRECATENUM3,
#endif
#ifdef _BATTLENPC_WARP_PLAYER
	CHAR_WORKTURN,
#endif
#ifdef _MIND_ICON
	CHAR_MIND_NUM,
#endif
#ifdef _ITEM_CRACKER
	CHAR_WORKITEM_CRACKER,
#endif
#ifdef _ITEM_ADDEXP	//vincent ��������
	CHAR_WORKITEM_ADDEXP,
    CHAR_WORKITEM_ADDEXPTIME,
#endif
#ifdef _ALLDOMAN // (���ɿ�) Syu ADD ���а�NPC
	CHAR_WORKHEROFLOOR, 
#endif

#ifdef _MAGICSTAUTS_RESIST
	CHAR_MAGICFIRE, //�𿹾���
	CHAR_MAGICTHUNDER,//�翹����
	CHAR_MAGICICE,//��������
#endif

#ifdef _PET_SKILL_SARS			// WON ADD ��ɷ����
	CHAR_WORKSARS,
	CHAR_WORKMODSARS,
#endif

#ifdef _PETSKILL_RETRACE
	CHAR_WORKRETRACE,
#endif

#ifdef _PETSKILL_BECOMEFOX
     CHAR_WORKFOXROUND,  //����������غ���
#endif

	CHAR_WORKSTANDBYPET, // Robin add ������
#ifdef _MAP_TIME
	CHAR_WORK_MAP_TIME,	// �����ͼ����
#endif
	CHAR_WORKSKYWALKER, // GM������??

	CHAR_WORKLASTMERGETIME, // ����ϳ�ʱ��

#ifdef _TIME_TICKET
	CHAR_WORKTICKETTIME,
	CHAR_WORKTICKETTIMESTART,
#endif

	CHAR_WORKDEBUGMODE, // ����ģʽ��

	CHAR_WORKDATAINTNUM,


    CHAR_WORKENCOUNTPROBABILITY_MIN = CHAR_NPCWORKINT1,
    CHAR_WORKENCOUNTPROBABILITY_MAX = CHAR_NPCWORKINT2,
    CHAR_WORK_TOHELOS_CUTRATE = CHAR_NPCWORKINT3,
    CHAR_WORK_TOHELOS_COUNT = CHAR_NPCWORKINT4,
    CHAR_WORKSHOPRELEVANT = CHAR_NPCWORKINT5,
    CHAR_WORKSHOPRELEVANTSEC = CHAR_NPCWORKINT6,
    CHAR_WORKSHOPRELEVANTTRD = CHAR_NPCWORKINT7,
    CHAR_WORKTRADER = CHAR_NPCWORKINT8,
	CHAR_WORKEFFECT = CHAR_NPCWORKINT9,
	CHAR_WORKRENAMEITEMNUM = CHAR_NPCWORKINT10, 
	CHAR_WORKRENAMEITEMINDEX = CHAR_NPCWORKINT5,
    CHAR_WORKPLAYERINDEX = CHAR_NPCWORKINT1,
    CHAR_WORKTACTICS = CHAR_NPCWORKINT2,
    CHAR_WORKPETFOLLOWMODE = CHAR_NPCWORKINT3,
    CHAR_WORKPETFOLLOWCOUNT = CHAR_NPCWORKINT4,
	CHAR_WORK_PETFLG = CHAR_NPCWORKINT1,
    CHAR_WORKGENERATEINDEX = CHAR_NPCWORKINT1,
    CHAR_WORKDOORCLOSETIME  = CHAR_NPCWORKINT1,
    CHAR_WORKDOORSWITCHCOUNT = CHAR_NPCWORKINT2,
    CHAR_WORKDOOROPENG = CHAR_NPCWORKINT3,
    CHAR_WORKDOORCLOSEG = CHAR_NPCWORKINT4,
    CHAR_WORKDOORSOONFLG = CHAR_NPCWORKINT5,
    CHAR_WORKDOORPASSFLG = CHAR_NPCWORKINT6,
    CHAR_WORKDOORKEYITEMID = CHAR_NPCWORKINT7,
    CHAR_WORKDOORSWITCHCURRENT = CHAR_NPCWORKINT8,
    CHAR_WORKDOORORIGINALGRAPHIC = CHAR_NPCWORKINT9,
    CHAR_WORKDOOREXPIRETIME = CHAR_NPCWORKINT10,
    CHAR_WORKOLDMANID = CHAR_NPCWORKINT1,
    CHAR_WORKSHOPCLIENTINDEX = CHAR_NPCWORKINT1,
    CHAR_WORKDENGONMAXID = CHAR_NPCWORKINT1,

}CHAR_WORKDATAINT;


enum{
	WORKFLG_DEBUGMODE = ( 1 << 0 ),
	WORKFLG_MICMODE = ( 2 << 0 ),
};

typedef enum
{
    CHAR_INITFUNC,
    CHAR_FIRSTFUNCTION = CHAR_INITFUNC,
    CHAR_WALKPREFUNC,
    CHAR_WALKPOSTFUNC,
    CHAR_PREOVERFUNC,
    CHAR_POSTOVERFUNC,
    CHAR_WATCHFUNC,
    CHAR_LOOPFUNC,
    CHAR_DYINGFUNC,
    CHAR_TALKEDFUNC,
    CHAR_PREATTACKEDFUNC,
    CHAR_POSTATTACKEDFUNC,
    CHAR_OFFFUNC,
    CHAR_LOOKEDFUNC,
    CHAR_ITEMPUTFUNC,
    CHAR_SPECIALTALKEDFUNC,
    CHAR_WINDOWTALKEDFUNC,
#ifdef _USER_CHARLOOPS
	CHAR_LOOPFUNCTEMP1,
	CHAR_LOOPFUNCTEMP2,
	CHAR_BATTLEPROPERTY,
#endif
    CHAR_LASTFUNCTION,
    CHAR_FUNCTABLENUM = CHAR_LASTFUNCTION,
}CHAR_FUNCTABLE;

typedef enum
{
    CHAR_WORKWALKARRAY,
    CHAR_NPCWORKCHAR1,
    CHAR_NPCWORKCHAR2,
    CHAR_NPCWORKCHAR3,
    CHAR_NPCWORKCHAR4,
    CHAR_NPCWORKCHAR5,
    CHAR_NPCWORKCHAR6,
	CHAR_WORKDATACHARNUM,
    CHAR_WORKBATTLE_TACTICSOPTION = CHAR_NPCWORKCHAR1,
#ifdef _BATTLENPC_WARP_PLAYER
	CHAR_WORKBATTLE_ACT_CONDITION = CHAR_NPCWORKCHAR2,
#endif
    CHAR_WORKDOORPASSWD			= CHAR_NPCWORKCHAR1,
    CHAR_WORKDOORNAME			= CHAR_NPCWORKCHAR2,
    CHAR_WORKDOORGOLDLIMIT		= CHAR_NPCWORKCHAR3,
    CHAR_WORKDOORWEEK			= CHAR_NPCWORKCHAR4,
    CHAR_WORKDOORHOUR			= CHAR_NPCWORKCHAR5,
    CHAR_WORKDOORMINUTE			= CHAR_NPCWORKCHAR6,
    CHAR_WORKDOORTITLE			= CHAR_NPCWORKCHAR6,
    CHAR_WORKDOORMANDOORNAME	= CHAR_NPCWORKCHAR1,
    CHAR_TIME1,
    CHAR_TIME2			= CHAR_NPCWORKCHAR6,
	
}CHAR_WORKDATACHAR;


typedef enum
{
    CHAR_IMAGETYPE_GIRL,
    CHAR_IMAGETYPE_BOY,
    CHAR_IMAGETYPE_CHILDBOY,
    CHAR_IMAGETYPE_CHILDGIRL,
    CHAR_IMAGETYPE_MAN,
    CHAR_IMAGETYPE_WOMAN,
    CHAR_IMAGETYPE_OLDMAN,
    CHAR_IMAGETYPE_DOG,
    CHAR_IMAGETYPENUM
}CHAR_ImageType;


#define			CHAR_P_STRING_HP				( 1 << 1  )		// 0x00000002
#define			CHAR_P_STRING_MAXHP				( 1 << 2  )		// 0x00000004
#define			CHAR_P_STRING_MP				( 1 << 3  )		// 0x00000008
#define			CHAR_P_STRING_MAXMP				( 1 << 4  )		// 0x00000010
#define			CHAR_P_STRING_VITAL				( 1 << 5  )
#define			CHAR_P_STRING_STR				( 1 << 6  )
#define			CHAR_P_STRING_TOUGH				( 1 << 7  )
#define			CHAR_P_STRING_DEX				( 1 << 8  )
#define			CHAR_P_STRING_EXP				( 1 << 9  )
#define			CHAR_P_STRING_NEXTEXP			( 1 << 10 )
#define			CHAR_P_STRING_LV				( 1 << 11 )
#define			CHAR_P_STRING_ATK				( 1 << 12 )
#define			CHAR_P_STRING_DEF				( 1 << 13 )
#define			CHAR_P_STRING_QUICK				( 1 << 14 )
#define			CHAR_P_STRING_CHARM				( 1 << 15 )
#define			CHAR_P_STRING_LUCK				( 1 << 16 )
#define			CHAR_P_STRING_EARTH				( 1 << 17 )
#define			CHAR_P_STRING_WATER				( 1 << 18 )
#define			CHAR_P_STRING_FIRE				( 1 << 19 )
#define			CHAR_P_STRING_WIND				( 1 << 20 )
#define			CHAR_P_STRING_GOLD				( 1 << 21 )
#define			CHAR_P_STRING_TITLE				( 1 << 22 )
#define			CHAR_P_STRING_DUELPOINT			( 1 << 23 )
#define			CHAR_P_STRING_TRANSMIGRATION	( 1 << 24 )
#define			CHAR_P_STRING_NAME				( 1 << 25 )
#define			CHAR_P_STRING_OWNTITLE			( 1 << 26 )
#define			CHAR_P_STRING_RIDEPET			( 1 << 27 )		// 0x08000000
#define			CHAR_P_STRING_LEARNRIDE			( 1 << 28 )		// 0x10000000
#define			CHAR_P_STRING_BASEBASEIMAGENUMBER	( 1 << 29 )		// 0x20000000
#define			CHAR_P_STRING_SKYWALKER			( 1 << 30 )		// 0x40000000
#define			CHAR_P_STRING_DEBUGMODE			( 1 << 31 )		// 0x80000000



#define			CHAR_N_STRING_OBJINDEX			( 1 << 1  )
#define			CHAR_N_STRING_LV				( 1 << 2  )
#define			CHAR_N_STRING_MAXHP				( 1 << 3  )
#define			CHAR_N_STRING_HP				( 1 << 4  )
#define			CHAR_N_STRING_MP				( 1 << 5  )
#define			CHAR_N_STRING_NAME				( 1 << 6  )

#define			CHAR_K_STRING_BASEIMAGENUMBER   ( 1 << 1  )
#define			CHAR_K_STRING_HP				( 1 << 2  )
#define			CHAR_K_STRING_MAXHP				( 1 << 3  )
#define			CHAR_K_STRING_MP				( 1 << 4  )
#define			CHAR_K_STRING_MAXMP				( 1 << 5  )
#define			CHAR_K_STRING_EXP				( 1 << 6  )
#define			CHAR_K_STRING_NEXTEXP			( 1 << 7  )
#define			CHAR_K_STRING_LV				( 1 << 8  )
#define			CHAR_K_STRING_ATK				( 1 << 9  )
#define			CHAR_K_STRING_DEF				( 1 << 10 )
#define			CHAR_K_STRING_QUICK				( 1 << 11 )
#define			CHAR_K_STRING_AI				( 1 << 12 )
#define			CHAR_K_STRING_EARTH				( 1 << 13 )
#define			CHAR_K_STRING_WATER				( 1 << 14 )
#define			CHAR_K_STRING_FIRE				( 1 << 15 )
#define			CHAR_K_STRING_WIND				( 1 << 16 )
#define			CHAR_K_STRING_SLOT				( 1 << 17 )
#define			CHAR_K_STRING_CHANGENAMEFLG		( 1 << 18 )
#define			CHAR_K_STRING_NAME				( 1 << 19 )
#define			CHAR_K_STRING_USERPETNAME		( 1 << 20 )

typedef union {
	int		indexOfPet[CHAR_MAXPETHAVE];
    int		indexOfPetskill[CHAR_MAXPETSKILLHAVE];
}CHAR_UNIONTABLE;

typedef struct tagChar
{
    BOOL        use;

    int			data[CHAR_DATAINTNUM];
    STRING64	string[CHAR_DATACHARNUM];
    char        flg[(CHAR_FLGNUM%(sizeof(char)*8))
                   ? (CHAR_FLGNUM/(sizeof(char)*8))+1
                   : (CHAR_FLGNUM/(sizeof(char)*8))];
    int			indexOfExistItems[CHAR_MAXITEMHAVE];
    int			indexOfExistPoolItems[CHAR_MAXPOOLITEMHAVE];

    CHAR_HaveSkill		haveSkill[CHAR_SKILLMAXHAVE];
    int			indexOfHaveTitle[CHAR_TITLEMAXHAVE];
    ADDRESSBOOK_entry	addressBook[ADDRESSBOOK_MAX];
    CHAR_UNIONTABLE		unionTable;
    int			indexOfPoolPet[CHAR_MAXPOOLPETHAVE];
    STRING32    charfunctable[CHAR_FUNCTABLENUM];
    int         workint[CHAR_WORKDATAINTNUM];
    STRING64    workchar[CHAR_WORKDATACHARNUM];
    int         CharMakeSequenceNumber;
    void*       functable[CHAR_FUNCTABLENUM];
}Char;


#define		CHAR_CHECKINDEX( index)	_CHAR_CHECKINDEX( __FILE__, __LINE__, index)
INLINE BOOL _CHAR_CHECKINDEX( char *file, int line, int index);
#define		CHAR_CHECKITEMINDEX( charaindex, iindex) _CHAR_CHECKITEMINDEX( __FILE__, __LINE__, charaindex, iindex)
INLINE BOOL _CHAR_CHECKITEMINDEX( char *file, int line, int charaindex, int iindex );
#define		CHAR_CHECKPETINDEX( petindex) _CHAR_CHECKPETINDEX( __FILE__, __LINE__, petindex )
INLINE BOOL _CHAR_CHECKPETINDEX( char *file, int line, int petindex );
#define		CHAR_CHECKPOOLPETINDEX( petindex) _CHAR_CHECKPOOLPETINDEX( __FILE__, __LINE__, petindex )
INLINE BOOL _CHAR_CHECKPOOLPETINDEX( char *file, int line, int petindex );
#define		CHAR_CHECKPETSKILLINDEX( havepetskillindex) _CHAR_CHECKPETSKILLINDEX( __FILE__, __LINE__, havepetskillindex)
INLINE BOOL _CHAR_CHECKPETSKILLINDEX( char *file, int line, int havepetskillindex );

INLINE int CHAR_getInt( int index , CHAR_DATAINT element);
#define CHAR_setInt( index , element, data) _CHAR_setInt( __FILE__, __LINE__, index , element, data)
INLINE int _CHAR_setInt( char *file, int line, int index ,CHAR_DATAINT element, int data);
#ifdef _FIX_SETWORKINT
#define CHAR_getWorkInt( index , element) _CHAR_getWorkInt( __FILE__, __LINE__, index , element)
INLINE int _CHAR_getWorkInt( char *file, int line, int index ,CHAR_WORKDATAINT element);
#define CHAR_setWorkInt( index, element, data) _CHAR_setWorkInt( __FILE__, __LINE__, index , element, data)
INLINE int _CHAR_setWorkInt( char *file, int line, int index ,CHAR_WORKDATAINT element, int data);
#else
INLINE int CHAR_setWorkInt( int index ,CHAR_WORKDATAINT element, int data);
#endif
#define		CHAR_getChar( index, elem) _CHAR_getChar( __FILE__, __LINE__, index, elem)
INLINE char* _CHAR_getChar( char *file, int line, int index ,CHAR_DATACHAR element );
#define		CHAR_setChar( index, elem, n) _CHAR_setChar( __FILE__, __LINE__, index, elem, n)
INLINE BOOL _CHAR_setChar( char *file, int line, int index ,CHAR_DATACHAR element, char* new );
#define		CHAR_getFlg( index, elem) _CHAR_getFlg( __FILE__, __LINE__, index, elem)
INLINE BOOL _CHAR_getFlg( char *file, int line, int index ,CHAR_DATAFLG element );
#define		CHAR_setFlg( index, elem, n) _CHAR_setFlg( __FILE__, __LINE__, index, elem, n)
INLINE BOOL _CHAR_setFlg( char *file, int line, int index , CHAR_DATACHAR element, int newdata );
#define		CHAR_getWorkChar( index, elem) _CHAR_getWorkChar( __FILE__, __LINE__, index, elem)
INLINE char * _CHAR_getWorkChar( char *file, int line, int index, CHAR_WORKDATACHAR element );
#define		CHAR_setWorkChar( index, elem, n) _CHAR_setWorkChar( __FILE__, __LINE__, index, elem, n)
INLINE BOOL _CHAR_setWorkChar( char *file, int line, int index, CHAR_WORKDATACHAR element, char * new);

#define		CHAR_getItemIndex( index, iindex) _CHAR_getItemIndex( __FILE__, __LINE__, index, iindex)
INLINE int _CHAR_getItemIndex( char *file, int line, int charaindex, int ti);
#define		CHAR_setItemIndex( index, iindex,id) _CHAR_setItemIndex( __FILE__, __LINE__, index, iindex, id)
INLINE int _CHAR_setItemIndex( char *file, int line, int index ,int iindex,int id );
#define		CHAR_getPoolItemIndex( index, iindex) _CHAR_getPoolItemIndex( __FILE__, __LINE__, index, iindex)
INLINE int _CHAR_getPoolItemIndex( char *file, int line, int index, int iindex);
#define		CHAR_setPoolItemIndex( index, iindex,id) _CHAR_setPoolItemIndex( __FILE__, __LINE__, index, iindex, id)
INLINE int _CHAR_setPoolItemIndex( char *file, int line, int index ,int iindex,int id );
INLINE BOOL CHAR_setAddressbookEntry( int index , int aindex , ADDRESSBOOK_entry *a );
INLINE ADDRESSBOOK_entry* CHAR_getAddressbookEntry( int index , int aindex);
INLINE Char* CHAR_getCharPointer( int index );
INLINE int CHAR_getCharNum( void );
INLINE int CHAR_getPlayerMaxNum( void );
INLINE int CHAR_getPetMaxNum( void );
INLINE int CHAR_getOthersMaxNum( void );
INLINE BOOL CHAR_getCharUse( int index );
INLINE CHAR_HaveSkill* CHAR_getCharHaveSkill( int index,int sindex );
INLINE int  CHAR_getCharHaveTitle( int index,int tindex );
INLINE int CHAR_setCharHaveTitle( int charaindex,int tindex, int new );
INLINE int CHAR_getCharPet( int charaindex,int petindex );
INLINE int CHAR_setCharPet( int charaindex,int petindex, int new );
INLINE int CHAR_getCharPoolPet( int charaindex,int petindex );
INLINE int CHAR_setCharPoolPet( int charaindex,int petindex, int new );
int CHAR_getCharPetElement( int charaindex );
int CHAR_getCharPoolPetElement( int charaindex );
int CHAR_getCharPoolItemIndexElement( int charaindex );
int CHAR_getEmptyCharPoolItemIndexNum( int charaindex);
#define		CHAR_getIntPSkill( index, skillti, ti) _CHAR_getIntPSkill( __FILE__, __LINE__, index, skillti, ti)
INLINE int _CHAR_getIntPSkill( char *file, int line, int index, int skillti, SKILL_DATAINT ti);

#define		CHAR_setIntPSkill( index, skillti, ti, data) _CHAR_setIntPSkill( __FILE__, __LINE__, index, skillti, ti, data)
INLINE void _CHAR_setIntPSkill( char *file, int line, int index, int skillti, SKILL_DATAINT ti, int data);


extern Char *CHAR_chara;


INLINE int CHAR_getCharMakeSequenceNumber( int charaindex );
void CHAR_constructFunctable( int charaindex );
void* CHAR_getFunctionPointer( int charaindex, int functype );
BOOL CHAR_initCharArray( int pnum,  int petnum,int onum );
BOOL CHAR_endCharArray( void );
int CHAR_initCharOneArray( Char* ch );
void CHAR_endCharOneArray( int index );
void CHAR_removeHaveItem( Char* ch );
void CHAR_endCharData( Char* ch );
char* CHAR_makeStringFromCharData( Char* one );
char* CHAR_makeStringFromCharIndex( int index );
BOOL CHAR_makeCharFromStringToArg( char* data, Char* one);
int CHAR_makePetFromStringToArg( char *src, Char *ch, int ti);
char *CHAR_makePetStringFromPetIndex( int petindex);
#define CHAR_getPetSkillElement( petindex) _CHAR_getPetSkillElement( __FILE__, __LINE__, petindex)
int _CHAR_getPetSkillElement( char *file, int line, int petindex );
#define	CHAR_setPetSkill( petindex, havepetskillindex, n) _CHAR_setPetSkill( __FILE__, __LINE__, petindex, havepetskillindex, n)
INLINE int _CHAR_setPetSkill( char *file, int line, int petindex,int havepetskillindex, int new );
#define		CHAR_getPetSkill( petindex, havepetskillindex) _CHAR_getPetSkill( __FILE__, __LINE__, petindex, havepetskillindex)
INLINE int _CHAR_getPetSkill( char *file, int line, int petindex,int havepetskillindex );
#define SETFLG(a,b,c,d,e,f,g,h) (((a)<<0)+((b)<<1)+((c)<<2)+((d)<<3)+((e)<<4)+((f)<<5)+((g)<<6)+((h)<<7))

INLINE int CHAR_AddMaxExp( int charaindex, int addexp);
INLINE int CHAR_setMaxExpFromLevel( int charaindex, int level);
INLINE int CHAR_setMaxExp( int charaindex, unsigned long int Setexp);
#ifdef _NEWOPEN_MAXEXP
INLINE int CHAR_ChangeExp( int charaindex );
INLINE int CHAR_HandleExp( int charaindex );

#endif

// ride Pet table
typedef struct tagtagRidePetTable
{
	int rideNo;
	int charNo;
	int petNo;
	int petId;
	
} tagRidePetTable;


// CoolFish: Family Member Kind 2001/8/28
typedef enum
{
	FMMEMBER_NONE = -1,
	FMMEMBER_MEMBER = 1,
	FMMEMBER_APPLY,
	FMMEMBER_LEADER,
	FMMEMBER_ELDER,
} CHAR_FM_MEMBERKIND;

// shan 2002/01/10
typedef enum
{
	PET_STAT_NONE = 0,
	PET_STAT_SELECT,		
	PET_STAT_MAIL = 4,	
} CHAR_PET_STAT;


#ifdef _PERSONAL_FAME	// Arminius: �����������
void CHAR_earnFame(int index, int fame);
#endif

// CoolFish: 2001/10/11
#ifdef _UNIQUE_P_I
void CHAR_setPetUniCode(int petindex);
void ITEM_setItemUniCode(int itemindex);
void Check_P_I_UniCode(int charindex);
#endif

// Arminius 12.15 �ж��Ƿ�����ʿ
int IsMale(int charindex);

// Arminius 12.15 �ж��Ƿ���Ůʿ
int IsFemale(int charindex);

#ifdef _NEW_RIDEPETS
#define RIDE_PET0	1
#define RIDE_PET1	( 1 << 1 )
#define RIDE_PET2	( 1 << 2 )
#define RIDE_PET3	( 1 << 3 )
#define RIDE_PET4	( 1 << 4 )
#define RIDE_PET5	( 1 << 5 )
#define RIDE_PET6	( 1 << 6 )
#define RIDE_PET7	( 1 << 7 )
#define RIDE_PET8	( 1 << 8 )
#define RIDE_PET9	( 1 << 9 )
#define RIDE_PET10	( 1 << 11 )
#define RIDE_PET11	( 1 << 10 )
#define RIDE_PET12	( 1 << 12 )
#define RIDE_PET13	( 1 << 13 )
#define RIDE_PET14	( 1 << 14 )
#ifdef _RIDE_CF
#define RIDE_PET15 ( 1 << 15 )
#define RIDE_PET16 ( 1 << 16 )
#define RIDE_PET17 ( 1 << 17 )
#define RIDE_PET19 ( 1 << 19 )
#define RIDE_PET18 ( 1 << 18 )
#define RIDE_PET20 ( 1 << 20 )
#define RIDE_PET21 ( 1 << 21 )
#define RIDE_PET22 ( 1 << 22 )
#define RIDE_PET23 ( 1 << 23 )
#define RIDE_PET24 ( 1 << 24 )
#define RIDE_PET25 ( 1 << 25 )
#define RIDE_PET26 ( 1 << 26 )
#define RIDE_PET27 ( 1 << 27 )
#define RIDE_PET28 ( 1 << 28 )
#define RIDE_PET29 ( 1 << 29 )
#define RIDE_PET30 ( 1 << 30 )
#define RIDE_PET31 ( 1 << 31 )
#define RIDE_TPYE1 1
#define RIDE_TPYE2 ( 1 << 1 )
#define RIDE_PET_ALL	( 0xFFFFFFFF )
#define RIDE_TPYE_ALL	( 0xFFFFFFFF )
#define MAXNOINDEX 32*2
#else
#define RIDE_PET_ALL	( RIDE_PET0|RIDE_PET1|RIDE_PET2|RIDE_PET3|RIDE_PET4|RIDE_PET5|RIDE_PET6|RIDE_PET7|RIDE_PET8|RIDE_PET9|RIDE_PET10|RIDE_PET11 )
#define MAXNOINDEX 15
#endif


#ifdef _PET_BEATITUDE
#define BEATITUDE_VITAL ( 1 << 0 )
#define BEATITUDE_STR   ( 1 << 1 )
#define BEATITUDE_TOUGH ( 1 << 2 )
#define BEATITUDE_DEX   ( 1 << 3 )
#endif

#ifdef _FM_LEADER_RIDE
typedef struct	{
	int fmfloor;
	int ride[3];
}tagFmLeaderRide;
#endif

#ifdef _FM_LEADER_RIDE
int CHAR_FmLeaderRide(int meindex,int pet);
#endif

typedef struct	{
	int petNo;
#ifdef _RIDE_CF	
	int petNo1;
#endif
	unsigned int learnCode;
}tagRideCodeMode;

typedef struct	{
	int RideNo[MAXNOINDEX];
	int flg;
}tagRideNoList;

typedef struct	{
	int charNo;
	int Noindex;
	int sex;
}tagRidePetList;
int RIDEPET_getNOindex( int baseNo);
#ifdef _RIDE_CF	
int RIDEPET_getPETindex( int PetNo, unsigned int learnCode, int unsigned learnCode1 );
int RIDEPET_getPETindex64( int PetNo, unsigned int learnCode, unsigned int learnCode1 );
#else
int RIDEPET_getPETindex( int PetNo, unsigned int learnCode);
#endif
int RIDEPET_getRIDEno( int index, int ti);
#endif
#ifdef _TYPE_TOXICATION
BOOL CHAR_CanCureFlg( int charaindex, char *arg);
#endif

BOOL CHAR_getCharOnArrayPercentage( int mode, int *max, int *min, int *cnt);


#define	CHAR_DelItemMess( index, ti, flg) _CHAR_DelItem( __FILE__, __LINE__, index, ti, 1, flg)
#define	CHAR_DelItem( index, ti) _CHAR_DelItem( __FILE__, __LINE__, index, ti, 1, 1)
#define	CHAR_DelPileItemMess( index, ti, num, flg) _CHAR_DelItem( __FILE__, __LINE__, index, ti, num, flg)

#define	CHAR_AddPileItem( index, itemindex) _CHAR_AddPileItem( __FILE__, __LINE__, index, itemindex)
INLINE int _CHAR_AddPileItem( char *file, int line, int charaindex, int itemindex);

INLINE int _CHAR_DelItem( char *file, int line, int charaindex, int ti, int num, int flg);

#define	CHAR_AddGold( index, ti) _CHAR_AddGold( __FILE__, __LINE__, index, ti)
INLINE int _CHAR_AddGold( char *file, int line, int charaindex, int gold);
#define	CHAR_DelGold( index, ti) _CHAR_DelGold( __FILE__, __LINE__, index, ti)
INLINE int _CHAR_DelGold( char *file, int line, int charaindex, int gold);
int CHAR_getMaxHaveGold( int charaindex);

#ifdef _NPC_SEPARATEDATA
void CHAR_showTempInt( int index);
BOOL CHAR_InitCharTempNum( Char* ch );
void CHAR_EndCharTempNum( void);
void CHAR_getDefaultCharTempNum( Char* ch );
#endif

#ifdef _RIDE_CF
int CHAR_Ride_CF_init();
#endif

void LodBadPetString( char *data, char *err, int ti);


#endif
