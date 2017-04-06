#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "title.h"
#include "char.h"
#include "char_base.h"
#include "item.h"
#include "skill.h"
#include "buf.h"
#include "util.h"
#include "configfile.h"


/*====================��į  ====================*/
static int TITLE_IntCheck( int charaindex,int elem, int *data, int flg);
static int TITLE_WorkIntCheck( int charaindex,int elem, int *data, int flg);
static int TITLE_ItemCheck( int charaindex,int elem, int *data, int flg);
static int TITLE_ItemEquipCheck( int charaindex,int elem, int *data, int flg);
static int TITLE_SkillCheck( int charaindex,int elem, int *data, int flg);
static int TITLE_SexCheck( int charaindex,int elem, int *data, int flg);

typedef enum
{
	TITLE_FUNCTYPENONE,     /*  ����Ϸ����ئ��  */
	TITLE_FUNCTYPEUSERFUNC, /* definefunction ë���Ȼ�  �ë����
							 * ���Ѽ�¦�ѷ�
							 *  int     ƽ�ҷ��̼������͵�
							 *  buf       �߼���������
							 *  buflen  �����������뼰Ӯ��
							 */
	TITLE_USEFUNCTYPENUM
}TITLE_USEFUNCTYPE;

typedef struct tagTITLE_Table
{
	int                 index;      /* �ܷ�������ѯ��ë�����׻���
									 * ��  įƥ���Ȼ�addtitle�羮֧�� 
									 */
	char               name[32];
	TITLE_USEFUNCTYPE   functype;
	void                (*definefunction)(int,char* buf,int buflen);
}TITLE_Table;

/* �������ɷ¶�����
 * STR,TGH,MAXMP��ATK,DEF
 * LEVEL,CLASS,SKILL,ITEM,FIREREG,ICEREG,THUNDERREG
 * KANJILV,TALKCNT,WALKCNT,DEADCNT,LOGINCNT,BASEIMAGENUMBER
 * GOLD
*/
typedef struct tagTITLE_Compare {
	char    compare[8];;
} TITLE_COMPARE;

TITLE_COMPARE TITLE_compare[] = { {"<="}, {">="},{"<>"}, {">"},{"<"},{"="}};

typedef struct tagTITLE_PARAM
{
	int     element;
	int     (*checkfunc)( int charaindex,int elem, int *data, int flg );
	char    *paramname;
}TITLE_PARAM;

TITLE_PARAM TITLE_param[] = 
{
	/**** ����� ****/
	{ -1,                     TITLE_ItemCheck,     "ITEM"         },
	{ -1,                     TITLE_ItemEquipCheck,"EQUIPITEM"    },
	{ -1,                     NULL,                "EQUIPEVENT"   },
	/**** �����ƥ��  ����ئ��ƥ�ʷֽ��У� ****/
	{ -1,                     TITLE_SkillCheck,    "SKILL"        },
	{ CHAR_STR,               TITLE_IntCheck,      "STR"           },
	{ CHAR_TOUGH,             TITLE_IntCheck,      "TGH"           },
	{ CHAR_MAXMP,             TITLE_IntCheck,      "MAXMP"         },
	{ CHAR_WORKFIXSTR,        TITLE_WorkIntCheck,  "ATK"           },
	{ CHAR_WORKFIXTOUGH,      TITLE_WorkIntCheck,  "DEF"           },
	{ CHAR_LV,                TITLE_IntCheck,      "LEVEL"         },
	{ CHAR_TALKCOUNT,         TITLE_IntCheck,      "TALKCNT"       },
	{ CHAR_WALKCOUNT,         TITLE_IntCheck,      "WALKCNT"       },
	{ CHAR_DEADCOUNT,         TITLE_IntCheck,      "DEADCNT"       },
	{ CHAR_LOGINCOUNT,        TITLE_IntCheck,      "LOGINCNT"      },
	{ CHAR_BASEBASEIMAGENUMBER,TITLE_IntCheck,      "BASEIMAGENUMBER" },
	{ CHAR_GOLD,              TITLE_IntCheck,      "GOLD"          },
	{ -1,                     TITLE_SexCheck,      "SEX"           },
};

#define     TITLE_PARAMSIZE     20
typedef struct tagTITLE_configTable
{
	int     paramindex[20];         /* TITLE_param�߼���ٯ */
	int     param[20][TITLE_PARAMSIZE];              /*   �Ұ� */
	int     compareflg[20];         /* ><=��������¾� */
	int     title;
	BOOL    equipcheckflg;          /* 
									 *  ʧ��  ة�  ��ì����ƥ���𼰰׷º뼰  �Ȼ�����
									 *  �ּ��ƾ�������ئ�У�
									 */
}TITLE_CONFIGTABLE;

typedef struct tagTITLE_configbuf
{
	int     title;
	int     flg;
}TITLE_CONFIGBUF;

