#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "net.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_exchangeman.h"
#include "npc_eventaction.h"
#include "log.h"
#include "battle.h"
#include "handletime.h"
#include "enemy.h"
#include "npc_warp.h"

#ifdef _TRANS_7_NPC
BOOL NPC_EventTRANS(int meindex, int talker, char *buff2,int mode);
#endif
static void NPC_ExChangeMan_selectWindow( int meindex, int talker,int num);
BOOL NPC_TypeCheck(int meindex,int talker,char *szMes);
int NPC_ExChangeManEventCheck( int meindex, int talker, char *buff1);
BOOL NPC_EventItemCheck(int meindex,int talker,int itemNo,int flg);
BOOL NPC_EventLevelCheck(int meindex,int talker,int level,int flg);
// Arminius 8.14 move to .h (for pet talk)
//BOOL NPC_EventBigSmallCheck(int meindex,int talker,char* buf);
BOOL NPC_EventFreeIfCheck(int meindex,int talker,char* buf,int kosuu,int flg);
BOOL NPC_EventBigSmallLastCheck(int point1,int mypoint,int flg);
BOOL NPC_ENDEventNoCheck(int meindex,int talker,int shiftbit,int flg);
BOOL NPC_NOWEventNoCheck(int meindex,int talker,int shiftbit,int flg);
BOOL NPC_TiemCheck(int meindex,int talker,int time,int flg);
BOOL NPC_EventAddPet(int meindex, int talker, char *buff2,int mode);
BOOL NPC_RandItemGet(int meindex, int talker,int rand_j,char *buf);

BOOL NPC_PetLvCheckType2(int petindex,int meindex,int talker,char *buf,int mode);
BOOL NPC_PetLvCheck(int meindex,int talker,char *buf,int mode);
void NPC_RequestMain(int meindex,int talker,char *buf);
void NPC_AcceptMain(int meindex,int  talker ,char*buf);
void NPC_MsgDisp(int meindex,int talker,int num);
BOOL NPC_EventAdd(int meindex,int talker,int mode);
BOOL NPC_AcceptDel(int meindex,int talker,int mode);
BOOL NPC_SavePointCheck(int meindex,int talker,int shiftbit,int flg);

BOOL NPC_EventReduce(int meindex,int talker,char *buf);
BOOL NPC_EventDelItem(int meindex,int talker,char *buf,int breakflg);
BOOL NPC_EventDelItemEVDEL(int meindex,int talker,char *buf,char *nbuf,int breakflg);
BOOL NPC_EventAddItem(int meindex,int talker,char *buf);
int NPC_EventFile(int meindex,int talker,char *buf);
BOOL NPC_EventDelPet(int  meindex,int  talker, int petsel);
void NPC_PetSkillMakeStr(int meindex,int toindex,int select);
void NPC_CleanMain(int meindex,int talker,char *buf);	
void NPC_EventPetSkill( int meindex, int talker, char *data);
BOOL NPC_ItemFullCheck(int meindex,int talker,char *buf,int mode,int evcnt);
void NPC_CharmStatus(int meindex,int talker);
BOOL NPC_ImageCheck(int meindex,int talker,int kosuu,int flg);
int NPC_EventGetCost(int meindex,int talker,char *arg);

BOOL NPC_EventWarpNpc(int meindex,char *buf);


enum {
	CHAR_WORK_EVENTWARP	= CHAR_NPCWORKINT4,

};

/*********************************
* ������  
*********************************/
BOOL NPC_ExChangeManInit( int meindex )
{

	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024*2];
	int i = 1;
	char buf4[256];

	CHAR_setWorkInt(meindex,CHAR_WORK_EVENTWARP,1);

	/*--NPC��������ë����������--*/
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEEVENT );


	/*--ɬ�ð����̻�ë  ��  ��--*/
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL) {
		print("NPC_ExChange.c TypeCheck: GetArgStrErr\n");
		print("NPCName=%s\n", CHAR_getChar( meindex ,CHAR_NAME));
		return FALSE;
	}

	i = 1;
	/*--��ì����NPC������ʧ��  ةë������������--*/
	/*--����������  ��  ���ƻ������¾�����ʸ����������ƥ˪ľ�¾���--*/
	while(getStringFromIndexWithDelim( argstr ,"EventEnd" ,i ,buf ,sizeof(buf))
	!= FALSE)
	{
		i++;
		/*--���ƥ̤��ľ�����¼����������߼���ë����-*/
		if(strstr( buf,"GetItem") != NULL) {
			if(strstr( buf,"ItemFullMsg") == NULL) {
				NPC_Util_GetStrFromStrWithDelim(buf,"EventNo",buf4,sizeof(buf4));
				print("\n### EventNo=%d %s:ItemFullMsg ����###",
				atoi( buf4),
				CHAR_getChar( meindex, CHAR_NAME));
			}
		}

		if(strstr( buf,"GetPet") != NULL) {
			if(strstr( buf,"PetFullMsg") == NULL) {
				NPC_Util_GetStrFromStrWithDelim(buf,"EventNo",buf4,sizeof(buf4));
				print("\n### ventNo=%d %s:PetFullMsg ���� ###",
				atoi(buf4),
				CHAR_getChar(meindex,CHAR_NAME));
			}
		}

		if(strstr( buf,"GetEgg") != NULL) {
			if(strstr( buf,"PetFullMsg") == NULL) {
				NPC_Util_GetStrFromStrWithDelim(buf,"EventNo",buf4,sizeof(buf4));
				print("\n### ventNo=%d %s:PetFullMsg ���� ###",
				atoi(buf4),
				CHAR_getChar(meindex,CHAR_NAME));
			}
		}

		if(strstr( buf,"GetStone") != NULL){
			if(strstr( buf,"StoneFullMsg") == NULL){
				NPC_Util_GetStrFromStrWithDelim(buf,"EventNo",buf4,sizeof(buf4));
				print("\n### EventNo=%d %s:StonFullMsg ���� ###",
				atoi( buf4),
				CHAR_getChar( meindex, CHAR_NAME));
			}
		}

		if(strstr( buf,"DelStone") != NULL) {
			if(strstr( buf,"StoneLessMsg") == NULL) {
				NPC_Util_GetStrFromStrWithDelim(buf,"EventNo",buf4,sizeof(buf4));
				print("\n### EventNo=%d %s:StoneLessMsg ���� ###",
				atoi(buf4),
				CHAR_getChar(meindex,CHAR_NAME));
			}
		}
	}

	i = 1;

	return TRUE;

}

/*--      ���  --*/
BOOL NPC_EventWarpNpc(int meindex,char *buf)
{
	char token[32];
	char buf2[32];
	int floor,x,y;
	int ret;
	int cnt;
	int dcnt = 1;

	/*--�ػ��������󼰴�  --*/
	cnt = CHAR_getWorkInt(meindex,CHAR_WORK_EVENTWARP);
	while(getStringFromIndexWithDelim(buf,",", dcnt,buf2,sizeof(buf2) )
	 !=FALSE ){
		dcnt++;
	}
	
	if(dcnt <= cnt) cnt=1;


	while(getStringFromIndexWithDelim(buf,",", cnt,buf2,sizeof(buf2)) !=FALSE )
	{
		cnt++;
	    ret=getStringFromIndexWithDelim(buf2,".", 1,token,sizeof(token));
        if( ret == FALSE ) {
        	 continue;
       	}
        floor = atoi( token );

        ret=getStringFromIndexWithDelim(buf2,".", 2,token,sizeof(token));
        if( ret == FALSE ){
          continue;
        }
        x = atoi( token );

        ret=getStringFromIndexWithDelim(buf2,".", 3,token,sizeof(token));
        if( ret == FALSE ) {
         continue;
        }
        y = atoi( token );

		CHAR_setWorkInt(meindex,CHAR_WORK_EVENTWARP, cnt);
        CHAR_warpToSpecificPoint(meindex, floor, x, y);
		return TRUE;
	}
	
	return FALSE;

}


/*********************************
*   �ƾ�����ľ���ݼ���  
*********************************/
void NPC_ExChangeManTalked( int meindex , int talkerindex , char *szMes ,int color )
{
	/* �����������帲�ƻ�����  ɱ���� */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	/*--  �������¾���������--*/
	if(NPC_Util_isFaceToFace( meindex ,talkerindex ,2) == FALSE) {
		/* ���������  ���� */
		if(NPC_Util_isFaceToChara(talkerindex,meindex,1 ) == FALSE) return;
	}
	
	/*--������������������    ë�����--*/
	CHAR_setWorkInt( talkerindex ,CHAR_WORKSHOPRELEVANT, 0);
	CHAR_setWorkInt( talkerindex ,CHAR_WORKSHOPRELEVANTSEC, 0);
	CHAR_setWorkInt( talkerindex ,CHAR_WORKSHOPRELEVANTTRD, 0);

	/*--������  ��������--*/
	NPC_TypeCheck( meindex, talkerindex, szMes);

}

/*------------------------------------------------------------
 *ɬ�ð����̻�ë  ��  �ģ�������ؤ������������  ë����
 *-------------------------------------------------------------*/
BOOL NPC_TypeCheck(int meindex,int talker,char *szMes)
{

	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024*2];
	char buf2[512];
	int i = 1;
	int EvNo;
	int evch;
	
	/*--ɬ�ð����̻�ë  ��  ��--*/
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("NPC_ExChange.c TypeCheck: GetArgStrErr\n");
		print("NPCName=%s\n", CHAR_getChar(meindex,CHAR_NAME));
		return FALSE;
	}

	/*-- ɬ�ð����̻�ë������� --*/
	while(getStringFromIndexWithDelim( argstr,"EventEnd",i,buf,sizeof( buf))
	!= FALSE){
		i++;
		CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANT, 0);
		CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 0);

		/*-- ��ì�����ϼ����ë������ --*/
		if(NPC_Util_GetStrFromStrWithDelim(buf,"EventNo",
								buf2, sizeof( buf2) ) == NULL) continue;

		if(strstr( buf2, "-") != 0) {
			EvNo = -1;
		}else{
			EvNo = atoi( buf2);
		}

		/*-- �׷º���������ë�浤����ƥ����ì��������  �ƻ������������ݳ� --*/
		if(NPC_EventCheckFlg( talker, EvNo) == TRUE) continue;

		/*--ƽ����������������ë�浤--*/
		if(NPC_Util_GetStrFromStrWithDelim( buf,"KeyWord", buf2,sizeof( buf2) ) 
		 != NULL){
			if(strcmp( szMes, buf2) != 0) continue;
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANT, i);
		}

		/*--ʸ������  󡼰�������ͻ�  �Ȼ��¾�ë��������--*/
		if(NPC_Util_GetStrFromStrWithDelim( buf,"Pet_Name", buf2,sizeof( buf2)) != NULL){
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 1);
		}
		evch = NPC_ExChangeManEventCheck( meindex, talker, buf);
		if(evch != -1) {
			if(NPC_Util_GetStrFromStrWithDelim( buf,"TYPE", buf2, sizeof( buf2)) != NULL){
				
				if( strstr( buf2, "REQUEST") != NULL) {
					NPC_RequestMain( meindex, talker, buf);
					return TRUE;
				}else if( strstr( buf2, "ACCEPT") != NULL) {
					/*-��  ë��������  --*/
					NPC_AcceptMain( meindex, talker, buf);
					return TRUE;
					
				}else if(strstr( buf2, "MESSAGE") != NULL) {
					char token[14*100];
					char token2[14*50];
					if(NPC_Util_GetStrFromStrWithDelim( buf,"NomalMsg",	token,sizeof( token) )
						!= NULL ) {
						if(NPC_AcceptDel( meindex, talker, 0) == FALSE) return FALSE;
						if(EvNo != -1)
							NPC_NowEventSetFlg( talker, EvNo);
						CHAR_talkToCli( talker, meindex, token, CHAR_COLORWHITE);
						if(NPC_Util_GetStrFromStrWithDelim( buf,"NpcWarp", token2,sizeof( token2) )
							!= NULL){
							NPC_EventWarpNpc(meindex,token2);
						}
						if(NPC_Util_GetStrFromStrWithDelim( buf, "EndSetFlg", buf2, sizeof( buf2) )
							!=NULL){
							char buf3[16];
							int k = 1;
							if(EvNo != -1) NPC_NowEventSetFlgCls( talker, EvNo);
							while(getStringFromIndexWithDelim(buf2 , "," ,k, buf3, sizeof(buf3))
								!=FALSE ){	
								k++;
								NPC_EventSetFlg( talker, atoi( buf3));
							}
						}
						if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlg",
							buf2, sizeof( buf2)) != NULL) {
							char buf3[16];
							int k = 1;
							k= 1 ;
							while(getStringFromIndexWithDelim(buf2 , "," , k, 
								buf3, sizeof(buf3))
								!= FALSE ){
								NPC_NowEndEventSetFlgCls( talker, atoi(buf3));
								k++;
							}
						}
					}else if(NPC_Util_GetStrFromStrWithDelim(buf, "NomalWindowMsg",
						token, sizeof( token) )
						!=NULL) {
						/*--���̼�����ƥ����������ë߯��--*/
						int fd = getfdFromCharaIndex( talker);
#ifdef _NEWEVENT
						if(NPC_Util_GetStrFromStrWithDelim(buf, "NomalWindowMsg1",
							buf2, sizeof(buf2)) != NULL){
							lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
						}else {
#endif
							char token2[14*50];
							if(NPC_AcceptDel( meindex, talker,0) == FALSE) return FALSE;
							if( EvNo != -1) NPC_NowEventSetFlg( talker, EvNo);
							
							if(NPC_Util_GetStrFromStrWithDelim( buf,"NpcWarp",
								token2,sizeof( token2) ) != NULL){
								NPC_EventWarpNpc(meindex,token2);
							}
							if(NPC_Util_GetStrFromStrWithDelim( buf, "EndSetFlg", 
								buf2, sizeof( buf2) ) !=NULL){
								char buf3[16];
								int k = 1;
								if(EvNo != -1) NPC_NowEventSetFlgCls( talker, EvNo);
								while(getStringFromIndexWithDelim(buf2 , "," ,k, 
									buf3, sizeof(buf3)) !=FALSE ){	
									k++;
									NPC_EventSetFlg( talker, atoi( buf3));
								}
							}
							//��ì�����׷º�ë����
							if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlg",
								buf2, sizeof( buf2)) != NULL) {
								char buf3[16];
								int k = 1;
								
								k= 1 ;
								while(getStringFromIndexWithDelim(buf2 , "," , k, 
									buf3, sizeof(buf3))	!= FALSE ){
									NPC_NowEndEventSetFlgCls( talker, atoi(buf3));
									k++;
								}
							}
							lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_YES,
								CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
#ifdef _NEWEVENT
						}
