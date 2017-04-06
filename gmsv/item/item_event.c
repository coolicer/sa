#include "version.h"
#include <string.h> /* strstr */
#include <ctype.h>  /* isdigit, isalnum */
#include "common.h"
#include "char.h"
#include "char_base.h"
#include "char_data.h"
#include "configfile.h"
#include "item.h"
#include "item_event.h"
#include "object.h"
#include "anim_tbl.h"
#include "magic.h"
#include "handletime.h"
#include "log.h"
#include "util.h"
#include "encount.h"
#include "battle.h"
#include "battle_item.h"
#include "lssproto_serv.h"
#include "net.h"
#include "pet.h"
#include "npcutil.h"
#include "enemy.h"
#ifdef _Item_ReLifeAct
#include "battle_magic.h"
#endif
#ifdef _ITEM_WARP_FIX_BI
extern	tagRidePetTable	ridePetTable[296];
#endif
#include "pet_skill.h"

#ifdef _CFREE_petskill
#include "npc_freepetskillshop.h"
#endif

int ITEM_TimeDelCheck( int itemindex )
{
	int icnt, jcnt, playernum;
	playernum = CHAR_getPlayerMaxNum();
	for( icnt = 0; icnt < playernum; icnt ++ ) {
		if( CHAR_CHECKINDEX( icnt ) ) {
			for( jcnt = 0; jcnt < CHAR_MAXITEMHAVE; jcnt ++ ) {
				if( CHAR_getItemIndex( icnt, jcnt ) == itemindex ) {
					print( "����Ч��ʱ���ѵ���������ֵ��ߴ���(%s)(%s)\n",
						CHAR_getUseName(icnt),ITEM_getAppropriateName(itemindex) );
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

int ITEM_eventDrop( int itemindex, int charaindex, int itemcharaindex )
{
	typedef void (*DROPF)(int,int);
	char szBuffer[256]="";
	DROPF dropfunc=NULL;

    if( ITEM_CHECKINDEX(itemindex) == FALSE )return -1;

	dropfunc=(DROPF)ITEM_getFunctionPointer( itemindex, ITEM_DROPFUNC );
	if( dropfunc ){
		dropfunc( charaindex, itemindex );
	}

    if( ITEM_getInt( itemindex, ITEM_VANISHATDROP) != 1 )return 0;
	snprintf( szBuffer, sizeof( szBuffer), "%s �����ˡ�",
		ITEM_getAppropriateName(itemindex) );
	CHAR_talkToCli( charaindex, -1, szBuffer, CHAR_COLORWHITE );

	{
		LogItem(
			CHAR_getChar( charaindex, CHAR_NAME ), /* ƽ�ҷ�   */
			CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
			itemindex,
#else
       		ITEM_getInt( itemindex, ITEM_ID ),  /* ʧ��  ة  į */
#endif
			"Drop&Delete(��������ʧ)",
			CHAR_getInt( charaindex,CHAR_FLOOR),
			CHAR_getInt( charaindex,CHAR_X ),
 	      	CHAR_getInt( charaindex,CHAR_Y ),
            ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
		);
	}
	CHAR_setItemIndex( charaindex, itemcharaindex, -1);
	ITEM_endExistItemsOne( itemindex );
	return 1;
}

#undef  UNDEF
#define UNDEF (-1)

typedef struct {
    char* cmd;        /* ġ    ٯ   */
    char*   onmessage;    /*  ����������  ٯ      */
    char*   offmessage;    /*  ����������  ٯ      */
    int   element;    /* ���ޱ�ئ��ۢ�� */
    int   maxElement; /* elementƥ϶�ý�ľ��ۢ�ټ�    ��ë�ֹ�ۢ�� */
} ITEM_EFFECTPARAM;
static ITEM_EFFECTPARAM ITEM_restorableParam[] = {
    {"hp", "HP�ظ��ˡ�", "HP�����ˡ�",CHAR_HP, CHAR_WORKMAXHP},
    {"mp", "MP�ظ��ˡ�", "MP�����ˡ�",CHAR_MP, CHAR_WORKMAXMP},
};
static ITEM_EFFECTPARAM ITEM_statusParam[] = {
    {"po", CHAR_POISONSTRING, CHAR_RECOVERPOISONSTRING,
     CHAR_POISON,    UNDEF},

    {"pa", CHAR_PARALYSISSTRING, CHAR_RECOVERPARALYSISSTRING,
     CHAR_PARALYSIS, UNDEF},

    {"si", CHAR_SILENCESTRING, CHAR_RECOVERSILENCESTRING,
     CHAR_SLEEP,UNDEF},

    {"st", CHAR_STONESTRING, CHAR_RECOVERSTONESTRING, CHAR_STONE, UNDEF},

    {"da", CHAR_DARKNESSSTRING, CHAR_RECOVERDARKNESSSTRING,
     CHAR_DRUNK,UNDEF},

    {"co", CHAR_CONFUSIONSTRING,CHAR_RECOVERCONFUSIONSTRING,
     CHAR_CONFUSION, UNDEF},
};

/*#define LOCAL_DEBUG*/
#ifdef LOCAL_DEBUG
#define DOUTFILE  "doutfile"
#include <stdio.h>
#include <stdarg.h>
static int eprintf(char* format, ...){
    va_list arg;
    long len;
    FILE* fp;
    fp = fopen(DOUTFILE, "a");
    va_start(arg, format);
    len = vfprintf(stderr, format, arg);
    if(fp){ vfprintf(fp, format, arg); fclose(fp); }
    va_end(arg);
    return len;
}

#define fprint eprintf
#endif

static BOOL ITEM_isValidEffect(char* cmd, int value){
    int i;

    for(i=0; i<arraysizeof(ITEM_restorableParam); i++){
        if(! strcmp(cmd, ITEM_restorableParam[i].cmd)){
            return value <= 0 ? FALSE : TRUE;
        }
    }
    if(value < 0 ) return FALSE;
    for(i=0; i<arraysizeof(ITEM_statusParam); i++){
        if(! strcmp(cmd, ITEM_statusParam[i].cmd)){
            return TRUE;
        }
    }
    return FALSE;
}

#define ID_BUF_LEN_MAX  20
#define SEPARATORI       '|'
BOOL ITEM_MedicineInit(ITEM_Item* itm)
{
    char cmd[ID_BUF_LEN_MAX], arg[ID_BUF_LEN_MAX];
    int value;
    int effectCount = 0;
    char* p, * q;
    char* effectarg;

    effectarg = itm->string[ITEM_ARGUMENT].string;
    if(* effectarg == '\0') return TRUE;
    for(p=effectarg; *p != '\0'; ){
        int i;
        if(*p == SEPARATORI) p++;
        for(q=cmd, i=0; isalnum(*p) && i<ID_BUF_LEN_MAX; i++){
            *q++ = *p++;
        }
        *q = '\0';
        if(q == cmd || *p != SEPARATORI){
            fprint("ITEM_medicineInit: error(c)? invalid:%s(%d)\n",
                   effectarg, effectCount);
            return FALSE;
        }
        p++;
        for(q=arg, i=0; isdigit(*p) && i<ID_BUF_LEN_MAX; i++){
            *q++ = *p++;
        }
        *q = '\0';
        value = strtol(arg, & q, 10); /* strtol()��OK? */
        if(ITEM_isValidEffect(cmd, value)){
            effectCount++;
        }else{
            fprint("ITEM_medicineInit: error(v)? invalid:%s(%d)\n",
                   effectarg, effectCount);
            return FALSE;
        }
    }
    return (effectCount == 0) ? FALSE : TRUE;
}

static BOOL ITEM_medicineRaiseEffect(int charaindex, char* cmd,int value)
{
    int i;
    char ansmsg[256];
    for( i=0 ; i<arraysizeof(ITEM_restorableParam); i++){
        if( ! strcmp(cmd, ITEM_restorableParam[i].cmd) ){
#if 1
            int maxv,curv,amount;
            char*   onoroff=NULL;
            maxv=CHAR_getWorkInt( charaindex, ITEM_restorableParam[i].maxElement);
            curv=CHAR_getInt(charaindex,ITEM_restorableParam[i].element);
            amount = value;
            if( curv + amount < 0 ) amount = -curv;
            CHAR_setInt(charaindex,ITEM_restorableParam[i].element, min((curv+amount),maxv));
            if( amount > 0 )
                onoroff=ITEM_restorableParam[i].onmessage;
            else if( amount < 0 )
                onoroff=ITEM_restorableParam[i].offmessage;
            else
                return FALSE;
            strcpysafe( ansmsg, sizeof(ansmsg), onoroff );
            CHAR_talkToCli( charaindex, -1, ansmsg, CHAR_COLORWHITE);
            return TRUE;
#else
            if(value == ITEM_EFFECT_RESTORE_NORMAL){
                int maxv, curv, amount;
                maxv =CHAR_getWorkInt(charaindex, ITEM_restorableParam[i].maxElement);
                curv =CHAR_getInt(charaindex, ITEM_restorableParam[i].element);
                amount = 30;
                if(curv + amount < 0) amount = -curv;
                CHAR_setInt(charaindex, ITEM_restorableParam[i].element, min((curv+amount), maxv));
                snprintf( ansmsg, sizeof(ansmsg), "%s�ظ��ˡ�",
                          ITEM_restorableParam[i].onmessage );
                CHAR_talkToCli( charaindex, -1, ansmsg, CHAR_COLORWHITE);
                return TRUE;
            }else if(value == ITEM_EFFECT_RESTORE_COMPLETE){
                int maxv;
                maxv =CHAR_getWorkInt(charaindex, ITEM_restorableParam[i].maxElement);
                CHAR_setInt(charaindex, ITEM_restorableParam[i].element, maxv);
                snprintf( ansmsg, sizeof(ansmsg), "%s����ȫ�ظ���", ITEM_restorableParam[i].onmessage );
                CHAR_talkToCli( charaindex, -1, ansmsg, CHAR_COLORWHITE );
                return TRUE;
            }else
                return FALSE;
#endif

        }
    }
    if( value >= 0 ){
        int found = 0;
        for(i=0; i<arraysizeof(ITEM_statusParam); i++){
            if(! strcmp(cmd, ITEM_statusParam[i].cmd)){
                ansmsg[0] = '\0';
                if( value && CHAR_getInt(charaindex,ITEM_statusParam[i].element) < value ){
                    CHAR_setInt(charaindex, ITEM_statusParam[i].element, value);
                    strcpysafe( ansmsg, sizeof(ansmsg), ITEM_statusParam[i].onmessage );
                }else{
                    if( CHAR_getInt(charaindex, ITEM_statusParam[i].element ) ){ 
						strcpysafe( ansmsg, sizeof(ansmsg), ITEM_statusParam[i].offmessage );
                    }
                    CHAR_setInt(charaindex, ITEM_statusParam[i].element, 0 );
                }
                found = 1;
                if( ansmsg[0] != '\0' )
                    CHAR_talkToCli( charaindex,-1,ansmsg,CHAR_COLORWHITE);
            }
        }
        if(found){
            CHAR_sendCToArroundCharacter( CHAR_getWorkInt(charaindex,CHAR_WORKOBJINDEX) );
            return TRUE;
        }
    }
#undef ITEM_STATUSCHANGEVALUE
    strcpysafe( ansmsg, sizeof(ansmsg),"ʲ��Ҳû������");
    CHAR_talkToCli( charaindex, -1, ansmsg, CHAR_COLORWHITE );
    return FALSE;
}

void ITEM_MedicineUsed(int charaindex, int to_charaindex, int itemindex)
{
    int itemid;
    int usedf = 0;
    char cmd[ID_BUF_LEN_MAX], arg[ID_BUF_LEN_MAX];
    int value;
    char* p, * q;
    char* effectarg;
    char ansmsg[256];
    itemid = CHAR_getItemIndex(charaindex, itemindex);
    if(!ITEM_CHECKINDEX(itemid)) return;
    effectarg = ITEM_getChar(itemid, ITEM_ARGUMENT);
    snprintf( ansmsg, sizeof(ansmsg), "ץ����%s ��", ITEM_getChar(itemid, ITEM_NAME) );
    CHAR_talkToCli( charaindex, -1, ansmsg, CHAR_COLORWHITE );
    for( p=effectarg ; *p != '\0'; ){
        int i;
        if(*p == SEPARATORI) p++;
        for(q=cmd, i=0; isalnum(*p) && i<ID_BUF_LEN_MAX; i++){
            *q++ = *p++;
        }
        *q = '\0';
        if(q == cmd || *p != SEPARATORI) return;
        p++;
        for(q=arg, i=0; isdigit(*p) && i<ID_BUF_LEN_MAX; i++){
            *q++ = *p++;
        }
        *q = '\0';
        value = strtol(arg, & q, 10);
        if(ITEM_medicineRaiseEffect(charaindex, cmd, value)){
            usedf = 1;
        }
    }
    if(usedf){
		CHAR_DelItem( charaindex, itemindex);
        CHAR_sendStatusString(charaindex, "P");

    }else
        fprint("ITEM_medicineUsed: error? cannot be used.\n");
#undef ID_BUF_LEN_MAX
#undef SEPARATORI
}

void ITEM_SandClockDetach( int charaindex , int itemid )
{
    int     i;
    if( !ITEM_CHECKINDEX( itemid )) return;
    for( i=0 ; i<CHAR_MAXITEMHAVE ; i++ ){
        if( CHAR_getItemIndex(charaindex,i ) == itemid ){
			CHAR_DelItem( charaindex, i);
            CHAR_talkToCli( charaindex, -1, "һж��ɳ©������Ȼ���ˣ�", CHAR_COLORWHITE );
            print( "deleted sand clock!\n" );
            break;
        }
    }
}

void ITEM_SandClockLogin( int charaindex )
{
	int i;
	int dTime;

	for( i=0 ; i<CHAR_MAXITEMHAVE; i++ ){
		int itemindex = CHAR_getItemIndex(charaindex,i);
		if( ITEM_getInt( itemindex, ITEM_ID ) != 29 )continue;
		if( ITEM_getInt( itemindex, ITEM_VAR4 ) == 0 )continue;
		dTime = NowTime.tv_sec - ITEM_getInt( itemindex, ITEM_VAR4 );
		ITEM_setInt( itemindex, ITEM_VAR3, ITEM_getInt( itemindex, ITEM_VAR3 ) + dTime );
	}

}

void ITEM_SandClockLogout( int charaindex )
{
	int i;
	for( i=0 ; i<CHAR_MAXITEMHAVE; i++ ){
		int itemindex = CHAR_getItemIndex(charaindex,i);
		if( ITEM_getInt( itemindex, ITEM_ID ) != 29 )continue;
		ITEM_setInt( itemindex, ITEM_VAR4, NowTime.tv_sec );
	}
}

// Arminius 7.2: Ra's amulet , remove "static"
/*static*/
BOOL ITEM_getArgument( char* argument , char* entryname, char* buf , int buflen )
{
    int     i;
    char    dividedbypipeline[512];
    for( i=1;  ; i++ ){
        BOOL   ret;
		ret = getStringFromIndexWithDelim( argument, "|", i, dividedbypipeline,
                                           sizeof(dividedbypipeline) );
        if( ret == TRUE ){
            int     tworet=1;
            char    first[512];
            tworet &= getStringFromIndexWithDelim( dividedbypipeline, ":", 1, first,sizeof(first) );
            tworet &= getStringFromIndexWithDelim( dividedbypipeline, ":", 2, buf,buflen );
            if( tworet != 0 )
                if( strcasecmp( first, entryname ) == 0 )
                    return TRUE;
        }else
            break;
    }

    return FALSE;
}

void ITEM_addTitleAttach( int charaindex, int itemindex )
{
    char    titlenumstring[256];
    int     titleindex;

    if( ITEM_CHECKINDEX(itemindex) == FALSE )return;

    if( ITEM_getArgument( ITEM_getChar(itemindex,ITEM_ARGUMENT),"addt",
                          titlenumstring, sizeof( titlenumstring) ) == FALSE ){
        print( "Can't find \"addt\" entry: %s\n",
               ITEM_getChar(itemindex,ITEM_ARGUMENT));
        return;
    }
    titleindex = atoi( titlenumstring );
    TITLE_addtitle( charaindex, titleindex );
    CHAR_sendStatusString( charaindex, "T" );
}

void ITEM_delTitleDetach( int charaindex, int itemindex )
{
    char    titlenumstring[256];
    int     titleindex;

    if( ITEM_CHECKINDEX(itemindex) == FALSE )return;

    if( ITEM_getArgument( ITEM_getChar(itemindex,ITEM_ARGUMENT),"delt",
                          titlenumstring, sizeof( titlenumstring) ) == FALSE ){
        print( "Can't find \"delt\" entry: %s\n",
               ITEM_getChar(itemindex,ITEM_ARGUMENT));
        return;
    }
    titleindex = atoi( titlenumstring );
    TITLE_deltitle( charaindex, titleindex );
    CHAR_sendStatusString( charaindex, "T" );
}

void ITEM_DeleteByWatched(
	int myobjindex, int moveobjindex, CHAR_ACTION act,
    int x, int y, int dir, int* opt, int optlen )
{

	int		itemindex, moveindex;
	char szBuffer[256]="";
	itemindex = OBJECT_getIndex(myobjindex);
    if( !ITEM_CHECKINDEX( itemindex )) return;
	if( OBJECT_getType( moveobjindex ) == OBJTYPE_CHARA ){
		moveindex = OBJECT_getIndex(moveobjindex);
    	if( CHAR_getInt( moveindex , CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
    		snprintf( szBuffer, sizeof( szBuffer ), "%s",
    							ITEM_getAppropriateName(itemindex) );
            CHAR_talkToCli( moveindex	, -1, "%s �����ˡ�", CHAR_COLORWHITE );
    	}
    }

	ITEM_endExistItemsOne(itemindex);
	CHAR_ObjectDelete(myobjindex);

}

void ITEM_DeleteTimeWatched(
	int objindex, int moveobjindex, CHAR_ACTION act,
    int x, int y, int dir, int* opt, int optlen)
{
	int	itemindex;
	int itemputtime;

	if( !CHECKOBJECTUSE(objindex) ){
		return;
	}
	itemindex = OBJECT_getIndex(objindex);
	if(!ITEM_CHECKINDEX(itemindex)){
		return;
	}
	itemputtime=ITEM_getInt(itemindex,ITEM_PUTTIME);
	if( !ITEM_CHECKINDEX( itemindex )) return;
	if( (int)NowTime.tv_sec > (int)( itemputtime + getItemdeletetime() ) ) {
		if( ITEM_TimeDelCheck( itemindex ) == FALSE ){
			return ;
		}
		{
			LogItem(
					"NULL",
					"NULL",
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
					itemindex,
#else
		       		ITEM_getInt( itemindex, ITEM_ID ),
#endif
					"TiemDelete",
					OBJECT_getFloor( objindex ),
					OBJECT_getX( objindex ),
					OBJECT_getY( objindex ),
					ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
				);
		}
		ITEM_endExistItemsOne(itemindex);
		CHAR_ObjectDelete(objindex);
	}
}

void ITEM_useEffectTohelos( int charaindex, int to_charaindex, int haveitemindex)
{
	char	buf[64];
	char	msgbuf[64];
	int		ret;
	int		itemindex;
	int		cutrate, limitcount;
    int		per;
    int		sendcharaindex = charaindex;

    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

    CHAR_setItemIndex(charaindex, haveitemindex ,-1);
    CHAR_sendItemDataOne( charaindex, haveitemindex);
	ret = getStringFromIndexWithDelim( ITEM_getChar(itemindex, ITEM_ARGUMENT) ,
										"|", 1, buf, sizeof( buf));
	if( ret != TRUE ) {
		{
			LogItem(
				CHAR_getChar( charaindex, CHAR_NAME ),
				CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
				itemindex,
#else
       			ITEM_getInt( itemindex, ITEM_ID ),
#endif
				"FieldErrorUse",
		       	CHAR_getInt( charaindex,CHAR_FLOOR),
				CHAR_getInt( charaindex,CHAR_X ),
        		CHAR_getInt( charaindex,CHAR_Y ),
				ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
			);
		}
	    ITEM_endExistItemsOne( itemindex);
		return;
	}
	cutrate = atoi( buf);
	if( cutrate < 0 ) cutrate = 0;
	ret = getStringFromIndexWithDelim( ITEM_getChar(itemindex, ITEM_ARGUMENT) ,
										"|", 2, buf, sizeof( buf));
	if( ret != TRUE ) {
		{
			LogItem(
				CHAR_getChar( charaindex, CHAR_NAME ),
				CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
				itemindex,
#else
       			ITEM_getInt( itemindex, ITEM_ID ),
#endif
				"FieldUse",
		       	CHAR_getInt( charaindex,CHAR_FLOOR),
				CHAR_getInt( charaindex,CHAR_X ),
        		CHAR_getInt( charaindex,CHAR_Y ),
				ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
			);
		}
	    ITEM_endExistItemsOne( itemindex);
		return;
	}
	limitcount = atoi( buf);
	if( limitcount < 0) limitcount = 0;
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT) {
		sendcharaindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1);
	}
	CHAR_setWorkInt( sendcharaindex, CHAR_WORK_TOHELOS_CUTRATE, cutrate);
	CHAR_setWorkInt( sendcharaindex, CHAR_WORK_TOHELOS_COUNT, limitcount);

    snprintf( msgbuf, sizeof(msgbuf),
              "ץ����%s ��", ITEM_getChar(itemindex, ITEM_NAME) );
    CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORWHITE );

    if( sendcharaindex != charaindex ) {
	    snprintf( msgbuf, sizeof(msgbuf),
	              "%s ץ���� %s�� ",
	              CHAR_getChar( charaindex, CHAR_NAME),
	              ITEM_getChar( itemindex, ITEM_NAME) );
	    CHAR_talkToCli( sendcharaindex, -1, msgbuf, CHAR_COLORWHITE );
	}

	{
		LogItem(
			CHAR_getChar( charaindex, CHAR_NAME ), 
			CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
			itemindex,
#else
       		ITEM_getInt( itemindex, ITEM_ID ),
#endif
			"FieldUse",
	       	CHAR_getInt( charaindex,CHAR_FLOOR),
			CHAR_getInt( charaindex,CHAR_X ),
        	CHAR_getInt( charaindex,CHAR_Y ),
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
		);
	}
	ITEM_endExistItemsOne( itemindex);
	per = ENCOUNT_getEncountPercentMin( sendcharaindex,
									CHAR_getInt( sendcharaindex, CHAR_FLOOR),
									CHAR_getInt( sendcharaindex, CHAR_X),
									CHAR_getInt( sendcharaindex, CHAR_Y));
	if( per != -1) {
		CHAR_setWorkInt( sendcharaindex, CHAR_WORKENCOUNTPROBABILITY_MIN, per);
	}
	per = ENCOUNT_getEncountPercentMax( sendcharaindex,
									CHAR_getInt( sendcharaindex, CHAR_FLOOR),
									CHAR_getInt( sendcharaindex, CHAR_X),
									CHAR_getInt( sendcharaindex, CHAR_Y));
	if( per != -1) {
		CHAR_setWorkInt( sendcharaindex, CHAR_WORKENCOUNTPROBABILITY_MAX, per);
	}
	CHAR_sendStatusString( sendcharaindex, "E" );
}

void ITEM_dropMic( int charaindex , int itemindex )
{
    if( !ITEM_CHECKINDEX( itemindex )) return;

	CHAR_setWorkInt( charaindex, CHAR_WORKFLG,
		CHAR_getWorkInt( charaindex, CHAR_WORKFLG ) & ~WORKFLG_MICMODE );
}

void ITEM_useMic_Field( int charaindex, int to_charaindex, int haveitemindex )
{
	if( CHAR_getWorkInt( charaindex, CHAR_WORKFLG ) & WORKFLG_MICMODE ){
		CHAR_setWorkInt( charaindex, CHAR_WORKFLG,
			CHAR_getWorkInt( charaindex, CHAR_WORKFLG ) & ~WORKFLG_MICMODE );
		CHAR_talkToCli( charaindex, -1, "����˷��趨ΪOFF��", CHAR_COLORWHITE);
	}else{
		CHAR_setWorkInt( charaindex, CHAR_WORKFLG,
			CHAR_getWorkInt( charaindex, CHAR_WORKFLG ) | WORKFLG_MICMODE );
		CHAR_talkToCli( charaindex, -1, "����˷��趨ΪON��", CHAR_COLORWHITE);
	}
}
#if 1
char *aszHealStringByOwn[] = {
	"%s���;����ظ�%d",
	"%s�������ظ�%d",
	"%s����������%d",
	"%s���ҳ϶�����%d",
	""
};

char *aszDownStringByOwn[] = {
	"%s���;�������%d",
	"%s����������%d",
	"%s�������½�%d",
	"%s���ҳ϶��½�%d",
	""
};

char *aszHealStringByOther[] = {
	"����%s%s���;����ظ�%d",
	"����%s%s�������ظ�%d",
	"����%s%s����������%d",
	"����%s%s���ҳ϶�����%d",
	""
};

char *aszDownStringByOther[] = {
	"����%s%s���;�������%d",
	"����%s%s����������%d",
	"����%s%s����������%d",
	"����%s%s���ҳ϶ȼ���%d",
	""
};

char *aszKeyString[] = {"��", "��", "��", "��", ""};
int  aHealInt[] = { CHAR_HP, CHAR_MP, CHAR_CHARM, CHAR_VARIABLEAI, -1 };
int  aHealMaxWork[] = { CHAR_WORKMAXHP, CHAR_WORKMAXMP, -1, -1,  -1 };

void ITEM_useRecovery_Field(
	int charaindex,
	int toindex,
	int haveitemindex
){
	int work, workmax, workmin;
	int power[BD_KIND_END] = {0,0,0},
		prevhp = 0,
		workhp = 0,
		recovery[BD_KIND_END] = {0,0,0};
	int itemindex, kind = BD_KIND_HP, HealFlg = 0, j;
	char *p = NULL, *arg, msgbuf[256];
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if( !ITEM_CHECKINDEX(itemindex) ) return;
	if( CHAR_CHECKINDEX( toindex ) == FALSE )return ;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( arg == "\0" ) return;

#ifdef _ITEM_UNBECOMEPIG
    if( (p = strstr( arg, "����" )) != NULL ){
        if( CHAR_getInt( toindex, CHAR_BECOMEPIG ) > -1 ){
		    CHAR_setInt( toindex, CHAR_BECOMEPIG, -1 );
			CHAR_complianceParameter( toindex );
		    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
		    CHAR_send_P_StatusString( toindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
		    CHAR_talkToCli( toindex,-1,"������ʧЧ�ˡ�",CHAR_COLORWHITE);
       		CHAR_DelItemMess( charaindex, haveitemindex, 0);
		}
		return;
	}
#endif
#ifdef _ITEM_LVUPUP
	if( (p = strstr( arg, "LVUPUP" ) ) != NULL ){
		if (CHAR_getInt(toindex, CHAR_WHICHTYPE)==CHAR_TYPEPET){
		    int pidx=0,lvup=0,lv=0;
			sprintf( msgbuf, "%s", p+7 );
            pidx = atoi( strtok( msgbuf, " " ) );
			if( pidx != CHAR_getInt( toindex, CHAR_PETID) || CHAR_getInt(toindex, CHAR_LIMITLEVEL) == 0 ){
			    CHAR_talkToCli( charaindex,-1,"�޷�ʹ��",CHAR_COLORWHITE);
				return;
			}
			lv = CHAR_getInt( toindex, CHAR_LV );
			if( lv < 10 || lv >= 140 ){
			    CHAR_talkToCli( charaindex,-1,"Ŀǰ�ȼ��޷�ʹ��",CHAR_COLORWHITE);
				return;
			}
			if( (p = strstr( arg, "��" )) != NULL )
					lvup=2;
		    if( (p = strstr( arg, "ˮ" )) != NULL )
					lvup=3;
            if( (p = strstr( arg, "��" )) != NULL )
					lvup=0;
			if( (p = strstr( arg, "��" )) != NULL )
					lvup=1;		    
			if( lv%4 != lvup || CHAR_getInt(toindex, CHAR_LIMITLEVEL)-lv >= 1 ){//
			    CHAR_talkToCli( charaindex,-1,"�Ƴ���ӡʧ��",CHAR_COLORWHITE);
			    CHAR_DelItemMess( charaindex, haveitemindex, 0);
				return;
			}
			CHAR_setInt(toindex, CHAR_LIMITLEVEL, CHAR_getInt(toindex, CHAR_LIMITLEVEL)+1);	
			CHAR_talkToCli( charaindex,-1,"��ӡħ���������ߵȼ�����",CHAR_COLORWHITE);    
			if( CHAR_getInt(toindex, CHAR_LIMITLEVEL) == 140 ){
				CHAR_setInt(toindex, CHAR_LIMITLEVEL, 0);
			    CHAR_talkToCli( charaindex,-1,"��ӡħ������",CHAR_COLORWHITE);    	
			}
			CHAR_DelItemMess( charaindex, haveitemindex, 0);
			CHAR_complianceParameter( toindex );
		    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
		}
		return;
	}
	if( (p = strstr( arg, "UPUPLV" ) ) != NULL ){
		if (CHAR_getInt(toindex, CHAR_WHICHTYPE)==CHAR_TYPEPET){
		    int pidx=0,lv=0;
			sprintf( msgbuf, "%s", p+7 );
			pidx = atoi( strtok( msgbuf, " " ) );
			if( pidx != CHAR_getInt( toindex, CHAR_PETID) || CHAR_getInt(toindex, CHAR_LIMITLEVEL) == 0 ){
			    CHAR_talkToCli( charaindex,-1,"�޷�ʹ��",CHAR_COLORWHITE);
				return;
			}
			lv = CHAR_getInt( toindex, CHAR_LV );
			if( lv < 125 || lv > 140 ){
			    CHAR_talkToCli( charaindex,-1,"Ŀǰ�ȼ��޷�ʹ��",CHAR_COLORWHITE);
				return;
			}
			CHAR_setInt(toindex, CHAR_LIMITLEVEL, 0);
			CHAR_talkToCli( charaindex,-1,"��ӡħ������",CHAR_COLORWHITE);    	
			CHAR_DelItemMess( charaindex, haveitemindex, 0);
			CHAR_complianceParameter( toindex );
		    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
		}
		return;
	}
#endif
#ifdef _ITEM_PROPERTY
    if( (p = strstr( arg, "PROPERTY" ) ) != NULL ){
		//print("��ˮ���:%s", arg );
		if (CHAR_getInt(toindex, CHAR_WHICHTYPE)!=CHAR_TYPEPLAYER)
            return;
		if( (p = strstr( arg, "+" ) ) != NULL ){//��ת���� ��ת
	        if( CHAR_getInt( toindex, CHAR_EARTHAT ) == 100 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_WATERAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_FIREAT ) == 100 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 );
			else if( CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 && CHAR_getInt( toindex, CHAR_WATERAT ) > 0 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WATERAT ) > 0 && CHAR_getInt( toindex, CHAR_FIREAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_FIREAT ) > 0 && CHAR_getInt( toindex, CHAR_WINDAT ) > 0 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) > 0 && CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 );
		}
        if( (p = strstr( arg, "-" ) ) != NULL ){//��ת���� ��ת
			if( CHAR_getInt( toindex, CHAR_EARTHAT ) == 100 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_WATERAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_FIREAT ) == 100 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 && CHAR_getInt( toindex, CHAR_WATERAT ) > 0 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 );
            else if( CHAR_getInt( toindex, CHAR_WATERAT ) > 0 && CHAR_getInt( toindex, CHAR_FIREAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 );
		    else if( CHAR_getInt( toindex, CHAR_FIREAT ) > 0 && CHAR_getInt( toindex, CHAR_WINDAT ) > 0 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) > 0 && CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 );
		}
		CHAR_DelItemMess( charaindex, haveitemindex, 0);
		CHAR_complianceParameter( toindex );
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
		CHAR_send_P_StatusString( toindex ,
					CHAR_P_STRING_EARTH	|
					CHAR_P_STRING_WATER	|
					CHAR_P_STRING_FIRE	|
					CHAR_P_STRING_WIND	
				);
		return;
	}
#endif
#ifdef _ITEM_ADDPETEXP
	if( (p = strstr( arg, "GETEXP" )) != NULL ){
		if (CHAR_getInt(toindex, CHAR_WHICHTYPE)==CHAR_TYPEPET){
			getStringFromIndexWithDelim( arg, "|", 2, msgbuf,sizeof( msgbuf));//������
			if( atoi(msgbuf) == CHAR_getInt( toindex, CHAR_PETID) ){
				getStringFromIndexWithDelim( arg, "|", 3, msgbuf,sizeof( msgbuf));//�ȼ�����(�ȼ������ſ���)
				if( CHAR_getInt( toindex, CHAR_LV ) >= atoi(msgbuf) ){
					getStringFromIndexWithDelim( arg, "|", 4, msgbuf,sizeof( msgbuf));//����ֵ����
					if( CHAR_getInt( toindex, CHAR_LV) < CHAR_MAXUPLEVEL ){
						int UpLevel = 0;
						CHAR_setWorkInt( toindex, CHAR_WORKGETEXP, atoi(msgbuf) );//�ش�CHAR_WORKGETEXP
						CHAR_AddMaxExp( toindex, CHAR_getWorkInt( toindex, CHAR_WORKGETEXP ) );
						sprintf( msgbuf,"��ʯͷ���治֪����ʲ�����Ŀ����ҿ�����(���Ӿ���%d)", CHAR_getWorkInt( toindex, CHAR_WORKGETEXP ) );
						CHAR_talkToCli( charaindex,-1,msgbuf,CHAR_COLORWHITE);  
						UpLevel = CHAR_LevelUpCheck( toindex , charaindex);
						if( UpLevel > 0 ){
							if( getBattleDebugMsg( ) != 0 ){
								snprintf( msgbuf, sizeof(msgbuf),
											"(%s) ������ %d",
											CHAR_getUseName( toindex ),
											CHAR_getInt( toindex, CHAR_LV ) );
							BATTLE_talkToCli( charaindex, msgbuf, CHAR_COLORYELLOW );
							}
						}
						for( j = 0; j < UpLevel; j ++ ){
							CHAR_PetLevelUp( toindex );
							CHAR_PetAddVariableAi( toindex, AI_FIX_PETLEVELUP );
						}
						CHAR_complianceParameter( toindex );
					}
					else{
						CHAR_talkToCli( charaindex,-1,"����ʹ��",CHAR_COLORWHITE);  
					}
					/*CHAR_send_P_StatusString(  charindex, CHAR_P_STRING_DUELPOINT|
					CHAR_P_STRING_TRANSMIGRATION| CHAR_P_STRING_RIDEPET|
					CHAR_P_STRING_BASEBASEIMAGENUMBER| CHAR_P_STRING_GOLD|
					CHAR_P_STRING_EXP| CHAR_P_STRING_LV| CHAR_P_STRING_HP|CHAR_P_STRING_LEARNRIDE);
				*/
				}
				else
					CHAR_talkToCli( charaindex,-1,"��ʯͷ....(�޷��б��޷����Ӿ���)",CHAR_COLORWHITE);  
			}
			CHAR_DelItemMess( charaindex, haveitemindex, 0);
			CHAR_complianceParameter( toindex );
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
			return;
		}
		else{
			CHAR_talkToCli( charaindex,-1,"����ʹ��",CHAR_COLORWHITE);  
		}
	}
#endif
	for( j = 0; j < BD_KIND_END; j ++ ){
		power[j] = 0;
		recovery[j] = 0;
	}
	if( (p = strstr( arg, "ȫ" )) != NULL ){
		HealFlg = ( 1 << BD_KIND_HP );
		if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET ){
		}else{
			HealFlg |= ( 1 << BD_KIND_MP );
		}
		power[BD_KIND_HP] = 10000000;
		power[BD_KIND_MP] = 100;
	}
	kind = BD_KIND_HP;
	if( (p = strstr( arg, aszKeyString[kind] )) != NULL ){
		HealFlg |= ( 1 << kind );
		if( sscanf( p+2, "%d", &work ) != 1 ){
			power[kind] = 1;
		}else{
			power[kind] = RAND( (work*0.9), (work*1.1) );
		}
#ifndef _MAGIC_REHPAI
		power[kind] *= GetRecoveryRate( toindex );
#endif
	}
	kind = BD_KIND_MP;
	if( (p = strstr( arg, aszKeyString[kind] )) != NULL
	&&  CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER){
		HealFlg |= ( 1 << kind );
		if( sscanf( p+2, "%d", &work ) != 1 ){
			power[kind] = 1;
		}else{
			power[kind] = RAND( (work*0.9), (work*1.1) );
		}
	}
	kind = BD_KIND_AI;
	if( (p = strstr( arg, aszKeyString[kind] )) != NULL
		&& CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET){
		HealFlg |= ( 1 << kind );
		if( sscanf( p+2, "%d", &work ) != 1 ){
			power[kind] = 1;
		}else{
			power[kind] = RAND( (work*0.9), (work*1.1) );
		}
		power[kind] *= 100;
	}
	kind = BD_KIND_CHARM;
	if( (p = strstr( arg, aszKeyString[kind] )) != NULL
	&& CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER	){
		HealFlg |= ( 1 << kind );
		if( sscanf( p+2, "%d", &work ) != 1 ){
			power[kind] = 1;
		}else{
			power[kind] = RAND( (work*0.9), (work*1.1) );
		}
	}
	if( HealFlg == 0 )return;
