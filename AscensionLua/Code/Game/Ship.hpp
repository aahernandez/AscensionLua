#pragma once
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Core/CommonInclude.hpp"

class Powerup;
class Missile;
enum eBulletType;

struct ThrustSmoke
{
	Vector2 m_position;
	float m_orientation;
	float m_lifetime;

	ThrustSmoke(Vector2 newPos, float newOrientation){m_position = newPos; m_orientation = newOrientation; m_lifetime = 2.f;}
};

class Ship : public Disc2D
{
public:
	//Vector m_center
	//float m_radius
	int m_health;
	float m_steer;
	float m_thrust;
	float m_rotationSpeed;
	float m_rotationSpeedWhileMoving;
	float m_thrustPower;
	float m_orientation;
	float m_bulletSpeed;
	float m_nextOrientation;
	float m_kickbackSpeed;
	float m_oceanLineTimer;
	float m_outOfBoundsTimeToTakeDamage;
	float m_smokeSpawnTime;
	float m_timeSinceLastSmokeSpawn;
	Vector2 m_velocity;
	Vector2 m_normalizedVelocity;
	float m_shotgunSpreadDegrees;
	bool m_isFiringLaser;
	Powerup *m_currentPowerup;
	float m_maximumSpeed;
	float m_gravity;
	float m_velocityDrag;
	bool m_wasPowerupPickedup;
	float m_timeSinceLastShot;
	
	Ship();
	~Ship();

	void Update(float deltaSeconds);
	void UpdateMovement(float deltaSeconds);
	void UpdatePosition(Vector2 newPosition);
	void UpdateSmoke(float deltaSeconds);
	void SpawnSmoke();
	bool IsBelowOcean();
	bool IsAboveSky();
	bool ShootBullet();
	bool ShootNormalBullet(float newLifetime = -1.f, float orientation = -1.f, eBulletType type = (eBulletType)0, float radius = -1.f);
	bool ShootShotgunBullet();
	bool ShootLaserBullet();
	bool ShootCanonballBullet();
	bool ShootLightningBullet();
	Missile* FindClosestMissileInRange(float range);
};