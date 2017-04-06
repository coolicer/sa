#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "pet_skill.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
/* ���������⼰enum */
enum {
	NPC_PETSHOP_MSG_OVER,
	NPC_PETSHOP_MSG_MAIN,
	NPC_PETSHOP_MSG_REALY,
	NPC_PETSHOP_MSG_THANKS,
	NPC_PETSHOP_MSG_COST,
	NPC_PETSHOP_MSG_POOLTHANKS,
	NPC_PETSHOP_MSG_POOLFULL,
	NPC_PETSHOP_MSG_GETFULL,
};
typedef struct {
	char	option[32];
	char	defaultmsg[128];
}NPC_PETSHOP_MSG;

NPC_PETSHOP_MSG		shopmsg[] = {
	{ "over_msg",		"Ǯ������!û������?"},
	{ "main_msg",		"��ӭ"},
	{ "realy_msg",		"��Ŀ�����?"},
	{ "thanks_msg",		"лл!"},
	{ "cost_msg",		"Ǯ�����!"},
	{ "pooltanks_msg",	"лл!"},
	{ "poolfull_msg",	"�������޷������"},
	{ "getfull_msg",	"�㲻���кܶ������"}
	
};

#define PETCOST	10

void NPC_PetDate(int meindex,int toindex,int select,char *token);
void NPC_PetDate2(int meindex,int talker,int select,char *token);
void NPC_PetDate3(int meindex,int talker,int select,char *token);
void NPC_PetDel2( int meindex, int talker, int select, char *token);
void NPC_PetDel3( int meindex, int talker, int select, char *token);
void NPC_getDrawPet( int meindex, int toindex, char *token, int *buttontype, int select);

static void NPC_PetShop_selectWindow( int meindex, int toindex, int num,int select);
int NPC_GetCost(int meindex,int talker,int petindex);
void NPC_PetDel(int meindex, int toindex,int select,char* token);
void NPC_MaxGoldOver(int meindex,int toindex,int select,char *token);

void NPC_DepotPet_CheckRepeat_del( int charaindex, int petindex);

//   ��  ľ�ź���ңֿ
#define		NPC_GETPOOLCOST( talker)		( 50 + CHAR_getInt( talker, CHAR_LV)*4)

//   ��  ľ�ŷ����ɻ���
#define		NPC_PETSHOP_POOLCOST_DEFAULT	200

/*--������    ������--*/
enum {
	CHAR_WORK_NOMALRATE	= CHAR_NPCWORKINT1,
	CHAR_WORK_SPECIALRATE	= CHAR_NPCWORKINT2,
	CHAR_WORK_POOLCOST	= CHAR_NPCWORKINT3,
	CHAR_WORK_POOLFLG	= CHAR_NPCWORKINT4,
};


/*********************************
* ������  
*********************************/
BOOL NPC_PetShopInit( int meindex )
{

	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
	char buf[256];
	double rate;
	int intrate;
//	int cost;
	int tmp;

	/*--������ɬ��--*/
   	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPESTONESHOP );


	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg)) == NULL){
		print("GetArgStrErr");
		return FALSE;
	}

	/*--������ë炙��巤ʢ����������*/
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "nomal_rate", buf, sizeof( buf)) != NULL){
		rate = atof( buf);		
		intrate = ( int)( rate *1000);		
   		CHAR_setWorkInt( meindex , CHAR_WORK_NOMALRATE ,intrate );
	}else{
		CHAR_setWorkInt( meindex , CHAR_WORK_NOMALRATE ,1000 );
	}

		
	/*--��ʸ���һ��������ַ�ʢ����������--*/
	if(NPC_Util_GetStrFromStrWithDelim( npcarg, "special_rate", buf, sizeof( buf))!=NULL)
	{
		rate = atof( buf);		
		intrate = ( int)( rate *1000);		
   		CHAR_setWorkInt( meindex , CHAR_WORK_SPECIALRATE ,intrate );
	}else{
		CHAR_setWorkInt( meindex , CHAR_WORK_SPECIALRATE ,1200 );
	}
	
	//   ��  ľ�ŷ�ʢ
	//cost = NPC_Util_GetNumFromStrWithDelim( npcarg, "pool_cost");
	//if( cost == -1 ) cost = NPC_PETSHOP_POOLCOST_DEFAULT;
	//CHAR_setWorkInt( meindex, CHAR_WORK_POOLCOST, cost);
	
	
	//   ��  ľ����  �¾�������ë��ʢ
	tmp = NPC_Util_GetNumFromStrWithDelim( npcarg, "pool_flg");
	if( tmp != 1 ) tmp = 0;
	CHAR_setWorkInt( meindex, CHAR_WORK_POOLFLG, tmp);
    
    return TRUE;

}




