#include "roguelike_ent.h"

//--------------------------------------------------------------------------------
/*
	makes ENTITY struct - must be freed by _freeEnt() function
	@param	pos			: position
	@param	icon		: icon
	@param	attr		: attributes(icon colors)
	@param	isPlayer	: check player
	@param	name		: name
	@param	hp			: hit points
	@param	atk			: attack
	@param	exp			: experience points
	@param	won			: coin
	@return	pointer to ENT struct
*/
PENT _makeEnt(COORD pos, WCHAR icon, WORD attr, BOOL isPlayer, PCHAR name, STATS hp, STATS atk, STATS exp, STATS won)
{
	PENT result = NULL;

	if ((result = (PENT)calloc(1, sizeof(ENT))) != NULL)
	{
		result->pos = pos;
		result->rep.icon = icon;
		result->rep.attr = attr;
		result->isPlayer = isPlayer;

		result->stats[_curHP] = result->stats[_maxHP] = hp;
		result->stats[_ATK] = atk;
		result->stats[_curEXP] = exp;
		result->stats[_WON] = won;
		result->HIT = 0;
		strcpy(result->name, name);

		if (result->isPlayer) {
			_initInven(result->items);
			result->stats[_LV] = 1;
			result->stats[_DEF] = BASIC_DEF;
			result->stats[_maxEXP] = BASIC_EXP;
			result->stats[_tmpATK] = result->stats[_tmpDEF] = 0;
			result->stats[_SCORE] = 0;
			result->stats[_killCNT] = 0;
		}
		else {
			result->stats[_DEF] = 0;
		}

	}
	return result;
}
//--------------------------------------------------------------------------------
/*
	원본 엔티티들 생성
	@param pList	: 원본 엔티티 리스트의 포인터
*/
void _initOriginEnt(PLIST pList) {

	COORD pos = { 0, };

	PENT monster[MAX_STAGE][NUM_ENTITY_SPECIES] = { 0, };

	for (int i = 0; i < MAX_STAGE; i++) {
		monster[i][0] = _makeEnt(pos, ICON_GHOS, _FR, FALSE, "Ghost", (SHORT)(18 * (1 + 0.2 * i)), (SHORT)(6 * (1 + 0.2 * i)), (SHORT)(5 * (1 + 0.2 * i)), (SHORT)(4 * (1 + 0.2 * i)));
		monster[i][1] = _makeEnt(pos, ICON_SKEL, _FR, FALSE, "Skeleton", (SHORT)(20 * (1 + 0.2 * i)), (SHORT)(7 * (1 + 0.2 * i)), (SHORT)(5 * (1 + 0.2 * i)), (SHORT)(5 * (1 + 0.2 * i)));
		monster[i][2] = _makeEnt(pos, ICON_ZOMB, _FR, FALSE, "Zombie", (SHORT)(21 * (1 + 0.2 * i)), (SHORT)(9 * (1 + 0.2 * i)), (SHORT)(5 * (1 + 0.2 * i)), (SHORT)(6 * (1 + 0.2 * i)));
		_pushListNode(pList, monster[i][0]);
		_pushListNode(pList, monster[i][1]);
		_pushListNode(pList, monster[i][2]);
	}
}
PTNODE _findTreeNode(PTNODE pNode, PVOID pVal)
{
	if (pNode != NULL && pVal != NULL)
	{
		PTNODE result = NULL;

		if (pNode->pVal == pVal) { return pNode; }
		else if ((result = _findTreeNode(pNode->pL, pVal)) != NULL && result->pVal == pVal) { return result; }
		else if ((result = _findTreeNode(pNode->pR, pVal)) != NULL && result->pVal == pVal) { return result; }
	}
	return NULL;
}
//--------------------------------------------------------------------------------
/*
	플레이어 주변 방에 몬스터 배치
	@parm pRL	:	로그라이크 구조체의 포인터
*/
void _spawnNearEnt(PRL pRL) {
	COORD pcPos = pRL->pPC->pos;

	PENV curEnv = pRL->curEnv;


	if (pRL->curIdx == 4)
		return;

	PSRECT curRoom = NULL;

	for (PLNODE tmp = pRL->curEnv->lRoom.head; tmp != NULL; tmp = tmp->next)
		if ((curRoom = tmp->pVal) != NULL && _coordInSrect(pRL->pPC->pos, *curRoom))
			break;

	PSRECT nearbyRoom0 = NULL;
	PSRECT nearbyRoom1 = NULL;
	PSRECT nearbyRoom2 = NULL;

	if (curRoom != NULL)	// PC is in a room
	{
		PTNODE tNodeC = _findTreeNode(pRL->curEnv->tRoom, curRoom);		// current tree node
		PTNODE tNodeP = (tNodeC != NULL ? tNodeC->pP : NULL);			// parent tree node
		PTNODE tNodeGP = (tNodeP != NULL ? tNodeP->pP : NULL);			// grandparent tree node

		if (tNodeC != NULL && tNodeP != NULL && tNodeGP != NULL)
		{
			nearbyRoom0 = (tNodeP->pL != tNodeC ? tNodeP->pL : tNodeP->pR)->pVal;
			nearbyRoom1 = (tNodeGP->pL != tNodeP ? tNodeGP->pL : tNodeGP->pR)->pL->pVal;
			nearbyRoom2 = (tNodeGP->pL != tNodeP ? tNodeGP->pL : tNodeGP->pR)->pR->pVal;

			// do stuff here
		}
	}
	PSRECT target = NULL;
	if (curRoom != NULL) {
		USHORT rnd = rand() % 3;

		switch (rnd) {
		case 0:
			target = nearbyRoom0;
			break;
		case 1:
			target = nearbyRoom1;
			break;
		case 2:
			target = nearbyRoom2;
			break;
		}


		if (!target)
			return;

		COORD pos = { 0, 0 };

		SHORT loopCnt = 0;
		do {
			pos.X = (SHORT)(rand() % (target->R - target->L - 2) + target->L + 1);
			pos.Y = (SHORT)(rand() % (target->B - target->T - 2) + target->T + 1);
			loopCnt++;
			if (loopCnt > 30)
				return;
		} while (!curEnv->cBuf[curEnv->dim.X * pos.Y + pos.X]);



		rnd = (USHORT)(rand() % (NUM_ENTITY_SPECIES * ((curEnv->curIdx) + 1)));
		PLNODE tmp = _getListNodeI(&(curEnv->lOriginEnti), rnd);
		_duplicateEnt(&(curEnv->lEnti), tmp->pVal, pos);
		//curEnv->cBuf[curEnv->dim.X * (pos.Y) + (pos.X)] = FALSE;

	}

}
//--------------------------------------------------------------------------------
/*
	duplicates ENTITY struct
	@param	list	: pointer to an ENT struct linked list
	@param	origin	: pointer to an original ENT strcut
	@param	pos		: position
*/
void _duplicateEnt(PLIST list, PENT origin, COORD pos) {

	PENT result = NULL;

	if ((result = (PENT)calloc(1, sizeof(ENT))) != NULL)
	{

		result->pos.X = pos.X;
		result->pos.Y = pos.Y;

		result->rep.attr = origin->rep.attr;
		result->rep.icon = origin->rep.icon;

		result->isPlayer = origin->isPlayer;
		strcpy(result->name, origin->name);

		for (int i = 0; i < NUM_STATS; i++) {
			result->stats[i] = origin->stats[i];
		}

		// 리스트에 추가
		_pushListNode(list, result);
	}
}
//--------------------------------------------------------------------------------
/*
	moves ENTITY struct
	@param	pPC		: 플레이어의 포인터
	@param	pEnv	: pointer to an ENV struct
	@param	pEnt	: pointer to an ENT struct
	@param	dX		: delta x
	@param	dY		: delta y
	@param	ppVal	: reserved, should be NULL
	@return	충돌 유형
*/
CTYPE _moveEnt(PENT pPC, PENV pEnv, PENT pEnt, SHORT dX, SHORT dY, PVOID* ppVal)
{

	if (pEnv != NULL && pEnt != NULL)
	{
		COORD newPos = pEnt->pos;
		newPos.X += dX;
		newPos.Y += dY;

		if (pEnv->cBuf[pEnv->dim.X * (newPos.Y) + (newPos.X)])
		{
			//no collision detected, position updated
			pEnv->cBuf[pEnv->dim.X * (pEnt->pos.Y) + (pEnt->pos.X)] = TRUE;
			pEnv->cBuf[pEnv->dim.X * (newPos.Y) + (newPos.X)] = FALSE;
			pEnt->pos = newPos;
			return _NIL;
		}
		else
		{
			// 플레이어와 충돌하는지 확인
			if (newPos.X == pPC->pos.X && newPos.Y == pPC->pos.Y) {
				(*ppVal) = pPC;
				return _PC;
			}

			PENT pValE = NULL;
			for (PLNODE temp = pEnv->lEnti.head; temp != NULL; temp = temp->next) {
				if ((pValE = temp->pVal) != NULL) {
					if (pValE->pos.X == newPos.X && pValE->pos.Y == newPos.Y)
					{
						// collision with an item detected, position updated
						if (ppVal != NULL) { (*ppVal) = pValE; }
						return _ENT;
					}
				}
			}
			// catch-all, collision with environment detected
			return _COL;
		}
	}
	return _COL;
}
//--------------------------------------------------------------------------------
/*
   몬스터 전체 이동
   @param   pEnv   : 현재 맵의 포인터
   @param   pPC      : 플레이어의 포인터
*/
void _moveAllEnemy(PENV pEnv, PENT pPC) {
	PLNODE tmp = pEnv->lEnti.head;

	COORD dir;
	PVOID pVal = NULL;

	while (tmp) {
		PENT tmpEnti = tmp->pVal;

		if (_isNearPC(tmpEnti, pPC->pos)) {
			dir = _EnemyMoveDir(tmpEnti, pPC);

			switch (_moveEnt(pPC, pEnv, tmpEnti, dir.X, dir.Y, &pVal))
			{
			case _NIL:
				break; // ignored
			case _ENT:
				break;
			case _COL:
				break;
			case _PC:
				_attackEnt(pEnv, tmpEnti, pPC);
				break;

			}
		}

		tmp = tmp->next;
	}
}
//--------------------------------------------------------------------------------
/*
   플레이어와 가까이 있는지 여부
   @param   pEnt   : 엔티티의 포인터
   @param   pos      : 플레이어의 좌표
   @return 플레이어와 가까이 있으면 TRUE,
*/
BOOL _isNearPC(PENT pEnt, COORD pPos) {
	COORD ePos = pEnt->pos;

	return ((abs(pPos.X - ePos.X) <= 10 && abs(pPos.Y - ePos.Y) <= 10)) ? TRUE : FALSE;
}
//--------------------------------------------------------------------------------
/*
	몬스터 이동 방향 결정
	@param	pEnt	: 엔티티의 포인터
	@param	pPC		: 플레이어의 포인터
	@return 이동할 방향 반환
*/
COORD _EnemyMoveDir(PENT pEnt, PENT pPC) {
	COORD result = { 0, };

	if (abs(pPC->pos.X - pEnt->pos.X) - 1 > abs(pPC->pos.Y - pEnt->pos.Y)) {
		if (pPC->pos.Y - pEnt->pos.Y == 0) {
			result.X = (pPC->pos.X - pEnt->pos.X > 0 ? 1 : -1);
			result.Y = 0;
		}
		else {
			result.X = 0;
			result.Y = (pPC->pos.Y - pEnt->pos.Y > 0 ? 1 : -1);
		}

	}
	else {
		if (pPC->pos.X - pEnt->pos.X == 0) {
			result.X = 0;
			result.Y = (pPC->pos.Y - pEnt->pos.Y > 0 ? 1 : -1);
		}
		else {
			result.X = (pPC->pos.X - pEnt->pos.X > 0 ? 1 : -1);
			result.Y = 0;
		}

	}

	return result;
}
//--------------------------------------------------------------------------------
/*
	공격자가 수비자 공격
	@param	pEnv	: 현재 맵의 포인터
	@param	ATKer	: 공격자의 포인터
	@param	DEFer	: 수비자의 포인터
*/
void _attackEnt(PENV pEnv, PENT ATKer, PENT DEFer) {
	DEFer->stats[_curHP] -= (ATKer->stats[_ATK] - DEFer->stats[_DEF] > 0 ? ATKer->stats[_ATK] - DEFer->stats[_DEF] : 0);
	DEFer->HIT = TRUE;

	strcpy(DEFer->causeOfDeath, ATKer->name);

	char tmpMsg[200];

	sprintf(tmpMsg, "%s took %d damage to %s", ATKer->name, (ATKer->stats[_ATK] - DEFer->stats[_DEF] > 0 ? ATKer->stats[_ATK] - DEFer->stats[_DEF] : 0), DEFer->name);
	_enQueueSysMsg(pEnv->systemMsg, tmpMsg);
}
//--------------------------------------------------------------------------------
/*
	엔티티가 사망했는지 여부 체크
	@param	pEnt	: 엔티티의 포인터
	@return 사망했으면 TRUE, 사망하지 않았으면 FALSE 반환
*/
BOOL _isDead(PENT pEnt) {
	return pEnt->stats[_curHP] <= 0 ? TRUE : FALSE;
}
//--------------------------------------------------------------------------------
/*
	엔티티의 사망 처리
	@param	pEnv	: 현재 맵의 포인터
	@param	pPC		: 플레이어의 포인터
	@param	pEnt	: 사망한 엔티티의 포인터
*/
void _killEnt(PSYS_MESSAGE_Q systemMsg, PENV pEnv, PENT pPC, PENT pEnt) {
	pPC->stats[_curEXP] += pEnt->stats[_curEXP];
	pPC->stats[_WON] += pEnt->stats[_WON];

	pPC->stats[_killCNT]++;

	pEnv->cBuf[pEnv->dim.X * pEnt->pos.Y + pEnt->pos.X] = TRUE;

	char tmpMsg[200];

	sprintf(tmpMsg, "You defeated %s", pEnt->name);
	_enQueueSysMsg(systemMsg, tmpMsg);

	_updateEXP(pEnv, pPC);
	_dropListNode(&(pEnv->lEnti), pEnt);
}
//--------------------------------------------------------------------------------
/*
	플레이어 경험치 정보 갱신
	@param	pPC	: 플레이어의 포인터
*/
void _updateEXP(PENV pEnv, PENT pPC) {
	if (pPC->stats[_curEXP] >= pPC->stats[_maxEXP]) {
		pPC->stats[_LV] += 1;
		pPC->stats[_ATK] += 1;
		pPC->stats[_curHP] += 3;
		pPC->stats[_maxHP] += 3;
		pPC->stats[_curEXP] -= pPC->stats[_maxEXP];
		pPC->stats[_maxEXP] += 10;

		char tmpMsg[200];

		sprintf(tmpMsg, "You have reached level %d", pPC->stats[_LV]);
		_enQueueSysMsg(pEnv->systemMsg, tmpMsg);
	}
}
//--------------------------------------------------------------------------------
/*
	플레이어 점수 갱신
	@param	pRL : 로그라이크 구조체의 포인터
	점수 = (레벨*100) + (몬스터 처치 횟수*100) + (재화*100) + (클리어한 층수*1000)
*/
void _updateScore(PRL pRL) {
	pRL->pPC->stats[_SCORE] = ((pRL->pPC->stats[_LV] - 1) * 10) + (pRL->pPC->stats[_killCNT] * 10) + (pRL->pPC->stats[_WON] * 10) + (pRL->curIdx * 100);
}
//--------------------------------------------------------------------------------
/*
	shows ENTITY struct
	@param	hCSB	: handle of a console screen buffer
	@param	pEnt	: pointer to ENT struct
*/
void _showEnt(HANDLE hCSB, PENT pEnt)
{
	/*
	USHORT r = 5;
	for (SHORT y = pEnt->pos.Y - r; y < pEnt->pos.Y + r; y++)
	{
		for (SHORT x = pEnt->pos.X - r; x < pEnt->pos.X + r; x++)
		{
			if (((SHORT)pow(y - pEnt->pos.Y, 2)) + ((SHORT)pow(x - pEnt->pos.X, 2)) < ((SHORT)pow(r, 2))-1)
			{
				_setCursorPos(hCSB, 2 * x, y);
				_putwch(L'▲');
			}
		}
	}
	*/

	SetConsoleTextAttribute(hCSB, pEnt->rep.attr);
	_setCursorPos(hCSB, 2 * pEnt->pos.X, pEnt->pos.Y);
	_putwch(pEnt->rep.icon);
	_setCursorPos(hCSB, 0, 0);
}
//--------------------------------------------------------------------------------
/*
	frees ENTITY struct
	@param	pEnt	: pointer to an ENTITY struct
*/
void _freeEnt(PENT pEnt)
{
	if (pEnt != NULL)
	{
		for (USHORT i = 0; i < 10; i++)
			_freeItem(pEnt->items[i]);
		free(pEnt);
	}
}

