#pragma once
#include "ThirdParty/Lua/Wrappers/LuaBinding.hpp"

class LuaBinding;

static int GetBulletCenterX_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	binder.PushFloat(bullet->m_center.x);
	return 1;
}

static int GetBulletCenterY_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	binder.PushFloat(bullet->m_center.y);
	return 1;
}

static int SetCenter_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	bullet->m_center.x = binder.CheckFloat(2);
	bullet->m_center.y = binder.CheckFloat(3);
	return 0;
}

static int GetVelocityX_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	binder.PushFloat(bullet->m_velocity.x);
	return 1;
}

static int GetVelocityY_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	binder.PushFloat(bullet->m_velocity.y);
	return 1;
}

static int SetVelocity_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	bullet->m_velocity.x = binder.CheckFloat(2);
	bullet->m_velocity.y = binder.CheckFloat(3);
	return 0;
}

static int GetTimeToDie_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	binder.PushFloat(bullet->m_timeToDie);
	return 1;
}

static int SetTimeToDie_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	bullet->m_timeToDie = binder.CheckFloat(2);
	return 0;
}

static int GetShipOrientation_bind(lua_State *L)
{
	LuaBinding binder(L);
	Ship *shipRef = Game::s_gameInstance->m_player->m_ship;
	if (shipRef != nullptr)
		binder.PushFloat(shipRef->m_orientation);
	else
		binder.PushFloat(0.f);
	return 1;
}

static int GetShipCenterX_bind(lua_State *L)
{
	LuaBinding binder(L);
	Ship *shipRef = Game::s_gameInstance->m_player->m_ship;
	if (shipRef != nullptr)
		binder.PushFloat(shipRef->m_center.x);
	else
		binder.PushFloat(0.f);
	return 1;
}

static int GetShipCenterY_bind(lua_State *L)
{
	LuaBinding binder(L);
	Ship *shipRef = Game::s_gameInstance->m_player->m_ship;
	if (shipRef != nullptr)
		binder.PushFloat(shipRef->m_center.y);
	else
		binder.PushFloat(0.f);
	return 1;
}

static int GetShipVelocityX_bind(lua_State *L)
{
	LuaBinding binder(L);
	Ship *shipRef = Game::s_gameInstance->m_player->m_ship;
	if (shipRef != nullptr)
		binder.PushFloat(shipRef->m_velocity.x);
	else
		binder.PushFloat(0.f);
	return 1;
}

static int GetShipVelocityY_bind(lua_State *L)
{
	LuaBinding binder(L);
	Ship *shipRef = Game::s_gameInstance->m_player->m_ship;
	if (shipRef != nullptr)
		binder.PushFloat(shipRef->m_velocity.y);
	else
		binder.PushFloat(0.f);
	return 1;
}

static int GetNormalizedVectorX_bind(lua_State *L)
{
	LuaBinding binder(L);
	float orientation = binder.CheckFloat(1);
	Vector2 vec;
	vec.SetUnitLengthAndHeadingDegrees(orientation);
	binder.PushFloat(vec.x);
	return 1;
}

static int GetNormalizedVectorY_bind(lua_State *L)
{
	LuaBinding binder(L);
	float orientation = binder.CheckFloat(1);
	Vector2 vec;
	vec.SetUnitLengthAndHeadingDegrees(orientation);
	binder.PushFloat(vec.y);
	return 1;
}

static int SetLaserCenter_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	Vector2 newCenter;
	newCenter.x = binder.CheckFloat(2);
	newCenter.y = binder.CheckFloat(3);
	bullet->m_laserOrLightningLine.SetCenter(newCenter);
	return 0;
}

static int SetRotationAroundPoint_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	Vector2 rotationPoint;
	rotationPoint.x = binder.CheckFloat(2);
	rotationPoint.y = binder.CheckFloat(3);
	float orientation = binder.CheckFloat(4);
	bullet->m_laserOrLightningLine.SetRotationDegreesAroundPoint(rotationPoint, orientation);
	return 0;
}

static int DrawTextToScreen(lua_State *L)
{
	LuaBinding binder(L);
	const char* str = binder.CheckString(1);
	g_theSimpleRenderer->DrawText2D(str, Vector2(500.f, 500.f));
	return 0;
}

//#TODO
//start adding from here
static int IsShipNull_bind(lua_State *L)
{
	LuaBinding binder(L);
	Ship *shipRef = Game::s_gameInstance->m_player->m_ship;
	bool isShipNull = shipRef == nullptr;
	binder.PushBool(isShipNull);
	return 1;
}

static int FindClosestMissile_bind(lua_State *L)
{
	LuaBinding binder(L);
	Ship *shipRef = Game::s_gameInstance->m_player->m_ship;
	float range = binder.CheckFloat(1);
	Missile *missile = shipRef->FindClosestMissileInRange(range);
	binder.PushUserType(missile);
	return 1;
}

static int IsMissileNull_bind(lua_State *L)
{
	LuaBinding binder(L);
	Missile *missile = (Missile*)binder.CheckUserType(1, "Missile");
	binder.PushBool(missile == nullptr);
	return 1;
}

