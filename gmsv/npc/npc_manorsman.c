#include "version.h"
#include <time.h>
#include "char.h"
#include "object.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "handletime.h"
#include "npc_scheduleman.h"
#include "family.h"
#include "npc_manorsman.h"
#include "saacproto_cli.h"
#include "net.h"
#include "configfile.h"
#include "log.h"

/*
 * ׯ԰ PK �Ǽ�Ա
 *
 */
#define MAX_MANORSMAN 22
static int FMPK_ManorsmanList[MAX_MANORSMAN]={
-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,
-1,-1 };

enum {
	NPC_WORK_ID = CHAR_NPCWORKINT1,		// �Ǽ�Ա ID, �� 0 ��ʼ
	NPC_WORK_MANORID = CHAR_NPCWORKINT2,	// ׯ԰���
	NPC_WORK_CHALLENGEWAIT = CHAR_NPCWORKINT3,	// ��ս�ȴ�ʱ��
	NPC_WORK_PEACEWAIT = CHAR_NPCWORKINT4,	// ��սʱ��
	NPC_WORK_PREVLOOPTIME = CHAR_NPCWORKINT5,	// ǰһ�δ��� Loop ��ʱ��
};

#define SCHEDULEFILEDIR		"./Schedule/"

#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD ������սׯ԰����ǰʮ���Ϊǰ��ʮ��
#define PK_LIMIT	20	       
#endif


extern struct FM_POINTLIST  fmpointlist;
extern void NPC_talkToFloor(int floor, int index1, int index2, char *data);
#ifdef _MANOR_PKRULE
extern struct  FMS_DPTOP fmdptop;
#endif

void NPC_ManorLoadPKSchedule(int meindex);	// Load schedule from disk

int NPC_getManorsmanListIndex( int ID)
{

	if( ID < 0 || ID >= MAX_MANORSMAN ){
		return	FMPK_ManorsmanList[ ID];
	}
	return -1;
}
BOOL NPC_ManorSmanInit( int meindex )
{
  char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
  int meid;
  int a;

  CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEMANORSCHEDULEMAN );

  // ����
  NPC_Util_GetArgStr(meindex, argstr, sizeof(argstr));
  meid = NPC_Util_GetNumFromStrWithDelim(argstr, "id" );
  if ((meid<0) || (meid>=MAX_SCHEDULEMAN)) {
    print("MANORSMAN init error: invalid ID(%d)\n",meid);
    meid=0;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_ID, meid);

  a = NPC_Util_GetNumFromStrWithDelim(argstr, "loop" );
  if ((a<100) || (a>10000)) a=1000;
  CHAR_setInt(meindex, CHAR_LOOPINTERVAL, a);

  a = NPC_Util_GetNumFromStrWithDelim(argstr, "manorid" );
  if ((a<1) || (a>/*4*/MANORNUM)) {// CoolFish 2002/2/25
    print("MANORSMAN init error: invalid manor id(%d).", a);
    a=1;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_MANORID, a);

  a = NPC_Util_GetNumFromStrWithDelim(argstr, "challengewait");
  if ((a<1) || (a>259200)) {
    print("MANORSMAN init error: invalid challengewait(%d).",a);
    a=259200;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_CHALLENGEWAIT, a);

  a = NPC_Util_GetNumFromStrWithDelim(argstr, "peacewait");
  if ((a<1) || (a>432000)) {
    print("MANORSMAN init error: invalid peacewait(%d).",a);
    a=604800;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_PEACEWAIT, a);

  NPC_ManorLoadPKSchedule(meindex);

  CHAR_setWorkInt(meindex, NPC_WORK_PREVLOOPTIME, NowTime.tv_sec);

  if( CHAR_getWorkInt( meindex, NPC_WORK_ID) >= 0 &&
	  CHAR_getWorkInt( meindex, NPC_WORK_ID) <MAX_MANORSMAN ){
		FMPK_ManorsmanList[ CHAR_getWorkInt( meindex, NPC_WORK_ID)] = meindex;
  }

  return TRUE;
}

