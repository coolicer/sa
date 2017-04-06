#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_windowhealer.h"



#define RATE  1000


/* 
 * ɬ�ý�ľ����ū������ë����NPC
 *
 */

enum {
	CHAR_WORK_LEVEL	= CHAR_NPCWORKINT1,
	CHAR_WORK_RANGE	= CHAR_NPCWORKINT2,
	CHAR_WORK_HP	= CHAR_NPCWORKINT3,
	CHAR_WORK_MP	= CHAR_NPCWORKINT4,

};


static void NPC_WindowHealer_selectWindow( int meindex, int toindex, int num);
void NPC_WindowHealerAllHeal( int talker,int mode );
BOOL NPC_WindowHealerLevelCheck(int meindex,int talker);
BOOL NPC_WindowMoneyCheck(int meindex,int talker,int mode);
int NPC_WindowCostCheck(int meindex,int talker);
BOOL NPC_PetHealerCheck(int talker);
int NPC_WindowCostCheckMp(int meindex,int talker);


/*********************************
*********************************/
BOOL NPC_WindowHealerInit( int meindex )
{

	char *npcarg;
	char buf2[256];
	int range=1;
	int rate;
	double drate;
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEHEALER );

    npcarg = CHAR_getChar(meindex,CHAR_NPCARGUMENT);

	/*--�����������弰����ë������ì��ë��ʢ--*/
	if(getStringFromIndexWithDelim(npcarg,"|",1,buf2,sizeof(buf2))!=FALSE){
		CHAR_setWorkInt(meindex,CHAR_WORK_LEVEL,atoi(buf2));

	}else{
		return FALSE;
	}
	if(getStringFromIndexWithDelim(npcarg, "|", 2, buf2, sizeof( buf2)) != FALSE ){
		drate = atof(buf2);
		if(drate==0) {
			rate=500;
		}else{
			rate=(int) (drate * RATE);
		}
		CHAR_setWorkInt(meindex,CHAR_WORK_HP,rate);
	}
	if(getStringFromIndexWithDelim(npcarg, "|", 3, buf2, sizeof( buf2)) != FALSE ){
		drate = atof(buf2);
		if(drate==0){
			rate=2000;
		}else{
			rate=(int)( drate * RATE);
		}
		CHAR_setWorkInt(meindex,CHAR_WORK_MP,rate);
	}
	if(getStringFromIndexWithDelim(npcarg, "|", 4, buf2, sizeof( buf2)) != FALSE ){
		range=atoi(buf2);
		if(range == 0){
			range=1;
		}
	}
	CHAR_setWorkInt(meindex,CHAR_WORK_RANGE,range);
    return TRUE;

}

