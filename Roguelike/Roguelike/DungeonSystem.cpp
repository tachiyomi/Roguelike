#include "DungeonSystem.h"

void DungeonSystem::update()
{
	MapData::getInstance().update();
	MapData::getInstance().deleteObject();
}

void DungeonSystem::draw()
{
	MapData::getInstance().drawMainArea();
	MapData::getInstance().drawSubArea();
	LogSystem::getInstance().displayLog();

	//if (Input::MouseL.clicked && MapData::getInstance().getOneGridData(getMouseGrid()).enableAddCharacter())
	//	MapData::getInstance().registerCharacter(Sandbag(getMouseGrid()));
	//if (Input::MouseR.clicked && MapData::getInstance().getOneGridData(getMouseGrid()).enableAddItem())
	//	MapData::getInstance().registerItem(Glasses(getMouseGrid()));
}

void DungeonSystem::shiftNextFloor()
{
	MapData::getInstance().deleteExceptPlayer();
	floorNumber++;
	MapData::getInstance().loadMap(floorNumber);
}

DungeonSystem::DungeonSystem():
	floorNumber(1)
{
	MapData::getInstance().loadMap(floorNumber);
	/*
	MapData::getInstance().loadMap(floorNumber);
	MapData::getInstance().registerCharacter(Player(5, 5));
	MapData::getInstance().registerCharacter(Sandbag(5, 4));
	MapData::getInstance().registerCharacter(Kyonshih(6, 4));
	MapData::getInstance().registerItem(Glasses(5, 6));
	MapData::getInstance().registerItem(ShimarinDango(6, 6));
	MapData::getInstance().registerItem(Microphone(7, 6));
	MapData::getInstance().registerItem(Blade(5, 7));
	MapData::getInstance().registerItem(Shield(6, 7));
	MapData::getInstance().registerItem(Ring(7, 7));
	MapData::getInstance().registerItem(GemOfFly(5, 8));
	*/
}