#ifdef _TYPE_TOXICATION
	if( CHAR_CanCureFlg( toindex, "HP") == FALSE )return;
#endif

	for( j = 0; j < BD_KIND_END; j ++ ){
		if( ( HealFlg & ( 1 << j ) ) == 0 )continue;
		prevhp = CHAR_getInt( toindex, aHealInt[j] );
		workhp = prevhp + (int)power[j];

		if( j == BD_KIND_CHARM ){
			workmax = 100;
			workmin = 0;
		}else
		if( j == BD_KIND_AI ){
			workmax = 10000;
			workmin = -10000;
		}else{
			workmax = CHAR_getWorkInt( toindex, aHealMaxWork[j] );
			workmin = 1;
		}
		workhp = min( workhp, workmax );
		workhp = max( workhp, workmin );
		CHAR_setInt( toindex, aHealInt[j], workhp );
		recovery[j] = workhp - prevhp;
		if( j == BD_KIND_AI ){
			recovery[j] *= 0.01;
		}
	}
	CHAR_complianceParameter( toindex );

	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
		if( charaindex != toindex ) {
			CHAR_send_P_StatusString( toindex, CHAR_P_STRING_HP|CHAR_P_STRING_MP|CHAR_P_STRING_CHARM);
		}
	}
	CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_HP|CHAR_P_STRING_MP|CHAR_P_STRING_CHARM);
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
		CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE ){
		CHAR_PartyUpdate( toindex, CHAR_N_STRING_HP|CHAR_N_STRING_MP );
	}
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET ){
		int		i;
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
			int workindex = CHAR_getCharPet( charaindex, i );
			if( workindex == toindex ){
				CHAR_send_K_StatusString( charaindex, i, CHAR_K_STRING_HP|CHAR_K_STRING_AI);
			}
		}
	}


	for( j = 0; j < BD_KIND_END; j ++ ){
		if( ( HealFlg & ( 1 << j ) ) == 0 )continue;
		if( charaindex != toindex) {
			if( power[j] >= 0 ){
				snprintf( msgbuf, sizeof( msgbuf),
					aszHealStringByOwn[j],
					CHAR_getUseName( toindex ), recovery[j] );
			}else{
				snprintf( msgbuf, sizeof( msgbuf),
					aszDownStringByOwn[j],
					CHAR_getUseName( toindex ), -recovery[j] );
			}
			CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORWHITE);
			if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
				if( power[j] >= 0 ){
					snprintf( msgbuf, sizeof( msgbuf),
						aszHealStringByOther[j],
						CHAR_getUseName( charaindex ),
						CHAR_getUseName( toindex ), recovery[j] );
				}else{
					snprintf( msgbuf, sizeof( msgbuf),
						aszDownStringByOther[j],
						CHAR_getUseName( charaindex ),
						CHAR_getUseName( toindex ), -recovery[j] );
				}
				CHAR_talkToCli( toindex, -1, msgbuf, CHAR_COLORWHITE);
			}
		}else {
			if( power[j] >= 0 ){
				snprintf( msgbuf, sizeof( msgbuf),
					aszHealStringByOwn[j],
					CHAR_getUseName( charaindex ), recovery[j] );
			}else{
				snprintf( msgbuf, sizeof( msgbuf),
					aszDownStringByOwn[j],
					CHAR_getUseName( charaindex ), -recovery[j] );
			}
			CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORWHITE);
		}
	}
	{
		LogItem(
			CHAR_getChar( charaindex, CHAR_NAME ),
			CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
			itemindex,
#else
       		ITEM_getInt( itemindex, ITEM_ID ),
#endif
			"FieldUse",
	       	CHAR_getInt( charaindex,CHAR_FLOOR),
			CHAR_getInt( charaindex,CHAR_X ),
        	CHAR_getInt( charaindex,CHAR_Y ),
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
		);
	}
	CHAR_DelItemMess( charaindex, haveitemindex, 0);

}

#endif

void ITEM_useRecovery( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
		if( battlemode ){
			ITEM_useRecovery_Battle( charaindex, toindex, haveitemindex );
		}else{
			ITEM_useRecovery_Field(	charaindex, toindex, haveitemindex );
		}

}

#ifdef _ITEM_MAGICRECOVERY
void ITEM_useMRecovery( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
		if( battlemode ){
			ITEM_useMRecovery_Battle( charaindex, toindex, haveitemindex );
		}else{
//			ITEM_useRecovery_Field(	charaindex, toindex, haveitemindex );
		}

}

#endif

#ifdef _ITEM_USEMAGIC
void ITEM_useMagic( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
		if( battlemode ){
			ITEM_useMagic_Battle( charaindex, toindex, haveitemindex );
		}
}
#endif

#ifdef _PET_LIMITLEVEL
void ITEM_useOtherEditBase( int charaindex, int toindex, int haveitemindex)
{
	int itemindex,i;
	int work[4];
	int num=-1,type;
	int LevelUpPoint,petrank;
	char buf1[256];
	char buf2[][32]={"�����ɳ���","�;����ɳ���","�ٶȳɳ���","�����ɳ���","����"};
	char buf3[][32]={"������","��Ϊ���","��Ϊ����"};
	if( !CHAR_CHECKINDEX( charaindex ) )  return;
	if( !CHAR_CHECKINDEX( toindex ) )  return; 
#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
		itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		if( !ITEM_CHECKINDEX(itemindex) )  return;
		if( CHAR_getInt( toindex, CHAR_PETID) == 718 
#ifdef _PET_2LIMITLEVEL
			|| CHAR_getInt( toindex, CHAR_PETID) == 401 
#endif
			)	{

			int maxnums=50;

			if( CHAR_getInt( toindex,CHAR_LV ) < 74 )	{
				sprintf(buf1,"���ҵ��𣿺�����������ร����������˱仯��");
				CHAR_talkToCli( charaindex, toindex, buf1, CHAR_COLORWHITE);
				num = ITEM_MODIFYATTACK;
				LevelUpPoint = CHAR_getInt( toindex, CHAR_ALLOCPOINT );
				petrank = CHAR_getInt( toindex, CHAR_PETRANK );
				work[3] =(( LevelUpPoint >> 24 ) & 0xFF);
				work[0] = (( LevelUpPoint >> 16 ) & 0xFF);
				work[1] = (( LevelUpPoint >> 8 ) & 0xFF);
				work[2] = (( LevelUpPoint >> 0 ) & 0xFF);
				for( i=0; i<4; i++)	{
					type = ITEM_getInt( itemindex, (num + i));
					work[i] += type;
					strcpy( buf1,"\0");
					if( work[i] > maxnums )	{
						sprintf(buf1,"%s �Ѿ��ﵽ����ˡ�", buf2[i]);
						work[i] = maxnums;
					}else if( work[i] < 0 )	{
						sprintf(buf1,"%s �Ѿ�Ϊ���ˡ�", buf2[i]);
						work[i] = 0;
					}else	{
						if( type > 0 )	{
							if( type > 2 )
								sprintf(buf1,"%s %s %s", buf2[i], buf3[0], "��");
							else
								sprintf(buf1,"%s %s %s", buf2[i], buf3[1], "��");
						}else if( type < 0 ){
							sprintf(buf1,"%s %s %s", buf2[i], buf3[2], "��");	
						}
					}
					if( strcmp( buf1, "\0"))	{
						CHAR_talkToCli( charaindex, toindex, buf1, CHAR_COLORWHITE);
					}
				}
				
				LevelUpPoint = ( work[3]<< 24) + ( work[0]<< 16) + ( work[1]<< 8) + ( work[2]<< 0);
				CHAR_setInt( toindex, CHAR_ALLOCPOINT, LevelUpPoint);
				CHAR_setInt( toindex, CHAR_PETRANK, petrank);
				LogPetPointChange(
					CHAR_getChar( charaindex, CHAR_NAME ),
					CHAR_getChar( charaindex, CHAR_CDKEY ),
					CHAR_getChar( charaindex, CHAR_NAME),
					toindex, 4,
					CHAR_getInt( toindex, CHAR_LV),
					"item_use",
					CHAR_getInt( charaindex, CHAR_FLOOR),
					CHAR_getInt( charaindex, CHAR_X ),
					CHAR_getInt( charaindex, CHAR_Y )
					);
			}
				
		}else	{
			sprintf(buf1,"������");
		}
		CHAR_DelItem( charaindex, haveitemindex);
		return;
}
#endif

void ITEM_useStatusChange( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ; //����  
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
	if( battlemode ){
		ITEM_useStatusChange_Battle( charaindex, toindex, haveitemindex );
	}else{
	}

}

void ITEM_useStatusRecovery( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ; //����  
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
	if( battlemode ){
		ITEM_useStatusRecovery_Battle( charaindex, toindex, haveitemindex );
	}else{
	}
}

void ITEM_useMagicDef( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
	if( battlemode ){
		ITEM_useMagicDef_Battle( charaindex, toindex, haveitemindex );
	}else{
	}

}

void ITEM_useParamChange( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
		if( battlemode ){
			ITEM_useParamChange_Battle( charaindex, toindex, haveitemindex );
		}
}

void ITEM_useFieldChange( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
		if( battlemode ){
			ITEM_useFieldChange_Battle( charaindex, toindex, haveitemindex );
		}
}

void ITEM_useAttReverse( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
	if( battlemode ){
		ITEM_useAttReverse_Battle( charaindex, toindex, haveitemindex );
	}else{
	}
}

void ITEM_useMic( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
		if( battlemode ){
		}else{
			ITEM_useMic_Field( charaindex, toindex, haveitemindex );
		}

}

void ITEM_useCaptureUp( int charaindex, int toindex, int haveitemindex)
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ; //����  

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
	}else
	if( battlemode ){
		ITEM_useCaptureUp_Battle( charaindex, toindex, haveitemindex );
	}else{
	}

}

#ifdef _PETSKILL_CANNEDFOOD
static void ITEM_usePetSkillCanned_PrintWindow( int charaindex, int flg)
{
	int fd;
	char	message[256], buf[2048];
	if( !CHAR_CHECKINDEX( charaindex )) return;
	fd = getfdFromCharaIndex( charaindex);
	if( fd == - 1 ) return;

	sprintf( message, "%d", flg);
	lssproto_WN_send( fd, WINDOWS_MESSAGETYPE_PETSKILLSHOW, 
					WINDOW_BUTTONTYPE_NONE,
					ITEM_WINDOWTYPE_SELECTPETSKILL_SELECT,
					-1,
					makeEscapeString( message, buf, sizeof( buf)));
}