/*********************************
*   �ƾ�����ľ���ݼ���  
*********************************/
void NPC_PetShopTalked( int meindex , int talkerindex , char *szMes ,int color )
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
	CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANT,0);
	/*-��Ԫ������      --*/
	NPC_PetShop_selectWindow( meindex, talkerindex,0,-1);
}

/*********************************
* έ��ľ���ݼ���  
*********************************/
void NPC_PetShopLooked( int meindex , int lookedindex)
{
    /* �����������帲�ƻ�����  ɱ���� */
    if( CHAR_getInt( lookedindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	/* ���������  ���� */
	if( NPC_Util_CharDistance( lookedindex, meindex ) > 2) return;

	/*--�����ͼ������--*/
//	CHAR_setWorkInt(lookedindex,CHAR_WORKSHOPRELEVANT,0);


//	NPC_PetShop_selectWindow( meindex, lookedindex,0,-1);

}

static void NPC_PetShop_selectWindow( int meindex, int toindex, int num,int select)
{
	char token[1024];
	int buttontype=0,windowtype=0,windowno=0;
	char buf[256];
	int fd = getfdFromCharaIndex( toindex);
	
	//print("\n NPC_PetShop_selectWindow num:%d select:%d  ", num, select);
	if( fd == -1 ) {
		fprint( "getfd err\n");
		return;
	}
	token[0] = '\0';
	switch(num){
      //   �ټ���ū������
	  case 0:
		NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_MAIN, buf);
		
		if( !CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG)) {
			sprintf(token, "4\n������������������̵��"
							"%s"
							"\n"
							"\n������������ �� ������ ��"
							"\n"
							"\n             �� �뿪 ��",
							buf);
		}else {
			sprintf(token, "4\n������������������̵��"
							"%s"
							"\n            �� �ķų��� ��"
							"\n            �� ��ȡ���� ��"
							"\n������������ �� ������ ��"
							"\n              �� �뿪 ��",
							buf);
		}
		buttontype=WINDOW_BUTTONTYPE_NONE;
		windowtype=WINDOW_MESSAGETYPE_SELECT;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_START; 
		break;
	  case 1:
	  buttontype=WINDOW_BUTTONTYPE_CANCEL;
	  windowtype=WINDOW_MESSAGETYPE_PETSELECT;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT; 
		break;
	  case 2:
		NPC_PetDate(meindex,toindex,select,token);
		buttontype=WINDOW_BUTTONTYPE_YESNO;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN; 
		break;
	  case 3:
		if( (CHAR_getInt(toindex,CHAR_GOLD)+CHAR_getWorkInt(toindex,CHAR_WORKSHOPRELEVANTSEC))
			> CHAR_getMaxHaveGold( toindex) ){

			NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_OVER, token);
			windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_GOLDOVER; 
			buttontype=WINDOW_BUTTONTYPE_YESNO;
		}else{
			NPC_PetDel(meindex,toindex,select,token);
			buttontype=WINDOW_BUTTONTYPE_OK;
			windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
		}

	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
		break;
	  case 4:
	 	NPC_PetDel(meindex,toindex,select,token);
		buttontype=WINDOW_BUTTONTYPE_OK;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
		windowtype=WINDOW_MESSAGETYPE_MESSAGE;
		break;
	  case 5: // ��ȡ�ķų��б�
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG ) != 1 )
	  		return;
	  	buttontype=WINDOW_BUTTONTYPE_CANCEL;
	  	windowtype=WINDOW_MESSAGETYPE_PETSELECT;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT2; 
		break;
	  case 6:
		if( CHAR_getInt( toindex, CHAR_GOLD) 
			< NPC_GETPOOLCOST( toindex)) 
		{
		  	NPC_MaxGoldOver(meindex, toindex, NPC_PETSHOP_MSG_COST, token);
			buttontype = WINDOW_BUTTONTYPE_OK;
		}
		else {
			NPC_PetDate2( meindex, toindex, select, token);
			buttontype = WINDOW_BUTTONTYPE_YESNO;
		}
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN2; 
	    break;
	  
	  case 7:
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG ) != 1 )
	  		return;	    
		if( CHAR_getInt( toindex, CHAR_GOLD) < NPC_GETPOOLCOST( toindex)){
		  	NPC_MaxGoldOver(meindex, toindex, NPC_PETSHOP_MSG_COST, token);
		}else {
			NPC_PetDel2( meindex, toindex, select, token);
		}
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
	  //   ��  ľ  ����  
	  case 8:
	  	NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_POOLFULL, token);
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
	  // ¦����Իʸ������  ��ū������
	  case 9: // ȡ���ķų��б�
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG) != 1 )
	  		return;
	  	NPC_getDrawPet( meindex, toindex, token, &buttontype, select);
	  	windowtype = WINDOW_MESSAGETYPE_SELECT;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_DRAWSELECT; 
		break;
	  // ¦����Ի    ��
	  case 10:
		NPC_PetDate3( meindex, toindex, select, token);
		buttontype = WINDOW_BUTTONTYPE_YESNO;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_ASKDRAW;
	    break;
	  // ߯��
	  case 11:
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG ) != 1 )
	  		return;	  
		NPC_PetDel3( meindex, toindex, select, token);
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
	  // ʸ����  ����  
	  case 12:
	  	NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_GETFULL, token);
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
	  // CoolFish: Add 2001/7/4
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

