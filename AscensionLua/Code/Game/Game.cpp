#include "Game/Game.hpp"
#include "Game/Ship.hpp"
#include "Game/Missile.hpp"
#include "Game/Bullet.hpp"
#include "Game/Player.hpp"
#include "Game/Battleship.hpp"
#include "Game/Powerup.hpp"
#include "Game/Camera2D.hpp"
#include "Game/MainMenu.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/Core/RGBA.hpp"
#include "Engine/Core/Config.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DeveloperConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/RHI/RHI.hpp"
#include "Engine/RHI/Sampler.hpp"
#include "Engine/RHI/IndexBuffer.hpp"
#include "Engine/RHI/MeshBuilder.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/RHI/ShaderDatabase.hpp"
#include "Engine/Memory/Memory.hpp"
#include "Engine/Memory/LogSystem.hpp"
#include "Engine/Math/Math2D.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Profiling/Profiler.hpp"

#include "Game/BulletLuaBinding.hpp"
#include "ThirdParty/Lua/Wrappers/LuaBinding.hpp"

Game *Game::s_gameInstance = nullptr;

Game::Game()
	: m_isPaused(false)
	, m_isRenderingMeshBuilder(false)
	, m_windowWidth(1280)
	, m_windowHeight(720)
	, m_fov(60.f)
	, m_diffuseTexture(nullptr)
	, m_pointSampler(nullptr)
	, m_myShader(nullptr)
	, m_shaderDatabase(nullptr)
	, m_gameState(MAIN_MENU_STATE)
	, m_mainMenu(nullptr)
	, m_player(nullptr)
	, m_deltaSecondsThisFrame(0.f)
	, m_maxMissilies(100)
	, m_maxBattleships(5)
{
	s_gameInstance = this;
	m_worldSize = Vector2(10000.f, 5000.f);
	Initialize();
}

Game::~Game()
{
	Destroy();
}

void Game::Initialize()
{
	g_theConfig->ConfigGetInt(&m_windowWidth, "window_res_x");
	g_theConfig->ConfigGetInt(&m_windowHeight, "window_res_y");

	m_shaderDatabase = new ShaderDatabase();
	CreateAndStoreShaderFromConfig();
	m_myShader = m_shaderDatabase->GetUntexturedShaderInMap();

	char const *imageFilepath;
	g_theConfig->ConfigGetString(&imageFilepath, "diffuseTexture");
	m_diffuseTexture = new Texture2D(g_theSimpleRenderer->m_rhiDevice, imageFilepath);
	char const *imageFilepath2;
	g_theConfig->ConfigGetString(&imageFilepath2, "normalTexture");
	m_normalTexture = new Texture2D(g_theSimpleRenderer->m_rhiDevice, imageFilepath2);

	m_parallaxBackground[0] = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/Parallax/Parallax00.png");
	m_parallaxBackground[1] = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/Parallax/Parallax01.png");
	m_parallaxBackground[2] = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/Parallax/Parallax02.png");
	m_parallaxBackground[3] = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/Parallax/Parallax03.png");
	m_parallaxBackground[4] = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/Parallax/Parallax04.png");
	m_parallaxBackground[5] = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/Parallax/Parallax05.png");
	m_parallaxBackground[6] = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/Parallax/Parallax06.png");
	m_cloudTexture = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/Clouds.png");

	m_pickupCanonTexture = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/PickupExplosion.png");
	m_pickupLaserTexture = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/PickupLaser.png");
	m_pickupLightningTexture = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/PickupLightning.png");
	m_pickupShotgunTexture = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/PickupShotgun.png");

	m_playerShipTexture = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/PlayerShip.png");
	m_friendlyShipTexture = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/FriendlyShip.png");
	m_bulletTexture = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/FriendlyFire.png");
	m_laserTexture = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/Laser.png");
	m_thrustSmokeTexture = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/ThrustSmoke.png");
	m_missileTexture = new Texture2D(g_theSimpleRenderer->m_rhiDevice, "Data/Images/Missile.png");

	m_pointSampler = new Sampler(g_theSimpleRenderer->m_rhiDevice, FILTER_POINT, FILTER_POINT);

	g_theConfig->ConfigGetFloat(&m_fov, "fov");

	g_theDevConsole = new DeveloperConsole();
	g_theDevConsole->SetProjectionSize(Vector2(1280.f, 720.f), Vector2( 1280.f, 720.f));
	
	InitDevConsoleFunctions();
	DebuggerPrintf("");

	m_player = new Player();

	m_mainMenu = new MainMenu();

	Bullet::s_binding = new LuaBinding("Data/Scripts/Bullets.lua"); 
	Bullet::s_binding->BindFunctionsToLua("Bullet", bulletLib, 12);
	Bullet::s_binding->BindFunctionsToLua("Laser", laserLib, 2);
	Bullet::s_binding->BindFunctionsToLua("Game", gameLib, 8);
	Bullet::s_binding->BindFunctionsToLua("Lightning", lightningLib, 3);
	Bullet::s_binding->BindFunctionsToLua("Vector", vectorLib, 2);
	Bullet::s_binding->BindFunctionsToLua("Debug", debugLib, 1);
	Bullet::s_binding->BindFunctionsToLua("Missile", missileLib, 5);
	Bullet::s_binding->RunScript();
}

void Game::InitDevConsoleFunctions()
{
}

void Game::Update(float deltaSeconds)
{
	UpdateProfileMemory();
	deltaSeconds = ChangeSimulationSpeed(deltaSeconds);
	m_deltaSecondsThisFrame = deltaSeconds;
	KeyDown();
	KeyUp();

	UpdateGameState(deltaSeconds);

	g_theDevConsole->UpdateCaret(deltaSeconds);
}

