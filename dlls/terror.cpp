//Allready fixed
/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include      "explode.h"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define	ZOMBIE_AE_ATTACK_RIGHT		0x01
#define	ZOMBIE_AE_ATTACK_LEFT		0x02
#define	ZOMBIE_AE_ATTACK_BOTH		0x03

#define ZOMBIE_FLINCH_DELAY		2		// at most one flinch every n secs

class CTerror : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void SetYawSpeed( void );
	int Classify( void );
	void HandleAnimEvent( MonsterEvent_t *pEvent );
	int IgnoreConditions( void );

	float m_flNextFlinch;

	void IdleSound( void );
    void DeathSound ( void );

	static const char *pIdleSounds[];
	static const char *pDeathSounds[];

	// No range attacks
	BOOL CheckRangeAttack1( float flDot, float flDist ) { return FALSE; }
	BOOL CheckRangeAttack2( float flDot, float flDist ) { return FALSE; }
	int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
};

LINK_ENTITY_TO_CLASS( monster_virusterror, CTerror )

const char *CTerror::pIdleSounds[] =
{
	"terror/virus.wav",
};

const char *CTerror::pDeathSounds[] =
{
	"terror/die.wav",
};



//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int CTerror::Classify( void )
{
	return	CLASS_ALIEN_MONSTER;
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CTerror::SetYawSpeed( void )
{
	int ys;

	ys = 120;
#if 0
	switch ( m_Activity )
	{
	}
#endif
	pev->yaw_speed = ys;
}

int CTerror::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	// Take 30% damage from bullets
	if( bitsDamageType == DMG_BULLET )
	{
		Vector vecDir = pev->origin - (pevInflictor->absmin + pevInflictor->absmax) * 0.5;
		vecDir = vecDir.Normalize();
		float flForce = DamageForce( flDamage );
		pev->velocity = pev->velocity + vecDir * flForce;
		flDamage *= 0.3;
	}

	// HACK HACK -- until we fix this.
	if( IsAlive() )
	return CBaseMonster::TakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}


void CTerror::IdleSound( void )
{
	int pitch = 95 + RANDOM_LONG( 0, 9 );

	// Play a random idle sound
	EMIT_SOUND_DYN( ENT( pev ), CHAN_VOICE, pIdleSounds[RANDOM_LONG( 0, ARRAYSIZE( pIdleSounds ) -1 )], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG( -5, 5 ) );
}

void CTerror::DeathSound( void )
{
	EMIT_SOUND_DYN( ENT( pev ), CHAN_VOICE, pIdleSounds[RANDOM_LONG( 0, ARRAYSIZE( pDeathSounds ) -1 )], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG( -5, 5 ) );
}


void CTerror::HandleAnimEvent( MonsterEvent_t *pEvent )
{
	switch( pEvent->event )
	{
		case ZOMBIE_AE_ATTACK_RIGHT:
		{
			ExplosionCreate(pev->origin ,pev->origin ,edict() ,60 , true ); 
		}
		break;
		case ZOMBIE_AE_ATTACK_LEFT:
{
			ExplosionCreate(pev->origin ,pev->origin,edict() ,60 , true ); 
		}
		break;
		case ZOMBIE_AE_ATTACK_BOTH:
		{
			ExplosionCreate(pev->origin ,pev->origin,edict() ,60, true);
		}
		break;
		default:
			CBaseMonster::HandleAnimEvent( pEvent );
			break;
	}
}
 
 
void CTerror::Spawn()
{
	Precache();

	SET_MODEL( ENT(pev), "models/terror.mdl" );
	UTIL_SetSize( pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX );

	pev->solid		= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	m_bloodColor		= BLOOD_COLOR_RED;
	pev->health		= 10;
	pev->view_ofs		= VEC_VIEW;// position of the eyes relative to monster's origin.
	m_flFieldOfView		= 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;
	m_afCapability		= bits_CAP_DOORS_GROUP;

	MonsterInit();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CTerror::Precache()
{
	int i;

	PRECACHE_MODEL( "models/terror.mdl" );

	for( i = 0; i < ARRAYSIZE( pIdleSounds ); i++ )
		PRECACHE_SOUND( (char *)pIdleSounds[i] );
	PRECACHE_SOUND_ARRAY( pDeathSounds );
	PRECACHE_SOUND( "terror/die.wav.wav" );
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================

int CTerror::IgnoreConditions( void )
{
	int iIgnore = CBaseMonster::IgnoreConditions();

	if( ( m_Activity == ACT_MELEE_ATTACK1 ) || ( m_Activity == ACT_MELEE_ATTACK1 ) )
	{
#if 0
		if( pev->health < 20 )
			iIgnore |= ( bits_COND_LIGHT_DAMAGE| bits_COND_HEAVY_DAMAGE );
		else
#endif
		if( m_flNextFlinch >= gpGlobals->time )
			iIgnore |= ( bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE );
	}

	if( ( m_Activity == ACT_SMALL_FLINCH ) || ( m_Activity == ACT_BIG_FLINCH ) )
	{
		if( m_flNextFlinch < gpGlobals->time )
			m_flNextFlinch = gpGlobals->time + ZOMBIE_FLINCH_DELAY;
	}

	return iIgnore;
}
