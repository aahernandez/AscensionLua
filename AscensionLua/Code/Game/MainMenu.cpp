#include "Game/Game.hpp"
#include "Game/MainMenu.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DeveloperConsole.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"

MainMenu::MainMenu()
	: m_selection(NEW_GAME_SELECTION)
{
	Initialize();
}

MainMenu::~MainMenu()
{
	DestroyMainMenu();
}

void MainMenu::Initialize()
{

}

void MainMenu::Update()
{
	KeyPress();
}

void MainMenu::Render() const
{
	g_theSimpleRenderer->DrawText2DCenteredOnPosition("Ascension", Vector2(640.f, 600.f), RGBA::WHITE, 1.5f);
	g_theSimpleRenderer->DrawText2DCenteredOnPosition("New Game", Vector2(640.f, 400.f), (m_selection == NEW_GAME_SELECTION ? RGBA::TEAL : RGBA::WHITE), 1.f);
	g_theSimpleRenderer->DrawText2DCenteredOnPosition("Quit", Vector2(640.f, 360.f), (m_selection == QUIT_SELECTION ? RGBA::TEAL : RGBA::WHITE), 1.f);
}

void MainMenu::DestroyMainMenu()
{

}

void MainMenu::KeyPress()
{
	if (g_theDevConsole->m_isConsoleActive)
		return;

	if (g_theInput->WasKeyJustPressed(KEY_UPARROW))
	{
		if (m_selection == NEW_GAME_SELECTION)
		{
			m_selection = QUIT_SELECTION;
		}
		else
		{
			int selectionAsInt = (int)m_selection;
			selectionAsInt--;
			m_selection = (eMainMenuSelection)selectionAsInt;
		}
	}

	if (g_theInput->WasKeyJustPressed(KEY_DOWNARROW))
	{
		int selectionAsInt = (int)m_selection;
		selectionAsInt++;
		m_selection = (eMainMenuSelection)selectionAsInt;

		if (m_selection == NUM_SELECTIONS)
		{
			m_selection = NEW_GAME_SELECTION;
		}
	}

	if (g_theInput->WasKeyJustPressed(KEY_ENTER))
	{
		switch (m_selection)
		{
		case NEW_GAME_SELECTION:
			Game::s_gameInstance->ChangeState(GAME_STATE);
			break;
		case QUIT_SELECTION:
			Game::s_gameInstance->ChangeState(QUIT_STATE);
			break;
		}
	}
}