PBOSS _makeBoss()
{
	PBOSS result = NULL;

	if ((result = (PENT)calloc(1, sizeof(ENT))) != NULL)
	{
		result->attck_loc.X = 0;
		result->attck_loc.Y = 0;
		result->pos.X = 0;
		result->pos.Y = 0;
		result->HP = 100;
		result->attakCnt = 0;
		result->bossTurn = 0;
		result->atk_ready = FALSE;
		result->rep.attr = _FG;
		result->rep.icon = ICON_BOSS;
		result->oil = FALSE;
	}

	return result;
}

void _bossPatern1(HANDLE hCSB, PRL pRL)
{
	SRECT win = { 0, 0, 0, 0 };
	win.T = pRL->pPC->pos.Y - (GUI_ENV_W / 2);
	win.L = pRL->pPC->pos.X - (GUI_ENV_W / 2);
	win.B = win.T + GUI_ENV_W;
	win.R = win.L + GUI_ENV_W;

	COORD d = { 0, 0 };
	if (win.T < 0)
		d.Y = 0 - win.T;
	if (win.L < 0)
		d.X = 0 - win.L;
	if (win.B > pRL->curEnv->dim.Y)
		d.Y = pRL->curEnv->dim.Y - win.B;
	if (win.R > (pRL->curEnv->dim.X - 1))	// '-1' is for newline
		d.X = pRL->curEnv->dim.X - win.R - 1;

	win.T += d.Y;
	win.L += d.X;
	win.B += d.Y;
	win.R += d.X;

	COORD pos = { 0,0 };
	if (pRL->pBOSS->bossTurn != 0 && pRL->pBOSS->bossTurn % 10 == 0) // 14턴마다 현재 pc좌표를 기준으로 4X4 위험알림 (노랑색)
	{
		pRL->pBOSS->attck_loc = pRL->pPC->pos;
		for (int y = pRL->pBOSS->attck_loc.Y - 3; y <= pRL->pBOSS->attck_loc.Y + 3; y++)
		{
			for (int x = pRL->pBOSS->attck_loc.X - 3; x <= pRL->pBOSS->attck_loc.X + 3; x++)
			{
				COORD tp = { x,y };
				if (_coordInSrect(tp, win))
				{
					pos.Y = (pRL->pGUI->rEnv.T + (y - win.T));
					pos.X = (pRL->pGUI->rEnv.L + (x - win.L));

					SetConsoleTextAttribute(hCSB, _FY);
					_setCursorPos(hCSB, 2 * pos.X, pos.Y);
					_putwch(BOSS_ATTACK);
				}
			}
		}

		pRL->pBOSS->bossTurn = 0;
		pRL->pBOSS->atk_ready = TRUE;
		pRL->pBOSS->attakCnt = 0;
	}


	if (pRL->pBOSS->atk_ready == TRUE)
	{
		char tmpMsg[200];
		sprintf(tmpMsg, "Yellow Area Damage in count : %d", 5 - pRL->pBOSS->attakCnt);
		PENV nextEnv = _getListNodeI(&(pRL->lEnv), 4)->pVal;
		_enQueueSysMsg(nextEnv->systemMsg, tmpMsg);

		if (pRL->pBOSS->attakCnt != 0 && pRL->pBOSS->attakCnt % 5 == 0) // 위험알림후 7턴뒤에 해당지역 데미지
		{
			for (int y = pRL->pBOSS->attck_loc.Y - 3; y <= pRL->pBOSS->attck_loc.Y + 3; y++)
			{
				for (int x = pRL->pBOSS->attck_loc.X - 3; x <= pRL->pBOSS->attck_loc.X + 3; x++)
				{
					COORD tp = { x,y };
					if (_coordInSrect(tp, win))
					{
						pos.Y = (pRL->pGUI->rEnv.T + (y - win.T));
						pos.X = (pRL->pGUI->rEnv.L + (x - win.L));

						SetConsoleTextAttribute(hCSB, _FC | FOREGROUND_INTENSITY);
						_setCursorPos(hCSB, 2 * pos.X, pos.Y);
						_putwch(BOSS_ATTACK);

					}
					if (pRL->pPC->pos.X == x && pRL->pPC->pos.Y == y)
						pRL->pPC->HIT = TRUE;
				}
			}
			if (pRL->pPC->HIT == TRUE)	pRL->pPC->stats[_curHP] -= 5;

			pRL->pBOSS->attakCnt = 0;
			pRL->pBOSS->atk_ready = FALSE;
		}

		pRL->pBOSS->attakCnt++;
	}
}