#endif
					}
					
					return TRUE;
					
				}else if(strstr( buf2, "CLEAN") != NULL) {
					NPC_CleanMain( meindex, talker, buf);
					return TRUE;
				}
			}
		}
	}

	strcpy( buf, "\0");
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "NomalMainMsg", buf, sizeof( buf)) == NULL )	{
		print("\n err:NOT FIND NomalMainMsg");
		return FALSE;
	}
	i = 1;
	while(getStringFromIndexWithDelim( buf, ",", i, buf2, sizeof( buf2))!= FALSE){
		i++;
	}
	i--;
	i = rand()%i + 1;
	getStringFromIndexWithDelim( buf,",", i, buf2, sizeof( buf2));
	CHAR_talkToCli( talker, meindex, buf2,  CHAR_COLORWHITE);
	return TRUE;
}


/*-----------------------------------
 *������  ����  ë������
 ------------------------------------*/
static void NPC_ExChangeMan_selectWindow( int meindex, int talker,int num)
{

	switch( num) {
		/*--���;޵�����--*/
		  case 0:
		  /*-��ì����  ئ��ƥ����ì����ë�����¾�������ë  ��--*/
		  	NPC_MsgDisp( meindex, talker, 0);
			break;

		  case 1:
		  /*--ޥ�����ì�����浤--*/
	  		NPC_MsgDisp( meindex, talker, 5);
		  	break;

			/*--ʧ�ͱ�������--*/
		  case 2:
			/*--������Ի--*/
			NPC_MsgDisp( meindex, talker, 6);
			break;

		  case 3:
			/*--������Ի--*/
			NPC_MsgDisp( meindex, talker, 11);
			break;
	}

}

/*-------------------------------------
 *��ì����ë��  ������  �����̼�
 --------------------------------------*/
void NPC_RequestMain(int meindex,int talker,char *buf)
{
	int shiftbit;
	char buf2[128];
		
	/*--��ì�����ϼ����ë������--*/
	NPC_Util_GetStrFromStrWithDelim( buf,"EventNo", buf2,sizeof( buf2) );
	shiftbit = atoi( buf2);

	if(NPC_NowEventCheckFlg( talker, shiftbit) == TRUE) {
		NPC_ExChangeMan_selectWindow( meindex, talker, 0);
	}else{
		NPC_ExChangeMan_selectWindow( meindex, talker, 1);
	}
}

/*---------------------------------------
 * ��ì������  �����̼�
 ----------------------------------------*/
void NPC_AcceptMain(int meindex,int  talker ,char *buf)
{

	NPC_ExChangeMan_selectWindow( meindex, talker, 2);
}

/*----------------------------------------
 *  �׷º�ë����ʧ���¿м����̼�
 ----------------------------------------*/
void NPC_CleanMain(int meindex,int talker,char *buf)
{
	NPC_ExChangeMan_selectWindow( meindex, talker, 3);
}

/*-----------------------------------------
�ͷ���ʧ��������߯�Ȼ������ݱ���̫���ľ�£�
-------------------------------------------*/
void NPC_ExChangeManWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	/*--       ������Ի����ئ�����巴��   --*/
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) {
		return;
	}


	switch( seqno) {

		/*--�׷º�ë�����ݼ����̼�����--*/
	  case CHAR_WINDOWTYPE_WINDOWEVENT_CLEANMSG:
		if( select == WINDOW_BUTTONTYPE_YES) {
			NPC_MsgDisp( meindex, talkerindex, 12);
		}else if(select == WINDOW_BUTTONTYPE_NO) {
		}
		break;

#ifdef _NEWEVENT
	  case CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG:
		if(select == WINDOW_BUTTONTYPE_NEXT) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
				CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD) + 1);
			NPC_MsgDisp( meindex, talkerindex, 99);
		}else if(select == WINDOW_BUTTONTYPE_PREV) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
				CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD) - 1);
			NPC_MsgDisp( meindex, talkerindex, 99);
		}
		break;
#else
	  case CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG:
		break;
#endif
		/*--���;޵�����ؤԻ���絤����������--*/
	  case CHAR_WINDOWTYPE_WINDOWEVENT_REQTHANK:
			if(select == WINDOW_BUTTONTYPE_NEXT) {
				 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
					CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)+1);
				NPC_MsgDisp( meindex, talkerindex, 22);
			}else if(select == WINDOW_BUTTONTYPE_PREV) {
				 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
					CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)-1);
				NPC_MsgDisp( meindex, talkerindex, 22);
			}
		break;
		
		/*--ʧ�ͱ�������ؤԻ���絤����������--*/
	  case CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANK:
			if(select == WINDOW_BUTTONTYPE_NEXT) {
				 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
					CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)+1);
				NPC_MsgDisp( meindex, talkerindex, 82);
			}else if(select == WINDOW_BUTTONTYPE_PREV) {
				 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
					CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)-1);
				NPC_MsgDisp( meindex, talkerindex, 82);
			}
		break;

		/*--�ػ�����ì����ë�����¾�--*/
	  case CHAR_WINDOWTYPE_WINDOWEVENT_NOWEVENT:
		if(select == WINDOW_BUTTONTYPE_YES) {
			NPC_MsgDisp( meindex, talkerindex, 7);
		}else if(select == WINDOW_BUTTONTYPE_NO){
			NPC_MsgDisp( meindex, talkerindex, 1);
		}
		break;
		
		/*--���;޵��������̼����̼�����--*/
	  case CHAR_WINDOWTYPE_WINDOWEVENT_REQMAINMSG:
		if(select == WINDOW_BUTTONTYPE_YES) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,0);
			NPC_MsgDisp( meindex, talkerindex, 2);
		}else if(select == WINDOW_BUTTONTYPE_NO) {
		
		}else if(select == WINDOW_BUTTONTYPE_NEXT) {
			 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
						CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)+1);
			NPC_MsgDisp( meindex, talkerindex, 55);
		}else if(select == WINDOW_BUTTONTYPE_PREV) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
				CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)-1);
			NPC_MsgDisp( meindex, talkerindex, 55);
		}
		break;

		/*--������Ի�����̼������̼�����--*/
	  case CHAR_WINDOWTYPE_WINDOWEVENT_ACCMAINMSG:
		if(select == WINDOW_BUTTONTYPE_YES) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,0);
#ifdef _NEWEVENT
			NPC_MsgDisp( meindex, talkerindex, 81);
#else
			NPC_MsgDisp( meindex, talkerindex, 8);
#endif
		}else if(select == WINDOW_BUTTONTYPE_NO) {
		}else if(select == WINDOW_BUTTONTYPE_NEXT) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
				CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)+1);
			NPC_MsgDisp( meindex, talkerindex, 66);
		}else if(select == WINDOW_BUTTONTYPE_PREV) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
				CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)-1);
			NPC_MsgDisp( meindex, talkerindex, 66);
		}
		break;

#ifdef _NEWEVENT
	  case CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANKNEW:
		  if(select == WINDOW_BUTTONTYPE_OK){
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,0);
				NPC_MsgDisp( meindex, talkerindex, 8);
		  }else if(select == WINDOW_BUTTONTYPE_NEXT) {
				 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
					CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)+1);
				NPC_MsgDisp( meindex, talkerindex, 81);
		  }else if(select == WINDOW_BUTTONTYPE_PREV) {
				 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
					CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)-1);
				NPC_MsgDisp( meindex, talkerindex, 81);
		  }
		  break;
#endif
		/*--ʸ������  ëܸ�������̼�����--*/
	  case CHAR_WINDOWTYPE_WINDOWPETSKILLSHOP:
			if(CHAR_getWorkInt( talkerindex ,CHAR_WORKSHOPRELEVANTTRD) ==0) {
				NPC_EventPetSkill( meindex, talkerindex, data);
				NPC_MsgDisp( meindex, talkerindex, 25);
			}else{
				NPC_EventPetSkill( meindex, talkerindex, data);
				NPC_MsgDisp( meindex, talkerindex, 88);
			}
		break;
	}
}


/*-------------------------
 *  ����ë�����ƻ����¾�
 --------------------------*/
int NPC_ExChangeManEventCheck( int meindex, int talker, char *buff1)
{
	char buff2[512];
	char buff3[128];
	char buf[512];
	int i = 1,j = 1;
	int loop = 0;
	
	NPC_Util_GetStrFromStrWithDelim( buff1, "EVENT", buf, sizeof( buf));

	/*-- EVENt������ë��  ��έ������ --*/
	while( getStringFromIndexWithDelim(buf, ",", i, buff2,sizeof( buff2))
	 !=FALSE )
	{
		i++;
		/*-- "&"��ؤ�¼�ƥ  ������ --*/
		if(strstr( buff2, "&")!=NULL){
			j = 1;
			loop = 0;
			while(getStringFromIndexWithDelim(buff2, "&", j, buff3, sizeof( buff3))
			 != FALSE )
			{
				j++;
				if(NPC_EventBigSmallCheck( meindex, talker, buff3) == FALSE)
				{
					loop = 1;
					break;
				}
			}
				
			if(loop == 0){
				i--;
				return i;
			}
		/*--���������繴--*/
		}else{
			if(NPC_EventBigSmallCheck( meindex, talker, buff2) == TRUE) {
				i--;
				return i;
			}
		}
	
	}

	return -1;

}

/*---------------------------------
 *   ����į��������
 --------------------------------*/
BOOL NPC_EventBigSmallCheck(int meindex,int talker, char* buf)
{

	char buff2[128];
	int kosuu;
	
	/*--ʸ�������������ͻ������Ȼ���������������PET����ë������������ --*/
	if(strstr( buf, "PET") != NULL) {
		if(strstr( buf, "EV") != NULL) {
			if(NPC_PetLvCheck( meindex, talker, buf, 1) != FALSE) return TRUE;
		}else{
			if(NPC_PetLvCheck( meindex, talker, buf, 0) != FALSE) return TRUE;
		}
		return FALSE;
	}

	/*--��Ʊ����  ɧ����į�������� --*/
	if(strstr( buf, "<") != NULL) {
		getStringFromIndexWithDelim( buf, "<", 2, buff2, sizeof( buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "<", 1, buff2, sizeof( buff2));

		if(NPC_EventFreeIfCheck( meindex, talker, buff2, kosuu, 1) == TRUE) {
			return TRUE;
		}

	}else if(strstr( buf, ">") != NULL) {
		getStringFromIndexWithDelim( buf, ">", 2, buff2, sizeof(buff2));
		kosuu = atoi(buff2);
		getStringFromIndexWithDelim( buf, ">" ,1, buff2, sizeof(buff2));

		if(NPC_EventFreeIfCheck( meindex, talker, buff2, kosuu, 2)==TRUE) {
			return TRUE;
		}

	}else if(strstr( buf, "!=" ) != NULL) {
		getStringFromIndexWithDelim( buf, "!=", 2, buff2, sizeof( buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "!=", 1, buff2, sizeof( buff2));
		if(NPC_EventFreeIfCheck( meindex, talker, buff2, kosuu, 3) == TRUE) {
			return TRUE;
		}
		
	}else if(strstr( buf, "=") != NULL) {
		getStringFromIndexWithDelim( buf, "=", 2, buff2, sizeof( buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "=", 1, buff2, sizeof( buff2));

		if(strstr( buf, "*") != NULL) {
			if(NPC_EventReduce( meindex, talker, buf) == TRUE){
				return TRUE;
			}
		}else if(NPC_EventFreeIfCheck( meindex, talker, buff2, kosuu, 0)
		== TRUE)
		{
			return TRUE;
		}
	}
	
	return FALSE;

}


/*-----------------------------------
 * ��  ë  ���ƻ����¾�����������
 *-----------------------------------*/
BOOL NPC_EventFreeIfCheck(int meindex,int talker,char* buf,int kosuu,int flg)
{

	/*--��ì����������--*/
	if(strcmp( buf, "LV") == 0) {
		if(NPC_EventLevelCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}

	/*--ʧ��  ة��������--*/
	if(strcmp( buf, "ITEM") == 0) {
		if(NPC_EventItemCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}

	/*--��  ��ì������������--*/
	if(strcmp( buf, "ENDEV") == 0) {
		if(NPC_ENDEventNoCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}

	if(strcmp( buf, "NOWEV") == 0) {
		if(NPC_NOWEventNoCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}

	if(strcmp( buf, "SP" ) == 0) {
		if(NPC_SavePointCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}

	if(strcmp( buf, "TIME") == 0) {
		if(NPC_TiemCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}

	if(strcmp( buf, "IMAGE") == 0) {
		if(NPC_ImageCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}

#ifdef _ADD_reITEM
	if(strstr( buf, "reITEM" ) != NULL) {
		int i,count=0,itemindex=-1;
		for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){
			itemindex = CHAR_getItemIndex( talker , i );
			if( !ITEM_CHECKINDEX( itemindex) )
				++count;
		}
		if(NPC_EventBigSmallLastCheck( kosuu, count, flg) == TRUE)
			return TRUE;
	}
#endif

	return FALSE;

}

/*--------------------------------
 *  ʸ��������ì��ë������������
 ---------------------------------*/
BOOL NPC_PetLvCheck(int meindex,int talker,char *buf,int mode)
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buff2[16];
	char buf3[8];
	int petno;
	int baseno;
	int petlevel = 0;
	int flg=0;
	int mypetlevel = 0;
	int i = 0;
	int petindex;
	char name[512];
	int cnt = 0;
	int loop;
	

	/*--϶�ý�ľ��ʸ�������ϼ����  ID)ë��  --*/
	getStringFromIndexWithDelim( buf, "-", 2, buff2, sizeof( buff2));
	if(strstr(buff2,"*") != NULL) {
		getStringFromIndexWithDelim( buff2, "*", 1, buf3, sizeof( buf3));
		petno = atoi( buf3);
		getStringFromIndexWithDelim( buff2, "*", 2, buf3, sizeof( buf3));
		loop = atoi( buf3);

	}else{
		petno = atoi( buff2);
		loop = 1;

	}

	/*--ʸ����  �����л����¾�����������--*/
	for(i=0; i < CHAR_MAXPETHAVE ; i++) {
		if(loop == cnt) return TRUE;
		
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )  continue;
	
		/*--ʸ������IDë��  �ƻ�����--*/
		baseno = CHAR_getInt( petindex, CHAR_PETID);

		/* ʸ������ID����ػ���ƹ����ëƩ���� */
		if( (baseno == petno) && (CHAR_getInt( petindex, CHAR_ENDEVENT)
		 == mode))
		 {
			/*--��ì��ë������������--*/
			mypetlevel = CHAR_getInt( petindex, CHAR_LV);
			getStringFromIndexWithDelim( buf, "-", 1, buff2, sizeof( buff2));

			if(strstr( buf, "<") != NULL) {
				flg = 1;
				getStringFromIndexWithDelim( buff2, "<", 2, buf3, sizeof(buf3));
			}else if(strstr( buf, ">") != NULL) {
				getStringFromIndexWithDelim( buff2, ">", 2, buf3, sizeof(buf3));
				flg = 2;
			}else if(strstr( buf, "=") != NULL) {
				getStringFromIndexWithDelim( buff2, "=", 2, buf3, sizeof(buf3));
				flg = 0;
			}

			petlevel = atoi( buf3);

			if(NPC_EventBigSmallLastCheck( petlevel, mypetlevel, flg) == TRUE) {
				/*--ʸ������  󡻥ɬ�ý�ľ��������  󡼰��������  ��--*/
				if(CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC)==1) {
					char *usename;
					
					/*--ɬ�ð����̻Ｐ  ��  ��--*/
					if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
						print("PetLevel:GetArgStrErr");
						return FALSE;
					}

					usename = CHAR_getUseName( petindex);
					NPC_Util_GetStrFromStrWithDelim( argstr, "Pet_Name",
													name,sizeof( name));
	
					if(strcmp( name, usename) == 0)
					{
						cnt++;
						continue;
					}
				}else {
					CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 0);
					cnt++;
					continue;
				}
			}
		}
	}
	
	if(loop == cnt) return TRUE;
	if(i == CHAR_MAXPETHAVE) return FALSE;

	return FALSE;
}


/*--------------------------------
 *  ʸ��������ì��ë������������
 ---------------------------------*/
int NPC_PetLvCheckType2(int petindex,int meindex,int talker,char *buf,int mode)
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buff2[16];
	char buf3[8];
	int petno;
	int baseno;
	int petlevel = 0;
	int flg = 0;
	int mypetlevel = 0;
	char name[512];
	
	/*--϶�ý�ľ��ʸ�������ϼ����  ID)ë��  --*/
	getStringFromIndexWithDelim( buf, "-", 2, buff2, sizeof( buff2));
	if(strstr(buff2,"*") != NULL) {
		getStringFromIndexWithDelim( buff2, "*", 1, buf3, sizeof( buf3));
		petno = atoi( buf3);
	}else{
		petno = atoi( buff2);
	}

	
	/*--ʸ������IDë��  �ƻ�����--*/
	baseno = CHAR_getInt( petindex, CHAR_PETID);

	/*--ʸ������ID����ػ���ƹ����ëƩ����--*/
	if( (baseno == petno) && (CHAR_getInt( petindex, CHAR_ENDEVENT)
	 == mode))
	{
		/*--��ì��ë������������--*/
		mypetlevel=CHAR_getInt( petindex, CHAR_LV);
		getStringFromIndexWithDelim( buf, "-", 1,buff2,sizeof(buff2));

		if(strstr( buf, "<") != NULL) {
			flg = 1;
			getStringFromIndexWithDelim( buff2, "<", 2,
										 buf3, sizeof(buf3));
		}else if(strstr( buf, ">") != NULL) {
			getStringFromIndexWithDelim( buff2, ">", 2, buf3, sizeof(buf3));
			flg = 2;
		}else if(strstr( buf, "=") != NULL) {
			getStringFromIndexWithDelim( buff2, "=", 2, buf3, sizeof(buf3));
			flg = 0;
		}
		
		petlevel = atoi(buf3);

		if(NPC_EventBigSmallLastCheck( petlevel, mypetlevel, flg)
		 == TRUE)
		{
			/*--ʸ������  󡻥ɬ�ý�ľ��������  󡼰��������  ��--*/
			if(CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC) == 1) {
				char *usename;

				/*--ɬ�ð����̻Ｐ  ��  ��--*/
				if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
					print("PetLevel:GetArgStrErr");
					return FALSE;
				}
				
				usename = CHAR_getUseName( petindex);
				NPC_Util_GetStrFromStrWithDelim(argstr,"Pet_Name",
												name, sizeof( name));
	
				if(strcmp( name, usename) == 0)
				{
					return TRUE;
				}
			}else {
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 0);
				return TRUE;
			}
		}
	}

	return FALSE;

}

