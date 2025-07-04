//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#pragma once
#if !defined ( EV_HLDMH )
#define EV_HLDMH

// bullet types
typedef	enum
{
	BULLET_NONE = 0,
	BULLET_PLAYER_9MM, // glock
	BULLET_PLAYER_MP5, // mp5
	BULLET_PLAYER_357, // python
	BULLET_PLAYER_BUCKSHOT, // shotgun
	BULLET_PLAYER_CROWBAR, // crowbar swipe
	BULLET_PLAYER_50CAL, // minigun
	BULLET_PLAYER_338, // Sniper Rifle

	BULLET_MONSTER_9MM,
	BULLET_MONSTER_MP5,
	BULLET_MONSTER_12MM
}Bullet;

enum glock_e
{
    GLOCK_IDLE1 = 0,
    GLOCK_IDLE2,
    GLOCK_IDLE3,
    GLOCK_SHOOT,
    GLOCK_SHOOT_EMPTY,
    GLOCK_RELOAD,
    GLOCK_RELOAD_NOT_EMPTY,
    GLOCK_DRAW,
    GLOCK_HOLSTER,
    GLOCK_ADD_SILENCER,
    GLOCK_SIDLE,
    GLOCK_SFIRE1,
    GLOCK_ZOOM_ON,
    GLOCK_ZOOM_OFF
};

enum shotgun_e
{
	SHOTGUN_IDLE = 0,
	SHOTGUN_FIRE,
	SHOTGUN_FIRE2,
	SHOTGUN_RELOAD,
	SHOTGUN_PUMP,
	SHOTGUN_START_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_HOLSTER,
	SHOTGUN_IDLE4,
	SHOTGUN_IDLE_DEEP
};

enum mp5_e
{
	MP5_LONGIDLE = 0,
	MP5_IDLE1,
	MP5_LAUNCH,
	MP5_RELOAD,
	MP5_DEPLOY,
	MP5_FIRE1,
	MP5_FIRE2,
	MP5_FIRE3,
	MP5_SIDLE,
	MP5_SFIRE1,
	MP5_ZOOM_ON,
	MP5_ZOOM_OFF
};

enum python_e
{
	PYTHON_IDLE1 = 0,
	PYTHON_FIDGET,
	PYTHON_FIRE1,
	PYTHON_RELOAD,
	PYTHON_HOLSTER,
	PYTHON_DRAW,
	PYTHON_IDLE2,
	PYTHON_IDLE3
};

enum minigun_e
{
	MINIGUN_LONGIDLE = 0,
	MINIGUN_IDLE1,
	MINIGUN_RELOAD,
	MINIGUN_DEPLOY,
	MINIGUN_FIRE1,
	MINIGUN_FIRE2,
	MINIGUN_FIRE3,
};


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


#define	GAUSS_PRIMARY_CHARGE_VOLUME	256// how loud gauss is while charging
#define GAUSS_PRIMARY_FIRE_VOLUME	450// how loud gauss is when discharged

enum gauss_e
{
	GAUSS_IDLE = 0,
	GAUSS_IDLE2,
	GAUSS_FIDGET,
	GAUSS_SPINUP,
	GAUSS_SPIN,
	GAUSS_FIRE,
	GAUSS_FIRE2,
	GAUSS_HOLSTER,
	GAUSS_DRAW
};

void EV_HLDM_GunshotDecalTrace( pmtrace_t *pTrace, char *decalName );
void EV_HLDM_DecalGunshot( pmtrace_t *pTrace, int iBulletType );
int EV_HLDM_CheckTracer( int idx, float *vecSrc, float *end, float *forward, float *right, int iBulletType, int iTracerFreq, int *tracerCount );
void EV_HLDM_FireBullets( int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, float flSpreadX, float flSpreadY );
#endif // EV_HLDMH
