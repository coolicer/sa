#include "version.h"
#include <time.h>
#include <string.h>
#include "char.h"
#include "object.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "npcutil.h"
#include "handletime.h"
#include "npc_fmdengon.h"
#include "family.h"

#define DENGONFILELINENUM      35     // ���������������ϱ���
#define FMSDENGONFILELINENUM   140    // ���������԰����ϱ���
#define DENGONFILEENTRYSIZE    128    // ���Ĵ�С
#define MESSAGEINONEWINDOW     7      // ÿҳ����ʾ�ı���
#define FMMAXNUM               1000   // �������������ֵ
#define FM_MEMBERLIST          2      // �����Ա�б�    (�����ܱ�İ���)
#define FM_MEMBERMEMO          3      // ��������        (�����ܱ�İ���)
#define FM_FMMEMO              4      // ����֮�����԰�  (�����ܱ�İ���)
#define FM_FMPOINT             5      // �������ݵ�    (�����ܱ�İ���)
#define FM_FMDPTOP             6      // �����ǿ�߱�    (�����ܱ�İ���)
#define FM_WAITTIME            (3*60)
#define FMSDENGON_SN           10000  // ����֮������԰��ʶ����

extern struct  FMMEMBER_LIST memberlist[FMMAXNUM];         // ���� AC ��Ա�б����ϵ� ARRAY 
extern struct  FMS_MEMO fmsmemo;                           // ����֮������԰�
extern struct  FM_POINTLIST fmpointlist;                   // ����ݵ�
extern struct  FMS_DPTOP fmdptop;                          // ����ǿ�߱�
extern int leaderdengonindex;                              // 777 ���幫���� index
char NPC_sendbuf[DENGONFILEENTRYSIZE*MESSAGEINONEWINDOW];  // һ��ҳ�Ĵ�С
char enlistbuf[4096];                                        // �����Ա��ļ BUF(��ʾ�õ�)

unsigned long READTIME1 = 0,
              READTIME2 = 0,
              READTIME3 = 0,
              READTIME4 = 0;

#ifdef _FIX_FM_FMPOINT
void ApplyFamilyPoint( int meindex, int toindex, int select);
#endif

// �������ĳ�ʼ��(when gmsv start)
BOOL NPC_FmDengonInit( int meindex)
{
    int fmindex=CHAR_getInt(meindex, CHAR_FMINDEX);
    CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEDENGON);
    if(fmindex>0){
	    
	    
//    if( CHAR_getInt(meindex, CHAR_FLOOR) == 777 ){
//        leaderdengonindex = meindex;
//    }
	    
	    if( READTIME1 == 0 || READTIME2 == 0 || READTIME3 == 0 || READTIME4 == 0 ){
	        READTIME1 = NowTime.tv_sec+FM_WAITTIME,
	        READTIME2 = NowTime.tv_sec+FM_WAITTIME,
	        READTIME3 = NowTime.tv_sec+FM_WAITTIME,
	        READTIME4 = NowTime.tv_sec+FM_WAITTIME;
	       
	        // ȡ�ü���ĳ�Ա�б�(memberlist struct)���Լ���������԰�

	        saacproto_ACShowMemberList_send( acfd, fmindex);
	        saacproto_ACFMReadMemo_send( acfd, fmindex);

	        // ����֮������԰�������ֵԤ��Ϊ FMSDENGON_SN
	        saacproto_ACFMReadMemo_send( acfd, FMSDENGON_SN);
	        saacproto_ACFMPointList_send(acfd);
	        saacproto_ACShowTopFMList_send(acfd, FM_TOP_INTEGRATE);
	        saacproto_ACShowTopFMList_send(acfd, FM_TOP_ADV);    
	        saacproto_ACShowTopFMList_send(acfd, FM_TOP_FEED);
	        saacproto_ACShowTopFMList_send(acfd, FM_TOP_SYNTHESIZE);
	        saacproto_ACShowTopFMList_send(acfd, FM_TOP_DEALFOOD);
	        saacproto_ACShowTopFMList_send(acfd, FM_TOP_PK);
	    }
	    
	 }
	 return TRUE;
}