/*------------------------
 *��  ������������
 --------------------------*/
BOOL NPC_SavePointCheck(int meindex,int talker,int shiftbit,int flg)
{
	int point;

	point = CHAR_getInt( talker, CHAR_SAVEPOINT);
	
	if( (point & ( 1<< shiftbit))  == ( 1<< shiftbit)) {
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;
}

/*------------------------
 *�̶����⼰��������
 --------------------------*/
BOOL NPC_ImageCheck(int meindex,int talker,int kosuu,int flg)
{
	int image;

	image = CHAR_getInt( talker, CHAR_IMAGETYPE);
	
	if(NPC_EventBigSmallLastCheck( image, kosuu, flg) == TRUE) {
		return TRUE;
	}
	return FALSE;

}


/*-----------------------------------------
 * ���޼���������ë�浤(ؤ����������)
 ------------------------------------------*/
BOOL NPC_TiemCheck(int meindex,int talker,int time,int flg)
{
	LSTIME		nowlstime;
	int now;

	RealTimeToLSTime( NowTime.tv_sec, &nowlstime);
	now = getLSTime( &nowlstime);

	if(NPC_EventBigSmallLastCheck( time, now, flg) == TRUE) {
		return TRUE;
	}
	return FALSE;

}

/*------------------------------
 *--Itemë��������				
 -------------------------------*/
BOOL NPC_EventItemCheck(int meindex,int talker,int itemNo,int flg)
{
	int i;
	int itemindex = -1;
	int id;

	//Change fix ����鴩�������ϵĵ���
	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ) {

		itemindex=CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) ) {
			id=ITEM_getInt( itemindex ,ITEM_ID );

			if(NPC_EventBigSmallLastCheck( itemNo, id, flg) == TRUE) {
				if(flg == 0)	return TRUE;
				continue;
			}else{
				if(flg == 0) continue;
				return FALSE;
			}
		}
	}

	if(flg == 3) return TRUE;

	return FALSE;

}


/*------------------------------
 * levelë��������				
 -------------------------------*/
