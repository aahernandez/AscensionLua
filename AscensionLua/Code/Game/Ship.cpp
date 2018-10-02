#include "Game/Ship.hpp"
#include "Game/Game.hpp"
#include "Game/Bullet.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/Missile.hpp"
#include "Game/Powerup.hpp"
#include "Engine/Network/NetObject.hpp"
#include "Engine/Network/NetObjectSystem.hpp"
#include "Engine/Network/NetMessage.hpp"
#include "Engine/Network/TCPSession.hpp"
#include "Engine/Network/NetConnection.hpp"
#include "Engine/Network/NetObjectConnectionState.hpp"

#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"

Ship::Ship()
	: m_health(20)
	, m_thrust(0.f)
	, m_steer(0.f)
	, m_rotationSpeed(270.f)
	, m_rotationSpeedWhileMoving(135.f)
	, m_thrustPower(20.f)
	, m_orientation(0.f)
	, m_velocity(0.f, 0.f)
	, m_normalizedVelocity(0.f, 0.f)
	, m_nextOrientation(0.f)
	, m_shotgunSpreadDegrees(5.f)
	, m_isFiringLaser(false)
	, m_maximumSpeed(250.f)
	, m_gravity(-4.8f)
	, m_bulletSpeed(600.f)
	, m_kickbackSpeed(600.f)
	, m_oceanLineTimer(0.5f)
	, m_outOfBoundsTimeToTakeDamage(0.5f)
	, m_smokeSpawnTime(0.02f)
	, m_timeSinceLastSmokeSpawn(0.5f)
	, m_wasPowerupPickedup(false)
	, m_timeSinceLastShot(0.f)
{
	const Vector2 worldSize = Game::s_gameInstance->m_worldSize;
	m_center = Vector2(worldSize.x * 0.5f, 0.f);
	m_radius = (15.f);
	m_currentPowerup = new Powerup();
}

Ship::~Ship()
{

}

void Ship::Update(float deltaSeconds)
{
	UpdateSmoke(deltaSeconds);
	m_timeSinceLastShot += deltaSeconds;
	m_currentPowerup->OnShipUpdate(deltaSeconds);
}

void Ship::UpdateSmoke(float deltaSeconds)
{
	m_timeSinceLastSmokeSpawn -= deltaSeconds;
	if (m_timeSinceLastSmokeSpawn <= 0.f && m_thrust != 0.f)
	{
		SpawnSmoke();
		m_timeSinceLastSmokeSpawn = m_smokeSpawnTime;
	}
}

void Ship::UpdateMovement(float deltaSeconds)
{
	float rotationSpeed = m_rotationSpeed;
	if (m_thrust != 0.f)
	{
		rotationSpeed = m_rotationSpeedWhileMoving;
	}

	if (m_steer != 0.f)
	{
		float rotationDampening = 1.f;
		if (IsBelowOcean())
		{
			rotationDampening = 0.3f;
		}

		m_orientation += m_steer * rotationDampening * rotationSpeed * deltaSeconds;
	}

	if (IsBelowOcean())
	{
		m_oceanLineTimer -= deltaSeconds;
		if (m_oceanLineTimer <= 0.f)
		{
			m_health--;
			m_oceanLineTimer = m_outOfBoundsTimeToTakeDamage;
		}

		Vector2 shipOrietation;
		shipOrietation.SetUnitLengthAndHeadingDegrees(m_orientation);

		Vector2 upwardDirection = Vector2(0.f, 1.f);
		float dotProduct = DotProduct(shipOrietation, upwardDirection);

		if (dotProduct < 0.9)
		{
			float rotationDampening = 0.8f;
			float newOrientation = m_orientation + (rotationSpeed * rotationDampening * deltaSeconds);
			Vector2 newNormalizedVelocity;
			newNormalizedVelocity.SetUnitLengthAndHeadingDegrees(newOrientation);
			float newDotProduct = DotProduct(newNormalizedVelocity, upwardDirection);

			if (newDotProduct > dotProduct)
			{
				m_orientation = newOrientation;
			}
			else
			{
				m_orientation -= rotationSpeed * rotationDampening * deltaSeconds;
			}
		}
	}
	else if (IsAboveSky())
	{
		m_oceanLineTimer -= deltaSeconds;
		if (m_oceanLineTimer <= 0.f)
		{
			m_health--;
			m_oceanLineTimer = m_outOfBoundsTimeToTakeDamage;
		}

		Vector2 shipOrietation;
		shipOrietation.SetUnitLengthAndHeadingDegrees(m_orientation);

		Vector2 upwardDirection = Vector2(0.f, -1.f);
		float dotProduct = DotProduct(shipOrietation, upwardDirection);

		if (dotProduct < 0.9)
		{
			float rotationDampening = 0.8f;
			float newOrientation = m_orientation + (rotationSpeed * rotationDampening * deltaSeconds);
			Vector2 newNormalizedVelocity;
			newNormalizedVelocity.SetUnitLengthAndHeadingDegrees(newOrientation);
			float newDotProduct = DotProduct(newNormalizedVelocity, upwardDirection);

			if (newDotProduct > dotProduct)
			{
				m_orientation = newOrientation;
			}
			else
			{
				m_orientation -= rotationSpeed * rotationDampening * deltaSeconds;
			}
		}
	}
	else
	{
		m_oceanLineTimer = m_outOfBoundsTimeToTakeDamage;
	}

	if (m_thrust > 0.f)
	{
		m_normalizedVelocity.SetUnitLengthAndHeadingDegrees(m_orientation);
		m_velocity += m_normalizedVelocity * m_thrust * m_thrustPower;
	}
	else
	{
		if (IsBelowOcean())
		{
			m_velocity += Vector2(0.f, -m_gravity * 2.f);
		}
		else
		{
			m_velocity += Vector2(0.f, m_gravity);
		}
	}


	if (m_velocity.CalcLength() > m_maximumSpeed)
	{
		m_velocity.SetLength(m_maximumSpeed);
	}

	Vector2 newPos = m_center;
	newPos += m_velocity * deltaSeconds;

	if (newPos != Vector2::ZERO)
	{
		UpdatePosition(newPos);
	}
}