static TITLE_CONFIGTABLE    *TITLE_ConfigTable;
static TITLE_Table          *TITLE_table;
static TITLE_CONFIGBUF      *TITLE_configbuf;
static int                  TITLE_titlenum;
static int                  TITLE_titlecfgnum;

/*------------------------------------------------------------
 * index  į����TITLE_table����ٯë  ��
 ------------------------------------------------------------*/
int TITLE_getTitleIndex( int index)
{
	int i;
	if( index < 0 ) return -1;
	for( i = 0; i < TITLE_titlenum; i ++ ) {
		if( TITLE_table[i].index == index ) {
			return( i);
		}
	}
	return -1;
}

/*  �������뼰������    */
#define TITLESTRINGBUFSIZ   256
/*  �ͷ���ʧ������έ���µ�ƽ�ﷸ������  ٯ  ����������    */
static char    TITLE_statusStringBuffer[TITLESTRINGBUFSIZ];
/*------------------------------------------------------------
 * �ͷ���ʧ������έ���±�į��  ٯ  ë����
 * ¦��
 *  title       Title*      ��ƽ��
 *  charaindex  int         �𼰱�įë  �Ȼ�����ƽ�ҷ¼��̼������͵�
 * ߯Ի��
 *  char*
 ------------------------------------------------------------*/
char* TITLE_makeTitleStatusString( int charaindex,int havetitleindex )
{
	int     attach;
	int     index;
	/*  ����  �߼��̼������͵����շ�����ë��������  */
	index = CHAR_getCharHaveTitle( charaindex,havetitleindex );

	attach = TITLE_getTitleIndex( index);
	if( attach == -1 ) {
		TITLE_statusStringBuffer[0] = '\0';
		return TITLE_statusStringBuffer;
	}
	switch( TITLE_table[attach].functype ){
	case TITLE_FUNCTYPENONE:
		snprintf( TITLE_statusStringBuffer,
				  sizeof(TITLE_statusStringBuffer ),"%s" ,
				  TITLE_table[attach].name );
		break;
	
	case TITLE_FUNCTYPEUSERFUNC:
	{
		char    string[256]={""};
		void    (*function)(int,char* buf,int buflen);
		function = TITLE_table[attach].definefunction;
		if( function )
			function( charaindex,string,sizeof(string) );

		strcpysafe( TITLE_statusStringBuffer,
					sizeof(TITLE_statusStringBuffer ),string );
	}
	break;
	default:
		TITLE_statusStringBuffer[0] = '\0';
		return TITLE_statusStringBuffer;
		break;
	}
	return TITLE_statusStringBuffer;
}

/*------------------------------------------------------------
 * ئ�б�į��  ٯ  ������ë߯��
 * ¦��
 *  ئ��
 * ߯Ի��
 *  char*
 ------------------------------------------------------------*/
char* TITLE_makeSkillFalseString( void )
{
	TITLE_statusStringBuffer[0]= '\0';
	return TITLE_statusStringBuffer;
}


/*------------------------------------------------------------
 * ϶�ý�ľ��  į����įëܰ�����£ۺֽ�ئ�Ȼ������գ�ܰ����ئ��
 * ¦��
 *  charaindex      int     ƽ�ҷ��̼������͵�
 *  titleindex      int     ��į�̼������͵�
 * ߯Ի��
 *  ܰ������index       
 *  ܰ����ئ������      FALSE(0)
 ------------------------------------------------------------*/
BOOL TITLE_addtitle( int charaindex, int titleindex )
{
	int i;
	int firstfindempty=-1;

	if( CHAR_CHECKINDEX(charaindex)  == FALSE )return FALSE;
	/*if( TITLE_CHECKTABLEINDEX(titleindex) == FALSE )return FALSE;*/
	if( TITLE_getTitleIndex( titleindex) == -1 ) return FALSE;

	for( i=0 ; i < CHAR_TITLEMAXHAVE ; i++ ){
		if( CHAR_getCharHaveTitle( charaindex,i ) == titleindex )
			/*  ��ƥ��  �Ȼ����¾���������ئ��  */
			return FALSE;
		if( firstfindempty == -1
			&& CHAR_getCharHaveTitle(charaindex,i) == -1 ){
			firstfindempty = i;
		}
	}
	return CHAR_setCharHaveTitle( charaindex,firstfindempty,titleindex );
}


/*------------------------------------------------------------
 * ϶�ý�ľ��  į����į��ؤ�����մ������£�ئ��������������ئ�У�
 *   ����ؤ��������  ���ʣ�
 * ¦��
 *  charaindex      int     ƽ�ҷ��̼������͵�
 *  titleindex      int     ��į�̼������͵�
 * ߯Ի��
 *  ��������            TRUE(1)
 *  ������ئ������      FALSE(0)
 ------------------------------------------------------------*/