void NPC_ManorSmanTalked(int meindex, int talkerindex, char *msg, int color)
{
  char buf[4096];
  int fd;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

  if (NPC_Util_CharDistance(meindex, talkerindex) > 2) return;
  if (!NPC_Util_isFaceToFace(meindex, talkerindex, 2)) return;
  fd = getfdFromCharaIndex(talkerindex);
  if (fd == -1) return;
	if(fmpks[fmpks_pos+1].flag==FMPKS_FLAG_CHALLENGE){
		fmpks[fmpks_pos+1].flag=FMPKS_FLAG_NONE;
	}
  switch (fmpks[fmpks_pos+1].flag) {
  case FMPKS_FLAG_NONE:
    // û��Լ��ս������ʱ������ս��
    saacproto_ACFMPointList_send(acfd);
#ifdef _MANOR_PKRULE
	sprintf(buf, "ׯ԰����Ȩ����ս����ս�ʸ�\n\n"
                 "һ��û��ӵ��ׯ԰�ļ���\n"
	#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD ������սׯ԰����ǰʮ���Ϊǰ��ʮ��
				 "�����������б���Ϊǰ��ʮ�����\n"
	#else
                 "�����������б���Ϊǰʮ�����\n"
	#endif
				 "�����߹�ʱ�䣺�����������賿����\n\n"
                 "���Դ����ҽ�ȷ������ʸ�");
#else
    sprintf(buf, "ׯ԰����Ȩ����ս����ս�ʸ�\n\n"
                 "һ��û��ӵ��ׯ԰�ļ���\n"
                 "����������������ׯ԰���������\n\n"
                 "���Դ����ҽ�ȷ������ʸ�");
#endif
    lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
	    		WINDOW_BUTTONTYPE_YESNO,
    			CHAR_WINDOWTYPE_CHECKMAN_START,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			buf);
    break;
  case FMPKS_FLAG_MANOR_PREPARE:
    // Ŀǰ�Ѿ�����ս����׼����
    {
      int timeleft=fmpks[fmpks_pos+1].dueltime-NowTime.tv_sec;
      int dd,hh,mm;
      char buf2[4096];
      dd = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 86400;
      hh = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 3600 - dd*24;
      mm = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 60 - dd*24*60 - hh*60;
      memset(buf2,0,sizeof(buf2));
      if (dd>0) sprintf(buf, " %d ��", dd); else strcpy(buf, "");
      strcat(buf2, buf);
      if (hh>0) sprintf(buf, " %d Сʱ", hh); else strcpy(buf, "");
      strcat(buf2, buf);
      if (mm>0) sprintf(buf, " %d ����", mm); else strcpy(buf, "");
      strcat(buf2, buf);
      if (strlen(buf2)==0)
        sprintf(buf, "ׯ԰����Ȩ����ս\n��%s �֣� %s��\n\n"
                    "������ʼ����׼���볡��",
    		fmpks[fmpks_pos+1].guest_name,
    		fmpks[fmpks_pos+1].host_name);
      else
        sprintf(buf, "ׯ԰����Ȩ����ս\n��%s �֣� %s��\n\n"
                    "Ԥ������%s�Ὺʼ��",
    		fmpks[fmpks_pos+1].guest_name,
    		fmpks[fmpks_pos+1].host_name,
    		buf2);
      lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    	WINDOW_BUTTONTYPE_OK,
    			CHAR_WINDOWTYPE_CHECKMAN_END,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			buf);
    }
    break;
  case FMPKS_FLAG_MANOR_PEACE:
    // ս���Ѿ������ĺ�ƽʱ��
    {
      int timeleft=fmpks[fmpks_pos+1].dueltime-NowTime.tv_sec;
      int dd,hh,mm;
      char buf2[4096];
      dd = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 86400;
      hh = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 3600 - dd*24;
      mm = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 60 - dd*24*60 - hh*60;
      strcpy(buf2,"");
      if (dd>0) sprintf(buf, " %d ��", dd); else strcpy(buf, "");
      strcat(buf2, buf);
      if (hh>0) sprintf(buf, " %d Сʱ", hh); else strcpy(buf, "");
      strcat(buf2, buf);
      if (mm>0) sprintf(buf, " %d ����", mm); else strcpy(buf, "");
      strcat(buf2, buf);

      if (strlen(buf2)==0)
        strcpy(buf, "���Եȣ�����׼��һ�������߹ݵı��");
      else
        sprintf(buf, "��������սʱ�ڣ�Ҫ�߹ݵĻ�\n��%s���������롣",buf2);
      lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    	WINDOW_BUTTONTYPE_OK,
    			CHAR_WINDOWTYPE_CHECKMAN_END,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
    }
    break;
  case FMPKS_FLAG_MANOR_BATTLEBEGIN:
    // Ŀǰ���ڽ����߹�
    sprintf(buf, "ׯ԰����Ȩ����ս\n��%s �֣� %s��\n\n"
                 "�������ޡ�\n��û�������˸Ͽ�����ɡ�",
    		fmpks[fmpks_pos+1].guest_name,
    		fmpks[fmpks_pos+1].host_name);
    lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    	WINDOW_BUTTONTYPE_OK,
    			CHAR_WINDOWTYPE_CHECKMAN_END,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			buf);
    break;
  case FMPKS_FLAG_MANOR_OTHERPLANET:
    // �ڱ���������ս��
    sprintf(buf, "ׯ԰����Ȩ����ս\n��%s �֣� %s��\n\n"
    		 "�����ص��� %s ��",
              fmpks[fmpks_pos+1].guest_name,
              fmpks[fmpks_pos+1].host_name,
              fmpks[fmpks_pos+2].host_name);
    lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    	WINDOW_BUTTONTYPE_OK,
    			CHAR_WINDOWTYPE_CHECKMAN_END,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			buf);
    break;
  }
}

