#include "Character.h"
#include "MapData.h"
#include "MenuSystem.h"
#include "DungeonSystem.h"

//キャラクター
Character::Character(Point pos) 
{
	xyPosition = GridtoCenterXY(pos);
	direction = Direction::Right;
	AS = ActionStatus::WaitOtherAction;
}
Character::Character(int x, int y) :Character(Point(x, y)) {}
void Character::act()
{
	attack();
	move();
}
void Character::draw()

{
	if (!MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).canBeDraw())
		return;

	const Point drawPosition = GridtoXY(XYtoGrid(xyPosition) - MapData::getInstance().getCenterPoint() + MapData::getInstance().getDrawRange() / 2);
	Rect(Point(drawPosition.x, drawPosition.y), MapData::getInstance().getMainGridSize()).rotated(ToRadian(direction))(img).draw();// .drawFrame(1, 0, color);

	/*
	Color c = Palette::Black;
	FontAsset(L"statusFont")(ToString(CS.getHP())+L"/"+ ToString(CS.getMaxHP())).draw(drawPosition + Point(3,0), c);
	FontAsset(L"statusFont")(L"[A]" + ToString(CS.getATK())).draw(drawPosition + Point(3, MapData::getInstance().getMainGridSize().y / 3 * 1), c);
	FontAsset(L"statusFont")(L"[D]" + ToString(CS.getDEF())).draw(drawPosition + Point(3, MapData::getInstance().getMainGridSize().y / 3 * 2), c);
	*/
}
void Character::move()
{
}
void Character::attack()
{
}
void Character::applyTurnStartAbility()
{
	for (size_t i = 0; i < abilities.size(); i++)
		abilities[i]->turnStart(shared_from_this());
	deleteAbility();
}
void Character::applyTurnEndAbility()
{
	for (size_t i = 0; i < abilities.size(); i++)
		abilities[i]->turnEnd(shared_from_this());
	deleteAbility();
}
void Character::applyAttackAbility(std::shared_ptr<Character> A, std::shared_ptr<Character> B,
	std::shared_ptr<Character> copyA, std::shared_ptr<Character>copyB)
{
	for (const auto& e : abilities)
	{
		e->whenCombat(A, B, copyA, copyB);
		e->whenAttack(A, B, copyA, copyB);
	}
}
void Character::applyDefendAbility(std::shared_ptr<Character> A, std::shared_ptr<Character> B, 
	std::shared_ptr<Character> copyA, std::shared_ptr<Character> copyB)
{
	for (const auto& e : abilities)
	{
		e->whenCombat(A, B, copyA, copyB);
		e->whenDefend(A, B, copyA, copyB);
	}
}
void Character::addAbility(std::shared_ptr<Ability> ability)
{
	if (ability == nullptr)
		return;

	if (AllOf(abilities, [ability](std::shared_ptr<Ability> a) {return a->getName() != ability->getName(); }))
		abilities.emplace_back(ability);
	LogSystem::getInstance().addLog(name + L"は" + ability->getName() + L"のアビリティを取得した。");
}
bool Character::enableLive()
{
	{
		if (CS.HP > 0)
			return true;
		else
			return false;
	}
}
void Character::doSomethingAtDeath()
{
	LogSystem::getInstance().addLog(name + L"を倒しました。");
}

//プレイヤー
Player::Player(Point pos) :Character(pos)
{
	id = CharacterId::player;
	img = Texture(L"/1000");
	color = Palette::Dodgerblue;
	name = L"メアリー";

	CS.setStatus(600, 60, 40, 600);
}
void Player::applyTurnEndAbility()
{
	for (size_t i = 0; i < abilities.size(); i++)
		abilities[i]->turnEnd(shared_from_this());
	deleteAbility();
	if (MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).getTerrain() == 2)
		DungeonSystem::getInstance().shiftNextFloor();
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
	if (AS != ActionStatus::WaitKeyInput)
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
		direction = Direction::Up;
	else if (Input::KeyDown.clicked || Gamepad(0).povBackward.clicked)
		direction = Direction::Down;
	else if (Input::KeyLeft.clicked || Gamepad(0).povLeft.clicked)
		direction = Direction::Left;
	else if (Input::KeyRight.clicked || Gamepad(0).povRight.clicked)
		direction = Direction::Right;
	else
		keyInput = false;

	if ( (Input::KeyShift.pressed || Gamepad(0).button(0).pressed) || !keyInput)
		return;

	if (MapData::getInstance().getOneGridData(XYtoGrid(xyPosition) + getGrid(direction)).enableAddCharacter())
		xyPosition += GridtoXY(getGrid(direction));

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

		for (auto& e : inventory)
			e.lock()->setGridPosition(XYtoGrid(xyPosition));
		AS = ActionStatus::EndAction;
	}
}
void Player::attack()
{
	if (AS != ActionStatus::WaitKeyInput)
		return;

	if (!(Input::KeyEnter.clicked || Gamepad(0).button(1).clicked))
		return;

	const Point frontOfMe = XYtoGrid(xyPosition) + getGrid(direction);
	if (MapData::getInstance().getOneGridData(frontOfMe).isUnderCharacter())
	{
		MapData::getInstance().fight(shared_from_this(), MapData::getInstance().getOneGridData(frontOfMe).getCharacter());
	}
	AS = ActionStatus::EndAction;
}
void Player::openInventory()
{
	if (AS != ActionStatus::WaitKeyInput)
		return;

	if (Input::KeyI.clicked || Gamepad(0).button(7).clicked)
	{
		MenuSystem::getInstance().openMenu(shared_from_this());
	}
}
void Player::useItem()
{
	if (AS != ActionStatus::WaitKeyInput)
		return;

	if (Input::KeyI.clicked || Gamepad(0).button(7).clicked)
		MenuSystem::getInstance().closeMenu();

	if (MenuSystem::getInstance().update())
		AS = ActionStatus::EndAction;
}

