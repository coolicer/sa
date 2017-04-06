#ifndef __VERSION_H__
#define __VERSION_H__
#include "correct_bug.h"  //BUG FIX

/*Server �汾����*/
#define _NEW_SERVER_
#define _SERVER_NUMS

#define UNIQUE_KEYCODEITEM	100
#define UNIQUE_KEYCODEPET	"i"
//#define _DEFAULT_PKEY "Wonwonwon"	// (�ɿ���)( ʯ�� 6.0 )
//#define _RUNNING_KEY  "11111111"  // (�ɿ���)( ʯ�� 6.0 )
//#define _DEFAULT_PKEY "ttttttttt"   // (�ɿ���)( ʯ�� 6.0 )
//#define _RUNNING_KEY  "20041215"    // (�ɿ���)( ʯ�� 6.0 )

/* -------------------------------------------------------------------
 * ר��������ׯ԰�����޸�	��Ա��С褡���ѫ��־ΰ��С��
 */

/*
�޸�װ԰������ʽ��
Account Server ��ʽ:
	acfamily.h	MAX_FMPOINTҪ�޸�
Account Server ��Ϸ����:
	saac/family/fmpoint/db_fmpoint������������ׯ԰�����������趨
	ׯ԰����ͼ��|ׯ԰����������|ׯ԰����������|ׯ԰�峤ͼ��|ׯ԰�ڽ�֮��ׯ|ռ��ׯ԰֮���� index|ռ��ׯ԰֮���� name|ռ��ׯ԰֮�����ۺ�����ֵ
	���磺
		100|607|563|1041|1|3|shan|3132238
		200|73|588|2031|2|||
Game Server ��ʽ��
	version.h FAMILY_MAXHOME��MANORNUMҪ����
	family.c	 FM_PKFLOOR	������ս��ͼ��
				 FAMILY_RidePet() ���������峤��ͼ��
				 FAMILY_LeaderFunc() �����������뺯id
	npcutil.c addNpcFamilyTax function��������(ׯ԰ͼ��%100)��Ϊׯ԰˰�ղ���
Game Server ��Ϸ����:
	gmsv_v2/data/npc/family/manorsman.arg* id������
	gmsv_v2/data/npc/family/scheduleman.arg* id������
*/

#define _FAMILY_MANORNUM_CHANGE	 // CoolFish �����޸�װ԰����(4.0)
#ifdef _FAMILY_MANORNUM_CHANGE
#define FAMILY_MAXHOME 4        // (4.0) �����ݵ�
#define MANORNUM       4        // (4.0) ׯ԰����
#define FMPOINTNUM     4        // (4.0) �оݵ���������������
#endif
#define _FMVER21                 // (�ɿ���) ����ְ�ƹ���  
#define _PERSONAL_FAME           // (�ɿ���) Arminius 8.30 ������������  
// -------------------------------------------------------------------
//ר����ʯ����Զ���		��Ա������������

#define _EVERYONE_RIDE           // (�ɿ���) Robin 0924 ����ƽ����  
// -------------------------------------------------------------------
//ר��������������������	��Ա����־
#define _ATTACK_MAGIC            // (�ɿ���)	�ṩս���еĹ���������
#define _ADD_ACTION              // (�ɿ���) һ��NPC����״�����趨

// -------------------------------------------------------------------
//ר����������߱���		��Ա��С��
#define _UNIQUE_P_I              // (�ɿ���) CoolFish 2001/10/11 

