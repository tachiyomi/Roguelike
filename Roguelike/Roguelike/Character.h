#pragma once
#include "Item.h"
#include "Ability.h"
#include <Siv3D.hpp>
#include <memory>
#include "LogSystem.h"
#include "function.h"

//キャラクター
enum ActionStatus
{
	WaitKeyInput,
	Acting,
	EndAction,
	WaitOtherAction
};
struct CharacterStatus
{
	void setStatus(int h, int a, int d, int mh)
	{
		HP = h;
		baseATK = a;
		baseDEF = d;
		baseMaxHP = mh;
	};
	/*
	void setATK(int i) 
	{
		exATK -= i;
		if(exATK)
	};
	void setDEF(int i) { return baseDEF + exDEF; };
	void setHP(int i) { return HP; };
	*/
	const int getMaxHP() { return baseMaxHP + exMaxHP; };
	const int getATK() { return baseATK + exATK; };
	const int getDEF() { return baseDEF + exDEF; };
	const int getHP() { return HP; };

	int baseMaxHP, baseATK, baseDEF, HP;
	int exMaxHP, exATK, exDEF;
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
	virtual bool enableLive();
	virtual void doSomethingAtDeath();

	void setGridPosition(Point p) { xyPosition = p; }
	void setRad(int d) { direction = d; }
	Point getGridPosition() { return XYtoGrid(xyPosition); }
	double getRad() { return Radians(direction); }
	void setStatus(ActionStatus cs) { AS = cs; }
	ActionStatus getStatus() { return AS; }
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
		for (size_t i = 0; i < abilities.size(); i++)
		{
			if (abilities[i] == item->getAbility())
			{
				abilities.erase(abilities.begin() + i);
				i--;
			}
		}
	};

	//changeHP
	int increaseHP(int i)
	{
		if (i + CS.getHP() > CS.getMaxHP())
			i = CS.getMaxHP() - CS.getHP();
		CS.HP += i;
		return i;
	}
	int decreaseHP(int i) 
	{
		if (i > CS.getHP())
			i = CS.getHP();
		CS.HP -= i;
		return i;
	}

	//changeATK
	int increaseATK(int i)
	{
		CS.exATK += i;
		return i;
	}
	int decreaseATK(int i)
	{
		if (i > CS.exATK)
			i = CS.exATK;
		CS.exATK -= i;
		return i;
	}

	//changeDEF

	int increaseDEF(int i)
	{
		CS.exDEF += i;
		return i;
	}
	int decreaseDEF(int i)
	{
		if (i > CS.exDEF)
			i = CS.exDEF;
		CS.exDEF -= i;
		return i;
	}

	int setHP(int h) { CS.HP = h; }
	int setATK(int a) { CS.baseATK = a; }
	int setDEF(int d) { CS.baseDEF = d; }
	const int getHP() { return CS.getHP(); }
	const int getATK() { return CS.getATK(); }
	const int getDEF() { return CS.getDEF(); }
	const int getMaxHP() { return CS.getMaxHP(); }

protected:
	Texture img;
	Point xyPosition;
	Color color;
	String name;
	int direction;

	ActionStatus AS;
	CharacterStatus CS;
	std::weak_ptr<Item>weapon, armor, accessory;
	Array<std::weak_ptr<Item>>inventory;
	Array<std::shared_ptr<Ability>>abilities;
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

		CS.setStatus(100, 10, 70, 100);
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

		CS.setStatus(900, 0, 20, 900);
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

		CS.setStatus(200, 40, 40, 200);

		abilities.emplace_back(std::make_shared<MindBreak>());
	}
	Kyonshih(int x, int y) :Kyonshih(Point(x, y)) {}

	void attack()override;
};