void NPC_WindowHealerTalked( int meindex , int talkerindex , char *szMes ,int color )
{
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	if( NPC_Util_CharDistance( talkerindex, meindex ) 
		> CHAR_getWorkInt(meindex,CHAR_WORK_RANGE)){
		return;
	}
	if( (CHAR_getWorkInt(talkerindex,CHAR_WORKPARTYMODE) == 0)
	 || (CHAR_getWorkInt(talkerindex,CHAR_WORKPARTYMODE) == 2) ){
		NPC_WindowHealer_selectWindow( meindex, talkerindex,0);
	}else{
		int i=0;
		int otherindex;

		for( i=0 ; i < CHAR_PARTYMAX ;i++){
			otherindex=CHAR_getWorkInt(talkerindex,CHAR_WORKPARTYINDEX1+i);
			if(otherindex != -1){
				NPC_WindowHealer_selectWindow( meindex, otherindex,0);
			}
		}
	}
}
void NPC_WindowHealerLooked( int meindex , int lookedindex)
{
    if( CHAR_getInt( lookedindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	if( NPC_Util_CharDistance( lookedindex, meindex ) 
		> CHAR_getWorkInt(meindex,CHAR_WORK_RANGE)){
		 return;
	}
	NPC_WindowHealer_selectWindow( meindex, lookedindex,0);
}

static void NPC_WindowHealer_selectWindow( int meindex, int toindex, int num)
{

	char token[1024];
	char escapedname[1024];
	int fd = getfdFromCharaIndex( toindex);
	int buttontype=0;
	int windowtype=0;
	int windowno=0;

	/*--���̼������������������⻥�����м�ƥ�Ʊ�ɬ��--*/
  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;

	switch( num){
	  case 0:
		sprintf(token," ��������ѽ�����������𣿡� \n\n "
				  "������������ �� �;����ظ������������� \n"
					" ��������������  �����ظ� ������������ \n"
					" �������� �� �;����������ظ� ������ \n\n"
				  " ���������� ������ظ�(���)���������� "
			);

	  	buttontype=WINDOW_BUTTONTYPE_CANCEL;
	  	windowtype=WINDOW_MESSAGETYPE_SELECT;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_STARTMSG; 
	  	break;


	  case 1:
		if( CHAR_getInt(toindex,CHAR_HP) ==CHAR_getWorkInt( toindex, CHAR_WORKMAXHP)){
			if(NPC_PetHealerCheck(toindex)==FALSE){
				sprintf(token,
				"������������ �����;����ظ�����"
				"\n\n\n\n���������ƺ�û�б�Ҫ�ظ�ࡣ���");
		  	}else{
		  		sprintf(token,
				"������������ �����;����ظ�����"
				"\n\n��������      �ƺ�û�б�Ҫ�ظ�ࡣ���"
					  "\n\n��������Ϊ�������Ҳ�����ˣ�"
					  "\n                            �Ȱ����ظ��ɣ�");
		  	
		  		NPC_WindowHealerAllHeal(toindex,0 );
		  	}

		  	buttontype=WINDOW_BUTTONTYPE_OK;
		  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG; 
			break;
		}else if(NPC_WindowHealerLevelCheck(meindex,toindex)==TRUE){
			sprintf(token,"������������ �����;����ظ�����"
					"\n\n\n����������������Ҫ�ظ��;���û��ɣ������� "
					"\n\n�����ڵĵȼ�������Ѱ���ظ�ࡣ�");
		
		}else{
			int gold;
			gold=NPC_WindowCostCheck(meindex,toindex);
			sprintf(token,"������������ �����;����ظ�����"
				"\n\n\n����������������Ҫ�ظ��;���û��ɣ������� "
				 "\n\n��������������%d��STONE ��"
				 ,gold);
		}

	  	buttontype=WINDOW_BUTTONTYPE_YESNO;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG; 
	  	break;

	  case 2:
		if( CHAR_getInt(toindex,CHAR_MP) ==CHAR_getWorkInt( toindex, CHAR_WORKMAXMP)){
			if(NPC_PetHealerCheck(toindex)==FALSE){
				sprintf(token,  "������������  ���������ظ�����"
					"\n\n\n\n���������ƺ�û�б�Ҫ�ظ�ࡣ���");
			}else{
			sprintf(token,  "������������  ���������ظ�����"
					"\n\n��������      �ƺ�û�б�Ҫ�ظ�ࡣ���"
					  "\n\n��������Ϊ�������Ҳ�����ˣ�"
					  "\n                            �Ȱ����ظ��ɣ�");
				NPC_WindowHealerAllHeal(toindex,0 );
			}
		  	buttontype=WINDOW_BUTTONTYPE_OK;
		  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG; 
			break;
		}

		if(NPC_WindowHealerLevelCheck(meindex,toindex)==TRUE){
			sprintf(token,"������������  �� �����ظ���"
					  "\n\n����������������Ҫ�ظ�����û��ɣ��������� "
					  "\n\n ���ڵĵȼ�������Ѱ���ظ�ࡣ�");
		}else{
			int cost;
			cost=NPC_WindowCostCheckMp(meindex,toindex);
			sprintf(token,"������������  �� �����ظ���"
					"\n\n\n����������������Ҫ�ظ�����û��ɣ� "
 					  "\n\n������������%d��STONE ��",
					cost);
		}
	  	buttontype=WINDOW_BUTTONTYPE_YESNO;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG; 
	  	break;


	  case 3:
		sprintf(token,	    "\n���������������;����ѻظ���"
						    "\n����  ������Ҳ�ѻظ�һ��״̬��"
						"\n\n\n����������  �����Ӿ�û�����ˣ���������");

	  	NPC_WindowHealerAllHeal( toindex ,1 );
	  	buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG; 
	  	break;


	  case 4:
		sprintf(token,	    "\n�������������������ѻظ���"
						    "\n���� ������Ҳ�ѻظ�һ��״̬��"
						"\n\n\n���������� �����Ӿ�û�����ˣ� ������");
	  	NPC_WindowHealerAllHeal( toindex ,2 );
	 	buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKSPIRITMSG; 
	  	break;


	  case 6:
		{
			int cost=0;

			if(CHAR_getInt(toindex,CHAR_HP) < CHAR_getWorkInt( toindex, CHAR_WORKMAXHP))
			{
				cost+=NPC_WindowCostCheck(meindex,toindex);
			}

			if( CHAR_getInt(toindex,CHAR_MP) < CHAR_getWorkInt( toindex, CHAR_WORKMAXMP))
			{
				cost=cost+NPC_WindowCostCheckMp(meindex,toindex);
			}
		
			if(cost==0) {
				if(NPC_PetHealerCheck(toindex)==FALSE){
				sprintf(token,
						"��������  �����;��������ظ�����"
				"\n\n\n\n���������ƺ�û�б�Ҫ�ظ�ࡣ���");
				}else{
				sprintf(token,"��������  �����;��������ظ�����"
					"\n\n��������      �ƺ�û�б�Ҫ�ظ�ࡣ���"
					  "\n\n��������Ϊ�������Ҳ�����ˣ�"
					  "\n                            �Ȱ����ظ��ɣ�");
				NPC_WindowHealerAllHeal(toindex,0 );
			}
		
				
				
			  	buttontype=WINDOW_BUTTONTYPE_OK;
			  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
			  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG; 
				break;

			}else{
				sprintf(token,"������  �����;��������ظ�����"
						"\n\n\n�������� ��Ҫ�ظ��;�������û��ɣ�"
						"\n\n��������������%d��STONE ��",cost);
			}
		}

		if(NPC_WindowHealerLevelCheck(meindex,toindex)==TRUE){
			sprintf(token,"��������  �����;��������ظ�����"
					"\n\n\n�������� ����Ҫ�ظ��;�������û��ɣ�"
					"\n\n�����ڵĵȼ�������Ѱ���ظ�ࡣ�");
		}
	
	  	buttontype=WINDOW_BUTTONTYPE_YESNO;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_ALLMSG; 
	  	break;


	  case 7:
		sprintf(token,"�������� ���;��������ѻظ���"
				  "\n����  ������Ҳ�ѻظ�һ��״̬��"
				  "\n\n\n������������һ���ͻظ������ˣ�");
	  	
	  	NPC_WindowHealerAllHeal( toindex ,3 );
		buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKALLMSG; 
	  	break;


	  case 8:
		sprintf(token,"\n\n\n\n �������ϧ�ƺ������Ľ�Ǯ����ࡣ�");

		buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG; 
	  	break;

	  case 9:
		sprintf(token,"��������������������ظ���������������"
				"\n\n\n���������������Ѿ�û��������������������"
				  "\n\n��������̫��ǿ�Ļ�Ҳ����ࡣ�����");
		NPC_WindowHealerAllHeal(toindex,0 );
		buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG; 
	  	break;

	  case 10:
		sprintf(token,"��������������������ظ���������������"
				"\n\n\n�� �ƺ�û�б�Ҫ�ظ���������ӡ���"
				  "\n\n��������̫��ǿ�Ļ�Ҳ����ࡣ�����");
		buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG; 
	  	break;


	}


	makeEscapeString( token, escapedname, sizeof(escapedname));
	/*-���ƥ˪������--*/
	lssproto_WN_send( fd, windowtype, 
					buttontype, 
					windowno,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					escapedname);

}


/*-----------------------------------------
-------------------------------------------*/
void NPC_WindowHealerWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{

	if( NPC_Util_CharDistance( talkerindex, meindex ) > (CHAR_getWorkInt(meindex,CHAR_WORK_RANGE)+4)) return;

	switch( seqno){
	  case CHAR_WINDOWTYPE_WINDOWHEALER_STARTMSG:
	  	if(atoi(data)==2)	/*--��  ������   1--*/
	  	{
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 1 );
	  	
	  	}else if(atoi(data)==3){
	  		NPC_WindowHealer_selectWindow( meindex, talkerindex, 2 );
	  	
	  	}else if(atoi(data)==4){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 6 );
		
		}else if(atoi(data)==6){
	  		if(NPC_PetHealerCheck( talkerindex)==TRUE){
		  		NPC_WindowHealer_selectWindow( meindex, talkerindex, 9 );
			}else{
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 10 );
			}
		}else if(select==WINDOW_BUTTONTYPE_CANCEL){
	  	}
	  	break;
	  

	  case CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG:
		if(select==WINDOW_BUTTONTYPE_OK){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		
		}else if(select==WINDOW_BUTTONTYPE_YES){
			/*--���ż���������--*/
			if(NPC_WindowMoneyCheck(meindex,talkerindex,1)==TRUE){
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 3 );
			}else{
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 8 );
			}
		}else if(select==WINDOW_BUTTONTYPE_NO){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );

		}
		break;


	  case CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG:
		if(select==WINDOW_BUTTONTYPE_OK){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		
		}else if(select==WINDOW_BUTTONTYPE_YES){
			/*--���ż���������--*/
			if(NPC_WindowMoneyCheck(meindex,talkerindex,2)==TRUE){
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 4 );
			}else{
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 8 );
			}
		
		}else if(select==WINDOW_BUTTONTYPE_NO){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		}
		
		break;


	  case CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG:
		if(select==WINDOW_BUTTONTYPE_OK){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		}
		break;


	  case CHAR_WINDOWTYPE_WINDOWHEALER_OKSPIRITMSG:
		if(select==WINDOW_BUTTONTYPE_OK){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		}
		break;
	

	  /*--����������--*/
	  case CHAR_WINDOWTYPE_WINDOWHEALER_ALLMSG:
		if(select==WINDOW_BUTTONTYPE_OK){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		
		}else if(select==WINDOW_BUTTONTYPE_YES){
			if(NPC_WindowMoneyCheck(meindex,talkerindex,3)==TRUE){
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 7 );
			}else{
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 8 );
			}
		
		}else if(select==WINDOW_BUTTONTYPE_NO){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		}
		break;
	

	  /*--����⼰����--*/
	  case CHAR_WINDOWTYPE_WINDOWHEALER_OKALLMSG:
		if(select==WINDOW_BUTTONTYPE_YES){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		}
		break;
	}

	if(select==WINDOW_BUTTONTYPE_CANCEL 
	&& CHAR_getWorkInt( talkerindex, CHAR_WORKPARTYMODE ) == 2)
	{
		CHAR_sendWatchEvent( CHAR_getWorkInt( talkerindex, CHAR_WORKOBJINDEX),
								CHAR_ACTPLEASURE,NULL,0,TRUE);
		CHAR_setWorkInt( talkerindex, CHAR_WORKACTION, CHAR_ACTPLEASURE);
	}

}