BOOL TITLE_deltitle( int charaindex, int titleindex )
{
	int i;
	BOOL    del=FALSE;
	int     index;
	if( CHAR_CHECKINDEX(charaindex)  == FALSE )return FALSE;
	/*if( TITLE_CHECKTABLEINDEX(titleindex) == FALSE )return FALSE;*/
	index = TITLE_getTitleIndex( titleindex);
	if(  index == -1 ) return FALSE;

	for( i=0 ; i < CHAR_TITLEMAXHAVE ; i++ )
		if( CHAR_getCharHaveTitle( charaindex,i ) == titleindex ){
			/*  ���л����Ȼ�����֧��ئ���ɣ���ľ��ئ�Ʊ�����    */
			if( CHAR_getInt(charaindex, CHAR_INDEXOFEQTITLE) == i ){
				CHAR_setInt(charaindex, CHAR_INDEXOFEQTITLE, -1 );
			}
			/*    �Ȼ����¾�������  */
			CHAR_setCharHaveTitle( charaindex,i,-1);
			
			del = TRUE;
		}

	return del;
}
/*------------------------------------------------------------
 * ��į�������ë���£�
 * ¦��
 *  filename        char*       ɬ�ð����̻�  
 * ߯Ի��
 *  ��      TRUE(1)
 *  ��      FALSE(0)
 *------------------------------------------------------------*/
BOOL TITLE_initTitleName( char* filename )
{
	FILE*   f;
	char    line[256];
	int     linenum=0;
	int     title_readlen=0;

	f = fopen(filename,"r");
	if( f == NULL ){
		errorprint;
		return FALSE;
	}

	TITLE_titlenum=0;

	/*  ����  ��ئ�滥�ϵ�ؤ�¾�������Ʃ����    */
	while( fgets( line, sizeof( line ), f ) ){
		linenum ++;
		if( line[0] == '#' )continue;        /* comment */
		if( line[0] == '\n' )continue;       /* none    */
		chomp( line );

		TITLE_titlenum++;
	}

	if( fseek( f, 0, SEEK_SET ) == -1 ){
		fprint( "Ѱ�Ҵ���\n" );
		fclose(f);
		return FALSE;
	}

	TITLE_table = allocateMemory( sizeof(struct tagTITLE_Table)
								   * TITLE_titlenum );
	if( TITLE_table == NULL ){
		fprint( "�޷������ڴ� %d\n" ,
				sizeof(TITLE_table)*TITLE_titlenum);
		fclose( f );
		return FALSE;
	}
	/* ����� */
{
	int     i;
	for( i = 0; i < TITLE_titlenum; i ++ ) {
		TITLE_table[i].index = -1;
		TITLE_table[i].name[0] = '\0';
		TITLE_table[i].functype = TITLE_FUNCTYPENONE;
		TITLE_table[i].definefunction = NULL;
	}
	
}

	/*  ����  ��  ��    */
	linenum = 0;
	while( fgets( line, sizeof( line ), f ) ){
		linenum ++;
		if( line[0] == '#' )continue;        /* comment */
		if( line[0] == '\n' )continue;       /* none    */
		chomp( line );

		/*  ��ë��������    */
		/*  ���� tab ë " " ��  �徧����    */
		replaceString( line, '\t' , ' ' );
		/* ��  ����ʸ����ë���£�*/
{
		int     i;
		char    buf[256];
		for( i = 0; i < strlen( line); i ++) {
			if( line[i] != ' ' ) {
				break;
			}
			strcpy( buf, &line[i]);
		}
		if( i != 0 ) {
			strcpy( line, buf);
		}
}
{
		char    token[256];
		int     ret;

		/*  ���繴���������ͼ�ëέ��    */
		ret = getStringFromIndexWithDelim( line,",",1,token,
										   sizeof(token));
		if( ret==FALSE ){
			fprint("�ļ��﷨����:%s ��%d��\n",filename,linenum);
			continue;
		}
		TITLE_table[title_readlen].index = atoi(token);

		/*  2�����������ͼ�ëέ��    */
		ret = getStringFromIndexWithDelim( line,",",2,token,
										   sizeof(token));
		if( ret==FALSE ){
			fprint("�ļ��﷨����:%s ��%d��\n",filename,linenum);
			continue;
		}
		if( strlen( token) > sizeof( TITLE_table[title_readlen].name)-1) {
			fprint("����! ͷ�����ƽ����ļ�:%s ��%d��\n",
					filename, linenum);
		}
		strcpysafe( TITLE_table[title_readlen].name, 
					sizeof( TITLE_table[title_readlen].name),
					token);

		title_readlen ++;
}
	}
	fclose(f);

	TITLE_titlenum = title_readlen;

	print( "��Чͷ���������� %d...", TITLE_titlenum );

#ifdef DEBUG

	{
		int i;
		for( i=0; i <TITLE_titlenum ; i++ )
			print( "ͷ������[%d] ����[%s] \n",
				   TITLE_table[i].index,
				   TITLE_table[i].name);
	}
#endif
	return TRUE;
}
/*------------------------------------------------------------
 * ��į���������ë���£�
 * ¦��
 *  filename        char*       ɬ�ð����̻�  
 * ߯Ի��
 *  ��      TRUE(1)
 *  ��      FALSE(0)
 *------------------------------------------------------------*/
