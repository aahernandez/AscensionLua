#include "Game/Powerup.hpp"
#include "Game/Game.hpp"

Powerup::Powerup()
	: m_powerupType(PU_LIGHTNING)
	, m_worldLifetime(15.f)
	, m_reloadTime(0.2f)
	, m_effectLifetime(5.f)
	, m_shipLifetime(25.f)
	, m_shipRotationSpeed(180.f)
{
	m_radius = 20.f;
	Initialize();
}

Powerup::Powerup(ePowerupType type)
	: m_powerupType(type)
	, m_worldLifetime(15.f)
	, m_reloadTime(0.2f)
	, m_effectLifetime(5.f)
	, m_shipLifetime(25.f)
	, m_shipRotationSpeed(180.f)
{
	m_radius = 20.f;
	Initialize();
}

Powerup::~Powerup()
{

}

void Powerup::Initialize()
{
	switch (m_powerupType)
	{
	case PU_NONE:
		m_reloadTime = 0.2f;
		m_effectLifetime = 5.f;
		break;
	case PU_SHOTGUN:
		m_reloadTime = 0.8f;
		m_effectLifetime = 5.f;
		break;
	case PU_CANONBALL:
		m_reloadTime = 1.5f;
		m_effectLifetime = 6.f;
		break;
	case PU_LASER:
		m_reloadTime = 0.2f;
		m_effectLifetime = 1.f;
		m_shipRotationSpeed = 45.f;
		break;
	case PU_LIGHTNING:
		m_reloadTime = 1.f;
		m_effectLifetime = 0.2f;
		break;
	}
}

void Powerup::Update(float deltaSeconds)
{
	m_worldLifetime -= deltaSeconds;
	if (m_worldLifetime <= 0.f)
	{
		m_powerupType = PU_NONE;
	}
}

void Powerup::OnShipUpdate(float deltaSeconds)
{
	m_shipLifetime -= deltaSeconds;
	if (m_shipLifetime <= 0.f)
	{
		m_powerupType = PU_NONE;
		Initialize();
	}
}

void Powerup::SetPowerupType(const Powerup &powerup)
{
	m_powerupType = powerup.m_powerupType;
	m_worldLifetime = powerup.m_worldLifetime;
	m_reloadTime = powerup.m_reloadTime;
	m_effectLifetime = powerup.m_effectLifetime;
	m_shipLifetime = powerup.m_shipLifetime;
}

void Powerup::Render() const
{

}