static int GetMissileCenterX_bind(lua_State *L)
{
	LuaBinding binder(L);
	Missile *missile = (Missile*)binder.CheckUserType(1, "Missile");
	binder.PushFloat(missile->m_center.x);
	return 1;
}

static int GetMissileCenterY_bind(lua_State *L)
{
	LuaBinding binder(L);
	Missile *missile = (Missile*)binder.CheckUserType(1, "Missile");
	binder.PushFloat(missile->m_center.y);
	return 1;
}

static int SetLaserLineEnd_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	Vector2 missileCenter;
	missileCenter.x = binder.CheckFloat(2);
	missileCenter.y = binder.CheckFloat(3);
	bullet->m_laserOrLightningLine.lineEnd = Vector2(missileCenter);
	return 0;
}

static int SetIsAttachedToMissile_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	bool isAttached = binder.CheckBool(2);
	bullet->m_isAttachedToMissile = isAttached;
	return 0;
}

static int GetMissileHealth_bind(lua_State *L)
{
	LuaBinding binder(L);
	Missile *missile = (Missile*)binder.CheckUserType(1, "Missile");
	binder.PushInteger(missile->m_health);
	return 1;
}

static int SetMissileHealth_bind(lua_State *L)
{
	LuaBinding binder(L);
	Missile *missile = (Missile*)binder.CheckUserType(1, "Missile");
	int newHealth = binder.CheckInteger(2);
	missile->m_health = newHealth;
	return 0;
}

static int CreateLightningShape_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	bullet->CreateLightning();
	return 0;
}

static int GetBulletType_bind(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	std::string bulletType = Bullet::GetTypeAsString(bullet->m_bulletType);
	binder.PushString(bulletType);
	return 1;
}

static int GetBulletRadius(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	binder.PushFloat(bullet->m_radius);
	return 1;
}

static int CreateBullet(lua_State *L)
{
	LuaBinding binder(L);
	Vector2 velocity;
	Vector2 center;
	velocity.x = binder.CheckFloat(1);
	velocity.y = binder.CheckFloat(2);
	center.x = binder.CheckFloat(3);
	center.y = binder.CheckFloat(4);
	Bullet *newBullet = new Bullet(Game::s_gameInstance->m_player, velocity, center);
	binder.PushUserType(newBullet);
	return 1;
}

static int SetBulletType(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	std::string typeString = binder.CheckString(2);
	eBulletType bulletType = Bullet::GetStringFromType(typeString);
	bullet->m_bulletType = bulletType;
	return 0;
}

static int AddBullet(lua_State *L)
{
	LuaBinding binder(L);
	Bullet *bullet = (Bullet*)binder.CheckUserType(1, "Bullet");
	Game::s_gameInstance->AddBullet(bullet);
	return 0;
}

static const luaL_reg bulletLib[] =
{
	{"GetCenterX", GetBulletCenterX_bind},
	{"GetCenterY", GetBulletCenterY_bind},
	{"SetCenter", SetCenter_bind},
	{"GetVelocityX", GetVelocityX_bind},
	{"GetVelocityY", GetVelocityY_bind},
	{"SetVelocity", SetVelocity_bind},
	{"GetTimeToDie", GetTimeToDie_bind},
	{"SetTimeToDie", SetTimeToDie_bind},
	{"GetBulletType", GetBulletType_bind},
	{"GetRadius", GetBulletRadius},
	{"Create", CreateBullet},
	{"SetType", SetBulletType},
	{NULL, NULL},
};

static const luaL_reg laserLib[] =
{
	{"SetCenter", SetLaserCenter_bind},
	{"SetRotationAroundPoint", SetRotationAroundPoint_bind},
	{NULL, NULL},
};

static const luaL_reg lightningLib[] =
{
	{"SetLineEnd", SetLaserLineEnd_bind},
	{"SetIsAttached", SetIsAttachedToMissile_bind},
	{"CreateLightningShape", CreateLightningShape_bind},
	{NULL, NULL},
};

static const luaL_reg gameLib[] =
{
	{"GetShipOrientation", GetShipOrientation_bind},
	{"GetShipCenterX", GetShipCenterX_bind},
	{"GetShipCenterY", GetShipCenterY_bind},
	{"GetShipVelocityX", GetShipVelocityX_bind},
	{"GetShipVelocityY", GetShipVelocityY_bind},
	{"IsShipNull", IsShipNull_bind},
	{"FindClosestMissile", FindClosestMissile_bind},
	{"AddBullet", AddBullet},
	{NULL, NULL},
};

static const luaL_reg vectorLib[] =
{
	{"GetNormalizedVectorX", GetNormalizedVectorX_bind},
	{"GetNormalizedVectorY", GetNormalizedVectorY_bind},
	{NULL, NULL},
};

static const luaL_reg debugLib[] =
{
	{"DebugDrawText", DrawTextToScreen},
	{NULL, NULL},
};

static const luaL_reg missileLib[] =
{
	{"IsNull", IsMissileNull_bind},
	{"GetCenterX", GetMissileCenterX_bind},
	{"GetCenterY", GetMissileCenterY_bind},
	{"GetHealth", GetMissileHealth_bind},
	{"SetHealth", SetMissileHealth_bind},
	{NULL, NULL},
};