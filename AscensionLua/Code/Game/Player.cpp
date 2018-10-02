#include "Game/Game.hpp"
#include "Game/Ship.hpp"
#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/DeveloperConsole.hpp"

Player::Player()
	: m_respawnTime(2.f)
	, m_timeSinceDeath(0.f)
{
	m_ship = new Ship();
}

Player::~Player()
{

}

void Player::Update(float deltaSeconds)
{
	KeyPress();

	if (m_ship == nullptr)
	{
		m_timeSinceDeath += deltaSeconds;
		if (m_timeSinceDeath >= m_respawnTime)
		{
			m_timeSinceDeath = 0.f;
			m_ship = new Ship();
		}
	}
}

void Player::KeyPress()
{
	if (g_theDevConsole->m_isConsoleActive)
		return;

	if (m_ship->m_steer != 1.f && g_theInput->IsKeyDown(KEY_RIGHTARROW))
	{
		m_ship->m_steer = 1.f;
	}
	else if (m_ship->m_steer != -1.f && g_theInput->IsKeyDown(KEY_LEFTARROW))
	{
		m_ship->m_steer = -1.f;
	}
	else if (m_ship->m_steer != 0.f && !g_theInput->IsKeyDown(KEY_RIGHTARROW) && !g_theInput->IsKeyDown(KEY_LEFTARROW))
	{
		m_ship->m_steer = 0.f;
	}

	if (m_ship->m_thrust != 1.f && g_theInput->IsKeyDown(KEY_UPARROW))
	{
		m_ship->m_thrust = 1.f;
	}
	else
	{
		m_ship->m_thrust = 0.f;
	}

	if (g_theInput->IsKeyDown(' '))
	{
		m_ship->ShootBullet();
	}
	else
	{
		m_ship->m_isFiringLaser = false;
	}

	m_ship->UpdateMovement(Game::s_gameInstance->m_deltaSecondsThisFrame);
}

