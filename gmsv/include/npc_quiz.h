#ifndef __NPC_QUIZ_H__
#define __NPC_QUIZ_H__

typedef struct NPC_Quiz{

	int 	no;				//  ����  į
	int 	type;			//  ����������  �ء�ة    �����̡�֭)	
	int		level;  		//  ������ì��
	int 	answertype;		//��������̫����(��  ����  ��ǩ��Ѩ����  
	int 	answerNo;		//  ��������
	char	question[512]; 	//  ��
	char	select1[128]; 	//��  ��1
	char	select2[128]; 	//��  ��2
	char	select3[128]; 	//��  ��3

}NPC_QUIZ;


void NPC_QuizTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_QuizInit( int meindex );
void NPC_QuizWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

BOOL QUIZ_initQuiz( char *filename);


#endif 

/*__NPC_QUIZ_H__*/
