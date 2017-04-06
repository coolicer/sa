#include "version.h"
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "char_base.h"
#include "char_data.h"
#include "item.h"
#include "battle.h"
#include "magic_base.h"
#include "magic.h"
#include "magic_field.h"
#include "family.h"
#include "handletime.h"
#include "battle_magic.h"
#include "battle_event.h"
#include"lssproto_serv.h"
#ifdef _ITEM_ATTSKILLMAGIC
#include "pet_skill.h"
#endif
//#include "battle_event.h"
// shan add code
#define MP_RATE    0.7


int MAGIC_Use( int charaindex, int haveitemindex, int toindex)
{
	int		itemindex;
	int		magicid,marray,mp;
	int		ret;
	MAGIC_CALLFUNC func;

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX( itemindex) ) return FALSE;
	magicid = ITEM_getInt( itemindex, ITEM_MAGICID);
	marray = MAGIC_getMagicArray( magicid);
	if( marray == -1 ) return FALSE;

	func = MAGIC_getMagicFuncPointer( MAGIC_getChar( marray, MAGIC_FUNCNAME));//MAGIC_NAME
	if( (mp = ITEM_getInt( itemindex, ITEM_MAGICUSEMP)) < 0 ){
		print("err mp:%d  -%s-%d\n", mp, ITEM_getChar( itemindex, ITEM_NAME),
			ITEM_getInt( itemindex, ITEM_ID) );
		
	}
	
        // shan add begin
        if( CHAR_getInt( charaindex, CHAR_FMINDEX ) >= 1 ){
            // �⾫
            if( CHAR_getInt( charaindex, CHAR_FMSPRITE ) == 0){
                if( (marray >=0 && marray <=31) || (marray >= 105 && marray <=107 ))
                    mp *= MP_RATE;
            }
            // ����
            if( CHAR_getInt( charaindex, CHAR_FMSPRITE ) == 1){
                if( marray >=32 && marray <=81 )
                    mp *= MP_RATE;
            }
        }
        // shan end
	if( func ) {
		ret = func( charaindex, toindex, marray, mp );
	}else {
		ret = FALSE;
	}
	return ret;
}

int MAGIC_GetArrayNo( int charaindex, int haveitemindex)
{
	int		itemindex;
	int		magicid;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX( itemindex) ) return FALSE;

	magicid = ITEM_getInt( itemindex, ITEM_MAGICID);

	return magicid;
}
int MAGIC_DirectUse(
	int charaindex,
	int magicid,
	int toindex,
	int itemnum
)
{
	MAGIC_CALLFUNC func;
	int mp, ret, marray;
	int itemindex=-1;
#ifdef _MAGIC_NOCAST//��Ĭ״̬�޷�ʩ��
    if( CHAR_getWorkInt( charaindex, CHAR_WORKNOCAST ) > 0 ){
		CHAR_talkToCli(charaindex, -1, "��Ĭ���޷���������", CHAR_COLORYELLOW);
		return FALSE;
	}
#endif

	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER){
		itemindex = CHAR_getItemIndex( charaindex, itemnum);
		if( !ITEM_CHECKINDEX( itemindex) ){
			print("ANDY err MAGIC_DirectUse() itemindex:%d !\n", itemindex);
			return FALSE;
		}
	}else{
		itemindex = itemnum;
	}

	marray = MAGIC_getMagicArray( magicid );
	func = MAGIC_getMagicFuncPointer( MAGIC_getChar( marray, MAGIC_FUNCNAME ) );
	if( (mp = ITEM_getInt( itemindex, ITEM_MAGICUSEMP )) < 0 ){
	}
        // shan add begin
        if( CHAR_getInt( charaindex, CHAR_FMINDEX ) >= 1 ){
            // �⾫
            if( CHAR_getInt( charaindex, CHAR_FMSPRITE ) == 0){
                if( (marray >=0 && marray <=31) )
                    mp *= MP_RATE;
            }
            // ����
            if( CHAR_getInt( charaindex, CHAR_FMSPRITE ) == 1){
                if( marray >=32 && marray <=81 )
                    mp *= MP_RATE;
            }
        }
	if( func ) {
		ret = func( charaindex, toindex, marray, mp );
	}
	else {
		ret = FALSE;
	}
	return ret;
}