void Game::UpdateGameState(float deltaSeconds)
{
	switch (m_gameState)
	{
	case MAIN_MENU_STATE:
		UpdateMainMenu();
		break;
	case GAME_STATE:
		UpdateHost(deltaSeconds);
// 		UpdateClient(deltaSeconds);
		break;
	case QUIT_STATE:
		UpdateQuit();
		break;
	}
}

void Game::UpdateMainMenu()
{
	m_mainMenu->Update();
}

void Game::UpdateHost(float deltaSeconds)
{
	UpdatePlayers(deltaSeconds);
	UpdateBattleships(deltaSeconds);
	UpdateMissiles(deltaSeconds);
	UpdateBullets(deltaSeconds);
	UpdatePowerUps(deltaSeconds);
	UpdateThrustSmokes(deltaSeconds);
}

void Game::UpdateQuit()
{
	g_theApp->OnExitRequested();
}

void Game::UpdatePlayers(float deltaSeconds)
{
	m_player->Update(deltaSeconds);

	Ship *shipRef = m_player->m_ship;
	if (shipRef != nullptr)
	{
		shipRef->Update(deltaSeconds);
		if (shipRef->m_health <= 0)
		{
			delete m_player->m_ship;
			m_player->m_ship = nullptr;
		}
	}
}

void Game::UpdateMissiles(float deltaSeconds)
{
// 	unsigned int numMissiles = GetNumMissiles();
// 	if (numMissiles < m_maxMissilies)
// 	{
// 		SpawnMissile();
// 	}

	std::vector<Missile*>::iterator missileIter;
	for (missileIter = m_missiles.begin(); missileIter != m_missiles.end(); ++missileIter)
	{
		Missile *missileRef = *missileIter;
		if (missileRef == nullptr)
		{
			continue;
		}
		missileRef->Update(deltaSeconds);

		if (missileRef->m_health <= 0.f)
		{
			missileRef->OnDeath();
			int index = missileRef->m_missileIndex;
			delete m_missiles[index];
			m_missiles[index] = nullptr;
			continue;
		}

		Ship *currentShip = m_player->m_ship;
		bool doOverlap = DoDiscsOverlap(*currentShip, *missileRef);
		if (doOverlap)
		{
			currentShip->m_health--;
			int index = missileRef->m_missileIndex;
			delete m_missiles[index];
			m_missiles[index] = nullptr;
			break;
		}
	}
}

void Game::UpdateBattleships(float deltaSeconds)
{
	unsigned int numBattleships = GetNumBattleships();
	if (numBattleships < m_maxBattleships)
	{
		SpawnBattleship();
	}

	std::vector<Battleship*>::iterator battleshipIter;
	for (battleshipIter = m_battleships.begin(); battleshipIter != m_battleships.end(); ++battleshipIter)
	{
		Battleship *battleshipRef = *battleshipIter;
		if (battleshipRef == nullptr)
		{
			continue;
		}
		battleshipRef->Update(deltaSeconds);

		if (battleshipRef->m_health <= 0.f)
		{
			battleshipRef->OnDeath();
			int index = battleshipRef->m_battleshipIndex;
			delete m_battleships[index];
			m_battleships[index] = nullptr;
			continue;
		}
	}
}

void Game::UpdateBullets(float deltaSeconds)
{
	for (int bulletIndex = 0; bulletIndex != (int)m_bullets.size(); bulletIndex++)
	{
		bool isBreakingOut = false;
		Bullet* currentBullet = m_bullets[bulletIndex];
		if (currentBullet == nullptr)
			continue;
		currentBullet->Update(deltaSeconds);
		if (currentBullet->m_timeToDie <= 0.f)
		{
			int index = currentBullet->m_bulletIndex;
			m_bullets[index]->OnDeath();
			delete m_bullets[index];
			m_bullets[index] = nullptr;
			continue;
		}
		if (currentBullet->m_bulletType == BT_LIGHTNING && !currentBullet->m_isAttachedToMissile)
		{
			int index = currentBullet->m_bulletIndex;
			m_bullets[index]->OnDeath();
			delete m_bullets[index];
			m_bullets[index] = nullptr;
			continue;
		}

		Ship *shipRef = m_player->m_ship;
 		if (currentBullet->m_bulletType == BT_LASER && (shipRef == nullptr || !shipRef->m_isFiringLaser))
		{
			int index = currentBullet->m_bulletIndex;
			m_bullets[index]->OnDeath();
			delete m_bullets[index];
			m_bullets[index] = nullptr;
			continue;
		}

		if (currentBullet->m_bulletType != BT_LIGHTNING)
		{
			std::vector<Missile*>::iterator missileIter;
			for (missileIter = m_missiles.begin(); missileIter != m_missiles.end(); ++missileIter)
			{
				Missile *currentMissile = *missileIter;
				if (currentMissile == nullptr)
					continue;
				if (currentBullet->m_bulletType != BT_LASER)
				{
					if (DoDiscsOverlap(*currentMissile, *currentBullet))
					{
						if (currentBullet->m_bulletType == BT_CANONBALL)
						{
							currentMissile->m_health -= 25;
						}
						else
						{
							currentMissile->m_health--;
						}
						if (currentBullet != nullptr)
						{
							int index = currentBullet->m_bulletIndex;
							currentBullet->OnDeath();
							delete currentBullet;
							m_bullets[index] = nullptr;
							isBreakingOut = true;
							break;
						}
					}
				}
				else if (currentBullet->m_bulletType == BT_LASER)
				{
					if (DoesDiscOverlapLineSegment(*currentMissile, currentBullet->m_laserOrLightningLine))
					{
						currentMissile->m_health--;
					}
				}
			}

			if (isBreakingOut)
			{
				continue;
			}

			std::vector<Battleship*>::iterator battleshipIter;
			for (battleshipIter = m_battleships.begin(); battleshipIter != m_battleships.end(); ++battleshipIter)
			{
				Battleship *currentBattleship = *battleshipIter;
				if (currentBattleship == nullptr)
					continue;
				if (currentBullet->m_bulletType != BT_LASER)
				{
					if (DoDiscsOverlap(*currentBattleship, *currentBullet))
					{
						if (currentBullet->m_bulletType == BT_CANONBALL)
						{
							currentBattleship->m_health -= 25;
						}
						else
						{
							currentBattleship->m_health--;
						}
						int index = currentBullet->m_bulletIndex;
						m_bullets[index]->OnDeath();
						delete m_bullets[index];
						m_bullets[index] = nullptr;
						isBreakingOut = true;
						break;
					}
				}
				else if (currentBullet->m_bulletType == BT_LASER)
				{
					if (DoesDiscOverlapLineSegment(*currentBattleship, currentBullet->m_laserOrLightningLine))
					{
						currentBattleship->m_health--;
					}
				}
			}

			if (isBreakingOut)
			{
				continue;
			}

		}
	}
}