void NPC_ManorSmanWindowTalked(int meindex, int talkerindex, int seqno, int select, char *data)
{
  int fd, fmpks_pos, manorid, tkfmindex, tkfmdp;
  char buf[4096],token[256];
  int hadfmindex;
#ifndef _ACFMPK_LIST
  char hadfmname[256];
#endif
  int hadfmpopular;
  
  if (NPC_Util_CharDistance(meindex,talkerindex) > 2) return;
  if (!NPC_Util_isFaceToFace(meindex, talkerindex, 2)) return;
  fd = getfdFromCharaIndex(talkerindex);
  if (fd == -1) return;
  fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;
  manorid = CHAR_getWorkInt(meindex, NPC_WORK_MANORID);
  tkfmindex = CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI);
  tkfmdp = CHAR_getWorkInt(talkerindex, CHAR_WORKFMDP);
  getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1], "|", 5, token, sizeof(token));
  hadfmindex = atoi(token);
  getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1], "|", 7, token, sizeof(token));
  hadfmpopular = atoi(token);
  
  switch (seqno){
  case CHAR_WINDOWTYPE_CHECKMAN_START:
    if (select==WINDOW_BUTTONTYPE_YES){
#ifdef _FMVER21      
		  if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==FMMEMBER_LEADER) &&
#else
      if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==1) &&
