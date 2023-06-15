#ifndef _ROGUELIKE_UTIL_
#define _ROGUELIKE_UTIL_

#include "roguelike.h"

void	_setCodePage(UINT);
void	_setCursorVisibility(HANDLE, BOOL);
void	_getCursorPos(HANDLE, PCOORD);
void	_setCursorPos(HANDLE, SHORT, SHORT);

PTNODE	_makeTreeNode(PVOID, PTNODE, PTNODE, PTNODE);
void	_initTree(PPTNODE, SHORT);
void	_freeTreeNode(PTNODE);

PLNODE	_makeListNode(PVOID, PLNODE, PLNODE);
void	_pushListNode(PLIST, PVOID);
void	_dropListNode(PLIST, PVOID);
PLNODE	_getListNodeI(PLIST, USHORT);
void	_freeListNode(PLNODE);

BOOL	_coordInSrect(COORD, SRECT);

#endif
