#ifndef _ROGUELIKE_DEFS_
#define _ROGUELIKE_DEFS_

#include "roguelike.h"

//------------------------------------------------------ roguelike_util -----

#define _FI	FOREGROUND_INTENSITY
#define _FR	FOREGROUND_RED
#define _FG	FOREGROUND_GREEN
#define _FB	FOREGROUND_BLUE
#define _FY	( _FR | _FG )		// Foreground Yellow
#define _FM	( _FR | _FB )		// Foreground Magenta
#define _FC	( _FG | _FB )		// Foreground Cyan
#define _FW	( _FR | _FG | _FB )	// Foreground White

#define _BI	BACKGROUND_INTENSITY
#define _BR	BACKGROUND_RED
#define _BG	BACKGROUND_GREEN
#define _BB	BACKGROUND_BLUE
#define _BY	( _BR | _BG )		// Background Yellow
#define _BM	( _BR | _BB )		// Background Magenta
#define _BC	( _BG | _BB )		// Background Cyan
#define _BW	( _BR | _BG | _BB )	// Background White

// same as SMALL_RECT struct
typedef struct _SRECT {
	SHORT		L;				// Left
	SHORT		T;				// Top
	SHORT		R;				// Right
	SHORT		B;				// Bottom
} SRECT, * PSRECT, ** PPSRECT;

// same as CHAR_INFO struct
typedef struct _TILE {
	WCHAR		icon;			// icon
	WORD		attr;			// icon's color
} TILE, * PTILe, ** PPTILE;

typedef struct _TREE_NODE {
	PVOID pVal;					// pointer to value
	struct _TREE_NODE* pP;		// pointer to parent node
	struct _TREE_NODE* pL;		// pointer to left node
	struct _TREE_NODE* pR;		// pointer to right node
} TNODE, * PTNODE, ** PPTNODE;

typedef struct _LIST_NODE {
	PVOID pVal;					// pointer to value
	struct _LIST_NODE* next;	// pointer to next node
	struct _LIST_NODE* prev;	// pointer to previous node
} LNODE, * PLNODE, ** PPLNODE;

typedef struct _LIST {
	USHORT len;
	PLNODE head;
	PLNODE tail;
} LIST, * PLIST, ** PPLIST;

//------------------------------------------------------ roguelike_item -----

#define NUM_ITEM_TYPE		4
#define NUM_ITEM			15
#define	ITEM_GEN_CHANCE		2	// 1 in 'n' chance of item generation
#define ICON_WEAPON			L'†'
#define ICON_ARMOUR			L'∀'
#define ICON_POTION			L'♡'
#define ICON_SCROLL			L'§'
#define ICON_KEY			L'¶'

#define ICON_OIL			L'♨'
#define ICON_FIRE			L'φ'

#define EFFECT_1			L'※'

typedef enum _ITEM_TYPE {
	_WEAPON,
	_ARMOUR,
	_POTION,
	_SCROLL,
	_KEY,
	_SPECIAL_OIL,
	_SPECIAL_FIRE
} ITYPE, * PITYPE, ** PPITYPE;

typedef struct _ITEM {
	COORD	pos;				// position
	ITYPE	type;				// type
	CHAR	name[16];			// name
	WCHAR	icon;
	WORD	color;
	USHORT	stat;
	USHORT	prob;				// 정상적인 기능을 할 확률
} ITEM, * PITEM, ** PPITEM;

typedef struct _ITEM_INVEN {
	SHORT num;
	PITEM pItem;
}ITEM_INVEN, * PITEM_INVEN, **PPITEM_INVEN;

typedef struct _SHOP {
	SHORT price;
	PITEM pItem;
}SHOP, *PSHOP, **PPSHOP;
//------------------------------------------------------- roguelike_ent -----

#define ICON_PLAYER		L'ⓟ'	// U+24C5
#define ICON_ENEMY		L'ⓔ'	// U+24BA
#define ICON_SKEL		L'ⓢ'
#define ICON_ZOMB		L'ⓩ'
#define ICON_GHOS		L'ⓖ'
#define ICON_BOSS		L'▒'
#define BOSS_ATTACK		L'※'

#define NAME_LENGTH		16		// length of names

#define NUM_ENTITY_SPECIES	3		// number of entity-species

#define SPAWN_TURN 25

#define ENTITY_GEN_CHANCE	2	// 1 in 'n' chance of entity generation
typedef enum _COLLISION_TYPE {
	_NIL,						// no collision
	_ENT,						// collision with entity
	_COL,						// collision with environment
	_PC							// 플레이어와의 충돌
} CTYPE, * PCTYPE, ** PPCTYPE;
#define	NUM_CTYPE		4		// number of collision type(s)

typedef enum _STATS {
	_curHP,						// 현재 체력
	_maxHP,						// 최대 체력
	_ATK,						// 공격력
	_DEF,						// 방어력
	_curEXP,					// 경험치
	_maxEXP,					// 최대 경험치
	_LV,						// 레벨
	_WON,						// 재화
	_tmpATK,					// 장비 착용 공격력
	_tmpDEF,					// 장비 착용 방어력
	_SCORE,						// 점수 (임시로 여기에 둚)
	_killCNT					// 킬 카운트 (임시로 여기에 둚) 더 깔끔한 방법 有?
} STATS, * PSTATS, ** PPSTATS;
#define	NUM_STATS		12		// number of stat(s)

