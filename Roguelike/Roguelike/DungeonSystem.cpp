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
}