void ITEM_usePetSkillCanned_WindowResult( int charaindex, int seqno, int select, char * data)
{
	int itemindex=-1, itemNo, petindex=-1, petNo;
	int SkillNo, SkillID;
	char buf1[256];
	char *skillarg=NULL;

	petNo = CHAR_getWorkInt( charaindex, CHAR_WORKRENAMEITEMNUM);
	itemNo = CHAR_getWorkInt( charaindex, CHAR_WORKRENAMEITEMINDEX);


	itemindex = CHAR_getItemIndex( charaindex, itemNo);
	if( !ITEM_CHECKINDEX( itemindex) )	return;
	petindex = CHAR_getCharPet( charaindex, petNo);
	if( !CHAR_CHECKINDEX(petindex) ) return;
	SkillNo = atoi( data);
	if( SkillNo < 0 || SkillNo >= CHAR_MAXPETSKILLHAVE ) return;

	skillarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	SkillID = atoi( skillarg);

#ifdef _CFREE_petskill
	if( NPC_CHECKFREEPETSKILL( charaindex, petindex, SkillID) == FALSE ){
		CHAR_talkToCli( charaindex, -1, "�ó����޷�ѧϰ����ܣ�", CHAR_COLORYELLOW);
		return;
	}
#endif

	CHAR_setPetSkill( petindex, SkillNo, SkillID);
	snprintf( buf1, sizeof( buf1 ), "W%d", petNo);
	CHAR_sendStatusString( charaindex, buf1 );
	CHAR_sendStatusString( charaindex, "P");
	{
		int skillarray = PETSKILL_getPetskillArray( SkillID);
		sprintf( buf1, "����%sѧϰ%s������ %s��ʧ�ˡ�",
			CHAR_getUseName( petindex), PETSKILL_getChar( skillarray, PETSKILL_NAME),
			ITEM_getChar( itemindex, ITEM_NAME) );
	}

	CHAR_talkToCli( charaindex, -1, buf1, CHAR_COLORYELLOW);
	CHAR_setItemIndex( charaindex, itemNo ,-1);
	CHAR_sendItemDataOne( charaindex, itemNo);
	ITEM_endExistItemsOne( itemindex);
}

#endif

static void ITEM_useRenameItem_PrintWindow( int charaindex, int page)
{
	int fd;
	int	pos = 0;
	int i;
	int btntype = WINDOW_BUTTONTYPE_CANCEL;
	char	message[1024];
	char	msgwk[1024];
	char	buf[2048];

	if( !CHAR_CHECKINDEX( charaindex )) return;
	if( page < 0 || page > 3 ) {
		print( "%s:%d err\n", __FILE__, __LINE__);
		return;
	}
	fd = getfdFromCharaIndex( charaindex);
	if( fd == - 1 ) return;
	
	snprintf( message, sizeof( message),
				"2\n   Ҫ����Ǹ���Ŀ��������\n"
				"                               Page:%d\n", page +1);
	
	for( i = page *5; i < page *5 +5; i ++ ) {
		int itemindex = CHAR_getItemIndex( charaindex, i);
		BOOL	flg = FALSE;
		while( 1 ) {
			char *cdkey;
			if( !ITEM_CHECKINDEX( itemindex)) break;
			cdkey = ITEM_getChar( itemindex, ITEM_CDKEY);
			if( !cdkey) {
				print( "%s:%d err\n", __FILE__, __LINE__);
				break;
			}
			if( ITEM_getInt( itemindex, ITEM_MERGEFLG) != 1) break;
			if( ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_DISH) break;
			if( ITEM_getInt( itemindex, ITEM_CRUSHLEVEL) != 0 ) break;
			if( strlen( cdkey) != 0 ) {
				if( strcmp( cdkey, CHAR_getChar( charaindex, CHAR_CDKEY)) != 0 ) {
					break;
				}
			}
			flg = TRUE;
			break;
		}
		if( flg ) {
			char *nm = ITEM_getChar( itemindex, ITEM_SECRETNAME);
			char wk[256];
			if( pos +strlen( nm) +1 > sizeof( msgwk)) {
				print( "buffer over error %s:%d\n", __FILE__, __LINE__);
				break;
			}
			snprintf( wk, sizeof( wk), "%s\n", nm);
			strncpy( &msgwk[pos],  wk, sizeof( msgwk) -pos -1);
			pos += strlen( wk);
		}
		else {
			if( pos +2 > sizeof( msgwk)) {
				print( "buffer over error %s:%d\n", __FILE__, __LINE__);
				break;
			}
			strncpy( &msgwk[pos], "\n", sizeof( msgwk) -pos -1 );
			pos += 1;
		}
	}
	strcat( message, msgwk);
	switch( page){
	  case 0:
	  	btntype |= WINDOW_BUTTONTYPE_NEXT;
	  	break;
	  case 3:
	  	btntype |= WINDOW_BUTTONTYPE_PREV;
	  	break;
	  case 1:
	  case 2:
	  	btntype |= WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_NEXT;
	  	break;
	}
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT, 
					btntype,
					CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE1+page,
					-1,
					makeEscapeString( message, buf, sizeof(buf)));
}

void ITEM_useRenameItem( int charaindex, int toindex, int haveitemindex)
{

	ITEM_useRenameItem_PrintWindow( charaindex, 0);

	CHAR_setWorkInt( charaindex, CHAR_WORKRENAMEITEMNUM, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKRENAMEITEMINDEX, haveitemindex);
/*
	char buf[256];
	int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX( itemindex) ) return;
	sprintf( buf, "%s�����ѱ�ȡ����", ITEM_getChar( itemindex, ITEM_NAME));
	CHAR_talkToCli( charaindex, -1, "�����ѱ�ȡ����", CHAR_COLORRED );

	{
		LogItem(
			CHAR_getChar( charaindex, CHAR_NAME ),
			CHAR_getChar( charaindex, CHAR_CDKEY ),
			itemindex,
			"ħ����DEL",
			CHAR_getInt( charaindex, CHAR_FLOOR),
			CHAR_getInt( charaindex, CHAR_X ),
 	      	CHAR_getInt( charaindex, CHAR_Y ),
	        ITEM_getChar( itemindex, ITEM_UNIQUECODE),
			ITEM_getChar( itemindex, ITEM_NAME),
			ITEM_getInt( itemindex, ITEM_ID)
		);
	}

	CHAR_setItemIndex( charaindex, haveitemindex, -1);
	CHAR_sendItemDataOne( charaindex, haveitemindex);
	ITEM_endExistItemsOne( itemindex );
*/
}

void ITEM_useRenameItem_WindowResult( int charaindex, int seqno, int select, char * data)
{
	int page = 0;
	int fd;
	if( select == WINDOW_BUTTONTYPE_CANCEL) return;

	fd = getfdFromCharaIndex( charaindex);
	if( fd == - 1 ) return;
	if( seqno != CHAR_WINDOWTYPE_SELECTRENAMEITEM_RENAME ) {
		if( select == WINDOW_BUTTONTYPE_NEXT ) page = 1;
		else if( select == WINDOW_BUTTONTYPE_PREV ) page = -1;
		if( select == WINDOW_BUTTONTYPE_NEXT || select == WINDOW_BUTTONTYPE_PREV ) {
			int winno = seqno - CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE1;
			winno += page;
			if( winno < 0 ) winno = 0;
			if( winno > 3 ) winno = 3;
			ITEM_useRenameItem_PrintWindow( charaindex, winno);
		}else {
			char	message[1024];
			char	buf[2048];
			char	haveitemindex = CHAR_getWorkInt( charaindex, CHAR_WORKRENAMEITEMNUM);
			int		itemindex;
			if( haveitemindex == -1 ) {
				haveitemindex = (seqno - CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE1) * 5 + 
								( atoi(data)-1);
				CHAR_setWorkInt( charaindex, CHAR_WORKRENAMEITEMNUM, haveitemindex);
			}
			itemindex = CHAR_getItemIndex( charaindex, haveitemindex);

			snprintf( message, sizeof( message),
							"%s ����Ҫ����\n"
							"����������\n"
							"ȫ��13����, ����26����",
							ITEM_getChar( itemindex, ITEM_NAME));
			
			
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT, 
							WINDOW_BUTTONTYPE_OKCANCEL,
							CHAR_WINDOWTYPE_SELECTRENAMEITEM_RENAME,
							-1,
							makeEscapeString( message, buf, sizeof(buf)));
			
		}
	}
	else {
		BOOL	flg = FALSE;
		char	message[1024];
		char	buf[2048];
		while( 1 ) {
			char	*p;
			if( strlen( data) > 26 || strlen( data) < 1) {
				if( strlen( data) > 26 ) {
					strcpy( message, "������������");
				}else {
					strcpy( message, "������һ����������");
				}
				break;
			}

            // WON ADD ����ħ���ʸ�������
			flg = TRUE;

			for( p = data; *p ; p ++) {
				if( *p == ' '){
					strcpy( message, "���ɿհ�");
					flg = FALSE;
					break;
				}
				if( strncmp( p, "��",2) == 0 ) {
					strcpy( message, "���ɿհ�");
					flg = FALSE;
					break;
				}
				if( *p == '|'){
					strcpy( message, "�������������");
					flg = FALSE;
					break;
				}
			}
	
			break;
		}


		if( !flg ) {
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
							WINDOW_BUTTONTYPE_OK,
							CHAR_WINDOWTYPE_SELECTRENAMEITEM_RENAME_ATTENTION,
							-1,
							makeEscapeString( message, buf, sizeof(buf)));
		}
		else {
			char	haveitemindex = CHAR_getWorkInt( charaindex, CHAR_WORKRENAMEITEMNUM);
			int		itemindex;
			int		renameitemindex;
			int		renameitemhaveindex;
			int		remain;
			char msgbuf[128];

			itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
			if( !ITEM_CHECKINDEX( itemindex)) {
				print( "%s %d err\n", __FILE__, __LINE__);
				return;
			}
			ITEM_setChar( itemindex, ITEM_SECRETNAME, data);
			ITEM_setChar( itemindex, ITEM_CDKEY, 
						CHAR_getChar( charaindex, CHAR_CDKEY));
			CHAR_sendItemDataOne( charaindex, haveitemindex);
			snprintf( msgbuf, sizeof(msgbuf),"�� %s ������ %s ", 
					ITEM_getChar( itemindex, ITEM_NAME), data);
		    CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORYELLOW);
			renameitemhaveindex = CHAR_getWorkInt( charaindex, CHAR_WORKRENAMEITEMINDEX);
			renameitemindex = CHAR_getItemIndex( charaindex, renameitemhaveindex);
			if( !ITEM_CHECKINDEX( renameitemindex)) {
				print( "%s %d err\n", __FILE__, __LINE__);
				return;
			}
			remain = atoi( ITEM_getChar( renameitemindex, ITEM_ARGUMENT));
			if( remain != 0 ) {
				remain --;
				if( remain <= 0 ) {
					snprintf( msgbuf, sizeof(msgbuf),"%s ��ʧ��", 
							ITEM_getChar( renameitemindex, ITEM_NAME));
				    CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORYELLOW);
				    CHAR_setItemIndex( charaindex, renameitemhaveindex, -1);
				    CHAR_sendItemDataOne( charaindex, renameitemhaveindex);
					ITEM_endExistItemsOne( renameitemindex );
				}else {
					char buf[32];
					snprintf( buf, sizeof( buf),"%d", remain);
					ITEM_setChar( renameitemindex, ITEM_ARGUMENT, buf);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------
//	���г�Ƿë  ���������ѣ�
//	  �������ݱ�ݷ¼�ĸة���  ��ë��̫��  ���  �  įë  �����£�
//-------------------------------------------------------------------------
void ITEM_dropDice( int charaindex, int itemindex)
{
	char *dicename[] = { "һ", "��",  "��", "��", "��", "��"};
	int  diceimagenumber[] = { 24298,24299,24300,24301,24302,24303};
	int r = RAND( 0,5);
	
	//   �  įë��  
	ITEM_setInt( itemindex, ITEM_VAR1, ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER));
	//   �  į��ޥ
	ITEM_setInt( itemindex, ITEM_BASEIMAGENUMBER, diceimagenumber[r]);
	//   ��ޥ
	ITEM_setChar( itemindex, ITEM_SECRETNAME, dicename[r]);
	
	// ���������ͷ���ʧ�����߼�˪����������ƥ�浤��ƥ�ݳ��ƥ��֧��ئ�У�
}
//-------------------------------------------------------------------------
//	���г�Ƿë  ���������ѣ�
//  �������ݱ��  ���  �  įë���  �ʣ�
//-------------------------------------------------------------------------
void ITEM_pickupDice( int charaindex, int itemindex)
{
	//   �  įë���  �ʣ�
	ITEM_setInt( itemindex,  ITEM_BASEIMAGENUMBER, ITEM_getInt( itemindex, ITEM_VAR1));
	//   ������  ��
	ITEM_setChar( itemindex, ITEM_SECRETNAME, ITEM_getChar( itemindex, ITEM_NAME));
}
enum {
	ITEM_LOTTERY_1ST,		// 1�
	ITEM_LOTTERY_2ND,
	ITEM_LOTTERY_3RD,
	ITEM_LOTTERY_4TH,
	ITEM_LOTTERY_5TH,		// 5�
	ITEM_LOTTERY_6TH,		// 6�
	ITEM_LOTTERY_NONE,		// ½ľ
	ITEM_LOTTERY_NUM,
};
//-------------------------------------------------------------------------
//	���������Ԫ������ľ���ݼ����ѣ�
//  ���ƥ��ٱ�ئ�¾�ë裻��£�
//  ��  ����Ʊ��ɧԻ��

#define PRE_6		(10000)
#define PRE_5		(1300 + PRE_6)
#define PRE_4		(600 + PRE_5)
#define PRE_3		(300 + PRE_4)
#define PRE_2		(8 + PRE_3)
#define PRE_1		(1 + PRE_2)


//#define PRE_5		(13000)
//#define PRE_4		(2300 + PRE_5)
//#define PRE_3		(540 + PRE_4)
//#define PRE_2		(8 + PRE_3)
//#define PRE_1		(1 + PRE_2)


//#define PRE_5		(16000)
//#define PRE_4		(2500 + PRE_5)
//#define PRE_3		(400 + PRE_4)
//#define PRE_2		(10 + PRE_3)
//#define PRE_1		(1 + PRE_2)
//#define PRE_5		(27000)
//#define PRE_4		(4000 + PRE_5)
//#define PRE_3		(400 + PRE_4)
//#define PRE_2		(10 + PRE_3)
//#define PRE_1		(1 + PRE_2)
//
//-------------------------------------------------------------------------
BOOL ITEM_initLottery(ITEM_Item* itm)
{
	int r = RAND( 0, 49999);
	int hit = ITEM_LOTTERY_NONE;		// 
	char result[7];		// ��  
	int countnum[6];
	int count;
	int i;
	int len;
	// ������Ȼ��¼�ƥ�۷���  �������羮����Իئ�ƽ�ľئ�з������
	if( itm->data[ITEM_VAR3] == 1 ) return TRUE;

	// �������£�
	if( r < PRE_6 ) hit = ITEM_LOTTERY_6TH;
	else if( r < PRE_5 ) hit = ITEM_LOTTERY_5TH;
	else if( r < PRE_4 ) hit = ITEM_LOTTERY_4TH;
	else if( r < PRE_3 ) hit = ITEM_LOTTERY_3RD;
	else if( r < PRE_2 ) hit = ITEM_LOTTERY_2ND;
	else if( r < PRE_1 ) hit = ITEM_LOTTERY_1ST;
	else hit = ITEM_LOTTERY_NONE;
	
	// ��  ë������£�
	count = 0;
	if( hit != ITEM_LOTTERY_NONE ) {
		// �Իë������
		result[0] = result[1] = result[2] = hit+1;
		count = 3;
		countnum[hit] = 3;
	}
	while( count < 6 ) {
		int r = RAND( ITEM_LOTTERY_1ST, ITEM_LOTTERY_6TH);
		if( countnum[r] >= 2 ) continue;
		// 2/3�����м���  ƥ��ְ��½ľ��  ë�����������֧�£�
		// ���������
		if(      ( hit != ITEM_LOTTERY_NONE && count == 3 )
		     ||  ( hit == ITEM_LOTTERY_NONE && count == 0 ) )
		{
			if( RAND( 0,2)) {
				result[count] = result[count+1] = r+1;
				countnum[r] += 2;
				count += 2;
				continue;
			}
		}
		countnum[r] ++;
		result[count] = r+1;
		count++;
	}
	// ��  ë�������׻����£�
	// �Ի���ݷ�2/3����  ƥ���Ի��ٯë��    Ƿ���������£�
	// ��ƽ��ƽ���ë�ѵ��У�
	len = sizeof( result)-2;
	if( hit != ITEM_LOTTERY_NONE ) {
		if( RAND( 0,2) ) {
			// �Ի��ٯë��    Ƿ���
			char s = result[0];
			result[0] = result[5];
			result[5] = s;
			len = sizeof( result)-3;
		}
	}
	for( i = 0; i < 10; i ++) {
		int x = RAND( 0, len);
		int y = RAND( 0, len);
		int s;
		s = result[x];
		result[x] = result[y];
		result[y] = s;
	}
	//   ���׻���  ٯ  ���б��ƻ����ʣ�
	result[sizeof(result)-1] = '\0';
	itm->data[ITEM_VAR1] = hit;
	// ����������������
	itm->data[ITEM_VAR2] = 0;
	itm->data[ITEM_VAR3] = 1;
	memcpy( itm->string[ITEM_ARGUMENT].string, result, sizeof( result));
	
	return TRUE;
}
//-------------------------------------------------------------------------
//	���������Ԫë�������ݼ����ѣ�
//  ������������پ���ʧ��  ة��  �����£�
//-------------------------------------------------------------------------
void ITEM_useLottery( int charaindex, int toindex, int haveitemindex)
{
	int i,j;
	int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	int count = ITEM_getInt( itemindex, ITEM_VAR2);
	int hit = ITEM_getInt( itemindex, ITEM_VAR1);
	char buff[1024];
	char num[6][3] = { {"��"},{"��"},{"��"},{"��"},{"��"}, {"��"}};
	char numbuff[128];
	char *n;
	int result;
	BOOL flg;
	if( !ITEM_CHECKINDEX( itemindex) ) return;
	if( count == 0 ) {
		ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "");
	}
	// ���  �����÷���½ľ�ƾ�ؤԻ  ئ�м�ƥ�ݳ��ƥʧ��  ةë����
	else if( count == 6 ) {
        CHAR_setItemIndex( charaindex , haveitemindex, -1 );
		CHAR_sendItemDataOne( charaindex, haveitemindex);
        ITEM_endExistItemsOne( itemindex );
		return;
	}
	n = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	// for debug
	result = (int)n[count]-1;
	// ������Ʃ����
	flg = FALSE;
	for( i = 0; i < count+1 && flg == FALSE; i ++ ) {
		for( j = i+1; j < count+1 && flg == FALSE; j ++ ) {
			if( i != j ) {
				if( n[i] == n[j] ) {
					flg = TRUE;
				}
			}
		}
	}
	memcpy( numbuff, ITEM_getChar( itemindex, ITEM_EFFECTSTRING), (count)*2);
	// ���  ٯ  �����  ë���ʣ�
	snprintf( buff, sizeof( buff), "%s%s", numbuff, num[result]);
	count ++;
	ITEM_setInt( itemindex, ITEM_VAR2, count);
	// ���  ƥ�Իئ���Իʧ��  ة��  �ʣ�½ľئ��½ľ��  ����
	// �ݼ�ĸƤ����������ƥ���ʣ�
	if( count >= 6 ) {
		// �Ի
		if( hit != ITEM_LOTTERY_NONE ) {
      		int newitemindex;
      		char strbuff[1024];
            char msgbuff[1024];
            // ���ƻ�
            CHAR_setItemIndex( charaindex , haveitemindex, -1 );
            ITEM_endExistItemsOne( itemindex );
			// 2729 - 2734 ��  ��Ԫ��1  6�
			newitemindex = ITEM_makeItemAndRegist( 2729 + hit);
            CHAR_setItemIndex( charaindex , haveitemindex, newitemindex );
			// ��  �������
			snprintf( strbuff, sizeof( strbuff), "%s                %s", buff, 
						ITEM_getChar( newitemindex, ITEM_EFFECTSTRING));
			ITEM_setChar( newitemindex, ITEM_EFFECTSTRING, strbuff);
			CHAR_sendItemDataOne( charaindex, haveitemindex);
            snprintf( msgbuff, sizeof( msgbuff), "���˵�%d��", hit+1 );
            CHAR_talkToCli( charaindex, -1,
                            msgbuff,
                            CHAR_COLORYELLOW );
		}
		// ½ľ
		else {
//            CHAR_setItemIndex( charaindex , haveitemindex, -1 );
			// ��  �������
      		char strbuff[1024];
			snprintf( strbuff, sizeof( strbuff), "%s                       û��,�´�����", buff);
			ITEM_setChar( itemindex, ITEM_EFFECTSTRING, strbuff);
			CHAR_sendItemDataOne( charaindex, haveitemindex);
//            ITEM_endExistItemsOne( itemindex );
//            CHAR_talkToCli( charaindex, -1,
//                            "����ľ��",
//                            CHAR_COLORWHITE );
		}
	}
	// ����Ԫ��  ���ݷ��ݷ�����ë˪Իئ���ʷ����
	else {
		char strbuff[1024];
		if( flg) {
			// ��ʸ������ң
			// ʧ��  ة��      ���춪�������ݨ����  ٯƥ��
			// ��ʸ����ë���Ȼ�ݱ���ƻ����£�
			int spc = 16 + ( 6-count)*2;
			char space[17];
			space[spc] = '\0';
			snprintf( strbuff, sizeof( strbuff), "%s%s������", buff, space);
		}
		else {
			strcpy( strbuff, buff);
		}
		ITEM_setChar( itemindex, ITEM_EFFECTSTRING, strbuff);
		CHAR_sendItemDataOne( charaindex, haveitemindex);
	}
}

void ITEM_WarpDelErrorItem( int charaindex )
{
	int j;
	for(j=0; j<CHAR_MAXITEMHAVE; j++){
		int itemindex=CHAR_getItemIndex(charaindex, j);
		if(ITEM_CHECKINDEX(itemindex)){
			int id=ITEM_getInt(itemindex, ITEM_ID);
			if(id==2609||id==2704){
				CHAR_setItemIndex(charaindex, j, -1);
				ITEM_endExistItemsOne(itemindex);
				CHAR_sendItemDataOne(charaindex, j);
			}
		}
	}
#ifdef _ITEM_WARP_FIX_BI
	if(!CHAR_getWorkInt( charaindex, CHAR_WORKITEMMETAMO))
		recoverbi(charaindex);
#endif
}

BOOL ITEM_WarpForAny(int charaindex, int haveitemindex, int ff, int fx, int fy,int flg)
{
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE ) != BATTLE_CHARMODE_NONE )
		return FALSE;
	if( CHAR_getInt( charaindex, CHAR_FLOOR) == 117 
		){
		CHAR_talkToCli(charaindex, -1, "�˴��޷�ʹ�á�", CHAR_COLORYELLOW);
		return FALSE;
	}
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER ){
		int i;
		if( flg == 0 )	{//����
			CHAR_talkToCli(charaindex, -1, "ֻ�ܵ���ʹ�á�", CHAR_COLORYELLOW);
			return FALSE;
		}
		for( i = 0; i < CHAR_PARTYMAX; i ++ ){
			int subindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1+i );
			if( CHAR_CHECKINDEX( subindex ) == FALSE ) continue;
			CHAR_talkToCli( subindex, -1, "ȫ��˲����У�����", CHAR_COLORWHITE);
			ITEM_WarpDelErrorItem( subindex );
			CHAR_warpToSpecificPoint( subindex, ff, fx, fy );
		}
	}else if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_CLIENT ){
		CHAR_talkToCli(charaindex, -1, "��Ա�޷�ʹ�á�", CHAR_COLORYELLOW);
		return FALSE;
	}else if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_NONE ){
		ITEM_WarpDelErrorItem( charaindex );
		CHAR_warpToSpecificPoint( charaindex, ff, fx, fy);
	}
	return TRUE;
}
//andy_end

