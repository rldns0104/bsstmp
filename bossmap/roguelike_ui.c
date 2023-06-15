#include "roguelike_ui.h"

//--------------------------------------------------------------------------------
/*
	handles player character
	■param	pRoguelike	: pointer to ROGUELIKE struct
	■return 행동을 취했으면 TRUE, 아니라면 FALSE 반환
*/
BOOL _handlePC(PRL pRL, HANDLE hCSB)
{
	COORD d = { 0, 0 };
	PVOID pVal = NULL;
	PITEM pItem = NULL;
	BOOL isMoved = FALSE;
	USHORT pressKey = 0;
	USHORT itemNum;
	char tmpMsg[200];

	pressKey = _getch();
	switch (pressKey)
	{
	case(INPUT_MOVE_U): { d.Y = -1; isMoved = TRUE; break; }
	case(INPUT_MOVE_L): { d.X = -1; isMoved = TRUE; break; }
	case(INPUT_MOVE_D): { d.Y = +1; isMoved = TRUE; break; }
	case(INPUT_MOVE_R): { d.X = +1; isMoved = TRUE; break; }
	case(INPUT_DELETE):
	{
		short tmpX = 2;

		short len = 29;

		_setCursorPos(hCSB, tmpX, GUI_ENV_H + 4);
		SetConsoleTextAttribute(hCSB, _FW);
		printf("　　               ");

		tmpX = (short)(GUI_ENV_W + 1 - len / 2) - 2;

		_setCursorPos(hCSB, tmpX, GUI_ENV_H + 4);
		printf("Press the number of the item");
		printf("          ");
		_setCursorPos(hCSB, 0,0);
		itemNum = _getwch();
		_putDownItem(pRL, itemNum - L'0');
		break;
	}
	case(L'1'):case(L'2'):case(L'3'):
	case(L'4'):case(L'5'):case(L'6'):
	case(L'7'):case(L'8'):case(L'9'):
	{
		itemNum = pressKey;
		_useItem(pRL, itemNum - L'0');
		break;
	}
	case(INPUT_SHOP_1): {
		if (pRL->curEnv->shop[0]) {
			if (pRL->curEnv->shop[0]->price <= pRL->pPC->stats[_WON]) {
				pRL->pPC->stats[_WON] -= pRL->curEnv->shop[0]->price;
				_equipItem(pRL->curEnv, pRL->pPC, pRL->curEnv->shop[0]->pItem);
				pRL->curEnv->shop[0] = NULL;
				
			}
			else {
				sprintf(tmpMsg, "You don't have enough money");
			}
			_enQueueSysMsg(pRL->curEnv->systemMsg, tmpMsg);
		}
		break;
	}
	case(INPUT_SHOP_2): {
		if (pRL->curEnv->shop[1]) {
			if (pRL->curEnv->shop[1]->price <= pRL->pPC->stats[_WON]) {
				pRL->pPC->stats[_WON] -= pRL->curEnv->shop[1]->price;
				_equipItem(pRL->curEnv, pRL->pPC, pRL->curEnv->shop[1]->pItem);
				pRL->curEnv->shop[1] = NULL;
			}
			else {
				sprintf(tmpMsg, "You don't have enough money");
			}
			_enQueueSysMsg(pRL->curEnv->systemMsg, tmpMsg);
		}
		break;
	}
	case(INPUT_SHOP_3): {
		if (pRL->curEnv->shop[2]) {
			if (pRL->curEnv->shop[2]->price <= pRL->pPC->stats[_WON]) {
				pRL->pPC->stats[_WON] -= pRL->curEnv->shop[2]->price;
				_usePotion(pRL->curEnv, pRL->pPC, pRL->curEnv->shop[2]->pItem);
				pRL->curEnv->shop[2] = NULL;
			}
			else {
				sprintf(tmpMsg, "You don't have enough money");
			}
			_enQueueSysMsg(pRL->curEnv->systemMsg, tmpMsg);
		}
		break;
	}
	case(27): { free(pRL->pPC); pRL->pPC = NULL; isMoved = FALSE; return; }
	}
	if (isMoved)
	{
		switch (_moveEnt(pRL->pPC, pRL->curEnv, pRL->pPC, d.X, d.Y, &pVal))
		{
		case _NIL:
			pItem = _getOverlapItem(pRL->curEnv, pRL->pPC);
			if (pItem != NULL)
				_addInven(pRL, pItem);
			break;
		case _ENT:
			_attackEnt(pRL->curEnv, pRL->pPC, pVal);
			if (_isDead(pVal)) {
				_killEnt(pRL->curEnv->systemMsg, pRL->curEnv, pRL->pPC, pVal);
			}
			break;
		case _COL:
			break;
		}
	}
	return isMoved;

}
//--------------------------------------------------------------------------------
/*
	makes GUI struct
	■return	pointer to GUI struct
*/
PGUI _makeGUI()
{
	PGUI result = NULL;

	if ((result = (PGUI)calloc(1, sizeof(GUI))) != NULL)
	{
		result->dim.X = GUI_W + 1;	// '+1' is for newline
		result->dim.Y = GUI_H;

		result->rEnv.T = 1;
		result->rEnv.L = 1;
		result->rEnv.B = result->rEnv.T + GUI_ENV_H;
		result->rEnv.R = result->rEnv.L + GUI_ENV_W;

		result->rEnt.T = 1;
		result->rEnt.L = result->rEnv.R + 1;
		result->rEnt.B = result->rEnt.T + GUI_ENV_H;
		result->rEnt.R = result->rEnt.L + GUI_ENT_W;

		result->rSys.T = result->rEnv.B + 1;
		result->rSys.L = 1;
		result->rSys.B = result->rSys.T + GUI_SYS_H;
		result->rSys.R = result->rSys.L + GUI_SYS_W;

		result->attr = _FW;
		result->gBuf = (PWCHAR)calloc(result->dim.X * result->dim.Y, sizeof(WCHAR));

		if (result->gBuf != NULL)
		{
			wmemset(result->gBuf, L'▩', result->dim.X * result->dim.Y);
			for (USHORT y = 0; y < result->dim.Y; y++)
				result->gBuf[result->dim.X * (y)+(result->dim.X - 1)] = L'\n';

			// clear area for ENV
			for (USHORT y = result->rEnv.T; y < result->rEnv.B; y++)
				for (USHORT x = result->rEnv.L; x < result->rEnv.R; x++)
					result->gBuf[result->dim.X * (y)+(x)] = ICON_VOID;
			// clear area for ENT
			for (USHORT y = result->rEnt.T; y < result->rEnt.B; y++)
				for (USHORT x = result->rEnt.L; x < result->rEnt.R; x++)
					result->gBuf[result->dim.X * (y)+(x)] = ICON_VOID;
			// clear area for SYS
			for (USHORT y = result->rSys.T; y < result->rSys.B; y++) {
				for (USHORT x = result->rSys.L; x < result->rSys.R; x++) {
					if (x == GUI_ENV_W + 1)
						continue;
					result->gBuf[result->dim.X * (y)+(x)] = ICON_VOID;
				}

			}

		}
		else
		{
			_freeGUI(result);
			return NULL;
		}
	}
	return result;
}
//--------------------------------------------------------------------------------
/*
_fillGUI() 삭제, 모든 것을 _showGUI()에서 처리
*/
//--------------------------------------------------------------------------------
/*
	shows GUI struct
	■param	hCSB	: console screen buffer handle
	■param	pRL		: pointer to ROGUELIKE struct
*/
void _showGUI(HANDLE hCSB, PRL pRL)
{
	if (hCSB != INVALID_HANDLE_VALUE && pRL != NULL)
	{
		// set console window & screen buffer size
		// winDim MUST BE SMALLER THAN bufDim, otherwise SetConsoleWindowInfo() function fails
		// https://docs.microsoft.com/en-us/windows/console/setconsolewindowinfo
		SMALL_RECT	winDim = { 0, 0, (2 * pRL->pGUI->dim.X), (pRL->pGUI->dim.Y) };
		COORD		bufDim = { (2 * pRL->pGUI->dim.X) + 1, (pRL->pGUI->dim.Y) + 1 };
		SetConsoleWindowInfo(hCSB, TRUE, &winDim);
		SetConsoleScreenBufferSize(hCSB, bufDim);

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


		for (USHORT y = 0; y < GUI_ENV_H; y++)
		{
			wmemcpy(
				&(pRL->pGUI->gBuf[pRL->pGUI->dim.X * (pRL->pGUI->rEnt.T + y) + (pRL->pGUI->rEnv.L)]),
				&(pRL->curEnv->gBuf[pRL->curEnv->dim.X * (win.T + y) + (win.L)]),
				GUI_ENV_H);
		}

		if (pRL->pPC->HIT == TRUE)
		{
			SetConsoleTextAttribute(hCSB, _FR);
			WriteConsoleW(hCSB, pRL->pGUI->gBuf, pRL->pGUI->dim.X * pRL->pGUI->dim.Y, NULL, NULL);
			pRL->pPC->HIT = FALSE;
			Sleep(64);
		}

		SetConsoleTextAttribute(hCSB, pRL->pGUI->attr);
		WriteConsoleW(hCSB, pRL->pGUI->gBuf, pRL->pGUI->dim.X * pRL->pGUI->dim.Y, NULL, NULL);

		PITEM	pI = NULL;
		PENT	pE = NULL;
		COORD	pos = { 0, 0 };

		// items
		for (PLNODE tmp = pRL->curEnv->lItem.head; tmp != NULL; tmp = tmp->next)
		{
			if ((pI = tmp->pVal) != NULL && _coordInSrect(pI->pos, win))
			{
				pos.Y = (pRL->pGUI->rEnv.T + (pI->pos.Y - win.T));
				pos.X = (pRL->pGUI->rEnv.L + (pI->pos.X - win.L));
				SetConsoleTextAttribute(hCSB, pI->color);
				_setCursorPos(hCSB, 2 * pos.X, pos.Y);
				_putwch(pI->icon);
			}
		}

		//// boss
		if (pRL->curIdx == 4)
		{
			PSRECT boss = _getListNodeI(&(pRL->curEnv->lRoom), 0)->pVal;
			pRL->pBOSS->pos.X = boss->R - 4;
			pRL->pBOSS->pos.Y = (boss->T + boss->B) / 2.0;
			_showBoss(hCSB, pRL);
			_bossPatern1(hCSB, pRL);
			_bossPatern2(pRL);
			pRL->pBOSS->bossTurn++;
		}

		// entities
		for (PLNODE tmp = pRL->curEnv->lEnti.head; tmp != NULL; tmp = tmp->next)
		{
			if ((pE = tmp->pVal) != NULL && _coordInSrect(pE->pos, win))
			{
				pos.Y = (pRL->pGUI->rEnv.T + (pE->pos.Y - win.T));
				pos.X = (pRL->pGUI->rEnv.L + (pE->pos.X - win.L));
				SetConsoleTextAttribute(hCSB, pE->rep.attr);
				_setCursorPos(hCSB, 2 * pos.X, pos.Y);
				_putwch(pE->rep.icon);
			}
		}
		


		// player character
		pos.Y = (pRL->pGUI->rEnv.T + (pRL->pPC->pos.Y - win.T));
		pos.X = (pRL->pGUI->rEnv.L + (pRL->pPC->pos.X - win.L));
		SetConsoleTextAttribute(hCSB, pRL->pPC->rep.attr);
		_setCursorPos(hCSB, 2 * pos.X, pos.Y);
		_putwch(pRL->pPC->rep.icon);

		_setCursorPos(hCSB, 0, 0);
	}
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
/*
	frees GUI struct
	■param	pGUI	: pointer to GUI struct
*/
void _freeGUI(PGUI pGUI)
{
	if (pGUI != NULL)
	{
		free(pGUI->gBuf);
		free(pGUI);
	}
}
//--------------------------------------------------------------------------------
/*
	TODO: write description
*/
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
/*
	TODO: write description
*/
//--------------------------------------------------------------------------------

PSYS_MESSAGE_Q _initSysMsg() {
	PSYS_MESSAGE_Q result;

	if ((result = (PSYS_MESSAGE_Q)calloc(1, sizeof(SYS_MESSAGE_Q))) != NULL) {
		result->front = result->rear = NULL;
		result->cnt = 0;
	}
	return result;
}

void _enQueueSysMsg(PSYS_MESSAGE_Q systemMsg, PCHAR msg) {
	PSYS_MESSAGE newNode;

	if ((newNode = (PSYS_MESSAGE)calloc(1, sizeof(SYS_MESSAGE))) != NULL) {
		strcpy(newNode->message, msg);
		newNode->next = NULL;
	}

	if (systemMsg->rear == NULL) {
		systemMsg->front = systemMsg->rear = newNode;
		systemMsg->cnt++;
		return;
	}

	systemMsg->rear->next = newNode;
	systemMsg->rear = newNode;
	systemMsg->cnt++;
}

void _printSysMsg(PENV pEnv, HANDLE hCSB) {
	if (pEnv->systemMsg->front == NULL)
		return;


	PSYS_MESSAGE tmp = pEnv->systemMsg->front;
	BOOL printed = FALSE;
	USHORT input;
	short tmpX = 2;

	for (int i = 0; i < pEnv->systemMsg->cnt; i++) {
		printed = FALSE;
		tmp = pEnv->systemMsg->front;
		pEnv->systemMsg->front = pEnv->systemMsg->front->next;
		if (pEnv->systemMsg->front == NULL) {
			pEnv->systemMsg->rear = NULL;
		}

		short len = strlen(tmp->message);

		_setCursorPos(hCSB, tmpX, GUI_ENV_H + 4);
		SetConsoleTextAttribute(hCSB, _FW);
		printf("　　               ");

		tmpX = (short)(GUI_ENV_W + 1 - len / 2) - 2;


		_setCursorPos(hCSB, tmpX, GUI_ENV_H + 4);
		printf(" %s", tmp->message);
		printf("          ");
		free(tmp);


		while (printed == FALSE) {
			for (USHORT j = 0; j < INPUT_CHECKS_PER_SEC; j++) {
				if (_kbhit()) {
					
					input = _getwch();
					if (input == L' ' || input == L'w' || input == L'a' || input == L's' || input == L'd') {
					printed = TRUE;
					
				}


				}
			}
		}
	}
	pEnv->systemMsg->cnt = 0;
	_setCursorPos(hCSB, 0, 0);
}

void _showLeaderBoard(HANDLE hCSB) {
	FILE* fp = NULL;

	int x, y;

	system("cls");

	SMALL_RECT winSize = { 0, 0, (50), (25) };
	SetConsoleWindowInfo(hCSB, TRUE, &winSize);

	for (y = 0; y <= 25; y++) {
		_setCursorPos(hCSB, 0, y);
		_putwch('#');

		_setCursorPos(hCSB, 50, y);
		_putwch('#');
	}

	for (x = 0; x < 50; x++) {
		_setCursorPos(hCSB, x, 0);
		_putwch('#');

		_setCursorPos(hCSB, x, 25);
		_putwch('#');
	}


	if (!fopen_s(&fp, "Leaderboard.txt", "rb")) {
		y = 0;
		while (!feof(fp)) {
			char buffer[2000] = { 0 };
			fgets(buffer, 2000, fp);
			_printCenter(hCSB, winSize, buffer, 8 + y++);
		}

		fclose(fp);
	}


	_printCenter(hCSB, winSize, "-Press space to continue-", 23);

	_setCursorPos(hCSB, 0, 0);

	BOOL isOK = FALSE;
	while (!isOK) {
		for (USHORT i = 0; i < INPUT_CHECKS_PER_SEC && !isOK; i++) {
			if (_kbhit()) {
				USHORT input = _getwch();

				if (input == ' ')
					isOK = TRUE;
			}
		}
	}
	return;
}

void _showStartMenu(HANDLE hCSB, PRL pRL) {
	int x, y = 0;


	BOOL gameStarted = FALSE;



	while (!gameStarted) {

		system("cls");

		SMALL_RECT winSize = { 0, 0, (80), (20) };
		SetConsoleWindowInfo(hCSB, TRUE, &winSize);

		BOOL playerMoved = FALSE;
		BOOL isSelected = FALSE;

		SHORT menu = 0;
		SHORT check = 0;

		_setCursorPos(hCSB, 35, 3);
		printf("Game Start");

		_setCursorPos(hCSB, 35, 8);
		printf("Leaderboard");

		_setCursorPos(hCSB, 35, 13);
		printf("Tutorial");

		_setCursorPos(hCSB, 35, 18);
		printf("Game End");

		_setCursorPos(hCSB, 30, 3);
		_putwch(L'▶');

		_setCursorPos(hCSB, 0, 0);

		while (!isSelected) {

			for (USHORT i = 0; i < INPUT_CHECKS_PER_SEC; i++)
			{
				if (_kbhit())
				{
					switch (_getwch())
					{
					default: { 		break; }
					case(INPUT_MOVE_U):
						playerMoved = TRUE;
						if (menu != 0) {
							menu--;

							check -= 5;

						}
						break;
					case(INPUT_MOVE_D):
						playerMoved = TRUE;
						if (menu != 3) {
							menu++;
							check += 5;
						}
						break;
					case(L' '): isSelected = TRUE;
					}
					if (playerMoved) {
						_setCursorPos(hCSB, 30, 3);
						printf("  ");

						_setCursorPos(hCSB, 30, 8);
						printf("  ");

						_setCursorPos(hCSB, 30, 13);
						printf("  ");

						_setCursorPos(hCSB, 30, 18);
						printf("  ");

						_setCursorPos(hCSB, 30, 3 + check);
						_putwch(L'▶');

					}
				}
				Sleep(1000 / INPUT_CHECKS_PER_SEC);
			}
		}
		switch (menu) {
		case 0:
			isSelected = FALSE;
			gameStarted = TRUE;

			strcpy(pRL->pPC->name, _readPCName(hCSB));
			getchar();
			break;
		case 1:
			isSelected = FALSE;
			_showLeaderBoard(hCSB);

			system("cls");

			break;
		case 2:
			isSelected = FALSE;
			_showTutorial(hCSB);

			system("cls");
			break;
		case 3:
			exit(1);
		}
	}
	return;
}

void _showTutorial(HANDLE hCSB) {

	int x, y;


	system("cls");


	SMALL_RECT winSize = { 0, 0, (80), (20) };
	SetConsoleWindowInfo(hCSB, TRUE, &winSize);

	for (y = 0; y <= 20; y++) {
		_setCursorPos(hCSB, 0, y);
		_putwch('#');

		_setCursorPos(hCSB, 80, y);
		_putwch('#');
	}

	for (x = 0; x < 80; x++) {
		_setCursorPos(hCSB, x, 0);
		_putwch('#');

		_setCursorPos(hCSB, x, 20);
		_putwch('#');
	}

	_setCursorPos(hCSB, 20, 2);
	printf("Move/Attack	: W A S D");

	_setCursorPos(hCSB, 20, 6);
	printf("Item Use	: 1~9");

	_setCursorPos(hCSB, 20, 10);
	printf("Item Drop	: Q");

	_setCursorPos(hCSB, 20, 14);
	printf("Interaction : Space");

	_setCursorPos(hCSB, 20, 18);
	printf("Buy Shop Item : I O P");

	_setCursorPos(hCSB, 56, 19);
	printf("Press Space to continue");

	BOOL isOK = FALSE;
	while (!isOK) {
		for (USHORT i = 0; i < INPUT_CHECKS_PER_SEC && !isOK; i++) {
			if (_kbhit()) {
				USHORT input = _getwch();

				if (input == ' ')
					isOK = TRUE;
			}
		}
	}
	return;
}

void _showInventory(HANDLE hCSB, PENT pPC)
{
	// SHORT xt = UI_BASIC_WIDTH;
	// SHORT yt = INVENTORY_UI_BASIC_HEIGHT;
	// SetConsoleTextAttribute(hCSB, _FW);
	// _setCursorPos(hCSB, xt, yt);
	// printf("=======Inventory======");

	// SHORT invenNum = 1;
	// for (int i = 0; i < 2; i++)
	// {
	// 	for (int j = 0; j < 5; j++)
	// 	{
	// 		_setCursorPos(hCSB, xt, yt + (j * 2 + 2));
	// 		printf("%d.", invenNum);

	// 		if (pPC->items[invenNum]->pItem != NULL)
	// 		{
	// 			printf("%s", pPC->items[invenNum]->pItem->name);
	// 			printf("(%d)", pPC->items[invenNum]->num);
	// 		}
	// 		invenNum++;
	// 		if (invenNum == 10)
	// 			break;
	// 	}
	// 	xt += 12;
	// }
	// _setCursorPos(hCSB, 0, 0);
}
void _showPlayerInfo(HANDLE hCSB, PENT pPC)
{
	// SHORT xt = UI_BASIC_WIDTH;
	// SHORT yt = STAT_UI_BASIC_HEIGHT;
	// SetConsoleTextAttribute(hCSB, _FW);
	// _setCursorPos(hCSB, xt, yt);
	// printf("======PlayerInfo======");

	// for (int i = 0; i < NUM_STATS; i++)
	// {
	// 	_setCursorPos(hCSB, xt, yt + (i * 2 + 2));
	// 	switch (i)
	// 	{
	// 	case 0:
	// 		printf("LV.%d", pPC->stats[_LV]);
	// 		break;
	// 	case 1:
	// 		printf("HP = %d/%d", pPC->stats[_curHP], pPC->stats[_maxHP]);
	// 		break;
	// 	case 2:
	// 		printf("ATK = %d (+%d)", pPC->stats[_ATK] - pPC->stats[_tmpATK], pPC->stats[_tmpATK]);
	// 		break;
	// 	case 3:
	// 		printf("DEF = %d (+%d)", pPC->stats[_DEF] - pPC->stats[_tmpDEF], pPC->stats[_tmpDEF]);
	// 		break;
	// 	case 4:
	// 		printf("EXP = %d/%d", pPC->stats[_curEXP], pPC->stats[_maxEXP]);
	// 		break;
	// 	case 5:
	// 		printf("WON = %d", pPC->stats[_WON]);
	// 		break;
	// 	}
	// }
	// _setCursorPos(hCSB, 0, 0);
}

void _showScore(HANDLE hCSB, PRL pRL)
{
	// SetConsoleTextAttribute(hCSB, _FW);
	// SHORT xt = UI_BASIC_WIDTH;
	// SHORT yt = 1;

	// _setCursorPos(hCSB, xt, yt);
	// printf("Score = %d ST = %d", pRL->pPC->stats[_SCORE], pRL->curIdx + 1); //점수가 999,999를 넘을 때 글씨가 UI를 뚫는 것을 미연에 방지


	// _setCursorPos(hCSB, 0, 0);
}



void _writeLeaderBoard(PRL pRL, BOOL isCleared) {
	FILE* fp = NULL;

	int index = 0;
	char original[11][200] = { 0 };
	int scores[11] = { 0 };
	char tmpText[200] = { 0 };
	int cnt = 1;
	int tmp = 0;


	if (!fopen_s(&fp, "Leaderboard.txt", "rb")) {
		while (!feof(fp)) {
			char buffer[200] = { 0 };
			char* ptr;

			fgets(buffer, 200, fp);
			if (strlen(buffer) < 2)
				break;
			strcpy(original[index], buffer);

			ptr = strtok(buffer, ":");
			ptr = strtok(NULL, " ");
			scores[index] = atoi(ptr);
			if (cnt == 10)
				break;
			index++;
			cnt++;
		}
		fclose(fp);
	}
	else {
		fopen_s(&fp, "Leaderboard.txt", "wb");

		if (isCleared) {
			sprintf(tmpText, "%s: %d Saved the world!\n", pRL->pPC->name, pRL->pPC->stats[_SCORE]);
		}
		else {
			sprintf(tmpText, "%s: %d died on stage %d by %s\n", pRL->pPC->name, pRL->pPC->stats[_SCORE], pRL->curIdx + 1, pRL->pPC->causeOfDeath);
		}
		fwrite(tmpText, 1, strlen(tmpText), fp);
		fclose(fp);
		return;

	}

	if (isCleared) {
		sprintf(tmpText, "%s: %d Saved the world!\n", pRL->pPC->name, pRL->pPC->stats[_SCORE]);
	}
	else {
		sprintf(tmpText, "%s: %d died on stage %d by %s\n", pRL->pPC->name, pRL->pPC->stats[_SCORE], pRL->curIdx + 1, pRL->pPC->causeOfDeath);
	}
	if (cnt == 10) {
		strcpy(original[cnt], tmpText);
		scores[cnt] = pRL->pPC->stats[_SCORE];

		for (int i = 0; i < cnt; i++) {
			for (int j = i + 1; j < cnt + 1; j++) {
				if (scores[i] < scores[j]) {
					tmp = scores[i];
					scores[i] = scores[j];
					scores[j] = tmp;
					strcpy(tmpText, original[i]);
					strcpy(original[i], original[j]);
					strcpy(original[j], tmpText);
				}
			}
		}
	}
	else {
		strcpy(original[index], tmpText);
		scores[index] = pRL->pPC->stats[_SCORE];

		for (int i = 0; i < index; i++) {
			for (int j = i + 1; j < index + 1; j++) {
				if (scores[i] < scores[j]) {
					tmp = scores[i];
					scores[i] = scores[j];
					scores[j] = tmp;
					strcpy(tmpText, original[i]);
					strcpy(original[i], original[j]);
					strcpy(original[j], tmpText);
				}
			}
		}
	}




	if (!fopen_s(&fp, "Leaderboard.txt", "wb")) {
		for (int i = 0; i < cnt; i++) {
			fwrite(original[i], 1, strlen(original[i]), fp);
		}
		fclose(fp);
	}
	return;
}


void _showShop(HANDLE hCSB, PRL pRL) {

	if (pRL->curEnv->shop[0]) {
		SetConsoleTextAttribute(hCSB, _FW);
		_setCursorPos(hCSB, (UI_BASIC_WIDTH), GUI_ENV_H + 3);
		printf(" %8s : %3d\\ (i)", pRL->curEnv->shop[0]->pItem->name, pRL->curEnv->shop[0]->price);
	}

	if (pRL->curEnv->shop[1]) {
		_setCursorPos(hCSB, (UI_BASIC_WIDTH), GUI_ENV_H + 4);
		printf(" %8s : %3d\\ (o)", pRL->curEnv->shop[1]->pItem->name, pRL->curEnv->shop[1]->price);

	}

	if (pRL->curEnv->shop[2]) {
		_setCursorPos(hCSB, (UI_BASIC_WIDTH), GUI_ENV_H + 5);
		printf(" %8s : %3d\\ (p)", pRL->curEnv->shop[2]->pItem->name, pRL->curEnv->shop[2]->price);
	}

	_setCursorPos(hCSB, 0, 0);
}

PCHAR _readPCName(HANDLE hCSB) {
	char name[200];

	system("cls");

	_setCursorPos(hCSB, 22, 9);
	printf("Enter User Name : ");

	scanf("%s", name);

	return name;
}

void _showBoss(HANDLE hCSB, PRL pRL)
{
	USHORT bossSizeX = 2;
	USHORT bossSizeY = 4;
	if (pRL->pBOSS->HP == 0) {
		COORD pos = { 0,0 };
		COORD bPos = { 0,0 };
		for (int j = pRL->pBOSS->pos.Y - bossSizeY; j <= pRL->pBOSS->pos.Y + bossSizeY; j++) {
			for (int i = pRL->pBOSS->pos.X - bossSizeX; i <= pRL->pBOSS->pos.X + bossSizeX; i++) {
				pRL->curEnv->cBuf[pRL->curEnv->dim.X * j + i] = TRUE;
			}
		}
		return;
	}

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
	COORD bPos = { 0,0 };
	for (int j = pRL->pBOSS->pos.Y - bossSizeY; j <= pRL->pBOSS->pos.Y + bossSizeY; j++)
	{
		for (int i = pRL->pBOSS->pos.X - bossSizeX; i <= pRL->pBOSS->pos.X + bossSizeX; i++)
		{
			bPos.X = i;
			bPos.Y = j;
			if (_coordInSrect(bPos, win))
			{
				pRL->curEnv->cBuf[pRL->curEnv->dim.X * j + i] = FALSE;
				if (i == pRL->pBOSS->pos.X && j == pRL->pBOSS->pos.Y)
				{
					pos.X = (pRL->pGUI->rEnv.L + (i - win.L));
					pos.Y = (pRL->pGUI->rEnv.T + (j - win.T));
					SetConsoleTextAttribute(hCSB, _FR);
					_setCursorPos(hCSB, 2 * pos.X, pos.Y);
					printf("BS");
				}
				else if (i == pRL->pBOSS->pos.X - bossSizeX || j == pRL->pBOSS->pos.Y - bossSizeY || j == pRL->pBOSS->pos.Y + bossSizeY)
				{
					pos.X = (pRL->pGUI->rEnv.L + (i - win.L));
					pos.Y = (pRL->pGUI->rEnv.T + (j - win.T));
					SetConsoleTextAttribute(hCSB, pRL->pBOSS->rep.attr);
					_setCursorPos(hCSB, 2 * pos.X, pos.Y);
					_putwch(pRL->pBOSS->rep.icon);
				}
			}
		}
	}
}

void _showDeathScreen(HANDLE hCSB, PRL pRL) {
	int x, y;
	const CHAR tmpText[200];


	system("cls");

	SMALL_RECT winSize = { 0, 0, (50), (25) };
	SetConsoleWindowInfo(hCSB, TRUE, &winSize);

	for (y = 0; y <= 25; y++) {
		_setCursorPos(hCSB, 0, y);
		_putwch('#');

		_setCursorPos(hCSB, 50, y);
		_putwch('#');
	}

	for (x = 0; x < 50; x++) {
		_setCursorPos(hCSB, x, 0);
		_putwch('#');

		_setCursorPos(hCSB, x, 25);
		_putwch('#');
	}


	_printCenter(hCSB, winSize, "-----------------------",5);
	_printCenter(hCSB, winSize, "|                     |",6);
	_printCenter(hCSB, winSize, "|                     |",7);
	_printCenter(hCSB, winSize, "|                     |",8);
	_printCenter(hCSB, winSize, "|                     |",9);
	_printCenter(hCSB, winSize, "|                     |",10);
	_printCenter(hCSB, winSize, "|                     |",11);

	_printCenter(hCSB, winSize, "|                     |",12);
	_printCenter(hCSB, winSize, "|                     |",13);

	_printCenter(hCSB, winSize, "|                     |",14);

	_printCenter(hCSB, winSize, "|                     |", 15);
	_printCenter(hCSB, winSize, "|                     |", 16);
	_printCenter(hCSB, winSize, "|                     |", 17);
	_printCenter(hCSB, winSize, "|                     |",18);
	_printCenter(hCSB, winSize, "|                     |", 19);
	_printCenter(hCSB, winSize, "-------------------------------------",20);


	_printCenter(hCSB, winSize, "R.I.P", 9);

	sprintf(tmpText, "[%s]",  pRL->pPC->name);
	_printCenter(hCSB, winSize, tmpText, 11);

	_printCenter(hCSB, winSize, "Passed away by", 13);

	sprintf(tmpText, pRL->pPC->causeOfDeath);
	_printCenter(hCSB, winSize, tmpText, 14);

	time_t t = time(NULL);
	struct tm curTime = *localtime(&t);
	sprintf(tmpText, "%d/%d/%d", curTime.tm_year + 1900, curTime.tm_mon + 1, curTime.tm_mday);
	_printCenter(hCSB, winSize, tmpText, 16);	

	sprintf(tmpText, "Your Score: %d", pRL->pPC->stats[_SCORE]);
	_printCenter(hCSB, winSize, tmpText, 22);

	_printCenter(hCSB, winSize, "-Press space to continue-", 23);

	_setCursorPos(hCSB, 0, 0);

	BOOL isOK = FALSE;
	while (!isOK) {
		for (USHORT i = 0; i < INPUT_CHECKS_PER_SEC && !isOK; i++) {
			if (_kbhit()) {
				USHORT input = _getwch();

				if (input == ' ')
					isOK = TRUE;
			}
		}
	}

	return;
}

void _showClearScreen(HANDLE hCSB, PRL pRL) {

	CHAR tmpText[200];

	system("cls");


	SMALL_RECT winSize = { 0, 0, (80), (20) };
	SetConsoleWindowInfo(hCSB, TRUE, &winSize);


	_printCenter(hCSB, winSize, " OOOO   OOO   O   O   OOOO  OOOO     O   OOOOO  OOOO  O O", 4);
	_printCenter(hCSB, winSize, "O      O   O  OO  O  O      O   O   O O    O   O      O O", 5);
	_printCenter(hCSB, winSize, "O      O   O  O O O  O  OO  OOOO   OOOOO   O    OOO   O O", 6);
	_printCenter(hCSB, winSize, "O      O   O  O  OO  O   O  O  O   O   O   O       O     ", 7);
	_printCenter(hCSB, winSize, " OOOO   OOO   O   O   OOOO  O   O  O   O   O   OOOO   O O", 8);

	Sleep(3000);

	system("cls");

	_printCenter(hCSB, winSize, "O   O  OOO   O   O      O  O  O   OOO   O   O   O O", 4);
	_printCenter(hCSB, winSize, " O O  O   O  O   O      O  O  O  O   O  OO  O   O O", 5);
	_printCenter(hCSB, winSize, "  O   O   O  O   O      O  O  O  O   O  O O O   O O", 6);
	_printCenter(hCSB, winSize, "O   O   O  O   O      O  O  O  O   O  O  OO    ", 7);
	_printCenter(hCSB, winSize, "  O    OOO    OOO        OO OO    OOO   O   O   O O", 8);

	_printCenter(hCSB, winSize, "You saved the world!", 13);

	sprintf(tmpText, "[%s]", pRL->pPC->name);
	_printCenter(hCSB, winSize, tmpText, 14);
	sprintf(tmpText, "Score : %d ", pRL->pPC->stats[_SCORE]);
	_printCenter(hCSB, winSize, tmpText, 15);

	_printCenter(hCSB, winSize, "-Press space to continue-", 20);

	_setCursorPos(hCSB, 0, 0);

	BOOL isOK = FALSE;
	while (!isOK) {
		for (USHORT i = 0; i < INPUT_CHECKS_PER_SEC && !isOK; i++) {
			if (_kbhit()) {
				USHORT input = _getwch();

				if (input == ' ')
					isOK = TRUE;
			}
		}
	}

	return;
}

void _printCenter(HANDLE hCSB, SMALL_RECT winSize, const PCHAR msg, SHORT y) {
	_setCursorPos(hCSB, (SHORT)((winSize.Right+2 - strlen(msg)) / 2), y);
	printf("%s", msg);

	_setCursorPos(hCSB, 0, 0);
}