BOOL TITLE_reinitTitleName( void)
{
	freeMemory( TITLE_table);
	return(TITLE_initTitleName( getTitleNamefile()));
}
/*------------------------------------------------------------
 * ��įɬ�ü������ë���£�
 * ¦��
 *  filename        char*       ɬ�ð����̻�  
 * ߯Ի��
 *  ��      TRUE(1)
 *  ��      FALSE(0)
 *------------------------------------------------------------*/
static void TITLE_initTitleData( int array)
{
	int j,k;
	for( j = 0; j < arraysizeof( TITLE_ConfigTable[array].paramindex); j ++) {
		for( k = 0; k < TITLE_PARAMSIZE; k++) {
			TITLE_ConfigTable[array].param[j][k] = -1;
		}
		TITLE_ConfigTable[array].paramindex[j] = -1;
		TITLE_ConfigTable[array].compareflg[j] = -1;
	}
	TITLE_ConfigTable[array].title = -1;
	TITLE_ConfigTable[array].equipcheckflg = FALSE;
	
}
/*------------------------------------------------------------
 *   �ѵ�ɬ��ë���继�������ƻ�֧�£�  �꼰�ֳ��ƥ���£�
 *------------------------------------------------------------*/
static int TITLE_getConfigOneLine( FILE *fp, char *line, int linelen)
{
	char    buf[1024];
	int     startflg = FALSE;
	int     linenum=0;
	line[0] = '\0';
	
	while( fgets( buf, sizeof( buf ), fp )){
		linenum ++;
		if( buf[0] == '#' )continue;        /* comment */
		if( buf[0] == '\n' )continue;       /* none    */
		/*  ��ë��������    */
		/*  ���� tab ë " " ��  �徧����    */
		replaceString( buf, '\t' , ' ' );
		/* ��ʸ�������� */
		deleteCharFromString( buf, " ");

		if( buf[0] == '{' ) {
			if( startflg == TRUE ) {
				print( "titleconfig:����û�йرա�{��ȴ������: %d \n",linenum);
				/* } */
				return -1;
			}
			startflg = TRUE;
		}
		else if( buf[0] == '}' ) {
			if( startflg == FALSE) {
				print( "titleconfig:����û�йرա�{��ȴ������: %d \n",linenum);
				/* } */
				return -1;
			}
			return 1;
		}
		else {
			/* "{"ƥ�ȱ�����Ȼ������� }*/
			if( startflg == TRUE ) {
				if( strlen( line) != 0 ) {
					if( line[strlen(line) -1] != ',' ) {
						strcatsafe( line, linelen, ",");
					}
				}
				/* �������继������*/
				chompex( buf );
				strcatsafe( line,linelen,  buf);
			}
			/*   ���ֻ�"{"ƥ��Ԫ���Ȼ�ئ�������������������ƥ߯�� }*/
			else {
				chompex( buf );
				strcatsafe( line,linelen,  buf);
				return 1;
			}
		}
	}
	/* �����������ɵ��Ƿ�EOF   ѱ��Ԫ��ئ��������   */
	return 0;
}
static int TITLE_getParamData( int readarray, int array,char *src)
{
	int cnt;
	int ret;
	char    wk[64];
	
	
	for( cnt = 1; ; cnt ++ ) {
		ret = getStringFromIndexWithDelim( src,"|",cnt,wk, sizeof(wk));
		if( ret ) {
			TITLE_ConfigTable[readarray].param[array][cnt-1] = atoi(wk);
		}
		else {
			break;
		}
	}
	if( cnt == 1 )  {
		return FALSE;
	}
	return TRUE;
}

/*------------------------------------------------------------
 * ��įɬ�ü������ë���£�
 * ¦��
 *  filename        char*       ɬ�ð����̻�  
 * ߯Ի��
 *  ��      TRUE(1)
 *  ��      FALSE(0)
 *------------------------------------------------------------*/
