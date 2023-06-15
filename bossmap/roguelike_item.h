#ifndef _ROGUELIKE_ITEM_
#define _ROGUELIKE_ITEM_

#include "roguelike.h"

PITEM			_makeItem(CHAR*, ITYPE, USHORT, USHORT);
void			_makeOriginalItem(PLIST);
void			_duplicateItem(PLIST, PITEM, COORD);
void			_freeItem(PITEM);
void			_makeRandomItem(PENV, COORD);
void			_initInven(PPITEM_INVEN);
void			_addInven(PRL, PITEM);
void			_removeInven(PENT, USHORT);
void			_putDownItem(PRL, USHORT);
void			_useItem(PRL, USHORT);
void			_usePotion(PENV, PENT, PITEM);
void			_useScroll(PRL, PENT, PITEM);
void			_equipItem(PENV, PENT, PITEM);
int 			_escape(PRL);
int				_attackBoss(PRL, PITEM);
PITEM _getOverlapItem(PENV, PENT);
#endif