#endif      
				(CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG)==1)){
        if (hadfmindex != -1) {
          if (hadfmindex-1 != tkfmindex){
            int check=0,i;
#ifdef _FM_POINT_PK
						if(strcmp(getFmPointPK(),"��")){
		          // Arminius 2.25 fix: fmpks �е� 1~"MANORNUM" ��һ��Ҫ�� manorsman
	            for (i=0; i<=/*3*/MANORNUM-1; i++) {	// 9��ׯ԰
	              getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, token, sizeof(token));
	              if (tkfmindex==atoi(token)-1) check=1;
	            }
	          }
#else 
  					// Arminius 2.25 fix: fmpks �е� 1~"MANORNUM" ��һ��Ҫ�� manorsman
            for (i=0; i<MANORNUM; i++) {	// 9��ׯ԰
              getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, token, sizeof(token));
              if (tkfmindex==atoi(token)-1) check=1;
            }
#endif
            for (i=1; i<=/*4*/MANORNUM; i++) {	// �Ƿ��Ѿ���ս����ׯ԰
              if ((fmpks[i*MAX_SCHEDULE+1].guest_index==tkfmindex) &&
								(strcmp(fmpks[i*MAX_SCHEDULE+1].guest_name,
								CHAR_getChar(talkerindex, CHAR_FMNAME))==0)
								) {
                check=2;
              }
            }
            if (check==0){
							int won1;
#ifdef _MANOR_PKRULE		    // WON ADD 2002/01/22
	//-----------------------------------------
						int i;
						char won2[256];
						won1 = 0;
	#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD ������սׯ԰����ǰʮ���Ϊǰ��ʮ��		
						for(i=0; i< PK_LIMIT; i++){
	#else
						for(i=0; i<10; i++){
	#endif
							if( getStringFromIndexWithDelim( fmdptop.topmemo[i], "|", 3, won2, sizeof(won2)) == FALSE )	{
								print("err Get fmdptop.topmemo[%d] if FALSE !!\n", i);
								break;
							}
							if( strcmp(CHAR_getChar(talkerindex, CHAR_FMNAME) ,won2) == 0 ){
								won1= 1;
								break;
							}
	//-----------------------------------------
						}
							if(won1 == 1){		
		// WON END
							sprintf(buf,"ׯ԰��ս����һ���׼����\n"
													"�������ʱѯ�����Ե�֪ʣ�µ�ʱ��\n"
													"��ս�����᲻��ʤ�������ж������ս��\n"
													"��ս�ڼ䲻����ս\n\n"
													"��ȷ��Ҫ��ս���ׯ԰��");
#else
              if(tkfmdp >= hadfmpopular){
								sprintf(buf,"ׯ԰��ս����һ���׼����\n"
                            "�������ʱѯ�����Ե�֪ʣ�µ�ʱ��\n"
												    "��ս�����᲻��ʤ���������������ս��\n"
														"��ս�ڼ䲻����ս\n\n"
														"��ȷ��Ҫ��ս���ׯ԰��");
#endif
                lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    							WINDOW_BUTTONTYPE_YESNO,
    															CHAR_WINDOWTYPE_CHECKMAN_MAIN,
    															CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    															buf);
    					}
							else{
#ifdef _MANOR_PKRULE
	#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD ������սׯ԰����ǰʮ���Ϊǰ��ʮ��
               sprintf(buf, "��ļ���������δ����ǰ��ʮ����壬���ټ��͡�");
	#else
               sprintf(buf, "��ļ���������δ����ǰʮ����壬���ټ��͡�");
	#endif
#else
               sprintf(buf, "��ļ��������в�������ս\n"
                            "ӵ�����ׯ԰�ļ��壬���ټ��͡�");
#endif
                lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						              	    	WINDOW_BUTTONTYPE_OK,
													   			CHAR_WINDOWTYPE_CHECKMAN_END,
												    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
													   			buf);
		   	      }
            }
						else if(check == 2){
              sprintf(buf, "��ļ���������ս����ׯ԰��\n"
                           "��ѻ���������������ɣ�");
              lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
													    	WINDOW_BUTTONTYPE_OK,
											    			CHAR_WINDOWTYPE_CHECKMAN_END,
											    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
											    			buf);
            }
						else{
              sprintf(buf, "һ������ֻ��ӵ��һ��ׯ԰��\n"
                           "���ׯ԰����֮����Ҫ�д�\n"
                           "�뵽����У˳���");
              lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    						WINDOW_BUTTONTYPE_OK,
											    			CHAR_WINDOWTYPE_CHECKMAN_END,
											    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
										    				buf);
            }
          }
					else{
            sprintf(buf, "���ׯ԰�Ѿ�����ļ����ม�");
            lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              						  	WINDOW_BUTTONTYPE_OK,
									    				CHAR_WINDOWTYPE_CHECKMAN_END,
											   			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
										    			buf);
          }
        }
				else{
          sprintf(buf, "���ڲ�û��ׯ԰���壡\n"
                       "ֱ��ȥ����Ǩ��Ϳ�����Ӵ��");
          lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			              	    	WINDOW_BUTTONTYPE_OK,
    												CHAR_WINDOWTYPE_CHECKMAN_END,
									    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
									    			buf);
        }
      }
			else{
        sprintf(buf, "ֻ���峤������ս��ม�");
        lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    			WINDOW_BUTTONTYPE_OK,
									   			CHAR_WINDOWTYPE_CHECKMAN_END,
								    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
								    			buf);
      }
    }
    break;
  case CHAR_WINDOWTYPE_CHECKMAN_MAIN:	  
    if (select==WINDOW_BUTTONTYPE_YES) {
#ifdef _FMVER21    
      if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==FMMEMBER_LEADER) && 
