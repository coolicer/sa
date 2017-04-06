#include "version.h"
#include <string.h>
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "item.h"
#include "util.h"
#include "handletime.h"
#include "npc_doorman.h"
#include "npc_door.h"
#include "npcutil.h"
#include "configfile.h"
/*
 *
 *    Ի����ʧ��ؤ������幫����ʧë���վ��������巽�Ȼ�����NPC.
 *  ��ʧ����ئԻ��create���·���ƥ��������ʧë���۽����³��绥ƥ���£�
 *  ��ئ�ƾ�����ľ���������ƽ�ҷ�ë�����ƻ�������Ի8Ѩ������ʧ������
 *  �������������ͻ��帲�ƻ��������£ۿ��׹�����ʧ��  �ݱ巰�ʳ����ئ�£�
 *
 *  �̼����������̵���Talkƥ
 *
 * ��ʧë�����׻���
 *
 * 1 ����ë������£۰��ƥ�����շ�����     gold|100
 * 2 ʧ��  ةë1�۰������ �۰��ƥ�����շ�����  item|45
 * 3 ʧ��  ةë  �Ȼ����¾�������Ʃ���£�   �Ȼ������շ��ʣ�itemhave|44
 * 4 ʧ��  ةë  �Ȼ���ئ�о�������Ʃ���£�  �Ȼ���ئ�������շ��ʣ�
 *          itemnothave|333
 * 5 ��įë���Ȼ����¾�������Ʃ���£�  �Ȼ������շ��ʣ� titlehave|string
 * 6 ��įë���Ȼ���ئ�о�������Ʃ���£�  �Ȼ���ئ�������շ��ʣ�
 *      titlenothave|string
 *
 * ��ئ����ߤ  ��������緰�ʣ��ż���������
 *��100����������׷������ʻ�����ƥ�ʾ�����ƥ�����У����е���100�������
 * ����ľ�£�����ئԻ�����У��������Ȼ�������ľ�£�ƥ����100�������
 * ���׷��������ףۣ����ɴ�ľ�£�
 *
 * ʧ��  ة��谼������������ϡ�ë�������׷������ʻ�����ƥ�ʾ����������ʣ�
 *  3����6�����������Ͼ���ئ�ƾ��껯��������Ƿ�Ȼ����շ��ʣ�
 *
 *
 *
 *    ������  ܷ
 *
 *1  ��ʧë�����絤��  ��
 *2  ��NPCë������ʧ����ئԻ��  �ʣ�¦��ë gold|100 ������
 *3  ��NPC�帲�ƻ���100������������Ȼ���������ƥ�����У����ɵ�
 *4  ��ʧ�������л��Ż�Ӽ��������  ��
 *
 */

static void NPC_DoormanOpenDoor( char *nm  );

BOOL NPC_DoormanInit( int meindex )
{
	char	arg[NPC_UTIL_GETARGSTR_BUFSIZE];
    char dname[1024];

	/* ��ì������������ɬ�� */
	CHAR_setWorkInt( meindex, CHAR_WORKEVENTTYPE,CHAR_EVENT_NPC);

    CHAR_setInt( meindex , CHAR_HP , 0 );
    CHAR_setInt( meindex , CHAR_MP , 0 );
    CHAR_setInt( meindex , CHAR_MAXMP , 0 );
    CHAR_setInt( meindex , CHAR_STR , 0 );
    CHAR_setInt( meindex , CHAR_TOUGH, 0 );
    CHAR_setInt( meindex , CHAR_LV , 0 );

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPETOWNPEOPLE );
    CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );  /*   ����ľئ�з��� */

	NPC_Util_GetArgStr( meindex, arg, sizeof( arg));

    if(!getStringFromIndexWithDelim( arg, "|", 3, dname, sizeof(dname ))){
        print("RINGO: �趨������ʱ��Ҫ�ŵ�����ࡣ�:%s:\n",
              arg );
        return FALSE;
    }
    print( "RINGO: Doorman create: arg: %s dname: %s\n",arg,dname);
    CHAR_setWorkChar( meindex , CHAR_WORKDOORMANDOORNAME , dname );

    return TRUE;
}

