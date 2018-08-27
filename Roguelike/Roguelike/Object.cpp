#include "Object.h"
#include <Siv3D.hpp>
#include "function.h"
#include "LogSystem.h"
#include "MapData.h"

//キャラクター
Character::Character(Point pos)
{
	xyPosition = GridtoCenterXY(pos);
	direction = 0;
	status = CharacterStatus::WaitOtherAction;
}
Character::Character(int x, int y) :Character(Point(x, y)) {}
void Character::act()
{
	if (move() || attack())
	{
		status = CharacterStatus::EndAction;
	}
}
void Character::draw()
{
	if (!MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).canBeDraw())
		return;

	const Point drawPosition = GridtoXY(XYtoGrid(xyPosition) - MapData::getInstance().getCenterPoint() + MapData::getInstance().getDrawRange() / 2);
	Rect(Point(drawPosition.x, drawPosition.y), MapData::getInstance().getMainGridSize())(img).draw().drawFrame(1, 1, color);
	Circle(GridtoCenterXY(XYtoGrid(drawPosition)) + Vec2(MapData::getInstance().getMainGridSize().x / 2 * cos(Radians(direction)), MapData::getInstance().getMainGridSize().x / 2 * sin(Radians(direction))), 2).draw(Palette::Black);

	FontAsset(L"statusFont")(L"HP " + ToString(HP)).draw(drawPosition + Point(5, MapData::getInstance().getMainGridSize().y / 3 * 0), Palette::Black);
	//FontAsset(L"statusFont")(L"ATK " + ToString(ATK)).draw(drawPosition + Point(5, gridSize.y / 3 * 1), Palette::Black);
	//FontAsset(L"statusFont")(L"DEF " + ToString(DEF)).draw(drawPosition + Point(5, gridSize.y / 3 * 2), Palette::Black);
}
bool Character::move()
{
	return false;
}
bool Character::attack()
{
	return false;
}
void Character::doSomethingAtDeath()
{
	LogSystem::getInstance().addLog(name + L"を倒しました。");
	MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).deleteCharacter();
}

//プレイヤー
Player::Player(Point pos) :Character(pos)
{
	img = Texture(L"Images/image.png");
	color = Palette::Dodgerblue;
	name = L"なでしこ";
	MapData::getInstance().setCenterPoint(XYtoGrid(xyPosition));

	HP = 1000;
	ATK = 80;
	DEF = 60;
}
void Player::act()
{
	if (move() || attack())
	{
		status = CharacterStatus::EndAction;
	}
}
bool Player::move()
{
	const Point formerGrid = XYtoGrid(xyPosition);
	const int formerDirection = direction;

	if (Input::KeySpace.clicked)
	{
		LogSystem::getInstance().addLog(L"<shima1>gうい");
		MapData::getInstance().setCenterPoint(XYtoGrid(xyPosition));
	}

	bool keyInput = true;
	if (Input::KeyUp.clicked || Gamepad(0).povForward.clicked)
		direction = 270;
	else if (Input::KeyDown.clicked || Gamepad(0).povBackward.clicked)
		direction = 90;
	else if (Input::KeyLeft.clicked || Gamepad(0).povLeft.clicked)
		direction = 180;
	else if (Input::KeyRight.clicked || Gamepad(0).povRight.clicked)
		direction = 0;
	else
		keyInput = false;

	if ( (Input::KeyControl.pressed || Gamepad(0).button(3).pressed) || !keyInput)
		return false;

	Point p(cos(Radians(direction)), sin(Radians(direction)));
	if (MapData::getInstance().getOneGridData(XYtoGrid(xyPosition) + p).enableAddCharacter())
		xyPosition += GridtoXY(p);

	if (XYtoGrid(xyPosition) != formerGrid)
	{
		MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).setCharacter(MapData::getInstance().getOneGridData(formerGrid).getCharacter());
		MapData::getInstance().getOneGridData(formerGrid).deleteCharacter();
		MapData::getInstance().setCenterPoint(XYtoGrid(xyPosition));

		if (MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).isUnderItem())
		{
			inventory.emplace_back(MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).getItem());
			LogSystem::getInstance().addLog(MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).getItem()->getName() + L"を拾った。");
			MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).deleteItem();
		}

		return true;
	}
	return false;
}
bool Player::attack()
{
	if (!(Input::KeyEnter.clicked || Gamepad(0).button(1).clicked))
		return false;

	const Point frontOfMe = XYtoGrid(xyPosition) + Point(cos(Radians(direction)), sin(Radians(direction)));
	if (MapData::getInstance().getOneGridData(frontOfMe).isUnderCharacter())
	{
		const int damage = ATK - MapData::getInstance().getOneGridData(frontOfMe).getCharacter()->getDEF();
		MapData::getInstance().getOneGridData(frontOfMe).getCharacter()->causeDamage(damage);
		LogSystem::getInstance().addLog(name + L"は" + MapData::getInstance().getOneGridData(frontOfMe).getCharacter()->getName() + L"に" + ToString(damage) + L"ダメージ与えた。");
	}
	return true;
}

bool Sandbag::move()
{
	return true;

	/*
	const Point formerGrid = XYtoGrid(xyPosition);
	const int formerDirection = direction;

	direction = int(Random() * 4) + 1;
	direction *= 90;

	Point p(cos(Radians(direction)), sin(Radians(direction)));
	if (MapData::getInstance().getOneGridData(XYtoGrid(xyPosition) + p).enableAddCharacter())
	xyPosition += GridtoXY(p);

	if (XYtoGrid(xyPosition) != formerGrid)
	{
	MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).setCharacter(MapData::getInstance().getOneGridData(formerGrid).getCharacter());
	MapData::getInstance().getOneGridData(formerGrid).deleteCharacter();
	return true;
	}
	return true;
	*/
}

bool Kyonshih::attack()
{
	for (int i = 0; i < 4; i++)
	{
		const Point frontOfMe = XYtoGrid(xyPosition) + Point(cos(Radians(i * 90)), sin(Radians(i * 90)));
		if (MapData::getInstance().getOneGridData(frontOfMe).isUnderCharacter())
		{
			if (typeid(*MapData::getInstance().getOneGridData(frontOfMe).getCharacter()) == typeid(Player))
			{
				const int damage = 20;
				MapData::getInstance().getOneGridData(frontOfMe).getCharacter()->causeDamage(damage);
				LogSystem::getInstance().addLog(name + L"は" + MapData::getInstance().getOneGridData(frontOfMe).getCharacter()->getName() + L"に" + ToString(damage) + L"ダメージ与えた。");
				direction = i * 90;
				return true;
			}
		}
	}
	return true;
}

//アイテム
Item::Item(Point pos)
{
	gridPosition = pos;
	xyPosition = GridtoCenterXY(pos);
	color = Palette::Palegreen;
	name = L"Item";
	used = false;
	choice = Array<String>{ L"食べる",L"捨てる" ,L"足元に置く" };
}
Item::Item(int x, int y) :Item(Point(x, y)) {}
void Item::draw()
{
	if (MapData::getInstance().getOneGridData(gridPosition).canBeDraw())
		Rect(GridtoXY(gridPosition - MapData::getInstance().getCenterPoint() + MapData::getInstance().getDrawRange() / 2), MapData::getInstance().getMainGridSize())(img).draw().drawFrame(1, 1, color);
}
void Item::doSomethingAtDeath()
{
	LogSystem::getInstance().addLog(name + L"は失われた。");
}

void Glasses::use()
{
	used = true;
}

void ShimarinDango::use()
{
	used = true;
}