#else
      if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==1) &&     
#endif      
          (CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG)==1) &&   
          (hadfmindex != -1) &&
		  (hadfmindex - 1 != tkfmindex)    
#ifndef _MANOR_PKRULE
          && (tkfmdp >= hadfmpopular)
#endif
		  ){
#ifndef _ACFMPK_LIST
		  struct tm tm1; 
#endif

#ifdef _MANOR_PKRULE
	  time_t timep;
	  struct tm *p;		  
	  time(&timep);
	  p = gmtime(&timep);		
	  if( p->tm_hour+8 > 24)
	  	  p->tm_hour = p->tm_hour-16;
	  else
		  p->tm_hour = p->tm_hour+8;		 
	  if(p->tm_hour<18 && p->tm_hour>2){
		  sprintf(buf, "������磶���������賿������������Լս�ɣ�");
		  lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
             	    	WINDOW_BUTTONTYPE_OK,
   			        CHAR_WINDOWTYPE_CHECKMAN_END,
   			        CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
   			        buf);
		  break;
	  }		  
#endif
			// WON ADD ��������pk����Լս����
			if( fmpks[fmpks_pos+1].flag != FMPKS_FLAG_NONE ){
			     sprintf(buf, "���ׯ԰�Ѿ�����Լս��ม�");
					 lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						 WINDOW_BUTTONTYPE_OK,
						 CHAR_WINDOWTYPE_CHECKMAN_END,
						 CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
						 buf);
					 return;
			}
#ifdef _ACFMPK_LIST
		NPC_ManorSavePKSchedule(meindex, talkerindex, 0);
		sprintf( buf, "ׯ԰��ս��½ȷ���У������ᡣ");
		CHAR_talkToCli( talkerindex, meindex, buf, CHAR_COLORYELLOW);
#else
		NPC_ManorSavePKSchedule(meindex, talkerindex, 0);
#endif
        
      }
    }
    break;
  }
}

void NPC_CleanPkList( int ti)
{
// Terry fix Ҫ��Ϊ-1������Ϊ0
/*	fmpks[ ti+1 ].host_index = 0;
	fmpks[ ti+1].guest_index=0;
	fmpks[ ti].host_index=0;
	fmpks[ ti].guest_index=0;*/
	fmpks[ ti+1 ].host_index = -1;
	fmpks[ ti+1].guest_index=-1;
	fmpks[ ti].host_index=-1;
	fmpks[ ti].guest_index=-1;

	strcpy(fmpks[ ti+1].host_name,"");
	strcpy(fmpks[ ti+1].guest_name,"");
	strcpy(fmpks[ ti].host_name,"");
	strcpy(fmpks[ ti].guest_name,"");
	strcpy(fmpks[ ti+2].host_name,"");
}