BOOL TITLE_initTitleConfig( char* filename )
{
	FILE*   f;
	char    line[1024];
	int     linenum=0;
	int     titlecfg_readlen=0;

	f = fopen(filename,"r");
	if( f == NULL ){
		errorprint;
		return FALSE;
	}

	TITLE_titlecfgnum=0;

	/*  ����  ��ئ�滥�ϵ�ؤ�¾�������Ʃ����    */
	/*while( fgets( line, sizeof( line ), f ) ){}*/
	while( 1 ) {
		int rc;
		rc = TITLE_getConfigOneLine( f, line, sizeof(line));
		if( rc == 0 ) break;
		if( rc == -1 ) continue;

		TITLE_titlecfgnum++;
	}

	if( fseek( f, 0, SEEK_SET ) == -1 ){
		fprint( "Seek Error\n" );
		fclose(f);
		return FALSE;
	}

	TITLE_ConfigTable = allocateMemory( sizeof(struct tagTITLE_configTable)
								   * TITLE_titlecfgnum );
	if( TITLE_ConfigTable == NULL ){
		fprint( "Can't allocate Memory %d\n" ,
				sizeof(TITLE_ConfigTable)*TITLE_titlecfgnum);
		fclose( f );
		return FALSE;
	}
	/* ����� */
	{
		int     i;
		for( i = 0; i < TITLE_titlecfgnum; i ++ ) {
			TITLE_initTitleData( i);
		}
	}
	TITLE_configbuf = allocateMemory( sizeof(TITLE_CONFIGBUF) * TITLE_titlecfgnum );
	if( TITLE_configbuf == NULL ){
		fprint( "Can't allocate Memory %d\n" ,
				sizeof(TITLE_CONFIGBUF)*TITLE_titlecfgnum);
		fclose( f );
		return FALSE;
	}
	
	/*  ����  ��  ��    */
	linenum = 0;
	/*while( fgets( line, sizeof( line ), f ) ){}*/
	while( 1) {
		int rc;
		rc = TITLE_getConfigOneLine( f, line, sizeof(line));
		if( rc == 0 ) break;
		if( rc == -1 ) continue;

		linenum ++;

{
		char    token[256];
		int     ret;
		int     i , j ;
		int     comppos;
		BOOL    errflg =FALSE;
		for( i = 1; ; i ++ ){
			/*  �����ͼ�ëέ��    */
			ret = getStringFromIndexWithDelim( line,",",i,token,
											   sizeof(token));
			if( ret == FALSE ){
				break;
			}
			/*     ٯ��  �� */
			for( j = 0; j < strlen( token); j ++ ) {
				token[j] = toupper( token[j]);
			}
			/* ��į϶��ëƩ���� */
			if( strncmp( "TITLE", token, 5) == 0 ) {
				char    buf[64];
				ret = getStringFromIndexWithDelim( token,"=",2,buf,
												   sizeof(buf));
				if( ret == FALSE) {
					fprint("�ļ��﷨����:%s ��%d��\n",filename,linenum);
					TITLE_initTitleData( titlecfg_readlen);
					errflg = TRUE;
					break;
				}
				TITLE_ConfigTable[titlecfg_readlen].title = atoi( buf);
			}
			else {
				/* �ɷ¶�����϶��ë  �� */
				for( j = 0; j < arraysizeof( TITLE_param); j ++ ) {
					if( strncmp( TITLE_param[j].paramname, 
								token, 
								strlen( TITLE_param[j].paramname)) 
						== 0 )
					{
						TITLE_ConfigTable[titlecfg_readlen].paramindex[i-1] = j;
						/* ��  ��ì����ƥ��įë�����������°׷º�ë  ���£� */
						if( j == 0 || j == 1 || j == 2) {
							TITLE_ConfigTable[titlecfg_readlen].equipcheckflg = TRUE;
						}
						break;
					}
				}
				/* �ɷ¶�������  ئ��ë϶�ý�ľ�� */
				if( j == arraysizeof( TITLE_param) ) {
					fprint("�ļ��﷨����:%s ��%d��\n",
							filename,linenum);
					TITLE_initTitleData( titlecfg_readlen);
					errflg = TRUE;
					break;
				}
				/* �į�����į��������¾�Ʃ���� */
				comppos = charInclude( token, "<>=");
				if( comppos == -1 ) {
					fprint("�ļ��﷨����:%s ��%d��\n",filename,linenum);
					TITLE_initTitleData( titlecfg_readlen);
					errflg = TRUE;
					break;;
				}
				/* �į�����į���Ṵ����  ������ */
				if( charInclude( &token[comppos+1], "<>=") != -1 ) {
					/* ����ئ  ʤ��ë  �� */
					for( j = 0; j < 3;  j ++ ) {
						if( memcmp( TITLE_compare[j].compare, &token[comppos], 2 ) == 0 ) {
							break;
						}
					}
					if( j == 3 ) {
						fprint("�ļ��﷨����:%s ��%d��\n",filename,linenum);
						TITLE_initTitleData( titlecfg_readlen);
						errflg = TRUE;
						break;
					}
					else {
						ret = TITLE_getParamData( titlecfg_readlen,i-1,&token[comppos+2]);
						if( !ret ) {
							fprint("�ļ��﷨����:%s ��%d��\n",filename,linenum);
							TITLE_initTitleData( titlecfg_readlen);
							errflg = TRUE;
							break;
						}

						/*TITLE_ConfigTable[titlecfg_readlen].param[i-1] 
								= atoi( &token[comppos+2]);*/
						TITLE_ConfigTable[titlecfg_readlen].compareflg[i-1] = j;
					}
				}
				else {
					ret = TITLE_getParamData( titlecfg_readlen,i-1,&token[comppos+1]);
					if( !ret ) {
						fprint("�ļ��﷨����:%s ��%d��\n",filename,linenum);
						TITLE_initTitleData( titlecfg_readlen);
						errflg = TRUE;
						break;
					}
					/*TITLE_ConfigTable[titlecfg_readlen].param[i-1] 
							= atoi( &token[comppos+1]);*/
					/* ����ئ  ʤ��ë  �� */
					for( j = 3; j < 6;  j ++ ) {
						if( memcmp( TITLE_compare[j].compare, &token[comppos], 1 ) == 0 ) {
							TITLE_ConfigTable[titlecfg_readlen].compareflg[i-1] = j;
							break;
						}
					}
				}
			}
		}
		/* ��į  ϶�û�  ������ or ئ�Ͼ����ռ��޷¡�*/
		if( errflg || TITLE_ConfigTable[titlecfg_readlen].title == -1 ) {
			fprint("�ļ��﷨����:%s ��%d��\n",filename,linenum);
			TITLE_initTitleData( titlecfg_readlen);
		}
		else {
			titlecfg_readlen ++;
		}
}
	}
	fclose(f);

	TITLE_titlecfgnum = titlecfg_readlen;

	print( "��Чͷ���������� %d...", TITLE_titlecfgnum );

	return TRUE;
}
/*------------------------------------------------------------
 * ��įɬ�ñ�������Ʃ�ͻ���įëܸ���£�
 * ¦��
 *  charaindex        int   ƽ�ҷ��̼������͵�
 *  mode              BOOL  TRUE:item=���ּ�����Ʃ���� FALSE:��  
 * ߯Ի��
 *  TRUE: ��į��  ��  Ի��
 *  FALSE:����  �����ף�
 *------------------------------------------------------------*/