// Robin 0523
void ITEM_useWarp( int charaindex, int toindex, int haveitemindex )
{
	char *arg;
	int itemindex, warp_t, warp_fl, warp_x, warp_y;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

#ifdef _ITEM_CHECKWARES
	if( CHAR_CheckInItemForWares( charaindex, 0) == FALSE ){
		CHAR_talkToCli(charaindex, -1, "Я�������޷�ʹ�á�", CHAR_COLORYELLOW);
		return;
	}
#endif
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( arg == "\0" )	return;
	if( sscanf( arg, "%d %d %d %d", &warp_t, &warp_fl, &warp_x, &warp_y) != 4 )
		return;
	if( ITEM_WarpForAny(charaindex, haveitemindex, warp_fl, warp_x, warp_y, warp_t) == FALSE )
		return;

	CHAR_DelItem( charaindex, haveitemindex);
	CHAR_sendStatusString(charaindex, "P");
}

#ifdef _USEWARP_FORNUM
void ITEM_useWarpForNum( int charaindex, int toindex, int haveitemindex )
{
	char *arg;
	int flg, ff, fx, fy, itemindex, usenum=0, i;
	int Mf, Mx, My;
	int MapPoint[12]={
		100, 200, 300, 400, 700,
		701, 702, 703, 704, 705,
		707, 708 };
		char MapString[12][256]={
			"������˹","��³��","��³��","ɳķ��","����԰��",
			"�����½����","���˹����","�����½�Ϸ�","����ŷ����","�����½����",
			"�����½����","����������"};
	
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX( itemindex) ) return;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( arg == "\0" )	return;

	if( sscanf( arg, "%d %d %d %d", &flg, &ff, &fx, &fy) != 4 )
		return;
	Mf = CHAR_getInt( charaindex, CHAR_FLOOR );
	Mx = CHAR_getInt( charaindex, CHAR_X );
	My = CHAR_getInt( charaindex, CHAR_Y );

	usenum = ITEM_getInt( itemindex, ITEM_DAMAGEBREAK);
	for( i=0; i<12; i++)	{
		if( Mf == MapPoint[i] ){
			break;
		}
	}
#ifdef _ITEM_CHECKWARES
	if( CHAR_CheckInItemForWares( charaindex, 0) == FALSE ){
		CHAR_talkToCli(charaindex, -1, "Я�������޷�ʹ�á�", CHAR_COLORYELLOW);
		return;
	}
#endif
	if( --usenum <= 0 )	{
		CHAR_DelItem( charaindex, haveitemindex);
		CHAR_sendStatusString(charaindex, "P");
	}else	{
		char buf[256];
		// WON ADD
		if( i >= 12 ){
			CHAR_talkToCli(charaindex, -1, "�˴��޷�ʹ�á�", CHAR_COLORYELLOW);
			return;
		}
		sprintf( buf, "%d %d %d %d", flg, Mf, Mx, My);
		ITEM_setChar(itemindex, ITEM_ARGUMENT, buf);
		ITEM_setInt( itemindex, ITEM_DAMAGEBREAK, usenum);
		sprintf( buf, "���м�¼��(%s,%d,%d)", MapString[i], Mx, My);
		ITEM_setChar( itemindex, ITEM_EFFECTSTRING, buf);
		CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORYELLOW);
		CHAR_sendItemDataOne( charaindex, haveitemindex);
	}

	// WON ADD
	if( ITEM_WarpForAny(charaindex, haveitemindex, ff, fx, fy, flg) == FALSE )
		return;
}
#endif

// Robin 0707 petFollow
void ITEM_petFollow( int charaindex, int toindex, int haveitemindex )
{
	char *arg;
	int itemindex, followLv, haveindex, i;
	//print(" PetFollow_toindex:%d ", toindex);
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPETFOLLOW ) != -1 ){
		if( CHAR_CHECKINDEX( CHAR_getWorkInt( charaindex, CHAR_WORKPETFOLLOW ) ) ){
			CHAR_talkToCli( charaindex, -1, "�����ջطų��ĳ��", CHAR_COLORWHITE );
			return;
		}
		CHAR_setWorkInt( charaindex, CHAR_WORKPETFOLLOW, -1);
	}
#ifdef _FIX_METAMORIDE
	if( CHAR_CHECKJOINENEMY( charaindex) == TRUE ){
		CHAR_talkToCli( charaindex, -1, "������޷�ʹ�ã�", CHAR_COLORWHITE );
		return;
	}
#endif

	if( CHAR_CHECKINDEX( toindex ) == FALSE )	return;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( arg == "\0" )return;

	if( sscanf( arg, "%d", &followLv) != 1 )
		return;
	if( CHAR_getInt( toindex, CHAR_LV ) > followLv ){
		CHAR_talkToCli( charaindex, -1, "���ߵĵȼ����㣡", CHAR_COLORWHITE );
		return;
	}
	if( CHAR_getWorkInt( toindex, CHAR_WORKFIXAI ) < 80 )
	{
		//CHAR_talkToCli( charaindex, -1, "������ҳ϶Ȳ��㣡", CHAR_COLORWHITE );
		//return;
	}	
	
	
	haveindex = -1;	
	for( i = 0; i < 5; i++ )
	{
		if( CHAR_getCharPet( charaindex, i) == toindex ) {
			haveindex = i;
			break;
		}
	}
	if( haveindex == -1) return;
	
	
	if( !PET_dropPetFollow( charaindex, haveindex, -1, -1, -1 ) ) {
		CHAR_talkToCli( charaindex, -1, "�������ʧ�ܣ�", CHAR_COLORWHITE );
		return;
	}

}

// Nuke start 0624: Hero's bless
void ITEM_useSkup( int charaindex, int toindex, int haveitemindex)
{
	int itemindex;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;	
	CHAR_setInt(charaindex,CHAR_SKILLUPPOINT,
	CHAR_getInt(charaindex,CHAR_SKILLUPPOINT)+1);
	CHAR_Skillupsend(charaindex);
	CHAR_talkToCli(charaindex, -1, "����ܵ��Լ��������������ˡ�", CHAR_COLORWHITE);

	CHAR_DelItem( charaindex, haveitemindex);
}
// Nuke end
extern void setNoenemy();
// Nuke start 0626: Dragon's bless
void ITEM_useNoenemy( int charaindex, int toindex, int haveitemindex)
{
	int itemindex,fd;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;	
	fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	setNoenemy(fd);
	CHAR_talkToCli(charaindex, -1, "����ܵ��ܱߵ�ɱ����ʧ�ˡ�", CHAR_COLORWHITE);
	CHAR_DelItem( charaindex, haveitemindex);

}
// Nuke end

// Arminius 7.2: Ra's amulet
void ITEM_equipNoenemy( int charaindex, int itemindex )
{
  char buf[4096];
  int evadelevel;
  int fl,fd;
  
  if( ITEM_CHECKINDEX(itemindex) == FALSE )return;

  if( ITEM_getArgument( ITEM_getChar(itemindex,ITEM_ARGUMENT),"noen", buf, sizeof(buf) )
      == FALSE ){
        return;
  }

  evadelevel=atoi(buf);
  fl=CHAR_getInt(charaindex, CHAR_FLOOR);
  fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
  if (evadelevel>=200) {
    setEqNoenemy(fd, 200);
    CHAR_talkToCli(charaindex, -1, "һ������Ĺ�â������������١�", CHAR_COLORWHITE);
    return;
  } else if (evadelevel>=120) {
    setEqNoenemy(fd, 120);

	if ( (fl==100)||(fl==200)||(fl==300)||(fl==400)||(fl==500) ){
	  CHAR_talkToCli(charaindex, -1, "һ������Ĺ�â������������١�", CHAR_COLORWHITE);
      return;
    }
  } else if (evadelevel>=80) {
    setEqNoenemy(fd, 80);
	if ( (fl==100)||(fl==200)||(fl==300)||(fl==400) ){
	
      CHAR_talkToCli(charaindex, -1, "һ������Ĺ�â������������١�", CHAR_COLORWHITE);
      return;
    }
  } else if (evadelevel>=40) {
    setEqNoenemy(fd, 40);
    if ( (fl==100)||(fl==200) ) {
      CHAR_talkToCli(charaindex, -1, "һ������Ĺ�â������������١�", CHAR_COLORWHITE);
      return;
    }
  }
  CHAR_talkToCli(charaindex, -1, "ʲ����Ҳû�з�����", CHAR_COLORWHITE);
}

#ifdef _Item_MoonAct
void ITEM_randEnemyEquipOne( int charaindex, int toindex, int haveitemindex)
{
	int itemindex, RandNum=0;
	char buf[256];

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX( itemindex)) return;

	if( ITEM_getArgument( ITEM_getChar( itemindex,ITEM_ARGUMENT), "rand", buf, sizeof(buf) ) == FALSE ){
		return;
	}

	if( (RandNum=atoi( buf)) > 0 ){
		int fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
		setEqRandenemy(fd, RandNum);
		CHAR_talkToCli(charaindex, -1, "�����ʽ����ˡ�", CHAR_COLORWHITE);
		sprintf( buf, "���� %s��ʧ�ˡ�", ITEM_getChar( itemindex, ITEM_NAME) );
		CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORYELLOW);
		CHAR_DelItem( charaindex, haveitemindex);
		return;
	}


}

void ITEM_randEnemyEquip( int charaindex, int itemindex)
{
  char buf[4096];
  int RandNum=0;
  int fd;
  
  if( ITEM_CHECKINDEX(itemindex) == FALSE )return;
  if( ITEM_getArgument( ITEM_getChar(itemindex,ITEM_ARGUMENT),"rand", buf, sizeof(buf) )
      == FALSE ){
        return;
  }

  RandNum=atoi( buf);
  fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
  if (RandNum > 0 ) {
    setEqRandenemy(fd, RandNum);
    CHAR_talkToCli(charaindex, -1, "�����ʽ����ˡ�", CHAR_COLORWHITE);
    return;
  }

}
void ITEM_RerandEnemyEquip( int charaindex, int itemindex)
{
  int RandNum=0;
  int fd;
  
  if( ITEM_CHECKINDEX(itemindex) == FALSE )return;

  fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
  RandNum = getEqRandenemy( fd);

  if (RandNum > 0 ) {
    clearEqRandenemy( fd);
    CHAR_talkToCli(charaindex, -1, "�����ʻظ���", CHAR_COLORWHITE);
    return;
  }
}
#endif

#ifdef _CHIKULA_STONE
void ITEM_ChikulaStone( int charaindex, int toindex, int haveitemindex)
{
	int itemindex,fd;
	char itemarg[256];

	if( !CHAR_CHECKINDEX( charaindex) ) return;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX( itemindex) ) return;

	fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	CHAR_setWorkInt( charaindex, CHAR_WORKCHIKULAHP, 0 );
	CHAR_setWorkInt( charaindex, CHAR_WORKCHIKULAMP, 0 );
	setChiStone( fd, 0);	//1hp 2mp

	if( ITEM_getArgument( ITEM_getChar( itemindex, ITEM_ARGUMENT),"hp", itemarg, sizeof(itemarg) ) != FALSE ){
		setChiStone( fd, 1);
		CHAR_setWorkInt( charaindex, CHAR_WORKCHIKULAHP, atoi( itemarg) );
	}else if( ITEM_getArgument( ITEM_getChar( itemindex, ITEM_ARGUMENT),"mp", itemarg, sizeof(itemarg) ) != FALSE ){
		setChiStone( fd, 2);
		CHAR_setWorkInt( charaindex, CHAR_WORKCHIKULAMP, atoi( itemarg) );
	}else{
	}

	CHAR_talkToCli(charaindex, -1, "�����������ף����", CHAR_COLORWHITE);
	CHAR_setItemIndex(charaindex, haveitemindex ,-1);
	CHAR_sendItemDataOne( charaindex, haveitemindex);
	ITEM_endExistItemsOne(itemindex);
}
#endif

#ifdef _ITEM_ORNAMENTS
void ITEM_PutOrnaments( int charaindex, int itemindex)
{
	char *arg=NULL;
	char itemname[256];
	int bbnums=0;
	if( !CHAR_CHECKINDEX( charaindex) ) return;
	if( !ITEM_CHECKINDEX( itemindex) ) return;
	arg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( arg == "\0" ) return;//ITEM_BASEIMAGENUMBER
	bbnums = atoi( arg);
	ITEM_setInt( itemindex, ITEM_BASEIMAGENUMBER, bbnums);
	ITEM_setWorkInt( itemindex, ITEM_CANPICKUP, 1);
	
	sprintf( itemname,"%s%s%s",CHAR_getChar( charaindex, CHAR_NAME), "��", ITEM_getChar( itemindex, ITEM_SECRETNAME));
	ITEM_setChar( itemindex, ITEM_SECRETNAME, itemname);
}
#endif

#ifdef _SUIT_ITEM

void ITEM_CheckSuitEquip( int charaindex)
{
	int i, j, itemindex, defCode=-1, same=0;
	int nItem[CHAR_STARTITEMARRAY];
	int maxitem;
	struct tagIntSuit{
		char fun[256];
		int intfun;	//CHAR_getInt
	};
	struct tagIntSuit ListSuit[]={
		{"VIT",CHAR_WORKSUITVIT},
		{"FSTR",CHAR_WORKSUITMODSTR},
		{"MSTR",CHAR_WORKSUITSTR},
		{"MTGH",CHAR_WORKSUITTGH},
		{"MDEX",CHAR_WORKSUITDEX},	
		{"HP", CHAR_WORKROUNDHP},
		{"MP", CHAR_WORKROUNDMP}
#ifdef _SUIT_ADDENDUM
	   ,{"RESIST",CHAR_WORKRESIST},   //�쳣������
		{"COUNTER",CHAR_WORKCOUNTER}, //������
		{"M_POW",CHAR_WORKMPOWER}      //��ǿ��ʦ��ħ��
#endif
#ifdef _SUIT_TWFWENDUM
	   ,{"EARTH",CHAR_WORK_EA},   //��
	   {"WRITER",CHAR_WORK_WR},   //ˮ
	   {"FIRE",CHAR_WORK_FI},   //��
	   {"WIND",CHAR_WORK_WI}     //��
#endif
#ifdef _SUIT_ADDPART3
	   ,{"WDUCKPOWER",CHAR_WORKDUCKPOWER},//��װ�ر�
	   {"RENOCASE",CHAR_WORKRENOCAST}, //��Ĭ������
	   {"SUITSTRP",CHAR_WORKSUITSTR_P},//������ ��λΪ%
	   {"SUITTGH_P",CHAR_WORKSUITTGH_P},//������ ��λΪ%
	   {"SUITDEXP",CHAR_WORKSUITDEX_P}//������ ��λΪ%
#endif
	};
	maxitem = sizeof(ListSuit)/sizeof(ListSuit[0]);
	CHAR_setWorkInt( charaindex, CHAR_WORKSUITITEM, 0);
	for( i=0; i<maxitem/*MAX_SUITTYPE*/; i++)	{
		CHAR_setWorkInt( charaindex, ListSuit[i].intfun, 0 );
	}
	j=0;
	for( i=0; i<CHAR_STARTITEMARRAY; i++){
		nItem[i] = -1;
		itemindex = CHAR_getItemIndex( charaindex ,i);
		if( !ITEM_CHECKINDEX( itemindex) ) continue;
		nItem[j++] = ITEM_getInt( itemindex, ITEM_SUITCODE);
	}
	for( i=0; i<j && defCode==-1; i++){
		int k;
		same = 0;
		if( nItem[i] <= 0 ) continue;
		for( k=(j-1); k>=0; k-- ){
			if( nItem[i] == nItem[k] ) same++;
		}
		if( same >= 3 && nItem[i] != 0 )defCode = nItem[i];
	}
	if( defCode == -1 ) return;
	CHAR_setWorkInt( charaindex, CHAR_WORKSUITITEM, defCode);
	for( i=0; i<CHAR_STARTITEMARRAY; i++){
		char *buf, buf1[256];
		itemindex = CHAR_getItemIndex( charaindex ,i);
		if( !ITEM_CHECKINDEX( itemindex) ) continue;
		if( ITEM_getInt( itemindex, ITEM_SUITCODE) == defCode ){
			for( j=0; j<maxitem/*MAX_SUITTYPE*/; j++){
				buf = ITEM_getChar( itemindex, ITEM_ARGUMENT);
				if( strstr( buf, ListSuit[j].fun) == NULL ) continue;
				if( NPC_Util_GetStrFromStrWithDelim( buf, ListSuit[j].fun, buf1, sizeof( buf1)) == NULL )continue;
				CHAR_setWorkInt( charaindex, ListSuit[j].intfun, atoi( buf1));
			}
		}
	}
	CHAR_complianceParameter( charaindex );
}

void ITEM_suitEquip( int charaindex, int itemindex)
{
	ITEM_CheckSuitEquip( charaindex);
}

void ITEM_ResuitEquip( int charaindex, int itemindex)
{
	ITEM_CheckSuitEquip( charaindex);
}
#endif//_SUIT_ITEM

void ITEM_remNoenemy( int charaindex, int itemindex )
{
  int fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
  int el=getEqNoenemy(fd);
  int fl=CHAR_getInt(charaindex, CHAR_FLOOR);

  if( ITEM_CHECKINDEX(itemindex) == FALSE )return;

  clearEqNoenemy(CHAR_getWorkInt(charaindex, CHAR_WORKFD));
  if (el>=200) {
    CHAR_talkToCli(charaindex, -1, "��������Ĺ�â��ʧ�ˡ�", CHAR_COLORWHITE);
    return;
  } else if (el>=120) {
    if ( (fl==100)||(fl==200)||(fl==300)||(fl==400)||(fl==500) ) {
      CHAR_talkToCli(charaindex, -1, "��������Ĺ�â��ʧ�ˡ�", CHAR_COLORWHITE);
      return;
    }
  } else if (el>=80) {
    if ( (fl==100)||(fl==200)||(fl==300)||(fl==400) ) {
      CHAR_talkToCli(charaindex, -1, "��������Ĺ�â��ʧ�ˡ�", CHAR_COLORWHITE);
      return;
    }
  } else if (el>=40) {
    if ( (fl==100)||(fl==200) ) {
      CHAR_talkToCli(charaindex, -1, "��������Ĺ�â��ʧ�ˡ�", CHAR_COLORWHITE);
      return;
    }
  }
  CHAR_talkToCli(charaindex, -1, "ʲ����Ҳû�з�����", CHAR_COLORWHITE);
}

extern void setStayEncount(int fd);
void ITEM_useEncounter( int charaindex, int toindex, int haveitemindex)
{
	int itemindex,fd;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;	

	fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	setStayEncount(fd);
#ifdef _USER_CHARLOOPS
	{
		Char 	*ch;
		ch  = CHAR_getCharPointer( charaindex);
		if( ch == NULL ) return;
		strcpysafe( ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
			sizeof( ch->charfunctable[CHAR_LOOPFUNCTEMP1]), "CHAR_BattleStayLoop");//ս��
		CHAR_setInt( charaindex, CHAR_LOOPINTERVAL, 2500);
		CHAR_constructFunctable( charaindex);
	}
#endif
	CHAR_talkToCli(charaindex, -1, "����ܵ��ܱ�ͻȻ������ɱ����", CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);

}

#ifdef _Item_DeathAct
void ITEM_UseDeathCounter( int charaindex, int toindex, int haveitemindex)
{
	int itemindex,fd;
	int itemmaxuse=-1;
	char buf1[256];
	char *itemarg;
	char itemnumstr[32];
	int  okfloor = 0;
	BOOL Useflag=FALSE;
	int i = 1;

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

#ifdef _ITEM_STONE
	itemarg = ITEM_getChar(itemindex,ITEM_ARGUMENT);
	while(1){
		if( getStringFromIndexWithDelim( itemarg, "|", i , itemnumstr, sizeof( itemnumstr)) == FALSE )
			break;
		okfloor = atoi(itemnumstr);
		if( CHAR_getInt( charaindex, CHAR_FLOOR ) == okfloor ){
			Useflag = TRUE;
			break;
		}
		i++;
	}
	if(okfloor != 0){   // ��ֵ�ʯͷ
		itemmaxuse = ITEM_getInt( itemindex, ITEM_DAMAGEBREAK);	
		if( itemmaxuse != -1 )	{
			itemmaxuse--;
			ITEM_setInt( itemindex, ITEM_DAMAGEBREAK, itemmaxuse);
			if( itemmaxuse < 1 )	{
				sprintf( buf1, "%s��ʧ�ˡ�", ITEM_getChar( itemindex, ITEM_NAME) );
				CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
				CHAR_DelItem( charaindex, haveitemindex);
				if(Useflag==FALSE) {
					CHAR_talkToCli(charaindex, -1, "û�з����κ����飡", CHAR_COLORYELLOW);
					return;
				}
			}else{
				sprintf( buf1, "ԭ�����У���ʹ�ô���ʣ��%d�Ρ�", itemmaxuse);
				ITEM_setChar( itemindex, ITEM_EFFECTSTRING, buf1);
				CHAR_sendItemDataOne( charaindex, haveitemindex);
				if(Useflag==FALSE) {
					CHAR_talkToCli(charaindex, -1, "û�з����κ����飡", CHAR_COLORYELLOW);
					return;
				}
			}
			fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
			setStayEncount(fd);
			CHAR_talkToCli(charaindex, -1, "���������������Ϣ��", CHAR_COLORYELLOW);
		}else{
			sprintf( buf1, "%s��ʧ�ˡ�", ITEM_getChar( itemindex, ITEM_NAME) );
			CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
			CHAR_DelItem( charaindex, haveitemindex);
			if(Useflag==FALSE) {
				CHAR_talkToCli(charaindex, -1, "û�з����κ����飡", CHAR_COLORYELLOW);
				return;
			}
			fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
			setStayEncount(fd);
			CHAR_talkToCli(charaindex, -1, "���������������Ϣ��", CHAR_COLORYELLOW);		
		}
	}else{		//��ħ��ʯ
#endif
#ifdef _ITEM_MAXUSERNUM
	itemmaxuse = ITEM_getInt( itemindex, ITEM_DAMAGEBREAK);
	if( itemmaxuse != -1 )	{
		itemmaxuse--;
		ITEM_setInt( itemindex, ITEM_DAMAGEBREAK, itemmaxuse);
		if( itemmaxuse < 1 )	{
			sprintf( buf1, "���� %s��ʧ�ˡ�", ITEM_getChar( itemindex, ITEM_NAME) );
			CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
			CHAR_DelItem( charaindex, haveitemindex);
		}else{
			sprintf( buf1, "ԭ�����У���ʹ�ô���ʣ��%d�Ρ�", itemmaxuse);
			ITEM_setChar( itemindex, ITEM_EFFECTSTRING, buf1);
			CHAR_sendItemDataOne( charaindex, haveitemindex);
		}
		fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
		setStayEncount(fd);
		CHAR_talkToCli(charaindex, -1, "����ܵ��ܱ�ͻȻ������ɱ����", CHAR_COLORYELLOW);
	}else{
		CHAR_DelItem( charaindex, haveitemindex);
		return;
	}
#else
	fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	setStayEncount(fd);
	sprintf( buf1, "���� %s��ʧ�ˡ�", ITEM_getChar( itemindex, ITEM_NAME) );
	CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);
#endif
#ifdef _ITEM_STONE
	}
#endif
#ifdef _USER_CHARLOOPS
	{
		Char 	*ch;
		ch  = CHAR_getCharPointer( charaindex);
		if( ch == NULL ) return;
		strcpysafe( ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
			sizeof( ch->charfunctable[CHAR_LOOPFUNCTEMP1]), "CHAR_BattleStayLoop");//ս��
		CHAR_setInt( charaindex, CHAR_LOOPINTERVAL, 2500);
		CHAR_constructFunctable( charaindex);
	}
#endif
}
#endif