void NPC_ManorSmanLoop(int meindex)
{
	struct tm tm1;
	int fmpks_pos;
	if(CHAR_CHECKINDEX(meindex) == FALSE){
		printf("\nNPC_ManorSmanLoop error!(meindex:%d)",meindex);
		return;
	}

	fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;
  memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));

  switch (fmpks[fmpks_pos+1].flag) {
  case FMPKS_FLAG_NONE: 
    break;
  case FMPKS_FLAG_MANOR_PREPARE:
    if(fmpks[fmpks_pos+1].dueltime <= NowTime.tv_sec){
      memcpy(&fmpks[fmpks_pos], &fmpks[fmpks_pos+1], sizeof(FamilyPKSchedule));
      fmpks[fmpks_pos].dueltime = tm1.tm_hour * 100 + tm1.tm_min;
      fmpks[fmpks_pos].flag = FMPKS_FLAG_SCHEDULED;
      fmpks[fmpks_pos+1].dueltime = 0;
      fmpks[fmpks_pos+1].flag = FMPKS_FLAG_MANOR_BATTLEBEGIN;
      NPC_talkToFloor(CHAR_getInt(meindex, CHAR_FLOOR) , fmpks[fmpks_pos].host_index,
         	fmpks[fmpks_pos].guest_index, "ׯ԰����ս�Ѿ���ʼ���뾡���볡��");
    }
    break;
  case FMPKS_FLAG_MANOR_PEACE:
    if (fmpks[fmpks_pos+1].dueltime <= NowTime.tv_sec) {
      fmpks[fmpks_pos+1].dueltime = 0;
      fmpks[fmpks_pos+1].flag = FMPKS_FLAG_NONE;
    }
    break;
  case FMPKS_FLAG_MANOR_BATTLEBEGIN:
    break;
  case FMPKS_FLAG_MANOR_BATTLEEND:
		{
			fmpks[fmpks_pos].flag = FMPKS_FLAG_NONE;
			fmpks[fmpks_pos+1].dueltime = CHAR_getWorkInt(meindex, NPC_WORK_PEACEWAIT) + NowTime.tv_sec;
			fmpks[fmpks_pos+1].flag = FMPKS_FLAG_MANOR_PEACE_SAVE;
			NPC_CleanPkList( fmpks_pos);
		}
    break;
  case FMPKS_FLAG_MANOR_PEACE_SAVE:
    fmpks[fmpks_pos+1].flag = FMPKS_FLAG_MANOR_PEACE;
    break;
  case FMPKS_FLAG_MANOR_OTHERPLANET:
    break;
  case FMPKS_FLAG_MANOR_READYTOFIGHT:
  case FMPKS_FLAG_MANOR_CLEANFLAG:
    fmpks[fmpks_pos].flag = FMPKS_FLAG_NONE;
    fmpks[fmpks_pos+1].dueltime = CHAR_getWorkInt(meindex, NPC_WORK_PEACEWAIT) + NowTime.tv_sec;
    fmpks[fmpks_pos+1].flag = FMPKS_FLAG_NONE;

		NPC_CleanPkList( fmpks_pos);
    break;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_PREVLOOPTIME, NowTime.tv_sec);
}

void NPC_ManorLoadPKSchedule(int meindex)
{
#ifdef _ACFMPK_LIST
	int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID);
	saacproto_ACLoadFmPk_send(acfd, fmpks_pos);
#else
	char filename[256], tmp[4096], token[256];
	FILE *f;
	int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

	snprintf(filename,sizeof(filename), "%s%d_%d_%d",
		       SCHEDULEFILEDIR, CHAR_getInt(meindex, CHAR_FLOOR),
			   CHAR_getInt(meindex, CHAR_X),
			CHAR_getInt(meindex, CHAR_Y) );

	if( ! (f=fopen( filename, "r" )) ){	// create new schedule file
		f = fopen( filename, "w" );
		if( !f ){
			print( "ERROR:Can't create Schedule file %s!\n",filename );
			return;
		}
		strcpy(tmp,"0|-1|-1|-1|-1|-1|-1|-1|-1\n");
		fwrite(tmp, strlen(tmp), 1, f);
	}

	fseek(f, 0, SEEK_SET);	// ������ͷ
	fgets(tmp, sizeof(tmp), f);
	fmpks[fmpks_pos].flag=-1;

	// ʱ��
	if (getStringFromIndexWithDelim(tmp,"|",1,token,sizeof(token))) {
		fmpks[fmpks_pos+1].dueltime=atoi(token);
	}
	// ���� familyindex
	if (getStringFromIndexWithDelim(tmp,"|",2,token,sizeof(token))) {
	    fmpks[fmpks_pos+1].host_index=atoi(token);
	}
	// ���� ������
	if (getStringFromIndexWithDelim(tmp,"|",3,token,sizeof(token))) {
		strcpy(fmpks[fmpks_pos+1].host_name,makeStringFromEscaped(token));
	}
	// �Ͷ� familyindex
	if (getStringFromIndexWithDelim(tmp,"|",4,token,sizeof(token))) {
	    fmpks[fmpks_pos+1].guest_index=atoi(token);
	}
	// �Ͷ� ������
	if (getStringFromIndexWithDelim(tmp,"|",5,token,sizeof(token))) {
	    strcpy(fmpks[fmpks_pos+1].guest_name,makeStringFromEscaped(token));
	}
	// ׼��ʱ��
	if (getStringFromIndexWithDelim(tmp,"|",6,token,sizeof(token))) {
		fmpks[fmpks_pos+1].prepare_time=atoi(token);
	}
	// �������
	if (getStringFromIndexWithDelim(tmp,"|",7,token,sizeof(token))) {
		fmpks[fmpks_pos+1].max_player=atoi(token);
	}
	// ���
	if (getStringFromIndexWithDelim(tmp,"|",8,token,sizeof(token))) {
		fmpks[fmpks_pos+1].flag=atoi(token);
	}
	// ��ս����
	if (getStringFromIndexWithDelim(tmp,"|",9,token,sizeof(token))) {
		strcpy(fmpks[fmpks_pos+2].host_name,makeStringFromEscaped(token));
	}
  
	// no schedule
	fmpks[fmpks_pos].flag=-1;
	// prepare time expired?
	if ((fmpks[fmpks_pos+1].flag==FMPKS_FLAG_MANOR_PREPARE) &&
		  (fmpks[fmpks_pos+1].dueltime<NowTime.tv_sec)) {
		fmpks[fmpks_pos].flag=-1;
// Terry fix Ҫ��Ϊ-1������Ϊ0
//		fmpks[fmpks_pos].host_index=0;
		fmpks[fmpks_pos].host_index=-1;
		strcpy(fmpks[fmpks_pos].host_name,"");
// Terry fix Ҫ��Ϊ-1������Ϊ0
//		fmpks[fmpks_pos].guest_index=0;
		fmpks[fmpks_pos].guest_index=-1;
		strcpy(fmpks[fmpks_pos].guest_name,"");
		fmpks[fmpks_pos+1].flag=-1;
		fmpks[fmpks_pos+1].dueltime=0;
	//    fmpks[fmpks_pos+1].dueltime=NowTime.tv_sec+30*60;	// add more 30 minutes
	}
	fclose(f);
