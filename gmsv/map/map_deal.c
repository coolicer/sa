#include "version.h"
#include "common.h"
#include "readmap.h"
#include "char.h"
#include "map_util.h"
#include "lssproto_serv.h"
#include "net.h"
#include "anim_tbl.h"

/*#define MAPEDITORTROUBLE*/
#define SPR_kmydamY CG_HIT_MARK_10 // anim_tbl.h ���  ��ľ����ƥ  ���ް�������

/*------------------------------------------------------------
 * �����¾�������  Ӭ���£�ƽ�ҷ»�  ��ƥ���¾�������¦�ѻ�ؤ�£�
 * ¦��
 *  ff          int     floor
 *  fx          int     x��  
 *  fy          int     y��  
 *  isfly       int       ��ƥ���¾�������
 * ߯Ի��
 *  ������      TRUE(1)
 *  ����ئ��    FALSE(0)
 ------------------------------------------------------------*/
BOOL MAP_walkAbleFromPoint( int ff, int fx, int fy, BOOL isfly )
{
    int map[2];

    if( !MAP_getTileAndObjData( ff,fx,fy, &map[0], &map[1] ) ){
        return FALSE;
	}

    if( isfly ){
        int i;
        for( i = 0 ; i < 2 ; i ++ )
            if( MAP_getImageInt( map[i], MAP_HAVEHEIGHT ) == TRUE  ){
                return FALSE;
						}
        return TRUE;
    }else{
        switch( MAP_getImageInt( map[1], MAP_WALKABLE ) ){
        case 0:
            return FALSE;
            break;
        case 1:
            if( MAP_getImageInt( map[0], MAP_WALKABLE ) == 1 ){
                return TRUE;
			}else{
                return FALSE;
			}
            break;
        case 2:
            return TRUE;
            break;
        default:
            return FALSE;
            break;
        }
    }
    return FALSE;
}



/*------------------------------------------------------------
 * �����¾�������  Ӭ���£�
 * ¦��
 *  index       int     Char    ƥ�� Index
 *  ff          int     floor id
 *  fx          int     x��  
 *  fy          int     y��  
 * ߯Ի��
 *  ���л�����      TRUE(1)
 *  ���л���ĸ��    FALSE(0)
 ------------------------------------------------------------*/
BOOL MAP_walkAble( int index,int ff, int fx, int fy)
{
    /*      ƽ�ҷ�ئ��OK    */
    if( CHAR_getFlg( index, CHAR_ISTRANSPARENT ) )      return TRUE;

    // Arminius 7.9 Airplane ok
    if( CHAR_getInt( index, CHAR_WHICHTYPE ) == CHAR_TYPEBUS) return TRUE;
    if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ) {
      int oyaindex = CHAR_getWorkInt( index, CHAR_WORKPARTYINDEX1);
      if( CHAR_CHECKINDEX( oyaindex)) {
        if (CHAR_getInt( oyaindex, CHAR_WHICHTYPE ) == CHAR_TYPEBUS) return TRUE;
      }
    }

    return MAP_walkAbleFromPoint( ff,fx,fy,CHAR_getFlg(index,CHAR_ISFLYING)|CHAR_getWorkInt(index,CHAR_WORKSKYWALKER) );
}

/*------------------------------------------------------------
 * ��  ë϶���ƻ���������Ƥ�����������ν�ؤ�¾�������
 * ¦��
 *  fl              int     �׷�ʧ
 *  x               int     x ��  
 *  y               int     y ��  
 * ߯Ի��
 *  �ν�ؤ��  TRUE
 *  �ν�ئ��  FALSE
 ------------------------------------------------------------*/
BOOL MAP_haveHeight( int fl, int x, int y )
{
    int     map[2];

    /*  Ѩ���󷸡���ë  ������  */
    if( !MAP_getTileAndObjData( fl,x,y, &map[0], &map[1] ) )
        return FALSE;

    return MAP_getImageInt( map[1], MAP_HAVEHEIGHT );
}

/*------------------------------------------------------------
 * Map ��ì������ pre postë��  ����
 * ¦��
 *  index       int     �Ƿ�����ƻ�����ƽ�ҷ¼��̼������͵�
 *  mode        BOOL    TRUE���ݷ���in  FALSE���ݷ� out �帲ɱ
 * ߯Ի��ئ��
 ------------------------------------------------------------*/