void NPC_PetShopWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	int datanum = -1;
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) {
		return;
	}
	//print("\n NPC_PetShopWindowTalked: data:%s seq:%d sel:%d ", data, seqno, select);

	makeStringFromEscaped( data);
	
	datanum = atoi( data);
	switch( seqno){
	case CHAR_WINDOWTYPE_WINDOWPETSHOP_START:
		{
			int poolflg = CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG);
			if( ( datanum == 3 && poolflg) || // ������
				( datanum == 2 && !poolflg ) ){	// ��ȡ����
				NPC_PetShop_selectWindow( meindex, talkerindex, 1, -1 );
			}else if( datanum == 1 && poolflg) { // �ķų��� 
				if( CHAR_getCharPoolPetElement( talkerindex) != -1 ) {
					NPC_PetShop_selectWindow( meindex, talkerindex, 5, -1 );
				}
				else {
					NPC_PetShop_selectWindow( meindex, talkerindex, 8, -1 );
				}
			}else if( datanum == 2 && poolflg ) { // ��ȡ����
				if( CHAR_getCharPetElement( talkerindex) != -1 ) {
					CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
					NPC_PetShop_selectWindow( meindex, talkerindex, 9, 0);
				}else {
					NPC_PetShop_selectWindow( meindex, talkerindex, 12, -1 );
				}
			}
		}
		break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT:
		if(select==WINDOW_BUTTONTYPE_CANCEL){
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
			
		}else{
			NPC_PetShop_selectWindow( meindex, talkerindex, 2, datanum );
		}
		break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT2:
		if(select==WINDOW_BUTTONTYPE_CANCEL){
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}else{
			NPC_PetShop_selectWindow( meindex, talkerindex, 6, datanum );
		}
		break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN:
		if(select==WINDOW_BUTTONTYPE_YES){
			NPC_PetShop_selectWindow( meindex, talkerindex, 3,-1 );
		}else{
			NPC_PetShop_selectWindow( meindex, talkerindex,1,-1);
	  	}
	  	break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN2:
		if(select==WINDOW_BUTTONTYPE_YES){
			NPC_PetShop_selectWindow( meindex, talkerindex, 7,-1 );
		}else {
			NPC_PetShop_selectWindow( meindex, talkerindex,5,-1);
	  	}
	  	break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_ASKDRAW:
		if( select == WINDOW_BUTTONTYPE_YES){
			if( CHAR_getCharPetElement( talkerindex) != -1 ) {
				NPC_PetShop_selectWindow( meindex, talkerindex, 11,-1 );
			}else {
				NPC_PetShop_selectWindow( meindex, talkerindex, 12, -1 );
			}
		}else {
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
			NPC_PetShop_selectWindow( meindex, talkerindex, 9, 0);
	  	}
	  	break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_GOLDOVER:
		if( select == WINDOW_BUTTONTYPE_YES){
			NPC_PetShop_selectWindow( meindex, talkerindex, 4,-1 );
		}else{
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}
	  	break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_DRAWSELECT:
		if( select == WINDOW_BUTTONTYPE_PREV){
			NPC_PetShop_selectWindow( meindex, talkerindex, 9, -1);
		}else if( select == WINDOW_BUTTONTYPE_NEXT) {
			NPC_PetShop_selectWindow( meindex, talkerindex, 9, 1);
		}else if( select != WINDOW_BUTTONTYPE_CANCEL) {
			if( CHAR_getCharPetElement( talkerindex) != -1 ) {
				NPC_PetShop_selectWindow( meindex, talkerindex, 10, datanum);
			}else {
				NPC_PetShop_selectWindow( meindex, talkerindex, 12, -1 );
			}
		}else {
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}
		break;

	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_END:
		if( select == WINDOW_BUTTONTYPE_OK){
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}
	  	break;

	  // CoolFish: Add 2001/7/4
	  default:
	  	break;

	}
}

