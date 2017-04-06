#include "version.h"
#define _NPC_MSG_C_

#include "char.h"
#include "char_base.h"
#include "npcutil.h"


/*
  �����ئ��  ��  쫷���ƥ�ַ��ʣ�
  ��Իؤ��������  ������ƥ��έ���·��������£�

  init , looked ���ı�  ɱ

  */  
BOOL NPC_MsgInit( int meindex )
{
    //CHAR_setInt( meindex , CHAR_HP , 0 );

    //CHAR_setInt( meindex , CHAR_MP , 0 );
    //CHAR_setInt( meindex , CHAR_MAXMP , 0 );

    //CHAR_setInt( meindex , CHAR_STR , 0 );
    //CHAR_setInt( meindex , CHAR_TOUGH, 0 );

    //CHAR_setInt( meindex , CHAR_LV , 0 );

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPETOWNPEOPLE );
    //CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );
    //CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );  /*   ����ľئ�з��� */

    return TRUE;
}


/*
 * έ��ľ�ף۷�ئ�ʷ����
 */
void NPC_MsgLooked( int meindex , int lookedindex )
{
	char	arg[NPC_UTIL_GETARGSTR_BUFSIZE];
    
	NPC_Util_GetArgStr( meindex, arg, sizeof( arg));
    CHAR_talkToCli( lookedindex , -1 , arg , CHAR_COLORWHITE );

}