BOOL NPC_EventLevelCheck(int meindex,int talker,int level,int flg)
{

	int mylevel;
	mylevel = CHAR_getInt(talker,CHAR_LV);

	if(NPC_EventBigSmallLastCheck( level, mylevel, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}



/*------------------------------
 * ENDEventNoë��������
 -------------------------------*/
BOOL NPC_ENDEventNoCheck(int meindex,int talker,int shiftbit,int flg)
{

	if(NPC_EventCheckFlg( talker, shiftbit) == TRUE) {
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;
}


/*------------------------------
 * NOWEventNoë��������
 -------------------------------*/
BOOL NPC_NOWEventNoCheck(int meindex,int talker,int shiftbit,int flg)
{
	if(NPC_NowEventCheckFlg( talker, shiftbit) == TRUE) {
		if(flg == 3) return TRUE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;
}

/*---------------------------------------
 *    ����������
 --------------------------------------*/
BOOL NPC_EventBigSmallLastCheck(int point1,int mypoint,int flg)
{

	if(flg == 0) {
		if(point1 == mypoint) {
			return TRUE;
		}
	}else if(flg == 1) {
		if(mypoint < point1) {
			return TRUE;
		}
	}else if(flg == 2) {
		if(mypoint > point1) {
			return TRUE;
		}
	}else if(flg == 3) {
		if(point1 != mypoint) {
			return TRUE;
		}
	}
	return FALSE;
}


/*---------------------------------
 *����������ë��  ����
 *----------------------------------*/
void NPC_MsgDisp(int meindex,int talker,int num)
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
#ifdef _NEWEVENT
	char buf[4096];
	char token2[4096];
	char buf2[4096];
	char token[4096];
	char tmp[4096];
#else
	char buf[1024*2];
	char token2[50*14];
	char buf2[512];
	char token[512];
	char tmp[32];
#endif
	int i = 1;
	int work = 0;
	int EvNo = 0;
	int fd = getfdFromCharaIndex( talker);
	int buttontype = WINDOW_BUTTONTYPE_OK;
	int windowtype = CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG;
	int evch;
	

	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("MsgDisp:GetArgStrErr");
		print("NPCName=%s\n", CHAR_getChar( meindex, CHAR_NAME));
		return ;
	}
	if(num == 22 || num == 82 || num == 55 || num == 66) {
		int pwork;

		pwork = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
		pwork = (pwork / 100) -1 ;

		getStringFromIndexWithDelim( argstr, "EventEnd", pwork, buf,sizeof( buf));
		
	}else{
		/*ɬ�ð����̻�ë�������*/
		while(getStringFromIndexWithDelim( argstr, "EventEnd", i, buf, sizeof(buf))
		 != FALSE)
		{
			i++;
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 0);
			
			/*--��ì�����ϼ����ë������--*/
			NPC_Util_GetStrFromStrWithDelim( buf, "EventNo", buf2, sizeof( buf2));
			if(strstr( buf2, "-1") != 0) {
				EvNo = -1;
			}else{
				EvNo = atoi( buf2);
			}
			EvNo = atoi( buf2);

			/*--�׷º���������ë�浤����ƥ�������ì��������  �ƻ������������ݳ�--*/
			if(NPC_EventCheckFlg( talker, EvNo) == TRUE) continue;

			/*--ƽ����������������ë�浤--*/
			if(NPC_Util_GetStrFromStrWithDelim( buf,"Pet_Name", buf2,sizeof( buf2)) 
			 != NULL )
			{
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 1);
			}

			/*--ƽ����������������ë�浤--*/
			if(NPC_Util_GetStrFromStrWithDelim( buf,"KeyWord", buf2,sizeof( buf2) ) 
			 != NULL)
			{
					if(CHAR_getWorkInt( talker ,CHAR_WORKSHOPRELEVANT) != i) continue;
			}

			evch = NPC_ExChangeManEventCheck( meindex, talker, buf);

			if(evch != -1) {

			/*--��ì������������������--*/
		//	if(NPC_ExChangeManEventCheck( meindex, talker, buf) == TRUE) {
				break;
			}
		}
	}
	
	//print(" NPC_MsgDisp:%d ", num);
	
	switch( num) {
		  case 0:
			/*�ػ�����ì����ë�����¾�������ë  �ʶ���������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "StopMsg", 
												buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token,sizeof( buf2), buf2);
				buttontype = WINDOW_BUTTONTYPE_YESNO;
				windowtype = CHAR_WINDOWTYPE_WINDOWEVENT_NOWEVENT;
			}
		break;
		
		  case 1:
		  	/*--֧���¾�������ë  ���׽��м�NOë����������������������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "NoStopMsg",
												 buf2, sizeof( buf2))
			 != NULL)
			{
				strcpysafe( token, sizeof(buf2), buf2);

			}
		break;
		
		  case 7:
			/*--�ػ�����ì����ë���������弰����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "EndStopMsg",
												 buf2, sizeof( buf2))
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
			
				/*--��ì����  ë  �Ȼ������������ջ�--*/
				NPC_EventAdd( meindex, talker, 1);
			
				/*--�׷º�ë����--*/
				NPC_NowEventSetFlgCls( talker, EvNo);
			
				/*--Ӭ���׼�ƥ��    ë������--*/
				if(CHAR_getInt( talker, CHAR_CHARM) > 0) {
					CHAR_setInt( talker, CHAR_CHARM, CHAR_getInt(talker,CHAR_CHARM) -1);
					CHAR_complianceParameter( talker );
					CHAR_send_P_StatusString( talker, CHAR_P_STRING_CHARM);
					NPC_CharmStatus( meindex, talker);
				}
			}
		break;
		
		  case 2:
				/*--ؤԻ���絤����������--*/
				if( NPC_Util_GetStrFromStrWithDelim( buf, "ThanksMsg",
													 buf2, sizeof( buf2) )
				 != NULL)
				{
					strcpysafe( token,sizeof( buf2), buf2);
					/*--��ì����  �׷º�ë������--*/
					if(NPC_EventAdd(meindex, talker, 0) != FALSE) {
						NPC_NowEventSetFlg( talker, EvNo);
					}
					else {
						return;
					}
				
				
					if(NPC_Util_GetStrFromStrWithDelim( buf, "EndSetFlg", 
													buf2, sizeof( buf2) )
					!=NULL)
					{
						char buf3[16];
						int k = 1;
						if(EvNo != -1) NPC_NowEventSetFlgCls( talker, EvNo);
						
						while(getStringFromIndexWithDelim(buf2 , "," ,k, 
												buf3, sizeof(buf3))
						 !=FALSE )
						{	
							k++;
							NPC_EventSetFlg( talker, atoi( buf3));
						}
					}

					if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlg",
													 buf2, sizeof( buf2)) 
					 != NULL) {
					 	char buf3[16];
						int k = 1;

					 	k= 1 ;
						while(getStringFromIndexWithDelim(buf2 , "," , k, 
															buf3, sizeof(buf3))
						!= FALSE )
						{
							NPC_NowEndEventSetFlgCls( talker, atoi(buf3));
							k++;
						}
					}

				
				
				}
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, 1);
				work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
				work = work + ( i * 100);
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, work);
				
				if(NPC_Util_GetStrFromStrWithDelim( buf,"NpcWarp",
										token2,sizeof( token2) )
				!= NULL){
					NPC_EventWarpNpc(meindex,token2);
				}

				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, "ThanksMsg2") != NULL) {
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_REQTHANK,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}

		break;

		   case 22:
				
				work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
				work = work % 100;

				if(work == 1){
					if( NPC_Util_GetStrFromStrWithDelim( buf, "ThanksMsg",
														 buf2, sizeof( buf2) )
					 != NULL)
					{
						strcpysafe( token,sizeof( buf2), buf2);
					}

					/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
					if(strstr( buf, "ThanksMsg2") != NULL) {
					
						/*���ƥ˪������*/
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_NEXT,
									CHAR_WINDOWTYPE_WINDOWEVENT_REQTHANK,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									token);
						return;
					}
				
				}else{
					
					sprintf( tmp, "ThanksMsg%d", work);
					if( NPC_Util_GetStrFromStrWithDelim( buf, tmp,
														 buf2, sizeof( buf2) )
					 != NULL) {
						strcpysafe( token,sizeof( buf2), buf2);
					}
	
					work++;
					sprintf( tmp, "ThanksMsg%d", work);
	
	
					/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
					if(strstr( buf, tmp) != NULL) {
						
						/*���ƥ˪������*/
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_NEXT,
									CHAR_WINDOWTYPE_WINDOWEVENT_REQTHANK,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									token);
						return;
					}else{
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_OK,
									CHAR_WINDOWTYPE_WINDOWEVENT_REQTHANK,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									token);
						return;
					}
				}
		break;

		  case 25:
			  /*--ؤԻ���絤����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "ThanksMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token,sizeof( buf2), buf2);
				/*--��ì����  �׷º�ë������--*/
				if(NPC_EventAdd(meindex, talker, 2) != FALSE) {
					NPC_NowEventSetFlg( talker, EvNo);
				}
			}
		break;


		  case 3:
			/*--  ɧ������������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "NomalMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe(token, sizeof( buf2), buf2);
			}
		break;

		  case 4:
		  	/*--ʧ��  ة���������м��ݼ�����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "ItemFullMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe(token, sizeof( buf2), buf2);
			}
		break;

		case 5:
			/*--��  ë���¶˼����̼�������������--*/
				if( NPC_Util_GetStrFromStrWithDelim( buf, "RequestMsg",
													 buf2, sizeof( buf2) )
				 != NULL)
				{
          
          if(strstr(buf2,"%8d") != NULL) {
						char buf3[64];
						int cost;
						
						NPC_Util_GetStrFromStrWithDelim( buf, "DelStone",
													 buf3, sizeof( buf3) );
						cost = NPC_EventGetCost( meindex, talker, buf3);
						
						sprintf(token,buf2,cost);

					}else{
						strcpysafe( token,sizeof( buf2), buf2);
					}
					buttontype = WINDOW_BUTTONTYPE_YESNO;
					windowtype = CHAR_WINDOWTYPE_WINDOWEVENT_REQMAINMSG;
				}

				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, 1);
				work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
				work = work + ( i * 100);
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, work);

				/*--��ʸ���⻥ؤ��������ʾ�������̼�������������ë������--*/
				if(strstr( buf, "RequestMsg2") != NULL) {
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_REQMAINMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
		break;

	   case 55:
				
				work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
				work = work % 100;

				if(work == 1){
					if( NPC_Util_GetStrFromStrWithDelim( buf, "RequestMsg",
														 buf2, sizeof( buf2) )
					 != NULL)
					{
						strcpysafe( token, sizeof( buf2), buf2);
					}

					/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
					if(strstr( buf, "RequestMsg2") != NULL){
						/*���ƥ˪������*/
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_NEXT,
									CHAR_WINDOWTYPE_WINDOWEVENT_REQMAINMSG,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									token);
						return;
					}
				}else{
					
					sprintf( tmp, "RequestMsg%d", work);
					if( NPC_Util_GetStrFromStrWithDelim( buf, tmp,
														 buf2, sizeof( buf2) )
					 != NULL){
						strcpysafe( token,sizeof( buf2), buf2);
							
					}
					work++;
					sprintf( tmp, "RequestMsg%d", work);
					/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
					if(strstr( buf, tmp) != NULL) {
						
						/*���ƥ˪������*/
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_NEXT,
									CHAR_WINDOWTYPE_WINDOWEVENT_REQMAINMSG,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									token);
						return;
					}else{
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_YESNO,
									CHAR_WINDOWTYPE_WINDOWEVENT_REQMAINMSG,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									token);
						return;
					}
				}
		break;


		  case 6:
			  //print(" 6_��Ӧ? ");
			/*--������Ի���˼����̼�������������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "AcceptMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				if(strstr(buf2,"%8d") != NULL) {
					char buf3[64];
					int cost;
						
					NPC_Util_GetStrFromStrWithDelim( buf, "DelStone",
											 buf3, sizeof( buf3) );
					cost = NPC_EventGetCost( meindex, talker, buf3);
						
					sprintf( token, buf2, cost);
				}else{
					strcpysafe( token, sizeof( buf2), buf2);
				}
				buttontype = WINDOW_BUTTONTYPE_YESNO;
				windowtype = CHAR_WINDOWTYPE_WINDOWEVENT_ACCMAINMSG;
			}
	
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, 1);
			work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
			work = work + ( i * 100);
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, work);

			/*--��ʸ���⻥ؤ��������ʾ�������̼�������������ë������--*/
			if(strstr( buf, "AcceptMsg2") != NULL) {
				/*���ƥ˪������*/
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
							WINDOW_BUTTONTYPE_NEXT,
							CHAR_WINDOWTYPE_WINDOWEVENT_ACCMAINMSG,
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							token);
				return;
			}
		break;


	   case 66:
		   //print(" 66_��Ӧ? ");
			work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
			work = work % 100;

			if(work == 1){
				if( NPC_Util_GetStrFromStrWithDelim( buf, "AcceptMsg",
													 buf2, sizeof( buf2) )
				 != NULL)
				{
					strcpysafe( token,sizeof( buf2), buf2);
				}

				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, "AcceptMsg2") != NULL) {
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCMAINMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
			}else{
			
				sprintf( tmp, "AcceptMsg%d", work);
				if( NPC_Util_GetStrFromStrWithDelim( buf, tmp,
													 buf2, sizeof( buf2) )
				 != NULL){
					strcpysafe( token,sizeof( buf2), buf2);
				}
				work++;
				sprintf( tmp, "AcceptMsg%d", work);
				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, tmp) != NULL) {
					
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCMAINMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}else{
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_YESNO,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCMAINMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
			}
		break;

		case 8:
				//print(" 8_��Ӧ? ");
			
				if( NPC_Util_GetStrFromStrWithDelim( buf, "ThanksMsg",
													buf2, sizeof( buf2) )
				 != NULL)
				{
					strcpysafe( token, sizeof( buf2), buf2);
					/*--��ì����  �׷º�ë������--*/
					if(NPC_AcceptDel( meindex, talker,0) == FALSE){
						return ;
					}
					
					if(NPC_Util_GetStrFromStrWithDelim( buf, "EndSetFlg", 
						buf2, sizeof( buf2) )
						!=NULL)
					{
						char buf3[16];
						int k = 1;
						if(EvNo != -1) NPC_NowEventSetFlgCls( talker, EvNo);
						
						while(getStringFromIndexWithDelim(buf2 , "," ,k, 
							buf3, sizeof(buf3))
							!=FALSE )
						{	
							k++;
							NPC_EventSetFlg( talker, atoi( buf3));
						}
					}
					
					if( NPC_Util_GetStrFromStrWithDelim( buf, "Charm",
						buf2, sizeof( buf2))
						!=NULL)
					{
						/*--�����ƥ  ���գ�    ë������---*/
						if( CHAR_getInt( talker, CHAR_CHARM) < 100 && EvNo > 0 ){
							if(CHAR_getInt( talker, CHAR_CHARM) + atoi(buf2) > 100){
								CHAR_setInt( talker, CHAR_CHARM,100);
								
							}else{
								CHAR_setInt( talker, CHAR_CHARM, 
									CHAR_getInt(talker,CHAR_CHARM) + atoi(buf2));
								
							}
							
							CHAR_complianceParameter( talker );
							CHAR_send_P_StatusString( talker, CHAR_P_STRING_CHARM);
							NPC_CharmStatus( meindex, talker);
						}
					}
					
					if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlg",
						buf2, sizeof( buf2)) 
						!= NULL) {
						char buf3[16];
						int k = 1;
						
						k= 1 ;
						while(getStringFromIndexWithDelim(buf2 , "," , k, 
							buf3, sizeof(buf3))
							!= FALSE )
						{
							NPC_NowEndEventSetFlgCls( talker, atoi(buf3));
							k++;
						}
					}
				}
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, 1);
				work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
				work = work + ( i * 100 );
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, work);
	
				if(NPC_Util_GetStrFromStrWithDelim( buf,"NpcWarp",
													token2,sizeof( token2) )
					!= NULL){
						NPC_EventWarpNpc(meindex,token2);
				}
				/*--��ʸ���⻥ؤ��������ʾ�������̼�������������ë������--*/
#ifdef _NEWEVENT
#else
				if(strstr( buf, "ThanksMsg2") != NULL){
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANK,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
#endif
		break;

#ifdef _NEWEVENT
		case 81:
			//print(" 81_��Ӧ? ");

			work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
			work = work % 100;
			sprintf(tmp, "ThanksMsg%d", work + 1);
			if(work == 0 && strstr(buf, tmp) == NULL)
			{
				if(NPC_Util_GetStrFromStrWithDelim(buf, "ThanksMsg",
					buf2, sizeof( buf2)) != NULL)
					strcpysafe( token,sizeof( buf2), buf2);
				if(strstr(buf, "ThanksMsg2") != NULL)
					lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
					WINDOW_BUTTONTYPE_NEXT,
					CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANKNEW,
					CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
					token);
				else
					lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
					WINDOW_BUTTONTYPE_OK,
					CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANKNEW,
					CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
					token);
				return;
			}
			else
			{
				if(NPC_Util_GetStrFromStrWithDelim(buf, tmp,
					buf2, sizeof( buf2)) != NULL)
					strcpysafe( token,sizeof( buf2), buf2);
				work++;
				sprintf(tmp, "ThanksMsg%d", work + 1);
				
				if(strstr(buf, tmp) != NULL)
				{
					lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_NEXT,
						CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANKNEW,
						CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
						token);
					return;
				}
				else
				{
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK,
						CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANKNEW,
						CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
						token);
					return;
				}
			}
			
			break;
#endif	
	
	   case 82:
			work = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTTRD);
			work = work % 100;

			if(work == 1){
				if( NPC_Util_GetStrFromStrWithDelim( buf, "ThanksMsg",
													 buf2, sizeof( buf2))
				 != NULL)
				{
					strcpysafe( token,sizeof( buf2), buf2);
				}

				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, "ThanksMsg2") != NULL){
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANK,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
			}else{
				
				sprintf( tmp, "ThanksMsg%d", work);
				if( NPC_Util_GetStrFromStrWithDelim( buf, tmp,
													 buf2, sizeof( buf2) )
				!= NULL)
				{
					strcpysafe( token,sizeof( buf2), buf2);
				}
				
				work++;
				sprintf( tmp, "ThanksMsg%d", work);

				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, tmp) != NULL) {
						
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANK,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}else{
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_OK,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANK,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
			}
			break;

		case 88:
			/*--ʧ�ͱ�������--*/
			/*--ؤԻ���絤����������--*/
			/*--ʸ������  ��--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "ThanksMsg",
												buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
				/*--��ì����  �׷º�ë������--*/
				if(NPC_AcceptDel( meindex, talker,1) == FALSE){
					return ;
				}
				
				if(NPC_Util_GetStrFromStrWithDelim( buf, "EndSetFlg", 
												buf2, sizeof( buf2) )
				!=NULL)
				{
					char buf3[16];
					int k = 1;
				
					if(EvNo != -1) NPC_NowEventSetFlgCls( talker, EvNo);
					
					while(getStringFromIndexWithDelim(buf2 , "," ,k, 
											buf3, sizeof( buf3))
					 !=FALSE )
					{	
						k++;
						NPC_EventSetFlg( talker, atoi( buf3));
					}
				}

				if( NPC_Util_GetStrFromStrWithDelim( buf, "Charm",
													buf2, sizeof( buf2))
				!=NULL)
				{
					/*--�����ƥ  ���գ�    ë������---*/
					if( CHAR_getInt( talker, CHAR_CHARM) < 100 && EvNo > 0 ){
						if(CHAR_getInt( talker, CHAR_CHARM) + atoi(buf2) > 100){
							CHAR_setInt( talker, CHAR_CHARM,100);
						}else{
							CHAR_setInt( talker, CHAR_CHARM, 
									CHAR_getInt(talker,CHAR_CHARM) + atoi( buf2));
						}
						CHAR_complianceParameter( talker );
						CHAR_send_P_StatusString( talker, CHAR_P_STRING_CHARM);
						NPC_CharmStatus( meindex, talker);
					}
				}
			}

		break;


		  case 9:
			
		  	if( NPC_Util_GetStrFromStrWithDelim( buf, "NomalWindowMsg",
												buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
				if(EvNo != -1) NPC_NowEventSetFlg( talker, EvNo);
			}
#ifdef _NEWEVENT
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, 1);
			work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
			work = work + ( i * 100);
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, work);

			/*--��ʸ���⻥ؤ��������ʾ�������̼�������������ë������--*/
			if(strstr( buf, "NomalWindowMsg2") != NULL) {
				/*���ƥ˪������*/
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
							WINDOW_BUTTONTYPE_NEXT,
							CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG,
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							token);
				return;
			}
#endif
		  	break;
		  	
#ifdef _NEWEVENT
	  case 99:
			work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
			work = work % 100;

			if(work == 1)
			{
				if( NPC_Util_GetStrFromStrWithDelim( buf, "NomalWindowMsg1",
									 buf2, sizeof( buf2) ) != NULL)
					strcpysafe( token,sizeof( buf2), buf2);

				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, "NomalWindowMsg2") != NULL) {
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
			}else{
			
				sprintf( tmp, "NomalWindowMsg%d", work);
				if( NPC_Util_GetStrFromStrWithDelim( buf, tmp,
													 buf2, sizeof( buf2) )
				 != NULL){
					strcpysafe( token,sizeof( buf2), buf2);
				}
				work++;
				sprintf( tmp, "NomalWindowMsg%d", work);
				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, tmp) != NULL) {
					
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}else{
					int fd = getfdFromCharaIndex( talker);
					char token2[14*50];
					/*--���ƥ��ʧ��  ةئ����ܰ��ë�浤--*/
					if(NPC_AcceptDel( meindex, talker,0) == FALSE) return;
	
					/*�׷º�ë�������ƻ���ľ�ɣ�����������--*/
					if( EvNo != -1) NPC_NowEventSetFlg( talker, EvNo);
						
					if(NPC_Util_GetStrFromStrWithDelim( buf,"NpcWarp",
										token2,sizeof( token2) )!= NULL){
								NPC_EventWarpNpc(meindex,token2);
					}
					
					//�׷º�ë  ����
					if(NPC_Util_GetStrFromStrWithDelim( buf, "EndSetFlg", 
										buf2, sizeof( buf2) )!=NULL)
					{
						char buf3[16];
						int k = 1;
				
						if(EvNo != -1) NPC_NowEventSetFlgCls( talker, EvNo);
				
						while(getStringFromIndexWithDelim(buf2 , "," ,k, 
								buf3, sizeof(buf3)) !=FALSE )
						{	
							k++;
							NPC_EventSetFlg( talker, atoi( buf3));
						}
					}
					//��ì�����׷º�ë����
					if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlg",
							 buf2, sizeof( buf2)) != NULL) {
						 	char buf3[16];
							int k = 1;

						 	k= 1 ;
							while(getStringFromIndexWithDelim(buf2 , "," , k, 
																buf3, sizeof(buf3))
							!= FALSE )
							{
								NPC_NowEndEventSetFlgCls( talker, atoi(buf3));
								k++;
							}
					}
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_YES,
								CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
			}
		  break;