void NPC_PetDate(int meindex,int talker,int select,char *token)
{
	
	int petindex;
	int cost;
	char *petname;
	char buf[1024];

	/*--�����ͼ������--*/
	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,select);

	petindex = CHAR_getCharPet( talker, select-1);
        

   if( !CHAR_CHECKINDEX(petindex) ) return;

  	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_REALY, buf);

	cost=NPC_GetCost(meindex,talker,petindex);

	petname = CHAR_getUseName( petindex);

	sprintf(token,"\n��[%s]û��ɣ�""%s"
					"\n\n    �����۸�         %d STONE\n"
					,petname,buf,cost);	
		
}

void NPC_PetDate2(int meindex,int talker,int select,char *token)
{
	
	int petindex;
	char *petname;
	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,select);

	petindex = CHAR_getCharPet( talker, select-1);
	if( !CHAR_CHECKINDEX(petindex) ) return;
	petname = CHAR_getUseName( petindex);
	sprintf(token,"\n�ķ�[%s]"
					"\n\n    �ķż۸�            %d stone\n"
					,petname, NPC_GETPOOLCOST( talker)
					);	
}

void NPC_PetDate3(int meindex,int talker,int select,char *token)
{
	int poolindex;
	int petindex;
	char *petname;
	poolindex = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) * 5 + select - 1;
	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,poolindex );
	petindex = CHAR_getCharPoolPet( talker, poolindex);
	if( !CHAR_CHECKINDEX(petindex) ) return;
	petname = CHAR_getUseName( petindex);
	sprintf(token,"\n���[%s]", petname);	
}

int NPC_GetCost(int meindex,int talker,int petindex)
{
	int cost;
	int levelcost;
	int level;
	int getlevel;
	int graNo;
	int rare;
	int charm=50;
	int petai;
	int i=1;
	double rate=1.0;
	int intrate;
	char buf[1024];
	char buff2[128];
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];
		
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return FALSE;
	}

		
	getlevel=CHAR_getInt(petindex,CHAR_PETGETLV);
	level=CHAR_getInt(petindex,CHAR_LV);
	graNo=CHAR_getInt(petindex,CHAR_BASEBASEIMAGENUMBER);
	rare=CHAR_getInt(petindex,CHAR_RARE);

	if(getlevel==0)	getlevel=1	;
	
	/*--���ë�ƻ���--*/
	levelcost=(level*level)*10;
	getlevel=(getlevel*getlevel)*10;