void Game::UpdatePowerUps(float deltaSeconds)
{
	std::vector<Powerup*>::iterator powerUpIter;
	for (powerUpIter = m_powerups.begin(); powerUpIter != m_powerups.end(); ++powerUpIter)
	{
		Powerup *currentPowerup = *powerUpIter;
		if (currentPowerup == nullptr)
			continue;

		currentPowerup->Update(deltaSeconds);
		if (currentPowerup->m_powerupType == PU_NONE)
		{
			int index = currentPowerup->m_powerUpIndex;
			delete m_powerups[index];
			m_powerups[index] = nullptr;
			continue;
		}

		Ship *currentShip = m_player->m_ship;
		if (currentShip == nullptr)
		{
			continue;
		}

		if (DoDiscsOverlap(*currentShip, *currentPowerup))
		{
			currentShip->m_currentPowerup->SetPowerupType(*currentPowerup);
			currentShip->m_wasPowerupPickedup = true;
			if (currentShip->m_currentPowerup->m_powerupType != PU_LASER)
			{
				currentShip->m_isFiringLaser = false;
			}
			int index = currentPowerup->m_powerUpIndex;
			delete m_powerups[index];
			m_powerups[index] = nullptr;
		}
	}
}

void Game::UpdateThrustSmokes(float deltaSeconds)
{
	std::vector<ThrustSmoke*>::iterator smokeIter;
	for (smokeIter = m_thrustSmokes.begin(); smokeIter != m_thrustSmokes.end(); ++smokeIter)
	{
		ThrustSmoke *currentSmoke = *smokeIter;
		currentSmoke->m_lifetime -= deltaSeconds;
		if (currentSmoke->m_lifetime <= 0.f)
		{
			delete currentSmoke;
			smokeIter = m_thrustSmokes.erase(smokeIter);
			if (smokeIter == m_thrustSmokes.end())
			{
				break;
			}
		}
	}
}

void Game::Render() const
{
	PROFILE_SCOPE_FUNCTION;
	RenderDefaults();
	Render2D();
	RenderSimpleRendererMeshBuilder();
	Present();
	g_theSimpleRenderer->m_meshBuilder->Clear();
}

void Game::RenderDefaults() const
{
	PROFILE_SCOPE_FUNCTION;
	g_theSimpleRenderer->SetRenderTarget(nullptr);
	g_theSimpleRenderer->ClearColor(RGBA::BLACK);
	g_theSimpleRenderer->ClearDepth();
	g_theSimpleRenderer->SetViewport(0, 0, (unsigned int)m_windowWidth, (unsigned int)m_windowHeight);
	g_theSimpleRenderer->SetShader(g_theSimpleRenderer->m_unlitShader);
}

void Game::Render2D() const
{
	PROFILE_SCOPE_FUNCTION;
	g_theSimpleRenderer->SetShader(g_theSimpleRenderer->m_lightShader);
	g_theSimpleRenderer->EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);

	g_theSimpleRenderer->SetViewMatrixTranslationAndRotation(Vector3(), 0.f, 0.f, 0.f);
	g_theSimpleRenderer->ClearDepth();
	g_theSimpleRenderer->EnableDepthTest(false, false);
	g_theSimpleRenderer->SetOrthoProjection(Vector2(0.f, 0.f), Vector2(1280.f, 720.f));

	switch (m_gameState)
	{
	case MAIN_MENU_STATE:
		m_mainMenu->Render();
		break;
	case GAME_STATE:
		RenderGameState();
		break;
	}

	g_theSimpleRenderer->SetOrthoProjection(Vector2(0.f, 0.f), Vector2(1280.f, 720.f));
	if (g_theDevConsole->m_isConsoleActive)
	{
		g_theDevConsole->Render();
		g_theDevConsole->RenderMemoryProfiler();
		RenderRemoteCommandService();
	}
}

