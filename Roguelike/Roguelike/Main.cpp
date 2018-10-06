#include <Siv3D.hpp>
#include "function.h"
#include "LogSystem.h"
#include "MenuSystem.h"
#include "MapData.h"
#include "Character.h"

void Main()
{
	MapData::getInstance().loadMap();
	Window::Resize(GridtoXY(MapData::getInstance().getDrawRange().x * 2, MapData::getInstance().getDrawRange().y) + Point(0, LogSystem::getInstance().getSize().y));
	LogSystem::getInstance().setOrigin(Point(0, Window::Height() - LogSystem::getInstance().getSize().y));
	LogSystem::getInstance().setSize(Size(Window::Width(), LogSystem::getInstance().getSize().y));

	FontAsset::Register(L"statusFont", int(MapData::getInstance().getMainGridSize().x / 6.0), Typeface::Medium);
	FontAsset::Register(L"menuFont", 15, Typeface::Medium);
	FontAsset::Register(L"logFont", 12, Typeface::Bold);

	MapData::getInstance().registerCharacter(Player(5, 5));
	MapData::getInstance().registerCharacter(Sandbag(5, 4));
	MapData::getInstance().registerCharacter(Kyonshih(6, 4));
	MapData::getInstance().registerItem(Glasses(5, 6));
	MapData::getInstance().registerItem(ShimarinDango(6, 6));
	MapData::getInstance().registerItem(Microphone(7, 6));

	while (System::Update())
	{
		MapData::getInstance().update();
		MapData::getInstance().deleteObject();

		MapData::getInstance().drawMainMap();
		MapData::getInstance().drawSubMap();
		LogSystem::getInstance().displayLog();


		if (Input::MouseL.clicked && MapData::getInstance().getOneGridData(getMouseGrid()).enableAddCharacter())
			MapData::getInstance().registerCharacter(Sandbag(getMouseGrid()));
		if (Input::MouseR.clicked && MapData::getInstance().getOneGridData(getMouseGrid()).enableAddItem())
			MapData::getInstance().registerItem(Glasses(getMouseGrid()));
	}
}