void NPC_DoormanTalked( int meindex , int talkerindex , char *msg ,
                     int color )
{
    char mode[128];
    char opt[256];
    char	arg[NPC_UTIL_GETARGSTR_BUFSIZE];

    /* ��������������ʧѨ����1��������  ئ�շ��ϼ��� */
    if(NPC_Util_CharDistance( talkerindex, meindex ) > 1)return;

	NPC_Util_GetArgStr( meindex, arg, sizeof( arg));

    if( !getStringFromIndexWithDelim( arg, "|", 1, mode, sizeof( mode )))
        return;

    if( !getStringFromIndexWithDelim( arg, "|", 2, opt, sizeof( opt ) ))
        return;

    if( strcmp( mode , "gold" ) == 0 ){
        int g = atoi( opt );
        int yn = NPC_Util_YN( msg );
        /*char *nm = CHAR_getChar( meindex , CHAR_NAME );*/
        char msg[256];

        if( g > 0 && yn < 0 ){
            snprintf( msg ,sizeof( msg ) ,
                      "������Ҫ����%d�Ľ�������������", g );
            CHAR_talkToCli( talkerindex, meindex , msg, CHAR_COLORWHITE );
        } else if( g > 0 && yn == 0 ){
            snprintf( msg , sizeof( msg ),
                      "���� %d�Ľ����Ǳ�Ҫ�ġ�", g );
        } else if( g > 0 && yn == 1 ){
            int now_g = CHAR_getInt( talkerindex, CHAR_GOLD );
            if( now_g < g ){
                snprintf( msg , sizeof( msg ) ,
                          "���� %d�Ľ����Ǳ�Ҫ�ġ�", g );
            	CHAR_talkToCli( talkerindex, meindex , msg, CHAR_COLORWHITE );
            } else {
                snprintf( msg , sizeof( msg ),
                          "%d �յ������ˡ����ھ������š�", g );
            	CHAR_talkToCli( talkerindex, meindex , msg, CHAR_COLORWHITE );

                /* ����ë������ */
                now_g -= g;
                CHAR_setInt( talkerindex , CHAR_GOLD , now_g );
                /* ؤ�������е�  ������ë˪�� */
                CHAR_send_P_StatusString(talkerindex, CHAR_P_STRING_GOLD);

                /* ��ʧ������ */
                NPC_DoormanOpenDoor(
                    CHAR_getWorkChar( meindex, CHAR_WORKDOORMANDOORNAME));
            }
        }
    } else if( strcmp( mode , "item" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "����δ֧Ԯģʽ��",
                        CHAR_COLORWHITE);
    } else if( strcmp( mode , "itemhave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "����δ֧Ԯģʽ��",
                        CHAR_COLORWHITE);
    } else if( strcmp( mode , "itemnothave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "����δ֧Ԯģʽ��",
                        CHAR_COLORWHITE);
    } else if( strcmp( mode , "titlehave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "����δ֧Ԯģʽ��",
                        CHAR_COLORWHITE);

    } else if( strcmp( mode , "roomlimit" ) == 0 ){

		/*   �м��������׻�ؤ������ */
		char szOk[256], szNg[256], szBuf[32];
		int checkfloor;
		int maxnum, i, iNum;

	    if( !getStringFromIndexWithDelim( arg, "|", 2, szBuf, sizeof( szBuf ) ))
    	    return;

		/* Ʃ���°׷�ʧ��    ���� */
		if( sscanf( szBuf, "%d:%d", &checkfloor, &maxnum ) != 2 ){
			return;
		}

		for( iNum = 0,i = 0; i < getFdnum(); i ++ ){
			/* ������������½�巴��  ��  �� */
			if( CHAR_getCharUse( i ) == FALSE )continue;
			if( CHAR_getInt( i, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )continue;
			/* ϶�ü��׷�ʧ��½����  ��  �� */
			if( CHAR_getInt( i, CHAR_FLOOR ) != checkfloor )continue;
			iNum++;
		}
	    if( !getStringFromIndexWithDelim( arg, "|", 5, szNg, sizeof( szNg ))){
   			strcpy( szNg, "��������" );	/* ��ɡئ�Ƽ������� */
		}
    	if( !getStringFromIndexWithDelim( arg, "|", 4, szOk, sizeof( szOk ))){
   			strcpy( szOk, "���Űɡ�����" );	/* ��ɡؤԻ�������� */
   		}

		if( iNum >= maxnum ){
			/*     ë�������������� */
	        CHAR_talkToCli( talkerindex, meindex ,szNg, CHAR_COLORWHITE);
		}else{
			/*     ��  ��ئ������ */
	        CHAR_talkToCli( talkerindex, meindex ,szOk, CHAR_COLORWHITE);
            NPC_DoormanOpenDoor(
                    CHAR_getWorkChar( meindex, CHAR_WORKDOORMANDOORNAME));
		}

    } else if( strcmp( mode , "titlenothave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "����δ֧Ԯģʽ��",
                        CHAR_COLORWHITE);
    }
}

/*
 *    �ƥ�����ƻ����������׼�ë���ͻ����ʣ�
 *
 */
static void NPC_DoormanOpenDoor( char *nm)
{
    int doori = NPC_DoorSearchByName( nm );
    print( "RINGO: Doorman's Door: index: %d\n", doori );

    NPC_DoorOpen( doori , -1 );

}