// Select Event
void NPC_FmDengonWindowTalked( int index, int talker, int seqno, int select, char *data)
{
//    char buf[DENGONFILEENTRYSIZE*MESSAGEINONEWINDOW*2];
    char buf[4096];
    int  buttonevent;
    int  buttontype = 0;
    struct timeval recvtime;
    if (!CHAR_CHECKINDEX(talker)) return;
    
    CONNECT_getLastrecvtime_D( getfdFromCharaIndex( talker), &recvtime);
    if( time_diff( NowTime, recvtime) < 0.5 ){
        return;
    }
    
    CONNECT_setLastrecvtime_D( getfdFromCharaIndex( talker), &NowTime);
#ifndef _FM_MODIFY
    // ���볬�� DENGONDISTANCE ��  Χ��ʱ����ȡ������
#define DENGONDISTANCE 3	
    if( CHAR_getInt(index, CHAR_FLOOR) != 777 )
        if(NPC_Util_CharDistance( index, talker) > DENGONDISTANCE) return;
#endif
    
    // �������԰�
    if( seqno == CHAR_WINDOWTYPE_FM_DENGON)
    {
			int dengonindex;
			int fmindex_wk;
			char tmp_buffer[4096],tmp[4096];
			
			getStringFromIndexWithDelim(data,"|",1,tmp_buffer,sizeof(tmp_buffer));
			dengonindex = atoi(tmp_buffer);
			
			fmindex_wk = CHAR_getWorkInt( talker, CHAR_WORKFMINDEXI);
			
			if( fmindex_wk < 0 || fmindex_wk >= FMMAXNUM) return;
			
			switch( select){
			case WINDOW_BUTTONTYPE_NEXT:
			case WINDOW_BUTTONTYPE_PREV:
				{
					int fd,i;
					fd = getfdFromCharaIndex( talker);
					if( fd == -1) return;
					
					dengonindex += 7 * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
					if( dengonindex > memberlist[fmindex_wk].memoindex && memberlist[fmindex_wk].memonum < DENGONFILELINENUM)
						dengonindex = memberlist[fmindex_wk].memoindex;
					else if( dengonindex < 6 && memberlist[fmindex_wk].memonum < DENGONFILELINENUM) 
						dengonindex = 6;
					else if( dengonindex < 1 && memberlist[fmindex_wk].memonum >= DENGONFILELINENUM)
						dengonindex = memberlist[fmindex_wk].memonum+dengonindex;
					else if( dengonindex > memberlist[fmindex_wk].memonum && memberlist[fmindex_wk].memonum >= DENGONFILELINENUM)
						dengonindex -= memberlist[fmindex_wk].memonum;
					
					buttontype = WINDOW_BUTTONTYPE_OKCANCEL;
					if( dengonindex==memberlist[fmindex_wk].memoindex && memberlist[fmindex_wk].memonum >= DENGONFILELINENUM) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;
					else if( (dengonindex - 7)<=memberlist[fmindex_wk].memoindex && (dengonindex - 7)>=(memberlist[fmindex_wk].memoindex - 7) &&
						memberlist[fmindex_wk].memonum >= DENGONFILELINENUM) 
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else if( dengonindex==memberlist[fmindex_wk].memoindex) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;    
					else if( dengonindex == 6) buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else{
						buttontype |= WINDOW_BUTTONTYPE_PREV;
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}
					if(dengonindex >= 6){
						strcpy( NPC_sendbuf, memberlist[fmindex_wk].memo[dengonindex - 6]);
						strcat( NPC_sendbuf, "\n");
						for( i=(dengonindex - 5); i<=dengonindex; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					if(dengonindex < 6){
						strcpy( NPC_sendbuf, memberlist[fmindex_wk].memo[memberlist[fmindex_wk].memonum+(dengonindex - 6)]);
						strcat( NPC_sendbuf, "\n");
						for( i=memberlist[fmindex_wk].memonum+(dengonindex - 5); i<memberlist[fmindex_wk].memonum; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						for( i=0; i<=dengonindex; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DENGON,
						buttontype,
						CHAR_WINDOWTYPE_FM_DENGON,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
				}
				break;
			case WINDOW_BUTTONTYPE_OK:
				{
					int    fd,i;
					struct tm tm1;
					char   m_buf[4096];
					
					fd = getfdFromCharaIndex( talker);
					if( fd == -1 ) return;
					if( strlen(data) == 0 ) break;
					
					memberlist[fmindex_wk].memonum++;
					if( memberlist[fmindex_wk].memonum > DENGONFILELINENUM) 
						memberlist[fmindex_wk].memonum = DENGONFILELINENUM;
					
					memberlist[fmindex_wk].memoindex++;
					if( memberlist[fmindex_wk].memoindex >= DENGONFILELINENUM) 
						memberlist[fmindex_wk].memoindex=0;
					
					dengonindex = memberlist[fmindex_wk].memoindex;
					
					if( dengonindex < 6 && memberlist[fmindex_wk].memonum<DENGONFILELINENUM)
						dengonindex = 6;
					
					memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof(struct tm));
					getStringFromIndexWithDelim(data,"|",2,tmp_buffer,sizeof(tmp_buffer));
					sprintf( m_buf,"%s|%2d/%02d %2d:%02d %s",
						tmp_buffer,
						tm1.tm_mon +1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min,
						CHAR_getChar( talker, CHAR_NAME));
					strcpy(memberlist[fmindex_wk].memo[memberlist[fmindex_wk].memoindex], m_buf);
					
					// send acsv 
					saacproto_ACFMWriteMemo_send( acfd, CHAR_getChar( talker, CHAR_FMNAME), 
						CHAR_getInt(talker, CHAR_FMINDEX),
						makeEscapeString( memberlist[fmindex_wk].memo[memberlist[fmindex_wk].memoindex], buf, sizeof(buf)),
						fmindex_wk);
					
					if( dengonindex >= 6){
						strcpy( NPC_sendbuf, memberlist[fmindex_wk].memo[dengonindex - 6]);
						strcat( NPC_sendbuf, "\n");
						for( i=(dengonindex-5); i<=dengonindex; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					if( dengonindex < 6){
						strcpy( NPC_sendbuf, memberlist[fmindex_wk].memo[memberlist[fmindex_wk].memonum+(dengonindex - 6)]);
						strcat( NPC_sendbuf, "\n");
						for( i=memberlist[fmindex_wk].memonum+(dengonindex - 5); i<memberlist[fmindex_wk].memonum; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						for( i=0; i<=dengonindex; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DENGON,
						WINDOW_BUTTONTYPE_OKCANCEL|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DENGON,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
				}
				break;
			default:
				break;
      }  // Switch End
    }  // If End
        
    // ����֮�����԰�
    else if(seqno == CHAR_WINDOWTYPE_FM_FMSDENGON)
    {
			int dengonindex;
			char tmp_buffer[4096],tmp[4096];
			getStringFromIndexWithDelim(data,"|",1,tmp_buffer,sizeof(tmp_buffer));
			dengonindex = atoi(tmp_buffer);
			
			switch( select ){
			case WINDOW_BUTTONTYPE_NEXT:
			case WINDOW_BUTTONTYPE_PREV:
				{
					int fd,i;
					fd = getfdFromCharaIndex( talker);
					if( fd == -1 ) return;
					
					dengonindex += 7 * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
					if( dengonindex > fmsmemo.memoindex && fmsmemo.memonum < FMSDENGONFILELINENUM)
						dengonindex = fmsmemo.memoindex;
					else if( dengonindex < 6 && fmsmemo.memonum < FMSDENGONFILELINENUM) 
						dengonindex = 6;
					else if( dengonindex < 1 && fmsmemo.memonum >= FMSDENGONFILELINENUM)
						dengonindex = fmsmemo.memonum+dengonindex;
					else if( dengonindex > fmsmemo.memonum && fmsmemo.memonum >= FMSDENGONFILELINENUM)
						dengonindex -= fmsmemo.memonum;
					
					buttontype = WINDOW_BUTTONTYPE_OKCANCEL;
					if( dengonindex==fmsmemo.memoindex && fmsmemo.memonum >= FMSDENGONFILELINENUM) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;
					else if( (dengonindex-7)<=fmsmemo.memoindex && (dengonindex - 7)>=(fmsmemo.memoindex - 7) &&
						fmsmemo.memonum >= FMSDENGONFILELINENUM) 
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else if( dengonindex == fmsmemo.memoindex) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;    
					else if( dengonindex == 6 ) 
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else{
						buttontype |= WINDOW_BUTTONTYPE_PREV;
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}
					
					if( dengonindex >= 6 ){
						if( (dengonindex - 6)  >= 140 ) return;
						strcpy( NPC_sendbuf, fmsmemo.memo[dengonindex - 6]);
						strcat( NPC_sendbuf, "\n");
						for( i=(dengonindex - 5); i<=dengonindex; i++){
							strcat( NPC_sendbuf, fmsmemo.memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					if(dengonindex < 6){
						if( (fmsmemo.memonum+(dengonindex - 6))  >= 140 ||
							(fmsmemo.memonum+(dengonindex - 6))  < 0 ) return;
						
						strcpy( NPC_sendbuf, fmsmemo.memo[fmsmemo.memonum+(dengonindex - 6)]);
						
						strcat( NPC_sendbuf, "\n");
						for( i=fmsmemo.memonum+(dengonindex - 5); i<fmsmemo.memonum; i++){
							strcat( NPC_sendbuf, fmsmemo.memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						for( i=0; i<=dengonindex; i++){
							strcat( NPC_sendbuf, fmsmemo.memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_FMSDENGON,
						buttontype,
						CHAR_WINDOWTYPE_FM_FMSDENGON,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
				}
				break;
			case WINDOW_BUTTONTYPE_OK:
				{
					int    fd,i;
					struct tm tm1;
					char   m_buf[4096];
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
#ifdef _FMVER21  
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER){              
#else
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) != 1){              
#endif              
							sprintf( NPC_sendbuf, "              ����       �桻\n ��Ǹ���㲻���峤�����Խ��ܲ鿴��");
							lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_OK,
								-1,
								-1,
								makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
							return;
						}
						
						if( strlen( data) == 0 )  break;
						
						fmsmemo.memonum++;
						if( fmsmemo.memonum > FMSDENGONFILELINENUM) 
							fmsmemo.memonum = FMSDENGONFILELINENUM;
            
						fmsmemo.memoindex++;
						if( fmsmemo.memoindex >= FMSDENGONFILELINENUM) 
							fmsmemo.memoindex = 0;
						
						dengonindex = fmsmemo.memoindex;
						if( dengonindex < 6 && fmsmemo.memonum<FMSDENGONFILELINENUM)
							dengonindex = 6;
						
						memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));
						getStringFromIndexWithDelim(data,"|",2,tmp_buffer,sizeof(tmp_buffer));
						sprintf( m_buf,"%s|%2d/%02d %2d:%02d %s",
							tmp_buffer,
							tm1.tm_mon +1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min,
							CHAR_getChar( talker, CHAR_NAME));
						strcpy( fmsmemo.memo[fmsmemo.memoindex], m_buf);
						
						// send acsv 
						saacproto_ACFMWriteMemo_send( acfd, "FMS", 
							FMSDENGON_SN,
							makeEscapeString( fmsmemo.memo[fmsmemo.memoindex], buf, sizeof(buf)),
							FMSDENGON_SN);
						
						if( dengonindex >= 6){
							strcpy( NPC_sendbuf, fmsmemo.memo[dengonindex - 6]);
							strcat( NPC_sendbuf, "\n");
							for( i=(dengonindex - 5); i<=dengonindex; i++){
								strcat( NPC_sendbuf, fmsmemo.memo[i]);
								strcat( NPC_sendbuf, "\n");
							}
							sprintf(tmp, "%d\n", dengonindex);
							strcat( NPC_sendbuf, tmp);
						}
						if(dengonindex < 6){
							strcpy( NPC_sendbuf, fmsmemo.memo[fmsmemo.memonum+(dengonindex - 6)]);
							strcat( NPC_sendbuf, "\n");
							for( i=fmsmemo.memonum+(dengonindex - 5); i<fmsmemo.memonum; i++){
								strcat( NPC_sendbuf, fmsmemo.memo[i]);
								strcat( NPC_sendbuf, "\n");
							}
							for( i=0; i<=dengonindex; i++){
								strcat( NPC_sendbuf, fmsmemo.memo[i]);
								strcat( NPC_sendbuf, "\n");
							}
							sprintf(tmp, "%d\n", dengonindex);
							strcat( NPC_sendbuf, tmp);
						}
						lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_FMSDENGON,
							WINDOW_BUTTONTYPE_OKCANCEL|
							WINDOW_BUTTONTYPE_PREV,
							CHAR_WINDOWTYPE_FM_FMSDENGON,
#ifndef _FM_MODIFY
							CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
							-1,
#endif
							makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
					}
					break;
				default: break;
      }  // Switch End
    }  // If End
        
    // ˵���Ӵ�(����ݵ�)
    else if( seqno == CHAR_WINDOWTYPE_FM_MESSAGE1)
    {
			int fd;
			char pointbuf[1024];
			strcpy( pointbuf, "");
			fd = getfdFromCharaIndex( talker );
			if( fd == -1 )  return;

			switch( select ){
			case WINDOW_BUTTONTYPE_OK:
				{
#ifdef _FIX_FM_FMPOINT
					char x[4];
					char y[4];
					char fmindex[4];
					char name[32];
					int windowtype = WINDOW_MESSAGETYPE_MESSAGE;
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER &&
						          CHAR_getWorkInt(talker, CHAR_WORKFMSETUPFLAG)==1){
						CHAR_setInt( talker, CHAR_LISTPAGE,0);
						int tkfmindex = CHAR_getWorkInt(talker, CHAR_WORKFMINDEXI);
						int i,check=TRUE;
						char fmindex[4];
						for(i=0; i<MANORNUM; i++){	// 10��ׯ԰
						  getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, fmindex, sizeof(fmindex));
						  if(tkfmindex==atoi(fmindex)-1 ){
						  	 check=FALSE;
						   	 break;
						  }
						}
					  if(check==TRUE){
							for (i=CHAR_getInt( talker, CHAR_LISTPAGE); i<CHAR_getInt( talker, CHAR_LISTPAGE)+5; i++){	// 10��ׯ԰
							  getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, fmindex, sizeof(fmindex));
						 	  if(atoi(fmindex)<=0 ){
						   		check=TRUE;
						    	break;
						  	}
						  }
						}
					  if(check==TRUE){
					   	windowtype = WINDOW_MESSAGETYPE_SELECT;
							strcpy(pointbuf,"3\n        ��    ������ݵ��б�\n\n");
					  }else{
					  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
					   	strcpy(pointbuf,"        ��    ������ݵ��б�\n\n");
							
						}
					}else{
						strcpy(pointbuf,"        ��    ������ݵ��б�\n\n");
					}
					strcat(pointbuf," ���ء��㡱���������ϡ���״����̬��\n");
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",3, x, sizeof( x));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",4, y, sizeof( y));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",5, fmindex, sizeof( fmindex));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",6, name, sizeof( name));
					if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
					sprintf(pointbuf,"%s  ��ķ����  %3s   %3s   %s\n", pointbuf, x, y, name);
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",3, x, sizeof( x));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",4, y, sizeof( y));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",5, fmindex, sizeof( fmindex));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",6, name, sizeof( name));
					if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
					sprintf(pointbuf,"%s  ������˹  %3s   %3s   %s\n", pointbuf, x, y, name);
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",3, x, sizeof( x));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",4, y, sizeof( y));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",5, fmindex, sizeof( fmindex));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",6, name, sizeof( name));
					if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
					sprintf(pointbuf,"%s  �ӡ�����  %3s   %3s   %s\n", pointbuf, x, y, name);
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",3, x, sizeof( x));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",4, y, sizeof( y));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",5, fmindex, sizeof( fmindex));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",6, name, sizeof( name));
					if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
					sprintf(pointbuf,"%s  ��³����  %3s   %3s   %s\n", pointbuf, x, y, name);
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",3, x, sizeof( x));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",4, y, sizeof( y));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",5, fmindex, sizeof( fmindex));
					getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",6, name, sizeof( name));
					if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
					sprintf(pointbuf,"%s  ��������  %3s   %3s   %s\n", pointbuf, x, y, name);
#else
					int i;
					strcpy( pointbuf, fmpointlist.pointlistarray[0]);
					strcat( pointbuf, "\n");
					for( i=1; i<=FMPOINTNUM; i++){
						strcat( pointbuf, fmpointlist.pointlistarray[i]);
						strcat( pointbuf, "\n");
					}
#endif

#ifdef _FIX_FM_FMPOINT
					lssproto_WN_send( fd, windowtype,
						WINDOW_BUTTONTYPE_OK|WINDOW_BUTTONTYPE_NEXT,
						CHAR_WINDOWTYPE_FM_POINTLIST,
#else
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_POINTLIST,
						WINDOW_BUTTONTYPE_OK,
						CHAR_WINDOWTYPE_FM_POINTLIST,
#endif
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif

#ifdef _FIX_FM_FMPOINT
					pointbuf);
#else
					makeEscapeString( pointbuf, buf, sizeof(buf)));
#endif
				}
				break;
			default:
				break;
			}
    }        

    // ˵���Ӵ�(��Ա�б�)
    else if( seqno == CHAR_WINDOWTYPE_FM_MESSAGE2)
    {
			int fd,i;
			char numberlistbuf[4096];
			int fmindex_wk;
			fmindex_wk = CHAR_getWorkInt( talker, CHAR_WORKFMINDEXI);
			
			if( fmindex_wk < 0 || fmindex_wk >= FMMAXNUM) return;
			
			fd = getfdFromCharaIndex( talker );
			if( fd == -1 )  return;
			
			switch( select ){
			case WINDOW_BUTTONTYPE_OK:
				{
					strcpy( numberlistbuf, memberlist[fmindex_wk].numberlistarray[0]);
					strcat( numberlistbuf, "\n");
					for( i=1; i<10; i++){
						strcat( numberlistbuf, memberlist[fmindex_wk].numberlistarray[i]);
						strcat( numberlistbuf, "\n");
					}
					// ���ӳ�ʽ��(��ACҪ��ļ��Ա��ֵ)
					sprintf(enlistbuf, "�Ƿ������ļ������Ա|0|%d",memberlist[fmindex_wk].accept);
					strcat( numberlistbuf, enlistbuf);
					strcat( numberlistbuf, "\n");
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_SELECT,
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_NEXT,
						CHAR_WINDOWTYPE_FM_MEMBERLIST,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( numberlistbuf, buf, sizeof(buf)));
				}
				break;
			default:
				break;
			}
    }        

    // ǿ�߱��ѡ���Ӵ�
    else if( seqno == CHAR_WINDOWTYPE_FM_DPSELECT)
    {
			int fmindex_wk;
			fmindex_wk = CHAR_getWorkInt( talker, CHAR_WORKFMINDEXI);
			
			if( CHAR_getInt(talker, CHAR_FMINDEX) > 0 ){
				if( fmindex_wk < 0 || fmindex_wk >= FMMAXNUM){
					print("FamilyNumber Data Error!!");
					return;
				}
			}
			
			buttonevent = atoi(data);
			switch( buttonevent ){
			case 1:				// ǰ��ʮ������ۺ������б�
				{
					int  fd,i;
					char listbuf[4096];
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( listbuf, fmdptop.topmemo[0]);
					strcat( listbuf, "\n");
					for( i=1; i<10; i++){
						strcat( listbuf, fmdptop.topmemo[i]);
						strcat( listbuf, "\n");
					}
					strcat( listbuf, "0\n");
#ifdef _FMVER21              
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_TOP30DP,
#else
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
#endif              
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_NEXT,
						CHAR_WINDOWTYPE_FM_DPTOP,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case 2:				// ǰʮ�����ð���б�
				{
					int  fd,i;
					char listbuf[4096];
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( listbuf, fmdptop.adv_topmemo[0]);
					strcat( listbuf, "\n");
					for( i=1; i<10; i++){
						strcat( listbuf, fmdptop.adv_topmemo[i]);
						strcat( listbuf, "\n");
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DPME,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case 3:				// ǰʮ����������б�
				{
					int  fd,i;
					char listbuf[4096];
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( listbuf, fmdptop.feed_topmemo[0]);
					strcat( listbuf, "\n");
					for( i=1; i<10; i++){
						strcat( listbuf, fmdptop.feed_topmemo[i]);
						strcat( listbuf, "\n");
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DPME,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case 4:				// ǰʮ�����ϳ��б�
				{
					int  fd,i;
					char listbuf[4096];
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( listbuf, fmdptop.syn_topmemo[0]);
					strcat( listbuf, "\n");
					for( i=1; i<10; i++){
						strcat( listbuf, fmdptop.syn_topmemo[i]);
						strcat( listbuf, "\n");
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DPME,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case 5:				// ǰʮ����������б�
				{
					int  fd,i;
					char listbuf[4096];
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( listbuf, fmdptop.food_topmemo[0]);
					strcat( listbuf, "\n");
					for( i=1; i<10; i++){
						strcat( listbuf, fmdptop.food_topmemo[i]);
						strcat( listbuf, "\n");
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DPME,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case 6:				// ǰʮ�����У��б�
				{
					int  fd,i;
					char listbuf[4096];
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( listbuf, fmdptop.pk_topmemo[0]);
					strcat( listbuf, "\n");
					for( i=1; i<10; i++){
						strcat( listbuf, fmdptop.pk_topmemo[i]);
						strcat( listbuf, "\n");
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
						WINDOW_BUTTONTYPE_OK|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DPME,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case 7:				// �Լ������������а�
				{
					int  fd,i,h,k,fmid;
					char listbuf[4096];
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					fmid = CHAR_getWorkInt(talker, CHAR_WORKFMINDEXI);
					if( fmid < 0 ){
						sprintf( NPC_sendbuf, "              ����       �桻\n ��Ǹ���㲻�Ǽ�����Ա���޷��鿴��");
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
							-1,
							-1,
							makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
						return;
					}
					
					for( h=0; h<FMMAXNUM; h++)
						if( fmdptop.fmtopid[h] == fmid ) 
							break;
						
						k = h;                 // ��ʾ��ɫ��(�ഫһ��1���Թ�Client֮��)
						if(h <= 4) h = 0;
						else if(h >= 994 ) h = 990;
						else h -= 4;
						
						strcpy( listbuf, fmdptop.topmemo[h]);
						if( k == h ) strcat( listbuf, "|1");
						strcat( listbuf, "\n");
						for( i = h + 1; i < h + 10; i++){
							strcat( listbuf, fmdptop.topmemo[i]);
							if(i == k) strcat( listbuf, "|1");
							strcat( listbuf, "\n");
						}
#ifdef _FMVER21              
						lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_TOP30DP,
#else
            lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
#endif              
							WINDOW_BUTTONTYPE_OK|
							WINDOW_BUTTONTYPE_PREV,
							CHAR_WINDOWTYPE_FM_DPME,
#ifndef _FM_MODIFY
							CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
							-1,
#endif
							makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			default:
				break;
        }
    }
        
    // ѡ���Ӵ�
    else if( seqno == CHAR_WINDOWTYPE_FM_SELECT)
    {
			int fmindex_wk;
			fmindex_wk = CHAR_getWorkInt( talker, CHAR_WORKFMINDEXI);
			
			if( CHAR_getInt(talker, CHAR_FMINDEX) > 0 ){
				if( fmindex_wk < 0 || fmindex_wk >= FMMAXNUM){
					print("FamilyNumber Data Error!!");
					return;
				}
			}
			
			buttonevent = atoi(data);
			
			switch( buttonevent ){
			case FM_MEMBERLIST:
				{
					int fd;
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					if( CHAR_getInt(talker, CHAR_FMINDEX) <= 0){  
						sprintf( NPC_sendbuf, "              ����       �桻\n ��Ǹ���㲻�Ǽ�����Ա������ʹ�ù�������");
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
							-1, -1, makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
						return;
					}
					/*
					#ifdef _FMVER21                   
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER ||
					CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_ELDER ){
					#else
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) == 1){
					#endif              
					saacproto_ACShowMemberList_send( acfd, fmindex_wk);
					READTIME1 = NowTime.tv_sec+FM_WAITTIME;
					}else
					*/
					if( NowTime.tv_sec > READTIME1 ){
						saacproto_ACShowMemberList_send( acfd, fmindex_wk);
						READTIME1 = NowTime.tv_sec+FM_WAITTIME;
					}
					
#ifdef _FMVER21                   
					//              if( CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER || 
					//                  CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_ELDER ||
					//                  CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_VICELEADER ){
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER || 
						CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_ELDER ){
#else
						if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) == 1 ){
#endif              
							sprintf( NPC_sendbuf, "               ���� �� �� ֪��\n��С�Ĵ�����Ա�����ϣ�һ���޸�����޷��ظ�ԭ̬������С�ġ�");
						}else{
							sprintf( NPC_sendbuf, "               ���� �� �� ֪��\n �˱��峤�����޸ģ���Ա���ܲ鿴��");
						}
						
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
							WINDOW_BUTTONTYPE_OK,
							CHAR_WINDOWTYPE_FM_MESSAGE2,
#ifndef _FM_MODIFY
							CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
							-1,
#endif
							makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
          }
          break;
			case FM_FMPOINT:
				{
					int fd;
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
#ifdef _FMVER21                   
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER ){
#else
						if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) == 1 ){
#endif              
							saacproto_ACFMPointList_send(acfd);
							sprintf( NPC_sendbuf, "               ���� �� �� ֪��\n��С����ѡ������ľݵ㣬һ��ѡȡ�ݵ�����޷��ظ�ԭ̬������С�ġ�");
							READTIME4 = NowTime.tv_sec+FM_WAITTIME;
						}
						else{
							sprintf( NPC_sendbuf, "               ���� �� �� ֪��\n�˱��峤�������룬���Ž��ܲ鿴��");
						}
						
						if( NowTime.tv_sec > READTIME4 ){
							saacproto_ACFMPointList_send(acfd);
							READTIME4 = NowTime.tv_sec+FM_WAITTIME;
						}
						
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
							WINDOW_BUTTONTYPE_OK,
							CHAR_WINDOWTYPE_FM_MESSAGE1,
#ifndef _FM_MODIFY
							CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
							-1,
#endif
							makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
          }
          break;
			case FM_FMDPTOP:
				{
					int  fd;
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					if( NowTime.tv_sec > READTIME3 ){
						saacproto_ACShowTopFMList_send( acfd, FM_TOP_INTEGRATE );
						saacproto_ACShowTopFMList_send( acfd, FM_TOP_ADV );    
						saacproto_ACShowTopFMList_send( acfd, FM_TOP_FEED );
						saacproto_ACShowTopFMList_send( acfd, FM_TOP_SYNTHESIZE );
						saacproto_ACShowTopFMList_send( acfd, FM_TOP_DEALFOOD );
						saacproto_ACShowTopFMList_send( acfd, FM_TOP_PK );                           
						READTIME3 = NowTime.tv_sec+FM_WAITTIME;
					}
					memset(NPC_sendbuf,0,sizeof(NPC_sendbuf));
					strcpy( NPC_sendbuf, "\n              ��ʮ����������б�\n");
					strcat( NPC_sendbuf, "              ʮ��ð�ռ���\n");
					strcat( NPC_sendbuf, "              ʮ����������\n");
					strcat( NPC_sendbuf, "              ʮ��ϳɼ���\n");
					strcat( NPC_sendbuf, "              ʮ���������\n");
					strcat( NPC_sendbuf, "              ʮ��ս������\n");
					strcat( NPC_sendbuf, "              �Լ����������б�\n");			
						
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT,
						WINDOW_BUTTONTYPE_NONE,
						CHAR_WINDOWTYPE_FM_DPSELECT,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
					
				}
				break;
			case FM_MEMBERMEMO:
				{
					int fd,i,dengonindex;
					char tmp[4096];
					fd = getfdFromCharaIndex( talker );
					
					if( fd == -1 )  return;
					
					if( CHAR_getInt(talker, CHAR_FMINDEX) <= 0){  
						sprintf( NPC_sendbuf, "              ����       �桻\n ��Ǹ���㲻�Ǽ�����Ա������ʹ�ù�������");
						
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
							WINDOW_BUTTONTYPE_OK,
							-1,
							-1,
							makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
						return;
					}
					
					if( NowTime.tv_sec > READTIME2 ){
						saacproto_ACFMReadMemo_send( acfd, fmindex_wk);
						READTIME2 = NowTime.tv_sec+FM_WAITTIME;
					}
					
					dengonindex = memberlist[fmindex_wk].memoindex;
					if( memberlist[fmindex_wk].memoindex < 6 && memberlist[fmindex_wk].memonum < DENGONFILELINENUM ){
						dengonindex = 6;
					}    
					
					if( dengonindex >= 6 ){
						strcpy( NPC_sendbuf, memberlist[fmindex_wk].memo[dengonindex - 6]);
						strcat( NPC_sendbuf, "\n");
						for( i=(dengonindex - 5); i<=dengonindex; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					if( dengonindex < 6 ){
						strcpy( NPC_sendbuf,
							memberlist[fmindex_wk].memo[memberlist[fmindex_wk].memonum + (dengonindex - 6)]);
						strcat( NPC_sendbuf, "\n");
						for( i=memberlist[fmindex_wk].memonum + (dengonindex - 5); i<memberlist[fmindex_wk].memonum; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						for( i=0; i<=dengonindex; i++){
							strcat( NPC_sendbuf, memberlist[fmindex_wk].memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DENGON,
						WINDOW_BUTTONTYPE_OKCANCEL|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_DENGON,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
				}
				break;
			case FM_FMMEMO:
				{
					int fd,i,dengonindex;
					char tmp[4096];
					fd = getfdFromCharaIndex( talker );
					
					if( fd == -1 )  return;
					
					if( NowTime.tv_sec > READTIME3 ){
						saacproto_ACFMReadMemo_send( acfd, FMSDENGON_SN);
						READTIME3 = NowTime.tv_sec+FM_WAITTIME;
					}
					dengonindex = fmsmemo.memoindex;
					if( fmsmemo.memoindex<6 || fmsmemo.memonum>FMSDENGONFILELINENUM ){
						dengonindex = 6; 
					}
					if( dengonindex >= 6 ){
						strcpy( NPC_sendbuf, fmsmemo.memo[dengonindex - 6]);
						strcat( NPC_sendbuf, "\n");
						for( i=(dengonindex - 5); i<=dengonindex; i++){
							strcat( NPC_sendbuf, fmsmemo.memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					if( dengonindex < 6 ){
						if( (fmsmemo.memonum + (dengonindex - 6)) < 0 || (fmsmemo.memonum + (dengonindex - 6)) >= 140 )
							return;
						strcpy( NPC_sendbuf, fmsmemo.memo[fmsmemo.memonum + (dengonindex - 6)]);
						strcat( NPC_sendbuf, "\n");
						for( i=fmsmemo.memonum + (dengonindex - 5); i<fmsmemo.memonum; i++){
							strcat( NPC_sendbuf, fmsmemo.memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						for( i=0; i<=dengonindex; i++){
							strcat( NPC_sendbuf, fmsmemo.memo[i]);
							strcat( NPC_sendbuf, "\n");
						}
						sprintf(tmp, "%d\n", dengonindex);
						strcat( NPC_sendbuf, tmp);
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_FMSDENGON,
						WINDOW_BUTTONTYPE_OKCANCEL|
						WINDOW_BUTTONTYPE_PREV,
						CHAR_WINDOWTYPE_FM_FMSDENGON,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));
				}   
				break;
			default:
				break;
        }
    }
        
    // ��Ա�б�
    else if( seqno == CHAR_WINDOWTYPE_FM_MEMBERLIST)
    {
			char numberlistbuf[4096],tmp_buffer[4096],dutybuf[64];
			int  numberlistindex;
			int  fmindex_wk;
			fmindex_wk = CHAR_getWorkInt( talker, CHAR_WORKFMINDEXI);
			if( fmindex_wk < 0 || fmindex_wk >= FMMAXNUM) return;
			
			getStringFromIndexWithDelim(data,"|",1,tmp_buffer,sizeof(tmp_buffer));
			numberlistindex = atoi(tmp_buffer);
			getStringFromIndexWithDelim(data,"|",2,tmp_buffer,sizeof(tmp_buffer));
			buttonevent = atoi(tmp_buffer);
			getStringFromIndexWithDelim(data,"|",3,dutybuf,sizeof(dutybuf));
			
#ifdef _FMVER21
			//        if( buttonevent>=1 && buttonevent<=11 && 
			//            ( CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER || 
			//              CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_ELDER ||
			//              CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_VICELEADER ))
			if( buttonevent>=1 && buttonevent<=11 && 
				( CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER || 
				CHAR_getInt( talker, CHAR_FMLEADERFLAG ) == FMMEMBER_ELDER ))
#else
        if( buttonevent>=1 && buttonevent<=11 && CHAR_getInt( talker, CHAR_FMLEADERFLAG) == 1 )
#endif        
        {
					int fd,i;
					int int_status;
					char getstatus[4096];
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					strcpy( getstatus, memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]
						+ (strlen( memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]) - 1));
					
					int_status = atoi(getstatus);
					
					// ����ļ��롢�˳��������ѡ��
					if( buttonevent!=11 )
#ifdef _FMVER21            
						strcpy( memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]
						+ (strlen(memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]) - 1), dutybuf);                			    
#else
					switch( int_status ){
					case 1:
					case 3:
						strcpy( memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]
							+ (strlen(memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]) - 1), "4");                      
						break;
					case 2:
						strcpy( memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]
							+ (strlen(memberlist[fmindex_wk].numberlistarray[numberlistindex+buttonevent - 1]) - 1), "1"); 					  
						break;
					default:
						break;
					}              
#endif                 
					// �������ļѡ��
					if( buttonevent == 11 )
					{
						strcpy( getstatus, enlistbuf + (strlen(enlistbuf) - 1));
						int_status = atoi(getstatus);
						
						switch( int_status ){
						case 1:
							memberlist[fmindex_wk].accept = 0;
							sprintf(enlistbuf, "�Ƿ������ļ������Ա|%d|%d",numberlistindex,memberlist[fmindex_wk].accept);
							break;
						case 0:
							memberlist[fmindex_wk].accept = 1;
							sprintf(enlistbuf, "�Ƿ������ļ������Ա|%d|%d",numberlistindex,memberlist[fmindex_wk].accept);
							break;
						default:
							break;    
						}
					}
					
					strcpy( numberlistbuf, memberlist[fmindex_wk].numberlistarray[numberlistindex]);
					strcat( numberlistbuf, "\n");
					for( i=(numberlistindex + 1); i<numberlistindex + 10; i++){
						strcat( numberlistbuf, memberlist[fmindex_wk].numberlistarray[i]);
						strcat( numberlistbuf, "\n");
					}
					sprintf(enlistbuf, "�Ƿ������ļ������Ա|%d|%d",numberlistindex,memberlist[fmindex_wk].accept);
					strcat( numberlistbuf, enlistbuf);
					strcat( numberlistbuf, "\n");
					
					buttontype = WINDOW_BUTTONTYPE_OK;
					if( (numberlistindex + 10) > memberlist[fmindex_wk].fmnum) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;
					else if( numberlistindex == 0 ) 
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else{
						buttontype |= WINDOW_BUTTONTYPE_PREV;
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}
					
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_SELECT,
						buttontype,
						CHAR_WINDOWTYPE_FM_MEMBERLIST,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( numberlistbuf, buf, sizeof(buf)));
        } // end if
        switch( select ){
				case WINDOW_BUTTONTYPE_NEXT:
				case WINDOW_BUTTONTYPE_PREV:
          {
						int fd,i;
						
						fd = getfdFromCharaIndex( talker );
						if( fd == -1 )  return;
						
						numberlistindex += 10 * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
						
						if( numberlistindex >= memberlist[fmindex_wk].fmnum) 
							numberlistindex -= 10;
						else if( numberlistindex < 1 ) 
							numberlistindex = 0;
						
						buttontype = WINDOW_BUTTONTYPE_OK;
						if( (numberlistindex + 10) >= memberlist[fmindex_wk].fmnum) 
							buttontype |= WINDOW_BUTTONTYPE_PREV;
						else if( numberlistindex==0 )
							buttontype |= WINDOW_BUTTONTYPE_NEXT;
						else{
							buttontype |= WINDOW_BUTTONTYPE_PREV;
							buttontype |= WINDOW_BUTTONTYPE_NEXT;
						}
						
						strcpy( numberlistbuf, memberlist[fmindex_wk].numberlistarray[numberlistindex]);
						strcat( numberlistbuf, "\n");
						for( i=(numberlistindex+1); i<numberlistindex+10; i++){
							strcat( numberlistbuf, memberlist[fmindex_wk].numberlistarray[i]);
							strcat( numberlistbuf, "\n");
						}
						sprintf(enlistbuf, "�Ƿ������ļ������Ա|%d|%d",numberlistindex,memberlist[fmindex_wk].accept);
						strcat( numberlistbuf, enlistbuf);
						strcat( numberlistbuf, "\n");
						lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_SELECT,
							buttontype,
							CHAR_WINDOWTYPE_FM_MEMBERLIST,
#ifndef _FM_MODIFY
							CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
							-1,
#endif
							makeEscapeString( numberlistbuf, buf, sizeof(buf)));
          }
          break;
				case WINDOW_BUTTONTYPE_OK:
          break;
				default:
          break;
        }
    }
    
    // ����ǿ�߱�(ǰ��ʮ)
    else if( seqno == CHAR_WINDOWTYPE_FM_DPTOP)
    {
			char listbuf[4096],tmp_buffer[4096];
			int  listindex;
			getStringFromIndexWithDelim(data,"|",1,tmp_buffer,sizeof(tmp_buffer));
			listindex = atoi(tmp_buffer);
			
			switch( select ){
			case WINDOW_BUTTONTYPE_NEXT:
			case WINDOW_BUTTONTYPE_PREV:
				{
					int fd,i;
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					listindex += 10 * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
					
					if( listindex >= 30) 
						return;
					//listindex = 20;
					//listindex -= 10;
					//else if( listindex < 1 ) 
					//    listindex = 0;
					if (listindex < 0) return;
					
					buttontype = WINDOW_BUTTONTYPE_OK;
					if( (listindex + 10) >= 30) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;
					else if( listindex==0 )
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else{
						buttontype |= WINDOW_BUTTONTYPE_PREV;
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}
					
					strcpy( listbuf, fmdptop.topmemo[listindex]);
					strcat( listbuf, "\n");
					for( i=(listindex+1); i<listindex+10; i++){
						strcat( listbuf, fmdptop.topmemo[i]);
						strcat( listbuf, "\n");
					}
					sprintf(tmp_buffer, "%d\n", listindex);
					strcat( listbuf, tmp_buffer);
					
#ifdef _FMVER21              
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_TOP30DP,
#else
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_DP,
#endif              
						buttontype,
						CHAR_WINDOWTYPE_FM_DPTOP,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( listbuf, buf, sizeof(buf)));
				}
				break;
			case WINDOW_BUTTONTYPE_OK:
				break;
			default:
				break;
			}
    }

    // �ݵ��б�
    else if( seqno == CHAR_WINDOWTYPE_FM_POINTLIST)
    {
			char pointbuf[1024];
			int  pointlistindex;
			strcpy( pointbuf, "");
			pointlistindex = 0;
			buttonevent = atoi(data);
			
			switch( select ){
			case WINDOW_BUTTONTYPE_NEXT:
			case WINDOW_BUTTONTYPE_PREV:
				{
					int fd;
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
#ifdef _FIX_FM_FMPOINT
					char x[4];
					char y[4];
					char fmindex[4];
					char name[32];
					int windowtype = WINDOW_MESSAGETYPE_MESSAGE;
					pointlistindex += 5 * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
					
					if( pointlistindex > 5) 
						pointlistindex -= 5;
					else if( pointlistindex < 1 ) 
						pointlistindex = 0;
						
					buttontype = WINDOW_BUTTONTYPE_OK;
					if( CHAR_getInt( talker, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER &&
						          CHAR_getWorkInt(talker, CHAR_WORKFMSETUPFLAG)==1){
						if(select==WINDOW_BUTTONTYPE_NEXT)
							CHAR_setInt( talker, CHAR_LISTPAGE,1);
						else if(select==WINDOW_BUTTONTYPE_PREV)
							CHAR_setInt( talker, CHAR_LISTPAGE,0);
						int tkfmindex = CHAR_getWorkInt(talker, CHAR_WORKFMINDEXI);
						int i,check=TRUE;
						char fmindex[4];
						for(i=0; i<MANORNUM; i++){	// 10��ׯ԰
						  getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, fmindex, sizeof(fmindex));
						  if(tkfmindex==atoi(fmindex)-1 ){
						  	 check=FALSE;
						   	 break;
						  }
						}
					  if(check==TRUE){
							for (i=CHAR_getInt( talker, CHAR_LISTPAGE); i<CHAR_getInt( talker, CHAR_LISTPAGE)+5; i++){	// 10��ׯ԰
							  getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, fmindex, sizeof(fmindex));
						 	  if(atoi(fmindex)<=0 ){
						   		check=TRUE;
						    	break;
						  	}
						  }
						}
					  if(check==TRUE){
					   	windowtype = WINDOW_MESSAGETYPE_SELECT;
							strcpy(pointbuf,"3\n        ��    ������ݵ��б�\n\n");
					  }else{
					  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
					   	strcpy(pointbuf,"        ��    ������ݵ��б�\n\n");
							
						}
					}else{
						strcpy(pointbuf,"        ��    ������ݵ��б�\n\n");
					}
					strcat(pointbuf," ���ء��㡱���������ϡ���״����̬��\n");
					if( (pointlistindex + 5) > 5){
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[5],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[5],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[5],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[5],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
							sprintf(pointbuf,"%s  �� �� ��  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[6],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[6],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[6],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[6],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
							sprintf(pointbuf,"%s  �� �� ˹  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[7],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[7],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[7],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[7],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
							sprintf(pointbuf,"%s  �� �� ��  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[8],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[8],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[8],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[8],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
							sprintf(pointbuf,"%s  �� �� ŷ  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[9],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[9],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[9],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[9],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
							sprintf(pointbuf,"%s  �����˹  %3s   %3s   %s\n", pointbuf, x, y, name);
							buttontype |= WINDOW_BUTTONTYPE_PREV;
					}else if( pointlistindex==0 ){
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[0],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
							sprintf(pointbuf,"%s  ��ķ����  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[1],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
							sprintf(pointbuf,"%s  ������˹  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[2],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
							sprintf(pointbuf,"%s  �ӡ�����  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[3],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
							sprintf(pointbuf,"%s  ��³����  %3s   %3s   %s\n", pointbuf, x, y, name);
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",3, x, sizeof( x));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",4, y, sizeof( y));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",5, fmindex, sizeof( fmindex));
							getStringFromIndexWithDelim( fmpointlist.pointlistarray[4],"|",6, name, sizeof( name));
							if(atoi(fmindex)<=0)strcpy(name,"  δռ��");
							sprintf(pointbuf,"%s  ��������  %3s   %3s   %s\n", pointbuf, x, y, name);
							buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}else{
							buttontype |= WINDOW_BUTTONTYPE_PREV;
							buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}
#else
					int i;
					pointlistindex += 10 * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
					
					if( pointlistindex > FMPOINTNUM) 
						pointlistindex -= 10;
					else if( pointlistindex < 1 ) 
						pointlistindex = 0;
					
					buttontype = WINDOW_BUTTONTYPE_OK;
					if( (pointlistindex + 10) > FMPOINTNUM) 
						buttontype |= WINDOW_BUTTONTYPE_PREV;
					else if( pointlistindex==0 )
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					else{
						buttontype |= WINDOW_BUTTONTYPE_PREV;
						buttontype |= WINDOW_BUTTONTYPE_NEXT;
					}
					
					strcpy( pointbuf, fmpointlist.pointlistarray[pointlistindex]);
					strcat( pointbuf, "\n");
					for( i=(pointlistindex+1); i<pointlistindex+10; i++){
						strcat( pointbuf, fmpointlist.pointlistarray[i]);
						strcat( pointbuf, "\n");
					}
#endif
#ifdef _FIX_FM_FMPOINT
					lssproto_WN_send( fd, windowtype,
#else
					lssproto_WN_send( fd, WINDOW_FMMESSAGETYPE_POINTLIST,
#endif
						buttontype,
						CHAR_WINDOWTYPE_FM_POINTLIST,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
#ifndef _FIX_FM_FMPOINT
					pointbuf);	
#else
					makeEscapeString( pointbuf, buf, sizeof(buf)));
#endif
				}
				break;
			case WINDOW_BUTTONTYPE_OK:
				break;
			default:
#ifdef _FIX_FM_FMPOINT
					ApplyFamilyPoint( index, talker, atoi(data)+CHAR_getInt( talker, CHAR_LISTPAGE)*5);
#endif
				break;
			}
    }        

    // ����ǿ�߱�(�Լ���ǰʮ��)
    else if( seqno == CHAR_WINDOWTYPE_FM_DPME )
    {
			switch( select ){
			case WINDOW_BUTTONTYPE_PREV:
				{
					int  fd;
					
					fd = getfdFromCharaIndex( talker );
					if( fd == -1 )  return;
					
					if( NowTime.tv_sec > READTIME3 ){
						saacproto_ACShowTopFMList_send(acfd, FM_TOP_INTEGRATE);
						saacproto_ACShowTopFMList_send(acfd, FM_TOP_ADV);    
						saacproto_ACShowTopFMList_send(acfd, FM_TOP_FEED);
						saacproto_ACShowTopFMList_send(acfd, FM_TOP_SYNTHESIZE);
						saacproto_ACShowTopFMList_send(acfd, FM_TOP_DEALFOOD);
						saacproto_ACShowTopFMList_send(acfd, FM_TOP_PK);                           
						READTIME3 = NowTime.tv_sec+FM_WAITTIME;
					}
					
					strcpy( NPC_sendbuf, "\n              ��ʮ����������б�\n");
					strcat( NPC_sendbuf, "              ʮ��ð�ռ���\n");
					strcat( NPC_sendbuf, "              ʮ����������\n");
					strcat( NPC_sendbuf, "              ʮ��ϳɼ���\n");
					strcat( NPC_sendbuf, "              ʮ���������\n");
					strcat( NPC_sendbuf, "              ʮ��ս������\n");
					strcat( NPC_sendbuf, "              �Լ����������б�\n");		
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT,
						WINDOW_BUTTONTYPE_NONE,
						CHAR_WINDOWTYPE_FM_DPSELECT,
#ifndef _FM_MODIFY
						CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
#else
						-1,
#endif
						makeEscapeString( NPC_sendbuf, buf, sizeof(buf)));                
				}
				break;
			case WINDOW_BUTTONTYPE_OK:
				break;
			default:
				break;
			}
			
    }  
}

// call FmDengon NPC event
#ifndef _FM_MODIFY
void NPC_FmDengonLooked( int meindex, int lookedindex )
{
    char buf[DENGONFILEENTRYSIZE*MESSAGEINONEWINDOW*2];
    char menubuf[4096];
    int  fd;
    
    if (!CHAR_CHECKINDEX(lookedindex)) return;
    
    fd = getfdFromCharaIndex( lookedindex );
    if( fd == -1 )  return;
    
    // ����վ�ڲ�������ǰ��һ��
    if( NPC_Util_CharDistance( lookedindex, meindex ) > 1) return;
    // �հ״��������
    strcpy( menubuf, "                �����岼������\n\n");
	strcat( menubuf, "                 �����Ա�б�\n");
	strcat( menubuf, "                   ��������\n");
	strcat( menubuf, "                ����֮�����԰�\n");
	strcat( menubuf, "                 �������ݵ�\n");
	strcat( menubuf, "                ����֮��ǿ�߱�");
        
    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_SELECT,
        	     WINDOW_BUTTONTYPE_CANCEL,
                     CHAR_WINDOWTYPE_FM_SELECT,
 		     CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
		     makeEscapeString( menubuf, buf, sizeof(buf)));        
}
#else
void NPC_FmDengonLooked( int meindex, int lookedindex )
{
    char buf[DENGONFILEENTRYSIZE*MESSAGEINONEWINDOW*2];
    char menubuf[4096];
    int  fd;
    
    if (!CHAR_CHECKINDEX(lookedindex)) return;
    
    fd = getfdFromCharaIndex( lookedindex );
    if( fd == -1 )  return;
    
    // �հ״��������
    strcpy( menubuf, "                �����岼������\n\n");
		strcat( menubuf, "                 �����Ա�б�\n");
		strcat( menubuf, "                   ��������\n");
		strcat( menubuf, "                ����֮�����԰�\n");
		strcat( menubuf, "                 �������ݵ�\n");
		strcat( menubuf, "                ����֮��ǿ�߱�");
        
    lssproto_WN_send(fd,
										 WINDOW_MESSAGETYPE_SELECT,
        						 WINDOW_BUTTONTYPE_CANCEL,
                     CHAR_WINDOWTYPE_FM_SELECT,
 										 -1,
										 makeEscapeString( menubuf, buf, sizeof(buf)));        
}
#endif

#ifdef _FIX_FM_FMPOINT
void ApplyFamilyPoint( int meindex, int toindex, int select)
{
		int fd = getfdFromCharaIndex(toindex);
		if (fd == -1) return;
		char fmindex[4];
		char buf[64];
		int tkfmindex = CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI);
		int i,check=0;
		for (i=0; i<=MANORNUM-1; i++) {	// 10��ׯ԰
       getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, fmindex, sizeof(fmindex));
       if (tkfmindex==atoi(fmindex)-1){
       	 return;
       }
    }
    if(memberlist[tkfmindex].fmjoinnum<30)
    	check=1;
    else if(CHAR_getInt( toindex, CHAR_FAME)<300000)
    	check=2;
		getStringFromIndexWithDelim( fmpointlist.pointlistarray[select-1],"|",5, fmindex, sizeof( fmindex));
		if(atoi(fmindex)<=0 && check==0){
			saacproto_ACFixFMPoint_send(acfd,CHAR_getChar(toindex, CHAR_FMNAME),tkfmindex+1,tkfmindex,
																		CHAR_getChar(toindex, CHAR_FMNAME),tkfmindex+1,tkfmindex,select);
			sprintf(buf, "��ϲ�㣡\n    ��ׯ԰�Ѿ�������ˡ�");
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,	WINDOW_BUTTONTYPE_OK,
	    												0,	CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),	buf);
	  }else{
	  	if(check==0)
					sprintf(buf, "��ׯ԰�ݵ�������ռ���ˣ��뵽��ׯ԰���߹ݹ���Ա�������߹�����ׯ԰�ɣ�");
			else if(check==1)
					sprintf(buf, "�ܱ�Ǹ������ׯ԰�ݵ�����˱�������30�ˣ�");
			else if(check==2)
					sprintf(buf, "�ܱ�Ǹ������ׯ԰�ݵ������������3000���ϣ�");
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,	WINDOW_BUTTONTYPE_OK,
	    												0,	CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),	buf);
	  }
}
#endif

