#ifndef __MAGIC_H__
#define __MAGIC_H__

/* ���� */
int MAGIC_Use( int charaindex, int haveitemindex, int toindex);
int MAGIC_Recovery( int charaindex, int toindex,int marray, int mp );
int MAGIC_OtherRecovery( int charaindex, int toindex, int marray, int mp );
int	MAGIC_FieldAttChange( int charaindex, int toindex, int marray, int mp );
int	MAGIC_StatusChange( int charaindex, int toindex, int marray, int mp );
#ifdef _MAGIC_DEEPPOISON
int	MAGIC_StatusChange2( int charaindex, int toindex, int marray, int mp );
#endif
int	MAGIC_StatusRecovery( int charaindex, int toindex, int marray, int mp );
int	MAGIC_MagicDef( int charaindex, int toindex, int marray, int mp );
int	MAGIC_Ressurect( int charaindex, int toindex, int marray, int mp );
int	MAGIC_AttReverse( int charaindex, int toindex, int marray, int mp );
int	MAGIC_ResAndDef( int charaindex, int toindex, int marray, int mp );
#ifdef _OTHER_MAGICSTAUTS
int	MAGIC_MagicStatusChange( int charaindex, int toindex, int marray, int mp );
#endif
#ifdef _ATTACK_MAGIC
int     MAGIC_AttMagic( int charaindex , int toindex , int marray , int mp );
#endif
#ifdef _ITEM_METAMO
int MAGIC_Metamo( int charaindex, int toindex,int marray, int mp );
#endif

#ifdef _ITEM_ATTSKILLMAGIC
int MAGIC_AttSkill( int charaindex, int toindex,int marray, int mp );
#endif
#ifdef _MAGIC_WEAKEN// vincent  ����:����
int	MAGIC_Weaken( int charaindex, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_BARRIER// vincent  ����:ħ��
int	MAGIC_Barrier( int charaindex, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_NOCAST// vincent  ����:��Ĭ
int	MAGIC_Nocast( int charaindex, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_TOCALL
int MAGIC_ToCallDragon( int charaindex, int toindex,int marray, int mp );
#endif

//----------------------------------------------------------------------
// ʧ��  ة����  ��������  įë߯��
//
int MAGIC_GetArrayNo(
	int charaindex, 	// ƽ�ҷ������̼������͵�
	int haveitemindex   //    �Ȼ���ʧ��  ة����  
);
//
//----------------------------------------------------------------------
//-------------------------------------------------------------------
//
//  ����ë  ������
//
int MAGIC_DirectUse(
	int charaindex, // ����ƽ�ҷ¼��̼������͵�
	int marray, 	// ��������̼������͵�
	int toindex, 	// ����������
	int itemnum
);
//
//-------------------------------------------------------------------

#endif 

