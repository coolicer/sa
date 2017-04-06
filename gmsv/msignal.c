#include "version.h"
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include <stdio.h>

#include "buf.h"
#include "net.h"
#include "char.h"
#include "item.h"
#include "object.h"
#include "configfile.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "log.h"
#include "petmail.h"

/*------------------------------------------------------------
 *  ئ��
 ------------------------------------------------------------*/
static void endlsprotocol( void )
{
    lssproto_CleanupServer();
    saacproto_CleanupClient();
}

/*------------------------------------------------------------
 * ���ͻ���������ئ��ľ��ئ��ئ�з�����ëĸ������������
 *  ئ��
 ------------------------------------------------------------*/
static void allDataDump( void )
{
  closeAllLogFile();
	storeCharaData();
}

/*------------------------------------------------------------
 *  ئ��
 ------------------------------------------------------------*/
void shutdownProgram( void )
{
    printf("�ر�SAAC����:%d\n",close( acfd ));
    printf("�رհ󶨶˿�:%d\n",close( bindedfd ));
    memEnd();
}
#ifdef _GMSV_DEBUG
char *DebugMainFunction = NULL;
extern time_t initTime;
#endif
extern int player_online;
extern int player_maxonline;
char saacfunc[255]="";
void sigshutdown( int number )
{
		if( number == 0 ){
			print( "\nGMSV�����ر�\n" );
		}else{
			print( "\n=========�����Ƿ���������ԭ��=========\n");
	    print( "��׼��Ϣ: %d\n" , number  );
#ifdef _GMSV_DEBUG
			print( "�� �� ��: %s\n", DebugMainFunction );
#endif
			print( "��������: %d\n", player_online);
			print( "�������: %d\n", player_maxonline);
			print( "SAAC����: %s\n", saacfunc);
#ifdef _GMSV_DEBUG
	    {
	    	time_t new_t;
	    	int dd,hh,mm,ss;
	    	char buf[128];
	    	time(&new_t);
	    	if(initTime==0){
	    		print( "����ʱ��: ��δ��ʼ����\n" );
	    	}else{
		    	new_t-=initTime;
			
					dd=(int) new_t / 86400; new_t=new_t % 86400;
		   		hh=(int) new_t / 3600;  new_t=new_t % 3600;
		      mm=(int) new_t / 60;    new_t=new_t % 60;
		      ss=(int) new_t;
		      
					if (dd>0) {
		      	snprintf( buf, sizeof( buf ) , "�������������� %d �� %d Сʱ %d �� %d �롣",dd,hh,mm,ss);
		      } else if (hh>0) {
		      	snprintf( buf, sizeof( buf ) , "�������������� %d Сʱ %d �� %d �롣",hh,mm,ss);
		      } else {
		       	snprintf( buf, sizeof( buf ) , "�������������� %d �� %d �롣",mm,ss);
		      }
		      print( "����ʱ��: %s\n", buf );
	    	}
			}
#endif
			print( "=========�����Ƿ���������ԭ��=========\n");
		}
		remove( "gmsvlog.err2");
		rename( "gmsvlog.err1", "gmsvlog.err2" );
		rename( "gmsvlog.err", "gmsvlog.err1" );
		rename( "gmsvlog", "gmsvlog.err" );

    allDataDump();

    signal( SIGINT , SIG_IGN );
    signal( SIGQUIT, SIG_IGN );
    signal( SIGKILL, SIG_IGN );
    signal( SIGSEGV, SIG_IGN );
    signal( SIGPIPE, SIG_IGN );
    signal( SIGTERM, SIG_IGN );

    shutdownProgram();
    exit(number);
}



void signalset( void )
{
    // CoolFish: Test Signal 2001/10/26
    print("\n��ʼ��ȡ�ź�..\n");

		print("SIGINT:%d\n", SIGINT);
		print("SIGQUIT:%d\n", SIGQUIT);
		print("SIGKILL:%d\n", SIGKILL);
		print("SIGSEGV:%d\n", SIGSEGV);
		print("SIGPIPE:%d\n", SIGPIPE);
		print("SIGTERM:%d\n", SIGTERM);
    
    signal( SIGINT , sigshutdown );
    signal( SIGQUIT, sigshutdown );
    signal( SIGKILL, sigshutdown );
    signal( SIGSEGV, sigshutdown );
    signal( SIGPIPE, SIG_IGN );
    signal( SIGTERM, sigshutdown );
}
