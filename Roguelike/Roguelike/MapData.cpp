#include "MapData.h"
#include <Siv3D.hpp>
#include <typeinfo>
#include "function.h"
#include "LogSystem.h"
#include "Character.h"
#include "MenuSystem.h"

MapData::MapData()
{
	outsideGrid = GridData();
	centerGrid = Point(0, 0);
	mainDrawRange = Size(7, 7);
	mainGridSize = Size(80, 80);
	mainOrigin = Point(0, 0);
	mainDrawSize = mainDrawRange * mainGridSize;
	subOrigin = Point(mainDrawSize.x, 0) + Point::One * 20;
	subDrawSize = mainDrawSize - Size::One * 40;
}
void MapData::loadMap()
{
	const CSVReader reader(L"Csvs/data.csv");

	if (!reader)
		return;

	const size_t width = reader.columns(0);
	const size_t height = reader.rows;

	mapGrid = Grid<GridData>(height, width);

	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			mapGrid[x][y].setTerrain(reader.getOr(y, x, -1));
		}
	}
	LogSystem::getInstance().addLog(L"マップが読み込まれました。");

	subGridSize = subDrawSize / std::max((int)mapGrid.height, (int)mapGrid.width);
}
void MapData::update()
{
	if (AllOf(characters, [](std::shared_ptr<Character> c) {return c->getStatus() == CharacterStatus::WaitOtherAction; }))
	{
		sort();
		if (!characters.empty())
			characters[0]->setStatus(CharacterStatus::WaitKeyInput);
		return;
	}

	for (size_t i = 0; i < characters.size(); i++)
	{
		if (characters[i]->getStatus() == CharacterStatus::WaitKeyInput)
		{
			if ( (Input::KeyShift.clicked || Gamepad(0).button(7).clicked) && !MenuSystem::getInstance().isOpening())
					MenuSystem::getInstance().openMenu(characters[i]);

			characters[i]->act();
		}
		else if (characters[i]->getStatus() == CharacterStatus::EndAction)
		{
			characters[i]->setStatus(CharacterStatus::WaitOtherAction);
			if (i != characters.size() - 1)
				characters[i + 1]->setStatus(CharacterStatus::WaitKeyInput);
			break;
		}
	}
}
void MapData::drawMainMap()
{
	Transformer2D transformer(Mat3x2::Translate(mainOrigin), false);
	setAllGridEnableDraw();
	const Size range = MapData::getInstance().getDrawRange();
	const Point startingPos = MapData::getInstance().getCenterPoint() - range / 2;
	const Point endPos = startingPos + range;

	for (int y = startingPos.y; y < endPos.y; y++)
	{
		for (int x = startingPos.x; x < endPos.x; x++)
		{
			int k = MapData::getInstance().getOneGridData(x, y).getTerrain();
			MapData::getInstance().getOneGridData(x, y).setEnableDraw(true);
			drawOneGridGround(GridtoXY(x - startingPos.x, y - startingPos.y), mainGridSize, k);

			if (MapData::getInstance().getOneGridData(x, y).isUnderCharacter())
			{
				if (typeid(*getOneGridData(x, y).getCharacter()) == typeid(Player))
					k = 10;
				else
					k = 20;
			}
			else if (MapData::getInstance().getOneGridData(x, y).isUnderItem())
				k = 30;
			drawOneGridGround(GridtoXY(x - startingPos.x, y - startingPos.y), mainGridSize, k);

			if (MapData::getInstance().getOneGridData(x, y).isUnderItem())
				getOneGridData(x, y).getItem()->draw();
			if (MapData::getInstance().getOneGridData(x, y).isUnderCharacter())
				getOneGridData(x, y).getCharacter()->draw();
		}
	}
	/*
	for (int y = startingPos.y; y < endPos.y; y++)
	{
		for (int x = startingPos.x; x < endPos.x; x++)
		{
			if (MapData::getInstance().getOneGridData(x, y).isUnderItem())
				getOneGridData(x, y).getItem()->draw();
			if (MapData::getInstance().getOneGridData(x, y).isUnderCharacter())
				getOneGridData(x, y).getCharacter()->draw();
		}
	}
	*/

	/*
	for (size_t i = 0; i < items.size(); i++)
	{
	if (MapData::getInstance().getOneGridData(items[i]->getGridPosition()).canBeDraw())
	items[i]->draw();
	}
	for (size_t i = 0; i < characters.size(); i++)
	{
	if (MapData::getInstance().getOneGridData(characters[i]->getGridPosition()).canBeDraw())
	characters[i]->draw();
	}
	*/
	if (Input::Key0.clicked)
		ClearPrint();
}
void MapData::drawSubMap()
{
	if (MenuSystem::getInstance().isOpening())
	{
		MenuSystem::getInstance().draw();
		return;
	}

	Transformer2D transformer(Mat3x2::Translate(subOrigin), false);
	for (int y = 0; y < (int)mapGrid.width; y++)
	{
		for (int x = 0; x < (int)mapGrid.height; x++)
		{
			int k = MapData::getInstance().getOneGridData(x, y).getTerrain();
			MapData::getInstance().getOneGridData(x, y).setEnableDraw(true);
			drawOneGridGround(SubGridtoXY(x, y), subGridSize, k);

			if (MapData::getInstance().getOneGridData(x, y).isUnderCharacter())
			{
				if (typeid(*getOneGridData(x, y).getCharacter()) == typeid(Player))
					k = 100;
				else
					k = 200;
			}
			else if (MapData::getInstance().getOneGridData(x, y).isUnderItem())
				k = 300;
			drawOneGridGround(SubGridtoXY(x, y), subGridSize, k);
		}
	}
}
void MapData::deleteObject()
{
	for (size_t i = 0; i < characters.size(); i++)
	{
		if (!characters[i]->enableLive())
		{
			characters[i]->doSomethingAtDeath();
			characters.erase(characters.begin() + i);
			i--;
		}
	}
	for (size_t i = 0; i < items.size(); i++)
	{
		if (items[i]->isInInventory())
		{
			getOneGridData(items[i]->getGridPosition()).deleteItem();
			//items[i]->doSomethingAtDeath();
			items.erase(items.begin() + i);
			i--;
		}
	}
}
void MapData::drawOneGridGround(Point p, Size s, int k)
{
	switch (k)
	{
	case 0:
		Rect(p, s).draw(Palette::Darkgray).drawFrame(1, 0, Palette::Black);
		break;
	case 1:
		Rect(p, s).draw(Palette::Sandybrown).drawFrame(1, 0, Palette::Sienna);
		break;

		//mainMap
	case 10:
		Rect(p, s).draw(Color(Palette::Aqua, 100)).drawFrame(1, 0, Palette::Black);
		break;
	case 20:
		Rect(p, s).draw(Color(Palette::Tomato, 100)).drawFrame(1, 0, Palette::Black);
		break;
	case 30:
		Rect(p, s).draw(Color(Palette::Mediumspringgreen, 100)).drawFrame(1, 0, Palette::Black);
		break;

		//subMap
	case 100:
		Rect(p, s).draw(Color(Palette::Aqua, 200)).drawFrame(1, 0, Palette::Black);
		break;
	case 200:
		Rect(p, s).draw(Color(Palette::Tomato, 200)).drawFrame(1, 0, Palette::Black);
		break;
	case 300:
		Rect(p, s).draw(Color(Palette::Mediumspringgreen, 200)).drawFrame(1, 0, Palette::Black);
		break;

	default:
		Rect(p, s).draw(Palette::Dimgray).drawFrame(1, 0, Palette::Black);
		break;
	}

}