void Game::RenderWorld() const
{
	Ship *shipRef = m_player->m_ship;
	Vector2 shipPos = shipRef->m_center;

	RenderParallaxBackground(shipPos);
	RenderBullets(shipPos);

	Vector2 playerPos;
	Player *currentPlayer = m_player;
	Ship *currentPlayerShip = nullptr;
	if (currentPlayer != nullptr)
		currentPlayerShip = currentPlayer->m_ship;
		
	if (currentPlayer != m_player && currentPlayer != nullptr && currentPlayerShip != nullptr)
	{
		playerPos.x = currentPlayerShip->m_center.x - shipPos.x + 640.f;
		playerPos.y = currentPlayerShip->m_center.y - shipPos.y + 360.f;
		AABB2D centerQuad;
		centerQuad.SetCenterAndRadii(playerPos, currentPlayerShip->m_radius, currentPlayerShip->m_radius);
		g_theSimpleRenderer->DrawQuad2DTextured(centerQuad, AABB2D::ZERO_TO_ONE, m_friendlyShipTexture, currentPlayerShip->m_orientation);
	}

	RenderThrustSmokes(shipPos);

	Vector2 center(m_windowWidth * 0.5f, m_windowHeight * 0.5f);
	AABB2D centerQuad;
	centerQuad.SetCenterAndRadii(center, shipRef->m_radius, shipRef->m_radius);
	g_theSimpleRenderer->DrawQuad2DTextured(centerQuad, AABB2D::ZERO_TO_ONE, m_playerShipTexture, shipRef->m_orientation);

	RenderPowerUps(shipPos);
	RenderMissiles();
	RenderBattleships();
	RenderParallaxForeground();

	if (m_gameState == GAME_STATE)
	{
		RenderShipHUD(shipPos);
	}
}

void Game::RenderStartZoneLayer() const
{
	Vector2 shipPos = m_player->m_ship->m_center;
	Vector2 renderPos;
	renderPos.x = 1500.f - shipPos.x + 3000.f;
	renderPos.y = shipPos.y - 1000.f;
	AABB2D renderBox;
	renderBox.SetCenterAndRadii(renderPos, 1920.f, 1080.f);
	g_theSimpleRenderer->DrawQuad2DTextured(renderBox, AABB2D::ZERO_TO_ONE, m_parallaxBackground[0], 0.f, RGBA::TEAL);
}

void Game::RenderParallaxBackground(const Vector2 &shipPos) const
{
	g_theSimpleRenderer->DrawQuad2DTextured(AABB2D(Vector2(0.f, 0.f), Vector2(1280.f, 720.f)), AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f)), m_parallaxBackground[6]);
	g_theSimpleRenderer->DrawQuad2DTextured(AABB2D(Vector2(0.f, 0.f), Vector2(1280.f, 720.f)), AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f)), m_parallaxBackground[5]);

	Vector2 centerTexturePos = RangeMap2D(shipPos, Vector2(m_worldSize.x, 0.f), Vector2(0.f, m_worldSize.y), Vector2(0.f, 1920.f), Vector2(0.f, 1080.f));
	RenderParallaxLayer(centerTexturePos, Vector2(0.3f, -0.6f),	1400.f, m_parallaxBackground[4]);
	RenderParallaxLayer(centerTexturePos, Vector2(0.5f,	-1.2f), 675.f, m_parallaxBackground[3]);
	RenderParallaxLayer(centerTexturePos, Vector2(0.9f, -2.0f), 375.f, m_parallaxBackground[2]);
}

void Game::RenderParallaxForeground() const
{
	Vector2 shipPos = m_player->m_ship->m_center;
	Vector2 parallaxPos;
	parallaxPos.x = 2500.f - shipPos.x + 640.f;
	parallaxPos.y = 800.f - shipPos.y + 360.f;
	AABB2D parallaxBox;
	parallaxBox.SetCenterAndRadii(parallaxPos, 1920.f, 1080.f);
	g_theSimpleRenderer->DrawQuad2DTextured(parallaxBox, AABB2D::ZERO_TO_ONE, m_parallaxBackground[1]);
	parallaxPos.x -= 1920.f;
	parallaxBox.SetCenterAndRadii(parallaxPos, 1920.f, 1080.f);
	g_theSimpleRenderer->DrawQuad2DTextured(parallaxBox, AABB2D::ZERO_TO_ONE, m_parallaxBackground[1]);
	parallaxPos.x += 1920.f;
	parallaxPos.x += 1920.f;
	parallaxBox.SetCenterAndRadii(parallaxPos, 1920.f, 1080.f);
	g_theSimpleRenderer->DrawQuad2DTextured(parallaxBox, AABB2D::ZERO_TO_ONE, m_parallaxBackground[1]);
	parallaxPos.x += 1920.f;
	parallaxBox.SetCenterAndRadii(parallaxPos, 1920.f, 1080.f);
	g_theSimpleRenderer->DrawQuad2DTextured(parallaxBox, AABB2D::ZERO_TO_ONE, m_parallaxBackground[1]);
	parallaxPos.x += 1920.f;
	parallaxBox.SetCenterAndRadii(parallaxPos, 1920.f, 1080.f);
	g_theSimpleRenderer->DrawQuad2DTextured(parallaxBox, AABB2D::ZERO_TO_ONE, m_parallaxBackground[1]);

	Vector2 centerTexturePos = RangeMap2D(shipPos, Vector2(m_worldSize.x, 0.f), Vector2(0.f, m_worldSize.y), Vector2(0.f, 1920.f), Vector2(0.f, 1080.f));
	RenderParallaxLayer(centerTexturePos, Vector2(1.f, -2.f), -150.f, m_parallaxBackground[0]);
}