int MAGIC_Recovery( int charaindex, int toindex, int marray, int mp )
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}


	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE;
	CHAR_setInt( charaindex, CHAR_MP,
		CHAR_getInt( charaindex, CHAR_MP ) - mp );
	if( IsBATTLING( charaindex ) == TRUE ){
		if (toindex==22){print("jinchao err\n");return FALSE;}  // shan(��ȫ��ʹ��ħ����bug)���޸���jinchao+2001/12/07
		MAGIC_Recovery_Battle( charaindex, toindex, marray, mp );
	}else{
		if( CHAR_CHECKINDEX( toindex ) == FALSE )return FALSE;  // shan(��ȫ��ʹ��ħ����bug)���޸���jinchao+2001/12/07
		MAGIC_Recovery_Field( charaindex, marray);
	}

	return TRUE;
}

int MAGIC_OtherRecovery( int charaindex, int toindex, int marray, int mp )
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE; //����  
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE; //   ��������ĸ��
	CHAR_setInt( charaindex, CHAR_MP,
		CHAR_getInt( charaindex, CHAR_MP ) - mp );
	if( IsBATTLING( charaindex ) == TRUE ){
		MAGIC_Recovery_Battle( charaindex, toindex, marray, mp );
	}else{
		if( CHAR_CHECKINDEX( toindex ) == FALSE )return FALSE; //����  
		MAGIC_OtherRecovery_Field( charaindex, toindex, marray);
	}
	return TRUE;
}

// ��ū����������ë  ��������  ܷ
int	MAGIC_FieldAttChange( int charaindex, int toindex, int marray, int mp )
{
	int battlemode;

	// INDEX��������
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE; //����  

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );

	// ��  ����ݷ�  ������
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

	//     ��  ��������
	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE; //   ��������ĸ��
	// ��  ܷ��  ɧ��    ��  �ƻ�  �У�Ѩ�̹ϵ���ئ�³��練  ��
	CHAR_setInt( charaindex, CHAR_MP,
		CHAR_getInt( charaindex, CHAR_MP ) - mp );

	// ��    ��������
	if( IsBATTLING( charaindex ) == TRUE ){
		return MAGIC_FieldAttChange_Battle( charaindex, toindex, marray, mp );
	}else{
	// ��    ƥئ��������
		return FALSE;
	}
}
int	MAGIC_StatusChange( int charaindex, int toindex, int marray, int mp )
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE;
	CHAR_setInt( charaindex, CHAR_MP, CHAR_getInt( charaindex, CHAR_MP ) - mp );
	if( IsBATTLING( charaindex ) == TRUE ){
		return MAGIC_StatusChange_Battle( charaindex, toindex, marray, mp );
	}else{
		return FALSE;
	}
}

#ifdef _OTHER_MAGICSTAUTS
int	MAGIC_MagicStatusChange( int charaindex, int toindex, int marray, int mp )
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )	return FALSE;

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}
	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE;
	CHAR_setInt( charaindex, CHAR_MP, CHAR_getInt( charaindex, CHAR_MP ) - mp );
	if( IsBATTLING( charaindex ) == TRUE ){
		return MAGIC_MagicStatusChange_Battle( charaindex, toindex, marray, mp );
	}else{
		return FALSE;
	}
}
#endif

