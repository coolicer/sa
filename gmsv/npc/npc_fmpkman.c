#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_fmpkman.h"
#include "npc_scheduleman.h"
#include "npc_fmwarpman.h"
#include "family.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"

enum {
	NPC_WORK_ID = CHAR_NPCWORKINT1,
};

static void NPC_FMPKMan_selectWindow(int meindex, int toindex, int num, int select);
// shan add
BOOL NPC_PARTY_CHAECK1(int meindex,int talker);
void NPC_ERR_DiSP1(int meindex,int talker,int errNO);

/*********************************
* ������  
*********************************/
BOOL NPC_FMPKManInit( int meindex )
{

	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024];
	char buff2[256];
	int fl, x, y, meid;

	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg)) == NULL){
		print("FMPKMan:GetArgStrErr");
		return FALSE;
	}

	/*--������ɬ�ý�ľ�����¾�----*/
	/*--������ɬ�ý�ľ��ئ��ľ��NPCë����ئ�г��������--*/
	if(NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP", buf, sizeof( buf))==NULL){
	        print("FMPKMan Err is %s",npcarg);
		print("FMPKMan Err");
		return FALSE;
	}

	/*--������ɬ�ý�ľ���л��ַ������ƻ�ئ��ľ������Ƿ��NPCë����ئ��--*/
	getStringFromIndexWithDelim(buf,",",1,buff2,sizeof(buff2));
	fl=atoi(buff2);
	getStringFromIndexWithDelim(buf,",",2,buff2,sizeof(buff2));
	x=atoi(buff2);
	getStringFromIndexWithDelim(buf,",",3,buff2,sizeof(buff2));
	y=atoi(buff2);

	if( MAP_IsValidCoordinate( fl,x,y )== FALSE ){
		print( "FMWarp NPC:Invalid warpman ERR" );
		return FALSE;
	}
	
	meid = NPC_Util_GetNumFromStrWithDelim(npcarg, "ID");
	if ((meid < 0) || (meid >= MAX_SCHEDULEMAN))
	{
		print("FMPKMAN NPC: Init error Invalid ID:%d", meid);
		return	FALSE;
	}
	CHAR_setWorkInt(meindex, NPC_WORK_ID, meid);

	/*--������ɬ��--*/
   	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEWARPMAN );

    return TRUE;

}

/*********************************
*   �ƾ�����ľ���ݼ���  
*********************************/
void NPC_FMPKManTalked( int meindex , int talkerindex , char *szMes ,int color )
{
    /* �����������帲�ƻ�����  ɱ���� */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	
	/*--  �������¾���������--*/
	if(NPC_Util_isFaceToFace(talkerindex,meindex,2 )==FALSE){
		/* ���������  ���� */
		if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;
	}

	/*--�����ͼ������--*/
	CHAR_setWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT, 0);

	/*-��Ԫ������      --*/
	NPC_FMPKMan_selectWindow( meindex, talkerindex, 0, -1);
}

static void NPC_FMPKMan_selectWindow( int meindex, int toindex, int num,int select)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[1024], buf[256], buf2[256];
	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex(toindex);
	int num1 = 0, num2 = 0;
	int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
	
	if( fd == -1 ) {
		print( "getfd err\n");
		return;
	}
	
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return ;
	}
	/*--ɬ�ð����̻Ｐ  ��׷�ʧ���ѻ�϶�ý�ľ�����¾���ľ����ľ�ɰ׷�ʧ���Ѽ��Ի����*/
	if(strstr(npcarg,"%4d")!=NULL){
		int work;
		NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP", buf, sizeof( buf));
		getStringFromIndexWithDelim(buf,",",1,buf2,sizeof(buf2));
		work = atoi( buf2);
	}

	token[0] = '\0';
	
	switch(num){
      //   �ټ���ū������
	  case 0:
	  	if(NPC_Util_GetStrFromStrWithDelim( npcarg, "MainMsg", buf,
	  		sizeof( buf)) == NULL)
	  			return;
			sprintf(token, "3\n����    ���������У˳���\n"
				"%s"
				"\n           �� �쿴˫������ ��"
				"\n            �� �뿪�У˳� ��",
				buf);
		buttontype = WINDOW_BUTTONTYPE_NONE;
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		windowno = CHAR_WINDOWTYPE_FMPKMAN_START; 
		break;
	  case 1:
	  	if(NPC_Util_GetStrFromStrWithDelim(npcarg, "ViewMsg", buf,
	  		sizeof(token)) == NULL)
	  			return;
	  	NPC_GetPKFMNum(CHAR_getInt(toindex, CHAR_FLOOR),
	  		fmpks[fmpks_pos].host_index,
	  		fmpks[fmpks_pos].guest_index,
	  		&num1, &num2);
/*
	  	print("host:%s guest:%s hostindex:%d guestindex:%d\n",
	  		fmpks[fmpks_pos].host_name,
	  		fmpks[fmpks_pos].guest_name,
	  		fmpks[fmpks_pos].host_index,
	  		fmpks[fmpks_pos].guest_index);
*/
	  	sprintf(token, "\n%s\n\n%s:%4d��\n\n%s:%4d��", buf,
	  		fmpks[fmpks_pos].host_name, num1,
	  		fmpks[fmpks_pos].guest_name, num2);
	  	buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno =  CHAR_WINDOWTYPE_FMPKMAN_VIEW;
	  	break;
	  case 2:
	  	if(NPC_Util_GetStrFromStrWithDelim(npcarg, "LeavepkMsg", token,
	  		sizeof(token)) == NULL)
	  			return;
	  			
	  	buttontype = WINDOW_BUTTONTYPE_YESNO;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno =  CHAR_WINDOWTYPE_FMPKMAN_LEAVEPK;
	  	break;
	  default:
	  	break;
	}
	
	/*--�޵�������--*/
	//makeEscapeString( token, escapedname, sizeof(escapedname));
		
		
	/*--˪��--*/
	lssproto_WN_send( fd, windowtype, 
				buttontype, 
				windowno,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);

}