#ifdef _CHRISTMAS_REDSOCKS
void ITEM_useMaxRedSocks( int charaindex, int toindex, int haveitemindex)
{
	int itemtimes = -1, itemindex;
	char *itemarg=NULL;
	int present[13]={ 13061, 13062, 13063, 13064, 13088, 13089, 13090, 13091, //1.
						14756, 17256,
					    13092,19692,20594};
	int nowtimes = time( NULL);

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

	itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "\0" ) {
		CHAR_talkToCli(charaindex, -1, "������Ч!", CHAR_COLORYELLOW);
		return;
	}
	itemtimes = atoi( itemarg);
	if( nowtimes >= itemtimes && nowtimes <= itemtimes+(60*60*24) ){ //����ʱ���ڿɻ�����
		int si=0, ret;
		char token[256];
		//ɾ��
		CHAR_setItemIndex(charaindex, haveitemindex ,-1);
		CHAR_sendItemDataOne( charaindex, haveitemindex);
		ITEM_endExistItemsOne(itemindex);
		itemindex = -1;
		//����
		si = rand()%100;
		if( si > 70 ){
			si = rand()%3+10;
		}else if( si > 60 ){
			si = rand()%2+8;
		}else {
			si = rand()%8;
		}
		itemindex = ITEM_makeItemAndRegist( present[ si]);
		if( !ITEM_CHECKINDEX( itemindex)){
			CHAR_talkToCli(charaindex, -1, "������Ч!", CHAR_COLORYELLOW);
			return;
		}
		ret = CHAR_addItemSpecificItemIndex( charaindex, itemindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			ITEM_endExistItemsOne( itemindex);
			CHAR_talkToCli(charaindex, -1, "�����÷���λ����!", CHAR_COLORYELLOW);
			return;
		}
		sprintf( token,"�õ�%s",ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
		CHAR_sendItemDataOne( charaindex, ret);
	}else{
		char token[256];
		if( nowtimes < itemtimes ){
			int days, hours, minute, second;
			int defTimes = itemtimes - nowtimes;

			days = defTimes/(24*60*60);
			defTimes = defTimes-( days*(24*60*60));
			hours = defTimes/(60*60);
			defTimes = defTimes-( hours*(60*60));
			minute = defTimes/60;
			defTimes = defTimes-( minute*60);
			second = defTimes;
			sprintf( token,"%s����%d��%dСʱ%d��%d��ſ�ʹ��!",
				ITEM_getChar( itemindex, ITEM_NAME), days, hours, minute, second);
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
		}else if( nowtimes > itemtimes+(60*60*24) ){
			sprintf( token,"%sʹ�������ѹ�!", ITEM_getChar( itemindex, ITEM_NAME));
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
		}
	}
}
#endif

#ifdef _CHRISTMAS_REDSOCKS_NEW
void ITEM_useMaxRedSocksNew( int charaindex, int toindex, int haveitemindex)
{
	int itemindex;
	char *itemarg=NULL;
	char itemnumstr[32];
	int itemnum=0;
	int present[20];
    int si=0, ret,i;
	char token[256];

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

	itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "\0" ){
		CHAR_talkToCli(charaindex, -1, "���Ǹ�����ʥ����!", CHAR_COLORYELLOW);
		return;
	}

	//��������
	if( getStringFromIndexWithDelim( itemarg, "|", 1, itemnumstr, sizeof( itemnumstr)) == FALSE )
		return;
    itemnum = atoi(itemnumstr);
	if( itemnum > 20 )
		itemnum = 20;
	for(i=0;i<itemnum;i++){
		if( getStringFromIndexWithDelim( itemarg, "|", 2+i, itemnumstr, sizeof( itemnumstr)) )
		    present[i] = atoi(itemnumstr);
	}
	
	//ɾ��
	CHAR_setItemIndex(charaindex, haveitemindex ,-1);
	CHAR_sendItemDataOne( charaindex, haveitemindex);
	ITEM_endExistItemsOne(itemindex);
	itemindex = -1;
	//����
	si = rand()%itemnum;

	itemindex = ITEM_makeItemAndRegist( present[ si]);
	if( !ITEM_CHECKINDEX( itemindex)){
		CHAR_talkToCli(charaindex, -1, "������Ч!", CHAR_COLORYELLOW);
		return;
	}
	ret = CHAR_addItemSpecificItemIndex( charaindex, itemindex);
	if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
		ITEM_endExistItemsOne( itemindex);
		CHAR_talkToCli(charaindex, -1, "�����÷���λ����!", CHAR_COLORYELLOW);
		return;
	}
	sprintf( token,"�õ�%s",ITEM_getChar( itemindex, ITEM_NAME));
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
	CHAR_sendItemDataOne( charaindex, ret);

}
#endif

#ifdef _PETSKILL_CANNEDFOOD
void ITEM_useSkillCanned( int charaindex, int toindex, int itemNo)
{
	int itemindex;
	char buf1[256];
	itemindex = CHAR_getItemIndex( charaindex, itemNo);
	if(!ITEM_CHECKINDEX( itemindex)) return;
	if( !CHAR_CHECKINDEX( toindex) ) return;
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
		int i, petNo=-1;
		for( i=0; i<CHAR_MAXPETHAVE; i++)	{
			if( toindex == CHAR_getCharPet( charaindex, i) ){
				petNo = i;
				break;
			}
		}
		if( petNo == -1 ){
			sprintf( buf1, "%s�����������ϡ�", CHAR_getChar( toindex, CHAR_NAME) );
			CHAR_talkToCli( charaindex, -1, buf1, CHAR_COLORYELLOW);
			return;
		}
		ITEM_usePetSkillCanned_PrintWindow( charaindex, petNo);
		CHAR_setWorkInt( charaindex, CHAR_WORKRENAMEITEMNUM, petNo);
		CHAR_setWorkInt( charaindex, CHAR_WORKRENAMEITEMINDEX, itemNo);
	}else{
		sprintf( buf1, "���� %s���޳���ʹ�á�", ITEM_getChar( itemindex, ITEM_NAME) );
		CHAR_talkToCli( charaindex, -1, buf1, CHAR_COLORYELLOW);
		return;
	}

}
#endif

#ifdef _ITEM_METAMO
void ITEM_metamo( int charaindex, int toindex, int haveitemindex )
{
	
	char *arg, msg[128];
	int itemindex, metamoTime, haveindex, battlemode, i;

	if( CHAR_CHECKINDEX( charaindex ) == FALSE )	return;
	//print(" PetMetamo_toindex:%d ", toindex);

	if( CHAR_getInt( charaindex, CHAR_RIDEPET ) != -1 ){
		CHAR_talkToCli( charaindex, -1, "�޷���������в��ܱ���", CHAR_COLORYELLOW );
		return;
	}
#ifdef _FIX_METAMORIDE
	if( CHAR_CHECKJOINENEMY( charaindex) == TRUE ){
		CHAR_talkToCli( charaindex, -1, "�޷���������в��ܱ���", CHAR_COLORYELLOW );
		return;
	}
#else
	if( CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER) == 100259 ){
		CHAR_talkToCli( charaindex, -1, "�޷���������в��ܱ���", CHAR_COLORYELLOW );
		return;
	}
#endif
#ifdef _PETSKILL_BECOMEPIG
    if( CHAR_getInt( charaindex, CHAR_BECOMEPIG) > -1 ){//���������
		CHAR_talkToCli( charaindex, -1, "�޷������������в��ܱ���", CHAR_COLORYELLOW );
	    return;
	}
#endif


#ifdef _FIXBUG_ATTACKBOW
	{
		int armindex = CHAR_getItemIndex( charaindex, CHAR_ARM);
		if( ITEM_CHECKINDEX( armindex) == TRUE )	{
			int armtype = BATTLE_GetWepon( charaindex);
			if( armtype == ITEM_BOW || armtype == ITEM_BOUNDTHROW ||
				armtype == ITEM_BREAKTHROW || armtype == ITEM_BOOMERANG)	{
				CHAR_talkToCli( charaindex, -1, "ʹ��Զ���������޷�����", CHAR_COLORYELLOW );
				return;
			}
		}
	}
#endif

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	// ��  ����ݷ�  ������
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return;
	}
	// ��    ��������
	if( IsBATTLING( charaindex ) == TRUE ){
		toindex = BATTLE_No2Index(CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX ), toindex );

	}
	
	if( CHAR_CHECKINDEX( toindex ) == FALSE )	return;

	haveindex = -1;	
	for( i = 0; i < 5; i++ ){
		if( CHAR_getCharPet( charaindex, i) == toindex ) {
			haveindex = i;
			break;
		}
	}
	if( haveindex == -1 && charaindex != toindex ){
		CHAR_talkToCli( charaindex, -1, "�޷�����ֻ�ܱ���Լ��ĳ��", CHAR_COLORYELLOW );
		return;
	}

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( arg == "\0" )return;
	if( sscanf( arg, "%d", &metamoTime) != 1 )
		return;


	if( toindex != charaindex ){
		CHAR_setWorkInt( charaindex, CHAR_WORKITEMMETAMO, NowTime.tv_sec +metamoTime);
		sprintf( msg, "�����%s��", CHAR_getChar( toindex, CHAR_NAME) );
	}
	else {
		CHAR_setWorkInt( charaindex, CHAR_WORKITEMMETAMO, 0);
		sprintf( msg, "����Լ���");
	}
	CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW );

	CHAR_setInt( charaindex, CHAR_BASEIMAGENUMBER, CHAR_getInt( toindex, CHAR_BASEBASEIMAGENUMBER) );
	CHAR_complianceParameter( charaindex);
	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
	CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);

	CHAR_DelItem( charaindex, haveitemindex);

}
#endif
#ifdef _ITEM_CRACKER
void ITEM_Cracker(int charaindex,int toindex,int haveitemindex)
{
	int battlemode;
	// �������Ƿ���Ч
	if(CHAR_CHECKINDEX(charaindex) == FALSE) return; //ʧ��
	battlemode = CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEMODE);
	// ս����ʹ����Ч
	if(!battlemode) ITEM_useCracker_Effect(charaindex,toindex,haveitemindex);
	else CHAR_talkToCli(charaindex,-1,"ʲ��Ҳû������",CHAR_COLORWHITE);
}
#endif

#ifdef _ITEM_ADDEXP	//vincent ��������
void ITEM_Addexp(int charaindex,int toindex,int haveitemindex)
{
	// �������Ƿ���Ч
	if(CHAR_CHECKINDEX(charaindex) == FALSE) return; //ʧ��
#if 1
	ITEM_useAddexp_Effect(charaindex,toindex,haveitemindex);
#else
	if( !CHAR_getWorkInt(charaindex,CHAR_WORKITEM_ADDEXP)){
		ITEM_useAddexp_Effect(charaindex,toindex,haveitemindex);
	}else{
		CHAR_talkToCli(charaindex,-1,"��ǰʹ��֮ҩЧ��Ȼ����",CHAR_COLORYELLOW);
	}
#endif
}
#endif

#ifdef _ITEM_REFRESH //vincent ����쳣״̬����
void ITEM_Refresh(int charaindex,int toindex,int haveitemindex)
{
	int battlemode,itemindex;
print("\nvincent--ITEM_Refresh");
		// �������Ƿ���Ч
	if(CHAR_CHECKINDEX(charaindex) == FALSE) 
	{
print("\nvincent--(charaindex) == FALSE");
		return; //ʧ��
	}
	itemindex = CHAR_getItemIndex(charaindex,haveitemindex);

	battlemode = CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEMODE);
	if(battlemode)
	{
print("\nvincent--enter ITEM_useAddexp_Effect");
print("\nvincent-->charaindex:%d,toindex:%d",charaindex,toindex);
		ITEM_useRefresh_Effect(charaindex,toindex,haveitemindex);
	}
	else CHAR_talkToCli(charaindex,-1,"ʲ��Ҳû������",CHAR_COLORWHITE);

    /* ƽ�ҷ�����������    �����������ջ� */
    CHAR_setItemIndex(charaindex, haveitemindex ,-1);
	CHAR_sendItemDataOne( charaindex, haveitemindex);/* ʧ��  ة��ޥ */
	/* ���� */
	ITEM_endExistItemsOne( itemindex );
}
#endif
//Terry 2001/12/21
#ifdef _ITEM_FIRECRACKER
void ITEM_firecracker(int charaindex,int toindex,int haveitemindex)
{
	int battlemode;

	// �������Ƿ���Ч
	if(CHAR_CHECKINDEX(charaindex) == FALSE) return; //ʧ��

	battlemode = CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEMODE);
	
	if( battlemode // ����Ƿ���ս����
#ifdef _PETSKILL_BECOMEPIG
	    && CHAR_getInt( charaindex, CHAR_BECOMEPIG) == -1 
#endif
		) 
		ITEM_useFirecracker_Battle(charaindex,toindex,haveitemindex);
	else 
		CHAR_talkToCli(charaindex,-1,"ʲ��Ҳû������",CHAR_COLORWHITE);
}
#endif
//Terry end


void ITEM_WearEquip( int charaindex, int itemindex)
{
	// WON ADD
//	if( ITEM_getInt(itemindex,ITEM_ID) == 20130 ){
		CHAR_setWorkInt( charaindex, CHAR_PickAllPet, TRUE);
///	}
	return;
}
void ITEM_ReWearEquip( int charaindex, int itemindex)
{
	CHAR_setWorkInt( charaindex, CHAR_PickAllPet, FALSE);
	return;
}


#ifdef _Item_ReLifeAct
void ITEM_DIErelife( int charaindex, int itemindex, int eqw)
{
	int ReceveEffect=-1;
	int toNo;
	int battleindex=-1;
	int attackNo=-1;
	int WORK_HP=1;
	char buf[256];
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE )	{
		print("\n battleindex =%d return", battleindex);
		return;
	}
	if( CHAR_CHECKINDEX( charaindex) == FALSE )
		return;
	if( ITEM_CHECKINDEX( itemindex) == FALSE )
		return;
#ifdef _DUMMYDIE
	if( CHAR_getFlg( charaindex, CHAR_ISDUMMYDIE) == FALSE ) {
#else
	if( CHAR_getFlg( charaindex, CHAR_ISDIE) == FALSE )	{
#endif
		print("\n [ %d, CHAR_ISDIE DUMMY FALSE] return !", charaindex);
		return;
	}

	if( ITEM_getArgument( ITEM_getChar(itemindex,ITEM_ARGUMENT),"HP", buf, sizeof(buf) )
		== FALSE ){
		WORK_HP = 1;
	}else	{
		if( !strcmp( buf, "FULL") )	{
			WORK_HP = CHAR_getWorkInt( charaindex, CHAR_WORKMAXHP );
		}else	{
			WORK_HP=atoi( buf);
		}
	}

	ReceveEffect = SPR_fukkatu3;
	toNo = BATTLE_Index2No( battleindex, charaindex );
	attackNo = -1;

	BATTLE_MultiReLife( battleindex, attackNo, toNo, WORK_HP, ReceveEffect );
	CHAR_setItemIndex( charaindex, eqw ,-1);
	ITEM_endExistItemsOne( itemindex);
	CHAR_sendItemDataOne( charaindex, eqw);
	return;
}
#endif

#ifdef _EQUIT_DEFMAGIC
void ITEM_MagicEquitWear( int charaindex, int itemindex)
{
	char buf[256];
	char *itemarg;
	char Free[][128]={"EA","WA","FI","WI","QU"};
	int index=0;
	int dMagic=0;
	itemarg = ITEM_getChar(itemindex,ITEM_ARGUMENT);
	if( itemarg == "\0" ) return;
	while( index < arraysizeof( Free) )	{
		if( ITEM_getArgument( itemarg, Free[ index], buf, sizeof(buf)) == TRUE )	{
			dMagic = atoi( buf);
			if( dMagic <= 100 && dMagic >= -100 )	{
				int def_magic = CHAR_getWorkInt( charaindex, CHAR_EQUITDEFMAGIC_E+index);
				CHAR_setWorkInt( charaindex, CHAR_EQUITDEFMAGIC_E+index, def_magic + dMagic);
			}
			dMagic = 0;
		}
		index++;
		if( (CHAR_EQUITDEFMAGIC_E+index) > CHAR_EQUITQUIMAGIC )
			break;
	}

	return;
}

void ITEM_MagicEquitReWear( int charaindex, int itemindex){
	char buf[256];
	char *itemarg;
	char Free[][128]={"EA","WA","FI","WI","QU"};
	int index=0;
	int dMagic=0;

	itemarg = ITEM_getChar(itemindex,ITEM_ARGUMENT);
	if( itemarg == "\0" ) return;
	while( index < arraysizeof( Free) )	{
		if( ITEM_getArgument( itemarg, Free[ index], buf, sizeof(buf)) == TRUE )	{
			dMagic = atoi( buf);
			if( dMagic <= 100 && dMagic >= -100 )	{
				int def_magic = CHAR_getWorkInt( charaindex, CHAR_EQUITDEFMAGIC_E+index);
				CHAR_setWorkInt( charaindex, CHAR_EQUITDEFMAGIC_E+index, def_magic - dMagic);
			}
			dMagic = 0;
		}
		index++;
		if( (CHAR_EQUITDEFMAGIC_E+index) > CHAR_EQUITQUIMAGIC )
			break;
	}

	return;	
}
#endif

#ifdef _ITEM_WARP_FIX_BI
void recoverbi(int index)
{
	int eBbi=-1, eArm=-1, eNum=-1, eBi=-1, bi=-1;	

	bi   = CHAR_getInt( index, CHAR_BASEIMAGENUMBER);	
	eBbi = CHAR_getInt( index, CHAR_BASEBASEIMAGENUMBER);
	eArm = CHAR_getItemIndex( index, CHAR_ARM);
	eNum = ITEM_FIST;
	if(ITEM_CHECKINDEX(eArm))
		eNum = ITEM_getInt( eArm, ITEM_TYPE);
	
	eBi = CHAR_getNewImagenumberFromEquip( eBbi, eNum);	

	if(CHAR_getInt( index, CHAR_RIDEPET)!=-1)	     //���			
		CHAR_complianceParameter(index);
	else                                             //�����		
		if((eBi!=-1)&&(eBi!=bi))
			CHAR_setInt( index, CHAR_BASEIMAGENUMBER, eBi);
}
#endif

#ifdef _ITEM_TIME_LIMIT
void ITEM_TimeLimit( int charaindex)
{
    int  i      = 0;
	int  iid    = 0;
	long lTime  = 0;
	char buff[256];

	if( !CHAR_CHECKINDEX( charaindex) ) return;
	for( i=0; i < CHAR_MAXITEMHAVE ; i++ ){
		int itemindex = CHAR_getItemIndex( charaindex , i );
		if( !ITEM_CHECKINDEX( itemindex ) ) continue;
		lTime = ITEM_getWorkInt( itemindex, ITEM_WORKTIMELIMIT);
		if( ITEM_getInt( itemindex, ITEM_ID) == 20173 //ȼ�ջ��
			|| ITEM_getInt( itemindex, ITEM_ID) == 20704 ){
			if( lTime > 0 && NowTime.tv_sec > lTime ){
				iid = ITEM_getInt( itemindex, ITEM_ID) + 1;
				snprintf( buff, sizeof( buff), "%s��Ч������ʧ..", ITEM_getChar( itemindex, ITEM_NAME));
				CHAR_talkToCli( charaindex, -1, buff, CHAR_COLORGREEN);
				CHAR_DelItemMess( charaindex, i, 0);
				itemindex = ITEM_makeItemAndRegist( iid);
				if(itemindex!=-1){
					CHAR_setItemIndex( charaindex, i, itemindex);
					ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1);
					ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, charaindex);
					CHAR_sendItemDataOne( charaindex, i);
				}
			}
		}
	}	

}
#endif

#ifdef _ITEM_CONSTITUTION
void ITEM_Constitution( int charaindex, int toindex, int haveitemindex)
{
	char buf[256];
	char *itemarg;
	char Free[][128]={"VI","ST","TG","DE"};
	int index=0, FixPoint=0, itemindex;
	BOOL FIXs=FALSE;
	int AllPoint=0;

	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER )
		return;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

	itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "\0" ) return;

	AllPoint = CHAR_getInt( charaindex, CHAR_VITAL)
		+CHAR_getInt( charaindex, CHAR_VITAL+1)
		+CHAR_getInt( charaindex, CHAR_VITAL+2)
		+CHAR_getInt( charaindex, CHAR_VITAL+3);

	while( index < arraysizeof( Free) )	{
		if( ITEM_getArgument( itemarg, Free[ index], buf, sizeof(buf)) == TRUE )	{
			FixPoint = atoi( buf);
			if( FixPoint <= 100 && FixPoint > 0 )	{
				int points = CHAR_getInt( charaindex, CHAR_VITAL+index);
				//Change Fix
				//if( points < (FixPoint*100) ) break;
				if( points < (FixPoint*100) || AllPoint-(FixPoint*100) <= 0 ) 
				{
					CHAR_talkToCli( charaindex, -1, "��Ʒ��Ч��", CHAR_COLORYELLOW);
					return;
				}
				CHAR_setInt( charaindex, CHAR_VITAL+index, (points-(FixPoint*100)));
				CHAR_setInt( charaindex, CHAR_SKILLUPPOINT, CHAR_getInt( charaindex, CHAR_SKILLUPPOINT) + FixPoint);
				FIXs = TRUE;
			}
			FixPoint = 0;
		}
		index++;
		if( (CHAR_VITAL+index) > CHAR_DEX )
			break;
	}
	CHAR_complianceParameter( charaindex);
	CHAR_send_P_StatusString(  charaindex,
		CHAR_P_STRING_MAXHP|CHAR_P_STRING_HP|CHAR_P_STRING_LV|CHAR_P_STRING_EXP|
		CHAR_P_STRING_ATK|CHAR_P_STRING_DEF|CHAR_P_STRING_QUICK|
		CHAR_P_STRING_VITAL|CHAR_P_STRING_STR|CHAR_P_STRING_TOUGH|CHAR_P_STRING_DEX);

	CHAR_Skillupsend( charaindex);
	if( FIXs == TRUE )	{
		sprintf( buf, "%s", "�������е��仯��");
	}else	{
		sprintf( buf, "%s", "��Ʒ��Ч��");
	}
	CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _NEW_RIDEPETS
