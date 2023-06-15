#include "roguelike_init.h"

//--------------------------------------------------------------------------------
/*
	makes ROGUELIKE struct
	@return	pointer to ROGUELIKE struct
*/
PRL _makeRL()
{
	PRL result = NULL;

	if ((result = (PRL)calloc(1, sizeof(RL))) != NULL)
	{
		srand((UINT)time(NULL));

		USHORT	iter	= ENV_MIN_ITER;
		COORD	dim		= { 0, 0 };
		
		for (USHORT i = 0; i < 4; i++)
		{
			if (i > 1) iter = ENV_MIN_ITER + 1;
			dim = _getValidParams(iter);
			_pushListNode(&(result->lEnv), _makeEnv(dim, _FC, iter,i));
		}

		dim = _getValidParams(ENV_MIN_ITER + 1);
		_pushListNode(&(result->lEnv), _makeEnv(dim, _FC, 0, 4));
		result->curIdx		= 0;
		result->curEnv		= _getListNodeI(&(result->lEnv), result->curIdx)->pVal;

		result->pPC			= _makeEnt(result->curEnv->posI, ICON_PLAYER, _FG | _FI, TRUE, "PC", BASIC_HP, BASIC_ATK, 0, 0);
		result->curEnv->cBuf[result->curEnv->dim.X * (result->pPC->pos.Y) + (result->pPC->pos.X)] = FALSE;
		result->pGUI		= _makeGUI();
		result->pBOSS	 = _makeBoss();
	}
	return result;
}
//--------------------------------------------------------------------------------
/*
	frees ROGUELIKE struct
*/
void _freeRL(PRL pRL)
{
	for (PLNODE temp = pRL->lEnv.head; temp != NULL; temp = temp->next)
		_freeEnv(temp->pVal);
	_freeListNode(pRL->lEnv.head);
	_freeEnt(pRL->pPC);
	_freeGUI(pRL->pGUI);
}
//--------------------------------------------------------------------------------
/*
	TODO: write description
*/
//--------------------------------------------------------------------------------
