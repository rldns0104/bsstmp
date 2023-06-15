#include "roguelike_env.h"

//--------------------------------------------------------------------------------
/*
	makes ENVIRONS struct - must be freed by _freeEnvirons() function
	@param	dim		: dimension of ENV
	@param	iter	: number of recursive call(s)
*/
PENV _makeEnv(COORD dim, WORD attr, SHORT iter, SHORT curIdx)
{
	PENV result = NULL;
	if (_areParamsValid(dim, iter))
	{
		if ((result = (PENV)calloc(1, sizeof(ENV))) != NULL)
		{
			result->seed		= (UINT)rand();
			result->dim.X		= dim.X + 1;	// '+1' is for newline
			result->dim.Y		= dim.Y;
			result->attr		= attr;

			// tree for room data
			result->tRoom		= NULL;
			// list for room data
			result->lRoom.len	= 0;
			result->lRoom.head	= NULL;
			result->lRoom.tail	= NULL;
			// list for item data
			result->lItem.len	= 0;
			result->lItem.head	= NULL;
			result->lItem.tail	= NULL;
			// list for entity data
			result->lEnti.len	= 0;
			result->lEnti.head	= NULL;
			result->lEnti.tail	= NULL;

			result->curIdx = curIdx;

			result->lOriginalItem.len = 0;
			result->lOriginalItem.head = NULL;
			result->lOriginalItem.tail = NULL;
			_makeOriginalItem(&(result->lOriginalItem));

			// list for original entity data
			result->lOriginEnti.len = 0;
			result->lOriginEnti.head = NULL;
			result->lOriginEnti.tail = NULL;
			_initOriginEnt(&(result->lOriginEnti));

			_initShop(result->shop, curIdx);

			result->systemMsg = _initSysMsg();


			// https://stackoverflow.com/a/2151141
			result->gBuf		= (PWCHAR)calloc(result->dim.X * result->dim.Y, sizeof(WCHAR));
			result->cBuf		= (PUCHAR)calloc(result->dim.X * result->dim.Y, sizeof(UCHAR));
			result->fBuf		= (PUCHAR)calloc(result->dim.X * result->dim.Y, sizeof(UCHAR));

			if (result->gBuf && result->cBuf && result->fBuf)
			{
				// fill gBuf with double-byte whitespace
				wmemset(result->gBuf, ICON_VOID, result->dim.X * result->dim.Y);
				// fill rightmost cells of gBuf with newline
				for (SHORT y = 0; y < result->dim.Y; y++)
					result->gBuf[(result->dim.X * (y)) + (result->dim.X - 1)] = L'\n';
				
				srand(result->seed);
				_initTree(&(result->tRoom), iter);

				SRECT rect = { 0, 0, dim.X, dim.Y };
				_rEnvProcGenBSP(result, result->tRoom, rect, iter);
				_designateEnvIO(result);

				// 보스 스테이지
				if (curIdx == 4) {
					PSRECT room = result->lRoom.head->pVal;
					COORD pos = { 0, 0 };
					BOOL overlap = FALSE;
					
					// 아이템 생성
					for (USHORT count = 0; count < 30;count++) {
						do {
							pos.X = (SHORT)(rand() % (room->R - room->L - 2) + room->L + 1);
							pos.Y = (SHORT)(rand() % (room->B - room->T - 2) + room->T + 1);
							for (PLNODE tmp = result->lItem.head; tmp != NULL; tmp = tmp->next) {
								PITEM tmpItem = tmp->pVal;
								if (tmpItem->pos.X != pos.X && tmpItem->pos.Y != pos.Y) {
									overlap = FALSE;
								}
								else overlap = TRUE;
							}
						} while (overlap == TRUE || !result->cBuf[result->dim.X * pos.Y + pos.X]);
						overlap = FALSE;
						USHORT rnd;
						rnd = (USHORT)(rand() % (NUM_ITEM_TYPE * 3));
						PLNODE tmp = _getListNodeI(&(result->lOriginalItem), rnd);
						_duplicateItem(&(result->lItem), tmp->pVal, pos);
						result->cBuf[result->dim.X * (pos.Y) + (pos.X)] = TRUE;
					}
					for (USHORT count = 0; count < 2; count++) {
						do {
							pos.X = (SHORT)(rand() % (room->R - room->L - 2) + room->L + 1);
							pos.Y = (SHORT)(rand() % (room->B - room->T - 2) + room->T + 1);
							for (PLNODE tmp = result->lItem.head; tmp != NULL; tmp = tmp->next) {
								PITEM tmpItem = tmp->pVal;
								if (tmpItem->pos.X != pos.X && tmpItem->pos.Y != pos.Y) {
									overlap = FALSE;
								}
								else overlap = TRUE;
							}
						} while (overlap == TRUE || !result->cBuf[result->dim.X * pos.Y + pos.X]);
						PLNODE tmp = _getListNodeI(&(result->lOriginalItem), NUM_ITEM - 3 + count);
						_duplicateItem(&(result->lItem), tmp->pVal, pos);
						result->cBuf[result->dim.X * (pos.Y) + (pos.X)] = TRUE;
					}
					// 몬스터 생성
					for (USHORT count = 0; count < 10; count++) {
						do {
							pos.X = (SHORT)(rand() % (room->R - room->L - 2) + room->L + 1);
							pos.Y = (SHORT)(rand() % (room->B - room->T - 2) + room->T + 1);
						} while (!result->cBuf[result->dim.X * pos.Y + pos.X]);
						overlap = FALSE;
						USHORT rnd = (USHORT)(rand() % (NUM_ENTITY_SPECIES * ((result->curIdx) + 1)));
						PLNODE tmp = _getListNodeI(&(result->lOriginEnti), rnd);
						_duplicateEnt(&(result->lEnti), tmp->pVal, pos);
						result->cBuf[result->dim.X * (pos.Y) + (pos.X)] = FALSE;
					}

				}

			}
			else
			{
				// if memory allocation fails, free everything
				_freeEnv(result);
				return NULL;
			}
		}
	}
	return result;
}
//--------------------------------------------------------------------------------
/*
	shows ENVIRONS struct
	@param	hCSB	: consonle screen buffer handle
	@param	pEnv	: pointer to ENVIRONS struct
*/
void _showEnv(HANDLE hCSB, PENV pEnv)
{
	if (hCSB != INVALID_HANDLE_VALUE && pEnv != NULL)
	{
		// set console window & screen buffer size
		// winDim MUST BE SMALLER THAN bufDim, otherwise SetConsoleWindowInfo() function fails
		// https://docs.microsoft.com/en-us/windows/console/setconsolewindowinfo
		SMALL_RECT	winDim = { 0, 0, (2 * pEnv->dim.X), (pEnv->dim.Y) };
		COORD		bufDim = { (2 * pEnv->dim.X) + 1, (pEnv->dim.Y) + 1 };
		SetConsoleWindowInfo(hCSB, TRUE, &winDim);
		SetConsoleScreenBufferSize(hCSB, bufDim);

		SetConsoleTextAttribute(hCSB, pEnv->attr);
		WriteConsoleW(hCSB, pEnv->gBuf, pEnv->dim.X * pEnv->dim.Y, NULL, NULL);

		// shows item
		SetConsoleTextAttribute(hCSB, _FY);
		PITEM pI = NULL;
		for (PLNODE temp = pEnv->lItem.head; temp != NULL; temp = temp->next)
		{
			if ((pI = temp->pVal) != NULL)
			{
				_setCursorPos(hCSB, 2 * pI->pos.X, pI->pos.Y);
				_putwch(pI->icon);
			}
		}

		// shows entity
		SetConsoleTextAttribute(hCSB, _FG);
		PENT pE = NULL;
		for (PLNODE temp = pEnv->lEnti.head; temp != NULL; temp = temp->next)
		{
			if ((pE = temp->pVal) != NULL)
			{
				_setCursorPos(hCSB, 2 * pE->pos.X, pE->pos.Y);
				_putwch(pE->rep.icon);
			}
		}
		_setCursorPos(hCSB, 0, 0);
	}
}
//--------------------------------------------------------------------------------
/*
	frees ENVIRONS struct
	@param	pEnv	: pointer to ENVIRONS struct
*/
void _freeEnv(PENV pEnv)
{
	if (pEnv != NULL)
	{
		// free tree for room data
		_freeTreeNode(pEnv->tRoom);
		// free list for room data
		for (PLNODE tmp = pEnv->lRoom.head; tmp != NULL; tmp = tmp->next)
			free(tmp->pVal);
		_freeListNode(pEnv->lRoom.head);
		// free list for item data
		for (PLNODE tmp = pEnv->lItem.head; tmp != NULL; tmp = tmp->next)
			_freeItem(tmp->pVal);
		_freeListNode(pEnv->lItem.head);
		// free list for entity data
		for (PLNODE tmp = pEnv->lEnti.head; tmp != NULL; tmp = tmp->next)
			_freeEnt(tmp->pVal);
		_freeListNode(pEnv->lEnti.head);
		for (PLNODE tmp = pEnv->lOriginalItem.head; tmp != NULL; tmp = tmp->next)
			_freeItem(tmp->pVal);
		_freeListNode(pEnv->lOriginalItem.head);

		free(pEnv->gBuf);
		free(pEnv->cBuf);
		free(pEnv->fBuf);
		free(pEnv);
	}
}
//--------------------------------------------------------------------------------
/*
	checks wheter dimension is valid
	@param	dim		: dimension
	@param	iter	: number of recursive call(s)
	@return	TRUE if dimension is valid, FALSE if otherwise
*/
BOOL _areParamsValid(COORD dim, SHORT iter)
{
	if (iter > 0)
	{
		if (dim.X < dim.Y)
		{
			dim.Y = dim.Y * 2 / 5;
			return _areParamsValid(dim, iter - 1);
		}
		else
		{
			dim.X = dim.X * 2 / 5;
			return _areParamsValid(dim, iter - 1);
		}
	}
	else if (dim.X >= ENV_MIN_LEN - 1 && dim.Y >= ENV_MIN_LEN - 1)
	{
		return TRUE;
	}
	return FALSE;
}
//--------------------------------------------------------------------------------
/*
	generates valid dimension of ENV
	@param	iter	: number of recursive call(s)
	@return	valid dimension of ENV
*/
COORD _getValidParams(SHORT iter)
{
	COORD dim = { ENV_MIN_LEN, ENV_MIN_LEN };

	for (SHORT i = 0; i < (iter / 2) + (iter % 2); i++)
	{ dim.X = dim.X * 5 / 2; }
	for (SHORT i = 0; i < (iter / 2); i++)
	{ dim.Y = dim.Y * 5 / 2; }

	return dim;
}
//--------------------------------------------------------------------------------
/*
	recursively generates contents of ENVIRONS struct's gBuf using BSP(Binary Space Partitioning) algorithm
	@param	pEnv	: pointer to ENVIRONS struct
	@param	pNode	: pointer to current tree node
	@param	sRect	: partition
	@param	iter	: number of recursive call(s)
*/
void _rEnvProcGenBSP(PENV pEnv, PTNODE pNode, SRECT rect, SHORT iter)
{
	// https://eskerda.com/bsp-dungeon-generation/
	if (iter > 0)
	{
		SRECT rect1 = { 0, 0, 0, 0 };
		SRECT rect2 = { 0, 0, 0, 0 };
		_splitPartition(rect, &rect1, &rect2);

		_rEnvProcGenBSP(pEnv, pNode->pL, rect1, iter - 1);
		_rEnvProcGenBSP(pEnv, pNode->pR, rect2, iter - 1);

		COORD cen1 = { (rect1.L + rect1.R) / 2, (rect1.T + rect1.B) / 2 };
		COORD cen2 = { (rect2.L + rect2.R) / 2, (rect2.T + rect2.B) / 2 };
		_connectCenters(pEnv, cen1, cen2);
	}
	else
	{
		pNode->pVal = _createRoomData(pEnv, rect);				// save room data in tree
		_pushListNode(&(pEnv->lRoom), pNode->pVal);				// save room data in list
	}
}
//--------------------------------------------------------------------------------
/*
	splits parition into two partitions
	@param	rect	: parent partition
	@param	pRect1	: pointer to 1st child partition
	@param	pRect2	: pointer to 2nd child partition
*/
void _splitPartition(SRECT rect, PSRECT pRect1,
	PSRECT pRect2)
{
	COORD dim = { (rect.R - rect.L), (rect.B - rect.T) };
	SHORT len = 0;

	if (dim.X < dim.Y)
	{
		/*
			┏━━━━━━━━━┓
			┃         ┃
			┃━━━━━━━━━┃ split horizontally
			┃         ┃
			┗━━━━━━━━━┛
		*/
		len = (SHORT)(rand() % (dim.Y / 5) + (dim.Y * 2 / 5));

		pRect1->T = rect.T;
		pRect1->L = rect.L;
		pRect1->B = rect.T + len;
		pRect1->R = rect.R;

		pRect2->T = rect.T + len;
		pRect2->L = rect.L;
		pRect2->B = rect.B;
		pRect2->R = rect.R;
	}
	else
	{
		/*
			┏━━━━━━━━━┓
			┃    ┃    ┃
			┃    ┃    ┃ split vertically
			┃    ┃    ┃
			┗━━━━━━━━━┛
		*/
		len = (SHORT)(rand() % (dim.X / 5) + (dim.X * 2 / 5));

		pRect1->T = rect.T;
		pRect1->L = rect.L;
		pRect1->B = rect.B;
		pRect1->R = rect.L + len;

		pRect2->T = rect.T;
		pRect2->L = rect.L + len;
		pRect2->B = rect.B;
		pRect2->R = rect.R;
	}
}
//--------------------------------------------------------------------------------
/*
	connects both partitions' centers
	@param pEnv	: pointer to ENV struct
	@param c1	: center of 1st partition
	@param c2	: center of 2nd partition
*/
void _connectCenters(PENV pEnv, COORD c1, COORD c2)
{
	WCHAR icon = L'\0';

	for (SHORT y = c1.Y; y <= c2.Y; y++)
	{
		for (SHORT x = c1.X; x <= c2.X; x++)
		{
			icon = pEnv->gBuf[pEnv->dim.X * (y) + (x)];

			if (icon == ICON_VOID || icon == ICON_WALL)
			{
				pEnv->gBuf[pEnv->dim.X * (y) + (x)] = ICON_HALLWAY;
				pEnv->cBuf[pEnv->dim.X * (y) + (x)] = TRUE;
			}
		}
	}
}
//--------------------------------------------------------------------------------
/*
	creates room data, and draws room in ENV struct's gBuf
	@param	pEnv	: pointer to ENV struct
	@param	rect	: partition
	@return	pointer to SRECT(SMALL_RECT) struct
*/
PSRECT _createRoomData(PENV pEnv, SRECT rect)
{
	PSRECT result = NULL;

	if ((result = (PSRECT)calloc(1, sizeof(SRECT))) != NULL)
	{
		COORD cen = { 0, 0 };
		cen.X = (rect.R + rect.L) / 2;
		cen.Y = (rect.B + rect.T) / 2;
		
		if (pEnv->curIdx == 4) {
			*result = rect;
		}
		else {
			result->T = cen.Y - (SHORT)(rand() % (cen.Y - rect.T - 1) + 2);
			result->L = cen.X - (SHORT)(rand() % (cen.X - rect.L - 1) + 2);
			result->B = cen.Y + (SHORT)(rand() % (rect.B - cen.Y - 2) + 3);
			result->R = cen.X + (SHORT)(rand() % (rect.R - cen.X - 2) + 3);
		}
		
		// draw room in ENV struct's gBuf
		for (SHORT y = result->T; y < result->B; y++)
		{
			for (SHORT x = result->L; x < result->R; x++)
			{
				if ((result->T < y && y < result->B - 1) && (result->L < x && x < result->R - 1))
				{
					pEnv->gBuf[pEnv->dim.X * (y) + (x)] = ICON_FLOOR;
					pEnv->cBuf[pEnv->dim.X * (y) + (x)] = TRUE;
				}
				else
				{
					pEnv->gBuf[pEnv->dim.X * (y) + (x)] = ICON_WALL;
					pEnv->cBuf[pEnv->dim.X * (y) + (x)] = FALSE;
				}
			}
		}
		
		// item
		if (!(SHORT)(rand() % (ITEM_GEN_CHANCE - ((pEnv->curIdx) < 1 ? 0 : 1))))
		{

			COORD pos = { 0, 0 };
			do {
				pos.X = (SHORT)(rand() % (result->R - result->L - 2) + result->L + 1);
				pos.Y = (SHORT)(rand() % (result->B - result->T - 2) + result->T + 1);
			} while (!pEnv->cBuf[pEnv->dim.X * pos.Y + pos.X] || (pEnv->posO.X == pos.X && pEnv->posO.Y == pos.Y) || (pEnv->posI.X == pos.X && pEnv->posI.Y == pos.Y));
			USHORT stage = pEnv->curIdx + 1;
			USHORT rnd;
			switch (stage) {
			case 1:case 2: rnd = (USHORT)(rand() % (NUM_ITEM_TYPE));	break;
			case 3:case 4: rnd = (USHORT)(rand() % (NUM_ITEM_TYPE * 2)); break;
			case 5:		   rnd = (USHORT)(rand() % (NUM_ITEM_TYPE * 3)); break;
			}
			PLNODE tmp = _getListNodeI(&(pEnv->lOriginalItem), rnd);
			_duplicateItem(&(pEnv->lItem), tmp->pVal, pos);
			pEnv->cBuf[pEnv->dim.X * (pos.Y) + (pos.X)] = TRUE;

			if (stage > 2) {
				COORD pos = { 0, 0 };
				do {
					pos.X = (SHORT)(rand() % (result->R - result->L - 2) + result->L + 1);
					pos.Y = (SHORT)(rand() % (result->B - result->T - 2) + result->T + 1);
				} while (!pEnv->cBuf[pEnv->dim.X * pos.Y + pos.X] || (pEnv->posO.X == pos.X && pEnv->posO.Y == pos.Y) || (pEnv->posI.X == pos.X && pEnv->posI.Y == pos.Y));
				USHORT rnd;
				switch (stage) {
				case 1:case 2: rnd = (USHORT)(rand() % (NUM_ITEM_TYPE));	break;
				case 3:case 4: rnd = (USHORT)(rand() % (NUM_ITEM_TYPE * 2)); break;
				case 5:		   rnd = (USHORT)(rand() % (NUM_ITEM_TYPE * 3)); break;
				}
				PLNODE tmp = _getListNodeI(&(pEnv->lOriginalItem), rnd);
				_duplicateItem(&(pEnv->lItem), tmp->pVal, pos);
				pEnv->cBuf[pEnv->dim.X * (pos.Y) + (pos.X)] = TRUE;
			}
		}
		// entity
		if (!(SHORT)(rand() % (ENTITY_GEN_CHANCE - ((pEnv->curIdx) < 1 ? 0 : 1))))
		{
			COORD pos = { 0, 0 };
			do {
				pos.X = (SHORT)(rand() % (result->R - result->L - 2) + result->L + 1);
				pos.Y = (SHORT)(rand() % (result->B - result->T - 2) + result->T + 1);
			} while (!pEnv->cBuf[pEnv->dim.X * pos.Y + pos.X]);


			USHORT rnd = (USHORT)(rand() % (NUM_ENTITY_SPECIES * ((pEnv->curIdx)+1)));
			PLNODE tmp = _getListNodeI(&(pEnv->lOriginEnti), rnd);
			_duplicateEnt(&(pEnv->lEnti), tmp->pVal, pos);
			pEnv->cBuf[pEnv->dim.X * (pos.Y) + (pos.X)] = FALSE;
		}
	}
	return result;
}
//--------------------------------------------------------------------------------
/*
	designates entry and exit point in random rooms
	@param	pEnv	: pointer to ENV struct
*/
void _designateEnvIO(PENV pEnv)
{
	if (pEnv->lRoom.len >= 2) {
		// choose random rooms
		USHORT roomIdxI = (USHORT)(rand() % (pEnv->lRoom.len / 2) + (0));
		USHORT roomIdxO = (USHORT)(rand() % (pEnv->lRoom.len / 2) + (pEnv->lRoom.len / 2));
		USHORT keyIdx = (USHORT)(rand() % (pEnv->lRoom.len / 2) + (pEnv->lRoom.len / 2));
		while (roomIdxO == keyIdx)keyIdx = (USHORT)(rand() % (pEnv->lRoom.len / 2) + (pEnv->lRoom.len / 2));
		// get random rooms
		PSRECT roomI = _getListNodeI(&(pEnv->lRoom), roomIdxI)->pVal;
		PSRECT roomO = _getListNodeI(&(pEnv->lRoom), roomIdxO)->pVal;
		PSRECT key = _getListNodeI(&(pEnv->lRoom), keyIdx)->pVal;

		// designate entrance
		pEnv->posI.X = (roomI->R + roomI->L) / 2;
		pEnv->posI.Y = (roomI->B + roomI->T) / 2;
		pEnv->gBuf[pEnv->dim.X * (pEnv->posI.Y) + (pEnv->posI.X)] = ICON_ENV_I;
		pEnv->cBuf[pEnv->dim.X * (pEnv->posI.Y) + (pEnv->posI.X)] = TRUE;
		// designate exit
		pEnv->posO.X = (roomO->R + roomO->L) / 2;
		pEnv->posO.Y = (roomO->B + roomO->T) / 2;
		pEnv->gBuf[pEnv->dim.X * (pEnv->posO.Y) + (pEnv->posO.X)] = ICON_ENV_O;
		pEnv->cBuf[pEnv->dim.X * (pEnv->posO.Y) + (pEnv->posO.X)] = TRUE;
		// key
		COORD pos;
		pos.X = (key->R + key->L) / 2;
		pos.Y = (key->B + key->T) / 2;
		PLNODE tmp = _getListNodeI(&(pEnv->lOriginalItem), NUM_ITEM - 1);
		_duplicateItem(&(pEnv->lItem), tmp->pVal, pos);
		pEnv->cBuf[pEnv->dim.X * (pos.Y) + (pos.X)] = TRUE;
	}
	else if (pEnv->lRoom.len == 1) {
		PSRECT room = pEnv->lRoom.head->pVal;
		// designate entrance
		pEnv->posI.X = room->L + 1;
		pEnv->posI.Y = (room->T + room->B) / 2;
		pEnv->gBuf[pEnv->dim.X * (pEnv->posI.Y) + (pEnv->posI.X)] = ICON_ENV_I;
		pEnv->cBuf[pEnv->dim.X * (pEnv->posI.Y) + (pEnv->posI.X)] = TRUE;
		// designate exit
		pEnv->posO.X = room->R - 2;
		pEnv->posO.Y = (room->T + room->B) / 2;
		pEnv->gBuf[pEnv->dim.X * (pEnv->posO.Y) + (pEnv->posO.X)] = ICON_ENV_O;
		pEnv->cBuf[pEnv->dim.X * (pEnv->posO.Y) + (pEnv->posO.X)] = TRUE;
	}
}
//--------------------------------------------------------------------------------
/*
	상점 아이템 초기화
 	@param shop		:상점아이템 배열
	@param curIdx	:현재 층
*/
//--------------------------------------------------------------------------------
void _initShop(PPSHOP shop, SHORT curIdx) {

	for (USHORT i = 0;  i < 3; i++) {
		shop[i] = (PSHOP)malloc(sizeof(SHOP));
		shop[i]->pItem = NULL;
		shop[i]->price = 0;
	}

	PITEM shopArmour = _makeItem("mArm", _ARMOUR, curIdx+2, 100);	
	PITEM shopWeapon = _makeItem("mSwd", _WEAPON, curIdx+2, 100);
	PITEM shopPotion = _makeItem("mElixir", _POTION, 3*(curIdx+2)+ 5, 100);

	shop[0]->pItem = shopArmour;
	shop[0]->price = 50*(1 + 0.2*(curIdx+1));
	shop[1]->pItem = shopWeapon;
	shop[1]->price = 45 * (1 + 0.2 * (curIdx + 1));
	shop[2]->pItem = shopPotion;
	shop[2]->price = 40 * (1 + 0.2 * (curIdx + 1));
}