// Syu ADD �޸���ת�����Ǯ��为��bug
#ifdef _PET_2TRANS
	if( CHAR_getInt(petindex , CHAR_TRANSMIGRATION)>0)
#else
	if( CHAR_getInt(petindex , CHAR_TRANSMIGRATION)==1)
#endif
		getlevel=10;


	if(rare==0){ rare=1;
	}else if(rare==1){ rare=5;
	}else if(rare==2){ rare=8;}

	cost = ((levelcost-getlevel)+(level*PETCOST))*rare;
	intrate = CHAR_getWorkInt( meindex , CHAR_WORK_NOMALRATE);
	rate=(double) intrate / 1000;
	

	NPC_Util_GetStrFromStrWithDelim( npcarg, "special_pet", buf, sizeof( buf));

	i=1;
	/*--��ʸ���һ�������ƥ��ң��ľ��ʸ����ë�Ի����  ʸ������϶�÷�  �  į)--*/
	while( getStringFromIndexWithDelim(buf,",",i,buff2,sizeof(buff2)) !=FALSE )
	{
		if(strstr( buff2, "-")==NULL){
			if(graNo == atoi(buff2)){
				intrate=CHAR_getWorkInt( meindex , CHAR_WORK_SPECIALRATE);
				rate=(double) intrate / 1000;
				break; 
			}
		}else{
			/*--ʧ��  ة��  15-25  ����ƥ����ľ����������--*/
			int start;
			int end;
			int tmp;
			int graflg=0;
			char	token2[128];
	
			/*-"-"ƥ����ľ��炙����Ѱ���  ���Ѱ�ë��  --*/
			getStringFromIndexWithDelim( buff2, "-", 1, token2, sizeof(token2));
			start = atoi( token2);
			getStringFromIndexWithDelim( buff2, "-", 2 ,token2, sizeof(token2));
			end = atoi( token2);

			/*--  į���ѱ�ئ�Ȼ������գ�  ľ׸����**/
			if(start > end){
				tmp=start;
				start=end;
				end=tmp;
			}

			end++;
			/*--"-"ƥ����ľ���м�ʧ��  ة��  ë  ��--*/
			if(start <= graNo && graNo < end){
				intrate=CHAR_getWorkInt( meindex , CHAR_WORK_SPECIALRATE);
				rate=(double) intrate / 1000;
				graflg=1;
				break; 
			}
			
			if(graflg ==1) break;
		}
		i++;
	}

	/*--ʸ���������պë      ---*/
	petai=CHAR_getWorkInt(petindex,CHAR_WORKFIXAI);

	/*--    ë������--*/
	charm = CHAR_getWorkInt(talker,CHAR_WORKFIXCHARM);

	/*--      �����պ  / ��  */
	charm = charm + petai;

	if(charm < 20){
		charm = 20;
	}

	charm = charm / 2;

	cost = (int)cost*rate;

	rate=(double)charm/100;

	/*--    �������--*/
	cost = cost * rate;

	/*--����ë��ʢ--*/
   	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,cost);

	return cost;
}


#if 1

