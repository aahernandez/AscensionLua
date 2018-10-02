#pragma once

enum eMainMenuSelection
{
	NEW_GAME_SELECTION,
	QUIT_SELECTION,
	NUM_SELECTIONS,
};

class MainMenu
{
public:
	eMainMenuSelection m_selection;

	MainMenu();
	~MainMenu();

	void Initialize();
	void Update();
	void Render() const;
	void DestroyMainMenu();
	
	void KeyPress();
};