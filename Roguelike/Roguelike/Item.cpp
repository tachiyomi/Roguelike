#include "Item.h"
#include "MapData.h"

//�A�C�e��
Item::Item(Point pos)
{
	gridPosition = pos;
	xyPosition = GridtoCenterXY(pos);
	color = Palette::Palegreen;
	name = L"Item";
	used = false;
	inInventory = false;
	choice = Array<String>{ L"�H�ׂ�",L"�̂Ă�" ,L"�����ɒu��" };
}
Item::Item(int x, int y) :Item(Point(x, y)) {}
void Item::draw()
{
	if (MapData::getInstance().getOneGridData(gridPosition).canBeDraw() && !inInventory)
		Rect(GridtoXY(gridPosition - MapData::getInstance().getCenterPoint() + MapData::getInstance().getDrawRange() / 2), MapData::getInstance().getMainGridSize())(img).draw().drawFrame(1, 0, color);
}
void Item::doSomethingAtRemoval()
{
	LogSystem::getInstance().addLog(name + L"�͎���ꂽ�B");
}

std::vector<String> ShimarinDango::selectTarget(std::vector<size_t> ints)
{
	std::vector<String> re;
	if (ints.size() == 1)
	{
		for (size_t i = 0; i < MapData::getInstance().getCharacterData().size(); i++)
			re.emplace_back(MapData::getInstance().getCharacterData()[i]->getName());
		return re;
	}
	else
	{
		int i = (int)ints[0];
		eat(i);
		re.clear();
		return re;
	}
}
void ShimarinDango::eat(size_t i)
{
	std::shared_ptr<Character> character = MapData::getInstance().getCharacterData()[i];
	int heal = 100;
	heal = character->increaseHP(heal);
	LogSystem::getInstance().addLog(name + L"��" + character->getName() + L"�ɐH�ׂ������B");
	LogSystem::getInstance().addLog(character->getName() + L"��" + name + L"�ɂ����" + ToString(heal) + L"��HP���񕜂����B");
	use();
}

std::vector<String> Microphone::selectSong(std::vector<size_t> ints)
{
	std::vector<String> re;
	if (ints.size() == 1)
	{
		re = std::vector<String>{ L"SHINY DAYS",L"�ӂ�т��" };
		return re;
	}
	else
	{
		int i = (int)ints[0];
		song(i);
		re.clear();
		return re;
	}
}
void Microphone::song(size_t i)
{
	std::shared_ptr<Character> character = MapData::getInstance().getOneGridData(gridPosition).getCharacter();
	switch (i)
	{
		case 0:
		{
			int atk = 10;
			atk = character->increaseATK(atk);
			LogSystem::getInstance().addLog(character->getName() + L"�́uSHINY DAYS�v��M�������B");
			LogSystem::getInstance().addLog(character->getName() + L"�͍U���͂�" + ToString(atk) + L"�㏸�����B");
			break;
		}
		case 1:
		{
			int def = 10;
			def = character->increaseDEF(def);
			LogSystem::getInstance().addLog(character->getName() + L"�́u�ӂ�т��v��M�������B");
			LogSystem::getInstance().addLog(character->getName() + +L"�͖h��͂�" + ToString(def) + L"�㏸�����B");
			break;
		}
	}
	use();
}

void Blade::equipped()
{
	MapData::getInstance().getOneGridData(gridPosition).getCharacter()->equipped(shared_from_this());
	isEquipped = true;
	LogSystem::getInstance().addLog(name + L"�𑕔������B");
	name.append(L"[������]");
}

void Blade::takeout()
{
	MapData::getInstance().getOneGridData(gridPosition).getCharacter()->takeout(shared_from_this());
	isEquipped = false;
	name = name.remove(L"[������]");
	LogSystem::getInstance().addLog(name + L"�𑕔����������B");
}
