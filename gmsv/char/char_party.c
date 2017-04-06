#include "version.h"
#include <stdio.h>

#include "readmap.h"
#include "object.h"
#include "char.h"
#include "char_base.h"
#include "battle.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "npc_bus.h"
#include "npc_airplane.h"       // Arminius 7.10 Airplane
#include "family.h"             // shan

#ifdef _ITEM_QUITPARTY
#include "init.h"
#endif
// shan add 
extern struct FM_PKFLOOR fmpkflnum[FAMILY_FMPKFLOOR];

/*------------------------------------------------------------
 * �ɡ�  ū��  ��ĩ����
 ------------------------------------------------------------*/

/*------------------------------------------------------------
 * ���л������ɡ�  ū  ë����
 * ئ��ľ��-1ë߯�ʣ�
 ------------------------------------------------------------*/
int CHAR_getEmptyPartyArray( int charaindex)
{
	int     i = -1;
	int     rc = FALSE;
	int		toindex;
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ) {
		toindex = charaindex;
	}
	else {
		toindex = CHAR_getPartyIndex( charaindex, 0);
	}
	if( CHAR_CHECKINDEX( toindex)){
		for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
			if( CHAR_getWorkInt( toindex, i + CHAR_WORKPARTYINDEX1) == -1 ) {
				rc = TRUE;
				break;
			}
		}
	}
	return( rc ? i: -1);
}
/*------------------------------------------------------------
 *   �˱��ɡ�  ū��  ����  
 *
 *  charaindex		int		����
 *  targetindex		int		  �����켰��
 ------------------------------------------------------------*/
void CHAR_JoinParty_Main( int charaindex, int targetindex)
{
	int		firstflg = FALSE;
	int		i;
	char	c[3];
	char buf[64];
	int		toindex;
	int		parray;

	/* �ʻ�������¦�Ƚ�Ի���� */
	if( CHAR_getWorkInt( targetindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ) {
		toindex = targetindex;
	}
	else {
		toindex = CHAR_getPartyIndex( targetindex, 0);
		if( !CHAR_CHECKINDEX( toindex) ) {
			print( " %s:%d err\n", __FILE__, __LINE__);
			return;
		}
	}


	/* �����ɡ�  ū�����ѷ�    ���� */
	parray = CHAR_getEmptyPartyArray( toindex) ;
	if( parray == -1 ) {
		print( "%s : %d err\n", __FILE__,__LINE__);
		return;
	}
	/* ����  ��  �ʼ��ݷ��ʱ�ئ����CAë˪������ */
	if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ) {
		CHAR_sendLeader( CHAR_getWorkInt( toindex, CHAR_WORKOBJINDEX), 1);
		/* ���켰���ؼ�̤�徧�� */
		/* �ʱ�ئ�� */
		CHAR_setWorkInt( toindex, CHAR_WORKPARTYMODE, 1);
		CHAR_setWorkInt( toindex, CHAR_WORKPARTYINDEX1, toindex);
		firstflg = TRUE;
	}
	CHAR_setWorkInt( toindex, parray + CHAR_WORKPARTYINDEX1, charaindex);

	CHAR_setWorkChar( charaindex, CHAR_WORKWALKARRAY, "");

	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYMODE, CHAR_PARTY_CLIENT);

	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYINDEX1, toindex);

	if( firstflg ) {
		CHAR_sendStatusString( toindex, "N0");
	}


	for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
		int index = CHAR_getWorkInt( toindex, i + CHAR_WORKPARTYINDEX1);
		if( CHAR_CHECKINDEX(index)) {
				snprintf( c, sizeof(c), "N%d", i);
				CHAR_sendStatusString( charaindex, c);
		}
	}

	snprintf( buf,sizeof( buf), "%s �����Ŷӣ�",
			  CHAR_getChar( charaindex, CHAR_NAME));

	for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
		int index = CHAR_getWorkInt( toindex, i + CHAR_WORKPARTYINDEX1);
		if( CHAR_CHECKINDEX(index)) {
			if( index != charaindex ) {
				snprintf( c, sizeof(c), "N%d", parray);
				CHAR_sendStatusString( index, c);
				CHAR_talkToCli( index, -1, buf, CHAR_COLORYELLOW);
			}
			else {
				CHAR_talkToCli( index, -1, "�����Ŷӣ�", CHAR_COLORYELLOW);
			}
		}
	}
}
/*------------------------------------------------------------
 * �ɡ�  ū��  Ƿ�������£�
 ------------------------------------------------------------*/