// -------------------------------------------------------------------
//ר�����°����ﴢ��pn �� ��������˷ 1018	��Ա����ѫ
#define _NEWSAVE                 // (�ɿ���) Robin
#define _STORECHAR               // (�ɿ���) Robin
#define _DROPCHECK               // (�ɿ���) Robin ��ֹ�ظ�������
#define _DROPCHECK2              // (�ɿ���) Robin ��ֹ�ظ�������
// -------------------------------------------------------------------
//ר�� : ������	��Ա : ����
#define _PET_TRANS               // (�ɿ���) ����ת������
#define _NEW_WARPMAN             // (�ɿ���) NPC����WARP����
#define _PET_TALK                // (�ɿ���) �����Ի�
#define _PET_LIMITLEVEL          // (�ɿ���) ����������(����)
#define _MAP_NOEXIT              // (�ɿ���) ������ͼ����ԭ��&ԭ�ǻ�ָ����
// -------------------------------------------------------------------
// ר����GMָ��㼶Reload		��Ա��С��
#define _GMRELOAD                // (�ɿ���) CoolFish: 2001/11/12 
// -------------------------------------------------------------------
// ר����������½	��Ա��������������С��
#define _GAMBLE_ROULETTE         // (�ɿ���) ���� �ĳ�����	����С��  
#define _FIX_GAMBLENUM           // (�ɿ���) �������� : ���߿ۻ��� 
#define _GAMBLE_BANK             // (�ɿ���) ����  �ĳ�����  
#define _DROPSTAKENEW            // (�ɿ���) CoolFish:  �ĳ���ע  
#define _PETRACE                 // (�ɿ���) CoolFish: 2001/12/3 ���ﾺ�ٳ�  
#define _NEWEVENT                // (�ɿ���) CoolFish: 2001/12/5 �µ���������  
// -------------------------------------------------------------------
//ר������������	��Ա������
#define _ITEM_FIRECRACKER        // (�ɿ���) ս����ʹ�ñ��ڿ��԰���������
// -------------------------------------------------------------------
//�¹��ܣ�ׯ԰PK����(���綨��)		��Ա��־ΰ��־��
#define _MANOR_PKRULE            // (�ɿ���)
// -------------------------------------------------------------------
// �¹��ܣ��¹���&���＼��		��Ա������
#define _PSKILL_FALLGROUND       // (�ɿ���) ���＼��  (������)
#define _ENEMY_FALLGROUND        // (�ɿ���) NPC ����
#define _BATTLE_NEWPOWER         // (�ɿ���)
#define _Item_MoonAct            // (�ɿ���) �������ػ�
#define _Item_DeathAct           // (�ɿ���) ��ħ������
#define _Item_PetAct             // (�ɿ���) ѱ�޽�ָ
#define _Item_ReLifeAct          // (�ɿ���) ��Ҫitemset1.txt ���ܿ�) ��������
#define _BATTLESTEAL_FIX         // (�ɿ���) ����͵��
#define _CFREE_petskill          // (�ɿ���) ��Ҫpetskill1.txt ���ܿ�) ��ѧϰ���＼�ܼ���
#define _data_newreadfile        // (�ɿ���) �µ�server setup.cf ���� ���� itemset1.txt petskill1.txt �����µ�
#define _ITEM_MAXUSERNUM         // (�ɿ���) �� itemset1.txt ITEM ʹ�ô���
// -------------------------------------------------------------------
//�¹��ܣ���������    ��Ա������
#define _PET_TALKPRO             // (�ɿ���) �����Ի���ǿ  & �� END
#define _PRO_NPCFIX              // (�ɿ���) NPC���ܼ�ǿ
#define _PRO_BATTLEENEMYSKILL    // (�ɿ���) ս����ǿ  ����NPC��AI����
#define _NEW_WARPPOINT           // (�ɿ���) ���͵����������ж�WARP
#define _FIX_CHECKITEM           // (�ɿ���) ���߲����Ƿ�����
// -------------------------------------------------------------------
//�¹��ܣ�������  ��Ա��С��,WON
#define _ADD_ENCOUNT             // (�ɿ���) WON ���ӵ����������޼� 
#define _WON_PET_MAIL_LOG        // (�ɿ���) WON ���Ҽĳ��� LOG 
// -------------------------------------------------------------------
// �¹��ܣ���������  4.0  ��Ա������, ־ΰ , ���� , ־��
//#define _LOSE_FINCH_           // (���ɿ�) ANDY ��ȸ����
#define _FIX_FAMILY_PK_LIMIT     // (�ɿ���) WON ������սׯ԰����ǰʮ����Ϊǰ��ʮ��
#define _ADD_POOL_ITEM           // (�ɿ���) WON ���ӿɼķŵĵ���
#define _ITEM_METAMO             // (�ɿ���) shan metamo sprite. code:robin->shan
#define _ENEMY_ATTACK_AI         // (�ɿ���) shan enemy attack mode. code:shan  
#define _ITEM_TIME_LIMIT         // (�ɿ���) shan time limit of item. code:shan
#define _MIND_ICON               // (�ɿ���) shan show icon from char mind. code:shan
#define _BATTLENPC_WARP_PLAYER   // (�ɿ���) shan npc warp player when n round. code:shan
#define _ITEM_WARP_FIX_BI        // (�ɿ���) shan fix bbi to bi. code:shan
#define _SKILL_DAMAGETOHP        // (�ɿ���) ANDY ��Ѫ�� 
#define _Skill_MPDAMAGE          // (�ɿ���) ANDY MP�˺�
#define _WAEI_KICK               // (�ɿ���) ANDY ����������
#define _BATTLE_TIMESPEED        // (�ɿ���) ANDY ս��ʱ�� ����(�������ؿ��������ٹ���)
#define _FIXBUG_ATTACKBOW        // (�ɿ���) ANDY ��������ʹ��Ͷ������bug
#define _NPCCHANGE_PLAYERIMG     // (�ɿ���) ANDY NPC�ı�����ͼ��, ǿ������,������ͼ�� (����)
#define _ITEM_ATTSKILLMAGIC      // (�ɿ���) ANDY ���߼���  
#define _EQUIT_DEFMAGIC          // (�ɿ���) ANDY ��ħװ��
#define _FIX_MAGICDAMAGE         // (�ɿ���) ANDY ����ħ����ʽ ���ڿ���
#define _ITEM_CONSTITUTION       // (�ɿ���) ANDY ������������
#define _MAGIC_REHPAI            // (�ɿ���) ANDY ��ѪAI
#define _TRANSER_MAN             // (�ɿ���) ANDY ����ʦ
#define _TAKE_ITEMDAMAGE         // (�ɿ���) ANDY ��������  ITEMSET2
#define _FIX_FIRECRACKER         // (�ɿ���) ANDY ���� ����
#define _FIX_ITEMPROB            // (�ɿ���) ANDY ���� ������
#define _ADD_DEAMGEDEFC          // (�ɿ���) ANDY �����˺� & ���ӹ���  ITEMSET2
#define _FIX_MAXCHARMP           // (�ɿ���) ANDY MP����
#define _ITEM_INSLAY             // (�ɿ���) ANDY �ⱦʯ ����
#define _ITEMSET2_ITEM           // (�ɿ���) ANDY ITEMSET2
#define _ITEM_NPCCHANGE          // (�ɿ���) ANDY ���� (����, ����)
#define _PETSKILL_FIXITEM        // (�ɿ���) ANDY �޸� && ͬ��������
#define _FIX_SETWORKINT          // (�ɿ���) ANDY (FIXWORKINT)
#define _ITEM_ADDEXP             // (�ɿ���) vincent  ����:ָ��ʱ�������Ӿ���ֵ��������
#define _ITEM_ADDEXP2            // �ǹ�ʱ�������ۼ� Robin
#define _FIX_METAMORIDE          // (�ɿ���) ANDY
#define _CHECK_ITEMDAMAGE        // (�ɿ���) ������������  7/26
#define _SKILL_WILDVIOLENT_ATT   // (�ɿ���) vincent  �輼:�񱩹���
#define _MAGIC_WEAKEN            // (�ɿ���) vincent  ����:����
#define _SKILL_WEAKEN            // (�ɿ���) vincent  �輼:����//��Ҫ��#define _MAGIC_WEAKEN
#define _SKILL_SPEEDY_ATT        // (�ɿ���) vincent  �輼:���ٹ���
#define _SKILL_GUARDBREAK2       // (�ɿ���) vincent  �輼:�Ƴ�����2
#define _SKILL_SACRIFICE         // (�ɿ���) vincent  �輼:��Ԯ
#define _PSKILL_MODIFY           // (�ɿ���) ANDY ����ǿ������
#define _PSKILL_MDFYATTACK       // (�ɿ���) ANDY ����ת������
#define _MAGIC_DEFMAGICATT       // (�ɿ���) ANDY ħ������
#define _MAGIC_SUPERWALL         // (�ɿ���) ANDY ���ڷ���
#define _OTHER_MAGICSTAUTS       // (�ɿ���) ANDY 
#define _SKILL_TOOTH             // (�ɿ���) ANDY   ����
#define _MAGIC_DEEPPOISON        // (�ɿ���) vincent  ����:�綾
#define _MAGIC_BARRIER           // (�ɿ���) vincent  ����:ħ��
#define _MAGIC_NOCAST            // (�ɿ���) vincent  ����:��Ĭ
#define _ITEM_CRACKER            // (�ɿ���) vincent  ����:���� //��Ҫ��#define _MIND_ICON
#define _SKILL_DEEPPOISON        // (�ɿ���) vincent  �輼:�綾 //��Ҫ��#define _MAGIC_DEEPPOISON
#define _SKILL_BARRIER           // (�ɿ���) vincent  �輼:ħ�� //��Ҫ��#define _MAGIC_BARRIER
#define _SKILL_ROAR              // (�ɿ���) vincent  �輼:����(������)
#define _SKILL_REFRESH           // (�ɿ���) vincent  �輼:�����쳣״̬
#define _ITEM_REFRESH            // (�ɿ���) vincent  �����쳣״̬����
#define _MAGIC_TOCALL            // (�ɿ���) kjl     �ٻ�  02/06/20 kjl  