#ifdef _MAGIC_DEEPPOISON//����ver2,��Ҫ�趨��״̬5��δ��ʱ���������
// ��  ����������ëܸ����  ܷ
int	MAGIC_StatusChange2( int charaindex, int toindex, int marray, int mp )
{
	int battlemode;

	// INDEX��������
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE; //����  

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );

	// ��  ����ݷ�  ������
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

    //����
	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE; 

	CHAR_setInt( charaindex, CHAR_MP,
		CHAR_getInt( charaindex, CHAR_MP ) - mp );

    //call func
	if( IsBATTLING( charaindex ) == TRUE ){
		return MAGIC_StatusChange_Battle2( charaindex, toindex, marray, mp );
	}else{
		return FALSE;
	}
}
#endif
// ��  ���������ټ��������  ܷ
int	MAGIC_StatusRecovery( int charaindex, int toindex, int marray, int mp )
{
	int battlemode;

	// INDEX��������
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE; //����  

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );

	// ��  ����ݷ�  ������
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

	//     ��  ��������
	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE; //   ��������ĸ��
	// ��  ܷ��  ɧ��    ��  �ƻ�  �У�Ѩ�̹ϵ���ئ�³��練  ��
	CHAR_setInt( charaindex, CHAR_MP,
		CHAR_getInt( charaindex, CHAR_MP ) - mp );

	// ��    ��������
	if( IsBATTLING( charaindex ) == TRUE ){
		return MAGIC_StatusRecovery_Battle( charaindex, toindex, marray, mp );
	}else{
	// ��    ƥئ��������
		return FALSE;
	}
}

int	MAGIC_MagicDef( int charaindex, int toindex, int marray, int mp )
{
	int battlemode;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE;
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE;
	CHAR_setInt( charaindex, CHAR_MP, CHAR_getInt( charaindex, CHAR_MP ) - mp );
	if( IsBATTLING( charaindex ) == TRUE ){
		return MAGIC_MagicDef_Battle( charaindex, toindex, marray, mp );
	}else{
		return FALSE;
	}
}


// ��姾������ٽ�����  ܷ
int	MAGIC_Ressurect( int charaindex, int toindex, int marray, int mp )
{
	int battlemode;

	// INDEX��������
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE; //����  

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );

	// ��  ����ݷ�  ������
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

	//     ��  ��������
	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE; //   ��������ĸ��
	// ��  ܷ��  ɧ��    ��  �ƻ�  �У�Ѩ�̹ϵ���ئ�³��練  ��
	CHAR_setInt( charaindex, CHAR_MP,
		CHAR_getInt( charaindex, CHAR_MP ) - mp );

	// ��    ��������
	if( IsBATTLING( charaindex ) == TRUE ){
		return MAGIC_Ressurect_Battle( charaindex, toindex, marray, mp );
	}else{
	// ��    ƥئ��������
		return FALSE;
	}
}

// ����ë  �������  ܷ
int	MAGIC_AttReverse( int charaindex, int toindex, int marray, int mp )
{
	int battlemode;

	// INDEX��������
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE; //����  

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );

	// ��  ����ݷ�  ������
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

	//     ��  ��������
	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE; //   ��������ĸ��
	// ��  ܷ��  ɧ��    ��  �ƻ�  �У�Ѩ�̹ϵ���ئ�³��練  ��
	CHAR_setInt( charaindex, CHAR_MP,
		CHAR_getInt( charaindex, CHAR_MP ) - mp );

	// ��    ��������
	if( IsBATTLING( charaindex ) == TRUE ){
		return MAGIC_AttReverse_Battle( charaindex, toindex, marray, mp );
	}else{
	// ��    ƥئ��������
		return FALSE;
	}
}



// ��姾������١�  ܷ  ��
int	MAGIC_ResAndDef( int charaindex, int toindex, int marray, int mp )
{
	int battlemode;

	// INDEX��������
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE; //����  

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );

	// ��  ����ݷ�  ������
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

	//     ��  ��������
	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE; //   ��������ĸ��
	// ��  ܷ��  ɧ��    ��  �ƻ�  �У�Ѩ�̹ϵ���ئ�³��練  ��
	CHAR_setInt( charaindex, CHAR_MP,
		CHAR_getInt( charaindex, CHAR_MP ) - mp );

	// ��    ��������
	if( IsBATTLING( charaindex ) == TRUE ){
		return MAGIC_ResAndDef_Battle( charaindex, toindex, marray, mp );
	}else{
	// ��    ƥئ��������
		return FALSE;
	}
}