BOOL CHAR_JoinParty( int charaindex )
{

	int     result = -1;
	int     x,y;
	OBJECT  object;
	int     found = FALSE;
	int     fd;
	int		cnt;
	int	i;

	fd = getfdFromCharaIndex( charaindex );
	if( fd == -1 ) {
		print( "%s : %d err\n", __FILE__, __LINE__);
		return FALSE;
	}

        /* ���л��ɡ�  ū׬��ƥ������   */
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE ) {
		lssproto_PR_send( fd, 1, FALSE);
		return FALSE;
	}

	/*   ��󡼰��  ë  �� */
	CHAR_getCoordinationDir( CHAR_getInt( charaindex, CHAR_DIR ) ,
							 CHAR_getInt( charaindex , CHAR_X ),
							 CHAR_getInt( charaindex , CHAR_Y ) ,
							 1 , &x , &y );

	/* ��������� */
	for( i = 0; i < CONNECT_WINDOWBUFSIZE; i ++ ) {
        CONNECT_setJoinpartycharaindex(fd,i,-1);
    }
	cnt = 0;

	/*���м�  ��󡼰ƽ�ҷ�ë��  ���� */

	for( object = MAP_getTopObj( CHAR_getInt( charaindex, CHAR_FLOOR),x,y) ;
		 object ;
		 object = NEXT_OBJECT(object ) )
	{
		int toindex;
		int parray;
		int objindex = GET_OBJINDEX(object);
		int targetindex = -1;

		/* ƽ�ҷ�������Ԫ��ئ�� */
		if( OBJECT_getType( objindex) != OBJTYPE_CHARA) continue;
		toindex = OBJECT_getIndex( objindex);
	
                // shan begin
                if( CHAR_getInt(charaindex, CHAR_FMINDEX) > 0 && CHAR_getInt(toindex, CHAR_FMINDEX) >0){
                    for( i = 0; i < FAMILY_FMPKFLOOR; i++){
                        if( fmpkflnum[i].fl == CHAR_getInt( charaindex, CHAR_FLOOR) )
                            if( CHAR_getInt(charaindex, CHAR_FMINDEX) != CHAR_getInt(toindex, CHAR_FMINDEX) ){
                                lssproto_PR_send( fd, 1, FALSE);
                                return FALSE;
                            }
                    }
                }
                // shan end
	
		/* �������������� */
		if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ){
			found = TRUE;
			/* ���컥�ҷ���������ë¦�Ƚ�Ի���� */
			if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ) {
				targetindex = CHAR_getWorkInt( toindex, CHAR_WORKPARTYINDEX1);
				if( !CHAR_CHECKINDEX( targetindex) ) {
					print( " %s:%d err\n", __FILE__, __LINE__);
					continue;
				}
				if( CHAR_getInt( targetindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
					continue;
				}
			}
			else {
				targetindex = toindex;
			}

			/*   ����  ���ڶ�  �����¾� */
			if( NPC_Util_CharDistance( charaindex, targetindex ) > 1) {
				continue;
			}

			/* ��    ��ƥئ���ǣ�*/
			if( CHAR_getWorkInt( targetindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE ){
				continue;
			}
			/* ���޸���ƹ���� */
			if( !CHAR_getFlg( targetindex, CHAR_ISPARTY) ) continue;

		}
		/* Ѩ��ƹ���ﵩ�������ݷ������޷�Ի������£� */
		else if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS ) {
			targetindex = toindex;
			cnt = 0;
			if( !NPC_BusCheckJoinParty( toindex, charaindex, TRUE)) {
				/* ����ë  �׽�ئ�����ף�����  �¼����δ��£����޼���  ����ئ�У�
				 * ֧֧�����м�ƥ��
				 */
				break;
			}
			{	// Arminius 7.10 Airplane
			  int busimg=CHAR_getInt(toindex, CHAR_BASEIMAGENUMBER);
		          if ((busimg!=100355) && (busimg!=100461)) {
		            CHAR_setInt(charaindex,CHAR_BASEIMAGENUMBER,busimg);
			    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex ,
				CHAR_WORKOBJINDEX ));
				// Robin debug 01/11/21
				if( CHAR_getInt( charaindex, CHAR_RIDEPET) != -1 ) {
					CHAR_setInt( charaindex, CHAR_RIDEPET, -1);
					CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_RIDEPET);
				}
			/*
			    CHAR_sendPMEToArroundCharacterFLXY(charaindex,
			        CHAR_getInt( charaindex, CHAR_FLOOR),
			        CHAR_getInt( charaindex, CHAR_X),
			        CHAR_getInt( charaindex, CHAR_Y),
			        0,1,CHAR_getInt( charaindex, CHAR_PETMAILEFFECT)
			        );
			*/
		          }
		        }
		}
		/* �����������跴Ѩ��ƹ���ﵩ��½��  ������ */
		else {
			continue;
		}
		/* �����ɡ�  ū�����ѷ�    ���� */
		parray = CHAR_getEmptyPartyArray( targetindex) ;
		if( parray == -1 ) continue;

		/* �����ƥ��ľ��     */
        CONNECT_setJoinpartycharaindex( fd,cnt,toindex);
		cnt++;
		if( cnt == CONNECT_WINDOWBUFSIZE ) break;
		
		/* Ѩ��ƹ���ﵩ  έ�Ʒ��Уݻ����ë  ���£� */
		if( CHAR_getInt( targetindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS ) break;

	}

	if( cnt == 0 ) {
		if( found == TRUE) {
			CHAR_talkToCli( charaindex, -1, "�޷������Ŷӡ�", CHAR_COLORYELLOW);
		}
		result = FALSE;
	}else if( cnt == 1 ) {
#ifdef _AUTO_PK
			if(CHAR_getInt(charaindex,CHAR_FLOOR)==20000){
				CHAR_talkToCli( charaindex, -1, "��Pϵͳ��ֹ���!", CHAR_COLORYELLOW);
				result = FALSE;
			}else
#endif
			{
				CHAR_JoinParty_Main( charaindex, CONNECT_getJoinpartycharaindex(fd,0));
				result = TRUE;
			}
	}else {
		int		strlength;
		char	msgbuf[1024];
		char	escapebuf[2048];
		strcpy( msgbuf, "1\n��˭����Ŷ��أ�\n");
		strlength = strlen( msgbuf);
		for( i = 0;
             CONNECT_getJoinpartycharaindex( fd,i ) != -1
			&& i< CONNECT_WINDOWBUFSIZE;
			i ++ ){
			char	*a = CHAR_getChar(
                CONNECT_getJoinpartycharaindex(fd,i) , CHAR_NAME);
			char	buf[256];
			snprintf( buf, sizeof( buf),"%s\n", a);
			if( strlength + strlen( buf) > arraysizeof( msgbuf)){
				print( "%s:%d�Ӵ�ѶϢbuffer���㡣\n",
						__FILE__,__LINE__);
				break;
			}
			strcpy( &msgbuf[strlength], buf);
			strlength += strlen(buf);
		}
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT,
						WINDOW_BUTTONTYPE_CANCEL,
						CHAR_WINDOWTYPE_SELECTPARTY,
						-1,
					makeEscapeString( msgbuf, escapebuf, sizeof(escapebuf)));


	}

	if( result != -1 ) {
		lssproto_PR_send( fd, 1, result);
	}

	return result;
}

