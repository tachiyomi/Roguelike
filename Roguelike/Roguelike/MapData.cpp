#include "MapData.h"
#include <typeinfo>
#include "function.h"
#include "LogSystem.h"
#include "Character.h"
#include "MenuSystem.h"

MapData::MapData()
{
	stairs = Texture(L"/2000");
	outsideGrid = GridData();
	centerGrid = Point(0, 0);
	mainDrawRange = Size(9, 7);
	mainGridSize = Size(80, 80);
	mainOrigin = Point(10, 40);
	mainDrawSize = mainDrawRange * mainGridSize;
	subOrigin = Point(mainDrawSize.x + mainOrigin.x * 2, 0);
	subDrawSize = Point(360, 300);

	MenuSystem::getInstance().setOrigin(Size::One * 30);
	MenuSystem::getInstance().setSize(mainDrawSize - Size::One * 60);
	LogSystem::getInstance().setOrigin(Point(subOrigin.x, subDrawSize.y));
	LogSystem::getInstance().setSize(Size(Window::Width() - subOrigin.x, subDrawSize.y));

	updateTimer.set(0s);
}
void MapData::loadMap(String stageName, int i)
{
	const CSVReader reader(L"Dungeon/" + stageName + L"/data" + ToString(i) + L".csv");

	if (!reader)
		return;

	const size_t width = reader.columns(0);
	const size_t height = reader.rows;

	mapGrid.clear();
	mapGrid = Grid<GridData>(height, width);

	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			String exit = reader.getOr<String>(y, x, L"");
			setupOneGrid(mapGrid[x][y], exit, Point(x, y));
		}
	}

	if (subDrawSize.x / mapGrid.height > subDrawSize.y / mapGrid.width)
	{
		subGridSize = Size::One*(int)(subDrawSize.y / mapGrid.width);
		subOrigin = Point(mainDrawSize.x + mainOrigin.x * 2 + subDrawSize.x / 2.0 - (subGridSize.x*mapGrid.height) / 2.0, 0);
	}
	else
	{
		subGridSize = Size::One*(int)(subDrawSize.x / mapGrid.height);
		subOrigin = Point(mainDrawSize.x + mainOrigin.x * 2 + subDrawSize.y / 2.0 - (subGridSize.y*mapGrid.width) / 2.0, 0);
	}
}
void MapData::setupOneGrid(GridData& grid, String str , Point p)
{
	std::vector<String> splittedStr = str.split('/');

	while (splittedStr.size() < 3)
		splittedStr.emplace_back(L"0");
	for (auto& e : splittedStr)
	{
		if (e.isEmpty)
			e = L"0";
	}

	grid.setTerrain(std::stoi(splittedStr[0].str()));

	switch (std::stoi(splittedStr[1].str()))
	{
	case CharacterId::player:
		if (getCharacterArray(CharacterId::player).size() == 0)
			MapData::getInstance().registerCharacter(Player(p));
		else
		{
			getCharacterArray(CharacterId::player)[0]->setGridPosition(p);
			getOneGridData(p).setCharacter(getCharacterArray(CharacterId::player)[0]);
		}
		MapData::getInstance().setCenterPoint(p);
		break;
	case CharacterId::frankenstein:
		MapData::getInstance().registerCharacter(Frankenstein(p));
		break;
	case CharacterId::ghost:
		MapData::getInstance().registerCharacter(Ghost(p));
		break;
	}

	switch (std::stoi(splittedStr[2].str()))
	{
	case ItemId::potion:
		MapData::getInstance().registerItem(Potion(p));
		break;
	}
}
void MapData::update()
{
	if (characters.empty())
		return;

	if (AllOf(characters, [](std::shared_ptr<Character> c) {return c->getStatus() == ActionStatus::WaitOtherAction; }))
	{
		sort();
		if (!characters.empty())
		{
			characters[0]->setStatus(ActionStatus::WaitKeyInput);
			characters[0]->applyTurnStartAbility();
		}
		return;
	}

	for (size_t i = 0; i < characters.size(); i++)
	{
		if (characters[i]->getStatus() == ActionStatus::WaitKeyInput)
		{
			characters[i]->act();
		}
		else if (characters[i]->getStatus() == ActionStatus::EndAction)
		{
			if (!updateTimer.isActive())
			{
				updateTimer.start();
				break;
			}
			else if (updateTimer.ms() < 1)
				break;
			else
				updateTimer.set(0s);

			characters[i]->applyTurnEndAbility();
			characters[i]->setStatus(ActionStatus::WaitOtherAction);
			if (i != characters.size() - 1)
			{
				characters[i + 1]->setStatus(ActionStatus::WaitKeyInput);
				characters[i + 1]->applyTurnStartAbility();
			}
			break;
		}
	}
}
void MapData::fight(const std::shared_ptr<Character>& A, const std::shared_ptr<Character>& B)
{
	std::shared_ptr<Character> copyA = std::make_shared<Character>(*A);
	std::shared_ptr<Character> copyB = std::make_shared<Character>(*B);
	A->applyAttackAbility(A, B, copyA, copyB);
	B->applyDefendAbility(A, B, copyA, copyB);

	int damage = copyA->getATK() - copyB->getDEF();
	if (damage < 0)
		damage = 0;
	damage = B->decreaseHP(damage);
	LogSystem::getInstance().addLog(A->getName() + L"は" + B->getName() + L"に" + ToString(damage) + L"ダメージ与えた。");
}
void MapData::drawMainArea()
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
			drawOneGridGround(GridToXY(x - startingPos.x, y - startingPos.y), mainGridSize, k);

			if (MapData::getInstance().getOneGridData(x, y).isUnderCharacter())
			{
				if (getOneGridData(x, y).getCharacter()->getId() == CharacterId::player)
					k = 10;
				else
					k = 20;
			}
			else if (MapData::getInstance().getOneGridData(x, y).isUnderItem())
				k = 30;
			drawOneGridGround(GridToXY(x - startingPos.x, y - startingPos.y), mainGridSize, k);
		}
	}


	for (int y = startingPos.y; y < endPos.y; y++)
		for (int x = startingPos.x; x < endPos.x; x++)
			if (MapData::getInstance().getOneGridData(x, y).isUnderItem())
				getOneGridData(x, y).getItem()->draw();

	for (int y = startingPos.y; y < endPos.y; y++)
		for (int x = startingPos.x; x < endPos.x; x++)
			if (MapData::getInstance().getOneGridData(x, y).isUnderCharacter())
				getOneGridData(x, y).getCharacter()->draw();

	if (MenuSystem::getInstance().isOpening())
	{
		Rect(mainDrawSize).draw(Color(Palette::Black, 120));
		MenuSystem::getInstance().draw();
	}
}
void MapData::drawSubArea()
{
	Transformer2D transformer(Mat3x2::Translate(subOrigin), false);
	drawSubMap();
}
void MapData::drawSubMap()
{
	for (int y = 0; y < (int)mapGrid.width; y++)
	{
		for (int x = 0; x < (int)mapGrid.height; x++)
		{
			int k = MapData::getInstance().getOneGridData(x, y).getTerrain();
			if (k == 0)k = 400;
			MapData::getInstance().getOneGridData(x, y).setEnableDraw(true);
			drawOneGridGround(SubGridToXY(x, y), subGridSize, k);

			if (MapData::getInstance().getOneGridData(x, y).isUnderCharacter())
			{
				if (getOneGridData(x, y).getCharacter()->getId() == CharacterId::player)
					k = 100;
				else
					k = 200;
			}
			else if (MapData::getInstance().getOneGridData(x, y).isUnderItem())
				k = 300;
			drawOneGridGround(SubGridToXY(x, y), subGridSize, k);
		}
	}
}
void MapData::drawAbility()
{
	if (characters.size() == 0)
		return;

	Array<String> strs = characters[0]->getAbility();
	for (size_t i = 0; i < strs.size(); i++)
		FontAsset(L"menuFont")(strs[i]).draw(0, (double)i * 30).drawFrame(0.0, 1.5, Palette::Green);
}
void MapData::reset()
{
	characters.clear();
	items.clear();
	LogSystem::getInstance().clearLog();
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
		if (items[i]->isUsed())
		{
			items[i]->doSomethingAtRemoval();
			items.erase(items.begin() + i);
			i--;
		}
	}
}
void MapData::deleteExceptPlayer()
{
	for (size_t i = 0; i < characters.size(); i++)
	{
		if (characters[i]->getId() != CharacterId::player)
		{
			characters.erase(characters.begin() + i);
			i--;
		}
	}
	for (size_t i = 0; i < items.size(); i++)
	{
		bool flag = true;
		Array<std::weak_ptr<Item>> ptr = MapData::getInstance().getCharacterArray(CharacterId::player)[0]->getInventory();
		for (size_t j = 0; j < ptr.size(); j++)
			if (items[i] == ptr[j].lock())
				flag = false;
		if (flag)
		{
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
	case 2:
		Rect(p, s).draw(Palette::Indianred).drawFrame(1, 0, Palette::Black);
		Rect(p, s)(stairs).draw(Palette::Navajowhite);
		break;

		//mainMap
	case 10:
		//Rect(p, s).draw(Color(Palette::Aqua, 100)).drawFrame(1, 0, Palette::Black);
		break;
	case 20:
		//Rect(p, s).draw(Color(Palette::Tomato, 100)).drawFrame(1, 0, Palette::Black);
		break;
	case 30:
		//Rect(p, s).draw(Color(Palette::Mediumspringgreen, 100)).drawFrame(1, 0, Palette::Black);
		break;

	case 60:
		Rect(p, s).draw(Color(Palette::Mediumorchid, 100)).drawFrame(1, 0, Palette::Black);
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
	case 400:
		break;

	default:
		Rect(p, s).draw(Palette::Dimgray).drawFrame(1, 0, Palette::Black);
		break;
	}
}
