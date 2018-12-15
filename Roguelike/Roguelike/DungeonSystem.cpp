#include "DungeonSystem.h"

void DungeonSystem::update()
{
	MapData::getInstance().update();
	MapData::getInstance().deleteObject();
}

void DungeonSystem::draw()
{
	/*
	if (!timer.isPaused())
	{
		if (timer.ms() > 2000)
			timer.pause();
		return;
	}
	*/
	if (MapData::getInstance().getCharacterArray(CharacterId::player).size() != 0)
	{
		const int hp = MapData::getInstance().getCharacterArray(CharacterId::player)[0]->getHP();
		const int mhp = MapData::getInstance().getCharacterArray(CharacterId::player)[0]->getMaxHP();
		FontAsset(L"menuFont")(L"B" + ToString(6 - floorNumber) + L"F   HP:" + ToString(hp) + L"/" + ToString(mhp)).draw(Point(15, 10), Palette::White);
		const Vec2 tl = Point(200, 10);
		Rect((int)tl.x, (int)tl.y, Size(500.0, 25)).draw(Palette::Darkslategray).drawFrame(1.0, 0.0, Palette::White);
		Rect((int)tl.x, (int)tl.y, Size(500.0*hp / mhp, 25))
			.draw((double)hp / mhp < 0.5 ? ((double)hp / mhp < 0.1 ? Palette::Red : Palette::Yellow) : Palette::Green).drawFrame(1.0, 0.0, Palette::White);
	}

	MapData::getInstance().drawMainArea();
	MapData::getInstance().drawSubArea();
	LogSystem::getInstance().displayLog();
}

void DungeonSystem::startAtFirstFloor()
{
	MapData::getInstance().reset();
	/*
	LogSystem::getInstance().addLog(L"------------");
	LogSystem::getInstance().addLog(L"�Q�[���|�[�Y:P�L�[");
	LogSystem::getInstance().addLog(L"�A�C�e��:I�L�[");
	LogSystem::getInstance().addLog(L"�U��:�G���^�[�L�[");
	LogSystem::getInstance().addLog(L"�ړ��F���L�[");
	LogSystem::getInstance().addLog(L"--������@--");
	*/
	floorNumber = 1;
	LogSystem::getInstance().addLog(L"�n��" + ToString(6 - floorNumber) + L"�K�ɂ��ǂ蒅�����B");
	MapData::getInstance().loadMap(floorNumber);
	timer.restart();
	totaltimer.restart();
}

void DungeonSystem::shiftNextFloor()
{
	MapData::getInstance().deleteExceptPlayer();
	floorNumber++;
	LogSystem::getInstance().addLog(L"�n��" + ToString(6 - floorNumber) + L"�K�ɂ��ǂ蒅�����B");
	MapData::getInstance().loadMap(floorNumber);
	timer.restart();
}

DungeonSystem::DungeonSystem():
	floorNumber(1)
{
	timer.set(0s);
}