/*-----------------------------------------
 * �ͷ���ʧ��������߯�Ȼ������ݱ���̫���ľ�£�
 *
-------------------------------------------*/
void NPC_FMPKManWindowTalked( int meindex, int talkerindex, 
		int seqno, int select, char *data)
{
	int datanum = -1, fl, x, y;
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024], buff2[256];

	if (NPC_Util_GetArgStr(meindex, npcarg, sizeof(npcarg)) == NULL)
	{
		print("GetArgStrErr");
		return;
	}
	NPC_Util_GetStrFromStrWithDelim(npcarg, "WARP", buf, sizeof(buf));
	getStringFromIndexWithDelim(buf, ",", 1, buff2, sizeof(buff2));
	fl = atoi(buff2);
	getStringFromIndexWithDelim(buf, ",", 2, buff2, sizeof(buff2));
	x = atoi(buff2);
	getStringFromIndexWithDelim(buf, ",", 3, buff2, sizeof(buff2));
	y = atoi(buff2);
	
	makeStringFromEscaped( data);

//	print("meindex:%d seqno:%d select:%d data:%s\n", meindex, seqno, select, data);
	
	datanum = atoi( data);
	switch( seqno){

	/*--��Ԫ��Ի��    --*/
	  case CHAR_WINDOWTYPE_FMPKMAN_START:
	  	if (datanum == 1)
	  		NPC_FMPKMan_selectWindow(meindex, talkerindex, 1, -1);
	  	else if (datanum == 2)
	  		NPC_FMPKMan_selectWindow(meindex, talkerindex, 2, -1);
		break;
	  case CHAR_WINDOWTYPE_FMPKMAN_VIEW:
	  	break;
	  case CHAR_WINDOWTYPE_FMPKMAN_LEAVEPK:
	  	if (select == WINDOW_BUTTONTYPE_YES)
	  	{
	  	   	if (CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLEMODE)
	  	   		!= BATTLE_CHARMODE_NONE)
	  	   			return;
	  	   			
	  	        // shan add
	  	        if(NPC_PARTY_CHAECK1( meindex, talkerindex)==FALSE){
	  	                NPC_ERR_DiSP1( meindex, talkerindex, 1);
	  	                return;
	  	        }
	  	                                                   
	  	   	CHAR_setWorkInt(talkerindex, CHAR_WORKWARPCHECK, FALSE);
	  	   	CHAR_warpToSpecificPoint(talkerindex, fl, x, y);
	  	}
	  	break;
	  default:
	  	break;
	}
}

// shan add
BOOL NPC_PARTY_CHAECK1(int meindex,int talker)
{
    if(CHAR_getWorkInt(talker,CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
         return FALSE;
    }
    return TRUE;
}

// shan add  errNO=1(���)
void NPC_ERR_DiSP1(int meindex,int talker,int errNO)
{
    char token[1024];
    int  i=0;
    int  otherindex;
    int  fd = getfdFromCharaIndex( talker);
    char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
    
    if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
       print("GetArgStrErr");
       return ;
    }
         
    if(errNO==1){
       if(NPC_Util_GetStrFromStrWithDelim( npcarg, "PartyMsg",token, sizeof( token))==NULL){
           sprintf(token, "�޷����Ŷ��볡��\n\n����Ŷӽ�ɢ֮���ٸ���\n�볡��");
       }
       
       if(CHAR_getWorkInt(talker,CHAR_WORKPARTYMODE)==CHAR_PARTY_CLIENT){
       
       }else{
           for( i=0 ; i < CHAR_PARTYMAX ;i++){
               otherindex=CHAR_getWorkInt(talker,CHAR_WORKPARTYINDEX1+i);
               if(otherindex != -1){
                   fd = getfdFromCharaIndex( otherindex);
                   lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
                                         WINDOW_BUTTONTYPE_OK,
                                         CHAR_WINDOWTYPE_WINDOWWARPMAN_ERR,
                                         CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
                                         token);
               }
           }
           return ;
       }
    }
}                   



