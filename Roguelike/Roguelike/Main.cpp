#include <Siv3D.hpp>
#include <HamFramework.hpp>
#include "Scene.h"
#include "function.h"
#include "LogSystem.h"
#include "MenuSystem.h"
#include "MapData.h"
#include "Character.h"
#include "DungeonSystem.h"

void Main()
{
	Window::SetTitle(L"GraveDungeon");
	Window::Resize(1120, 630);

	SceneManager<int,CommonData> manager;
	manager.add<Title>(0);
	manager.add<Select>(1);
	manager.add<DisplayFloor>(2);
	manager.add<Play>(3);
	manager.add<GameClear>(4);
	manager.add<GameOver>(5);

	manager.init(0);

	FontAsset::Register(L"statusFont", int(MapData::getInstance().getMainGridSize().x / 8.0), Typeface::Heavy);
	FontAsset::Register(L"menuFont", 13, Typeface::Medium);
	FontAsset::Register(L"logFont", 12, Typeface::Bold);
	FontAsset::Register(L"pauseFont", 25, Typeface::Medium);
	FontAsset::Register(L"titleFont", 20, Typeface::Light);

	while (System::Update())
	{
		manager.updateAndDraw();
	}
}