#ifdef _ATTACK_MAGIC

int MAGIC_AttMagic( int charaindex , int toindex , int marray , int mp )
{
   int battlemode;
   
   if( FALSE == CHAR_CHECKINDEX( charaindex ) )
   	return FALSE;
   	
   battlemode = CHAR_getWorkInt( charaindex , CHAR_WORKBATTLEMODE );
   if( BATTLE_CHARMODE_INIT == battlemode )
   	return FALSE;
	 // ���ǳ���ħ��,����mp
   if(CHAR_getInt(charaindex,CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
	 {
     if( CHAR_getInt( charaindex , CHAR_MP ) < mp )
   	  return FALSE;
   
     CHAR_setInt( charaindex , CHAR_MP , CHAR_getInt( charaindex , CHAR_MP ) - mp );
	 }

   if( TRUE == IsBATTLING( charaindex ) )
   	 return MAGIC_AttMagic_Battle( charaindex , toindex , marray , mp );
   	
   return FALSE;   

}

#endif

#ifdef _ITEM_METAMO
int MAGIC_Metamo( int charaindex, int toindex, int marray, int mp )
{
	int battlemode, haveindex, power, i;
	char msg[128];
	
	if( CHAR_CHECKINDEX( charaindex ) == FALSE ) return FALSE;
	
	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	// ��  ����ݷ�  ������
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}
	// ��    ��������
	if( IsBATTLING( charaindex ) == TRUE ){
		//MAGIC_Recovery_Battle( charaindex, toindex, marray, mp );
		//return FALSE;
		toindex = BATTLE_No2Index(
				CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX ), toindex );
		//print(" battle_metamo:%d ", toindex);
	}

	if( CHAR_CHECKINDEX( toindex ) == FALSE ) return FALSE;

	if( CHAR_getInt( charaindex, CHAR_RIDEPET ) != -1 ){
		CHAR_talkToCli( charaindex, -1, "�޷���������в��ܱ���", CHAR_COLORWHITE );
		return FALSE;
	}
#ifdef _FIX_METAMORIDE
	if( CHAR_CHECKJOINENEMY( charaindex) == TRUE ){
		CHAR_talkToCli( charaindex, -1, "�޷���������в��ܱ���", CHAR_COLORYELLOW );
		return FALSE;
	}
#else
	if( CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER) == 100259 ){
		CHAR_talkToCli( charaindex, -1, "�޷���������в��ܱ���", CHAR_COLORWHITE );
		return FALSE;
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
				return FALSE;
			}
		}
	}
#endif
	haveindex = -1;	
	for( i = 0; i < 5; i++ )
	{
		if( CHAR_getCharPet( charaindex, i) == toindex ) {
			haveindex = i;
			break;
		}
	}
	if( haveindex == -1 && charaindex != toindex )
	{
		CHAR_talkToCli( charaindex, -1, "�޷�����ֻ�ܱ���Լ��ĳ��", CHAR_COLORWHITE );
		return FALSE;
	}

	if( CHAR_getInt( toindex, CHAR_BASEIMAGENUMBER) == 100259 ){
		CHAR_talkToCli( charaindex, -1, "���ܱ���ɼ�����", CHAR_COLORWHITE );
		return FALSE;
	}

	power = atoi(MAGIC_getChar( marray, MAGIC_OPTION ));

	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE;
	CHAR_setInt( charaindex, CHAR_MP,
		CHAR_getInt( charaindex, CHAR_MP ) - mp );
	CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_MP);

	if( toindex != charaindex ) {
		CHAR_setWorkInt( charaindex, CHAR_WORKITEMMETAMO, NowTime.tv_sec +power);
		sprintf( msg, "�����%s��", CHAR_getChar( toindex, CHAR_NAME) );
	}
	else {
		CHAR_setWorkInt( charaindex, CHAR_WORKITEMMETAMO, 0);
		sprintf( msg, "����Լ���");
	}
	CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORWHITE );

	CHAR_setInt( charaindex, CHAR_BASEIMAGENUMBER,
					CHAR_getInt( toindex, CHAR_BASEBASEIMAGENUMBER) );
	CHAR_complianceParameter( charaindex);
	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
	CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
