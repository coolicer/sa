#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_fmpkcallman.h"
#include "family.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"

// WON ADD �����峤�ٻ�������
#include "npc_scheduleman.h"
#include "handletime.h"


static void NPC_FMPKCallMan_selectWindow(int meindex, int toindex, int num, int select);
void NPC_CallFMMember(int meindex, int floor, int fmindex, char *fmname, int index);

/*********************************
* ������  
*********************************/
BOOL NPC_FMPKCallManInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024];
	char buff2[256];
	int fl, x, y;

	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg)) == NULL){
		print("FMPKCallMan:GetArgStrErr");
		return FALSE;
	}

	/*--������ɬ�ý�ľ�����¾�----*/
	/*--������ɬ�ý�ľ��ئ��ľ��NPCë����ئ�г��������--*/
	if(NPC_Util_GetStrFromStrWithDelim( npcarg, "WARP", buf, sizeof( buf))==NULL){
	        print("FMPKCallMan Err is %s",npcarg);
		print("FMPKCallMan Err");
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

	/*--������ɬ��--*/
   	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEWARPMAN );

    return TRUE;
}

/*********************************
*   �ƾ�����ľ���ݼ���  
*********************************/
void NPC_FMPKCallManTalked( int meindex , int talkerindex , char *szMes ,int color )
{
    /* �����������帲�ƻ�����  ɱ���� */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	
	/*--  �������¾���������--*/
	if(NPC_Util_isFaceToFace(talkerindex,meindex,2 )==FALSE){
		/* ���������  ���� */
		if( NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}

	/*--�����ͼ������--*/
	CHAR_setWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT, 0);

	/*-��Ԫ������      --*/
	NPC_FMPKCallMan_selectWindow( meindex, talkerindex, 0, -1);
}

static void NPC_FMPKCallMan_selectWindow( int meindex, int toindex,
	int num,int select)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[1024], buf[256], buf2[256];
	int fd = getfdFromCharaIndex(toindex);
	
	if( fd == -1 ) {
		fprint( "getfd err\n");
		return;
	}

	// print("meindex:%d toindex:%d num:%d select:%d", meindex, toindex, num, select);
	
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
	
	switch(num) {
	   
	   case 0:
	   	if (NPC_Util_GetStrFromStrWithDelim(npcarg, "MainMsg", buf,
	   		sizeof(buf)) == NULL)
	   			return;
	   	sprintf(token, "3\n               �����У˳���\n"
	   			"%s"
	   			"\n              ���ٻ������Ա��"
	   			"\n               �����ؼ�¼�㡷"
	   			"\n                  ��ȡ����",
	   			buf);
	   	lssproto_WN_send(fd, WINDOW_MESSAGETYPE_SELECT,
	   		WINDOW_BUTTONTYPE_NONE,
	   		CHAR_WINDOWTYPE_FMPKCALLMAN_START,
	   		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
	   	break;
	   case 1:
#ifdef _FMVER21	   
	   	if( CHAR_getInt( toindex, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER)
#else
	   	if( CHAR_getInt( toindex, CHAR_FMLEADERFLAG) == 1)
#endif	   	
	   	{
	   	   if (CHAR_getWorkInt(toindex, CHAR_WORKFMSETUPFLAG) != 1)
	   	   {
	   	      if (NPC_Util_GetStrFromStrWithDelim(npcarg, "NoSetupMsg", buf,
	   	      	 sizeof(buf)) == NULL)
	   	      	 	return;
	   	      if (strstr(buf, "%s") != NULL)
	   	      	 sprintf(token, buf, CHAR_getChar(toindex, CHAR_FMNAME));
	   	      lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
	   	         WINDOW_BUTTONTYPE_OK, -1, -1, token);
	   	   }
	   	   else
	   	   {
	   	      if (NPC_Util_GetStrFromStrWithDelim(npcarg, "CallMsg", buf,
	   	         sizeof(buf)) == NULL)
	   	      	 	return;
	   	      if (strstr(buf, "%s") != NULL)
	   	      	 sprintf(token, buf, CHAR_getChar(toindex, CHAR_FMNAME));

// WON ADD �����峤�ٻ�������
			  {
					int fmindex, fmpk_pos=-1, j;
					int now_time;
					struct  tm tm1;
// Terry add for ����ֵΪ 0 �ļ����������,���Զ���жϼ�������
					char szFMName[32];
// end
					
					memcpy(&tm1,localtime((time_t *)&NowTime.tv_sec),sizeof(tm1));

					fmindex = CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI);
// Terry add for ����ֵΪ 0 �ļ����������,���Զ���жϼ�������
					strncpy(szFMName,CHAR_getChar(toindex,CHAR_FMNAME),sizeof(szFMName));
// end

					for( j=0; j <= MAX_SCHEDULEMAN*MAX_SCHEDULE; j++ ){
// Terry fix for ����ֵΪ 0 �ļ����������,���Զ���жϼ�������
//						if( (fmindex == fmpks[j].host_index ) || (fmindex == fmpks[j].guest_index ) ){
						if((fmindex == fmpks[j].host_index && strcmp(szFMName,fmpks[j].host_name) == 0) || 
							 (fmindex == fmpks[j].guest_index && strcmp(szFMName,fmpks[j].guest_name) == 0)){
//end
							fmpk_pos=j;
							break;
						}
					}
					
					if(fmpk_pos != -1){

						if(fmpk_pos <= MANORNUM*MAX_SCHEDULE+1){
							now_time=NowTime.tv_sec+(60*60);
						}else{
							if(tm1.tm_hour+1==24)  now_time=10000;
							else                   now_time=(tm1.tm_hour+1) * 100;
						}

						if( (fmpks[fmpk_pos].flag != FMPKS_FLAG_SCHEDULED) &&
							(fmpks[fmpk_pos].flag != FMPKS_FLAG_MANOR_PREPARE) )
						{
                            sprintf(token,"��û�м���Լս������Լս�ɡ�");
	   						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
	   							WINDOW_BUTTONTYPE_OK, -1, -1, token);

						}else if( now_time  < fmpks[fmpk_pos].dueltime ){
	   	      		
							sprintf(token,"���ڶ�սǰһСʱ�����ٻ���Ա��!");
	   						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
	   							WINDOW_BUTTONTYPE_OK, -1, -1, token);

						}else{
	   						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
	   							WINDOW_BUTTONTYPE_YESNO,
	   							CHAR_WINDOWTYPE_FMPKCALLMAN_CALL,
	   							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
						}
					}else{
                            sprintf(token,"��û�м���Լս������Լս�ɡ�");
                            lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
                                     WINDOW_BUTTONTYPE_OK, -1, -1, token);
					}
			  }

	   	   }
	   	} else {
	   	   if (NPC_Util_GetStrFromStrWithDelim(npcarg, "NoLeaderMsg", buf,
	   	         sizeof(buf)) == NULL)
	   	      	 	return;
	   	   lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
	   	      WINDOW_BUTTONTYPE_OK, -1, -1, buf);
	   	}
	   	break;
	   case 2:
	   	   if (NPC_Util_GetStrFromStrWithDelim(npcarg, "LeavepkMsg", buf,
	   	         sizeof(buf)) == NULL)
	   	      	 	return;
	   	      lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
	   	         WINDOW_BUTTONTYPE_YESNO,
	   	         CHAR_WINDOWTYPE_FMPKCALLMAN_LEAVE,
	   	         CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), buf);
	   	break;
	}
}

