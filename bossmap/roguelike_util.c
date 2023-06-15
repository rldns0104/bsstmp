#include "roguelike_util.h"

//--------------------------------------------------------------------------------
/*
	sets code page of console
	@param wCodePageID : code page id
*/
void _setCodePage(UINT wCodePageID)
{
	SetConsoleCP(wCodePageID);
	SetConsoleOutputCP(wCodePageID);
}
//--------------------------------------------------------------------------------
/*
	sets cursor visibility of console screen buffer
	@param hCSB				: handle of a console screen buffer
	@param cursorVisibility	: cursor visibility
*/
void _setCursorVisibility(HANDLE hCSB, BOOL cursorVisibility)
{
	CONSOLE_CURSOR_INFO ccInfo;
	GetConsoleCursorInfo(hCSB, &ccInfo);
	ccInfo.bVisible = cursorVisibility;
	SetConsoleCursorInfo(hCSB, &ccInfo);
}
//--------------------------------------------------------------------------------
/*
	gets cursor position
	@param hCSB		: handle of a console screen buffer
	@param pCoord	: pointer to a COORD variable used to store cursor position
*/
void _getCursorPos(HANDLE hCSB, PCOORD pCoord)
{
	CONSOLE_SCREEN_BUFFER_INFO csbInfo;
	GetConsoleScreenBufferInfo(hCSB, &csbInfo);
	(*pCoord) = csbInfo.dwCursorPosition;
}
//--------------------------------------------------------------------------------
/*
	sets cursor position
	@param	hCSB	: console screen buffer handle
	@param	X		: new x coordinate
	@param	Y		: new y coordinate
*/
void _setCursorPos(HANDLE hCSB, SHORT X, SHORT Y)
{
	COORD pos = { X, Y };
	SetConsoleCursorPosition(hCSB, pos);
}
//--------------------------------------------------------------------------------
/*
	makes tree node
	@param	pVal	: pointer to value
	@param	pP		: pointer to parent node
	@param	pL		: pointer to left node
	@param	pR		: pointer to right node
	@return	pointer to dynamically allocated tree node
*/
PTNODE _makeTreeNode(PVOID pVal, PTNODE pP, PTNODE pL, PTNODE pR)
{
	PTNODE result = NULL;

	if ((result = (PTNODE)calloc(1, sizeof(TNODE))) != NULL)
	{
		result->pVal	= pVal;
		result->pP		= pP;
		result->pL		= pL;
		result->pR		= pR;
	}
	return result;
}
//--------------------------------------------------------------------------------
/*
	initializes tree
	@param	ppNode	: double pointer to tree node
	@param	iter	: number of recursive calls
*/
void _initTree(PPTNODE ppNode, SHORT iter)
{
	if (iter >= 0)
	{
		(*ppNode) = _makeTreeNode(NULL, NULL, NULL, NULL);

		_initTree(&((*ppNode)->pL), iter - 1);
		_initTree(&((*ppNode)->pR), iter - 1);

		if ((*ppNode)->pL != NULL) { (*ppNode)->pL->pP = (*ppNode); }
		if ((*ppNode)->pR != NULL) { (*ppNode)->pR->pP = (*ppNode); }
	}
}
//--------------------------------------------------------------------------------
/*
	recursively frees tree nodes
	@param	pNode	: pointer to tree node
*/
void _freeTreeNode(PTNODE pNode)
{
	if (pNode != NULL)
	{
		_freeTreeNode(pNode->pL);
		_freeTreeNode(pNode->pR);
		free(pNode);
	}
}
//--------------------------------------------------------------------------------
/*
	makes list node
	@param	pVal	: pointer to value
	@param	next	: pointer to next node
	@param	prev	: pointer to previous node
	@return	pointer to dynamically allocated list node
*/
PLNODE _makeListNode(PVOID pVal, PLNODE next, PLNODE prev)
{
	PLNODE result = NULL;

	if ((result = (PLNODE)calloc(1, sizeof(LNODE))) != NULL)
	{
		result->pVal = pVal;
		result->next = next;
		result->prev = prev;

		if (next != NULL) { result->next->prev = result; }
		if (prev != NULL) { result->prev->next = result; }
	}
	return result;
}
//--------------------------------------------------------------------------------
/*
	inserts list node with ceratin value into list
	@param	pList	:
	@param	pVal	:
*/
void _pushListNode(PLIST pList, PVOID pVal)
{
	if (pList->head == NULL)
	{
		pList->head = _makeListNode(pVal, NULL, NULL);
		pList->tail = pList->head;
	}
	else
	{
		pList->tail->next = _makeListNode(pVal, NULL, pList->tail);
		pList->tail = pList->tail->next;
	}
	pList->len++;
}
//--------------------------------------------------------------------------------
/*
	removes list node with certain value from list
	@param	pList	:
	@param	pVal	:
*/
void _dropListNode(PLIST pList, PVOID pVal)
{
	if (pList->head == NULL)
	{
		return;
	}
	else if (pList->head->pVal == pVal)
	{
		PLNODE temp = pList->head;
		pList->head = pList->head->next;
		free(temp);
	}
	else
	{
		for (PLNODE temp = pList->head; temp != NULL; temp = temp->next)
		{
			if (temp->pVal == pVal)
			{
				if (temp->next != NULL) { temp->next->prev = temp->prev; }
				if (temp->prev != NULL) { temp->prev->next = temp->next; }
				free(temp);
				break;
			}
		}
	}
	pList->len--;
}
//--------------------------------------------------------------------------------
/*
	gets list node at index
	@param	pList	:
	@param	idx		:
	@return	
*/
PLNODE _getListNodeI(PLIST pList, USHORT idx)
{
	for (PLNODE temp = pList->head; temp != NULL; temp = temp->next, idx--)
		if (idx == 0)
			return temp;
	return NULL;
}
//--------------------------------------------------------------------------------
/*
	recursively frees list nodes
	@param	pNode	: pointer to list node
*/
void _freeListNode(PLNODE pNode)
{
	if (pNode != NULL)
	{
		_freeListNode(pNode->next);
		free(pNode);
	}
}
//--------------------------------------------------------------------------------
/*
	checks whether COORD in inside SRECT(SMALL_RECT)
	@param	pos		: position
	@param	rect	: rectangle
	@return TRUE if pos is inside rect, FALSE if otherwise
*/
BOOL _coordInSrect(COORD pos, SRECT rect)
{
	if ((rect.L <= pos.X && pos.X < rect.R) && (rect.T <= pos.Y && pos.Y < rect.B))
	{
		return TRUE;
	}
	return FALSE;
}
//--------------------------------------------------------------------------------
/*
	TODO: write description
*/
//--------------------------------------------------------------------------------