#endif
		  case 10:
			  
			/*--ʸ�������������м��ݼ�����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "PetFullMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
				print(" PetFullMsg:%s ", token);
			}
			else
			{
				print(" PetFullMsg_Error:%d ", __LINE__);
			}
		break;

	  case 11:
			/*--�׷º�ë�������弰����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanMainMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
				buttontype = WINDOW_BUTTONTYPE_YESNO;
				windowtype = CHAR_WINDOWTYPE_WINDOWEVENT_CLEANMSG;
			}
		break;
	  case 12:
			/*--�׷º�ë������  ����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlgMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				char buf3[64];
				int loop=1;
				
				strcpysafe( token, sizeof( buf2), buf2);
				NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlg",
												 buf2, sizeof( buf2) );
				while(getStringFromIndexWithDelim(buf2 , "," , loop, buf3, sizeof(buf3))
				!= FALSE )
				{
					NPC_NowEndEventSetFlgCls( talker, atoi(buf3));
					loop++;
				}
			}
		break;

	  case 13:
			/*--���Ż��������м�����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "StoneFullMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
			}
		break;
		
		 case 14:
			/*--���Ż���Իئ�ж���������--*/
					if( NPC_Util_GetStrFromStrWithDelim( buf, "StoneLessMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
			}
		break;
	}

	/*���ƥ˪������*/
#ifdef _NEWEVENT
	if(num != 8)
#endif
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
					buttontype,
					windowtype,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					token);

}

/*--------------------------------------
 *��ì�����ü�ʧ��  ةëܰ������
 ---------------------------------------*/
BOOL NPC_EventAdd(int meindex,int talker,int mode)
{

	char buf[64];
	char buff2[128];
	int j = 1,i = 0;
	int kosuucnt = 0;
	int itemindex;
	char buff[1024*2];
	int rand_j = 0;
	int rand_flg = 0;
	int rand_cnt = 0;
	int evcnt;
	int b_flg = 0;

	/*--ɬ�ð����̻�  ��̤�л�ؤ������ë  ���ƻ������ּ�ë  ������--*/
	evcnt = NPC_EventFile( meindex, talker, buff);

	if(evcnt == -1) return FALSE;

//	if(NPC_EventFile( meindex, talker, buff) == FALSE) return FALSE;

	/*--ʧ��  ة����ë��������--*/
	if(NPC_ItemFullCheck( meindex, talker, buff, mode, evcnt) == FALSE){
		NPC_MsgDisp( meindex, talker, 4);
		return FALSE;
	}

	/*--�����������ͱ�  ��--*/
	if(NPC_Util_GetStrFromStrWithDelim( buff, "DelStone", buff2, sizeof( buff2) )
	!=NULL)
	{
		int stone;
		
		stone = NPC_EventGetCost( meindex, talker, buff2);

		if( (CHAR_getInt( talker, CHAR_GOLD) - stone) < 0 ){
			NPC_MsgDisp( meindex, talker, 14);
			return FALSE;
		}
	}


	/*--ʸ������  ëܰ������--*/
	if((NPC_Util_GetStrFromStrWithDelim( buff, "pet_skill", buff2, sizeof( buff2) )
	 !=NULL) && mode == 0 ){
		if(mode == 0) {
			NPC_PetSkillMakeStr( meindex, talker, 1);
			return TRUE;
		}
	}
	
	
	if(mode == 2) mode = 0;
	
	/*--ʸ����ëܰ������---*/
	if(NPC_Util_GetStrFromStrWithDelim( buff, "GetPet", buff2, sizeof( buff2) )
	 !=NULL)
	{	
		if(mode == 0){
			if(NPC_EventAddPet( meindex, talker, buff2,0) == FALSE) {
				NPC_MsgDisp( meindex, talker, 10);
				return FALSE;
			}
		}else if(mode == 1){
		
		}
	}

	// Robin add ��NPCȡ���ﵰ
	if(NPC_Util_GetStrFromStrWithDelim( buff, "GetEgg", buff2, sizeof( buff2) )
	 !=NULL)
	{	
		if(mode == 0){
			if(NPC_EventAddEgg( meindex, talker, buff2,0) == FALSE) {
				print(" AddEgg_Error:%d ", __LINE__);
				NPC_MsgDisp( meindex, talker, 10);
				return FALSE;
			}
		}else if(mode == 1){
		
		}
	}

	if(NPC_Util_GetStrFromStrWithDelim( buff, "DelItem", buff2, sizeof( buff2) ) !=NULL){
		if(strstr(buff2,"EVDEL") != NULL){
			char work[256];
			char nbuf[256];
			
			NPC_Util_GetStrFromStrWithDelim(buf, "EVENT", buff2, sizeof( buff2));
			getStringFromIndexWithDelim( buff2, ",", evcnt, work, sizeof( work));

			if(strstr(buff,"Break")!=NULL){
				b_flg=1;
			}else{
				b_flg=0;
			}
			
			if(NPC_Util_GetStrFromStrWithDelim( buff, "NotDel", nbuf, sizeof( nbuf)) !=  NULL) {
				NPC_EventDelItemEVDEL( meindex, talker, work, nbuf,b_flg);
			}else{
				NPC_EventDelItemEVDEL( meindex, talker, work, "-1",b_flg);
			}
			
		}else{
			if(strstr(buff,"Break")!=NULL){
				b_flg=1;
			}else{
				b_flg=0;
			}

			NPC_EventDelItem( meindex, talker, buff2, b_flg);
		}
	}

	/*--����ë����---*/
	if(NPC_Util_GetStrFromStrWithDelim( buff, "DelStone", buff2, sizeof( buff2) )
	!=NULL)
	{
		int stone;
		char token[128];
		
		stone = NPC_EventGetCost( meindex, talker, buff2);

		CHAR_setInt(talker,CHAR_GOLD,CHAR_getInt( talker, CHAR_GOLD) - stone);

		/*--�ɷ¶�����˪Ի--*/
		CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);
		sprintf(token, "����%d��stone��", stone);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
	}


	/*--�¼�ĸةʧ��  ة---*/
	if(NPC_Util_GetStrFromStrWithDelim( buff, "GetRandItem", buf, sizeof( buf) ) != NULL ){
		j=1;
		while( getStringFromIndexWithDelim( buf , "," , j, buff2, sizeof( buff2)) != FALSE ){
			j++;
			rand_j++;
		}
		rand_cnt = 1;
	}


	/*--  ɧ��ʧ��  ة������--*/
	if(NPC_Util_GetStrFromStrWithDelim( buff, "GetItem", buf, sizeof( buf)) != NULL ){
		if(mode == 0) {
			j = 1;
			/*--���ۼ�ʧ��  ةë  �ʾ���--*/
			while(getStringFromIndexWithDelim( buf, "," , j, buff2, sizeof(buff2)) !=FALSE ){
				j++;
			}
			
			j--;
			for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){
				itemindex=CHAR_getItemIndex( talker , i );
				if( !ITEM_CHECKINDEX( itemindex) ){
					kosuucnt++;
				 }
			}
			j= j + rand_cnt;
			if( kosuucnt < j ){
				NPC_MsgDisp( meindex, talker, 4);
				return FALSE;

			}else{
				if(NPC_Util_GetStrFromStrWithDelim( buff, "GetRandItem", buff2, sizeof( buff2))	!=NULL)	{
	 				rand_flg = 1;
					NPC_RandItemGet( meindex, talker, rand_j, buff2);
	 			}
				if(NPC_EventAddItem( meindex, talker, buf) == FALSE) return FALSE;
			}
		}else if(mode == 1){
			if(strstr(buff,"Break")!=NULL){
				b_flg=1;
			}else{
				b_flg=0;
			}
			NPC_EventDelItem( meindex, talker, buf,b_flg);
		}
	}

	if( (NPC_Util_GetStrFromStrWithDelim( buff, "GetRandItem", buf, sizeof( buf) )
		!=NULL) && (rand_flg == 0) && mode == 0 )
	{	
		/*--��������   ����    �¾���-*/
		for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){
			itemindex=CHAR_getItemIndex( talker , i );
			if( !ITEM_CHECKINDEX( itemindex) ){
				kosuucnt++;
			 }
		}
		/*--���廥ؤ�¾�������--*/
		if( kosuucnt == 0 ){
			NPC_MsgDisp( meindex, talker, 4);
			return FALSE;
		}

	
		NPC_RandItemGet( meindex, talker, rand_j,buf);
	}
	
#ifdef _EXCHANGEMAN_REQUEST_DELPET
	if( NPC_Util_GetStrFromStrWithDelim( buff, "DelPet", buff2, sizeof( buff2) ) != NULL && mode == 0 ) {
		char buf4[32];
		char buf5[256];
		int i=1;
		char work[256];
		int j=1;
		
		if(strstr(buff2,"EVDEL") != NULL){
			NPC_Util_GetStrFromStrWithDelim( buf, "EVENT", buf5, sizeof( buf5));
			getStringFromIndexWithDelim( buf5, ",",evcnt , buff2, sizeof( buff2));	
			while(getStringFromIndexWithDelim( buff2, "&", j, work, sizeof( work))!=FALSE){
				j++;			
				if(strstr( work, "PET") != NULL) {
					if(strstr( work, "EV") != NULL) {
						if(NPC_PetLvCheck( meindex, talker, work, 1) == FALSE) return FALSE;
					}else{
						if(NPC_PetLvCheck( meindex, talker, work, 0) == FALSE) return FALSE;
					}
				}
			}
			j = 1;		
			while(getStringFromIndexWithDelim( buff2, "&", j, work, sizeof( work)) != FALSE){
				j++;
				if(strstr( work, "PET") == NULL ) continue;
	
				if(strstr(work,"*") != NULL){
					int cnt=0;
					int petcnt=0;
					int petindex;
					
					getStringFromIndexWithDelim( work, "*", 2, buf4, sizeof( buf4));
					cnt = atoi(buf4);
					
					for(i=0 ; i < CHAR_MAXPETHAVE ; i++){
						petindex = CHAR_getCharPet( talker, i);
		
						if( petindex == -1  )  continue;
		
						if(strstr( work, "EV") != NULL) {
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 1)
							== FALSE) {
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 0) == FALSE) {
								continue;
							}
						}

						if(NPC_EventDelPet( meindex, talker,i) == FALSE) return FALSE;
						
						petcnt++;
						/*--϶�������м�ʸ����ë�����׾���������--*/
						if(cnt == petcnt) break;

					}
				}else{
					/*--ʸ����ë����������ë϶���ƻ�ئ�м�ƥ���ʸ����ë�廯����--*/

					int petindex;

					for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
						petindex = CHAR_getCharPet( talker, i);

					    if( !CHAR_CHECKINDEX( petindex) )  continue;

						if(strstr( buff2, "EV") != NULL){
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 1) == FALSE){
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 0) == FALSE) {
								continue;
							}
						}
					
						if(NPC_EventDelPet( meindex, talker, i) == FALSE) return FALSE;
					}
				}
			}
		
		}else{
			/*--�ֵ�1պ��ʸ����ë  �Ȼ����¾�����������ë�浤--*/
			while(getStringFromIndexWithDelim( buff2, ",", j, work, sizeof( work))
			!=FALSE)
			{
				j++;
				/*--ʸ�������������ͻ������Ȼ���������������PET����ë������������ --*/
				if(strstr( work, "PET") != NULL) {
					if(strstr( work, "EV") != NULL) {
						if(NPC_PetLvCheck( meindex, talker, work, 1) == FALSE) return FALSE;
					}else{
						if(NPC_PetLvCheck( meindex, talker, work, 0) == FALSE) return FALSE;
					}
				}
			}
			j = 1;
			
			while(getStringFromIndexWithDelim( buff2, ",", j, work, sizeof( work)) != FALSE)
			{
				j++;
				if(strstr(work,"*") != NULL)
				{
					int cnt=0;
					int petcnt=0;
					int petindex;
					
					getStringFromIndexWithDelim( work, "*", 2, buf4, sizeof( buf4));
					cnt = atoi(buf4);
					
					for(i=0 ; i < CHAR_MAXPETHAVE ; i++){
						petindex = CHAR_getCharPet( talker, i);
		
						if( petindex == -1  )  continue;
		
						if(strstr( work, "EV") != NULL) {
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 1) == FALSE) {
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 0) == FALSE) {
								continue;
							}
						}

						if(NPC_EventDelPet( meindex, talker,i) == FALSE) return FALSE;
						
						petcnt++;
						/*--϶�������м�ʸ����ë�����׾���������--*/
						if(cnt == petcnt) break;
					}
				}else{
					/*--ʸ����ë����������ë϶���ƻ�ئ�м�ƥ���ʸ����ë�廯����--*/

					int petindex;

					for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
						petindex = CHAR_getCharPet( talker, i);

					    if( !CHAR_CHECKINDEX( petindex) )  continue;

						if(strstr( buff2, "EV") != NULL){
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 1) == FALSE){
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 0) == FALSE) {
								continue;
							}
						}
						if(NPC_EventDelPet( meindex, talker, i) == FALSE) return FALSE;
					}
				}
			}
		}
	}
#endif
	return TRUE;

}


/*--------------------------------------
 *�¼�ĸةƥʧ��  ةë������������
 ------------------------------------ */