static BOOL TITLE_TitleCheck_Main( int charaindex, BOOL mode, int *addcnt, int *delcnt)
{
	int     i, j,k, ret;
	
	/* ����� */
{
	int     i;
	for( i = 0; i < TITLE_titlecfgnum && TITLE_configbuf[i].title != -1; i ++ ) {
		TITLE_configbuf[i].title = -1;
		TITLE_configbuf[i].flg = 0;
	}
}
	*addcnt = 0;
	*delcnt = 0;
	
	for( i = 0; i < TITLE_titlecfgnum; i ++ ) {
		int rc = TRUE;
		int title = TITLE_ConfigTable[i].title;
		if( mode == TRUE ) {
			if( TITLE_ConfigTable[i].equipcheckflg != TRUE ) {
				continue;
			}
		}
		for( j = 0; 
			j < arraysizeof( TITLE_ConfigTable[i].param) && 
			TITLE_ConfigTable[i].param[j][0] != -1 ; 
			j ++ ) 
		{
			int index = TITLE_ConfigTable[i].paramindex[j];
			if( TITLE_param[index].checkfunc != NULL ) {
				rc = TITLE_param[index].checkfunc( 
						charaindex,
						TITLE_param[index].element, 
						TITLE_ConfigTable[i].param[j],
						TITLE_ConfigTable[i].compareflg[j]
						);
				if( rc != TRUE ) break;
			}
		}
		/*   ��������  Ԫ��į���Ի񲻯��ľ�������ݼ��б��
		 * �����������������������������add,deltile����
		 */
		for( k = 0; k < TITLE_titlecfgnum; k ++ ) {
			if( TITLE_configbuf[k].title == title ) { 
				if( rc ) {
					TITLE_configbuf[k].flg = 1;
				}
				break;
			}
			else if( TITLE_configbuf[k].title == -1 ) {
				TITLE_configbuf[k].title = title;
				TITLE_configbuf[k].flg = rc ? 1: -1;
				break;
			}
		}
	}
	ret = FALSE;
	for( i = 0; i < TITLE_titlecfgnum && TITLE_configbuf[i].title != -1; i ++ ) {
		if( TITLE_configbuf[i].flg == -1 ) {
			*delcnt += TITLE_deltitle( charaindex, TITLE_configbuf[i].title);
		}
		else {
			*addcnt += TITLE_addtitle( charaindex, TITLE_configbuf[i].title) ? 1:0;
		}
	}
	if( *delcnt > 0 || *addcnt > 0) {
		ret = TRUE;
	}
	return ret;
	
}
/*------------------------------------------------------------
 * ��įɬ�ñ�������Ʃ�ͻ���įëܸ���£�
 * ¦��
 *  charaindex        int   ƽ�ҷ��̼������͵�
 *  mode              BOOL  TRUE:item=���ּ�����Ʃ���� FALSE:��  
 * ߯Ի��
 *  TRUE: ��į��  ��  Ի��
 *  FALSE:����  �����ף�
 *------------------------------------------------------------*/