/*-----------------------------------------
 * �ͷ���ʧ��������߯�Ȼ������ݱ���̫���ľ�£�
 *
-------------------------------------------*/
void NPC_FMPKCallManWindowTalked( int meindex, int talkerindex, 
		int seqno, int select, char *data)
{
	int datanum = -1, fl, x, y;
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024], buff2[256];
	
	int fd = getfdFromCharaIndex(talkerindex);
	if (fd == -1)	return;
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

	print("meindex:%d seqno:%d select:%d data:%s\n", meindex, seqno, select, data);
	
	datanum = atoi( data);
	switch( seqno){

	/*--��Ԫ��Ի��    --*/
	  case CHAR_WINDOWTYPE_FMPKCALLMAN_START:
	  	if (datanum == 1)
	  		NPC_FMPKCallMan_selectWindow(meindex, talkerindex, 1, -1);
	  	else if (datanum == 2)
	  		NPC_FMPKCallMan_selectWindow(meindex, talkerindex, 2, -1);
	  	break;
	  case CHAR_WINDOWTYPE_FMPKCALLMAN_CALL:
	  	if (select == WINDOW_BUTTONTYPE_YES)
	  	{
	  	   	lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
	  	   		WINDOW_BUTTONTYPE_OK, -1, -1,
	  	   		makeEscapeString("\n�ٻ��С��������Ժ�\n�����磥������������", buf, sizeof(buf)));
	  		NPC_CallFMMember(meindex,
	  			CHAR_getInt(talkerindex, CHAR_FLOOR),
	  			CHAR_getInt(talkerindex, CHAR_FMINDEX),
	  			CHAR_getChar(talkerindex, CHAR_FMNAME),
	  			CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI));
	  	}
	  	break;
	  case CHAR_WINDOWTYPE_FMPKCALLMAN_COME:
	  	if (select == WINDOW_BUTTONTYPE_YES)
	  	{
	  		print("CHAR_WINDOWTYPE_FMPKCALLMAN_COME\n");
	  	   	if (CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLEMODE)
	  	   		!= BATTLE_CHARMODE_NONE)
	  	   			return;
	  	   	if (CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE)
	  	   		!= CHAR_PARTY_NONE)
		  	   		CHAR_DischargeParty(talkerindex, 0);

	  	   	CHAR_setWorkInt(talkerindex, CHAR_WORKWARPCHECK, FALSE);
	  	   	CHAR_setWorkInt(talkerindex, CHAR_WORKFMPKFLAG, 1);
	  	   	CHAR_warpToSpecificPoint(talkerindex, fl, x, y);
	  	}
	  	break;
	  case CHAR_WINDOWTYPE_FMPKCALLMAN_LEAVE:
	  	if (select == WINDOW_BUTTONTYPE_YES)
	  	{
	  		int spfl = 0, spx = 0, spy = 0, i = 0, itemindex = 0;
	  		if (CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLEMODE)
	  			!= BATTLE_CHARMODE_NONE)
	  				return;
	  		if (CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE)
	  			!= CHAR_PARTY_NONE)
	  		{
	  			lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
	  				WINDOW_BUTTONTYPE_OK, -1, -1,
	  				makeEscapeString("\n�޷����Ŷӷ�ʽ�뿪ࡣ�\n���Ƚ�ɢ�Ŷӣ�", buf, sizeof(buf)));
	  			return;
	  		}
	  		for (i = 0; i < CHAR_MAXITEMHAVE; i++)
	  		{
	  			itemindex = CHAR_getItemIndex(talkerindex, i);
	  			if (ITEM_CHECKINDEX(itemindex) == FALSE)	continue;

	  			if (ITEM_getInt(itemindex, ITEM_DROPATLOGOUT) == TRUE)
	  			{
	  				lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
	  					WINDOW_BUTTONTYPE_OK, -1, -1,
	  					makeEscapeString("\n�������й�����Ʒร�\nΪ�˱����ڴ���;�в�С���𻵣�\n���Ƚ�������Ʒж�£�\nлл���ĺ�����", buf, sizeof(buf)));
	  				return;
	  			}
	  				
	  		}
	  	   	CHAR_setWorkInt(talkerindex, CHAR_WORKWARPCHECK, FALSE);
	  	   	CHAR_getElderPosition(CHAR_getInt(talkerindex, CHAR_LASTTALKELDER),
	  	   		&spfl, &spx, &spy);
	  	   	CHAR_warpToSpecificPoint(talkerindex, spfl, spx, spy);
	  	}
	  	break;
	  default:
	  	break;
	}
}

