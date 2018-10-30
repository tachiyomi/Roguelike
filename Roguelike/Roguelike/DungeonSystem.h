#pragma once
#include<Siv3D.hpp>
#include "MapData.h"

class DungeonSystem
{
public:
	static DungeonSystem& getInstance()
	{
		static DungeonSystem instance;
		return instance;
	}
	void update();
	void draw();

	void shiftNextFloor();

protected:
	DungeonSystem();
	virtual ~DungeonSystem() {}
private:
	void operator = (const DungeonSystem&) {}
	DungeonSystem(const DungeonSystem&) {}

	int floorNumber;
};