void ITEM_useLearnRideCode( int charaindex, int toindex, int haveitemindex)
{//CHAR_LOWRIDEPETS
	
	if(getRideMode()==2){
		CHAR_talkToCli( charaindex, -1, "�����Ϊ2.0����֤ģʽ���������޷�ʹ�ø���Ʒ", CHAR_COLORYELLOW);
		return;
	}
	int itemindex, i;
	char buf1[256];
	char *itemarg=NULL;
	int ridetrans;
	typedef struct{
		char arg[32];
		int Code;
	}tagNewRideCode;
	tagNewRideCode NewRides[]={
		{ "RIDE_PET0",  RIDE_PET0},  { "RIDE_PET1",  RIDE_PET1},  { "RIDE_PET2",  RIDE_PET2},
		{ "RIDE_PET3",  RIDE_PET3},  { "RIDE_PET4",  RIDE_PET4},  { "RIDE_PET5",  RIDE_PET5},
		{ "RIDE_PET6",  RIDE_PET6},  { "RIDE_PET7",  RIDE_PET7},  { "RIDE_PET8",  RIDE_PET8},
		{ "RIDE_PET9",  RIDE_PET9},  { "RIDE_PET10", RIDE_PET10}, { "RIDE_PET11", RIDE_PET11},
		{ "RIDE_PET12", RIDE_PET12}, { "RIDE_PET13", RIDE_PET13}, { "RIDE_PET14", RIDE_PET14},
#ifdef _RIDE_CF
		{ "RIDE_PET15", RIDE_PET15}, { "RIDE_PET16", RIDE_PET16}, { "RIDE_PET17", RIDE_PET17},
	  { "RIDE_PET18", RIDE_PET18}, { "RIDE_PET19", RIDE_PET19}, { "RIDE_PET20", RIDE_PET20},
	  { "RIDE_PET21", RIDE_PET21}, { "RIDE_PET22", RIDE_PET22}, { "RIDE_PET23", RIDE_PET23},
	  { "RIDE_PET24", RIDE_PET24}, { "RIDE_PET25", RIDE_PET25}, { "RIDE_PET26", RIDE_PET26},
	  { "RIDE_PET27", RIDE_PET27}, { "RIDE_PET28", RIDE_PET28}, { "RIDE_PET29", RIDE_PET29},
	  { "RIDE_PET30", RIDE_PET30}, { "RIDE_PET31", RIDE_PET31},
	  { "RIDE_PET32", RIDE_PET0 }, { "RIDE_PET33", RIDE_PET1 }, { "RIDE_PET34", RIDE_PET2 }, 
	  { "RIDE_PET35", RIDE_PET3 }, { "RIDE_PET36", RIDE_PET4 }, { "RIDE_PET37", RIDE_PET5 }, 
	  { "RIDE_PET38", RIDE_PET6 }, { "RIDE_PET39", RIDE_PET7 }, { "RIDE_PET40", RIDE_PET8 }, 
	  { "RIDE_PET41", RIDE_PET9 }, { "RIDE_PET42", RIDE_PET10}, { "RIDE_PET43", RIDE_PET11}, 
	  { "RIDE_PET44", RIDE_PET12}, { "RIDE_PET45", RIDE_PET13}, { "RIDE_PET46", RIDE_PET14}, 
	  { "RIDE_PET47", RIDE_PET15}, { "RIDE_PET48", RIDE_PET16}, { "RIDE_PET49", RIDE_PET17}, 
	  { "RIDE_PET50", RIDE_PET18}, { "RIDE_PET51", RIDE_PET19}, { "RIDE_PET52", RIDE_PET20}, 
	  { "RIDE_PET53", RIDE_PET21}, { "RIDE_PET54", RIDE_PET22}, { "RIDE_PET55", RIDE_PET23}, 
	  { "RIDE_PET56", RIDE_PET24}, { "RIDE_PET57", RIDE_PET25}, { "RIDE_PET58", RIDE_PET26}, 
	  { "RIDE_PET59", RIDE_PET27}, { "RIDE_PET60", RIDE_PET28}, { "RIDE_PET61", RIDE_PET29}, 
	  { "RIDE_PET62", RIDE_PET30}, { "RIDE_PET63", RIDE_PET31}
#endif
	};

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "\0" ) return;

	if( getStringFromIndexWithDelim(itemarg,"|", 3, buf1, sizeof(buf1)) == FALSE )
		ridetrans=0;
	else
		ridetrans=atoi(buf1);
	if( getStringFromIndexWithDelim(itemarg,"|", 1, buf1, sizeof(buf1)) == FALSE )
		return;
	for( i=0; i<MAXNOINDEX; i++){
		if( !strcmp( NewRides[i].arg, buf1))	{
			int LRCode;
			if(CHAR_getInt( charaindex, CHAR_TRANSMIGRATION) < ridetrans){
				char token[256];
				if( getStringFromIndexWithDelim(itemarg,"|", 2, buf1, sizeof(buf1)) != FALSE ){
					sprintf( token, "����%dת�����ϲ���ѧϰ��%s��",ridetrans,buf1);
					CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
					return;
				}
			}
#ifdef _RIDE_CF
			if(i<MAXNOINDEX/2){
				LRCode = CHAR_getInt( charaindex, CHAR_LOWRIDEPETS);
				LRCode = LRCode|NewRides[i].Code;
				CHAR_setInt( charaindex, CHAR_LOWRIDEPETS, LRCode);
			}else{
				LRCode = CHAR_getInt( charaindex, CHAR_LOWRIDEPETS1);
				LRCode = LRCode|NewRides[i].Code;
				CHAR_setInt( charaindex, CHAR_LOWRIDEPETS1, LRCode);
			}
#else
			LRCode = CHAR_getInt( charaindex, CHAR_LOWRIDEPETS);
			LRCode = LRCode|NewRides[i].Code;
			CHAR_setInt( charaindex, CHAR_LOWRIDEPETS, LRCode);
#endif
			if( getStringFromIndexWithDelim(itemarg,"|", 2, buf1, sizeof(buf1)) != FALSE ){
				char token[256];

				sprintf( token, "ѧϰ���µ���� (%s)��", buf1);
				CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
				CHAR_DelItem( charaindex, haveitemindex);
				CHAR_sendStatusString( charaindex, "x");
			}
			break;
		}
	}
}

#endif
#ifdef _THROWITEM_ITEMS
static int Niceitem = 0;
	int golds[3]={ 10000, 20000, 50000};
	int items1[18] = { 13092, 13091, 20439, 20417, 1284, 20172, 18210, 19014, 18360, 18362, 18364,
						18359, 18356, 18357, 18510, 20418, 20419, 1452};
	int items2[11] = { 15842, 16136, 14334, 14034, 14634, 14934, 15534, 14934, 16432, 17057, 19695};

	int items3[10] = { 16014, 16314, 14515, 14215, 14815, 15115, 15715, 15295, 16552, 17157};

	int items4[18] = { 14516, 14513, 14216, 14213, 14816, 14813, 15116, 15716, 15415, 17360, 20279,
						20282, 20276, 20270, 20288, 20290, 20291, 20289};
	int items5[5] = { 20280, 20283, 20277, 20271, 20274};
	int items6[5] = { 20284, 20272, 20275, 20281, 20278};
void ITEM_ThrowItemBox( int charaindex, int toindex, int haveitemindex)
{
	int i, ret, Iindex, ItemID=-1, itemindex;
	char token[256];

	if( !CHAR_CHECKINDEX(charaindex) ) return;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX(itemindex) )  return;

	if( Niceitem > 10 ) ret = rand()%920;
	else ret = rand()%1000;

	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){
		Iindex = CHAR_getItemIndex( charaindex , i );
		if( !ITEM_CHECKINDEX(itemindex) ) continue;
		if( itemindex == Iindex ){
			CHAR_DelItem( charaindex, i);

			LogItem(
				CHAR_getChar( charaindex, CHAR_NAME ), CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
				itemindex,
#else
				ITEM_getInt( itemindex, ITEM_ID),
#endif
				"WarpManDelItem",
				CHAR_getInt( charaindex, CHAR_FLOOR), CHAR_getInt( charaindex, CHAR_X ),
				CHAR_getInt( charaindex, CHAR_Y ), ITEM_getChar( itemindex, ITEM_UNIQUECODE),
				ITEM_getChar( itemindex, ITEM_NAME),
				ITEM_getInt( itemindex, ITEM_ID)
			);
			break;
		}
	}
	if( i >= CHAR_MAXITEMHAVE ){
		return;
	}

	if( ret <= 774 ){
		int Golds=0;
		Golds = golds[ RAND(0,2)];
		CHAR_AddGold( charaindex, Golds);
	}else {
		if( ret < 924 ){
			ItemID = items1[RAND(0,17)];
		}else if( ret < 964 ){
			ItemID = items2[RAND(0,10)];
		}else if( ret < 984 ){
			ItemID = items3[RAND(0,9)];
			Niceitem++;
		}else if( ret < 994 ){
			ItemID = items4[RAND(0,17)];
			Niceitem++;
		}else if( ret < 999 ){
			ItemID = items5[RAND(0,4)];
			Niceitem++;
		}else {
			ItemID = items6[RAND(0,4)];
			Niceitem++;
		}

		Iindex = ITEM_makeItemAndRegist( ItemID);
		if( !ITEM_CHECKINDEX( Iindex) ){
			return;
		}
		ret = CHAR_addItemSpecificItemIndex( charaindex, Iindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			ITEM_endExistItemsOne( Iindex);
			return;
		}
		sprintf( token,"�õ�%s", ITEM_getChar( Iindex, ITEM_NAME));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
		CHAR_sendItemDataOne( charaindex, ret);
	}
}
#endif

#ifdef _ITEM_LOVERPARTY
void ITEM_LoverSelectUser( int charaindex, int toindex, int haveitemindex)
{
	int itemindex, i;
	int playernum = CHAR_getPlayerMaxNum();

	if( !CHAR_CHECKINDEX( charaindex ) )  return;
	if( !CHAR_CHECKINDEX( toindex ) )  return;
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER){
		CHAR_talkToCli( charaindex, -1, "��Ʒ��������ʹ�á�", CHAR_COLORYELLOW);
		return;
	}
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX(itemindex) )  return;
	if( ITEM_getInt( itemindex, ITEM_TYPE) != 16 ){
		ITEM_setInt( itemindex, ITEM_TYPE, 16);
		return;
	}
	if( !strcmp( ITEM_getChar( itemindex, ITEM_FORUSERNAME), "") ||
		!strcmp( ITEM_getChar( itemindex, ITEM_FORUSERCDKEY), "") ){//Ѱ���趨����
		if( charaindex == toindex ){
			CHAR_talkToCli( charaindex, -1, "����ѡ���Լ�ʹ�á�", CHAR_COLORYELLOW);
			return;
		}
		ITEM_setChar( itemindex, ITEM_FORUSERNAME, CHAR_getChar( toindex, CHAR_NAME) );
		ITEM_setChar( itemindex, ITEM_FORUSERCDKEY, CHAR_getChar( toindex, CHAR_CDKEY) );
		ITEM_setInt( itemindex, ITEM_TARGET, 0);
		{
			char token[256];
			sprintf( token, "%s(%s)",
				ITEM_getChar( itemindex, ITEM_SECRETNAME), CHAR_getChar( toindex, CHAR_NAME));
			ITEM_setChar( itemindex, ITEM_SECRETNAME, token);
			sprintf( token, "���Ͷ����趨Ϊ%s��", CHAR_getChar( toindex, CHAR_NAME));
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
		}
		CHAR_sendItemDataOne( charaindex, haveitemindex);
	}else{
		char buf1[256];
		//if( CHAR_getInt( charaindex, CHAR_FLOOR) == 117 || CHAR_getInt( charaindex, CHAR_FLOOR) == 887 ||
		//CHAR_getInt( charaindex, CHAR_FLOOR) == 1042 || CHAR_getInt( charaindex, CHAR_FLOOR) == 2032 ||
		//CHAR_getInt( charaindex, CHAR_FLOOR) == 3032 || CHAR_getInt( charaindex, CHAR_FLOOR) == 4032 ||
		//CHAR_getInt( charaindex, CHAR_FLOOR) == 5032 || CHAR_getInt( charaindex, CHAR_FLOOR) == 6032 ||
		//CHAR_getInt( charaindex, CHAR_FLOOR) == 7032 || CHAR_getInt( charaindex, CHAR_FLOOR) == 8032 ||
		//CHAR_getInt( charaindex, CHAR_FLOOR) == 9032 ){ 
		if( checkUnlawWarpFloor( CHAR_getInt( charaindex, CHAR_FLOOR) ) ) {
			CHAR_talkToCli( charaindex, -1, "�������λ���޷����͡�", CHAR_COLORYELLOW );
			return;
		}

		for( i=0; i<playernum; i++)	{
			int itemmaxuse=0;
			if( !CHAR_CHECKINDEX( i) ) continue;
			if( !strcmp( ITEM_getChar( itemindex, ITEM_FORUSERNAME), CHAR_getChar( i, CHAR_NAME)) &&
				!strcmp( ITEM_getChar( itemindex, ITEM_FORUSERCDKEY), CHAR_getChar( i, CHAR_CDKEY)) ){
				int floor, x, y;
				char token[256];
				floor = CHAR_getInt( i, CHAR_FLOOR);
				x = CHAR_getInt( i, CHAR_X);
				y = CHAR_getInt( i, CHAR_Y);

			//if( floor == 887 || floor == 117 ||
			//	floor == 1042 || floor == 2032 || floor == 3032 || floor == 4032 ||
			//	floor == 5032 || floor == 6032 ||floor == 7032 || floor == 8032 || floor == 9032 ){ 
				if( checkUnlawWarpFloor( floor) ) {
					CHAR_talkToCli( charaindex, -1, "�������ڵط��޷����͡�", CHAR_COLORYELLOW );
					return;
				}

				CHAR_warpToSpecificPoint( charaindex, floor, x, y );
				sprintf( token, "%s���ɽ�ָ���͵����������", CHAR_getChar( charaindex, CHAR_NAME));
				CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
				sprintf( token, "���ɽ�ָ���͵�%s��ߡ�", CHAR_getChar( i, CHAR_NAME));
				CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
				CHAR_DischargePartyNoMsg( charaindex);//��ɢ�Ŷ�
#ifdef _ITEM_MAXUSERNUM
				itemmaxuse = ITEM_getInt( itemindex, ITEM_DAMAGEBREAK);
#endif
				if( itemmaxuse != -1 )	{
					itemmaxuse--;
#ifdef _ITEM_MAXUSERNUM
					ITEM_setInt( itemindex, ITEM_DAMAGEBREAK, itemmaxuse);
#endif
					if( itemmaxuse < 1 )	{
						sprintf( buf1, "���� %s��ʧ�ˡ�", ITEM_getChar( itemindex, ITEM_NAME) );
						CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
						CHAR_DelItem( charaindex, haveitemindex);
						return;
					}else{
						sprintf( buf1, "������Ŀ���������λ�ã���ʹ�ô���ʣ��%d�Ρ�", itemmaxuse);
						ITEM_setChar( itemindex, ITEM_EFFECTSTRING, buf1);
					}
				}else{
					itemmaxuse = 10;
					ITEM_setInt( itemindex, ITEM_DAMAGEBREAK, itemmaxuse);
					sprintf( buf1, "������Ŀ���������λ�ã���ʹ�ô���ʣ��%d�Ρ�", itemmaxuse);
					ITEM_setChar( itemindex, ITEM_EFFECTSTRING, buf1);
				}

				CHAR_DischargePartyNoMsg( charaindex);
				CHAR_complianceParameter( charaindex );
				CHAR_sendItemDataOne( charaindex, haveitemindex);
				return;
			}
		}
		CHAR_talkToCli( charaindex, -1, "�����ڴ��ŷ����������ϡ�", CHAR_COLORYELLOW );
	}
}
#endif

#ifdef _HALLOWEEN_EFFECT
void ITEM_MapEffect(int charaindex,int toindex,int haveitemindex)
{
	int itemindex,floor;
	char *pActionNumber,szMsg[128];

	if(CHAR_CHECKINDEX(charaindex) == FALSE )return ;

	itemindex = CHAR_getItemIndex(charaindex,haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	
	// �ҳ�������ڵĵ�ͼ���
	floor = CHAR_getInt(charaindex,CHAR_FLOOR);
	// �ҳ�����Ҫ�ŵ���Ч�ı��
	pActionNumber = ITEM_getChar(itemindex,ITEM_ARGUMENT);
	sprintf(szMsg,"%d 8 %s",floor,pActionNumber);
	// ִ��
	CHAR_CHAT_DEBUG_effect(charaindex,szMsg);
	CHAR_DelItemMess(charaindex,haveitemindex,0);
}
#endif

void ITEM_changePetOwner( int charaindex, int toindex, int haveitemindex)
{
	int itemindex, i;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return ;
	if( CHAR_CHECKINDEX( toindex ) == FALSE )return ;

	//ITEM_useRecovery_Field(	charaindex, toindex, haveitemindex );

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX(itemindex) ) return;
	
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET ) {
		CHAR_talkToCli( charaindex, -1, "��ֻ�����ڳ�������ม�", CHAR_COLORYELLOW );
		return;
	}

	if( !strcmp( CHAR_getChar( toindex, CHAR_NPCARGUMENT), "") || 
		!strcmp( CHAR_getChar( toindex, CHAR_NPCARGUMENT), CHAR_getChar( charaindex, CHAR_CDKEY))) {
		CHAR_talkToCli( charaindex, -1, "��ֻ���ﱾ��������ģ�������Ҫʹ�����ѽ��", CHAR_COLORYELLOW);
		return;
	}

	CHAR_setChar( toindex, CHAR_NPCARGUMENT, "");
	
	for( i =0; i < CHAR_MAXPETHAVE; i++)
		if( CHAR_getCharPet( charaindex, i) == toindex )
			CHAR_send_K_StatusString( charaindex, i,CHAR_K_STRING_NAME|CHAR_K_STRING_CHANGENAMEFLG);

	CHAR_talkToCli( charaindex, -1, "��������԰���ֻ����������ˡ�", CHAR_COLORYELLOW );

	CHAR_DelItemMess( charaindex, haveitemindex, 0);
}


#ifdef _DEL_DROP_GOLD
void GOLD_DeleteTimeCheckLoop( void)
{
	int amount;
	int objindex;
	int objmaxnum;
	
	objmaxnum = OBJECT_getNum();
	for( objindex=0; objindex<objmaxnum; objindex++)	{
		if( CHECKOBJECT( objindex ) == FALSE ) continue;
		if( OBJECT_getType( objindex) != OBJTYPE_GOLD) continue;
		
		amount = OBJECT_getIndex( objindex);
		if( amount >= 10000) continue;
		
		if( (int)NowTime.tv_sec > (int)( OBJECT_getTime( objindex) + getItemdeletetime() ) ) {
			
			LogStone(
				-1,
				"NULL",
				"NULL",
				amount,
				0,
				"Del(ɾ����ʱ��Ǯ)",
				OBJECT_getFloor( objindex ),
				OBJECT_getX( objindex ),
				OBJECT_getY( objindex )
				);
			
			CHAR_ObjectDelete(objindex);
		}
	}
}

void GOLD_DeleteTimeCheckOne( int objindex)
{
	int amount;
	
	if( CHECKOBJECT( objindex ) == FALSE ) return;
	if( OBJECT_getType( objindex) != OBJTYPE_GOLD) return;
	
	amount = OBJECT_getIndex( objindex);
	if( amount >= 10000) return;
	
	if( (int)NowTime.tv_sec > (int)( OBJECT_getTime( objindex) + getItemdeletetime() ) ) {
		
		LogStone(
			-1,
			"NULL",
			"NULL",
			amount,
			0,
			"Del(ɾ����ʱ��Ǯ)",
			OBJECT_getFloor( objindex ),
			OBJECT_getX( objindex ),
			OBJECT_getY( objindex )
			);
		
		CHAR_ObjectDelete(objindex);
	}

}

#endif

#ifdef _TIME_TICKET
void ITEM_timeticketEx( int charaindex, int toindex, int haveitemindex, int flag);
void ITEM_timeticket( int charaindex, int toindex, int haveitemindex)
{
	ITEM_timeticketEx( charaindex, toindex, haveitemindex, 0);
}

void ITEM_timeticketEx( int charaindex, int toindex, int haveitemindex, int flag)
{
	int itemindex;
	int addtime;
	int nowtime = time(NULL);
	int tickettime;
	int lefttime;
	char msg[1024];
	
	if( !CHAR_CHECKINDEX( charaindex ) )  return;

	if( check_TimeTicketMap( CHAR_getInt( charaindex, CHAR_FLOOR)) == FALSE 
		&& flag == 0 ) {
		CHAR_talkToCli( charaindex, -1, "����ص㲻��ʹ�á�", CHAR_COLORYELLOW);
		return;
	}
	
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX(itemindex) )  return;

	tickettime = CHAR_getWorkInt( charaindex, CHAR_WORKTICKETTIME);
	if( tickettime > nowtime+20 ) {
		CHAR_talkToCli( charaindex, -1, "ʱ�����ʣ�£��������ڲſ�ʹ�á�", CHAR_COLORYELLOW);
		return;
	}
	// ��һ��ʹ�õĻ�
	if( tickettime == 0 ) {
		tickettime = nowtime;
		CHAR_setWorkInt( charaindex, CHAR_WORKTICKETTIMESTART, nowtime);
	}
	// ս�����ҳ���ʱ��ʱʹ��
	//if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE 
	//		&& tickettime < nowtime ) {
		//tickettime = nowtime;
	//	sprintf(msg, "ʱ�䲻��%d�롣", nowtime - tickettime );
	//	CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);
	//}

	addtime = atoi( ITEM_getChar( itemindex, ITEM_ARGUMENT));
	tickettime += addtime;
	CHAR_setWorkInt( charaindex, CHAR_WORKTICKETTIME, tickettime );
	lefttime = tickettime - nowtime;
	if( lefttime > 0 )
		sprintf(msg, "ʱ������%d�룬��ʣ��%d��%d�롣", addtime, lefttime/60, lefttime%60 );
	else
		sprintf(msg, "ʱ������%d�룬������%d��%d�롣", addtime, (-lefttime)/60, (-lefttime)%60 );
	CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif
#ifdef _ITEM_SETLOVER
void ITEM_SetLoverUser( int charaindex, int toindex, int haveitemindex)
{
   int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
   int playernum = CHAR_getPlayerMaxNum();
   char token[256], szMsg[128];
   int floor = CHAR_getInt(charaindex,CHAR_FLOOR);

   if( !strcmp( CHAR_getChar( charaindex, CHAR_LOVE), "YES"))
        {
            CHAR_talkToCli( charaindex, -1, "�ػ���Υ���ģ����Ѿ������Ŷ~", CHAR_COLORYELLOW);
            return;
        }
   if( !CHAR_CHECKINDEX( charaindex ) ) return;
   if( !CHAR_CHECKINDEX( toindex ) ) return;
   if( CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
   {
   		 CHAR_talkToCli( charaindex, -1, "ֻ�ܺ���ҽ��Ŷ", CHAR_COLORYELLOW);
       return;
   }
   if( CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)<3 ||CHAR_getInt(toindex,CHAR_TRANSMIGRATION)<3)
   {
   	   CHAR_talkToCli( charaindex, -1, "�������Ļ���˫������3ת����Ŷ~", CHAR_COLORYELLOW);
       return;
   }
   if( charaindex == toindex )
   {
       CHAR_talkToCli( charaindex, -1, "�ѵ�������Լ������", CHAR_COLORYELLOW);
       return;
   }
   if ( IsMale(charaindex)==IsMale(toindex))
   {
       CHAR_talkToCli( charaindex, -1, "ͬ����ô����أ�", CHAR_COLORYELLOW);
       return;    
   }
   if( !ITEM_CHECKINDEX(itemindex) ) return;
			int id = ITEM_getInt( itemindex, ITEM_ID);
			itemindex = ITEM_makeItemAndRegist( id );

   if( strcmp( CHAR_getChar( charaindex, CHAR_LOVE), "YES") &&
   			strcmp( CHAR_getChar( toindex, CHAR_LOVE), "YES"))//�ڶ����ж�˫���Ƿ��Ѿ����
   {
	    if( itemindex != -1 ){
					 CHAR_setChar( charaindex, CHAR_LOVERID, CHAR_getChar( toindex, CHAR_CDKEY) );
					 CHAR_setChar( charaindex, CHAR_LOVERNAME, CHAR_getChar( toindex, CHAR_NAME) );
	    	 if (strcmp(CHAR_getChar( toindex, CHAR_LOVERID), CHAR_getChar( charaindex, CHAR_CDKEY))==0 &&
         			strcmp(CHAR_getChar( toindex, CHAR_LOVERNAME), CHAR_getChar( charaindex, CHAR_NAME))==0){
         		int i;
            sprintf( token, "�Ѿ���Ӧ��%s�����", CHAR_getChar( toindex, CHAR_NAME));
		        CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
		        sprintf( token, "%s�Ѿ���Ӧ��������", CHAR_getChar( charaindex, CHAR_NAME));
		        CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);
            CHAR_setChar( toindex, CHAR_LOVE,"YES");
		        CHAR_setChar( charaindex, CHAR_LOVE,"YES");
						CHAR_setChar( charaindex, CHAR_LOVERID, CHAR_getChar( toindex, CHAR_CDKEY) );
						CHAR_setChar( charaindex, CHAR_LOVERNAME, CHAR_getChar( toindex, CHAR_NAME) );
						CHAR_setChar( toindex, CHAR_LOVERID, CHAR_getChar( charaindex, CHAR_CDKEY) );
						CHAR_setChar( toindex, CHAR_LOVERNAME, CHAR_getChar( charaindex, CHAR_NAME) );
						sprintf(szMsg,"%d 8 101883",floor);
		        CHAR_CHAT_DEBUG_effect(charaindex,szMsg);
		        CHAR_DelItem( charaindex, haveitemindex);
         		for(i=0;i<playernum;i++)
            {    
		           if(CHAR_CHECKINDEX(i) == FALSE) continue;
		           sprintf( token, "��ϲ%s��%s�»�֮ϲ����ף���ǰ���ú�.��ͷ����.", 
														           CHAR_getChar( charaindex, CHAR_NAME),
														           CHAR_getChar( toindex, CHAR_NAME));
														           CHAR_talkToCli( i, -1, token, CHAR_COLORRED);
		        } 
		        return;
		     }else{
					 	int emptyitemindexinchara = CHAR_findEmptyItemBox( toindex );
								
						if( emptyitemindexinchara < 0 ){
								sprintf( token, "%s��Ʒ�����������ʧ��" , CHAR_getChar( toindex, CHAR_NAME));
								CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
								sprintf( token, "%s����������飬��������Ʒ��������" , CHAR_getChar( charaindex, CHAR_NAME));
								CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);}
						else{
						 		CHAR_setItemIndex( toindex, emptyitemindexinchara, itemindex);
						   	sprintf( token, "%s��%s���Ľ�ָ!ͬ�������ʹ��!", CHAR_getChar( charaindex, CHAR_NAME) 
																																							,CHAR_getChar( toindex, CHAR_NAME));
						    ITEM_setChar( itemindex, ITEM_EFFECTSTRING, token);
						   	CHAR_sendItemDataOne( toindex, emptyitemindexinchara);
						   	ITEM_endExistItemsOne(itemindex);
					  }
				
						sprintf( token, "%s������飬��öԷ���%s", CHAR_getChar( charaindex, CHAR_NAME),ITEM_getChar( itemindex, ITEM_NAME));
						CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);
						sprintf( token, "�Ѿ���%s�������", CHAR_getChar( toindex, CHAR_NAME));
						CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
						CHAR_DelItem( charaindex, haveitemindex);
				}
				CHAR_talkToCli( charaindex, -1, "�����ĵȴ��Է���Ӧ��", CHAR_COLORYELLOW );    
			}else{
					sprintf( token, "���ʧ�ܣ���ָ�޷�����!" );
					CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
					return;
			}
   }
}

