/***
*
*	Copyright (c) 2007-2010, Hammermaps.de All rights reserved.
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

enum ak47_e
{
	AK47_LONGIDLE = 0,
	AK47_IDLE1,
	AK47_SHOOT_1,
	AK47_RELOAD,
	AK47_DEPLOY_1,
	AK47_SHOOT_2,
	AK47_SHOOT_3,
	AK47_SIDLE,
	AK47_SFIRE1,
	AK47_ZOOM_ON,
	AK47_ZOOM_OFF
};



LINK_ENTITY_TO_CLASS(weapon_ak47, Cak47);

//=========================================================
//=========================================================
int Cak47::SecondaryAmmoIndex(void)
{
	return m_iSecondaryAmmoType;
}

void Cak47::Spawn()
{
	pev->classname = MAKE_STRING("weapon_ak47"); // hack to allow for old names
	Precache();
	SET_MODEL(ENT(pev), "models/w_ak47.mdl");
	m_iId = WEAPON_AK47;

	m_iDefaultAmmo = AK47_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}


void Cak47::Precache(void)
{
	PRECACHE_MODEL("models/v_ak47.mdl");
	PRECACHE_MODEL("models/w_ak47.mdl");
	PRECACHE_MODEL("models/p_ak47.mdl");

	m_iShell = PRECACHE_MODEL("models/ak47_shell.mdl");// brass shellTE_MODEL

	PRECACHE_MODEL("models/w_ak47clip.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("items/clipinsert1.wav");
	PRECACHE_SOUND("items/cliprelease1.wav");

	PRECACHE_SOUND("weapons/ak471.wav");
	PRECACHE_SOUND("weapons/ak472.wav");
	PRECACHE_SOUND("weapons/ak473.wav");

	PRECACHE_SOUND("weapons/357_cock1.wav");

	m_usak47 = PRECACHE_EVENT(1, "events/ak47.sc");
	m_usak472 = PRECACHE_EVENT(1, "events/ak472.sc");
}

int Cak47::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;
	p->iMaxClip = AK47_MAX_CLIP;
	p->iSlot = 3;
	p->iPosition = 4;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_AK47;
	p->iWeight = AK47_WEIGHT;

	return 1;
}

int Cak47::AddToPlayer(CBasePlayer *pPlayer)
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev);
		WRITE_BYTE(m_iId);
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

BOOL Cak47::Deploy()
{
	return DefaultDeploy("models/v_ak47.mdl", "models/p_ak47.mdl", AK47_DEPLOY_1, "ak47");
}


void Cak47::PrimaryAttack()
{
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
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_MP5, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed ); //sight on bullet spread you can change the vector
	}
	else
	{
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_MP5, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed ); // sight off bullet spread you can change the vector
	}
	

    int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	if ( m_fInZoom )
	{
		PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usak472, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );
	}
	else
	{
		PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usak47, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );
	}

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = 0.1;
	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}



void Cak47::SecondaryAttack(void)
{
	if ( m_pPlayer->pev->fov != 0 )
	{
		SendWeaponAnim( AK47_ZOOM_OFF, 0, 0 ); // sight on anim // if u dont have an anim you can delete this line
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0; // zoom
		m_fInZoom = 0; 
		PlayEmptySound( ); // sound effect
		WeaponIdle(); // just put this here
	}
	else if ( m_pPlayer->pev->fov != 80 )
	{	
		SendWeaponAnim( AK47_ZOOM_ON, 0, 0 ); // sight on anim // if u dont have an anim you can delete this line
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 80; // zoom
		m_fInZoom = 1;
		PlayEmptySound( ); // sound effect
		WeaponIdle(); // just put this here
	}
	m_flNextSecondaryAttack = 1.0;
}

void Cak47::Reload(void)
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == AK47_MAX_CLIP)
		return;
	DefaultReload(AK47_MAX_CLIP, AK47_RELOAD, 2.3);

}


void Cak47::WeaponIdle(void)
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	if (m_flSoundDelay != 0 && m_flSoundDelay <= UTIL_WeaponTimeBase() )
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/clipinsert1.wav", RANDOM_FLOAT(0.8, 0.9), ATTN_NORM);
		m_flSoundDelay = 0;
	}

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;
	int iAnim;
	if ( m_fInZoom )
	{
		{
			iAnim = AK47_SIDLE; // sight on idle anim
			m_flTimeWeaponIdle = (70.0/30.0);
		}
	}	
	else
	{
		{
			iAnim = AK47_IDLE1; // sight off idle anim
			m_flTimeWeaponIdle = (70.0/30.0);
		}
	}
	
	
	SendWeaponAnim( iAnim, 1 );
	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 ); // how long till we do this again.

}



class Cak47AmmoClip : public CBasePlayerAmmo
{
	void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_9mmARclip.mdl");
		CBasePlayerAmmo::Spawn();
	}
	void Precache(void)
	{
		PRECACHE_MODEL("models/w_ak47clip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo(CBaseEntity *pOther)
	{
		int bResult = (pOther->GiveAmmo(AMMO_AK47CLIP_GIVE, "9mm", _9MM_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS(ammo_ak47clip, Cak47AmmoClip);