BOOL NPC_PetHealerCheck(int talker)
{

	int petindex;
	int i;
	
	/*--ʸ������������ƥ�����ƻ�ؤ����--*/
	for(i=0;i<CHAR_MAXPETHAVE;i++){
    	petindex = CHAR_getCharPet(talker,i);

		if( petindex == -1  )  continue;

		if( !CHAR_CHECKINDEX( talker ) )  continue;
		
		
		if( CHAR_getInt(petindex,CHAR_HP) != CHAR_getWorkInt(petindex,CHAR_WORKMAXHP) )
		{
			return TRUE;
		}
	}
	
	return FALSE;

}


/*-------------------------------------
---------------------------------------*/
void NPC_WindowHealerAllHeal( int talker,int mode )
{
	int i;
	int petindex;
	char petsend[64];	
	char msgbuf[64];

	if(mode==1){
    	CHAR_setInt( talker , CHAR_HP ,CHAR_getWorkInt( talker, CHAR_WORKMAXHP ) );
	}else if(mode==2){
    	CHAR_setInt( talker , CHAR_MP ,CHAR_getWorkInt( talker, CHAR_WORKMAXMP ) );
	}else if(mode==3){
    	/*--�����---*/
    	CHAR_setInt( talker , CHAR_HP ,CHAR_getWorkInt( talker, CHAR_WORKMAXHP ) );
    	CHAR_setInt( talker , CHAR_MP ,CHAR_getWorkInt( talker, CHAR_WORKMAXMP ) );
	}

	/*--ʸ������������ƥ�����ƻ�ؤ����--*/
	for(i=0;i<CHAR_MAXPETHAVE;i++){
    	petindex = CHAR_getCharPet(talker,i);

		if( petindex == -1  )  continue;

		if( !CHAR_CHECKINDEX( talker ) )  continue;

		/*--����--*/
		CHAR_setFlg( petindex,CHAR_ISDIE,0 );
		CHAR_setInt( petindex , CHAR_HP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXHP ) );
		CHAR_setInt( petindex , CHAR_MP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXMP ) );

		/*--�ɷ¶�����Ʃ��--*/
		CHAR_complianceParameter( petindex );
		sprintf( petsend, "K%d", i );
		CHAR_sendStatusString( talker , petsend );

	}

	/*--�ɷ¶�����˪Ի--*/
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_HP);
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_MP);


	/*---���޻���ľ�����ޱ���˪��--*/
	if(CHAR_getWorkInt( talker, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE )
	{
		int	topartyarray = -1;
		int	oyaindex = CHAR_getWorkInt( talker	, CHAR_WORKPARTYINDEX1);

		if( CHAR_CHECKINDEX( oyaindex )) {
			int		i;
		
			for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
				int workindex = CHAR_getWorkInt( oyaindex, CHAR_WORKPARTYINDEX1 +i);
		
				if( CHAR_CHECKINDEX( workindex) ) {
					if( workindex == talker ) {
						topartyarray = i;
						break;
					}
				}
			}
		
			for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
				int otherindex = CHAR_getPartyIndex( talker, i);
		
				/* �����ɷ¶�����ë˪�� */
				if( CHAR_CHECKINDEX( otherindex) ) {
					snprintf( msgbuf, sizeof( msgbuf), "N%d", topartyarray);
					if( otherindex != talker) {
						CHAR_sendStatusString( otherindex, msgbuf);
					}
				}
			}
		}
	}

}