void Ship::UpdatePosition(Vector2 newPos)
{
	m_center = newPos;
}

void Ship::SpawnSmoke()
{
	Vector2 normalVelocity;
	normalVelocity.SetUnitLengthAndHeadingDegrees(m_orientation);
	Game::s_gameInstance->m_thrustSmokes.push_back(new ThrustSmoke(m_center + (normalVelocity * -m_radius * 2.f), m_orientation));
}

bool Ship::IsBelowOcean()
{
	if (m_center.y < 250.f)
	{
		return true;
	}
	return false;
}

bool Ship::IsAboveSky()
{
	Vector2 &worldSize = Game::s_gameInstance->m_worldSize;
	if (m_center.y > worldSize.y)
	{
		return true;
	}
	return false;
}

bool Ship::ShootBullet()
{
	if (m_timeSinceLastShot >= m_currentPowerup->m_reloadTime)
	{
		switch (m_currentPowerup->m_powerupType)
		{
		case PU_NONE:
			return ShootNormalBullet();
			break;
		case PU_SHOTGUN:
			return ShootShotgunBullet();
			break;
		case PU_LASER:
			return ShootLaserBullet();
			break;
		case PU_CANONBALL:
			return ShootCanonballBullet();
			break;
		case PU_LIGHTNING:
			return ShootLightningBullet();
			break;
		}
	}
	return false;
}

bool Ship::ShootNormalBullet(float newLifetime, float orientation, eBulletType type /*= BT_NORMAL*/, float radius /*= -1.f*/)
{
	m_timeSinceLastShot = 0.f;
	Vector2 normalVelocity;
	if (orientation != -1.f)
	{
		normalVelocity.SetUnitLengthAndHeadingDegrees(orientation);
	}
	else
	{
		normalVelocity.SetUnitLengthAndHeadingDegrees(m_orientation);
	}
	Vector2 bulletVelocity = (normalVelocity * m_bulletSpeed);
	Bullet *newBullet = new Bullet(Game::s_gameInstance->m_player, bulletVelocity, m_center + (normalVelocity * m_radius));
	if (newLifetime != -1.f)
	{
		newBullet->m_timeToDie = newLifetime;
	}
	if (radius != -1.f)
	{
		newBullet->m_radius = radius;
	}
	if (type == BT_LASER)
	{
		newBullet->m_center = m_center + (normalVelocity * radius);
		newBullet->m_laserOrLightningLine = LineSegment2D(newBullet->m_center, radius);
		newBullet->m_laserOrLightningLine.SetRotationDegreesAroundPoint(Game::s_gameInstance->m_player->m_ship->m_center, Game::s_gameInstance->m_player->m_ship->m_orientation);
	}
	else if (type == BT_LIGHTNING)
	{
		newBullet->m_laserOrLightningLine = LineSegment2D(m_center, m_center);
		newBullet->m_velocity = Vector2::ZERO;
	}
	else if (type == BT_CANONBALL)
	{
		m_velocity += m_normalizedVelocity * -m_kickbackSpeed;
	}
	newBullet->m_bulletType = type;
	Game::s_gameInstance->AddBullet(newBullet);

	return true;
}

bool Ship::ShootShotgunBullet()
{
	ShootNormalBullet(-1.f, m_orientation);
	ShootNormalBullet(-1.f, m_orientation + m_shotgunSpreadDegrees);
	ShootNormalBullet(-1.f, m_orientation + (m_shotgunSpreadDegrees * 2.f));
	ShootNormalBullet(-1.f, m_orientation - m_shotgunSpreadDegrees);
	return ShootNormalBullet(-1.f, m_orientation - (m_shotgunSpreadDegrees * 2.f));
}

bool Ship::ShootLaserBullet()
{
	if (m_isFiringLaser)
		return true;
	m_isFiringLaser = true;
	return ShootNormalBullet(m_currentPowerup->m_effectLifetime, m_orientation, BT_LASER, 200.f);
}

bool Ship::ShootCanonballBullet()
{
	return ShootNormalBullet(m_currentPowerup->m_effectLifetime, m_orientation, BT_CANONBALL, 30.f);
}

bool Ship::ShootLightningBullet()
{
	return ShootNormalBullet(m_currentPowerup->m_effectLifetime, m_orientation, BT_LIGHTNING);
}

Missile* Ship::FindClosestMissileInRange(float range)
{
	std::vector<Missile*> &missiles = Game::s_gameInstance->m_missiles;
	std::vector<Missile*>::iterator missileIter;
	Missile *closestMissile = nullptr;
	float closestDistance = range;
	for (missileIter = missiles.begin(); missileIter != missiles.end(); ++missileIter)
	{
		Missile *currentMissile = *missileIter;
		if (currentMissile == nullptr)
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
