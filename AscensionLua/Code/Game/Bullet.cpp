#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/Ship.hpp"
#include "Game/Missile.hpp"
#include "Game/GameCommon.hpp"
#include "ThirdParty/Lua/Wrappers/LuaBinding.hpp"

#include <algorithm>

LuaBinding* Bullet::s_binding = nullptr;

std::string Bullet::GetTypeAsString(eBulletType bulletType)
{
	switch(bulletType)
	{
	case BT_NORMAL:
		return "Normal";
	case BT_SHOTGUN:
		return "Shotgun";
	case BT_CANONBALL:
		return "Canonball";
	case BT_LASER:
		return "Laser";
	case BT_LIGHTNING:
		return "Lightning";
	}
	return "";
}

eBulletType Bullet::GetStringFromType(std::string typeString)
{
	if (typeString == "Shotgun")
		return BT_SHOTGUN;
	if (typeString == "Canonball")
		return BT_CANONBALL;
	if (typeString == "Laser")
		return BT_LASER;
	if (typeString == "Lightning")
		return BT_LIGHTNING;

	return BT_NORMAL;
}

Bullet::Bullet()
	: m_timeToDie(5.f)
	, m_velocity(0.f, 0.f)
	, m_playerRef(nullptr)
	, m_bulletType(BT_NORMAL)
	, m_isAttachedToMissile(false)
	, m_lightningRange(400.f)
{
	m_laserOrLightningLine = LineSegment2D(m_center, 50.f);
	m_laserThickness = 1.f;
}

Bullet::Bullet(Player *playerRef, const Vector2& velocity, const Vector2& center)
	: m_timeToDie(5.f)
	, m_velocity(velocity)
	, m_playerRef(playerRef)
	, m_bulletType(BT_NORMAL)
	, m_isAttachedToMissile(false)
	, m_lightningRange(400.f)
{
	m_center = center;
	m_radius = 8.f;
	m_laserOrLightningLine = LineSegment2D(m_center, 50.f);
	m_laserThickness = 1.f;
}

Bullet::~Bullet()
{
}

void Bullet::Initialize()
{

}

void Bullet::Update(float deltaSeconds)
{
	if (m_bulletType != BT_LASER)
	{
		s_binding->ClearStack();
		s_binding->PushFunctionOntoStack("UpdateNormalBullet");
		s_binding->PushFloat(deltaSeconds);
		s_binding->PushUserType(this);
		s_binding->RunFunction(2, 0);
	}
	else if (m_bulletType == BT_LASER)
	{
		s_binding->ClearStack();
		s_binding->PushFunctionOntoStack("UpdateLaser");
		s_binding->PushFloat(deltaSeconds);
		s_binding->PushUserType(this);
		s_binding->RunFunction(2, 0);
	}

	if (m_bulletType == BT_LIGHTNING && !m_isAttachedToMissile)
	{
		s_binding->ClearStack();
		s_binding->PushFunctionOntoStack("UpdateLightning");
		s_binding->PushFloat(deltaSeconds);
		s_binding->PushUserType(this);
		s_binding->RunFunction(2, 0);
	}
}

void Bullet::OnDeath()
{
	s_binding->ClearStack();
	s_binding->PushFunctionOntoStack("OnDeath");
	s_binding->PushUserType(this);
	s_binding->RunFunction(1, 0);
}

void Bullet::CreateLightning()
{
	Vector2 lightningDisplacement = m_laserOrLightningLine.lineEnd - m_laserOrLightningLine.lineStart;
	Vector2 lightningNormal = lightningDisplacement.GetVectorNormal(Vector2(lightningDisplacement.y, -lightningDisplacement.x));
	float lightningLength = lightningDisplacement.CalcLength();

	std::vector<float> lightningSectionPositions;
	lightningSectionPositions.push_back(0.f);

	for (int lightningSectionCount = 0; lightningSectionCount < lightningLength / 4; lightningSectionCount++)
	{
		lightningSectionPositions.push_back(GetRandomFloatZeroToOne());
	}

	std::sort(lightningSectionPositions.begin(), lightningSectionPositions.end());

	const float lightningSectionSway = 80;
	const float lightningSectionJaggedness = 1 / lightningSectionSway;

	Vector2 prevLightningSection = m_laserOrLightningLine.lineStart;
	float prevDisplacement = 0;
	for (int lightningSectionPositionIndex = 1; lightningSectionPositionIndex < (int)lightningSectionPositions.size(); lightningSectionPositionIndex++)
	{
		float pos = lightningSectionPositions[lightningSectionPositionIndex];
		float scale = (lightningLength * lightningSectionJaggedness) * (pos - lightningSectionPositions[lightningSectionPositionIndex - 1]);
		float envelope = pos > 0.95f ? 20 * (1 - pos) : 1;
		float displacement = GetRandomFloatInRange(-lightningSectionSway, lightningSectionSway);
		displacement -= (displacement - prevDisplacement) * (1 - scale);
		displacement *= envelope;

		Vector2 point = m_laserOrLightningLine.lineStart + pos * lightningDisplacement + displacement * lightningNormal;
		m_lightningPoints.push_back(LineSegment2D(prevLightningSection, point));
		prevLightningSection = point;
		prevDisplacement = displacement;
	}
	
	m_lightningPoints.push_back(LineSegment2D(prevLightningSection, m_laserOrLightningLine.lineEnd));
}