#endif // end AC_SEND_FM_PK

}
#ifdef _ACFMPK_LIST
void NPC_ManorSavePKSchedule(int meindex, int toindex, int flg)
{
	int fmpks_pos, manorid, dueltime;
	char msg[1024], n1[256], n2[256], n3[256];
	char token[256], hadfmname[256];
	int hadfmindex=0, hadfmpopular=0, tkfmindex=0;
	int PkFlg=0;
	if( toindex == -1 ){
		dueltime = CHAR_getWorkInt(meindex, NPC_WORK_PEACEWAIT) + NowTime.tv_sec;
		sprintf(msg, "%d|%d|%s|%d|%s|%d|%d|%d|%s",
			dueltime, 0, "", 0, "", 0, 0, flg, "");
		PkFlg = 0;
	}else{
		struct tm tm1;
		int index;
		tkfmindex = CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI);
		fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
		manorid = CHAR_getWorkInt(meindex, NPC_WORK_MANORID);
		memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));
		dueltime = CHAR_getWorkInt(meindex, NPC_WORK_CHALLENGEWAIT) + NowTime.tv_sec - tm1.tm_min*60 + 1800;
		getStringFromIndexWithDelim( fmpointlist.pointlistarray[manorid-1], "|", 5, token, sizeof(token));
		hadfmindex = atoi( token);
		getStringFromIndexWithDelim( fmpointlist.pointlistarray[manorid-1], "|", 7, token, sizeof(token));
		hadfmpopular = atoi( token);
		getStringFromIndexWithDelim( fmpointlist.pointlistarray[manorid-1], "|", 6, token, sizeof(token));
		strcpy( hadfmname, token);
		makeEscapeString( hadfmname, n1, sizeof(n1));
		makeEscapeString( CHAR_getChar(toindex, CHAR_FMNAME), n2, sizeof(n2));		
		makeEscapeString( getGameserverID(), n3, sizeof(n3));
		sprintf(msg, "%d|%d|%s|%d|%s|%d|%d|%d|%s",
			dueltime, hadfmindex-1, n1, tkfmindex, n2, 15, 50, FMPKS_FLAG_MANOR_OTHERPLANET, n3);
		PkFlg = 1;
		{
			char buf1[256];
			sprintf( buf1,"%d", CHAR_getWorkInt( meindex, NPC_WORK_ID) );
			Logfmpk(
				 n1, hadfmindex-1, 0,
				 n2, tkfmindex, 0,
				 "", buf1, n3, 1);
		}
	}
	saacproto_ACSendFmPk_send( acfd, -1, PkFlg, CHAR_getWorkInt( meindex, NPC_WORK_ID), msg);
}
#endif