void Frankenstein::move()
{
	if (AS != ActionStatus::WaitKeyInput)
		return;

	const Point formerGrid = XYtoGrid(xyPosition);

	if (Random() < 0.4)
	{
		for (int i = 0; i < 8; i++)
		{
			if (MapData::getInstance().getOneGridData(XYtoGrid(xyPosition) + getGrid(direction + i)).enableAddCharacter())
			{
				xyPosition += GridtoXY(getGrid(direction + i));
				rotateDirection(direction, i);
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			int ran = (int)(Random() * 8);
			if (MapData::getInstance().getOneGridData(XYtoGrid(xyPosition) + getGrid(direction + ran)).enableAddCharacter())
			{
				xyPosition += GridtoXY(getGrid(direction + ran));
				rotateDirection(direction, ran);
				break;
			}
		}
	}

	if (XYtoGrid(xyPosition) != formerGrid)
	{
		MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).getWeakCharacter().swap(MapData::getInstance().getOneGridData(formerGrid).getWeakCharacter());
	}
	AS = ActionStatus::EndAction;
}

void Frankenstein::attack()
{
	for (int i = 0; i < 8; i++)
	{
		const Point frontOfMe = XYtoGrid(xyPosition) + getGrid(i);
		if (MapData::getInstance().getOneGridData(frontOfMe).isUnderCharacter())
		{
			if (MapData::getInstance().getOneGridData(frontOfMe).getCharacter()->getId() == CharacterId::player)
			{
				direction = static_cast<Direction>(i);
				MapData::getInstance().fight(shared_from_this(), MapData::getInstance().getOneGridData(frontOfMe).getCharacter());
				AS = ActionStatus::EndAction;
				break;
			}
		}
	}
}

void Ghost::move()
{
	if (AS != ActionStatus::WaitKeyInput)
		return;

	const Point formerGrid = XYtoGrid(xyPosition);

	if (Random() < 0.1)
	{
		for (int i = 0; i < 8; i++)
		{
			if (MapData::getInstance().getOneGridData(XYtoGrid(xyPosition) + getGrid(direction + i)).enableAddCharacter())
			{
				xyPosition += GridtoXY(getGrid(direction + i));
				rotateDirection(direction, i);
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			int ran = (int)(Random() * 8);
			if (MapData::getInstance().getOneGridData(XYtoGrid(xyPosition) + getGrid(direction + ran)).enableAddCharacter())
			{
				xyPosition += GridtoXY(getGrid(direction + ran));
				rotateDirection(direction, ran);
				break;
			}
		}
	}

	if (XYtoGrid(xyPosition) != formerGrid)
	{
		MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).getWeakCharacter().swap(MapData::getInstance().getOneGridData(formerGrid).getWeakCharacter());
	}
	AS = ActionStatus::EndAction;
}

void Ghost::attack()
{
	for (int i = 0; i < 8; i++)
	{
		const Point frontOfMe = XYtoGrid(xyPosition) + getGrid(static_cast<Direction>(i));
		if (MapData::getInstance().getOneGridData(frontOfMe).isUnderCharacter())
		{
			if (MapData::getInstance().getOneGridData(frontOfMe).getCharacter()->getId() == CharacterId::player)
			{
				direction = static_cast<Direction>(i);
				MapData::getInstance().fight(shared_from_this(), MapData::getInstance().getOneGridData(frontOfMe).getCharacter());
				AS = ActionStatus::EndAction;
				break;
			}
		}
	}
}

