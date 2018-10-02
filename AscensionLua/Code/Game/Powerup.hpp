#pragma once
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Core/CommonInclude.hpp"

enum ePowerupType
{
	PU_NONE,
	PU_SHOTGUN,
	PU_LASER,
	PU_CANONBALL,
	PU_LIGHTNING,
	NUM_POWERUPS,
};

class Powerup : public Disc2D
{
public:
	ePowerupType m_powerupType;
	float m_worldLifetime;
	float m_shipLifetime;
	float m_reloadTime;
	float m_effectLifetime;
	float m_shipRotationSpeed;
	unsigned int m_powerUpIndex;
	uint16_t m_netID;

	Powerup();
	Powerup(ePowerupType type);
	~Powerup();

	void Initialize();
	void Update(float deltaSeconds);
	void OnShipUpdate(float deltaSeconds);
	void SetPowerupType(const Powerup &powerup);
	void Render() const;
};