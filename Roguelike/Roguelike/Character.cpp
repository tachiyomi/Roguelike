#include "Character.h"
#include "Item.h"
#include <Siv3D.hpp>
#include "function.h"
#include "LogSystem.h"
#include "MapData.h"
#include "MenuSystem.h"

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
	move();
	attack();
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
void Character::move()
{
}
void Character::attack()
{
}
void Character::doSomethingAtDeath()
{
	LogSystem::getInstance().addLog(name + L"を倒しました。");
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
	deleteItem();
	if (!MenuSystem::getInstance().isOpening())
	{
		move();
		attack();
		openInventory();
	}
	else
		useItem();
}
void Player::move()
{
	if (status != CharacterStatus::WaitKeyInput)
		return;

	const Point formerGrid = XYtoGrid(xyPosition);
	const int formerDirection = direction;

	if (Input::KeySpace.clicked)
	{
		LogSystem::getInstance().addLog(L"<shima1>うい");
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

	if ( (Input::KeyControl.pressed || Gamepad(0).button(0).pressed) || !keyInput)
		return;

	Point p(cos(Radians(direction)), sin(Radians(direction)));
	if (MapData::getInstance().getOneGridData(XYtoGrid(xyPosition) + p).enableAddCharacter())
		xyPosition += GridtoXY(p);

	if (XYtoGrid(xyPosition) != formerGrid)
	{
		MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).getWeakCharacter().swap(MapData::getInstance().getOneGridData(formerGrid).getWeakCharacter());
		MapData::getInstance().setCenterPoint(XYtoGrid(xyPosition));

		if (MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).isUnderItem())
		{
			MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).getItem()->inInventory = true;
			inventory.push_back(std::weak_ptr<Item>());
			inventory.back().swap(MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).getWeakItem());
			LogSystem::getInstance().addLog(inventory.back().lock()->getName() + L"を拾った。");
		}
		status = CharacterStatus::EndAction;
	}
}
void Player::attack()
{
	if (status != CharacterStatus::WaitKeyInput)
		return;

	if (!(Input::KeyEnter.clicked || Gamepad(0).button(1).clicked))
		return;

	const Point frontOfMe = XYtoGrid(xyPosition) + Point(cos(Radians(direction)), sin(Radians(direction)));
	if (MapData::getInstance().getOneGridData(frontOfMe).isUnderCharacter())
	{
		int damage = ATK - MapData::getInstance().getOneGridData(frontOfMe).getCharacter()->getDEF();
		damage = MapData::getInstance().getOneGridData(frontOfMe).getCharacter()->decreaseHP(damage);
		LogSystem::getInstance().addLog(name + L"は" + MapData::getInstance().getOneGridData(frontOfMe).getCharacter()->getName() + L"に" + ToString(damage) + L"ダメージ与えた。");
	}
	status = CharacterStatus::EndAction;
}
void Player::openInventory()
{
	if (status != CharacterStatus::WaitKeyInput)
		return;

	if (Input::KeyShift.clicked || Gamepad(0).button(7).clicked)
	{
		MenuSystem::getInstance().openMenu(shared_from_this());
	}
}
void Player::useItem()
{
	if (status != CharacterStatus::WaitKeyInput)
		return;

	if (Input::KeyShift.clicked || Gamepad(0).button(7).clicked)
		MenuSystem::getInstance().closeMenu();

	if (MenuSystem::getInstance().update())
		status = CharacterStatus::EndAction;
}

void Sandbag::move()
{
	status = CharacterStatus::EndAction;

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

void Kyonshih::attack()
{
	for (int i = 0; i < 4; i++)
	{
		const Point frontOfMe = XYtoGrid(xyPosition) + Point(cos(Radians(i * 90)), sin(Radians(i * 90)));
		if (MapData::getInstance().getOneGridData(frontOfMe).isUnderCharacter())
		{
			if (typeid(*MapData::getInstance().getOneGridData(frontOfMe).getCharacter().get()) == typeid(Player))
			{
				int damage = 20;
				damage = MapData::getInstance().getOneGridData(frontOfMe).getCharacter()->decreaseHP(damage);
				LogSystem::getInstance().addLog(name + L"は" + MapData::getInstance().getOneGridData(frontOfMe).getCharacter()->getName() + L"に" + ToString(damage) + L"ダメージ与えた。");
				direction = i * 90;
				status = CharacterStatus::EndAction;
				return;
			}
		}
	}
	status = CharacterStatus::EndAction;
}