void Game::RenderParallaxLayer(const Vector2 &centerPos, const Vector2 &parallaxDistance, float yDistanceOffset, Texture2D *texture) const
{
	Vector2 halfScreen = Vector2(860.f, 540.f);
	Vector2 bottomLeft = (centerPos * parallaxDistance) - halfScreen;
	Vector2 topRight = (centerPos * parallaxDistance) + halfScreen;
	AABB2D position(bottomLeft, topRight);
	float yCenterPos = (centerPos.y - yDistanceOffset) * parallaxDistance.y;
	position.SetYCenter(yCenterPos);
	g_theSimpleRenderer->DrawQuad2DTextured(position, AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f)), texture);

	if (bottomLeft.x > 0.f)
	{
		Vector2 nextTopRight(bottomLeft.x, topRight.y);
		Vector2 nextBottomLeft(bottomLeft.x - 1920.f, bottomLeft.y);
		AABB2D nextPos(nextBottomLeft, nextTopRight);
		nextPos.SetYCenter(yCenterPos);
		g_theSimpleRenderer->DrawQuad2DTextured(nextPos, AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f)), texture);

		Vector2 nextTopRight2(nextBottomLeft.x, nextTopRight.y);
		Vector2 nextBottomLeft2(nextBottomLeft.x - 1920.f, nextBottomLeft.y);
		AABB2D nextPos2(nextBottomLeft2, nextTopRight2);
		nextPos2.SetYCenter(yCenterPos);
		g_theSimpleRenderer->DrawQuad2DTextured(nextPos2, AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f)), texture);
	}
	if (topRight.x < 1280.f)
	{
		Vector2 nextBottomLeft(topRight.x, bottomLeft.y);
		Vector2 nextTopRight(topRight.x + 1920.f, topRight.y);
		AABB2D nextPos(nextBottomLeft, nextTopRight);
		nextPos.SetYCenter(yCenterPos);
		g_theSimpleRenderer->DrawQuad2DTextured(nextPos, AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f)), texture);

		Vector2 nextBottomLeft2(nextTopRight.x, nextBottomLeft.y);
		Vector2 nextTopRight2(nextTopRight.x + 1920.f, nextTopRight.y);
		AABB2D nextPos2(nextBottomLeft2, nextTopRight2);
		nextPos2.SetYCenter(yCenterPos);
		g_theSimpleRenderer->DrawQuad2DTextured(nextPos2, AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f)), texture);

		Vector2 nextBottomLeft3(nextTopRight2.x, nextBottomLeft2.y);
		Vector2 nextTopRight3(nextTopRight2.x + 1920.f, nextTopRight2.y);
		AABB2D nextPos3(nextBottomLeft3, nextTopRight3);
		nextPos3.SetYCenter(yCenterPos);
		g_theSimpleRenderer->DrawQuad2DTextured(nextPos3, AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f)), texture);
	}
}

void Game::RenderPlayerInfo() const
{
// 	std::string playerPos = Stringf("%.2f, %.2f", m_player->m_ship->m_center.x, m_player->m_ship->m_center.y);
// 	g_theSimpleRenderer->DrawText2D(playerPos.c_str(), Vector2(500.f, 500.f));
}

void Game::RenderBullets(const Vector2 &shipPos) const
{
	for (unsigned int bulletIndex = 0; bulletIndex < (unsigned int)m_bullets.size(); bulletIndex++)
	{
		Bullet *currentBullet = m_bullets[bulletIndex];
		if (currentBullet == nullptr)
			continue;
		Vector2 center = currentBullet->m_center - shipPos + Vector2(640.f, 360.f);
		if (currentBullet->m_bulletType == BT_LASER)
		{
			LineSegment2D lineSegmentCenter;
			lineSegmentCenter.lineStart = currentBullet->m_laserOrLightningLine.lineStart - shipPos + Vector2(640.f, 360.f);
			lineSegmentCenter.lineEnd = currentBullet->m_laserOrLightningLine.lineEnd - shipPos + Vector2(640.f, 360.f);
			Vector2 newCenter = lineSegmentCenter.CalcCenter();
			float length = lineSegmentCenter.CalcLength();
			AABB2D centerQuad;
			centerQuad.SetCenterAndRadii(newCenter, 5.f, length - 200.f);
			Ship *owningShip = m_player->m_ship;
			if (owningShip != nullptr)
				g_theSimpleRenderer->DrawQuad2DTextured(centerQuad, AABB2D::ZERO_TO_ONE, m_laserTexture, owningShip->m_orientation);
		}
		else if (currentBullet->m_bulletType == BT_LIGHTNING)
		{
			for (int lineIndex = 0; lineIndex < (int)currentBullet->m_lightningPoints.size(); lineIndex++)
			{
				LineSegment2D lightningLineSection;
				lightningLineSection.lineStart = currentBullet->m_lightningPoints[lineIndex].lineStart - shipPos + Vector2(640.f, 360.f);
				lightningLineSection.lineEnd = currentBullet->m_lightningPoints[lineIndex].lineEnd - shipPos + Vector2(640.f, 360.f);
				Vector2 newCenter = lightningLineSection.CalcCenter();
				float length = lightningLineSection.CalcLength();
				AABB2D centerQuad;
				centerQuad.SetCenterAndRadii(newCenter, 1.f, length / 2.f);
				g_theSimpleRenderer->DrawQuad2DTextured(centerQuad, AABB2D::ZERO_TO_ONE, g_theSimpleRenderer->m_whiteTexture, lightningLineSection.GetRotationDegreesFromStartToEnd(), RGBA::BLUE_LIGHT);
			}
		}
		else
		{
			AABB2D centerQuad;
			centerQuad.SetCenterAndRadii(center, currentBullet->m_radius, currentBullet->m_radius);
			float orientation = (float)atan2(currentBullet->m_velocity.x, currentBullet->m_velocity.y);
			orientation = orientation * (180.0f / fPI);
			g_theSimpleRenderer->DrawQuad2DTextured(centerQuad, AABB2D::ZERO_TO_ONE, m_bulletTexture, orientation);
		}
	}

	g_theSimpleRenderer->DrawText2D(Stringf("%.2f", lightningRotationDegrees).c_str(), Vector2(50.f, 50.f));
}

