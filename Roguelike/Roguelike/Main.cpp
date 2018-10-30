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
	Window::Resize(GridtoXY(MapData::getInstance().getDrawRange().x * 2, MapData::getInstance().getDrawRange().y) + Point(0, LogSystem::getInstance().getSize().y));
	LogSystem::getInstance().setOrigin(Point(0, Window::Height() - LogSystem::getInstance().getSize().y));
	LogSystem::getInstance().setSize(Size(Window::Width(), LogSystem::getInstance().getSize().y));

	FontAsset::Register(L"statusFont", int(MapData::getInstance().getMainGridSize().x / 8.0), Typeface::Heavy);
	FontAsset::Register(L"menuFont", 13, Typeface::Medium);
	FontAsset::Register(L"logFont", 12, Typeface::Bold);


	while (System::Update())
	{
		DungeonSystem::getInstance().update();
		DungeonSystem::getInstance().draw();
	}
}
