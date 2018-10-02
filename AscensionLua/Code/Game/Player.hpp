#pragma once
#include "Engine/Math/Vector2.hpp"

#include <string>

// class Ship;

class Player
{
public:
	float m_respawnTime;
	float m_timeSinceDeath;
	Ship *m_ship;

	Player();
	~Player();

	void Update(float deltaSeconds);
	void KeyPress();
};