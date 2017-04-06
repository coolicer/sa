#ifndef _BATTLE_MAGIC_H_
#define _BATTLE_MAGIC_H_

#define MAGIC_EFFECT_USER	100600
#define SPR_heal	100601
#define SPR_heal2	100602
#define SPR_heal3	100603
#define SPR_tyusya	100604
#define SPR_hoshi	100605


#define MAGIC_ID_RECOVERY	 		1
#define MAGIC_ID_STATUS_RECOVERY	3
#define MAGIC_ID_FIELD_CHANGE 		4
#define MAGIC_ID_BADSTATUS	 		5

enum{
	BFUKI_CAPTUREUP,
	BFUKI_END
};


enum{
	PC_KIND_NONE,
	PC_KIND_ATTACK,
	PC_KIND_DEFENSE,
	PC_KIND_QUICK,
	PC_KIND_CHARM,
	PC_KIND_CAPTURE,
	PC_KIND_END
};

int	MAGIC_Recovery_Battle(
	int charaindex,
	int toindex,
	int magicindex,
	int mp
);

int	MAGIC_FieldAttChange_Battle(
	int charaindex,
	int toindex,
	int marray,
	int mp
);

int	MAGIC_StatusChange_Battle(
	int charaindex,
	int toindex,
	int marray,
	int mp
);

#ifdef _MAGIC_DEEPPOISON
int	MAGIC_StatusChange_Battle2(
	int charaindex,
	int toindex,
	int marray,
	int mp
);
#endif

#ifdef _OTHER_MAGICSTAUTS
int	MAGIC_MagicStatusChange_Battle( int charaindex, int toNo, int marray, int mp );

#endif

int	MAGIC_MagicDef_Battle(
	int charaindex,
	int toNo,
	int marray,
	int mp
);

void BATTLE_MultiRecovery(
	int battleindex,
	int attackNo,
	int toNo,
	int kind,
	int power,
	int per,
	int UseEffect,
	int RecevEffect

);

#ifdef _IMPRECATE_ITEM
void BATTLE_ImprecateRecovery(
	int battleindex,
	int attackNo,
	int toNo,
	int kind,
	int powers,
	int rounds,
	int UseEffect,
	int RecevEffect
);
#endif
void BATTLE_MultiStatusChange(
	int battleindex,
	int attackNo,
	int toNo,
	int status,
	int turn,
	int UseEffect,
	int RecevEffect,
	int Success
);

#ifdef _OTHER_MAGICSTAUTS
void BATTLE_MultiMagicStatusChange(
	int battleindex,
	int attackNo,
	int toNo,
	int status,
	int turn,
	int UseEffect,
	int RecevEffect,
	int nums
);
#endif

void BATTLE_MultiMagicDef(
	int battleindex,
	int attackNo,
	int toNo,
	int kind,
	int count,
	int UseEffect,
	int RecevEffect
);

void BATTLE_MultiParamChange(
	int battleindex,
	int attackNo,
	int toNo,
	int kind,
	int power,
	int par,
	int UseEffect,
	int RecevEffect
);

int	MAGIC_FieldAttChange_Battle(
	int charaindex,
	int toNo,
	int marray,
	int mp 
);

int	BATTLE_FieldAttChange(
	int charaindex,
	char *pArg
);

void BATTLE_MultiAttReverse(
	int battleindex,
	int attackNo,
	int toNo,
	int UseEffect,
	int RecevEffect
);


void BATTLE_MultiStatusRecovery(
	int battleindex,
	int attackNo,
	int toNo,
	int status,
	int UseEffect,
	int RecevEffect
);

int	MAGIC_StatusRecovery_Battle(
	int charaindex,
	int toNo,
	int marray,
	int mp
);

void BATTLE_MultiRessurect(
	int battleindex,
	int attackNo,
	int toNo,
	int power,
	int per,
	int UseEffect,
	int RecevEffect
);