static void MAP_dealprepostevent( int index, BOOL mode )
{
    int     map[2];
    int     i;
    int     damaged=FALSE;

    if( CHAR_getFlg(index,CHAR_ISFLYING) )
        /*    ��ƥ��֧���巴������ئ��  */
        return;

    if( !MAP_getMapDataFromCharIndex( index  ,  map ) ) return;

    for( i = 0 ; i < 2 ; i ++ ){
        /*  damage */
        int damage;
        damage = MAP_getImageInt(map[i],
                                 mode ? MAP_INTODAMAGE : MAP_OUTOFDAMAGE);
        if( damage != 0 ){
            /*  ĸ��������ë��ƥ  �����¼�ƥ  į  ľ׸��    */
            int opt[2]={SPR_kmydamY,-damage};
            damaged=TRUE;
            CHAR_setInt(index, CHAR_HP,
                        CHAR_getInt(index,CHAR_HP) + damage );
            CHAR_complianceParameter(index);
            /*  ĸ�����ⰾ���׾ް�����������    */
            CHAR_sendWatchEvent(CHAR_getWorkInt(index,CHAR_WORKOBJINDEX),
                                CHAR_ACTDAMAGE,opt,2,TRUE);
        }
    }
    if( damaged )
    	CHAR_sendStatusString(index,"M");
}


/*----------------------------------------
 * �������̻�����������ľ�£۳��ƥ������ת�����xxx�̼�
 * ��ë  �����£�
 * ¦��
 *  index       int     �Ƿ�����ƻ�����ƽ�ҷ¼��̼������͵�
 *  flooor      int     �׷�ʧID
 *  fx          int     x��  
 *  fy          int     y��  
 * ߯Ի��
 *  ئ��
 ----------------------------------------*/
void MAP_preovered( int index )
{
    MAP_dealprepostevent( index, FALSE );
}

/*----------------------------------------
 * �������̻�������  ������ľ�£�
 * ¦��
 *  index       int     �Ƿ�����ƻ�����ƽ�ҷ¼��̼������͵�
 * ߯Ի��
 *  ئ��
 ----------------------------------------*/
void MAP_postovered( int index )
{
    MAP_dealprepostevent( index, TRUE );
}


/*----------------------------------------
 * ƽ�ҷ¼���Ի��Ѩ����ë���ͻ�˪��
 * ¦��
 *  fd          int
 *  charaindex  int     ƽ�ҷ¼��̼������͵�
 * ߯Ի��
 *  ��      TRUE(1)
 *  ��      FALSE(0)
 ----------------------------------------*/
BOOL MAP_sendArroundCharNeedFD( int fd,int charaindex )
{
    char*   stringdata;
    int     x=CHAR_getInt(charaindex,CHAR_X);
    int     y=CHAR_getInt(charaindex,CHAR_Y);
    int     fl=CHAR_getInt(charaindex,CHAR_FLOOR);
    int     size=MAP_CHAR_DEFAULTSEESIZ;
    RECT    seekr,retr;
    seekr.x = x - (int)(size/2);
    seekr.y = y - (int)(size/2);
    seekr.width  = size;
    seekr.height = size;
#if 1
{
	int		tilesum, objsum, eventsum;
    stringdata = MAP_getChecksumFromRECT(fl,&seekr,&retr, &tilesum,&objsum,&eventsum);
    if( stringdata == NULL )
        return FALSE;

    lssproto_MC_send(fd,fl,
                    retr.x,              retr.y,
                    retr.x + retr.width, retr.y + retr.height,
                    tilesum,
                    objsum,
                    eventsum,
                    stringdata );
}
#else
    stringdata = MAP_getdataFromRECT(fl,&seekr,&retr);
    if( stringdata == NULL )
        return FALSE;

    lssproto_M_send(fd,fl,
                    retr.x,              retr.y,
                    retr.x + retr.width, retr.y + retr.height,
                    stringdata );
#endif
    return TRUE;
}


/*----------------------------------------
 * Ѩ���󷸡���ë  �£�
 * ¦��
 *  charaindex      int     ƽ�ҷ¼��̼������͵�
 * ߯Ի��
 *  ��      TRUE(1)
 *  ��      FALSE(0)
 ----------------------------------------*/
BOOL MAP_sendArroundChar(int charaindex)
{
    int fd;
    fd = getfdFromCharaIndex( charaindex );
    if( fd == -1 )return FALSE;

    return MAP_sendArroundCharNeedFD(fd, charaindex);
}