// -------------------------------------------------------------------
// �¹��ܣ�����������  ��Ա������
#define _ITEMSET3_ITEM           // (�ɿ���) ANDY itemset3.txt
#define _SUIT_ITEM               // (�ɿ���) ANDY (��װ) ������ ����itemset3.txt
#define _PETSKILL_SETDUCK        // (�ɿ���) ANDY ���漼��
#define _VARY_WOLF               // (�ɿ���) pet skill : vary wolf. code:shan
// -------------------------------------------------------------------
// �¹��ܣ�ר��5.0    ��Ա��
#define _USEWARP_FORNUM          // (�ɿ���) ANDY ������ë ������
#define _IMPRECATE_ITEM          // (�ɿ���) ANDY �������� ������
#define _MAGICPET_SKILL          // (�ɿ���) ANDY ħ���輼��
#define _ITEM_CHECKWARES         // (�ɿ���) ANDY �������Ͳ���
#define _NPC_REPLACEMENT         // (�ɿ���) ANDY �û�ս��npc
#define _TYPE_TOXICATION         // (�ɿ���) ANDY �ж��޷���Ѫ

// -------------------------------------------------------------------
// �¹��ܣ�5.0׷�Ӳ���    ��Ա��
#define _AVID_TRADETRYBUG        // (�ɿ���) ANDY ��ֹ�����ж���/���� ��(����)�ʼ� ʯ��
#define _ASSESS_SYSEFFICACY      // (�ɿ���) ANDY ����ϵͳЧ�� 12/04 ����
#define _ASSESS_SYSEFFICACY_SUB  // (�ɿ���) Robin ����ϵͳЧ�� ���Ӽ����ڻ�Ȧ
#define _ITEM_ORNAMENTS          // (�ɿ���) ANDY װ�ε���  12/04 ����  //���沿
#define _CHIKULA_STONE           // (�ɿ���) ANDY ������֮ʯ 12/04 ����  //������ ����
#define _SEND_EFFECT             // (�ɿ���) WON  AC����ѩ����������Ч 12/04 ����
#define _PETMAIL_DEFNUMS         // (�ɿ���) ANDY �����ʼ�����
#define _PETSKILL_TIMID          // (�ɿ���) ANDY �輼-��ս
#define _PETS_SELECTCON          // (�ɿ���) ANDY ���ﲻ�ɵ�ѡ��ս
#define _CHRISTMAS_REDSOCKS      // (�ɿ���) ANDY ʥ������
#define _FIX_ARRAYBUG            // (�ɿ���) ANDY ����array ��λ
#define _USER_CHARLOOPS          // (�ɿ���) ANDY ��������LOOP
#define _BATTLE_PROPERTY         // (�ɿ���) ANDY ս����������
#define _PETSKILL_PROPERTY       // (�ɿ���) ANDY ����ħ�޳輼
#define _ITEM_FIXALLBASE         // (�ɿ���) ANDY �޸�֮ʯ
#define _ITEM_LOVERPARTY         // (�ɿ���) ANDY ���˽ڵ���
#define _ITEM_FORUSERNAMES       // (�ɿ���) ANDY ����������
#define _BATTLECOMMAND_TIME      // (�ɿ���) ANDY ��ֹ���ҿ�����(�غ�ս��ʱ��)
#define _NPCENEMY_ADDPOWER       // (�ɿ���) ANDY �޸Ĺ���������ֵ