void NPC_PetDel(int meindex, int talker,int select,char* token)
{
	int petindex;
	int petsel;
	char szPet[128];
	int cost;
	int defpet;
	int fd = getfdFromCharaIndex( talker );

	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_THANKS, token);
	petsel=(CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANT)-1);
	petindex = CHAR_getCharPet( talker, petsel);
	
    if( !CHAR_CHECKINDEX(petindex) ) return;

        // CoolFish: Family 2001/6/14
	if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1){	
/*		char buf[1024];
		lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
		makeEscapeString("\n\n�ܱ�Ǹ���ػ����޷�����ร�\n���Լ��ú��չˣ�", buf, sizeof(buf)));
*/
		sprintf(token,"\n\n�ܱ�Ǹ���ػ����޷�����ร�\n���Լ��ú��չˣ�");
		return;
	}

        // Robin 0725
        if (CHAR_getInt( talker, CHAR_RIDEPET ) == petsel ){	
/*        	char buf[1024];
        	lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
        		WINDOW_BUTTONTYPE_OK,
        		-1, -1,
        		makeEscapeString("\n\n����еĳ����޷�����ร�", buf, sizeof(buf)));
*/
					sprintf(token,"\n\n����еĳ����޷�����ร�");
        	return;
        }

	if( CHAR_getWorkInt( CONNECT_getCharaindex(fd),
                         CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return ;
	defpet=CHAR_getInt(talker,CHAR_DEFAULTPET);
	if(defpet==petsel){
		CHAR_setInt(talker,CHAR_DEFAULTPET,-1);
		lssproto_KS_send( fd, -1, TRUE);
	}
   	CHAR_setCharPet( talker, petsel, -1);
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"sell(����)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
	CHAR_endCharOneArray( petindex );
		
	snprintf( szPet, sizeof( szPet ), "K%d", petsel);
	CHAR_sendStatusString( talker, szPet );
	cost=NPC_GetCost(meindex,talker,petindex);
	CHAR_AddGold( talker, cost);

}
#endif
void NPC_PetDel2( int meindex, int talker, int select, char *token)
{
	int petindex;
	int selectpet;
	int emptyindex;
	char szPet[128];
	int fd = getfdFromCharaIndex( talker );

	if( fd == -1 ) {
		fprint( "err\n");
		return;
	}
	// ʸ����������  �ʣ�ʸ����      ��  ľ  
	selectpet = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) -1;
	petindex = CHAR_getCharPet( talker, selectpet);

	if( !CHAR_CHECKINDEX( petindex) ) return;
        // Robin 0725
        if (CHAR_getInt( talker, CHAR_RIDEPET ) == selectpet )
        {
/*        	char buf[1024];
        	lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
        		WINDOW_BUTTONTYPE_OK,
        		-1, -1,
        		makeEscapeString("\n\n����еĳ����޷��ķ�ร�", buf, sizeof(buf)));
*/
					sprintf(token,"\n\n����еĳ����޷��ķ�ร�");
        	return;
        }
        
	// ����ë���ʣ�����ƻ����������ƻ����¼�ƥ�ݳ��ƥ���л���ئ�м����ƾ�����
	emptyindex = CHAR_getCharPoolPetElement( talker);
	if( emptyindex == -1 ) {
		fprint( "err\n");
		return;
	}

	// ��  ��  ��ʸ����ئ�գݹ�ľ���  ë���ʣ�
	if( CHAR_getInt( talker, CHAR_DEFAULTPET) == selectpet) {
		CHAR_setInt( talker, CHAR_DEFAULTPET, -1);
		lssproto_KS_send( fd, -1, TRUE);
	}
	//   ��  ľ  ���ʸ�������̼������͵�ë�����
	CHAR_setCharPoolPet( talker, emptyindex, petindex);
	// ʸ����  ��������
	CHAR_setCharPet( talker, selectpet, -1);

	// ������������  
	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
	// ����Ӽ����

	CHAR_DelGold( talker, NPC_GETPOOLCOST( talker) );

	// ���ŷ�����˪��
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_GOLD);

	snprintf( szPet, sizeof( szPet ), "K%d", selectpet);
	// ޥ����ʸ������  ������˪Ի������
	CHAR_sendStatusString( talker, szPet );
	// ʸ����ë  ���׷��
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"pool(�ķų����)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
        CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);

}	