void Game::RenderMissiles() const
{
	Ship *shipRef = m_player->m_ship;
	Vector2 gamePos = shipRef->m_center;

	Vector2 playerPos;
	for (int index = 0; index < (int)m_missiles.size(); index++)
	{
		Missile *currentMissile = m_missiles[index];
		if (currentMissile == nullptr)
			continue;
		playerPos.x = currentMissile->m_center.x - gamePos.x + 640.f;
		playerPos.y = currentMissile->m_center.y - gamePos.y + 360.f;
		AABB2D centerQuad;
		centerQuad.SetCenterAndRadii(playerPos, currentMissile->m_radius * (1.f/2.68f), currentMissile->m_radius);
		g_theSimpleRenderer->DrawQuad2DTextured(centerQuad, AABB2D::ZERO_TO_ONE, m_missileTexture, currentMissile->m_orientation);
	}
}

void Game::RenderBattleships() const
{
	Ship *shipRef = m_player->m_ship;
	Vector2 gamePos = shipRef->m_center;

	Vector2 battleshipPos;
	for (int index = 0; index < (int)m_battleships.size(); index++)
	{
		Battleship *currentBattleship = m_battleships[index];
		if (currentBattleship == nullptr)
			continue;
		battleshipPos.x = currentBattleship->m_center.x - gamePos.x + 640.f;
		battleshipPos.y = currentBattleship->m_center.y - gamePos.y + 360.f;
		AABB2D battleshipBox;
		battleshipBox.SetCenterAndRadii(battleshipPos, currentBattleship->m_radius, currentBattleship->m_radius);
		g_theSimpleRenderer->DrawQuad2DTextured(battleshipBox, AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f)), g_theSimpleRenderer->m_whiteTexture, 0.f, RGBA::TEAL);
	}
}

void Game::RenderPowerUps(const Vector2 &selfPos) const
{
	for (unsigned int powerUpIndex = 0; powerUpIndex < (unsigned int)m_powerups.size(); powerUpIndex++)
	{
		Powerup *currentPowerup = m_powerups[powerUpIndex];
		if (currentPowerup == nullptr)
			continue;
		Texture2D *pickupTexture = m_pickupShotgunTexture;
		Vector2 center = currentPowerup->m_center - selfPos + Vector2(640.f, 360.f);
		if (currentPowerup->m_powerupType == PU_CANONBALL)
		{
			pickupTexture = m_pickupCanonTexture;
		}
		else if (currentPowerup->m_powerupType == PU_LASER)
		{
			pickupTexture = m_pickupLaserTexture;
		}
		else if (currentPowerup->m_powerupType == PU_LIGHTNING)
		{
			pickupTexture = m_pickupLightningTexture;
		}
		AABB2D drawQuad;
		drawQuad.SetCenterAndRadii(center, currentPowerup->m_radius, currentPowerup->m_radius);
		g_theSimpleRenderer->DrawQuad2DTextured(drawQuad, AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f)), pickupTexture);
	}
}

void Game::RenderThrustSmokes(const Vector2 &selfPos) const
{
	for (int smokeCount = 0; smokeCount < (int)m_thrustSmokes.size(); smokeCount++)
	{
		ThrustSmoke *currentSmoke = m_thrustSmokes[smokeCount];

		float lifePercentage = currentSmoke->m_lifetime / 2.f;
		RGBA smokeColor = RGBA::WHITE;
		smokeColor.a = ( (unsigned char)(255 * lifePercentage) );

		Vector2 center = currentSmoke->m_position - selfPos + Vector2(640.f, 360.f);
		AABB2D centerQuad;
		centerQuad.SetCenterAndRadii(center, 15.f * lifePercentage, 25.f * lifePercentage);
		g_theSimpleRenderer->DrawQuad2DTextured(centerQuad, AABB2D::ZERO_TO_ONE, m_thrustSmokeTexture, currentSmoke->m_orientation, smokeColor);
	}
}

void Game::RenderGameState() const
{
	RenderWorld();
	RenderPlayerInfo();
}

void Game::RenderShipHUD(const Vector2 &gamePos) const
{
	Vector2 playerPos;
	playerPos.x = m_player->m_ship->m_center.x - gamePos.x + 640.f;
	playerPos.y = m_player->m_ship->m_center.y - gamePos.y + 360.f;
	g_theSimpleRenderer->DrawCircleHollow2D(playerPos, m_player->m_ship->m_health * 50.f, 10.f, 50, RGBA::WHITE_TRANSPARENT);

	AABB2D powerupTimerBar;
	powerupTimerBar.SetCenterAndRadii(Vector2(640.f, 160.f), m_player->m_ship->m_currentPowerup->m_shipLifetime * 5.f, 10.f);

	g_theSimpleRenderer->DrawQuad2DTextured(powerupTimerBar, AABB2D::ZERO_TO_ONE, g_theSimpleRenderer->m_whiteTexture, 0.f, RGBA::YELLOW);
}

void Game::RenderSimpleRendererMeshBuilder() const
{
	g_theSimpleRenderer->DrawMeshBuilder();
}

