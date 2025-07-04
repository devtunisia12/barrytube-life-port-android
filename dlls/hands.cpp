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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"


#define	HANDS_BODYHIT_VOLUME 128
#define	HANDS_WALLHIT_VOLUME 512

LINK_ENTITY_TO_CLASS(weapon_hands, CHANDS);



enum hands_e {
	HANDS_IDLE = 0,
	HANDS_DRAW,
	HANDS_ATTACK1HIT,
	HANDS_ATTACK1MISS,
	HANDS_ATTACK2HIT,
	HANDS_ATTACK2MISS,
	HANDS_ATTACK3HIT,
	HANDS_ATTACK3MISS
};


void CHANDS::Spawn()
{
	Precache();
	m_iId = WEAPON_HANDS;
	SET_MODEL(ENT(pev), "models/w_hands.mdl");
	m_iClip = -1;
	FallInit();// get ready to fall down.
}


void CHANDS::Precache(void)
{
	PRECACHE_MODEL("models/v_hands.mdl");
	PRECACHE_MODEL("models/w_hands.mdl");
	PRECACHE_MODEL("models/p_hands.mdl");
	PRECACHE_SOUND("weapons/cbar_hit1.wav");
	PRECACHE_SOUND("weapons/cbar_hit2.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod1.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod2.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod3.wav");
	PRECACHE_SOUND("weapons/hands_miss1.wav");

	PRECACHE_SOUND("weapons/hands_miss.wav");
	PRECACHE_SOUND("weapons/hands1.wav");
	PRECACHE_SOUND("weapons/hands_hitbody1.wav");

	m_usHANDS = PRECACHE_EVENT(1, "events/hands.sc");
}

int CHANDS::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iId = WEAPON_HANDS;
	p->iWeight = CROWBAR_WEIGHT;
	return 1;
}



BOOL CHANDS::Deploy()
{
	return DefaultDeploy("models/v_hands.mdl", "models/p_crowbar.mdl", HANDS_DRAW, "crowbar");
}

void CHANDS::Holster(int skiplocal /* = 0 */)
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	//	SendWeaponAnim( CROWBAR_HOLSTER );
}


void FindHullIntersectionSHOVEL(const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity)
{
	int			i, j, k;
	float		distance;
	float		*minmaxs[2] = { mins, maxs };
	TraceResult tmpTrace;
	Vector		vecHullEnd = tr.vecEndPos;
	Vector		vecEnd;

	distance = 1e6f;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc) * 2);
	UTIL_TraceLine(vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &tmpTrace);
	if (tmpTrace.flFraction < 1.0)
	{
		tr = tmpTrace;
		return;
	}

	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
			for (k = 0; k < 2; k++)
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];

				UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, pEntity, &tmpTrace);
				if (tmpTrace.flFraction < 1.0)
				{
					float thisDistance = (tmpTrace.vecEndPos - vecSrc).Length();
					if (thisDistance < distance)
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}


void CHANDS::PrimaryAttack()
{
	if (!SwingHANDS(1))
	{
		SetThink(&CHANDS::SwingAgainHANDS);
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CHANDS::SecondaryAttack( void )
{
	Kick();
}


void CHANDS::SmackHANDS()
{
	DecalGunshot(&m_trHit, BULLET_PLAYER_CROWBAR);
}


void CHANDS::SwingAgainHANDS(void)
{
	SwingHANDS(0);
}


int CHANDS::SwingHANDS(int fFirst)
{
	int fDidHit = FALSE;

	TraceResult tr;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 32;

	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

#ifndef CLIENT_DLL
	if (tr.flFraction >= 1.0)
	{
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);
		if (tr.flFraction < 1.0)
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
			if (!pHit || pHit->IsBSPModel())
				FindHullIntersectionSHOVEL(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict());
			vecEnd = tr.vecEndPos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}
#endif

	PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_usHANDS,
		0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, 0,
		0.0, 0, 0.0);


	if (tr.flFraction >= 1.0)
	{
		if (fFirst)
		{
			// miss
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;

			// player "shoot" animation
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);
		}
	}
	else
	{
		switch (((m_iSwing++) % 2) + 1)
		{
		case 0:
			SendWeaponAnim(HANDS_ATTACK1HIT); break;
		case 1:
			SendWeaponAnim(HANDS_ATTACK2HIT); break;
		case 2:
			SendWeaponAnim(HANDS_ATTACK3HIT); break;
		}

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

#ifndef CLIENT_DLL

		// hit
		fDidHit = TRUE;
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		ClearMultiDamage();

		if ((m_flNextPrimaryAttack + 1 < UTIL_WeaponTimeBase()) || g_pGameRules->IsMultiplayer())
		{
			// first swing does full damage
			pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgCrowbar, gpGlobals->v_forward, &tr, DMG_CLUB);
		}
		else
		{
			// subsequent swings do half
			pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgCrowbar / 2, gpGlobals->v_forward, &tr, DMG_CLUB);
		}
		ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		int fHitWorld = TRUE;

		if (pEntity)
		{
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
			{
				// play thwack or smack sound
				switch (RANDOM_LONG(0, 2))
				{
				case 0:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/hands_miss1.wav", 1, ATTN_NORM); break;
				case 1:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/hands_miss1.wav", 1, ATTN_NORM); break;
				case 2:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/hands_miss1.wav", 1, ATTN_NORM); break;
				}
				m_pPlayer->m_iWeaponVolume = HANDS_BODYHIT_VOLUME;
				if (!pEntity->IsAlive())
					return TRUE;
				else
					flVol = 0.1;

				fHitWorld = FALSE;
			}
		}

		// play texture hit sound
		// UNDONE: Calculate the correct point of intersection when we hit with the hull instead of the line

		if (fHitWorld)
		{
			float fvolbar = TEXTURETYPE_PlaySound(&tr, vecSrc, vecSrc + (vecEnd - vecSrc) * 2, BULLET_PLAYER_CROWBAR);

			if (g_pGameRules->IsMultiplayer())
			{
				// override the volume here, cause we don't play texture sounds in multiplayer, 
				// and fvolbar is going to be 0 from the above call.

				fvolbar = 1;
			}

			// also play crowbar strike
			switch (RANDOM_LONG(0, 1))
			{
			case 0:
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/hands1.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0, 3));
				break;
			case 1:
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/hands1.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0, 3));
				break;
			}

			// delay the decal a bit
			m_trHit = tr;
		}

		m_pPlayer->m_iWeaponVolume = flVol * HANDS_WALLHIT_VOLUME;
#endif
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.25;

		SetThink(&CHANDS::SmackHANDS);
		pev->nextthink = UTIL_WeaponTimeBase() + 0.2;


	}
	return fDidHit;
}