/*-------------------------------------
��ì����������
	ɬ����ì�ﶯ��ئ��	FALSE
---------------------------------------*/
BOOL NPC_WindowHealerLevelCheck(int meindex,int talker)
{
		int level;
	
		level=CHAR_getWorkInt(meindex,CHAR_WORK_LEVEL);

		if(level > CHAR_getInt(talker,CHAR_LV)){
			return TRUE;
		}

		return FALSE;

}


/*-------------------------------------------
¦��
talker��������ƽ�ҷ����������̼������͵�
---------------------------------------------*/
BOOL NPC_WindowMoneyCheck(int meindex,int talker,int mode)
{
	int cost=0;
	int level;
	
	level=CHAR_getWorkInt(meindex,CHAR_WORK_LEVEL);
	
	if(mode==1){
		if(level <= CHAR_getInt(talker,CHAR_LV)){
			/*---����ë�Ի����---*/
			cost=NPC_WindowCostCheck(meindex,talker);
			/*--�ػ�����ɬ��---*/
			/*---���Ż���Ի�¾�����������������---*/
			if(CHAR_getInt(talker,CHAR_GOLD) < cost){
				return FALSE;
			
			}
			CHAR_DelGold( talker, cost);
		}
	}
	if(mode==2){
		if(level <= CHAR_getInt(talker,CHAR_LV)){
			/*---����ë�Ի����---*/
			cost=NPC_WindowCostCheckMp(meindex,talker);
	
			/*--�ػ�����ɬ��---*/
			/*---���Ż���Ի�¾�����������������---*/
			if(CHAR_getInt(talker,CHAR_GOLD) < cost){
				return FALSE;
			}
			CHAR_DelGold( talker, cost);
		}
	}

	if(mode==3){
		if(level <= CHAR_getInt(talker,CHAR_LV)){

			if( CHAR_getInt(talker,CHAR_HP) <CHAR_getWorkInt( talker, CHAR_WORKMAXHP))
			{
				cost=NPC_WindowCostCheck(meindex,talker);
			}

			if( CHAR_getInt(talker,CHAR_MP) < CHAR_getWorkInt( talker, CHAR_WORKMAXMP))
			{
				cost=cost+NPC_WindowCostCheckMp(meindex,talker);
			}
		
			/*--��ɬ��--*/
			if(CHAR_getInt(talker,CHAR_GOLD) < cost){
				return FALSE;
			}
		
			/*---����ëӼ����---*/
			CHAR_DelGold( talker, cost);
		}
		
	}
	
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_GOLD);

	return TRUE;

}



/*-----------------------------------------
�쵩������ң
------------------------------------------*/
int NPC_WindowCostCheck(int meindex,int talker)
{
	int cost;
	double drate;
	int rate;
	
	rate=CHAR_getWorkInt(meindex,CHAR_WORK_HP);
	drate=(double) rate / 1000 ;
	cost=CHAR_getInt(talker,CHAR_LV);
	cost=(int)cost*drate;
	if(cost < 1) cost=1;
	return cost;
}

/*-----------------------------------------
------------------------------------------*/
int NPC_WindowCostCheckMp(int meindex,int talker)
{
	int cost;
	double drate;
	int rate;
	
	rate=CHAR_getWorkInt(meindex,CHAR_WORK_MP);
	drate=(double) rate / 1000 ;
	cost=CHAR_getInt(talker,CHAR_LV);
	cost=(int)cost*drate;
	if(cost==0) cost=1;
	return cost;
}