static BOOL CHAR_DischargePartySub( int charaindex, int msgflg)
{
	char buf[64], c[3];
	int toindex,flg,i;
#ifdef _ITEM_QUITPARTY
    int j = 0,k;
#endif
	

	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ) {
		int pindex, airplaneflag=0;
		// Arminius 7.10 Airplane
		if( CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS ) {
		  if ((CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) !=100355) &&
		      (CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) !=100461)){
		    airplaneflag=1;
		  }
		}
		for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
			pindex = CHAR_getWorkInt( charaindex, i + CHAR_WORKPARTYINDEX1);
			if( CHAR_CHECKINDEX( pindex) ) {
				int     fd = getfdFromCharaIndex( pindex );
				CHAR_setWorkInt( pindex, CHAR_WORKPARTYINDEX1, -1);
				CHAR_setWorkInt( pindex, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
				if( msgflg ){
					CHAR_talkToCli( pindex, -1, "�Ŷ��ѽ�ɢ��", CHAR_COLORYELLOW);
#ifdef _ITEM_QUITPARTY
					// won fix
	                for( j=0;j<CHAR_MAXITEMHAVE;j++ ){
						int del_item_index = CHAR_getItemIndex( pindex , j );
                        if( ITEM_CHECKINDEX(del_item_index) ){ //�������е���
                            for( k=0;k<itemquitparty_num;k++ ){
							    if( ITEM_getInt( del_item_index, ITEM_ID) == atoi(Disappear_Item[k].string) ){ //��������趨�ĵ���ID
			                        CHAR_setItemIndex( pindex, j, -1); //�����ڵ�����ʧ
									ITEM_endExistItemsOne( del_item_index );
				                    CHAR_sendItemDataOne( pindex, j);
								}
							}
						}
					}
#endif
				}
				if( fd != -1 ) {
					lssproto_PR_send( fd, 0, 1);
				}
				// Arminius 7.10 Airplane
				if (airplaneflag && (CHAR_getInt(pindex,CHAR_WHICHTYPE)!=CHAR_TYPEBUS)) {
				  int bi,bbi,ii,category;
				  bbi=CHAR_getInt(pindex,CHAR_BASEBASEIMAGENUMBER);
				  ii=CHAR_getItemIndex(pindex,CHAR_ARM);
				  if (!ITEM_CHECKINDEX(ii))
				    category=ITEM_FIST;
				  else
				    category=ITEM_getInt(ii,ITEM_TYPE);
				  bi=CHAR_getNewImagenumberFromEquip(bbi,category);
				  if (bi==-1) bi=bbi;
				  CHAR_setInt(pindex,CHAR_BASEIMAGENUMBER,bi);
				  // Robin 0810 debug
				  CHAR_complianceParameter( pindex );				  
				  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(pindex ,
				    CHAR_WORKOBJINDEX));
				}
			}
			CHAR_setWorkInt( charaindex, i + CHAR_WORKPARTYINDEX1, -1);
		}
		CHAR_sendLeader( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX), 0);
	}else if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ) {
		int		myarray = -1;
		int     fd = getfdFromCharaIndex( charaindex );
		CHAR_setWorkInt( charaindex, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
		toindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1);
		if( !CHAR_CHECKINDEX(toindex ) ) return FALSE;
		if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS ) {
			NPC_BusCheckAllowItem( toindex, charaindex, TRUE);
		  // Arminius 7.9 Airplane
		  if ((CHAR_getInt( toindex, CHAR_BASEIMAGENUMBER) !=100355) &&
		      (CHAR_getInt( toindex, CHAR_BASEIMAGENUMBER) !=100461)){
		    int bi,bbi,ii,category;
		    
		    bbi=CHAR_getInt(charaindex,CHAR_BASEBASEIMAGENUMBER);
		    ii=CHAR_getItemIndex(charaindex,CHAR_ARM);
		    if (!ITEM_CHECKINDEX(ii))
		      category=ITEM_FIST;
		    else
		      category=ITEM_getInt(ii,ITEM_TYPE);
		    bi=CHAR_getNewImagenumberFromEquip(bbi,category);
		    if (bi==-1) bi=bbi;
		    CHAR_setInt(charaindex,CHAR_BASEIMAGENUMBER,bi);

		    // Robin 0810 debug
		    CHAR_complianceParameter( charaindex );		    

		    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
		    if(CHAR_getWorkInt(toindex,CHAR_NPCWORKINT5)==1) {
		      if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER)>=0){
		        int fl,x,y;
		        CHAR_getElderPosition( CHAR_getInt( charaindex, CHAR_LASTTALKELDER),
		          &fl, &x, &y );
		        CHAR_warpToSpecificPoint(charaindex, fl, x, y);
		      }
		    }
		  }
		}
		CHAR_setWorkInt( charaindex, CHAR_WORKPARTYINDEX1, -1);
		for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
			int index = CHAR_getWorkInt( toindex, i + CHAR_WORKPARTYINDEX1);
			if( CHAR_CHECKINDEX(index) ){
				if( index == charaindex) {
					myarray = i;
					break;
				}
			}
		}
		if( myarray == CHAR_PARTYMAX) {
			print( "DischargeParty(): ����֣�");
			return FALSE;
		}
		CHAR_setWorkInt( toindex, CHAR_WORKPARTYINDEX1 + myarray, -1);
		snprintf( buf,sizeof( buf), "%s �����Ŷӣ�",
				  CHAR_getChar( charaindex, CHAR_NAME));
		if( msgflg ){
			CHAR_talkToCli( charaindex, -1, "�����Ŷӣ�", CHAR_COLORYELLOW);
#ifdef _ITEM_QUITPARTY
			// won fix
	        for( i=0;i<CHAR_MAXITEMHAVE;i++ ){
				int del_item_index = CHAR_getItemIndex( charaindex , j );
				if( ITEM_CHECKINDEX(del_item_index) ){ //�������е���
                   for( j=0;j<itemquitparty_num;j++ ){
					    if( ITEM_getInt( del_item_index, ITEM_ID) == atoi(Disappear_Item[j].string) ){ //��������趨�ĵ���ID
			                CHAR_setItemIndex( charaindex, i, -1); //�����ڵ�����ʧ
							ITEM_endExistItemsOne( del_item_index );
			                CHAR_sendItemDataOne( charaindex, i);
						}
					}
				}
			}
#endif
		}
		snprintf( c, sizeof(c), "N%d", myarray);
		if( fd != -1 ) {
			lssproto_PR_send( fd, 0, 1);
		}
		for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
			int index = CHAR_getWorkInt( toindex, i + CHAR_WORKPARTYINDEX1);
			if( CHAR_CHECKINDEX(index) ){
#ifdef _ITEM_QUITPARTY
				// won fix
	            for( j=0;j<CHAR_MAXITEMHAVE;j++ ){
					int del_item_index = CHAR_getItemIndex( index , j );	
                    if( ITEM_CHECKINDEX(del_item_index) ){ //�������е���
                        for( k=0;k<itemquitparty_num;k++ ){
						    if( ITEM_getInt( del_item_index, ITEM_ID) == atoi(Disappear_Item[k].string) ){ //��������趨�ĵ���ID
			                    CHAR_setItemIndex( index, j, -1); //�����ڵ�����ʧ
								ITEM_endExistItemsOne( del_item_index );
						        CHAR_sendItemDataOne( index, j);
							}
						}
					}
				}
#endif
				if( msgflg ){
					CHAR_talkToCli( index, -1, buf, CHAR_COLORYELLOW);
				}
				CHAR_sendStatusString( index, c);
			}
		}
		flg = FALSE;
		for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
			int index = CHAR_getWorkInt( toindex, i + CHAR_WORKPARTYINDEX1);
			if( CHAR_CHECKINDEX(index) ){
				flg = TRUE;
				break;
			}
		}
		if( !flg) {
			CHAR_setWorkInt( toindex, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
			CHAR_sendLeader( CHAR_getWorkInt( toindex, CHAR_WORKOBJINDEX), 0);
		}else {

			POINT	start,end;
			int 	previndex = toindex;
			end.x = CHAR_getInt( charaindex, CHAR_X);
			end.y = CHAR_getInt( charaindex, CHAR_Y);
			for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
				int index = CHAR_getWorkInt( toindex, i + CHAR_WORKPARTYINDEX1);
				if( CHAR_CHECKINDEX( index) ) {
					if( NPC_Util_CharDistance( index, previndex) > 1) {
						int		parent_dir;
						start.x = CHAR_getInt( index, CHAR_X);
						start.y = CHAR_getInt( index, CHAR_Y);
						parent_dir = NPC_Util_getDirFromTwoPoint( &start,&end );
						end = start;
						if( parent_dir != -1 ) {
							CHAR_walk( index, parent_dir, 0);
						}
					}
					previndex = index;
				}
			}
		}
		
	}

	return TRUE;
}
BOOL CHAR_DischargeParty( int charaindex, int flg)
{
	return CHAR_DischargePartySub( charaindex, 1);
}