BOOL NPC_RandItemGet(int meidex,int talker,int rand_j,char *buf)
{
	char buff2[64];
	int randitem;
	int ret;
	int itemindex;
	char token[128];

	if(rand_j == 0) {
		print("Event:��춣��Ľ��룬���ִ���");
 		return FALSE;
 	}
 	
	randitem = rand()%rand_j;
	if(randitem == 0) randitem = rand_j;

	getStringFromIndexWithDelim(buf , "," , randitem, buff2, sizeof(buff2)) ;

	itemindex = ITEM_makeItemAndRegist( atoi( buff2));

	if(itemindex == -1) return FALSE;
	
	/*ʧ��  ة��ܰ��(  ��ʧ��  ة  ����ľ��������  */
	ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
	if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
		print( "npc_exchangeman.c: additem error itemindex[%d]\n", itemindex);
		ITEM_endExistItemsOne( itemindex);
		return FALSE;
	}

	if(itemindex != -1) {
		LogItem(
			CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
			CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
			itemindex,
#else
			ITEM_getInt( itemindex, ITEM_ID),  /* ʧ��  ة  į */
#endif
			"EventAddItem(�����������õ��ĵ���)",
			CHAR_getInt( talker,CHAR_FLOOR),
			CHAR_getInt( talker,CHAR_X ),
 			CHAR_getInt( talker,CHAR_Y ),
            ITEM_getChar( itemindex, ITEM_UNIQUECODE),
			ITEM_getChar( itemindex, ITEM_NAME),
			ITEM_getInt( itemindex, ITEM_ID)
		);
	}
					
	sprintf(token,"������%s",ITEM_getChar( itemindex, ITEM_NAME));
	CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);

	CHAR_sendItemDataOne( talker, ret);
	return TRUE;

}


/*----------------------------------------------------
 * ��ì����
 ----------------------------------------------------*/
BOOL NPC_AcceptDel(int meindex,int talker,int mode )
{

	char buf[1024*2];
	char buff2[256];
	int rand_j = 0;
	int j = 0;
	int evcnt;
	int b_flg = 0;
	evcnt = NPC_EventFile( meindex, talker, buf);
	if(evcnt == -1) return FALSE;

	/*--ɬ�ð����̻�  �微��ľ����������ë  ���ƻ������ּ�ë  ������--*/
//	if(NPC_EventFile(meindex,talker,buf)==FALSE) return FALSE;

	/*--ʧ��  ة����ë��������--*/
	if(NPC_ItemFullCheck( meindex, talker, buf, mode, evcnt) == FALSE){
		NPC_MsgDisp( meindex, talker, 4);
		return FALSE;
	}
	
	/*--�����������ͱ�  ��--*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelStone", buff2, sizeof( buff2) )
	!=NULL)
	{
		int stone;
		stone = NPC_EventGetCost( meindex, talker, buff2);

		if( (CHAR_getInt( talker, CHAR_GOLD) - stone) < 0 ) {
			NPC_MsgDisp( meindex, talker, 14);
			return FALSE;
		}
	}


	/*--�����������ͱ�  ��--*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetStone", buff2, sizeof( buff2) )!=NULL){
		int stone;
		stone = atoi(buff2);

		if( (CHAR_getInt( talker, CHAR_GOLD) + stone) >= CHAR_getMaxHaveGold( talker) ) {
			NPC_MsgDisp( meindex, talker, 13);
			return FALSE;
		}
	}


	/*--ʸ������  ëܰ������--*/
	if((NPC_Util_GetStrFromStrWithDelim( buf, "pet_skill", buff2, sizeof( buff2) )
	 !=NULL) && mode == 0 ){
		if(mode == 0){
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, 1);
			NPC_PetSkillMakeStr( meindex, talker, 1);
			return TRUE;
		}
	}

	/*--ʸ����ë����---*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelPet", buff2, sizeof( buff2) ) != NULL) {
		char buf4[32];
		char buf5[256];
		int i=1;
		char work[256];
		int j=1;
		
		if(strstr(buff2,"EVDEL") != NULL){
			NPC_Util_GetStrFromStrWithDelim( buf, "EVENT", buf5, sizeof( buf5));
			getStringFromIndexWithDelim( buf5, ",",evcnt , buff2, sizeof( buff2));
		
		
			/*--�ֵ�1պ��ʸ����ë  �Ȼ����¾�����������ë�浤--*/
			while(getStringFromIndexWithDelim( buff2, "&", j, work, sizeof( work))
			!=FALSE)
			{
				j++;
				
				/*--ʸ�������������ͻ������Ȼ���������������PET����ë������������ --*/
				if(strstr( work, "PET") != NULL) {
					if(strstr( work, "EV") != NULL) {
						if(NPC_PetLvCheck( meindex, talker, work, 1) == FALSE) return FALSE;
					}else{
						if(NPC_PetLvCheck( meindex, talker, work, 0) == FALSE) return FALSE;
					}
				}
			}
			j = 1;
			
			
			while(getStringFromIndexWithDelim( buff2, "&", j, work, sizeof( work))
			!= FALSE)
			{
				j++;
				if(strstr( work, "PET") == NULL ) continue;
	
				if(strstr(work,"*") != NULL)
				{
					int cnt=0;
					int petcnt=0;
					int petindex;
					
					getStringFromIndexWithDelim( work, "*", 2, buf4, sizeof( buf4));
					cnt = atoi(buf4);
					
					for(i=0 ; i < CHAR_MAXPETHAVE ; i++){
						petindex = CHAR_getCharPet( talker, i);
		
						if( petindex == -1  )  continue;
		
						if(strstr( work, "EV") != NULL) {
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 1)
							== FALSE) {
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 0) 
							== FALSE) {
								continue;
							}
						}

						if(NPC_EventDelPet( meindex, talker,i) == FALSE) return FALSE;
						
						petcnt++;
						/*--϶�������м�ʸ����ë�����׾���������--*/
						if(cnt == petcnt) break;

					}
				}else{
					/*--ʸ����ë����������ë϶���ƻ�ئ�м�ƥ���ʸ����ë�廯����--*/

					int petindex;

					for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
						petindex = CHAR_getCharPet( talker, i);

					    if( !CHAR_CHECKINDEX( petindex) )  continue;

						if(strstr( buff2, "EV") != NULL){
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 1)
							== FALSE){
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 0)
							== FALSE) {
								continue;
							}
						}
					
						if(NPC_EventDelPet( meindex, talker, i) == FALSE) return FALSE;
					}
				}
			}
		
		}else{
		
			/*--�ֵ�1պ��ʸ����ë  �Ȼ����¾�����������ë�浤--*/
			while(getStringFromIndexWithDelim( buff2, ",", j, work, sizeof( work))
			!=FALSE)
			{
				j++;
				/*--ʸ�������������ͻ������Ȼ���������������PET����ë������������ --*/
				if(strstr( work, "PET") != NULL) {
					if(strstr( work, "EV") != NULL) {
						if(NPC_PetLvCheck( meindex, talker, work, 1) == FALSE) return FALSE;
					}else{
						if(NPC_PetLvCheck( meindex, talker, work, 0) == FALSE) return FALSE;
					}
				}
			}
			j = 1;
			
			
			while(getStringFromIndexWithDelim( buff2, ",", j, work, sizeof( work))
			!= FALSE)
			{
				j++;
				if(strstr(work,"*") != NULL)
				{
					int cnt=0;
					int petcnt=0;
					int petindex;
					
					getStringFromIndexWithDelim( work, "*", 2, buf4, sizeof( buf4));
					cnt = atoi(buf4);
					
					for(i=0 ; i < CHAR_MAXPETHAVE ; i++){
						petindex = CHAR_getCharPet( talker, i);
		
						if( petindex == -1  )  continue;
		
						if(strstr( work, "EV") != NULL) {
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 1)
							== FALSE) {
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 0) 
							== FALSE) {
								continue;
							}
						}

						if(NPC_EventDelPet( meindex, talker,i) == FALSE) return FALSE;
						
						petcnt++;
						/*--϶�������м�ʸ����ë�����׾���������--*/
						if(cnt == petcnt) break;

					}
				}else{
					/*--ʸ����ë����������ë϶���ƻ�ئ�м�ƥ���ʸ����ë�廯����--*/

					int petindex;

					for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
						petindex = CHAR_getCharPet( talker, i);

					    if( !CHAR_CHECKINDEX( petindex) )  continue;

						if(strstr( buff2, "EV") != NULL){
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 1)
							== FALSE){
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 0)
							== FALSE) {
								continue;
							}
						}
					
						if(NPC_EventDelPet( meindex, talker, i) == FALSE) return FALSE;
					}
				}
			}
		}
	}
	


	/*--����ëؤ����---*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetStone", buff2, sizeof( buff2) )
	!=NULL)
	{
		int stone;
		char token[128];

		stone = atoi(buff2);
		CHAR_setInt(talker,CHAR_GOLD,CHAR_getInt( talker, CHAR_GOLD) + stone);
		/*--�ɷ¶�����˪Ի--*/
		CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);
	
		sprintf( token,"�õ�%d��stone", stone);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
	}


	/*--ʸ����ëܰ������---*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetPet", buff2, sizeof( buff2) )
	!=NULL)
	{
		if(NPC_EventAddPet( meindex, talker, buff2,1) == FALSE) {
			NPC_MsgDisp( meindex, talker, 10);
			return FALSE;
		}
	}
	
#ifdef _TRANS_7_NPC
	if(NPC_Util_GetStrFromStrWithDelim( buf, "TRANS7", buff2, sizeof( buff2) )
	!=NULL)
	{
		if(NPC_EventTRANS( meindex, talker, buff2,1) == FALSE) {
			NPC_MsgDisp( meindex, talker, 10);
			return FALSE;
		}
	}
#endif	

	// Robin add ��NPCȡ���ﵰ
	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetEgg", buff2, sizeof( buff2) )
	!=NULL)
	{
		if(NPC_EventAddEgg( meindex, talker, buff2,1) == FALSE) {
			print(" AddEgg_Error:%d ", __LINE__);
			NPC_MsgDisp( meindex, talker, 10);
			return FALSE;
		}
	}

	/*--ʧ��  ةë����--*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelItem", buff2, sizeof( buff2) )	!= NULL){
		if(strstr(buff2,"EVDEL") != NULL){
			char work[256];
			char nbuf[256];
		
			NPC_Util_GetStrFromStrWithDelim(buf, "EVENT", buff2, sizeof( buff2));
			getStringFromIndexWithDelim( buff2, ",", evcnt, work, sizeof( work));

			if(strstr(buf,"Break")!=NULL){
				b_flg=1;
			}else{
				b_flg=0;
			}

			if(NPC_Util_GetStrFromStrWithDelim( buf, "NotDel", nbuf, sizeof( nbuf))	!= NULL ){
				NPC_EventDelItemEVDEL( meindex, talker, work, nbuf,b_flg);
			}else{
				NPC_EventDelItemEVDEL( meindex, talker, work, "-1",b_flg);
			}
		}else{
			if(strstr(buf,"Break")!=NULL){
				b_flg=1;
			}else{
				b_flg=0;
			}
			NPC_EventDelItem( meindex, talker, buff2,b_flg);
		}
	}


	/*--����ë����---*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelStone", buff2, sizeof( buff2) )
	!=NULL)
	{
		int stone;
		char token[128];
		
		stone = NPC_EventGetCost( meindex, talker, buff2);

		CHAR_setInt(talker,CHAR_GOLD,CHAR_getInt( talker, CHAR_GOLD) - stone);
		/*--�ɷ¶�����˪Ի--*/
		CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);
		sprintf( token, "����%d��stone��", stone);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
	}


	/*--�¼�ĸةʧ��  ةëܰ������---*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetRandItem", buff2, sizeof( buff2) )
	!=NULL)
	{
		char buf3[32];
		j = 1;
		while(getStringFromIndexWithDelim(buff2 , "," , j, buf3, sizeof( buf3))
		 != FALSE )
		{
			j++;
			rand_j++;
		}
		NPC_RandItemGet( meindex, talker, rand_j, buff2);
	}


	/*--ʧ��  ةëܰ������---*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetItem", buff2, sizeof( buff2) )
	!=NULL)
	{
		 NPC_EventAddItem( meindex, talker, buff2);
	}

	/*--�ɷ¶�������˪��--*/
	CHAR_complianceParameter( talker );
	CHAR_send_P_StatusString( talker ,
					CHAR_P_STRING_ATK|CHAR_P_STRING_DEF|
					CHAR_P_STRING_CHARM|CHAR_P_STRING_QUICK|
					CHAR_P_STRING_WATER|CHAR_P_STRING_FIRE|
					CHAR_P_STRING_WIND|CHAR_P_STRING_EARTH
				);
	
	return TRUE;

}


/*--------------------------
 * ��ì����ʸ����ë����
 ---------------------------*/
BOOL NPC_EventDelPet(int  meindex,int  talker, int petsel)
{

	int petindex;
	char szPet[128];
	int defpet;
	char msgbuf[64];

	int fd = getfdFromCharaIndex( talker );

	petindex = CHAR_getCharPet( talker, petsel);

    if( !CHAR_CHECKINDEX(petindex) ) return FALSE;

	if( CHAR_getInt( talker, CHAR_RIDEPET) == petsel ) {

		//CHAR_talkToCli( talker, -1, "����еĳ����޷�������", CHAR_COLORYELLOW );
    	//return	FALSE;

		CHAR_setInt( talker, CHAR_RIDEPET, -1);
		CHAR_send_P_StatusString( talker, CHAR_P_STRING_RIDEPET );
		CHAR_complianceParameter( talker );
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( talker , CHAR_WORKOBJINDEX ));
		print(" DelRidePet ");
	}
	print(" EventDelPet ");

	/*--����ƽ�ҷ»���    ئ�գ�ʸ����ë�����ڱ�  --*/
	if( CHAR_getWorkInt( CONNECT_getCharaindex( fd),
   	                     CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return FALSE;
	/*--����ʸ�����������ﾮ����������������--*/
	defpet = CHAR_getInt( talker, CHAR_DEFAULTPET);
	if(defpet == petsel){
		CHAR_setInt( talker, CHAR_DEFAULTPET, -1);
		lssproto_KS_send( fd, -1, TRUE);
	}

	snprintf( msgbuf,sizeof( msgbuf), "����%s��",
									CHAR_getChar( petindex, CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORWHITE);

	// ʸ����ë��  ���׷��
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"EvnetDell(����ɾ��)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);


	/*   ��  �������� */
	CHAR_setCharPet( talker, petsel, -1);

	CHAR_endCharOneArray( petindex );

	snprintf( szPet, sizeof( szPet ), "K%d", petsel);
	// ޥ����ʸ������  ������˪Ի������
	CHAR_sendStatusString( talker, szPet );

	return TRUE;

}


/*----------------------------
 *  ʸ����ëܰ������
 ------------------------------*/
BOOL NPC_EventAddPet(int meindex, int talker, char *buff2,int mode)
{
	int	ret;
	char msgbuf[64];
	int	enemynum;
	int	enemyid;
	int	i;
	int petindex;

	/*--ʸ����  �����л����¾�����������--*/
	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )  break;
	}

	if(i == CHAR_MAXPETHAVE) return FALSE;


	/* ¦�Ѽ�ID����indexë��   */
	if(strstr(buff2,",") != NULL){
		char buf2[16];
		while(getStringFromIndexWithDelim( buff2, ",", i, buf2, sizeof( buf2))
		!= FALSE) {
			i++;
		}
		i--;
		i = rand()%i + 1;
		getStringFromIndexWithDelim( buff2, ",", i, buf2, sizeof( buf2));
		enemyid = atoi( buf2);
	}else{
		enemyid = atoi( buff2);
	}
	
	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
			break;
		}
	}

	if( i == enemynum ) return FALSE;

	ret = ENEMY_createPetFromEnemyIndex( talker, i);

	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		if( CHAR_getCharPet( talker, i ) == ret )break;
	}
	if( i == CHAR_MAXPETHAVE ) i = 0;
	if( CHAR_CHECKINDEX( ret ) == TRUE ){
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
	}

	petindex = CHAR_getCharPet(talker,i);

    if( !CHAR_CHECKINDEX( petindex) )return FALSE;
	CHAR_complianceParameter( petindex );
	snprintf( msgbuf, sizeof( msgbuf ), "K%d", i );
	CHAR_sendStatusString( talker, msgbuf );

	snprintf( msgbuf, sizeof( msgbuf ), "W%d", i );
	CHAR_sendStatusString( talker, msgbuf );
	snprintf( msgbuf,sizeof( msgbuf), "�õ�%s��",
								CHAR_getChar(petindex,CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORWHITE);

	// ʸ����ë���  ľ�׷��
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"EventGet(����õ�)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);



	if(mode == 0) {
		/*--��ì�����ü�ʸ����--*/
		CHAR_setInt( petindex, CHAR_ENDEVENT, 1);
	}

	return TRUE;

}