void BATTLE_MultiCaptureUp(
	int battleindex,
	int attackNo,
	int toNo,
	int power,
	int UseEffect,
	int RecevEffect
);
//
//***********************************************************

//****************************************************************
//
// ��姾���������������
//
int	MAGIC_Ressurect_Battle(
	int charaindex, // �������м��̼������͵�
	int toNo, 		// ������ľ���м��̼������͵�
	int marray, 	// magicindex
	int mp 			// MP
);
//  ��  ������ TRUE
//  ��  ������ FALSE
//****************************************************************

//****************************************************************
//
// ���뼰  ��ë��������
//
int	MAGIC_AttReverse_Battle(
	int charaindex, // �������м��̼������͵�
	int toNo, 		// ������ľ���м��̼������͵�
	int marray, 	// magicindex
	int mp 			// MP
);
//  ��  ������ TRUE
//  ��  ������ FALSE
//****************************************************************
//****************************************************************
//
//   ��    ����������
//
int	MAGIC_CaptureUp_Battle(
	int charaindex, // �������м��̼������͵�
	int toNo, 		// ������ľ���м��̼������͵�
	int marray, 	// magicindex
	int mp 			// MP
);
//  ��  ������ TRUE
//  ��  ������ FALSE
//****************************************************************

//***********************************************************
//
//    ܷ�ƥ  �Ѽ�ƽ�ҷ�������ë���ٽ����½��ձ�  ܷ  ��ëܰ��
//
void BATTLE_MultiResAndDef( 
	int battleindex, 	// �������̼������͵�
	int attackNo,		// ��������  į
	int toNo,			// ������ľ����  į
	int power,			// ��  �����
	int per,			// �Ѿ���
	int kind, 			//   ܷ  ������  
	int count, 			// �ϼ�  ʲ��
	int UseEffect,		// �����м��ް�������
	int RecevEffect		// ������ľ���м��ް�������
);
//
//***********************************************************
//****************************************************************
//
// ���١�  ܷ  ��ëܸ��������
//
int	MAGIC_ResAndDef_Battle(
	int charaindex, // �������м��̼������͵�
	int toNo, 		// ������ľ���м��̼������͵�
	int marray, 	// magicindex
	int mp 			// MP
);
//  ��  ������ TRUE
//  ��  ������ FALSE
//****************************************************************

#ifdef _ATTACK_MAGIC
int MAGIC_AttMagic_Battle( int charaindex , int toNo , int marray , int mp );
#ifdef _FIX_MAGICDAMAGE
void BATTLE_MultiAttMagic( int battleindex, int attackNo, int toNo,
						  int attIdx ,int FieldAttr ,int Power, int MagicLv);

#else
void BATTLE_MultiAttMagic( int battleindex , int attackNo , int toNo , int attIdx , int FieldAttr , int Power );
#endif
int MAGIC_AttMagic_Battle( int charaindex , int toNo , int marray , int mp );

#endif



#ifdef _MAGIC_TOCALL
int MAGIC_ToCallDragon_Battle( int charaindex , int toNo , int marray , int mp );

void BATTLE_MultiToCallDragonMagic( int battleindex, int attackNo, int toNo,
						  int attIdx ,int FieldAttr ,int Power, int ImageNo);

#endif

#ifdef _Item_ReLifeAct
void BATTLE_MultiReLife( int battleindex, int attackNo, int toNo, int power, int RecevEffect );
#endif
#ifdef _MAGIC_WEAKEN
int	MAGIC_ParamChange_Turn_Battle(int charaindex,int toNo,int marray,int mp);
void BATTLE_MultiParamChangeTurn( int battleindex,int attackNo,int toNo,int status,int UseEffect,int RecevEffect,int turn, int Success);
#endif
int BATTLE_MagicEffect(	int battleindex,int attackNo,int ToList[],int MyEffect,int ToEffect );

#ifdef _PETSKILL_FIREKILL
void BATTLE_MultiAttMagic_Fire( int battleindex, int attackNo, int defNo,
						  int FieldAttr , int Power);
#endif
#endif