BOOL CHAR_DischargePartyNoMsg( int charaindex)
{
	return CHAR_DischargePartySub( charaindex, 0);
}


/*------------------------------------------------------------
 * ���л�����ĸ����������ë˪�����£�
 ------------------------------------------------------------*/
void CHAR_sendLeader( int objindex, int leader)
{
	int		opt[1];
	opt[0] = leader;
	CHAR_sendWatchEvent( objindex,CHAR_ACTLEADER,opt,1,TRUE);
}
/*------------------------------------------------------------
 * ����ë��    CHAR_WORKPARTYINDEX)϶��ƥƽ�ҷ�indexë¦�Ƚ��£�
 * ���л���ƥ�ֺ�ƥ��    ��
 ------------------------------------------------------------*/
int CHAR_getPartyIndex( int index, int num)
{
	int	nindex = -1;

	/* ���޼��̼������͵�ë��   */
	/* �ʼ����� */
	if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ) {
		nindex = CHAR_getWorkInt( index, CHAR_WORKPARTYINDEX1 + num );
	}
	/* �Ҽ����� */
	else {
		int oyaindex = CHAR_getWorkInt( index, CHAR_WORKPARTYINDEX1);
		if( CHAR_CHECKINDEX( oyaindex)) {
			nindex = CHAR_getWorkInt( oyaindex, CHAR_WORKPARTYINDEX1+num);
		}
	}
	return nindex;
}
/*------------------------------------------------------------
 * ����������ë˪�����£�
 * ���޻���ľ�ɹ������ޱ��ֶ���������ë˪�����£�
 ------------------------------------------------------------*/
void CHAR_talkToCliAndParty( int talkedcharaindex,int talkcharaindex,
					 char* message, CHAR_COLOR color )
{
	int		i;
	/* ���ڷ��� */
	CHAR_talkToCli( talkedcharaindex, talkcharaindex, message, color);

	for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
		int index = CHAR_getPartyIndex( talkedcharaindex, i);
		if( CHAR_CHECKINDEX( index) &&
			index != talkedcharaindex)
		{
			CHAR_talkToCli( index, talkcharaindex, message, color);
		}
	}
}