void ITEM_LoverWarp( int charaindex, int toindex, int haveitemindex)
{
  int itemindex, i;
  int playernum = CHAR_getPlayerMaxNum();
  //����Ƿ���
  if( !strcmp( CHAR_getChar( charaindex, CHAR_LOVE), "YES"))
  {   
    for(i=0;i<playernum;i++)
    {
        if(CHAR_CHECKINDEX(i) == FALSE) continue;
        if(strcmp( CHAR_getChar( i, CHAR_LOVE), "YES") == 0 &&
        		strcmp(CHAR_getChar(charaindex,CHAR_LOVERID),CHAR_getChar(i,CHAR_CDKEY)) == 0 &&
            strcmp(CHAR_getChar(charaindex,CHAR_LOVERNAME),CHAR_getChar(i,CHAR_NAME)) == 0)
        {
          break;
        }
    }//��index�жԱ�LOVERNAME
        if (i>=playernum)
          {
            CHAR_talkToCli( charaindex, -1, "�ܱ�Ǹ�����İ��˲�����Ŷ~", CHAR_COLORYELLOW );   
            return;
          }

  if( !CHAR_CHECKINDEX( charaindex ) ) return;
  if( !CHAR_CHECKINDEX( i ) ) return;
  
  itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
  if( !ITEM_CHECKINDEX(itemindex) ) return;
  
    if( checkUnlawWarpFloor( CHAR_getInt( charaindex, CHAR_FLOOR) ) ) {
        CHAR_talkToCli( charaindex, -1, "�ܱ�Ǹ�����İ���������ңԶ�ط����޷����͡�", CHAR_COLORYELLOW );
        return;
    }//UNWARP��ͼ��ֹʹ��
    if( checkUnlawWarpFloor( CHAR_getInt( i, CHAR_FLOOR) ) ) {
        CHAR_talkToCli( charaindex, -1, "�ܱ�Ǹ�����İ���������ңԶ�ط����޷����͡�", CHAR_COLORYELLOW );
        return;
    }//UNWARP��ͼ��ֹʹ��
        if(strcmp(CHAR_getChar(i,CHAR_LOVERID),CHAR_getChar(charaindex,CHAR_CDKEY)) == 0 &&
            strcmp(CHAR_getChar(i,CHAR_LOVERNAME),CHAR_getChar(charaindex,CHAR_NAME)) == 0)
          {
          CHAR_DischargePartyNoMsg( charaindex);//��ɢ�Ŷ�
          int floor, x, y;
          char token[256];
          floor = CHAR_getInt( i, CHAR_FLOOR);
          x = CHAR_getInt( i, CHAR_X);
          y = CHAR_getInt( i, CHAR_Y);

          CHAR_warpToSpecificPoint( charaindex, floor, x, y );
          sprintf( token, "%s:�װ��ģ������ˣ�", CHAR_getChar( charaindex, CHAR_NAME));
          CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW );
          sprintf( token, "�Ѵ��͵�����%s��ߣ�", CHAR_getChar( i, CHAR_NAME));
          CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
          print("\n���%s ���͵����%s���", CHAR_getChar( charaindex, CHAR_NAME),CHAR_getChar(i, CHAR_NAME));
          return;
        }
    }else
    CHAR_talkToCli( charaindex, -1, "��û�н��Ŷ~", CHAR_COLORYELLOW );
}

void ITEM_LoverUnmarry( int charaindex, int toindex, int haveitemindex)
{
  int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
  int  i;
  char token[256];
  int playernum = CHAR_getPlayerMaxNum();
  //����Ƿ���
  if( !strcmp( CHAR_getChar( charaindex, CHAR_LOVE), "YES"))
  {   
    for(i=0;i<playernum;i++)
    {
        if(CHAR_CHECKINDEX(i) == FALSE) continue;
        if(strcmp( CHAR_getChar( i, CHAR_LOVE), "YES") == 0 &&
        		strcmp(CHAR_getChar(charaindex,CHAR_LOVERID),CHAR_getChar(i,CHAR_CDKEY)) == 0 &&
            strcmp(CHAR_getChar(charaindex,CHAR_LOVERNAME),CHAR_getChar(i,CHAR_NAME)) == 0)
        {
          break;
        }
    }//��index�жԱ�LOVERNAME
        if (i>=playernum)
          {
            CHAR_talkToCli( charaindex, -1, "�ܱ�Ǹ�����İ��˲�����Ŷ~", CHAR_COLORYELLOW );   
            return;
          }

  if( !CHAR_CHECKINDEX( charaindex ) ) return;
  if( !CHAR_CHECKINDEX( i ) ) return;
  if( !ITEM_CHECKINDEX(itemindex) ) return;
  	
  	 if(strcmp(ITEM_getChar( itemindex, ITEM_SECRETNAME), "ͬ������ָ")){
				int id = ITEM_getInt( itemindex, ITEM_ID);
				itemindex = ITEM_makeItemAndRegist( id );
	
		    if( itemindex != -1 ){
		    	
		   			int emptyitemindexinchara = CHAR_findEmptyItemBox( toindex );
					
						if( emptyitemindexinchara < 0 ){
								sprintf( token, "%s��Ʒ�����������ʧ��" , CHAR_getChar( i, CHAR_NAME));
								CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
								sprintf( token, "%s����������飬��������Ʒ��������" , CHAR_getChar( charaindex, CHAR_NAME));
								CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW);}
					 	else{
					 		CHAR_setItemIndex( i, emptyitemindexinchara, itemindex);
				      ITEM_setChar( itemindex, ITEM_NAME, "ͬ������ָ");
				      ITEM_setChar( itemindex, ITEM_SECRETNAME, "ͬ������ָ");
			      	sprintf( token, "%s����������飬���ͬ����ʹ�øý�ָ!", CHAR_getChar( charaindex, CHAR_NAME));
				      ITEM_setChar( itemindex, ITEM_EFFECTSTRING, token);
			      	CHAR_sendItemDataOne( i, emptyitemindexinchara);
			      	ITEM_endExistItemsOne(itemindex);
		       		sprintf( token, " %s���������鲢�� %s ���㣡", CHAR_getChar( charaindex, CHAR_NAME),ITEM_getChar( itemindex, ITEM_NAME));
		       		CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW);
		       		sprintf( token, "�Ѿ���%s�������", CHAR_getChar( i, CHAR_NAME));
		       		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
			      	CHAR_DelItem( charaindex, haveitemindex);
	          	return;}}
      }else{
     			CHAR_setChar( i, CHAR_LOVE,"");
		      CHAR_setChar( i, CHAR_LOVERID,"");
					CHAR_setChar( i, CHAR_LOVERNAME,"");
		      CHAR_setChar( charaindex, CHAR_LOVE,"");
					CHAR_setChar( charaindex, CHAR_LOVERID,"");
		      CHAR_setChar( charaindex, CHAR_LOVERNAME,"");
	       	CHAR_talkToCli( i, -1, "˫�����ɹ���", CHAR_COLORYELLOW);
	      	CHAR_talkToCli( charaindex, -1, "˫�����ɹ���", CHAR_COLORYELLOW);}
	      	CHAR_DelItem( charaindex, haveitemindex);
    }else
    CHAR_talkToCli( charaindex, -1, "��û�н��Ŷ~", CHAR_COLORYELLOW );
}


#endif

#ifdef _ITEM_METAMO
void ITEM_ColorMetamo( int charaindex, int toindex, int haveitemindex)
{
		int MetamoList[13][7]={
		/*{  ���� ,   ��  ,   ��  ,   ��  ,   ��  ,   ��  ,   ��  },  //����Ϊ˵����*/
			{ 100000, 100000, 100005, 100010, 100015, 100700, 100705},	//С����
			{ 100020, 100025, 100030, 100035, 100020, 100710, 100715},	//������
			{ 100040, 100055, 100050, 100045, 100040, 100720, 100725},	//�����к�
			{ 100060, 100060, 100065, 100070, 100075, 100730, 100735},	//���
			{ 100080, 100095, 100085, 100090, 100080, 100740, 100745},	//��Ƥ��
			{ 100100, 100100, 100115, 100110, 100105, 100750, 100755},	//���
			{ 100120, 100135, 100120, 100125, 100130, 100760, 100765},	//С����
			{ 100140, 100145, 100140, 100150, 100155, 100770, 100775},	//��Ƥ��
			{ 100160, 100165, 100170, 100160, 100175, 100780, 100785},	//ñ����
			{ 100180, 100190, 100195, 100185, 100180, 100790, 100795},	//�̷�����
			{ 100200, 100200, 100210, 100210, 100205, 100800, 100805},	//����Ů
			{ 100220, 100230, 100225, 100220, 100235, 100810, 100815},	//����
			{ 100240,   0   ,   0   ,   0   ,   0   , 100820,   0   },	//
		};
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char *itemarg="\0";
		int OldMetamoId, NewMetamoId, i;
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		if( itemarg == "\0" ) return;
		OldMetamoId=CHAR_getInt( charaindex, CHAR_BASEBASEIMAGENUMBER);
		for(i=0;i<12;i++)
			if((OldMetamoId>=MetamoList[i][0] && OldMetamoId<MetamoList[i+1][0])
				  || (OldMetamoId>=MetamoList[i][5] && OldMetamoId<MetamoList[i+1][5]))
					break;
		if(i==12){
				CHAR_talkToCli( charaindex, -1, "��ɫʧ�ܣ������Ͳ�������������", CHAR_COLORYELLOW);
				return;
		}
		if( strstr( itemarg, "��" ) ){
				CHAR_talkToCli( charaindex, -1, "��ɺ�ɫ", CHAR_COLORYELLOW);
				NewMetamoId=MetamoList[i][1];
		}else if( strstr( itemarg, "��" ) ){
				CHAR_talkToCli( charaindex, -1, "�����ɫ", CHAR_COLORYELLOW);
				NewMetamoId=MetamoList[i][2];
		}else if( strstr( itemarg, "��" ) ){
				CHAR_talkToCli( charaindex, -1, "��ɻ�ɫ", CHAR_COLORYELLOW);
				NewMetamoId=MetamoList[i][3];
		}else if( strstr( itemarg, "��" ) ){
				CHAR_talkToCli( charaindex, -1, "��ɻ�ɫ", CHAR_COLORYELLOW);
				NewMetamoId=MetamoList[i][4];
		}else if( strstr( itemarg, "��" ) ){
#ifdef _FM_METAMO
				switch( CHAR_getWorkInt( charaindex, CHAR_WORKFMFLOOR) ){
					case 1041:
					case 2031:
					case 3031:
					case 4031:
					case 5031:
					case 6031:
					case 7031:
					case 8031:
					case 9031:
					case 10031:
						if(CHAR_getInt( charaindex, CHAR_FMLEADERFLAG ) != FMMEMBER_NONE
								&& CHAR_getInt( charaindex, CHAR_FMLEADERFLAG ) != FMMEMBER_APPLY
								&&  CHAR_getInt( charaindex, CHAR_FMSPRITE ) == 0)
						{
#endif
							CHAR_talkToCli( charaindex, -1, "��ɰ�ɫ", CHAR_COLORYELLOW);
							NewMetamoId=MetamoList[i][5];
#ifdef _FM_METAMO
							break;
						}
					default:
						CHAR_talkToCli( charaindex, -1, "�ý�ָֻ��ӵ��ׯ԰�Ĺ��������Աʹ�ã�", CHAR_COLORYELLOW);
						return;
				}
#endif
		}else if( strstr( itemarg, "��" ) ){
#ifdef _FM_METAMO
				switch( CHAR_getWorkInt( charaindex, CHAR_WORKFMFLOOR) ){
					case 1041:
					case 2031:
					case 3031:
					case 4031:
					case 5031:
					case 6031:
					case 7031:
					case 8031:
					case 9031:
					case 10031:
						if(CHAR_getInt( charaindex, CHAR_FMLEADERFLAG ) != FMMEMBER_NONE
								&& CHAR_getInt( charaindex, CHAR_FMLEADERFLAG ) != FMMEMBER_APPLY
								&&  CHAR_getInt( charaindex, CHAR_FMSPRITE ) == 1)
						{
#endif
							CHAR_talkToCli( charaindex, -1, "��ɺ�ɫ", CHAR_COLORYELLOW);
							NewMetamoId=MetamoList[i][6];
#ifdef _FM_METAMO
							break;
						}
					default:
						CHAR_talkToCli( charaindex, -1, "�ý�ָֻ��ӵ��ׯ԰�ĺڰ������Աʹ�ã�", CHAR_COLORYELLOW);
						return;
				}
#endif
		}else if( strstr( itemarg, "��" ) ){
				srand(time(0));
				do
						NewMetamoId=MetamoList[i][rand()%4+1];
				while(NewMetamoId==OldMetamoId);
				CHAR_talkToCli( charaindex, -1, "�����ɫ�ɹ�", CHAR_COLORYELLOW);
		}else{
				CHAR_talkToCli( charaindex, -1, "��ɫʧ��", CHAR_COLORYELLOW);
			return;
		}
		if(NewMetamoId==0)return;
		CHAR_setInt( charaindex , CHAR_BASEIMAGENUMBER , NewMetamoId );
		CHAR_setInt( charaindex , CHAR_BASEBASEIMAGENUMBER , NewMetamoId );
	
		CHAR_complianceParameter( charaindex );
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
		CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
		CHAR_DelItem( charaindex, haveitemindex);
}