#ifdef _TRANS_7_NPC
BOOL NPC_EventTRANS(int meindex, int talker, char *buff2,int mode)
{
	char token[128];
	int work[10];
	int Trans=CHAR_getInt(talker,CHAR_TRANSMIGRATION)+1;
	if(Trans>7 || CHAR_getInt(talker,CHAR_LV)<80){
		sprintf( token, "����������ת��ȼ�С��80��������ת��ʧ��!", CHAR_getChar( talker, CHAR_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		return;
	}
#ifdef _ADD_POOL_ITEM			   // WON ADD ���ӿɼķŵĵ���	
	int tran_pool_item[5] = { 4, 4, 4, 4, 4 };		// ���ӵĵ��߼ķ���
	int tran_pool_pet[5] = { 2, 2, 2, 2, 2};		// ���ӵĳ���ķ���
	int j,item_sum=0, pet_sum=0;

	for(j=0; j<5 ;j++){
		item_sum += tran_pool_item[j] ;			
		pet_sum += tran_pool_pet[j];		
	}

	item_sum += 10;								// �������ļĵ�����
	pet_sum  += 5;							    // �������ļĳ���

#endif
	NPC_TransmigrationStatus(talker, talker, work);
	NPC_TransmigrationFlg_CLS(talker, talker);
	CHAR_setInt(talker ,CHAR_TRANSMIGRATION, Trans);
	CHAR_setInt(talker, CHAR_LV ,1);
	CHAR_setMaxExp( talker, 0);
  CHAR_setInt( talker,CHAR_SKILLUPPOINT,CHAR_getInt( talker, CHAR_TRANSMIGRATION)*10);
  CHAR_Skillupsend( talker );
  CHAR_setInt( talker,CHAR_RIDEPET, -1 );
  CHAR_setInt( talker , CHAR_BASEIMAGENUMBER , CHAR_getInt( talker , CHAR_BASEBASEIMAGENUMBER) );
	CHAR_sendStatusString( talker , "P");
	
	CHAR_talkToCli( talker, -1, buff2, CHAR_COLORYELLOW );
}
#endif	


// Robin add NPC�����ﵰ
BOOL NPC_EventAddEgg(int meindex, int talker, char *buff2,int mode)
{
	int	ret;
	char msgbuf[64];
	int	enemynum;
	int	enemyid;
	int	petid;
	int raise;
	int	i;
	int petindex;
	char buf2[32];
	char buf3[32];

	print("\n ������!!:%s ", buff2);

	/*--ʸ����  �����л����¾�����������--*/
	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )  break;
	}

	if(i == CHAR_MAXPETHAVE) {
		print(" AddEgg_Error:%d ", __LINE__);
		return FALSE;
	}


	/* ¦�Ѽ�ID����indexë��   */
	if(strstr(buff2,",") != NULL){ // �Ƿ�Ϊ�������
		while(getStringFromIndexWithDelim( buff2, ",", i, buf2, sizeof( buf2))
		!= FALSE) {
			i++;
		}
		i--;
		i = rand()%i + 1;
		getStringFromIndexWithDelim( buff2, ",", i, buf2, sizeof( buf2));

	}else{
		strcpy( buf2, buff2);
	}

	getStringFromIndexWithDelim( buf2, ";", 1, buf3, sizeof( buf3));
	enemyid = atoi( buf3); // ���ﵰID
	getStringFromIndexWithDelim( buf2, ";", 2, buf3, sizeof( buf3));
	petid = atoi( buf3); // �����ĳ���ID
	getStringFromIndexWithDelim( buf2, ";", 3, buf3, sizeof( buf3));
	raise = atoi( buf3); //   ������


	// ��enemyidת��enemybaseid
	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == petid) {
			break;
		}
	}
	if( i == enemynum ) {
		print(" AddEgg_Error:%d ", __LINE__);
		return FALSE;
	}
	petid = ENEMY_getInt( i, ENEMY_TEMPNO);


	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
			break;
		}
	}
	if( i == enemynum ) {
		print(" AddEgg_Error:%d ", __LINE__);
		return FALSE;
	}

	ret = ENEMY_createPetFromEnemyIndex( talker, i);

	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		if( CHAR_getCharPet( talker, i ) == ret )break;
	}
	if( i == CHAR_MAXPETHAVE ) i = 0;
	if( CHAR_CHECKINDEX( ret ) == TRUE ){
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
	}

	petindex = CHAR_getCharPet(talker,i);

    if( !CHAR_CHECKINDEX( petindex) ) {
		print(" AddEgg_Error:%d ", __LINE__);
		return FALSE;
	}

	CHAR_complianceParameter( petindex );
	snprintf( msgbuf, sizeof( msgbuf ), "K%d", i );
	CHAR_sendStatusString( talker, msgbuf );

	snprintf( msgbuf, sizeof( msgbuf ), "W%d", i );
	CHAR_sendStatusString( talker, msgbuf );
	snprintf( msgbuf,sizeof( msgbuf), "�õ�%s��",
								CHAR_getChar(petindex,CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORWHITE);

	// ʸ����ë���  ľ�׷��
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"EventGetEgg(����õ�)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);



	if(mode == 0) {
		/*--��ì�����ü�ʸ����--*/
		CHAR_setInt( petindex, CHAR_ENDEVENT, 1);
	}
	return TRUE;

}


/*---------------------------
 *  ��ì����  ë��������
 -----------------------------**/
BOOL NPC_EventDelItem(int meindex,int talker,char *buf,int breakflg)
{

	int i = 1, j = 1, k = 1 ;
	char buff3[128];
	char buf2[32];
	int itemindex;
	char token[256];

	while(getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3))
	 !=FALSE )
	{
		k++;
		if(strstr(buff3,"*") != NULL) {
			int itemno;
			int kosuu;
			int id;
			int cnt=0;
				
			getStringFromIndexWithDelim( buff3, "*", 1, buf2, sizeof( buf2));
			itemno = atoi( buf2); 
			getStringFromIndexWithDelim( buff3, "*", 2, buf2, sizeof( buf2));
			kosuu = atoi( buf2);
	 
			if( breakflg != 1 ){
      }else{
			    for( i =0 ; i < CHAR_MAXITEMHAVE ; i++ ){
				    itemindex = CHAR_getItemIndex( talker , i );
				    if( ITEM_CHECKINDEX( itemindex) ) {
					    id = ITEM_getInt( itemindex , ITEM_ID );
					    if(itemno == id) {
						    cnt++;
						    LogItem(
							CHAR_getChar( talker, CHAR_NAME ), // ƽ�ҷ�   
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  // ʧ��  ة  į 
#endif
							"EventDelItem(�����������ջصĵ���)",
							CHAR_getInt( talker, CHAR_FLOOR),
							CHAR_getInt( talker, CHAR_X ),
 							CHAR_getInt( talker, CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)
	     					);
	 
		    				if(breakflg == 1){
			    			    sprintf(token,"%s ����",ITEM_getChar( itemindex, ITEM_NAME));
							    CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
							}else{
							    sprintf(token,"����%s",ITEM_getChar( itemindex, ITEM_NAME));
							    CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
							}
						    //--ʧ��  ةë��ڽ--
						    CHAR_setItemIndex( talker, i ,-1);
						    ITEM_endExistItemsOne(itemindex);
						    CHAR_sendItemDataOne( talker, i);

						    if(cnt == kosuu){
							    break;
							}
						}
					}
				}
			}
		}
		else{
			/*--���Ϸ�  į��ʧ��  ةë����---*/
			for( j = 0 ;  j < CHAR_MAXITEMHAVE ; j++){
				itemindex = CHAR_getItemIndex( talker ,j);

				if( ITEM_CHECKINDEX( itemindex)){
					if( atoi( buff3) == ITEM_getInt( itemindex, ITEM_ID)) {
						LogItem(
							CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  /* ʧ��  ة  į */
#endif
							"EventDelItem(�����������ջصĵ���)",
							CHAR_getInt( talker,CHAR_FLOOR),
							CHAR_getInt( talker,CHAR_X ),
							CHAR_getInt( talker,CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)
						);
						if(breakflg == 1){
								sprintf(token,"%s ����",ITEM_getChar( itemindex, ITEM_NAME));
							CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
						}else{
							sprintf( token, "����%s��", 
											ITEM_getChar( itemindex, ITEM_NAME));
							CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
						}
						/*--ʧ��  ةë��ڽ--*/
						CHAR_setItemIndex( talker, j ,-1);
						ITEM_endExistItemsOne( itemindex);
						/*--�ɷ¶�����˪��--*/
						CHAR_sendItemDataOne( talker, j);
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL NPC_EventDelItemEVDEL(int meindex,int talker,char *buf,char *nbuf,int breakflg)
{

	int k = 1, l = 1;
	char buff3[128];
	char buf2[32];
	char buf4[32];



	while(getStringFromIndexWithDelim(buf , "&" , k, buf4, sizeof(buf4)) !=FALSE ){
		int itemno;
		k++;
		if(strstr(buf4,"ITEM") == NULL) continue;
		getStringFromIndexWithDelim(buf4 , "=" , 2, buff3, sizeof(buff3));

		if(strstr(buff3,"*") != NULL) {
			int kosuu;
			char token[256];
			int i, itemindex, id, cnt;
			getStringFromIndexWithDelim( buff3, "*", 1, buf2, sizeof( buf2));
			itemno = atoi( buf2); 
			getStringFromIndexWithDelim( buff3, "*", 2, buf2, sizeof( buf2));
			kosuu = atoi( buf2);
			if(strstr(nbuf,"-1") == NULL){
				l = 1;
				while(getStringFromIndexWithDelim(nbuf , "," , l, buf2, sizeof(buf2))){
					l++;
					if(itemno == atoi( buf2)){
						l = -1;
						break;
					}
				}
				if(l == -1) continue;
			}
			cnt = 0;
			for( i =0 ; i < CHAR_MAXITEMHAVE ; i++ ){
				itemindex = CHAR_getItemIndex( talker , i );
				if( ITEM_CHECKINDEX( itemindex) ) {
					id = ITEM_getInt( itemindex , ITEM_ID );
					if(itemno == id) {
						cnt++;
						LogItem(
							CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  /* ʧ��  ة  į */
#endif
							"EventDelItem(�����������ջصĵ���)",
							CHAR_getInt( talker, CHAR_FLOOR),
							CHAR_getInt( talker, CHAR_X ),
 							CHAR_getInt( talker, CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)
						);
						if(breakflg == 1){
							sprintf(token,"%s ����",ITEM_getChar( itemindex, ITEM_NAME));
							CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
						}else{
							sprintf(token,"����%s",ITEM_getChar( itemindex, ITEM_NAME));
							CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
						}
						CHAR_setItemIndex( talker, i ,-1);
						ITEM_endExistItemsOne(itemindex);
						CHAR_sendItemDataOne( talker, i);
						if(cnt == kosuu){
							break;
						}
					}
				}
			}		
		}else{
			int j, itemindex;
			char token[256];
			if(strstr(nbuf,"-1") == NULL){
				l = 1;
				while(getStringFromIndexWithDelim(nbuf , "," , l, buf2, sizeof(buf2))){
					l++;
					if(atoi(buff3) == atoi( buf2)){
						l = -1;
						break;
					}
				}
				if(l == -1) continue;
			}
			itemno = -1;
			for( j = 0 ;  j < CHAR_MAXITEMHAVE ; j++){
				itemindex = CHAR_getItemIndex( talker ,j);
				if( ITEM_CHECKINDEX( itemindex)){
					if( atoi( buff3) == ITEM_getInt( itemindex, ITEM_ID)) {
						LogItem(
							CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  /* ʧ��  ة  į */
#endif
							"EventDelItem(�����������ջصĵ���)",
							CHAR_getInt( talker,CHAR_FLOOR),
							CHAR_getInt( talker,CHAR_X ),
							CHAR_getInt( talker,CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)
						);
						if(breakflg == 1){
							sprintf(token,"%s ����",ITEM_getChar( itemindex, ITEM_NAME));
							CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
						}else{
							sprintf( token, "����%s��", 
										ITEM_getChar( itemindex, ITEM_NAME));
							CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
						}
						CHAR_setItemIndex( talker, j ,-1);
						ITEM_endExistItemsOne( itemindex);
						CHAR_sendItemDataOne( talker, j);
					}
				}
			}
		}
	}

	return TRUE;
}


/*----------------------
 * ��ì����  ëܰ��]
 -----------------------*/
BOOL NPC_EventAddItem(int meindex,int talker,char *buf)
{

	char buff3[128];
	int i = 1;
	int itemindex;
	char buf3[32];
	int ret;
	char token[256];

	while(getStringFromIndexWithDelim(buf , "," , i, buff3, sizeof( buff3)) 
	!= FALSE )
	{
		i++;
		if(strstr( buff3, "*") != NULL) {
			
			int itemno;
			int kosuu;
			int loop = 0;

			getStringFromIndexWithDelim( buff3, "*", 1, buf3,sizeof( buf3));
			itemno = atoi( buf3);
			getStringFromIndexWithDelim( buff3, "*", 2, buf3,sizeof( buf3));
			kosuu = atoi( buf3);
	
			for(loop = 0 ; loop < kosuu ; loop++) {
				itemindex = ITEM_makeItemAndRegist( itemno);
	
				if(itemindex == -1) return FALSE;
	
				/*ʧ��  ة��ܰ��(  ��ʧ��  ة  ����ľ��������  */
				ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
				if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
				    print("npc_exchange:ACCEPTadditem error itemindex[%d]\n",itemindex);
					ITEM_endExistItemsOne( itemindex);
					return FALSE;
				}

				if(itemindex != -1) {
					LogItem(
						CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
						CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
						itemindex,
#else
	    				ITEM_getInt( itemindex, ITEM_ID),  /* ʧ��  ة  į */
#endif
						"EventAddItem(�����������õ��ĵ���)",
						CHAR_getInt( talker, CHAR_FLOOR),
						CHAR_getInt( talker, CHAR_X ),
						CHAR_getInt( talker, CHAR_Y ),
						ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
					);
				}
				sprintf( token, "�õ�%s��", ITEM_getChar( itemindex, ITEM_NAME));
				CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);

				CHAR_sendItemDataOne( talker, ret);
			}
		}else{
			itemindex = ITEM_makeItemAndRegist( atoi( buff3));
	
			if(itemindex == -1) return FALSE;

			/*ʧ��  ة��ܰ��(  ��ʧ��  ة  ����ľ��������  */
			ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
			if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
				print( "npc_exchange.c: ACCEPTadditem error itemindex[%d]\n", itemindex);
				ITEM_endExistItemsOne( itemindex);
				return FALSE;
			}

			if(itemindex != -1) {
				LogItem(
				CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
				CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
				itemindex,
#else
	       		ITEM_getInt( itemindex, ITEM_ID),  /* ʧ��  ة  į */
#endif
				"EventAddItem(�����������õ��ĵ���)",
				CHAR_getInt( talker, CHAR_FLOOR),
				CHAR_getInt( talker, CHAR_X ),
 				CHAR_getInt( talker, CHAR_Y ),
				ITEM_getChar( itemindex, ITEM_UNIQUECODE),
				ITEM_getChar( itemindex, ITEM_NAME),
				ITEM_getInt( itemindex, ITEM_ID)
			);
			}
			sprintf(token,"�õ�%s��", ITEM_getChar( itemindex, ITEM_NAME));
			CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);

			CHAR_sendItemDataOne( talker, ret);
		}
	}

	return TRUE;
}


/*---------------------------------
 *����ë  ���ƻ����¼�ë  ������
 ------------- ---------------------*/
int NPC_EventFile(int meindex,int talker,char *arg)
{

	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buff2[256];
	char buf[1024*2];
	int i=1;
	int EvNo=0;
	int evch;
	
	/*--ɬ�ð����̻�ë  ��  ��--*/
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {

		print("GetArgStrErr");
		return -1;
	}

	/*ɬ�ð����̻�ë�������*/
	while(getStringFromIndexWithDelim( argstr, "EventEnd", i, buf,sizeof( buf))
	!= FALSE)
	{
		CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 0);
		i++;
		/*--��ì�����ϼ����ë������--*/
		NPC_Util_GetStrFromStrWithDelim( buf, "EventNo", buff2,sizeof( buff2) );
		EvNo = atoi( buff2);

		/*--�׷º���������ë�浤����ƥ�������ì��������  �ƻ������������ݳ�--*/
		if(NPC_EventCheckFlg( talker, EvNo) == TRUE) continue;

		/*--ƽ����������������ë�浤--*/
		if(NPC_Util_GetStrFromStrWithDelim( buf,"Pet_Name", buff2,sizeof( buff2) ) 
		 != NULL)
		{
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 1);
		}
		/*--ƽ����������������ë�浤--*/
		if(NPC_Util_GetStrFromStrWithDelim( buf,"KeyWord", buff2,sizeof( buff2) ) 
		 != NULL)
		{
			if(CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) != i ) continue;
		}

		evch = NPC_ExChangeManEventCheck( meindex, talker, buf);

		if(evch != -1) {
		/*--��ì������������������--*/
//		if(NPC_ExChangeManEventCheck( meindex, talker, buf) == TRUE) {
			strcpysafe(arg, sizeof( buf) ,buf);
			return evch;
		}
	}

	return -1;

}


