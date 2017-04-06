#include <stdio.h>
#include <stdlib.h>
#include "lock.h"
#include "main.h"
#include "char.h"

LockNode **userlock;

void Lock_Init(void)
{
	int i;
 	userlock= (LockNode **) calloc( 1, sizeof(LockNode *) * 256);
	memset(userlock, 0, sizeof(userlock));
	for (i=0; i<256; i++) {
		userlock[i] = (LockNode *)calloc( 1,sizeof(LockNode));
		userlock[i]->use=0;
		userlock[i]->next=NULL;
		userlock[i]->prev=NULL;
		memset( userlock[i]->cdkey, 0, sizeof( userlock[i]->cdkey) );
		memset( userlock[i]->server, 0, sizeof( userlock[i]->server) );
	}
	log("��������ʼ��");
}

LockNode *Creat_newNodes( void)
{
	LockNode *newln=NULL;
	newln = ( LockNode *)calloc( 1, sizeof( LockNode) );
	if( newln == NULL ){
		log( "err Can't calloc:%d lock nodes !!\n", sizeof(LockNode));
		return 0;
	}
	newln->use=0;
	newln->next=NULL;
	memset( newln->cdkey, 0, sizeof( newln->cdkey) );
	memset( newln->server, 0, sizeof( newln->server) );
	return newln;
}

int InsertMemLock(int entry, char *cdkey, char *passwd, char *server, int process, char *deadline)
{
	int j;
	LockNode *ln = userlock[entry];
	log("������Ϸ:Ŀ¼:%x �˺�:%s ������:%s\n", entry, cdkey, server);
  
	while( (ln!=NULL) && (ln->use!=0)) ln=ln->next;

	if( ln == NULL ) {
		LockNode *fhead=NULL;
		LockNode *p = userlock[entry];
		log("Add more lock nodes.\n");
		while (p->next!=NULL) p=p->next;
		fhead = p;
		for( j=0; j<32; j++) {	// allocate more nodes
			if( (ln = Creat_newNodes() ) == NULL ) return 0;
			ln->prev=p;
			p->next=ln;
			p=ln;
		}
		while( (fhead!=NULL) && (fhead->use!=0)) fhead=fhead->next;
		ln = fhead;
	}

	if( ln->use !=0 ) return 0;
	ln->use = 1;
	strcpy( ln->cdkey, cdkey);
	strcpy( ln->server, server);
	ln->process = process;
	return 1;
}

int DeleteMemLock(int entry, char *cdkey, int *process)
{
	LockNode *ln = userlock[entry];

	log("ɾ���ڴ���Ϣ λ��=%x �˺�=%s ..", entry, cdkey);

	while (ln!=NULL) {
		if( ln->use != 0) {
			if( strcmp( ln->cdkey, cdkey ) == 0 ) break;
		}
		ln=ln->next;
	}
	if( ln != NULL ) {
		ln->use=0;
		memset( ln->cdkey, 0, sizeof( ln->cdkey) );
		memset( ln->server, 0, sizeof( ln->server) );
		*process = ln->process;
		log("ɾ���ɹ�\n");
		return 1;
	}
	log("ɾ��ʧ��!!\n");
	return 0;
}

void DeleteMemLockServer(char *sname)
{
	int i;
	LockNode *ln;
	for (i=0; i<256; i++) {
		ln = userlock[i];
		while (ln!=NULL) {
			if (ln->use != 0) {
				if( strcmp( ln->server, sname)==0) {
					ln->use=0;
				}
			}
			ln=ln->next;
		}
	}
}

int isMemLocked(int entry, char *cdkey)
{
	LockNode *ln = userlock[entry];
	while (ln!=NULL) {
		if (ln->use != 0) {
			if (strcmp(ln->cdkey, cdkey)==0) {
				if( !strcmp(ln->server, "��ϵ����"))
					log(" ��ϵ������ ");
				break;
			}
		}
		ln=ln->next;
	}
	if (ln!=NULL) return 1; else return 0;
}

int GetMemLockState(int entry, char *cdkey, char *result)
{
	LockNode *ln = userlock[entry];
  
	while (ln!=NULL) {
		if (ln->use != 0) {
			if (strcmp(ln->cdkey, cdkey)==0) {
				sprintf(result, "%s ���� %s ������.",cdkey, ln->server);
				return 1;
			}
		}
		ln=ln->next;
	}
	sprintf(result, "%s û�б���.", cdkey);
	return 0;
}

int GetMemLockServer(int entry, char *cdkey, char *result)
{
	LockNode *ln = userlock[entry];
  	while (ln!=NULL) {
		if (ln->use != 0) {
			if (strcmp(ln->cdkey, cdkey)==0) {
				strcpy(result, ln->server);
				return 1;
			}
		}
		ln=ln->next;
	}
	return 0;
}

int LockNode_getGname( int entries, char *id, char *gname)
{
	LockNode *ln = userlock[entries];
	while (ln!=NULL) {
		if (ln->use != 0) {
			if( !strcmp(ln->cdkey, id) ){
				sprintf( gname,"%s", ln->server );
				return 1;
			}
		}
		ln=ln->next;
	}
	return 0;
}
