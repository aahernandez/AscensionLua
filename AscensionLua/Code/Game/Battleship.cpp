#include "Game/Ship.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/Missile.hpp"
#include "Game/Battleship.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Network/NetObject.hpp"
#include "Engine/Network/NetObjectSystem.hpp"
#include "Engine/Network/NetObjectConnectionState.hpp"

Battleship::Battleship()
	: m_health(50)
	, m_speed(50.f)
	, m_isFacingRight(false)
	, m_velocity(0.f, 0.f)
	, m_battleshipIndex(0)
	, m_netID(0)
	, m_timeToFireMissile(5.f)
	, m_timeSinceLastMissile(0.f)
{
	m_radius = 200.f;
}

Battleship::~Battleship()
{

}

void Battleship::Update(float deltaSeconds)
{
	m_velocity = Vector2(1.f, 0.f) * m_speed * deltaSeconds;
	m_center += m_velocity;

	m_timeSinceLastMissile += deltaSeconds;
	if (m_timeSinceLastMissile >= m_timeToFireMissile)
	{
		ShootMissile();
		m_timeSinceLastMissile = 0.f;
	}
}

void Battleship::ShootMissile()
{
	if (Game::s_gameInstance->GetNumMissiles() > Game::s_gameInstance->m_maxMissilies)
	{
		return;
	}

	Vector2 spawnPos = m_center;
	Ship *shipRef = Game::s_gameInstance->m_player->m_ship;
	if (shipRef == nullptr)
		return;

	Missile *newMissile = new Missile(spawnPos);
	newMissile->m_targetRef = shipRef;
	Game::s_gameInstance->AddMissile(newMissile);
}

void Battleship::OnDeath()
{

}