/*---------------------------------
 * ʧ��  ة���������ͷ���ë�浤
 -----------------------------------*/
BOOL NPC_EventReduce(int meindex,int talker,char *buf)
{

	char buf2[512];
	char buf3[256];
	int id = 0;
	int i;
	int itemindex;
	int itemno;
	int kosuu;
	int cnt = 0;
	
	getStringFromIndexWithDelim( buf, "=", 2, buf2, sizeof( buf2));
	getStringFromIndexWithDelim( buf2, "*", 1, buf3, sizeof( buf3));
	itemno = atoi( buf3);
	getStringFromIndexWithDelim( buf2, "*", 2, buf3, sizeof( buf3));
	kosuu = atoi( buf3);
	
	for( i = 0 ; i < CHAR_MAXITEMHAVE ; i++ ){
		itemindex = CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) ){
			id = ITEM_getInt( itemindex ,ITEM_ID);
			if(itemno == id){
				//change add ���ӶԶѵ����ж�
				int pilenum = ITEM_getInt( itemindex, ITEM_USEPILENUMS);
				if( pilenum )
					cnt+=pilenum;
				else
					cnt++;

				if(cnt >= kosuu){
					return TRUE;
				}
			}
		}
	}

	return FALSE;

}

/*----------------------------------
 *ʸ�����嵩ƽ��ë����������
 ----------------------------------*/
void NPC_EventPetSkill( int meindex, int talker, char *data)
{

	int skill;
	int pet;
	int slot;
	int cost;
	int skillID = 0;
	char buf[64];
	char argstr[1024];
	char msg[512];
	int petindex;
	char msgbuf[128];
	int fd = getfdFromCharaIndex( talker );
	int evcnt;
	
	if( NPC_Util_CharDistance( talker, meindex ) > 2) {
		return;
	}

	evcnt = NPC_EventFile( meindex, talker, argstr);

	if(evcnt == -1) return;

	/*--ɬ�ð����̻�  ������k��  ��������ë  ���ƻ������ּ�ë  ������--*/
//	if(NPC_EventFile( meindex, talker, argstr) == FALSE) return ;

	/*�ͷ���ʧ��������߯�Ȼ����·�����--*/
	/*����  ë�����Ϸ־���(��    į)  ����ʸ��������(��    į)  
	����������������(��    į)  ���*/
	makeStringFromEscaped( data);

	/*--����������Ի����--*/
	getStringFromIndexWithDelim( data, "|", 1, buf, sizeof( buf));
	skill = atoi( buf);
	getStringFromIndexWithDelim( data, "|", 2, buf, sizeof( buf));
	pet = atoi( buf);
	getStringFromIndexWithDelim( data, "|", 3, buf ,sizeof( buf));
	slot = atoi(buf);
	getStringFromIndexWithDelim( data, "|", 4, buf, sizeof( buf));
	cost = atoi( buf);

	/*--��ƽ��    ë����������--*/
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "pet_skill", 
									msg, sizeof( msg)) != NULL) 
	{
		getStringFromIndexWithDelim( msg, ",", skill, buf ,sizeof( buf));
		skillID = atoi( buf);
	}
	slot--;


	petindex = CHAR_getCharPet( talker, pet-1);

	if(petindex == -1){
		print("PetindexErr");
		return;
	}

	/*--����ƽ�ҷ»���    ئ�գ�ʸ����ë�����ڱ�  --*/
	if( CHAR_getWorkInt( CONNECT_getCharaindex( fd),
                         CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return ;

	/*--ʸ������  ë����������--*/
	CHAR_setPetSkill( petindex, slot, skillID);
			
	/*--����ëӼ����--*/
	CHAR_setInt( talker, CHAR_GOLD, (CHAR_getInt( talker, CHAR_GOLD) - cost));

	snprintf( msgbuf, sizeof( msgbuf ), "W%d",pet-1);

	// ޥ����ʸ������  ������ë˪Ի������
	CHAR_sendStatusString( talker, msgbuf );
			
	CHAR_sendStatusString( talker, "P");

}


/*----------------------------------------
 *  ʧ��  ة���������о�����������������
----------- ------------------------------*/
BOOL NPC_ItemFullCheck(int meindex,int talker,char *buf,int mode,int evcnt)
{

	char buff2[256];
	int i = 1,j = 1;
	int maxitem = 0;
	int kosuucnt = 0;
	int itemindex;
	char buf3[256];
	int rand_j = 0;
	int rand_cnt = 0;
	char nbuf[256];
	char buff6[16];
	int l = 1;
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelItem", buff2, sizeof( buff2) )	!= NULL ){
		if(strstr(buff2,"EVDEL") != NULL){
			char buff4[64];
			char buff5[32];
			
			i = 1;
			NPC_Util_GetStrFromStrWithDelim( buf, "EVENT", buff2, sizeof( buff2));
			getStringFromIndexWithDelim(buff2, "," , evcnt, buf3, sizeof(buf3));
			
			while(getStringFromIndexWithDelim(buf3, "&" , j, buff4, sizeof(buff4))){
				j++;
				if(strstr(buff4,"ITEM") != NULL) {
					if(strstr(buff4,"*") != NULL) {
						int itemno;

						l = 1;
						getStringFromIndexWithDelim(buff4, "=" , 2, buff5, sizeof(buff5));
						getStringFromIndexWithDelim(buff5, "*" , 1, buff6, sizeof(buff6));
						itemno = atoi(buff6);
						
						if(NPC_Util_GetStrFromStrWithDelim( buf, "NotDel", nbuf, sizeof( nbuf))	!=NULL){
							while(getStringFromIndexWithDelim(nbuf , "," , l,buff6, sizeof(buff6)))
							{
								l++;
								if(itemno == atoi( buff6))
								{
									l = -1;
									break;
								}
							}
						}
						if(l == -1) continue;
						getStringFromIndexWithDelim(buff5, "*" , 2, buff6, sizeof(buff6));
						maxitem -= atoi( buff6);
					}else{
						getStringFromIndexWithDelim(buff4, "=" , 2, buff5, sizeof(buff5));
						l = 1; 

						if(NPC_Util_GetStrFromStrWithDelim( buf, "NotDel", nbuf, sizeof( nbuf))
						!=NULL){
							while(getStringFromIndexWithDelim(nbuf , "," , l,buff6, sizeof(buff6)))	{
								l++;
								if(atoi(buff5) == atoi( buff6)){
									l = -1;
									break;
								}
							}
						}
						if(l == -1) continue;
						for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ) {
							itemindex = CHAR_getItemIndex( talker , i );
							if( ITEM_CHECKINDEX( itemindex)) {
								if(atoi( buff5) == ITEM_getInt( itemindex, ITEM_ID)) {
									maxitem--;
								}
							}
						}
					}
				}
			}
		}else{
			char buff3[128];
			i = 1;
			while(getStringFromIndexWithDelim(buff2, "," , i, buff3, sizeof(buff3)) !=FALSE ){
				i++;
				if(strstr( buff3, "*") != NULL){
					getStringFromIndexWithDelim( buff3, "*", 2, buf3, sizeof( buf3));
					maxitem -= atoi( buf3);
				}else{
					for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ) {
						itemindex=CHAR_getItemIndex( talker , i );
						if( ITEM_CHECKINDEX( itemindex)) {
							if(atoi( buff3) == ITEM_getInt( itemindex, ITEM_ID)) {
							maxitem--;
							}
						}
					}
				}
			}

		}
	}

	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetRandItem", buff2, sizeof( buff2) ) !=NULL && mode == 0){
		j = 1;
		while(getStringFromIndexWithDelim(buff2 , "," , j, buf3, sizeof( buf3)) !=FALSE ){
			j++;
			rand_j++;
		}
		rand_cnt = 1;
		for( i = CHAR_STARTITEMARRAY ; i <CHAR_MAXITEMHAVE ; i++ ) {
			itemindex = CHAR_getItemIndex( talker , i );
			if( !ITEM_CHECKINDEX(itemindex) ){
				kosuucnt++;
			}
		}
		if( maxitem == 0 && kosuucnt== 0){
			return FALSE;
		}
	}


	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetItem", buff2, sizeof( buff2) )
	!= NULL && mode == 0){
		char buff3[256];
		j = 1;
		while(getStringFromIndexWithDelim(buff2 , "," , j, buff3, sizeof( buff3)) != FALSE ){
			j++;
			if(strstr(buff3,"*") !=NULL ) {
				getStringFromIndexWithDelim( buff3, "*", 2, buf3,sizeof( buf3));
				maxitem += atoi( buf3);
			}else{
				maxitem++;
			}
		}
		kosuucnt=0;
		for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){
			itemindex = CHAR_getItemIndex( talker , i );
			if( !ITEM_CHECKINDEX( itemindex)){
				kosuucnt++;
			 }
		}
		maxitem = maxitem + rand_cnt;
		if( kosuucnt < maxitem){
			return FALSE;
		}
	}


	return TRUE;
}

void NPC_CharmStatus(int meindex,int talker)
{
	int i = 0;
	int petindex;
	char petsend[64];	
	for(i = 0 ; i < CHAR_MAXPETHAVE ; i++) {
    	petindex = CHAR_getCharPet( talker, i);

		if( petindex == -1  )  continue;
		if( !CHAR_CHECKINDEX( talker ) )  continue;
		CHAR_complianceParameter( petindex );
		sprintf( petsend, "K%d", i );
		CHAR_sendStatusString( talker , petsend );
	}
}

int NPC_EventGetCost(int meindex,int talker,char *arg)
{

	int cost;
	int level;
	char buf[32];
	if(strstr( arg, "LV") != NULL) {
		level = CHAR_getInt( talker, CHAR_LV);
		getStringFromIndexWithDelim( arg, "*", 2, buf,sizeof( buf));
		cost = level * atoi( buf);
	}else{
		cost = atoi( arg);
	}
	return cost;

}

