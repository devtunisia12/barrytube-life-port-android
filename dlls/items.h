/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#pragma once
#if !defined(ITEMS_H)
#define ITEMS_H
#define ITEM_SLOT6			6
#define ITEM_SLOT7			7

class CItem : public CBaseEntity
{
public:
	void Spawn( void );
	CBaseEntity *Respawn( void );
	void EXPORT ItemTouch( CBaseEntity *pOther );
	void EXPORT Materialize( void );
	virtual BOOL MyTouch( CBasePlayer *pPlayer )
	{
		return FALSE;
	};
};
#endif // ITEMS_H