void _bossPatern2(PRL pRL) {
	if (pRL->pBOSS->bossTurn % 15 != 0) return;

	PENV pEnv = pRL->curEnv;
	PSRECT room = &(pEnv->lRoom).head->pVal;

	COORD pos = { 0, 0 };
	USHORT random = (USHORT)(rand() % 7);
	switch (random) {
	case 0:
		pos.X = pRL->pBOSS->pos.X - 5;
		pos.Y = pRL->pBOSS->pos.Y - 5;
		break;
	case 1:
		pos.X = pRL->pBOSS->pos.X - 5;
		pos.Y = pRL->pBOSS->pos.Y;
		break;
	case 2:
		pos.X = pRL->pBOSS->pos.X - 5;
		pos.Y = pRL->pBOSS->pos.Y + 5;
		break;
	case 3:
		pos.X = pRL->pBOSS->pos.X - 35;
		pos.Y = pRL->pBOSS->pos.Y - 8;
		break;
	case 4:
		pos.X = pRL->pBOSS->pos.X - 35;
		pos.Y = pRL->pBOSS->pos.Y + 8;
		break;
	case 5:
		pos.X = pRL->pBOSS->pos.X - 80;
		pos.Y = pRL->pBOSS->pos.Y - 10;
		break;
	case 6:
		pos.X = pRL->pBOSS->pos.X - 80;
		pos.Y = pRL->pBOSS->pos.Y + 10;
		break;
	}

	USHORT rnd = (USHORT)(rand() % (NUM_ENTITY_SPECIES * ((pEnv->curIdx) + 1)));
	PLNODE tmp = _getListNodeI(&(pEnv->lOriginEnti), rnd);
	_duplicateEnt(&(pEnv->lEnti), tmp->pVal, pos);
	pEnv->cBuf[pEnv->dim.X * (pos.Y) + (pos.X)] = FALSE;
}
//--------------------------------------------------------------------------------
/*
	TODO: write description
*/
//--------------------------------------------------------------------------------