//#endif

	return TRUE;
}
#endif

#ifdef _ITEM_ATTSKILLMAGIC
int MAGIC_AttSkill( int charaindex, int toindex,int marray, int mp )
{
	int battlemode;
	int battleindex=-1;
	char *magicarg, funName[256];
	int skillID=-1;
	PETSKILL_CALLFUNC func;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE ) return FALSE;
	if( IsBATTLING( charaindex ) == FALSE ) return FALSE;
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE ) return FALSE;

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );
	if( battlemode == BATTLE_CHARMODE_INIT ) return FALSE;
	if( toindex < 0 || toindex > 19 ) return FALSE;
	if( CHAR_getInt( charaindex, CHAR_MP ) < mp ) return FALSE;

	memset( funName, 0, sizeof( char)*256 );
	//MAGIC_Recovery_Battle( charaindex, toindex, marray, mp );
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );
	if ( magicarg == "\0") return FALSE;
	if( strstr( magicarg, ";" ) != NULL )	{
		char buff1[256];
		if( getStringFromIndexWithDelim( magicarg, ";", 1, buff1, sizeof( buff1)) == FALSE )
			return FALSE;
		snprintf( funName, sizeof(funName), buff1);
		if( getStringFromIndexWithDelim( magicarg, ";", 2, buff1, sizeof( buff1)) == FALSE )
			return FALSE;
		skillID = atoi( buff1);
	}else
		return FALSE;
	func = PETSKILL_getPetskillFuncPointer( funName);
	if( !func ){
		print("\n *func not FOUND !!");
		return FALSE;
	}
	if( func( charaindex,  toindex,  skillID, "MAGICSKILL" ) == FALSE ) return FALSE;
	CHAR_setInt( charaindex, CHAR_MP, CHAR_getInt( charaindex, CHAR_MP ) - mp );
	return TRUE;

}
#endif
#ifdef _MAGIC_BARRIER// vincent  ����:ħ��
int	MAGIC_Barrier( int charaindex, int toindex, int marray, int mp )
{

	int battlemode;
	int i,turn,perStatus;
	int ToList[SIDE_OFFSET*2+1];
	char *magicarg;
	int  attackNo=0;
	int battleindex,Success = 0;
	char szTurn[] = "turn";
	char szSuccess[] = "��";
	char *pszP;

	//check index
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE;

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );

	// ��  ����ݷ�  ������//??
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

	//mp����
	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE; 
	//����mp
	CHAR_setInt( charaindex, CHAR_MP,
		CHAR_getInt( charaindex, CHAR_MP ) - mp );
    //ħ������
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );
	if( magicarg == "\0" )	{
		print("\n magicarg == NULL ");
		return FALSE;
	}
	pszP = magicarg;

    //��ȡ���ûغ���
	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}
	// ��    ɬ��//������
	if( ( pszP = strstr( pszP, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );
	BATTLE_MultiList( battleindex, toindex, ToList );
//ʩħ������Ч��
	BATTLE_MagicEffect(battleindex, attackNo, ToList, MAGIC_EFFECT_USER, SPR_hoshi);

	charaindex = BATTLE_No2Index( battleindex, attackNo );
	//check�Ƿ�ս����
	if( IsBATTLING( charaindex ) == TRUE )
	{

		 for( i = 0; ToList[i] != -1; i ++ )
		{
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		    if( BATTLE_StatusAttackCheck( charaindex,toindex, BATTLE_ST_BARRIER, Success, 30, 1.0, &perStatus ) == TRUE )
			{
		       CHAR_setWorkInt( toindex, CHAR_WORKBARRIER, turn+1 );
//		       BATTLE_BadStatusString( ToList[i], BATTLE_ST_BARRIER );
			}
		}
		return TRUE;
	}else{
		return FALSE;
	}
}
#endif
#ifdef _MAGIC_NOCAST// vincent  ����:��Ĭ
int	MAGIC_Nocast( int charaindex, int toindex, int marray, int mp )
{

	int battlemode;
	int i,turn,perStatus;
	int ToList[SIDE_OFFSET*2+1];
	char *magicarg;
	int  attackNo=0;
	int battleindex,Success = 0;
	char szTurn[] = "turn";
	char szSuccess[] = "��";
	char *pszP;

	//check index
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE;

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );

	// ��  ����ݷ�  ������//??
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

	//mp����
	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE; 
	//����mp
	CHAR_setInt( charaindex, CHAR_MP,
		CHAR_getInt( charaindex, CHAR_MP ) - mp );
    //ħ������
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );
	if( magicarg == "\0" )	{
		print("\n magicarg == NULL ");
		return FALSE;
	}
	pszP = magicarg;

    //��ȡ���ûغ���
	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}
	// ��    ɬ��//������
	if( ( pszP = strstr( pszP, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}
//               print("\nvincent-->MAGIC_Nocast use");
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );
	BATTLE_MultiList( battleindex, toindex, ToList );
//ʩħ������Ч��
	BATTLE_MagicEffect(battleindex, attackNo, ToList, MAGIC_EFFECT_USER, SPR_hoshi);
	charaindex = BATTLE_No2Index( battleindex, attackNo );
	//check�Ƿ�ս����
	if( IsBATTLING( charaindex ) == TRUE )
	{

		 for( i = 0; ToList[i] != -1; i ++ )
		{
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		    if( BATTLE_StatusAttackCheck( charaindex,toindex, BATTLE_ST_NOCAST, Success, 30, 1.0, &perStatus ) == TRUE 
				&& CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET)
			{
		       CHAR_setWorkInt( toindex, CHAR_WORKNOCAST, turn );
//�޷�չ������ҳ
	           lssproto_NC_send( getfdFromCharaIndex( toindex ), 1);
               //print("\nvincent-->NOCASTING2");
//		       BATTLE_BadStatusString( ToList[i], BATTLE_ST_NOCAST );
			}
		}
		return TRUE;
	}else{
		return FALSE;
	}
}
#endif

