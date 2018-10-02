#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

#include <string>
#include <vector>

class ShaderProgram;
class ShaderDatabase;
class Font;
class MainMenu;
class StructuredBuffer;
class ConstantBuffer;
class Texture2D;
class Sampler;
class Camera2D;
class Player;
class Ship;
class Bullet;
class Missile;
class Battleship;
class Powerup;

struct ThrustSmoke;

struct TimeConstant_t
{
	float time;
	Vector3 padding;
};

struct GarbageFileData_t
{
	std::string filepath;
	unsigned int byteSize;
};

struct LogTestData_t
{
	int threadNum;
	int lineCount;
};

struct ColorConstant_t
{
	float firstColor[4];
	float secondColor[4];
};

struct DataConstant_t
{
	unsigned int numIterations;
	float width;
	float height;
	float zoom;
	float panX;
	float panY;
	float buffer1;
	float buffer2;
};

enum eGameState
{
	MAIN_MENU_STATE,
	GAME_STATE,
	QUIT_STATE,
};

class Game
{
public:
	static Game *s_gameInstance;

	float m_fov;
	bool m_isPaused;
	bool m_hasSentSnapshots;
	int m_windowWidth;
	int m_windowHeight;
	bool m_isRenderingMeshBuilder;
	float m_deltaSecondsThisFrame;
	unsigned int m_maxMissilies;
	unsigned int m_maxBattleships;
	Vector2 m_worldSize;
	eGameState m_gameState;

	float lightningRotationDegrees;

	Sampler *m_pointSampler;
	ShaderDatabase *m_shaderDatabase;
	ShaderProgram *m_myShader;
	Texture2D *m_normalTexture;
	Texture2D *m_diffuseTexture;
	Texture2D *m_parallaxBackground[7];
	Texture2D *m_cloudTexture;
	Texture2D *m_pickupShotgunTexture;
	Texture2D *m_pickupLaserTexture;
	Texture2D *m_pickupLightningTexture;
	Texture2D *m_pickupCanonTexture;
	Texture2D *m_playerShipTexture;
	Texture2D *m_friendlyShipTexture;
	Texture2D *m_bulletTexture;
	Texture2D *m_laserTexture;
	Texture2D *m_thrustSmokeTexture;
	Texture2D *m_missileTexture;
	StructuredBuffer *m_structuredBuffer;
	MainMenu *m_mainMenu;
	Player *m_player;
	std::vector<Bullet*> m_bullets;
	std::vector<Missile*> m_missiles;
	std::vector<Battleship*> m_battleships;
	std::vector<Powerup*> m_powerups;
	std::vector<ThrustSmoke*> m_thrustSmokes;

	Game();
	~Game();

	void Initialize();
	void InitDevConsoleFunctions();
	void Update(float deltaSeconds);
	void UpdateGameState(float deltaSeconds);
	void UpdateMainMenu();
	void UpdateHost(float deltaSeconds);
	void UpdateQuit();
	void UpdatePlayers(float deltaSeconds);
	void UpdateMissiles(float deltaSeconds);
	void UpdateBattleships(float deltaSeconds);
	void UpdateBullets(float deltaSeconds);
	void UpdatePowerUps(float deltaSeconds);
	void UpdateThrustSmokes(float deltaSeconds);
	void Render() const;
	void RenderDefaults() const;
	void Render2D() const;
	void RenderWorld() const;
	void RenderStartZoneLayer() const;
	void RenderParallaxBackground(const Vector2 &gamePos) const;
	void RenderParallaxForeground() const;
	void RenderParallaxLayer(const Vector2 &centerPos, const Vector2 &parallaxDistance, float yDistanceOffset, Texture2D *texture) const;
	void RenderPlayerInfo() const;
	void RenderBullets(const Vector2 &selfPos) const;
	void RenderMissiles() const;
	void RenderBattleships() const;
	void RenderPowerUps(const Vector2 &selfPos) const;
	void RenderThrustSmokes(const Vector2 &selfPos) const;
	void RenderGameState() const;
	void RenderShipHUD(const Vector2 &gamePos) const;
	void RenderSimpleRendererMeshBuilder() const;
	void RenderRemoteCommandService() const;
	void RenderGameSessionConnections() const;
	void Present() const;
	void Destroy();

	float ChangeSimulationSpeed(float deltaSeconds) const;
	void KeyDown();
	void KeyUp();

	void CreateAndStoreShaderFromConfig();
	void ChangeToNextShader();
	void ChangeToPrevShader();

	void ChangeState(eGameState newState);
	void AddBullet(Bullet *newBullet, int bulletIndex = -1);
	void AddMissile(Missile *newMissile, int index = -1);
	void AddPowerUp(Powerup *newPowerUp, int index = -1);
	void AddBattleship(Battleship *newBattleship, int index = -1);
	void SpawnMissile();
	void SpawnBattleship();
	unsigned int GetNumMissiles();
	unsigned int GetNumBattleships();
};