void Game::RenderRemoteCommandService() const
{
// 	float xPosToDraw = 760.f;
// 	std::string sessionState = m_RCS->m_session->GetSessionStateAsString();
// 	g_theSimpleRenderer->DrawText2D(Stringf("Remote Command Service [%s]", sessionState.c_str()).c_str(), Vector2(xPosToDraw, 700.f));
// 	if (m_RCS->m_session->IsHost())
// 		g_theSimpleRenderer->DrawText2D(Vector2(xPosToDraw, 670.f), 1.f, RGBA::WHITE, g_theSimpleRenderer->m_font, Stringf("[host] join address: %s", NetAddressToString(m_RCS->m_session->m_hostConnection->m_address).c_str()).c_str());
// 	else if (m_RCS->m_session->m_hostConnection != nullptr)
// 		g_theSimpleRenderer->DrawText2D(Vector2(xPosToDraw, 670.f), 1.f, RGBA::WHITE, g_theSimpleRenderer->m_font, Stringf("[client] join address: %s", NetAddressToString(m_RCS->m_session->m_hostConnection->m_address).c_str()).c_str());
// 
// 	g_theSimpleRenderer->DrawText2D(Vector2(xPosToDraw, 640.f), 1.f, RGBA::WHITE, g_theSimpleRenderer->m_font, Stringf("client(s): %i/16", m_RCS->m_session->GetNumberOfValidConnections()).c_str());
	
// 	xPosToDraw += 40;
// 	float yPosToDraw = 610.f;
// 	for (int index = 0; index < (int)m_RCS->m_session->m_connections.size(); index++)
// 	{
// 		NetConnection *connection = m_RCS->m_session->m_connections[index];
// 		if (connection != nullptr)
// 		{
// 			if (connection == m_RCS->m_session->m_hostConnection && connection == m_RCS->m_session->m_myConnection)
// 			{
// 				g_theSimpleRenderer->DrawText2D(Vector2(xPosToDraw, yPosToDraw), 1.f, RGBA::GREEN, g_theSimpleRenderer->m_font, Stringf(" * [%i] %s", index, NetAddressToString(m_RCS->m_session->m_connections[index]->m_address).c_str()).c_str());
// 			}
// 			else if (connection == m_RCS->m_session->m_hostConnection)
// 			{
// 				g_theSimpleRenderer->DrawText2D(Vector2(xPosToDraw, yPosToDraw), 1.f, RGBA::WHITE, g_theSimpleRenderer->m_font, Stringf(" * [%i] %s", index, NetAddressToString(m_RCS->m_session->m_connections[index]->m_address).c_str()).c_str());
// 			}
// 			else if (connection == m_RCS->m_session->m_myConnection)
// 			{
// 				g_theSimpleRenderer->DrawText2D(Vector2(xPosToDraw, yPosToDraw), 1.f, RGBA::GREEN, g_theSimpleRenderer->m_font, Stringf(" - [%i] ANY", index).c_str());
// 			}
// 			else
// 			{
// 				g_theSimpleRenderer->DrawText2D(Vector2(xPosToDraw, yPosToDraw), 1.f, RGBA::WHITE, g_theSimpleRenderer->m_font, Stringf(" - [%i] %s", index, NetAddressToString(m_RCS->m_session->m_connections[index]->m_address).c_str()).c_str());
// 			}
// 				yPosToDraw -= 30.f;
// 		}
// 	}
}

void Game::RenderGameSessionConnections() const
{

}

void Game::Present() const
{
	PROFILE_SCOPE_FUNCTION;
	g_theSimpleRenderer->Present();
}

void Game::Destroy()
{
	delete m_mainMenu;
	m_mainMenu = nullptr;

	delete g_theDevConsole;
	g_theDevConsole = nullptr;

	if (m_pointSampler != nullptr)
		delete m_pointSampler;
	m_pointSampler = nullptr;

	delete m_diffuseTexture;
	m_diffuseTexture = nullptr;

	delete m_normalTexture;
	m_normalTexture = nullptr;

	m_myShader = nullptr;

	delete m_shaderDatabase;
	m_shaderDatabase = nullptr;
}

float Game::ChangeSimulationSpeed(float deltaSeconds) const
{
	if (m_isPaused)
		deltaSeconds = 0.f;

	return deltaSeconds;
}

void Game::KeyDown()
{
	if (g_theDevConsole->m_isConsoleActive)
	{
		int textChar = g_theInput->GetLastKeyCode();
		g_theDevConsole->AddCharacterToCurrentLine(textChar, g_theInput->IsKeyDown(KEY_SHIFT) ? false : true);
	}
	else
	{
	}
}

void Game::KeyUp()
{
	if (g_theInput->WasKeyJustPressed(KEY_TILDE))
	{
		g_theDevConsole->ToggleConsole();
	}

	if (g_theDevConsole->m_isConsoleActive)
	{
		if (g_theInput->WasKeyJustPressed(KEY_BACKSPACE))
		{
			g_theDevConsole->RemoveLastCharacterInCurrentLine();
		}

		if (g_theInput->WasKeyJustPressed(KEY_ENTER))
		{
			g_theDevConsole->RunCommandFromCurrentLine();
		}

		if (g_theInput->IsKeyDown(KEY_CTRL))
		{
			if (g_theInput->WasKeyJustPressed('V'))
			{
				g_theDevConsole->PasteFromWindowsClipboard();
			}
		}
	}
	else
	{
		if (g_theInput->WasKeyJustPressed('0'))
		{
			g_theDevConsole->RunCommand("Launch instance");
		}
	}
}

void Game::CreateAndStoreShaderFromConfig()
{
	int varNum = 1;
	std::string shaderString = "shader";
	std::string varName = shaderString;
	varName += std::to_string(varNum);
	while (g_theConfig->IsConfigSet(varName.c_str()))
	{
		char const *shaderFileName;
		g_theConfig->ConfigGetString(&shaderFileName, varName.c_str());
		m_shaderDatabase->CreateShaderFromFile(g_theSimpleRenderer->m_rhiDevice, shaderFileName);
		
		varName = shaderString;
		varNum++;
		varName += std::to_string(varNum);
	}
}