extern struct FM_PKFLOOR fmpkflnum[FAMILY_FMPKFLOOR];
void NPC_CallFMMember(int meindex, int floor, int fmindex, char *fmname, int index)
{
	int i, charindex;
	char buf[256];
	extern int familyMemberIndex[FAMILY_MAXNUM][FAMILY_MAXMEMBER];
	print("CallFMMember_NPC_meindex:%d\n", meindex);
        for( i = 0; i < FAMILY_MAXMEMBER; i++)
        {
           charindex = familyMemberIndex[index][i];
           if( charindex >= 0 ) {
              if( CHAR_getCharUse(charindex) )
	      {
	      	 print("charindex:%d name:%s\n", charindex, CHAR_getChar(charindex, CHAR_NAME));
	         if (CHAR_getInt(charindex, CHAR_FLOOR) == floor)	continue;
	         if (CHAR_getWorkInt(charindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE)
	         {
	            int fd = getfdFromCharaIndex(charindex);
	            int charfloor = CHAR_getInt(charindex, CHAR_FLOOR);
	            int j = 0, checkflag = 0;
	            if (fd == -1)	continue;
#ifdef _FMVER21	            
	            if (CHAR_getInt(charindex, CHAR_FMLEADERFLAG) < 0 ||
	                CHAR_getInt(charindex, CHAR_FMLEADERFLAG) == FMMEMBER_APPLY)	continue;
#else
	            if (CHAR_getInt(charindex, CHAR_FMLEADERFLAG) <= 0 )	continue;
#endif	                
	            for (j = 0; j < FAMILY_FMPKFLOOR; j++)
	            {
	            	if (charfloor == fmpkflnum[j].fl)
	            		checkflag = 1;
	            }
	            if (checkflag != 1)
	            	lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
	            		WINDOW_BUTTONTYPE_YESNO, CHAR_WINDOWTYPE_FMPKCALLMAN_COME,
	            		CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
	            		makeEscapeString("\n�����Ѿ��ڣУ��ޡ�Ҫ��Ҫ�����أ�\n�������������״̬�У����������Ŷ�ࡣ�", buf, sizeof(buf)));
	         }
	      }	 
	      else
	         familyMemberIndex[index][i] = -1;
	   }
	}                                                                                                                                    
}
