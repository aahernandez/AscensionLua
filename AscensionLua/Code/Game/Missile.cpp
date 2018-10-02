#include "Game/Missile.hpp"
#include "Game/Ship.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/Powerup.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Network/NetObject.hpp"
#include "Engine/Network/NetObjectSystem.hpp"
#include "Engine/Network/NetObjectConnectionState.hpp"

Missile::Missile()
	: m_health(1)
	, m_orientation(0.f)
	, m_thrustSpeed(100.f)
	, m_rotateSpeed(35.f)
	, m_velocity(0.f, 0.f)
	, m_normalizedVelocity(0.f, 0.f)
	, m_nextOrientation(0.f)
	, m_missileIndex(0)
{
	m_center = Vector2(0.f, 0.f);
	m_radius = (15.f);
}

Missile::Missile(const Vector2 &position)
	: m_health(1)
	, m_orientation(0.f)
	, m_thrustSpeed(100.f)
	, m_rotateSpeed(35.f)
	, m_velocity(0.f, 0.f)
	, m_normalizedVelocity(0.f, 0.f)
	, m_nextOrientation(0.f)
	, m_missileIndex(0)
{
	m_center = position;
	m_radius = (15.f);
}

Missile::~Missile()
{

}

void Missile::Update(float deltaSeconds)
{
	Ship *targetShip = Game::s_gameInstance->m_player->m_ship;
	if (targetShip != nullptr)
	{
		Vector2 displacementToPlayer = m_center - targetShip->m_center;
		Vector2 normalizedDisp = displacementToPlayer;
		normalizedDisp.Normalize();
		
		Vector2 turretOrietation;
		turretOrietation.SetUnitLengthAndHeadingDegrees(m_orientation);

		float dotProduct = DotProduct(normalizedDisp, turretOrietation);
		dotProduct = -dotProduct;
		m_angleToTarget = dotProduct;

		if (m_previousDotProductToTarget > dotProduct)
		{
			m_rotateSpeed = -m_rotateSpeed;
		}

		m_orientation += m_rotateSpeed * deltaSeconds;
		m_previousDotProductToTarget = dotProduct;
	}
	m_normalizedVelocity.SetUnitLengthAndHeadingDegrees(m_orientation);
	m_velocity = m_normalizedVelocity * m_thrustSpeed;

	Vector2 newPos = m_center;
	m_center += m_velocity * deltaSeconds;
}

void Missile::OnDeath()
{
	int randomChanceNum = GetRandomIntInRange(0, 100);
	if (randomChanceNum >= chanceToDropPowerUp)
	{
		DropPowerUp();
	}
}

void Missile::DropPowerUp()
{
	int powerUpNum = GetRandomIntInRange(1, NUM_POWERUPS - 1);
	Powerup *powerUp = new Powerup((ePowerupType)powerUpNum);
	powerUp->m_center = m_center;
	Game::s_gameInstance->AddPowerUp(powerUp);
}

Missile* Missile::FindClosestMissileInRange(float range)
{
	std::vector<Missile*> &missiles = Game::s_gameInstance->m_missiles;
	std::vector<Missile*>::iterator missileIter;
	Missile *closestMissile = nullptr;
	float closestDistance = range;
	for (missileIter = missiles.begin(); missileIter != missiles.end(); ++missileIter)
	{
		Missile *currentMissile = *missileIter;
		if (currentMissile == nullptr || currentMissile == this)
			continue;
		float distance = m_center.CalcDistanceToVector(currentMissile->m_center);
		if (distance < closestDistance)
		{
			closestMissile = currentMissile;
			closestDistance = distance;
		}
	}
	return closestMissile;
}