// -------------------------------------------------------------------
// �¹��ܣ�ר��6.0    ��Ա��
//
#define _PETSKILL_CANNEDFOOD     // (�ɿ���) ANDY ���＼�ܹ�ͷ  for 6.0
#define _TEST_DROPITEMS          // (�ɿ���) ANDY ���Զ���
#define _MAP_WARPPOINT           // (�ɿ���) ANDY Map WarpPoint
#define _TREASURE_BOX            // (�ɿ���) ANDY ����
#define _BENEMY_EVENTRUN         // (�ɿ���) ANDY ս��npc������
#define _BATTLE_ABDUCTII         // (�ɿ���) ANDY ��;ⷰ�2
#define _BATTLE_LIGHTTAKE        // (�ɿ���) ANDY �ɹ���
#define _BATTLE_ATTCRAZED        // (�ɿ���) ANDY ��������
#define _CAPTURE_FREES           // (�ɿ���) ANDY ��ץ����
#define _THROWITEM_ITEMS         // (�ɿ���) �������н�Ʒ
//--------------------------------------------------------------------------
//ר�� 7.0  ְҵϵͳ  ��Ա������ ־�� ���� ����
#define _ACFMPK_LIST             // (�ɿ���) ANDY ������ս�б�����
#define _NPC_NOSAVEPOINT         // (�ɿ���) ANDY ��¼�㲻����������
#define _PETSKILL2_TXT           // (�ɿ���) ANDY petskill2.txt
#define _PETSKILL_CHECKTYPE      // (�ɿ���) ANDY ���＼��ʹ��ʱ��
#define _PETSKILL_TEAR           // (�ɿ���) ANDY �輼 ˺���˿�
#define _ITEMSET4_TXT            // (�ɿ���) ANDY itemset4.txt
#define _PET_LOSTPET             // (�ɿ���) ANDY ��ʧ������Ѱ�ƻ�����
#define _ITEMSET5_TXT            // (�ɿ���) ANDY itemset5.txt
#define _ITEMSET6_TXT            // (�ɿ���) ANDY itemset6.txt
#define _EQUIT_SEQUENCE          // (�ɿ���) ANDY ����˳�� ��Ҫ itemset5.txt
#define _EQUIT_HITRIGHT          // (�ɿ���) ANDY ���� ��Ҫ itemset5.txt
#define _EQUIT_NEGLECTGUARD      // (�ɿ���) ANDY ����Ŀ��������% ��Ҫ itemset6.txt
#define _FIX_MAX_GOLD            // (�ɿ���) WON ADD ����������Ǯ����  
#define _PET_SKILL_SARS          // (�ɿ���) WON ADD ��ɷ����
#define _SONIC_ATTACK            // (�ɿ���) WON ADD ��������
#define _NET_REDUCESEND          // (�ɿ���) ANDY ����DB���ϴ���
#define _FEV_ADD_NEW_ITEM        // (�ɿ���) Change ���Ӹ����ؾ�
#define _ALLDOMAN                // (�ɿ���) Syu ADD ���а�NPC
#define _LOCKHELP_OK             // (�ɿ���) Syu ADD �������ɼ���ս��

//ϵͳ����
#define _SIMPLIFY_ITEMSTRING     // (�ɿ���) ANDY �򻯵����ִ�

#define _SIMPLIFY_PETSTRING      // (�ɿ���) ANDY �򻯳����浵�ִ�
#define _SIMPLIFY_ITEMSTRING2    // (�ɿ���) ANDY �򻯵����ִ�2
#define _WOLF_TAKE_AXE           // (�ɿ���) WON ADD ץ˫ͷ�ǵ�����
#define _FIX_UNNECESSARY         // (�ɿ���) ANDY ��������Ҫ���ж�
#define _ITEM_MAGICRECOVERY      // (�ɿ���) ANDY �¹⾵��
//9/17����
#define _PETSKILL_GYRATE         // (�ɿ���) Change �輼:��������
#define _PETSKILL_RETRACE        // (�ɿ���) Change �輼:׷������
#define _PETSKILL_HECTOR         // (�ɿ���) Change �輼:����
//10/13����
#define _PETSKILL_FIREKILL       // (�ɿ���) Change �輼:������ɱ
#define _PETSKILL_DAMAGETOHP     // (�ɿ���) Change �輼:���¿���(��Ѫ���ı���) 
#define _PETSKILL_BECOMEFOX      // (�ɿ���) Change �輼:�Ļ���

//11/12����
#define _PETSKILL_SHOWMERCY      // (�ɿ�) Change �輼:��������
#define _NPC_ActionFreeCmp       // (�ɿ�) Change ��NPC���������µıȽϷ�ʽ
//--------------------------------------------------------------------------


//11/26
#define _NPC_NewDelPet           // (�ɿ�) Change ��NPC����ɾ������(ԭ��������,ֻ����warpmanʹ��ʱ���ɹ�,��������һ����һ��)
#define _ALLDOMAN_DEBUG          // (�ɿ�) Change ��������server�����а���ʧbug
//20031217
#define _CHRISTMAS_REDSOCKS_NEW  // (�ɿ�) Change ��ʥ������
#define _PETSKILL_COMBINED       // (�ɿ�) Change �輼:�ѵú�Ϳ [�ۺ�ħ��(����������ȡ��ħ��)]
//���˽ڻ
#define _NPC_ADDWARPMAN1         // (�ɿ���) Change npcgen_warpman���ӹ���(�趨�����������趨������Ů����)
#define _ITEM_QUITPARTY          // (�ɿ���) Change ������ɢʱ�ض����߼�����ʧ
#define _ITEM_ADDEQUIPEXP        // (�ɿ�) Change װ���ᾭ��ֵ����
//2004/2/18
#define _ITEM_LVUPUP             // (�ɿ�) Change ͻ�Ƴ����ȼ����Ƶ�ҩ(����ר��)
//2004/02/25
#define _PETSKILL_BECOMEPIG      // (�ɿ�) Change �輼:������
#define _ITEM_UNBECOMEPIG        // (�ɿ�) Change �������Ľ�ҩ
#define _PET_TALKBBI             // (�ɿ�) Change �����ж�bbi
#define _ITEM_USEMAGIC           // (�ɿ�) Change �ɵ���ʹ��ħ��
//2004/04/07
#define _ITEM_PROPERTY           // (�ɿ�) Change �ı������Ե���
#define _SUIT_ADDENDUM           // (�ɿ�) Change ����װ���ӹ��� (_SUIT_ITEMҪ��)
//2004/05/05
#define _ITEM_CHECKDROPATLOGOUT  // (�ɿ�) Change �����ؼ�¼��ʱ�������޵ǳ�����ʧ����,����,���ɻؼ�¼��
#define _FM_MODIFY               // ���幦���޸�(����)
#define _STANDBYPET              // Robin ����������
#define  _PETSKILL_OPTIMUM       // Robin ���＼�ܴ������ѻ�
#define _MAGIC_OPTIMUM           // Robin Magic table ���ѻ�
#define _AC_PIORITY              // Nuke ����AC������Ȩ(�ɿ�)
// -------------------------------------------------------------------

//ר�� 7.5  �������л�  ��Ա����ѫ ���� С��
// Terry define start -------------------------------------------------------------------
#define _MAP_TIME                // ������ͼ,�ᵹ��(����)

