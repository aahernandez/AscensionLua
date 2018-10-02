#pragma once
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Core/CommonInclude.hpp"

const int chanceToDropPowerUp = 20;

class Ship;

class Missile : public Disc2D
{
public:
	//Vector m_center
	//float m_radius
	int m_health;
	float m_orientation;
	float m_thrustSpeed;
	float m_rotateSpeed;
	float m_previousDotProductToTarget;
	float m_nextOrientation;
	float m_angleToTarget;
	Ship *m_targetRef;
	unsigned int m_missileIndex;
	Vector2 m_velocity;
	Vector2 m_normalizedVelocity;

	Missile();
	Missile(const Vector2 &position);
	~Missile();

	void Update(float deltaSeconds);
	void OnDeath();
	void DropPowerUp();
	Missile *FindClosestMissileInRange(float range);
};