void NPC_PetDel3( int meindex, int talker, int select, char *token)
{
	int petindex;
	int selectpet;
	int emptyindex;
	char szPet[128];
	int workindex[CHAR_MAXPOOLPETHAVE];
	int i;
	int cnt;

	selectpet = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) ;
	petindex = CHAR_getCharPoolPet( talker, selectpet);
	
	if( !CHAR_CHECKINDEX( petindex) ) return;
	emptyindex = CHAR_getCharPetElement( talker);
	if( emptyindex == -1 ) {
		fprint( "err\n");
		return;
	}

	// ������ػ��޸�Ϊ��ͨ��
	if( CHAR_getInt( petindex, CHAR_PETFAMILY) == 1 
		&& CHAR_getInt( talker, CHAR_FMLEADERFLAG) != 3 ) {
		CHAR_setInt( petindex, CHAR_PETFAMILY, 0);
		CHAR_talkToCli( talker, -1, "�ػ��޸�Ϊ��ͨ��", CHAR_COLORYELLOW);
	}
	CHAR_setCharPet( talker, emptyindex, petindex);
	CHAR_setCharPoolPet( talker, selectpet, -1);
	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
	snprintf( szPet, sizeof( szPet ), "K%d", emptyindex);
	CHAR_sendStatusString( talker, szPet );
	snprintf( szPet, sizeof( szPet), "W%d",emptyindex);
	CHAR_sendStatusString( talker, szPet );
	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		workindex[i] = -1;
	}
	cnt = 0;
	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		int petindex = CHAR_getCharPoolPet( talker, i);
		if( CHAR_CHECKINDEX( petindex) ) {
			workindex[cnt++] = petindex;
		}
	}
	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		CHAR_setCharPoolPet( talker, i, workindex[i]);
	}
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ),
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"draw(��ȡ�����)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);

}	

void NPC_getDrawPet( int meindex, int toindex, char *token, int *buttontype, int select)
{
	// ��ʸ����ؤ��Ի������
	#define		LINENUM		5
	char buf[1024];
	int i;
	int startnum, endnum;
	int petindex = -1;
	// �ػ���ʸ����
	int page = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
	int limit;
	//andy_reEdit 2003/09/18
	limit = (CHAR_getInt(toindex,CHAR_TRANSMIGRATION)*2)+5;

	endnum = (page + select+1)*LINENUM ;
	startnum = endnum - LINENUM;
	*buttontype = WINDOW_BUTTONTYPE_CANCEL;
	// ��ʸ����Ʃ���
	if( endnum >= limit ) {
		endnum = limit;
		if( page +select!= 0 ) *buttontype |= WINDOW_BUTTONTYPE_PREV;
	}
	else if( endnum <= LINENUM ) {
		endnum = LINENUM;
		*buttontype |= WINDOW_BUTTONTYPE_NEXT;
	}
	else {
		*buttontype |= WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_NEXT;
	}
	CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, page +select);
	
	strcpy( token, "2\n����������ѡ����������\n\n");
	for( i =  startnum; i < endnum; i ++ ) {
		petindex = CHAR_getCharPoolPet( toindex, i);
		if( CHAR_CHECKINDEX( petindex) ) {
			snprintf( buf, sizeof( buf), "LV.%3d MaxHP %3d %s\n", 
					CHAR_getInt( petindex, CHAR_LV),
					CHAR_getWorkInt( petindex, CHAR_WORKMAXHP),
					CHAR_getUseName( petindex)
					);
		}
		else {
			strcpy( buf, "\n");
		}
		strcat( token, buf);
	}
	// �ݼ�����ëƩ�ͻ���  ���������ݳ�ʾ����ë����
	if( i != limit ) {
		petindex = CHAR_getCharPoolPet( toindex, i);
		if( !CHAR_CHECKINDEX( petindex)) {
			*buttontype &= ~WINDOW_BUTTONTYPE_NEXT;
		}
	}
}

/*
 *���Ż�����������ְ��Ƿ��Ƿ
 */
void	NPC_MaxGoldOver(int meindex,int toindex,int select,char *token)
{

	char buf[1024];
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE - 1024 * 20];

	NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg));
	
	if( select < 0 || select >= arraysizeof( shopmsg)) return;
	
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, shopmsg[select].option, buf, sizeof( buf))	== NULL ) 
	{
		strcpy( buf, shopmsg[select].defaultmsg);
	}

	snprintf(token,sizeof(buf),"\n\n%s",buf);

}