// Terry define end   -------------------------------------------------------------------
#define _ITEM_ADDPETEXP          // (�ɿ�) Change ���ӳ��ﾭ��ֵ����(�ɳԴ˵��߳���,ƽ�����ɻ��þ���ֵ)
#define _TRANS_6                 // (�ɿ�) Change ����6ת
#define _PET_2TRANS              // (�ɿ�) Change ����2ת
#define _PET_2LIMITLEVEL         // (�ɿ�) Change ��������2ת������
#define _MAGICSTAUTS_RESIST      // (�ɿ�) Change (������)���Ծ���
#define _SUIT_TWFWENDUM          // (�ɿ�) Change ��������װ���� (_SUIT_ITEMҪ��)
#define _EXCHANGEMAN_REQUEST_DELPET   // (�ɿ�) Change ��exchangeman��TYPE:REQUEST����ɾ������
//--------------------------------------------------------------------------
#define _DEL_DROP_GOLD           // ɾ�����Ϲ�ʱ��ʯ��  Robin
#define _ITEM_STONE              // cyg ���ֵ�ʯͷ
#define _HELP_NEWHAND            // cyg �´���ɫ�������ֵ���)
#define _DEF_GETYOU              // cyg GMָ�� ��ѯ�Լ�����  Χ�ڵ������ʺ�
#define _DEF_NEWSEND             // cyg GMָ�� ��������ʱ�ɼӾ���Ҳ�ɲ���
#define _DEF_SUPERSEND           // cyg GMָ�� �ɴ�����ĳ�ʺ�Ϊ���ĸ���3������������
#define _SUIT_ADDPART3           // (�ɿ�) Change ��װ���ܵ����� �ر�����% ��Ĭ��������% ������% ������% ������%
#define _PETSKILL_2TIMID         // (�ɿ�) Change ��ս�ڶ���(��ʨŭ��)
#define _TIME_TICKET             // Robin ��ʱ�볡 ɭ��׽����
#define _HALLOWEEN_EFFECT        // ��ʥ����Ч
#define _ADD_reITEM              // (�ɿ�) Change ����������λ�ո�
#define _ADD_NOITEM_BATTLE       // ����NPC������������û���߻�����ս��
#define _PETSKILL_BATTLE_MODEL   // ���＼��ս��ģ��
#define _PETSKILL_ANTINTER       // (�ɿ�) Change �輼:��֮�� �ο�_PETSKILL_2TIMID
#define _PETSKILL_REGRET         // (�ɿ�) Change �輼:����һ�� �ο�_SONIC_ATTACK
//----------xfei--------------------------
#define  _NEW_RIDEPETS
#define _RIDE_CF 
#define _FM_LEADER_RIDE          // �峤ר������
#define _RIDEMODE_20             // 2.0����ģʽ
#define _ITEM_SETLOVER           // ���鹦��
#define _ITEM_RIDE               // ѱ��װ��
#define _SHOW_VIP_CF             // �Զ�������VIP�����ˣ����壬������ʾ��ʽ
#define _VIP_RIDE                // ��Ա���ڼ�
#define _VIP_ALL
#define _VIP_SERVER              // ��Ա����
#define _PLAYER_ANNOUNCE         // ��������
#define _MAKE_PET_CF             // �Զ��������ɳ�
#define _MAKE_PET_ABILITY        // �Զ���������Χ
#define _RELOAD_CF               // �ض�CF
#define _PET_UP                  // �ɼ����˶����ĳ���
#define _BATTLE_GOLD             // ��ȡ����ͬʱ���ý�Ǯ
#define _ITEM_GOLD               // ��Ǯ����Ʒ
#define _PLAYER_NUM              // ������������
#define _NEW_PLAYER_CF           // ����״̬����
#define _TRANS_LEVEL_CF          // ת����Խ160������
#define _UNLAW_WARP_FLOOR        // ��ֹ���͵���
#define _UNREG_NEMA              // ��ֹ��������
#define _LOCK_IP                 // ����IP
#define _POINT                   // ��������
#define _FIX_CHARLOOPS           // ������ħ��ʯ����ʱ��
#define _TALK_ACTION             // ˵������
#define _ENEMY_ACTION            // ���Ҳ������л���
#define _CHAR_CDKEY              // ��ֹ��������
#define _CHECK_PEPEAT            // �Ƿ���������
#define _SILENTZERO              // ������Ʒ
#define _SAVE_GAME_ID            // ��ʾ�����ķ�������
#define _AUTO_PK                 // �Զ���PKϵͳ
#define _JZ_CF_DELPETITEM				//���� ADD CF������½ʱɾ��ָ����������Ʒ
//��ͨ����������-------------------------------------
//#define _PUB_ALL
#ifdef _PUB_ALL
#define _CREATE_MM_1_2           // ������Ʒ���ٷ���
#define _SendTo                  // ���͵���������
#define _GET_BATTLE_EXP          // ��������
#define _NEW_PLAYER_CF           // ����״̬����
#define _USER_EXP_CF             // �û��Զ��徭��
#define _TRANS_LEVEL_CF          // ת����Խ160������
#define _UNLAW_WARP_FLOOR        // ��ֹ���͵���
#define _UNREG_NEMA              // ��ֹ��������
#define _GM_ITEM                 // ����GM������Ʒ
#define _GM_METAMO_RIDE          // GM���������г�
#define _GM_RIDE                 // GM������������Ȩ��
#define _LOCK_IP                 // ����IP
#define _POINT                   // ��������
#define _VIP_SERVER              // ��Ա����
#define _WATCH_FLOOR             // �Զ����ɹ�ս��ͼ
#define _BATTLE_FLOOR            // �Զ���ǿ��ս����ͼ
#define _VIP_SHOP                // ��Ա�̵�
#define _LOOP_ANNOUNCE           // ѭ������
#define _SKILLUPPOINT_CF         // �Զ�����������
#define _RIDELEVEL               // �Զ������������ȼ�
#define _REVLEVEL                // �Զ���ԭ�ȼ�
#define _FM_ITEM                 // ��������
#define _LUCK_ITEM               // ��������
#define _NEW_PLAYER_RIDE         // ��������������
#define _FIX_CHARLOOPS           // ������ħ��ʯ����ʱ��
#define _PLAYER_ANNOUNCE         // ��������
#define _BATTLE_GOLD             // ��ȡ����ͬʱ���ý�Ǯ
#define _ITEM_GOLD               // ��Ǯ����Ʒ
#define _PLAYER_NUM              // ������������
#define _PET_MM                  // ������ʯMM
#define _MYSTERIOUS_GIFT         // ��������
#define _RELOAD_CF               // �ض�CF
#define _PET_UP                  // �ɼ����˶����ĳ���
#define _TRANS                   // ����ת������
#define _TRANS_7                 // ʵ������7ת
#define _RIDEMODE_20             // 2.0����ģʽ
#define _TALK_ACTION             // ˵������
#define _ENEMY_ACTION            // ���Ҳ������л���
#define _CHAR_CDKEY              // ��ֹ��������
#define _CHECK_PEPEAT            // �Ƿ���������
#define _SILENTZERO              // ������Ʒ
#define _SAVE_GAME_ID            // ��ʾ�����ķ�������
#endif
//��Ա��׷�ӹ���--------------------------------------
//#define _VIP_ALL
#ifdef _VIP_ALL
#define _VIP_RIDE                // ��Ա���ڼ�
#define _AUTO_PK                 // �Զ���PKϵͳ
#define _FM_METAMO               // ����ר�ñ�����ָ
#define _ITEM_METAMO_TIME        // ���Զ��������Ľ�ָ
#define _SHOW_VIP_CF             // �Զ�������VIP�����ˣ����壬������ʾ��ʽ
#define _GM_KING                 // GM��ħ��
#define _ANGEL_TIME              // �Զ��徫���ٻ�ʱ��
#define _FM_POINT_PK             // ׯ԰���廥��ׯ԰
#define _PLAYER_MOVE             // ����˳�ƹ���
#define _BATTLE_PK               // ǿ��PK���ߣ����˻ؼ�¼��
#define _TRANS_7_NPC             // ������תNPC
#define _DP_NPC                  // DP�ű�
#define _RIDE_CF                 // �Զ�������
#define _PET_LEVEL_ITEM          // �����ȼ�������Ʒ
#define _FM_LEADER_RIDE          // �峤ר������
#define _FM_JOINLIMIT            // �������ٴμ���������ʱ������
#define _PET_BEATITUDE           // ����ף��
#define _ITEM_EFMETAMO           // ���ñ�����ָ
#define _OPEN_E_PETSKILL         // ���ų���E����
#define _ITEM_RIDE               // ѱ��װ��
#define _MAKE_MAP                // ��ͼ����
#define _MAKE_PET_CF             // �Զ��������ɳ�
#define _MAKE_PET_ABILITY        // �Զ���������Χ
#define _NEED_ITEM_ENEMY         // ץ��������Ʒ�Զ���
#define _GET_MULTI_ITEM          // һ�θ�������Ʒ
#endif
//˽���汾����----------------------------------------
#define _DEFAULT_PKEY "cary"   // ( ʯ�� 2.5 )
#define _RUNNING_KEY  "cary"   // ( ʯ�� 2.5 )
#define _SA_VERSION 'L'
#define SERVER_VERSION "��zoro��GMSV_2.5������"
//���⹦�ܿ���----------------------------------------

// #define _BACK_DOOR               // ���Ź���

//#define _DEBUG                   // ����

//#define _DEBUG_RET               // ���տͻ������ݵ���
//#define _DEBUG_RET_CLI           // ���տͻ������ݵ���
//#define _DEBUG_SEND_CLI          // ���Ϳͻ������ݵ���
//#define _GMSV_DEBUG              // GMSV������Ϣ
#endif

