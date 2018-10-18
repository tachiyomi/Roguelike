#pragma once
#include "Item.h"
#include "Ability.h"
#include <Siv3D.hpp>
#include <memory>
#include "LogSystem.h"
#include "function.h"

//キャラクター
enum CharacterStatus
{
	WaitKeyInput,
	Acting,
	EndAction,
	WaitOtherAction
};
class Character : public std::enable_shared_from_this<Character>
{
public:
	Character(Point);
	Character(int, int);
	virtual void act();
	virtual void draw();
	virtual void move();
	virtual void attack();
	void applyTurnStartAbility();
	void applyTurnEndAbility();
	void applyAttackAbility(std::shared_ptr<Character> A, std::shared_ptr<Character> B,
		std::shared_ptr<Character> copyA, std::shared_ptr<Character>copyB);
	void applyDefendAbility(std::shared_ptr<Character> A, std::shared_ptr<Character> B,
		std::shared_ptr<Character> copyA, std::shared_ptr<Character>copyB);
	void addAbility(std::shared_ptr<Ability>);
	virtual bool enableLive() 
	{ 
		if (HP > 0)
			return true;
		else
		{
			return false;
		}
	}
	virtual void doSomethingAtDeath();

	void setGridPosition(Point p) { xyPosition = p; }
	void setRad(int d) { direction = d; }
	Point getGridPosition() { return XYtoGrid(xyPosition); }
	double getRad() { return Radians(direction); }
	void setStatus(CharacterStatus cs) { status = cs; }
	CharacterStatus getStatus() { return status; }
	Array<String> getAbility()
	{ 
		Array<String> strs;
		for (auto& e : abilities)
			strs.emplace_back(e->getName());
		return strs;
	};
	void deleteItem()
	{
		for (size_t i = 0; i < inventory.size(); i++)
		{
			if (inventory[i].expired())
			{
				inventory.erase(inventory.begin() + i);
				i--;
			}
		}
	}
	void deleteAbility()
	{
		for (size_t i = 0; i < abilities.size(); i++)
		{
			if (abilities[i]->remove())
			{
				abilities.erase(abilities.begin() + i);
				i--;
			}
		}
	}
	std::vector<String> getChoice(std::vector<size_t> ints)
	{ 
		std::vector<String> re;
		if (ints.size() == 1)
		{
			for (size_t i = 0; i < inventory.size(); i++)
				re.emplace_back(inventory[i].lock()->getName());
			return re;
		}
		else
		{
			int i = (int)ints[0];
			ints.erase(ints.begin());
			return inventory[i].lock()->getChoice(ints);
		}
	}

	String getName() { return name; }

	void equipped(std::shared_ptr<Item> item) 
	{
		switch (item->getType())
		{
		case ItemType::Weapon:
			weapon = item;
			break;
		case ItemType::Armor:
			armor = item;
			break;
		case ItemType::Accessory:
			accessory = item;
			break;
		}
		addAbility(item->getAbility());
	};

	void takeout(std::shared_ptr<Item> item)
	{
		switch (item->getType())
		{
		case ItemType::Weapon:
			weapon.reset();
			break;
		case ItemType::Armor:
			armor.reset();
			break;
		case ItemType::Accessory:
			accessory.reset();
			break;
		}
	};

	//changeHP
	int increaseHP(int i)
	{
		HP += i;
		return i;
	}
	int decreaseHP(int i) 
	{
		if (i > HP)
			i = HP;
		HP -= i;
		return i;
	}

	//changeATK
	int increaseATK(int i)
	{
		ATK += i;
		return i;
	}
	int decreaseATK(int i)
	{
		if (i > ATK)
			i = ATK;
		ATK -= i;
		return i;
	}

	//changeDEF

	int increaseDEF(int i)
	{
		DEF += i;
		return i;
	}
	int decreaseDEF(int i)
	{
		if (i > DEF)
			i = DEF;
		DEF -= i;
		return i;
	}

	int setHP(int h) { HP = h; }
	int setATK(int a) { ATK = a; }
	int setDEF(int d) { DEF = d; }
	int getHP() { return HP; }
	int getATK() { return ATK; }
	int getDEF() { return DEF; }

protected:
	Texture img;
	Point xyPosition;
	Color color;
	String name;
	int direction;

	CharacterStatus status;
	std::weak_ptr<Item>weapon, armor, accessory;
	Array<std::weak_ptr<Item>>inventory;
	Array<std::shared_ptr<Ability>>abilities;
	int HP, ATK, DEF;
};
//プレイヤー
class Player :public Character
{
public:
	Player(Point pos);
	Player(int x, int y) :Player(Point(x, y)) {}

	void act()override;
	void move()override;
	void attack()override;
	void openInventory();
	void useItem();
};
//エネミー
class Enemy :public Character
{
public:
	Enemy(Point pos) :Character(pos)
	{
		color = Palette::Tomato;

		HP = 100;
		ATK = 10;
		DEF = 70;
	}
	Enemy(int x, int y) :Enemy(Point(x, y)) {}
};
class Sandbag :public Enemy
{
public:
	Sandbag(Point pos) :Enemy(pos)
	{
		img = Texture((L"Images/sandbag.png"));
		name = L"サンドバッグ";

		HP = 900;
		ATK = 0;
		DEF = 20;
	}
	Sandbag(int x, int y) :Sandbag(Point(x, y)) {}

	void move()override;
};
class Kyonshih :public Enemy
{
public:
	Kyonshih(Point pos) :Enemy(pos)
	{
		img = Texture((L"Images/pop.png"));
		name = L"キョンシーもどき";

		HP = 200;
		ATK = 40;
		DEF = 40;

		abilities.emplace_back(std::make_shared<MindBreak>());
	}
	Kyonshih(int x, int y) :Kyonshih(Point(x, y)) {}

	void attack()override;
};