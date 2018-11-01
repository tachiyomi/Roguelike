#include <Siv3D.hpp>
#include "function.h"
#include "LogSystem.h"
#include "MenuSystem.h"
#include "MapData.h"
#include "Character.h"
#include "DungeonSystem.h"

void Main()
{
	DungeonSystem::getInstance();
	MapData::getInstance();

	FontAsset::Register(L"statusFont", int(MapData::getInstance().getMainGridSize().x / 8.0), Typeface::Heavy);
	FontAsset::Register(L"menuFont", 13, Typeface::Medium);
	FontAsset::Register(L"logFont", 12, Typeface::Bold);

	while (System::Update())
	{
		DungeonSystem::getInstance().update();
		DungeonSystem::getInstance().draw();

		if (Input::Key0.clicked)
			DungeonSystem::getInstance().shiftNextFloor();
	}
}