BOOL TITLE_TitleCheck( int charaindex, BOOL mode)
{
#define     TITLE_MSGUNIT1      "TSU"
#define     TITLE_MSGUNIT2      "KO"
	int     addcnt,delcnt;
	BOOL    rc;
	char    msgbuf[64];
	rc = TITLE_TitleCheck_Main( charaindex, mode, &addcnt,&delcnt);
	if( rc ) {
		if( delcnt > 0 ) {
			snprintf( msgbuf, sizeof( msgbuf), 
						"ʧȥ%d%s �ƺţ�", delcnt,
						delcnt < 10 ?  TITLE_MSGUNIT1:TITLE_MSGUNIT2);
			CHAR_talkToCli( charaindex, -1, msgbuf,  CHAR_COLORYELLOW);
		}
		if( addcnt > 0 ) {
			snprintf( msgbuf, sizeof( msgbuf), 
						"���%d%s �ƺţ�", addcnt,
						addcnt < 10 ?  TITLE_MSGUNIT1:TITLE_MSGUNIT2);
			CHAR_talkToCli( charaindex, -1, msgbuf,  CHAR_COLORYELLOW);
		}
	}
	return rc;
}
/*------------------------------------------------------------
 * ��įɬ�ñ�������Ʃ�ͻ���įëܸ���£�
 * ¦��
 *  charaindex        int   ƽ�ҷ��̼������͵�
 *  mode              BOOL  TRUE:item=���ּ�����Ʃ���� FALSE:��  
 * ߯Ի��
 *  TRUE: ��į��  ��  Ի��
 *  FALSE:����  �����ף�
 *------------------------------------------------------------*/
BOOL TITLE_TitleCheck_Nomsg( int charaindex, BOOL mode, int *addcnt, int *delcnt)
{
	return( TITLE_TitleCheck_Main( charaindex, mode, addcnt,delcnt));
}


static int TITLE_IntCheck( int charaindex, int elem, int *data, int flg)
{
	int     rc = FALSE;
	int     i;
	for( i = 0; i < TITLE_PARAMSIZE && *(data+i) != -1; i ++ ) {
		switch( flg) {
		  case 0:       /* "<=" */
			if( CHAR_getInt( charaindex, elem) <= *(data+i) ) rc = TRUE;
			break;
		  case 1:       /* ">=" */
			if( CHAR_getInt( charaindex, elem) >= *(data+i) ) rc = TRUE;
			break;
		  case 2:       /* "<>" */
			if( CHAR_getInt( charaindex, elem) != *(data+i) ) rc = TRUE;
			break;
		  case 3:       /* ">" */
			if( CHAR_getInt( charaindex, elem) > *(data+i) ) rc = TRUE;
			break;
		  case 4:       /* "<"  */
			if( CHAR_getInt( charaindex, elem) < *(data+i) ) rc = TRUE;
			break;
		  case 5:       /* "=" */
			if( CHAR_getInt( charaindex, elem) == *(data+i) ) rc = TRUE;
			break;
		  default:
		  rc= FALSE;
		  break;
		}
		if( rc) break;
	}
	return rc;
}
static int TITLE_WorkIntCheck( int charaindex,int elem, int *data, int flg)
{
	int     rc = FALSE;
	int i;
	for( i = 0; i < TITLE_PARAMSIZE && *(data+i) != -1; i ++ ) {
		switch( flg) {
		  case 0:       /* "<=" */
			if( CHAR_getWorkInt( charaindex, elem) <= *(data+i) ) rc = TRUE;
			break;
		  case 1:       /* ">=" */
			if( CHAR_getWorkInt( charaindex, elem) >= *(data+i) ) rc = TRUE;
			break;
		  case 2:       /* "<>" */
			if( CHAR_getWorkInt( charaindex, elem) != *(data+i) ) rc = TRUE;
			break;
		  case 3:       /* ">" */
			if( CHAR_getWorkInt( charaindex, elem) > *(data +i)) rc = TRUE;
			break;
		  case 4:       /* "<"  */
			if( CHAR_getWorkInt( charaindex, elem) < *(data+i) ) rc = TRUE;
			break;
		  case 5:       /* "=" */
			if( CHAR_getWorkInt( charaindex, elem) == *(data+i) ) rc = TRUE;
			break;
		  default:
		  rc= FALSE;
		  break;
		}
		if( rc ) break;
	}
	return rc;
}
/* --------------------------------------
 * ϶�ý�ľ��data��  �Ȼ�����ʧ��  ةë  ʤ���£�
 * -------------------------------------*/