#define BASIC_HP		15		// PC's basic HP
#define BASIC_ATK		16		// PC's basic ATK
#define BASIC_DEF		4		// PC's basic DEF
#define BASIC_EXP		15		// PC's basic max EXP

typedef struct _ENTITY {
	COORD	pos;						// position
	TILE	rep;						// graphic representation

	CHAR	name[NAME_LENGTH];			// name
	CHAR	causeOfDeath[NAME_LENGTH];	// cause of death

	SHORT	stats[NUM_STATS];			// stats/ability scores
	PITEM_INVEN	items[10];				// array of items

	BOOL	isPlayer;
	BOOL HIT; // 피격 여부  
} ENT, * PENT, ** PPENT;

typedef struct _BOSS {
	COORD attck_loc;  // 범위공격좌표
	COORD pos; // 보스좌표 (맵 가운데)
	TILE rep;

	BOOL atk_ready; // 범위공격준비
	SHORT attakCnt; // 범위공격 TRUE시 cnt증가 일정수치되면 범위공격실행
	SHORT bossTurn;
	SHORT HP; // 보스체력?

	BOOL oil;
}BOSS, * PBOSS;

//------------------------------------------------------- roguelike_env -----

#define ENV_MIN_ITER	4		// MUST NOT BE LOWER THAN 4, affects _designateEnvIO() function
#define	ENV_MIN_LEN		5 + 1	// MUST NOT BE LOWER THAN 6, affects _createRoomData() function

#define ICON_ENV_I		L'▲'	// U+25B2
#define ICON_ENV_O		L'▼'	// U+25BC

#define ICON_VOID		L'　'	// U+3000
#define ICON_WALL		L'■'	// U+25A0
#define ICON_FLOOR		L'·'	// U+00B7
#define ICON_HALLWAY	L'□'	// U+25A1
#define ICON_CHEST		L'▣'	// U+25A3

#define MAX_STAGE		5


typedef struct _SYS_MESSAGE {
	char message[200];
	struct _SYS_MESSAGE* next;
}SYS_MESSAGE, * PSYS_MESSAGE;

typedef struct _SYS_MESSAGE_Q {
	PSYS_MESSAGE front, rear;
	USHORT cnt;
}SYS_MESSAGE_Q, * PSYS_MESSAGE_Q;

typedef struct _ENVIRONMENT {
	UINT	seed;				// seed for proc-gen
	COORD	dim;				// dimension of environment
	WORD	attr;				// color attributes

	PTNODE	tRoom;				// binary tree for storing room data
	LIST	lRoom;				// linked list for storing room data
	LIST	lItem;				// linked list for storing item data
	LIST	lEnti;				// linked list for storing entity data

	LIST	lOriginalItem;
	LIST	lOriginEnti;

	PSHOP	shop[3];

	COORD	posI;				// coordinate of entrance
	COORD	posO;				// coordinate of exit

	PWCHAR	gBuf;				// buffer for graphics
	PWCHAR	cBuf;				// buffer for collision detection
	PUCHAR	fBuf;				// buffer for fog-of-war

	SHORT	curIdx;

	PSYS_MESSAGE_Q systemMsg;

} ENV, * PENV, ** PPENV;

//-------------------------------------------------------- roguelike_ui -----

#define	GUI_ENV_W		27
#define	GUI_ENV_H		27

#define	GUI_ENT_W		13
#define	GUI_ENT_H		GUI_ENV_H

#define	GUI_SYS_W		( GUI_ENV_W + 1 + GUI_ENT_W )
#define GUI_SYS_H		5

#define GUI_W			( 1 + GUI_SYS_W + 1 )
#define GUI_H			( 1 + GUI_ENV_H + 1 + GUI_SYS_H + 1 )


#define INPUT_MOVE_U	L'w'
#define INPUT_MOVE_L	L'a'
#define INPUT_MOVE_D	L's'
#define INPUT_MOVE_R	L'd'
#define INPUT_DELETE	L'q'
#define INPUT_SHOP_1	L'i'
#define INPUT_SHOP_2	L'o'
#define	INPUT_SHOP_3	L'p'

#define INPUT_CHECKS_PER_SEC 20 
#define UI_BASIC_WIDTH 60 
#define INVENTORY_UI_BASIC_HEIGHT 16 
#define STAT_UI_BASIC_HEIGHT 2


typedef struct _GUI {
	COORD	dim;				// dimension of GUI
	SRECT	rEnv;				// pos/dim of ENV part of GUI
	SRECT	rEnt;				// pos/dim of ENT part of GUI
	SRECT	rSys;				// pos/dim of SYS part of GUI

	WORD	attr;
	PWCHAR	gBuf;				// buffer for graphics
} GUI, * PGUI, ** PPGUI;


//---------------------------------------------------------------------------

typedef struct _ROGUELIKE {
	LIST	lEnv;				// list of ENV struct
	SHORT	curIdx;				// current ENV struct's index in list
	PENV	curEnv;				// current ENV struct

	PBOSS	pBOSS;  // 보스추가
	PENT	pPC;
	PGUI	pGUI;
} RL, * PRL, ** PPRL;

//---------------------------------------------------------------------------

#endif