void Game::ChangeToNextShader()
{
	m_myShader = m_shaderDatabase->GetNextShaderInMap();
}

void Game::ChangeToPrevShader()
{
	m_myShader = m_shaderDatabase->GetPreviousShaderInMap();
}

void Game::ChangeState(eGameState newState)
{
	m_gameState = newState;

	switch (m_gameState)
	{
	case GAME_STATE:
		break;
	case QUIT_STATE:
		UpdateQuit();
		break;
	}
}

void Game::AddBullet(Bullet *newBullet, int bulletIndex /*= -1*/)
{
	if (bulletIndex == -1)
	{
		for (int index = 0; index < (int)m_bullets.size(); index++)
		{
			if (m_bullets[index] == nullptr)
			{
				m_bullets[index] = newBullet;
				newBullet->m_bulletIndex = index;
				return;
			}
		}
		m_bullets.push_back(newBullet);
		newBullet->m_bulletIndex = (int)m_bullets.size() - 1;
	}
	else
	{
		while (bulletIndex >= (int)m_bullets.size())
		{
			m_bullets.push_back(nullptr);
		}
		m_bullets[bulletIndex] = newBullet;
		newBullet->m_bulletIndex = bulletIndex;
	}
}

void Game::AddMissile(Missile *newMissile, int missileIndex)
{
	if (missileIndex == -1)
	{
		for (int index = 0; index < (int)m_missiles.size(); index++)
		{
			if (m_missiles[index] == nullptr)
			{
				m_missiles[index] = newMissile;
				newMissile->m_missileIndex = index;
				return;
			}
		}
		m_missiles.push_back(newMissile);
		newMissile->m_missileIndex = (unsigned int)m_missiles.size() - 1;
	}
	else
	{
		while (missileIndex >= (int)m_missiles.size())
		{
			m_missiles.push_back(nullptr);
		}
		m_missiles[missileIndex] = newMissile;
		newMissile->m_missileIndex = missileIndex;
	}
}

void Game::AddPowerUp(Powerup *newPowerUp, int powerUpIndex /*= -1*/)
{
	if (powerUpIndex == -1)
	{
		for (int index = 0; index < (int)m_powerups.size(); index++)
		{
			if (m_powerups[index] == nullptr)
			{
				m_powerups[index] = newPowerUp;
				newPowerUp->m_powerUpIndex = index;
				return;
			}
		}
		m_powerups.push_back(newPowerUp);
		newPowerUp->m_powerUpIndex = (unsigned int)m_powerups.size() - 1;
	}
	else
	{
		while (powerUpIndex >= (int)m_powerups.size())
		{
			m_powerups.push_back(nullptr);
		}
		m_powerups[powerUpIndex] = newPowerUp;
		newPowerUp->m_powerUpIndex = powerUpIndex;
	}
}

void Game::AddBattleship(Battleship *newBattleship, int battleshipIndex /*= -1*/)
{
	if (battleshipIndex == -1)
	{
		for (int index = 0; index < (int)m_battleships.size(); index++)
		{
			if (m_battleships[index] == nullptr)
			{
				m_battleships[index] = newBattleship;
				newBattleship->m_battleshipIndex = index;
				return;
			}
		}
		m_battleships.push_back(newBattleship);
		newBattleship->m_battleshipIndex = (unsigned int)m_battleships.size() - 1;
	}
	else
	{
		while (battleshipIndex >= (int)m_battleships.size())
		{
			m_battleships.push_back(nullptr);
		}
		m_battleships[battleshipIndex] = newBattleship;
		newBattleship->m_battleshipIndex = battleshipIndex;
	}
}

void Game::SpawnMissile()
{
	Vector2 spawnPos(0.f, 0.f);
	Ship *shipRef = m_player->m_ship;
	if (shipRef == nullptr)
		return;
	
	bool isSpawningLeft = GetRandomBool();
	if (isSpawningLeft)
		spawnPos.x = shipRef->m_center.y - 680.f;
	else
		spawnPos.x = shipRef->m_center.y + 680.f;
	spawnPos.y = GetRandomFloatInRange(shipRef->m_center.x - 360.f, shipRef->m_center.x + 360.f);

	Missile *newMissile = new Missile(spawnPos);
	newMissile->m_targetRef = shipRef;
	AddMissile(newMissile);
}

void Game::SpawnBattleship()
{
	Vector2 spawnPos(2500.f, 350.f);
	spawnPos.x = GetRandomFloatInRange(0.f, m_worldSize.x);

	Battleship *newBattleship = new Battleship();
	newBattleship->m_center = spawnPos;
	newBattleship->m_isFacingRight = spawnPos.x < m_worldSize.x * 0.5f ? true : false;
	AddBattleship(newBattleship);
}

unsigned int Game::GetNumMissiles()
{
	unsigned int numMissiles = 0;
	std::vector<Missile*>::iterator missileIter;
	for (missileIter = m_missiles.begin(); missileIter != m_missiles.end(); ++missileIter)
	{
		Missile *missileRef = *missileIter;
		if (missileRef != nullptr)
			numMissiles++;
	}
	return numMissiles;
}

unsigned int Game::GetNumBattleships()
{
	unsigned int numBattleships = 0;
	std::vector<Battleship*>::iterator battleshipIter;
	for (battleshipIter = m_battleships.begin(); battleshipIter != m_battleships.end(); ++battleshipIter)
	{
		Battleship *battleshipRef = *battleshipIter;
		if (battleshipRef != nullptr)
			numBattleships++;
	}
	return numBattleships;
}
