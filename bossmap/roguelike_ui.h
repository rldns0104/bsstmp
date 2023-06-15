#ifndef _ROGUELIKE_UI_
#define _ROGUELIKE_UI_

#include "roguelike.h"


BOOL	_handlePC(PRL, HANDLE);

PGUI	_makeGUI();
void	_showGUI(HANDLE, PRL);
void	_freeGUI(PGUI);

PSYS_MESSAGE_Q	_initSysMsg();
void _enQueueSysMsg(PSYS_MESSAGE_Q, PCHAR);
void _printSysMsg(PENV, HANDLE);

void _showLeaderBoard(HANDLE);
void _showStartMenu(HANDLE, PRL);
PCHAR _readPCName(HANDLE);
void _showTutorial(HANDLE);
void _showDeathScreen(HANDLE, PRL);
void _showClearScreen(HANDLE, PRL);


void _printCenter(HANDLE hCSB, SMALL_RECT winSize, const PCHAR msg, SHORT y);


void _showInventory(HANDLE, PENT);
void _showPlayerInfo(HANDLE, PENT);
void _showScore(HANDLE, PRL);
void _showShop(HANDLE, PRL);

void _writeLeaderBoard(PRL, BOOL);
void _showBoss(HANDLE, PRL);

void _showDeathScreen(HANDLE hCSB, PRL pRL);
void _showClearScreen(HANDLE hCSB, PRL pRL);
void _printCenter(HANDLE hCSB, SMALL_RECT winSize, const PCHAR msg, SHORT y);



#endif
