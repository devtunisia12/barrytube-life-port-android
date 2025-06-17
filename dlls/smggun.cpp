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
#include "soundent.h"
#include "gamerules.h"

enum smggun_e
{
	SMGGUN_LONGIDLE = 0,
	SMGGUN_IDLE1,
	SMGGUN_FIRE1,
	SMGGUN_RELOAD,
	SMGGUN_DEPLOY,
	SMGGUN_FIRE2,
	SMGGUN_FIRE3,
	SMGGUN_SIDLE,
	SMGGUN_SFIRE1,
	SMGGUN_ZOOM_ON,
	SMGGUN_ZOOM_OFF
};

LINK_ENTITY_TO_CLASS( weapon_smggun, CSMGGUN )
//=========================================================
//=========================================================
int CSMGGUN::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void CSMGGUN::Spawn()
{
	pev->classname = MAKE_STRING( "weapon_smggun" ); // hack to allow for old names
	Precache();
	SET_MODEL( ENT( pev ), "models/w_smggun.mdl" );
	m_iId = WEAPON_SMGGUN;

	m_iDefaultAmmo = MP5_DEFAULT_GIVE;

#if CLIENT_DLL
	if( bIsMultiplayer() )
#else
	if( g_pGameRules->IsMultiplayer() )
#endif
		m_iDefaultAmmo = MP5_DEFAULT_GIVE_MP;

	FallInit();// get ready to fall down.
}

void CSMGGUN::Precache( void )
{
	PRECACHE_MODEL( "models/v_smggun.mdl" );
	PRECACHE_MODEL( "models/w_smggun.mdl" );
	PRECACHE_MODEL( "models/p_smggun.mdl" );

	m_iShell = PRECACHE_MODEL( "models/shell.mdl" );// brass shellTE_MODEL

	PRECACHE_MODEL( "models/grenade.mdl" );	// grenade

	PRECACHE_MODEL( "models/w_9mmARclip.mdl" );
	PRECACHE_SOUND( "items/9mmclip1.wav" );

	PRECACHE_SOUND( "items/clipinsert1.wav" );
	PRECACHE_SOUND( "items/cliprelease1.wav" );

	PRECACHE_SOUND( "weapons/smgshoot1.wav" );// H to the K
	PRECACHE_SOUND( "weapons/smgshoot2.wav" );// H to the K
	PRECACHE_SOUND( "weapons/smgshoot3.wav" );// H to the K

	PRECACHE_SOUND( "weapons/glauncher.wav" );
	PRECACHE_SOUND( "weapons/glauncher2.wav" );

	PRECACHE_SOUND( "weapons/357_cock1.wav" );

	m_usSMGGUN = PRECACHE_EVENT( 1, "events/smggun.sc" );
	m_usSMGGUN2 = PRECACHE_EVENT( 1, "events/smggun2.sc" );
}

int CSMGGUN::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "smggunammo";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;
	p->iMaxClip = MP5_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 2;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SMGGUN;
	p->iWeight = SMGGUN_WEIGHT;

	return 1;
}

int CSMGGUN::AddToPlayer( CBasePlayer *pPlayer )
{
	if( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

BOOL CSMGGUN::Deploy()
{
	return DefaultDeploy( "models/v_smggun.mdl", "models/p_smggun.mdl", SMGGUN_DEPLOY, "mp5" );
}

void CSMGGUN::PrimaryAttack()
{

	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );


	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	Vector vecDir;
	if ( m_fInZoom )
	{
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 8192, BULLET_PLAYER_MP5, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed ); //sight on bullet spread you can change the vector
	}
	else
	{
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_4DEGREES, 8192, BULLET_PLAYER_MP5, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed ); // sight off bullet spread you can change the vector
	}
	

    int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	if ( m_fInZoom )
	{
		PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usSMGGUN2, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );
	}
	else
	{
		PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usSMGGUN, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );
	}

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = GetNextAttackDelay( 0.1f );

	if( m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1f;
}

void CSMGGUN::SecondaryAttack( void )
{
	if ( m_pPlayer->pev->fov != 0 )
	{
		SendWeaponAnim( SMGGUN_ZOOM_OFF, 0, 0 ); // sight on anim // if u dont have an anim you can delete this line
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0; // zoom
		m_fInZoom = 0; 
		PlayEmptySound( ); // sound effect
		WeaponIdle(); // just put this here
	}
	else if ( m_pPlayer->pev->fov != 80 )
	{	
		SendWeaponAnim( SMGGUN_ZOOM_ON, 0, 0 ); // sight on anim // if u dont have an anim you can delete this line
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 80; // zoom
		m_fInZoom = 1;
		PlayEmptySound( ); // sound effect
		WeaponIdle(); // just put this here
	}
	m_flNextSecondaryAttack = 1.0;
}

void CSMGGUN::Reload( void )
{
	if( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == MP5_MAX_CLIP )
		return;

	DefaultReload( MP5_MAX_CLIP, SMGGUN_RELOAD, 1.5f );
}

void CSMGGUN::WeaponIdle( void )
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	
	if (m_flSoundDelay != 0 && m_flSoundDelay <= UTIL_WeaponTimeBase() )
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/clipinsert1.wav", RANDOM_FLOAT(0.8, 0.9), ATTN_NORM);
		m_flSoundDelay = 0;
	}

	if( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	
	if ( m_fInZoom )
	{
		{
			iAnim = SMGGUN_SIDLE; // sight on idle anim
			m_flTimeWeaponIdle = (70.0/30.0);
		}
	}	
	else
	{
		{
			iAnim = SMGGUN_IDLE1; // sight off idle anim
			m_flTimeWeaponIdle = (70.0/30.0);
		}
	}
	
	
	SendWeaponAnim( iAnim, 1 );	
	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 ); // how long till we do this again.

}

class CSMGGUNAmmoClip : public CBasePlayerAmmo
{
	void Spawn( void )
	{
		Precache();
		SET_MODEL( ENT( pev ), "models/w_smggun_clip.mdl" );
		CBasePlayerAmmo::Spawn();
	}
	void Precache( void )
	{
		PRECACHE_MODEL( "models/w_smggun_clip.mdl" );
		PRECACHE_SOUND( "items/9mmclip1.wav" );
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = ( pOther->GiveAmmo( AMMO_MP5CLIP_GIVE, "9mm", _9MM_MAX_CARRY ) != -1 );
		if( bResult )
		{
			EMIT_SOUND( ENT( pev ), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
		}
		return bResult;
	}
};

LINK_ENTITY_TO_CLASS( ammo_smggunclip, CSMGGUNAmmoClip )
