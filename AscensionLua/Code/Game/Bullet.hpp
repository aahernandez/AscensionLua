#pragma once
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Math/LineSegment2D.hpp"
#include "Engine/Core/CommonInclude.hpp"

#include <vector>

class Player;
class LuaBinding;

enum eBulletType
{
	BT_NORMAL,
	BT_SHOTGUN,
	BT_LASER,
	BT_CANONBALL,
	BT_LIGHTNING,
	NUM_BULLET_TYPES,
};

class Bullet : public Disc2D
{
public:
	//Vector m_center
	//float m_radius
	LineSegment2D m_laserOrLightningLine;
	float m_laserThickness;
	float m_timeToDie;
	Vector2 m_velocity;
	Player *m_playerRef;
	unsigned int m_bulletIndex;
	bool m_isAttachedToMissile;
	float m_lightningRange;
	eBulletType m_bulletType;
	std::vector<LineSegment2D> m_lightningPoints;


	Bullet();
	Bullet(Player *playerRef, const Vector2& velocity, const Vector2& center);
	~Bullet();

	void Initialize();
	void Update(float deltaSeconds);
	void OnDeath();
	void CreateLightning();

	static LuaBinding *s_binding;

	static std::string GetTypeAsString(eBulletType bulletType);
	static eBulletType GetStringFromType(std::string typeString);
};