#include "Item.h"
#include "MapData.h"

//アイテム
Item::Item(Point pos)
{
	gridPosition = pos;
	xyPosition = GridtoCenterXY(pos);
	color = Palette::Palegreen;
	name = L"Item";
	used = false;
	inInventory = false;
	choice = Array<String>{ L"食べる",L"捨てる" ,L"足元に置く" };
}
Item::Item(int x, int y) :Item(Point(x, y)) {}
void Item::draw()
{
	if (MapData::getInstance().getOneGridData(gridPosition).canBeDraw() && !inInventory)
		Rect(GridtoXY(gridPosition - MapData::getInstance().getCenterPoint() + MapData::getInstance().getDrawRange() / 2), MapData::getInstance().getMainGridSize())(img).draw().drawFrame(1, 0, color);
}
void Item::doSomethingAtRemoval()
{
	LogSystem::getInstance().addLog(name + L"は失われた。");
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
	LogSystem::getInstance().addLog(name + L"を" + character->getName() + L"に食べさせた。");
	LogSystem::getInstance().addLog(character->getName() + L"は" + name + L"によって" + ToString(heal) + L"のHPを回復した。");
	use();
}

std::vector<String> Microphone::selectSong(std::vector<size_t> ints)
{
	std::vector<String> re;
	if (ints.size() == 1)
	{
		re = std::vector<String>{ L"SHINY DAYS",L"ふゆびより" };
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
			LogSystem::getInstance().addLog(character->getName() + L"は「SHINY DAYS」を熱唱した。");
			LogSystem::getInstance().addLog(character->getName() + L"は攻撃力が" + ToString(atk) + L"上昇した。");
			break;
		}
		case 1:
		{
			int def = 10;
			def = character->increaseDEF(def);
			LogSystem::getInstance().addLog(character->getName() + L"は「ふゆびより」を熱唱した。");
			LogSystem::getInstance().addLog(character->getName() + +L"は防御力が" + ToString(def) + L"上昇した。");
			break;
		}
	}
	use();
}

void Blade::equipped()
{
	MapData::getInstance().getOneGridData(gridPosition).getCharacter()->equipped(shared_from_this());
	isEquipped = true;
	LogSystem::getInstance().addLog(name + L"を装備した。");
	name.append(L"[装備中]");
}

void Blade::takeout()
{
	MapData::getInstance().getOneGridData(gridPosition).getCharacter()->takeout(shared_from_this());
	isEquipped = false;
	name = name.remove(L"[装備中]");
	LogSystem::getInstance().addLog(name + L"を装備解除した。");
}