static int TITLE_ItemCheckMain( int charaindex, int itemhaveindex, int *data, int flg)
{
	int i,j;
	int rc = FALSE;
	
	if( flg == 2 ) rc = TRUE;

	for( j = 0; j < TITLE_PARAMSIZE && *(data+j) != -1; j ++ ) {
		for( i =0; i < itemhaveindex; i ++ ) {
			int     itemindex = CHAR_getItemIndex(charaindex,i);
			if( ITEM_CHECKINDEX(itemindex) ) {
				switch( flg) {
				  case 0:       /* "<=" */
					if( ITEM_getInt( itemindex, ITEM_ID) <= *(data+j) ) rc = TRUE;
					break;
				  case 1:       /* ">=" */
					if( ITEM_getInt( itemindex, ITEM_ID) >= *(data+j) ) rc = TRUE;
					break;
				  case 3:       /* ">" */
					if( ITEM_getInt( itemindex, ITEM_ID) > *(data +j)) rc = TRUE;
					break;
				  case 4:       /* "<"  */
					if( ITEM_getInt( itemindex, ITEM_ID) < *(data +j)) rc = TRUE;
					break;
				  case 5:       /* "=" */
					if( ITEM_getInt( itemindex, ITEM_ID) == *(data +j)) rc = TRUE;
					break;
				  /* ��ľ����    ���У�
				   * ʧ��  ة��  ëέ����ľë  �Ȼ���ئ��������ީ�����£�
				   * ���ƥ��  �Ȼ�������ë�����ƻ���������
				   */
				  case 2:       /* "<>" */
					if( ITEM_getInt( itemindex, ITEM_ID) == *(data +j)) rc = FALSE;
					break;
				  default:
				  rc= FALSE;
				  break;
				}
				if( rc && flg != 2) break;
			}
		}
		if( rc) break;
	}
	return rc;
}


static int TITLE_ItemCheck( int charaindex,int elem, int *data, int flg)
{
	return( TITLE_ItemCheckMain( charaindex, CHAR_MAXITEMHAVE,data,flg));
}

static int TITLE_ItemEquipCheck( int charaindex,int elem, int *data, int flg)
{
	return( TITLE_ItemCheckMain( charaindex, CHAR_EQUIPPLACENUM,data,flg));
}

static int TITLE_SkillCheck( int charaindex,int elem, int *data, int flg)
{
	int i;
	int rc = FALSE;
	CHAR_HaveSkill* hskill;

	for( i = 0 ; i < CHAR_SKILLMAXHAVE ; i ++ ){
		hskill = CHAR_getCharHaveSkill( charaindex, i );
		if( hskill != NULL && hskill->use == TRUE ) {

			if( *(data + 1) != -2 ) {
				if( SKILL_getInt( &hskill->skill, SKILL_IDENTITY) == *data) {
					rc = TRUE;
					break;
				}
			}

			else {
				if( SKILL_getInt( &hskill->skill, SKILL_IDENTITY) == *data) {
					rc = FALSE;
					break;
				}
				else {
					rc = TRUE;
				}
			}
		}
	}
	if( rc ) {

		if( *(data+1) != -1 && *(data+1) != -2) {
			rc = FALSE;
			switch( flg) {
			  case 0:       /* "<=" */
				if( SKILL_getInt( &hskill->skill, SKILL_LEVEL) <= *(data+1) ) rc = TRUE;
				break;
			  case 1:       /* ">=" */
				if( SKILL_getInt( &hskill->skill, SKILL_LEVEL) >= *(data+1) ) rc = TRUE;
				break;
			  case 2:       /* "<>" */
				if( SKILL_getInt( &hskill->skill, SKILL_LEVEL) != *(data+1) ) rc = TRUE;
				break;
			  case 3:       /* ">" */
				if( SKILL_getInt( &hskill->skill, SKILL_LEVEL) > *(data +1)) rc = TRUE;
				break;
			  case 4:       /* "<"  */
				if( SKILL_getInt( &hskill->skill, SKILL_LEVEL) < *(data+1) ) rc = TRUE;
				break;
			  case 5:       /* "=" */
				if( SKILL_getInt( &hskill->skill, SKILL_LEVEL) == *(data+1) ) rc = TRUE;
				break;
			  default:
			  rc= FALSE;
			  break;
			}
		}
	}
	return rc;
}
/* --------------------------------------
 * ��ƽ�ҷ¾���ƽ�ҷ¾������������£�
 * -------------------------------------*/
static int TITLE_SexCheck( int charaindex,int elem, int *data, int flg)
{
	int     i,j;
	struct {
		int sex[25];
	}s_sex[] =
	{ { { 30008,30009,30010,30011,30012,30013,30014,30015,
		30020,30021,30022,30023,30024,30025,30026,30027,
		30028,30029,30030,30031,30032,30033,30034,30035,-1}
	  },
	  { {30000,30001,30002,30003,30004,30005,30006,30007,
		30016,30017,30018,30019,30036,30037,30038,30039,
		-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  -1}
	  }
	};
	for( i = 0; i < 2; i ++ ) {
		for( j = 0; s_sex[i].sex[j] != -1 ; j ++ ) {
			if( CHAR_getInt( charaindex, CHAR_BASEBASEIMAGENUMBER ) 
				== s_sex[i].sex[j] ) 
			{
				if( i == *data ) return TRUE;
				else            return FALSE;
			}
		}
	}
	return FALSE;
}


