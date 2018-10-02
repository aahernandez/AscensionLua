#pragma once
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Core/CommonInclude.hpp"

class Battleship : public Disc2D
{
public:
	//Vector m_center
	//float m_radius
	int m_health;
	float m_speed;
	float m_timeToFireMissile;
	float m_timeSinceLastMissile;
	bool m_isFacingRight;
	unsigned int m_battleshipIndex;
	uint16_t m_netID;
	Vector2 m_velocity;

	Battleship();
	~Battleship();

	void Update(float deltaSeconds);
	void ShootMissile();
	void OnDeath();
};