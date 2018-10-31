#include <Siv3D.hpp>
#include "function.h"
#include "LogSystem.h"
#include "MenuSystem.h"
#include "MapData.h"
#include "Character.h"
#include "DungeonSystem.h"

void Main()
{
	/*
	String str(L"1/2/3/1");
	auto sss = str.split('/');
	for (auto& e : sss)
	{
		if(std::stoi(e.str())==1)
			Println(e);
	}
	*/
	DungeonSystem::getInstance();
	Window::Resize(GridtoXY(MapData::getInstance().getDrawRange()) + Point(400, 0));;
	LogSystem::getInstance().setOrigin(Point(GridtoXY(MapData::getInstance().getDrawRange()).x, 300));
	LogSystem::getInstance().setSize(Size(Window::Width() - LogSystem::getInstance().getOrigin().x, LogSystem::getInstance().getSize().y));

	FontAsset::Register(L"statusFont", int(MapData::getInstance().getMainGridSize().x / 8.0), Typeface::Heavy);
	FontAsset::Register(L"menuFont", 13, Typeface::Medium);
	FontAsset::Register(L"logFont", 12, Typeface::Bold);

	//MapData::getInstance().drawCharaNum();
	//MapData::getInstance().drawItemNum();
	while (System::Update())
	{
		DungeonSystem::getInstance().update();
		DungeonSystem::getInstance().draw();

		if (Input::Key0.clicked)
			DungeonSystem::getInstance().shiftNextFloor();
	}
}
