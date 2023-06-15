#ifndef _ROGUELIKE_ENT_
#define _ROGUELIKE_ENT_

#include "roguelike.h"

PENT	_makeEnt(COORD, WCHAR, WORD, BOOL, PCHAR, STATS, STATS, STATS, STATS);
void	_initOriginEnt(PLIST);
void	_duplicateEnt(PLIST, PENT, COORD);

CTYPE	_moveEnt(PENT, PENV, PENT, SHORT, SHORT, PVOID*);
void	_moveAllEnemy(PENV, PENT);
BOOL   _isNearPC(PENT, COORD);
COORD	_EnemyMoveDir(PENT, PENT);

PTNODE _findTreeNode(PTNODE pNode, PVOID pVal);
void _spawnNearEnt(PRL);
PTNODE _inorderPcSearch(PTNODE, COORD);

void	_attackEnt(PENV, PENT, PENT);
BOOL	_isDead(PENT);
void	_killEnt(PSYS_MESSAGE_Q, PENV, PENT, PENT);
void	_updateEXP(PENV, PENT);
void	_updateScore(PRL pRL);

void	_showEnt(HANDLE, PENT);
PBOSS	_makeBoss();
void	_bossPatern1(HANDLE, PRL);
void	_bossPatern2(PRL);

void	_freeEnt(PENT);

#endif
