#include "roguelike_item.h"


PITEM _makeItem(CHAR* name, ITYPE type, USHORT stat, USHORT prob) {

	PITEM result = NULL;

	if ((result = (PITEM)calloc(1, sizeof(ITEM))) != NULL)
	{
		strcpy(result->name, name);
		result->type = type;
		result->stat = stat;
		result->prob = prob;
	}
	return result;
}
void _makeOriginalItem(PLIST list) { 
	PITEM basicArmour = _makeItem("wArm", _ARMOUR, 1, 100);		_pushListNode(list, basicArmour);
	PITEM basicWeapon = _makeItem("wSwd", _WEAPON, 1, 100);		_pushListNode(list, basicWeapon);
	PITEM basicPotion = _makeItem("apple", _POTION, 10, 70);		_pushListNode(list, basicPotion);
	PITEM basicScroll = _makeItem("magic1", _SCROLL, 20, 80);	_pushListNode(list, basicScroll);


	// 3~ stage에서 등장
	PITEM improvedArmour = _makeItem("iArm", _ARMOUR, 2, 100);		_pushListNode(list, improvedArmour);
	PITEM improvedWeapon = _makeItem("iSwd", _WEAPON, 2, 100);		_pushListNode(list, improvedWeapon);
	PITEM improvedPotion = _makeItem("milk", _POTION, 15, 50);		_pushListNode(list, improvedArmour);
	PITEM improvedScroll = _makeItem("magic2", _SCROLL, 30, 90);	_pushListNode(list, improvedScroll);

	// 5 stage에서 등장
	PITEM finalArmour = _makeItem("sArm", _ARMOUR, 3, 100);			_pushListNode(list, finalArmour);
	PITEM finalWeapon = _makeItem("sSwd", _WEAPON, 3, 100);			_pushListNode(list, finalWeapon);
	PITEM finalPotion = _makeItem("elixir", _POTION, 20, 100);		_pushListNode(list, finalPotion);
	PITEM finalScroll = _makeItem("magic3", _SCROLL, 30, 100);		_pushListNode(list, finalScroll);

	PITEM oil =			_makeItem("oil", _SPECIAL_OIL, 0, 0);			_pushListNode(list, oil);
	PITEM fire =		_makeItem("fire", _SPECIAL_FIRE, 0, 0);			_pushListNode(list, fire);

	// key
	PITEM key = _makeItem("key", _KEY, 12, 100);		_pushListNode(list, key);
}
void _duplicateItem(PLIST list, PITEM origin, COORD pos) {

	PITEM result = NULL;
	WORD itemColor = _FG;

	USHORT ran = (USHORT)(rand() % 5);
	if ((origin->type == _WEAPON || origin->type == _ARMOUR) && (ran < 3)) return;

	if ((result = (PITEM)calloc(1, sizeof(ITEM))) != NULL)
	{
		WORD itemColor = _FG;

		result->pos.X = pos.X;
		result->pos.Y = pos.Y;

		strcpy(result->name, origin->name);
		result->type = origin->type;
		result->stat = origin->stat;
		result->prob = origin->prob;

		result->color = itemColor;
		switch (origin->type) {
		case _WEAPON: result->icon = ICON_WEAPON; break;
		case _ARMOUR: result->icon = ICON_ARMOUR; break;
		case _POTION: result->icon = ICON_POTION; break;
		case _SCROLL: result->icon = ICON_SCROLL; break;
		case _KEY:result->icon = ICON_KEY; break;
		case _SPECIAL_OIL:	result->icon = ICON_OIL; break;
		case _SPECIAL_FIRE: result->icon = ICON_FIRE; break;
		}

		// 리스트에 추가
		_pushListNode(list, result);
	}
}
void _freeItem(PITEM item) {
	if (item != NULL)
	{
		free(item);
	}
}
void _makeRandomItem(PENV pEnv, COORD pos) {
	PLIST itemList = &(pEnv->lItem);
	int itemNum = NUM_ITEM_TYPE * 3;
	switch (pEnv->curIdx) {
	case 0:
	case 1:itemNum = NUM_ITEM_TYPE;		break;
	case 2:
	case 3:itemNum = NUM_ITEM_TYPE * 2; break;
	case 4:itemNum = NUM_ITEM_TYPE * 3; break;
	}
	USHORT rnd = (USHORT)(rand() % itemNum) + 1;

	_duplicateItem(itemList, _getListNodeI(&(pEnv->lOriginalItem), rnd), pos);

}
//--------------------------------------------------------------------------------
/*
	인벤토리 초기화
*/
void _initInven(PPITEM_INVEN inven) {
	for (USHORT i = 1; i <= 10; i++) {
		inven[i] = (PITEM_INVEN)malloc(sizeof(ITEM_INVEN));
		inven[i]->pItem = NULL;
		inven[i]->num = 0;
	}
}
//--------------------------------------------------------------------------------
/*
	인벤토리에 아이템 추가
*/
void _addInven(PRL pRL, PITEM item) {
	PENT player = pRL->pPC;
	PENV curEnv = pRL->curEnv;

	// 소모품인지 확인
	if (item->type == _POTION || item->type == _SCROLL) {
		for (USHORT i = 1; i <= 10; i++) {
			USHORT count = 0;
			if (player->items[i]->pItem != NULL) {
				for (USHORT j = 0; j < 16; j++) {
					if (item->name[j] == player->items[i]->pItem->name[j])count++;
				}
				if (count == 16) {
					player->items[i]->num++;
					_dropListNode(&(curEnv->lItem), item);
					return;
				}
			}
		}
	}
	for (USHORT i = 1; i <= 10; i++)
	{
		if (player->items[i]->pItem == NULL)
		{
			player->items[i]->pItem = item;
			player->items[i]->num	  = 1;
			_dropListNode(&(curEnv->lItem), item);
			return;
		}
	}
}
//--------------------------------------------------------------------------------
/*
	인벤토리에서 아이템 버리기
	아이템 삭제 고민중
*/
/*
void _removeInven(PRL pRL, USHORT pressKey) {
	PENT player = pRL->pPC;	
*/
void _removeInven(PENT player, USHORT pressKey) {
	if (player->items[pressKey]->num > 0) {
		player->items[pressKey]->num--;
		if (player->items[pressKey]->num == 0) {
			player->items[pressKey]->pItem = NULL;
		}
	}
}
void _putDownItem(PRL pRL, USHORT pressKey) {
	PENV curEnv = pRL->curEnv;
	PENT player = pRL->pPC;
	char tmpMsg[200];
	if (pressKey < 1 || pressKey > 9)
		return;

	// 해당 인벤토리 위치가 비어있을 경우
	if (player->items[pressKey]->pItem == NULL) 
	{
		return;
	}

	PITEM tmpItem = player->items[pressKey]->pItem;

	char tmpName[200] = { 0 }; 
	strcpy(tmpName, tmpItem->name);

	if (tmpItem->type == _KEY) {


		sprintf(tmpMsg, "You can't throw away the key");
		_enQueueSysMsg(pRL->curEnv->systemMsg, tmpMsg);
		return;
	}
	_removeInven(player, pressKey);

	sprintf(tmpMsg, "You threw away %s", tmpName);
	_enQueueSysMsg(pRL->curEnv->systemMsg, tmpMsg);
}
//--------------------------------------------------------------------------------
/*
	아이템 사용
	스텟 관련 설정이 끝나면 재보수
*/
void _useItem(PRL pRL, USHORT pressKey) {
	PENT player = pRL->pPC;
	PENV pEnv = pRL->curEnv;
	PITEM useItem = player->items[pressKey]->pItem;

	if (useItem == NULL)
		return;

	strcpy(pRL->pPC->causeOfDeath, useItem->name);

	switch (useItem->type) {
	case _POTION:
		_usePotion(pEnv, player, useItem);
		_removeInven(player, pressKey);
		break;
	case _SCROLL:
		_useScroll(pRL, player, useItem);
		_removeInven(player, pressKey);
		break;
	case _WEAPON:
	case _ARMOUR:
		_equipItem(pEnv, player, useItem);
		_removeInven(player, pressKey);
		break;
	case _KEY:
		if(_escape(pRL))_removeInven(player, pressKey);
		break;
	default:
		if(_attackBoss(pRL, useItem)) _removeInven(player, pressKey);
	}

}
void _usePotion(PENV pEnv, PENT player, PITEM pItem) {
	USHORT probability = (rand() % 100) + 1;
	if (pItem->prob >= probability) {
		if (player->stats[_curHP] + pItem->stat >= player->stats[_maxHP]) {
			player->stats[_curHP] = player->stats[_maxHP];
		}
		else {
			player->stats[_curHP] += pItem->stat;
		}

		char tmpMsg[200];

		sprintf(tmpMsg, "You are healed");

		_enQueueSysMsg(pEnv->systemMsg, tmpMsg);

	}
	else {
		player->stats[_curHP] -= pItem->stat;

		
		char tmpMsg[200];

		sprintf(tmpMsg, "You feel gross");

		_enQueueSysMsg(pEnv->systemMsg, tmpMsg);
	}
}
void _useScroll(PRL pRL, PENT player, PITEM pItem) {
	PENV pEnv = pRL->curEnv;
	USHORT probability = (rand() % 100) + 1;
	// 위치 역 추적
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
	if (pItem->prob >= probability)
	{
		// 주변 몬스터 리스트 생성
		LIST lNear;
		lNear.head = NULL;
		lNear.tail = NULL;
		lNear.len = 0;
		PLIST lEnti = &(pEnv->lEnti);
		for (PLNODE temp = lEnti->head; temp != NULL; temp = temp->next) {
			PENT pEnt = temp->pVal;
			if (abs(pEnt->pos.X - player->pos.X) <= 2) {
				if (abs(pEnt->pos.Y - player->pos.Y) <= 2) {
					_pushListNode(&lNear, pEnt);
				}
			}
		}
		if (lNear.len != 0) {
			for (PLNODE temp = lNear.head; temp != NULL; temp = temp->next) {
				PENT pEnt = temp->pVal;
				// 주변 몬스터에게 데미지
				USHORT tmp1 = player->stats[_ATK];
				USHORT tmp2 = player->stats[_tmpATK];
				player->stats[_tmpATK] = 0;
				player->stats[_ATK] = pItem->stat;
				_attackEnt(pEnv, player, pEnt);
				player->stats[_ATK] = tmp1;
				player->stats[_tmpATK] = tmp2;
				if (_isDead(pEnt)) {
					_killEnt(pRL->curEnv->systemMsg, pEnv, player, pEnt);
				}
				// 이펙트
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _FC | FOREGROUND_INTENSITY);
				_setCursorPos(GetStdHandle(STD_OUTPUT_HANDLE), 2 * (pEnt->pos.X - win.L + 1), pEnt->pos.Y - win.T + 1);
				_putwch(EFFECT_1);

			}
			Sleep(300);

			WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), pRL->pGUI->gBuf, pRL->pGUI->dim.X * pRL->pGUI->dim.Y, NULL, NULL);
		}
	}
	else {
		// 자폭
		player->stats[_curHP] -= pItem->stat/3;

		// 이펙트
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _FC | FOREGROUND_INTENSITY);
		_setCursorPos(GetStdHandle(STD_OUTPUT_HANDLE), 2 * (player->pos.X - win.L + 1), player->pos.Y - win.T + 1);
		_putwch(EFFECT_1); 
		
		char tmpMsg[200];

		sprintf(tmpMsg, "It just exploded");
		_enQueueSysMsg(pRL->curEnv->systemMsg, tmpMsg);

		Sleep(300);

		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), pRL->pGUI->gBuf, pRL->pGUI->dim.X * pRL->pGUI->dim.Y, NULL, NULL);
	}
}
void _equipItem(PENV pEnv, PENT player, PITEM pItem) {
	switch (pItem->type)
	{
	case _ARMOUR:
		player->stats[_DEF] -= player->stats[_tmpDEF];
		player->stats[_DEF] += pItem->stat;
		player->stats[_tmpDEF] = pItem->stat;
		break;
	case _WEAPON:
		player->stats[_ATK] -= player->stats[_tmpATK];
		player->stats[_ATK] += pItem->stat;
		player->stats[_tmpATK] = pItem->stat;
		break;
	}

	char tmpMsg[200];

	sprintf(tmpMsg, "You equipped %s", pItem->name);

	_enQueueSysMsg(pEnv->systemMsg, tmpMsg);


}
int _escape(PRL pRL) {
	PENT player = pRL->pPC;
	PENV pEnv = pRL->curEnv;
	if (player->pos.X == pEnv->posO.X && player->pos.Y == pEnv->posO.Y) {
		// go to next level
		PLNODE temp = _getListNodeI(&(pRL->lEnv), ++(pRL->curIdx));
		if (temp != NULL) {
			pRL->pPC->pos = (pRL->curEnv = temp->pVal)->posI;
			if (pRL->curIdx == 4) {
				char tmpMsg[200];
				sprintf(tmpMsg, "A tree-like monster is blocking the road.");
				PENV nextEnv = _getListNodeI(&(pRL->lEnv), 4)->pVal;
				_enQueueSysMsg(nextEnv->systemMsg, tmpMsg);
			}
		}

		return 1;
	}
	return 0;
}
int _attackBoss(PRL pRL, PITEM pItem) {
	PENT player = pRL->pPC;
	PBOSS boss = pRL->pBOSS;
	PENV pEnv = pRL->curEnv;
	char tmpMsg[200];

	switch (pItem->type) {
	case _SPECIAL_OIL:
		if (abs(player->pos.X - boss->pos.X) < 5 && abs(player->pos.Y - boss->pos.Y) < 5) {
			boss->oil = TRUE;
			boss->rep.attr = _FY;
			sprintf(tmpMsg, "You poured oil on the monster.");
			_enQueueSysMsg(pEnv->systemMsg, tmpMsg);
			return 1;
		}
		else {
			sprintf(tmpMsg, "it can be used somewhere.");
			_enQueueSysMsg(pEnv->systemMsg, tmpMsg);
			return 0;
		}
		break;
	case _SPECIAL_FIRE:
		if (abs(player->pos.X - boss->pos.X) < 5 && abs(player->pos.Y - boss->pos.Y) < 5) {
			if (boss->oil) {
				boss->rep.attr = _FR;
				boss->HP = 0;
				sprintf(tmpMsg, "The monster burned down.");
				_enQueueSysMsg(pEnv->systemMsg, tmpMsg);
				return 1;
			}
			else {
				sprintf(tmpMsg, "Does not catch fire");
				_enQueueSysMsg(pEnv->systemMsg, tmpMsg);
				return 0;
			}
		}
		else {
			sprintf(tmpMsg, "it can be used somewhere.");
			_enQueueSysMsg(pEnv->systemMsg, tmpMsg);
			return 0;
		}
		break;
	}
}
//--------------------------------------------------------------------------------
/*
	엔티티와 동좌표에 있는 아이템 포인터 반환
	@param pEnv 맵 포인터
	@param pEnt 엔티티 포인터
	@return 엔티티와 동좌표에 있는 아이템 포인터
*/
PITEM _getOverlapItem(PENV pEnv, PENT pEnt) {
	PLNODE tmp = pEnv->lItem.head;
	PITEM pItem;
	while (tmp) {
		pItem = tmp->pVal;
		if (pItem->pos.X == pEnt->pos.X && pItem->pos.Y == pEnt->pos.Y)
			return pItem;
		tmp = tmp->next;
	}
	return NULL;
}
//--------------------------------------------------------------------------------
/*
	
*/