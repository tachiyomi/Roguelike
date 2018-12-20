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

	bool selectStage(String);
	void startAtFirstFloor();
	void shiftNextFloor();

	int getFloorNumber() { return floorNumber; }
	String getTotaltimeANDstop()
	{
		totaltimer.pause();
		return ToString(totaltimer.min()) + L"•ª " + ToString(totaltimer.s() % 60) + L"•b"; 
	}

	bool gameClear() { return floorNumber > 5; }
	bool gameOver() { return MapData::getInstance().getCharacterArray(CharacterId::player).size() == 0; }
protected:
	DungeonSystem();
	virtual ~DungeonSystem() {}
private:
	void operator = (const DungeonSystem&) {}
	DungeonSystem(const DungeonSystem&) {}

	String stageName;
	int floorNumber;
	Stopwatch timer, totaltimer;
};