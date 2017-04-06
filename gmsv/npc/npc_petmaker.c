#include "version.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "object.h"
#include "char_base.h"
#include "char.h"
#include "util.h"
#include "npcutil.h"
#include "item.h"
#include "readmap.h"
#include "enemy.h"
#include "chatmagic.h"
#include "log.h"

#define EVFLG 118                 // 58->�׻�  118->����
  
void pet_make( int charaindex, char* message );
  
BOOL NPC_PetMakerInit( int meindex )
{
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEPETMAKER );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );
    return TRUE;
}

void NPC_PetMakerTalked( int meindex , int talker , char *msg ,int color )
{
    char* npcarg;
    char  token[32],tmpbuf[256];
    int   msgNo/*,translv*/;
    int   point,ckpoint,array,shift; 
    int   i,num=0;

    array   = EVFLG / 32;
    shift   = EVFLG % 32;
    point   = CHAR_getInt( talker, CHAR_ENDEVENT + array);
    ckpoint = point;

    npcarg = CHAR_getChar(meindex,CHAR_NPCARGUMENT);

    getStringFromIndexWithDelim( npcarg, "|", 1, token,sizeof( token));
    msgNo  = atoi( token );

    if( CHAR_getInt( talker , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )	
        return;

    if( NPC_Util_isFaceToFace( talker, meindex , 2 ) == FALSE ) 
    {
    	if( NPC_Util_isFaceToFace( talker, meindex , 1 ) == FALSE ) 
        	return;
    }
    
    //translv = CHAR_getInt( talker, CHAR_TRANSMIGRATION);    // �׻�
    
    for(i=0; i<CHAR_MAXPETHAVE; i++)
        if(CHAR_getCharPet(talker, i) != -1) 
            num++;

    // �׻�
    /*if( translv < 5 || CHAR_getInt( talker, CHAR_LV ) < 125){
        sprintf(tmpbuf, "��ǸŶ���ȵ�ת�����μ��ȼ���������ʱ�������Ұɣ�");
        CHAR_talkToCli( talker, meindex,
                        tmpbuf,CHAR_COLORWHITE);
        return;
    }*/   
	// ����	
    if( CHAR_getInt( talker, CHAR_FAME) < 200000){
        sprintf(tmpbuf, "��ǸŶ����Ŀǰ�������� %d �㣬��ȵ�������ǧ��ʱ�������Ұɣ�", (CHAR_getInt( talker, CHAR_FAME)/100));
        CHAR_talkToCli( talker, meindex,
                        tmpbuf,CHAR_COLORWHITE);
        return;
    }
    else if( ckpoint & (1 << shift) ){
        sprintf(tmpbuf, "Ŷ���Ҽǵ��Һ���������˰ɣ�");
        CHAR_talkToCli( talker, meindex,
                        tmpbuf,CHAR_COLORWHITE);
        return;
    }    
    else if( num>=CHAR_MAXPETHAVE ){
        sprintf(tmpbuf, "��Ǹ��û����Ŀռ�ɷ��ó��");
        CHAR_talkToCli( talker, meindex,
                        tmpbuf,CHAR_COLORWHITE);
        return;    
    }
    
    pet_make( talker, "1845" );
    
    point = point | (1 << shift);
    CHAR_setInt( talker, CHAR_ENDEVENT + array, point);
}

void pet_make( int charaindex, char* message )
{
    int  ret;
    char msgbuf[64];
    int  enemynum;
    int  enemyid;
    int  i;

    enemyid  = atoi(message);
    enemynum = ENEMY_getEnemyNum();

    for( i=0; i<enemynum; i++ ){
        if( ENEMY_getInt( i, ENEMY_ID) == enemyid){
            break;
        }
    }
    if(i==enemynum) return;

    ret = ENEMY_createPetFromEnemyIndex( charaindex, i);
    snprintf( msgbuf, sizeof( msgbuf), "Ŷ��������������������һֻ��˵����ðɣ��������ˡ�");
    CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORYELLOW);
                
    for( i=0; i<CHAR_MAXPETHAVE; i++ ){
        if( CHAR_getCharPet( charaindex, i) == ret )break;
    }
  
    if( i==CHAR_MAXPETHAVE) i = 0;
    if( CHAR_CHECKINDEX( ret ) == TRUE ){
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
    }
                                                                                                
    snprintf( msgbuf, sizeof( msgbuf ), "K%d", i );
    CHAR_sendStatusString( charaindex, msgbuf );
                     
    snprintf( msgbuf, sizeof( msgbuf ), "W%d", i );
    CHAR_sendStatusString( charaindex, msgbuf );
	
	for( i=0; i<CHAR_MAXPETHAVE; i++ )
		if( CHAR_getCharPet(charaindex, i) == ret )
			CHAR_send_K_StatusString(charaindex, i, CHAR_K_STRING_HP|CHAR_K_STRING_AI);
	
	LogPet(
		CHAR_getChar( charaindex, CHAR_NAME ), /* ƽ�ҷ�   */
		CHAR_getChar( charaindex, CHAR_CDKEY ),
		"����",1,"PetMaker",
		CHAR_getInt( charaindex,CHAR_FLOOR),
		CHAR_getInt( charaindex,CHAR_X ),
		CHAR_getInt( charaindex,CHAR_Y ),
		CHAR_getChar( ret, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
}