void ITEM_CharaMetamo( int charaindex, int toindex, int haveitemindex)
{
		int MetamoList[12][7]={
		/*{  ���� ,   ��  ,   ��  ,   ��  ,   ��  ,   ��  ,   ��  },  //����Ϊ˵����*/
			{ 100000, 100000, 100005, 100010, 100015, 100700, 100705},	//С����
			{ 100020, 100025, 100030, 100035, 100020, 100710, 100715},	//������
			{ 100040, 100055, 100050, 100045, 100040, 100720, 100725},	//�����к�
			{ 100060, 100060, 100065, 100070, 100075, 100730, 100735},	//���
			{ 100080, 100095, 100085, 100090, 100080, 100740, 100745},	//��Ƥ��
			{ 100100, 100100, 100115, 100110, 100105, 100750, 100755},	//���
			{ 100120, 100135, 100120, 100125, 100130, 100760, 100765},	//С����
			{ 100140, 100145, 100140, 100150, 100155, 100770, 100775},	//��Ƥ��
			{ 100160, 100165, 100170, 100160, 100175, 100780, 100785},	//ñ����
			{ 100180, 100190, 100195, 100185, 100180, 100790, 100795},	//�̷�����
			{ 100200, 100200, 100210, 100215, 100205, 100800, 100805},	//����Ů
			{ 100220, 100230, 100225, 100220, 100235, 100810, 100815},	//����
		};
		int OldMetamoId=0, NewMetamoId=0;
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char *itemarg="\0";
		srand(time(0));
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		if( itemarg == "\0" ) return;
		OldMetamoId=CHAR_getInt( charaindex, CHAR_BASEBASEIMAGENUMBER);
		if((OldMetamoId >= 100000 && OldMetamoId < 100240 )
				|| (OldMetamoId >= 100700 && OldMetamoId < 100820)){
			if( strstr( itemarg, "������" ) && OldMetamoId >= 100000 && OldMetamoId < 100120){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ����", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[0][rand()%4+1];
			}else if( strstr( itemarg, "������" ) && OldMetamoId >= 100000 && OldMetamoId < 100120){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ������", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[1][rand()%4+1];
			}else if( strstr( itemarg, "�����к�" ) && OldMetamoId >= 100000 && OldMetamoId < 100120){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ�����к�", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[2][rand()%4+1];
			}else if( strstr( itemarg, "���" ) && OldMetamoId >= 100000 && OldMetamoId < 100120){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ���", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[3][rand()%4+1];
			}else if( strstr( itemarg, "��Ƥ��" ) && OldMetamoId >= 100000 && OldMetamoId < 100120){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ��Ƥ��", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[4][rand()%4+1];
			}else if( strstr( itemarg, "��ֻ��" ) && OldMetamoId >= 100000 && OldMetamoId < 100120){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ��ֻ��", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[5][rand()%4+1];
			}else if( strstr( itemarg, "������" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ������", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[6][rand()%4+1];
			}else if( strstr( itemarg, "��Ƥ��" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ��Ƥ��", CHAR_COLORYELLOW);
  				NewMetamoId=MetamoList[7][rand()%4+1];
			}else if( strstr( itemarg, "ñ����" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					CHAR_talkToCli( charaindex, -1, "��������Ϊñ����", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[8][rand()%4+1];
			}else if( strstr( itemarg, "�̷�����" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ�̷�����", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[9][rand()%4+1];
			}else if( strstr( itemarg, "����Ů" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ����Ů", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[10][rand()%4+1];
			}else if( strstr( itemarg, "����" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ����", CHAR_COLORYELLOW);
					NewMetamoId=MetamoList[11][rand()%4+1];
			}else if( strstr( itemarg, "������" ) && OldMetamoId >= 100700 && OldMetamoId < 100760){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ����", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[0][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[0][6];
			}else if( strstr( itemarg, "������" ) && OldMetamoId >= 100700 && OldMetamoId < 100760){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ������", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[1][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[1][6];
			}else if( strstr( itemarg, "�����к�" ) && OldMetamoId >= 100700 && OldMetamoId < 100760){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ�����к�", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[2][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[2][6];
			}else if( strstr( itemarg, "���" ) && OldMetamoId >= 100700 && OldMetamoId < 100760){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ���", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[3][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[3][6];
			}else if( strstr( itemarg, "��Ƥ��" ) && OldMetamoId >= 100700 && OldMetamoId < 100760){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ��Ƥ��", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[4][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[4][6];
			}else if( strstr( itemarg, "��ֻ��" ) && OldMetamoId >= 100700 && OldMetamoId < 100760){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ��ֻ��", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[5][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[5][6];	
			}else if( strstr( itemarg, "������" ) && OldMetamoId >= 100760 && OldMetamoId < 100820){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ������", CHAR_COLORYELLOW);
					if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[6][6];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[6][6];
			}else if( strstr( itemarg, "��Ƥ��" ) && OldMetamoId >= 100760 && OldMetamoId < 100820){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ��Ƥ��", CHAR_COLORYELLOW);
								if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[7][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[7][6];	
			}else if( strstr( itemarg, "ñ����" ) && OldMetamoId >= 100760 && OldMetamoId < 100820){
					CHAR_talkToCli( charaindex, -1, "��������Ϊñ����", CHAR_COLORYELLOW);
								if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[8][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[8][6];	
			}else if( strstr( itemarg, "�̷�����" ) && OldMetamoId >= 100760 && OldMetamoId < 100820){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ�̷�����", CHAR_COLORYELLOW);
								if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[9][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[9][6];	
		  }else if( strstr( itemarg, "����Ů" ) && OldMetamoId >= 100760 && OldMetamoId < 100820){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ����Ů", CHAR_COLORYELLOW);
								if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[10][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[10][6];	
		  }else if( strstr( itemarg, "����" ) && OldMetamoId >= 100760 && OldMetamoId < 100820){
					CHAR_talkToCli( charaindex, -1, "��������Ϊ����", CHAR_COLORYELLOW);
								if( ( OldMetamoId - 100700 ) % 10 == 0 )
						  NewMetamoId=MetamoList[11][5];
					else if( ( OldMetamoId - 100700 ) % 10 == 5 )
						  NewMetamoId=MetamoList[11][6];	
			}else if( strstr( itemarg, "���" ) && OldMetamoId >= 100120 && OldMetamoId < 100240){
					do
							if( OldMetamoId >= 100000 && OldMetamoId < 100120 ){
									NewMetamoId=MetamoList[rand()%6][rand()%4+1];
							}else if( OldMetamoId >= 100120 && OldMetamoId < 100240 ){
								  NewMetamoId=MetamoList[rand()%6+6][rand()%4+1];
							}else if( OldMetamoId >= 100700 && OldMetamoId < 100760 ){
									if( ( OldMetamoId - 100700 ) % 10 == 0 )
										  NewMetamoId=MetamoList[rand()%6][5];
									else if( ( OldMetamoId - 100700 ) % 10 == 5 )
										  NewMetamoId=MetamoList[rand()%6][6];
							}else if( OldMetamoId >= 100760 && OldMetamoId < 100820 ){
									if( ( OldMetamoId - 100700 ) % 10 == 0 )
										  NewMetamoId=MetamoList[rand()%6+6][5];
									else if( ( OldMetamoId - 100700 ) % 10 == 5 )
										  NewMetamoId=MetamoList[rand()%6+6][6];
							}
					while(NewMetamoId==OldMetamoId);
					CHAR_talkToCli( charaindex, -1, "����������ͳɹ�", CHAR_COLORYELLOW);
			}
		}else{
			CHAR_talkToCli( charaindex, -1, "�㲢������������!", CHAR_COLORYELLOW);
			return;
		}
		if(NewMetamoId==0){
			CHAR_talkToCli( charaindex, -1, "����Ա�ͬ����û���Ĵ�����!", CHAR_COLORYELLOW);
			return;}
		CHAR_setInt( charaindex , CHAR_BASEIMAGENUMBER , NewMetamoId );
		CHAR_setInt( charaindex , CHAR_BASEBASEIMAGENUMBER , NewMetamoId );
	
		CHAR_complianceParameter( charaindex );
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
		CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
		CHAR_DelItem( charaindex, haveitemindex);
}

void ITEM_SexMetamo( int charaindex, int toindex, int haveitemindex)
{
		int OldMetamoId, NewMetamoId;
		OldMetamoId=CHAR_getInt( charaindex, CHAR_BASEBASEIMAGENUMBER);
		if(OldMetamoId >=100000 && OldMetamoId <100020){                 //������
				NewMetamoId=OldMetamoId+120;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գɶ�����!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100020 && OldMetamoId <100040){           //������
				NewMetamoId=OldMetamoId+160;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գɶ̷�����!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100040 && OldMetamoId <100060){           //�����к�
				NewMetamoId=OldMetamoId+120;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գ�ñ����!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100060 && OldMetamoId <100080){           //���
				NewMetamoId=OldMetamoId+160;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գ�����!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100080 && OldMetamoId <100100){           //��Ƥ��
				NewMetamoId=OldMetamoId+60;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գ���Ƥ��!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100100 && OldMetamoId <100120){           //��ֻ��
				NewMetamoId=OldMetamoId+100;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գ�����Ů!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100120 && OldMetamoId <100140){           //������
				NewMetamoId=OldMetamoId-120;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գɶ�����!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100140 && OldMetamoId <100160){           //��Ƥ��
				NewMetamoId=OldMetamoId-60;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գ���Ƥ��!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100160 && OldMetamoId <100180){           //ñ����
				NewMetamoId=OldMetamoId-120;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գɱ����к�!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100180 && OldMetamoId <100200){           //�̷�����
				NewMetamoId=OldMetamoId-160;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գ�������!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100200 && OldMetamoId <100220){           //����Ů
				NewMetamoId=OldMetamoId-100;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գɴ�ֻ��!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100220 && OldMetamoId <100240){           //����
				NewMetamoId=OldMetamoId-160;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գɿ��!", CHAR_COLORYELLOW);
		}else if(OldMetamoId >=100700 && OldMetamoId <100710){          //������
				NewMetamoId=OldMetamoId+60;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գɶ�����!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100710 && OldMetamoId <100720){           //������
				NewMetamoId=OldMetamoId+80;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գɶ̷�����!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100720 && OldMetamoId <100730){           //�����к�
				NewMetamoId=OldMetamoId+60;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գ�ñ����!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100730 && OldMetamoId <100740){           //���
				NewMetamoId=OldMetamoId+80;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գ�����!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100740 && OldMetamoId <100750){           //��Ƥ��
				NewMetamoId=OldMetamoId+30;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գ���Ƥ��!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100750 && OldMetamoId <100760){           //��ֻ��
				NewMetamoId=OldMetamoId+50;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գ�����Ů!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100760 && OldMetamoId <100770){           //������
				NewMetamoId=OldMetamoId-60;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գɶ�����!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100770 && OldMetamoId <100780){           //��Ƥ��
				NewMetamoId=OldMetamoId-30;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գ���Ƥ��!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100780 && OldMetamoId <100790){           //ñ����
				NewMetamoId=OldMetamoId-60;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գɱ����к�!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100790 && OldMetamoId <100800){           //�̷�����
				NewMetamoId=OldMetamoId-80;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գ�������!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100810 && OldMetamoId <100820){           //����Ů
				NewMetamoId=OldMetamoId-50;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գɴ�ֻ��!", CHAR_COLORYELLOW);
		}else if(OldMetamoId>=100820 && OldMetamoId <100830){           //����
				NewMetamoId=OldMetamoId-80;
				CHAR_talkToCli( charaindex, -1, "��ϲ����Գɿ��!", CHAR_COLORYELLOW);
		}else{
				CHAR_talkToCli( charaindex, -1, "�㲢�����������ͣ��޷�����!", CHAR_COLORYELLOW);
				return;}
		if(NewMetamoId==0)return;
		CHAR_setInt( charaindex , CHAR_BASEIMAGENUMBER , NewMetamoId );
		CHAR_setInt( charaindex , CHAR_BASEBASEIMAGENUMBER , NewMetamoId );
		
		CHAR_complianceParameter( charaindex );
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
		CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
		CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _GM_ITEM
void ITEM_GMFUNCTION( int charaindex, int toindex, int haveitemindex)
{
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char *itemarg="\0";
		char gmtime[16];
		char gmfunction[16];
		char token[64];
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		if( itemarg == "\0" ) return;

		getStringFromIndexWithDelim(itemarg,"|", 1, gmfunction, sizeof(gmfunction));
		getStringFromIndexWithDelim(itemarg,"|", 2, gmtime, sizeof(gmtime));
		CHAR_setChar( charaindex , CHAR_GMFUNCTION, gmfunction );
		CHAR_setInt( charaindex , CHAR_GMTIME, atoi(gmtime) );
		sprintf( token, "���ʹ��%sȨ��%d!", gmfunction, atoi(gmtime));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
		sprintf( token, "���ʹ����鿴[help %s]����GM����!",CHAR_getChar( charaindex, CHAR_GMFUNCTION));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );		
		CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _VIP_SERVER
void ITEM_AddMemberPoint( int charaindex, int toindex, int haveitemindex)
{
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char *itemarg="\0";
		char token[64];
		char fd;
		int point=CHAR_getInt( charaindex , CHAR_AMPOINT);
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		if( itemarg == "\0" ) return;
		CHAR_setInt( charaindex , CHAR_AMPOINT, point+atoi(itemarg));
		sprintf( token, "��û�Ա����%d,Ŀǰ��ӵ�л�Ա����Ϊ%d!", atoi(itemarg),point+atoi(itemarg));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );		
		CHAR_DelItem( charaindex, haveitemindex);
		CHAR_charSaveFromConnect(fd, FALSE);
		CHAR_talkToCli( charaindex, -1, "���������Ա���", CHAR_COLORRED );
}
#endif

#ifdef _VIP_RIDE
void ITEM_VipRide( int charaindex, int toindex, int haveitemindex)
{
		if(CHAR_getInt( charaindex , CHAR_VIPRIDE)!=1 
					|| CHAR_getInt( charaindex , CHAR_LOWRIDEPETS)&RIDE_PET14!=RIDE_PET14){
			int LRCode = CHAR_getInt( charaindex, CHAR_LOWRIDEPETS);
			LRCode = LRCode|RIDE_PET14;
			CHAR_setInt( charaindex, CHAR_LOWRIDEPETS, LRCode);
			CHAR_sendStatusString( charaindex, "x");
			CHAR_setInt( charaindex , CHAR_VIPRIDE, 1);
			CHAR_talkToCli( charaindex, -1, "��ϲ�㣬���ѳ�Ϊ��ʽ��Ա�ˣ�", CHAR_COLORYELLOW );		
			CHAR_DelItem( charaindex, haveitemindex);
		}else
			CHAR_talkToCli( charaindex, -1, "�����ǻ�Ա�ˣ�û��Ҫ��ʹ�ã�", CHAR_COLORRED );
}
#endif

#ifdef _FM_ITEM
void ITEM_AddFame( int charaindex, int toindex, int haveitemindex)
{
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char *itemarg="\0";
		char token[64];
		int fame=CHAR_getInt( charaindex , CHAR_FAME);
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		if( itemarg == "\0" ) return;
		CHAR_setInt( charaindex , CHAR_FAME, fame+atoi(itemarg)*100);
		sprintf( token, "������������%d,Ŀǰ���������%d!", atoi(itemarg),fame/100+atoi(itemarg));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );		
		CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _LUCK_ITEM
void ITEM_Luck( int charaindex, int toindex, int haveitemindex)
{
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		int i;
		char *itemarg="\0";
		char token[64];
		char luck[][5]={"��","һ��","С��","�м�","��"};
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		for(i=0;i<5;i++)
			if(strstr( itemarg, luck[i]))
				break;
		if(i==6)i=0;
		CHAR_setInt( charaindex , CHAR_LUCK, i+1);
		sprintf( token, "�����ڵ�����Ϊ%s", luck[i]);
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );		
		CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _ITEM_METAMO_TIME
void ITEM_MetamoTime( int charaindex, int toindex, int haveitemindex )
{
	char *itemarg="\0", msg[128], buff[32];
	int itemindex, metamoTime, battlemode, metamoNo;

	if( CHAR_CHECKINDEX( charaindex ) == FALSE )	return;
	//print(" PetMetamo_toindex:%d ", toindex);

	if( CHAR_getInt( charaindex, CHAR_RIDEPET ) != -1 ){
		CHAR_talkToCli( charaindex, -1, "�޷���������в��ܱ���", CHAR_COLORYELLOW );
		return;
	}
#ifdef _FIX_METAMORIDE
	if( CHAR_CHECKJOINENEMY( charaindex) == TRUE ){
		CHAR_talkToCli( charaindex, -1, "�޷���������в��ܱ���", CHAR_COLORYELLOW );
		return;
	}
#else
	if( CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER) == 100259 ){
		CHAR_talkToCli( charaindex, -1, "�޷���������в��ܱ���", CHAR_COLORYELLOW );
		return;
	}
#endif
#ifdef _PETSKILL_BECOMEPIG
    if( CHAR_getInt( charaindex, CHAR_BECOMEPIG) > -1 ){//���������
		CHAR_talkToCli( charaindex, -1, "�޷������������в��ܱ���", CHAR_COLORYELLOW );
	    return;
	}
#endif


#ifdef _FIXBUG_ATTACKBOW
	{
		int armindex = CHAR_getItemIndex( charaindex, CHAR_ARM);
		if( ITEM_CHECKINDEX( armindex) == TRUE )	{
			int armtype = BATTLE_GetWepon( charaindex);
			if( armtype == ITEM_BOW || armtype == ITEM_BOUNDTHROW ||
				armtype == ITEM_BREAKTHROW || armtype == ITEM_BOOMERANG)	{
				CHAR_talkToCli( charaindex, -1, "ʹ��Զ���������޷�����", CHAR_COLORYELLOW );
				return;
			}
		}
	}
#endif

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	// ��  ����ݷ�  ������
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return;
	}
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( itemarg == "\0" )return;
	if( getStringFromIndexWithDelim(itemarg,"|", 1, buff, sizeof(buff)) )
		metamoNo=atoi(buff);
	if( getStringFromIndexWithDelim(itemarg,"|", 2, buff, sizeof(buff)) )
		metamoTime=atoi(buff);
	getStringFromIndexWithDelim(itemarg,"|", 3, buff, sizeof(buff));

	CHAR_setWorkInt( charaindex, CHAR_WORKITEMMETAMO, NowTime.tv_sec + metamoTime);
	if(metamoTime>60)
		sprintf( msg, "�����%s���ɳ���%dСʱ��", buff, metamoTime / 60 );
	else
		sprintf( msg, "�����%s���ɳ���%d���ӣ�", buff, metamoTime );

	CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW );

	CHAR_setInt( charaindex, CHAR_BASEIMAGENUMBER, metamoNo );
	CHAR_complianceParameter( charaindex);
	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
	CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);

	CHAR_DelItem( charaindex, haveitemindex);

}
#endif

#ifdef _ITEM_GOLD
void ITEM_Gold( int charaindex, int toindex, int haveitemindex)
{
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char itemarg[10];
		char token[64];
		int gold=CHAR_getInt( charaindex , CHAR_GOLD);
		strcpy(itemarg,ITEM_getChar(itemindex,ITEM_ARGUMENT));
		gold+=atoi(itemarg);
		if(gold>CHAR_MAXGOLDHAVE)
			gold=CHAR_MAXGOLDHAVE;
		CHAR_setInt( charaindex , CHAR_GOLD, gold);
		CHAR_complianceParameter( charaindex);	
	  CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_GOLD);
		sprintf( token, "���ʯ������%dS", atoi(itemarg));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );		
		CHAR_DelItem( charaindex, haveitemindex);
}
#endif


#ifdef _MYSTERIOUS_GIFT
void ITEM_MysteriousGift( int charaindex, int toindex, int haveitemindex)
{
	int itemindex;
	char *itemarg=NULL;
	char petnumstr[32];
	int petnum=0;
	int present[20]={0};
    int si=0, ret,i;
	char token[256];

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

	itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "" ){
		CHAR_talkToCli(charaindex, -1, "���Ǹ���������!", CHAR_COLORYELLOW);
		return;
	}

	if( getStringFromIndexWithDelim( itemarg, "|", 1, petnumstr, sizeof( petnumstr)) == FALSE )
		return;
    petnum = atoi(petnumstr);
	if( petnum > 20 )
		petnum = 20;
	for(i=0;i<petnum;i++){
		if( getStringFromIndexWithDelim( itemarg, "|", 2+i, petnumstr, sizeof( petnumstr)) )
		    present[i] = atoi(petnumstr);
	}
	
	srand(time(0));
	si = rand()%petnum;

	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		if( CHAR_getCharPet( charaindex, i) == -1  )
			break;
	}

  if( i == CHAR_MAXPETHAVE ){
		snprintf( token,sizeof( token), "������������");
		CHAR_talkToCli( charaindex, -1, token,  CHAR_COLORYELLOW);
		return;
	}	
	
	int enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == present[si]) {
			break;
		}
	}
	
	if( i == enemynum )return;
	
	ret = ENEMY_createPetFromEnemyIndex( charaindex, i);
	if(!CHAR_CHECKINDEX( ret))
		return;
	
	sprintf( token,"�õ�%s",ENEMY_getChar( i, ENEMY_NAME));
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
	
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		if( CHAR_getCharPet( charaindex, i ) == ret )break;
	}
	if( i == CHAR_MAXPETHAVE )i = 0;
			
	snprintf( token, sizeof( token ), "K%d", i );
	CHAR_sendStatusString( charaindex, token );
	snprintf( token, sizeof( token ), "W%d", i );
	CHAR_sendStatusString( charaindex, token );
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _BATTLE_PK
void ITEM_BattlePK( int charaindex, int toindex, int haveitemindex)
{
	int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	int playernum = CHAR_getPlayerMaxNum();
	char token[256];
	if( !CHAR_CHECKINDEX( charaindex ) ) return;
	if( !CHAR_CHECKINDEX( toindex ) ) return;
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
	{
	   CHAR_talkToCli( charaindex, -1, "ֻ����ս���Ŷ��", CHAR_COLORYELLOW);
	   return;
	}
	if( charaindex == toindex )
	{
	   CHAR_talkToCli( charaindex, -1, "�ѵ����Լ���ս�Լ�����", CHAR_COLORYELLOW);
	   return;
	}
	CHAR_DischargePartyNoMsg( charaindex );//��ɢ�Ŷ�
	CHAR_DischargePartyNoMsg( toindex );//��ɢ�Ŷ�
	BATTLE_CreateVsPlayer(charaindex,toindex);
	CHAR_setInt( charaindex, CHAR_BATTLEPK,TRUE);
	CHAR_setInt( toindex, CHAR_BATTLEPK,TRUE);
	sprintf( token,"�ɹ���Է�%s��ս!",CHAR_getChar( toindex, CHAR_NAME));
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
	sprintf( token,"�Է�%s��ʽ������ս!",CHAR_getChar( charaindex, CHAR_NAME));
	CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _SILENTZERO
void ITEM_SetSilentZero( int charaindex, int toindex, int haveitemindex)
{
		CHAR_setInt(charaindex,CHAR_SILENT, 0);
		CHAR_setWorkInt( charaindex, CHAR_WORKLOGINTIME, (int)NowTime.tv_sec);
		CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _PET_LEVEL_ITEM
void ITEM_PetLevelItem( int charaindex, int toindex, int haveitemindex)
{
  if( !CHAR_CHECKINDEX( charaindex ) ) return;
  if( !CHAR_CHECKINDEX( toindex ) ) return;
  if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
  {
  	CHAR_talkToCli( charaindex, -1, "����Ʒֻ�ܸ�����ʹ�ã�", CHAR_COLORYELLOW);
    return;
  }
	if( CHAR_getInt( toindex, CHAR_PETID) == 718 || CHAR_getInt( toindex, CHAR_PETID) == 401 )
	{
		CHAR_talkToCli( charaindex, -1, "����Ʒ���ܸ�MMʹ�ã�", CHAR_COLORYELLOW);
		return;
	}
	int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	char *itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	char low[12],hight[12];
	getStringFromIndexWithDelim( itemarg, "|", 1, low, sizeof(low));
	getStringFromIndexWithDelim( itemarg, "|", 2, hight, sizeof(hight));
	if(CHAR_getInt( toindex, CHAR_LV)<atoi(low) && (CHAR_getInt(toindex, CHAR_LIMITLEVEL)>0 && CHAR_getInt(toindex, CHAR_LIMITLEVEL)<atoi(low))){
		CHAR_talkToCli( charaindex, -1, "Ŀǰ��ĳ����޷�ʹ�ø���Ʒ��", CHAR_COLORYELLOW);
		return;
	}else	if(CHAR_getInt( toindex, CHAR_LV)>=atoi(hight) && CHAR_getInt(toindex, CHAR_LIMITLEVEL)>=atoi(hight)){
		CHAR_talkToCli( charaindex, -1, "Ŀǰ��ĳ��ﲻ����ʹ�ø���Ʒ��", CHAR_COLORYELLOW);
		return;
	}
	CHAR_setInt(toindex, CHAR_LIMITLEVEL, atoi(hight));
	if(CHAR_getInt( toindex, CHAR_EXP)<0){
		int nextexp = CHAR_GetLevelExp( toindex, CHAR_getInt(toindex, CHAR_LV));
		CHAR_setInt( charaindex, CHAR_EXP , nextexp );
	}
		
	char token[256];
	sprintf(token, "����%s�ȼ���ͻ��%d����",CHAR_getChar( toindex, CHAR_NAME), atoi(hight));
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _ITEM_EFMETAMO
void ITEM_efMetamo( int charaindex, int toindex, int haveitemindex)
{
	int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

	char *itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "" ){
		CHAR_talkToCli(charaindex, -1, "���Ǹ����ñ����ָ!", CHAR_COLORYELLOW);
		return;
	}
	CHAR_setInt( charaindex , CHAR_BASEIMAGENUMBER , atoi(itemarg) );
	CHAR_setInt( charaindex , CHAR_BASEBASEIMAGENUMBER , atoi(itemarg));

	CHAR_complianceParameter( charaindex );
	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
	CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif

#ifdef _PET_BEATITUDE
void PET_BEATITUDE( int charaindex, int toindex, int haveitemindex)
{
  if( !CHAR_CHECKINDEX( charaindex ) ) return;
  if( !CHAR_CHECKINDEX( toindex ) ) return;
  if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
  {
  	CHAR_talkToCli( charaindex, -1, "����Ʒֻ�ܸ�����ʹ�ã�", CHAR_COLORYELLOW);
    return;
  }
	if( CHAR_getInt( toindex, CHAR_PETID) == 718 || CHAR_getInt( toindex, CHAR_PETID) == 401 )
	{
		CHAR_talkToCli( charaindex, -1, "����Ʒ���ܸ�MMʹ�ã�", CHAR_COLORYELLOW);
		return;
	}
	int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	char *itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	char beatitude[12],mun[12];
	getStringFromIndexWithDelim( itemarg, "|", 1, beatitude, sizeof(beatitude));
	getStringFromIndexWithDelim( itemarg, "|", 2, mun, sizeof(mun));
	char token[256];
	int beat=CHAR_getInt( toindex, CHAR_BEATITUDE);
	if(strcmp( beatitude, "��")==0){
		if(CHAR_getInt( toindex, CHAR_BEATITUDE)&BEATITUDE_VITAL){
			CHAR_talkToCli( charaindex, -1, "���ĳ�������������������ˣ�", CHAR_COLORYELLOW);
			return;
		}else{
			sprintf(token, "%s��������%d", CHAR_getChar( toindex, CHAR_NAME),atoi(mun));
			CHAR_setInt( toindex, CHAR_VITAL,(CHAR_getInt( toindex, CHAR_VITAL)+atoi(mun)));
			beat|=BEATITUDE_VITAL;
		}
	}else if(strcmp( beatitude, "��")==0){
		if(CHAR_getInt( toindex, CHAR_BEATITUDE)&BEATITUDE_STR){
			CHAR_talkToCli( charaindex, -1, "���ĳ�������������������ˣ�", CHAR_COLORYELLOW);
			return;
		}else{
			sprintf(token, "%s����������%d", CHAR_getChar( toindex, CHAR_NAME),atoi(mun));
			CHAR_setInt( toindex, CHAR_STR, (CHAR_getInt( toindex, CHAR_STR)+atoi(mun)));
			beat|=BEATITUDE_STR;
		}
	}else if(strcmp( beatitude, "��")==0){
		if(CHAR_getInt( toindex, CHAR_BEATITUDE)&BEATITUDE_TOUGH){
			CHAR_talkToCli( charaindex, -1, "���ĳ�������������������ˣ�", CHAR_COLORYELLOW);
			return;
		}else{
			sprintf(token, "%s����������%d", CHAR_getChar( toindex, CHAR_NAME),atoi(mun));
			CHAR_setInt( toindex, CHAR_TOUGH, (CHAR_getInt( toindex, CHAR_TOUGH)+atoi(mun)));
			beat|=BEATITUDE_TOUGH;
		}
	}else if(strcmp( beatitude, "��")==0){
		if(CHAR_getInt( toindex, CHAR_BEATITUDE)&BEATITUDE_DEX){
			CHAR_talkToCli( charaindex, -1, "���ĳ�������������������ˣ�", CHAR_COLORYELLOW);
			return;
		}else{
			sprintf(token, "%s����������%d", CHAR_getChar( toindex, CHAR_NAME),atoi(mun));
			CHAR_setInt( toindex, CHAR_DEX, (CHAR_getInt( toindex, CHAR_DEX)+atoi(mun)));
			beat|=BEATITUDE_DEX;
		}
	}else{
		sprintf(token, "%s��ǰ����Ϊ��:%d,��:%d,��:%d,��:%d", CHAR_getChar( toindex, CHAR_NAME),
																													CHAR_getInt( toindex, CHAR_VITAL),
																													CHAR_getInt( toindex, CHAR_STR),
																													CHAR_getInt( toindex, CHAR_TOUGH),
																													CHAR_getInt( toindex, CHAR_DEX));
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
		return;
	}
	CHAR_talkToCli( charaindex, -1, "���ˣ��ⶫ��̫���ˣ����ѳ��������ˣ�", CHAR_COLORYELLOW);
	CHAR_setInt( toindex, CHAR_BEATITUDE,beat);
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
	int i;
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		if( CHAR_getCharPet( charaindex, i ) == toindex )break;
	}
	if( i == CHAR_MAXPETHAVE )return;
	CHAR_complianceParameter( toindex );
	snprintf( token, sizeof( token ), "K%d", i );
	CHAR_sendStatusString( charaindex, token );
	snprintf( token, sizeof( token ), "W%d", i );
	CHAR_sendStatusString( charaindex, token );

	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
	CHAR_DelItem( charaindex, haveitemindex);
}
#endif




#ifdef _GET_MULTI_ITEM
void ITEM_GetMultiItem( int charaindex, int toindex, int haveitemindex)
{
	int itemindex;
	char *itemarg=NULL;
	char buf[32];
	int itemnum=0;
	int itemid={-1};
	char itemname[512]="";
  int si=0, ret,i;
	char token[256];

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;

	itemarg = ITEM_getChar( itemindex, ITEM_ARGUMENT);
	if( itemarg == "" ){
		CHAR_talkToCli(charaindex, -1, "���Ǹ��պ���!", CHAR_COLORYELLOW);
		return;
	}

	if( getStringFromIndexWithDelim( itemarg, "|", 1, buf, sizeof( buf)) == FALSE ){
		CHAR_talkToCli(charaindex, -1, "��������!", CHAR_COLORYELLOW);
		return;
	}
  itemnum = atoi(buf);
	if( itemnum > 10 ){
		itemnum = 10;
	}else if( itemnum < 0 ){
		CHAR_talkToCli(charaindex, -1, "���Ǹ��պ���!", CHAR_COLORYELLOW);
		return;
	}
	if( (itemnum - 1) <= CHAR_findSurplusItemBox( charaindex )){
		CHAR_DelItem( charaindex, haveitemindex);
	}else{
		sprintf( token, "��������%d����Ʒ�������Ʒ��λ���㡣", itemnum);
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
		return;
	}

	for(i=0;i<itemnum;i++){
		if( getStringFromIndexWithDelim( itemarg, "|", 2+i, buf, sizeof( buf)) == FALSE ){
			CHAR_talkToCli(charaindex, -1, "��������!", CHAR_COLORYELLOW);
			return;
		}
		itemid = atoi(buf);
		itemindex = ITEM_makeItemAndRegist( itemid );
		if( itemindex != -1 ){
			int emptyteimbox=CHAR_findEmptyItemBox( charaindex );
		  CHAR_setItemIndex( charaindex, emptyteimbox, itemindex );
		  ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
		  ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charaindex);
		  CHAR_sendItemDataOne( charaindex, emptyteimbox);
		  strcat(itemname,ITEM_getChar( itemindex, ITEM_NAME));
		  strcat(itemname,"|");
		}
	}
	sprintf( token,"�����Ʒ:|%s",itemname);
	CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
}
#endif

void ITEM_NewName( int charaindex, int toindex, int haveitemindex)
{
		int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
		char *itemarg="\0";
		char token[64];
		itemarg=ITEM_getChar(itemindex,ITEM_ARGUMENT);
		if( itemarg == "\0" ) return;
		CHAR_setChar( charaindex , CHAR_NEWNAME, itemarg);
		sprintf( token, "��ϲ��,���ѻ�á�%s���³ƺ�!", itemarg);
		CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );		
		int i;
		int playernum = CHAR_getPlayerMaxNum();
		char NameMsg[256];
		char *MyName = CHAR_getChar( charaindex,CHAR_NAME );
		for( i = 0 ; i < playernum ; i++) 
				{
        	sprintf( NameMsg, "��ϲ��ҡ�%s����á�%s�۳ƺ�!", MyName, itemarg );
					CHAR_talkToCli( i, -1, NameMsg, CHAR_COLORBLUE2);
  			}
		CHAR_DelItem( charaindex, haveitemindex); 
}