#ifdef _MAGIC_WEAKEN// vincent  ����:����
int	MAGIC_Weaken( int charaindex, int toindex, int marray, int mp )
{

	int battlemode;
	//check index
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE;

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );

	// ��  ����ݷ�  ������//??
	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

	//mp����
	if( CHAR_getInt( charaindex, CHAR_MP ) < mp )return FALSE; 
	//����mp
	CHAR_setInt( charaindex, CHAR_MP,
		CHAR_getInt( charaindex, CHAR_MP ) - mp );

	//check�Ƿ�ս����
	if( IsBATTLING( charaindex ) == TRUE ){
		return MAGIC_ParamChange_Turn_Battle( charaindex, toindex, marray, mp );
	}else{
		return FALSE;
	}
}
#endif

#ifdef _MAGIC_TOCALL
int MAGIC_ToCallDragon( int charaindex, int toindex,int marray, int mp )
{
	int battlemode;
   
	//print("MAGIC_ToCallDragon in .................\n");
	if( FALSE == CHAR_CHECKINDEX( charaindex ) )
		return FALSE;
   	
	battlemode = CHAR_getWorkInt( charaindex , CHAR_WORKBATTLEMODE );
	if( BATTLE_CHARMODE_INIT == battlemode )
		return FALSE;
	if( CHAR_getInt( charaindex , CHAR_MP ) < mp )
		return FALSE;
   
	CHAR_setInt( charaindex , CHAR_MP , CHAR_getInt( charaindex , CHAR_MP ) - mp );

	if( TRUE == IsBATTLING( charaindex ) )
		return MAGIC_ToCallDragon_Battle( charaindex , toindex , marray , mp );
   	
	return FALSE;   
}
#endif




