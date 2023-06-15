#ifndef _ROGUELIKE_ENV_
#define _ROGUELIKE_ENV_

#include "roguelike.h"

PENV	_makeEnv(COORD, WORD, SHORT, SHORT);
void	_showEnv(HANDLE, PENV);
void	_freeEnv(PENV);

BOOL	_areParamsValid(COORD, SHORT);
COORD	_getValidParams(SHORT);

void	_rEnvProcGenBSP(PENV, PTNODE, SRECT, SHORT);
void	_splitPartition(SRECT, PSRECT, PSRECT);
void	_connectCenters(PENV, COORD, COORD);
PSRECT	_createRoomData(PENV, SRECT);
void	_designateEnvIO(PENV);

void	_initShop(PPSHOP, SHORT);

#endif
