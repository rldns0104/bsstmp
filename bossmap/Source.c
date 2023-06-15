#include "roguelike.h"

int main()
{
	HANDLE	hCSB = GetStdHandle(STD_OUTPUT_HANDLE);
	PRL		pRL = _makeRL();

	_setCodePage(CP_UTF8);
	_setCursorVisibility(hCSB, FALSE);
	_showStartMenu(hCSB, pRL);

	int turnCnt = 0;
	while (pRL->pPC != NULL)
	{

		_showGUI(hCSB, pRL);

		_showInventory(hCSB, pRL->pPC);
		_showPlayerInfo(hCSB, pRL->pPC);
		_showScore(hCSB, pRL);
		_showShop(hCSB, pRL);
		_printSysMsg(pRL->curEnv, hCSB);


		if (_handlePC(pRL, hCSB)) {

			_moveAllEnemy(pRL->curEnv, pRL->pPC);
			_updateScore(pRL);


			turnCnt++;
		}
		if ((turnCnt % 10 == 0) && (pRL->pPC->stats[_curHP] < pRL->pPC->stats[_maxHP]))
		{
			pRL->pPC->stats[_curHP]++;

		}

		if (turnCnt % SPAWN_TURN == 0 && turnCnt) {
			_spawnNearEnt(pRL);
		}

		// º¸½º Å»Ãâ±¸
		if (pRL->curIdx == 4 && pRL->pPC->pos.X == pRL->curEnv->posO.X && pRL->pPC->pos.Y == pRL->curEnv->posO.Y) {
			pRL->pPC->stats[_SCORE] += 10000;
			_showClearScreen(hCSB, pRL);
			_writeLeaderBoard(pRL, TRUE);
			_showLeaderBoard(hCSB);
			getchar();
		}
		//if (_isDead(pRL->pPC)) {
		//	_showDeathScreen(hCSB, pRL);
		//	_writeLeaderBoard(pRL, FALSE);
		//	_showLeaderBoard(hCSB);
		//	return;
		//}
	}
	_freeRL(pRL);

	return 0;
}

