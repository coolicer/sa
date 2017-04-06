#ifndef __MAGIC_BASE_H__
#define __MAGIC_BASE_H__

#include "util.h"

typedef enum
{
	MAGIC_FIELD_ALL,			/* ���ͻ�������ƥ������ */
	MAGIC_FIELD_BATTLE,				/* ��    ���� */
	MAGIC_FIELD_MAP,				/* ɧ��Ѩ���������� */

}MAGIC_FIELDTYPE;

typedef enum
{
	MAGIC_TARGET_MYSELF,		/* ���м��� */
	MAGIC_TARGET_OTHER,			/* ְ����  ����ֳ��) */
	MAGIC_TARGET_ALLMYSIDE,		/*     ��   */
	MAGIC_TARGET_ALLOTHERSIDE,	/* ���촡��   */
	MAGIC_TARGET_ALL,			/* �廯 */
	MAGIC_TARGET_NONE,			/* ���ּ�  ��  ئ�У�  ��֧�׻����� */
	MAGIC_TARGET_OTHERWITHOUTMYSELF,/* ְ����  ����ֳ��ئ��) */
	MAGIC_TARGET_WITHOUTMYSELFANDPET,  /* ������ʸ������½ */
	MAGIC_TARGET_WHOLEOTHERSIDE,/*     ����������   */

#ifdef _ATTACK_MAGIC

        MAGIC_TARGET_SINGLE,            // ��Եз���ĳһ��
        MAGIC_TARGET_ONE_ROW,           // ��Եз���ĳһ��
        MAGIC_TARGET_ALL_ROWS,          // ��Եз���������

#endif
}MAGIC_TARGETTYPE;

typedef enum
{
	MAGIC_ID,					/* ��    į */
	MAGIC_FIELD,				/* ���������� */
	MAGIC_TARGET,				/* ���� */
	MAGIC_TARGET_DEADFLG,		/* ���Ϸַ��ָ��ޱ�ֳ���¾� */
#ifdef _ATTACK_MAGIC
  MAGIC_IDX ,
#endif
	MAGIC_DATAINTNUM,
}MAGIC_DATAINT;

typedef enum
{
	MAGIC_NAME,					/* ����   */
	MAGIC_COMMENT,				/* �춪����*/
	MAGIC_FUNCNAME,				/* ����   */
	MAGIC_OPTION,				/* ��������� */
	MAGIC_DATACHARNUM,
}MAGIC_DATACHAR;

typedef struct tagMagic
{
	int			data[MAGIC_DATAINTNUM];
	STRING64	string[MAGIC_DATACHARNUM];

}Magic;

#ifdef _ATTACK_MAGIC

typedef struct tagAttMagic
{
  unsigned int  uiSpriteNum;// ��������Spr_x.bin�ı��
  unsigned int  uiAttackType;// �����ķ�ʽ�����ˣ�����( ���� ) ,  ����( ���� ) , ����( ͬʱ ) , ȫ��( ���� ) , ȫ��( ͬʱ )
  unsigned int  uiSliceTime;// ��������ʱ��ʱ���
  unsigned int  uiShowType;             // ��ʾ��λ�÷�ʽ�����롢ָ��
  int           siSx;                   // ��ʾ��λ�� - X��
  int           siSy;                   // ��ʾ��λ�� - Y��
  unsigned int  uiShowBehindChar;       // ��ʾ�������ǰ�����·�
  unsigned int  uiShakeScreen;          // �Ƿ��𶯻���
  unsigned int  uiShakeFrom;            // �𶯻������ʼʱ��( ���� )
  unsigned int  uiShakeTo;              // �𶯻���Ľ���ʱ��( ���� _
  unsigned int  uiPrevMagicNum;         // ǰ��������������( 0XFFFFFFFFFF ��ʾ��ǰ������ )
  int           siPrevMagicSx;          // ǰ����������ʾλ�� - X��
  int           siPrevMagicSy;          // ǰ����������ʾλ�� - Y��
  int           siPrevMagicOnChar;      // ǰ��������ʾ�������ǰ�����·�
  unsigned int  uiPostMagicNum;         // ����������������( 0XFFFFFFFF ��ʾ���������� )
  int           siPostMagicSx;          // ������������ʾλ�� - X��
  int           siPostMagicSy;          // ������������ʾλ�� - Y��
  int           siPostMagicOnChar;      // ����������ʾ�������ǰ�����·�
  int           siField[3][5];          // ��������
}AttMagic;

#endif

#ifdef _MAGIC_TOCALL

typedef struct tagToCallMagic
{
  unsigned int  uiSpriteNum;// ��������Spr_x.bin�ı��
  unsigned int  uiAttackType;// �����ķ�ʽ�����ˣ�����( ���� ) ,  ����( ���� ) , ����( ͬʱ ) , ȫ��( ���� ) , ȫ��( ͬʱ )
  unsigned int  uiSliceTime;// ��������ʱ��ʱ���
  unsigned int  uiShowType;             // ��ʾ��λ�÷�ʽ�����롢ָ��
  int           siSx;                   // ��ʾ��λ�� - X��
  int           siSy;                   // ��ʾ��λ�� - Y��
  unsigned int  uiShowBehindChar;       // ��ʾ�������ǰ�����·�
  unsigned int  uiShakeScreen;          // �Ƿ��𶯻���
  unsigned int  uiShakeFrom;            // �𶯻������ʼʱ��( ���� )
  unsigned int  uiShakeTo;              // �𶯻���Ľ���ʱ��( ���� _
  unsigned int  uiPrevMagicNum;         // ǰ��������������( 0XFFFFFFFFFF ��ʾ��ǰ������ )
  int           siPrevMagicSx;          // ǰ����������ʾλ�� - X��
  int           siPrevMagicSy;          // ǰ����������ʾλ�� - Y��
  int           siPrevMagicOnChar;      // ǰ��������ʾ�������ǰ�����·�
  unsigned int  uiPostMagicNum;         // ����������������( 0XFFFFFFFF ��ʾ���������� )
  int           siPostMagicSx;          // ������������ʾλ�� - X��
  int           siPostMagicSy;          // ������������ʾλ�� - Y��
  int           siPostMagicOnChar;      // ����������ʾ�������ǰ�����·�
  int			isPostDisappear;		// ����һ�㹥����ʱ�Ƿ�������ʧ
  int			ToCallMagicNo;			// �ٻ����ı��
}ToCallMagic;

#endif

typedef int (*MAGIC_CALLFUNC)( int, int, int, int );

INLINE BOOL MAGIC_CHECKINDEX( int index );
INLINE int MAGIC_getInt( int index, MAGIC_DATAINT element);
INLINE int MAGIC_setInt( int index, MAGIC_DATAINT element, int data);
INLINE char* MAGIC_getChar( int index, MAGIC_DATACHAR element);
INLINE BOOL MAGIC_setChar( int index ,MAGIC_DATACHAR element, char* new );
int MAGIC_getMagicNum( void);
BOOL MAGIC_initMagic( char *filename);
BOOL MAGIC_reinitMagic( void );

#ifdef _ATTACK_MAGIC

BOOL ATTMAGIC_initMagic( char *filename );
BOOL ATTMAGIC_reinitMagic( void );

#endif

int MAGIC_getMagicArray( int magicid);
MAGIC_CALLFUNC MAGIC_getMagicFuncPointer(char* name);
// Nuke +1 08/23 : For checking the validity of magic target
int MAGIC_isTargetValid( int magicid, int toindex);

#endif

